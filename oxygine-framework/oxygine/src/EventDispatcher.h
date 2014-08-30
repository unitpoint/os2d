#pragma once
#include "oxygine_include.h"
#include <list>
#include "core/Object.h"
#include "closure/closure.h"

namespace oxygine
{
	typedef int eventType;
	DECLARE_SMART(EventDispatcher, spEventDispatcher);	
	
	DECLARE_SMART(Event, spEvent);	
	class Event: public Object
	{
	public:
		OS_DECLARE_CLASSINFO(Event);

		enum {COMPLETE = makefourcc('_', 'E', 'C', 'M')};

		enum Phase
		{
			phase_capturing = 1,
			phase_target,
			phase_bubbling
		};

		eventType type;
		Phase phase;
		bool bubbles;
		bool stopsImmediatePropagation;
		bool stopsPropagation;

		void *userData;
		spObject userDataObject;

		spEventDispatcher target;
		spEventDispatcher currentTarget;// = object with our listener

		Event(eventType Type, bool Bubbles = false):userData(0), type(Type), phase(phase_target), bubbles(Bubbles), stopsImmediatePropagation(false), stopsPropagation(false){}

		virtual ~Event(){}

		void stopPropagation(){stopsPropagation = true;}
		void stopImmediatePropagation(){stopsPropagation = stopsImmediatePropagation = true;}

		virtual Event* clone(){return new Event(*this);}
		virtual void dump()
		{

		}

		eventType getType() const { return type; }
		Phase getPhase() const { return phase; }
		
		spEventDispatcher getTarget() const { return target; }
		void setTarget(spEventDispatcher value){ target = value; }

		spEventDispatcher getCurrentTarget() const { return currentTarget; }
		void setCurrentTarget(spEventDispatcher value){ currentTarget = value; }
	};

#ifndef OX_WITH_OBJECTSCRIPT
	typedef Closure<void (Event *ev)> EventCallback;
#else
	typedef Closure<void (Event *ev)> OriginEventCallback;
	class EventCallback
	{
		bool checkFunctionById()
		{
			os->pushValueById(func_id);
			OX_ASSERT(os->isFunction());
			os->pop();
			return true;
		}

	public:

		OriginEventCallback cb;
		void * p_this; // used by OX

		ObjectScript::OS * os; // not retained
		int func_id; // retained

		EventCallback()
		{
			p_this = NULL;
			os = NULL;
			func_id = 0;
		}

		EventCallback(const OriginEventCallback& _cb): cb(_cb)
		{
			p_this = cb.p_this;
			os = NULL;
			func_id = 0;
		}

		EventCallback(const EventCallback& b): cb(b.cb)
		{
			p_this = cb.p_this;
			os = b.os;
			func_id = b.func_id;
			if(func_id){
				OX_ASSERT(os);
				retainOS(os);
				os->retainValueById(func_id);
				OX_ASSERT(checkFunctionById());
			}
		}

		EventCallback(ObjectScript::OS * _os, int _func_id)
		{
			OX_ASSERT(_os && _func_id);
			p_this = NULL;
			os = retainOS(_os); 
			os->retainValueById(func_id = _func_id);
			OX_ASSERT(checkFunctionById());
		}

		~EventCallback()
		{
			if(func_id){
				OX_ASSERT(os);
				os->releaseValueById(func_id);
				releaseOS(os);
			}
		}

		ObjectScript::OS * retainOS(ObjectScript::OS * os)
		{
			OX_ASSERT(os);
			// extern void retainOSEventCallback(ObjectScript::OS*, EventCallback*);
			retainOSEventCallback(os, this);
			// os->retain();
			return os;
		}

		void releaseOS(ObjectScript::OS * os)
		{
			OX_ASSERT(os);
			// extern void releaseOSEventCallback(ObjectScript::OS*, EventCallback*);
			releaseOSEventCallback(os, this);
			// os->release();
		}

		EventCallback& operator=(const EventCallback& b)
		{
			OX_ASSERT(this != &b);
			cb = b.cb;
			p_this = cb.p_this;

			int old_func_id = func_id;
			ObjectScript::OS * old_os = os;
			func_id = b.func_id;
			os = b.os;
			if(func_id){
				OX_ASSERT(os);
				retainOS(os); // ->retain();
				os->retainValueById(func_id);
				OX_ASSERT(checkFunctionById());
			}
			if(old_func_id){
				OX_ASSERT(old_os);
				old_os->releaseValueById(old_func_id);
				releaseOS(old_os);
			}
			return *this;
		}

		void reset()
		{
			cb = OriginEventCallback();
			p_this = NULL;
			if(func_id){
				OX_ASSERT(os);
				os->releaseValueById(func_id);
				releaseOS(os);
			}
			func_id = 0;
			os = NULL;
		}
		
		void operator()(Event *ev) const
		{
			if(func_id){
				OX_ASSERT(os && !cb);
				// extern void callOSEventFunction(ObjectScript::OS*, int, Event*);
				callOSEventFunction(os, func_id, ev);
			}else{
				OX_ASSERT(!os);
				// OriginEventCallback cb = this->cb;
				cb(ev);
			}
		}

		operator bool() const
		{
			return func_id || cb;
		}

		bool operator==(const EventCallback& b) const
		{
			if(func_id){
				OX_ASSERT(os);
				return func_id == b.func_id && os == b.os;
			}
			return cb == b.cb; 
		}
	};
#endif

	class EventDispatcher: public Object
	{
	public:
		OS_DECLARE_CLASSINFO(EventDispatcher);

		EventDispatcher(const EventDispatcher &ed):Object(ed), _lastID(0), _listeners(0){}
		EventDispatcher();
		~EventDispatcher();

		int addEventListener(eventType, EventCallback);
		void removeEventListener(eventType, EventCallback);
		void removeEventListener(int id);
		void removeEventListeners(void *CallbackThis);
		void removeAllEventListeners();

		virtual void dispatchEvent(Event *event);

		int getListenersCount() const {if (!_listeners) return 0; return _listeners->size();}

	protected:

		struct listener
		{
			EventCallback cb;
			eventType type;
			int id;
		};

		int _lastID;
		
		typedef std::list<listener> listeners;
		listeners *_listeners;
	};
}