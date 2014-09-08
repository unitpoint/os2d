#include "EaseFunction.h"

namespace EaseFunction {

#define OS_MATH_PI 3.1415926535897932384626433832795
#define OS_MATH_PI_2 ((float)OS_MATH_PI * 2.0f)

#define easeReverseHelper(t, func) (1.0f - func(1.0f - (t)))

// Linear
float linear(float time)
{
    return time;
}

// Sine Ease
float sineEaseIn(float time)
{
    return -1 * cosf(time * (float)OS_MATH_PI_2) + 1;
}
    
float sineEaseOut(float time)
{
    return sinf(time * (float)OS_MATH_PI_2);
}
    
float sineEaseInOut(float time)
{
    return -0.5f * (cosf((float)OS_MATH_PI * time) - 1);
}


// Quad Ease
float quadEaseIn(float time)
{
    return time * time;
}
    
float quadEaseOut(float time)
{
    return -1 * time * (time - 2);
}
    
float quadEaseInOut(float time)
{
    time = time*2;
    if (time < 1)
        return 0.5f * time * time;
    --time;
    return -0.5f * (time * (time - 2) - 1);
}

// Cubic Ease
float cubicEaseIn(float time)
{
    return time * time * time;
}
float cubicEaseOut(float time)
{
    time -= 1;
    return (time * time * time + 1);
}
float cubicEaseInOut(float time)
{
    time = time*2;
    if(time < 1)
        return 0.5f * time * time * time;
    time -= 2;
    return 0.5f * (time * time * time + 2);
}


// Quart Ease
float quartEaseIn(float time)
{
    return time * time * time * time;
}
    
float quartEaseOut(float time)
{
    time -= 1;
    return -(time * time * time * time - 1);
}
    
float quartEaseInOut(float time)
{
    time = time*2;
    if(time < 1)
        return 0.5f * time * time * time * time;
    time -= 2;
    return -0.5f * (time * time * time * time - 2);
}

// Quint Ease
float quintEaseIn(float time)
{
    return time * time * time * time * time;
}
    
float quintEaseOut(float time)
{
    time -=1;
    return (time * time * time * time * time + 1);
}
    
float quintEaseInOut(float time)
{
    time = time*2;
    if(time < 1)
        return 0.5f * time * time * time * time * time;
    time -= 2;
    return 0.5f * (time * time * time * time * time + 2);
}


// Expo Ease
float expoEaseIn(float time)
{
    return time == 0 ? 0 : powf(2, 10 * (time - 1)) - 1 * 0.001f;
}
float expoEaseOut(float time)
{
    return time == 1 ? 1 : (-powf(2, -10 * time) + 1);
}
float expoEaseInOut(float time)
{
    time /= 0.5f;
    if(time < 1){
        time = 0.5f * powf(2, 10 * (time - 1));
    }else{
        time = 0.5f * (-powf(2, -10 * (time - 1)) + 2);
    }

    return time;
}


// Circ Ease
float circEaseIn(float time)
{
    return -1 * (sqrt(1 - time * time) - 1);
}
float circEaseOut(float time)
{
    time = time - 1;
    return sqrt(1 - time * time);
}
float circEaseInOut(float time)
{
    time = time * 2;
    if(time < 1)
        return -0.5f * (sqrt(1 - time * time) - 1);
    time -= 2;
    return 0.5f * (sqrt(1 - time * time) + 1);
}


// Elastic Ease
float elasticEaseIn(float time, float period)
{
    float newT = 0;
    if(time == 0 || time == 1){
        newT = time;
    }else{
        float s = period / 4;
        time = time - 1;
        newT = -powf(2, 10 * time) * sinf((time - s) * OS_MATH_PI_2 / period);
    }
    return newT;
}
float elasticEaseOut(float time, float period)
{
    float newT = 0;
    if(time == 0 || time == 1){
        newT = time;
    }else{
        float s = period / 4;
        newT = powf(2, -10 * time) * sinf((time - s) * OS_MATH_PI_2 / period) + 1;
    }
    return newT;
}
float elasticEaseInOut(float time, float period)
{
    float newT = 0;
    if(time == 0 || time == 1){
        newT = time;
    }else{
        time = time * 2;
        if(!period){
            period = 0.3f * 1.5f;
        }

        float s = period / 4;

        time = time - 1;
        if(time < 0){
            newT = -0.5f * powf(2, 10 * time) * sinf((time -s) * OS_MATH_PI_2 / period);
        }else{
            newT = powf(2, -10 * time) * sinf((time - s) * OS_MATH_PI_2 / period) * 0.5f + 1;
        }
    }
    return newT;
}


// Back Ease
float backEaseIn(float time)
{
    float overshoot = 1.70158f;
    return time * time * ((overshoot + 1) * time - overshoot);
}
float backEaseOut(float time)
{
    float overshoot = 1.70158f;
    time = time - 1;
    return time * time * ((overshoot + 1) * time + overshoot) + 1;
}
float backEaseInOut(float time)
{
    float overshoot = 1.70158f * 1.525f;

    time = time * 2;
    if(time < 1){
        return (time * time * ((overshoot + 1) * time - overshoot)) / 2;
    }else{
        time = time - 2;
        return (time * time * ((overshoot + 1) * time + overshoot)) / 2 + 1;
    }
}



// Bounce Ease
float bounceTime(float time)
{
    if(time < 1 / 2.75){
        return 7.5625f * time * time;
    }else if(time < 2 / 2.75){
        time -= 1.5f / 2.75f;
        return 7.5625f * time * time + 0.75f;
    }else if(time < 2.5 / 2.75){
        time -= 2.25f / 2.75f;
        return 7.5625f * time * time + 0.9375f;
    }
    time -= 2.625f / 2.75f;
    return 7.5625f * time * time + 0.984375f;
}
float bounceEaseIn(float time)
{
    return 1 - bounceTime(1 - time);
}

float bounceEaseOut(float time)
{
    return bounceTime(time);
}

float bounceEaseInOut(float time)
{
    float newT = 0;
    if(time < 0.5f){
        time = time * 2;
        newT = (1 - bounceTime(1 - time)) * 0.5f;
    }else{
        newT = bounceTime(time * 2 - 1) * 0.5f + 0.5f;
    }
    return newT;
}

float easeIn(float time, float rate)
{
    return powf(time, rate);
}

float easeOut(float time, float rate)
{
    return powf(time, 1 / rate);
}
    
float easeInOut(float time, float rate)
{
    time *= 2;
    if(time < 1){
        return 0.5f * powf(time, rate);
    }else{
        return (1.0f - 0.5f * powf(2 - time, rate));
    }
}
    
float quadraticIn(float time)
{
    return powf(time,2);
}
    
float quadraticOut(float time)
{
    return -time*(time-2);
}
    
float quadraticInOut(float time)
{
    float resultTime = time;
    time = time*2;
    if(time < 1){
        resultTime = time * time * 0.5f;
    }else{
        --time;
        resultTime = -0.5f * (time * (time - 2) - 1);
    }
    return resultTime;
}
    
float bezieratFunction( float a, float b, float c, float d, float t )
{
    return (powf(1-t,3) * a + 3*t*(powf(1-t,2))*b + 3*powf(t,2)*(1-t)*c + powf(t,3)*d );
}
    
// Custom Ease
float customEase(float time, float *easingParam)
{
    if(easingParam){
        float tt = 1-time;
        return easingParam[1]*tt*tt*tt + 3*easingParam[3]*time*tt*tt + 3*easingParam[5]*time*time*tt + easingParam[7]*time*time*time;
    }
    return time;
}

float run(float time, EaseType type, float *easingParam)
{
	float period;
    switch (type){
    case Linear:
        return linear(time);
            
    case SineIn:
        return sineEaseIn(time);
    case SineOut:
        return sineEaseOut(time);
    case SineInOut:
        return sineEaseInOut(time);
	case SineOutIn:
		return easeReverseHelper(time, sineEaseInOut);
            
    case QuadIn:
        return quadEaseIn(time);
    case QuadOut:
        return quadEaseOut(time);
    case QuadInOut:
        return quadEaseInOut(time);
	case QuadOutIn:
		return easeReverseHelper(time, quadEaseInOut);
            
    case CubicIn:
        return cubicEaseIn(time);
    case CubicOut:
        return cubicEaseOut(time);
    case CubicInOut:
        return cubicEaseInOut(time);
	case CubicOutIn:
		return easeReverseHelper(time, cubicEaseInOut);
            
    case QuartIn:
        return quartEaseIn(time);
    case QuartOut:
        return quartEaseOut(time);
    case QuartInOut:
        return quartEaseInOut(time);
	case QuartOutIn:
		return easeReverseHelper(time, quartEaseInOut);
            
    case QuintIn:
        return quintEaseIn(time);
    case QuintOut:
        return quintEaseOut(time);
    case QuintInOut:
        return quintEaseInOut(time);
	case QuintOutIn:
		return easeReverseHelper(time, quintEaseInOut);
            
    case ExpoIn:
        return expoEaseIn(time);
    case ExpoOut:
        return expoEaseOut(time);
    case ExpoInOut:
        return expoEaseInOut(time);
	case ExpoOutIn:
		return easeReverseHelper(time, expoEaseInOut);
            
    case CircIn:
        return circEaseIn(time);
    case CircOut:
        return circEaseOut(time);
    case CircInOut:
        return circEaseInOut(time);
	case CircOutIn:
		return easeReverseHelper(time, circEaseInOut);
            
    case ElasticIn:
        period = easingParam ? easingParam[0] : 0.3f;
        return elasticEaseIn(time, period);
    case ElasticOut:
        period = easingParam ? easingParam[0] : 0.3f;
        return elasticEaseOut(time, period);
    case ElasticInOut:
        period = easingParam ? easingParam[0] : 0.3f;
        return elasticEaseInOut(time, period);
	case ElasticOutIn:
        period = easingParam ? easingParam[0] : 0.3f;
		return 1.0f - elasticEaseInOut(1.0f - time, period);
            
    case BackIn:
        return backEaseIn(time);
    case BackOut:
        return backEaseOut(time);
    case BackInOut:
        return backEaseInOut(time);
	case BackOutIn:
		return easeReverseHelper(time, backEaseInOut);
            
    case BounceIn:
        return bounceEaseIn(time);
    case BounceOut:
        return bounceEaseOut(time);
    case BounceInOut:
        return bounceEaseInOut(time);
	case BounceOutIn:
		return easeReverseHelper(time, bounceEaseInOut);

    /* case CUSTOM_EASING:
        return customEase(time, easingParam); */
    }

    // return sineEaseInOut(time);
	return linear(time);
}

EaseType getReverseType(EaseType type)
{
    switch (type){
    case Linear:
        return Linear;
            
    case SineIn:
        return SineOut;
    case SineOut:
        return SineIn;
    case SineInOut:
        return SineOutIn;
	case SineOutIn:
		return SineInOut;
            
    case QuadIn:
        return QuadOut;
    case QuadOut:
        return QuadIn;
    case QuadInOut:
        return QuadOutIn;
	case QuadOutIn:
		return QuadInOut;
            
    case CubicIn:
        return CubicOut;
    case CubicOut:
        return CubicIn;
    case CubicInOut:
        return CubicOutIn;
	case CubicOutIn:
		return CubicInOut;
            
    case QuartIn:
        return QuartOut;
    case QuartOut:
        return QuartIn;
    case QuartInOut:
        return QuartOutIn;
	case QuartOutIn:
		return QuartInOut;
            
    case QuintIn:
        return QuintOut;
    case QuintOut:
        return QuintIn;
    case QuintInOut:
        return QuintOutIn;
	case QuintOutIn:
		return QuintInOut;
            
    case ExpoIn:
        return ExpoOut;
    case ExpoOut:
        return ExpoIn;
    case ExpoInOut:
        return ExpoOutIn;
	case ExpoOutIn:
		return ExpoInOut;
            
    case CircIn:
        return CircOut;
    case CircOut:
        return CircIn;
    case CircInOut:
        return CircOutIn;
	case CircOutIn:
		return CircInOut;
            
    case ElasticIn:
        return ElasticOut;
    case ElasticOut:
        return ElasticIn;
    case ElasticInOut:
        return ElasticOutIn;
	case ElasticOutIn:
		return ElasticInOut;
            
    case BackIn:
        return BackOut;
    case BackOut:
        return BackIn;
    case BackInOut:
        return BackOutIn;
	case BackOutIn:
		return BackInOut;
            
    case BounceIn:
        return BounceOut;
    case BounceOut:
        return BounceIn;
    case BounceInOut:
        return BounceOutIn;
	case BounceOutIn:
		return BounceInOut;

    /* case CUSTOM_EASING:
        return customEase(time, easingParam); */
    }
	return Linear;
}

}
