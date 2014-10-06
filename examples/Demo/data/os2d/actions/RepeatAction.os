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

/** @brief Repeats an action a number of times.
 * To repeat an action forever use the RepeatForever action.
 */
RepeatAction = extends IntervalAction {
	__object = {
		_innerAction = null,
		_times = 1,
		_actionInstant = false,
		_total = 0,
		_nextDt = 0,
	},	

    __get@innerAction = function(){ return @_innerAction },
    __set@innerAction = function(action){
		if(action !== @_innerAction){
			@_innerAction = action as FiniteTimeAction || throw "FiniteTimeAction required"
			@_innerAction.target = target
			@_actionInstant = action is InstantAction
		}
	},
	
	__set@target = function(target){
		if(@_target !== target){
			super(target)
			@_innerAction.target = target
		}
	},
	
	__get@times = function(){ return @_times }
	__set@times = function(times){ @_times = times },

    //
    // Overrides
    //
	clone = function(){
		return RepeatAction {
			innerAction = @innerAction.clone(),
			times = @_times,
		}
	},
	
	reverse = function(){
		return RepeatAction {
			innerAction = @innerAction.reverse(),
			times = @_times,
		}
	},
    
	start = function(){
		super()
		@_total = 0
		@_nextDt = @_innerAction.duration / @_duration
		@_actionInstant && @_times -= 1
		@_innerAction.start()
	},
    
	stop = function(){
		@_innerAction.stop()
		super()
	},
	
    update = function(dt){
		if(dt >= @_nextDt){
			while(dt > @_nextDt && @_total < @_times){

				@_innerAction.update(1)
				@_total++

				@_innerAction.stop()
				@_innerAction.start()
				@_nextDt = @_innerAction.duration / @_duration * (@_total + 1)
			}

			// fix for issue #1288, incorrect end value of repeat
			if(dt >= 1 && @_total < @_times){
				@_total++
			}

			// don't set an instant action back or update it, it has no use because it has no duration
			if(!@_actionInstant){
				if(@_total == @_times){
					@_innerAction.update(1)
					@_innerAction.stop()
				}else{
					// issue #390 prevent jerk, use right update
					@_innerAction.update(dt - (@_nextDt - @_innerAction.duration / @_duration))
				}
			}
		}else{
			@_innerAction.update(math.fmod(dt * @_times, 1))
		}
	},
	
    __get@isDone = function(){
		return @_total == @_times
	},
