Level_1 = extends Level {
	__construct = function(game){
		super(game)
		@levelNum = 1
		
		@btnLeft = Sprite().attrs {
			resAnim = res.getResAnim("light-red"),
		}
		
		var y = @elevatorInside.y + @elevatorInside.height/2
		var offs = @elevatorInside.width/2 + @btnLeft.width
		
		@btnLeft.attrs {
			pos = vec2(@width/2 - offs, y),
			pivot = vec2(0.5, 0.5),
			parent = this,
			extendedClickArea = @btnLeft.width * 2,
			priority = 10,
			// name = "btnLeft",
			btnIndex = 0,
		}
		
		@btnRight = Sprite().attrs {
			resAnim = res.getResAnim("light-red"),
			pos = vec2(@width/2 + offs, y),
			pivot = vec2(0.5, 0.5),
			parent = this,
			extendedClickArea = @btnLeft.width * 2,
			priority = 10,
			// name = "btnRight",
			btnIndex = 1,
		}
		
		@openDoors = [false, false]
		@addEventListener(TouchEvent.CLICK, @click.bind(this))
		
		// @setSlotObject(0, "obj-02")
		// @setSlotObject(1, "obj-03")		
	},
	
	
	openDoor = function(i){
		if(!@openDoors[i]){
			@openDoors[i] = true
			var door = @doors[i]
			// print "openDoor(${i}): ${door}, ${@doors}"
			door.addTween("pos", door.openPos, 1000, 1, false, 0, Tween.EASE_OUTCUBIC).attrs {
				doneCallback = function(){
					if(@openDoors[0] && @openDoors[1]){
						@allowNextLevel()
					}
				}.bind(this),
			}
		}
	},
	
	click = function(ev){
		if("btnIndex" in ev.target){
			ev.target.resAnim = res.getResAnim("light-green")
			@openDoor(ev.target.btnIndex)
		}
	},
}
