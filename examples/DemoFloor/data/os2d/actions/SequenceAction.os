/** @brief Runs actions sequentially, one after another
 */
SequenceAction = extends IntervalAction {
	__object = {
		// _actions = [],
		_actionOne = null,
		_actionTwo = null,
		_split = 0,
		_lastAction = null,
	},
	
	__construct = function(actionOne, actionTwo, actionThree){
		super()
		actionOne is FiniteTimeAction || throw "1st argument should be FiniteTimeAction"
		if(actionThree){
			var count = #arguments - 1
			for(var i = 1; i < count; i++){
				actionTwo = arguments[i] as FiniteTimeAction || throw "argument[${i}] should be FiniteTimeAction"
				actionOne = SequenceAction(actionOne, actionTwo)
			}
			actionTwo = arguments.last as FiniteTimeAction || throw "last argument should be FiniteTimeAction"
		}
		if(!actionTwo){
			actionTwo = ExtraAction()
		}else{
			actionTwo is FiniteTimeAction || throw "2nd argument should be FiniteTimeAction"
		}
		@duration = actionOne.duration + actionTwo.duration
		@_actionOne = actionOne
		@_actionTwo = actionTwo
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
		return SequenceAction(@_actionOne.clone(), @_actionTwo.clone())
	},
	
	reverse = function(){
		return SequenceAction(@_actionTwo.reverse(), @_actionOne.reverse())
	},
	
    start = function(){
		super()
		@_split = @_actionOne.duration / @duration
		@_lastAction = null
	},
	
    stop = function(){
		@_lastAction.stop()
		super()
	},
	
    update = function(t){
		var found = null
		var new_t = 0

		if(t < @_split){
			// action[0]
			found = @_actionOne
			if(@_split != 0)
				new_t = t / @_split
			else
				new_t = 1

		}else{
			// action[1]
			found = @_actionTwo
			if(@_split == 1)
				new_t = 1
			else
				new_t = (t - @_split) / (1 - @_split)
		}
		if(found === @_actionTwo){
			if(!@_lastAction){
				// action[0] was skipped, execute it.
				@_actionOne.start()
				@_actionOne.update(1)
				@_actionOne.stop()
			}else if(@_lastAction === @_actionOne){
				// switching to action 1. stop action 0.
				@_actionOne.update(1)
				@_actionOne.stop()
			}
		}else if(found === @_actionOne && @_lastAction === @_actionTwo){
			// Reverse mode ?
			// XXX: Bug. this case doesn't contemplate when _last==-1, found=0 and in "reverse mode"
			// since it will require a hack to know if an action is on reverse mode or not.
			// "step" should be overriden, and the "reverseMode" value propagated to inner Sequences.
			@_actionTwo.update(0)
			@_actionTwo.stop()
		}
		// Last action found and it is done.
		if(found === @_lastAction && found.isDone){
			return
		}

		// Last action found and it is done
		if(found !== @_lastAction){
			found.start()
		}

		found.update(new_t)
		@_lastAction = found
	},
}
