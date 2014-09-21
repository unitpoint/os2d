#include "core/files_io.h"
#include "core/oxygine.h"
#include "res/Resources.h"
#include "res/CreateResourceContext.h"

#include "SoundPlayer.h"
#include "Sound.h"
#include "SoundInstance.h"
#include "SoundSystem.h"
#include "Channel.h"
#include "ResSound.h"


namespace oxygine
{
	void SoundPlayer::initialize()
	{
		Resources::registerResourceType(&ResSound::createResSound, "sound");		
	}

	void SoundPlayer::free()
	{
		Resources::unregisterResourceType("sound");

	}

	SoundPlayer::SoundPlayer():_resources(0), _volume(1.0f), _time(0), _lastUpdateTime(0), _paused(false)
	{
		_time = getTimeMS();
		_lastUpdateTime = _time;
	}

	SoundPlayer::~SoundPlayer()
	{

	}

	float SoundPlayer::getVolume() const
	{
		return _volume;
	}

	void SoundPlayer::setVolume(float v)
	{
		_volume = v;
		for (playingSounds::iterator i = _sounds.begin(); i != _sounds.end(); ++i)
		{
			SoundInstance *s = (*i).get();
			s->setVolume(v);
		}

	}

	void SoundPlayer::removeSoundInstance(SoundInstance *soundInstance)
	{
		playingSounds::iterator i = find(_sounds.begin(), _sounds.end(), soundInstance);
		OX_ASSERT(i != _sounds.end());

		_sounds.erase(i);
	}

	void SoundPlayer::_onSoundDone(void *sound_instance, Channel *channel, const sound_desc &desc)
	{
		SoundInstance *t = (SoundInstance *)sound_instance;
		t->finished();	
	}

	void SoundPlayer::_onSoundAboutDone(void *sound_instance, Channel *channel, const sound_desc &desc)
	{
		SoundInstance *t = (SoundInstance *)sound_instance;
		t->aboutDone();
	}

	void SoundPlayer::onSoundAboutDone(SoundInstance *soundInstance, Channel *channel, const sound_desc &desc)
	{
		/*
		soundInstance->_channel = 0;
		removeSoundInstance(soundInstance);
		*/
	}

	spSoundInstance SoundPlayer::getSoundByIndex(int index)
	{
		return _sounds[index];
	}

	spResources SoundPlayer::getResources() const
	{
		return _resources;
	}
	
	void SoundPlayer::setResources(Resources *res)
	{
		_resources = res;
	}

	void SoundPlayer::setIsPlaying(bool value)
	{
		if(value != getIsPlaying()){
			if(value){
				resume();
			}else{
				pause();
			}
		}
	}

	spSoundInstance SoundPlayer::play(const std::string &id, bool looping, unsigned int fadeInMS, unsigned int fadeOutMS, float primaryVolume, bool pause, Channel *continueChannel)
	{
		if (!_resources)
			return 0;

		ResSound *res = _resources->getT<ResSound>(id);
		if (!res)
			return 0;

		return play(res, looping, fadeInMS, fadeOutMS, primaryVolume, pause, continueChannel);
	}

	spSoundInstance SoundPlayer::play(Resource *ressound_, bool looping, unsigned int fadeInMS, unsigned int fadeOutMS, float primaryVolume, bool pause, Channel *continueChannel)
	{
		ResSound *ressound = safeCast<ResSound*>(ressound_);
		spSoundInstance s = new SoundInstance();
		s->_player = this;
		if ( primaryVolume < 0.f )
		{
			primaryVolume = _volume;
		}

		if (!ressound || !ressound->getSound())
			return s;

		//printf("PlayResSound:\n");
		Channel *channel = continueChannel;
		if (!channel)
			channel = SoundSystem::instance->getFreeChannel();
		if (!channel)
			return s;


		sound_desc desc;
		desc.sound = ressound->getSound();
		desc.cbDone = _onSoundDone;
		desc.cbAboutEnd = _onSoundAboutDone;
		desc.cbUserData = s.get();
		desc.looping = looping;
		desc.id = ressound->getName();
		desc.volume = primaryVolume;
		desc.paused = pause;



		s->_desc = desc;
		s->_channel = channel;
		s->_startTime = getTime();

		s->_startFadeIn = 0;
		s->_fadeInMS = fadeInMS;

		if (looping)
			s->_startFadeOut = 0;
		else
			s->_startFadeOut = desc.sound->getDuration() - fadeOutMS;

		s->_fadeOutMS = fadeOutMS;

		s->_volume = primaryVolume;	
		s->_state = SoundInstance::Normal;

		if (fadeInMS)
		{
			s->_state = SoundInstance::FadingIn;
			desc.volume = 0.0f;
		}

		_sounds.push_back(s);

		if (continueChannel)
			channel->continuePlay(desc);
		else
			channel->play(desc);

		return s;
	}

	void SoundPlayer::pause()
	{
		for (playingSounds::iterator i = _sounds.begin(); i != _sounds.end(); ++i)
		{
			SoundInstance *s = (*i).get();
			if (s->_channel)
			{
				s->_channel->pause();
			}
		}
		_paused = true;
	}

	void SoundPlayer::resume()
	{		
		for (playingSounds::iterator i = _sounds.begin(); i != _sounds.end(); ++i)
		{
			SoundInstance *s = (*i).get();
			if (s->_channel)
			{
				s->_channel->resume();
			}
		}
		_paused = false;
	}

	void SoundPlayer::stopByID(const string &id)
	{
		bool try_again = true;
		while (try_again)
		{
			try_again = false;

			for (playingSounds::iterator i = _sounds.begin(); i != _sounds.end(); ++i)
			{
				spSoundInstance s = *i;
				if (!s->_channel)
					continue;

				if (s->_desc.id == id)
				{
					s->_channel->stop();
				}
			}
		}	
	}

	void SoundPlayer::stop()
	{
		for (playingSounds::iterator i = _sounds.begin(); i != _sounds.end(); ++i)
		{
			spSoundInstance sound = *i;
			sound->stop();
		}

		_sounds.resize(0);
	}

	void SoundPlayer::fadeOut(int ms)
	{
		for (playingSounds::iterator i = _sounds.begin(); i != _sounds.end(); ++i)
		{
			spSoundInstance sound = *i;
			sound->fadeOut(ms);
		}
	}

	unsigned int SoundPlayer::getTime()const
	{
		return _time;
	}

	void SoundPlayer::update()
	{	
		timeMS t = getTimeMS();
		timeMS d = t - _lastUpdateTime;
		if (_paused)
			d = 0;
		_time += d;

		for (size_t i = 0; i < _sounds.size();)
		{
			spSoundInstance s = _sounds[i];
			s->update();

			if (s->getChannel())
				++i;
			else
				_sounds.erase(_sounds.begin() + i);
		}

		//log::messageln("sounds %d", _sounds.size());

		_lastUpdateTime = t;
	}
}