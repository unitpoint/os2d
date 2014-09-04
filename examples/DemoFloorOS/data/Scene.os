Scene = extends Actor {
	__construct = function(){
		@size = stage.size
		@sceneTransition = null
	},
	
	changeScene = function(next){
		@hide()
		next.show()
	},
	
	show = function(parent){
		@sceneTransition = "show"
		@parent = parent || @parent || stage
		@removeTweensByName("sceneTransition")
		@opacity != 1 && @addTween("opacity", 1, 500).attrs {
			name = "sceneTransition",
			doneCallback = function(){
				@sceneTransition = null
			}.bind(this),
		}
	},
	
	hide = function(){
		@sceneTransition = "hide"
		@removeTweensByName("sceneTransition")
		@addTween("opacity", 0, 500).attrs {
			name = "sceneTransition",
			detachActor = true,
			doneCallback = function(){
				@sceneTransition = null
			}.bind(this),
		}
	},
}
