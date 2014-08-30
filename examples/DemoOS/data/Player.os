Player = extends Actor {
	__construct = function(){
		@ship = Sprite().attrs {
			resAnim = res.getResAnim("ship"),
			parent = this,
			anchor = vec2(0.5f, 0.5f),
		}
		@engine = Sprite().attrs {
			resAnim = res.getResAnim("shipengine"),
			parent = @ship,
			visible = false,
		}
		@lastRocketSpawn = 0
		//animate engine's fire
		@engine.addTween("color", Color(1, 0, 0), 500, -1, true)
	},
	
	update = function(ev){
		@engine.visible = false
		if(ev.game.move.active){
			// print "move: ${ev.game.move}"
			var dir = ev.game.move.dir
			
			@pos += dir * (ev.us.dt / 1000 * 5)
			
			@rotation = math.atan2(dir.y, dir.x)
			@engine.visible = true
		}
		
		if(ev.game.fire.active){
			//fire rockets each 300 ms
			if(@lastRocketSpawn + 300 < ev.us.time){
				// print "spawn rocket"
				@lastRocketSpawn = ev.us.time
				ev.game.addRocket(Rocket(@pos, ev.game.fire.dir))
			}		
		}		
	},
}
