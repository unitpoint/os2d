print "--"
print "[start] ${DateTime.now()}"

require "std.os"

var displaySize = stage.size
var gameSize = vec2(960, 480)
var scale = displaySize / gameSize
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

MainMenuScene.instance.show()
// GameScene.instance.show()
