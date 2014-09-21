#pragma once
#include "res/Resource.h"

namespace oxygine
{
	class Sound;
	class SoundSystem;
	class CreateResourceContext;

	DECLARE_SMART(ResSound, spResSound);
	class ResSound: public Resource
	{
	public:
		OS_DECLARE_CLASSINFO(ResSound);

		static Resource* createResSound(CreateResourceContext &context);

		ResSound();
		~ResSound();

		bool init(CreateResourceContext &context, SoundSystem *soundSystem);

		Sound*			getSound();
		const string&	getPath() const;

	private:
		void _load(LoadResourcesContext *context);
		void _unload();

		SoundSystem *_soundSystem;

		string _file;
		bool _streaming;

		Sound *_sound;
	};
}