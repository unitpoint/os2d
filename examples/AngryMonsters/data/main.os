print "--"
print "[start] ${DateTime.now()}"

GAME_SIZE = vec2(960, 540)

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
bg.scale = math.max(stage.width / bg.width, stage.height / bg.height)

monsterAnimations = { // define monsterAnimations for our monsters
	"monster-01" = {
		generalParams = {
			delay = 0.08,
			rect = {
				center = {x=0.48, y=0.45},
				width = 0.5,
				height = 0.74,
			},
		},
		walkRight 	= { start = 0, frames = 17 },
		walkUp 		= { start = 17, frames = 13 },
		walkDown 	= { start = 30, frames = 12 },
		fightRight 	= { start = 42, frames = 12 },
		dieRight 	= { start = 67, frames = 17 },
	},
	"monster-03" = {
		generalParams = {
			delay = 0.08,
			rect = {
				center = {x=0.46, y=0.48},
				width = 0.7 ,
				height = 0.8,
			},
		},
		walkUp 		= { start = 17, frames = 13 },
		walkRight 	= { start = 0, frames = 17 },
		walkDown 	= { start = 30, frames = 12 },
		fightRight 	= { start = 42, frames = 11 },
		dieRight 	= { start = 70, frames = 11},
	},
}

var monsters = [] // alive monsters array
var monsterNames = monsterAnimations.keys // save keys of animations object, its are filenames

var z = 0.7
var colors = [
	Color(z, z, 1),
	Color(z, 1, z),
	Color(z, 1, 1),
	Color(1, z, z),
	Color(1, z, 1),
	Color(1, 1, z),
	Color(1, 1, 1),
]
function randColor(){
	// return Color(math.random(0.8, 1), math.random(0.8, 1), math.random(0.8, 1))
	return colors[math.random(#colors)]
}

function createMonster(i){ // create new monster
	i = i || math.random(#monsterNames) // determine random monster name
	var monster = Monster(monsterNames[ i % #monsterNames ]).attrs { // create new instance of Monster class
		parent = stage,
		pos = stage.size * vec2(math.random(0.2, 0.8), math.random(0.2, 0.8)),
		color = randColor(), // set random color
		opacity = 0, // set initial opacity to zero because of we want run fade in transition
		touchEnabled = false,
	}
	monster.addTweenAction {
		opacity = 1,
		duration = 1,
	}
	monsters[] = monster // save our monster to the alive monsters array
}

stage.addEventListener("onMonsterDead", function(ev){ // capture onExit event (when monster will be dead)
	var i = monsters.indexOf(ev.monster)
	// print "onMonsterDeleted: #${ev.monster.__id} as ${i}"
	if(i){
		delete monsters[i] // remove it
		createMonster() // create new monster
	} 
})

for(var i = 0; i < 50; i++){ // create initial monsters
	createMonster()
}

function moveMonstersTo(pos){ // move monsters to the touched position
	for(var i, monster in monsters){
		monster.fighting || monster.moveTo(pos)
	}
}

stage.addEventListener(TouchEvent.CLICK, function(ev){
	moveMonstersTo(ev.localPosition)
})

stage.addUpdate(0.3, function(){ // set new timeout function to check monsters crossed
	for(var i, monster in monsters){
		if(monster.fighting) continue
		for(var j = i+1; j < #monsters; j++){
			var other = monsters[j]
			var dist = #(monster.pos - other.pos)
			if(dist < monster.width){ // are monsters close to each other to fight
				monster.fight(other)
				break;
			}
		}
	}
})
