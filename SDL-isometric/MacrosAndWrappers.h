// include guards
#ifndef MACROSANDWRAPPERS_H
#define MACROSANDWRAPPERS_H

//---------------------------------------------------------------------------------------------------------------------
#include <cassert>

// AA_ASSERT = Debug only asserts, handy if you don't want them in release builds.
// AA_VERIFY = same as the assert but the parameter is passed through so it still executes in release builds.
#ifdef _DEBUG
#define AA_ASSERT(x) assert(x)
#define AA_VERIFY(x) assert(x)
#else
#define AA_ASSERT(x)
#define AA_VERIFY(x) x
#endif

// safer deletion of new and new [] allocated data, checks it's not NULL, nukes it, then NULL the pointer.
#define SAFE_DELETE(x) if(nullptr!=x) { delete x; x=nullptr; }
#define SAFE_DELETE_ARRAY(x) if(nullptr!=x) { delete [] x; x=nullptr; }

#endif // MACROSANDWRAPPERS_H