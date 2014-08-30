#include <sstream>

#include "res/ResAnim.h"
#include "res/Resources.h"

#include "utils/stringUtils.h"

#include "core/NativeTexture.h"
#include "core/ZipFileSystem.h"
#include "core/system_data.h"
#include "core/Renderer.h"

#include "dev_tools/DeveloperMenu.h"
#include "dev_tools/TreeInspector.h"
#include "dev_tools/TexturesInspector.h"

#include "DebugActor.h"
#include "RootActor.h"
#include "TextField.h"
#include "ColorRectSprite.h"
#include "Button.h"
#include "Event.h"
#include "RenderState.h"
#include "initActor.h"
#include <stdio.h>
#include <stdarg.h>
#include "MaskedSprite.h"

#ifdef __S3E__
#include "s3eMemory.h"
#endif

namespace oxygine
{
	Resources* DebugActor::resSystem = 0;
	file::ZipFileSystem zp;

	spDebugActor DebugActor::instance;

	void DebugActor::initialize()
	{
		if (resSystem)
			return;

		spActor a = new MaskedSprite;

		zp.setPrefix("system/");
		zp.add(system_data, system_size);

		file::mount(&zp);
		resSystem = new Resources;
		resSystem->loadXML("system/res.xml", 0, true, false, "system");
		//

		//log::messageln("initialized DebugActor");
	}

	void DebugActor::release()
	{
		instance = 0;
		delete resSystem;
		resSystem = 0;
		file::unmount(&zp);
	}
	
	void DebugActor::setCornerPosition(int corner)
	{
		_corner = corner;
	}

	void DebugActor::addButton(float &x, const char *name, const char *anim)
	{
		spButton btn;
		btn = initActor(new Button,
			arg_resAnim = resSystem->getResAnim(anim),
			arg_y = 1,
			arg_name = name,
			arg_attachTo = this);
		x = x - btn->getWidth();
		btn->setX(x);
		btn->addEventListener(TouchEvent::CLICK, CLOSURE(this, &DebugActor::_btnClicked));
	}

	DebugActor::DebugActor():_frames(0), _startTime(0), _corner(0), _showTexel2PixelErrors(false), _showTouchedActor(false)
	{
		DebugActor::initialize();

		setName(getDefaultName());

		_startTime = getTimeMS();
		setPriority(1000);

		float scale = 1.0f;
		if (getRoot())
			scale = 1.0f/getRoot()->getScaleX();

		setScale(scale);

		setInputEnabled(false);
		

		TextStyle st;
		st.font = resSystem->getResFont("system")->getFont();
		st.vAlign = TextStyle::VALIGN_TOP;
		//st.color = Color(rand()%255, rand()%255, rand()%255, 255);
		st.color = Color(Color::Black, 255);
		st.multiline = true;
		setWidth(230);
		setHeight(45);

		_bg = new ColorRectSprite;
		_bg->setColor(Color(Color::White, 128));
		_bg->setSize(getSize());
		_bg->setInputEnabled(false);
		addChild(_bg);


		float x = getWidth();
		addButton(x, "tree", "tree");
		addButton(x, "finger", "finger");		
#ifdef OXYGINE_DEBUG_T2P
		addButton(x, "t2p", "t2p");
#endif

#ifdef OXYGINE_DEBUG_TRACE_LEAKS
		
		addButton(x, "images", "images");		
#endif
		
		

		_text = new TextField;
		addChild(_text);
		_text->setPosition(2, 5);
		_text->setInputEnabled(false);
		_text->setStyle(st);
		_text->setWidth(getWidth());
		_text->setText("debug text");

		getRoot()->addEventListener(TouchEvent::MOVE, CLOSURE(this, &DebugActor::onDAEvent));

		instance = this;
	}

	/*
	void DebugActor::addDebugString(const string &str)
	{
		_debugText += str;
	}
	*/

	void DebugActor::addDebugString(const char *format, ...)
	{
		char buff[1024] = "";
		va_list args;
		va_start(args, format);
		int len = strlen(buff);
		int i = vsnprintf(buff + len, sizeof(buff) - len, format, args);
		va_end(args);

		_debugText += buff;
	}

	void DebugActor::_btnClicked(Event *ev)
	{
		string name = ev->currentTarget->getName();
		if (name == "finger")
		{
			showTouchedActor(!_showTouchedActor);
			return;
		}

		if (name == "t2p")
		{
			showTexel2PixelErrors(!_showTexel2PixelErrors);
			return;
		}

		spActor inspector = getRoot()->getChild(DeveloperMenu::getDefaultName(), ep_ignore_error);
		if (inspector)
			inspector->detach();
		else
		{		
			spDeveloperMenu dm = new DeveloperMenu();
			dm->setPriority(getPriority()  + 1); 
			float scale = getRoot()->getScaleX();
			Vector2 size = core::getDisplaySize();

			Vector2 s = size;// * scale;
			s.y -= 24;
			
			if (name == "tree")
			{
				spTreeInspector tree = new TreeInspector;
				tree->init(s, getRoot());

				dm->init(s, "Tree Inspector", tree, Color(230, 230, 230, 255));
			}

			if (name == "images")
			{
				spTexturesInspector tree = new TexturesInspector(s);
				dm->init(s, "Textures Inspector", tree, Color(60, 60, 60, 255));
			}

			dm->setScale(1.0f / scale);
			Vector2 p = -getRoot()->getPosition() / scale;
			dm->setPosition(p);
			getRoot()->addChild(dm);
		}
	}

	DebugActor::~DebugActor()
	{
		if (getRoot())
			getRoot()->removeEventListeners(this);
	}

	extern IVideoDriver::Stats _videoStats;

	void DebugActor::doUpdate(const UpdateState &us)
	{
		static int fps = 0;
		++_frames;
		if (_frames > 50)
		{
			timeMS this_time = getTimeMS();
			if (this_time != _startTime)
			{
				fps = int(((float)_frames/(this_time - _startTime))*1000);
			}
			_startTime = this_time;
			_frames = 0;
		}

		stringstream s;
		s << "fps=" << fps << endl;

		

#ifdef __S3E__
		int mem_used = -1;
		int mem_free = -1;

		mem_used = s3eMemoryGetInt(S3E_MEMORY_USED);
		mem_free = s3eMemoryGetInt(S3E_MEMORY_FREE);

		s << "mfree=" << mem_free << " mem=" << mem_used << endl;
#endif
		
		

#ifdef OXYGINE_DEBUG_TRACE_LEAKS
		s << "objects=" << (int)ObjectBase::__getCreatedObjects().size() << endl;
#endif
#ifdef OXYGINE_TRACE_VIDEO_STATS
		s << "batches="<< _videoStats.batches << " triangles=" << _videoStats.triangles << endl;
#endif
		s << "update=" << getRoot()->_statUpdate << "ms ";
		s << "render=" << getRoot()->_statRender << "ms ";
		s << "textures=" << NativeTexture::created << " ";
		s << "\nlisteners=" << getRoot()->getListenersCount() << "";

#ifdef OX_WITH_OBJECTSCRIPT
		// std::string getOSDebugStr();
		s << getOSDebugStr();
#endif

		if (!_debugText.empty())
		{
			s << "\n";
			s << _debugText;
		}
		_debugText = "";

		_text->setText(s.str());

		
		setHeight(_text->getTextRect().size.y + _text->getY() + 3);
		_bg->setSize(getSize());


		float scale = getScaleX();
		Vector2 parentSize = getParent()->getSize();
		Vector2 realSize = getSize() * scale;

		setX(0);
		setY(0);

		switch(_corner)
		{			
		case 1:
			setX(parentSize.x - realSize.x);
			setY(0);
			break;
		case 2:
			setPosition(parentSize - realSize);
			break;
		case 3:
			setX(0);
			setY(parentSize.y - realSize.y);
			break;
		}
	}

	void DebugActor::render( RenderState const& parentRenderState )
	{
		parentRenderState.renderer->drawBatch();
		parentRenderState.renderer->getDriver()->setDebugStats(false);
		Actor::render(parentRenderState);
		parentRenderState.renderer->drawBatch();
		parentRenderState.renderer->getDriver()->setDebugStats(true);
	}

	void DebugActor::showTexel2PixelErrors(bool show)
	{
		_showTexel2PixelErrors = show;
#ifdef OXYGINE_DEBUG_T2P
		Renderer::showTexel2PixelErrors(_showTexel2PixelErrors);
		spActor btn = getChild("t2p");
		btn->removeTweens(true);
		if (show)
			btn->addTween(Actor::TweenAlpha(0), 300, 999999, true);
#endif		
	}
	
	void DebugActor::showTouchedActor(bool show)
	{
		getRoot()->removeEventListener(TouchEvent::TOUCH_DOWN, CLOSURE(this, &DebugActor::onEvent));
		_showTouchedActor = show;
		if (show)
			getRoot()->addEventListener(TouchEvent::TOUCH_DOWN, CLOSURE(this, &DebugActor::onEvent));

		spActor btn = getChild("finger");
		btn->removeTweens(true);
		if (show)
			btn->addTween(Actor::TweenAlpha(0), 300, 999999, true);
	}
	
	void DebugActor::onDAEvent(Event *ev)
	{
		TouchEvent *t = safeCast<TouchEvent*>(ev);
		Vector2 loc = convert_global2local(this, getRoot(), t->localPosition);
		setAlpha(isOn(loc) ? 64 : 255);
	}

	void DebugActor::onEvent(Event *ev)
	{
		spActor actor = safeSpCast<Actor>(ev->target);
		spColorRectSprite cr = new ColorRectSprite;
		cr->setInputEnabled(false);
		//cr->setAlpha(100);
		cr->setColor(Color(rand()%255, rand()%255, rand()%255, 0));
		cr->setSize(actor->getSize());
		cr->addTween(ColorRectSprite::TweenColor(Color(Color::White, 200)), 700, 1, true, 0, Tween::ease_inCubic)->setDetachActor(true);
		actor->addChild(cr);
		string dmp = actor->dump(0);
		log::messageln("touched actor:\n%s", dmp.c_str());
	}
}