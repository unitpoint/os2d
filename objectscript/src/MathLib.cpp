//============================================================================
//
// Author: Evgeniy Golovin, egolovin@superscape.com
//
//============================================================================

#include "MathLib.h"
// #include <SwerveHelper.h>

#define IEEE_FLT_MANTISSA_BITS  23
#define IEEE_FLT_EXPONENT_BITS  8
#define IEEE_FLT_EXPONENT_BIAS  127
#define IEEE_FLT_SIGN_BIT       31

const float MathLib::PI = 3.14159265358979323846f;
const float MathLib::TWO_PI = 6.28318530718f; // PI * 2
const float MathLib::HALF_PI = 1.570796326795f; // PI / 2

const float MathLib::DEG2RAD = 0.01745329251994f; // PI / 180.0f
const float MathLib::RAD2DEG = 57.29577951308f; // 180.0f / PI

float MathLib::invSqrt(float x)
{
	float y = x * 0.5f;
	long i = *(long*)&x;
	i = 0x5f3759df - (i >> 1);
	float r = *(float*)&i;
	r = r * (1.5f - r * r * y);
	// r = r * (1.5f - r * r * y);
	return r;
}

float MathLib::normAngle(float angle)
{
	if(IS_FLOAT_SIGN_BIT_SET(angle))
	{
		return 360.0f + angle - (float)(int(angle / 360.0f) * 360);
	}
	if(angle >= 360.0f)
	{
		return angle - (float)(int(angle / 360.0f) * 360);
	}
	return angle;
}

float MathLib::normAngle180(float angle)
{
	angle = normAngle(angle);
	return angle > 180.0f ? angle - 360.0f : angle;
}

float MathLib::anglesDiff(float a, float b)
{
	a = normAngle(a);
	b = normAngle(b);
	float diff = b - a;
	if(diff > 180.0f)
	{
		return diff - 360.0f;
	}
	if(diff < -180.0f)
	{
		return diff + 360.0f;
	}
	return diff;
}

float MathLib::sin(float angle)
{
	angle = normAngle(angle);
	if(angle < 180)
	{
		if(angle > 90)
		{
			angle = 180 - angle;
		}
	}
	else
	{
		if(angle > 270)
		{
			angle -= 360;
		}
		else
		{
			angle = 180 - angle;
		}
	}
	float a = angle * DEG2RAD;
	float s = a * a;

#ifdef DEVICE_LOW_SPEED
	return a * ((8.3333315e-03f * s - 1.666666664e-01f) * s + 1.0f); 
#else
	return a * (((((-2.39e-08f * s + 2.7526e-06f) * s - 1.98409e-04f) * s + 8.3333315e-03f) * s - 1.666666664e-01f) * s + 1.0f);
	// return a * (((1.98409e-04f * s + 8.3333315e-03f) * s - 1.666666664e-01f) * s + 1.0f);
#endif
}

float MathLib::cos(float angle)
{
	float d;
	angle = normAngle(angle);
	if(angle < 180)
	{
		if(angle > 90)
		{
			angle = 180 - angle;
			d = -1.0f;
		}
		else
		{
			d = 1.0f;
		}
	}
	else
	{
		if(angle > 270)
		{
			angle -= 360;
			d = 1.0f;
		}
		else
		{
			angle = 180 - angle;
			d = -1.0f;
		}
	}
	float a = angle * DEG2RAD;
	float s = a * a;
#ifdef DEVICE_LOW_SPEED
	return d * ((4.16666418e-02f * s - 4.999999963e-01f) * s + 1.0f);
#else
	return d * (((((-2.605e-07f * s + 2.47609e-05f) * s - 1.3888397e-03f) * s + 4.16666418e-02f) * s - 4.999999963e-01f) * s + 1.0f);
	// return d * (((1.3888397e-03f * s + 4.16666418e-02f) * s - 4.999999963e-01f) * s + 1.0f);
#endif
}

void MathLib::sinCos(float angle, float& sv, float& cv)
{
	float d;
	angle = normAngle(angle);
	if(angle < 180)
	{
		if(angle > 90)
		{
			angle = 180 - angle;
			d = -1.0f;
		}
		else
		{
			d = 1.0f;
		}
	}
	else
	{
		if(angle > 270)
		{
			angle -= 360;
			d = 1.0f;
		}
		else
		{
			angle = 180 - angle;
			d = -1.0f;
		}
	}
	float a = angle * DEG2RAD;
	float s = a * a;

#ifdef DEVICE_LOW_SPEED
	sv = a * ((8.3333315e-03f * s - 1.666666664e-01f) * s + 1.0f); 
	cv = d * ((4.16666418e-02f * s - 4.999999963e-01f) * s + 1.0f);
#else
	sv = a * (((((-2.39e-08f * s + 2.7526e-06f) * s - 1.98409e-04f) * s + 8.3333315e-03f) * s - 1.666666664e-01f) * s + 1.0f);
	cv = d * (((((-2.605e-07f * s + 2.47609e-05f) * s - 1.3888397e-03f) * s + 4.16666418e-02f) * s - 4.999999963e-01f) * s + 1.0f);
	// sv = a * (((1.98409e-04f * s + 8.3333315e-03f) * s - 1.666666664e-01f) * s + 1.0f);
	// cv = d * (((1.3888397e-03f * s + 4.16666418e-02f) * s - 4.999999963e-01f) * s + 1.0f);
#endif
}

float MathLib::tan(float angle)
{
	// return Sin(angle) / Cos(angle);
	bool reciprocal;
	angle = normAngle(angle);
	if(angle < 90)
	{
		if(angle > 45)
		{
			angle = 90 - angle;
			reciprocal = true;
		}
		else
		{
			reciprocal = false;
		}
	}
	else
	{
		if(angle > 90+45)
		{
			angle -= 180;
			reciprocal = false;
		}
		else
		{
			angle = 90 - angle;
			reciprocal = true;
		}
	}
	float a = angle * DEG2RAD;
	float s = a * a;
#ifdef DEVICE_LOW_SPEED
	s = a * ((1.333923995e-01f * s + 3.333314036e-01f) * s + 1.0f);
#else
	// s = a * ((((((9.5168091e-03f * s + 2.900525e-03f) * s + 2.45650893e-02f) * s + 5.33740603e-02f) * s + 1.333923995e-01f) * s + 3.333314036e-01f) * s + 1.0f);
	s = a * (((5.33740603e-02f * s + 1.333923995e-01f) * s + 3.333314036e-01f) * s + 1.0f);
#endif
	return reciprocal ? 1.0f / s : s;
}

float MathLib::arcSin(float a)
{
	if(IS_FLOAT_SIGN_BIT_SET(a))
	{
		if(a <= -1.0f)
		{
			return -90;
		}
		a = -a;
#ifdef DEVICE_LOW_SPEED
		return ((0.2121144f * a + 1.5707288f) * Sqrt(1.0f - a) - HALF_PI) * RAD2DEG;
#else
		return ((((-0.0187293f * a + 0.0742610f) * a - 0.2121144f) * a + 1.5707288f) * sqrt(1.0f - a) - HALF_PI) * RAD2DEG;
		// return (((0.0742610f * a - 0.2121144f) * a + 1.5707288f) * Sqrt(1.0f - a) - HALF_PI) * RAD2DEG;
#endif
	}
	if(a >= 1.0f)
	{
		return 90;
	}
#ifdef DEVICE_LOW_SPEED
	return (HALF_PI - (0.2121144f * a + 1.5707288f) * Sqrt(1.0f - a)) * RAD2DEG;
#else
	return (HALF_PI - (((-0.0187293f * a + 0.0742610f) * a - 0.2121144f) * a + 1.5707288f) * sqrt(1.0f - a)) * RAD2DEG;
	// return (HALF_PI - ((0.0742610f * a - 0.2121144f) * a + 1.5707288f) * Sqrt(1.0f - a)) * RAD2DEG;
#endif
}

float MathLib::arcCos(float a)
{
	if(IS_FLOAT_SIGN_BIT_SET(a))
	{
		if(a <= -1.0f)
		{
			return 180;
		}
		a = -a;
#ifdef DEVICE_LOW_SPEED
		return (PI - (0.2121144f * a + 1.5707288f) * Sqrt(1.0f - a)) * RAD2DEG;
#else
		return (PI - (((-0.0187293f * a + 0.0742610f) * a - 0.2121144f) * a + 1.5707288f) * sqrt(1.0f - a)) * RAD2DEG;
		// return (PI - ((0.0742610f) * a - 0.2121144f) * a + 1.5707288f) * Sqrt(1.0f - a) * RAD2DEG;
#endif
	}
	if(a >= 1.0f)
	{
		return 0.0f;
	}
#ifdef DEVICE_LOW_SPEED
	return ((0.2121144f * a + 1.5707288f) * Sqrt(1.0f - a)) * RAD2DEG;
#else
	return ((((-0.0187293f * a + 0.0742610f) * a - 0.2121144f) * a + 1.5707288f) * sqrt(1.0f - a)) * RAD2DEG;
	// return (((0.0742610f * a - 0.2121144f) * a + 1.5707288f) * Sqrt(1.0f - a)) * RAD2DEG;
#endif
}

// =============================================================================
// =============================================================================
// =============================================================================

#define FLT_ZERO_EPSILON (1e-06f)

inline static bool util_fcmp_g (float a, float b)  {return a - b > FLT_ZERO_EPSILON;}
inline static bool util_fcmp_ge(float a, float b) {return a - b > -FLT_ZERO_EPSILON;}
inline static bool util_fcmp_l (float a, float b)  {return util_fcmp_g(b, a);}
inline static bool util_fcmp_le(float a, float b) {return util_fcmp_ge(b, a);}
inline static bool util_fcmp_e (float a, float b)  {return MathLib::abs(a - b) < FLT_ZERO_EPSILON;}
inline static bool util_fcmp_ne(float a, float b) {return util_fcmp_e(a, b) == false;}

//= snake ======================================================================
// defined on [0..1]
inline static float util_fastatan(float x)
{
	return x*(1.0f + x*(0.010413f - x*(0.411333f - x*(0.21537f - x*0.028779f))));
}

//= snake ======================================================================
inline static float util_atan(float a)
{
	float result;
	float aa = MathLib::abs(a);
	if(a >= -1 && a <= 1)
	{
		result = util_fastatan(aa);
	}
	else
	{
		result = MathLib::HALF_PI - util_fastatan(1 / aa);
	}
	result *= MathLib::RAD2DEG;
	return a >= 0 ? result : -result;
}

//= snake ======================================================================
// returns angle in (-180..180)
inline static float util_atan2(float t_y, float t_x)
{
	if(util_fcmp_e(t_x, 0))
	{
		if(util_fcmp_e(t_y, 0))
		{
			return 0;
		}
		else if(t_y > 0)
		{
			return 90;
		}
		else
		{
			return -90;
		}
	}
	else if(util_fcmp_e(t_y, 0))
	{
		if(t_x > 0)
		{
			return 0;
		}
		else
		{
			return 180;
		}
	}
	else
	{
		float ax = MathLib::abs(t_x);
		float ay = MathLib::abs(t_y);

		float result;

		if(ax > ay)
		{
			result = util_fastatan(ay / ax);
			if (t_x < 0)
			{
				result = MathLib::PI - result;
			}
		}
		else
		{
			if(t_x >= 0)
			{
				result = MathLib::HALF_PI - util_fastatan(ax / ay);
			}
			else
			{
				result = MathLib::HALF_PI + util_fastatan(ax / ay);
			}
		}

		result *= MathLib::RAD2DEG;
		return t_y >= 0 ? result : -result;
	}
}

// =============================================================================
// =============================================================================
// =============================================================================

float MathLib::arcTan(float a)
{
	return util_atan(a);
	/*
	float s;
	if(abs(a) > 1.0f)
	{
	a = 1.0f / a;
	s = a * a;
	#ifdef DEVICE_LOW_SPEED
	s = - ((((0.1420889944f * s + 0.1999355085f) * s - 0.3333314528f) * s) + 1.0f) * a;
	#else
	s = - (((((((((0.0028662257f * s - 0.0161657367f) * s + 0.0429096138f) * s - 0.0752896400f)
	* s + 0.1065626393f) * s - 0.1420889944f) * s + 0.1999355085f) * s - 0.3333314528f) * s) + 1.0f) * a;
	// s = - (((((((0.0429096138f * s - 0.0752896400f)
	//     * s + 0.1065626393f) * s - 0.1420889944f) * s + 0.1999355085f) * s - 0.3333314528f) * s) + 1.0f) * a;
	#endif
	if(IS_FLOAT_SIGN_BIT_SET(a))
	{
	return (s - HALF_PI) * RAD2DEG;
	}
	return (s + HALF_PI) * RAD2DEG;
	}
	s = a * a;
	#ifdef DEVICE_LOW_SPEED
	return (((((0.1420889944f * s + 0.1999355085f) * s - 0.3333314528f) * s) + 1.0f) * a) * RAD2DEG;
	#else
	return ((((((((((0.0028662257f * s - 0.0161657367f) * s + 0.0429096138f) * s - 0.0752896400f)
	* s + 0.1065626393f) * s - 0.1420889944f) * s + 0.1999355085f) * s - 0.3333314528f) * s) + 1.0f) * a) * RAD2DEG;
	// return ((((((((0.0429096138f * s - 0.0752896400f)
	//    * s + 0.1065626393f) * s - 0.1420889944f) * s + 0.1999355085f) * s - 0.3333314528f) * s) + 1.0f) * a) * RAD2DEG;
	#endif
	*/
}

/*
float MathLib::__ArcTan(float x, float y)
{
float a, s;
if(abs(y) > abs(x))
{
a = x / y;
s = a * a;
#ifdef DEVICE_LOW_SPEED
s = ((((0.1420889944f * s + 0.1999355085f) * s - 0.3333314528f) * s) + 1.0f) * a;
#else
s = (((((((((0.0028662257f * s - 0.0161657367f) * s + 0.0429096138f) * s - 0.0752896400f)
* s + 0.1065626393f) * s - 0.1420889944f) * s + 0.1999355085f) * s - 0.3333314528f) * s) + 1.0f) * a;
// s = - (((((((0.0429096138f * s - 0.0752896400f)
//      * s + 0.1065626393f) * s - 0.1420889944f) * s + 0.1999355085f) * s - 0.3333314528f) * s) + 1.0f) * a;
#endif
return s * RAD2DEG;
}
a = y / x;
s = a * a;
#ifdef DEVICE_LOW_SPEED
s = - ((((0.1420889944f * s + 0.1999355085f) * s - 0.3333314528f) * s) + 1.0f) * a;
#else
s = - (((((((((0.0028662257f * s - 0.0161657367f) * s + 0.0429096138f) * s - 0.0752896400f)
* s + 0.1065626393f) * s - 0.1420889944f) * s + 0.1999355085f) * s - 0.3333314528f) * s) + 1.0f) * a;
// s = (((((((0.0429096138f * s - 0.0752896400f)
//   * s + 0.1065626393f) * s - 0.1420889944f) * s + 0.1999355085f) * s - 0.3333314528f) * s) + 1.0f) * a;

if(IS_FLOAT_SIGN_BIT_SET(a))
{
return (s - HALF_PI) * RAD2DEG;
}
return (s + HALF_PI) * RAD2DEG;
#endif
}
*/

float MathLib::arcTan(float x, float y)
{
	/*
	y = 0.5f;
	x = 1.0f;
	float f2 = atan2(x, y) * RAD2DEG;
	float f = __ArcTan(x, y);
	OS_ASSERT(abs(f-f2) < 0.1);
	return f;
	*/
	return util_atan2(x, y);
}

// =========================================================================================
// =========================================================================================
// =========================================================================================
// http://www.raspberryginger.com/jbailey/minix/html/dir_c621c1a38526b1de0afafbfeaed1eb2c.html

struct f64
{
	OS_U32 low_word;
	OS_U32 high_word;
};

#define F64_SIGN_SHIFT  31
#define F64_SIGN_MASK   1

#define F64_EXP_SHIFT   20
#define F64_EXP_MASK    0x7ff
#define F64_EXP_BIAS    1023
#define F64_EXP_MAX     2047

#define F64_MANT_SHIFT  0
#define F64_MANT_MASK   0xfffff

#define F64_GET_SIGN(fp)        (((fp)->high_word >> F64_SIGN_SHIFT) & \
	F64_SIGN_MASK)
#define F64_GET_EXP(fp)         (((fp)->high_word >> F64_EXP_SHIFT) & \
	F64_EXP_MASK)
#define F64_SET_EXP(fp, val)    ((fp)->high_word= ((fp)->high_word &    \
	~(F64_EXP_MASK << F64_EXP_SHIFT)) |     \
	(((val) & F64_EXP_MASK) << F64_EXP_SHIFT))

#define F64_GET_MANT_LOW(fp)            ((fp)->low_word)
#define F64_SET_MANT_LOW(fp, val)       ((fp)->low_word= (val))
#define F64_GET_MANT_HIGH(fp)   (((fp)->high_word >> F64_MANT_SHIFT) & \
	F64_MANT_MASK)
#define F64_SET_MANT_HIGH(fp, val)      ((fp)->high_word= ((fp)->high_word & \
	~(F64_MANT_MASK << F64_MANT_SHIFT)) |   \
	(((val) & F64_MANT_MASK) << F64_MANT_SHIFT))


float MathLib::frexp(float x, int * eptr)
{
	struct f64 *f64p;
	int exp, exp_bias;
	double factor;

	double value = x;
	f64p= (struct f64 *)&value;
	exp_bias= 0;

	exp= F64_GET_EXP(f64p);
	if (exp == F64_EXP_MAX)
	{       /* Either infinity or Nan */
		*eptr= 0;
		return (float)value;
	}
	if (exp == 0)
	{
		/* Either 0 or denormal */
		if (F64_GET_MANT_LOW(f64p) == 0 && F64_GET_MANT_HIGH(f64p) == 0)
		{
			*eptr= 0;
			return (float)value;
		}

		/* Multiply by 2^64 */
		factor= 65536.0;        /* 2^16 */
		factor *= factor;       /* 2^32 */
		factor *= factor;       /* 2^64 */
		value *= factor;
		exp_bias= 64;
		exp= F64_GET_EXP(f64p);
	}

	exp= exp - F64_EXP_BIAS - exp_bias + 1;
	*eptr= exp;
	F64_SET_EXP(f64p, F64_EXP_BIAS-1);

	return (float)value;
}

#ifndef HUGE_VAL
#define HUGE_VAL __huge_val()
#endif

static double __huge_val()
{
	return DBL_MAX; // 1.0e+1000; /* This will generate a warning */
}

float MathLib::ldexp(float x, int exp)
{
	int sign = 1;
	int currexp;

	if(isNan(x))
	{
		// errno = EDOM;
		return x;
	}
	if (x == 0.0f) return 0.0f;
	/*
	if (fl<0) {
	fl = -fl;
	sign = -1;
	}
	if (fl > DBL_MAX) { // for infinity
	// errno = ERANGE;
	return (float)(sign * fl);
	}
	*/

	if (IS_FLOAT_SIGN_BIT_SET(x)) { // fl<0) {
		x = -x;
		sign = -1;
	}
	if (x > FLT_MAX) {             /* for infinity */
		// errno = ERANGE;
		return sign * x;
	}

	double fl = frexp(x, &currexp);
	exp += currexp;
	if (exp > 0) {
		if (exp > DBL_MAX_EXP) {
			// errno = ERANGE;
			return (float)(sign * HUGE_VAL);
		}
		while (exp>30) {
			fl *= (double) (1L << 30);
			exp -= 30;
		}
		fl *= (double) (1L << exp);
	}
	else
	{
		/* number need not be normalized */
		if (exp < DBL_MIN_EXP - DBL_MANT_DIG)
		{
			return 0.0;
		}
		while (exp<-30)
		{
			fl /= (double) (1L << 30);
			exp += 30;
		}
		fl /= (double) (1L << -exp);
	}
	return (float)(sign * fl);
}

bool MathLib::isNan(float f)
{
	return (*((long*)&f) & 0x7f800000) == 0x7f800000; // && (*((long*)&f) & 0x007fffff) != 0;
}

static double __modf(double value, double * iptr)
{
	struct f64 *f64p;
	double tmp;
	int exp;
	int mask_bits;
	unsigned long mant;

	f64p= (struct f64 *)&value;

	exp= F64_GET_EXP(f64p);
	exp -= F64_EXP_BIAS;
	if (exp < 0)
	{
		*iptr= 0;
		return value;
	}
	mask_bits= 52-exp;
	if (mask_bits <= 0)
	{
		*iptr= value;
		return 0;
	}
	tmp= value;
	if (mask_bits >= 32)
	{
		F64_SET_MANT_LOW(f64p, 0);
		mask_bits -= 32;
		mant= F64_GET_MANT_HIGH(f64p);
		mant &= ~((1 << mask_bits)-1);
		F64_SET_MANT_HIGH(f64p, mant);
	}
	else
	{
		mant= F64_GET_MANT_LOW(f64p);
		mant &= ~((1 << mask_bits)-1);
		F64_SET_MANT_LOW(f64p, mant);
	}
	*iptr= value;
	return tmp-value;
}

float MathLib::floor(float x)
{
	double val;
	return __modf(x, &val) < 0 ? (float)val - 1.0f : (float)val;
	/*
	this also works if modf always returns a positive
	fractional part
	*/
}

float MathLib::ceil(float x)
{
	double val;
	return __modf(x, &val) > 0 ? (float)val + 1.0f : (float)val;
	/*
	this also works if modf always returns a positive
	fractional part
	*/
}

float MathLib::epsRound(float f, float eps)
{
	return floor(f / eps + 0.5f) * eps;
}

int MathLib::round(float f)
{
	return (int)(f + 0.5f);
}

float MathLib::mod(float x, float y)
{
#if 1
	if(y == 0)
	{
		// errno = EDOM;
		return 0;
	}
	double val;
	float frac = (float)__modf( x / y, &val);
	return frac * y;

	/*
	val = x / y;
	if (val > LONG_MIN && val < LONG_MAX) {
	i = val;
	return x - i * y;
	}
	*/
#else
	return x - int(x / y) * y;
#endif
}

float MathLib::pow(float x, float y)
{
	if(x == 0.0f)
	{
		return 0.0f;
	}
	if(y == 0.0f)
	{
		return 1.0f;
	}
	return exp(y * log(x));
}

float MathLib::exp(float f)
{
	int i, s, e, m, exponent;
	float x, x2, y, p, q;

	x = f * 1.44269504088896340f;    // multiply with (1 / log(2))
#if 1
	i = *(int*)&x;
	s = (i >> IEEE_FLT_SIGN_BIT);
	e = ((i >> IEEE_FLT_MANTISSA_BITS) & ((1 << IEEE_FLT_EXPONENT_BITS) - 1)) - IEEE_FLT_EXPONENT_BIAS;
	m = (i & ((1 << IEEE_FLT_MANTISSA_BITS) - 1)) | (1 << IEEE_FLT_MANTISSA_BITS);
	i = ((m >> (IEEE_FLT_MANTISSA_BITS - e)) & ~(e >> 31)) ^ s;
#else
	i = (int) x;
	if (x < 0.0f) {
		i--;
	}
#endif
	exponent = (i + IEEE_FLT_EXPONENT_BIAS) << IEEE_FLT_MANTISSA_BITS;
	y = *(float*)&exponent;
	x -= (float) i;
	if(x >= 0.5f)
	{
		x -= 0.5f;
		y *= 1.4142135623730950488f;  // multiply with sqrt(2)
	}
	x2 = x * x;
	p = x * (7.2152891511493f + x2 * 0.0576900723731f);
	q = 20.8189237930062f + x2;
	x = y * (q + p) / (q - p);
	return x;
}

#if 1

#define M_1_SQRT2   0.70710678118654752440084436210484904

#define POLYNOM1(x, a) ((a)[1]*(x)+(a)[0])
#define POLYNOM2(x, a) (POLYNOM1((x),(a)+1)*(x)+(a)[0])
#define POLYNOM3(x, a) (POLYNOM2((x),(a)+1)*(x)+(a)[0])

float MathLib::log(float x)
{
	/*
	Algorithm and coefficients from:
	"Software manual for the elementary functions"
	by W.J. Cody and W. Waite, Prentice-Hall, 1980
	*/
	static double a[] = {
		-0.64124943423745581147e2,
		0.16383943563021534222e2,
		-0.78956112887491257267e0
	};
	static double b[] = {
		-0.76949932108494879777e3,
		0.31203222091924532844e3,
		-0.35667977739034646171e2,
		1.0
	};

	double  znum, zden, z, w;
	int     exponent;

	if (isNan(x)) {
		// errno = EDOM;
		return (float)x;
	}
	if (x < 0) {
		// errno = EDOM;
		return (float)-HUGE_VAL;
	}
	else if (x == 0) {
		// errno = ERANGE;
		return (float)-HUGE_VAL;
	}
	if (x <= DBL_MAX) {
	}
	else return x;  /* for infinity and Nan */
	x = frexp(x, &exponent);
	if (x > M_1_SQRT2) {
		znum = (x - 0.5) - 0.5;
		zden = x * 0.5 + 0.5;
	}
	else {
		znum = x - 0.5;
		zden = znum * 0.5 + 0.5;
		exponent--;
	}
	z = znum/zden; w = z * z;
	x = (float)(z + z * w * (POLYNOM2(w,a)/POLYNOM3(w,b)));
	z = exponent;
	x += (float)(z * (-2.121944400546905827679e-4));
	return (float)(x + z * 0.693359375);
}
#else
float MathLib::Log(float f)
{
	int i, exponent;
	float y, y2;

	i = *(int*)&f;
	exponent = ((i >> IEEE_FLT_MANTISSA_BITS) & ((1 << IEEE_FLT_EXPONENT_BITS) - 1)) - IEEE_FLT_EXPONENT_BIAS;
	i -= (exponent + 1) << IEEE_FLT_MANTISSA_BITS;  // get value in the range [.5, 1>
	y = *(float*)&i;
	y *= 1.4142135623730950488f;            // multiply with sqrt(2)
	y = (y - 1.0f) / (y + 1.0f);
	y2 = y * y;
	y = y * (2.000000000046727f + y2 * (0.666666635059382f + y2 * (0.4000059794795f + y2 * (0.28525381498f + y2 * 0.2376245609f))));
	y += 0.693147180559945f * ((float)exponent + 0.5f);
	return y;
}
#endif

int MathLib::floorPowerOfTwo(int x)
{
	return ceilPowerOfTwo(x) >> 1;
}

int MathLib::ceilPowerOfTwo(int x)
{
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x++;
	return x;
}

bool MathLib::isPowerOfTwo(int x)
{
	return (x & (x - 1)) == 0 && x > 0;
}

// =========================================================================================
// =========================================================================================
// =========================================================================================

bool vec3::fixDegenerateNormal()
{
	if (x == 0.0f)
	{
		if (y == 0.0f)
		{
			if (z > 0.0f)
			{
				if (z != 1.0f)
				{
					z = 1.0f;
					return true;
				}
			} 
			else
			{
				if (z != -1.0f)
				{
					z = -1.0f;
					return true;
				}
			}
			return false;
		} else if (z == 0.0f)
		{
			if (y > 0.0f)
			{
				if (y != 1.0f)
				{
					y = 1.0f;
					return true;
				}
			}
			else
			{
				if (y != -1.0f)
				{
					y = -1.0f;
					return true;
				}
			}
			return false;
		}
	}
	else if (y == 0.0f)
	{
		if (z == 0.0f)
		{
			if (x > 0.0f)
			{
				if (x != 1.0f)
				{
					x = 1.0f;
					return true;
				}
			}
			else
			{
				if (x != -1.0f)
				{
					x = -1.0f;
					return true;
				}
			}
			return false;
		}
	}
	if (MathLib::abs(x) == 1.0f)
	{
		if (y != 0.0f || z != 0.0f)
		{
			y = z = 0.0f;
			return true;
		}
		return false;
	}
	else if (MathLib::abs(y) == 1.0f)
	{
		if (x != 0.0f || z != 0.0f)
		{
			x = z = 0.0f;
			return true;
		}
		return false;
	}
	else if (MathLib::abs(z) == 1.0f)
	{
		if (x != 0.0f || y != 0.0f)
		{
			x = y = 0.0f;
			return true;
		}
		return false;
	}
	return false;
}

// =========================================================================================

int Bounds3::planeSide(const Plane& plane, float epsilon) const
{
	vec3 center = vec3
		(
		(b[0].x + b[1].x) * 0.5f,
		(b[0].y + b[1].y) * 0.5f,
		(b[0].z + b[1].z) * 0.5f 
		);

	float d1 = plane.distTo(center);
	float d2 = MathLib::abs((b[1].x - center.x) * plane.normal.x)
		+ MathLib::abs((b[1].y - center.y) * plane.normal.y)
		+ MathLib::abs((b[1].z - center.z) * plane.normal.z);

	if(d1 - d2 > epsilon)
	{
		return SIDE_FRONT;
	}
	if(d1 + d2 < -epsilon)
	{
		return SIDE_BACK;
	}
	return SIDE_CROSS;
}

// =========================================================================================

Plane::Plane(const vec3& p1, const vec3& p2, const vec3& p3, bool fixDegenerateNormal)
{
	init(p1, p2, p3, fixDegenerateNormal);
}

void Plane::init(const vec3& p1, const vec3& p2, const vec3& p3, bool fixDegenerateNormal)
{
	vec3 n = (p1.sub(p2)).cross(p3.sub(p2)).norm();
	if(fixDegenerateNormal)
	{
		n.fixDegenerateNormal();
	}
	init(n, p2);
}

void Plane::init(const vec3& n, const vec3& p)
{
#ifdef _DEBUG 
	float sl = n.sqrLen();
	OS_ASSERT(sl >= 0.993f && sl <= 1.001f);
#endif

	normal = n;
	dist = n.dot(p);
}

int Plane::planeAxeNum() const
{
	float n[] = {MathLib::abs(normal.x), MathLib::abs(normal.y), MathLib::abs(normal.z)};
	if(n[0] > n[1])
	{
		return n[0] > n[2] ? 0 : 2;
	}
	return n[1] > n[2] ? 1 : 2;
}

float Plane::distTo(const vec3& p) const
{
	return normal.dot(p) - dist;
}

int Plane::side(const vec3& p) const
{
	float dist = distTo(p);
	if(dist > SIDE_ON_EPSILON)
		return SIDE_FRONT;
	if(dist < -SIDE_ON_EPSILON)
		return SIDE_BACK;

	return SIDE_ON;
}

bool Plane::compareEps(const Plane& b, float distEps, float minNormalDot) const
{
	if(MathLib::abs(dist - b.dist) > distEps || normal.dot(b.normal) < minNormalDot)
		return false;

	return true;
}

bool Plane::rayIntersection(const vec3& start, const vec3& dir, float& scale) const
{
	float d1 = distTo(start);
	float d2 = normal.dot(dir);
	if(d2 == 0.0f)
	{
		return false;
	}
	scale = -d1 / d2;
	return true;
}

bool Plane::splitLine(const vec3& start, const vec3& end, vec3& mid, float& dot) const
{
	float dists[] = { distTo(start), distTo(end) };
	int side[] = { dists[0] >= 0.0f, dists[1] >= 0.0f };
	if(side[0] ^ side[1])
	{
		float n;
		int j;

		// always calculate the split going from the same side
		// or minor epsilon issues can happen
		if(side[0])
		{
			dot = dists[0] / (dists[0] - dists[1]);
			for(j = 0; j < 3; j++){
				// avoid round off error when possible
				n = normal.at(j);
				if(n == 1.0f)
					mid.set(j, dist);
				else if(n == -1.0f)
					mid.set(j, -dist);
				else
					mid.set(j, start.at(j) + dot * (end.at(j) - start.at(j)));
			}
		}
		else
		{
			dot = dists[1] / (dists[1] - dists[0]);
			for(j = 0; j < 3; j++) {	
				// avoid round off error when possible
				n = normal.at(j);
				if(n == 1.0f)
					mid.set(j, dist);
				else if(n == -1.0f)
					mid.set(j, -dist);
				else
					mid.set(j, end.at(j) + dot * (start.at(j) - end.at(j)));
			}
		}
		return true;
	}
	return false;
}

vec3 Plane::reflect(const vec3& dirToPlane, const vec3& normal)
{
	/*
	N
	R       V
	\  ^  /
	\ | /
	\|/
	.
	O

	если построить Vproj - проекцию вектора V на N, то можно увидеть, что

	R + V = Vproj * 2 

	Vproj = N * длину проекции V на N, т.е.
	Vproj = N * dot(V, N) - при условии, что N нормализована

	R = Vproj * 2 - V = 2 * N * dot(V, N) - V

	В данном случае V направлен от точки O, в твоем случае V = -I (I - направление луча), и формула выглядит как 
	R = I - 2 * N * dot(I, N)
	*/
	return dirToPlane.sub( normal.mul( 2.0f * normal.dot(dirToPlane) ) ).norm();
}

// =========================================================================================

Vectors::Vectors(const vec3& __forward, const vec3& __up)
{
#ifdef _DEBUG 
	float sl = __forward.sqrLen();
	OS_ASSERT(sl >= 0.992f && sl <= 1.001f);
	sl = __up.sqrLen();
	OS_ASSERT(sl >= 0.992f && sl <= 1.001f);
#endif

	forward = __forward;
	right = __forward.cross(MathLib::abs(__forward.dot(__up)) < 0.99f ? __up : VEC3_FORWARD).norm();
	up = right.cross(forward).norm();
}

Vectors::Vectors(const Angles& a)
{
	a.toVectors(*this);
}

bool Vectors::fixDegenerateNormal()
{
	bool f = forward.fixDegenerateNormal();
	bool r = right.fixDegenerateNormal();
	bool u = up.fixDegenerateNormal();
	return f | r | u;
}

Angles& Vectors::toAngles(Angles& out) const
{
	float sp = forward.z;
	if(sp > 1)
	{
		sp = 1;
	}
	else if(sp < -1)
	{
		sp = -1;
	}

	// float pitch = normAngle180(90-arcSin(sp));
	out.pitch = MathLib::normAngle180(-MathLib::arcSin(sp));
	if(out.pitch >= -89 && out.pitch <= 89)
	{
		out.yaw  = MathLib::normAngle180(MathLib::arcTan(-forward.x, forward.y));
		out.roll = MathLib::normAngle180(-MathLib::arcTan(right.z, up.z));
	}
	else
	{
		out.yaw  = MathLib::normAngle180(-MathLib::arcTan(right.x, right.y));
		out.roll = 0;
	}
	return out;
}

Angles::Angles(const vec3& forward, const vec3& up)
{
	Vectors(forward, up).toAngles(*this);
}

Angles& Angles::setDiff(const Angles& a, const Angles& b)
{
	yaw   = MathLib::anglesDiff(a.yaw,   b.yaw);
	pitch = MathLib::anglesDiff(a.pitch, b.pitch);
	roll  = MathLib::anglesDiff(a.roll,  b.roll);
	return *this;
}

vec3 Angles::forward() const
{
	float sy, cy, sp, cp;

	MathLib::sinCos(yaw, sy, cy);
	MathLib::sinCos(pitch, sp, cp);

	vec3 v = vec3(-cp * sy, cp * cy, -sp);
	v.fixDegenerateNormal();
	return v;
}

Angles Angles::slerpTo(const Angles& to, float t) const
{
	Angles diff;
	diff.setDiff(*this, to);
	return slerpByDiff(diff, t);
}

Angles Angles::slerpByDiff(const Angles& diff, float t) const
{
	return Angles(yaw + diff.yaw * t,
		pitch + diff.pitch * t,
		roll + diff.roll * t);
}

Vectors& Angles::toVectors(Vectors& out) const
{
	float sy, cy, sp, cp, sr, cr;

	MathLib::sinCos(yaw, sy, cy);
	MathLib::sinCos(pitch, sp, cp);
	MathLib::sinCos(roll, sr, cr);

	out.forward = vec3(-cp * sy, cp * cy, -sp);
	out.right   = vec3(sr * sp * sy + cr * cy, -sr * sp * cy + cr * sy, -sr * cp);
	out.up      = vec3(sr * cy - cr * sp * sy, cr * sp * cy + sr * sy, cr * cp);

	out.forward.fixDegenerateNormal();
	out.right.fixDegenerateNormal();
	out.up.fixDegenerateNormal();

	return out;
}

// =========================================================================================

Mat4x4::Mat4x4(float s)
{
	ident(s);
}

Mat4x4::Mat4x4(const Vectors& v, const vec3& pos)
{
	rows[3] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	setVectors(v, pos);
}

Mat4x4::Mat4x4(const Angles& a, const vec3& pos)
{
	rows[3] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	setAngles(a, pos);
}

void Mat4x4::ident(float s)
{
	OS_MEMSET(this, 0, sizeof(*this));
	rows[0].x = s;
	rows[1].y = s;
	rows[2].z = s;
	rows[3].w = 1.0f;
}

Vectors& Mat4x4::toVectors(Vectors& out) const
{
	out.forward = vec3(rows[0].y, rows[1].y, rows[2].y);
	out.right   = vec3(rows[0].x, rows[1].x, rows[2].x);
	out.up      = vec3(rows[0].z, rows[1].z, rows[2].z);
	return out;
}

void Mat4x4::setVectors(const Vectors& v)
{
	rows[0] = vec4(v.right.x, v.forward.x, v.up.x, rows[0].w);
	rows[1] = vec4(v.right.y, v.forward.y, v.up.y, rows[1].w);
	rows[2] = vec4(v.right.z, v.forward.z, v.up.z, rows[2].w);
}

void Mat4x4::setVectors(const Vectors& v, const vec3& pos)
{
	rows[0] = vec4(v.right.x, v.forward.x, v.up.x, pos.x);
	rows[1] = vec4(v.right.y, v.forward.y, v.up.y, pos.y);
	rows[2] = vec4(v.right.z, v.forward.z, v.up.z, pos.z);
}

Angles& Mat4x4::toAngles(Angles& out) const
{
	Vectors temp;
	return toVectors(temp).toAngles(out);
}

void Mat4x4::setAngles(const Angles& a)
{
	Vectors temp;
	setVectors(a.toVectors(temp));
}

void Mat4x4::setAngles(const Angles& a, const vec3& pos)
{
	Vectors temp;
	setVectors(a.toVectors(temp), pos);
}

vec3 Mat4x4::translation() const
{
	return vec3(rows[0].w, rows[1].w, rows[2].w);
}

void Mat4x4::setTranslation(const vec3& pos)
{
	rows[0].w = pos.x;
	rows[1].w = pos.y;
	rows[2].w = pos.z;
}

void Mat4x4::scale(const vec3& s)
{
	rows[0].x *= s.x;
	rows[1].y *= s.y;
	rows[2].z *= s.z;
}

vec3 Mat4x4::rotateFast(const vec3& p) const
{
	/*
	assert(mat[ 3 ].x * p.x + mat[ 3 ].y * p.y + mat[ 3 ].z * p.z + mat[ 3 ].w == 1.0f);
	return xVec3(
	mat[ 0 ].x * p.x + mat[ 0 ].y * p.y + mat[ 0 ].z * p.z,
	mat[ 1 ].x * p.x + mat[ 1 ].y * p.y + mat[ 1 ].z * p.z,
	mat[ 2 ].x * p.x + mat[ 2 ].y * p.y + mat[ 2 ].z * p.z);
	*/
	return vec3(
		rows[ 0 ].x * p.x + rows[ 0 ].y * p.y + rows[ 0 ].z * p.z,
		rows[ 1 ].x * p.x + rows[ 1 ].y * p.y + rows[ 1 ].z * p.z,
		rows[ 2 ].x * p.x + rows[ 2 ].y * p.y + rows[ 2 ].z * p.z);
}

vec3 Mat4x4::transformFast(const vec3& p) const
{
	/* float s = rows[ 3 ].x * p.x + rows[ 3 ].y * p.y + rows[ 3 ].z * p.z + rows[ 3 ].w;
	if(s == 0.0f)
	{
	return VEC3_ZERO;
	}
	if(s == 1.0f)
	{
	*/
	return vec3(
		rows[ 0 ].x * p.x + rows[ 0 ].y * p.y + rows[ 0 ].z * p.z + rows[ 0 ].w,
		rows[ 1 ].x * p.x + rows[ 1 ].y * p.y + rows[ 1 ].z * p.z + rows[ 1 ].w,
		rows[ 2 ].x * p.x + rows[ 2 ].y * p.y + rows[ 2 ].z * p.z + rows[ 2 ].w);
	/*}
	else
	{
	float invS = 1.0f / s;
	return vec3(
	(rows[ 0 ].x * p.x + rows[ 0 ].y * p.y + rows[ 0 ].z * p.z + rows[ 0 ].w) * invS,
	(rows[ 1 ].x * p.x + rows[ 1 ].y * p.y + rows[ 1 ].z * p.z + rows[ 1 ].w) * invS,
	(rows[ 2 ].x * p.x + rows[ 2 ].y * p.y + rows[ 2 ].z * p.z + rows[ 2 ].w) * invS);
	}*/
}
