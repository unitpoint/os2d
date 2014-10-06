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

/** @brief Spawn a new action immediately
 */
SpawnAction = extends IntervalAction {
	__object = {
		// _actions = [],
		_actionOne = null,
		_actionTwo = null,
	},
	
	__construct = function(actionOne, actionTwo, actionThree){
		super()
		actionOne is FiniteTimeAction || throw "1st argument should be FiniteTimeAction"
		if(actionThree){
			var count = #arguments - 1
			for(var i = 1; i < count; i++){
				actionTwo = arguments[i] as FiniteTimeAction || throw "argument[${i}] should be FiniteTimeAction"
				actionOne = SpawnAction(actionOne, actionTwo)
			}
			actionTwo = arguments.last as FiniteTimeAction || throw "last argument should be FiniteTimeAction"
		}
		if(!actionTwo){
			actionTwo = ExtraAction()
		}else{
			actionTwo is FiniteTimeAction || throw "2nd argument should be FiniteTimeAction"
		}
		var d1, d2 = actionOne.duration, actionTwo.duration
		@duration = d1 > d2 ? d1 : d2
		@_actionOne = actionOne
		@_actionTwo = actionTwo
		
        if(d1 > d2){
            @_actionTwo = SequenceAction(actionTwo, DelayTimeAction(d1 - d2))
        }else if(d1 < d2){
            @_actionOne = SequenceAction(actionOne, DelayTimeAction(d2 - d1))
        }
	},
	
	__get@actionOne = function(){ return @_actionOne },
	__get@actionTwo = function(){ return @_actionTwo },
	
	__set@target = function(target){
		if(@_target !== target){
			super(target)
			@_actionOne.target = target
			@_actionTwo.target = target
		}
	},

    //
    // Overrides
    //
    clone = function(){
		return SpawnAction(@_actionOne.clone(), @_actionTwo.clone())
	},
	
	reverse = function(){
		return SpawnAction(@_actionOne.reverse(), @_actionTwo.reverse())
	},
	
    start = function(){
		super()
		@_actionOne.start()
		@_actionTwo.start()
	},
	
    stop = function(){
		@_actionTwo.stop()
		@_actionOne.stop()
		super()
	},
	
    update = function(t){
		@_actionOne.update(t)
		@_actionTwo.update(t)
	},
