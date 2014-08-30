vec2 = extends Object {
	__construct = function(x, y){
		@x = x || 0 // numberOf(x) || throw "x number required"
		@y = y || @x // numberOf(y || x) || throw "y number required"
	},
	
	clone = function(){
		return vec2(@x, @y)
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
	
	__len = function(){
		return math.sqrt(@x*@x + @y*@y)
	},
	
	/* __cmp = function(b){
		b is vec2 && return @x <=> b.x || 
	}, */
	
	__add = function(b){
		b is vec2 && return vec2(@x + b.x, @y + b.y)
		b = numberOf(b) || throw "number or vec2 required"
		return vec2(@x + b, @y + b)
	},
	
	__sub = function(b){
		b is vec2 && return vec2(@x - b.x, @y - b.y)
		b = numberOf(b) || throw "number or vec2 required"
		return vec2(@x - b, @y - b)
	},
	
	__mul = function(b){
		b is vec2 && return vec2(@x * b.x, @y * b.y)
		b = numberOf(b) || throw "number or vec2 required"
		return vec2(@x * b, @y * b)
	},
	
	__div = function(b){
		b is vec2 && return vec2(@x / b.x, @y / b.y)
		b = numberOf(b) || throw "number or vec2 required"
		return vec2(@x / b, @y / b)
	},
}