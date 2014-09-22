Joystick = extends Sprite {
	__construct = function(){
		super()
		
		@resAnim = res.get("joystick")
		@opacity = 0.5
		
		@addEventListener(TouchEvent.TOUCH_DOWN, @onEvent.bind(this))
		@addEventListener(TouchEvent.TOUCH_UP, @onEvent.bind(this))
		@addEventListener(TouchEvent.MOVE, @onEvent.bind(this))
		
		@finger = Sprite().attrs {
			resAnim = res.get("finger"),
			parent = this,
			visible = false,
			pivot = vec2(0.5f, 0.5f),
			touchEnabled = false,
		}
		@active = false
		@dir = vec2(0, 0)
	},
		
	onEvent = function(ev){
		// print "Joystick.onEvent"
		ev as TouchEvent || throw "TouchEvent required"

		//if player touched down
		if(ev.type == TouchEvent.TOUCH_DOWN){
			@finger.visible = true
			
			@replaceTweenAction {
				name = "colorTween",
				duration = 0.2,
				color = Color(0.7, 0, 0),
			}
			
			@active = true
		}

		//if player touched up
		if(ev.type == TouchEvent.TOUCH_UP){
			@finger.visible = false
			
			@replaceTweenAction {
				name = "colorTween",
				duration = 0.2,
				color = Color(1, 1, 1),
			}
			
			@active = false
		}

		if(ev.type == TouchEvent.MOVE){
		
		}

		var center = @size / 2
		@dir = (ev.localPosition - center).normalizeToMax(100)
		@finger.pos = center + @dir
		// @active || @dir = vec2(0, 0)
	}
}
