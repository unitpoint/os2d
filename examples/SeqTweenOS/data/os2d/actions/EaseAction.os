/** 
 @brief Base class for Easing actions
 @ingroup Actions
 */
EaseAction = extends IntervalAction {
	__object = {
		_innerAction = null,
		_easeType = Ease.LINEAR,
	},

	__construct = function(action, easeType){
		if(action.prototype === Object){
			super(action)
		}else{
			super()
			@_innerAction = action
			@_easeType = easeType
			@duration = @_innerAction.duration
		}
	},

	__get@innerAction = function(){ return @_innerAction },
    __set@innerAction = function(action){
		if(action !== @_innerAction){
			@_innerAction = action as IntervalAction || throw "IntervalAction required"
			@_innerAction.target = target
		}
	},
	
    __get@easeType = function(){ return @_easeType },
    __set@easeType = function(easeType){ @_easeType = easeType },
	
	__set@target = function(target){
		if(@_target !== target){
			super(target)
			@duration = @_innerAction.duration
			@_innerAction.target = target
		}
	},

    /*
	__get@elapsed = function(){ return @_innerAction.elapsed },
	
	__get@duration = function(){ return @_innerAction.duration },
    __set@duration = function(duration){ @_innerAction.duration = duration },
	*/
	
    //
    // Overrides
    //
	clone = function(){
		return EaseAction {
			innerAction = @_innerAction.clone(),
			easeType = @_easeType,
		}
	},
	
    reverse = function(){
		return EaseAction {
			innerAction = @_innerAction.clone(),
			easeType = Ease.getReverseType(@_easeType),
		}
	},

	__set@target = function(target){
		super(target)
		@_innerAction.target = target
	},
	
    start = function(){
		super()
		@_innerAction.start()
	},
	
    stop = function(){
		@_innerAction.stop()
		super()
	},
	
    update = function(t){
		@_innerAction.update(Ease.run(t, @_easeType))
	},
}
