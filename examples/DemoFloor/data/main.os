print "--"
print "[start] ${DateTime.now()}"

GAME_SIZE = vec2(540, 960)

var displaySize = stage.size
var scale = displaySize / GAME_SIZE
// scale = math.max(scale.x, scale.y)
scale = math.min(scale.x, scale.y)
stage.size = displaySize / scale
stage.scale = scale

stage.addEventListener(Stage.ACTIVATE, function(){
	print "Stage.ACTIVATE"
})

stage.addEventListener(Stage.DEACTIVATE, function(){
	print "Stage.DEACTIVATE"
})

print "stage.size: ${stage.size}, scale: ${stage.scale}, display: ${displaySize}"

GameScene().show()
