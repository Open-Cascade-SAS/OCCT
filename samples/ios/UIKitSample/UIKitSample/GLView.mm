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

#import <Foundation/Foundation.h>

#import "GLView.h"
#import "GLViewController.h"

@implementation GLView

// =======================================================================
// function : layerClass
// purpose  :
// =======================================================================
+ (Class)layerClass
{
  return [CAEAGLLayer class];
}

// =======================================================================
// function : setupLayer
// purpose  :
// =======================================================================
- (void)setupLayer
{
  CAEAGLLayer* anEAGLLayer = (CAEAGLLayer*) self.layer;
  anEAGLLayer.opaque = YES;
  anEAGLLayer.contentsScale = [[UIScreen mainScreen] scale];
}

// =======================================================================
// function : setupContext
// purpose  :
// =======================================================================
- (void)setupContext
{
  EAGLRenderingAPI aRendApi = kEAGLRenderingAPIOpenGLES2;
  myGLContext = [[EAGLContext alloc] initWithAPI:aRendApi];
  if (!myGLContext) {
    NSLog(@"Failed to initialize OpenGLES 2.0 context");
  }
  
  if (![EAGLContext setCurrentContext:myGLContext]) {
    NSLog(@"Failed to set current OpenGL context");
  }
}

// =======================================================================
// function : createBuffers
// purpose  :
// =======================================================================
- (void) createBuffers
{
  glGenFramebuffers(1, &myFrameBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
  glGenRenderbuffers(1, &myRenderBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, myRenderBuffer);
  
  [myGLContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)self.layer];
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, myRenderBuffer);
  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &myBackingWidth);
  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &myBackingHeight);
  
  glGenRenderbuffers(1, &myDepthBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, myDepthBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, myBackingWidth, myBackingHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, myDepthBuffer);
}

// =======================================================================
// function : destroyBuffers
// purpose  :
// =======================================================================
- (void) destroyBuffers
{
  glDeleteFramebuffers(1, &myFrameBuffer);
  myFrameBuffer = 0;
  glDeleteRenderbuffers(1, &myRenderBuffer);
  myRenderBuffer = 0;
  glDeleteRenderbuffers(1, &myDepthBuffer);
  myDepthBuffer = 0;
}

// =======================================================================
// function : drawView
// purpose  :
// =======================================================================
- (void) drawView
{
  glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
  
  [myController Draw];
  
  glBindRenderbuffer(GL_RENDERBUFFER, myRenderBuffer);
  [myGLContext presentRenderbuffer:GL_RENDERBUFFER];
}

// =======================================================================
// function : layoutSubviews
// purpose  :
// =======================================================================
- (void) layoutSubviews
{
  [EAGLContext setCurrentContext:myGLContext];
  
  [self destroyBuffers];
  [self createBuffers];
  [self drawView];
  
  glBindRenderbuffer(GL_RENDERBUFFER, myRenderBuffer);
  
  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &myBackingWidth);
  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &myBackingHeight);
  
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    NSLog(@"Failed to make complete framebuffer object %u",
          glCheckFramebufferStatus(GL_FRAMEBUFFER));
  }
  
  glViewport(0, 0, myBackingWidth, myBackingHeight);
  
  [myController Setup];
}

// =======================================================================
// function : init
// purpose  :
// =======================================================================
- (id) init
{
  self = [super init];
  
  if (self) {
    [self setupLayer];
    [self setupContext];
    
    myController = NULL;
    
    myBackingWidth  = 0;
    myBackingHeight = 0;
    myFrameBuffer   = 0;
    myRenderBuffer  = 0;
    myDepthBuffer   = 0;
  }
  
  return self;
}

@end
