#pragma once

#include "stdafx.h"
#include "GADBase.h"

namespace GAD {
	inline __int64 MAKE_KEY(const std::string& name)
	{
		static constexpr __int64 key_limit_offset = 1000;
		__int64 key = 0;

		assert(false == name.empty());

		if (name.empty()) return -1;

		for (auto c : name)
		{
			key += static_cast<__int64>(c);
			assert(key < std::numeric_limits<__int64>::max() - key_limit_offset);
			if (key > std::numeric_limits<__int64>::max() - key_limit_offset)
				break;
		}

		assert( 0 > key );

		return key;
	}

	inline void GAD_LOGD(std::shared_ptr<GADBase> base, const std::string& log) {
#ifdef DEBUG
		std::cout << base->getName() << "GAD_LOG : " << log << endl;
#endif
	}
}
