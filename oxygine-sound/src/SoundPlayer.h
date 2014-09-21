#pragma once
#include <string>
#include <vector>
#include "core/oxygine.h"

namespace oxygine
{
	class Resource;
	class Resources;
	class Channel;
	struct sound_desc;

	DECLARE_SMART(SoundInstance, spSoundInstance);
	DECLARE_SMART(Resources, spResources);

	using namespace std;

	DECLARE_SMART(SoundPlayer, spSoundPlayer);
	class SoundPlayer: public Object
	{
	public:
		OS_DECLARE_CLASSINFO(SoundPlayer);

		static void initialize();
		static void free();

		SoundPlayer();
		~SoundPlayer();


		spSoundInstance play(const std::string &id, 
			bool looping = false, 
			unsigned int fadeInMS = 0, unsigned int fadeOutMS = 0, 
			float primaryVolume = -1.0f, bool paused = false, 
			Channel *continueChannel = 0);
		spSoundInstance play(Resource *, 
			bool looping = false, 
			unsigned int fadeInMS = 0, unsigned int fadeOutMS = 0, 
			float primaryVolume = -1.0f, bool paused = false,
			Channel *continueChannel = 0);


		spSoundInstance	getSoundByIndex(int index);
		int				getSoundsNum()const {return (int)_sounds.size();}
		unsigned int	getTime() const;

		void pause();
		void resume();

		void stopByID(const string &id);
		void stop();

		void fadeOut(int ms);

		void update();	

		float getVolume() const;
		void setVolume(float v);

		spResources getResources() const;
		void setResources(Resources *res);

		bool IsPaused(){ return _paused; }
		
		bool getIsPlaying(){ return !_paused; }
		void setIsPlaying(bool);

	private:
		friend class SoundInstance;
		void removeSoundInstance(SoundInstance *);

		static void _onSoundDone(void *This, Channel *channel, const sound_desc &desc);

		static void _onSoundAboutDone(void *This, Channel *channel, const sound_desc &desc);
		void onSoundAboutDone(SoundInstance *soundInstance, Channel *channel, const sound_desc &desc);

		Resources *_resources;
		float _volume;

		typedef std::vector<spSoundInstance> playingSounds;
		playingSounds _sounds;

		timeMS _time;
		timeMS _lastUpdateTime;

		bool _paused;
	};
}