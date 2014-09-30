/** 
@brief Base class for Action objects.
 */
Action = extends Object {
	__object = {
		_target = null,
		_name = "anonymous",
		detachTarget = false,
		doneCallback = null,
	},
	
	__construct = function(params){
		super()
		if(params.prototype === Object){
			@attrs params
		}else if(params){
			throw "error argument ${params}"
		}
	},
	
    __get@target = function(){ return @_target },
    /** The action will modify the target properties. */
    __set@target = function(target){ @_target = target },
    
    __get@name = function(){ return @_name },
    __set@name = function(name){ @_name = name },
    
	/** return true if the action has finished */
	__get@isDone = function(){ throw "${@classname}.${_F.__name} is not implemented" },
	
	/** returns a clone of action */
	clone = function(){ throw "${@classname}.${_F.__name} is not implemented" },
	
	/** returns a new action that performs the exactly the reverse action */
	reverse = function(){ throw "${@classname}.${_F.__name} is not implemented" },
	
	/** called before the action start. It will also set the target. */
	start = function(){
	},
    
	/** 
    called after the action has finished. It will set the 'target' to nil.
    IMPORTANT: You should never call "[action stop]" manually. Instead, use: "target->stopAction(action);"
    */
	stop = function(){
		// @target = null
	},
	
	/** called every frame with it's delta time. DON'T override unless you know what you are doing. */
	step = function(dt){ throw "${@classname}.${_F.__name} is not implemented" },
	
    /** 
    called once per frame. time a value between 0 and 1

    For example: 
    - 0 means that the action just started
    - 0.5 means that the action is in the middle
    - 1 means that the action is over
    */
    update = function(t){ throw "${@classname}.${_F.__name} is not implemented" },
}
