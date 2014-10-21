#include "Tweener.h"
#include "Actor.h"
#include "ColorRectSprite.h"
#include <limits.h>
namespace oxygine
{
	Tween::Tween():_duration(0), 
		_loops(0), 
		_percent(0), 
		_status(status_not_started),
		_elapsed(0), _twoSides(false), _ease(ease_linear), _detach(false), _delay(0), _client(0)
	{

	}

	Tween::~Tween()
	{
	}

	void Tween::reset()
	{
		_elapsed = 0;
		_status = status_not_started;
	}

	void Tween::init(timeMS duration, int loops, bool twoSides, timeMS delay, EASE ease)
	{
		_duration = duration;
		_ease = ease;
		_loops = loops;
		_twoSides = twoSides;
		_delay = delay;
				
		if (_duration <= 0)
		{
			OX_ASSERT(!"Tweener duration should be more than ZERO");
			_duration = 1;
		}
	}

	void Tween::setDoneCallback(const EventCallback& cb)
	{
		ObjectScript::unregisterEventCallback(this, "_cbDone", _cbDone);
		_cbDone = cb;
		ObjectScript::registerEventCallback(this, "_cbDone", _cbDone);
	}

	EventCallback Tween::getDoneCallback() const
	{
		return _cbDone;
	}

	void Tween::addDoneCallback(const EventCallback& cb)
	{
		addEventListener(TweenEvent::DONE, cb);
	}

	static float outBounce(float t)
	{
		if (t < 0.363636363636f){
			return 7.5625f * t * t;
		}
		if (t < 0.727272727273f) {
			t = t - 0.545454545455f;
			return 7.5625f * t * t + 0.75f;
		}
		if (t < 0.909090909091f) {
			t = t - 0.818181818182f;
			return 7.5625f * t * t + 0.9375f;
		}
		t = t - 0.954545454545f;
		return 7.5625f * t * t + 0.984375f;
	}


	float Tween::calcEase(EASE ease, float t)
	{
		const float s = 1.70158f;

		switch (ease)
		{
		case ease_linear:
			return t;

#define DEF_EASY_FROM_IN(EasyPost, formula) \
		case ease_in ## EasyPost: \
			return (formula); \
		case ease_out ## EasyPost: \
			return 1 - calcEase(ease_in ## EasyPost, 1 - t); \
		case ease_inOut ## EasyPost: \
			return t <= 0.5f ? calcEase(ease_in ## EasyPost, t * 2) / 2 : 1 - calcEase(ease_in ## EasyPost, 2 - t * 2) / 2; \
		case ease_outIn ## EasyPost: \
			return t <= 0.5f ? calcEase(ease_in ## EasyPost, t * 2) / 2 : 1 - calcEase(ease_in ## EasyPost, 2 - t * 2) / 2; \

		DEF_EASY_FROM_IN(Quad, t * t);
		DEF_EASY_FROM_IN(Cubic, t * t * t);
		DEF_EASY_FROM_IN(Quart, powf(t, 4));
		DEF_EASY_FROM_IN(Quint, powf(t, 5));
			DEF_EASY_FROM_IN(Sin, 1.0f - scalar::cos(t * (MATH_PI / 2.0f)));
		DEF_EASY_FROM_IN(Expo, powf(2, 10 * (t - 1)));
			DEF_EASY_FROM_IN(Circ, -1.0f * (sqrt(1 - t * t) - 1) );
		DEF_EASY_FROM_IN(Back, t * t * ((s + 1) * t - s));
		DEF_EASY_FROM_IN(Bounce, 1 - outBounce(1 - t));

		default:
			OX_ASSERT(!"unsupported ease");
			break;
		}

#undef DEF_EASY_FROM_IN

		return t;
	}


	string ease2String(Tween::EASE ease)
	{
#define STRINGIFY(x) #x
#define S(x) STRINGIFY(x)

		switch (ease)
		{
		case Tween::ease_linear:
			return "linear";

#define DEF_EASY_FROM_IN(EasyPost) \
		case Tween::ease_in ## EasyPost: \
			return  "in" S(EasyPost);\
		case Tween::ease_out ## EasyPost: \
			return "out" S(EasyPost); \
		case Tween::ease_inOut ## EasyPost: \
			return "inOut" S(EasyPost); \
		case Tween::ease_outIn ## EasyPost: \
			return "outIn" S(EasyPost);

			DEF_EASY_FROM_IN(Quad);
			DEF_EASY_FROM_IN(Cubic);
			DEF_EASY_FROM_IN(Quart);
			DEF_EASY_FROM_IN(Quint);
			DEF_EASY_FROM_IN(Sin);
			DEF_EASY_FROM_IN(Expo);
			DEF_EASY_FROM_IN(Circ);
			DEF_EASY_FROM_IN(Back);
			DEF_EASY_FROM_IN(Bounce);
		}

		return "unknown";
	}
	
	float Tween::_calcEase(float v)
	{
		if (_twoSides)
		{
			if (v > 0.5f)
				v = 1.0f - v;
			v *= 2.0f;
		}

		v = calcEase(_ease, v);
		return v;
	}

	void Tween::complete(timeMS deltaTime)
	{
		if (_loops == -1)
			return;

		//if already done
		if (_status >= status_done)
			return;

		OX_ASSERT(_client);

		//OX_ASSERT(!"not implemented");

		//not started yet because if delay
		if (_status == status_delayed)
		{
			_start(*_client);
			_status = status_started;
		}


		OX_ASSERT(_status == status_started);
		//while (_status != status_remove)
		{
			UpdateState us;
			us.dt = deltaTime;

			update(*_client, us);
		}

		OX_ASSERT(_status == status_done);
		
		//_client->removeTween(this);		
	}

	void Tween::start(Actor &actor)
	{
		_client = &actor; 
		_status = status_delayed;
		if (_delay == 0)
		{
			_status = status_started;
			_start(actor);
		}
	}

	void Tween::update(Actor &actor, const UpdateState &us)
	{
		_elapsed += us.dt;
		switch(_status)
		{
		case status_delayed:
			{
				if (_elapsed >= _delay)
				{
					_status = status_started;
					_start(*_client);					
				}
			}
			break;
		case status_started:
			{
				if (_duration)
				{				
					timeMS localElapsed = _elapsed - _delay;

					int loopsDone = localElapsed / _duration;

					_percent = _calcEase(((float)(localElapsed - loopsDone * _duration)) / _duration);

					if (_loops > 0 && int(loopsDone) >= _loops)
					{
						if (_twoSides)
							_percent = 0;
						else
							_percent = 1;

						_status = status_done;
					}
				}
				_update(*_client, us);
			}
			break;
		case status_done:
			{
				done(*_client, us);
			}
			break;
		}
	}

	void Tween::done(Actor &actor, const UpdateState &us)
	{		
		_done(actor, us);
		
		if (_detach)
		{
			actor.detach();
		}

		TweenEvent ev(this, &us);
		ev.target = ev.currentTarget = &actor;
		ev.tween = this;

		if (_cbDone)		
			_cbDone(&ev);

		dispatchEvent(&ev);

		_status = status_remove;
	}

	TweenQueue::TweenQueue()
	{
	}
	
	spTween TweenQueue::add(spTween t)
	{
		OX_ASSERT(t);
		if (!t)
			return 0;

		_tweens.append(t);
		ObjectScript::registerTween(this, t.get());
		return t;
	}

	void TweenQueue::complete(timeMS deltaTime)
	{
		OX_ASSERT("Tween::complete is not supported for TweenQueue");
	}

	void TweenQueue::_start(Actor &actor)
	{
		_current = _tweens._first;
		if (!_current)
			return;

		_current->start(actor);
	}

	void TweenQueue::_update(Actor &actor, const UpdateState &us)
	{
		_elapsed += us.dt;

		if (_current)
		{
			spTween next = _current->getNextSibling();
			_current->update(actor, us);
			if (_current->isDone())
			{
				_current = next;
				if (_current)				
					_current->start(actor);				
			}
		}

		if (!_current)
			_status = status_done;
	}

	Actor* TweenEvent::getActor() const
	{
		return safeCast<Actor*>(currentTarget.get());
	}
}