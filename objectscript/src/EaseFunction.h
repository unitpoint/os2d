/******************************************************************************
* Copyright (C) 2014 Evgeniy Golovin (evgeniy.golovin@unitpoint.ru)
*
* Please feel free to contact me at anytime, 
* my email is evgeniy.golovin@unitpoint.ru, skype: egolovin
*
* Latest source code: https://github.com/unitpoint/os2d
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

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
