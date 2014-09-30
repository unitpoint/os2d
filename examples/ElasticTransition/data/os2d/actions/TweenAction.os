TweenAction = extends IntervalAction {
	
	__construct = function(duration, prop, from, to){
		super()
		if(duration.prototype === Object){
			prop = duration
			@duration = prop.duration || throw "duration required"; delete prop.duration
			@name = prop.name; delete prop.name
			@detachTarget = prop.detachTarget; delete prop.detachTarget
			@doneCallback = prop.doneCallback; delete prop.doneCallback
			@tickCallback = prop.tickCallback; delete prop.tickCallback
		}else{
			@duration = duration
		}
		if(prop.prototype === Object){
			@_props = {}
			var defEase = prop.ease; delete prop.ease
			for(var prop, values in prop){
				if(values.prototype === Object){
					@_props[prop] = {
						from = values.from,
						to = values.to,
						fixRotation = values.fixRotation,
						ease = values.ease || defEase,
					}
				}else{
					@_props[prop] = {
						from = null,
						to = values,
						ease = defEase,
					}
				}
			}
		}else{
			if(!to){
				from, to = null, from
			}
			@_props = {
				[prop] = {
					from = from,
					to = to,
				}
			}
		}
		// print "TweenAction created: ${this}"
	},

    //
    // Overrides
    //
    clone = function(){
		var props = {
			doneCallback = @doneCallback,
			tickCallback = @tickCallback,
		}
		for(var prop, values in @_props){
			props[prop] = {
				from = values.from, to = values.to, 
				fixRotation = values.fixRotation,
				ease = values.ease,
			}
		}
		return TweenAction(@_duration, props)
	},
	
	reverse = function(){
		var props = {
			doneCallback = @doneCallback,
			tickCallback = @tickCallback,
		}
		for(var prop, values in @_props){
			props[prop] = {
				from = values.to, to = values.from, 
				fixRotation = values.fixRotation,
				ease = values.ease,
			}
		}
		return TweenAction(@_duration, props)
	},
	
    start = function(){
		super()
		for(var prop, values in @_props){
			values.delta = (values.to || /*values.to =*/ @_target[prop]) 
				- (values.from || /*values.from =*/ @_target[prop])
			if(prop == "angle" && values.fixRotation){
				var diff = values.delta
				if(math.abs(diff) > 180){
					if(diff < 0){
						diff = 360 + diff - math.floor(diff / 360) * 360
					}else if(diff > 360){
						diff = diff - math.floor(diff / 360) * 360
					}
					if(diff > 180){
						diff = diff - 360
					}
					values.delta = diff
				}
			}
		}
	},
	
    update = function(t){
		for(var prop, values in @_props){
			var time = values.ease ? Ease.run(t, values.ease) : t
			@_target[prop] = values.to - values.delta * (1 - time)
			@tickCallback()
		}
	},
}
