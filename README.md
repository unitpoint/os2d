# OS2D is ObjectScript engine for 2d mobile games

**OS2D** is ObjectScript engine for 2d mobile games. It's made on top of Oxygine framework.
Best Regards to Denis Muratshin (author of the Oxygine framework).

## Build and run

### Windows

Open and run <code>examples\DemoOS\proj.win32\Demo_vs2010.sln</code>. Please see more examples.

### Android

Run <code>examples\DemoOS\proj.android\build-run.bat</code>

## Getting Started

The main ObjectScript file is <code>DemoOS\data\main.os</code> for DemoOS project. 

### Examples of ObjectScript code:

	// include std library to autoload user's classes and some other functions
	require "std.os"

	// create sprite and display it
	var sprite = Sprite()
	sprite.resAnim = res.getResAnim("anim")
	sprite.parent = stage
	sprite.scale = 0.5
	
	// run animation tween
	sprite.addTween("resAnim", res.getResAnim("anim2"), duration, loops)
	
	// run position tween
	sprite.addTween("pos", vec2(x, y), duration, loops)
	
	// run alpha tween
	sprite.addTween("alpha", 0.5, duration, loops)
	
	// handle events
	sprite.addEventListener(TouchEvent.CLICK, function(ev){  
		// here is your code
		// ...
	})

	// handle user event
	sprite.addEventListener("hide", function(ev){  
		// here is your code
		// ...
	})
	
	// dispatch user events
	sprite.dispatchEvent{"hide", prop1 = "prop1", prop2 = 100}
	
	// short syntax of initialization
	sprite = Sprite().attrs {
		resAnim = res.getResAnim("plus"),
		scale = 0.5f,
		pos = vec2(70, 55),
		parent = stage,
	}
	
## Examples

### SeqTween

![Opensource SeqTween example made with OS2D](https://raw.githubusercontent.com/unitpoint/os2d-bin-win/master/SeqTween/SeqTween.jpg)

### DemoFloor

Click next youtube video to view the DemoFloor:

[![Opensource DemoFloor example made with OS2D, level 2](https://raw.githubusercontent.com/unitpoint/os2d-bin-win/master/DemoFloor/DemoFloor.jpg)](http://www.youtube.com/watch?v=rJRRq-x2uBI)

### Demo

Click next youtube video to view the Demo:

[![Opensource Demo example made with OS2D](https://raw.githubusercontent.com/unitpoint/os2d-bin-win/master/Demo/Demo.jpg)](http://www.youtube.com/watch?v=w8IdHx2uq0c)

### ElasticTransition

![Opensource ElasticTransition example made with OS2D](https://raw.githubusercontent.com/unitpoint/os2d-bin-win/master/ElasticTransition/ElasticTransition.jpg)

## Resources

* [Windows compiled examples ready to run](https://github.com/unitpoint/os2d-bin-win)
* [CastleMonster made with OS2D](https://github.com/unitpoint/CastleMonster) is one more opensource example
* [objectscript.org](https://github.com/unitpoint/objectscript.org) is ObjectScript language site
* [Programming in ObjectScript](https://github.com/unitpoint/objectscript/wiki/Programming-in-ObjectScript)
* [ObjectScript Reference](https://github.com/unitpoint/objectscript/wiki/ObjectScript-Reference)
	
##Contacts
Please feel free to contact me at anytime if further information is needed.
My email is evgeniy.golovin@unitpoint.ru, skype: egolovin