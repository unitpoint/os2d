InstantAction = extends FiniteTimeAction {
    __get@isDone = function(){
		return true
	},
	
    step = function(dt){
		@update(1)
	},
	
	update = function(dt){
	},
}
