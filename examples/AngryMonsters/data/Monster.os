Monster = extends Sprite {
	__construct = function(name){
		super()
		@name = name
		@resAnim = res.get(name)
		@monsterAnimations = monsterAnimations[name] || throw "monster ${name} is not found"
		@pivot = @monsterAnimations.generalParams.rect.center
		@width = @width * @monsterAnimations.generalParams.rect.width
		@height = @height * @monsterAnimations.generalParams.rect.height
		@fighting = null
		@animName = null
		@animUpdateHandle = null
		@prevPos = @pos
		@health = 100
		
		@trackMoveHandle = @addUpdate(0.1, @trackMove.bind(this))
		
		@playAnim("walkRight")
	},
	
	playAnim = function(name, callback){
		@animName === name && return;
		@animName = name
		var start = @monsterAnimations[name].start
		var count = @monsterAnimations[name].frames
		var animNum = 0
		@resAnimFrameNum = start
		@animUpdateHandle && @removeUpdate(@animUpdateHandle)
		var delay = @monsterAnimations.generalParams.delay * math.random(0.8, 1.2)
		@animUpdateHandle = @addUpdate(delay, function(){
			animNum = (animNum + 1) % count
			@resAnimFrameNum = start + animNum
			if(callback && animNum == count-1){
				callback()
			}
		})
	},
	stopAnim = function(){
		@animUpdateHandle && @removeUpdate(@animUpdateHandle)
		@animUpdateHandle = null
		@animName = null
	},

	trackMove = function(){ // determine the direction of moving to switch animation
		// print "set priority: ${@y + @height * (1 - @pivot.y)}"
		if(/*!@animUpdateHandle ||*/ @fighting) return; // skip if no animation or monster is fighting
		var dir = (@pos - @prevPos).normalize()
		@prevPos = @pos
		if(dir.y < -0.85 && "walkUp" in @monsterAnimations){ // monster goes up
			@playAnim("walkUp")
			@scaleX = 1
		}else if(dir.y > 0.85 && "walkDown" in @monsterAnimations){ // monster goes down
			@playAnim("walkDown")
			@scaleX = 1
		}else{ // monster goes left or right
			@playAnim("walkRight");
			@scaleX = dir.x < 0 ? -1 : 1
		}
		@priority = @y + @height/2
	},
	
	moveTo = function(pos, doneCallback){ // move monster to pos
		var len = #(@pos - pos)
		@replaceTweenAction {
			name = "moveAction",
			pos = pos,
			duration = len * 20.0 / stage.width,
			doneCallback = doneCallback
		}
	},
	
	fight = function(other){ // try to fight with other monster
		if(@fighting || other.fighting) return; // skip if monster is already fighting
		var side = @x < other.x ? -1 : 1
		var center = (@pos + other.pos) / 2
		var dest = vec2(center.x + @width*0.4*side, center.y)
		@moveTo(dest, @startFightingAnimation.bind(this)) // move monster to fighting position
		@fighting = other
		
		dest = vec2(center.x - other.width*0.4*side, center.y)
		other.moveTo(dest, other.startFightingAnimation.bind(other)) // move other monster to fighting position
		other.fighting = this
	},
	
	startFightingAnimation = function(){ // start fighting
		@health > 0 || return; // skip if monster is dead
		@scaleX = @x > @fighting.x ? -1 : 1 // determine side of monster
		@playAnim("fightRight", function(){ // run fighting animations
			@fighting.health = @fighting.health - @health * 0.3 // make damage to other monster
			if(@fighting.health <= 0){ // check if other monster is dead
				@fighting.die() // start die animations
				@fighting = null
				// go away a bit when the fight is finished
				var x = @x + math.random(-1, 1) * @width
				var y = @y + math.random(-1, 1) * @width
				@moveTo(vec2(x, y))
				@playAnim("walkRight")
			}
		})
	},
	
	die = function(){ // start dying animations
		@removeUpdate(@trackMoveHandle) // remove @trackMove update functions
		@removeActionsByName("moveAction") // stop moving animations
		@scaleX = @x > @fighting.x ? -1 : 1 // determine side of monster
		@playAnim("dieRight", function(){ // run dying animations
			@stopAnim()
			@addTweenAction { // start fade out transition
				opacity = 0,
				duration = 1,
				doneCallback = function(){
					@detach()
					stage.dispatchEvent{"onMonsterDead", monster = this}
				},
			}
		})
	},
}
