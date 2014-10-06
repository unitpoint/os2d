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

/** 
 @brief Changes the speed of an action, making it take longer (speed>1)
 or less (speed<1) time.
 Useful to simulate 'slow motion' or 'fast forward' effect.
 @warning This action can't be Sequenceable because it is not an IntervalAction
 */
SpeedAction = extends Action {
	__object = {
		_innerAction = null,
		_speed = 1,
	},
    
    __get@speed = function(){ return @_speed },
    /** alter the speed of the inner function in runtime */
    __set@speed = function(speed){ @_speed = speed },

	__get@innerAction = function(){ return @_innerAction },
    __set@innerAction = function(action){
		if(action !== @_innerAction){
			@_innerAction = action as IntervalAction || throw "IntervalAction required"
			@_innerAction.target = target
		}
	},
	
	__set@target = function(target){
		if(@_target !== target){
			super(target)
			@_innerAction.target = target
		}
	},

    //
    // Override
    //
	clone = function(){
		return SpeedAction {
			innerAction = @_innerAction.clone(),
			speed = @_speed,
		}
	},
	
    reverse = function(){
		return SpeedAction {
			innerAction = @_innerAction.reverse(),
			speed = @speed,
		}
	},
	
	__set@target = function(target){
		super(target)
		@_innerAction.target = target
	},
	
    start = function(){
		super()
		@_innerAction.start()
	},
	
    stop = function(){
		@_innerAction.stop()
		super()
	},
	
    step = function(dt){
		@_innerAction.step(dt * @speed)
	},
	
    __get@isDone function(){
		return @_innerAction.isDone
	},
}