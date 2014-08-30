print "--"
print "[start] ${DateTime.now()}"

require "std.os"

var displaySize = root.size
var gameSize = vec2(960, 480)
var scale = displaySize / gameSize
// scale = math.max(scale.x, scale.y)
scale = math.min(scale.x, scale.y)
root.size = displaySize / scale
root.scale = scale

root.addEventListener(RootActor.ACTIVATE, function(){
	print "RootActor.ACTIVATE"
})

root.addEventListener(RootActor.DEACTIVATE, function(){
	print "RootActor.DEACTIVATE"
})

print "root.size: ${root.size}, scale: ${root.scale}, display: ${displaySize}"

MainMenuScene.instance.show()
// GameScene.instance.show()
