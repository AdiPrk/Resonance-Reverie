#version 330 core

in vec2 TexCoords;
out vec4 fragColor;

uniform sampler2D image;
uniform vec4 spriteColor;

uniform float iTime;

// totetmatt, Blackle, Dave Hoskins, byt3_m3chanic, tater,
// and a bunch of others for sharing their time and knowledge!

// License: Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
#define MIN_DIST	1e-4
#define START_DIST	1.
#define MAX_DIST	64.
#define MAX_STEPS	120.
#define MAX_RDIST	7.
#define MAX_RSTEPS	32.
#define SHADOW_STEPS	30.
#define LIGHT_RGB	vec3(4, 2.34, .04)

#define R	vec2(1600, 900)
#define Z0	min(iTime, 0.)
#define I0	min(iTime, 0)
#define sat(x)	clamp(x, 0., 1.)
#define S(a, b, c)	smoothstep(a, b, c)
#define S01(a)	S(0., 1., a)

float t;
vec2 g;
struct Hit {
	float d;
	int id;
	vec3 p;
};

void U(inout Hit h, float d, int id, vec3 p) { if (d < h.d) h = Hit(d, id, p); }

float min2(vec2 v) { return min(v.x, v.y); }

float max2(vec2 v) { return max(v.x, v.y); }

float max3(vec3 v) { return max(v.x, max(v.y, v.z)); }

float dot3(vec3 v) { return dot(v, v); }

float sum2(vec2 v) { return dot(v, vec2(1)); }

// Thnx Dave_Hoskins - https://www.shadertoy.com/view/4djSRW
float h11(float p) {
	p = fract(p * .1031);
	p *= p + 3.3456;
	return fract(p * (p + p));
}

float h31(vec3 p3) {
	p3 = fract(p3 * .1031);
	p3 += dot(p3, p3.yzx + 333.3456);
	return fract(sum2(p3.xy) * p3.z);
}

float h21(vec2 p) { return h31(p.xyx); }

float n31(vec3 p) {
	const vec3 s = vec3(7, 157, 113);

	// Thanks Shane - https://www.shadertoy.com/view/lstGRB
	vec3 ip = floor(p);
	p = fract(p);
	p = p * p * (3. - 2. * p);
	vec4 h = vec4(0, s.yz, sum2(s.yz)) + dot(ip, s);
	h = mix(fract(sin(h) * 43758.545), fract(sin(h + s.x) * 43758.545), p.x);
	h.xy = mix(h.xz, h.yw, p.y);
	return mix(h.x, h.y, p.z);
}

// Three n31 results from three scales.
vec3 n3331(vec3 p) {
	const vec3 s = vec3(5, 13, 196);
	vec3 ns;
	for (int i = int(I0); i < 3; i++)
		ns[i] = n31(p * s[i]);

	return ns;
}

float fbm(vec3 p) {
	float i,
	      a = 0.,
	      b = .5;
	for (i = Z0; i < 4.; i++) {
		a += b * n31(p);
		b *= .5;
		p *= 2.;
	}

	return a * .5;
}

float sabs(float f) { return sqrt(f * f + .01); }

float smin(float a, float b, float k) {
	float h = sat(.5 + .5 * (b - a) / k);
	return mix(b, a, h) - k * h * (1. - h);
}

mat2 rot(float a) {
	float c = cos(a),
	      s = sin(a);
	return mat2(c, s, -s, c);
}

vec3 ax(vec3 p) { return vec3(abs(p.x) - .5, p.yz); }

vec3 az(vec3 p) { return vec3(p.xy, abs(p.z) - .8); }

float rep(float p, float c) { return p - c * floor(p / c + .5); }

vec3 bend(vec3 p, float k) {
	float c = cos(k * p.x),
	      s = sin(k * p.x);
	p.xz *= mat2(c, s, -s, c);
	return p;
}

float box(vec3 p, vec3 b) {
	vec3 q = abs(p) - b;
	return length(max(q, 0.)) + min(max3(q), 0.);
}

float box2d(vec2 p) {
	vec2 q = abs(p) - vec2(.015, 0);
	return length(max(q, 0.)) + min(max2(q), 0.);
}

float cyl(vec3 p, vec2 hr) {
	vec2 d = abs(vec2(length(p.yz), p.x)) - hr;
	return min(max(d.x, d.y), 0.) + length(max(d, 0.));
}

float cap(vec3 p, float h, float r) {
	r *= 1. - p.x / h * .14;
	p.x -= clamp(p.x, 0., h);
	return length(p) - r;
}

float tri(vec3 p, vec3 a, vec3 c) {
	const vec3 b = vec3(.06, 0, 0);
	vec3 ba = b - a,
	     pa = p - a,
	     cb = c - b,
	     pb = p - b,
	     ac = a - c,
	     pc = p - c,
	     n = cross(ba, ac);
	return sqrt((sign(dot(cross(ba, n), pa)) + sign(dot(cross(cb, n), pb)) + sign(dot(cross(ac, n), pc)) < 2.) ? min(min(dot3(ba * sat(dot(ba, pa) / dot3(ba)) - pa), dot3(cb * sat(dot(cb, pb) / dot3(cb)) - pb)), dot3(ac * sat(dot(ac, pc) / dot3(ac)) - pc)) : dot(n, pa) * dot(n, pa) / dot3(n));
}

float honk(inout vec3 p, mat2 rot, float h, float r) {
	p.xz *= rot;
	float d = cap(p, h, r);
	p.x -= h;
	return d;
}

float hex3D(vec3 p, vec2 h) {
	const vec3 k = vec3(-.8660254, .5, .57735);
	p = abs(p);
	p.xz -= 2. * min(dot(k.xy, p.xz), 0.) * k.xy;
	vec2 d = vec2(length(p.xz - vec2(clamp(p.x, -k.z * h.x, k.z * h.x), h.x)) * sign(p.z - h.x), p.y - h.y);
	return min(max2(d), 0.) + length(max(d, 0.));
}

vec3 rayDir(vec3 ro, vec3 lookAt, vec2 uv) {
	vec3 f = normalize(lookAt - ro),
	     r = normalize(cross(vec3(0, 1, 0), f));
	return normalize(f + r * uv.x + cross(f, r) * uv.y);
}

float fakeEnv(vec3 n) {
	// Thanks Blackle.
	return length(sin(n * 2.5) * .5 + .5) * (.4 + .6 * n31(n - t));
}

vec3 skyCol(vec3 rd) { return mix(vec3(.2, .001, .005), vec3(1, .15, .001), fakeEnv(rd)); }

// Shameless self-promotion. :)
float dtc(vec2 p) {
	p *= vec2(-4, 5);
	if (abs(p.x) > .6 || abs(p.y) > .5) return 0.;
	if (step(min2(abs(p - vec2(0, .25))), .08) * step(p.y, .3) * step(abs(p.x), .4) > 0.) return 1.;
	float f,
	      dc = step(.5, p.x);
	p.x = abs(p.x) - .46;
	f = dot(p, p);
	dc += step(f, .25) * step(.16, f);
	return dc > 0. ? .8 : 0.;
}

#define SKY_ID	0
#define LAVA_ID	1
#define GLOVE_ID	2
#define SKIN_ID	3
#define LEATHER_ID	4
#define BELOW_ARE_REFLECTIVE	5
#define STEEL_ID	6
#define CHAIN_ID	7

float links(vec3 p) {
	p.y = rep(p.y, .36);
	float r2 = .01 + .002 * S(.015, 0., abs(p.y)) * step(p.x, 0.);
	return length(vec2(length(vec2(p.x, max(abs(p.y) - .05, 0.))) - .06, p.z)) - r2;
}

float chain(vec3 p, float b) {
	p.y -= 5.;
	p.yzx = bend(p.yzx, -.003 * sin(t) * b);
	p.y += 5.;
	float d = links(p);
	p.y += .18;
	p.xz *= mat2(.07074, .99749, -.99749, .07074);
	return min(d, links(p));
}

Hit sdf(vec3 p, float shad) {
	p.x += .25;
	float curl, l,
	      lava = p.y + 1. + sin(p.x + t) * .05,
	      d = lava;
	Hit h = Hit(d + shad * 1e7, LAVA_ID, p);

	// Big floaty.
	vec3 r,
	     q = ax(p);
	if (d < .3 && q.x > 2.) {
		q.xz -= 3.;
		q.y += 1.05 - sin(p.x + t - 2.) * .04;
		d = n31(q * 4.5) * .1;
		d = hex3D(q, vec2(.7 + d, .1)) - d;
		U(h, d, LEATHER_ID, q);
	}

	// Dunk!
	p.y += t * .15 - .3;

	// Glow near the lava.
	g += 1e-4 / (.01 + lava);
	g += S(.7 * S(16., 10., t), 0., length(p.xz + .25)) * 2e-4 / (2e-5 + lava * lava);

	// Chains.
	q = p;
	q.xz *= mat2(.92106, .38942, -.38942, .92106);
	q = az(q - vec3(1, 0, -.5));
	U(h, chain(q, sign(p.z)), CHAIN_ID, q);

	// Sleeve.
	q = p.yxz;
	q.x++;
	q.y += .16;
	d = abs(length(q.yz) - .5 + .01 * cos(p.y * 14.)) - .01;
	d = smin(d, p.y + p.x * .3 + .4 * cos(p.x * 1.6), -.05);
	U(h, d, LEATHER_ID, q);

	// Zip.
	q = p;
	q.x = abs(q.x) - .2;
	q.xy *= mat2(.995, -.09983, .09983, .995);
	q.y = rep(q.y, .05);
	d = max(box2d(q.xy), d - .02);
	d = max(d, q.z);
	d = max(d, .43 + p.y + .3 * p.x);
	U(h, d, STEEL_ID, q);

	// Wrist.
	q = p.yxz;
	q.xy *= mat2(.995, .09983, -.09983, .995);
	q.y = sabs(q.y + .2) - .16;
	q.x += .8;
	q.z -= .04;
	d = cap(q, .7, .13);
	U(h, d, SKIN_ID, q);
	curl = mix(sin(t * 3.) * .1, S(2., 6., t), sat(t / 4.));
	p.xy *= rot(1. - .4 * curl);

	// Thumb.
	q = p + vec3(.25, -.1, .07);
	q.xy *= mat2(.49757, .86742, -.86742, .49757);
	d = honk(q, mat2(.98007, -.19867, .19867, .98007), .42, .04 - .07 * S(.1, .5, q.x));
	q.yz *= mat2(.16997, -.98545, .98545, .16997);
	d = smin(d, honk(q, rot(.3 * curl - .5), .26, -.025), .04);
	U(h, cyl(q, vec2(.06, .15)), STEEL_ID, p);
	d = smin(d, honk(q, mat2(.995, -.09983, .09983, .995), .22, -.03 - .065 * S(.1, .25, q.x) * S(.05, -.08, q.z)), .02);

	// Glove cut-outs.
	d = max(d, sin(q.x * 70.) * .01 + .22 - length(q - vec3(-.25, .12, .05)));

	// Palm.
	p = bend(p, -.3);
	r = vec3(.37, .47 - S(.1, -.4, p.x) * .15, .12);
	d = smin(d, box(p, r - .12), .16);
	d += S(0., .2, abs(sin((atan(p.x, p.z) + atan(p.z, p.y - .2)) * 4.))) * .005;
	p.x -= r.x;
	curl = S01(curl * (1. + step(p.y, 0.) * .3));

	// Pre-fingers paddy bit.
	d = smin(d, tri(p, vec3(0, r.y - .12, 0), vec3(0, .12 - r.y, 0)), .05);
	d -= .12;
	p.xz *= rot(-.2 - curl * .63);
	l = 1. + step(0., p.y) * .1 + step(.25, p.y) * .2;
	p.y = -abs(p.y);
	p.xz -= .05;

	// Ring/Middle.
	q = p;
	q.y += r.y * .5 - .12;
	q.xy *= mat2(.9998, -.02, .02, .9998);
	mat2 r1 = rot(-.7 * curl),
	     r2 = rot(-1.4 * curl);
	d = smin(d, honk(q, r1, .32 * l, .105), .06);
	d = smin(d, honk(q, r2, .17 * l, .09), .01);
	d = smin(d, honk(q, r1, .13 * l, .08), .01);

	// Pinky/Index.
	q = p;
	q.y += r.y - .12;
	q.z += .07;
	q.xy *= mat2(.995, -.09983, .09983, .995);
	d = smin(d, honk(q, r1, .19 * l, .105), .06);
	d = smin(d, honk(q, r2, .13 * l, .09), .01);
	d = smin(d, honk(q, mat2(1, 0, 0, 1), .12 * l, .08), .01);

	// Glove cut-outs.
	d = max(d, .13 - length(q + vec3(0, .02, .03)));
	U(h, d, GLOVE_ID, p);

	// Robot finger tip.
	q.x += .07;
	d = cyl(q, vec2(.02, .1));
	q.x -= .03;
	d = min(d, cyl(q, vec2(.04)));
	U(h, d, STEEL_ID, p);

	// Nothing is perfectly sharp.
	h.d -= .01;
	return h;
}

vec3 N(vec3 p, float t) {
	float h = t * .1;
	vec3 n = vec3(0);
	for (int i = int(I0); i < 4; i++) {
		vec3 e = .005773 * (2. * vec3(((i + 3) >> 1) & 1, (i >> 1) & 1, i & 1) - 1.);
		n += e * sdf(p + e * h, 0.).d;
	}

	return normalize(n);
}

float shadow(vec3 p, vec3 lp, vec3 ld, vec3 n) {
	// Quick abort if light is behind the normal.
	if (dot(ld, n) < -.1) return 0.;
	float d,
	      s = 1.,
	      t = .005,
	      mxt = length(p - lp);
	for (float i = Z0; i < SHADOW_STEPS; i++) {
		d = sdf(t * ld + p, 1.).d;
		s = min(s, 15. * d / t);
		t += max(.03, d);
		if (mxt - t < 5. || s < .05) break;
	}

	return S01(s);
}

// Quick 2-level ambient occlusion.
float ao(vec3 p, vec3 n) {
	const vec2 h = vec2(.25, .7);
	vec2 ao;
	for (int i = int(I0); i < 2; i++)
		ao[i] = sdf(h[i] * n + p, 0.).d;

	return sat(min2(ao / h));
}

vec3 lights(vec3 p, vec3 ro, vec3 rd, vec3 n, Hit h) {
	const vec3 lp = vec3(0, -10, -6);
	if (h.id == SKY_ID) return skyCol(rd);
	float _ao, fre, fogY, fg,
	      spe = 10.,
	      shine = 1.;
	vec3 c, ns, l, sky, col, uv,
	     ld = normalize(lp - p);

	// Cache noise.
	ns = n3331(h.p);
	_ao = ao(p, n);
	if (h.id == LAVA_ID) {
		c = vec3(5, 2.49, .98);
		c *= .6 + .4 * (ns.x + .6 * ns.y);
		c += (1. - _ao) * 13.;
	}
	else if (h.id == GLOVE_ID) {
		c = vec3(.2, .15, .9);
		c *= .1 + .6 * ns.z;
		shine *= 4. * ns.y;
	}
	else if (h.id == LEATHER_ID) {
		c = vec3(.1 * ns.x * (.2 + .8 * ns.y));
		shine *= 20. * S01(abs(ns.z - .5 + rd.x));
	}
	else if (h.id == CHAIN_ID) {
		c = vec3(.15);
		shine = 5. * ns.y * ns.z;
	}
	else if (h.id == SKIN_ID) c = vec3(1, .63, .46);
	else {
		// Exo-skeleton / zip.
		c = vec3(.912, .914, 10.92);
		shine = 20.;
		spe = 50.;
	}

	// Specular imperfections.
	shine *= .5 + .5 * ns.x * ns.y;

	// Key light, reverse, sky.
	l = sat(vec3(dot(ld, n), dot(-ld.xz, n.xz), n.y));

	// Diffuse.
	l.xy = .1 + .9 * l.xy;

	// Ambient occlusion.
	l *= .1 + .9 * _ao;

	// Light contributions (key, reverse, sky).
	l *= vec3(1, .8, .3);

	// Specular (Blinn-Phong)
	l.x += pow(sat(dot(normalize(ld - rd), n)), spe) * shine;

	// Shadow.
	l.x *= .1 + .9 * shadow(p, lp, ld, n);

	// Light falloff
	l.x *= dot(lp, lp) / (1. + dot(lp - p, lp - p));
	fre = S(.7, 1., 1. + dot(rd, n)) * .2;
	sky = skyCol(rd);
	col = mix((sum2(l.xy) * LIGHT_RGB + l.z * sky) * c, sky, fre);

	// Simple fog layer.
	fogY = -.25;
	uv = vec3(p.xz, fogY) * .4 + t * vec3(.1, -.9, .2);
	float fogTex = S(0., .5, fbm(uv));
	fogY -= (1. - fogTex) * .5;
	fg = S(0., -.5, p.y - fogY);
	fg *= fogTex;
	fg *= 1. - sat(-rd.y);

	// Distance Fog.
	fg += 1. - exp(dot3(p - ro) * -.01);
	return mix(skyCol(vec3(rd.x, 0, rd.z)), col, 1. - sat(fg));
}

vec3 march(vec3 ro, vec3 rd) {
	// Heat haze.
	rd.xy *= 1. + n31((rd - vec3(0, t * .5, 0)) * 10.) * .03;
	rd = normalize(rd);

	// March the scene.
	vec3 dv, n, col,
	     p = ro;
	float i,
	      d = START_DIST;
	Hit h;
	for (i = Z0; i < MAX_STEPS; i++) {
		if (d > MAX_DIST) {
			h.id = SKY_ID;
			break;
		}

		h = sdf(p, 0.);
		if (abs(h.d) < MIN_DIST * d) break;
		d += h.d;
		p += h.d * rd;
	}

	// Floaty particles.
	dv = rd;
	for (i = 1.5; i < d; i += 4.) {
		vec3 vp = ro + dv * i;
		vp.yz -= t * .15;
		g.x += 1. - S(0., mix(.05, .02, sat((i - 1.) / 19.)), length(fract(vp - ro) - .5));
		dv.xz *= mat2(.87758, .47943, -.47943, .87758);
	}

	col = g.x * LIGHT_RGB;
	col += lights(p, ro, rd, n = N(p, d), h);
	if (h.id > BELOW_ARE_REFLECTIVE) {
		// We hit a reflective surface, so march reflection.
		rd = reflect(rd, n);
		p += n * .01;
		ro = p;
		d = .01;
		for (i = Z0; i < MAX_RSTEPS; i++) {
			if (d > MAX_RDIST) {
				h.id = SKY_ID;
				break;
			}

			h = sdf(p, 0.);
			if (abs(h.d) < MIN_DIST * d) break;
			d += h.d;
			p += h.d * rd;
		}

		// Add a hint of the reflected color.
		col += .2 * lights(p, ro, rd, N(p, d), h);
	}

	return pow(max(vec3(0), col), vec3(.4545));
}

#define RR(v)	mod(4e4 * sin(dot(ceil(v), vec2(12, 7))), 10.)

// Based on https://www.shadertoy.com/view/llXSzj by FabriceNeyret2
// in turn based on https://www.shadertoy.com/view/XlXSz2 by patriciogv
// Thanks all!
vec3 os(vec2 u) {
	u *= 1. + sat(S(21., 23., t) * vec2(1, 1.5) - vec2(.3, 0)) * 50.;
	if (max2(abs(u)) > 1.) return vec3(0);
	vec2 o = u;
	float s = S(22., 20., t);
	u.x += u.y * .5 * (1. - s) * h11(u.y * 129.45 + 2.3 * t);
	float l = dtc(u - vec2(.62, -.35));
	float c = step(-.2, u.y) + step(u.x, .4);
	c *= step(abs(u.y), .458);
	vec2 p = 6. * fract(u *= 24.) - .5;
	u.y += ceil(t * RR(u.xx));
	int i = int(p.y);
	i = (abs(p.x - 1.5) > 1.5 ? 0 : i == 5 ? 972980223 : i == 4 ? 690407533 : i == 3 ? 704642687 : i == 2 ? 696556137 : i == 1 ? 972881535 : 0) / int(exp2(30. - ceil(p.x) - 3. * floor(RR(u))));
	c *= i > i / 2 * 2 ? 1. : 0.;
	c *= step(1., abs(u.x));
	c *= step(1., abs(abs(u.x) - 10.));
	c *= step(abs(u.x), 19.);
	vec3 f = max(vec3(l), sat(vec3(.9, .3, .01) + c));
	f *= s + (1. - s) * h21(floor(o * 1e2) + floor(t * 50.) * 123.4);
	f *= .6 + .4 * step(.5, fract(o.y * 80.)) * sat(.5 + .5 * sin(o.y * 8. - t * 3.));
	return f * S(25., 22.5, t);
}

void main() {
	t = mod(max(0., iTime - 1.), 28.);
	g = vec2(0);
	vec2 uv = (gl_FragCoord.xy - .5 * R.xy) / R.y;
	vec3 col;
	if (t < 18.) {
		float st = S(0., 15., t);
		vec3 lookAt = vec3(-.1, .1 - 1.2 * st, 0),
		     ro = vec3(-st * .5, .001, mix(-2.8, -1., st));

		// View bob.
		ro += .1 * sin(iTime * vec3(.9, .7, .3));
		ro.yz *= rot(mix(-.1, .4, st));
		ro.xz *= rot(mix(-.4, .4, st));
		col = march(ro, rayDir(ro, lookAt, uv));
	}
	else col = os(uv);

	// Vignette.
	col *= 1. - .5 * dot(uv, uv);

	// Grain.
	col += (h21(gl_FragCoord.xy) - .5) / 48.;
	fragColor = vec4(col * min(1., t), 1);
}