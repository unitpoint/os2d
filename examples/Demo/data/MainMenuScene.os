MainMenuScene = extends Scene {
	__construct = function(){
		super()
		var sky = Sprite().attrs {
			resAnim = res.getResAnim("bg"),
			parent = @view,
		}		
		var btn = MyButton().attrs {
			name = "play",
			resAnim = res.getResAnim("play"),		
			pivot = vec2(0.5f, 0.5f),
			pos = @view.size / 2,
			parent = @view,	
		}
		var self = this
		btn.addEventListener(TouchEvent.CLICK, {||
			!@transition && self.changeScene(GameScene.instance)
		})
	},
}

MainMenuScene.instance = MainMenuScene()
