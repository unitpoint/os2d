UpdateTween = extends BaseUpdateTween {
	__construct = function(interval, func){
		super()
		if(functionOf(interval)){
			@updateCallback = interval
		}else if(func){
			@interval = interval
			@updateCallback = functionOf(func) || throw "2nd argument must be function"
		}
	},
}
