/** 
@brief An interval action is an action that takes place within a certain period of time.
It has an start time, and a finish time. The finish time is the parameter
duration plus the start time.

These IntervalAction actions have some interesting properties, like:
- They can run normally (default)
- They can run reversed with the reverse method
- They can run with the time altered with the Accelerate, AccelDeccel and Speed actions.

For example, you can simulate a Ping Pong effect running the action normally and
then running it again in Reverse mode.

Example:

Action *pingPongAction = SequenceAction(action, action.reverse());
*/
IntervalAction = extends FiniteTimeAction {
	__object = {
		_elapsed = 0,
		_firstTick = true,
	},
	
	/** how many seconds had elapsed since the actions started to run. */
	__get@elapsed = function(){ return @_elapsed },

	__set@duration = function(duration){
		@_duration = duration == 0 ? math.NUM_EPSILON : duration
	},

	__get@isDone = function(){ return @_elapsed > @_duration },
	
	start = function(){
		super()
		@_elapsed = 0
		@_firstTick = true
	},
	
	step = function(dt){
		if(@_firstTick){
			@_firstTick = false
			@_elapsed = 0
		}else{
			@_elapsed += dt
		}
		
		var t = @_elapsed / @_duration
		if(t < 0) t = 0
		else if(t > 1) t = 1
		
		@update(t)
	},
}
