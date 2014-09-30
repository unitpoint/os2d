CustomEvent = extends Event {
	__construct = function(type, params){
		if(objectOf(type)){
			params && throw "2rd argument should be null here"
			if("type" in type){
				params = type
				type = params.type; delete params.type
			}else{
				type, params = type.shift(), type
			}
		}
		// @type = type
		@setProperty("type", type)
		@merge(params)
		// print "CustomEvent: ${this}"
	},
	
	/* __get@type = function(){
		return @_type
	},
	__set@type = function(value){
		@_type = value
	}, */
}
