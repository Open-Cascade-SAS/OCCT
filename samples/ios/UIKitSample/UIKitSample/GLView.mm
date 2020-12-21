// Copyright (c) 2017 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

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
  if (!myGLContext)
  {
    NSLog(@"Failed to initialize OpenGL ES 2.0 context");
  }

  if (![EAGLContext setCurrentContext:myGLContext])
  {
    NSLog(@"Failed to set current OpenGL ES context");
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
