/******************************************************************************
* Copyright (C) 2014 Evgeniy Golovin (evgeniy.golovin@unitpoint.ru)
*
* Please feel free to contact me at anytime, 
* my email is evgeniy.golovin@unitpoint.ru, skype: egolovin
*
* Latest source code: https://github.com/unitpoint/os2d
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

require.paths[] = __DIR__.."/actions"

var filesChecked = {}
function __get(name){
	if(!(name in filesChecked)){
		filesChecked[name] = true
		require(name, false)
		if(name in this){
			return this[name]
		}
	}
	throw "unknown class or global property \"${name}\""
}

function _T(text){
	return text
}

function Object.__get(name){
	if(@prototype !== Object || @getProperty("__instantiable", false) !== null){
		// throw "unknown \"${name}\" property in ${@__name || @classname}"
		throw "unknown \"${name}\" property in ${@__name} <${typeOf(this)}#${@__id}:classname=${@classname}>"
	}
	return null
}

function printBackTrace(skipFuncs){
	for(var i, t in arrayOf(skipFuncs) || debugBackTrace((skipFuncs || 0) + 1)){ // skip printBackTrace
		printf("#${i} ${t.file}%s: %s, args: ${t.arguments}\n",
			t.line > 0 ? "(${t.line},${t.pos})" : "",
			t.object && t.object !== _G ? "{${typeOf(t.object)}#${t.object.__id}}.${t.func.__name}" : t.func.__name)
	}
}

function assert(a, message){
	return a || throw(message || "assert failed")
}

function eval(str){
	return compileText(str).apply(null, ...)
}

function evalEnv(str, env){
	return compileText(str).applyEnv(env || _G, null, ...)
}

function delegate(self, func){
	var args = ...
	if(#args == 0){
		return function(){ return func.apply(self, arguments) }
	}
	return function(){ 
		if(#arguments == 0){
			return func.apply(self, args)
		}
		return func.apply(self, [].merge(args, arguments))
	}
}

function Function.bind(self){
	var args = ...
	if(#args == 0){
		return delegate(self, this)
	}
	return delegate.apply(_E, [self, this].merge(args))
}

function toArray(a){
	arrayOf(a) && return a;
	var type = typeOf(a)
	if(type == "object"){
		var arr = []
		for(var i, v in a){
			arr.push(v)
		}
		return arr
	}
	if(type == "null"){
		return []
	}
	return [a]
}

function toObject(a){
	objectOf(a) && return a;
	var type = typeOf(a)
	if(type == "array"){
		var object = {}
		for(var i, v in a){
			object.push(v)
		}
		return object
	}
	if(type == "null"){
		return {}
	}
	return {a}
}

function File.readContents(filename)
{
	var f = File(filename, "rb")
	var content = f.read()
	f.close()
	return content
}

function File.writeContents(filename, content)
{
	var f = File(filename, "wb")
	content !== null && f.write(content)
	f.close()
}

Buffer.__lshift = Buffer.append	// make alias to << operator

function Object.do(func){
	return func() || this
}

function Object.deepClone(){
	var recurs = {}
	var clone = function(value){
		recurs[value] && return value
		recurs[value] = true
		var copy = value.clone()
		for(var k, v in copy){
			copy[k] = clone(v)
		}
		return copy
	}
	return clone(this)
}

function Object.flip(){
	var r = {}
	for(var k, v in this){
		r[v] = k
	}
	return r
}

function Object.reverse(){
	var r = {}
	for(var k, v in @reverseIter()){
		r[k] = v
	}
	return r
}

function Array.reverse(){
	var r = []
	for(var k, v in @reverseIter()){
		r[] = v
	}
	return r
}

function Object.map(func){
	var r = {}
	for(var k, v in this){
		r[k] = func(v, k, this)
	}
	return r
}

function Array.map(func){
	var r = []
	for(var k, v in this){
		r[] = func(v, k, this)
	}
	return r
}

function Object.filter(func){
	var r = {}
	for(var k, v in this){
		func(v, k, this) && r[k] = v
	}
	return r
}

function Array.filter(func){
	var r = []
	for(var k, v in this){
		func(v, k, this) && r[] = v
	}
	return r
}

function Object.each(func){
	for(var k, v in this){
		return func(v, k, this) || continue
	}
}

function Object.reduce(func, value){
	for(var k, v in this){
		value = func(value, v, k, this)
	}
	return value
}

function Object.reduceRight(func, value){
	for(var k, v in @reverseIter()){
		value = func(value, v, k, this)
	}
	return value
}

function Object.merge(){
	for(var _, arg in arguments){
		if(arrayOf(arg)){
			for(var k, v in arg){
				this[] = v
			}
		}else{
			for(var k, v in arg){
				this[k] = v
			}
		}
	}
	return this
}

function Array.merge(){
	for(var _, param in arguments){
		for(var k, v in param){
			this[] = v
		}
	}
	return this
}

DateTime || throw "DateTime required"

appStartTime = DateTime.now()

function DateTime.__add(b){
	return DateTime {
		comdate = @comdate + (numberOf(b) || throw "DateTime.__add requires Number")
	}
}

function DateTime.__sub(b){
	b is DateTime && return @comdate - b.comdate
	return DateTime {
		comdate = @comdate - (numberOf(b) || throw "DateTime.__sub requires DateTime or Number")
	}
}

function DateTime.__cmp(b){
	b is DateTime || throw "DateTime.__cmp requires DateTime"
	return @comdate <=> b.comdate
}

var shutdownFunctions, cleanupFunctions = {}, {}

function registerShutdownFunction(func){
	shutdownFunctions[functionOf(func) || throw "function required"] = true
}

function unregisterShutdownFunction(func){
	delete shutdownFunctions[func]
}

function triggerShutdownFunctions(){
	var funcs = shutdownFunctions
	shutdownFunctions = {}
	for(var func, _ in funcs.reverseIter()){
		func()
	}
}

function registerCleanupFunction(func){
	cleanupFunctions[functionOf(func) || throw "function required"] = true
}

function unregisterCleanupFunction(func){
	delete cleanupFunctions[func]
}

function triggerCleanupFunctions(){
	var funcs = cleanupFunctions
	cleanupFunctions = {}
	for(var func, _ in funcs.reverseIter()){
		func()
	}
}

function String.reverse(){
	var buf = Buffer()
	for(var i = #this-1; i >= 0; i--){
		buf.append(@sub(i, 1))
	}
	return toString(buf)
}

function String.reverseAnsi(){
	var buf = Buffer()
	for(var i = @lenAnsi()-1; i >= 0; i--){
		buf.append(@subAnsi(i, 1))
	}
	return toString(buf)
}

function String.reverseUtf8(){
	var buf = Buffer()
	for(var i = @lenUtf8()-1; i >= 0; i--){
		buf.append(@subUtf8(i, 1))
	}
	return toString(buf)
}

function String.reverseIconv(){
	var buf = Buffer()
	for(var i = @lenIconv()-1; i >= 0; i--){
		buf.append(@subIconv(i, 1))
	}
	return toString(buf)
}

function String.__mul(count){
	count < 0 && return this.reverse() * -count
	count == 1 && return this
	var buf = Buffer()
	for(; count >= 1; count--){
		buf.append(this)
	}
	if(count > 0){
		buf.append(@sub(0, #this * count + 0.5))
	}
	return toString(buf)
}

function String.__div(count){
	return this * (1 / count)
}

/*
function String.__add(b){
	return this .. b
}

function String.__radd(b){
	return b .. this
}
*/

function String.flower(){
	return @sub(0, 1).upper() .. @sub(1)
}

function String.switchToAnsi(){
	String.__len = String.lenAnsi
	String.find = String.findAnsi
	String.sub = String.subAnsi
}
	
function String.switchToUtf8(){
	String.__len = String.lenUtf8
	String.find = String.findUtf8
	String.sub = String.subUtf8
}
	
function String.switchToIconv(){
	String.__len = String.lenIconv
	String.find = String.findIconv
	String.sub = String.subIconv
}

function Object.attrs(attrs){
	for(var name, value in attrs){
		this[name] = value
	}
	return this
}

function OS2DObject.__get@_externalCallbacks(){
	@setProperty("_externalCallbacks", {})
	return @_externalCallbacks
}

function OS2DObject._registerExternalCallback(id, func){
	@_externalCallbacks[id] = func
}

function OS2DObject._unregisterExternalCallback(id, func){
	delete @_externalCallbacks[id]
}

function OS2DObject.__get@_externalEventCallbacks(){
	@setProperty("_externalEventCallbacks", {})
	return @_externalEventCallbacks
}

function OS2DObject._registerExternalEventCallback(id, func){
	@_externalEventCallbacks[id] = func
}

function OS2DObject._unregisterExternalEventCallback(id, func){
	delete @_externalEventCallbacks[id]
}

function OS2DObject._unregisterAllExternalEventCallbacks(){
	// @_externalCallbacks = {}
	delete @_externalEventCallbacks
}

function OS2DObject.__get@_externalTweens(){
	@setProperty("_externalTweens", {})
	return @_externalTweens
}

function OS2DObject._registerExternalTween(tween){
	@_externalTweens[tween] = true
}

function OS2DObject._unregisterExternalTween(tween){
	delete @_externalTweens[tween]
}

function OS2DObject._unregisterAllExternalTweens(){
	// @_externalTweens = {}
	delete @_externalTweens
}

/* function Actor.__construct(){
	super()
	@touchEnabled = false
} */

function Actor.__get@_internalActions(){
	@setProperty("_internalActions", {})
	var self = this
	@_actionUpdate = @addUpdate(function(ev){
		var dt = ev.dt // * 0.001
		for(var action in self._internalActions){
			action.step(dt)
			// print "after step: ${action.elapsed}, ${action.duration}, ${action.isDone}"
			if(action.isDone){
				action.detachTarget && action.target.detach()
				self.removeAction(action)
				var doneCallback = action.doneCallback
				doneCallback() // use function's this instead of current object
			}
		}
	})
	@_actionUpdate.name = "actionUpdateFunc"
	return @_internalActions
}

function Actor.addAction(action){
	// print "addAction: ${action.__id}"
	if(action in @_internalActions){
		throw "action is already exist"
	}
	@_internalActions[action] = true
	action.target = this
	action.start()
	return action
}

function Actor.replaceAction(name, action){
	if(name is Action){
		name, action = name.name, name
	}else{
		action.name = name
	}
	@removeActionsByName(name)
	return @addAction(action)
}

function Actor.addTweenAction(duration, prop, from, to){
	return @addAction(TweenAction(duration, prop, from, to))
}

function Actor.replaceTweenAction(name, duration, prop, from, to){
	if(name.prototype === Object){
		var params = name
		@removeActionsByName(params.name)
		return @addTweenAction(params)
	}
	@removeActionsByName(name)
	var action = @addTweenAction(duration, prop, from, to)
	action.name = name
	return action
}

function Actor.removeAction(action){
	// print "removeAction: ${action.__id}"
	if(action in @_internalActions){
		action.target === this || throw "action.target !== this"
		action.stop()
		action.target = null
		delete @_internalActions[action]
		if(#@_internalActions == 0){
			// print "no actions in ${@name || @classname}: #${@__id}"
			@removeUpdate(@_actionUpdate)
			delete @_actionUpdate
			delete @_internalActions
		}
		return
	}
	// throw "action is not exist"
}

function Actor.removeActionsByName(name){
	for(var action in @_internalActions){
		if(action.name == name){
			@removeAction(action)
		}
	}
}

function Actor.addTimeout(delay, func){
	return @addTween(DoneTween(delay, func))
}

function Actor.removeTimeout(t){
	t && @removeTween(t)
}

function Actor.addUpdate(dt, func){
	if(functionOf(dt)){
		dt, func = 0, dt
	}
	return @addTween(UpdateTween(dt, func))
}

function Actor.removeUpdate(t){
	t && @removeTween(t)
}

function OS2DObject.__get@_externalChildren(){
	@setProperty("_externalChildren", {})
	return @_externalChildren
}

function OS2DObject._registerExternalChild(child){
	@_externalChildren[child] = true
}

function OS2DObject._unregisterExternalChild(child){
	delete @_externalChildren[child]
}

function OS2DObject._unregisterAllExternalChildren(){
	delete @_externalChildren
}

function OS2DObject.__len(){
	// return @numChildren()
	return #@_externalChildren
}

function OS2DObject.__get(name){
	if(typeOf(name) === "number"){
		return @getChild(name) || throw "child at index ${name} not exist in ${@__name || @classname}"
	}
	return super(name)
}

function OS2DObject.__set(i, value){
	if(typeOf(i) === "number"){
		throw "attempt to set child at index ${i}, use addChild or attachTo or set parent instead of"
	}
	super(i, value)
}

function OS2DObject.__get@childrenList()
{
	var childrenList = []
	var cur = @firstChild
	while(cur){
		childrenList[] = cur
		cur = cur.nextSibling
	}
	return childrenList
}

function OS2DObject.__iter(){
	var next, i = @firstChild, 0
	return function(){
		var cur = next
		next = next.nextSibling
		// print "iter step: ${!!cur}, ${i}, ${cur}"
		cur && return true, i++, cur
	}
}

function OS2DObject.reverseIter(){
	var next, i = @lastChild, #@_externalChildren-1 // @numChildren-1
	return function(){
		var cur = next
		next = next.prevSibling
		// print "iter step: ${!!cur}, ${i}, ${cur}"
		cur && return true, i--, cur
	}
}

/*	
url || throw "url module required"

function url.buildQuery(p, amp){
	var r = []
	for(var k, v in p){
		r[] = url.encode(k)..'='..url.encode(v)
	}
	return r.join(amp || '&')
}

path || throw "path module required"

function path.normalize(path){
	var r = []
	for(var _, p in (stringOf(path) || throw "string required").split(Regexp("#[/\\\\]#"))){
		if(p == "" || p == "."){
			continue
		}
		if(p == ".."){
			delete r.last
			continue
		}
		r[] = p
	}
	return r.join("/").replace(Regexp("#^\w+:/#s"), "$0/")
}
*/
