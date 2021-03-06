#pragma once
#include "oxygine_include.h"
#include "Sprite.h"

namespace oxygine
{
	DECLARE_SMART(ColorRectSprite, spColorRectSprite);

	class ColorRectSprite: public Sprite
	{
	public:
		OS_DECLARE_CLASSINFO(ColorRectSprite);
		DECLARE_COPYCLONE_NEW(ColorRectSprite);
		ColorRectSprite();
		~ColorRectSprite();

		void serialize(serializedata* data);

		void doRender(const RenderState &rs);

	private:
	};
}