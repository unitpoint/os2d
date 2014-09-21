#pragma once
#include <string>
namespace oxygine
{
	class Sound;
	class Channel;

	struct sound_desc
	{
		Sound *sound;

		bool looping;
		float volume;
		bool paused;
		std::string id;

		typedef void (*callback)(void*, Channel *, const sound_desc &);
		callback cbDone;
		callback cbAboutEnd;
		void* cbUserData;

		sound_desc():sound(0), looping(false), volume(1.0), paused(false), cbDone(0), cbUserData(0), cbAboutEnd(0){}	
	};

	class Channel
	{
	public:
		enum State
		{
			Free, 
			Playing,
			Paused,			
			Ended,
		};

		virtual ~Channel(){}


		virtual void play(const sound_desc &) = 0;
		virtual void continuePlay(const sound_desc &) = 0;
		virtual void pause() = 0;
		virtual void resume() = 0;
		virtual void stop() = 0;

		virtual Sound*				getSound() const = 0;
		virtual float				getVolume()const = 0;
		virtual const sound_desc&	getSoundDesc() const = 0;
		virtual int					getPosition() const = 0;

		virtual void setVolume(float v) = 0;	

	private:
	};
}