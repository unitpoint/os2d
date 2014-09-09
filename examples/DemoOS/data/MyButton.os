MyButton = extends Sprite {
	__construct = function(){
		super()
		@addEventListener(TouchEvent.TOUCH_DOWN, @onTouchDown.bind(this))
		@addEventListener(TouchEvent.TOUCH_UP, @onTouchUp.bind(this))
		@addEventListener(TouchEvent.CLICK, @onClick.bind(this))
	},
	
	onTouchDown = function(ev){
		// print "onTouchDown"
		// @color = Color(1, 0, 0)
		@replaceTweenAction {
			name = "buttonTweenAction",
			duration = 0.15,
			color = Color(1, 0, 0),
		}
	},
	
	onTouchUp = function(ev){
		@replaceTweenAction {
			name = "buttonTweenAction",
			duration = 0.15,
			color = Color(1, 1, 1),
		}
	},
	
	onClick = function(ev){
		@scale = 1
		@color = Color(1, 1, 1)
		@replaceTweenAction {
			name = "buttonTweenAction",
			duration = 0.3,
			scale = 1.1,
			ease = Ease.PING_PONG,
		}
	},
}
