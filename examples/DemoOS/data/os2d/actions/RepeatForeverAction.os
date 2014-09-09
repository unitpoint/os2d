/** @brief Repeats an action for ever.
To repeat the an action for a limited number of times use the Repeat action.
@warning This action can't be Sequenceable because it is not an IntervalAction
*/
RepeatForeverAction = extends Action {

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
