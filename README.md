# OS2D is cross platform engine for 2d mobile games

**OS2D** is cross platform engine for 2d mobile games made with ObjectScript and Oxygine.

![OS2D is cross platform engine for 2d games](https://raw.githubusercontent.com/unitpoint/os2d/master/preview.jpg)

## Build and run

### Windows

Open and run <code>examples\Demo\proj.win32\Demo_vs2010.sln</code>. Please see more examples.

### Android

Run <code>examples\Demo\proj.android\build-run.bat</code>

## Getting Started

The main ObjectScript file is <code>Demo\data\main.os</code> of Demo example. 

### Examples of ObjectScript code:

	// create sprite and display it
	var sprite = Sprite()
	sprite.resAnim = res.getResAnim("anim")
	sprite.parent = stage
	sprite.scale = 0.5
	
	// run opacity and position tween using one action (see SeqTween example)
	sprite.addTweenAction {
		duration = 1.0, // in seconds
		opacity = 0.5,
		pos = vec2(100, 120),
		ease = Ease.BACK_IN_OUT // ease type, it's linear by default
	}
	
	// run forever rotate action (see SeqTween example)
	// rotate sprite in 2 seconds and repeat it forever
	myAction = sprite.addAction(RepeatForeverAction(TweenAction{
		duration = 2, 
		angle = 360,
		name = "rotate-action"
	}))
	
	// remove action
	sprite.removeAction(myAction)
	
	// remove actions by name
	sprite.removeActionsByName("rotate-action")
		
	// handle events
	sprite.addEventListener(TouchEvent.CLICK, function(ev){  
		// your code here
	})

	// handle user event
	sprite.addEventListener("hide", function(ev){  
		// your code here
	})
	
	// dispatch user events
	sprite.dispatchEvent{"hide", prop1 = "prop1", prop2 = 100}
	
	// short syntax of initialization
	// you could any properties you want
	sprite = Sprite().attrs {
		resAnim = res.getResAnim("plus"),
		scale = 0.5f,
		pos = vec2(70, 55),
		parent = stage,
	}
	
	// Actor is base class that could be rendered
	// use it as group of actors, sprites and so on
	actor = Actor().attachTo(stage)
	
	// add update function that will be called each 0.3 seconds
	// (see AngryMonsters examle)
	myUpdate = actor.addUpdate(0.3, function(){
		// your code here
	})

	// remove update function
	actor.removeUpdate(myUpdate)

	// add update function of each frame
	actor.addUpdate(function(){
		// your code here
	})
	
	// add timeout function to call in 2.5 seconds
	// (see DemoFloor example)
	actor.addTimeout(2.5, function(){
		// your code here
	})
	
	
## Examples

### SeqTween

[![Opensource SeqTween example made with OS2D](https://raw.githubusercontent.com/unitpoint/os2d-bin-win/master/SeqTween/SeqTween-youtube.jpg)](http://www.youtube.com/watch?v=p5zzlDh7DuI)

### DemoFloor

[![Opensource DemoFloor example made with OS2D, level 2](https://raw.githubusercontent.com/unitpoint/os2d-bin-win/master/DemoFloor/DemoFloor-youtube.jpg)](http://www.youtube.com/watch?v=rJRRq-x2uBI)

### AngryMonsters

[![Opensource AngryMonsters example made with OS2D](https://raw.githubusercontent.com/unitpoint/os2d-bin-win/master/AngryMonsters/AngryMonsters-youtube.jpg)](http://www.youtube.com/watch?v=vlrua_emfPM)

### Demo

[![Opensource Demo example made with OS2D](https://raw.githubusercontent.com/unitpoint/os2d-bin-win/master/Demo/Demo-youtube.jpg)](http://www.youtube.com/watch?v=w8IdHx2uq0c)

### ElasticTransition

![Opensource ElasticTransition example made with OS2D](https://raw.githubusercontent.com/unitpoint/os2d-bin-win/master/ElasticTransition/ElasticTransition.jpg)

### CastleMonster

[![Opensource CastleMonster example made with OS2D](https://raw.githubusercontent.com/unitpoint/CastleMonster-bin-win/master/CastleMonster-youtube.jpg)](http://www.youtube.com/watch?v=a4afnUWjVR4)

## Resources

* [OS2D](https://github.com/unitpoint/os2d) is cross platform engine for 2d mobile games (examples included) made with ObjectScript and Oxygine
* [OS2D binaries](https://github.com/unitpoint/os2d) is OS2D binaries and executables examples for Windows
* [CastleMonster](https://github.com/unitpoint/CastleMonster) is opensource old school battle mobile game made with OS2D
* [CastleMonster binaries](https://github.com/unitpoint/CastleMonster-bin-win) is CastleMonster binaries and executables files for Windows
* [ObjectScript](https://github.com/unitpoint/objectscript) is ObjectScript language repository of full opensource code
* [ObjectScript binaries](https://github.com/unitpoint/objectscript-bin-win) is ObjectScript binaries and executables files for Windows (os.exe & os-fcgi.exe included)
* [objectscript.org](http://objectscript.org) is ObjectScript site
* [objectscript.org repo](https://github.com/unitpoint/objectscript.org) is ObjectScript site opensource code repository
* [Programming in ObjectScript](https://github.com/unitpoint/objectscript/wiki/Programming-in-ObjectScript)
* [ObjectScript Reference](https://github.com/unitpoint/objectscript/wiki/ObjectScript-Reference)
* [ObjectScript C++ binder generator](https://github.com/unitpoint/objectscript/blob/master/src/os-binder-generator.os)


##Contacts

Please feel free to contact me at anytime, my email is evgeniy.golovin@unitpoint.ru, skype: egolovin