#ifndef __MATH_LIB_H__
#define __MATH_LIB_H__

//============================================================================
//
// Author: Evgeniy Golovin (evgeniy.golovin@unitpoint.ru)
//
//============================================================================

#include <objectscript.h>
#include <math/vector2.h>
#include <math/Color.h>
#include <float.h>

#define IS_FLOAT_SIGN_BIT_SET(f) ((*(const unsigned int*)&(f)) & (1U << 31U))
#define FLOAT_ZERO 0.000000001f

// from std lib float.h
#ifndef FLT_EPSILON

#define DBL_DIG         15                      /* # of decimal digits of precision */
#define DBL_EPSILON     2.2204460492503131e-016 /* smallest such that 1.0+DBL_EPSILON != 1.0 */
#define DBL_MANT_DIG    53                      /* # of bits in mantissa */
#define DBL_MAX         1.7976931348623158e+308 /* max value */
#define DBL_MAX_10_EXP  308                     /* max decimal exponent */
#define DBL_MAX_EXP     1024                    /* max binary exponent */
#define DBL_MIN         2.2250738585072014e-308 /* min positive value */
#define DBL_MIN_10_EXP  (-307)                  /* min decimal exponent */
#define DBL_MIN_EXP     (-1021)                 /* min binary exponent */
#define _DBL_RADIX      2                       /* exponent radix */
#define _DBL_ROUNDS     1                       /* addition rounding: near */

#define FLT_DIG         6                       /* # of decimal digits of precision */
#define FLT_EPSILON     1.192092896e-07F        /* smallest such that 1.0+FLT_EPSILON != 1.0 */
#define FLT_GUARD       0
#define FLT_MANT_DIG    24                      /* # of bits in mantissa */
#define FLT_MAX         3.402823466e+38F        /* max value */
#define FLT_MAX_10_EXP  38                      /* max decimal exponent */
#define FLT_MAX_EXP     128                     /* max binary exponent */
#define FLT_MIN         1.175494351e-38F        /* min positive value */
#define FLT_MIN_10_EXP  (-37)                   /* min decimal exponent */
#define FLT_MIN_EXP     (-125)                  /* min binary exponent */
#define FLT_NORMALIZE   0
#define FLT_RADIX       2                       /* exponent radix */
#define FLT_ROUNDS      1                       /* addition rounding: near */

#endif

#define	SIDE_ON					0
#define	SIDE_FRONT				1
#define	SIDE_BACK				2
#define	SIDE_CROSS				3

#define	SIDE_ON_EPSILON		0.001f

struct vec2;
struct vec3;
struct Plane;

/**
*  Math lib class.
*/
class MathLib
{
public:

	static const float PI;
	static const float TWO_PI;
	static const float HALF_PI;

	static const float DEG2RAD;
	static const float RAD2DEG;

	static float invSqrt(float x);
	static float sqrt(float x) { return x * invSqrt(x); }

	static float sqr(float x) { return x * x; }

	static float normAngle(float angle);
	static float normAngle180(float angle);
	static float anglesDiff(float a, float b);

	static float sin(float angle);
	static float cos(float angle);
	static void sinCos(float angle, float& sv, float& cv);

	static float tan(float angle);

	static float arcSin(float a);
	static float arcCos(float a);

	static float arcTan(float a);
	static float arcTan(float x, float y);

	static float pow(float x, float y);

	static float exp(float f);
	static float log(float f);

	static float mod(float x, float y);

	static float frexp(float x, int * eptr);
	static float ldexp(float x, int exp);

	static float floor(float x);
	static float ceil(float x);

	static bool isNan(float x);

	static inline float abs(float f)
	{
		OS_INT32 tmp = *(OS_INT32*)&f;
		tmp &= 0x7FFFFFFF;
		return *(float*)&tmp;
	}

	static float epsRound(float f, float eps); // floor(f / eps + 0.5f) * eps
	static int round(float f);

	static int floorPowerOfTwo(int);
	static int ceilPowerOfTwo(int);
	static bool isPowerOfTwo(int);

	template <class T>
	static inline T clamp(const T& f, const T& a, const T& b)
	{
		if(f < a)
			return a;
		if(f > b)
			return b;
		return f;
	}

	template <class T>
	static inline T min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	template <class T>
	static inline T max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}

	static inline float sign(float f)
	{
		if(f < 0.0f) return -1.0f;
		if(f > 0.0f) return 1.0f;
		return 0.0f;
	}

	// static inline float min(float a, float b) { return a < b ? a : b; }
	// static inline float max(float a, float b) { return a > b ? a : b; }

	static inline float lerp(float a, float b, float t) { return a * (1.0f - t) + b * t; }
};

// ===================================================================================

struct vec3;

/**
*  2d vector class.
*/

struct vec2
{
	typedef float type;

	type x; ///< x position
	type y; ///< y position

	vec2(){ x = y = 0; }
	vec2(type ax, type ay)
	{
		x = ax;
		y = ay;
	}
	vec2(type a)
	{
		x = a;
		y = a;
	}
	vec2(const vec3&);
	vec2(const oxygine::Vector2& v)
	{
		x = (type)v.x;
		y = (type)v.y;
	}

	operator oxygine::Vector2() const
	{
		return oxygine::Vector2((oxygine::Vector2::type)x, (oxygine::Vector2::type)y);
	}

	type at(int i) const
	{
		OS_ASSERT(i >= 0 && i < 2);
		return (&x)[i];
	}
	void set(int i, type f)
	{
		OS_ASSERT(i >= 0 && i < 2);
		(&x)[i] = f;
	}

	bool operator==(const vec2& b) const
	{
		return x == b.x && y == b.y;
	}
	bool operator!=(const vec2& b) const
	{
		return x != b.x || y != b.y;
	}

	vec2 operator+(const vec2& b) const
	{
		return vec2(x+b.x, y+b.y);
	}
	vec2 operator+(float b) const
	{
		return vec2(x+b, y+b);
	}

	vec2 operator-(const vec2& b) const
	{
		return vec2(x-b.x, y-b.y);
	}
	vec2 operator-(float b) const
	{
		return vec2(x-b, y-b);
	}

	vec2 operator*(const vec2& b) const
	{
		return vec2(x*b.x, y*b.y);
	}
	vec2 operator*(float b) const
	{
		return vec2(x*b, y*b);
	}

	vec2 operator/(const vec2& b) const
	{
		return vec2(x/b.x, y/b.y);
	}
	vec2 operator/(float b) const
	{
		b = 1.0f / b;
		return vec2(x*b, y*b);
	}

	vec2 operator-() const
	{
		return vec2(-x, -y);
	}

	vec2 add(const vec2& b) const
	{
		return vec2(x+b.x, y+b.y);
	}
	vec2 add(float b) const
	{
		return vec2(x+b, y+b);
	}

	vec2 sub(const vec2& b) const
	{
		return vec2(x-b.x, y-b.y);
	}
	vec2 sub(float b) const
	{
		return vec2(x-b, y-b);
	}

	vec2 mul(const vec2& b) const
	{
		return vec2(x*b.x, y*b.y);
	}
	vec2 mul(float b) const
	{
		return vec2(x*b, y*b);
	}

	vec2 div(const vec2& b) const
	{
		return vec2(x/b.x, y/b.y);
	}
	vec2 div(float b) const
	{
		b = 1.0f / b;
		return vec2(x*b, y*b);
	}

	type dot(const vec2& b) const
	{
		return x*b.x + y*b.y;
	}

	type invLen() const
	{
		return MathLib::invSqrt(dot(*this));
	}
	type len() const
	{
		return MathLib::sqrt(dot(*this));
	}
	type sqrLen() const
	{
		return dot(*this);
	}

	vec2 norm() const
	{
		return mul(invLen());
	}

	type lenNorm(vec2& dir)
	{
		type sqrLen = dot(*this);
		type invLen = MathLib::invSqrt(sqrLen);
		dir.x = x * invLen;
		dir.y = y * invLen;
		return sqrLen * invLen;
	}

	vec2 min(const vec2& b) const
	{
		return vec2(MathLib::min(x, b.x), MathLib::min(y, b.y));
	}

	vec2 max(const vec2& b) const
	{
		return vec2(MathLib::max(x, b.x), MathLib::max(y, b.y));
	}

	vec2 neg() const
	{
		return vec2( -x, -y );
	}
};

// ===================================================================================

/**
*  3d vector class.
*/

struct vec3
{
	typedef float type;

	type x;  ///< x pos
	type y;  ///< y pos
	type z;  ///< z pos

	vec3(){ x = y = z = 0; }
	vec3(type ax, type ay, type az)
	{
		x = ax;
		y = ay;
		z = az;
	}
	vec3(type a)
	{
		x = a;
		y = a;
		z = a;
	}
	vec3(const vec2& b)
	{
		x = b.x;
		y = b.y;
		z = 0.0f;
	}
	vec3(const oxygine::Color& color)
	{
		// oxygine::Color color = _color.premultiplied();
		x = (type)color.getRedF();
		y = (type)color.getGreenF();
		z = (type)color.getBlueF();
	}


	operator oxygine::Color() const 
	{
		OS_BYTE rgb[3];
		for(int i = 0; i < 3; i++){
			float v = at(i) * 255.0f;
			rgb[i] = (OS_BYTE)(v < 0.0f ? 0.0f : v > 255.0f ? 255.0f : v);
		}
		return oxygine::Color(rgb[0], rgb[1], rgb[2]);
	}


	type at(int i) const
	{
		OS_ASSERT(i >= 0 && i < 3);
		return (&x)[i];
	}
	void set(int i, float f)
	{
		OS_ASSERT(i >= 0 && i < 3);
		(&x)[i] = f;
	}

	bool fixDegenerateNormal();

	bool operator==(const vec3& b) const
	{
		return x == b.x && y == b.y && z == b.z;
	}
	bool operator!=(const vec3& b) const
	{
		return x != b.x || y != b.y || z != b.z;
	}

	vec3 operator+(const vec3& b) const
	{
		return vec3(x+b.x, y+b.y, z+b.z);
	}
	vec3 operator+(float b) const
	{
		return vec3(x+b, y+b, z+b);
	}

	vec3 operator-(const vec3& b) const
	{
		return vec3(x-b.x, y-b.y, z-b.z);
	}
	vec3 operator-(float b) const
	{
		return vec3(x-b, y-b, z-b);
	}
	/* friend class vec3 operator-(float a, const vec3& b) const
	{
	return vec3(a-b.x, a-b.y, a-b.z);
	} */

	vec3 operator*(const vec3& b) const
	{
		return vec3(x*b.x, y*b.y, z*b.z);
	}
	vec3 operator*(float b) const
	{
		return vec3(x*b, y*b, z*b);
	}

	vec3 operator/(const vec3& b) const
	{
		return vec3(x/b.x, y/b.y, z/b.z);
	}
	vec3 operator/(float b) const
	{
		b = 1.0f / b;
		return vec3(x*b, y*b, z*b);
	}

	vec3 operator-() const
	{
		return vec3(-x, -y, -z);
	}

	// ----------------------------

	vec3 add(const vec3& b) const
	{
		return vec3(x+b.x, y+b.y, z+b.z);
	}
	vec3 add(float b) const
	{
		return vec3(x+b, y+b, z+b);
	}

	vec3 sub(const vec3& b) const
	{
		return vec3(x-b.x, y-b.y, z-b.z);
	}
	vec3 sub(float b) const
	{
		return vec3(x-b, y-b, z-b);
	}

	vec3 mul(const vec3& b) const
	{
		return vec3(x*b.x, y*b.y, z*b.z);
	}
	vec3 mul(float b) const
	{
		return vec3(x*b, y*b, z*b);
	}

	vec3 div(const vec3& b) const
	{
		return vec3(x/b.x, y/b.y, z/b.z);
	}
	vec3 div(float b) const
	{
		b = 1.0f / b;
		return vec3(x*b, y*b, z*b);
	}

	type dot(const vec3& b) const
	{
		return x*b.x + y*b.y + z*b.z;
	}
	vec3 cross(const vec3& b) const
	{
		return vec3(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);
	}

	type invLen() const
	{
		return MathLib::invSqrt(dot(*this));
	}
	type len() const
	{
		return MathLib::sqrt(dot(*this));
	}
	type sqrLen() const
	{
		return dot(*this);
	}

	vec3 norm() const
	{
		return mul(invLen());
	}

	type lenNorm(vec3& dir) const
	{
		type sqrLen = dot(*this);
		type invLen = MathLib::invSqrt(sqrLen);
		dir.x = x * invLen;
		dir.y = y * invLen;
		dir.z = z * invLen;
		return sqrLen * invLen;
	}

	vec3 min(const vec3& b)
	{
		return vec3(MathLib::min(x, b.x), MathLib::min(y, b.y), MathLib::min(z, b.z));
	}

	vec3 max(const vec3& b)
	{
		return vec3(MathLib::max(x, b.x), MathLib::max(y, b.y), MathLib::max(z, b.z));
	}

	vec3 neg() const
	{
		return vec3( -x, -y, -z );
	}
};

inline vec2::vec2(const vec3& p)
{
	x = p.x;
	y = p.y;
}

// ===================================================================================

/**
*  4d vector class.
*/

struct vec4
{
	float x;  ///< x pos
	float y;  ///< y pos
	float z;  ///< z pos
	float w;  ///< w pos

	vec4(){ x = y = z = w = 0; }
	vec4(float ax, float ay, float az, float aw)
	{
		x = ax;
		y = ay;
		z = az;
		w = aw;
	}

	float at(int i) const
	{
		OS_ASSERT(i >= 0 && i < 4);
		return (&x)[i];
	}
	void set(int i, float f)
	{
		OS_ASSERT(i >= 0 && i < 4);
		(&x)[i] = f;
	}
};

// ===================================================================================

/**
*  2d bounds class.
*/

struct Bounds2
{
	vec2 b[2];  ///< min & max bounds

	Bounds2(){}
	Bounds2(const vec2& p_a, const vec2& p_b) { b[0] = p_a; b[1] = p_b; }

	Bounds2& init(const vec2& p)
	{
		b[0] = p;
		b[1] = p;
		return *this;
	}

	Bounds2& add(const vec2& p)
	{
		b[0] = b[0].min(p);
		b[1] = b[1].max(p);
		return *this;
	}

	Bounds2& add(const Bounds2& a)
	{
		b[0] = b[0].min(a.b[0]);
		b[1] = b[1].max(a.b[1]);
		return *this;
	}

	void expand(float f)
	{
		b[0] = b[0].sub(f);
		b[1] = b[1].add(f);
	}

	bool containsPoint(const vec2& p) const
	{
		return p.x >= b[0].x && p.y >= b[0].y
			&& p.x <= b[1].x && p.y <= b[1].y;
	}

	bool containsBounds(const Bounds2& a) const
	{
		if(a.b[0].x >= b[0].x && a.b[0].y >= b[0].y
			&& a.b[1].y <= b[1].y && a.b[1].y <= b[1].y
			)
		{
			return true;
		}
		return false;
	}

	bool intersectsBounds(const Bounds2& a) const
	{
		if(a.b[1].x < b[0].x || a.b[1].y < b[0].y 
			|| a.b[0].x > b[1].x || a.b[0].y > b[1].y)
		{
			return false;
		}
		return true;
	}
};

// ===================================================================================

/**
*  3d bounds class.
*/

struct Bounds3
{
	vec3 b[2];  ///< min & max bounds

	Bounds3(){}
	Bounds3(const vec3& p_a, const vec3& p_b) { b[0] = p_a; b[1] = p_b; }

	Bounds3& init(const vec3& p)
	{
		b[0] = p;
		b[1] = p;
		return *this;
	}

	Bounds3& add(const vec3& p)
	{
		b[0] = b[0].min(p);
		b[1] = b[1].max(p);
		return *this;
	}

	Bounds3& add(const Bounds3& a)
	{
		b[0] = b[0].min(a.b[0]);
		b[1] = b[1].max(a.b[1]);
		return *this;
	}

	void expand(float f)
	{
		b[0] = b[0].sub(f);
		b[1] = b[1].add(f);
	}

	bool containsPoint(const vec3& p) const
	{
		return p.x >= b[0].x && p.y >= b[0].y && p.z >= b[0].z
			&& p.x <= b[1].x && p.y <= b[1].y && p.z <= b[1].z;
	}

	bool containsBounds(const Bounds3& a) const
	{
		if(a.b[0].x >= b[0].x && a.b[0].y >= b[0].y && a.b[0].z >= b[0].z
			&& a.b[1].y <= b[1].y && a.b[1].y <= b[1].y && a.b[1].z <= b[1].z
			)
		{
			return true;
		}
		return false;
	}

	bool intersectsBounds(const Bounds3& a) const
	{
		if(a.b[1].x < b[0].x || a.b[1].y < b[0].y || a.b[1].z < b[0].z
			|| a.b[0].x > b[1].x || a.b[0].y > b[1].y || a.b[0].z > b[1].z)
		{
			return false;
		}
		return true;
	}

	int planeSide(const Plane& plane, float epsilon = SIDE_ON_EPSILON) const;
};

// ===================================================================================

struct Plane
{
	vec3 normal;
	float dist;

	Plane(){ dist = 0; }
	Plane(float a, float b, float c, float d): normal(a,b,c), dist(d){}
	Plane(const vec3& p1, const vec3& p2, const vec3& p3, bool fixDegenerateNormal = true);
	Plane(const vec3& n, const vec3& p){ init(n, p); }

	void init(const vec3& p1, const vec3& p2, const vec3& p3, bool fixDegenerateNormal = true);
	void init(const vec3& n, const vec3& p);

	int planeAxeNum() const;
	float distTo(const vec3& p) const;
	int side(const vec3& p) const;

	bool compareEps(const Plane& b, float distEps, float minNormalDot = 0.98f) const;

	Plane neg() const { return Plane(-normal.x, -normal.y, -normal.z, -dist); }

	bool rayIntersection(const vec3& start, const vec3& dir, float& scale) const;
	bool splitLine(const vec3& start, const vec3& end, vec3& mid, float& dot) const;

	static vec3 reflect(const vec3& dirToPlane, const vec3& normal);
};

// ===================================================================================

struct Angles;

/**
*  3d normalized vectors class.
*/

struct Vectors
{
	vec3 forward, right, up;

	Vectors(){}
	Vectors(const vec3& f, const vec3& r, const vec3& u)
	{
		forward = f;
		right = r;
		up = u;
	}
	Vectors(const vec3& f, const vec3& u);
	Vectors(const Angles& a);

	vec3& at(int i)
	{
		OS_ASSERT(i >= 0 && i < 3);
		return (&forward)[i];
	}
	const vec3& at(int i) const
	{
		OS_ASSERT(i >= 0 && i < 3);
		return (&forward)[i];
	}
	float at(int i, int j) const { return at(i).at(j); }

	bool fixDegenerateNormal();

	Angles& toAngles(Angles& out) const;
};

// ===================================================================================

#define VEC3_ZERO     vec3(0.0f, 0.0f, 0.0f)  ///< 3d zero vector

#define VEC3_FORWARD  vec3(0.0f, 1.0f, 0.0f)  ///< 3d forward vector
#define VEC3_RIGHT    vec3(1.0f, 0.0f, 0.0f)  ///< 3d right vector
#define VEC3_UP       vec3(0.0f, 0.0f, 1.0f)  ///< 3d up vector

/**
*  Angles class.
*/

struct Angles
{
	float yaw;    ///< yaw angle in grad
	float pitch;  ///< pitch angle in grad
	float roll;   ///< roll angle in grad

	Angles(float p_yaw = 0, float p_pitch = 0, float p_roll = 0)
	{
		yaw = p_yaw;
		pitch = p_pitch;
		roll = p_roll;
	}
	Angles(const vec3& forward, const vec3& up);

	Angles& setDiff(const Angles& a, const Angles& b);

	vec3 forward() const;

	Vectors& toVectors(Vectors& out) const;

	float& at(int i)
	{
		OS_ASSERT(i >= 0 && i < 3);
		return (&yaw)[i];
	}
	const float at(int i) const
	{
		OS_ASSERT(i >= 0 && i < 3);
		return (&yaw)[i];
	}

	Angles slerpTo(const Angles& to, float t) const;
	Angles slerpByDiff(const Angles& diff, float t) const;
};

// ===================================================================================

/**
*  4x4 matrix class.
*/

struct Mat4x4
{
	vec4 rows[4]; ///< matrix rows

	Mat4x4(float s = 1);
	Mat4x4(const Vectors& v, const vec3& pos);
	Mat4x4(const Angles& a, const vec3& pos);

	void ident(float s = 1);

	Vectors& toVectors(Vectors& out) const;
	void setVectors(const Vectors& v);
	void setVectors(const Vectors& v, const vec3& pos);

	Angles& toAngles(Angles& out) const;
	void setAngles(const Angles& a);
	void setAngles(const Angles& a, const vec3& pos);

	vec3 translation() const;
	void setTranslation(const vec3& pos);

	void scale(const vec3& s);

	vec3 rotateFast(const vec3& v) const;
	vec3 transformFast(const vec3& p) const;
};

// ===================================================================================

#endif // __MATH_LIB_H__
