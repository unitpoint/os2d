DoneTween = extends BaseDoneTween {
	__construct = function(duration, func){
		super()
		if(duration){
			@duration = duration
			@doneCallback = functionOf(func) || throw "2nd argument must be function"
		}
	},
}
