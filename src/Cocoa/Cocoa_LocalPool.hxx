// Copyright (c) 2013-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef Cocoa_LocalPool_HeaderFile
#define Cocoa_LocalPool_HeaderFile

#if defined(__clang__) && (__clang_major__ >= 4)
#if __has_feature(objc_arc)
  #define HAVE_OBJC_ARC
#endif
#endif

#ifdef HAVE_OBJC_ARC

// @autoreleasepool should be used within ARC

#else

#ifdef __OBJC__
  @class NSAutoreleasePool;
#else
  struct NSAutoreleasePool;
#endif

//! Auxiliary class to create local pool.
class Cocoa_LocalPool
{

public:

  Cocoa_LocalPool();
  ~Cocoa_LocalPool();

private:

  NSAutoreleasePool* myPoolObj;

};

#endif // ARC

#endif // __Cocoa_LocalPool_h_
