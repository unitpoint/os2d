#include "ResFontBM.h"
#include "Font.h"
#include <vector>
#include "pugixml/pugixml.hpp"
#include "MemoryTexture.h"
#include "core/NativeTexture.h"
#include "core/VideoDriver.h"
#include "core/file.h"
#include "CreateResourceContext.h"
#include "utils/stringUtils.h"
#include "core/Mem2Native.h"
#include "core/Renderer.h"
#include "Resources.h"
namespace oxygine
{
	Resource *ResFontBM::create(CreateResourceContext &context)
	{
		ResFontBM *font = 0;
		
		font = new ResFontBM();		
		font->_createFont(&context, false, false);
		setNode(font, context.walker.getNode());
		context.resources->add(font);
		
		//context.meta = context.meta.next_sibling();

		return font;
	}

	Resource *ResFontBM::createBM(CreateResourceContext &context)
	{
		ResFontBM *font = 0;

		font = new ResFontBM();		
		font->_createFont(&context, false, true);
		setNode(font, context.walker.getNode());
		context.resources->add(font);

		//context.meta = context.meta.next_sibling();

		return font;
	}

	Resource *ResFontBM::createSD(CreateResourceContext &context)
	{
		ResFontBM *font = 0;

		font = new ResFontBM();		
		font->_createFont(&context, true, false);
		setNode(font, context.walker.getNode());

		//context.meta = context.meta.next_sibling();

		return font;
	}

	ResFontBM::ResFontBM():_font(0), _format(TF_R8G8B8A8), _premultipliedAlpha(false)
	{

	}

	ResFontBM::~ResFontBM()
	{
		cleanup();
	}
		
	void ResFontBM::init(const char *path, const char *file, bool premultipliedAlpha, bool signedDistanceFont)
	{
		_premultipliedAlpha = premultipliedAlpha;
		_file = string(path) + "/" + file;
		_createFont(0, signedDistanceFont, false);
	}

	void ResFontBM::cleanup()
	{
		for (pages::iterator i = _pages.begin(); i != _pages.end(); ++i)
		{
			const page &p = *i;
			p.texture->release();
		}
		_pages.clear();
		delete _font;
		_font = 0;
	}

	void ResFontBM::_restore(Restorable *r, void*)
	{
		void *object = r->_getRestorableObject();
		for (pages::iterator i = _pages.begin(); i != _pages.end(); ++i)
		{
			const page &p = *i;
			if (p.texture.get() == object)
			{
				_loadPage(p, &RestoreResourcesContext::instance);
			}
		}
	}

	void ResFontBM::_loadPage(const page &p, LoadResourcesContext *load_context)
	{
		if (!load_context->isNeedProceed(p.texture))
			return;

		spMemoryTexture mt = new MemoryTexture;

		file::buffer bf;
		file::read(p.file.c_str(), bf);

		bool premultAlpha = !_premultipliedAlpha;
		if (!Renderer::getPremultipliedAlphaRender())
			premultAlpha = false;

		mt->init(bf, premultAlpha, _format);		
		load_context->createTexture(mt, p.texture);
		p.texture->reg(CLOSURE(this, &ResFontBM::_restore), 0);
	}

	void ResFontBM::_load(LoadResourcesContext *load_context)
	{
		OX_ASSERT(!_pages.empty());
		if (_pages.empty())
			return;

		for (pages::iterator i = _pages.begin(); i != _pages.end(); ++i)
		{
			const page &p = *i;
			_loadPage(p, load_context);			
			
		}		
	}

	int ucs2_to_utf8 (int ucs2, unsigned char * utf8)
	{
		if (ucs2 < 0x80) {
			utf8[0] = ucs2;
			utf8[1] = '\0';
			return 1;
		}
		if (ucs2 >= 0x80  && ucs2 < 0x800) {
			utf8[0] = (ucs2 >> 6)   | 0xC0;
			utf8[1] = (ucs2 & 0x3F) | 0x80;
			utf8[2] = '\0';
			return 2;
		}
		if (ucs2 >= 0x800 && ucs2 < 0xFFFF) {
			utf8[0] = ((ucs2 >> 12)       ) | 0xE0;
			utf8[1] = ((ucs2 >> 6 ) & 0x3F) | 0x80;
			utf8[2] = ((ucs2      ) & 0x3F) | 0x80;
			utf8[3] = '\0';
			return 3;
		}
		return -1;
	}

	void ResFontBM::_createFont(CreateResourceContext *context, bool sd, bool bmc)
	{	
		if (sd)
			_format = TF_L8;

		if (context)
		{
			pugi::xml_node node = context->walker.getNode();
			_premultipliedAlpha = node.attribute("premultiplied_alpha").as_bool(_premultipliedAlpha);
			
			_file = context->walker.getPath("file");
			setName(Resource::extractID(node, _file, ""));

			if (bmc)
			{
				_file = *context->prebuilt_folder + getName() + ".fnt";
			}
			else
			{
				
			}			

			
		}		

		string path = _file;
		file::buffer fb;
		file::read(path.c_str(), fb);

		pugi::xml_document doc;
		doc.load_buffer_inplace(&fb.data[0], fb.data.size());

		
		pugi::xml_node root = doc.first_child();
		pugi::xml_node info = root.child("info");

		//<info face="Century Gothic" size="-24" bold="0" italic="0" charset="" unicode="1" stretchH="100" smooth="1" aa="1" padding="0,0,0,0" spacing="1,1" outline="0"/>
		int fontSize = info.attribute("size").as_int();
		

		pugi::xml_node common = info.next_sibling("common");
		int lineHeight = common.attribute("lineHeight").as_int();
		int base = common.attribute("base").as_int();

		pugi::xml_node pages = common.next_sibling("pages");

		int tw = common.attribute("scaleW").as_int();
		int th = common.attribute("scaleH").as_int();
		

		for (pugi::xml_node page_node = pages.child("page"); page_node; page_node = page_node.next_sibling("page"))
		{
			page p;

			const char *textureFile = page_node.attribute("file").value();

			char tail[255];
			char head[255];
			path::split(path.c_str(), head, tail);
			p.file = head;
			p.file += "//";
			p.file += textureFile;

			p.texture = IVideoDriver::instance->createTexture();
			p.texture->setName(p.file);

			if (tw)
			{
				p.texture->init(0, tw, th, TF_UNDEFINED);
			}

			_pages.push_back(p);
		}
		

		if (!tw)
		{
			load(0);
		}

				

		fontSize = abs(fontSize);
		Font *font = new Font();
		font->init(getName().c_str(), fontSize, fontSize, lineHeight + fontSize - base);
		_font = font;

		if (context)
		{
			float scale_factor = context->walker.getMeta().attribute("sf").as_float(1);
			_font->setScaleFactor(scale_factor);
		}
		

		pugi::xml_node chars = pages.next_sibling("chars");
		pugi::xml_node child = chars.first_child();
		while (!child.empty())
		{	
			int charID = 0;
			int xpos = 0;
			int ypos = 0;
			int width = 0;
			int height = 0;
			int xoffset = 0;
			int yoffset = 0;
			int xadvance = 0;
			int page = 0;
			
			pugi::xml_attribute attr = child.first_attribute();
			while(!attr.empty())
			{
				const char *attr_name = attr.name();
				int value = attr.as_int();

				if (!strcmp(attr_name, "id"))
					charID = value;
				else
					if (!strcmp(attr_name, "x"))
						xpos = value;
					else
						if (!strcmp(attr_name, "y"))
							ypos = value;
						else
							if (!strcmp(attr_name, "width"))
								width = value;
							else
								if (!strcmp(attr_name, "height"))
									height = value;
								else
									if (!strcmp(attr_name, "xoffset"))
										xoffset = value;
									else
										if (!strcmp(attr_name, "yoffset"))
											yoffset = value;
										else
											if (!strcmp(attr_name, "xadvance"))
												xadvance = value;
											else
												if (!strcmp(attr_name, "page"))
													page = value;

				attr = attr.next_attribute();
			}

			spTexture t = _pages[page].texture;
			float iw = 1.0f / t->getWidth();
			float ih = 1.0f / t->getHeight();

			glyph gl;
			gl.src = RectF(xpos * iw, ypos * ih, width * iw, height * ih);
			gl.sw = width;
			gl.sh = height;
			gl.offset_x = xoffset;				
			gl.offset_y = yoffset - base;
			gl.advance_x = xadvance;
			gl.advance_y = 0;

			int code = 0;
			ucs2_to_utf8(charID, (unsigned char*)&code);
			gl.ch = code;
			gl.texture = _pages[page].texture;

			font->addGlyph(gl);

			child = child.next_sibling();
		}

		font->sortGlyphs();
	}

	void ResFontBM::_unload()
	{
		OX_ASSERT(!_pages.empty());
		for (pages::iterator i = _pages.begin(); i != _pages.end(); ++i)
		{
			const page &p = *i;
			p.texture->release();
		}
	}

	Font *ResFontBM::getFont(const char *name, int size) const
	{
		return _font;
	}
}