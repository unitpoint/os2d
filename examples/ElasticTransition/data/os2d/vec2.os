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

vec2 = extends Object {
	__construct = function(x, y){
		@x = x || 0 // numberOf(x) || throw "x number required"
		@y = y || @x // numberOf(y || x) || throw "y number required"
	},
	
	clone = function(){
		return vec2(@x, @y)
	},
	
	round = function(p){
		return vec2(math.round(@x, p), math.round(@y, p))
	},
	
	ceil = function(){
		return vec2(math.ceil(@x), math.ceil(@y))
	},
	
	floor = function(){
		return vec2(math.floor(@x), math.floor(@y))
	},
	
	normalize = function(){
		var len = #this
		if(len > 0){
			@x /= len
			@y /= len
		}
		return this
	},
	
	normalizeTo = function(newLen){
		var len = #this
		if(len > 0){
			var scale = newLen / len
			@x *= scale
			@y *= scale
		}
		return this
	},
	
	normalizeToMax = function(maxLen){
		var len = #this
		if(len > maxLen){
			var scale = maxLen / len
			@x *= scale
			@y *= scale
		}
		return this
	},
	
	dot = function(b){
		b is vec2 || throw "vec2 required"
		return @x*b.x + @y*b.y
	},
	
	__get@rotation = function(){
		return math.atan2(@y, @x)
	},
	__get@angle = function(){
		return math.deg(@rotation)
	},
	
	fromRotation = function(rot){
		return vec2(math.cos(rot), math.sin(rot))
	},
	fromAngle = function(angle){
		return @fromRotation(math.rad(angle))
	},
	
	__len = function(){
		return math.sqrt(@x*@x + @y*@y)
	},
	
	__cmp = function(b){
		b is vec2 || b = vec2(b) // throw "vec2 required"
		// print "vec2 cmp: ${@x} ${@y}, other: ${b.x} ${b.y}, cmp: ${@x <=> b.x} ${@y <=> b.y}"
		var x, y = @x <=> b.x, @y <=> b.y
		x < 0 && y < 0 && return -1
		x > 0 && y > 0 && return 1
		x == 0 && y == 0 && return 0
		// else is not comparable
	},
	
	__minus = function(){
		return vec2(-@x, -@y)
	},

	__add = function(b){
		b is vec2 && return vec2(@x + b.x, @y + b.y)
		b = numberOf(b) || throw "number or vec2 required"
		return vec2(@x + b, @y + b)
	},
	
	__radd = function(b){
		b = numberOf(b) || throw "number or vec2 required"
		return vec2(b + @x, b + @y)
	},
	
	__sub = function(b){
		b is vec2 && return vec2(@x - b.x, @y - b.y)
		b = numberOf(b) || throw "number or vec2 required"
		return vec2(@x - b, @y - b)
	},
	
	__rsub = function(a){
		a = numberOf(a) || throw "number required"
		return vec2(a - @x, a - @y)
	},
	
	__mul = function(b){
		b is vec2 && return vec2(@x * b.x, @y * b.y)
		b = numberOf(b) || throw "number or vec2 required"
		return vec2(@x * b, @y * b)
	},
	
	__rmul = function(b){
		b = numberOf(b) || throw "number or vec2 required"
		return vec2(b * @x, b * @y)
	},
	
	__div = function(b){
		b is vec2 && return vec2(@x / b.x, @y / b.y)
		b = numberOf(b) || throw "number or vec2 required"
		return vec2(@x / b, @y / b)
	},
	
	__rdiv = function(b){
		b = numberOf(b) || throw "number or vec2 required"
		return vec2(b / @x, b / @y)
	},
	
	__get = function(i){
		if(typeOf(i) === "number"){
			i |= 0 // fast way to convert to int
			return i == 0 ? @x : i == 1 ? @y : throw "error index ${i}, 0 or 1 required"
		}
		return super(i)
	},
	
	__set = function(i, value){
		if(typeOf(i) === "number"){
			i |= 0 // fast way to convert to int
			i == 0 ? @x = value : i == 1 ? @y = value : throw "error index ${i}, 0 or 1 required"
			return
		}
		super(i, value)
	},
}