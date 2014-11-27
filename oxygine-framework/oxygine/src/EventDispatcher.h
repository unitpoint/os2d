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
		void setType(eventType value){ type = value; }

		Phase getPhase() const { return phase; }
		void setPhase(Phase value){ phase = value; }

		spEventDispatcher getTarget() const { return target; }
		void setTarget(spEventDispatcher value){ target = value; }

		spEventDispatcher getCurrentTarget() const { return currentTarget; }
		void setCurrentTarget(spEventDispatcher value){ currentTarget = value; }
	};

	typedef Closure<void (Event *ev)> OriginEventCallback;
	class EventCallback: public OriginEventCallback
	{
	public:

		int os_func_id;

		EventCallback()
		{
			os_func_id = 0;
		}

		EventCallback(const OriginEventCallback& _cb): OriginEventCallback(_cb)
		{
			os_func_id = 0;
		}

		EventCallback(const EventCallback& b): OriginEventCallback(b)
		{
			os_func_id = b.os_func_id;
		}

		EventCallback(int _func_id)
		{
			OX_ASSERT(_func_id);
			os_func_id = _func_id;
		}

		EventCallback& operator=(const EventCallback& b)
		{
			OX_ASSERT(this != &b);
			OriginEventCallback::operator=(b);
			os_func_id = b.os_func_id;
			return *this;
		}

		void operator()(Event *ev) const
		{
			if(os_func_id){
				OX_ASSERT(!OriginEventCallback::operator bool());
				ObjectScript::callEventFunction(os_func_id, ev);
			}else{
				OriginEventCallback::operator()(ev);
			}
		}

		operator bool() const
		{
			return os_func_id || OriginEventCallback::operator bool();
		}

		bool operator!() const
		{
			return !bool(*this);
		}

		bool operator==(const EventCallback& b) const
		{
			return OriginEventCallback::operator==(b) && os_func_id == b.os_func_id;
		}

		bool operator!=(const EventCallback& b) const
		{
			return !(*this == b);
		}
	};

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