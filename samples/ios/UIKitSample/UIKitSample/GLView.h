// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef GLView_h
#define GLView_h

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#import "GLViewController.h"

//! Open GL view
@interface GLView : UIView {
  
@public
  GLViewController* myController;
  
  EAGLContext* myGLContext;
  
  int myBackingWidth;
  int myBackingHeight;
  GLuint myFrameBuffer;
  GLuint myRenderBuffer;
  GLuint myDepthBuffer;
}

@end
#endif // GLView_h
