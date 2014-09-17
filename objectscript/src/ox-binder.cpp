#include "ox-binder.h"

#ifdef OX_WITH_OBJECTSCRIPT

namespace ObjectScript {

// =====================================================================

std::vector<RegisterFunction> * registerFunctions = NULL;

bool addRegFunc(RegisterFunction func)
{
	if(!registerFunctions){
		registerFunctions = new std::vector<RegisterFunction>();
	}
	if(std::find(registerFunctions->begin(), registerFunctions->end(), func) != registerFunctions->end()){
		OX_ASSERT(false && "function is already registered");
		return false;
	}
	registerFunctions->push_back(func);
	return true;
}

struct CleanupRegisterFunctions
{
	~CleanupRegisterFunctions()
	{
		delete registerFunctions;
		registerFunctions = NULL;
	}
} __cleanupRegisterFunctions;


// =====================================================================

static void registerGlobal(OS * os)
{
	struct Lib {
		static int getSettingCreateCompiledFile(OS * os, int params, int, int, void*)
		{
			os->pushNumber(os->getSetting(OS_SETTING_CREATE_COMPILED_FILE));
			return 0;
		}
		static int setSettingCreateCompiledFile(OS * os, int params, int, int, void*)
		{
			os->setSetting(OS_SETTING_CREATE_COMPILED_FILE, os->toBool(-params+0));
			return 0;
		}

		static int getSettingCreateTextOpcodesFile(OS * os, int params, int, int, void*)
		{
			os->pushNumber(os->getSetting(OS_SETTING_CREATE_TEXT_OPCODES));
			return 0;
		}
		static int setSettingCreateTextOpcodesFile(OS * os, int params, int, int, void*)
		{
			os->setSetting(OS_SETTING_CREATE_TEXT_OPCODES, os->toBool(-params+0));
			return 0;
		}
	};

	OS::FuncDef funcs[] = {
		{"__get@settingCreateCompiledFile", &Lib::getSettingCreateCompiledFile},
		{"__set@settingCreateCompiledFile", &Lib::setSettingCreateCompiledFile},

		{"__get@settingCreateTextOpcodesFile", &Lib::getSettingCreateTextOpcodesFile},
		{"__set@settingCreateTextOpcodesFile", &Lib::setSettingCreateTextOpcodesFile},
		{}
	};
	os->pushGlobals();
	os->setFuncs(funcs);
	os->pop();
}
static bool __registerGlobal = addRegFunc(registerGlobal);

// =====================================================================

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
	registerOXClass<Object>(os, funcs, NULL, true OS_DBG_FILEPOS);
}
static bool __registerObject = addRegFunc(registerObject);

// =====================================================================
/*
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
	registerOXClass<Font, Object>(os, funcs, NULL, true OS_DBG_FILEPOS);
}
static bool __registerFont = addRegFunc(registerFont);
*/
// =====================================================================

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
	registerOXClass<Clock, Object>(os, funcs, NULL, true OS_DBG_FILEPOS);
}
static bool __registerClock = addRegFunc(registerClock);

// =====================================================================

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
	registerOXClass<Event, Object>(os, funcs, NULL, true OS_DBG_FILEPOS);
}
static bool __registerEvent = addRegFunc(registerEvent);

// =====================================================================

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
	registerOXClass<TouchEvent, Event>(os, funcs, nums, true OS_DBG_FILEPOS);
}
static bool __registerTouchEvent = addRegFunc(registerTouchEvent);

// =====================================================================

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
	registerOXClass<TweenEvent, Event>(os, funcs, nums, true OS_DBG_FILEPOS);
}
static bool __registerTweenEvent = addRegFunc(registerTweenEvent);

// =====================================================================

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
	registerOXClass<EventDispatcher, Object>(os, funcs, NULL, true OS_DBG_FILEPOS);
}
static bool __registerEventDispatcher = addRegFunc(registerEventDispatcher);

// =====================================================================

static void registerResource(OS * os)
{
	struct Lib
	{
		static int load(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(Resource*);
			self->load();
			return 0;
		}
	};

	OS::FuncDef funcs[] = {
		{"load", &Lib::load},
		def("unload", &Resource::unload),
		{}
	};
	OS::NumberDef nums[] = {
		{}
	};
	registerOXClass<Resource, Object>(os, funcs, nums, true OS_DBG_FILEPOS);
}
static bool __registerResource = addRegFunc(registerResource);

// =====================================================================

static void registerResAnim(OS * os)
{
	OS::FuncDef funcs[] = {
		// void init(MemoryTexture *original, int columns = 1, int rows = 1, float scaleFactor = 1.0f);
		// void init(animationFrames &frames, int columns, float scaleFactor = 1.0f);
		// void init(spNativeTexture texture, const Point &originalSize, int columns, int rows, float scaleFactor);		
		def("scaleFactor", &ResAnim::getScaleFactor),
		def("columns", &ResAnim::getColumns),
		def("rows", &ResAnim::getRows),
		def("totalFrames", &ResAnim::getTotalFrames),
		// const AnimationFrame&	getFrame(int col, int row) const;
		// const AnimationFrame&	getFrame(int index) const;
		// Resource*				getAtlas(){return _atlas;}
		// void setFrame(int col, int row, const AnimationFrame &frame);
		{}
	};
	OS::NumberDef nums[] = {
		{}
	};
	registerOXClass<ResAnim, Resource>(os, funcs, nums, true OS_DBG_FILEPOS);
}
static bool __registerResAnim = addRegFunc(registerResAnim);

// =====================================================================

static void registerResFont(OS * os)
{
	OS::FuncDef funcs[] = {
		{}
	};
	OS::NumberDef nums[] = {
		{}
	};
	registerOXClass<ResFont, Resource>(os, funcs, nums, true OS_DBG_FILEPOS);
}
static bool __registerResFont = addRegFunc(registerResFont);

// =====================================================================

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

static void registerTween(OS * os)
{
	struct Lib
	{
		static int getDuration(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(Tween*);
			os->pushNumber((float)self->getDuration() / 1000.0f);
			return 1;
		}
	};
	OS::FuncDef funcs[] = {
		// def("__newinstance", &Lib::__newinstance),
		// {"createTween", &Lib::myCreateTween},
		DEF_PROP(loops, Tween, Loops),
		{"__get@duration", &Lib::getDuration},
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
	registerOXClass<Tween, EventDispatcher>(os, funcs, nums, true OS_DBG_FILEPOS);
#undef DEF_EASE
#undef DEF_EASE_DEPRECATED
}
static bool __registerTween = addRegFunc(registerTween);

// =====================================================================

using namespace EaseFunction; // it's really needed by gcc to work DEF_EASE macro 

static void registerEase(OS * os)
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
#undef CONPHYS_CAT_IDENT
}
static bool __registerEase = addRegFunc(registerEase);

// =====================================================================

static void registerTweenQueue(OS * os)
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
			timeMS duration = (timeMS)(os->toFloat(-params+2) * 1000.0f);
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
	registerOXClass<TweenQueue, Tween>(os, funcs, nums, true OS_DBG_FILEPOS);
}
static bool __registerTweenQueue = addRegFunc(registerTweenQueue);

// =====================================================================
/*
static void registerTweenAnim(OS * os)
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
	registerOXClass<TweenAnim, Tween>(os, funcs, nums, true OS_DBG_FILEPOS);
}
static bool __registerTweenAnim = addRegFunc(registerTweenAnim);
*/
// =====================================================================

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
		DEF_PROP(time, UpdateEvent, Time),
		{}
	};
	OS::NumberDef nums[] = {
		{"UPDATE", UpdateEvent::UPDATE},
		{}
	};
	registerOXClass<UpdateEvent, Event>(os, funcs, nums, true OS_DBG_FILEPOS);
}
static bool __registerUpdateEvent = addRegFunc(registerUpdateEvent);

// =====================================================================

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
	registerOXClass<BaseUpdateTween, Tween>(os, funcs, nums, true OS_DBG_FILEPOS);
}
static bool __registerBaseUpdateTween = addRegFunc(registerBaseUpdateTween);

// =====================================================================

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
	registerOXClass<BaseDoneTween, Tween>(os, funcs, nums, true OS_DBG_FILEPOS);
}
static bool __registerBaseDoneTween = addRegFunc(registerBaseDoneTween);

// =====================================================================

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
			timeMS duration = (timeMS)(os->toFloat(-params+2) * 1000.0f);
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

		static int numChildren(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(Actor*);
			int count = 0;
			spActor child = self->getFirstChild();
			for(; child; child = child->getNextSibling()){
				count++;
			}
			os->pushNumber(count);
			return 1;
		}

		static int childAt(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(Actor*);
			int i = os->toInt(-params+0);
			if(i < 0){
				return 0;
			}
			spActor child = self->getFirstChild();
			for(; child; child = child->getNextSibling(), i--){
				if(i == 0){
					pushCtypeValue(os, child);
					return 1;
				}
			}
			return 0;
		}
	};

	OS::FuncDef funcs[] = {
		def("__newinstance", &Lib::__newinstance),

		{"__get@numChildren", &Lib::numChildren},
		{"childAt", &Lib::childAt},

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
	registerOXClass<Actor, EventDispatcher>(os, funcs, NULL, true OS_DBG_FILEPOS);
	// os->eval("Actor.dispatchEvent = EventDispatcher.dispatchEvent");
}
static bool __registerActor = addRegFunc(registerActor);

// =====================================================================

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
	registerOXClass<VStyleActor, Actor>(os, funcs, NULL, true OS_DBG_FILEPOS);

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
	registerOXClass<TextField, VStyleActor>(os, funcs, NULL, true OS_DBG_FILEPOS);
}
static bool __registerTextField = addRegFunc(registerTextField);

// =====================================================================

static void registerSprite(OS * os)
{
	struct Lib { // Actor: public oxygine::Actor
		static Sprite * __newinstance()
		{
			return new Sprite();
		}

		static int getResAnim(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(Sprite*);
			pushCtypeValue(os, (ResAnim*)self->getResAnim());
			return 1;
		}

		static int setResAnimFrameNum(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(Sprite*);
			if(params < 1){
				os->setException("argument required");
				return 0;
			}
			const ResAnim * res = self->getResAnim();
			if(!res){
				os->setException("Requires that the resAnim has been set");
				return 0;
			}
			int num;
			if(params >= 2){
				int col = os->toInt(-params+0); if(col < 0) col = 0; else if(col >= res->getColumns()) col = res->getColumns()-1;
				int row = os->toInt(-params+1); if(row < 0) row = 0; else if(col >= res->getRows()) col = res->getRows()-1;
				self->setAnimFrame(res, col, row);
				num = row * res->getColumns() + col;
			}else{
				num = os->toInt(-params+0); if(num < 0) num = 0; else if(num >= res->getTotalFrames()) num = res->getTotalFrames()-1;
				int col = num % res->getColumns();
				int row = num / res->getColumns();
				self->setAnimFrame(res, col, row);
			}
			// pushCtypeValue(os, self);
			os->pushNumber(num);
			os->setProperty(-params-1-1, "_resAnimFrameNum", false);
			return 0;
		}

		static int getResAnimFrameNum(OS * os, int params, int, int, void*)
		{
			// OS_GET_SELF(Sprite*);
			os->getProperty(-params-1, "_resAnimFrameNum", false);
			os->pushNumber(os->toInt());
			return 1;
		}
	};

	OS::FuncDef funcs[] = {
		def("__newinstance", &Lib::__newinstance),
		def("setResAnim", &Sprite::setResAnim),
		DEF_SET(resAnim, Sprite, ResAnim),
		{"__get@resAnim", &Lib::getResAnim},
		{"__get@resAnimFrameNum", &Lib::getResAnimFrameNum},
		{"__set@resAnimFrameNum", &Lib::setResAnimFrameNum},
		{}
	};
	registerOXClass<Sprite, VStyleActor>(os, funcs, NULL, true OS_DBG_FILEPOS);
}
static bool __registerSprite = addRegFunc(registerSprite);

// =====================================================================

static void registerButton(OS * os)
{
	OS::FuncDef funcs[] = {
		DEF_PROP(row, Button, Row), 
		{}
	};
	OS::NumberDef nums[] = {
		{}
	};
	registerOXClass<Button, Actor>(os, funcs, nums, true OS_DBG_FILEPOS);
}
static bool __registerButton = addRegFunc(registerButton);

// =====================================================================

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
	registerOXClass<Stage, Actor>(os, funcs, nums, true OS_DBG_FILEPOS);
}
static bool __registerStage = addRegFunc(registerStage);

// =====================================================================

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
			bool loadAll = params >= 2 ? os->toBool(params+1) : true;
			bool autoManage = params >= 3 ? os->toBool(params+2) : !loadAll;
			self->loadXML(name.toChar(), NULL, loadAll, autoManage);
			return 0;
		}

		static int get(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(Resources*);
			if(params < 1){
				os->setException("string argument required");
				return 0;
			}
			OS::String name = os->toString(-params+0);
			error_policy ep = CtypeValue<error_policy>::getArg(os, -params+1);
			pushCtypeValue(os, self->get(name.toChar(), ep));
			return 1;
		}			
	};
	OS::FuncDef funcs[] = {
		def("__newinstance", &Lib::__newinstance),
		{"loadXML", &Lib::loadXML},
		{"get", &Lib::get},
		def("getResAnim", &Resources::getResAnim),
		{}
	};
	OS::NumberDef nums[] = {
		{}
	};
	registerOXClass<Resources, Resource>(os, funcs, nums, true OS_DBG_FILEPOS);
}
static bool __registerResources = addRegFunc(registerResources);

// =====================================================================

OS2D * os;

} // namespace ObjectScript

void registerOSCallback(oxygine::EventDispatcher * ed, int id, int funcId)
{
	OX_ASSERT(dynamic_cast<OS2D*>(ObjectScript::os));
	ObjectScript::OS * os = ObjectScript::os;
	if(!ed->osValueId && funcId){
		ObjectScript::pushCtypeValue(os, ed);
		os->pop();
	}
	if(ed->osValueId && funcId){
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
		os->pushValueById(funcId);
		OX_ASSERT(os->isFunction());
		os->call(2);
		os->handleException();
	}
}

void unregisterOSCallback(oxygine::EventDispatcher * ed, int id, int funcId)
{
	OX_ASSERT(dynamic_cast<OS2D*>(ObjectScript::os));
	ObjectScript::OS * os = ObjectScript::os;
	if(ed->osValueId && funcId){
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
		os->pushValueById(funcId);
		OX_ASSERT(os->isFunction());
		os->call(2);
		os->handleException();
	}
}

void registerOSEventCallback(EventDispatcher * ed, int id, const EventCallback& cb)
{
	registerOSCallback(ed, id, cb.os_func_id);
}

void unregisterOSEventCallback(EventDispatcher * ed, int id, const EventCallback& cb)
{
	unregisterOSCallback(ed, id, cb.os_func_id);
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

static int OS_maxAllocatedBytes = 0;
static int OS_maxUsedBytes = 0;
static int OS_maxNumValues = 0;

std::string getOSDebugStr()
{
	std::stringstream s;
	OS2D * os = ObjectScript::os;
	if(os){
		if(OS_maxAllocatedBytes < os->getAllocatedBytes()){
			OS_maxAllocatedBytes = os->getAllocatedBytes();
		}
		if(OS_maxUsedBytes < os->getUsedBytes()){
			OS_maxUsedBytes = os->getUsedBytes();
		}
		if(OS_maxNumValues < os->getNumValues()){
			OS_maxNumValues = os->getNumValues();
		}
		s << endl;
		s << "OS GC max values: " << OS_maxNumValues << endl;
		s << "OS MEM (Kb) max";
		s << " allocated:" << (OS_maxAllocatedBytes / 1024);
		s << " used:" << (OS_maxUsedBytes / 1024);
		// s << endl;
	}
	return s.str();
}

#endif // OX_WITH_OBJECTSCRIPT
