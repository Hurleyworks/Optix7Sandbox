#pragma once

template <typename T>
class Math
{
public:
    // Wrappers to hide implementations of functions.  The ACos and ASin
    // functions clamp the input argument to [-1,1] to avoid NaN issues
    // when the input is slightly larger than 1 or slightly smaller than -1.
    // Other functions have the potential for using a fast and approximate
    // algorithm rather than calling the standard math library functions.
    static T ACos (T value);
    static T ASin (T value);
    static T ATan (T value);
    static T ATan2 (T y, T x);
    static T Ceil (T value);
    static T Cos (T value);
    static T Exp (T value);
    static T FAbs (T value);
    static T Floor (T value);
    static T FMod (T x, T y);
    static T InvSqrt (T value);
    static T Log (T value);
    static T Log2 (T value);
    static T Log10 (T value);
    static T Pow (T base, T exponent);
    static T Sin (T value);
    static T Sqr (T value);
    static T Sqrt (T value);
    static T Tan (T value);

    // Return -1 if the input is negative, 0 if the input is zero, and +1
    // if the input is positive.
    static int sign (int value);
    static T sign (T value);

    // Generate a random number in [0,1].  The random number generator may
    // be seeded by a first call to unitRandom with a positive seed.
    static T unitRandom (unsigned int seed = 0);

    // Generate a random number in [-1,1].  The random number generator may
    // be seeded by a first call to symetricRandom with a positive seed.
    static T symetricRandom (unsigned int seed = 0);

    // Generate a random number in [min,max].  The random number generator may
    // be seeded by a first call to intervalRandom with a positive seed.
    static T intervalRandom (T min, T max, unsigned int seed = 0);

    // clamp the input to the specified interval [min,max].
    static T clamp (T value, T minValue, T maxValue);

    // clamp the input to [0,1].
    static T saturate (T value);

    // fast evaluation of trigonometric and inverse trigonometric functions
    // using polynomial approximations.  The speed ups vary with processor.

    // The input must be in [0,pi/2].
    static T fastSin0 (T angle);
    static T fastSin1 (T angle);

    // The input must be in [0,pi/2]
    static T fastCos0 (T angle);
    static T fastCos1 (T angle);

    // The input must be in [0,pi/4].
    static T fastTan0 (T angle);
    static T fastTan1 (T angle);

    // The input must be in [0,1].
    static T fastInvSin0 (T value);
    static T fastInvSin1 (T value);

    // The input must be in [0,1].
    static T fastInvCos0 (T value);
    static T fastInvCos1 (T value);

    // The input must be in [-1,1]. 
    static T fastInvTan0 (T value);
    static T fastInvTan1 (T value);

    // fast approximations to exp(-x).  The input x must be in [0,infinity).
    static T fastNegExp0 (T value);
    static T fastNegExp1 (T value);
    static T fastNegExp2 (T value);
    static T fastNegExp3 (T value);

    // Common constants.
    static const T EPSILON;
    static const T ZERO_TOLERANCE;
    static const T MAX_REAL;
    static const T PI;
    static const T TWO_PI;
    static const T HALF_PI;
    static const T INV_PI;
    static const T INV_TWO_PI;
    static const T DEG_TO_RAD;
    static const T RAD_TO_DEG;
    static const T LN_2;
    static const T LN_10;
    static const T INV_LN_2;
    static const T INV_LN_10;
    static const T SQRT_2;
    static const T INV_SQRT_2;
    static const T SQRT_3;
    static const T INV_SQRT_3;
};

typedef Math<float> Mathf;
typedef Math<double> Mathd;

