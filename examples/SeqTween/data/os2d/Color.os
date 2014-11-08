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

local function clamp(a){
	return a < 0 ? 0 : a > 1 ? 1 : a
}

Color = extends Object {
	__construct = function(r, g, b, a){
		@r = r || 0
		@g = g || 0
		@b = b || 0
		@a = a || 1
	},
	
	fromInt = function(value, a){
		return Color(
			((value >> 16) & 0xff) / 0xff,
			((value >> 8) & 0xff) / 0xff,
			((value >> 0) & 0xff) / 0xff,
			a
		)
	},
	
	clamp = function(){
		return Color(clamp(@r), clamp(@g), clamp(@b), clamp(@a))
	},
	
	__cmp = function(b){
		b is Color || throw "Color required"
		var r, g, b, a = @r <=> b.r, @g <=> b.g, @b <=> b.b, @a <=> b.a
		r < 0 && g < 0 && b < 0 && a < 0 && return -1
		r > 0 && g > 0 && b > 0 && a > 0 && return 1
		r == 0 && g == 0 && b == 0 && a == 0 && return 0
		// else is not comparable
	},
	
	__add = function(b){
		b is Color && return Color(@r + b.r, @g + b.g, @b + b.b, @a + b.a)
		b = numberOf(b) || throw "number or Color required"
		return Color(@r + b, @g + b, @b + b, @a + b)
	},
	
	__sub = function(b){
		// print "Color ${this} sub ${b}"
		b is Color && return Color(@r - b.r, @g - b.g, @b - b.b, @a - b.a)
		b = numberOf(b) || throw "number or Color required"
		return Color(@r - b, @g - b, @b - b, @a - b)
	},
	
	__mul = function(b){
		// print "Color ${this} mul ${b}"
		b is Color && return Color(@r * b.r, @g * b.g, @b * b.b, @a * b.a)
		b = numberOf(b) || throw "number or Color required"
		return Color(@r * b, @g * b, @b * b, @a * b)
	},
	
	__div = function(b){
		b is Color && return Color(@r / b.r, @g / b.g, @b / b.b, @a / b.a)
		b = numberOf(b) || throw "number or Color required"
		return Color(@r / b, @g / b, @b / b, @a / b)
	},
}

Color.RED = Color(1, 0, 0)
Color.GREEN = Color(0, 1, 0)
Color.BLUE = Color(0, 0, 1)
Color.WHITE = Color(1, 1, 1)
Color.BLACK = Color(0, 0, 0)

