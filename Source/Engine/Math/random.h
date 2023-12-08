#pragma once

 /*! \brief             Initialize the pseudorandom number generator.
  */
void RandomInit();

/*! \brief             Generate a random integer in the interval [rangeMin, rangeMax] (inclusive)
 *  \param rangeMin:   The minimum.
 *  \param rangeMax:   The maximum.
 *  \returns           A random number between rangeMin and rangeMax.
 */
int RandomRangeInt(int rangeMin, int rangeMax);

/*! \brief             Generate a random float in the interval [rangeMin, rangeMax] (inclusive)
 *  \param rangeMin:   The minimum.
 *  \param rangeMax:   The maximum.
 *  \returns           A random number between rangeMin and rangeMax.
 */
float RandomRangeFloat(float rangeMin, float rangeMax);

/*! \brief             Generate a random float from 0 to 1
 */
float RandomFloat();