DoneTween = extends BaseDoneTween {
	__construct = function(func){
		super()
		if(func){
			@doneCallback = functionOf(func) || throw "function required"
		}
	},
}
