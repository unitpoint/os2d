/** 
@brief 
 Base class actions that do have a finite time duration.
 Possible actions:
   - An action with a duration of 0 seconds
   - An action with a duration of 35.5 seconds

 Infinite time actions are valid
 */
FiniteTimeAction = extends Action {
	__object = {
		_duration = 0,
	},
	
	//! get duration in seconds of the action
    __get@duration = function(){ return @_duration },
	
    //! set duration in seconds of the action
    __set@duration = function(duration){ @_duration = duration },
}
