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
@brief Base class for Action objects.
 */
Action = extends Object {
	__object = {
		_target = null,
		_name = "anonymous",
		detachTarget = false,
		doneCallback = null,
	},
	
	__construct = function(params){
		super()
		if(params.prototype === Object){
			@attrs params
		}else if(params){
			throw "error argument ${params}"
		}
	},
	
    __get@target = function(){ return @_target },
    /** The action will modify the target properties. */
    __set@target = function(target){ @_target = target },
    
    __get@name = function(){ return @_name },
    __set@name = function(name){ @_name = name },
    
	/** return true if the action has finished */
	__get@isDone = function(){ throw "${@classname}.${_F.__name} is not implemented" },
	
	/** returns a clone of action */
	clone = function(){ throw "${@classname}.${_F.__name} is not implemented" },
	
	/** returns a new action that performs the exactly the reverse action */
	reverse = function(){ throw "${@classname}.${_F.__name} is not implemented" },
	
	/** called before the action start. It will also set the target. */
	start = function(){
	},
    
	/** 
    called after the action has finished. It will set the 'target' to nil.
    IMPORTANT: You should never call "[action stop]" manually. Instead, use: "target->stopAction(action);"
    */
	stop = function(){
		// @target = null
	},
	
	/** called every frame with it's delta time. DON'T override unless you know what you are doing. */
	step = function(dt){ throw "${@classname}.${_F.__name} is not implemented" },
	
    /** 
    called once per frame. time a value between 0 and 1

    For example: 
    - 0 means that the action just started
    - 0.5 means that the action is in the middle
    - 1 means that the action is over
    */
    update = function(t){ throw "${@classname}.${_F.__name} is not implemented" },
}
