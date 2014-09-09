#ifndef __EASE_FUNCTION_H__
#define __EASE_FUNCTION_H__

#include <math.h>
#include <stddef.h>

namespace EaseFunction
{
    enum EaseType
    {
        Linear,
		PingPong,
        
        SineIn,
        SineOut,
        SineInOut,
        SineOutIn,
        
        QuadIn,
        QuadOut,
        QuadInOut,
        QuadOutIn,
        
        CubicIn,
        CubicOut,
        CubicInOut,
        CubicOutIn,
        
        QuartIn,
        QuartOut,
        QuartInOut,
        QuartOutIn,
        
        QuintIn,
        QuintOut,
        QuintInOut,
        QuintOutIn,
        
        ExpoIn,
        ExpoOut,
        ExpoInOut,
        ExpoOutIn,
        
        CircIn,
        CircOut,
        CircInOut,
        CircOutIn,
        
        ElasticIn,
        ElasticOut,
        ElasticInOut,
        ElasticOutIn,
        
        BackIn,
        BackOut,
        BackInOut,
        BackOutIn,
        
        BounceIn,
        BounceOut,
        BounceInOut,
        BounceOutIn,
    };
    
    float run(float time, EaseType type, float *easingParam = NULL);
	EaseType getReverseType(EaseType type);
}

#endif // __EASE_FUNCTION_H__
