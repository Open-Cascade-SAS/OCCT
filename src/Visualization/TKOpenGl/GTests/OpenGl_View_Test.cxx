// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <gtest/gtest.h>

#if defined(HAVE_XLIB)
  #include <Aspect_DisplayConnection.hxx>
  #include <Graphic3d_StructureManager.hxx>
  #include <Graphic3d_Camera.hxx>
  #include <Graphic3d_RenderingParams.hxx>
  #include <OpenGl_Caps.hxx>
  #include <OpenGl_GraphicDriver.hxx>
  #include <OpenGl_View.hxx>
  #include <OpenGl_Window.hxx>
  #include <OpenGl_FrameBuffer.hxx>
  #include <OpenGl_Context.hxx>
  #include <OpenGl_ArbFBO.hxx>
  #include <Xw_Window.hxx>
  #include <cstdlib>

namespace
{
// Simulates unsupported OIT framebuffer formats without rejecting scene buffers.
class OpenGl_TestFramebufferStatus
{
public:
  OpenGl_TestFramebufferStatus(const occ::handle<OpenGl_Context>& theContext,
                               const bool                         theToFailSingleSample = false);

  ~OpenGl_TestFramebufferStatus();

  OpenGl_TestFramebufferStatus(const OpenGl_TestFramebufferStatus&)            = delete;
  OpenGl_TestFramebufferStatus& operator=(const OpenGl_TestFramebufferStatus&) = delete;

  size_t NbFailures() const;

private:
  static GLenum APIENTRY checkStatus(GLenum theTarget);

  static OpenGl_TestFramebufferStatus*  myActiveCallback; // Active callback owner.
  const occ::handle<OpenGl_Context>     myContext;     // Context whose function table is replaced.
  const PFNGLCHECKFRAMEBUFFERSTATUSPROC myCheckStatus; // Original status function.
  const bool myToFailSingleSample; // Whether to reject single-sample OIT buffers too.
  size_t     myNbFailures;         // Number of rejected OIT buffers.
};

OpenGl_TestFramebufferStatus* OpenGl_TestFramebufferStatus::myActiveCallback = nullptr;

//==================================================================================================

OpenGl_TestFramebufferStatus::OpenGl_TestFramebufferStatus(
  const occ::handle<OpenGl_Context>& theContext,
  const bool                         theToFailSingleSample)
    : myContext(theContext),
      myCheckStatus(theContext->arbFBO->glCheckFramebufferStatus),
      myToFailSingleSample(theToFailSingleSample),
      myNbFailures(0)
{
  myActiveCallback                            = this;
  myContext->arbFBO->glCheckFramebufferStatus = checkStatus;
}

//==================================================================================================

OpenGl_TestFramebufferStatus::~OpenGl_TestFramebufferStatus()
{
  myContext->arbFBO->glCheckFramebufferStatus = myCheckStatus;
  myActiveCallback                            = nullptr;
}

//==================================================================================================

size_t OpenGl_TestFramebufferStatus::NbFailures() const
{
  return myNbFailures;
}

//==================================================================================================

GLenum APIENTRY OpenGl_TestFramebufferStatus::checkStatus(GLenum theTarget)
{
  OpenGl_TestFramebufferStatus& aStatus          = *myActiveCallback;
  GLint                         anAttachmentType = GL_NONE;
  aStatus.myContext->arbFBO->glGetFramebufferAttachmentParameteriv(
    theTarget,
    GL_COLOR_ATTACHMENT1,
    GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
    &anAttachmentType);
  GLint aNbSamples = 0;
  aStatus.myContext->core11fwd->glGetIntegerv(GL_SAMPLES, &aNbSamples);
  if (anAttachmentType != GL_NONE && (aNbSamples > 0 || aStatus.myToFailSingleSample))
  {
    ++aStatus.myNbFailures;
    return GL_FRAMEBUFFER_UNSUPPORTED;
  }
  return aStatus.myCheckStatus(theTarget);
}

// Exposes framebuffer preparation and fallback status for regression checks.
class OpenGl_TestView : public OpenGl_View
{
public:
  OpenGl_TestView(const occ::handle<Graphic3d_StructureManager>& theManager,
                  const occ::handle<OpenGl_GraphicDriver>&       theDriver);

  bool Prepare();

  const occ::handle<OpenGl_FrameBuffer>& OitBuffer() const;

  const occ::handle<OpenGl_FrameBuffer>& SceneBuffer() const;

  bool MsaaDisabled() const;

  bool OitDisabled() const;

  bool OitMsaaDisabled() const;
};

//==================================================================================================

OpenGl_TestView::OpenGl_TestView(const occ::handle<Graphic3d_StructureManager>& theManager,
                                 const occ::handle<OpenGl_GraphicDriver>&       theDriver)
    : OpenGl_View(theManager, theDriver, new OpenGl_Caps, theDriver->GetStateCounter())
{
}

//==================================================================================================

bool OpenGl_TestView::Prepare()
{
  Graphic3d_Camera::Projection aProjection;
  return prepareFrameBuffers(aProjection);
}

//==================================================================================================

const occ::handle<OpenGl_FrameBuffer>& OpenGl_TestView::OitBuffer() const
{
  return myMainSceneFbosOit[0];
}

//==================================================================================================

const occ::handle<OpenGl_FrameBuffer>& OpenGl_TestView::SceneBuffer() const
{
  return myMainSceneFbos[0];
}

//==================================================================================================

bool OpenGl_TestView::MsaaDisabled() const
{
  return myToDisableMSAA;
}

//==================================================================================================

bool OpenGl_TestView::OitDisabled() const
{
  return myToDisableOIT;
}

//==================================================================================================

bool OpenGl_TestView::OitMsaaDisabled() const
{
  return myToDisableOITMSAA;
}

// Creates an X11 view and releases its GPU resources after each test.
class OpenGl_ViewTest : public ::testing::Test
{
protected:
  void SetUp() override;

  void TearDown() override;

  void CheckSingleSampleOit();

  occ::handle<Aspect_DisplayConnection>   myDisplay; // X11 display connection.
  occ::handle<OpenGl_GraphicDriver>       myDriver;  // Driver owning the OpenGL resources.
  occ::handle<Graphic3d_StructureManager> myManager; // Structure manager for the test view.
  occ::handle<Xw_Window>                  myWindow;  // Native window for the OpenGL context.
  occ::handle<OpenGl_TestView>            myView;    // View under test.
  occ::handle<OpenGl_Context>             myContext; // Current context for framebuffer allocation.
};

//==================================================================================================

void OpenGl_ViewTest::SetUp()
{
  if (std::getenv("DISPLAY") == nullptr)
  {
    GTEST_SKIP() << "Requires an X11 OpenGL display (for example xvfb-run).";
  }
  myDisplay = new Aspect_DisplayConnection;
  myDriver  = new OpenGl_GraphicDriver(myDisplay);
  myManager = new Graphic3d_StructureManager(myDriver);
  myView    = new OpenGl_TestView(myManager, myDriver);
  myWindow  = new Xw_Window(myDisplay, "OCCT OIT regression", 0, 0, 64, 64);
  myView->SetWindow(nullptr, myWindow, nullptr);
  myContext = myView->GlWindow()->GetGlContext();
  ASSERT_TRUE(myContext->MakeCurrent());
  if (myContext->MaxMsaaSamples() < 2 || !myContext->HasTextureMultisampling()
      || (myContext->hasFloatBuffer == OpenGl_FeatureNotAvailable
          && myContext->hasHalfFloatBuffer == OpenGl_FeatureNotAvailable)
      || myContext->hasDrawBuffers == OpenGl_FeatureNotAvailable)
  {
    GTEST_SKIP() << "Requires floating-point OIT and MSAA support.";
  }
  myView->ChangeRenderingParams().TransparencyMethod = Graphic3d_RTM_BLEND_OIT;
  myView->ChangeRenderingParams().NbMsaaSamples      = 4;
}

//==================================================================================================

void OpenGl_ViewTest::TearDown()
{
  if (!myView.IsNull())
  {
    myView->Remove();
    myView.Nullify();
  }
}

//==================================================================================================

void OpenGl_ViewTest::CheckSingleSampleOit()
{
  ASSERT_TRUE(myView->Prepare());
  ASSERT_TRUE(myView->OitBuffer()->IsValid());
  EXPECT_EQ(myView->OitBuffer()->NbSamples(), 0);
  ASSERT_TRUE(myView->SceneBuffer()->IsValid());
  EXPECT_EQ(myView->SceneBuffer()->NbSamples(), 0);
  EXPECT_FALSE(myView->MsaaDisabled());
  EXPECT_FALSE(myView->OitDisabled());
}
} // namespace

#else
namespace
{
class OpenGl_ViewTest : public ::testing::Test
{
protected:
  void SetUp() override;
};

//==================================================================================================

void OpenGl_ViewTest::SetUp()
{
  GTEST_SKIP() << "Requires an X11 OpenGL context.";
}
} // namespace
#endif

TEST_F(OpenGl_ViewTest, MissingSampleVariablesRetainsSingleSampleOit)
{
#if defined(HAVE_XLIB)
  myContext->hasSampleVariables = OpenGl_FeatureNotAvailable;
  CheckSingleSampleOit();
#endif
}

TEST_F(OpenGl_ViewTest, SupportedMsaaOitKeepsMsaa)
{
#if defined(HAVE_XLIB)
  if (myContext->hasSampleVariables == OpenGl_FeatureNotAvailable)
  {
    GTEST_SKIP() << "Requires GLSL sample variables for MSAA OIT.";
  }
  ASSERT_TRUE(myView->Prepare());
  if (myView->OitMsaaDisabled())
  {
    GTEST_SKIP() << "Requires MSAA OIT framebuffer formats supported by this context.";
  }
  ASSERT_TRUE(myView->OitBuffer()->IsValid());
  ASSERT_TRUE(myView->SceneBuffer()->IsValid());
  EXPECT_GT(myView->SceneBuffer()->NbSamples(), 0);
  EXPECT_EQ(myView->OitBuffer()->NbSamples(), myView->SceneBuffer()->NbSamples());
  EXPECT_FALSE(myView->OitMsaaDisabled());
  EXPECT_FALSE(myView->MsaaDisabled());
  EXPECT_FALSE(myView->OitDisabled());
#endif
}

TEST_F(OpenGl_ViewTest, FailedMsaaOitRetainsSingleSampleOit)
{
#if defined(HAVE_XLIB)
  if (myContext->hasSampleVariables == OpenGl_FeatureNotAvailable)
  {
    GTEST_SKIP() << "Requires GLSL sample variables for MSAA OIT.";
  }
  OpenGl_TestFramebufferStatus aStatus(myContext);
  CheckSingleSampleOit();
  EXPECT_GT(aStatus.NbFailures(), 0);
  EXPECT_TRUE(myView->OitMsaaDisabled());
  const size_t aNbFailures = aStatus.NbFailures();
  CheckSingleSampleOit();
  EXPECT_EQ(aStatus.NbFailures(), aNbFailures);
#endif
}

TEST_F(OpenGl_ViewTest, FailedSingleSampleOitDisablesOit)
{
#if defined(HAVE_XLIB)
  if (myContext->hasSampleVariables == OpenGl_FeatureNotAvailable)
  {
    GTEST_SKIP() << "Requires GLSL sample variables for MSAA OIT.";
  }
  OpenGl_TestFramebufferStatus aStatus(myContext, true);
  ASSERT_TRUE(myView->Prepare());
  EXPECT_GT(aStatus.NbFailures(), 0);
  EXPECT_TRUE(myView->OitMsaaDisabled());
  EXPECT_TRUE(myView->OitDisabled());
  EXPECT_FALSE(myView->MsaaDisabled());
  EXPECT_FALSE(myView->OitBuffer()->IsValid());
  ASSERT_TRUE(myView->SceneBuffer()->IsValid());
  EXPECT_EQ(myView->SceneBuffer()->NbSamples(), 0);
#endif
}
