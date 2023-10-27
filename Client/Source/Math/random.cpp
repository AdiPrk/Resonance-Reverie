#include <PCH/pch.h>
#include "random.h"
#include <time.h>

void RandomInit() {
	time_t seconds;
	time(&seconds);

	srand((int)seconds);
}

/*! \brief             Generate a random integer in the interval [rangeMin, rangeMax] (inclusive)
 *  \param rangeMin:   The minimum.
 *  \param rangeMax:   The maximum.
 *  \returns           A random number between rangeMin and rangeMax.
 */
int RandomRangeInt(int rangeMin, int rangeMax) {
	return (rand() % (rangeMax - rangeMin + 1)) + rangeMin;
}

/*! \brief             Generate a random float in the interval [rangeMin, rangeMax] (inclusive)
 *  \param rangeMin:   The minimum.
 *  \param rangeMax:   The maximum.
 *  \returns           A random number between rangeMin and rangeMax.
 */
float RandomRangeFloat(float rangeMin, float rangeMax) {
	return ((float)rand() / (float)(RAND_MAX)) * (rangeMax - rangeMin) + rangeMin;
}

/*! \brief             Generate a random float from 0 to 1
 */
float RandomFloat() {
	return ((float)rand() / (float)(RAND_MAX));
}