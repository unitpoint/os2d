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

#ifndef __OX_BINDER_H__
#define __OX_BINDER_H__

#include <oxygine-framework.h>

#include <objectscript.h>
#include <os-binder.h>
#include <ext-datetime/os-datetime.h>
#include <EaseFunction.h>
#include <string>

// namespace oxygine { class EventCallback; class Event; }

// using namespace oxygine;

#define OUTPUT_FILENAME "out.txt"
#define OUTPUT_FILENAME2 "out-prev.txt"

class OS2D: public ObjectScript::OS
{
	typedef ObjectScript::OS super;

public:

	bool logToOutFile;

	OS2D()
	{
		logToOutFile = true;
	}

	virtual void initSettings()
	{
#ifndef OS_DEBUG
		setSetting(ObjectScript::OS_SETTING_CREATE_COMPILED_FILE, false);
		setSetting(ObjectScript::OS_SETTING_CREATE_TEXT_OPCODES, false);
#endif
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
		handleException();
	}

	// void destroyValueById(int id);

	int getNumValues() const
	{
		return core->num_created_values - core->num_destroyed_values;
	}

	virtual bool isFileExist(const OS_CHAR * filename)
	{
		return oxygine::file::exists(filename);
	}

	/* virtual int getFileSize(const OS_CHAR * filename)
	{
	}

	virtual int getFileSize(FileHandle * f)
	{
	} */
	
	virtual FileHandle * openFile(const OS_CHAR * filename, const OS_CHAR * mode)
	{
		return (FileHandle*)oxygine::file::open(filename, mode);
	}

	virtual int readFile(void * buf, int size, FileHandle * f)
	{
		if(f){
			return oxygine::file::read((oxygine::file::handle)f, buf, (unsigned)size);
		}
		return 0;
	}

	virtual int writeFile(const void * buf, int size, FileHandle * f)
	{
		if(f){
			oxygine::file::write((oxygine::file::handle)f, buf, (unsigned)size);
		}
		return size;
	}

	virtual int seekFile(FileHandle * f, int offset, int whence)
	{
		if(f){
			return oxygine::file::seek((oxygine::file::handle)f, offset, whence);
		}
		return 0;
	}
	
	virtual void closeFile(FileHandle * f)
	{
		if(f){
			oxygine::file::close((oxygine::file::handle)f);
		}
	}

	virtual void echo(const void * buf, int size)
	{
#ifdef WIN32
		FileHandle * f = openFile(OUTPUT_FILENAME, "a");
		OS_ASSERT(f);
		writeFile((const char*)buf, size, f);
		closeFile(f);
#endif
#if 1
		char * str = (char*)malloc(size+1 OS_DBG_FILEPOS);
		memcpy(str, buf, size);
		str[size] = 0;
		oxygine::log::message(str);
		free(str);
#endif
	}

	void triggerShutdownFunctions()
	{
		resetTerminated();
		getGlobal("triggerShutdownFunctions");
		OS_ASSERT(isFunction() || isNull());
		pushGlobals();
		callFT();
	}

	void triggerCleanupFunctions()
	{
		resetTerminated();
		getGlobal("triggerCleanupFunctions");
		OS_ASSERT(isFunction() || isNull());
		pushGlobals();
		callFT();
	}
};

namespace ObjectScript {

struct OXObjectInfo
{
	oxygine::Object * obj;

	OXObjectInfo(oxygine::Object * _obj, int id)
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
			OX_ASSERT(data && dynamic_cast<oxygine::Object*>(((OXObjectInfo*)data)->obj)); \
			destroyOXObjectInfo((OXObjectInfo*)data); \
		} \
	};

#define OS_DECL_OX_CLASS(type) \
	template <> struct CtypeName<type> { static const OS_CHAR * getName(){ return type::getClassInfoStatic().classname; } }; \
	template <> struct CtypeName< oxygine::intrusive_ptr<type> > { static const OS_CHAR * getName(){ return type::getClassInfoStatic().classname; } }; \
	template <> struct CtypeValue<type*>: public CtypeOXClass<type*>{}; \
	template <> struct CtypeValue< oxygine::intrusive_ptr<type> >: public CtypeOXSmartClass< oxygine::intrusive_ptr<type> >{}; \
	template <> struct CtypeValue< oxygine::intrusive_ptr<type>& >: public CtypeOXSmartClass< oxygine::intrusive_ptr<type> >{}; \
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
		return p ? *p : def(os);
	}
	static void push(ObjectScript::OS * os, const type& val)
	{
		type * data = (type*)os->pushUserdata(CtypeId<type>::getInstanceId(), sizeof(type), CtypeOXStructDestructor<type>::dtor);
		OX_ASSERT(data);
		new (data) type(val);
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

template <class T> struct CtypeOXStructPointer
{
	typedef typename T type;

	static bool isValid(const type * p){ return p != NULL; }
	// static const type * def(ObjectScript::OS*){ static T temp; return &temp; }
	static type * getArg(ObjectScript::OS * os, int offs)
	{
		type * p = (type*)os->toUserdata(CtypeId<type>::getInstanceId(), offs, CtypeId<type>::getId());
		return p;
	}
	/* static void push(ObjectScript::OS * os, const type * val)
	{
		type * data = (type*)os->pushUserdata(CtypeId<type>::getInstanceId(), sizeof(*type), CtypeOXStructDestructor<type>::dtor);
		OX_ASSERT(data);
		new (data) type(*val);
		os->pushStackValue();
		os->getGlobal(CtypeName<ttype>::getName());
		if(!os->isUserdata(CtypeId<ttype>::getId(), -1)){
			OX_ASSERT(false);
			os->pop(2);
		}else{
			os->setPrototype(CtypeId<ttype>::getInstanceId());
		}
	} */
};

#define OS_DECL_OX_STRUCT(type) \
	OS_DECL_CTYPE(type); \
	template <> struct CtypeValue<type>: public CtypeOXStruct<type>{}; \
	template <> struct CtypeValue<type*>: public CtypeOXStructPointer<type>{};

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

OS_DECL_CTYPE_ENUM(oxygine::error_policy);
OS_DECL_CTYPE_ENUM(oxygine::Tween::EASE);
OS_DECL_CTYPE_ENUM(oxygine::blend_mode);
OS_DECL_CTYPE_ENUM(oxygine::Event::Phase);
OS_DECL_CTYPE_ENUM(oxygine::MouseButton);
OS_DECL_CTYPE_ENUM(oxygine::Box9Sprite::StretchMode);

// =====================================================================

typedef void(*RegisterFunction)(OS*);
extern std::vector<RegisterFunction> * registerFunctions;

bool addRegFunc(RegisterFunction func);

#define DEF_GET(prop, type, func_post) \
	def("__get@" prop, &type::get ## func_post)

#define DEF_SET(prop, type, func_post) \
	def("__set@" prop, &type::set ## func_post)

#define DEF_PROP(prop, type, func_post) \
	DEF_GET(prop, type, func_post), \
	DEF_SET(prop, type, func_post)

#define DEF_CONST(name) \
	{ #name, name }

#define READ_PROP_IF_SET(dest, offs, name, type) \
	if((os->getProperty(offs, name), !os->isNull())){ \
		dest = CtypeValue<type>::getArg(os, -1); \
	} \
	os->pop()

#ifdef OS_DEBUG
#define OS_DBG_FILEPOS_DECL_DEF , const OS_CHAR * dbg_filename = "unknown", int dbg_line = 0
#else
#define OS_DBG_FILEPOS_DECL_DEF
#endif

// =====================================================================

template <class T>
void registerOXClass(ObjectScript::OS * os, const ObjectScript::OS::FuncDef * list, 
	const ObjectScript::OS::NumberDef * numbers = NULL, bool instantiable = true OS_DBG_FILEPOS_DECL_DEF)
{
	const OS_ClassInfo& classinfo = T::getClassInfoStatic();
	
	os->getGlobal(classinfo.classname, false);
	if(!os->isNull()){
		os->pop();
#ifdef OS_DEBUG
		os->setException(OS::String::format(os, "Class %s is already registered\n"
			"Check OS_DECLARE_CLASSINFO(Classname) macro to be used\n"
			"[%d] %s", 
			classinfo.classname, dbg_line, dbg_filename));
#else
		os->setException(OS::String::format(os, "Class %s is already registered\n"
			"Check OS_DECLARE_CLASSINFO(Classname) macro to be used", 
			classinfo.classname));
#endif
		os->handleException();
		return;
	}

	os->pop();
	os->pushGlobals();
	os->pushString(classinfo.classname);
	os->pushUserdata(classinfo.class_id, 0, NULL, NULL);
	os->setFuncs(list);
	os->setNumbers(numbers);
	os->pushBool(instantiable);
	os->setProperty(-2, OS_TEXT("__instantiable"), false);
	os->setProperty(false);
}

template <class T, class Prototype>
void registerOXClass(ObjectScript::OS * os, const ObjectScript::OS::FuncDef * list, 
	const ObjectScript::OS::NumberDef * numbers = NULL, bool instantiable = true OS_DBG_FILEPOS_DECL_DEF)
{
	const OS_ClassInfo& classinfo = T::getClassInfoStatic();

	os->getGlobal(classinfo.classname, false);
	if(!os->isNull()){
#ifdef OS_DEBUG
		os->setException(OS::String::format(os, "Class %s is already registered\n"
			"Check OS_DECLARE_CLASSINFO(Classname) macro to be used\n"
			"[%d] %s", 
			classinfo.classname, dbg_line, dbg_filename));
#else
		os->setException(OS::String::format(os, "Class %s is already registered\n"
			"Check OS_DECLARE_CLASSINFO(Classname) macro to be used", 
			classinfo.classname));
#endif
		os->handleException();
		return;
	}
	os->pop();
	
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
	if(os->isNull()){
#ifdef OS_DEBUG
		os->setException(OS::String::format(os, "Class %s is not registered\n"
			"[%d] %s", 
			classinfo.classname, dbg_line, dbg_filename));
#else
		os->setException(OS::String::format(os, "Class %s is not registered\n", 
			classinfo.classname));
#endif
		os->handleException();
		return;
	}
	os->setPrototype(classinfo.class_id);

	os->setProperty(false);
}

// =====================================================================

OS_DECL_CTYPE_NAME(oxygine::Vector2, "vec2");
OS_DECL_CTYPE_NAME(oxygine::Point, "vec2");

template <class T, class SubType>
struct CtypeValuePointOf
{
	typedef T type;
	// typedef typename T::type SubType;

	static bool isValid(const type&){ return true; }

	static type def(ObjectScript::OS * os){ return type(0, 0); }
	static type getArg(ObjectScript::OS * os, int offs)
	{
		if(os->isObject(offs)){
			SubType x = (SubType)(os->getProperty(offs, "x"), os->popNumber());
			SubType y = (SubType)(os->getProperty(offs, "y"), os->popNumber());
			return type(x, y);
		}
		OS_NUMBER v;
		if(os->isNumber(offs, &v)){
			return type((SubType)v, (SubType)v);
		}
#if 0
		if(os->isArray(offs)){
			os->pushStackValue(offs);
			os->pushNumber(0);
			os->getProperty();
			SubType x = (SubType)os->popNumber();
		
			os->pushStackValue(offs);
			os->pushNumber(1);
			os->getProperty();
			SubType y = (SubType)os->popNumber();

			return type(x, y);
		}
#endif
		os->setException(OS::String(os, "vec2 or number required instead of ") + os->toString(offs));
		return type(0, 0);
	}

	static void push(ObjectScript::OS * os, const type& p)
	{
#if 1
		os->getGlobal("vec2");
		OX_ASSERT(os->isObject());
		os->pushGlobals();
		os->pushNumber(p.x);
		os->pushNumber(p.y);
		os->callFT(2, 1);
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

template <class T>
struct CtypeValuePoint: public CtypeValuePointOf<T, typename T::type>
{
};

template <> struct CtypeValue<oxygine::Vector2>: public CtypeValuePoint<oxygine::Vector2> {};
template <> struct CtypeValue<oxygine::Point>: public CtypeValuePoint<oxygine::Point> {};

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
	registerUserClass<UpdateState>(os, funcs, NULL, true OS_DBG_FILEPOS);
}
static bool __registerUpdateState = addRegFunc(registerUpdateState);

#else

OS_DECL_CTYPE_NAME(oxygine::UpdateState, "UpdateState");

template <>
struct CtypeValue<oxygine::UpdateState>
{
	typedef oxygine::UpdateState type;

	static bool isValid(const type&){ return true; }

	static type def(ObjectScript::OS * os){ return type(); }
	static type getArg(ObjectScript::OS * os, int offs)
	{
		if(os->isObject(offs)){
			type us;
			us.time = (oxygine::timeMS)(os->getProperty(offs, "time"), os->popNumber() * 1000.0f);
			us.dt = (oxygine::timeMS)(os->getProperty(offs, "dt"), os->popNumber() * 1000.0f);
			us.iteration = (os->getProperty(offs, "iteration"), os->popInt());
			return us;
		}
		os->setException("UpdateState required");
		return type();
	}

	static void push(ObjectScript::OS * os, const type& p)
	{
		os->getGlobal("UpdateState");
		OX_ASSERT(os->isObject());
		os->pushGlobals();
		os->pushNumber(p.time / 1000.0f);
		os->pushNumber(p.dt / 1000.0f);
		os->pushNumber(p.iteration);
		os->callFT(3, 1);
		os->handleException();
	}
};

#endif

// =====================================================================

OS_DECL_CTYPE_NAME(oxygine::EventCallback, "EventCallback");

template <>
struct CtypeValue<oxygine::EventCallback>
{
	typedef oxygine::EventCallback type;

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

OS_DECL_CTYPE(oxygine::Color);

template <>
struct CtypeValue<oxygine::Color>
{
	typedef oxygine::Color type;

	static bool isValid(const type&){ return true; }

	static type def(ObjectScript::OS * os){ return type(0, 0, 0, 0); }
	static type getArg(ObjectScript::OS * os, int offs)
	{
		if(os->isObject(offs)){
			unsigned char r = (unsigned char)oxygine::scalar::clamp((os->getProperty(offs, "r"), os->popFloat(0.0f)) * 255.0f, 0.0f, 255.0f);
			unsigned char g = (unsigned char)oxygine::scalar::clamp((os->getProperty(offs, "g"), os->popFloat(0.0f)) * 255.0f, 0.0f, 255.0f);
			unsigned char b = (unsigned char)oxygine::scalar::clamp((os->getProperty(offs, "b"), os->popFloat(0.0f)) * 255.0f, 0.0f, 255.0f);
			unsigned char a = (unsigned char)oxygine::scalar::clamp((os->getProperty(offs, "a"), os->popFloat(1.0f)) * 255.0f, 0.0f, 255.0f);
			return type(r, g, b, a);
		}
		OS_NUMBER v;
		if(os->isNumber(offs, &v)){
			unsigned char a = (unsigned char)oxygine::scalar::clamp((float)v * 255.0f, 0.0f, 255.0f);
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
		os->callFT(4, 1);
		os->handleException();
	}
};

// =====================================================================

OS_DECL_OX_CLASS(oxygine::Object);
// OS_DECL_OX_CLASS(Font);
OS_DECL_OX_CLASS(oxygine::Clock);
OS_DECL_OX_CLASS(oxygine::Event);
OS_DECL_OX_CLASS(oxygine::TouchEvent);
OS_DECL_OX_CLASS(oxygine::TweenEvent);
OS_DECL_OX_CLASS(oxygine::EventDispatcher);
OS_DECL_OX_CLASS(oxygine::Resource);
OS_DECL_OX_CLASS(oxygine::ResAnim);
OS_DECL_OX_CLASS(oxygine::ResFont);

OS_DECL_CTYPE_ENUM(oxygine::TextStyle::HorizontalAlign);
OS_DECL_CTYPE_ENUM(oxygine::TextStyle::VerticalAlign);

OS_DECL_CTYPE(oxygine::TextStyle);

template <>
struct CtypeValue<oxygine::TextStyle>
{
	typedef oxygine::TextStyle type;

	static bool isValid(const type&){ return true; }

	static type def(ObjectScript::OS * os){ return type(); }
	static type getArg(ObjectScript::OS * os, int offs)
	{
		if(os->isObject(offs)){
			type t;
			
			os->getProperty(offs, "resFont");
			if(!os->isNull()){
				t.resFont = CtypeValue<oxygine::ResFont*>::getArg(os, -1);
			}
			os->pop();

			t.hAlign = (oxygine::TextStyle::HorizontalAlign)(os->getProperty(offs, "hAlign"), os->popInt(t.hAlign));
			t.vAlign = (oxygine::TextStyle::VerticalAlign)(os->getProperty(offs, "vAlign"), os->popInt(t.vAlign));
			t.linesOffset = (os->getProperty(offs, "linesOffset"), os->popInt(t.linesOffset));
			t.kerning = (os->getProperty(offs, "kerning"), os->popInt(t.kerning));
			t.multiline = (os->getProperty(offs, "multiline"), os->popBool(t.multiline));
			t.breakLongWords = (os->getProperty(offs, "breakLongWords"), os->popBool(t.breakLongWords));

			os->getProperty(offs, "color");
			if(!os->isNull()){
				t.color = CtypeValue<oxygine::Color>::getArg(os, -1);
			}
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

OS_DECL_OX_CLASS(oxygine::Tween);

OS_DECL_CTYPE_ENUM(EaseFunction::EaseType);

OS_DECL_OX_CLASS(oxygine::TweenQueue);
// OS_DECL_OX_CLASS(TweenAnim);

// =====================================================================

DECLARE_SMART(KeyboardEvent, spKeyboardEvent);
class KeyboardEvent: public oxygine::Event
{
public:
	OS_DECLARE_CLASSINFO(KeyboardEvent);

	enum {
		DOWN = makefourcc('K', 'E', 'D', 'O'),
		UP = makefourcc('K', 'E', 'U', 'P'),
		TEXTEDITING = makefourcc('K', 'E', 'T', 'E'),
		TEXTINPUT = makefourcc('K', 'E', 'T', 'I')
	};

	SDL_KeyboardEvent key;
	char str[5];

	KeyboardEvent(int type);
	KeyboardEvent(int type, const SDL_KeyboardEvent&);

	SDL_Scancode getScancode() const { return key.keysym.scancode; }
	SDL_Keycode getSym() const { return key.keysym.sym; }
	int getMod() const { return key.keysym.mod; }
	
	void makeStr();
	const char * getStr() const { return str; }

	static void process(oxygine::Event*, oxygine::EventDispatcher*);
};

OS_DECL_OX_CLASS(KeyboardEvent);
OS_DECL_CTYPE_ENUM(SDL_Scancode);
// OS_DECL_CTYPE_ENUM(SDL_Keycode);

// =====================================================================

DECLARE_SMART(UpdateEvent, spUpdateEvent);
class UpdateEvent: public oxygine::Event
{
public:
	OS_DECLARE_CLASSINFO(UpdateEvent)

	enum {
		UPDATE = makefourcc('U','D','E','V')
	};

	oxygine::UpdateState us;
	oxygine::spTween tween;

	UpdateEvent(): Event(UPDATE){}

	const oxygine::UpdateState& getUpdateState() const { return us; }
	void setUpdateState(const oxygine::UpdateState& _us){ us = _us; }

	oxygine::spTween getTween() const { return tween; }
	void setTween(const oxygine::spTween& t){ tween = t; }

	float getDt() const
	{
		return (float)us.dt / 1000.0f;
	}
	void setDt(float dt)
	{
		us.dt = (oxygine::timeMS)(dt * 1000.0f);
	}

	float getTime() const
	{
		return (float)us.time / 1000.0f;
	}
	void setTime(float time)
	{
		us.time = (oxygine::timeMS)(time * 1000.0f);
	}
};

OS_DECL_OX_CLASS(UpdateEvent);

// =====================================================================

DECLARE_SMART(BaseUpdateTween, spBaseUpdateTween);
class BaseUpdateTween: public oxygine::Tween
{
public:
	OS_DECLARE_CLASSINFO(BaseUpdateTween);

	oxygine::EventCallback updateCallback;

	BaseUpdateTween()
	{
		// share same instance for each update (evoid create new OS object each update)
		ev = new UpdateEvent();
		interval = 0;
		curInterval = fixInterval = 0;
	}

	const oxygine::EventCallback& getUpdateCallback() const { return updateCallback; }
	void setUpdateCallback(const oxygine::EventCallback& cb)
	{ 
		unregisterEventCallback(this, "updateCallback", updateCallback);
		updateCallback = cb;
		registerEventCallback(this, "updateCallback", updateCallback);
	}

	float getInterval() const
	{
		return interval / 1000.0f;
	}

	void setInterval(float _interval)
	{ 
		if(_interval < 0) _interval = 0;
		interval = (oxygine::timeMS)(_interval * 1000.0f); 
		if(curInterval > interval){
			curInterval = interval;
		}
		if(fixInterval > interval){
			fixInterval = interval;
		}
	}

protected:

	spUpdateEvent ev;
	oxygine::timeMS interval;
	oxygine::timeMS curInterval;
	oxygine::timeMS fixInterval;

	virtual void _update(oxygine::Actor& actor, const oxygine::UpdateState& us)
	{
		oxygine::timeMS dt = us.dt;
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

// =====================================================================

DECLARE_SMART(BaseDoneTween, spBaseDoneTween);
class BaseDoneTween: public oxygine::Tween
{
public:
	OS_DECLARE_CLASSINFO(BaseDoneTween)

	BaseDoneTween(){}

	void setDuration(float duration)
	{
		oxygine::timeMS delay = (oxygine::timeMS)(duration * 1000.0f);
		init(delay > 1 ? delay : 1);
	}

protected:

	// void init(Actor &){}
	void update(oxygine::Actor &, float p, const oxygine::UpdateState &us){}
};

OS_DECL_OX_CLASS(BaseDoneTween);

// =====================================================================

OS_DECL_OX_CLASS(oxygine::Actor);
OS_DECL_OX_CLASS(oxygine::VStyleActor);
OS_DECL_OX_CLASS(oxygine::TextField);
OS_DECL_OX_CLASS(oxygine::Sprite);
OS_DECL_OX_CLASS(oxygine::Box9Sprite);
OS_DECL_OX_CLASS(oxygine::ColorRectSprite);
OS_DECL_OX_CLASS(oxygine::Button);
OS_DECL_OX_CLASS(oxygine::Stage);
OS_DECL_OX_CLASS(oxygine::Resources);
// =====================================================================

extern OS2D * os;

struct Oxygine
{
	static void init()
	{
		os = OS::create(new OS2D());
		os->setGCStartWhenUsedBytes(32*1024);

		initDateTimeExtension(os);
	}

	static void postInit()
	{
		if(oxygine::file::exists(OUTPUT_FILENAME)){
			oxygine::file::deleteFile(OUTPUT_FILENAME2, oxygine::ep_ignore_error);
			oxygine::file::rename(OUTPUT_FILENAME, OUTPUT_FILENAME2, oxygine::ep_ignore_error);
			oxygine::file::deleteFile(OUTPUT_FILENAME, oxygine::ep_ignore_error);
		}

#ifdef WIN32
		extern std::vector<std::string>& parseCommandLine();
		extern void addFileSystem(const std::string& folder);

		std::vector<std::string>& argv = parseCommandLine();
		for(int i = 1; i < (int)argv.size();){
			if(argv[i] == "-a" && i+1 < (int)argv.size()){
				addFileSystem(argv[i+1]);
				i += 2;
			}else
				i++;
		}
#endif

		std::vector<RegisterFunction>::iterator it = registerFunctions->begin();
		for(; it != registerFunctions->end(); ++it){
			RegisterFunction func = *it;
			func(os);
		}

#ifdef OS_DEBUG
		bool debug = true;
#else
		bool debug = false;
#endif
		os->pushBool(debug);
		os->setGlobal("DEBUG");

		os->pushBool(!debug);
		os->setGlobal("RELEASE");

#if defined WIN32
		const char * platform = "windows";
#elif defined __ANDROID__
		const char * platform = "android";
#endif
		os->pushString(platform);
		os->setGlobal("PLATFORM");

		os->eval("require.paths[] = 'os2d'");

		os->require("std.os");
	}

	static void shutdown()
	{
		os->triggerShutdownFunctions();
		os->triggerCleanupFunctions();
		os->release();

#ifdef WIN32
		extern void shutdownAddedFileSystems();
		shutdownAddedFileSystems();
#endif
	}

	static void run()
	{
		// pushCtypeValue(os, getStage().get());
		pushCtypeValue(os, oxygine::getStage());
		os->setGlobal("stage");

		os->require("main.os");
	}

}; // struct Oxygine

} // namespace ObjectScript

#endif // __OX_BINDER_H__
