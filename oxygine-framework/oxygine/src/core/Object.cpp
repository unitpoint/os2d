#include "Object.h"
#include "oxygine.h"
#include <typeinfo>
#include <stdio.h>
#include "log.h"
#include "Mutex.h"
#include "utils/stringUtils.h"
#include <string.h>

namespace oxygine
{
	ObjectBase::__createdObjects&	ObjectBase::__getCreatedObjects()
	{
		static __createdObjects __objects;
		return __objects;
	}

	int ObjectBase::_lastID = 0;
	int ObjectBase::_assertCtorID = -1;	
	int ObjectBase::_assertDtorID = -1;	
	bool _tracingLeaks = false;

	

	//Mutex mutexDebugList;

	Mutex& getMutex()
	{
		static Mutex mutex;
		return mutex;
	}

#if OBJECT_POOL_ALLOCATOR
	void* PoolObject::operator new(size_t size)
	{
		return fastAlloc(size);			 
	}

	void PoolObject::operator delete(void *data, size_t size)
	{
#ifdef OX_DEBUG
		memset(data, 0xCDCDCDCD, size);
#endif
        
		fastFree(data);
	}
#endif

	string *ObjectBase::__getName() const
	{
#if DYNAMIC_OBJECT_NAME
		if (!__name)
		{
			__name = (string*)fastAlloc(sizeof(string));
			new (__name) string;			
		}
		return __name;
#else
		return &__name;
#endif
	}

	const std::string &ObjectBase::getName() const
	{
		return *__getName();
	}

	bool ObjectBase::isName(const string &name) const
	{
		return isName(name.c_str());
	}

	bool ObjectBase::isName(const char *name) const
	{
#if DYNAMIC_OBJECT_NAME
		if (__name && !strcmp(__name->c_str(), name))
			return true;
#else
		if (__name == name)
			return true;
#endif
		return false;
	}

	void ObjectBase::setName(const string &name)
	{
		string *n = __getName();
		*n = name;
	}

	ObjectBase::ObjectBase(const ObjectBase &src):__id(0), __name(0)
	{
		__addToDebugList(this);

		__userData = src.__userData;
#if DYNAMIC_OBJECT_NAME
		if (src.__name)
			setName(*src.__name);
#else
		setName(src.__name);
#endif
		__generateID();
	}

	void ObjectBase::__removeFromDebugList()
	{
		__removeFromDebugList(this);		
	}

	void ObjectBase::__generateID()
	{
		__id = ++_lastID;

		//OX_ASSERT(_id != 889);
		OX_ASSERT(_assertCtorID != __id);
	}

	ObjectBase::ObjectBase(bool assignID):__userData(0), __id(0)
	{
#if DYNAMIC_OBJECT_NAME
		__name = 0;
#endif
		__addToDebugList(this);

		if (assignID)
			__generateID();
	}

	void ObjectBase::showAssertInCtor(int id)
	{
		_assertCtorID = id;
	}

	void ObjectBase::showAssertInDtor(int id)
	{
		_assertDtorID = id;
	}

	void ObjectBase::__freeName() const
	{
#if DYNAMIC_OBJECT_NAME
		if (__name)
		{
			__name->~string();
			fastFree(__name);		
			__name = 0;
		}
#else
		__name.clear();
#endif
	}

	ObjectBase::~ObjectBase()
	{
		OX_ASSERT(_assertDtorID != __id);

#ifdef OXYGINE_DEBUG_TRACE_LEAKS
		__removeFromDebugList(this);
#endif

		__freeName();
	}

	void ObjectBase::__startTracingLeaks()
	{
		_tracingLeaks = true;
	}

	void ObjectBase::__stopTracingLeaks()
	{
		_tracingLeaks = false;
	}

	void ObjectBase::__addToDebugList(ObjectBase *base)
	{
#ifdef OXYGINE_DEBUG_TRACE_LEAKS		
		if (_tracingLeaks)
		{
			MutexAutoLock m(getMutex());
			base->__traceLeak = true;
			__getCreatedObjects().push_back(base);
		}		
#endif
	}

	void ObjectBase::__removeFromDebugList(ObjectBase *base)
	{
#ifdef OXYGINE_DEBUG_TRACE_LEAKS		
		if (_tracingLeaks)
		{	
			MutexAutoLock m_(getMutex());
			if (base->__traceLeak)
			{
				base->__traceLeak = false;   
				__createdObjects& objs = __getCreatedObjects();
				__createdObjects::iterator i = std::find(objs.begin(), objs.end(), base);
				OX_ASSERT(i != objs.end());
				objs.erase(i);	
			}			
		}		
#endif
	}

	void ObjectBase::dumpObject() const
	{
		const Object *o = dynamic_cast<const Object*>(this);
		string name;

		char refs[16] = "-";
		if (o)
		{
			safe_sprintf(refs, "%d", o->_ref_counter);
#if DYNAMIC_OBJECT_NAME
			if (o->__name)
				name = *o->__name;
#else
			name = o->__name;
#endif
		}

				
		log::messageln("id = %d, name = '%s', typeid = '%s', refs = %s", this->__id, name.c_str(), typeid(*this).name(), refs);
	}

	void ObjectBase::dumpCreatedObjects()
	{
#ifdef OXYGINE_DEBUG_TRACE_LEAKS
		MutexAutoLock m(getMutex());

		log::messageln("\n\n\nallocated objects:");
		int n = 0;		
		__createdObjects& objs = __getCreatedObjects();
		for (__createdObjects::iterator i = objs.begin(); i != objs.end(); ++i)
		{
			ObjectBase *object = *i;
			//log::message("%d)", n);
			object->dumpObject();			

			++n;
		}
		log::message("total: %d -----------------------------\n\n\n", (int)objs.size());
#endif
	}

	Object::Object(const Object &src):ObjectBase(src)
	{
#ifdef OX_DEBUG
		__check = 0xABCDEFAB;
#endif
#ifdef OX_WITH_OBJECTSCRIPT
		osValueId = 0;
#endif
	}

	Object::Object(bool assignUniqueID):ObjectBase(assignUniqueID)
	{
#ifdef OX_DEBUG
		__check = 0xABCDEFAB;
#endif
#ifdef OX_WITH_OBJECTSCRIPT
		osValueId = 0;
#endif
	}
#ifdef OX_DEBUG
	void Object::__doCheck()
	{
		OX_ASSERT(__check == 0xABCDEFAB);
	}
	#endif
}