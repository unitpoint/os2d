Level_1 = extends Level {
	__construct = function(){
		super()
		
		var btnLeft = Sprite().attrs {
			resAnim = res.getResAnim("light-red"),
		}
		
		var y = @elevatorInside.y + @elevatorInside.height/2
		var offs = @elevatorInside.width/2 + btnLeft.width
		
		btnLeft.attrs {
			pos = vec2(@width/2 - offs, y),
			anchor = vec2(0.5, 0.5),
			parent = this,
			extendedClickArea = btnLeft.width * 2,
			priority = 10,
		}
		var openDoors = [false, false]
		var openDoor = function(i){
			var door = @doors[i]
			// print "openDoor(${i}): ${door}, ${@doors}"
			door.addTween("pos", door.openPos, 1000, 1, false, 0, Tween.EASE_OUTCUBIC).attrs {
				doneCallback = function(){
					openDoors[i] = true
					if(openDoors[0] && openDoors[1]){
						// @canFinishLevel()
						print "doors opened"
					}
				}.bind(this),
			}
		}.bind(this)
		var click = function(ev){
			var btn = ev.target
			var i = btn === btnLeft ? 0 : btn === btnRight ? 1 : 0
			btn.removeEventListener(btn.evId) // TouchEvent.CLICK)
			btn.resAnim = res.getResAnim("light-green")
			openDoor(i)
		}.bind(this)
		
		var btnRight = Sprite().attrs {
			resAnim = res.getResAnim("light-red"),
			pos = vec2(@width/2 + offs, y),
			anchor = vec2(0.5, 0.5),
			parent = this,
			extendedClickArea = btnLeft.width * 2,
			priority = 10,
		}

		btnLeft.evId = btnLeft.addEventListener(TouchEvent.CLICK, click)
		btnRight.evId = btnRight.addEventListener(TouchEvent.CLICK, click)
		
		@setSlotObject(0, "obj-02")
		@setSlotObject(1, "obj-03")		
	},
}
