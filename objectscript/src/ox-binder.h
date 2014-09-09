#ifndef __OX_BINDER_H__
#ifdef OX_WITH_OBJECTSCRIPT

#include <oxygine-framework.h>

#include <objectscript.h>
#include <os-binder.h>
#include <ext-datetime/os-datetime.h>
#include <EaseFunction.h>
#include <string>

namespace oxygine { class EventCallback; class Event; }

void registerOSEventCallback(oxygine::EventDispatcher*, int id, const oxygine::EventCallback&);
void unregisterOSEventCallback(oxygine::EventDispatcher*, int id, const oxygine::EventCallback&);
void unregisterOSAllEventCallbacks(oxygine::EventDispatcher*);

void registerOSTween(oxygine::Object*, oxygine::Tween*);
void unregisterOSTween(oxygine::Object*, oxygine::Tween*);
void unregisterOSAllTweens(oxygine::Object*);

void registerOSActorChild(oxygine::Actor*, oxygine::Actor*);
void unregisterOSActorChild(oxygine::Actor*, oxygine::Actor*);
void unregisterOSAllActorChildren(oxygine::Actor*);

void callOSEventFunction(int func_id, oxygine::Event * ev);
void handleOSErrorPolicyVa(const char *format, va_list args);
// void destroyOSValueById(int);
std::string getOSDebugStr();

using namespace oxygine;

#define OUTPUT_FILENAME "out.txt"
#define OUTPUT_FILENAME2 "out-prev.txt"

class OS2D: public ObjectScript::OS
{
	typedef ObjectScript::OS super;

public:

	OS2D()
	{
	}

	virtual void initSettings()
	{
		// setSetting(ObjectScript::OS_ESettings::OS_SETTING_CREATE_COMPILED_FILE, false);
		// setSetting(ObjectScript::OS_SETTING_CREATE_TEXT_OPCODES, false);
	}

	String getWritableFilename(const String& filename)
	{
#if 1
		return getFilename(filename);
#else
		Core::Buffer buf(this);
		buf += filename;
		char * str = (char*)buf.buffer.buf;
		int count = buf.buffer.count;
		if(str[0] == '.' && OS_IS_SLASH(str[1])){
			str += 2;
			count -= 2;
		}
		for(int i = 0; i < count; i++){
			if(OS_IS_SLASH(str[i])){
				str[i] = '_';
			}
		}
		return String(this, str, count);
#endif
	}

	virtual String getCompiledFilename(const String& resolved_filename)
	{
		return getWritableFilename(super::getCompiledFilename(resolved_filename));
	}

	virtual String getTextOpcodesFilename(const String& resolved_filename)
	{
		return getWritableFilename(super::getTextOpcodesFilename(resolved_filename));
	}

	void handleErrorPolicyVa(const char *format, va_list va)
	{
		String str = String::formatVa(this, format, va);
		setException(str);
	}

	// void destroyValueById(int id);

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
		FileHandle * f = openFile(OUTPUT_FILENAME, "a");
		OS_ASSERT(f);
		writeFile((const char*)buf, size, f);
		closeFile(f);

#if 1
		char * str = (char*)malloc(size+1 OS_DBG_FILEPOS);
		memcpy(str, buf, size);
		str[size] = 0;
		oxygine::log::message(str);
		free(str);
#endif
	}
};

namespace ObjectScript {

struct OXObjectInfo
{
	Object * obj;

	OXObjectInfo(Object * _obj, int id)
	{
		obj = _obj;
		obj->osValueId = id;
		obj->addRef();
	}

	~OXObjectInfo()
	{
		OX_ASSERT(obj && obj->osValueId);
		obj->osValueId = 0;
		obj->releaseRef();
	}
};

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
		OXObjectInfo * info = (OXObjectInfo*)os->toUserdata(classinfo.instance_id, offs, classinfo.class_id);
		return info ? (type)info->obj : NULL;
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
		OXObjectInfo * info = (OXObjectInfo*)os->pushUserdata(classinfo.instance_id, sizeof(OXObjectInfo), UserDataDestructor<ttype>::dtor);
		OX_ASSERT(info);
		new (info) OXObjectInfo(val, os->getValueId());
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
		OXObjectInfo * info = (OXObjectInfo*)os->toUserdata(classinfo.instance_id, offs, classinfo.class_id);
		return info ? (type)info->obj : NULL;
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
		OXObjectInfo * info = (OXObjectInfo*)os->pushUserdata(classinfo.instance_id, sizeof(OXObjectInfo), UserDataDestructor<ttype>::dtor);
		OX_ASSERT(info);
		new (info) OXObjectInfo(val.get(), os->getValueId());
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

static void destroyOXObjectInfo(OXObjectInfo * info)
{
	// OX_ASSERT(info->obj && info->obj->osValueId);
	info->~OXObjectInfo();
}

#define OS_DECL_OX_CLASS_DTOR(type) \
	template <> struct UserDataDestructor<type>{ \
		static void dtor(ObjectScript::OS * os, void * data, void *){ \
			OX_ASSERT(data); \
			destroyOXObjectInfo((OXObjectInfo*)data); \
		} \
	};

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

template <class T>
void registerOXClass(ObjectScript::OS * os, const ObjectScript::OS::FuncDef * list, const ObjectScript::OS::NumberDef * numbers = NULL, bool instantiable = true)
{
	const OS_ClassInfo& classinfo = T::getClassInfoStatic();
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
	os->pushGlobals();
	os->pushString(classinfo.classname);
	os->pushUserdata(classinfo.class_id, 0, NULL, NULL);
	os->setFuncs(list);
	os->setNumbers(numbers);
	os->pushBool(instantiable);
	os->setProperty(-2, OS_TEXT("__instantiable"), false);

	const OS_ClassInfo& classinfo_proto = Prototype::getClassInfoStatic();
	os->pushStackValue();
	os->getGlobal(classinfo_proto.classname);
	os->setPrototype(classinfo.class_id);
	os->setProperty();
}

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
			return type(os->getValueId(offs));
		}
		if(!os->isNull()){
			os->setException("function or null required");
		}
		return type();
	}

	static void push(ObjectScript::OS * os, const type& p)
	{
		os->pushValueById(p.os_func_id);
		OX_ASSERT(os->isFunction());
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
		os->call(4, 1);
		os->handleException();
	}
};

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
			std::string name = self->getName();
			if(name.length() > 0){
				pushCtypeValue(os, name);
				return 1;
			}
			return 0;
		}

		static int setName(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(Object*);
			self->setName(params > 0 && !os->isNull(-params+0) ? os->toString(-params+0).toChar() : "");
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
/*
OS_DECL_OX_CLASS(Font);
static void registerFont(OS * os)
{
	struct Lib {
	};

	OS::FuncDef funcs[] = {
		DEF_PROP("scaleFactor", Font, ScaleFactor),
		def("init", &Font::init),
		// void addGlyph(const glyph &g);
		def("sortGlyphs", &Font::sortGlyphs),
		// const glyph*	getGlyph(int code) const;
		DEF_GET("baselineDistance", Font, BaselineDistance),
		DEF_GET("size", Font, Size),
		DEF_GET("lineHeight", Font, LineHeight),
		{}
	};
	OS::NumberDef nums[] = {
		{}
	};
	registerOXClass<Font, Object>(os, funcs);
}
static bool __registerFont = addRegFunc(registerFont);
*/
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
		DEF_PROP(type, Event, Type),
		DEF_PROP(phase, Event, Phase),
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
		DEF_PROP(localPosition, TouchEvent, LocalPosition),
		DEF_PROP(position, TouchEvent, Position),
		DEF_PROP(pressure, TouchEvent, Pressure),
		DEF_PROP(mouseButton, TouchEvent, MouseButton),
		DEF_PROP(index, TouchEvent, Index),
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
		{"START", TouchEvent::TOUCH_DOWN},
		{"END", TouchEvent::TOUCH_UP},
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
			os->pushNumber(self->addEventListener(ev, EventCallback(funcId)));
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
			self->removeEventListener(ev, EventCallback(funcId));
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

OS_DECL_OX_CLASS(ResFont);
static void registerResFont(OS * os)
{
	OS::FuncDef funcs[] = {
		{}
	};
	OS::NumberDef nums[] = {
		{}
	};
	registerOXClass<ResFont, Resource>(os, funcs, nums);
}
static bool __registerResFont = addRegFunc(registerResFont);

// =====================================================================

OS_DECL_CTYPE_ENUM(TextStyle::HorizontalAlign);
OS_DECL_CTYPE_ENUM(TextStyle::VerticalAlign);

OS_DECL_CTYPE(TextStyle);

template <>
struct CtypeValue<TextStyle>
{
	typedef TextStyle type;

	static bool isValid(const type&){ return true; }

	static type def(ObjectScript::OS * os){ return type(); }
	static type getArg(ObjectScript::OS * os, int offs)
	{
		if(os->isObject(offs)){
			type t;
			
			os->getProperty(offs, "resFont");
			t.resFont = CtypeValue<ResFont*>::getArg(os, -1);
			os->pop();

			t.hAlign = (TextStyle::HorizontalAlign)(os->getProperty(offs, "hAlign"), os->popInt(t.hAlign));
			t.vAlign = (TextStyle::VerticalAlign)(os->getProperty(offs, "vAlign"), os->popInt(t.vAlign));
			t.linesOffset = (os->getProperty(offs, "linesOffset"), os->popInt(t.linesOffset));
			t.kerning = (os->getProperty(offs, "kerning"), os->popInt(t.kerning));
			t.multiline = (os->getProperty(offs, "multiline"), os->popBool(t.multiline));
			t.breakLongWords = (os->getProperty(offs, "breakLongWords"), os->popBool(t.breakLongWords));

			os->getProperty(offs, "color");
			t.color = CtypeValue<Color>::getArg(os, -1);
			os->pop();

			t.fontSize2Scale = (os->getProperty(offs, "fontSize2Scale"), os->popInt(t.fontSize2Scale));
			return t;
		}
		os->setException("object required");
		return type();
	}

	static void push(ObjectScript::OS * os, const type& t)
	{
		os->newObject();
	
		pushCtypeValue(os, t.resFont);
		os->setProperty(-2, "resFont", false);
	
		pushCtypeValue(os, t.hAlign);
		os->setProperty(-2, "hAlign", false);
	
		pushCtypeValue(os, t.vAlign);
		os->setProperty(-2, "vAlign", false);
	
		pushCtypeValue(os, t.linesOffset);
		os->setProperty(-2, "linesOffset", false);
	
		pushCtypeValue(os, t.kerning);
		os->setProperty(-2, "kerning", false);
	
		pushCtypeValue(os, t.multiline);
		os->setProperty(-2, "multiline", false);
	
		pushCtypeValue(os, t.breakLongWords);
		os->setProperty(-2, "breakLongWords", false);
	
		pushCtypeValue(os, t.color);
		os->setProperty(-2, "color", false);
	
		pushCtypeValue(os, t.fontSize2Scale);
		os->setProperty(-2, "fontSize2Scale", false);
	}
};

// =====================================================================

// OS_DECL_OX_CLASS(TextStyle);
static void registerTextStyle(OS * os)
{
	OS::NumberDef nums[] = {
		{"HALIGN_DEFAULT", TextStyle::HALIGN_DEFAULT},
		{"HALIGN_LEFT", TextStyle::HALIGN_LEFT},
		{"HALIGN_MIDDLE", TextStyle::HALIGN_MIDDLE},
		{"HALIGN_CENTER", TextStyle::HALIGN_CENTER},
		{"HALIGN_RIGHT", TextStyle::HALIGN_RIGHT},
		{"VALIGN_DEFAULT", TextStyle::VALIGN_DEFAULT},
		{"VALIGN_BASELINE", TextStyle::VALIGN_BASELINE},
		{"VALIGN_TOP", TextStyle::VALIGN_TOP},
		{"VALIGN_MIDDLE", TextStyle::VALIGN_MIDDLE},
		{"VALIGN_BOTTOM", TextStyle::VALIGN_BOTTOM},
		{}
	};
	os->getGlobalObject("TextStyle");
	os->setNumbers(nums);
	os->pop();
}
static bool __registerTextStyle = addRegFunc(registerTextStyle);

// =====================================================================

#define CASE_OX_TWEEN(prop, type, tween, func) \
	if(name == prop){ \
		type dest = CtypeValue<type>::getArg(os, -params+1); \
		pushCtypeValue(os, func(tween(dest), \
			duration, loops, twoSides, delay, ease)); \
		return 1; \
	}

#define SWITCH_OX_TWEENS(func) \
	CASE_OX_TWEEN("alpha", unsigned char, Actor::TweenAlpha, func); \
	CASE_OX_TWEEN("opacity", unsigned char, Actor::TweenOpacity, func); \
	CASE_OX_TWEEN("pos", Vector2, Actor::TweenPosition, func); \
	CASE_OX_TWEEN("x", float, Actor::TweenX, func); \
	CASE_OX_TWEEN("y", float, Actor::TweenY, func); \
	CASE_OX_TWEEN("width", float, Actor::TweenWidth, func); \
	CASE_OX_TWEEN("height", float, Actor::TweenHeight, func); \
	CASE_OX_TWEEN("rotation", float, Actor::TweenRotation, func); \
	CASE_OX_TWEEN("rotationDegrees", float, Actor::TweenRotationDegrees, func); \
	CASE_OX_TWEEN("angle", float, Actor::TweenRotationDegrees, func); \
	CASE_OX_TWEEN("scale", Vector2, Actor::TweenScale, func); \
	CASE_OX_TWEEN("scaleX", float, Actor::TweenScaleX, func); \
	CASE_OX_TWEEN("scaleY", float, Actor::TweenScaleY, func); \
	CASE_OX_TWEEN("color", Color, VStyleActor::TweenColor, func); \
	CASE_OX_TWEEN("resAnim", ResAnim*, TweenAnim, func);

OS_DECL_OX_CLASS(Tween);
void registerTween(OS * os)
{
	struct Lib
	{
		/* static int myCreateTween(OS * os, int params, int, int, void*)
		{
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

			SWITCH_OX_TWEENS(createTween);

			return 0;
		} */
	};
	OS::FuncDef funcs[] = {
		// def("__newinstance", &Lib::__newinstance),
		// {"createTween", &Lib::myCreateTween},
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
		DEF_PROP(detachActor, Tween, DetachActor),
		DEF_PROP(detachTarget, Tween, DetachActor),
		// virtual void complete(timeMS deltaTime = std::numeric_limits<int>::max()/2);
		// void start(Actor &actor);
		// void update(Actor &actor, const UpdateState &us);	
		// static float calcEase(EASE ease, float v);
		{}
	};
#define DEF_EASE_DEPRECATED(Ease, EASE) \
	{"EASE_IN" OS_MAKE_STRING(EASE), Tween::ease_in ## Ease}, \
	{"EASE_OUT" OS_MAKE_STRING(EASE), Tween::ease_out ## Ease}, \
	{"EASE_INOUT" OS_MAKE_STRING(EASE), Tween::ease_inOut ## Ease}, \
	{"EASE_OUTIN" OS_MAKE_STRING(EASE), Tween::ease_outIn ## Ease}

#define DEF_EASE(Ease, EASE) \
	{OS_MAKE_STRING(EASE) "_IN", Tween::ease_in ## Ease}, \
	{OS_MAKE_STRING(EASE) "_OUT", Tween::ease_out ## Ease}, \
	{OS_MAKE_STRING(EASE) "_IN_OUT", Tween::ease_inOut ## Ease}, \
	{OS_MAKE_STRING(EASE) "_OUT_IN", Tween::ease_outIn ## Ease}

	OS::NumberDef nums[] = {
		{"EASE_LINEAR", Tween::ease_linear},
		DEF_EASE(Quad, QUAD),
		DEF_EASE(Cubic, CUBIC),
		DEF_EASE(Quart, QUART),
		DEF_EASE(Quint, QUINT),
		DEF_EASE(Sine, SINE),
		DEF_EASE(Expo, EXPO),
		DEF_EASE(Circ, CIRC),
		DEF_EASE(Back, BACK),
		DEF_EASE(Bounce, BOUNCE),

		DEF_EASE_DEPRECATED(Quad, QUAD),
		DEF_EASE_DEPRECATED(Cubic, CUBIC),
		DEF_EASE_DEPRECATED(Quart, QUART),
		DEF_EASE_DEPRECATED(Quint, QUINT),
		DEF_EASE_DEPRECATED(Sine, SINE),
		DEF_EASE_DEPRECATED(Expo, EXPO),
		DEF_EASE_DEPRECATED(Circ, CIRC),
		DEF_EASE_DEPRECATED(Back, BACK),
		DEF_EASE_DEPRECATED(Bounce, BOUNCE),
		{}
	};
	registerOXClass<Tween, EventDispatcher>(os, funcs, nums);
#undef DEF_EASE
#undef DEF_EASE_DEPRECATED
}
static bool __registerTween = addRegFunc(registerTween);

// =====================================================================

using namespace EaseFunction; // it's really needed to work DEF_EASE macro

OS_DECL_CTYPE_ENUM(EaseFunction::EaseType);

void registerEase(OS * os)
{
	struct Lib
	{
		static int run(OS * os, int params, int, int, void*)
		{
			if(params < 2){
				os->setException("two arguments required");
				return 0;
			}
			float t = os->toFloat(-params+0);
			EaseFunction::EaseType type = (EaseFunction::EaseType)os->toInt(-params+1);
			if(params == 2){
				os->pushNumber(EaseFunction::run(t, type));
				return 1;
			}
			float easingParam[5];
			if(params - 2 > 5){
				params = 7;
			}
			for(int i = 3; i < params; i++){
				easingParam[i-3] = os->toFloat(-params+i);
			}
			os->pushNumber(EaseFunction::run(t, type, easingParam));
			return 1;
		}
	};
	OS::FuncDef funcs[] = {
		{"run", &Lib::run},
		def("getReverseType", &EaseFunction::getReverseType),
		{}
	};

#define DEF_EASE(Ease, EASE) \
	{OS_MAKE_STRING(EASE) "_IN", Ease ## In}, \
	{OS_MAKE_STRING(EASE) "_OUT", Ease ## Out}, \
	{OS_MAKE_STRING(EASE) "_IN_OUT", Ease ## InOut}, \
	{OS_MAKE_STRING(EASE) "_OUT_IN", Ease ## OutIn}

	OS::NumberDef nums[] = {
		{"LINEAR", EaseFunction::Linear},
		{"PING_PONG", EaseFunction::PingPong},
		DEF_EASE(Sine, SINE),
		DEF_EASE(Quad, QUAD),
		DEF_EASE(Cubic, CUBIC),
		DEF_EASE(Quart, QUART),
		DEF_EASE(Quint, QUINT),
		DEF_EASE(Expo, EXPO),
		DEF_EASE(Circ, CIRC),
		DEF_EASE(Elastic, ELASTIC),
		DEF_EASE(Back, BACK),
		DEF_EASE(Bounce, BOUNCE),
		{}
	};
	os->getGlobalObject("Ease");
	os->setFuncs(funcs);
	os->setNumbers(nums);
	os->pop();
#undef DEF_EASE
#undef CONCAT_IDENT
}
static bool __registerEase = addRegFunc(registerEase);

// =====================================================================

OS_DECL_OX_CLASS(TweenQueue);
void registerTweenQueue(OS * os)
{
	struct Lib
	{
		static TweenQueue * __newinstance()
		{
			return new TweenQueue();
		}

		static int add(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(TweenQueue*);
			if(params < 1){
				os->setException("argument required");
				return 0;
			}
			Tween * tween = CtypeValue<Tween*>::getArg(os, -params+0);
			if(tween){
				pushCtypeValue(os, self->add(tween));
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

			SWITCH_OX_TWEENS(self->add);

			return 0;
		}
	};
	OS::FuncDef funcs[] = {
		def("__newinstance", &Lib::__newinstance),
		{"add", &Lib::add},
		{}
	};
	OS::NumberDef nums[] = {
		{}
	};
	registerOXClass<TweenQueue, Tween>(os, funcs, nums);
}
static bool __registerTweenQueue = addRegFunc(registerTweenQueue);

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

	timeMS getDt() const
	{
		return us.dt;
	}
	
	void setDt(timeMS dt)
	{
		us.dt = dt;
	}
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
		DEF_PROP(dt, UpdateEvent, Dt),
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
	void setUpdateCallback(const EventCallback& cb)
	{ 
		unregisterOSEventCallback(this, (intptr_t)this, updateCallback);
		updateCallback = cb;
		registerOSEventCallback(this, (intptr_t)this, updateCallback);
	}

	timeMS getInterval() const { return interval; }
	void setInterval(timeMS _interval){ interval = _interval; curInterval = fixInterval = 0; }

protected:

	spUpdateEvent ev;
	timeMS interval;
	timeMS curInterval;
	timeMS fixInterval;

	virtual void _update(Actor& actor, const UpdateState& us)
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

	void setDuration(timeMS duration)
	{
		init(duration);
	}

protected:

	// void init(Actor &){}
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
		DEF_SET(duration, BaseDoneTween, Duration), 
		{}
	};
	OS::NumberDef nums[] = {
		{}
	};
	registerOXClass<BaseDoneTween, Tween>(os, funcs, nums);
}
static bool __registerBaseDoneTween = addRegFunc(registerBaseDoneTween);

// =====================================================================

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

			SWITCH_OX_TWEENS(self->addTween);

			/*
			VStyleActor * styleActor = dynamic_cast<VStyleActor*>(self);
			if(styleActor){
				CASE_OX_TWEEN("color", Color, VStyleActor::TweenColor, self->addTween);
			}
			Sprite * sprite = dynamic_cast<Sprite*>(self);
			if(sprite){
				CASE_OX_TWEEN("resAnim", ResAnim*, TweenAnim, self->addTween);
			}
			*/
			return 0;
		}
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
		
		// deprecated
		DEF_GET(anchor, Actor, Anchor),
		def("__set@anchor", (void(Actor::*)(const Vector2 &))&Actor::setAnchor),
		def("__get@isAnchorInPixels", &Actor::getIsAnchorInPixels),

		DEF_GET(pivot, Actor, Anchor),
		def("__set@pivot", (void(Actor::*)(const Vector2 &))&Actor::setAnchor),
		def("__get@isPivotInPixels", &Actor::getIsAnchorInPixels),

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

OS_DECL_OX_CLASS(TextField);
static void registerTextField(OS * os)
{
	struct Lib {
		static TextField * __newinstance()
		{
			return new TextField();
		}
	};

	OS::FuncDef funcs[] = {
		def("__newinstance", &Lib::__newinstance),
		DEF_PROP("style", TextField, Style),
		// const Rect&					getTextRect();
		DEF_GET("text", TextField, Text),
		def("__set@text", (void(TextField::*)(const string &))&TextField::setText),
		def("__set@htmlText", (void(TextField::*)(const string &))&TextField::setHtmlText),
		DEF_PROP("fontSize2Scale", TextField, FontSize2Scale),
		DEF_PROP("vAlign", TextField, VAlign),
		DEF_PROP("hAlign", TextField, HAlign),
		DEF_PROP("multiline", TextField, Multiline),
		DEF_PROP("breakLongWords", TextField, BreakLongWords),
		DEF_PROP("hAlign", TextField, HAlign),
		DEF_PROP("hAlign", TextField, HAlign),
		DEF_PROP("resFont", TextField, ResFont),
		{}
	};
	registerOXClass<TextField, VStyleActor>(os, funcs);
}
static bool __registerTextField = addRegFunc(registerTextField);

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

OS2D * os;

struct Oxygine
{
	static void init()
	{
		os = OS::create(new OS2D());

		file::rename(OUTPUT_FILENAME, OUTPUT_FILENAME2, ep_ignore_error);
		file::deleteFile(OUTPUT_FILENAME, ep_ignore_error);

		initDateTimeExtension(os);

		std::vector<RegisterFunction>::iterator it = registerFunctions.begin();
		for(; it != registerFunctions.end(); ++it){
			RegisterFunction func = *it;
			func(os);
		}
	}

	static void postInit()
	{
#ifdef OS_DEBUG
		bool debug = true;
#else
		bool debug = false;
#endif
		os->pushBool(debug);
		os->setGlobal("DEBUG");

		os->eval("require.paths[] = 'os2d'");

		os->require("std.os");
	}

	static void shutdown()
	{
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

void registerOSEventCallback(EventDispatcher * ed, int id, const EventCallback& cb)
{
	OX_ASSERT(dynamic_cast<OS2D*>(ObjectScript::os));
	ObjectScript::OS * os = ObjectScript::os;
	if(!ed->osValueId && cb.os_func_id){
		ObjectScript::pushCtypeValue(os, ed);
		os->pop();
	}
	if(ed->osValueId && cb.os_func_id){
#ifdef OS_DEBUG
		ObjectScript::pushCtypeValue(os, ed);
		OX_ASSERT(os->getValueId() == ed->osValueId);
		os->pop();
#endif
		os->pushValueById(ed->osValueId);
		os->getProperty("_registerExternalCallback");
		OX_ASSERT(os->isFunction());
		os->pushValueById(ed->osValueId); // this
		os->pushNumber(id);
		os->pushValueById(cb.os_func_id);
		OX_ASSERT(os->isFunction());
		os->call(2);
		os->handleException();
	}
}

void unregisterOSEventCallback(EventDispatcher * ed, int id, const EventCallback& cb)
{
	OX_ASSERT(dynamic_cast<OS2D*>(ObjectScript::os));
	ObjectScript::OS * os = ObjectScript::os;
	if(ed->osValueId && cb.os_func_id){
#ifdef OS_DEBUG
		ObjectScript::pushCtypeValue(os, ed);
		OX_ASSERT(os->getValueId() == ed->osValueId);
		os->pop();
#endif
		os->pushValueById(ed->osValueId);
		os->getProperty("_unregisterExternalCallback");
		OX_ASSERT(os->isFunction());
		os->pushValueById(ed->osValueId); // this
		os->pushNumber(id);
		os->pushValueById(cb.os_func_id);
		OX_ASSERT(os->isFunction());
		os->call(2);
		os->handleException();
	}
}

void unregisterOSAllEventCallbacks(EventDispatcher * ed)
{
	OX_ASSERT(dynamic_cast<OS2D*>(ObjectScript::os));
	ObjectScript::OS * os = ObjectScript::os;
	if(ed->osValueId){
#ifdef OS_DEBUG
		ObjectScript::pushCtypeValue(os, ed);
		OX_ASSERT(os->getValueId() == ed->osValueId);
		os->pop();
#endif
		os->pushValueById(ed->osValueId);
		os->getProperty("_unregisterAllExternalCallbacks");
		OX_ASSERT(os->isFunction());
		os->pushValueById(ed->osValueId); // this
		os->call(0);
		os->handleException();
	}
}

void registerOSTween(Object * a, Tween * t)
{
	OX_ASSERT(dynamic_cast<OS2D*>(ObjectScript::os));
	ObjectScript::OS * os = ObjectScript::os;
	ObjectScript::pushCtypeValue(os, a);
	OX_ASSERT(os->getValueId() == a->osValueId);
	os->getProperty("_registerExternalTween");
	OX_ASSERT(os->isFunction());
	ObjectScript::pushCtypeValue(os, a); // os->pushValueById(a->osValueId); // this
	ObjectScript::pushCtypeValue(os, t);
	OX_ASSERT(os->getValueId() == t->osValueId);
	os->call(1);
	os->handleException();
}

void unregisterOSTween(Object * a, Tween * t)
{
	OX_ASSERT(dynamic_cast<OS2D*>(ObjectScript::os));
	ObjectScript::OS * os = ObjectScript::os;
	if(a->osValueId && t->osValueId){
#ifdef OS_DEBUG
		ObjectScript::pushCtypeValue(os, a);
		OX_ASSERT(os->getValueId() == a->osValueId);
		os->pop();

		ObjectScript::pushCtypeValue(os, t);
		OX_ASSERT(os->getValueId() == t->osValueId);
		os->pop();
#endif
		os->pushValueById(a->osValueId);
		os->getProperty("_unregisterExternalTween");
		OX_ASSERT(os->isFunction());
		os->pushValueById(a->osValueId); // this
		os->pushValueById(t->osValueId);
		os->call(1);
		os->handleException();
	}
}

void unregisterOSAllTweens(Object * a)
{
	OX_ASSERT(dynamic_cast<OS2D*>(ObjectScript::os));
	ObjectScript::OS * os = ObjectScript::os;
	if(a->osValueId){
#ifdef OS_DEBUG
		ObjectScript::pushCtypeValue(os, a);
		OX_ASSERT(os->getValueId() == a->osValueId);
		os->pop();
#endif
		os->pushValueById(a->osValueId);
		os->getProperty("_unregisterAllExternalTweens");
		OX_ASSERT(os->isFunction());
		os->pushValueById(a->osValueId); // this
		os->call(0);
		os->handleException();
	}
}

void registerOSActorChild(Actor * a, Actor * child)
{
	OX_ASSERT(dynamic_cast<OS2D*>(ObjectScript::os));
	ObjectScript::OS * os = ObjectScript::os;
	ObjectScript::pushCtypeValue(os, a);
	OX_ASSERT(os->getValueId() == a->osValueId);
	os->getProperty("_registerExternalChild");
	OX_ASSERT(os->isFunction());
	ObjectScript::pushCtypeValue(os, a); // os->pushValueById(a->osValueId); // this
	ObjectScript::pushCtypeValue(os, child);
	OX_ASSERT(os->getValueId() == child->osValueId);
	os->call(1);
	os->handleException();
}

void unregisterOSActorChild(Actor * a, Actor * child)
{
	OX_ASSERT(dynamic_cast<OS2D*>(ObjectScript::os));
	ObjectScript::OS * os = ObjectScript::os;
	if(a->osValueId && child->osValueId){
#ifdef OS_DEBUG
		ObjectScript::pushCtypeValue(os, a);
		OX_ASSERT(os->getValueId() == a->osValueId);
		os->pop();

		ObjectScript::pushCtypeValue(os, child);
		OX_ASSERT(os->getValueId() == child->osValueId);
		os->pop();
#endif
		os->pushValueById(a->osValueId);
		os->getProperty("_unregisterExternalChild");
		OX_ASSERT(os->isFunction());
		os->pushValueById(a->osValueId); // this
		os->pushValueById(child->osValueId);
		os->call(1);
		os->handleException();
	}
}

void unregisterOSAllActorChildren(Actor * a)
{
	OX_ASSERT(dynamic_cast<OS2D*>(ObjectScript::os));
	ObjectScript::OS * os = ObjectScript::os;
	if(a->osValueId){
#ifdef OS_DEBUG
		ObjectScript::pushCtypeValue(os, a);
		OX_ASSERT(os->getValueId() == a->osValueId);
		os->pop();
#endif
		os->pushValueById(a->osValueId);
		os->getProperty("_unregisterAllExternalChildren");
		OX_ASSERT(os->isFunction());
		os->pushValueById(a->osValueId); // this
		os->call(0);
		os->handleException();
	}
}

void callOSEventFunction(int func_id, Event * ev)
{
	OX_ASSERT(dynamic_cast<OS2D*>(ObjectScript::os));
	ObjectScript::OS * os = ObjectScript::os;

	int is_stack_event = !ev->_ref_counter; // ((intptr_t)ev < (intptr_t)&ev) ^ ObjectScript::Oxygine::stackOrder;
	if(is_stack_event){
		// ev = ev->clone();
		ev->_ref_counter++;
	}
	
	os->pushValueById(func_id);
	if(os->isFunction()){
		os->pushNull(); // this
		pushCtypeValue(os, ev);
		// int eventId = os->getValueId();
		os->call(1);
		os->handleException();
	}else{
		os->pop();
	}
	
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
	OX_ASSERT(dynamic_cast<OS2D*>(ObjectScript::os));
	ObjectScript::os->handleErrorPolicyVa(format, args);
}

/*
void OS2D::destroyValueById(int id)
{
#if 0
	Core::GCValue * value = core->values.get(id);
	if(value){
		core->saveFreeCandidateValue(value);
	}
#else
	pushValueById(id);
	const OS_ClassInfo& classinfo = Object::getClassInfoStatic();
	ObjectScript::OXObjectInfo * info = (ObjectScript::OXObjectInfo*)toUserdata(classinfo.instance_id, -1, classinfo.class_id);
	if(info){
		OX_ASSERT(info->obj->osValueId == id);
		info->obj = NULL;
	}
	pop();
	Core::GCValue * value = core->values.get(id);
	if(value){
		core->clearValue(value);
	}
#endif
}

void destroyOSValueById(int id)
{
	OX_ASSERT(dynamic_cast<OS2D*>(ObjectScript::os));
	ObjectScript::os->destroyValueById(id);
}
*/

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
