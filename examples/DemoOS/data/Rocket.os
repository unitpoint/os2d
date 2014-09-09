Rocket = extends Actor {
	__construct = function(pos, dir){
		super()
		
		@pos = pos
		var sp = Sprite().attrs {
			resAnim = res.getResAnim("rocket"),
			pivot = vec2(0.5f, 0.5f),
			parent = this,
			rotation = math.atan2(dir.y, dir.x),
			scale = 0,
			// opacity = 0,
		}
		sp.addTweenAction(0.5, "scale", 1)
		
		@dir = dir.clone().normalize()
		@isAlive = true
	},
	
	update = function(ev){
		// print "Rocket.update, us: ${ev.us}, dir: ${@dir}"
		@pos += @dir * (ev.dt * 500)
		
		/* if(math.random() < 0.05){
			@explode()
			return
		} */
		
		for(var unit in ev.game.units){
			if(unit is Enemy){
				var len = #(unit.pos - @pos)
				// print "enemy${unit.__id} len: ${len}, unit.pos: ${unit.pos}, self pos: ${@pos}"
				if(len < 20){
					unit.explode(@pos)
					@explode()
					return
				}
			}
		}
		
		var pos, size = @pos, ev.game.view.size
		// print "rocket pos: ${pos}, view size: ${size}"
		if(pos.x < 0 || pos.x >= size.x ||
			pos.y < 0 || pos.y >= size.y)
		{
			@explode()
		}
	},
	
	explode = function(){
		// print "rocket explode"
		@isAlive = false
		// @detach()
		// return;
		
		var anim = Sprite().attrs {
			parent = @parent,
			blendMode = BLEND_ADD,
			pos = @pos,
			pivot = vec2(0.5f, 0.5f),
		}
		
		//run tween with explosion animation
		var tween = anim.addTween("resAnim", res.getResAnim("explosion"), 1)
		//auto detach sprite when tween is done
		tween.detachTarget = true

		//hide rocket and then detach it
		@addTweenAction {
			duration = 0.5,
			opacity = 0,
			detachTarget = true,
		}
	},
}
