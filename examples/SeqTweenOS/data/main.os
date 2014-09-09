print "--"
print "[start] ${DateTime.now()}"

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

var useNewTech = true
if(useNewTech){

	fan.addAction(RepeatForeverAction(TweenAction{
		duration = 3.5, 
		angle = {from = 0, to = 360*2, ease = Ease.BACK_OUT}
	}))

	base.addAction(RepeatForeverAction(SequenceAction(
		TweenAction {
			duration = 2,
			pos = {to = stage.size, ease = Ease.BACK_IN_OUT},
			angle = {to = -45, ease = Ease.BACK_IN_OUT},
		},
		TweenAction {
			duration = 2,
			pos = {to = vec2(stage.width, 0), ease = Ease.EXPO_IN_OUT},
			angle = {to = -135, ease = Ease.EXPO_IN_OUT},
		},
		TweenAction {
			duration = 2,
			pos = {to = vec2(0, 0), ease = Ease.BOUNCE_OUT},
			angle = {to = -225, ease = Ease.BOUNCE_OUT},
		},
		TweenAction {
			duration = 2,
			pos = {to = vec2(0, stage.height)},
			angle = {to = 45, fixRotation = true},
		},
	)))

}else{

	fan.addTween("angle", 360*2, 3.5, -1, false, 0, Tween.EASE_OUTBACK)

	var startSeqTween = function(){
		var seq = SequenceTween()
		seq.add("pos", stage.size, 2, 1, false, 0, Tween.EASE_INOUTBACK)
		seq.add("pos", vec2(stage.width, 0), 2, 1, false, 0, Tween.EASE_INOUTEXPO)
		seq.add("pos", vec2(0, 0), 2, 1, false, 0, Tween.EASE_OUTBOUNCE)
		seq.add("pos", vec2(0, stage.height), 2, 1, false, 0, Tween.EASE_LINEAR)
		base.addTween(seq)
		
		var seq = SequenceTween()
		seq.add("angle", -45, 2, 1, false, 0, Tween.EASE_INOUTBACK)
		seq.add("angle", -135, 2, 1, false, 0, Tween.EASE_INOUTEXPO)
		seq.add("angle", -225, 2, 1, false, 0, Tween.EASE_OUTBOUNCE).doneCallback = function(){ base.angle = 135 }
		seq.add("angle", 45, 2, 1, false, 0, Tween.EASE_LINEAR)
		seq.doneCallback = startSeqTween
		base.addTween(seq)
	}
	startSeqTween()

}