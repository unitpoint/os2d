Player = extends Actor {
	__construct = function(){
		@ship = Sprite().attrs {
			resAnim = res.get("ship"),
			parent = this,
			pivot = vec2(0.5f, 0.5f),
		}
		@engine = Sprite().attrs {
			resAnim = res.get("shipengine"),
			parent = @ship,
			visible = false,
		}
		@lastRocketSpawn = 0
		// animate engine's fire
		@engine.addAction(RepeatForeverAction(TweenAction{
			duration = 0.5,
			color = Color(1, 0, 0),
			ease = Ease.PING_PONG,
		}))
	},
	
	update = function(ev){
		@engine.visible = false
		if(ev.game.move.active){
			// print "move: ${ev.game.move}"
			var dir = ev.game.move.dir
			
			@pos += dir * (ev.dt * 5)
			
			@rotation = math.atan2(dir.y, dir.x)
			@engine.visible = true
		}
		
		if(ev.game.fire.active){
			//fire rockets each 300 ms
			if(@lastRocketSpawn + 0.2 < ev.time){
				// print "spawn rocket"
				@lastRocketSpawn = ev.time
				ev.game.addRocket(Rocket(@pos, ev.game.fire.dir))
			}		
		}		
	},
}
