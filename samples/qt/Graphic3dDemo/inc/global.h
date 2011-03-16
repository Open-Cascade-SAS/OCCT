#ifndef GLOBAL_H
#define GLOBAL_H

#include <qobject.h>
#include <qstring.h>

#if defined WNT
#ifdef _DEBUG
#include <assert.h>
#define verify(a)   assert(a)
#else
#define verify(a) a
#endif
#else
#define verify(a)   a;
#endif

#ifndef WNT
#define true TRUE
#define false FALSE
#endif

#endif
