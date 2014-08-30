Scene = extends EventDispatcher {
	__construct = function(){
		@view = Actor()
		@view.size = root.size
		print "${@classname}.view.size: ${@view.size}"
		
		@addEventListener("hidden", function(ev){
			print "catch event: hidden, ${ev}"
			// @view.detach()
		}.bind(this))
		
		@view.alpha = 0
		@transition = null
	},
	
	changeScene = function(next){
		@hide()
		next.show()
	},
	
	show = function(){
		@transition = "show"
		@view.parent = root
		@view.removeTweensByName("transition")
		@view.addTween("alpha", 1, 1000).attrs {
			name = "transition",
			doneCallback = function(){
				@transition = null
				// @dispatchEvent{"hidden", value = "text", xyz = 123}
			}.bind(this),
		}
	},
	
	hide = function(){
		@transition = "hide"
		@view.removeTweensByName("transition")
		@view.addTween("alpha", 0, 1000).attrs {
			name = "transition",
			detachActor = true,
			doneCallback = function(){
				@transition = null
				@dispatchEvent{"hidden", value = "text", xyz = 123}
			}.bind(this),
		}
	},
}
