Enemy = extends Actor {
	__construct = function(){	
		//you could hit enemy 3 times
		@hp = 3

		var sprite = Sprite().attrs {
			resAnim = res.getResAnim("asteroid"),
			parent = this,
			pivot = vec2(0.5f, 0.5f),
		}
		
		//it random scale and rotation
		sprite.angle = math.random(0, 360)
		sprite.scale = math.random(0.4, 1.0)

		//it is rotating by tween with random speed
		var dest = (math.random() > 0.5 ? 360 : -360) + sprite.angle
		sprite.addTween("angle", dest, math.random(20000) + 10000, -1)
		
		@isAlive = true
	},
	
	update = function(ev){
	},
	
	explode = function(){
		if(--@hp == 0){
			@isAlive = false
			@addTween("opacity", 0, 300).detachActor = true
		}
	},
}
