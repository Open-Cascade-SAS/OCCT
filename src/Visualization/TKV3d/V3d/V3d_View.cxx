// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <V3d_View.hxx>

#include <Aspect_CircularGrid.hxx>
#include <Aspect_GradientBackground.hxx>
#include <Aspect_Grid.hxx>
#include <Aspect_NeutralWindow.hxx>
#include <Aspect_RectangularGrid.hxx>
#include <Aspect_Window.hxx>
#include <Bnd_Box.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_MapOfStructure.hxx>
#include <Graphic3d_MapOfStructure.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_TextureEnv.hxx>
#include <Image_AlienPixMap.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Assert.hxx>
#include <Standard_DivideByZero.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_ShortReal.hxx>
#include <Standard_Type.hxx>
#include <Standard_TypeMismatch.hxx>
#include <NCollection_Array2.hxx>
#include <V3d.hxx>
#include <V3d_BadValue.hxx>
#include <V3d_Light.hxx>
#include <V3d_StereoDumpOptions.hxx>
#include <V3d_UnMapped.hxx>
#include <V3d_Viewer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(V3d_View, Standard_Transient)

namespace
{
constexpr double THE_TWO_PI          = 2.0 * M_PI;
constexpr int    THE_NB_BOUND_POINTS = 8;
} // namespace

//=================================================================================================

V3d_View::V3d_View(const occ::handle<V3d_Viewer>& theViewer, const V3d_TypeOfView theType)
    : myIsInvalidatedImmediate(true),
      MyViewer(theViewer.operator->()),
      SwitchSetFront(false),
      myZRotation(false),
      MyTrsf(1, 4, 1, 4)
{
  myView = theViewer->Driver()->CreateView(theViewer->StructureManager());

  myView->SetBackground(theViewer->GetBackgroundColor());
  myView->SetGradientBackground(theViewer->GetGradientBackground());

  ChangeRenderingParams() = theViewer->DefaultRenderingParams();

  // camera init
  occ::handle<Graphic3d_Camera> aCamera = new Graphic3d_Camera();
  aCamera->SetFOVy(45.0);
  aCamera->SetIOD(Graphic3d_Camera::IODType_Relative, 0.05);
  aCamera->SetZFocus(Graphic3d_Camera::FocusType_Relative, 1.0);
  aCamera->SetProjectionType((theType == V3d_ORTHOGRAPHIC)
                               ? Graphic3d_Camera::Projection_Orthographic
                               : Graphic3d_Camera::Projection_Perspective);

  myDefaultCamera = new Graphic3d_Camera();

  myImmediateUpdate = false;
  SetAutoZFitMode(true, 1.0);
  SetBackFacingModel(V3d_TOBM_AUTOMATIC);
  SetCamera(aCamera);
  SetAxis(0., 0., 0., 1., 1., 1.);
  SetVisualization(theViewer->DefaultVisualization());
  SetTwist(0.);
  SetAt(0., 0., 0.);
  SetProj(theViewer->DefaultViewProj());
  SetSize(theViewer->DefaultViewSize());
  double zsize = theViewer->DefaultViewSize();
  SetZSize(2. * zsize);
  SetDepth(theViewer->DefaultViewSize() / 2.0);
  SetViewMappingDefault();
  SetViewOrientationDefault();
  theViewer->AddView(this);
  Init();
  myImmediateUpdate = true;
}

//=================================================================================================

V3d_View::V3d_View(const occ::handle<V3d_Viewer>& theViewer, const occ::handle<V3d_View>& theView)
    : myIsInvalidatedImmediate(true),
      MyViewer(theViewer.operator->()),
      SwitchSetFront(false),
      myZRotation(false),
      MyTrsf(1, 4, 1, 4)
{
  myView = theViewer->Driver()->CreateView(theViewer->StructureManager());

  myView->CopySettings(theView->View());
  myDefaultViewPoint = theView->myDefaultViewPoint;
  myDefaultViewAxis  = theView->myDefaultViewAxis;

  myDefaultCamera = new Graphic3d_Camera(theView->DefaultCamera());

  myImmediateUpdate = false;
  SetAutoZFitMode(theView->AutoZFitMode(), theView->AutoZFitScaleFactor());
  theViewer->AddView(this);
  Init();
  myImmediateUpdate = true;
}

//=================================================================================================

V3d_View::~V3d_View()
{
  if (myParentView != nullptr)
  {
    myParentView->RemoveSubview(this);
    myParentView = nullptr;
  }
  {
    NCollection_Sequence<occ::handle<V3d_View>> aSubviews = mySubviews;
    mySubviews.Clear();
    for (const occ::handle<V3d_View>& aViewIter : aSubviews)
    {
      // aViewIter->Remove();
      aViewIter->myParentView = nullptr;
      aViewIter->MyWindow.Nullify();
      aViewIter->myView->Remove();
      if (aViewIter->MyViewer != nullptr)
      {
        aViewIter->MyViewer->SetViewOff(aViewIter);
      }
    }
  }

  if (!myView->IsRemoved())
  {
    myView->Remove();
  }
}

//=================================================================================================

void V3d_View::SetMagnify(const occ::handle<Aspect_Window>& theWindow,
                          const occ::handle<V3d_View>&      thePreviousView,
                          const int       theX1,
                          const int       theY1,
                          const int       theX2,
                          const int       theY2)
{
  if (!myView->IsRemoved() && !myView->IsDefined())
  {
    double aU1, aV1, aU2, aV2;
    thePreviousView->Convert(theX1, theY1, aU1, aV1);
    thePreviousView->Convert(theX2, theY2, aU2, aV2);
    myView->SetWindow(occ::handle<Graphic3d_CView>(), theWindow, nullptr);
    FitAll(aU1, aV1, aU2, aV2);
    MyViewer->SetViewOn(this);
    MyWindow = theWindow;
    SetRatio();
    Redraw();
    SetViewMappingDefault();
  }
}

//=================================================================================================

void V3d_View::SetWindow(const occ::handle<Aspect_Window>&  theWindow,
                         const Aspect_RenderingContext theContext)
{
  if (myView->IsRemoved())
  {
    return;
  }
  if (myParentView != nullptr)
  {
    throw Standard_ProgramError("V3d_View::SetWindow() called twice");
  }

  // method V3d_View::SetWindow() should assign the field MyWindow before calling Redraw()
  MyWindow = theWindow;
  myView->SetWindow(occ::handle<Graphic3d_CView>(), theWindow, theContext);
  MyViewer->SetViewOn(this);
  SetRatio();
  if (myImmediateUpdate)
  {
    Redraw();
  }
}

//=================================================================================================

void V3d_View::SetWindow(const occ::handle<V3d_View>&       theParentView,
                         const NCollection_Vec2<double>&        theSize,
                         Aspect_TypeOfTriedronPosition theCorner,
                         const NCollection_Vec2<double>&        theOffset,
                         const NCollection_Vec2<int>&        theMargins)
{
  if (myView->IsRemoved())
  {
    return;
  }

  occ::handle<V3d_View> aParentView =
    !theParentView->IsSubview() ? theParentView : theParentView->ParentView();
  if (aParentView != myParentView)
  {
    if (myParentView != nullptr)
    {
      throw Standard_ProgramError("V3d_View::SetWindow() called twice");
    }

    myParentView = aParentView.get();
    aParentView->AddSubview(this);
  }

  occ::handle<Aspect_NeutralWindow> aWindow = new Aspect_NeutralWindow();
  aWindow->SetVirtual(true);
  aWindow->SetSize(4, 4);
  myView->SetSubviewCorner(theCorner);
  myView->SetSubviewSize(theSize);
  myView->SetSubviewOffset(theOffset);
  myView->SetSubviewMargins(theMargins);

  MyWindow = aWindow;
  myView->SetWindow(aParentView->View(), aWindow, 0);
  MyViewer->SetViewOn(this);
  SetRatio();
}

//=================================================================================================

void V3d_View::Remove()
{
  if (!MyGrid.IsNull())
  {
    MyGrid->Erase();
  }
  if (!myTrihedron.IsNull())
  {
    myTrihedron->Erase();
  }

  if (myParentView != nullptr)
  {
    myParentView->RemoveSubview(this);
    myParentView = nullptr;
  }
  {
    NCollection_Sequence<occ::handle<V3d_View>> aSubviews = mySubviews;
    mySubviews.Clear();
    for (const occ::handle<V3d_View>& aViewIter : aSubviews)
    {
      aViewIter->Remove();
    }
  }

  if (MyViewer != nullptr)
  {
    MyViewer->DelView(this);
    MyViewer = nullptr;
  }
  myView->Remove();
  MyWindow.Nullify();
}

//=================================================================================================

void V3d_View::AddSubview(const occ::handle<V3d_View>& theView)
{
  mySubviews.Append(theView);
}

//=================================================================================================

bool V3d_View::RemoveSubview(const V3d_View* theView)
{
  for (NCollection_Sequence<occ::handle<V3d_View>>::Iterator aViewIter(mySubviews); aViewIter.More();
       aViewIter.Next())
  {
    if (aViewIter.Value() == theView)
    {
      mySubviews.Remove(aViewIter);
      return true;
    }
  }
  return false;
}

//=================================================================================================

occ::handle<V3d_View> V3d_View::PickSubview(const NCollection_Vec2<int>& thePnt) const
{
  if (thePnt.x() < 0 || thePnt.x() >= MyWindow->Dimensions().x() || thePnt.y() < 0
      || thePnt.y() >= MyWindow->Dimensions().y())
  {
    return occ::handle<V3d_View>();
  }

  // iterate in opposite direction - from front to bottom views
  for (int aSubviewIter = mySubviews.Upper(); aSubviewIter >= mySubviews.Lower();
       --aSubviewIter)
  {
    const occ::handle<V3d_View>& aSubview = mySubviews.Value(aSubviewIter);
    if (aSubview->View()->IsActive() && thePnt.x() >= aSubview->View()->SubviewTopLeft().x()
        && thePnt.x()
             < (aSubview->View()->SubviewTopLeft().x() + aSubview->Window()->Dimensions().x())
        && thePnt.y() >= aSubview->View()->SubviewTopLeft().y()
        && thePnt.y()
             < (aSubview->View()->SubviewTopLeft().y() + aSubview->Window()->Dimensions().y()))
    {
      return aSubview;
    }
  }

  return this;
}

//=================================================================================================

void V3d_View::Update() const
{
  if (!myView->IsDefined() || !myView->IsActive())
  {
    return;
  }

  myIsInvalidatedImmediate = false;
  myView->Update();
  myView->Compute();
  AutoZFit();
  myView->Redraw();
}

//=================================================================================================

void V3d_View::Redraw() const
{
  if (!myView->IsDefined() || !myView->IsActive())
  {
    return;
  }

  myIsInvalidatedImmediate                         = false;
  occ::handle<Graphic3d_StructureManager> aStructureMgr = MyViewer->StructureManager();
  for (int aRetryIter = 0; aRetryIter < 2; ++aRetryIter)
  {
    if (aStructureMgr->IsDeviceLost())
    {
      aStructureMgr->RecomputeStructures();
    }

    AutoZFit();

    myView->Redraw();

    if (!aStructureMgr->IsDeviceLost())
    {
      return;
    }
  }
}

//=================================================================================================

void V3d_View::RedrawImmediate() const
{
  if (!myView->IsDefined() || !myView->IsActive())
  {
    return;
  }

  myIsInvalidatedImmediate = false;
  myView->RedrawImmediate();
}

//=================================================================================================

void V3d_View::Invalidate() const
{
  if (!myView->IsDefined())
  {
    return;
  }

  myView->Invalidate();
}

//=================================================================================================

bool V3d_View::IsInvalidated() const
{
  return !myView->IsDefined() || myView->IsInvalidated();
}

//=================================================================================================

void V3d_View::SetAutoZFitMode(const bool theIsOn, const double theScaleFactor)
{
  Standard_ASSERT_RAISE(theScaleFactor > 0.0, "Zero or negative scale factor is not allowed.");
  myAutoZFitScaleFactor = theScaleFactor;
  myAutoZFitIsOn        = theIsOn;
}

//=================================================================================================

void V3d_View::AutoZFit() const
{
  if (!AutoZFitMode())
  {
    return;
  }

  ZFitAll(myAutoZFitScaleFactor);
}

//=================================================================================================

void V3d_View::ZFitAll(const double theScaleFactor) const
{
  Bnd_Box aMinMaxBox = myView->MinMaxValues(false); // applicative min max boundaries
                                                             // clang-format off
  Bnd_Box aGraphicBox  = myView->MinMaxValues (true);  // real graphical boundaries (not accounting infinite flag).
                                                             // clang-format on

  myView->Camera()->ZFitAll(theScaleFactor, aMinMaxBox, aGraphicBox);
}

//=================================================================================================

bool V3d_View::IsEmpty() const
{
  bool TheStatus = true;
  if (myView->IsDefined())
  {
    int Nstruct = myView->NumberOfDisplayedStructures();
    if (Nstruct > 0)
      TheStatus = false;
  }
  return (TheStatus);
}

//=================================================================================================

void V3d_View::UpdateLights() const
{
  occ::handle<Graphic3d_LightSet> aLights = new Graphic3d_LightSet();
  for (NCollection_List<occ::handle<Graphic3d_CLight>>::Iterator anActiveLightIter(myActiveLights); anActiveLightIter.More();
       anActiveLightIter.Next())
  {
    aLights->Add(anActiveLightIter.Value());
  }
  myView->SetLights(aLights);
}

//=================================================================================================

void V3d_View::DoMapping()
{
  if (!myView->IsDefined())
  {
    return;
  }

  myView->Window()->DoMapping();
}

//=================================================================================================

void V3d_View::MustBeResized()
{
  if (!myView->IsDefined())
  {
    return;
  }

  myView->Resized();

  SetRatio();
  if (myImmediateUpdate)
  {
    Redraw();
  }
}

//=================================================================================================

void V3d_View::SetBackgroundColor(const Quantity_TypeOfColor theType,
                                  const double        theV1,
                                  const double        theV2,
                                  const double        theV3)
{
  double aV1 = std::max(std::min(theV1, 1.0), 0.0);
  double aV2 = std::max(std::min(theV2, 1.0), 0.0);
  double aV3 = std::max(std::min(theV3, 1.0), 0.0);

  SetBackgroundColor(Quantity_Color(aV1, aV2, aV3, theType));
}

//=================================================================================================

void V3d_View::SetBackgroundColor(const Quantity_Color& theColor)
{
  myView->SetBackground(Aspect_Background(theColor));

  if (myImmediateUpdate)
  {
    Redraw();
  }
}

//=================================================================================================

void V3d_View::SetBgGradientColors(const Quantity_Color&           theColor1,
                                   const Quantity_Color&           theColor2,
                                   const Aspect_GradientFillMethod theFillStyle,
                                   const bool          theToUpdate)
{
  Aspect_GradientBackground aGradientBg(theColor1, theColor2, theFillStyle);

  myView->SetGradientBackground(aGradientBg);

  if (myImmediateUpdate || theToUpdate)
  {
    Redraw();
  }
}

//=================================================================================================

void V3d_View::SetBgGradientStyle(const Aspect_GradientFillMethod theFillStyle,
                                  const bool          theToUpdate)
{
  Quantity_Color aColor1;
  Quantity_Color aColor2;
  GradientBackground().Colors(aColor1, aColor2);

  SetBgGradientColors(aColor1, aColor2, theFillStyle, theToUpdate);
}

//=================================================================================================

void V3d_View::SetBackgroundImage(const char* const  theFileName,
                                  const Aspect_FillMethod theFillStyle,
                                  const bool  theToUpdate)
{
  occ::handle<Graphic3d_Texture2D> aTextureMap = new Graphic3d_Texture2D(theFileName);
  aTextureMap->DisableModulate();
  SetBackgroundImage(aTextureMap, theFillStyle, theToUpdate);
}

//=================================================================================================

void V3d_View::SetBackgroundImage(const occ::handle<Graphic3d_Texture2D>& theTexture,
                                  const Aspect_FillMethod            theFillStyle,
                                  const bool             theToUpdate)
{
  myView->SetBackgroundImage(theTexture);
  myView->SetBackgroundImageStyle(theFillStyle);
  if (myImmediateUpdate || theToUpdate)
  {
    Redraw();
  }
}

//=================================================================================================

void V3d_View::SetBgImageStyle(const Aspect_FillMethod theFillStyle,
                               const bool  theToUpdate)
{
  myView->SetBackgroundImageStyle(theFillStyle);

  if (myImmediateUpdate || theToUpdate)
  {
    Redraw();
  }
}

//=================================================================================================

void V3d_View::SetBackgroundCubeMap(const occ::handle<Graphic3d_CubeMap>& theCubeMap,
                                    bool                 theToUpdatePBREnv,
                                    bool                 theToUpdate)
{
  myView->SetBackgroundImage(theCubeMap, theToUpdatePBREnv);
  if (myImmediateUpdate || theToUpdate)
  {
    Redraw();
  }
}

//=================================================================================================

void V3d_View::SetBackgroundSkydome(const Aspect_SkydomeBackground& theAspect,
                                    bool                theToUpdatePBREnv)
{
  myView->SetBackgroundSkydome(theAspect, theToUpdatePBREnv);
}

//=================================================================================================

bool V3d_View::IsImageBasedLighting() const
{
  return !myView->IBLCubeMap().IsNull();
}

//=================================================================================================

void V3d_View::SetImageBasedLighting(bool theToEnableIBL, bool theToUpdate)
{
  myView->SetImageBasedLighting(theToEnableIBL);
  if (myImmediateUpdate || theToUpdate)
  {
    Redraw();
  }
}

//=================================================================================================

void V3d_View::SetAxis(const double theX,
                       const double theY,
                       const double theZ,
                       const double theVx,
                       const double theVy,
                       const double theVz)
{
  myDefaultViewPoint.SetCoord(theX, theY, theZ);
  myDefaultViewAxis.SetCoord(theVx, theVy, theVz);
}

//=================================================================================================

void V3d_View::SetShadingModel(const Graphic3d_TypeOfShadingModel theShadingModel)
{
  myView->SetShadingModel(theShadingModel);
}

//=================================================================================================

void V3d_View::SetTextureEnv(const occ::handle<Graphic3d_TextureEnv>& theTexture)
{
  myView->SetTextureEnv(theTexture);

  if (myImmediateUpdate)
  {
    Redraw();
  }
}

//=================================================================================================

void V3d_View::SetVisualization(const V3d_TypeOfVisualization theType)
{
  myView->SetVisualizationType(static_cast<Graphic3d_TypeOfVisualization>(theType));

  if (myImmediateUpdate)
  {
    Redraw();
  }
}

//=================================================================================================

void V3d_View::SetFront()
{
  gp_Ax3        a = MyViewer->PrivilegedPlane();
  double xo, yo, zo, vx, vy, vz, xu, yu, zu;

  a.Direction().Coord(vx, vy, vz);
  a.YDirection().Coord(xu, yu, zu);
  a.Location().Coord(xo, yo, zo);

  occ::handle<Graphic3d_Camera> aCamera = Camera();

  aCamera->SetCenter(gp_Pnt(xo, yo, zo));

  if (SwitchSetFront)
  {
    aCamera->SetDirection(gp_Dir(vx, vy, vz));
  }
  else
  {
    aCamera->SetDirection(gp_Dir(vx, vy, vz).Reversed());
  }

  aCamera->SetUp(gp_Dir(xu, yu, zu));

  SwitchSetFront = !SwitchSetFront;

  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::Rotate(const double    ax,
                      const double    ay,
                      const double    az,
                      const bool Start)
{
  double Ax = ax;
  double Ay = ay;
  double Az = az;

  if (Ax > 0.)
    while (Ax > THE_TWO_PI)
      Ax -= THE_TWO_PI;
  else if (Ax < 0.)
    while (Ax < -THE_TWO_PI)
      Ax += THE_TWO_PI;
  if (Ay > 0.)
    while (Ay > THE_TWO_PI)
      Ay -= THE_TWO_PI;
  else if (Ay < 0.)
    while (Ay < -THE_TWO_PI)
      Ay += THE_TWO_PI;
  if (Az > 0.)
    while (Az > THE_TWO_PI)
      Az -= THE_TWO_PI;
  else if (Az < 0.)
    while (Az < -THE_TWO_PI)
      Az += THE_TWO_PI;

  occ::handle<Graphic3d_Camera> aCamera = Camera();

  if (Start)
  {
    myCamStartOpUp     = aCamera->Up();
    myCamStartOpDir    = aCamera->Direction();
    myCamStartOpEye    = aCamera->Eye();
    myCamStartOpCenter = aCamera->Center();
  }

  aCamera->SetUp(myCamStartOpUp);
  aCamera->SetEyeAndCenter(myCamStartOpEye, myCamStartOpCenter);
  aCamera->SetDirectionFromEye(myCamStartOpDir);

  // rotate camera around 3 initial axes
  gp_Dir aBackDir = -myCamStartOpDir;
  gp_Dir aXAxis(myCamStartOpUp.Crossed(aBackDir));
  gp_Dir aYAxis(aBackDir.Crossed(aXAxis));
  gp_Dir aZAxis(aXAxis.Crossed(aYAxis));

  gp_Trsf aRot[3], aTrsf;
  aRot[0].SetRotation(gp_Ax1(myCamStartOpCenter, aYAxis), -Ax);
  aRot[1].SetRotation(gp_Ax1(myCamStartOpCenter, aXAxis), Ay);
  aRot[2].SetRotation(gp_Ax1(myCamStartOpCenter, aZAxis), Az);
  aTrsf.Multiply(aRot[0]);
  aTrsf.Multiply(aRot[1]);
  aTrsf.Multiply(aRot[2]);

  aCamera->Transform(aTrsf);

  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::Rotate(const double    ax,
                      const double    ay,
                      const double    az,
                      const double    X,
                      const double    Y,
                      const double    Z,
                      const bool Start)
{

  double Ax = ax;
  double Ay = ay;
  double Az = az;

  if (Ax > 0.)
    while (Ax > THE_TWO_PI)
      Ax -= THE_TWO_PI;
  else if (Ax < 0.)
    while (Ax < -THE_TWO_PI)
      Ax += THE_TWO_PI;
  if (Ay > 0.)
    while (Ay > THE_TWO_PI)
      Ay -= THE_TWO_PI;
  else if (Ay < 0.)
    while (Ay < -THE_TWO_PI)
      Ay += THE_TWO_PI;
  if (Az > 0.)
    while (Az > THE_TWO_PI)
      Az -= THE_TWO_PI;
  else if (Az < 0.)
    while (Az < -THE_TWO_PI)
      Az += THE_TWO_PI;

  occ::handle<Graphic3d_Camera> aCamera = Camera();

  if (Start)
  {
    myGravityReferencePoint.SetCoord(X, Y, Z);
    myCamStartOpUp     = aCamera->Up();
    myCamStartOpDir    = aCamera->Direction();
    myCamStartOpEye    = aCamera->Eye();
    myCamStartOpCenter = aCamera->Center();
  }

  const Graphic3d_Vertex& aVref = myGravityReferencePoint;

  aCamera->SetUp(myCamStartOpUp);
  aCamera->SetEyeAndCenter(myCamStartOpEye, myCamStartOpCenter);
  aCamera->SetDirectionFromEye(myCamStartOpDir);

  // rotate camera around 3 initial axes
  gp_Pnt aRCenter(aVref.X(), aVref.Y(), aVref.Z());

  gp_Dir aZAxis(aCamera->Direction().Reversed());
  gp_Dir aYAxis(aCamera->Up());
  gp_Dir aXAxis(aYAxis.Crossed(aZAxis));

  gp_Trsf aRot[3], aTrsf;
  aRot[0].SetRotation(gp_Ax1(aRCenter, aYAxis), -Ax);
  aRot[1].SetRotation(gp_Ax1(aRCenter, aXAxis), Ay);
  aRot[2].SetRotation(gp_Ax1(aRCenter, aZAxis), Az);
  aTrsf.Multiply(aRot[0]);
  aTrsf.Multiply(aRot[1]);
  aTrsf.Multiply(aRot[2]);

  aCamera->Transform(aTrsf);

  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::Rotate(const V3d_TypeOfAxe    Axe,
                      const double    angle,
                      const bool Start)
{
  switch (Axe)
  {
    case V3d_X:
      Rotate(angle, 0., 0., Start);
      break;
    case V3d_Y:
      Rotate(0., angle, 0., Start);
      break;
    case V3d_Z:
      Rotate(0., 0., angle, Start);
      break;
  }
}

//=================================================================================================

void V3d_View::Rotate(const V3d_TypeOfAxe    theAxe,
                      const double    theAngle,
                      const double    theX,
                      const double    theY,
                      const double    theZ,
                      const bool theStart)
{
  double anAngle = theAngle;

  if (anAngle > 0.0)
    while (anAngle > THE_TWO_PI)
      anAngle -= THE_TWO_PI;
  else if (anAngle < 0.0)
    while (anAngle < -THE_TWO_PI)
      anAngle += THE_TWO_PI;

  occ::handle<Graphic3d_Camera> aCamera = Camera();

  if (theStart)
  {
    myGravityReferencePoint.SetCoord(theX, theY, theZ);
    myCamStartOpUp     = aCamera->Up();
    myCamStartOpDir    = aCamera->Direction();
    myCamStartOpEye    = aCamera->Eye();
    myCamStartOpCenter = aCamera->Center();
    switch (theAxe)
    {
      case V3d_X:
        myViewAxis = gp::DX();
        break;
      case V3d_Y:
        myViewAxis = gp::DY();
        break;
      case V3d_Z:
        myViewAxis = gp::DZ();
        break;
    }
  }

  const Graphic3d_Vertex& aVref = myGravityReferencePoint;

  aCamera->SetUp(myCamStartOpUp);
  aCamera->SetEyeAndCenter(myCamStartOpEye, myCamStartOpCenter);
  aCamera->SetDirectionFromEye(myCamStartOpDir);

  // rotate camera around passed axis
  gp_Trsf aRotation;
  gp_Pnt  aRCenter(aVref.X(), aVref.Y(), aVref.Z());
  gp_Dir  aRAxis((theAxe == V3d_X) ? 1.0 : 0.0,
                (theAxe == V3d_Y) ? 1.0 : 0.0,
                (theAxe == V3d_Z) ? 1.0 : 0.0);

  aRotation.SetRotation(gp_Ax1(aRCenter, aRAxis), anAngle);

  aCamera->Transform(aRotation);

  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::Rotate(const double angle, const bool Start)
{
  double Angle = angle;

  if (Angle > 0.)
    while (Angle > THE_TWO_PI)
      Angle -= THE_TWO_PI;
  else if (Angle < 0.)
    while (Angle < -THE_TWO_PI)
      Angle += THE_TWO_PI;

  occ::handle<Graphic3d_Camera> aCamera = Camera();

  if (Start)
  {
    myCamStartOpUp     = aCamera->Up();
    myCamStartOpDir    = aCamera->Direction();
    myCamStartOpEye    = aCamera->Eye();
    myCamStartOpCenter = aCamera->Center();
  }

  aCamera->SetUp(myCamStartOpUp);
  aCamera->SetEyeAndCenter(myCamStartOpEye, myCamStartOpCenter);
  aCamera->SetDirectionFromEye(myCamStartOpDir);

  gp_Trsf aRotation;
  gp_Pnt  aRCenter(myDefaultViewPoint);
  gp_Dir  aRAxis(myDefaultViewAxis);
  aRotation.SetRotation(gp_Ax1(aRCenter, aRAxis), Angle);

  aCamera->Transform(aRotation);

  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::Turn(const double    ax,
                    const double    ay,
                    const double    az,
                    const bool Start)
{
  double Ax = ax;
  double Ay = ay;
  double Az = az;

  if (Ax > 0.)
    while (Ax > THE_TWO_PI)
      Ax -= THE_TWO_PI;
  else if (Ax < 0.)
    while (Ax < -THE_TWO_PI)
      Ax += THE_TWO_PI;
  if (Ay > 0.)
    while (Ay > THE_TWO_PI)
      Ay -= THE_TWO_PI;
  else if (Ay < 0.)
    while (Ay < -THE_TWO_PI)
      Ay += THE_TWO_PI;
  if (Az > 0.)
    while (Az > THE_TWO_PI)
      Az -= THE_TWO_PI;
  else if (Az < 0.)
    while (Az < -THE_TWO_PI)
      Az += THE_TWO_PI;

  occ::handle<Graphic3d_Camera> aCamera = Camera();

  if (Start)
  {
    myCamStartOpUp     = aCamera->Up();
    myCamStartOpDir    = aCamera->Direction();
    myCamStartOpEye    = aCamera->Eye();
    myCamStartOpCenter = aCamera->Center();
  }

  aCamera->SetUp(myCamStartOpUp);
  aCamera->SetEyeAndCenter(myCamStartOpEye, myCamStartOpCenter);
  aCamera->SetDirectionFromEye(myCamStartOpDir);

  // rotate camera around 3 initial axes
  gp_Pnt aRCenter = aCamera->Eye();
  gp_Dir aZAxis(aCamera->Direction().Reversed());
  gp_Dir aYAxis(aCamera->Up());
  gp_Dir aXAxis(aYAxis.Crossed(aZAxis));

  gp_Trsf aRot[3], aTrsf;
  aRot[0].SetRotation(gp_Ax1(aRCenter, aYAxis), -Ax);
  aRot[1].SetRotation(gp_Ax1(aRCenter, aXAxis), Ay);
  aRot[2].SetRotation(gp_Ax1(aRCenter, aZAxis), Az);
  aTrsf.Multiply(aRot[0]);
  aTrsf.Multiply(aRot[1]);
  aTrsf.Multiply(aRot[2]);

  aCamera->Transform(aTrsf);

  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::Turn(const V3d_TypeOfAxe    Axe,
                    const double    angle,
                    const bool Start)
{
  switch (Axe)
  {
    case V3d_X:
      Turn(angle, 0., 0., Start);
      break;
    case V3d_Y:
      Turn(0., angle, 0., Start);
      break;
    case V3d_Z:
      Turn(0., 0., angle, Start);
      break;
  }
}

//=================================================================================================

void V3d_View::Turn(const double angle, const bool Start)
{
  double Angle = angle;

  if (Angle > 0.)
    while (Angle > THE_TWO_PI)
      Angle -= THE_TWO_PI;
  else if (Angle < 0.)
    while (Angle < -THE_TWO_PI)
      Angle += THE_TWO_PI;

  occ::handle<Graphic3d_Camera> aCamera = Camera();

  if (Start)
  {
    myCamStartOpUp     = aCamera->Up();
    myCamStartOpDir    = aCamera->Direction();
    myCamStartOpEye    = aCamera->Eye();
    myCamStartOpCenter = aCamera->Center();
  }

  aCamera->SetUp(myCamStartOpUp);
  aCamera->SetEyeAndCenter(myCamStartOpEye, myCamStartOpCenter);
  aCamera->SetDirectionFromEye(myCamStartOpDir);

  gp_Trsf aRotation;
  gp_Pnt  aRCenter = aCamera->Eye();
  gp_Dir  aRAxis(myDefaultViewAxis);
  aRotation.SetRotation(gp_Ax1(aRCenter, aRAxis), Angle);

  aCamera->Transform(aRotation);

  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::SetTwist(const double angle)
{
  double Angle = angle;

  if (Angle > 0.)
    while (Angle > THE_TWO_PI)
      Angle -= THE_TWO_PI;
  else if (Angle < 0.)
    while (Angle < -THE_TWO_PI)
      Angle += THE_TWO_PI;

  occ::handle<Graphic3d_Camera> aCamera = Camera();

  const gp_Dir aReferencePlane(aCamera->Direction().Reversed());
  if (!screenAxis(aReferencePlane, gp::DZ(), myXscreenAxis, myYscreenAxis, myZscreenAxis)
      && !screenAxis(aReferencePlane, gp::DY(), myXscreenAxis, myYscreenAxis, myZscreenAxis)
      && !screenAxis(aReferencePlane, gp::DX(), myXscreenAxis, myYscreenAxis, myZscreenAxis))
  {
    throw V3d_BadValue("V3d_ViewSetTwist, alignment of Eye,At,Up,");
  }

  gp_Pnt aRCenter = aCamera->Center();
  gp_Dir aZAxis(aCamera->Direction().Reversed());

  gp_Trsf aTrsf;
  aTrsf.SetRotation(gp_Ax1(aRCenter, aZAxis), Angle);

  aCamera->SetUp(gp_Dir(myYscreenAxis));
  aCamera->Transform(aTrsf);

  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::SetEye(const double X, const double Y, const double Z)
{
  double aTwistBefore = Twist();

  bool wasUpdateEnabled = SetImmediateUpdate(false);

  occ::handle<Graphic3d_Camera> aCamera = Camera();

  aCamera->SetEye(gp_Pnt(X, Y, Z));

  SetTwist(aTwistBefore);

  SetImmediateUpdate(wasUpdateEnabled);

  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::SetDepth(const double Depth)
{
  V3d_BadValue_Raise_if(Depth == 0., "V3d_View::SetDepth, bad depth");

  occ::handle<Graphic3d_Camera> aCamera = Camera();

  if (Depth > 0.)
  {
    // Move eye using center (target) as anchor.
    aCamera->SetDistance(Depth);
  }
  else
  {
    // Move the view ref point instead of the eye.
    gp_Vec aDir(aCamera->Direction());
    gp_Pnt aCameraEye    = aCamera->Eye();
    gp_Pnt aCameraCenter = aCameraEye.Translated(aDir.Multiplied(std::abs(Depth)));

    aCamera->SetCenter(aCameraCenter);
  }

  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::SetProj(const double Vx, const double Vy, const double Vz)
{
  V3d_BadValue_Raise_if(std::sqrt(Vx * Vx + Vy * Vy + Vz * Vz) <= 0.,
                        "V3d_View::SetProj, null projection vector");

  double aTwistBefore = Twist();

  bool wasUpdateEnabled = SetImmediateUpdate(false);

  Camera()->SetDirection(gp_Dir(Vx, Vy, Vz).Reversed());

  SetTwist(aTwistBefore);

  SetImmediateUpdate(wasUpdateEnabled);

  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::SetProj(const V3d_TypeOfOrientation theOrientation, const bool theIsYup)
{
  NCollection_Vec3<double> anUp = theIsYup ? NCollection_Vec3<double>(0.0, 1.0, 0.0) : NCollection_Vec3<double>(0.0, 0.0, 1.0);
  if (theIsYup)
  {
    if (theOrientation == V3d_Ypos || theOrientation == V3d_Yneg)
    {
      anUp.SetValues(0.0, 0.0, -1.0);
    }
  }
  else
  {
    if (theOrientation == V3d_Zpos)
    {
      anUp.SetValues(0.0, 1.0, 0.0);
    }
    else if (theOrientation == V3d_Zneg)
    {
      anUp.SetValues(0.0, -1.0, 0.0);
    }
  }

  const gp_Dir aBck = V3d::GetProjAxis(theOrientation);

  // retain camera panning from origin when switching projection
  const occ::handle<Graphic3d_Camera>& aCamera     = Camera();
  const gp_Pnt                    anOriginVCS = aCamera->ConvertWorld2View(gp::Origin());

  const double aNewDist = aCamera->Eye().Distance(gp_Pnt(0, 0, 0));
  aCamera->SetEyeAndCenter(gp_XYZ(0, 0, 0) + aBck.XYZ() * aNewDist, gp_XYZ(0, 0, 0));
  aCamera->SetDirectionFromEye(-aBck);
  aCamera->SetUp(gp_Dir(anUp.x(), anUp.y(), anUp.z()));
  aCamera->OrthogonalizeUp();

  Panning(anOriginVCS.X(), anOriginVCS.Y());

  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::SetAt(const double X, const double Y, const double Z)
{
  double aTwistBefore = Twist();

  bool wasUpdateEnabled = SetImmediateUpdate(false);

  Camera()->SetCenter(gp_Pnt(X, Y, Z));

  SetTwist(aTwistBefore);

  SetImmediateUpdate(wasUpdateEnabled);

  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::SetUp(const double theVx,
                     const double theVy,
                     const double theVz)
{
  occ::handle<Graphic3d_Camera> aCamera = Camera();

  const gp_Dir aReferencePlane(aCamera->Direction().Reversed());
  const gp_Dir anUp(theVx, theVy, theVz);
  if (!screenAxis(aReferencePlane, anUp, myXscreenAxis, myYscreenAxis, myZscreenAxis)
      && !screenAxis(aReferencePlane, gp::DZ(), myXscreenAxis, myYscreenAxis, myZscreenAxis)
      && !screenAxis(aReferencePlane, gp::DY(), myXscreenAxis, myYscreenAxis, myZscreenAxis)
      && !screenAxis(aReferencePlane, gp::DX(), myXscreenAxis, myYscreenAxis, myZscreenAxis))
  {
    throw V3d_BadValue("V3d_View::Setup, alignment of Eye,At,Up");
  }

  aCamera->SetUp(gp_Dir(myYscreenAxis));

  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::SetUp(const V3d_TypeOfOrientation theOrientation)
{
  occ::handle<Graphic3d_Camera> aCamera = Camera();

  const gp_Dir aReferencePlane(aCamera->Direction().Reversed());
  const gp_Dir anUp = V3d::GetProjAxis(theOrientation);
  if (!screenAxis(aReferencePlane, anUp, myXscreenAxis, myYscreenAxis, myZscreenAxis)
      && !screenAxis(aReferencePlane, gp::DZ(), myXscreenAxis, myYscreenAxis, myZscreenAxis)
      && !screenAxis(aReferencePlane, gp::DY(), myXscreenAxis, myYscreenAxis, myZscreenAxis)
      && !screenAxis(aReferencePlane, gp::DX(), myXscreenAxis, myYscreenAxis, myZscreenAxis))
  {
    throw V3d_BadValue("V3d_View::SetUp, alignment of Eye,At,Up");
  }

  aCamera->SetUp(gp_Dir(myYscreenAxis));

  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::SetViewOrientationDefault()
{
  myDefaultCamera->CopyOrientationData(Camera());
}

//=================================================================================================

void V3d_View::SetViewMappingDefault()
{
  myDefaultCamera->CopyMappingData(Camera());
}

//=================================================================================================

void V3d_View::ResetViewOrientation()
{
  Camera()->CopyOrientationData(myDefaultCamera);
  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::ResetViewMapping()
{
  Camera()->CopyMappingData(myDefaultCamera);
  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::Reset(const bool theToUpdate)
{
  Camera()->Copy(myDefaultCamera);

  SwitchSetFront = false;

  if (myImmediateUpdate || theToUpdate)
  {
    Update();
  }
}

//=================================================================================================

void V3d_View::SetCenter(const int theXp, const int theYp)
{
  double aXv, aYv;
  Convert(theXp, theYp, aXv, aYv);
  Translate(Camera(), aXv, aYv);

  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::SetSize(const double theSize)
{
  V3d_BadValue_Raise_if(theSize <= 0.0, "V3d_View::SetSize, Window Size is NULL");

  occ::handle<Graphic3d_Camera> aCamera = Camera();

  aCamera->SetScale(aCamera->Aspect() >= 1.0 ? theSize / aCamera->Aspect() : theSize);

  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::SetZSize(const double theSize)
{
  occ::handle<Graphic3d_Camera> aCamera = Camera();

  double Zmax = theSize / 2.;

  double aDistance = aCamera->Distance();

  if (theSize <= 0.)
  {
    Zmax = aDistance;
  }

  // ShortReal precision factor used to add meaningful tolerance to
  // ZNear, ZFar values in order to avoid equality after type conversion
  // to ShortReal matrices type.
  const double aPrecision = 1.0 / std::pow(10.0, ShortRealDigits() - 1);

  double aZFar  = Zmax + aDistance * 2.0;
  double aZNear = -Zmax + aDistance;
  aZNear -= std::abs(aZNear) * aPrecision;
  aZFar += std::abs(aZFar) * aPrecision;

  if (!aCamera->IsOrthographic())
  {
    if (aZFar < aPrecision)
    {
      // Invalid case when both values are negative
      aZNear = aPrecision;
      aZFar  = aPrecision * 2.0;
    }
    else if (aZNear < std::abs(aZFar) * aPrecision)
    {
      // Z is less than 0.0, try to fix it using any appropriate z-scale
      aZNear = std::abs(aZFar) * aPrecision;
    }
  }

  // If range is too small
  if (aZFar < (aZNear + std::abs(aZFar) * aPrecision))
  {
    aZFar = aZNear + std::abs(aZFar) * aPrecision;
  }

  aCamera->SetZRange(aZNear, aZFar);

  if (myImmediateUpdate)
  {
    Redraw();
  }
}

//=================================================================================================

void V3d_View::SetZoom(const double theCoef, const bool theToStart)
{
  V3d_BadValue_Raise_if(theCoef <= 0., "V3d_View::SetZoom, bad coefficient");

  occ::handle<Graphic3d_Camera> aCamera = Camera();

  if (theToStart)
  {
    myCamStartOpEye    = aCamera->Eye();
    myCamStartOpCenter = aCamera->Center();
  }

  double aViewWidth  = aCamera->ViewDimensions().X();
  double aViewHeight = aCamera->ViewDimensions().Y();

  // ensure that zoom will not be too small or too big
  double aCoef = theCoef;
  if (aViewWidth < aCoef * Precision::Confusion())
  {
    aCoef = aViewWidth / Precision::Confusion();
  }
  else if (aViewWidth > aCoef * 1e12)
  {
    aCoef = aViewWidth / 1e12;
  }
  if (aViewHeight < aCoef * Precision::Confusion())
  {
    aCoef = aViewHeight / Precision::Confusion();
  }
  else if (aViewHeight > aCoef * 1e12)
  {
    aCoef = aViewHeight / 1e12;
  }

  aCamera->SetEye(myCamStartOpEye);
  aCamera->SetCenter(myCamStartOpCenter);
  aCamera->SetScale(aCamera->Scale() / aCoef);

  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::SetScale(const double Coef)
{
  V3d_BadValue_Raise_if(Coef <= 0., "V3d_View::SetScale, bad coefficient");

  occ::handle<Graphic3d_Camera> aCamera = Camera();

  double aDefaultScale = myDefaultCamera->Scale();
  aCamera->SetAspect(myDefaultCamera->Aspect());
  aCamera->SetScale(aDefaultScale / Coef);

  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::SetAxialScale(const double Sx, const double Sy, const double Sz)
{
  V3d_BadValue_Raise_if(Sx <= 0. || Sy <= 0. || Sz <= 0.,
                        "V3d_View::SetAxialScale, bad coefficient");

  Camera()->SetAxialScale(gp_XYZ(Sx, Sy, Sz));
}

//=================================================================================================

void V3d_View::SetRatio()
{
  if (MyWindow.IsNull())
  {
    return;
  }

  int aWidth  = 0;
  int aHeight = 0;
  MyWindow->Size(aWidth, aHeight);
  if (aWidth > 0 && aHeight > 0)
  {
    double aRatio = static_cast<double>(aWidth) / static_cast<double>(aHeight);

    Camera()->SetAspect(aRatio);
    myDefaultCamera->SetAspect(aRatio);
  }
}

//=================================================================================================

void V3d_View::FitAll(const double theMargin, const bool theToUpdate)
{
  FitAll(myView->MinMaxValues(), theMargin, theToUpdate);
}

//=================================================================================================

void V3d_View::FitAll(const Bnd_Box&         theBox,
                      const double    theMargin,
                      const bool theToUpdate)
{
  Standard_ASSERT_RAISE(theMargin >= 0.0 && theMargin < 1.0, "Invalid margin coefficient");

  if (myView->NumberOfDisplayedStructures() == 0)
  {
    return;
  }

  if (!FitMinMax(Camera(), theBox, theMargin, 10.0 * Precision::Confusion()))
  {
    return;
  }

  if (myImmediateUpdate || theToUpdate)
  {
    Update();
  }
}

//=================================================================================================

void V3d_View::DepthFitAll(const double Aspect, const double Margin)
{
  double Xmin, Ymin, Zmin, Xmax, Ymax, Zmax, U, V, W, U1, V1, W1;
  double Umin, Vmin, Wmin, Umax, Vmax, Wmax;
  double Dx, Dy, Dz, Size;

  int Nstruct = myView->NumberOfDisplayedStructures();

  if ((Nstruct <= 0) || (Aspect < 0.) || (Margin < 0.) || (Margin > 1.))
  {
    ImmediateUpdate();
    return;
  }

  Bnd_Box aBox = myView->MinMaxValues();
  if (aBox.IsVoid())
  {
    ImmediateUpdate();
    return;
  }
  aBox.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
  Project(Xmin, Ymin, Zmin, U, V, W);
  Project(Xmax, Ymax, Zmax, U1, V1, W1);
  Umin = std::min(U, U1);
  Umax = std::max(U, U1);
  Vmin = std::min(V, V1);
  Vmax = std::max(V, V1);
  Wmin = std::min(W, W1);
  Wmax = std::max(W, W1);
  Project(Xmin, Ymin, Zmax, U, V, W);
  Umin = std::min(U, Umin);
  Umax = std::max(U, Umax);
  Vmin = std::min(V, Vmin);
  Vmax = std::max(V, Vmax);
  Wmin = std::min(W, Wmin);
  Wmax = std::max(W, Wmax);
  Project(Xmax, Ymin, Zmax, U, V, W);
  Umin = std::min(U, Umin);
  Umax = std::max(U, Umax);
  Vmin = std::min(V, Vmin);
  Vmax = std::max(V, Vmax);
  Wmin = std::min(W, Wmin);
  Wmax = std::max(W, Wmax);
  Project(Xmax, Ymin, Zmin, U, V, W);
  Umin = std::min(U, Umin);
  Umax = std::max(U, Umax);
  Vmin = std::min(V, Vmin);
  Vmax = std::max(V, Vmax);
  Wmin = std::min(W, Wmin);
  Wmax = std::max(W, Wmax);
  Project(Xmax, Ymax, Zmin, U, V, W);
  Umin = std::min(U, Umin);
  Umax = std::max(U, Umax);
  Vmin = std::min(V, Vmin);
  Vmax = std::max(V, Vmax);
  Wmin = std::min(W, Wmin);
  Wmax = std::max(W, Wmax);
  Project(Xmin, Ymax, Zmax, U, V, W);
  Umin = std::min(U, Umin);
  Umax = std::max(U, Umax);
  Vmin = std::min(V, Vmin);
  Vmax = std::max(V, Vmax);
  Wmin = std::min(W, Wmin);
  Wmax = std::max(W, Wmax);
  Project(Xmin, Ymax, Zmin, U, V, W);
  Umin = std::min(U, Umin);
  Umax = std::max(U, Umax);
  Vmin = std::min(V, Vmin);
  Vmax = std::max(V, Vmax);
  Wmin = std::min(W, Wmin);
  Wmax = std::max(W, Wmax);

  // Adjust Z size
  Wmax = std::max(std::abs(Wmin), std::abs(Wmax));
  Dz   = 2. * Wmax + Margin * Wmax;

  // Compute depth value
  Dx = std::abs(Umax - Umin);
  Dy = std::abs(Vmax - Vmin); // Dz = std::abs(Wmax - Wmin);
  Dx += Margin * Dx;
  Dy += Margin * Dy;
  Size = std::sqrt(Dx * Dx + Dy * Dy + Dz * Dz);
  if (Size > 0.)
  {
    SetZSize(Size);
    SetDepth(Aspect * Size / 2.);
  }

  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::WindowFit(const int theMinXp,
                         const int theMinYp,
                         const int theMaxXp,
                         const int theMaxYp)
{
  bool wasUpdateEnabled = SetImmediateUpdate(false);

  occ::handle<Graphic3d_Camera> aCamera = Camera();

  if (!aCamera->IsOrthographic())
  {
    // normalize view coordinates
    int aWinWidth, aWinHeight;
    MyWindow->Size(aWinWidth, aWinHeight);

    // z coordinate of camera center
    double aDepth = aCamera->Project(aCamera->Center()).Z();

    // camera projection coordinate are in NDC which are normalized [-1, 1]
    double aUMin = (2.0 / aWinWidth) * theMinXp - 1.0;
    double aUMax = (2.0 / aWinWidth) * theMaxXp - 1.0;
    double aVMin = (2.0 / aWinHeight) * theMinYp - 1.0;
    double aVMax = (2.0 / aWinHeight) * theMaxYp - 1.0;

    // compute camera panning
    gp_Pnt aScreenCenter(0.0, 0.0, aDepth);
    gp_Pnt aFitCenter((aUMin + aUMax) * 0.5, (aVMin + aVMax) * 0.5, aDepth);
    gp_Pnt aPanTo   = aCamera->ConvertProj2View(aFitCenter);
    gp_Pnt aPanFrom = aCamera->ConvertProj2View(aScreenCenter);
    gp_Vec aPanVec(aPanFrom, aPanTo);

    // compute section size
    gp_Pnt aFitTopRight(aUMax, aVMax, aDepth);
    gp_Pnt aFitBotLeft(aUMin, aVMin, aDepth);
    gp_Pnt aViewBotLeft  = aCamera->ConvertProj2View(aFitBotLeft);
    gp_Pnt aViewTopRight = aCamera->ConvertProj2View(aFitTopRight);

    double aUSize = aViewTopRight.X() - aViewBotLeft.X();
    double aVSize = aViewTopRight.Y() - aViewBotLeft.Y();

    Translate(aCamera, aPanVec.X(), -aPanVec.Y());
    Scale(aCamera, aUSize, aVSize);
  }
  else
  {
    double aX1, aY1, aX2, aY2;
    Convert(theMinXp, theMinYp, aX1, aY1);
    Convert(theMaxXp, theMaxYp, aX2, aY2);
    FitAll(aX1, aY1, aX2, aY2);
  }

  SetImmediateUpdate(wasUpdateEnabled);

  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::ConvertToGrid(const int theXp,
                             const int theYp,
                             double&         theXg,
                             double&         theYg,
                             double&         theZg) const
{
  NCollection_Vec3<double> anXYZ;
  Convert(theXp, theYp, anXYZ.x(), anXYZ.y(), anXYZ.z());

  Graphic3d_Vertex aVrp;
  aVrp.SetCoord(anXYZ.x(), anXYZ.y(), anXYZ.z());
  if (MyViewer->IsGridActive())
  {
    Graphic3d_Vertex aNewVrp = Compute(aVrp);
    aNewVrp.Coord(theXg, theYg, theZg);
  }
  else
  {
    aVrp.Coord(theXg, theYg, theZg);
  }
}

//=================================================================================================

void V3d_View::ConvertToGrid(const double theX,
                             const double theY,
                             const double theZ,
                             double&      theXg,
                             double&      theYg,
                             double&      theZg) const
{
  if (MyViewer->IsGridActive())
  {
    Graphic3d_Vertex aVrp(theX, theY, theZ);
    Graphic3d_Vertex aNewVrp = Compute(aVrp);
    aNewVrp.Coord(theXg, theYg, theZg);
  }
  else
  {
    theXg = theX;
    theYg = theY;
    theZg = theZ;
  }
}

//=================================================================================================

double V3d_View::Convert(const int Vp) const
{
  int aDxw, aDyw;

  V3d_UnMapped_Raise_if(!myView->IsDefined(), "view has no window");

  MyWindow->Size(aDxw, aDyw);
  double aValue;

  gp_Pnt aViewDims = Camera()->ViewDimensions();
  aValue           = aViewDims.X() * (double)Vp / (double)aDxw;

  return aValue;
}

//=================================================================================================

void V3d_View::Convert(const int Xp,
                       const int Yp,
                       double&         Xv,
                       double&         Yv) const
{
  int aDxw, aDyw;

  V3d_UnMapped_Raise_if(!myView->IsDefined(), "view has no window");

  MyWindow->Size(aDxw, aDyw);

  gp_Pnt aPoint(Xp * 2.0 / aDxw - 1.0, (aDyw - Yp) * 2.0 / aDyw - 1.0, 0.0);
  aPoint = Camera()->ConvertProj2View(aPoint);

  Xv = aPoint.X();
  Yv = aPoint.Y();
}

//=================================================================================================

int V3d_View::Convert(const double Vv) const
{
  V3d_UnMapped_Raise_if(!myView->IsDefined(), "view has no window");

  int aDxw, aDyw;
  MyWindow->Size(aDxw, aDyw);

  gp_Pnt           aViewDims = Camera()->ViewDimensions();
  int aValue    = RealToInt(aDxw * Vv / (aViewDims.X()));

  return aValue;
}

//=================================================================================================

void V3d_View::Convert(const double Xv,
                       const double Yv,
                       int&   Xp,
                       int&   Yp) const
{
  V3d_UnMapped_Raise_if(!myView->IsDefined(), "view has no window");

  int aDxw, aDyw;
  MyWindow->Size(aDxw, aDyw);

  gp_Pnt aPoint(Xv, Yv, 0.0);
  aPoint = Camera()->ConvertView2Proj(aPoint);
  aPoint = gp_Pnt((aPoint.X() + 1.0) * aDxw / 2.0, aDyw - (aPoint.Y() + 1.0) * aDyw / 2.0, 0.0);

  Xp = RealToInt(aPoint.X());
  Yp = RealToInt(aPoint.Y());
}

//=================================================================================================

void V3d_View::Convert(const int theXp,
                       const int theYp,
                       double&         theX,
                       double&         theY,
                       double&         theZ) const
{
  V3d_UnMapped_Raise_if(!myView->IsDefined(), "view has no window");
  int aHeight = 0, aWidth = 0;
  MyWindow->Size(aWidth, aHeight);

  const gp_Pnt anXYZ(2.0 * theXp / aWidth - 1.0,
                     2.0 * (aHeight - 1 - theYp) / aHeight - 1.0,
                     Camera()->IsZeroToOneDepth() ? 0.0 : -1.0);
  const gp_Pnt aResult = Camera()->UnProject(anXYZ);
  theX                 = aResult.X();
  theY                 = aResult.Y();
  theZ                 = aResult.Z();
}

//=================================================================================================

void V3d_View::ConvertWithProj(const int theXp,
                               const int theYp,
                               double&         theX,
                               double&         theY,
                               double&         theZ,
                               double&         theDx,
                               double&         theDy,
                               double&         theDz) const
{
  V3d_UnMapped_Raise_if(!myView->IsDefined(), "view has no window");
  int aHeight = 0, aWidth = 0;
  MyWindow->Size(aWidth, aHeight);

  const double anX = 2.0 * theXp / aWidth - 1.0;
  const double anY = 2.0 * (aHeight - 1 - theYp) / aHeight - 1.0;
  const double aZ  = 2.0 * 0.0 - 1.0;

  const occ::handle<Graphic3d_Camera>& aCamera  = Camera();
  const gp_Pnt                    aResult1 = aCamera->UnProject(gp_Pnt(anX, anY, aZ));
  const gp_Pnt                    aResult2 = aCamera->UnProject(gp_Pnt(anX, anY, aZ - 10.0));

  theX = aResult1.X();
  theY = aResult1.Y();
  theZ = aResult1.Z();
  NCollection_Vec3<double> aNormDir(theX - aResult2.X(), theY - aResult2.Y(), theZ - aResult2.Z());
  aNormDir.Normalize();

  theDx = aNormDir.x();
  theDy = aNormDir.y();
  theDz = aNormDir.z();
}

//=================================================================================================

void V3d_View::Convert(const double X,
                       const double Y,
                       const double Z,
                       int&   Xp,
                       int&   Yp) const
{
  V3d_UnMapped_Raise_if(!myView->IsDefined(), "view has no window");
  int aHeight, aWidth;
  MyWindow->Size(aWidth, aHeight);

  gp_Pnt aPoint = Camera()->Project(gp_Pnt(X, Y, Z));

  Xp = RealToInt((aPoint.X() + 1) * 0.5 * aWidth);
  Yp = RealToInt(aHeight - 1 - (aPoint.Y() + 1) * 0.5 * aHeight);
}

//=================================================================================================

void V3d_View::Project(const double theX,
                       const double theY,
                       const double theZ,
                       double&      theXp,
                       double&      theYp) const
{
  double aZp;
  Project(theX, theY, theZ, theXp, theYp, aZp);
}

//=================================================================================================

void V3d_View::Project(const double theX,
                       const double theY,
                       const double theZ,
                       double&      theXp,
                       double&      theYp,
                       double&      theZp) const
{
  occ::handle<Graphic3d_Camera> aCamera = Camera();

  gp_XYZ        aViewSpaceDimensions = aCamera->ViewDimensions();
  double aXSize               = aViewSpaceDimensions.X();
  double aYSize               = aViewSpaceDimensions.Y();
  double aZSize               = aViewSpaceDimensions.Z();

  gp_Pnt aPoint = aCamera->Project(gp_Pnt(theX, theY, theZ));

  // NDC [-1, 1] --> PROJ [ -size / 2, +size / 2 ]
  theXp = aPoint.X() * aXSize * 0.5;
  theYp = aPoint.Y() * aYSize * 0.5;
  theZp = Camera()->IsZeroToOneDepth() ? aPoint.Z() * aZSize : aPoint.Z() * aZSize * 0.5;
}

//=================================================================================================

void V3d_View::BackgroundColor(const Quantity_TypeOfColor Type,
                               double&             V1,
                               double&             V2,
                               double&             V3) const
{
  Quantity_Color C = BackgroundColor();
  C.Values(V1, V2, V3, Type);
}

//=================================================================================================

Quantity_Color V3d_View::BackgroundColor() const
{
  return myView->Background().Color();
}

//=================================================================================================

void V3d_View::GradientBackgroundColors(Quantity_Color& theColor1, Quantity_Color& theColor2) const
{
  myView->GradientBackground().Colors(theColor1, theColor2);
}

//=================================================================================================

Aspect_GradientBackground V3d_View::GradientBackground() const
{
  return myView->GradientBackground();
}

//=================================================================================================

double V3d_View::Scale() const
{
  return myDefaultCamera->Scale() / Camera()->Scale();
}

//=================================================================================================

void V3d_View::AxialScale(double& Sx, double& Sy, double& Sz) const
{
  gp_Pnt anAxialScale = Camera()->AxialScale();
  Sx                  = anAxialScale.X();
  Sy                  = anAxialScale.Y();
  Sz                  = anAxialScale.Z();
}

//=================================================================================================

void V3d_View::Size(double& Width, double& Height) const
{
  gp_Pnt aViewDims = Camera()->ViewDimensions();

  Width  = aViewDims.X();
  Height = aViewDims.Y();
}

//=================================================================================================

double V3d_View::ZSize() const
{
  gp_Pnt aViewDims = Camera()->ViewDimensions();

  return aViewDims.Z();
}

//=================================================================================================

int V3d_View::MinMax(double& Umin,
                                  double& Vmin,
                                  double& Umax,
                                  double& Vmax) const
{
  double Wmin, Wmax, U, V, W;
  double Xmin, Ymin, Zmin, Xmax, Ymax, Zmax;
  // CAL 6/11/98
  int Nstruct = myView->NumberOfDisplayedStructures();

  if (Nstruct)
  {
    Bnd_Box aBox = myView->MinMaxValues();
    aBox.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
    Project(Xmin, Ymin, Zmin, Umin, Vmin, Wmin);
    Project(Xmax, Ymax, Zmax, Umax, Vmax, Wmax);
    Project(Xmin, Ymin, Zmax, U, V, W);
    Umin = std::min(U, Umin);
    Umax = std::max(U, Umax);
    Vmin = std::min(V, Vmin);
    Vmax = std::max(V, Vmax);
    Wmin = std::min(W, Wmin);
    Wmax = std::max(W, Wmax);
    Project(Xmax, Ymin, Zmax, U, V, W);
    Umin = std::min(U, Umin);
    Umax = std::max(U, Umax);
    Vmin = std::min(V, Vmin);
    Vmax = std::max(V, Vmax);
    Wmin = std::min(W, Wmin);
    Wmax = std::max(W, Wmax);
    Project(Xmax, Ymin, Zmin, U, V, W);
    Umin = std::min(U, Umin);
    Umax = std::max(U, Umax);
    Vmin = std::min(V, Vmin);
    Vmax = std::max(V, Vmax);
    Wmin = std::min(W, Wmin);
    Wmax = std::max(W, Wmax);
    Project(Xmax, Ymax, Zmin, U, V, W);
    Umin = std::min(U, Umin);
    Umax = std::max(U, Umax);
    Vmin = std::min(V, Vmin);
    Vmax = std::max(V, Vmax);
    Wmin = std::min(W, Wmin);
    Wmax = std::max(W, Wmax);
    Project(Xmin, Ymax, Zmax, U, V, W);
    Umin = std::min(U, Umin);
    Umax = std::max(U, Umax);
    Vmin = std::min(V, Vmin);
    Vmax = std::max(V, Vmax);
    Wmin = std::min(W, Wmin);
    Wmax = std::max(W, Wmax);
    Project(Xmin, Ymax, Zmin, U, V, W);
    Umin = std::min(U, Umin);
    Umax = std::max(U, Umax);
    Vmin = std::min(V, Vmin);
    Vmax = std::max(V, Vmax);
    Wmin = std::min(W, Wmin);
    Wmax = std::max(W, Wmax);
  }
  return Nstruct;
}

//=================================================================================================

int V3d_View::MinMax(double& Xmin,
                                  double& Ymin,
                                  double& Zmin,
                                  double& Xmax,
                                  double& Ymax,
                                  double& Zmax) const
{
  // CAL 6/11/98
  // int Nstruct = (MyView->DisplayedStructures())->Extent() ;
  int Nstruct = myView->NumberOfDisplayedStructures();

  if (Nstruct)
  {
    Bnd_Box aBox = myView->MinMaxValues();
    aBox.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
  }
  return Nstruct;
}

//=================================================================================================

gp_Pnt V3d_View::GravityPoint() const
{
  NCollection_Map<occ::handle<Graphic3d_Structure>> aSetOfStructures;
  myView->DisplayedStructures(aSetOfStructures);

  bool hasSelection = false;
  for (NCollection_Map<occ::handle<Graphic3d_Structure>>::Iterator aStructIter(aSetOfStructures); aStructIter.More();
       aStructIter.Next())
  {
    if (aStructIter.Key()->IsHighlighted() && aStructIter.Key()->IsVisible())
    {
      hasSelection = true;
      break;
    }
  }

  double    Xmin, Ymin, Zmin, Xmax, Ymax, Zmax;
  int aNbPoints = 0;
  gp_XYZ           aResult(0.0, 0.0, 0.0);
  for (NCollection_Map<occ::handle<Graphic3d_Structure>>::Iterator aStructIter(aSetOfStructures); aStructIter.More();
       aStructIter.Next())
  {
    const occ::handle<Graphic3d_Structure>& aStruct = aStructIter.Key();
    if (!aStruct->IsVisible() || aStruct->IsInfinite()
        || (hasSelection && !aStruct->IsHighlighted()))
    {
      continue;
    }

    const Graphic3d_BndBox3d& aBox = aStruct->CStructure()->BoundingBox();
    if (!aBox.IsValid())
    {
      continue;
    }

    // skip transformation-persistent objects
    if (!aStruct->TransformPersistence().IsNull())
    {
      continue;
    }

    // use camera projection to find gravity point
    Xmin                              = aBox.CornerMin().x();
    Ymin                              = aBox.CornerMin().y();
    Zmin                              = aBox.CornerMin().z();
    Xmax                              = aBox.CornerMax().x();
    Ymax                              = aBox.CornerMax().y();
    Zmax                              = aBox.CornerMax().z();
    gp_Pnt aPnts[THE_NB_BOUND_POINTS] = {gp_Pnt(Xmin, Ymin, Zmin),
                                         gp_Pnt(Xmin, Ymin, Zmax),
                                         gp_Pnt(Xmin, Ymax, Zmin),
                                         gp_Pnt(Xmin, Ymax, Zmax),
                                         gp_Pnt(Xmax, Ymin, Zmin),
                                         gp_Pnt(Xmax, Ymin, Zmax),
                                         gp_Pnt(Xmax, Ymax, Zmin),
                                         gp_Pnt(Xmax, Ymax, Zmax)};

    for (int aPntIt = 0; aPntIt < THE_NB_BOUND_POINTS; ++aPntIt)
    {
      const gp_Pnt& aBndPnt    = aPnts[aPntIt];
      const gp_Pnt  aProjected = Camera()->Project(aBndPnt);
      if (std::abs(aProjected.X()) <= 1.0 && std::abs(aProjected.Y()) <= 1.0)
      {
        aResult += aBndPnt.XYZ();
        ++aNbPoints;
      }
    }
  }

  if (aNbPoints == 0)
  {
    // fallback - just use bounding box of entire scene
    Bnd_Box aBox = myView->MinMaxValues();
    if (!aBox.IsVoid())
    {
      aBox.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
      gp_Pnt aPnts[THE_NB_BOUND_POINTS] = {gp_Pnt(Xmin, Ymin, Zmin),
                                           gp_Pnt(Xmin, Ymin, Zmax),
                                           gp_Pnt(Xmin, Ymax, Zmin),
                                           gp_Pnt(Xmin, Ymax, Zmax),
                                           gp_Pnt(Xmax, Ymin, Zmin),
                                           gp_Pnt(Xmax, Ymin, Zmax),
                                           gp_Pnt(Xmax, Ymax, Zmin),
                                           gp_Pnt(Xmax, Ymax, Zmax)};

      for (int aPntIt = 0; aPntIt < THE_NB_BOUND_POINTS; ++aPntIt)
      {
        const gp_Pnt& aBndPnt = aPnts[aPntIt];
        aResult += aBndPnt.XYZ();
        ++aNbPoints;
      }
    }
  }

  if (aNbPoints > 0)
  {
    aResult /= aNbPoints;
  }

  return aResult;
}

//=================================================================================================

void V3d_View::Eye(double& X, double& Y, double& Z) const
{
  gp_Pnt aCameraEye = Camera()->Eye();
  X                 = aCameraEye.X();
  Y                 = aCameraEye.Y();
  Z                 = aCameraEye.Z();
}

//=================================================================================================

void V3d_View::ProjReferenceAxe(const int Xpix,
                                const int Ypix,
                                double&         XP,
                                double&         YP,
                                double&         ZP,
                                double&         VX,
                                double&         VY,
                                double&         VZ) const
{
  double Xo, Yo, Zo;

  Convert(Xpix, Ypix, XP, YP, ZP);
  if (Type() == V3d_PERSPECTIVE)
  {
    FocalReferencePoint(Xo, Yo, Zo);
    VX = Xo - XP;
    VY = Yo - YP;
    VZ = Zo - ZP;
  }
  else
  {
    Proj(VX, VY, VZ);
  }
}

//=================================================================================================

double V3d_View::Depth() const
{
  return Camera()->Distance();
}

//=================================================================================================

void V3d_View::Proj(double& Dx, double& Dy, double& Dz) const
{
  gp_Dir aCameraDir = Camera()->Direction().Reversed();
  Dx                = aCameraDir.X();
  Dy                = aCameraDir.Y();
  Dz                = aCameraDir.Z();
}

//=================================================================================================

void V3d_View::At(double& X, double& Y, double& Z) const
{
  gp_Pnt aCameraCenter = Camera()->Center();
  X                    = aCameraCenter.X();
  Y                    = aCameraCenter.Y();
  Z                    = aCameraCenter.Z();
}

//=================================================================================================

void V3d_View::Up(double& Vx, double& Vy, double& Vz) const
{
  gp_Dir aCameraUp = Camera()->Up();
  Vx               = aCameraUp.X();
  Vy               = aCameraUp.Y();
  Vz               = aCameraUp.Z();
}

//=================================================================================================

double V3d_View::Twist() const
{
  gp_Vec       Xaxis, Yaxis, Zaxis;
  const gp_Dir aReferencePlane(Camera()->Direction().Reversed());
  if (!screenAxis(aReferencePlane, gp::DZ(), Xaxis, Yaxis, Zaxis)
      && !screenAxis(aReferencePlane, gp::DY(), Xaxis, Yaxis, Zaxis)
      && !screenAxis(aReferencePlane, gp::DX(), Xaxis, Yaxis, Zaxis))
  {
    //
  }

  // Compute Cross Vector From Up & Origin
  const gp_Dir aCameraUp = Camera()->Up();
  const gp_XYZ aP        = Yaxis.XYZ().Crossed(aCameraUp.XYZ());

  // compute Angle
  double anAngle = std::asin(std::max(std::min(aP.Modulus(), 1.0), -1.0));
  if (Yaxis.Dot(aCameraUp.XYZ()) < 0.0)
  {
    anAngle = M_PI - anAngle;
  }
  if (anAngle > 0.0 && anAngle < M_PI)
  {
    const gp_Dir aProjDir = Camera()->Direction().Reversed();
    if (aP.Dot(aProjDir.XYZ()) < 0.0)
    {
      anAngle = THE_TWO_PI - anAngle;
    }
  }
  return anAngle;
}

//=================================================================================================

Graphic3d_TypeOfShadingModel V3d_View::ShadingModel() const
{
  return myView->ShadingModel();
}

//=================================================================================================

occ::handle<Graphic3d_TextureEnv> V3d_View::TextureEnv() const
{
  return myView->TextureEnv();
}

//=================================================================================================

V3d_TypeOfVisualization V3d_View::Visualization() const
{
  return static_cast<V3d_TypeOfVisualization>(myView->VisualizationType());
}

//=================================================================================================

bool V3d_View::IfWindow() const
{
  return myView->IsDefined();
}

//=================================================================================================

V3d_TypeOfView V3d_View::Type() const
{
  return Camera()->IsOrthographic() ? V3d_ORTHOGRAPHIC : V3d_PERSPECTIVE;
}

//=================================================================================================

void V3d_View::SetFocale(const double focale)
{
  occ::handle<Graphic3d_Camera> aCamera = Camera();

  if (aCamera->IsOrthographic())
  {
    return;
  }

  double aFOVyRad = std::atan(focale / (aCamera->Distance() * 2.0));

  aCamera->SetFOVy(aFOVyRad * (360 / M_PI));

  ImmediateUpdate();
}

//=================================================================================================

double V3d_View::Focale() const
{
  occ::handle<Graphic3d_Camera> aCamera = Camera();

  if (aCamera->IsOrthographic())
  {
    return 0.0;
  }

  return aCamera->Distance() * 2.0 * std::tan(aCamera->FOVy() * M_PI / 360.0);
}

//=================================================================================================

bool V3d_View::screenAxis(const gp_Dir& theVpn,
                                      const gp_Dir& theVup,
                                      gp_Vec&       theXaxe,
                                      gp_Vec&       theYaxe,
                                      gp_Vec&       theZaxe)
{
  theXaxe = theVup.XYZ().Crossed(theVpn.XYZ());
  if (theXaxe.Magnitude() <= gp::Resolution())
  {
    return false;
  }
  theXaxe.Normalize();

  theYaxe = theVpn.XYZ().Crossed(theXaxe.XYZ());
  if (theYaxe.Magnitude() <= gp::Resolution())
  {
    return false;
  }
  theYaxe.Normalize();

  theZaxe = theVpn.XYZ();
  theZaxe.Normalize();
  return true;
}

//=================================================================================================

gp_XYZ V3d_View::TrsPoint(const Graphic3d_Vertex& thePnt, const NCollection_Array2<double>& theMat)
{
  // CAL. S3892
  const int lr = theMat.LowerRow();
  const int ur = theMat.UpperRow();
  const int lc = theMat.LowerCol();
  const int uc = theMat.UpperCol();
  if ((ur - lr + 1 != 4) || (uc - lc + 1 != 4))
  {
    return gp_XYZ(thePnt.X(), thePnt.Y(), thePnt.Z());
  }

  double X, Y, Z;
  thePnt.Coord(X, Y, Z);
  const double XX =
    (theMat(lr, lc + 3) + X * theMat(lr, lc) + Y * theMat(lr, lc + 1) + Z * theMat(lr, lc + 2))
    / theMat(lr + 3, lc + 3);
  const double YY = (theMat(lr + 1, lc + 3) + X * theMat(lr + 1, lc)
                            + Y * theMat(lr + 1, lc + 1) + Z * theMat(lr + 1, lc + 2))
                           / theMat(lr + 3, lc + 3);
  const double ZZ = (theMat(lr + 2, lc + 3) + X * theMat(lr + 2, lc)
                            + Y * theMat(lr + 2, lc + 1) + Z * theMat(lr + 2, lc + 2))
                           / theMat(lr + 3, lc + 3);
  return gp_XYZ(XX, YY, ZZ);
}

//=================================================================================================

void V3d_View::Pan(const int theDXp,
                   const int theDYp,
                   const double    theZoomFactor,
                   const bool theToStart)
{
  Panning(Convert(theDXp), Convert(theDYp), theZoomFactor, theToStart);
}

//=================================================================================================

void V3d_View::Panning(const double    theDXv,
                       const double    theDYv,
                       const double    theZoomFactor,
                       const bool theToStart)
{
  Standard_ASSERT_RAISE(theZoomFactor > 0.0, "Bad zoom factor");

  occ::handle<Graphic3d_Camera> aCamera = Camera();

  if (theToStart)
  {
    myCamStartOpDir    = aCamera->Direction();
    myCamStartOpEye    = aCamera->Eye();
    myCamStartOpCenter = aCamera->Center();
  }

  bool wasUpdateEnabled = SetImmediateUpdate(false);

  gp_Pnt aViewDims = aCamera->ViewDimensions();

  aCamera->SetEyeAndCenter(myCamStartOpEye, myCamStartOpCenter);
  aCamera->SetDirectionFromEye(myCamStartOpDir);
  Translate(aCamera, -theDXv, -theDYv);
  Scale(aCamera, aViewDims.X() / theZoomFactor, aViewDims.Y() / theZoomFactor);

  SetImmediateUpdate(wasUpdateEnabled);

  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::Zoom(const int theXp1,
                    const int theYp1,
                    const int theXp2,
                    const int theYp2)
{
  int aDx = theXp2 - theXp1;
  int aDy = theYp2 - theYp1;
  if (aDx != 0 || aDy != 0)
  {
    double aCoeff = std::sqrt((double)(aDx * aDx + aDy * aDy)) / 100.0 + 1.0;
    aCoeff               = (aDx > 0) ? aCoeff : 1.0 / aCoeff;
    SetZoom(aCoeff, true);
  }
}

//=================================================================================================

void V3d_View::StartZoomAtPoint(const int theXp, const int theYp)
{
  MyZoomAtPointX = theXp;
  MyZoomAtPointY = theYp;
}

//=================================================================================================

void V3d_View::ZoomAtPoint(const int theMouseStartX,
                           const int theMouseStartY,
                           const int theMouseEndX,
                           const int theMouseEndY)
{
  bool wasUpdateEnabled = SetImmediateUpdate(false);

  // zoom
  double aDxy =
    double((theMouseEndX + theMouseEndY) - (theMouseStartX + theMouseStartY));
  double aDZoom = std::abs(aDxy) / 100.0 + 1.0;
  aDZoom               = (aDxy > 0.0) ? aDZoom : 1.0 / aDZoom;

  V3d_BadValue_Raise_if(aDZoom <= 0.0, "V3d_View::ZoomAtPoint, bad coefficient");

  occ::handle<Graphic3d_Camera> aCamera = Camera();

  double aViewWidth  = aCamera->ViewDimensions().X();
  double aViewHeight = aCamera->ViewDimensions().Y();

  // ensure that zoom will not be too small or too big.
  double aCoef = aDZoom;
  if (aViewWidth < aCoef * Precision::Confusion())
  {
    aCoef = aViewWidth / Precision::Confusion();
  }
  else if (aViewWidth > aCoef * 1e12)
  {
    aCoef = aViewWidth / 1e12;
  }
  if (aViewHeight < aCoef * Precision::Confusion())
  {
    aCoef = aViewHeight / Precision::Confusion();
  }
  else if (aViewHeight > aCoef * 1e12)
  {
    aCoef = aViewHeight / 1e12;
  }

  double aZoomAtPointXv = 0.0;
  double aZoomAtPointYv = 0.0;
  Convert(MyZoomAtPointX, MyZoomAtPointY, aZoomAtPointXv, aZoomAtPointYv);

  double aDxv = aZoomAtPointXv / aCoef;
  double aDyv = aZoomAtPointYv / aCoef;

  aCamera->SetScale(aCamera->Scale() / aCoef);
  Translate(aCamera, aZoomAtPointXv - aDxv, aZoomAtPointYv - aDyv);

  SetImmediateUpdate(wasUpdateEnabled);

  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::AxialScale(const int Dx,
                          const int Dy,
                          const V3d_TypeOfAxe    Axis)
{
  if (Dx != 0. || Dy != 0.)
  {
    double Sx, Sy, Sz;
    AxialScale(Sx, Sy, Sz);
    double dscale = std::sqrt(Dx * Dx + Dy * Dy) / 100. + 1;
    dscale               = (Dx > 0) ? dscale : 1. / dscale;
    if (Axis == V3d_X)
      Sx = dscale;
    if (Axis == V3d_Y)
      Sy = dscale;
    if (Axis == V3d_Z)
      Sz = dscale;
    SetAxialScale(Sx, Sy, Sz);
  }
}

//=================================================================================================

void V3d_View::FitAll(const double theXmin,
                      const double theYmin,
                      const double theXmax,
                      const double theYmax)
{
  occ::handle<Graphic3d_Camera> aCamera  = Camera();
  double            anAspect = aCamera->Aspect();

  double aFitSizeU  = std::abs(theXmax - theXmin);
  double aFitSizeV  = std::abs(theYmax - theYmin);
  double aFitAspect = aFitSizeU / aFitSizeV;
  if (aFitAspect >= anAspect)
  {
    aFitSizeV = aFitSizeU / anAspect;
  }
  else
  {
    aFitSizeU = aFitSizeV * anAspect;
  }

  Translate(aCamera, (theXmin + theXmax) * 0.5, (theYmin + theYmax) * 0.5);
  Scale(aCamera, aFitSizeU, aFitSizeV);

  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::StartRotation(const int X,
                             const int Y,
                             const double    zRotationThreshold)
{
  sx = X;
  sy = Y;
  double x, y;
  Size(x, y);
  rx              = double(Convert(x));
  ry              = double(Convert(y));
  myRotateGravity = GravityPoint();
  Rotate(0.0,
         0.0,
         0.0,
         myRotateGravity.X(),
         myRotateGravity.Y(),
         myRotateGravity.Z(),
         true);
  myZRotation = false;
  if (zRotationThreshold > 0.)
  {
    double dx = std::abs(sx - rx / 2.);
    double dy = std::abs(sy - ry / 2.);
    //  if( dx > rx/3. || dy > ry/3. ) myZRotation = true;
    double dd = zRotationThreshold * (rx + ry) / 2.;
    if (dx > dd || dy > dd)
      myZRotation = true;
  }
}

//=================================================================================================

void V3d_View::Rotation(const int X, const int Y)
{
  if (rx == 0. || ry == 0.)
  {
    StartRotation(X, Y);
    return;
  }
  double dx = 0., dy = 0., dz = 0.;
  if (myZRotation)
  {
    dz = atan2(double(X) - rx / 2., ry / 2. - double(Y))
         - atan2(sx - rx / 2., ry / 2. - sy);
  }
  else
  {
    dx = (double(X) - sx) * M_PI / rx;
    dy = (sy - double(Y)) * M_PI / ry;
  }

  Rotate(dx, dy, dz, myRotateGravity.X(), myRotateGravity.Y(), myRotateGravity.Z(), false);
}

//=================================================================================================

void V3d_View::SetComputedMode(const bool theMode)
{
  if (theMode)
  {
    if (myComputedMode)
    {
      myView->SetComputedMode(true);
    }
  }
  else
  {
    myView->SetComputedMode(false);
  }
}

//=================================================================================================

bool V3d_View::ComputedMode() const
{
  return myView->ComputedMode();
}

//=================================================================================================

void V3d_View::SetBackFacingModel(const Graphic3d_TypeOfBackfacingModel theModel)
{
  myView->SetBackfacingModel(theModel);
  Redraw();
}

//=================================================================================================

Graphic3d_TypeOfBackfacingModel V3d_View::BackFacingModel() const
{
  return myView->BackfacingModel();
}

//=================================================================================================

void V3d_View::Init()
{
  myComputedMode = MyViewer->ComputedMode();
  if (!myComputedMode || !MyViewer->DefaultComputedMode())
  {
    SetComputedMode(false);
  }
}

//=================================================================================================

bool V3d_View::Dump(const char* const      theFile,
                                const Graphic3d_BufferType& theBufferType)
{
  int aWinWidth, aWinHeight;
  MyWindow->Size(aWinWidth, aWinHeight);
  Image_AlienPixMap anImage;

  return ToPixMap(anImage, aWinWidth, aWinHeight, theBufferType) && anImage.Save(theFile);
}

//=================================================================================================

bool V3d_View::ToPixMap(Image_PixMap& theImage, const V3d_ImageDumpOptions& theParams)
{
  NCollection_Vec2<int> aTargetSize(theParams.Width, theParams.Height);
  if (aTargetSize.x() != 0 && aTargetSize.y() != 0)
  {
    // allocate image buffer for dumping
    if (theImage.IsEmpty() || theImage.SizeX() != size_t(aTargetSize.x())
        || theImage.SizeY() != size_t(aTargetSize.y()))
    {
      Image_Format aFormat = Image_Format_UNKNOWN;
      switch (theParams.BufferType)
      {
        case Graphic3d_BT_RGB:
          aFormat = Image_Format_RGB;
          break;
        case Graphic3d_BT_RGBA:
          aFormat = Image_Format_RGBA;
          break;
        case Graphic3d_BT_Depth:
          aFormat = Image_Format_GrayF;
          break;
        case Graphic3d_BT_RGB_RayTraceHdrLeft:
          aFormat = Image_Format_RGBF;
          break;
        case Graphic3d_BT_Red:
          aFormat = Image_Format_Gray;
          break;
        case Graphic3d_BT_ShadowMap:
          aFormat = Image_Format_GrayF;
          break;
      }

      if (!theImage.InitZero(aFormat,
                             size_t(aTargetSize.x()),
                             size_t(aTargetSize.y())))
      {
        Message::SendFail(TCollection_AsciiString("Fail to allocate an image ") + aTargetSize.x()
                          + "x" + aTargetSize.y() + " for view dump");
        return false;
      }
    }
  }
  if (theImage.IsEmpty())
  {
    Message::SendFail("V3d_View::ToPixMap() has been called without image dimensions");
    return false;
  }
  aTargetSize.x() = (int)theImage.SizeX();
  aTargetSize.y() = (int)theImage.SizeY();

  occ::handle<Standard_Transient> aFBOPtr;
  occ::handle<Standard_Transient> aPrevFBOPtr = myView->FBO();
  NCollection_Vec2<int>            aFBOVPSize  = aTargetSize;

  bool isTiling = false;
  if (theParams.TileSize > 0)
  {
    if (aFBOVPSize.x() > theParams.TileSize || aFBOVPSize.y() > theParams.TileSize)
    {
      aFBOVPSize.x() = std::min(aFBOVPSize.x(), theParams.TileSize);
      aFBOVPSize.y() = std::min(aFBOVPSize.y(), theParams.TileSize);
      isTiling       = true;
    }
  }

  NCollection_Vec2<int> aPrevFBOVPSize;
  if (!aPrevFBOPtr.IsNull())
  {
    NCollection_Vec2<int> aPrevFBOSizeMax;
    myView->FBOGetDimensions(aPrevFBOPtr,
                             aPrevFBOVPSize.x(),
                             aPrevFBOVPSize.y(),
                             aPrevFBOSizeMax.x(),
                             aPrevFBOSizeMax.y());
    if (aFBOVPSize.x() <= aPrevFBOSizeMax.x() && aFBOVPSize.y() <= aPrevFBOSizeMax.y())
    {
      aFBOPtr = aPrevFBOPtr;
    }
  }

  if (aFBOPtr.IsNull())
  {
    int aMaxTexSizeX =
      MyViewer->Driver()->InquireLimit(Graphic3d_TypeOfLimit_MaxViewDumpSizeX);
    int aMaxTexSizeY =
      MyViewer->Driver()->InquireLimit(Graphic3d_TypeOfLimit_MaxViewDumpSizeY);
    if (theParams.TileSize > aMaxTexSizeX || theParams.TileSize > aMaxTexSizeY)
    {
      Message::SendFail(
        TCollection_AsciiString("Image dump can not be performed - specified tile size (")
        + theParams.TileSize + ") exceeds hardware limits (" + aMaxTexSizeX + "x" + aMaxTexSizeY
        + ")");
      return false;
    }

    if (aFBOVPSize.x() > aMaxTexSizeX || aFBOVPSize.y() > aMaxTexSizeY)
    {
      if (MyViewer->Driver()->InquireLimit(Graphic3d_TypeOfLimit_IsWorkaroundFBO))
      {
        Message::SendWarning("Warning, workaround for Intel driver problem with empty FBO for "
                             "images with big width is applied");
      }
      Message::SendInfo(TCollection_AsciiString("Info, tiling image dump is used, image size (")
                        + aFBOVPSize.x() + "x" + aFBOVPSize.y() + ") exceeds hardware limits ("
                        + aMaxTexSizeX + "x" + aMaxTexSizeY + ")");
      aFBOVPSize.x() = std::min(aFBOVPSize.x(), aMaxTexSizeX);
      aFBOVPSize.y() = std::min(aFBOVPSize.y(), aMaxTexSizeY);
      isTiling       = true;
    }

    // Try to create hardware accelerated buffer
    aFBOPtr = myView->FBOCreate(aFBOVPSize.x(), aFBOVPSize.y());
  }
  myView->SetFBO(aFBOPtr);

  if (aFBOPtr.IsNull())
  {
    // try to use on-screen buffer
    NCollection_Vec2<int> aWinSize;
    MyWindow->Size(aWinSize.x(), aWinSize.y());
    if (aFBOVPSize.x() != aWinSize.x() || aFBOVPSize.y() != aWinSize.y())
    {
      isTiling = true;
    }
    aFBOVPSize = aWinSize;

    Message::SendWarning(
      "Warning, on screen buffer is used for image dump - content might be invalid");
  }

  // backup camera parameters
  occ::handle<Graphic3d_Camera> aStoreMapping = new Graphic3d_Camera();
  occ::handle<Graphic3d_Camera> aCamera       = Camera();
  aStoreMapping->Copy(aCamera);
  if (aCamera->IsStereo())
  {
    switch (theParams.StereoOptions)
    {
      case V3d_SDO_MONO: {
        aCamera->SetProjectionType(Graphic3d_Camera::Projection_Perspective);
        break;
      }
      case V3d_SDO_LEFT_EYE: {
        aCamera->SetProjectionType(Graphic3d_Camera::Projection_MonoLeftEye);
        break;
      }
      case V3d_SDO_RIGHT_EYE: {
        aCamera->SetProjectionType(Graphic3d_Camera::Projection_MonoRightEye);
        break;
      }
      case V3d_SDO_BLENDED: {
        break; // dump as is
      }
    }
  }
  if (theParams.ToAdjustAspect)
  {
    aCamera->SetAspect(double(aTargetSize.x()) / double(aTargetSize.y()));
  }
  // apply zlayer rendering parameters to view
  myView->SetZLayerTarget(theParams.TargetZLayerId);
  myView->SetZLayerRedrawMode(theParams.IsSingleLayer);
  // render immediate structures into back buffer rather than front
  const bool aPrevImmediateMode = myView->SetImmediateModeDrawToFront(false);

  bool isSuccess = true;
  if (!isTiling)
  {
    if (!aFBOPtr.IsNull())
    {
      myView->FBOChangeViewport(aFBOPtr, aTargetSize.x(), aTargetSize.y());
    }
    Redraw();
    if (theParams.BufferType == Graphic3d_BT_ShadowMap)
    {
      // draw shadow maps
      if (!myView->ShadowMapDump(theImage, theParams.LightName))
      {
        Message::SendFail("OpenGl_View::BufferDump() failed to dump shadowmap");
        isSuccess = false;
      }
    }
    else
    {
      isSuccess = isSuccess && myView->BufferDump(theImage, theParams.BufferType);
    }
  }
  else
  {
    Image_PixMap aTilePixMap;
    aTilePixMap.SetTopDown(theImage.IsTopDown());

    NCollection_Vec2<int> anOffset(0, 0);
    for (; anOffset.y() < aTargetSize.y(); anOffset.y() += aFBOVPSize.y())
    {
      anOffset.x() = 0;
      for (; anOffset.x() < aTargetSize.x(); anOffset.x() += aFBOVPSize.x())
      {
        Graphic3d_CameraTile aTileUncropped;
        aTileUncropped.Offset            = anOffset;
        aTileUncropped.TotalSize         = aTargetSize;
        aTileUncropped.TileSize          = aFBOVPSize;
        const Graphic3d_CameraTile aTile = aTileUncropped.Cropped();
        if (aTile.TileSize.x() < 1 || aTile.TileSize.y() < 1)
        {
          continue;
        }

        const int aLeft   = aTile.Offset.x();
        int       aBottom = aTile.Offset.y();
        if (theImage.IsTopDown())
        {
          const int aTop = aTile.Offset.y() + aTile.TileSize.y();
          aBottom                     = aTargetSize.y() - aTop;
        }
        aTilePixMap.InitWrapper(theImage.Format(),
                                theImage.ChangeData() + theImage.SizeRowBytes() * aBottom
                                  + theImage.SizePixelBytes() * aLeft,
                                aTile.TileSize.x(),
                                aTile.TileSize.y(),
                                theImage.SizeRowBytes());

        if (!aFBOPtr.IsNull())
        {
          aCamera->SetTile(aTile);
          myView->FBOChangeViewport(aFBOPtr, aTile.TileSize.x(), aTile.TileSize.y());
        }
        else
        {
          // no API to resize viewport of on-screen buffer - render uncropped
          aCamera->SetTile(aTileUncropped);
        }
        Redraw();
        isSuccess = isSuccess && myView->BufferDump(aTilePixMap, theParams.BufferType);
        if (!isSuccess)
        {
          break;
        }
      }
      if (!isSuccess)
      {
        break;
      }
    }
  }

  // restore state
  myView->SetImmediateModeDrawToFront(aPrevImmediateMode);
  aCamera->Copy(aStoreMapping);
  if (aFBOPtr != aPrevFBOPtr)
  {
    myView->FBORelease(aFBOPtr);
  }
  else if (!aPrevFBOPtr.IsNull())
  {
    myView->FBOChangeViewport(aPrevFBOPtr, aPrevFBOVPSize.x(), aPrevFBOVPSize.y());
  }
  myView->SetFBO(aPrevFBOPtr);
  // apply default zlayer rendering parameters to view
  myView->SetZLayerTarget(Graphic3d_ZLayerId_BotOSD);
  myView->SetZLayerRedrawMode(false);
  return isSuccess;
}

//=================================================================================================

void V3d_View::ImmediateUpdate() const
{
  if (myImmediateUpdate)
  {
    Update();
  }
}

//=================================================================================================

bool V3d_View::SetImmediateUpdate(const bool theImmediateUpdate)
{
  bool aPreviousMode = myImmediateUpdate;
  myImmediateUpdate              = theImmediateUpdate;
  return aPreviousMode;
}

//=================================================================================================

void V3d_View::SetCamera(const occ::handle<Graphic3d_Camera>& theCamera)
{
  myView->SetCamera(theCamera);

  ImmediateUpdate();
}

//=================================================================================================

const occ::handle<Graphic3d_Camera>& V3d_View::Camera() const
{
  return myView->Camera();
}

//=================================================================================================

bool V3d_View::FitMinMax(const occ::handle<Graphic3d_Camera>& theCamera,
                                     const Bnd_Box&                  theBox,
                                     const double             theMargin,
                                     const double             theResolution,
                                     const bool          theToEnlargeIfLine) const
{
  if (!theCamera->FitMinMax(theBox, theResolution, theToEnlargeIfLine))
  {
    return false; // bounding box is out of bounds...
  }

  const double aZoomCoef = myView->ConsiderZoomPersistenceObjects();
  Scale(theCamera,
        theCamera->ViewDimensions().X() * (aZoomCoef + theMargin),
        theCamera->ViewDimensions().Y() * (aZoomCoef + theMargin));
  return true;
}

//=================================================================================================

void V3d_View::Scale(const occ::handle<Graphic3d_Camera>& theCamera,
                     const double             theSizeXv,
                     const double             theSizeYv) const
{
  double anAspect = theCamera->Aspect();
  if (anAspect > 1.0)
  {
    theCamera->SetScale(std::max(theSizeXv / anAspect, theSizeYv));
  }
  else
  {
    theCamera->SetScale(std::max(theSizeXv, theSizeYv * anAspect));
  }
  Invalidate();
}

//=================================================================================================

void V3d_View::Translate(const occ::handle<Graphic3d_Camera>& theCamera,
                         const double             theDXv,
                         const double             theDYv) const
{
  const gp_Pnt& aCenter = theCamera->Center();
  const gp_Dir& aDir    = theCamera->Direction();
  const gp_Dir& anUp    = theCamera->Up();
  gp_Ax3        aCameraCS(aCenter, aDir.Reversed(), aDir ^ anUp);

  gp_Vec  aCameraPanXv = gp_Vec(aCameraCS.XDirection()) * theDXv;
  gp_Vec  aCameraPanYv = gp_Vec(aCameraCS.YDirection()) * theDYv;
  gp_Vec  aCameraPan   = aCameraPanXv + aCameraPanYv;
  gp_Trsf aPanTrsf;
  aPanTrsf.SetTranslation(aCameraPan);

  theCamera->Transform(aPanTrsf);
  Invalidate();
}

//=================================================================================================

void V3d_View::DiagnosticInformation(NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>& theDict,
                                     Graphic3d_DiagnosticInfo              theFlags) const
{
  myView->DiagnosticInformation(theDict, theFlags);
}

//=================================================================================================

void V3d_View::StatisticInformation(NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>& theDict) const
{
  myView->StatisticInformation(theDict);
}

//=================================================================================================

TCollection_AsciiString V3d_View::StatisticInformation() const
{
  return myView->StatisticInformation();
}

//=================================================================================================

const Graphic3d_RenderingParams& V3d_View::RenderingParams() const
{
  return myView->RenderingParams();
}

//=================================================================================================

Graphic3d_RenderingParams& V3d_View::ChangeRenderingParams()
{
  return myView->ChangeRenderingParams();
}

//=================================================================================================

void V3d_View::SetLightOn(const occ::handle<V3d_Light>& theLight)
{
  if (!myActiveLights.Contains(theLight))
  {
    myActiveLights.Append(theLight);
    UpdateLights();
  }
}

//=================================================================================================

void V3d_View::SetLightOff(const occ::handle<V3d_Light>& theLight)
{
  if (MyViewer->IsGlobalLight(theLight))
    throw Standard_TypeMismatch("V3d_View::SetLightOff, the light is global");
  myActiveLights.Remove(theLight);
  UpdateLights();
}

//=================================================================================================

bool V3d_View::IsActiveLight(const occ::handle<V3d_Light>& theLight) const
{
  return !theLight.IsNull() && myActiveLights.Contains(theLight);
}

//=================================================================================================

void V3d_View::SetLightOn()
{
  for (NCollection_List<occ::handle<Graphic3d_CLight>>::Iterator aDefLightIter(MyViewer->DefinedLightIterator());
       aDefLightIter.More();
       aDefLightIter.Next())
  {
    if (!myActiveLights.Contains(aDefLightIter.Value()))
    {
      myActiveLights.Append(aDefLightIter.Value());
    }
  }
  UpdateLights();
}

//=================================================================================================

void V3d_View::SetLightOff()
{
  for (NCollection_List<occ::handle<Graphic3d_CLight>>::Iterator anActiveLightIter(myActiveLights); anActiveLightIter.More();)
  {
    if (!MyViewer->IsGlobalLight(anActiveLightIter.Value()))
    {
      myActiveLights.Remove(anActiveLightIter);
    }
    else
    {
      anActiveLightIter.Next();
    }
  }
  UpdateLights();
}

//=================================================================================================

bool V3d_View::IfMoreLights() const
{
  return myActiveLights.Extent() < LightLimit();
}

//=================================================================================================

int V3d_View::LightLimit() const
{
  return Viewer()->Driver()->InquireLightLimit();
}

//=================================================================================================

void V3d_View::AddClipPlane(const occ::handle<Graphic3d_ClipPlane>& thePlane)
{
  occ::handle<Graphic3d_SequenceOfHClipPlane> aSeqOfPlanes = ClipPlanes();
  if (aSeqOfPlanes.IsNull())
  {
    aSeqOfPlanes = new Graphic3d_SequenceOfHClipPlane();
  }
  else
  {
    for (Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt(*aSeqOfPlanes); aPlaneIt.More();
         aPlaneIt.Next())
    {
      const occ::handle<Graphic3d_ClipPlane>& aPlane = aPlaneIt.Value();
      if (aPlane == thePlane)
      {
        // plane is already defined in view
        return;
      }
    }
  }

  aSeqOfPlanes->Append(thePlane);
  SetClipPlanes(aSeqOfPlanes);
}

//=================================================================================================

void V3d_View::RemoveClipPlane(const occ::handle<Graphic3d_ClipPlane>& thePlane)
{
  occ::handle<Graphic3d_SequenceOfHClipPlane> aSeqOfPlanes = ClipPlanes();
  if (aSeqOfPlanes.IsNull())
  {
    return;
  }

  for (Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt(*aSeqOfPlanes); aPlaneIt.More();
       aPlaneIt.Next())
  {
    const occ::handle<Graphic3d_ClipPlane>& aPlane = aPlaneIt.Value();
    if (aPlane != thePlane)
    {
      continue;
    }

    aSeqOfPlanes->Remove(aPlaneIt);
    SetClipPlanes(aSeqOfPlanes);
    return;
  }
}

//=================================================================================================

void V3d_View::SetClipPlanes(const occ::handle<Graphic3d_SequenceOfHClipPlane>& thePlanes)
{
  myView->SetClipPlanes(thePlanes);
}

//=================================================================================================

const occ::handle<Graphic3d_SequenceOfHClipPlane>& V3d_View::ClipPlanes() const
{
  return myView->ClipPlanes();
}

//=================================================================================================

int V3d_View::PlaneLimit() const
{
  return Viewer()->Driver()->InquirePlaneLimit();
}

//=================================================================================================

void V3d_View::Move(const double    theDx,
                    const double    theDy,
                    const double    theDz,
                    const bool theStart)
{
  occ::handle<Graphic3d_Camera> aCamera = Camera();
  if (theStart)
  {
    myCamStartOpEye = aCamera->Eye();

    gp_Dir aReferencePlane(aCamera->Direction().Reversed());
    gp_Dir anUp(aCamera->Up());
    if (!screenAxis(aReferencePlane, anUp, myXscreenAxis, myYscreenAxis, myZscreenAxis))
    {
      throw V3d_BadValue("V3d_View::Translate, alignment of Eye,At,Up");
    }
  }

  double XX, XY, XZ, YX, YY, YZ, ZX, ZY, ZZ;
  myXscreenAxis.Coord(XX, XY, XZ);
  myYscreenAxis.Coord(YX, YY, YZ);
  myZscreenAxis.Coord(ZX, ZY, ZZ);

  aCamera->SetEye(myCamStartOpEye);

  aCamera->SetEye(aCamera->Eye().XYZ() + theDx * gp_Pnt(XX, XY, XZ).XYZ()
                  + theDy * gp_Pnt(YX, YY, YZ).XYZ() + theDz * gp_Pnt(ZX, ZY, ZZ).XYZ());

  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::Move(const double theLength, const bool theStart)
{
  occ::handle<Graphic3d_Camera> aCamera = Camera();
  if (theStart)
  {
    myCamStartOpEye = aCamera->Eye();
  }
  aCamera->SetEye(myCamStartOpEye);
  aCamera->SetEye(aCamera->Eye().XYZ() + theLength * myDefaultViewAxis.XYZ());

  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::Move(const V3d_TypeOfAxe    theAxe,
                    const double    theLength,
                    const bool theStart)
{
  switch (theAxe)
  {
    case V3d_X:
      Move(theLength, 0., 0., theStart);
      break;
    case V3d_Y:
      Move(0., theLength, 0., theStart);
      break;
    case V3d_Z:
      Move(0., 0., theLength, theStart);
      break;
  }
}

//=================================================================================================

void V3d_View::Translate(const double    theDx,
                         const double    theDy,
                         const double    theDz,
                         const bool theStart)
{
  occ::handle<Graphic3d_Camera> aCamera = Camera();
  if (theStart)
  {
    myCamStartOpEye    = aCamera->Eye();
    myCamStartOpCenter = aCamera->Center();

    gp_Dir aReferencePlane(aCamera->Direction().Reversed());
    gp_Dir anUp(aCamera->Up());
    if (!screenAxis(aReferencePlane, anUp, myXscreenAxis, myYscreenAxis, myZscreenAxis))
    {
      throw V3d_BadValue("V3d_View::Translate, alignment of Eye,At,Up");
    }
  }

  aCamera->SetEye(myCamStartOpEye);
  aCamera->SetCenter(myCamStartOpCenter);

  aCamera->SetCenter(aCamera->Center().XYZ() - theDx * myXscreenAxis.XYZ()
                     - theDy * myYscreenAxis.XYZ() - theDz * myZscreenAxis.XYZ());

  aCamera->SetEye(aCamera->Eye().XYZ() - theDx * myXscreenAxis.XYZ() - theDy * myYscreenAxis.XYZ()
                  - theDz * myZscreenAxis.XYZ());

  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::Translate(const V3d_TypeOfAxe    theAxe,
                         const double    theLength,
                         const bool theStart)
{
  switch (theAxe)
  {
    case V3d_X:
      Translate(theLength, 0., 0., theStart);
      break;
    case V3d_Y:
      Translate(0., theLength, 0., theStart);
      break;
    case V3d_Z:
      Translate(0., 0., theLength, theStart);
      break;
  }
}

//=================================================================================================

void V3d_View::Place(const int theXp,
                     const int theYp,
                     const double    theZoomFactor)
{
  int aWinWidth  = 0;
  int aWinHeight = 0;
  View()->Window()->Size(aWinWidth, aWinHeight);

  int aWinCXp = aWinWidth / 2;
  int aWinCYp = aWinHeight / 2;
  Pan(aWinCXp - theXp, -(aWinCYp - theYp), theZoomFactor / Scale());
}

//=================================================================================================

void V3d_View::Translate(const double theLength, const bool theStart)
{
  occ::handle<Graphic3d_Camera> aCamera = Camera();
  if (theStart)
  {
    myCamStartOpCenter = aCamera->Center();
  }

  gp_Pnt aNewCenter(myCamStartOpCenter.XYZ() - myDefaultViewAxis.XYZ() * theLength);
  aCamera->SetCenter(aNewCenter);

  ImmediateUpdate();
}

//=================================================================================================

void V3d_View::SetGrid(const gp_Ax3& aPlane, const occ::handle<Aspect_Grid>& aGrid)
{
  MyPlane = aPlane;
  MyGrid  = aGrid;

  double xl, yl, zl;
  double xdx, xdy, xdz;
  double ydx, ydy, ydz;
  double dx, dy, dz;
  aPlane.Location().Coord(xl, yl, zl);
  aPlane.XDirection().Coord(xdx, xdy, xdz);
  aPlane.YDirection().Coord(ydx, ydy, ydz);
  aPlane.Direction().Coord(dx, dy, dz);

  double CosAlpha = std::cos(MyGrid->RotationAngle());
  double SinAlpha = std::sin(MyGrid->RotationAngle());

  NCollection_Array2<double> Trsf1(1, 4, 1, 4);
  Trsf1(4, 4) = 1.0;
  Trsf1(4, 1) = Trsf1(4, 2) = Trsf1(4, 3) = 0.0;
  // Translation
  Trsf1(1, 4) = xl, Trsf1(2, 4) = yl, Trsf1(3, 4) = zl;
  // Transformation change of marker
  Trsf1(1, 1) = xdx, Trsf1(2, 1) = xdy, Trsf1(3, 1) = xdz, Trsf1(1, 2) = ydx, Trsf1(2, 2) = ydy,
           Trsf1(3, 2) = ydz, Trsf1(1, 3) = dx, Trsf1(2, 3) = dy, Trsf1(3, 3) = dz;

  NCollection_Array2<double> Trsf2(1, 4, 1, 4);
  Trsf2(4, 4) = 1.0;
  Trsf2(4, 1) = Trsf2(4, 2) = Trsf2(4, 3) = 0.0;
  // Translation of the origin
  Trsf2(1, 4) = -MyGrid->XOrigin(), Trsf2(2, 4) = -MyGrid->YOrigin(), Trsf2(3, 4) = 0.0;
  // Rotation Alpha around axis -Z
  Trsf2(1, 1) = CosAlpha, Trsf2(2, 1) = -SinAlpha, Trsf2(3, 1) = 0.0, Trsf2(1, 2) = SinAlpha,
           Trsf2(2, 2) = CosAlpha, Trsf2(3, 2) = 0.0, Trsf2(1, 3) = 0.0, Trsf2(2, 3) = 0.0,
           Trsf2(3, 3) = 1.0;

  double    valuetrsf;
  double    valueoldtrsf;
  double    valuenewtrsf;
  int i, j, k;
  // Calculation of the product of matrices
  for (i = 1; i <= 4; i++)
    for (j = 1; j <= 4; j++)
    {
      MyTrsf(i, j) = 0.0;
      for (k = 1; k <= 4; k++)
      {
        valueoldtrsf = Trsf1(i, k);
        valuetrsf    = Trsf2(k, j);
        valuenewtrsf = MyTrsf(i, j) + valueoldtrsf * valuetrsf;
        MyTrsf(i, j) = valuenewtrsf;
      }
    }
}

//=================================================================================================

void V3d_View::SetGridActivity(const bool AFlag)
{
  if (AFlag)
    MyGrid->Activate();
  else
    MyGrid->Deactivate();
}

//=================================================================================================

void toPolarCoords(const double theX,
                   const double theY,
                   double&      theR,
                   double&      thePhi)
{
  theR   = std::sqrt(theX * theX + theY * theY);
  thePhi = std::atan2(theY, theX);
}

//=================================================================================================

void toCartesianCoords(const double theR,
                       const double thePhi,
                       double&      theX,
                       double&      theY)
{
  theX = theR * std::cos(thePhi);
  theY = theR * std::sin(thePhi);
}

//=================================================================================================

Graphic3d_Vertex V3d_View::Compute(const Graphic3d_Vertex& theVertex) const
{
  const occ::handle<Graphic3d_Camera>& aCamera = Camera();
  gp_Dir                          VPN     = aCamera->Direction().Reversed(); // RefPlane
  gp_Dir                          GPN     = MyPlane.Direction();

  double XPp = 0.0, YPp = 0.0;
  Project(theVertex.X(), theVertex.Y(), theVertex.Z(), XPp, YPp);

// Casw when the plane of the grid and the plane of the view
// are perpendicular to MYEPSILON2 close radians
#define MYEPSILON2 M_PI / 180.0 // Delta between 2 angles
  if (std::abs(VPN.Angle(GPN) - M_PI / 2.) < MYEPSILON2)
  {
    return theVertex;
  }

  const gp_XYZ aPnt0 = V3d_View::TrsPoint(Graphic3d_Vertex(0.0, 0.0, 0.0), MyTrsf);

  // get grid axes in world space
  const gp_XYZ aPnt1 = V3d_View::TrsPoint(Graphic3d_Vertex(1.0, 0.0, 0.0), MyTrsf);
  gp_Vec       aGridX(aPnt0, aPnt1);
  aGridX.Normalize();

  const gp_XYZ aPnt2 = V3d_View::TrsPoint(Graphic3d_Vertex(0.0, 1.0, 0.0), MyTrsf);
  gp_Vec       aGridY(aPnt0, aPnt2);
  aGridY.Normalize();

  // project ray from camera onto grid plane
  const gp_Vec aProjection =
    aCamera->IsOrthographic()
      ? gp_Vec(aCamera->Direction())
      : gp_Vec(aCamera->Eye(), gp_Pnt(theVertex.X(), theVertex.Y(), theVertex.Z())).Normalized();
  const gp_Vec aPointOrigin = gp_Vec(gp_Pnt(theVertex.X(), theVertex.Y(), theVertex.Z()), aPnt0);
  const double aT =
    aPointOrigin.Dot(MyPlane.Direction()) / aProjection.Dot(MyPlane.Direction());
  const gp_XYZ aPointOnPlane =
    gp_XYZ(theVertex.X(), theVertex.Y(), theVertex.Z()) + aProjection.XYZ() * aT;

  if (occ::handle<Aspect_RectangularGrid> aRectGrid = occ::down_cast<Aspect_RectangularGrid>(MyGrid))
  {
    // project point on plane to grid local space
    const gp_Vec        aToPoint(aPnt0, aPointOnPlane);
    const double anXSteps = std::round(aGridX.Dot(aToPoint) / aRectGrid->XStep());
    const double anYSteps = std::round(aGridY.Dot(aToPoint) / aRectGrid->YStep());

    // clamp point to grid
    const gp_Vec aResult = aGridX * anXSteps * aRectGrid->XStep()
                           + aGridY * anYSteps * aRectGrid->YStep() + gp_Vec(aPnt0);
    return Graphic3d_Vertex(aResult.X(), aResult.Y(), aResult.Z());
  }
  else if (occ::handle<Aspect_CircularGrid> aCircleGrid = occ::down_cast<Aspect_CircularGrid>(MyGrid))
  {
    const double anAlpha = M_PI / double(aCircleGrid->DivisionNumber());

    // project point on plane to grid local space
    const gp_Vec  aToPoint(aPnt0, aPointOnPlane);
    double aLocalX = aGridX.Dot(aToPoint);
    double aLocalY = aGridY.Dot(aToPoint);
    double anR = 0.0, aPhi = 0.0;
    toPolarCoords(aLocalX, aLocalY, anR, aPhi);

    // clamp point to grid
    const double anRSteps  = std::round(anR / aCircleGrid->RadiusStep());
    const double aPhiSteps = std::round(aPhi / anAlpha);
    toCartesianCoords(anRSteps * aCircleGrid->RadiusStep(), aPhiSteps * anAlpha, aLocalX, aLocalY);

    const gp_Vec aResult = aGridX * aLocalX + aGridY * aLocalY + gp_Vec(aPnt0);
    return Graphic3d_Vertex(aResult.X(), aResult.Y(), aResult.Z());
  }
  return Graphic3d_Vertex(0.0, 0.0, 0.0);
}

//=================================================================================================

void V3d_View::ZBufferTriedronSetup(const Quantity_Color&  theXColor,
                                    const Quantity_Color&  theYColor,
                                    const Quantity_Color&  theZColor,
                                    const double    theSizeRatio,
                                    const double    theAxisDiametr,
                                    const int theNbFacettes)
{
  const occ::handle<V3d_Trihedron>& aTrihedron = Trihedron(true);
  aTrihedron->SetArrowsColor(theXColor, theYColor, theZColor);
  aTrihedron->SetSizeRatio(theSizeRatio);
  aTrihedron->SetNbFacets(theNbFacettes);
  aTrihedron->SetArrowDiameter(theAxisDiametr);
}

//=================================================================================================

void V3d_View::TriedronDisplay(const Aspect_TypeOfTriedronPosition thePosition,
                               const Quantity_Color&               theColor,
                               const double                 theScale,
                               const V3d_TypeOfVisualization       theMode)
{
  const occ::handle<V3d_Trihedron>& aTrihedron = Trihedron(true);
  aTrihedron->SetLabelsColor(theColor);
  aTrihedron->SetScale(theScale);
  aTrihedron->SetPosition(thePosition);
  aTrihedron->SetWireframe(theMode == V3d_WIREFRAME);

  aTrihedron->Display(*this);
}

//=================================================================================================

void V3d_View::TriedronErase()
{
  if (!myTrihedron.IsNull())
  {
    myTrihedron->Erase();
  }
}

//=================================================================================================

const Graphic3d_GraduatedTrihedron& V3d_View::GetGraduatedTrihedron() const
{
  return myView->GetGraduatedTrihedron();
}

//=================================================================================================

void V3d_View::GraduatedTrihedronDisplay(const Graphic3d_GraduatedTrihedron& theTrihedronData)
{
  myView->GraduatedTrihedronDisplay(theTrihedronData);
}

//=================================================================================================

void V3d_View::GraduatedTrihedronErase()
{
  myView->GraduatedTrihedronErase();
}

//=================================================================================================

void V3d_View::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myOldMouseX)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myOldMouseY)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myCamStartOpUp)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myCamStartOpDir)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myCamStartOpEye)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myCamStartOpCenter)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myDefaultCamera.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myView.get())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myImmediateUpdate)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsInvalidatedImmediate)

  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, MyViewer)
  for (NCollection_List<occ::handle<Graphic3d_CLight>>::Iterator anIterator(myActiveLights); anIterator.More(); anIterator.Next())
  {
    class occ::handle<Graphic3d_CLight>& anActiveLight = anIterator.ChangeValue();
    OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, anActiveLight)
  }
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myDefaultViewAxis)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myDefaultViewPoint)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, MyWindow.get())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, sx)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, sy)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, rx)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, ry)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myRotateGravity)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myComputedMode)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, SwitchSetFront)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myZRotation)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, MyZoomAtPointX)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, MyZoomAtPointY)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myTrihedron.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, MyGrid.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &MyPlane)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, MyGridEchoStructure.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, MyGridEchoGroup.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myXscreenAxis)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myYscreenAxis)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myZscreenAxis)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myViewAxis)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myGravityReferencePoint)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myAutoZFitIsOn)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myAutoZFitScaleFactor)
}
