GameScene = extends Scene {
	__construct = function(){
		super()
		var sky = Sprite().attrs {
			resAnim = res.get("bg"),
			parent = @view,
		}
		
		var btn = MyButton().attrs {
			name = "play",
			resAnim = res.get("menu"),		
			pivot = vec2(1, 0),
			parent = @view,	
		}
		btn.x = @view.width
		btn.addEventListener(TouchEvent.CLICK, function(){
			!@transition && @changeScene(MainMenuScene.instance)
		})
		
		@move = Joystick()
		@move.parent = @view
		@move.y = @view.height - @move.height
		@move.priority = 1
		
		@fire = Joystick()
		@fire.parent = @view
		@fire.pos = @view.size - @move.size
		@fire.priority = 1
		
		@view.addUpdate(@update.bind(this))
		// @view.setInterval(1/10, @updateUnits.bind(this)))
		
		@units = {}
		for(var i = 0; i < 10; i++){
			var enemy = Enemy().attrs {
				parent = @view,
				x = math.random(0.1, 0.9) * @view.width,
				y = math.random(0.1, 0.9) * @view.height,
			}
			@units[enemy] = true
		}
		
		@player = Player().attrs {
			parent = @view,
			pos = @view.size / 2,
		}		
	},
	
	update = function(ev){
		ev.game = this
		@player.update(ev)
		for(var unit in @units){
			unit.update(ev)
			if(!unit.isAlive){
				delete @units[unit]
			}
		}
	},
	
	addRocket = function(rocket){
		rocket is Rocket || throw "Rocket required"
		rocket.parent = @view
		@units[rocket] = true
		// print "add rocket: ${rocket}"
	},
}

GameScene.instance = GameScene()
