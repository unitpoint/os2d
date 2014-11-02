#include "ox-binder.h"
#include <sstream>

/*
oxygine-framework merge state
SHA 06446049
by dmuratshin, 18.10.2014 12:00
*/


// #ifdef OX_WITH_OBJECTSCRIPT

#ifdef OXYGINE_SDL
namespace oxygine { extern SDL_Window * _window; }
extern oxygine::Rect viewport; extern oxygine::Renderer renderer;
#endif

using namespace oxygine;

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

		static int setWindowSize(OS * os, int params, int, int, void*)
		{
#ifdef OXYGINE_SDL
			int width = (int)getStage()->getWidth();
			int height = (int)getStage()->getHeight();
			if(os->isObject(-params+0)){
				width = (os->getProperty(-params+0, "x"), os->popInt(width));
				height = (os->getProperty(-params+0, "y"), os->popInt(height));
			}else{
				width = os->toInt(-params+0, width);
				height = os->toInt(-params+0, height);
			}

			SDL_SetWindowSize(oxygine::_window, width, height);
			SDL_GetWindowSize(oxygine::_window, &width, &height);

			viewport = Rect(0, 0, width, height);
			renderer.initCoordinateSystem(width, height);
#endif
			return 0;
		}

		static int getWindowSize(OS * os, int params, int, int, void*)
		{
			int width, height;
#ifdef OXYGINE_SDL
			SDL_GetWindowSize(oxygine::_window, &width, &height);
#else
			width = (int)getStage()->getWidth();
			height = (int)getStage()->getHeight();
#endif
			pushCtypeValue(os, Vector2((float)width, (float)height));
			return 1;
		}
	};

	OS::FuncDef funcs[] = {
		{"__get@settingCreateCompiledFile", &Lib::getSettingCreateCompiledFile},
		{"__set@settingCreateCompiledFile", &Lib::setSettingCreateCompiledFile},

		{"__get@settingCreateTextOpcodesFile", &Lib::getSettingCreateTextOpcodesFile},
		{"__set@settingCreateTextOpcodesFile", &Lib::setSettingCreateTextOpcodesFile},

		{"setWindowSize", &Lib::setWindowSize},
		{"getWindowSize", &Lib::getWindowSize},
		{}
	};
	OS::NumberDef nums[] = {
		{"TEXT_HALIGN_DEFAULT", TextStyle::HALIGN_DEFAULT},
		{"TEXT_HALIGN_LEFT", TextStyle::HALIGN_LEFT},
		{"TEXT_HALIGN_MIDDLE", TextStyle::HALIGN_MIDDLE},
		{"TEXT_HALIGN_CENTER", TextStyle::HALIGN_CENTER},
		{"TEXT_HALIGN_RIGHT", TextStyle::HALIGN_RIGHT},
		{"TEXT_VALIGN_DEFAULT", TextStyle::VALIGN_DEFAULT},
		{"TEXT_VALIGN_BASELINE", TextStyle::VALIGN_BASELINE},
		{"TEXT_VALIGN_TOP", TextStyle::VALIGN_TOP},
		{"TEXT_VALIGN_MIDDLE", TextStyle::VALIGN_MIDDLE},
		{"TEXT_VALIGN_BOTTOM", TextStyle::VALIGN_BOTTOM},
		{}
	};
	os->pushGlobals();
	os->setFuncs(funcs);
	os->setNumbers(nums);
	os->pop();
}
static bool __registerGlobal = addRegFunc(registerGlobal);

// =====================================================================

// Object.__get is implemented in "std.os"

/* static int getUnknown(OS * os, int params, int, int, void*)
{
	os->setException(OS::String::format(os, "property \"%s\" not found in \"%s\"", 
		os->toString(-params+0).toChar(),
		os->getValueNameOrClassname(-params-1).toChar()));
	return 0;
} */

static void registerObject(OS * os)
{
	struct Lib {
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
		// {"__get", &getUnknown},
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
		static Clock * __newinstance()
		{
			return new Clock();
		}
	};

	OS::FuncDef funcs[] = {
		def("__newinstance", &Lib::__newinstance),
		DEF_GET("time", Clock, Time),
		DEF_GET("pauseCounter", Clock, PauseCounter),
		DEF_PROP("fixedStep", Clock, FixedStep),
		DEF_PROP("multiplier", Clock, Multiplier),
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
		DEF_PROP("type", Event, Type),
		DEF_PROP("phase", Event, Phase),
		DEF_PROP("target", Event, Target),
		DEF_PROP("currentTarget", Event, CurrentTarget),
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
		DEF_PROP("localPosition", TouchEvent, LocalPosition),	// deprecated
		DEF_PROP("position", TouchEvent, Position),				// deprecated
		DEF_PROP("localPos", TouchEvent, LocalPosition),
		DEF_PROP("pos", TouchEvent, Position),
		DEF_PROP("pressure", TouchEvent, Pressure),
		DEF_PROP("mouseButton", TouchEvent, MouseButton),
		DEF_PROP("index", TouchEvent, Index),
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
		DEF_GET("actor", TweenEvent, Actor),
		DEF_GET("tween", TweenEvent, Tween),
		DEF_GET("us", TweenEvent, UpdateState),
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
			os->pushStackValue(offs);
			if(params == 1 || os->isObject(offs)){
				os->callF(1, 1);
			}else{
				os->pushStackValue(offs+1);
				os->callF(2, 1);
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
		DEF_GET("listenersCount", EventDispatcher, ListenersCount),
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
		DEF_GET("scaleFactor", ResAnim, ScaleFactor),
		DEF_GET("columns", ResAnim, Columns),
		DEF_GET("rows", ResAnim, Rows),
		DEF_GET("totalFrames", ResAnim, TotalFrames),
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
		DEF_PROP("loops", Tween, Loops),
		{"__get@duration", &Lib::getDuration},
		DEF_PROP("ease", Tween, Ease),
		DEF_PROP("delay", Tween, Delay),
		DEF_PROP("client", Tween, Client),
		DEF_GET("percent", Tween, Percent),
		DEF_PROP("dataObject", Tween, DataObject),
		DEF_GET("nextSibling", Tween, NextSibling),
		DEF_GET("prevSibling", Tween, PrevSibling),
		def("__get@isStarted", &Tween::isStarted),
		def("__get@isDone", &Tween::isDone),
		// {"addDoneCallback", &Lib::addDoneCallback},
		def("addDoneCallback", &Tween::addDoneCallback),
		DEF_PROP("doneCallback", Tween, DoneCallback),
		DEF_PROP("detachActor", Tween, DetachActor),
		DEF_PROP("detachTarget", Tween, DetachActor),
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
		DEF_EASE(Sin, SINE),
		DEF_EASE(Expo, EXPO),
		DEF_EASE(Circ, CIRC),
		DEF_EASE(Back, BACK),
		DEF_EASE(Bounce, BOUNCE),

		DEF_EASE_DEPRECATED(Quad, QUAD),
		DEF_EASE_DEPRECATED(Cubic, CUBIC),
		DEF_EASE_DEPRECATED(Quart, QUART),
		DEF_EASE_DEPRECATED(Quint, QUINT),
		DEF_EASE_DEPRECATED(Sin, SINE),
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
		// {"__get", &getUnknown},
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

KeyboardEvent::KeyboardEvent(int type): Event(type)
{
}

KeyboardEvent::KeyboardEvent(int type, const SDL_KeyboardEvent& p_key): Event(type), key(p_key)
{
	makeStr();
}

void KeyboardEvent::makeStr()
{
	int code = key.keysym.sym;
	str[0] = (char)(code & 0xff);
	str[1] = (char)((code>>8) & 0xff);
	str[2] = (char)((code>>16) & 0xff);
	str[3] = (char)((code>>24) & 0xff);
	str[4] = '\0';
}

void KeyboardEvent::process(Event * p_ev, EventDispatcher * ed)
{
	struct Lib
	{
		static void dispatchEvent(EventDispatcher * ed, int type, const SDL_KeyboardEvent& key)
		{
			KeyboardEvent keyEvent(type, key);
			ed->dispatchEvent(&keyEvent);
		}
	};

	SDL_Event * ev = dynamic_cast<SDL_Event*>((SDL_Event*)p_ev->userData);
	if(!ev){
		OX_ASSERT(false);
		return;
	}
	switch(ev->type){
	case SDL_KEYDOWN:
		Lib::dispatchEvent(ed, DOWN, ev->key);
		break;

	case SDL_KEYUP:
		Lib::dispatchEvent(ed, UP, ev->key);
		break;

	case SDL_TEXTEDITING:
		Lib::dispatchEvent(ed, TEXTEDITING, ev->key);
		break;

	case SDL_TEXTINPUT:
		Lib::dispatchEvent(ed, TEXTINPUT, ev->key);
		break;
	}
}

static void registerKeyboardEvent(OS * os)
{
	struct Lib {
		static KeyboardEvent * __newinstance()
		{
			return new KeyboardEvent(0);
		}
	};

	OS::FuncDef funcs[] = {
		def("__newinstance", &Lib::__newinstance),
		DEF_GET("scancode", KeyboardEvent, Scancode),
		DEF_GET("sym", KeyboardEvent, Sym),
		DEF_GET("mod", KeyboardEvent, Mod),
		DEF_GET("str", KeyboardEvent, Str),
		{}
	};
	OS::NumberDef nums[] = {
		{"DOWN", KeyboardEvent::DOWN},
		{"UP", KeyboardEvent::UP},
		{"TEXTEDITING", KeyboardEvent::TEXTEDITING},
		{"TEXTINPUT", KeyboardEvent::TEXTINPUT},
#include "KeyboardEventCodes.inc"
		{}
	};
	registerOXClass<KeyboardEvent, Event>(os, funcs, nums, true OS_DBG_FILEPOS);
}
static bool __registerKeyboardEvent = addRegFunc(registerKeyboardEvent);

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
		DEF_PROP("us", UpdateEvent, UpdateState),
		DEF_PROP("tween", UpdateEvent, Tween),
		DEF_PROP("dt", UpdateEvent, Dt),
		DEF_PROP("time", UpdateEvent, Time),
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
		DEF_PROP("updateCallback", BaseUpdateTween, UpdateCallback),
		DEF_PROP("interval", BaseUpdateTween, Interval),
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
		DEF_SET("duration", BaseDoneTween, Duration), 
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

		static int numChildren(Actor * actor)
		{
			int count = 0;
			spActor child = actor->getFirstChild();
			for(; child; child = child->getNextSibling()){
				count++;
			}
			return count;
		}

		static int numChildren(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(Actor*);
			os->pushNumber(numChildren(self));
			return 1;
		}

		static int getChild(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(Actor*);
			OS_EValueType type = os->getType(-params+0);
			if(type == OS_VALUE_TYPE_NUMBER){
				int i = os->toInt(-params+0);
				if(i < 0 && (i += numChildren(self)) < 0){
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
			if(type == OS_VALUE_TYPE_STRING){
				spActor child = self->getChild(os->toString(-params+0).toChar());
				pushCtypeValue(os, child);
				return 1;
			}
			os->setException("string or number required");
			return 0;
		}

		static int localToGlobal(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(Actor*);
			if(params < 1){
				os->setException("argument required");
				return 0;
			}
			Vector2 pos(0, 0);
			Actor * root = CtypeValue<Actor*>::getArg(os, -params+0);
			if(!root){
				pos = CtypeValue<Vector2>::getArg(os, -params+0);
				root = CtypeValue<Actor*>::getArg(os, -params+1);
				/* if(!root){
					os->setException("Actor required for second argument");
					return 0;
				} */
			}
			if(os->isExceptionSet()){
				return 0;
			}
			pos = convert_local2stage(self, pos, root);
			pushCtypeValue(os, pos);
			return 1;
		}

		static int globalToLocal(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(Actor*);
			if(params < 1){
				os->setException("argument required");
				return 0;
			}
			Vector2 pos(0, 0);
			Actor * root = CtypeValue<Actor*>::getArg(os, -params+0);
			if(!root){
				pos = CtypeValue<Vector2>::getArg(os, -params+0);
				root = CtypeValue<Actor*>::getArg(os, -params+1);
				/* if(!root){
					os->setException("Actor required for second argument");
					return 0;
				} */
			}
			if(os->isExceptionSet()){
				return 0;
			}
			pos = convert_stage2local(self, pos, root);
			pushCtypeValue(os, pos);
			return 1;
		}

		static int localToLocal(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(Actor*);
			if(params < 1){
				os->setException("argument required");
				return 0;
			}
			Vector2 pos(0, 0);
			Actor * other = CtypeValue<Actor*>::getArg(os, -params+0);
			if(!other){
				pos = CtypeValue<Vector2>::getArg(os, -params+0);
				other = CtypeValue<Actor*>::getArg(os, -params+1);
				if(!other){
					os->setException("Actor required for second argument");
					return 0;
				}
			}
			if(os->isExceptionSet()){
				return 0;
			}
			pos = convert_local2stage(self, pos);
			pos = convert_stage2local(other, pos);  
			pushCtypeValue(os, pos);
			return 1;
		}

		static int changeParentAndSavePos(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(Actor*);
			if(params < 1){
				os->setException("argument required");
				return 0;
			}
			Actor * newParent = CtypeValue<Actor*>::getArg(os, -params+0);
			if(!newParent){
				os->setException("Actor argument required");
				return 0;
			}
			changeParentAndSavePosition(NULL, self, newParent);
			return 0;
		}

		static spActor findActorByPos(OS * os, spActor self, const Vector2& pos, int funcId)
		{
			for(spActor child = self->getLastChild(); child && !os->isExceptionSet(); child = child->getPrevSibling()){
				Vector2 childPos = child->global2local(pos);
				spActor foundChild = findActorByPos(os, child, childPos, funcId);
				if(foundChild){
					return foundChild;
				}
				if(child->isOn(childPos)){
					os->pushValueById(funcId);
					pushCtypeValue(os, child);
					os->callF(1, 1);
					if(os->toBool()){
						return child;
					}
				}
			}
			return NULL;
		}

		static int findActorByPos(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(Actor*);
			if(params < 2){
				os->setException("two arguments required");
				return 0;
			}
			Vector2 pos = CtypeValue<Vector2>::getArg(os, -params+0);
			if(os->isExceptionSet()){
				return 0;
			}
			if(!os->isFunction(-params+1)){
				os->setException("function required for second argument");
				return 0;
			}
			int funcId = os->getValueId(-params+1);
			pushCtypeValue(os, findActorByPos(os, self, pos, funcId));
			return 1;
		}
	};

	OS::FuncDef funcs[] = {
		def("__newinstance", &Lib::__newinstance),

		{"__get@numChildren", &Lib::numChildren},
		// {"childAt", &Lib::childAt},

		DEF_GET("firstChild", Actor, FirstChild),
		DEF_GET("lastChild", Actor, LastChild),
		DEF_GET("nextSibling", Actor, NextSibling),
		DEF_GET("prevSibling", Actor, PrevSibling),
		def("getDescendant", &Actor::getDescendant),
		{"getChild", &Lib::getChild},
		def("getTween", &Actor::getTween),
		DEF_GET("firstTween", Actor, FirstTween),
		DEF_GET("lastTween", Actor, LastTween),
		
		// deprecated
		DEF_GET("anchor", Actor, Anchor),
		def("__set@anchor", (void(Actor::*)(const Vector2 &))&Actor::setAnchor),
		def("__get@isAnchorInPixels", &Actor::getIsAnchorInPixels),

		DEF_GET("pivot", Actor, Anchor),
		def("__set@pivot", (void(Actor::*)(const Vector2 &))&Actor::setAnchor),
		def("__get@isPivotInPixels", &Actor::getIsAnchorInPixels),

		def("__get@pos", &Actor::getPosition),
		def("__set@pos", (void(Actor::*)(const Vector2 &))&Actor::setPosition),
		DEF_PROP("x", Actor, X),
		DEF_PROP("y", Actor, Y),
		
		DEF_GET("scale", Actor, Scale),
		def("__set@scale", (void(Actor::*)(const Vector2 &))&Actor::setScale),
		DEF_PROP("scaleX", Actor, ScaleX),
		DEF_PROP("scaleY", Actor, ScaleY),

		DEF_PROP("rotation", Actor, Rotation),					// deprecated
		DEF_PROP("rotationDegrees", Actor, RotationDegrees),	// deprecated
		DEF_PROP("angle", Actor, RotationDegrees),

		DEF_PROP("priority", Actor, Priority),
		DEF_PROP("visible", Actor, Visible),
		DEF_PROP("cull", Actor, Cull),

		def("__get@parent", &Actor::getParent),
		// def("__set@parent", (void(Actor::*)(Actor*))&Actor::attachTo),
		{"__set@parent", &Lib::setParent},

		DEF_GET("size", Actor, Size),
		def("__set@size", (void(Actor::*)(const Vector2 &))&Actor::setSize),
		DEF_PROP("width", Actor, Width),
		DEF_PROP("height", Actor, Height),

		DEF_PROP("opacity", Actor, Opacity),
		DEF_PROP("alpha", Actor, Alpha),
		// {"__get@alpha", &Lib::getAlpha},
		// {"__set@alpha", &Lib::setAlpha},

		DEF_PROP("clock", Actor, Clock),

		// virtual RectF		getDestRect() const;

		DEF_PROP("touchEnabled", Actor, TouchEnabled),
		DEF_PROP("touchChildrenEnabled", Actor, TouchChildrenEnabled),
		DEF_PROP("childrenRelative", Actor, ChildrenRelative),

		// const Renderer::transform&		getTransform() const;
		// const Renderer::transform&		getTransformInvert() const;
		// void setTransform(const AffineTransform &tr);

		DEF_PROP("extendedClickArea", Actor, ExtendedClickArea),

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

		DEF_PROP("pressedTouchIndex", Actor, Pressed),
		DEF_PROP("overedTouchIndex", Actor, Overed),

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

		{"globalToLocal", &Lib::globalToLocal},
		{"localToGlobal", &Lib::localToGlobal},
		{"localToLocal", &Lib::localToLocal},

		{"changeParentAndSavePos", &Lib::changeParentAndSavePos},
		{"findActorByPos", &Lib::findActorByPos},

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
		DEF_PROP("blendMode", Sprite, BlendMode),
		DEF_PROP("color", Sprite, Color),
		{}
	};
	registerOXClass<VStyleActor, Actor>(os, funcs, NULL, true OS_DBG_FILEPOS);

	OS::NumberDef globalNums[] = {
		{"BLEND_DEFAULT", blend_default},
		{"BLEND_DISABLED", blend_disabled},
		{"BLEND_PREMULTIPLIED_ALPHA", blend_premultiplied_alpha},
		{"BLEND_ALPHA", blend_alpha},
		{"BLEND_ADD", blend_add},
		{"BLEND_MULTIPLY", blend_multiply},
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

		static int getTextPos(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(TextField*);
			pushCtypeValue(os, self->getTextRect().pos);
			return 1;
		}

		static int getTextSize(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(TextField*);
			pushCtypeValue(os, self->getTextRect().size);
			return 1;
		}
	};

	OS::FuncDef funcs[] = {
		def("__newinstance", &Lib::__newinstance),
		DEF_PROP("style", TextField, Style),
		// const Rect&					getTextRect();
		DEF_GET("text", TextField, Text),
		{"__get@textPos", &Lib::getTextPos},
		{"__get@textSize", &Lib::getTextSize},
		def("__set@text", (void(TextField::*)(const string &))&TextField::setText),
		def("__set@htmlText", (void(TextField::*)(const string &))&TextField::setHtmlText),
		DEF_PROP("fontSize2Scale", TextField, FontSize2Scale),
		DEF_PROP("vAlign", TextField, VAlign),
		DEF_PROP("hAlign", TextField, HAlign),
		DEF_PROP("multiline", TextField, Multiline),
		DEF_PROP("breakLongWords", TextField, BreakLongWords),
		DEF_PROP("linesOffset", TextField, LinesOffset),
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

		static int setResAnim(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(Sprite*);
			ResAnim * resAnim = NULL;
			if(!os->isNull(-params+0)){
				resAnim = CtypeOXClass<ResAnim*>::getArg(os, -params+0);
				if(!resAnim){
					os->setException("ResAnim required");
					return 0;
				}
			}
			self->setResAnim(resAnim);
			return 0;
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

		static int setAnimFrameRect(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(Sprite*);
			AnimationFrame frame = self->getAnimFrame();
			float w = (float)frame.getDiffuse().base->getWidth();
			float h = (float)frame.getDiffuse().base->getHeight();
			ResAnim * resAnim = frame.getResAnim();
			float sc = resAnim ? resAnim->getScaleFactor() : 1.0f;
			RectF rect = frame.getSrcRect() * Vector2(w, h);
			if(params == 4){
				rect.pos.x += os->toFloat(-params+0) * sc;
				rect.pos.y += os->toFloat(-params+1) * sc;
				rect.size.x = os->toFloat(-params+2);
				rect.size.y = os->toFloat(-params+3);
			}else if(params == 2){
				rect.pos += CtypeValue<Vector2>::getArg(os, -params+0);
				rect.size = CtypeValue<Vector2>::getArg(os, -params+1);
			}else{
				os->setException("two or four arguments required");
				return 0;
			}
			if(!os->isExceptionSet()){
				frame.setSrcRect(rect / Vector2(w, h));
				// frame.setFrameSize(rect.size);
				self->setAnimFrame(frame);
			}
			return 0;
		}
	};

	OS::FuncDef funcs[] = {
		def("__newinstance", &Lib::__newinstance),
		// def("setResAnim", &Sprite::setResAnim),
		// DEF_SET("resAnim", Sprite, ResAnim),
		{"__get@resAnim", &Lib::getResAnim},
		{"__set@resAnim", &Lib::setResAnim},
		{"__get@resAnimFrameNum", &Lib::getResAnimFrameNum},
		{"__set@resAnimFrameNum", &Lib::setResAnimFrameNum},
		{"setAnimFrameRect", &Lib::setAnimFrameRect},
		{}
	};
	registerOXClass<Sprite, VStyleActor>(os, funcs, NULL, true OS_DBG_FILEPOS);
}
static bool __registerSprite = addRegFunc(registerSprite);

// =====================================================================

static void registerBox9Sprite(OS * os)
{
	struct Lib
	{
		static Box9Sprite * __newinstance()
		{
			return new Box9Sprite();
		}
	};
	OS::FuncDef funcs[] = {
		def("__newinstance", &Lib::__newinstance),
		DEF_PROP("verticalMode", Box9Sprite, VerticalMode),
		DEF_PROP("horizontalMode", Box9Sprite, HorizontalMode),
		def("setGuides", &Box9Sprite::setGuides),
		def("setVerticalGuides", &Box9Sprite::setVerticalGuides),
		def("setHorizontalGuides", &Box9Sprite::setHorizontalGuides),
		{}
	};
	OS::NumberDef nums[] = {
		{}
	};
	registerOXClass<Box9Sprite, Sprite>(os, funcs, nums, true OS_DBG_FILEPOS);
}
static bool __registerBox9Sprite = addRegFunc(registerBox9Sprite);

// =====================================================================

static void registerColorRectSprite(OS * os)
{
	struct Lib
	{
		static ColorRectSprite * __newinstance()
		{
			return new ColorRectSprite();
		}
	};
	OS::FuncDef funcs[] = {
		def("__newinstance", &Lib::__newinstance),
		{}
	};
	OS::NumberDef nums[] = {
		{}
	};
	registerOXClass<ColorRectSprite, Sprite>(os, funcs, nums, true OS_DBG_FILEPOS);
}
static bool __registerColorRectSprite = addRegFunc(registerColorRectSprite);

// =====================================================================

static void registerButton(OS * os)
{
	struct Lib
	{
		static Button * __newinstance()
		{
			return new Button();
		}
	};
	OS::FuncDef funcs[] = {
		def("__newinstance", &Lib::__newinstance),
		DEF_PROP("row", Button, Row), 
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

void registerCallback(Object * obj, int id, const char * name, int funcId, const char * internalFuncName)
{
	OX_ASSERT(dynamic_cast<OS2D*>(os));
	if(!obj->osValueId && funcId){
		ObjectScript::pushCtypeValue(os, obj);
		OX_ASSERT(obj->osValueId);
		os->pop();
	}
	if(obj->osValueId && funcId){
#ifdef OS_DEBUG
		ObjectScript::pushCtypeValue(os, obj);
		OX_ASSERT(os->getValueId() == obj->osValueId);
		os->pop();
#endif
		os->pushValueById(obj->osValueId); // this
		os->getProperty(-1, internalFuncName ? internalFuncName : "_registerExternalCallback"); // func
		OX_ASSERT(os->isFunction());
		name ? os->pushString(name) : os->pushNumber(id);
		os->pushValueById(funcId);
		OX_ASSERT(os->isFunction());
		os->callTF(2);
		os->handleException();
	}
}

void unregisterCallback(Object * obj, int id, const char * name, int funcId, const char * internalFuncName)
{
	OX_ASSERT(dynamic_cast<OS2D*>(os));
	if(obj->osValueId && funcId){
#ifdef OS_DEBUG
		ObjectScript::pushCtypeValue(os, obj);
		OX_ASSERT(os->getValueId() == obj->osValueId);
		os->pop();
#endif
		os->pushValueById(obj->osValueId); // this
		os->getProperty(-1, internalFuncName ? internalFuncName : "_unregisterExternalCallback"); // func
		OX_ASSERT(os->isFunction());
		name ? os->pushString(name) : os->pushNumber(id);
		os->pushValueById(funcId);
		OX_ASSERT(os->isFunction());
		os->callTF(2);
		os->handleException();
	}
}

void registerCallback(oxygine::Object * obj, const char * name, int funcId)
{
	registerCallback(obj, 0, name, funcId, NULL);
}

void unregisterCallback(oxygine::Object * obj, const char * name, int funcId)
{
	unregisterCallback(obj, 0, name, funcId, NULL);
}

void registerCallback(Object * obj, int id, int funcId)
{
	registerCallback(obj, id, NULL, funcId, NULL);
}

void unregisterCallback(Object * obj, int id, int funcId)
{
	unregisterCallback(obj, id, NULL, funcId, NULL);
}

void registerEventCallback(Object * obj, const char * name, const EventCallback& cb)
{
	registerCallback(obj, 0, name, cb.os_func_id, "_registerExternalEventCallback");
}

void unregisterEventCallback(Object * obj, const char * name, const EventCallback& cb)
{
	unregisterCallback(obj, 0, name, cb.os_func_id, "_unregisterExternalEventCallback");
}

void registerEventCallback(Object * obj, int id, const EventCallback& cb)
{
	registerCallback(obj, id, NULL, cb.os_func_id, "_registerExternalEventCallback");
}

void unregisterEventCallback(Object * obj, int id, const EventCallback& cb)
{
	unregisterCallback(obj, id, NULL, cb.os_func_id, "_unregisterExternalEventCallback");
}

void unregisterAllEventCallbacks(Object * obj)
{
	OX_ASSERT(dynamic_cast<OS2D*>(os));
	if(obj->osValueId){
#ifdef OS_DEBUG
		ObjectScript::pushCtypeValue(os, obj);
		OX_ASSERT(os->getValueId() == obj->osValueId);
		os->pop();
#endif
		os->pushValueById(obj->osValueId); // this
		os->getProperty(-1, "_unregisterAllExternalEventCallbacks"); // func
		OX_ASSERT(os->isFunction());
		os->callTF();
		os->handleException();
	}
}

void registerTween(Object * a, Tween * t)
{
	OX_ASSERT(dynamic_cast<OS2D*>(os));
	ObjectScript::pushCtypeValue(os, a); // this
	OX_ASSERT(os->getValueId() == a->osValueId);
	os->getProperty(-1, "_registerExternalTween"); // func
	OX_ASSERT(os->isFunction());
	ObjectScript::pushCtypeValue(os, t);
	OX_ASSERT(os->getValueId() == t->osValueId);
	os->callTF(1);
	os->handleException();
}

void unregisterTween(Object * a, Tween * t)
{
	OX_ASSERT(dynamic_cast<OS2D*>(os));
	if(a->osValueId && t->osValueId){
#ifdef OS_DEBUG
		ObjectScript::pushCtypeValue(os, a);
		OX_ASSERT(os->getValueId() == a->osValueId);
		os->pop();

		ObjectScript::pushCtypeValue(os, t);
		OX_ASSERT(os->getValueId() == t->osValueId);
		os->pop();
#endif
		os->pushValueById(a->osValueId); // this
		os->getProperty(-1, "_unregisterExternalTween"); // func
		OX_ASSERT(os->isFunction());
		os->pushValueById(t->osValueId);
		os->callTF(1);
		os->handleException();
	}
}

void unregisterAllTweens(Object * a)
{
	OX_ASSERT(dynamic_cast<OS2D*>(os));
	if(a->osValueId){
#ifdef OS_DEBUG
		ObjectScript::pushCtypeValue(os, a);
		OX_ASSERT(os->getValueId() == a->osValueId);
		os->pop();
#endif
		os->pushValueById(a->osValueId); // this
		os->getProperty(-1, "_unregisterAllExternalTweens"); // func
		OX_ASSERT(os->isFunction());
		os->callTF();
		os->handleException();
	}
}

void registerActorChild(Actor * a, Actor * child)
{
	OX_ASSERT(dynamic_cast<OS2D*>(os));
	ObjectScript::pushCtypeValue(os, a); // this
	OX_ASSERT(os->getValueId() == a->osValueId);
	os->getProperty(-1, "_registerExternalChild"); // func
	OX_ASSERT(os->isFunction());
	ObjectScript::pushCtypeValue(os, child);
	OX_ASSERT(os->getValueId() == child->osValueId);
	os->callTF(1);
	os->handleException();
}

void unregisterActorChild(Actor * a, Actor * child)
{
	OX_ASSERT(dynamic_cast<OS2D*>(os));
	if(a->osValueId && child->osValueId){
#ifdef OS_DEBUG
		ObjectScript::pushCtypeValue(os, a);
		OX_ASSERT(os->getValueId() == a->osValueId);
		os->pop();

		ObjectScript::pushCtypeValue(os, child);
		OX_ASSERT(os->getValueId() == child->osValueId);
		os->pop();
#endif
		os->pushValueById(a->osValueId); // this
		os->getProperty(-1, "_unregisterExternalChild"); // func
		OX_ASSERT(os->isFunction());
		os->pushValueById(child->osValueId);
		os->callTF(1);
		os->handleException();
	}
}

void unregisterAllActorChildren(Actor * a)
{
	OX_ASSERT(dynamic_cast<OS2D*>(os));
	if(a->osValueId){
#ifdef OS_DEBUG
		ObjectScript::pushCtypeValue(os, a);
		OX_ASSERT(os->getValueId() == a->osValueId);
		os->pop();
#endif
		os->pushValueById(a->osValueId); // this
		os->getProperty(-1, "_unregisterAllExternalChildren"); // func
		OX_ASSERT(os->isFunction());
		os->callTF();
		os->handleException();
	}
}

void callEventFunction(int func_id, Event * ev)
{
	OX_ASSERT(dynamic_cast<OS2D*>(os));
	
	int is_stack_event = !ev->_ref_counter; // ((intptr_t)ev < (intptr_t)&ev) ^ ObjectScript::Oxygine::stackOrder;
	if(is_stack_event){
		// ev = ev->clone();
		ev->_ref_counter++;
		OX_ASSERT(!ev->osValueId);
	}
	
	os->pushValueById(func_id);
	if(os->isFunction()){
		pushCtypeValue(os, ev);
		OX_ASSERT(ev->osValueId);
		os->callF(1);
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

void processKeyboardEvent(Event * ev, EventDispatcher * ed)
{
	KeyboardEvent::process(ev, ed);
}

void handleErrorPolicyVa(const char *format, va_list args)
{
	OX_ASSERT(dynamic_cast<OS2D*>(os));
	os->handleErrorPolicyVa(format, args);
}

static int OS_maxAllocatedBytes = 0;
static int OS_maxUsedBytes = 0;
static int OS_maxNumValues = 0;

::std::string getDebugStr()
{
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
		char buf[256];
		sprintf(buf, "\n"
			"OS GC max values:%d\n"
			"OS (Kb) max allocated:%d used:%d"
			, OS_maxNumValues, (OS_maxAllocatedBytes / 1024), (OS_maxUsedBytes / 1024)
			);
		return buf;
	}
	return "";
}

} // namespace ObjectScript

#ifdef WIN32
#include <windows.h>
#include <core\STDFileSystem.h>

static std::vector<std::string> argv;

std::vector<std::string>& parseCommandLine()
{
	const char * cmd = GetCommandLineA();
	while(*cmd){
		if(*cmd == '\"'){
			const char * start = ++cmd;
			for(; *cmd && *cmd != '\"'; cmd++);
			// int len = cmd - start;
			const char * end = cmd; if(*cmd) cmd++;
			argv.push_back(std::string(start, end));
		}else{
			const char * start = cmd++;
			for(; *cmd && !OS_IS_SPACE(*cmd); cmd++);
			const char * end = cmd;
			argv.push_back(std::string(start, end));
		}
		for(; *cmd && OS_IS_SPACE(*cmd); cmd++);
	}
	return argv;
}

static std::vector<file::STDFileSystem*> fsAdded;

void addFileSystem(const std::string& folder)
{
	file::STDFileSystem * fs = new file::STDFileSystem(true);
	fs->setPath(file::fs().getFullPath(folder.c_str()).c_str());
	fsAdded.push_back(fs);
	file::mount(fs);
}

void shutdownAddedFileSystems()
{
	std::vector<file::STDFileSystem*>::iterator it = fsAdded.begin();
	for(; it != fsAdded.end(); ++it){
		file::STDFileSystem * fs = *it;
		file::unmount(fs);
		delete fs;
	}
	fsAdded.clear();
}

#endif

// #endif // OX_WITH_OBJECTSCRIPT
