TimeoutAction = extends IntervalAction {
	
	__construct = function(duration, func){
		super()
		if(duration){
			@duration = duration
			@doneCallback = func
		}
	},

    //
    // Overrides
    //
    clone = function(){
		return TimeoutAction(@_duration, @doneCallback)
	},
	
	reverse = function(){
		return TimeoutAction(@_duration, @doneCallback)
	},
	
    update = function(t){
	},
}
