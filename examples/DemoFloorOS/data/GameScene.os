GameScene = extends Scene {
	__construct = function(){
		super()
		@loadConfig()
		@nextLevel(@config.levelNum)
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
		@level = _G["Level_${levelNum}"](this).attrs {
			parent = this
		}
	},
	
	update = function(ev){
	
	},
}
