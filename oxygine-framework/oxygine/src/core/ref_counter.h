#pragma once
#include "oxygine_include.h"

#include "intrusive_ptr.h"

namespace oxygine
{
	/** RefCounter **/
	class ref_counter
	{				
	public:
		int _ref_counter;

#ifdef OX_WITH_OBJECTSCRIPT
		int osValueId;
#endif

		ref_counter():_ref_counter(0)
		{
#ifdef OX_WITH_OBJECTSCRIPT
			osValueId = 0;
#endif
		}

		virtual ~ref_counter()
		{
#ifdef OX_WITH_OBJECTSCRIPT
			OX_ASSERT(!osValueId);
#endif
		}

		void addRef()
		{
			++_ref_counter;
		}

		void releaseRef()
		{
			if (0 == --_ref_counter){
#ifdef OX_WITH_OBJECTSCRIPT
				OX_ASSERT(!osValueId);
#endif
				delete this;
			}
		}


		//commented, unsafe
		//todo, find solution

	private: 
		ref_counter( const ref_counter& );
		const ref_counter& operator=( const ref_counter& );	

	};


	inline void intrusive_ptr_add_ref(ref_counter *p)
	{
		p->addRef();
	}

	inline void intrusive_ptr_release(ref_counter *p)
	{
		p->releaseRef();
	}

#define DECLARE_SMART(class_name, spname) class class_name;\
	typedef oxygine::intrusive_ptr<class_name> spname;

#define DECLARENS_SMART(name_space, class_name, spname) namespace name_space \
	{\
		class class_name;\
		typedef oxygine::intrusive_ptr<class_name> spname;\
	}

}
