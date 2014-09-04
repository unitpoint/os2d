GameScene = extends Scene {
	__construct = function(){
		super()
		@loadConfig()
		@nextLevel(@config.levelNum)
		
		if(DEBUG || true){
			var btnNext = Sprite().attrs {
				resAnim = res.getResAnim("hud-button-restart"),
				parent = this,
				pivot = vec2(1, 0),
				pos = vec2(@width, 0),
				priority = 1,
			}
			btnNext.addEventListener(TouchEvent.CLICK, function(){
				@nextLevel(@level.levelNum + 1)
			}.bind(this))
		}
		
		// debug, check mem leaks
		false && @addTween(UpdateTween(1000/100, function(){
			@nextLevel(@level.levelNum + 1)
		}.bind(this)))
	},
	
	configFilename = "config.json",
	loadConfig = function(){
		return @config = File.exists(@configFilename) && json.decode(File.readContents(@configFilename)) || {levelNum = 1}
	},
	
	saveConfig = function(){
		File.writeContents(@configFilename, json.encode(@config))
	},
	
	level = null,
	nextLevel = function(levelNum, exception){
		print "nextLevel: ${levelNum}"
		if(@config.levelNum !== levelNum){
			@config.levelNum = levelNum
			@saveConfig()
		}
		try{
			var LevelClass = _G["Level_${levelNum}"]
		}catch(e){
			// workaround now
			e is CompilerException && throw e;
			exception && throw exception
			return @nextLevel(1, e)
		}
		// @level.parent = null
		@level.hide()
		@level = LevelClass(this).attrs { opacity = 0 }
		@level.show(this)
	},
	
	update = function(ev){
	
	},
}
