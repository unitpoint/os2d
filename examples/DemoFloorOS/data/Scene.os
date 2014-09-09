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
		@opacity != 1 && @replaceTweenAction {
			name = "sceneTransition",
			duration = 0.5,
			opacity = 1,
			doneCallback = function(){
				@sceneTransition = null
			}.bind(this),
		}
	},
	
	hide = function(){
		@sceneTransition = "hide"
		@replaceTweenAction {
			name = "sceneTransition",
			duration = 0.5,
			opacity = 0,
			detachTarget = true,
			doneCallback = function(){
				@sceneTransition = null
			}.bind(this),
		}
	},
}
