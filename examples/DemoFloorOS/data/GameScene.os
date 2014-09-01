GameScene = extends Scene {
	__construct = function(){
		super()
		@loadConfig()
		@nextLevel(@config.levelNum)
		
		/* @addTween(UpdateTween(1000/100, function(){
			@nextLevel(@level.levelNum + 1)
		}.bind(this))) */
	},
	
	configFilename = "config.json",
	loadConfig = function(){
		return @config = File.exists(@configFilename) && json.decode(File.readContents(@configFilename)) || {levelNum = 1}
	},
	
	saveConfig = function(){
		File.writeContents(@configFilename, json.encode(@config))
	},
	
	level = null,
	nextLevel = function(levelNum){
		print "nextLevel: ${levelNum}"
		if(@config.levelNum !== levelNum){
			@config.levelNum = levelNum
			@saveConfig()
		}
		@level.parent = null
		try{
			var LevelClass = _G["Level_${levelNum}"]
		}catch(e){
			// workaround now
			return @nextLevel(1)
		}
		@level = LevelClass(this).attrs {
			parent = this
		}
	},
	
	update = function(ev){
	
	},
}
