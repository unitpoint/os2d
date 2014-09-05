print "--"
print "[start] ${DateTime.now()}"

require "std.os"

GAME_SIZE = vec2(720, 405) // 960, 540)

var displaySize = stage.size
var scale = displaySize / GAME_SIZE
// scale = math.max(scale.x, scale.y)
scale = math.min(scale.x, scale.y)
stage.size = displaySize / scale
stage.scale = scale

var bg = Sprite().attrs {
	resAnim = res.getResAnim("bg"),
	parent = stage,
	pos = stage.size/2,
	pivot = vec2(0.5, 0.5),
}
bg.scale = math.max(stage.width / bg.width, stage.height / bg.height)

var base = Sprite().attrs {
	resAnim = res.getResAnim("base"),
	parent = stage,
	pivot = vec2(0.5, 0.95),
	pos = vec2(0, stage.height),
	angle = 45,
}

var fan = Sprite().attrs {
	resAnim = res.getResAnim("fan"),
	parent = base,
	pivot = vec2(0.5, 0.5),
	pos = vec2(base.width/2, base.height*0.05),
}

fan.addTween("angle", 360*2, 3500, -1, false, 0, Tween.EASE_OUTBACK)

var startSeqTween = function(){
	var seq = SequenceTween()
	seq.add("pos", stage.size, 2000, 1, false, 0, Tween.EASE_INOUTBACK)
	seq.add("pos", vec2(stage.width, 0), 2000, 1, false, 0, Tween.EASE_INOUTEXPO)
	seq.add("pos", vec2(0, 0), 2000, 1, false, 0, Tween.EASE_OUTBOUNCE)
	seq.add("pos", vec2(0, stage.height), 2000, 1, false, 0, Tween.EASE_LINEAR)
	base.addTween(seq)
	
	var seq = SequenceTween()
	seq.add("angle", -45, 2000, 1, false, 0, Tween.EASE_INOUTBACK)
	seq.add("angle", -135, 2000, 1, false, 0, Tween.EASE_INOUTEXPO)
	seq.add("angle", -225, 2000, 1, false, 0, Tween.EASE_OUTBOUNCE).doneCallback = function(){ base.angle = 135 }
	seq.add("angle", 45, 2000, 1, false, 0, Tween.EASE_LINEAR)
	seq.doneCallback = startSeqTween
	base.addTween(seq)
}
startSeqTween()
