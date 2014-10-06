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

/** @brief Repeats an action for ever.
To repeat the an action for a limited number of times use the Repeat action.
@warning This action can't be Sequenceable because it is not an IntervalAction
*/
RepeatForeverAction = extends Action {
	__object = {
		_innerAction = null,
	},
	
	__construct = function(action){
		if(action is FiniteTimeAction){
			super()
			@innerAction = action
		}else{
			super(action)
		}
	},

    __get@innerAction = function(){ return @_innerAction },
    __set@innerAction = function(action){
		if(action !== @_innerAction){
			@_innerAction = action as FiniteTimeAction || throw "FiniteTimeAction required"
			// @_actionInstant = action is InstantAction
		}
	},
	
	__set@target = function(target){
		if(@_target !== target){
			super(target)
			@_innerAction.target = target
		}
	},

    //
    // Overrides
    //
	clone = function(){
		return RepeatForeverAction(@_innerAction.clone())
	},
	
	reverse = function(){
		return RepeatForeverAction(@_innerAction.reverse())
	},
    
	start = function(){
		super()
		@_innerAction.start()
	},
    
    step = function(dt){
		@_innerAction.step(dt)
		if(@_innerAction.isDone){
			var diff = @_innerAction.elapsed - @_innerAction.duration
			if(diff > @_innerAction.duration){
				diff = math.fmod(diff, @_innerAction.duration)
			}
			// print "@_innerAction: ${@_innerAction}"
			@_innerAction.start()
			// to prevent jerk. issue #390, 1247
			@_innerAction.step(0)
			@_innerAction.step(diff)
		}
	},
	
	/* update = function(t){
		
	}, */
	
    __get@isDone = function(){ return false },
}
