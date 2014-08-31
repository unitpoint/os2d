Level = extends Actor {
	walls = "level-01-walls",
	elevator = "elevator-inside-01",
	
	__construct = function(){
		super()
		@size = stage.size
		
		var hudPanel = Sprite().attrs {
			resAnim = res.getResAnim("hud-items-panel"),
			parent = this,
			anchor = vec2(0.5, 1),
			pos = vec2(@width/2, @height),
			priority = 10,
		}
		
		@slots, @selectedSlotNum = [], -1
		var padding = hudPanel.height * 0.1
		var slotX = padding
		for(var i = 0; i < 4; i++){
			var slot = Sprite().attrs {
				resAnim = res.getResAnim("hud-panel-slot"),
				parent = this,
				anchor = vec2(0, 0.5),
				pos = vec2(slotX, hudPanel.y - hudPanel.height/2),
				priority = 12,
				touchChildrenEnabled = false,
				slotNum = i,
				object = null,
			}
			var selectedSprite = Sprite().attrs {
				resAnim = res.getResAnim("hud-panel-slot-selected"),
				parent = slot,
				anchor = vec2(0.5, 0.5),
				pos = slot.size / 2,
				priority = 1,
				visible = false,
				// touchEnabled = false,
			}
			slot.selectedSprite = selectedSprite
			slot.addEventListener(TouchEvent.CLICK, {|ev|
				// print "slot click: ${ev.target}"
				if(ev.target.slotNum != @selectedSlotNum){
					@slots[@selectedSlotNum].selectedSprite.visible = false
					@selectedSlotNum = ev.target.slotNum
					@slots[@selectedSlotNum].selectedSprite.visible = true
				}
			}.bind(this))
			slotX = slotX + slot.width + padding
			@slots[] = slot
		}
		/* @addTween(UpdateTween(1000/30, {|ev|
			var num = (@selectedSlotNum + 1) % #@slots
			@slots[num].dispatchEvent{TouchEvent.CLICK, target = @slots[num]}
		}.bind(this))) */
		
		var walls = Sprite().attrs {
			resAnim = res.getResAnim(@walls),
			parent = this,
			anchor = vec2(0.5, 0),
			x = @width/2,
			y = 0,
			priority = 2,
		}
		@elevatorInside = Sprite().attrs {
			resAnim = res.getResAnim(@elevator),
			parent = this,
			anchor = vec2(0.5, 0),
			pos = vec2(@width/2, 218),
			priority = 1,
		}
		@elevatorInsideButton = Sprite().attrs {
			resAnim = res.getResAnim("elevator-inside-up-button"),
			parent = @elevatorInside,
			anchor = vec2(0.5, 0.5),
			pos = @elevatorInside.size / 2,
		}
		@initDoors()
	},
	
	doorLeft = "door-01-left",
	doorRight = "door-01-right",
	
	initDoors = function(){
		@doorLeft = Sprite().attrs {
			resAnim = res.getResAnim(@doorLeft),
			parent = @elevatorInside,
			anchor = vec2(0, 0),
			pos = vec2(0, 0),
		}
		@doorLeft.closedPos = @doorLeft.pos
		@doorLeft.openPos = @doorLeft.pos - vec2(@doorLeft.width, 0)
		
		@doorRight = Sprite().attrs {
			resAnim = res.getResAnim(@doorRight),
			parent = @elevatorInside,
			anchor = vec2(1, 0),
			pos = vec2(@elevatorInside.width, 0),
		}
		@doorRight.closedPos = @doorRight.pos
		@doorRight.openPos = @doorRight.pos + vec2(@doorRight.width, 0)
		/*
			@doorRight = Sprite().attrs {
				resAnim = res.getResAnim("door-01-right"),
				parent = @doorLeft,
				anchor = vec2(0, 0),
				pos = vec2(@doorLeft.width, 0),
				priority = -1,
			}
			@doorRight.closedPos = @doorRight.pos
			@doorRight.openPos = @doorRight.pos - vec2(@doorRight.width, 0)
		*/
		
		@doors = [@doorLeft, @doorRight]
		
		/*
		@setAllDoorsPos(0)
		for(var i, door in @doors){
			door.addTween("pos", door.closedPos, 2000, 1, false, 0, Tween.EASE_OUTCUBIC).name = "animDoor"
		}
		*/
	},
	
	setSlotObject = function(i, name){
		var slot = @slots[i]
		slot.object.name == name && return;
		slot.object.parent = null
		slot.object = Sprite().attrs {
			name = name,
			resAnim = res.getResAnim(name.."-slot"),
			parent = slot,
			anchor = vec2(0.5, 0.5),
			pos = slot.size / 2,
			priority = 2,
			// visible = false,
			// touchEnabled = false,
		}
	},
	
	removeSlotObject = function(i){
		var slot = @slots[i]
		slot.object.parent = null
		slot.object = null
	},
	
	getDoorPos = function(i, move){ // 0 - open, 1 - close
		return @doors[i].openPos * (1 - move) + @doors[i].closedPos * move;
	},
	
	setAllDoorsPos = function(move){
		for(var i, door in @doors){
			door.pos = @getDoorPos(i, move)
		}
	},
}
