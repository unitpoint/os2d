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
 @brief Base class for Easing actions
 @ingroup Actions
 */
EaseAction = extends IntervalAction {
	__object = {
		_innerAction = null,
		_easeType = Ease.LINEAR,
	},

	__construct = function(action, easeType){
		if(action.prototype === Object){
			super(action)
		}else{
			super()
			@_innerAction = action
			@_easeType = easeType
			@duration = @_innerAction.duration
		}
	},

	__get@innerAction = function(){ return @_innerAction },
    __set@innerAction = function(action){
		if(action !== @_innerAction){
			@_innerAction = action as IntervalAction || throw "IntervalAction required"
			@_innerAction.target = target
		}
	},
	
    __get@easeType = function(){ return @_easeType },
    __set@easeType = function(easeType){ @_easeType = easeType },
	
	__set@target = function(target){
		if(@_target !== target){
			super(target)
			@duration = @_innerAction.duration
			@_innerAction.target = target
		}
	},

    /*
	__get@elapsed = function(){ return @_innerAction.elapsed },
	
	__get@duration = function(){ return @_innerAction.duration },
    __set@duration = function(duration){ @_innerAction.duration = duration },
	*/
	
    //
    // Overrides
    //
	clone = function(){
		return EaseAction {
			innerAction = @_innerAction.clone(),
			easeType = @_easeType,
		}
	},
	
    reverse = function(){
		return EaseAction {
			innerAction = @_innerAction.clone(),
			easeType = Ease.getReverseType(@_easeType),
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
	
    update = function(t){
		@_innerAction.update(Ease.run(t, @_easeType))
	},
}
