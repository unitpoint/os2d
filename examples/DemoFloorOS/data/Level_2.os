Level_2 = extends Level {
	__construct = function(game){
		super(game)
		@levelNum = 2
		@openDoors = [false, false]
		@state = "generic"
		
		@brokenPanel = Sprite().attrs {
			resAnim = res.getResAnim("broken-panel"),
			pivot = vec2(0.01, 0.99),
			pos = vec2(@width/2 - 200, 208),
			angle = 0,
			parent = this,
			priority = 10,
		}
		
		@boltInitPos = vec2(@width/2 - 210, @elevatorInside.y + @elevatorInside.height + 30)
		@bolt = Sprite().attrs {
			name = "obj-02",
			resAnim = res.getResAnim("obj-02"),
			pivot = vec2(0.5, 0.5),
			pos = @boltInitPos,
			parent = this,
			priority = 11,
			// extendedClickArea = 30,
		}
		
		@vase = Sprite().attrs {
			// name = "obj-02",
			resAnim = res.getResAnim("obj-01"),
			pivot = vec2(0.5, 0.9),
			pos = vec2(@width/2 - 210, @elevatorInside.y + @elevatorInside.height + 60),
			parent = this,
			priority = 12,
			// extendedClickArea = 30,
		}
		
		@selectedObject = null
		
		@addEventListener(TouchEvent.CLICK, function(ev){
		}.bind(this))
		
		@addEventListener(TouchEvent.TOUCH_DOWN, function(ev){
			if(ev.target === @brokenPanel){
				@stopPanelSwing()
				@selectedObject = @brokenPanel
				@prepareRotateByTouchEvent(@selectedObject, ev)
				return
			}
			if(ev.target == @vase){
				@selectedObject = ev.target
				@prepareMoveByTouchEvent(@selectedObject, ev)
				return
			}
			if(ev.target === @bolt){
				@selectedObject = ev.target
				return
			}
			if(@selectedObject === @brokenPanel){
				@selectedObject = null
				@startPanelSwing()
			}
		}.bind(this))
		
		@addEventListener(TouchEvent.MOVE, function(ev){
			if(@selectedObject === @brokenPanel){
				@rotateByTouchEvent(@selectedObject, ev, -10, 65)
				return
			}
			if(@selectedObject === @vase){
				@horizMoveByTouchEvent(@selectedObject, ev)
				return
			}
		}.bind(this))
		
		@addEventListener(TouchEvent.TOUCH_UP, function(ev){
			if(@selectedObject === @brokenPanel){
				if(@state != "panelFixed"){
					var goodPos = math.abs(@brokenPanel.angle) < 5
					if(@getSlotObjectSelected(@bolt)){
						@removeSlotObject(@bolt)
						@brokenPanel.addTween(UpdateTween(2500, function(){
							@brokenPanel.removeTweensByName("waitFix")
							@bolt.attrs {
								pos = @boltInitPos,
								parent = this,
								opacity = 0
							}.addTween("opacity", 1, 100)
							@state = "generic"
						}.bind(this))).name = "waitFix"
					}
				}
				@selectedObject = null
				@startPanelSwing(goodPos ? 2000 : 0)
				if(goodPos){
					@brokenPanel.addTween("angle", 0, 300, 1, false, 0, Tween.EASE_INOUTQUAD).name = "panelSwing"
				}
				return
			}
			if(@selectedObject === @bolt && @addSlotObject(@bolt)){
				@bolt.parent = null
			}
			@selectedObject = null
		}.bind(this))
		
		@startPanelSwing()
	},
	
	stopPanelSwing = function(){
		@brokenPanel.removeTweensByName("panelSwing")
	},
	
	startPanelSwing = function(delay){
		@stopPanelSwing()
		var angles = [50, 30]
		if(@brokenPanel.angle > angles[0]){
			angles[0], angles[1] = angles[1], angles[0]
		}
		var seq = SequenceTween()
		seq.add("angle", angles[0], 1000, 1, false, delay || 0, Tween.EASE_INOUTQUAD)
		seq.add("angle", angles[1], 1000, 1, false, 0, Tween.EASE_INOUTQUAD)
		seq.doneCallback = @startPanelSwing.bind(this)
		seq.name = "panelSwing"
		@brokenPanel.addTween(seq)
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
