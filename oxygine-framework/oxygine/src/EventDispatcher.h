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
		bool checkOSFunction()
		{
			os->pushValueById(os_func_id);
			OX_ASSERT(os->isFunction());
			os->pop();
			return true;
		}

	public:

		OriginEventCallback cb;
		void * p_this; // used by OX

		ObjectScript::OS * os; // not retained
		int os_func_id; // retained

		EventCallback()
		{
			p_this = NULL;
			os = NULL;
			os_func_id = 0;
		}

		EventCallback(const OriginEventCallback& _cb): cb(_cb)
		{
			p_this = cb.p_this;
			os = NULL;
			os_func_id = 0;
		}

		EventCallback(const EventCallback& b): cb(b.cb)
		{
			p_this = cb.p_this;
			os = b.os;
			os_func_id = b.os_func_id;
			OX_ASSERT(os_func_id && os && checkOSFunction() || !os_func_id && !os);
			retainOS();
		}

		EventCallback(ObjectScript::OS * _os, int _func_id)
		{
			OX_ASSERT(_os && _func_id);
			p_this = NULL;
			os = _os; 
			os_func_id = _func_id;
			OX_ASSERT(checkOSFunction());
			retainOS();
		}

		~EventCallback()
		{
			releaseOS();
		}

		void retainOS()
		{
			if(os){
				OX_ASSERT(os_func_id);
				retainOSEventCallback(os, this);
			}
		}

		void releaseOS()
		{
			if(os){
				OX_ASSERT(os_func_id);
				releaseOSEventCallback(os, this);
			}
		}

		EventCallback& operator=(const EventCallback& b)
		{
			OX_ASSERT(this != &b);
			if(*this != b){
				releaseOS();
				
				cb = b.cb;
				p_this = cb.p_this;
				
				os = b.os;
				os_func_id = b.os_func_id;
				
				retainOS();
			}
			return *this;
		}

		void reset()
		{
			releaseOS();
			cb = OriginEventCallback();
			p_this = NULL;
			os = NULL;
			os_func_id = 0;
		}
		
		void operator()(Event *ev) const
		{
			if(os_func_id){
				OX_ASSERT(os && !cb);
				// extern void callOSEventFunction(ObjectScript::OS*, int, Event*);
				callOSEventFunction(os, os_func_id, ev);
			}else{
				OX_ASSERT(!os);
				// OriginEventCallback cb = this->cb;
				cb(ev);
			}
		}

		operator bool() const
		{
			return os_func_id || cb;
		}

		bool operator==(const EventCallback& b) const
		{
			return cb == b.cb && os_func_id == b.os_func_id && os == b.os;
		}

		bool operator!=(const EventCallback& b) const
		{
			return !(*this == b);
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

		int addEventListener(eventType, const EventCallback&);
		void removeEventListener(eventType, const EventCallback&);
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