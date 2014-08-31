#ifndef __OX_BINDER_H__
#ifdef OX_WITH_OBJECTSCRIPT

#include <oxygine-framework.h>

#include <objectscript.h>
#include <os-binder.h>
#include <ext-datetime/os-datetime.h>
#include <string>

namespace oxygine { class EventCallback; class Event; }
void retainOSEventCallback(ObjectScript::OS * os, oxygine::EventCallback * cb);
void releaseOSEventCallback(ObjectScript::OS * os, oxygine::EventCallback * cb);
void callOSEventFunction(ObjectScript::OS * os, int func_id, oxygine::Event * ev);
std::string getOSDebugStr();

using namespace oxygine;

class OxygineOS: public ObjectScript::OS
{
public:

	typedef std::map<EventCallback*, int> EventCallbacks;
	EventCallbacks eventCallbacks;

	OxygineOS()
	{
	}

	void handleErrorPolicyVa(const char *format, va_list va)
	{
		String str = String::formatVa(this, format, va);
		setException(str);
	}

	void retainFromEventCallback(EventCallback * cb)
	{
		// retain();
		EventCallbacks::iterator it = eventCallbacks.find(cb);
		if(it != eventCallbacks.end()){
			OX_ASSERT(false);
			it->second++;
		}else{
			eventCallbacks[cb] = 1;
		}
	}

	void releaseFromEventCallback(EventCallback * cb)
	{
		EventCallbacks::iterator it = eventCallbacks.find(cb);
		if(it != eventCallbacks.end()){
			if(--it->second <= 0){
				eventCallbacks.erase(it);
			}
		}else{
			OX_ASSERT(false);
		}
		// release();
	}

	void resetAllEventCallbacks()
	{
		EventCallbacks::iterator it = eventCallbacks.begin();
		for(; it != eventCallbacks.end();){
			it->first->reset();
			it = eventCallbacks.begin();
		}
	}

	int getNumValues() const
	{
		return core->num_created_values - core->num_destroyed_values;
	}

	virtual bool isFileExist(const OS_CHAR * filename)
	{
		return file::exists(filename);
	}

	/* virtual int getFileSize(const OS_CHAR * filename)
	{
	}

	virtual int getFileSize(FileHandle * f)
	{
	} */
	
	virtual FileHandle * openFile(const OS_CHAR * filename, const OS_CHAR * mode)
	{
		return (FileHandle*)file::open(filename, mode);
	}

	virtual int readFile(void * buf, int size, FileHandle * f)
	{
		if(f){
			return file::read((file::handle)f, buf, (unsigned)size);
		}
		return 0;
	}

	virtual int writeFile(const void * buf, int size, FileHandle * f)
	{
		if(f){
			file::write((file::handle)f, buf, (unsigned)size);
		}
		return size;
	}

	virtual int seekFile(FileHandle * f, int offset, int whence)
	{
		if(f){
			return file::seek((file::handle)f, offset, whence);
		}
		return 0;
	}
	
	virtual void closeFile(FileHandle * f)
	{
		if(f){
			file::close((file::handle)f);
		}
	}

	virtual void echo(const void * buf, int size)
	{
		const char * output_filename = "out.txt";
		FileHandle * f = openFile(output_filename, "a");
		OS_ASSERT(f);
		writeFile((const char*)buf, size, f);
		closeFile(f);
	}
};

// #define OS_USE_OX_USERPOINTER

namespace ObjectScript {

template <class T> struct CtypeOXClass{};
template <class T> struct CtypeOXClass<T*>
{
	typedef typename RemoveConst<T>::type ttype;
	typedef typename RemoveConst<T>::type * type;

	static bool isValid(const type p){ return p != NULL; }
	static type def(ObjectScript::OS*){ return type(); }
	static type getArg(ObjectScript::OS * os, int offs)
	{
		const OS_ClassInfo& classinfo = ttype::getClassInfoStatic();
#ifdef OS_USE_OX_USERPOINTER
		return (type)os->toUserdata(classinfo.instance_id, offs, classinfo.class_id);
#else
		type * p = (type*)os->toUserdata(classinfo.instance_id, offs, classinfo.class_id);
		return p ? *p : NULL;
#endif
	}
	static void push(ObjectScript::OS * os, const type& val)
	{
		if(!val){
			os->pushNull();
			return;
		}
		const OS_ClassInfo& classinfo = val->getClassInfo();
		if(val->osValueId){
			os->pushValueById(val->osValueId);
			OX_ASSERT(!os->isNull());
			OX_ASSERT(os->isUserdata(classinfo.instance_id, -1, classinfo.class_id));
			return;
		}
#ifdef OS_USE_OX_USERPOINTER
		os->pushUserPointer(classinfo.instance_id, val, UserDataDestructor<ttype>::dtor);
#else
		void ** data = (void**)os->pushUserdata(classinfo.instance_id, sizeof(void*), UserDataDestructor<ttype>::dtor);
		OX_ASSERT(data);
		data[0] = val;
#endif
		val->osValueId = os->getValueId();
		val->addRef();
		os->pushStackValue();
		os->getGlobal(classinfo.classname);
		if(!os->isUserdata(classinfo.class_id, -1)){
			OX_ASSERT(false);
			os->pop(2);
		}else{
			os->setPrototype(classinfo.instance_id);
		}
	}
};

template <class T> struct CtypeOXSmartClass
{
	typedef typename T::element_type ttype;
	typedef typename T::element_type * type;

	static bool isValid(const type p){ return p != NULL; }
	static type def(ObjectScript::OS*){ return type(); }
	static type getArg(ObjectScript::OS * os, int offs)
	{
		const OS_ClassInfo& classinfo = ttype::getClassInfoStatic();
#ifdef OS_USE_OX_USERPOINTER
		return (type)os->toUserdata(classinfo.instance_id, offs, classinfo.class_id);
#else
		type * p = (type*)os->toUserdata(classinfo.instance_id, offs, classinfo.class_id);
		return p ? *p : NULL;
#endif
	}
	static void push(ObjectScript::OS * os, const T& val)
	{
		if(!val){
			os->pushNull();
			return;
		}
		const OS_ClassInfo& classinfo = val->getClassInfo();
		if(val->osValueId){
			os->pushValueById(val->osValueId);
			OX_ASSERT(!os->isNull());
			OX_ASSERT(os->isUserdata(classinfo.instance_id, -1, classinfo.class_id));
			return;
		}
#ifdef OS_USE_OX_USERPOINTER
		os->pushUserPointer(classinfo.instance_id, val.get(), UserDataDestructor<ttype>::dtor);
#else
		void ** data = (void**)os->pushUserdata(classinfo.instance_id, sizeof(void*), UserDataDestructor<ttype>::dtor);
		OX_ASSERT(data);
		data[0] = val.get();
#endif
		val->osValueId = os->getValueId();
		val->addRef();
		os->pushStackValue();
		os->getGlobal(classinfo.classname);
		if(!os->isUserdata(classinfo.class_id, -1)){
			OX_ASSERT(false);
			os->pop(2);
		}else{
			os->setPrototype(classinfo.instance_id);
		}
	}
};

static void releaseOXObject(Object * obj)
{
	OX_ASSERT(obj->osValueId);
	obj->osValueId = 0;
	obj->releaseRef();
}

#ifdef OS_USE_OX_USERPOINTER
#define OS_DECL_OX_CLASS_DTOR(type) \
	template <> struct UserObjectDestructor<type>{ static void dtor(type * p){ releaseOXObject(p); } };
#else
#define OS_DECL_OX_CLASS_DTOR(type) \
	template <> struct UserDataDestructor<type>{ \
		static void dtor(ObjectScript::OS * os, void * data, void *){ \
			OX_ASSERT(data); \
			releaseOXObject(*(type**)data); \
		} \
	};
#endif

#define OS_DECL_OX_CLASS(type) \
	template <> struct CtypeName<type> { static const OS_CHAR * getName(){ return type::getClassInfoStatic().classname; } }; \
	template <> struct CtypeName< intrusive_ptr<type> > { static const OS_CHAR * getName(){ return type::getClassInfoStatic().classname; } }; \
	template <> struct CtypeValue<type*>: public CtypeOXClass<type*>{}; \
	template <> struct CtypeValue< intrusive_ptr<type> >: public CtypeOXSmartClass< intrusive_ptr<type> >{}; \
	template <> struct CtypeValue< intrusive_ptr<type>& >: public CtypeOXSmartClass< intrusive_ptr<type> >{}; \
	OS_DECL_OX_CLASS_DTOR(type)

// =====================================================================
#if 0

template <class T> struct CtypeOXStructDestructor
{
	static void dtor(ObjectScript::OS * os, void * data, void *){
		OX_ASSERT(data);
		type * obj = (type*)data;
		obj->~type();
	}
};

template <class T> struct CtypeOXStruct
{
	typedef typename T type;

	static bool isValid(const type& p){ return true; }
	static type def(ObjectScript::OS*){ return type(); }
	static type getArg(ObjectScript::OS * os, int offs)
	{
		type * p = (type*)os->toUserdata(CtypeId<type>::getInstanceId(), offs, CtypeId<type>::getId());
		return p ? *p : NULL;
	}
	static void push(ObjectScript::OS * os, const type& val)
	{
		type * data = (type*)os->pushUserdata(CtypeId<type>::getInstanceId(), sizeof(type), CtypeOXStructDestructor<type>::dtor);
		OX_ASSERT(data);
		data[0] = val;
		os->pushStackValue();
		os->getGlobal(CtypeName<ttype>::getName());
		if(!os->isUserdata(CtypeId<ttype>::getId(), -1)){
			OX_ASSERT(false);
			os->pop(2);
		}else{
			os->setPrototype(CtypeId<ttype>::getInstanceId());
		}
	}
};

#define OS_DECL_OX_STRUCT(type) \
	OS_DECL_CTYPE(type); \
	template <> struct CtypeValue<type>: public CtypeOXStruct<type>{};

#endif // 0
// =====================================================================

template <class T>
struct CtypeEnumNumber
{
	typedef typename RemoveConst<T>::type type;

	static bool isValid(type){ return true; }

	static type def(ObjectScript::OS*){ return type(); }
	static type getArg(ObjectScript::OS * os, int offs)
	{
		return (type)(int)os->toNumber(offs); // enum can't be converted to double so use int convert before
	}

	static void push(ObjectScript::OS * os, const type& val)
	{
		os->pushNumber((OS_NUMBER)val);
	}
};

#define OS_DECL_CTYPE_ENUM(type) \
	OS_DECL_CTYPE(type); \
	template <> struct CtypeValue<type>: public CtypeEnumNumber<type> {}

OS_DECL_CTYPE_ENUM(error_policy);
OS_DECL_CTYPE_ENUM(Tween::EASE);
OS_DECL_CTYPE_ENUM(blend_mode);
OS_DECL_CTYPE_ENUM(Event::Phase);
OS_DECL_CTYPE_ENUM(MouseButton);

// =====================================================================

typedef void(*RegisterFunction)(OS*);
static std::vector<RegisterFunction> registerFunctions;

static bool addRegFunc(RegisterFunction func)
{
	registerFunctions.push_back(func);
	return true;
}

#define DEF_GET(prop, type, func_post) \
	def("__get@" #prop, &type::get ## func_post)

#define DEF_SET(prop, type, func_post) \
	def("__set@" #prop, &type::set ## func_post)

#define DEF_PROP(prop, type, func_post) \
	DEF_GET(prop, type, func_post), \
	DEF_SET(prop, type, func_post)

// =====================================================================

OS_DECL_CTYPE_NAME(Vector2, "vec2");

template <>
struct CtypeValue<Vector2>
{
	typedef Vector2 type;

	static bool isValid(const type&){ return true; }

	static type def(ObjectScript::OS * os){ return type(0, 0); }
	static type getArg(ObjectScript::OS * os, int offs)
	{
		if(os->isObject(offs)){
			float x = (os->getProperty(offs, "x"), os->popFloat());
			float y = (os->getProperty(offs, "y"), os->popFloat());
			return type(x, y);
		}
		OS_NUMBER v;
		if(os->isNumber(offs, &v)){
			return type((float)v, (float)v);
		}
#if 0
		if(os->isArray(offs)){
			os->pushStackValue(offs);
			os->pushNumber(0);
			os->getProperty();
			float x = os->popFloat();
		
			os->pushStackValue(offs);
			os->pushNumber(1);
			os->getProperty();
			float y = os->popFloat();

			return type(x, y);
		}
#endif
		os->setException("vec2 or number required");
		return type(0, 0);
	}

	static void push(ObjectScript::OS * os, const type& p)
	{
#if 1
		os->getGlobal("vec2");
		os->pushGlobals();
		os->pushNumber(p.x);
		os->pushNumber(p.y);
		os->call(2, 1);
		os->handleException();
#else
		os->newObject();
	
		os->pushStackValue();
		os->pushNumber(p.x);
		os->setProperty("x", false);
				
		os->pushStackValue();
		os->pushNumber(p.y);
		os->setProperty("y", false);
#endif
	}
};

// =====================================================================
#if 0

OS_DECL_OX_STRUCT(UpdateState);
static void registerUpdateState(OS * os)
{
	struct Lib {
		UpdateState __newinstance()
		{
			return UpdateState();
		}
	};

	OS::FuncDef funcs[] = {
		def("__newinstance", &Lib::__newinstance),
		{}
	};
	registerUserClass<UpdateState>(os, funcs);
}
static bool __registerUpdateState = addRegFunc(registerUpdateState);

#else

OS_DECL_CTYPE_NAME(UpdateState, "UpdateState");

template <>
struct CtypeValue<UpdateState>
{
	typedef UpdateState type;

	static bool isValid(const type&){ return true; }

	static type def(ObjectScript::OS * os){ return UpdateState(); }
	static type getArg(ObjectScript::OS * os, int offs)
	{
		if(os->isObject(offs)){
			UpdateState us;
			us.time = (os->getProperty(offs, "time"), os->popInt());
			us.dt = (os->getProperty(offs, "dt"), os->popInt());
			us.iteration = (os->getProperty(offs, "iteration"), os->popInt());
			return us;
		}
		os->setException("UpdateState required");
		return type();
	}

	static void push(ObjectScript::OS * os, const type& p)
	{
		os->getGlobal("UpdateState");
		os->pushGlobals();
		os->pushNumber(p.time);
		os->pushNumber(p.dt);
		os->pushNumber(p.iteration);
		os->call(3, 1);
		os->handleException();
	}
};

#endif

// =====================================================================

OS_DECL_CTYPE_NAME(EventCallback, "EventCallback");

template <>
struct CtypeValue<EventCallback>
{
	typedef EventCallback type;

	static bool isValid(const type& val){ return val; }

	static type def(ObjectScript::OS * os){ return type(); }
	static type getArg(ObjectScript::OS * os, int offs)
	{
		if(os->isFunction(offs)){
			return type(os, os->getValueId(offs));
		}
		os->setException("function required");
		return type();
	}

	static void push(ObjectScript::OS * os, const type& p)
	{
		os->pushValueById(p.func_id);
	}
};

// =====================================================================

OS_DECL_CTYPE(Color);

template <>
struct CtypeValue<Color>
{
	typedef Color type;

	static bool isValid(const type&){ return true; }

	static type def(ObjectScript::OS * os){ return type(0, 0, 0, 0); }
	static type getArg(ObjectScript::OS * os, int offs)
	{
		if(os->isObject(offs)){
			unsigned char r = (unsigned char)scalar::clamp((os->getProperty(offs, "r"), os->popFloat(0.0f)) * 255.0f, 0.0f, 255.0f);
			unsigned char g = (unsigned char)scalar::clamp((os->getProperty(offs, "g"), os->popFloat(0.0f)) * 255.0f, 0.0f, 255.0f);
			unsigned char b = (unsigned char)scalar::clamp((os->getProperty(offs, "b"), os->popFloat(0.0f)) * 255.0f, 0.0f, 255.0f);
			unsigned char a = (unsigned char)scalar::clamp((os->getProperty(offs, "a"), os->popFloat(1.0f)) * 255.0f, 0.0f, 255.0f);
			return type(r, g, b, a);
		}
		OS_NUMBER v;
		if(os->isNumber(offs, &v)){
			unsigned char a = (unsigned char)scalar::clamp((float)v * 255.0f, 0.0f, 255.0f);
			return type(a, a, a);
		}
		os->setException("Color or number required");
		return type(0, 0, 0, 0);
	}

	static void push(ObjectScript::OS * os, const type& color)
	{
		os->getGlobal("Color");
		os->pushGlobals();
		os->pushNumber(color.r / 255.0);
		os->pushNumber(color.g / 255.0);
		os->pushNumber(color.b / 255.0);
		os->pushNumber(color.a / 255.0);
		os->call(2, 1);
		os->handleException();
	}
};

// =====================================================================

template <class T>
void registerOXClass(ObjectScript::OS * os, const ObjectScript::OS::FuncDef * list, const ObjectScript::OS::NumberDef * numbers = NULL, bool instantiable = true)
{
	const OS_ClassInfo& classinfo = T::getClassInfoStatic();
	/* OS_ClassInfos::iterator it = classinfos.find(classinfo.classname);
	if(it != classinfos.end()){
		OX_ASSERT(false);
	}
	classinfos.insert(OS_ClassInfos:: classinfo.classname, classinfo);
	*/

	os->pushGlobals();
	os->pushString(classinfo.classname);
	os->pushUserdata(classinfo.class_id, 0, NULL, NULL);
	os->setFuncs(list);
	os->setNumbers(numbers);
	os->pushBool(instantiable);
	os->setProperty(-2, OS_TEXT("__instantiable"), false);
	os->setProperty();
}

template <class T, class Prototype>
void registerOXClass(ObjectScript::OS * os, const ObjectScript::OS::FuncDef * list, const ObjectScript::OS::NumberDef * numbers = NULL, bool instantiable = true)
{
	const OS_ClassInfo& classinfo = T::getClassInfoStatic();
	/* OS_ClassInfos::iterator it = classinfos.find(classinfo.classname);
	if(it != classinfos.end()){
		OX_ASSERT(false);
	}
	classinfos.insert(classinfo.classname, classinfo);
	*/

	os->pushGlobals();
	os->pushString(classinfo.classname);
	os->pushUserdata(classinfo.class_id, 0, NULL, NULL);
	os->setFuncs(list);
	os->setNumbers(numbers);
	os->pushBool(instantiable);
	os->setProperty(-2, OS_TEXT("__instantiable"), false);

	const OS_ClassInfo& classinfo_proto = Prototype::getClassInfoStatic();
	/* it = classinfos.find(classinfo_proto.classname);
	if(it == classinfos.end()){
		OX_ASSERT(false);
	} */

	os->pushStackValue();
	os->getGlobal(classinfo_proto.classname);
	os->setPrototype(classinfo.class_id);
	os->setProperty();
}

// =====================================================================

OS_DECL_OX_CLASS(Object);
static void registerObject(OS * os)
{
	struct Lib {
		static int get(OS * os, int params, int, int, void*)
		{
			os->setException(OS::String::format(os, "property \"%s\" not found in \"%s\"", 
				os->toString(-params+0).toChar(),
				os->getValueNameOrClassname(-params-1).toChar()));
			return 0;
		}

		static int cmp(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(Object*);
			Object * other = CtypeValue<Object*>::getArg(os, -params+0);
			os->pushNumber((intptr_t)self - (intptr_t)other);
			return 1;
		}

		static int getName(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(Object*);
			pushCtypeValue(os, self->getName());
			return 1;
		}

		static int setName(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(Object*);
			self->setName(params > 0 ? os->toString(-params+0).toChar() : "");
			return 0;
		}
	};

	OS::FuncDef funcs[] = {
		{"__get", &Lib::get},
		{"__cmp", &Lib::cmp},
		{"__get@name", &Lib::getName},
		{"__set@name", &Lib::setName},
		{}
	};
	registerOXClass<Object>(os, funcs);
}
static bool __registerObject = addRegFunc(registerObject);

// =====================================================================

OS_DECL_OX_CLASS(Clock);
static void registerClock(OS * os)
{
	struct Lib {
	};

	OS::FuncDef funcs[] = {
		DEF_GET(time, Clock, Time),
		DEF_GET(pauseCounter, Clock, PauseCounter),
		DEF_PROP(fixedStep, Clock, FixedStep),
		DEF_PROP(multiplier, Clock, Multiplier),
		def("pause", &Clock::pause),
		def("resume", &Clock::resume),
		def("resetPause", &Clock::resetPause),
		def("update", &Clock::update),
		def("doTick", &Clock::doTick),
		def("dump", &Clock::dump),
		{}
	};
	registerOXClass<Clock, Object>(os, funcs);
}
static bool __registerClock = addRegFunc(registerClock);

// =====================================================================

OS_DECL_OX_CLASS(Event);
static void registerEvent(OS * os)
{
	struct Lib {
		static Event * __newinstance()
		{
			return new Event(0); // makefourcc('U','N','K','N'));
		}

	};

	OS::FuncDef funcs[] = {
		def("__newinstance", &Lib::__newinstance),
		DEF_GET(type, Event, Type),
		DEF_GET(phase, Event, Phase),
		DEF_PROP(target, Event, Target),
		DEF_PROP(currentTarget, Event, CurrentTarget),
		def("stopPropagation", &Event::stopPropagation),
		def("stopImmediatePropagation", &Event::stopImmediatePropagation),
		{}
	};
	registerOXClass<Event, Object>(os, funcs);
}
static bool __registerEvent = addRegFunc(registerEvent);

// =====================================================================

OS_DECL_OX_CLASS(TouchEvent);
static void registerTouchEvent(OS * os)
{
	struct Lib {
	};

	OS::FuncDef funcs[] = {
		DEF_GET(localPosition, TouchEvent, LocalPosition),
		DEF_GET(position, TouchEvent, Position),
		DEF_GET(pressure, TouchEvent, Pressure),
		DEF_GET(mouseButton, TouchEvent, MouseButton),
		DEF_GET(index, TouchEvent, Index),
		{}
	};
	OS::NumberDef nums[] = {
		{"CLICK", TouchEvent::CLICK},
		{"OVER", TouchEvent::OVER},
		{"OUT", TouchEvent::OUT},
		{"MOVE", TouchEvent::MOVE},
		{"TOUCH_DOWN", TouchEvent::TOUCH_DOWN},
		{"TOUCH_UP", TouchEvent::TOUCH_UP},
		{"WHEEL_UP", TouchEvent::WHEEL_UP},
		{"WHEEL_DOWN", TouchEvent::WHEEL_DOWN},
		{}
	};
	registerOXClass<TouchEvent, Event>(os, funcs, nums);
}
static bool __registerTouchEvent = addRegFunc(registerTouchEvent);

// =====================================================================

OS_DECL_OX_CLASS(TweenEvent);
static void registerTweenEvent(OS * os)
{
	struct Lib {
	};

	OS::FuncDef funcs[] = {
		DEF_GET(actor, TweenEvent, Actor),
		DEF_GET(tween, TweenEvent, Tween),
		DEF_GET(us, TweenEvent, UpdateState),
		{}
	};
	OS::NumberDef nums[] = {
		{"DONE", TweenEvent::DONE},
		{}
	};
	registerOXClass<TweenEvent, Event>(os, funcs, nums);
}
static bool __registerTweenEvent = addRegFunc(registerTweenEvent);

// =====================================================================

OS_DECL_OX_CLASS(EventDispatcher);
static void registerEventDispatcher(OS * os)
{
	struct Lib {
		static EventDispatcher * __newinstance()
		{
			return new EventDispatcher();
		}

		static int addEventListener(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(EventDispatcher*);
			if(params < 2){
				os->setException("two arguments required");
				return 0;
			}
			if(!os->isFunction(-params+1)){
				os->setException("2nd argument must be function");
				return 0;
			}
			int funcId = os->getValueId(-params+1);
			eventType ev;
			switch(os->getType(-params+0)){
			case OS_VALUE_TYPE_NUMBER:
				ev = (eventType)os->toInt(-params+0);
				break;

			case OS_VALUE_TYPE_STRING:
				ev = (eventType)os->toString(-params+0).string->hash; // TODO: change to crc32
				break;

			default:
				os->setException("the first argument should be string or number");
				return 0;
			}
			os->pushNumber(self->addEventListener(ev, EventCallback(os, funcId)));
			return 1;
		}

		static int removeEventListener(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(EventDispatcher*);
			if(params == 1){
				switch(os->getType(-params+0)){
				case OS_VALUE_TYPE_NUMBER:
					self->removeEventListener(os->toInt(-params+0));
					return 0;

				default:
					os->setException("argument should be number here");
					return 0;
				}
			}
			eventType ev;
			switch(os->getType(-params+0)){
			case OS_VALUE_TYPE_NUMBER:
				ev = (eventType)os->toInt(-params+0);
				break;

			case OS_VALUE_TYPE_STRING:
				ev = (eventType)os->toString(-params+0).string->hash; // TODO: change to crc32
				break;

			default:
				os->setException("the first argument should be string or number here");
				return 0;
			}
			if(!os->isFunction(-params+1)){
				os->setException("2nd argument must be function");
				return 0;
			}
			int funcId = os->getValueId(-params+1);
			self->removeEventListener(ev, EventCallback(os, funcId));
			return 0;
		}

		static int dispatchEvent(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(EventDispatcher*);
			if(params < 1){
				os->setException("Event required");
				return 0;
			}
			Event * ev = CtypeValue<Event*>::getArg(os, -params+0);
			if(ev){
				self->dispatchEvent(ev);
				return 0;
			}
			int offs = os->getAbsoluteOffs(-params+0);
			os->getGlobal("CustomEvent");
			os->pushGlobals();
			os->pushStackValue(offs);
			if(params == 1 || os->isObject(offs)){
				os->call(1, 1);
			}else{
				os->pushStackValue(offs+1);
				os->call(2, 1);
			}
			os->handleException();
			ev = CtypeValue<Event*>::getArg(os, -1);
			if(ev){
				os->getProperty(-1, "type");
				switch(os->getType(-1)){
				case OS_VALUE_TYPE_NUMBER:
					ev->type = (eventType)os->toInt(-params+0);
					break;

				case OS_VALUE_TYPE_STRING:
					ev->type = (eventType)os->toString(-params+0).string->hash; // TODO: change to crc32
					break;
				}
				self->dispatchEvent(ev);
			}else{
				OX_ASSERT(false);
			}
			return 0;
		}
	};

	OS::FuncDef funcs[] = {
		def("__newinstance", &Lib::__newinstance),
		DEF_GET(listenersCount, EventDispatcher, ListenersCount),
		{"addEventListener", &Lib::addEventListener},
		{"removeEventListener", &Lib::removeEventListener},
		def("removeAllEventListeners", &EventDispatcher::removeAllEventListeners),
		{"dispatchEvent", &Lib::dispatchEvent},
		{}
	};
	registerOXClass<EventDispatcher, Object>(os, funcs);
}
static bool __registerEventDispatcher = addRegFunc(registerEventDispatcher);

// =====================================================================

OS_DECL_OX_CLASS(Tween);
void registerTween(OS * os)
{
	struct Lib
	{
		/* static Tween * __newinstance()
		{
			return new Tween();
		} */
	};
	OS::FuncDef funcs[] = {
		// def("__newinstance", &Lib::__newinstance),
		DEF_PROP(loops, Tween, Loops),
		DEF_GET(duration, Tween, Duration),
		DEF_PROP(ease, Tween, Ease),
		DEF_PROP(delay, Tween, Delay),
		DEF_PROP(client, Tween, Client),
		DEF_GET(percent, Tween, Percent),
		DEF_PROP(dataObject, Tween, DataObject),
		DEF_GET(nextSibling, Tween, NextSibling),
		DEF_GET(prevSibling, Tween, PrevSibling),
		def("__get@isStarted", &Tween::isStarted),
		def("__get@isDone", &Tween::isDone),
		// {"addDoneCallback", &Lib::addDoneCallback},
		def("addDoneCallback", &Tween::addDoneCallback),
		DEF_PROP(doneCallback, Tween, DoneCallback),
		DEF_SET(detachActor, Tween, DetachActor),
		// virtual void complete(timeMS deltaTime = std::numeric_limits<int>::max()/2);
		// void start(Actor &actor);
		// void update(Actor &actor, const UpdateState &us);	
		// static float calcEase(EASE ease, float v);
		{}
	};
	OS::NumberDef nums[] = {
		{"EASE_LINEAR", Tween::ease_linear},
		{"EASE_INEXPO", Tween::ease_inExpo},
		{"EASE_OUTEXPO", Tween::ease_outExpo},
		{"EASE_INSIN", Tween::ease_inSin},
		{"EASE_OUTSIN", Tween::ease_outSin},
		{"EASE_INCUBIC", Tween::ease_inCubic},
		{"EASE_OUTCUBIC", Tween::ease_outCubic},
		{"EASE_INOUTBACK", Tween::ease_inOutBack},
		{"EASE_INBACK", Tween::ease_inBack},
		{"EASE_OUTBACK", Tween::ease_outBack},
		{}
	};
	registerOXClass<Tween, EventDispatcher>(os, funcs, nums);
}
static bool __registerTween = addRegFunc(registerTween);

// =====================================================================
/*
OS_DECL_OX_CLASS(TweenAnim);
void registerTweenAnim(OS * os)
{
	struct Lib
	{
		static Tween * __newinstance()
		{
			return new Tween();
		}
	};
	OS::FuncDef funcs[] = {
		def("__newinstance", &Lib::__newinstance),
		{}
	};
	OS::NumberDef nums[] = {
		{}
	};
	registerOXClass<TweenAnim, Tween>(os, funcs, nums);
}
static bool __registerTweenAnim = addRegFunc(registerTweenAnim);
*/
// =====================================================================

DECLARE_SMART(UpdateEvent, spUpdateEvent);
class UpdateEvent: public Event
{
public:
	OS_DECLARE_CLASSINFO(UpdateEvent)

	enum {
		UPDATE = makefourcc('U','D','E','V')
	};

	UpdateState us;
	spTween tween;

	UpdateEvent(): Event(UPDATE){}

	const UpdateState& getUpdateState() const { return us; }
	void setUpdateState(const UpdateState& _us){ us = _us; }

	spTween getTween() const { return tween; }
	void setTween(const spTween& t){ tween = t; }
};

OS_DECL_OX_CLASS(UpdateEvent);
static void registerUpdateEvent(OS * os)
{
	struct Lib
	{
		static UpdateEvent * __newinstance()
		{
			return new UpdateEvent();
		}
	};
	OS::FuncDef funcs[] = {
		def("__newinstance", &Lib::__newinstance),
		DEF_PROP(us, UpdateEvent, UpdateState),
		DEF_PROP(tween, UpdateEvent, Tween),
		{}
	};
	OS::NumberDef nums[] = {
		{"UPDATE", UpdateEvent::UPDATE},
		{}
	};
	registerOXClass<UpdateEvent, Event>(os, funcs, nums);
}
static bool __registerUpdateEvent = addRegFunc(registerUpdateEvent);

// =====================================================================

DECLARE_SMART(BaseUpdateTween, spBaseUpdateTween);
class BaseUpdateTween: public Tween
{
public:
	OS_DECLARE_CLASSINFO(BaseUpdateTween);

	EventCallback updateCallback;

	BaseUpdateTween()
	{
		// share same instance for each update (evoid create new OS object each update)
		ev = new UpdateEvent();
		interval = 0;
		curInterval = fixInterval = 0;
	}

	const EventCallback& getUpdateCallback() const { return updateCallback; }
	void setUpdateCallback(const EventCallback& cb){ updateCallback = cb; }

	timeMS getInterval() const { return interval; }
	void setInterval(timeMS _interval){ interval = _interval; curInterval = fixInterval = 0; }

protected:

	spUpdateEvent ev;
	timeMS interval;
	timeMS curInterval;
	timeMS fixInterval;

	virtual void _update(Actor &actor, const UpdateState &us)
	{
		timeMS dt = us.dt;
		if(interval > 0){
			curInterval += us.dt;
			if(curInterval < interval){
				return;
			}
			dt = curInterval - fixInterval;
			OX_ASSERT(us.dt >= 0);
			curInterval -= interval;
			fixInterval = curInterval;
		}

		ev->type = UpdateEvent::UPDATE;
		ev->phase = UpdateEvent::phase_target;
		ev->bubbles = false;
		ev->stopsImmediatePropagation = false;
		ev->stopsPropagation = false;
		ev->userData = NULL;
		ev->userDataObject = NULL;
		ev->target = ev->currentTarget = &actor;
		ev->tween = this;
		ev->us.time = us.time;
		ev->us.dt = dt;
		ev->us.iteration = us.iteration;

		if(updateCallback){
			updateCallback(ev.get());
		}
		dispatchEvent(ev.get());

		// clear refs
		ev->userDataObject = NULL;
		ev->target = ev->currentTarget = NULL;
		ev->tween = NULL;
	}
};

OS_DECL_OX_CLASS(BaseUpdateTween);
static void registerBaseUpdateTween(OS * os)
{
	struct Lib
	{
		static BaseUpdateTween * __newinstance()
		{
			return new BaseUpdateTween();
		}
	};
	OS::FuncDef funcs[] = {
		def("__newinstance", &Lib::__newinstance),
		DEF_PROP(updateCallback, BaseUpdateTween, UpdateCallback),
		DEF_PROP(interval, BaseUpdateTween, Interval),
		{}
	};
	OS::NumberDef nums[] = {
		{}
	};
	registerOXClass<BaseUpdateTween, Tween>(os, funcs, nums);
}
static bool __registerBaseUpdateTween = addRegFunc(registerBaseUpdateTween);

// =====================================================================

DECLARE_SMART(BaseDoneTween, spBaseDoneTween);
class BaseDoneTween: public Tween
{
public:
	OS_DECLARE_CLASSINFO(BaseDoneTween)

	BaseDoneTween(){}

protected:

	void init(Actor &){}
	void update(Actor &, float p, const UpdateState &us){}
};

OS_DECL_OX_CLASS(BaseDoneTween);
static void registerBaseDoneTween(OS * os)
{
	struct Lib
	{
		static BaseDoneTween * __newinstance()
		{
			return new BaseDoneTween();
		}
	};
	OS::FuncDef funcs[] = {
		def("__newinstance", &Lib::__newinstance),
		{}
	};
	OS::NumberDef nums[] = {
		{}
	};
	registerOXClass<BaseDoneTween, Tween>(os, funcs, nums);
}
static bool __registerBaseDoneTween = addRegFunc(registerBaseDoneTween);

// =====================================================================

OS_DECL_OX_CLASS(ResAnim);
static void registerResAnim(OS * os)
{
	OS::FuncDef funcs[] = {
		{}
	};
	OS::NumberDef nums[] = {
		{}
	};
	registerOXClass<ResAnim, Resource>(os, funcs, nums);
}
static bool __registerResAnim = addRegFunc(registerResAnim);

// =====================================================================

#define CASE_OX_TWEEN(prop, type, tween) \
	if(name == prop){ \
		type dest = CtypeValue<type>::getArg(os, -params+1); \
		pushCtypeValue(os, self->addTween(tween(dest), \
			duration, loops, twoSides, delay, ease)); \
		return 1; \
	}

OS_DECL_OX_CLASS(Actor);
static void registerActor(OS * os)
{
	struct Lib { // Actor: public oxygine::Actor
		static Actor * __newinstance()
		{
			return new Actor();
		}

		static int setParent(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(Actor*);
			if(os->isNull(-params+0)){
				self->detach();
				return 0;
			}
			Actor * p = CtypeValue<Actor*>::getArg(os, -params+0);
			if(!p){
				os->setException("Actor required");
				return 0;
			}
			self->attachTo(p);
			return 0;
		}

		static int addTween(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(Actor*);
			if(params < 1){
				os->setException("argument required");
				return 0;
			}
			Tween * tween = CtypeValue<Tween*>::getArg(os, -params+0);
			if(tween){
				pushCtypeValue(os, self->addTween(tween));
				return 1;
			}
			if(params < 3){
				os->setException("3 arguments required");
				return 0;
			}
			OS::String name = os->toString(-params+0);
			timeMS duration = os->toInt(-params+2);
			int loops = params > 3 ? os->toInt(-params+3) : 1;
			bool twoSides = params > 4 ? os->toBool(-params+4) : false;
			timeMS delay = params > 5 ? os->toInt(-params+5) : 0;
			Tween::EASE ease = params > 6 ? (Tween::EASE)os->toInt(-params+6) : Tween::ease_linear;

			/* if(name == "alpha"){
				float dest = os->toFloat(-params+1);
				if(dest < 0) dest = 0; else if(dest > 1) dest = 1;
				pushCtypeValue(os, self->addTween(Actor::TweenAlpha((unsigned char)(dest * 255)), 
					duration, loops, twoSides, delay, ease));
				return 1;
			} */

			CASE_OX_TWEEN("alpha", unsigned char, Actor::TweenAlpha);
			CASE_OX_TWEEN("opacity", unsigned char, Actor::TweenOpacity);
			CASE_OX_TWEEN("pos", Vector2, Actor::TweenPosition);
			CASE_OX_TWEEN("x", float, Actor::TweenX);
			CASE_OX_TWEEN("y", float, Actor::TweenY);
			CASE_OX_TWEEN("width", float, Actor::TweenWidth);
			CASE_OX_TWEEN("height", float, Actor::TweenHeight);
			CASE_OX_TWEEN("rotation", float, Actor::TweenRotation);
			CASE_OX_TWEEN("rotationDegrees", float, Actor::TweenRotationDegrees);
			CASE_OX_TWEEN("angle", float, Actor::TweenRotationDegrees);
			CASE_OX_TWEEN("scale", Vector2, Actor::TweenScale);
			CASE_OX_TWEEN("scaleX", float, Actor::TweenScaleX);
			CASE_OX_TWEEN("scaleY", float, Actor::TweenScaleY);
			CASE_OX_TWEEN("width", float, Actor::TweenWidth);

			VStyleActor * styleActor = dynamic_cast<VStyleActor*>(self);
			if(styleActor){
				CASE_OX_TWEEN("color", Color, VStyleActor::TweenColor);
			}
			Sprite * sprite = dynamic_cast<Sprite*>(self);
			if(sprite){
				CASE_OX_TWEEN("resAnim", ResAnim*, TweenAnim);
			}
			return 0;
		}

		/* static int getAlpha(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(Actor*);
			os->pushNumber((float)self->getAlpha() / 255.0f);
			return 1;
		}

		static int setAlpha(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(Actor*);
			float alpha = os->toFloat(-params+0);
			if(alpha < 0) alpha = 0; else if(alpha > 1) alpha = 1;
			self->setAlpha((unsigned char)(alpha * 255));
			return 0;
		} */
	};

	OS::FuncDef funcs[] = {
		def("__newinstance", &Lib::__newinstance),

		DEF_GET(firstChild, Actor, FirstChild),
		DEF_GET(lastChild, Actor, LastChild),
		DEF_GET(nextSibling, Actor, NextSibling),
		DEF_GET(prevSibling, Actor, PrevSibling),
		def("getDescendant", &Actor::getDescendant),
		def("getChild", &Actor::getChild),
		def("getTween", &Actor::getTween),
		DEF_GET(firstTween, Actor, FirstTween),
		DEF_GET(lastTween, Actor, LastTween),
		
		DEF_GET(anchor, Actor, Anchor),
		def("__set@anchor", (void(Actor::*)(const Vector2 &))&Actor::setAnchor),
		def("__get@isAnchorInPixels", &Actor::getIsAnchorInPixels),

		def("__get@pos", &Actor::getPosition),
		def("__set@pos", (void(Actor::*)(const Vector2 &))&Actor::setPosition),
		DEF_PROP(x, Actor, X),
		DEF_PROP(y, Actor, Y),
		
		DEF_GET(scale, Actor, Scale),
		def("__set@scale", (void(Actor::*)(const Vector2 &))&Actor::setScale),
		DEF_PROP(scaleX, Actor, ScaleX),
		DEF_PROP(scaleY, Actor, ScaleY),

		DEF_PROP(rotation, Actor, Rotation),
		DEF_PROP(rotationDegrees, Actor, RotationDegrees),
		DEF_PROP(angle, Actor, RotationDegrees),

		DEF_PROP(priority, Actor, Priority),
		DEF_PROP(visible, Actor, Visible),
		DEF_PROP(cull, Actor, Cull),

		def("__get@parent", &Actor::getParent),
		// def("__set@parent", (void(Actor::*)(Actor*))&Actor::attachTo),
		{"__set@parent", &Lib::setParent},

		DEF_GET(size, Actor, Size),
		def("__set@size", (void(Actor::*)(const Vector2 &))&Actor::setSize),
		DEF_PROP(width, Actor, Width),
		DEF_PROP(height, Actor, Height),

		DEF_PROP(opacity, Actor, Opacity),
		DEF_PROP(alpha, Actor, Alpha),
		// {"__get@alpha", &Lib::getAlpha},
		// {"__set@alpha", &Lib::setAlpha},

		DEF_PROP(clock, Actor, Clock),

		// virtual RectF		getDestRect() const;

		DEF_PROP(touchEnabled, Actor, TouchEnabled),
		DEF_PROP(touchChildrenEnabled, Actor, TouchChildrenEnabled),
		DEF_PROP(childrenRelative, Actor, ChildrenRelative),

		// const Renderer::transform&		getTransform() const;
		// const Renderer::transform&		getTransformInvert() const;
		// void setTransform(const AffineTransform &tr);

		DEF_PROP(extendedClickArea, Actor, ExtendedClickArea),

		def("isOn", &Actor::isOn),
		def("isDescendant", &Actor::isDescendant),

		def("insertChildBefore", &Actor::insertChildBefore),
		def("insertChildAfter", &Actor::insertChildAfter),
		def("prependChild", (void(Actor::*)(Actor*))&Actor::prependChild),
		def("addChild", (void(Actor::*)(Actor*))&Actor::addChild),
		def("attachTo", (void(Actor::*)(Actor*))&Actor::attachTo),
		def("removeChild", &Actor::removeChild),
		def("removeChildren", &Actor::removeChildren),

		def("detach", &Actor::detach),

		// it's virtual method
		// def("dispatchEvent", &Actor::dispatchEvent),

		DEF_PROP(pressedTouchIndex, Actor, Pressed),
		DEF_PROP(overedTouchIndex, Actor, Overed),

		def("updateState", &Actor::updateState),

		{"addTween", &Lib::addTween},
		def("removeTween", &Actor::removeTween),
		def("removeTweensByName", &Actor::removeTweensByName),
		def("removeTweens", &Actor::removeTweens),

		// virtual void update(const UpdateState &us);
		// virtual void render(const RenderState &rs);
		// virtual void handleEvent(Event *event);		
		// virtual void doRender(const RenderState &rs){}
		// virtual std::string dump(const dumpOptions &opt) const;

		def("global2local", &Actor::global2local),
		def("local2global", &Actor::local2global),

		// void serialize(serializedata* data);
		// void deserialize(const deserializedata* data);
		{}
	};
	registerOXClass<Actor, EventDispatcher>(os, funcs);
	// os->eval("Actor.dispatchEvent = EventDispatcher.dispatchEvent");
}
static bool __registerActor = addRegFunc(registerActor);

// =====================================================================

OS_DECL_OX_CLASS(VStyleActor);
static void registerVStyleActor(OS * os)
{
	struct Lib {
		static VStyleActor * __newinstance()
		{
			return new VStyleActor();
		}
	};

	OS::FuncDef funcs[] = {
		def("__newinstance", &Lib::__newinstance),
		DEF_PROP(blendMode, Sprite, BlendMode),
		DEF_PROP(color, Sprite, Color),
		{}
	};
	registerOXClass<VStyleActor, Actor>(os, funcs);

	OS::NumberDef globalNums[] = {
		{"BLEND_DEFAULT", blend_default},
		{"BLEND_DISABLED", blend_disabled},
		{"BLEND_PREMULTIPLIED_ALPHA", blend_premultiplied_alpha},
		{"BLEND_ALPHA", blend_alpha},
		{"BLEND_ADD", blend_add},
		{"BLEND_SUB", blend_sub},
		{}
	};
	os->pushGlobals();
	os->setNumbers(globalNums);
	os->pop();
}
static bool __registerVStyleActor = addRegFunc(registerVStyleActor);

// =====================================================================

OS_DECL_OX_CLASS(Sprite);
static void registerSprite(OS * os)
{
	struct Lib { // Actor: public oxygine::Actor
		static Sprite * __newinstance()
		{
			return new Sprite();
		}
	};

	OS::FuncDef funcs[] = {
		def("__newinstance", &Lib::__newinstance),
		def("setResAnim", &Sprite::setResAnim),
		DEF_SET(resAnim, Sprite, ResAnim),
		{}
	};
	registerOXClass<Sprite, VStyleActor>(os, funcs);
}
static bool __registerSprite = addRegFunc(registerSprite);

// =====================================================================

OS_DECL_OX_CLASS(Button);
static void registerButton(OS * os)
{
	OS::FuncDef funcs[] = {
		DEF_PROP("row", Button, Row), 
		{}
	};
	OS::NumberDef nums[] = {
		{}
	};
	registerOXClass<Button, Actor>(os, funcs, nums);
}
static bool __registerButton = addRegFunc(registerButton);

// =====================================================================

OS_DECL_OX_CLASS(Stage);
static void registerStage(OS * os)
{
	OS::FuncDef funcs[] = {
		{}
	};
	OS::NumberDef nums[] = {
		{"ACTIVATE", Stage::ACTIVATE},
		{"DEACTIVATE", Stage::DEACTIVATE},
		{"LOST_CONTEXT", Stage::LOST_CONTEXT},
		{}
	};
	registerOXClass<Stage, Actor>(os, funcs, nums);
}
static bool __registerStage = addRegFunc(registerStage);

// =====================================================================

OS_DECL_OX_CLASS(Resource);
static void registerResource(OS * os)
{
	OS::FuncDef funcs[] = {
		{}
	};
	OS::NumberDef nums[] = {
		{}
	};
	registerOXClass<Resource, Object>(os, funcs, nums);
}
static bool __registerResource = addRegFunc(registerResource);

// =====================================================================

OS_DECL_OX_CLASS(Resources);
static void registerResources(OS * os)
{
	struct Lib
	{
		static Resources * __newinstance()
		{
			return new Resources();
		}

		static int loadXML(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(Resources*);
			if(params < 1){
				os->setException("string argument required");
				return 0;
			}
			OS::String name = os->toString(-params+0);
			self->loadXML(name.toChar());
			return 0;
		}
			
	};
	OS::FuncDef funcs[] = {
		def("__newinstance", &Lib::__newinstance),
		{"loadXML", &Lib::loadXML},
		def("getResAnim", &Resources::getResAnim),
		{}
	};
	OS::NumberDef nums[] = {
		{}
	};
	registerOXClass<Resources, Resource>(os, funcs, nums);
}
static bool __registerResources = addRegFunc(registerResources);

// =====================================================================

OxygineOS * os;

struct Oxygine
{
	static void init()
	{
		os = OS::create(new OxygineOS());

		initDateTimeExtension(os);

		std::vector<RegisterFunction>::iterator it = registerFunctions.begin();
		for(; it != registerFunctions.end(); ++it){
			RegisterFunction func = *it;
			func(os);
		}
	}

	static void shutdown()
	{
		os->resetAllEventCallbacks();
		// os->gcFull();
		os->release();
	}

	static void run()
	{
		os->setGCStartWhenUsedBytes(32*1024);

		// pushCtypeValue(os, getStage().get());
		pushCtypeValue(os, getStage());
		os->setGlobal("stage");

		os->require("main.os");
	}

}; // struct Oxygine

} // namespace ObjectScript

void retainOSEventCallback(ObjectScript::OS * os, EventCallback * cb)
{
	OX_ASSERT(dynamic_cast<OxygineOS*>(os));
	dynamic_cast<OxygineOS*>(os)->retainFromEventCallback(cb);
}

void releaseOSEventCallback(ObjectScript::OS * os, EventCallback * cb)
{
	OX_ASSERT(dynamic_cast<OxygineOS*>(os));
	dynamic_cast<OxygineOS*>(os)->releaseFromEventCallback(cb);
}

void callOSEventFunction(ObjectScript::OS * os, int func_id, Event * ev)
{
	int is_stack_event = !ev->_ref_counter; // ((intptr_t)ev < (intptr_t)&ev) ^ ObjectScript::Oxygine::stackOrder;
	if(is_stack_event){
		// ev = ev->clone();
		ev->_ref_counter++;
	}
	
	os->pushValueById(func_id);
	OX_ASSERT(os->isFunction());
	os->pushNull(); // this
	pushCtypeValue(os, ev);
	// int eventId = os->getValueId();
	os->call(1);
	os->handleException();
	
	if(is_stack_event){
		OX_ASSERT(ev->osValueId);
		os->pushValueById(ev->osValueId); // eventId);
		const OS_ClassInfo& info = Event::getClassInfoStatic();
		os->clearUserdata(info.instance_id, -1, info.class_id);
		os->pop();
		OX_ASSERT(!ev->osValueId);

		ev->_ref_counter--; // don't use releaseRef() here to prevent destroy event
	}
}

void handleOSErrorPolicyVa(const char *format, va_list args)
{
	OX_ASSERT(dynamic_cast<OxygineOS*>(ObjectScript::os));
	dynamic_cast<OxygineOS*>(ObjectScript::os)->handleErrorPolicyVa(format, args);
}

#include <sstream>

std::string getOSDebugStr()
{
	std::stringstream s;
	if(ObjectScript::os){
		s << endl;
		s << "OS GC values: " << ObjectScript::os->getNumValues() << endl;
		s << "OS MEM (Kb)";
		s << " used:" << (ObjectScript::os->getUsedBytes() / 1024);
		s << " cached:" << (ObjectScript::os->getCachedBytes() / 1024);
		s << " allocated:" << (ObjectScript::os->getAllocatedBytes() / 1024);
		// s << " max:" << (ObjectScript::os->getMaxAllocatedBytes() / 1024);
		// s << endl;
	}
	return s.str();
}

#endif // OX_WITH_OBJECTSCRIPT
#endif // __OX_BINDER_H__
