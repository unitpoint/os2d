Level_1 = extends Level {
	__construct = function(game){
		super(game)
		@levelNum = 1
		
		@btnLeft = Sprite().attrs {
			resAnim = res.get("light-red"),
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
			resAnim = res.get("light-red"),
			pos = vec2(@width/2 + offs, y),
			pivot = vec2(0.5, 0.5),
			parent = this,
			extendedClickArea = @btnLeft.width * 2,
			priority = 10,
			// name = "btnRight",
			btnIndex = 1,
		}
		
		@addEventListener(TouchEvent.CLICK, @click.bind(this))
		
		// @setSlotObject(0, "obj-02")
		// @setSlotObject(1, "obj-03")		
	},
	
	click = function(ev){
		if("btnIndex" in ev.target){
			ev.target.resAnim = res.get("light-green")
			@openDoor(ev.target.btnIndex)
		}
	},
}
