#pragma once
#include "oxygine_include.h"
#include "Resource.h"
namespace oxygine
{
	class Font;

	DECLARE_SMART(ResFont, spResFont);
	class ResFont: public Resource
	{
	public:
		OS_DECLARE_CLASSINFO(ResFont);

		virtual Font *getFont(const char *name = 0, int size = 0) const = 0;
	};
}