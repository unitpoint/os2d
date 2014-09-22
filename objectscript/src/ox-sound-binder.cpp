#include "ox-sound-binder.h"

/*
oxygine-sound merge state
SHA 83fff573
by dmuratshin, 10.09.2014 16:09
*/

#ifdef OX_WITH_OBJECTSCRIPT

namespace ObjectScript {

// =====================================================================

static void registerResSound(OS * os)
{
	struct Lib
	{
	};
	OS::FuncDef funcs[] = {
		// def("__newinstance", &Lib::__newinstance),
		{}
	};
	OS::NumberDef nums[] = {
		{}
	};
	registerOXClass<ResSound, Resource>(os, funcs, nums, true OS_DBG_FILEPOS);
}
static bool __registerResSound = addRegFunc(registerResSound);

// =====================================================================

static void registerSoundInstance(OS * os)
{
	struct Lib
	{
		static int fadeOut(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(SoundPlayer*);
			int ms = (int)(os->toNumber(-params+0) / 1000.0f);
			self->fadeOut(ms < 0 ? 0 : ms);
			return 0;
		}
		
		static int getDuration(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(SoundInstance*);
			os->pushNumber(self->getDuration() * 1000);
			return 1;
		}
		
		static int getPosition(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(SoundInstance*);
			os->pushNumber(self->getPosition() * 1000);
			return 1;
		}
	};
	OS::FuncDef funcs[] = {
		// def("__newinstance", &Lib::__newinstance),
		def("stop", &SoundInstance::stop),
		{"fadeOut", &Lib::fadeOut},
		{"__get@duration", &Lib::getDuration},
		{"__get@pos", &Lib::getPosition},
		DEF_PROP(volume, SoundInstance, Volume),
		def("__get@isPlaying", &SoundInstance::isPlaying),
		DEF_PROP(doneCallback, SoundInstance, DoneCallback),
		DEF_PROP(aboutDoneCallback, SoundInstance, AboutDoneCallback),
		{}
	};
	OS::NumberDef nums[] = {
		{}
	};
	registerOXClass<SoundInstance, Object>(os, funcs, nums, true OS_DBG_FILEPOS);
}
static bool __registerSoundInstance = addRegFunc(registerSoundInstance);

// =====================================================================

static void registerSoundPlayer(OS * os)
{
	struct Lib
	{
		static SoundPlayer * __newinstance()
		{
			return new SoundPlayer();
		}

		static int play(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(SoundPlayer*);
			if(params < 1){
				os->setException("argument required");
				return 0;
			}
			ResSound * sound = CtypeValue<ResSound*>::getArg(os, -params+0);
			if(sound){
				pushCtypeValue(os, self->play(sound));
				return 1;
			}
			OS::String name(os);
			if(os->isString(-params+0, &name)){
				pushCtypeValue(os, self->play(name.toChar()));
				return 1;
			}
			if(!os->isObject(-params+0)){
				os->setException("1st argument should be object of play properties or string or ResSound");
				return 0;
			}
			os->getProperty(-params+0, "sound");
			sound = CtypeValue<ResSound*>::getArg(os, -1);
			if(!sound && !os->isString(-1, &name)){
				os->setException("sound should be string or ResSound");
				return 0;
			}
			os->pop();

			bool looping = (os->getProperty(-params+0, "looping"), os->popBool(false));
			bool paused = (os->getProperty(-params+0, "paused"), os->popBool(false));
			int fadeInMS = (int)(os->getProperty(-params+0, "fadeIn"), os->popFloat(0.0f) * 1000.0f); if(fadeInMS < 0) fadeInMS = 0;
			int fadeOutMS = (int)(os->getProperty(-params+0, "fadeOut"), os->popFloat(0.0f) * 1000.0f); if(fadeOutMS < 0) fadeOutMS = 0;
			float primaryVolume = (os->getProperty(-params+0, "primaryVolume"), os->popFloat(-1.0f));

			if(sound){
				pushCtypeValue(os, self->play(sound, looping, fadeInMS, fadeOutMS, primaryVolume, paused));
			}else{
				pushCtypeValue(os, self->play(name.toChar(), looping, fadeInMS, fadeOutMS, primaryVolume, paused));
			}

			return 1;
		}

		static int stop(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(SoundPlayer*);
			if(params > 0){
				Resource * sound = CtypeValue<Resource*>::getArg(os, -params+0);
				if(sound){
					self->stopByID(sound->getName());
					return 0;
				}
				OS::String name(os);
				if(!os->isString(-1, &name)){
					os->setException("argument should be string or Resource, no argument is also supported");
					return 0;
				}
				self->stopByID(name.toChar());
				return 0;
			}
			self->stop();
			return 0;
		}

		static int getTime(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(SoundPlayer*);
			os->pushNumber(self->getTime() * 1000);
			return 1;
		}

		static int fadeOut(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(SoundPlayer*);
			int ms = (int)(os->toNumber(-params+0) / 1000.0f);
			self->fadeOut(ms < 0 ? 0 : ms);
			return 0;
		}
	};
	OS::FuncDef funcs[] = {
		def("__newinstance", &Lib::__newinstance),
		{"play", &Lib::play},
		{"stop", &Lib::stop},
		def("getSound", &SoundPlayer::getSoundByIndex),
		DEF_GET(numSounds, SoundPlayer, SoundsNum),
		{"__get@time", &Lib::getTime},
		def("pause", &SoundPlayer::pause),
		def("resume", &SoundPlayer::resume),
		{"fadeOut", &Lib::fadeOut},
		def("update", &SoundPlayer::update),
		DEF_PROP(volume, SoundPlayer, Volume),
		DEF_PROP(resources, SoundPlayer, Resources),
		DEF_PROP(isPlaying, SoundPlayer, IsPlaying),
		{}
	};
	OS::NumberDef nums[] = {
		{}
	};
	registerOXClass<SoundPlayer, Object>(os, funcs, nums, true OS_DBG_FILEPOS);
}
static bool __registerSoundPlayer = addRegFunc(registerSoundPlayer);


} // namespace ObjectScript

#endif // OX_WITH_OBJECTSCRIPT
