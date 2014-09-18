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