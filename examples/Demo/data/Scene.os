Scene = extends EventDispatcher {
	__construct = function(){
		@view = Actor()
		@view.size = stage.size
		print "${@classname}.view.size: ${@view.size}"
		
		@addEventListener("hidden", function(ev){
			print "catch event: hidden, ${ev}"
			// @view.detach()
		}.bind(this))
		
		@view.opacity = 0
		@transition = null
	},
	
	changeScene = function(next){
		@hide()
		next.show()
	},
	
	show = function(){
		@transition = "show"
		@view.parent = stage
		@view.replaceTweenAction {
			name = "transition",
			duration = 1,
			opacity = 1,
			doneCallback = function(){
				@transition = null
				// @dispatchEvent{"hidden", value = "text", xyz = 123}
			}.bind(this),
		}
	},
	
	hide = function(){
		@transition = "hide"
		@view.replaceTweenAction {
			name = "transition",
			duration = 1,
			opacity = 0,
			detachTarget = true,
			doneCallback = function(){
				@transition = null
				@dispatchEvent{"hidden", value = "text", xyz = 123}
			}.bind(this),
		}
	},
}
