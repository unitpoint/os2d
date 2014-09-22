print "--"
print "[start] ${DateTime.now()}"

GAME_SIZE = vec2(460 ,312) // 960, 540)

var displaySize = stage.size
var scale = displaySize / GAME_SIZE
// scale = math.max(scale.x, scale.y)
scale = math.min(scale.x, scale.y)
stage.size = displaySize / scale
stage.scale = scale

var bg = Sprite().attrs {
	resAnim = res.get("bg"),
	parent = stage,
	pos = stage.size/2,
	pivot = vec2(0.5, 0.5),
}
// bg.scale = math.max(stage.width / bg.width, stage.height / bg.height)

var spring = Sprite().attrs {
	resAnim = res.get("spring"),
	parent = stage,
	pos = stage.size/2,
	pivot = vec2(0.05, 0.5),
	touchStarted = false,
}

stage.addEventListener(TouchEvent.START, function(ev){
	if(ev.target === spring){
		spring.removeActionsByName("elasticTransition")
		var dx = ev.localPosition.x - spring.x
		var dy = ev.localPosition.y - spring.y
		spring.len0 = math.sqrt(dx*dx + dy*dy)
		spring.scale0 = spring.scaleX
		spring.angle0 = math.deg(math.atan2(dy, dx)) - spring.angle
		spring.touchStarted = true
	}
})

stage.addEventListener(TouchEvent.MOVE, function(ev){
	if(ev.target === spring && spring.touchStarted){
		var dx = ev.localPosition.x - spring.x
		var dy = ev.localPosition.y - spring.y
		var len = math.sqrt(dx*dx + dy*dy)
		spring.scaleX = spring.scale0 * len / spring.len0
		spring.angle = math.deg(math.atan2(dy, dx)) - spring.angle0
	}
})

stage.addEventListener(TouchEvent.END, function(ev){
	if(ev.target === spring){ // && spring.touchStarted){
		spring.touchStarted = false
		
		var angle = math.round(spring.angle / 90) * 90 // snap to angle
		spring.addTweenAction {
			name = "elasticTransition",
			duration = 0.5,
			scaleX = {to = 1, ease = Ease.BACK_OUT},
			angle = {to = angle, ease = Ease.BACK_OUT},
		}
	}
})