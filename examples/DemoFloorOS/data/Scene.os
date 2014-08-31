Scene = extends Actor {
	__construct = function(){
		@size = stage.size
		print "${@classname}.size: ${@size}"
		
		@alpha = 1
		@transition = null
	},
	
	changeScene = function(next){
		@hide()
		next.show()
	},
	
	show = function(){
		@transition = "show"
		@parent = stage
		@removeTweensByName("sceneTransition")
		@alpha != 1 && @addTween("alpha", 1, 1000).attrs {
			name = "sceneTransition",
			doneCallback = function(){
				@transition = null
			}.bind(this),
		}
	},
	
	hide = function(){
		@transition = "hide"
		@removeTweensByName("sceneTransition")
		@addTween("alpha", 0, 1000).attrs {
			name = "sceneTransition",
			detachActor = true,
			doneCallback = function(){
				@transition = null
			}.bind(this),
		}
	},
}
