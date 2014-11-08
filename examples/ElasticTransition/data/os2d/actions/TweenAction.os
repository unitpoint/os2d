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

TweenAction = extends IntervalAction {
	
	__construct = function(duration, prop, from, to){
		super()
		if(duration.prototype === Object){
			prop = duration
			@duration = prop.duration || throw "duration required"; delete prop.duration
			@name = prop.name; delete prop.name
			@detachTarget = prop.detachTarget; delete prop.detachTarget
			@doneCallback = prop.doneCallback; delete prop.doneCallback
			@tickCallback = prop.tickCallback; delete prop.tickCallback
		}else{
			@duration = duration
			@tickCallback = null
		}
		if(prop.prototype === Object){
			@_props = {}
			var defEase = prop.ease; delete prop.ease
			for(var prop, values in prop){
				if(values.prototype === Object){
					@_props[prop] = {
						from = values.from,
						to = values.to,
						fixRotation = values.fixRotation,
						ease = values.ease || defEase,
					}
				}else{
					@_props[prop] = {
						from = null,
						to = values,
						ease = defEase,
					}
				}
			}
		}else{
			if(!to){
				from, to = null, from
			}
			@_props = {
				[prop] = {
					from = from,
					to = to,
				}
			}
		}
		// print "TweenAction created: ${this}"
	},

    //
    // Overrides
    //
    clone = function(){
		var props = {
			duration = @duration,
			doneCallback = @doneCallback,
			tickCallback = @tickCallback,
		}
		for(var prop, values in @_props){
			props[prop] = {
				from = values.from, to = values.to, 
				fixRotation = values.fixRotation,
				ease = values.ease,
			}
		}
		return TweenAction(props)
	},
	
	reverse = function(){
		var props = {
			duration = @duration,
			doneCallback = @doneCallback,
			tickCallback = @tickCallback,
		}
		for(var prop, values in @_props){
			props[prop] = {
				from = values.to, to = values.from, 
				fixRotation = values.fixRotation,
				ease = values.ease,
			}
		}
		return TweenAction(props)
	},
	
    start = function(){
		super()
		for(var prop, values in @_props){
			/* if(!values.to && !values.from){
				print "null values: ${values}, this: ${this}"
			} */
			values.delta = (values.to || /*values.to =*/ @_target[prop] || throw "${prop} is not set in ${@_target}") 
				- (values.from || /*values.from =*/ @_target[prop] || throw "${prop} is not set in ${@_target}")
			if(prop == "angle" && values.fixRotation){
				var diff = values.delta
				if(math.abs(diff) > 180){
					if(diff < 0){
						diff = 360 + diff - math.floor(diff / 360) * 360
					}else if(diff > 360){
						diff = diff - math.floor(diff / 360) * 360
					}
					if(diff > 180){
						diff = diff - 360
					}
					values.delta = diff
				}
			}
		}
	},
	
    update = function(t){
		for(var prop, values in @_props){
			var time = values.ease ? Ease.run(t, values.ease) : t
			// try{
			@_target[prop] = (values.to || @_target[prop] || throw "${prop} is not set in ${@_target}") 
				- values.delta * (1 - time)
			/* }catch(e){
				print "exception of values: ${values}, this: ${this}"
				throw e
			} */
		}
		var tickCallback = @tickCallback
		tickCallback() // use function's this instead of current object
	},
}
