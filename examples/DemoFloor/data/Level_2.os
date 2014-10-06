Level_2 = extends Level {
	__construct = function(game){
		super(game)
		@levelNum = 2
		@state = "GENERIC"
		@openDoors = [false, false]
		
		@brokenPanel = Sprite().attrs {
			resAnim = res.get("broken-panel"),
			pivot = vec2(0.01, 0.99),
			pos = vec2(@width/2 - 200, 208),
			angle = 0,
			parent = this,
			priority = 10,
		}
		@initRotatableObject {
			@brokenPanel,
			// minAngle = -10,
			// maxAngle = 65,
			onBegin = function(){
				@state = "GENERIC"
				@findSlotObject(@bolt) || @returnSlotObject(@bolt)
				@findSlotObject(@screwdriver) || @returnSlotObject(@screwdriver)
				@stopPanelSwing()
				@selectSlot(null)
			},
			onEnd = function(){
				if(@state != "GENERIC"){
					throw "expect GENERIC state, but ${state} found"
				}
				var goodPos = math.abs(@brokenPanel.angle) < 5
				if(goodPos){
					@state = "WAIT_BOLT_USED"
					@addTimeout(2.5, function(){
						if(@state == "WAIT_BOLT_USED"){
							@state = "GENERIC"
							// @returnSlotObject(@bolt)
							@startPanelSwing()
						}
					})
				}else{
					// @state = "GENERIC"
					@startPanelSwing()
				}
			},
		}
		
		@bolt = Sprite().attrs {
			name = "obj-02",
			resAnim = res.get("obj-02"),
			pivot = vec2(0.5, 0.5),
			pos = vec2(@width/2 - 210, @elevatorInside.y + @elevatorInside.height + 30),
			parent = this,
			priority = 11,
			// extendedClickArea = 30,
			onSlotSelected = function(){
				if(@state == "WAIT_BOLT_USED"){
					@removeSlotObject(@bolt)
					@setPanelPosHoriz()
					@state = "WAIT_SCREWDRIVER_USED"
					@addTimeout(2.5, function(){
						if(@state == "WAIT_SCREWDRIVER_USED"){
							@state = "GENERIC"
							@returnSlotObject(@bolt)
							@startPanelSwing()
						}
					})
				}
			},
		}
		@initSlotObject{@bolt}
		
		@screwdriver = Sprite().attrs {
			name = "obj-03",
			resAnim = res.get("obj-03"),
			pivot = vec2(0.5, 0.5),
			pos = vec2(@width/2 + 210, @elevatorInside.y + @elevatorInside.height + 80),
			parent = this,
			priority = 11,
			// extendedClickArea = 30,
		}
		@initSlotObject{
			@screwdriver,
			onSlotSelected = function(){
				if(@state == "WAIT_SCREWDRIVER_USED"){
					@removeSlotObject(@screwdriver)
					@state = "WAIT_VASE_POS"
					@checkVasePosToFinish()
					@addTimeout(2.5, function(){
						if(@state == "WAIT_VASE_POS"){
							@state = "GENERIC"
							@returnSlotObject(@bolt)
							@returnSlotObject(@screwdriver)
							@startPanelSwing()
						}
					})
				}
			},
		}
		
		@vase = Sprite().attrs {
			// name = "obj-02",
			resAnim = res.get("obj-01"),
			pivot = vec2(0.5, 0.9),
			pos = vec2(@width/2 - 210, @elevatorInside.y + @elevatorInside.height + 60),
			parent = this,
			priority = 12,
			// extendedClickArea = 30,
		}
		@initHorizMovableObject{
			@vase,
			onMove = function(){
				@checkVasePosToFinish()
			},
			onEnd = function(){
				@checkVasePosToFinish()
			},
		}
		
		@startPanelSwing(1.0)
	},
	
	checkVasePosToFinish = function(){
		var offs = 210
		if(@state == "WAIT_VASE_POS" && @vase.x < @width/2 - offs || @vase.x > @width/2 + offs){
			@state = "FINISHED"
			@openDoor(0)
			@openDoor(1)
		}
	},
	
	stopPanelSwing = function(){
		@brokenPanel.removeActionsByName("panelSwing")
	},
	
	setPanelPosHoriz = function(){
		@stopPanelSwing()
		@brokenPanel.addTweenAction {
			name = "panelSwing",
			duration = 0.1, 
			angle = 0, 
			ease = Ease.QUAD_IN_OUT
		}
	},
	
	startPanelSwing = function(delay){
		var angles = [50, 30]
		if(@brokenPanel.angle > angles[0]){
			angles[0], angles[1] = angles[1], angles[0]
		}
		var action = RepeatForeverAction(SequenceAction(
			TweenAction {
				duration = 1,
				angle = angles[0],
				ease = Ease.QUAD_IN_OUT,
			},
			TweenAction {
				duration = 1,
				angle = angles[1],
				ease = Ease.QUAD_IN_OUT,
			},
		))
		if(delay){
			/* RepeatForeverAction.update is not implemented
			action = SequenceAction(
				TimeoutAction(delay),
				action,
			) */
			var normalAction = action
			action = TimeoutAction(delay, function(){
				@brokenPanel.addAction(normalAction).name = "panelSwing"
			})
		}
		@brokenPanel.addAction(action).name = "panelSwing"
	},
}
