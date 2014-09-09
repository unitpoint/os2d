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
