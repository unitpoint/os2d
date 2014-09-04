Level = extends Scene {
	wallsName = "level-01-walls",
	elevatorName = "elevator-inside-01",
	elevatorInsideButtonName = "elevator-inside-up-button",
	levelNum = 0,
	
	__construct = function(game){
		super()
		@game = game
		@size = stage.size
		
		var hudPanel = Sprite().attrs {
			resAnim = res.getResAnim("hud-items-panel"),
			parent = this,
			anchor = vec2(0.5, 1),
			pos = vec2(@width/2, @height),
			priority = 100,
		}
		
		@slots, @selectedSlot, @selectedObject = []
		var padding = hudPanel.height * 0.1
		var slotX = padding
		for(var i = 0; i < 4; i++){
			var slot = Sprite().attrs {
				resAnim = res.getResAnim("hud-panel-slot"),
				parent = this,
				anchor = vec2(0, 0.5),
				pos = vec2(slotX, hudPanel.y - hudPanel.height/2),
				priority = 102,
				touchChildrenEnabled = false,
				// slotNum = i,
				slotPicture = null,
				sceneObject = null,
			}
			var selectedBG = Sprite().attrs {
				resAnim = res.getResAnim("hud-panel-slot-selected"),
				parent = slot,
				anchor = vec2(0.5, 0.5),
				pos = slot.size / 2,
				priority = 1,
				visible = false,
				// touchEnabled = false,
			}
			slot.selectedBG = selectedBG
			slotX = slotX + slot.width + padding
			@slots[] = slot
		}
		@addEventListener(TouchEvent.CLICK, {|ev|
			// print "slot click: ${ev.target}"
			if("selectedBG" in ev.target && ev.target !== @selectedSlot){
				@selectSlot(ev.target)
			}
		}.bind(this))
		
		var wallsName = Sprite().attrs {
			resAnim = res.getResAnim(@wallsName),
			parent = this,
			pivot = vec2(0.5, 0),
			x = @width/2,
			y = 0,
			priority = 2,
			touchEnabled = false,
		}
		@elevatorInside = Sprite().attrs {
			resAnim = res.getResAnim(@elevatorName),
			parent = this,
			pivot = vec2(0.5, 0),
			pos = vec2(@width/2, 218),
			priority = 1,
		}
		@elevatorInsideButton = Sprite().attrs {
			resAnim = res.getResAnim(@elevatorInsideButtonName),
			parent = @elevatorInside,
			pivot = vec2(0.5, 0.5),
			pos = @elevatorInside.size / 2,
			opacity = 0,
			priority = 100,
		}
		
		@openDoors = [false, false]
		@initDoors()
	},
	
	nextLevelOpened = false,
	allowNextLevel = function(){
		if(!@nextLevelOpened){
			@nextLevelOpened = true
			@elevatorInsideButton.addTween("opacity", 1, 300)
			@elevatorInsideButton.addEventListener(TouchEvent.CLICK, {||
				// print "next level clicked, ${this}"
				@game.nextLevel(@levelNum + 1)
			}.bind(this))
		}
	},
	
	doorLeftName = "door-01-left",
	doorRightName = "door-01-right",
	
	initDoors = function(){
		@doorLeft = Sprite().attrs {
			resAnim = res.getResAnim(@doorLeftName),
			parent = @elevatorInside,
			pivot = vec2(0, 0),
			pos = vec2(0, 0),
		}
		@doorLeft.closedPos = @doorLeft.pos
		@doorLeft.openPos = @doorLeft.pos - vec2(@doorLeft.width*0.95, 0)
		
		@doorRight = Sprite().attrs {
			resAnim = res.getResAnim(@doorRightName),
			parent = @elevatorInside,
			pivot = vec2(1, 0),
			pos = vec2(@elevatorInside.width, 0),
		}
		@doorRight.closedPos = @doorRight.pos
		@doorRight.openPos = @doorRight.pos + vec2(@doorRight.width*0.95, 0)
		/*
			@doorRight = Sprite().attrs {
				resAnim = res.getResAnim("door-01-right"),
				parent = @doorLeft,
				pivot = vec2(0, 0),
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
	
	selectSlot = function(slot){
		@selectedSlot.selectedBG.visible = false
		@selectedSlot = slot
		@selectedSlot.selectedBG.visible = true
		@selectedSlot.sceneObject.onSlotSelected()
	},
	
	isSlotObjectSelected = function(obj){
		return @selectedSlot.sceneObject === obj
	},
	
	returnSlotObject = function(obj){
		@removeSlotObject(obj)
		if(!obj.parent){
			obj.removeTweensByName("returnSlotObject")
			obj.attrs {
				opacity = 0, 
				parent = obj.originParent,
				pos = obj.originPos,
			}
			obj.addTween("opacity", 1, 200).name = "returnSlotObject"
		}
	},
	
	addSlotObject = function(obj){
		var emptySlot = null
		for(var i, slot in @slots){
			slot.sceneObject === obj && return;
			!emptySlot && !slot.sceneObject && emptySlot = slot
		}
		if(emptySlot){
			obj.originPos = obj.pos
			obj.originParent = obj.parent
			obj.parent = null
			emptySlot.sceneObject = obj
			emptySlot.slotPicture = Sprite().attrs {
				resAnim = res.getResAnim(obj.name.."-slot"),
				parent = emptySlot,
				pivot = vec2(0.5, 0.5),
				pos = emptySlot.size / 2,
				priority = 2,
			}
			@selectSlot(null)
			return true
		}
	},
	
	setSlotObject = function(i, obj){
		var slot = @slots[i]
		slot.sceneObject === obj && return;
		slot.sceneObject = obj
		slot.slotPicture.parent = null
		slot.slotPicture = Sprite().attrs {
			resAnim = res.getResAnim(obj.name.."-slot"),
			parent = slot,
			pivot = vec2(0.5, 0.5),
			pos = slot.size / 2,
			priority = 2,
			// visible = false,
			// touchEnabled = false,
		}
	},
	
	removeSlotObject = function(obj){
		for(var i, slot in @slots){
			if(slot.sceneObject === obj){
				if(@selectedSlot === slot){
					@selectSlot(null)
				}
				slot.slotPicture.parent = null
				slot.slotPicture = null
				slot.sceneObject = null
				return true
			}
		}
	},
	
	findSlotObject = function(obj){
		for(var i, slot in @slots){
			if(slot.sceneObject === obj){
				return true
			}
		}
	},
	
	getDoorPos = function(i, move){ // 0 - open, 1 - close
		return @doors[i].openPos * (1 - move) + @doors[i].closedPos * move;
	},
	
	setAllDoorsPos = function(move){
		for(var i, door in @doors){
			door.pos = @getDoorPos(i, move)
		}
	},
	
	dirToAngle = function(dir){
		return math.deg(math.atan2(dir.y, dir.x))
	},
	
	normAngle = function(angle){
		if(angle < 0){
			return 360 + angle - math.floor(angle / 360) * 360
		}
		if(angle >= 360){
			return angle - math.floor(angle / 360) * 360
		}
		return angle
	},
	
	normAngle180 = function(angle){
		angle = @normAngle(angle)
		return angle > 180 ? angle - 360 : angle
	},	
	
	anglesDiff = function(a, b){
		a = @normAngle(a)
		b = @normAngle(b)
		var diff = b - a
		if(diff > 180){
			return diff - 360
		}
		if(diff < -180){
			return diff + 360
		}
		return diff
	},	
	
	prepareRotateByTouchEvent = function(actor, ev){
		actor.prevTouchPoint = actor.parent.local2global(ev.localPosition)
	},
	
	rotateByTouchEvent = function(actor, ev, minAngle, maxAngle){
		var pos = actor.pos
		var prevAngle = @dirToAngle(actor.parent.global2local(actor.prevTouchPoint) - pos)
		var curAngle = @dirToAngle(ev.localPosition - pos)
		var newAngle = actor.angle - prevAngle + curAngle
		if(minAngle){
			if(minAngle > maxAngle){
				minAngle, maxAngle = maxAngle, minAngle
			}
			if(newAngle < minAngle || newAngle > maxAngle){
				var offs1 = math.abs(@anglesDiff(minAngle, newAngle))
				var offs2 = math.abs(@anglesDiff(maxAngle, newAngle))
				newAngle = offs1 < offs2 ? minAngle : maxAngle
			}
		}
		actor.angle = newAngle
		actor.prevTouchPoint = actor.parent.local2global(ev.localPosition)
	},
	
	prepareMoveByTouchEvent = function(actor, ev){
		actor.prevTouchPoint = actor.parent.local2global(ev.localPosition)
	},
	
	horizMoveByTouchEvent = function(actor, ev){
		var prevPos = actor.parent.global2local(actor.prevTouchPoint)
		var newPos = ev.localPosition
		actor.x = actor.x - prevPos.x + newPos.x
		actor.prevTouchPoint = actor.parent.local2global(ev.localPosition)
	},
	
	initSlotObject = function(obj, params){
		if(obj.prototype === Object){
			params && throw "2rd argument should be null here"
			obj, params = obj.shift(), obj
		}
		obj.parent.addEventListener(TouchEvent.TOUCH_DOWN, function(ev){
			if(ev.target === obj && params.onBegin() !== false){
				@selectedObject = obj
				params.onRotate()
			}
		}.bind(this))
		
		obj.parent.addEventListener(TouchEvent.TOUCH_UP, function(ev){
			if(@selectedObject === obj){
				@selectedObject = null
				params.onEnd(@addSlotObject(obj))
			}
		}.bind(this))
	},
	
	initHorizMovableObject = function(obj, params){
		if(obj.prototype === Object){
			params && throw "2rd argument should be null here"
			obj, params = obj.shift(), obj
		}
		obj.parent.addEventListener(TouchEvent.TOUCH_DOWN, function(ev){
			if(ev.target === obj && params.onBegin() !== false){
				@selectedObject = obj
				@prepareMoveByTouchEvent(obj, ev)
			}
		}.bind(this))
		
		obj.parent.addEventListener(TouchEvent.MOVE, function(ev){
			if(@selectedObject === obj){
				@horizMoveByTouchEvent(obj, ev)
				params.onMove()
			}
		}.bind(this))
		
		obj.parent.addEventListener(TouchEvent.TOUCH_UP, function(ev){
			if(@selectedObject === obj){
				@selectedObject = null
				params.onEnd()
			}
		}.bind(this))
	},
	
	initRotableObject = function(obj, params){
		if(obj.prototype === Object){
			params && throw "2rd argument should be null here"
			obj, params = obj.shift(), obj
		}
		obj.parent.addEventListener(TouchEvent.TOUCH_DOWN, function(ev){
			if(ev.target === obj && params.onBegin() !== false){
				@selectedObject = obj
				@prepareRotateByTouchEvent(obj, ev)
			}
		}.bind(this))
		
		obj.parent.addEventListener(TouchEvent.MOVE, function(ev){
			if(@selectedObject === obj){
				@rotateByTouchEvent(obj, ev, params.minAngle, params.maxAngle)
			}
		}.bind(this))
		
		obj.parent.addEventListener(TouchEvent.TOUCH_UP, function(ev){
			if(@selectedObject === obj){
				@selectedObject = null
				params.onEnd()
			}
		}.bind(this))
	},
}
