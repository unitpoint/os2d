# OS2D is ObjectScript engine for 2d games

**OS2D** is ObjectScript engine for 2d games. **OS2D** is made on top of Oxygine framework.
Best Regards to Denis Muratshin (author of the Oxygine framework).

## Build and run on Windows

### Windows

Open and run __examples\DemoOS\proj.win32\Demo_vs2010.sln__.

### Android

Run __examples\DemoOS\proj.android\build-run.bat__

### Examples

Please see more examples.

## Getting Started

The main ObjectScript file is __DemoOS\data\main.os__ for DemoOS project. 

### Examples of ObjectScript code:

	// include std library to autoload user's classes and some other functions
	require "std.os"

	// create sprite and display it
	var sprite = Sprite()
	sprite.resAnim = res.getResAnim("anim")
	sprite.parent = root
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
		parent = root,
	}

##Contacts
Please feel free to contact me at anytime if further information is needed.
My email is evgeniy.golovin@unitpoint.ru, skype: egolovin