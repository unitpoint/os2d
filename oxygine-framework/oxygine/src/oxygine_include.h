#pragma once
#ifndef OXYGINE_INCLUDE
#define OXYGINE_INCLUDE
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>


//#define OXYGINE_NO_YEILD 1

#if __S3E__
	#define OXYGINE_MARMALADE 1
	#ifdef IW_DEBUG
		#define OX_DEBUG 1
	#endif
#elif EMSCRIPTEN
	#define OXYGINE_EMSCRIPTEN 1
	#ifndef NDEBUG
		#define OX_DEBUG 1
	#endif // DEBUG	
#else
	#define OXYGINE_SDL 1
	#ifdef WIN32
		#ifndef _CRT_SECURE_NO_WARNINGS
			#define _CRT_SECURE_NO_WARNINGS
		#endif
	#endif
#endif


#if _DEBUG || DEBUG
	#ifndef OX_DEBUG
		#define OX_DEBUG 1
	#endif
#endif


#define DYNAMIC_OBJECT_NAME 1


#ifndef OX_DEBUG
	#ifndef EMSCRIPTEN
		#define USE_MEMORY_POOL 1
		#define OBJECT_POOL_ALLOCATOR 1
	#endif
#endif

#if OX_DEBUG
	#define OXYGINE_DEBUG_TRACE_LEAKS 1
	#define OXYGINE_DEBUG_T2P 1
	#define OXYGINE_DEBUG_SAFECAST 1
	#define OXYGINE_TRACE_VIDEO_STATS 1
#endif

#define OXYGINE_ASSERT2LOG 1

#if OXYGINE_ASSERT2LOG
namespace oxygine{namespace log{void error(const char *format, ...);}}
#ifdef OXYGINE_QT
#define OX_ASSERT(x) if (!(x)) {__asm("int3");oxygine::log::error("Assert! %d %s", __LINE__, __FILE__); }(assert(x))
#elif EMSCRIPTEN
#define OX_ASSERT(x) if (!(x)) {oxygine::log::error("Assert! %d %s", __LINE__, __FILE__);}
#else
#define OX_ASSERT(x) if (!(x)) {oxygine::log::error("Assert! %d %s", __LINE__, __FILE__);}(assert(x))
#endif
#else
#define OX_ASSERT(x) (if (!(x)) oxygine::log::error("Assert! %d %s", __LINE__, __FILE__))
#endif

#define OXYGINE_HAS_RESTORE

#ifdef __GNUC__
#define OXYGINE_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
#define OXYGINE_DEPRECATED __declspec(deprecated)
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define OXYGINE_DEPRECATED
#endif

#ifdef OX_WITH_OBJECTSCRIPT

#include <objectscript.h>

struct OS_ClassInfo
{
	const char * classname;
	int class_id;
	int instance_id;

	/*
	OS_ClassInfo()
	{
		classname = "";
		class_id = 0;
		instance_id = 0;
	}
	
	OS_ClassInfo(const OS_ClassInfo& info)
	{
		classname = info.classname;
		class_id = info.class_id;
		instance_id = info.instance_id;
	}
	*/

	OS_ClassInfo(
		const char * classname,
		int class_id,
		int instance_id)
	{
		this->classname = classname;
		this->class_id = class_id;
		this->instance_id = instance_id;
	}
};

#define OS_MAKE_STRING(name) #name
#define OS_CHECK_CLASSNAME(info, type)
	/* if(strcmp(info.classname, OS_MAKE_STRING(type)) != 0){ \
		OX_ASSERT(false && "Error classname " OS_MAKE_STRING(type)); \
	} */

#define OS_DECLARE_CLASSINFO_STATIC_NAME(type, name) \
	static const OS_ClassInfo& getClassInfoStatic(){ \
		static int class_id = (int)(intptr_t)&class_id; \
		static int instance_id = (int)(intptr_t)&instance_id; \
		static OS_ClassInfo class_info(name, class_id, instance_id); \
		return class_info; \
	}

#define OS_DECLARE_CLASSINFO_STATIC(type) \
	OS_DECLARE_CLASSINFO_STATIC_NAME(type, #type)

#define OS_DECLARE_CLASSINFO_NAME(type, name) \
	OS_DECLARE_CLASSINFO_STATIC_NAME(type, name) \
	virtual const OS_ClassInfo& getClassInfo(){ \
		const OS_ClassInfo& info = getClassInfoStatic(); \
		OS_CHECK_CLASSNAME(info, type); \
		return info; \
	}

#define OS_DECLARE_CLASSINFO(type) \
	OS_DECLARE_CLASSINFO_NAME(type, #type)

#include <string>

namespace oxygine { class Object; class EventCallback; class Event; class EventDispatcher; class Actor; class Tween; }

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

#else // OX_WITH_OBJECTSCRIPT

#define OS_DECLARE_CLASSINFO_STATIC_NAME(type, name)
#define OS_DECLARE_CLASSINFO_STATIC(type)
#define OS_DECLARE_CLASSINFO_NAME(type, name)
#define OS_DECLARE_CLASSINFO(type)

#endif // OX_WITH_OBJECTSCRIPT

#endif //OXYGINE_INCLUDE
