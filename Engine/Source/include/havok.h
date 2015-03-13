#pragma once

#define HK_DISABLE_OLD_VECTOR4_INTERFACE
#include <Common/Base/hkBase.h>

// This excludes libraries that are not going to be linked
// from the project configuration, even if the keycodes are present

#ifdef HK_FEATURE_PRODUCT_AI
	#undef HK_FEATURE_PRODUCT_AI
#endif
#ifdef HK_FEATURE_PRODUCT_ANIMATION
	#undef HK_FEATURE_PRODUCT_ANIMATION
#endif
#ifdef HK_FEATURE_PRODUCT_CLOTH
	#undef HK_FEATURE_PRODUCT_CLOTH
#endif
#ifdef HK_FEATURE_PRODUCT_DESTRUCTION
	#undef HK_FEATURE_PRODUCT_DESTRUCTION
#endif
#ifdef HK_FEATURE_PRODUCT_DESTRUCTION_2012
	#undef HK_FEATURE_PRODUCT_DESTRUCTION_2012
#endif
#ifdef HK_FEATURE_PRODUCT_BEHAVIOR
	#undef HK_FEATURE_PRODUCT_BEHAVIOR
#endif
#ifdef HK_FEATURE_PRODUCT_MILSIM
	#undef HK_FEATURE_PRODUCT_MILSIM
#endif