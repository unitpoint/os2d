Level_2 = extends Level {
	__construct = function(game){
		super(game)
		@levelNum = 2
		@state = "GENERIC"
		@openDoors = [false, false]
		
		@brokenPanel = Sprite().attrs {
			resAnim = res.getResAnim("broken-panel"),
			pivot = vec2(0.01, 0.99),
			pos = vec2(@width/2 - 200, 208),
			angle = 0,
			parent = this,
			priority = 10,
		}
		@initRotableObject {
			@brokenPanel,
			// minAngle = -10,
			// maxAngle = 65,
			onBegin = function(){
				@state = "GENERIC"
				@findSlotObject(@bolt) || @returnSlotObject(@bolt)
				@findSlotObject(@screwdriver) || @returnSlotObject(@screwdriver)
				@stopPanelSwing()
				@selectSlot(null)
			}.bind(this),
			onEnd = function(){
				if(@state != "GENERIC"){
					throw "expect GENERIC state, but ${state} found"
				}
				var goodPos = math.abs(@brokenPanel.angle) < 5
				if(goodPos){
					@state = "WAIT_BOLT_USED"
					@addTween(DoneTween(2500, function(){
						if(@state == "WAIT_BOLT_USED"){
							@state = "GENERIC"
							// @returnSlotObject(@bolt)
							@startPanelSwing()
						}
					}.bind(this)))
				}else{
					// @state = "GENERIC"
					@startPanelSwing()
				}
			}.bind(this),
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
			onSlotSelected = function(){
				if(@state == "WAIT_BOLT_USED"){
					@removeSlotObject(@bolt)
					@setPanelPosHoriz()
					@state = "WAIT_SCREWDRIVER_USED"
					@addTween(DoneTween(2500, function(){
						if(@state == "WAIT_SCREWDRIVER_USED"){
							@state = "GENERIC"
							@returnSlotObject(@bolt)
							@startPanelSwing()
						}
					}.bind(this)))
				}
			}.bind(this),
		}
		@initSlotObject{@bolt}
		
		@screwdriverInitPos = vec2(@width/2 + 210, @elevatorInside.y + @elevatorInside.height + 80)
		@screwdriver = Sprite().attrs {
			name = "obj-03",
			resAnim = res.getResAnim("obj-03"),
			pivot = vec2(0.5, 0.5),
			pos = @screwdriverInitPos,
			parent = this,
			priority = 11,
			// extendedClickArea = 30,
			onSlotSelected = function(){
				if(@state == "WAIT_SCREWDRIVER_USED"){
					@removeSlotObject(@screwdriver)
					@state = "WAIT_VASE_POS"
					@checkVasePosToFinish()
					@addTween(DoneTween(2500, function(){
						if(@state == "WAIT_VASE_POS"){
							@state = "GENERIC"
							@returnSlotObject(@bolt)
							@returnSlotObject(@screwdriver)
							@startPanelSwing()
						}
					}.bind(this)))
				}
			}.bind(this),
		}
		@initSlotObject{@screwdriver}
		
		@vase = Sprite().attrs {
			// name = "obj-02",
			resAnim = res.getResAnim("obj-01"),
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
			}.bind(this),
			onEnd = function(){
				@checkVasePosToFinish()
			}.bind(this),
		}
		
		@startPanelSwing(1000)
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
		@brokenPanel.removeTweensByName("panelSwing")
	},
	
	setPanelPosHoriz = function(){
		@stopPanelSwing()
		@brokenPanel.addTween("angle", 0, 100, 1, false, 0, Tween.EASE_INOUTQUAD).name = "panelSwing"
	},
	
	startPanelSwing = function(delay){
		var angles = [50, 30]
		if(@brokenPanel.angle > angles[0]){
			angles[0], angles[1] = angles[1], angles[0]
		}
		var seq = SequenceTween()
		seq.add("angle", angles[0], 1000, 1, false, delay || 0, Tween.EASE_INOUTQUAD)
		seq.add("angle", angles[1], 1000, 1, false, 0, Tween.EASE_INOUTQUAD)
		seq.doneCallback = function(){ @startPanelSwing() }.bind(this)
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
