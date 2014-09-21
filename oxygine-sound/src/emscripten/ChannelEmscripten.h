#pragma once
#include "Channel.h"

namespace oxygine
{
	class ChannelEmscripten : public Channel
	{
	public:
		ChannelEmscripten();
		~ChannelEmscripten();

		void play(const sound_desc &);
		void continuePlay(const sound_desc &);
		void pause();
		void resume();
		void stop();
		bool update();

		Sound*	getSound() const;
		float				getVolume() const;
		const sound_desc&	getSoundDesc() const;
		int					getPosition() const;

		void setVolume(float v);

		bool isFree() const;
		void _updateMasterVolume(float){}

	private:
		sound_desc _desc;
		int _handle;
	};
}