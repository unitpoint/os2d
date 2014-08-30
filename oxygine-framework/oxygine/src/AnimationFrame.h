#pragma once
#include "oxygine_include.h"
#include "math/Rect.h"
#include "core/Texture.h"
#include "core/NativeTexture.h"

namespace oxygine
{
	DECLARE_SMART(Texture, spTexture);
	DECLARE_SMART(NativeTexture, spNativeTexture);

	class Diffuse
	{
	public:
		Diffuse():premultiplied(true){}
		spNativeTexture base;
		spNativeTexture alpha;
		bool premultiplied;
	};

	class ResAnim;
	class AnimationFrame
	{
	public:
		AnimationFrame():/* _flags(0), */_srcRect(0, 0, 1, 1), _destRect(0, 0, 1, 1), _frameSize(0, 0), _resAnim(0){}

		void init(ResAnim *rs, const Diffuse &df, 
			const RectF &srcRect, const RectF &destRect, const Vector2 &frame_size);
		/**ResAnim should be valid!*/
		AnimationFrame getClipped(const RectF &rect) const;
		AnimationFrame getFlipped(bool vertical, bool horizontal) const;

		/**Returns size of frame in pixels*/		
		const Vector2&	getSize() const {return _frameSize;}
		float			getWidth() const {return _frameSize.x;}
		float			getHeight() const {return _frameSize.y;}
		//deprecated, use getSize
		const Vector2&	getFrameSize() const {return _frameSize;}

		ResAnim*		getResAnim() const {return _resAnim;} 
		const RectF&	getSrcRect() const {return _srcRect;}
		const RectF&	getDestRect() const {return _destRect;}
		const Diffuse&	getDiffuse() const {return _diffuse;}

		void			setSrcRect(const RectF &r){_srcRect = r;}
		void			setDestRect(const RectF &r){_destRect = r;}
		void			setResAnim(ResAnim *rs){_resAnim = rs;}
		

	private:
		enum flags
		{
			clipped = 0x01,
		};
		
		Diffuse			_diffuse;

		RectF			_srcRect;
		RectF			_destRect;
		Vector2			_frameSize;//real size without clipping
		ResAnim*		_resAnim;
		//unsigned short	_flags;
	};
}
