MyButton = extends Sprite {
	__construct = function(){
		super()
		@addEventListener(TouchEvent.TOUCH_DOWN, @onTouchDown.bind(this))
		@addEventListener(TouchEvent.TOUCH_UP, @onTouchUp.bind(this))
		@addEventListener(TouchEvent.CLICK, @onClick.bind(this))
	},
	
	onTouchDown = function(ev){
		print "onTouchDown"
		@color = Color(1, 0, 0)
	},
	
	onTouchUp = function(ev){
		print "onTouchUp"
		@color = Color(1, 1, 1)
	},
	
	onClick = function(ev){
		print "onClick"
		@scale = 1
		@addTween("scale", 0.9, 300, 1, true)
	},
}
