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
