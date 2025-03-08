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

#if (defined(__APPLE__))

#include <Cocoa_LocalPool.hxx>

#import <TargetConditionals.h>

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  #import <UIKit/UIKit.h>
#else
  #import <Cocoa/Cocoa.h>
#endif

#ifndef HAVE_OBJC_ARC

// =======================================================================
// function : Cocoa_LocalPool
// purpose  :
// =======================================================================
Cocoa_LocalPool::Cocoa_LocalPool()
: myPoolObj ([[NSAutoreleasePool alloc] init])
{
  //
}

// =======================================================================
// function : ~Cocoa_LocalPool
// purpose  :
// =======================================================================
Cocoa_LocalPool::~Cocoa_LocalPool()
{
  //[myPoolObj drain];
  [myPoolObj release];
}

#endif

#endif // __APPLE__
