print "--"
print "[start] ${DateTime.now()}"

require "std.os"

GAME_SIZE = vec2(540, 960)

var displaySize = root.size
var scale = displaySize / GAME_SIZE
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

GameScene().show()
