// Copyright (c) 1995-1999 Matra Datavision
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

#ifdef _WIN32
  #include <windows.h>
#endif

#include <Visual3d_View.ixx>

#include <Graphic3d_DataStructureManager.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_MapOfStructure.hxx>
#include <Graphic3d_MapIteratorOfMapOfStructure.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_TextureEnv.hxx>
#include <Graphic3d_Vector.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Visual3d_DepthCueingDefinitionError.hxx>
#include <Visual3d_Light.hxx>
#include <Visual3d_HSequenceOfView.hxx>
#include <Visual3d_ZClippingDefinitionError.hxx>
#include <OSD.hxx>
#include <TColStd_HArray2OfReal.hxx>

#if defined(_WIN32)
  #include <WNT_Window.hxx>
#elif (defined(__APPLE__) && !defined(MACOSX_USE_GLX))
  #include <Cocoa_Window.hxx>
#else
  #include <Xw_Window.hxx>
#endif

#include <float.h>

// =======================================================================
// function : Visual3d_View
// purpose  :
// =======================================================================
Visual3d_View::Visual3d_View (const Handle(Visual3d_ViewManager)& theMgr)
: myViewManager         (theMgr.operator->()),
  myIsInComputedMode    (Standard_False),
  myAutoZFitIsOn        (Standard_True),
  myAutoZFitScaleFactor (1.0),
  myStructuresUpdated   (Standard_True)
{
  MyCView.ViewId                  = theMgr->Identification (this);
  MyCView.Active                  = 0;
  MyCView.IsDeleted               = 0;
  MyCView.WsId                    = -1;
  MyCView.DefWindow.IsDefined     = 0;
  MyCView.Context.NbActiveLight   = 0;
  MyCView.Context.ZBufferActivity = -1;

  MyCView.Backfacing    = 0;
  MyCView.ptrUnderLayer = 0;
  MyCView.ptrOverLayer  = 0;
  MyCView.GContext      = 0;
  MyCView.GDisplayCB    = 0;
  MyCView.GClientData   = 0;

  myGraphicDriver = myViewManager->GraphicDriver();
}

// =======================================================================
// function : SetWindow
// purpose  :
// =======================================================================
void Visual3d_View::SetWindow (const Handle(Aspect_Window)&      theWindow,
                               const Aspect_RenderingContext     theContext,
                               const Aspect_GraphicCallbackProc& theDisplayCB,
                               const Standard_Address            theClientData)
{
  if (IsDeleted())
  {
    return;
  }
  else if (IsDefined())
  {
    Visual3d_ViewDefinitionError::Raise ("Window already defined");
  }

  MyCView.GContext    = theContext;
  MyCView.GDisplayCB  = theDisplayCB;
  MyCView.GClientData = theClientData;
  SetWindow (theWindow);
}

// =======================================================================
// function : SetWindow
// purpose  :
// =======================================================================
void Visual3d_View::SetWindow (const Handle(Aspect_Window)& theWindow)
{
  if (IsDeleted())
  {
    return;
  }
  else if (IsDefined())
  {
    Visual3d_ViewDefinitionError::Raise ("Window already defined");
  }

  MyWindow = theWindow;
  MyCView.WsId = MyCView.ViewId;
  MyCView.DefWindow.IsDefined = 1;
  MyCView.DefWindow.XWindow       = theWindow->NativeHandle();
  MyCView.DefWindow.XParentWindow = theWindow->NativeParentHandle();

  Standard_Integer Width, Height;
  theWindow->Size (Width, Height);
  MyCView.DefWindow.dx = float(Width);
  MyCView.DefWindow.dy = float(Height);

  Standard_Real R, G, B;
  MyBackground = MyWindow->Background ();
  MyBackground.Color().Values (R, G, B, Quantity_TOC_RGB);
  MyCView.DefWindow.Background.r = float (R);
  MyCView.DefWindow.Background.g = float (G);
  MyCView.DefWindow.Background.b = float (B);

  UpdateView();
  if (!myGraphicDriver->View (MyCView))
  {
    Visual3d_ViewDefinitionError::Raise ("Association failed");
  }

  MyGradientBackground = MyWindow->GradientBackground();
  SetGradientBackground(MyGradientBackground,1);

  Standard_Boolean AWait = Standard_False; // => immediate update
  myGraphicDriver->SetVisualisation (MyCView);
  myGraphicDriver->AntiAliasing (MyCView, MyContext.AliasingIsOn());
  myGraphicDriver->DepthCueing  (MyCView, MyContext.DepthCueingIsOn());
  myGraphicDriver->ClipLimit    (MyCView, AWait);
  myGraphicDriver->Environment  (MyCView);

  // Make view manager z layer list consistent with the view's list.
  myViewManager->InstallZLayers (this);

  // Update planses of model clipping
  UpdatePlanes();

  // Update light sources
  UpdateLights();

  // Association view-window does not cause the display
  // of structures that can be displayed in the new view.
  // In fact, association view-window is done, but the
  // display is produced only if the view is activated (Activate).
  SetRatio();
}

// =======================================================================
// function : Window
// purpose  :
// =======================================================================
Handle(Aspect_Window) Visual3d_View::Window() const
{
  if (!IsDefined())
  {
    Visual3d_ViewDefinitionError::Raise ("Window not defined");
  }
  return MyWindow;
}

// =======================================================================
// function : IsDefined
// purpose  :
// =======================================================================
Standard_Boolean Visual3d_View::IsDefined() const
{
  return MyCView.DefWindow.IsDefined != 0;
}

// =======================================================================
// function : IsDeleted
// purpose  :
// =======================================================================
Standard_Boolean Visual3d_View::IsDeleted() const
{
  return MyCView.IsDeleted != 0;
}

// =======================================================================
// function : Destroy
// purpose  :
// =======================================================================
void Visual3d_View::Destroy()
{
  // Since myViewManager can be already destroyed,
  // avoid attempts to access it in SetBackground()
  myViewManager = NULL;
  Remove();
}

// =======================================================================
// function : Remove
// purpose  :
// =======================================================================
void Visual3d_View::Remove()
{
  if (IsDeleted()
  || !IsDefined())
  {
    return;
  }

  myStructsToCompute.Clear();
  myStructsComputed .Clear();
  myStructsDisplayed.Clear();

  Aspect_GradientBackground aBlackGrad;
  SetBackground (Aspect_Background (Quantity_NOC_BLACK));
  SetGradientBackground (aBlackGrad, Standard_False);

  if (myViewManager != NULL)
  {
    myViewManager->UnIdentification (MyCView.ViewId);
  }

  myGraphicDriver->RemoveView (MyCView);

  MyCView.WsId                = -1;
  MyCView.IsDeleted           = 1;
  MyCView.DefWindow.IsDefined = 0;

  MyWindow.Nullify();
}

// =======================================================================
// function : Resized
// purpose  :
// =======================================================================
void Visual3d_View::Resized()
{
  if (IsDeleted())
  {
    return;
  }
  else if (!IsDefined())
  {
    Visual3d_ViewDefinitionError::Raise ("Window not defined");
  }
  MyWindow->DoResize();
  SetRatio();
}

// =======================================================================
// function : SetRatio
// purpose  :
// =======================================================================
void Visual3d_View::SetRatio()
{
  if (IsDeleted())
  {
    return;
  }

  const Aspect_TypeOfUpdate anUpdateMode = myViewManager->UpdateMode();
  myViewManager->SetUpdateMode (Aspect_TOU_WAIT);

  Standard_Integer aWidth, aHeight;
  MyWindow->Size (aWidth, aHeight);
  if (aWidth > 0 && aHeight > 0)
  {
    Standard_Real aRatio = (Standard_Real)aWidth / (Standard_Real)aHeight; 

    MyCView.DefWindow.dx = Standard_ShortReal (aWidth);
    MyCView.DefWindow.dy = Standard_ShortReal (aHeight);

    myGraphicDriver->RatioWindow (MyCView);

    // Update camera aspect
    const Handle(Graphic3d_Camera)& aCamera = MyCView.Context.Camera;
    if (!aCamera.IsNull())
    {
      aCamera->SetAspect (aRatio);
    }

    if (!myDefaultCamera.IsNull())
    {
      myDefaultCamera->SetAspect (aRatio);
    }
  }

  myViewManager->SetUpdateMode (anUpdateMode);
  Update (anUpdateMode);
}

// =======================================================================
// function : UpdateLights
// purpose  :
// =======================================================================
void Visual3d_View::UpdateLights()
{
  if (IsDeleted()
   || !IsDefined())
  {
    return;
  }

  if (MyContext.Model() == Visual3d_TOM_NONE)
  {
    // activate only a white ambient light
    Graphic3d_CLight aCLight;
    aCLight.Type        = Visual3d_TOLS_AMBIENT;
    aCLight.IsHeadlight = Standard_False;
    aCLight.Color.r() = aCLight.Color.g() = aCLight.Color.b() = 1.0f;

    MyCView.Context.NbActiveLight = 1;
    MyCView.Context.ActiveLight   = &aCLight;
    myGraphicDriver->SetLight (MyCView);
    MyCView.Context.ActiveLight   = NULL;
    return;
  }

  MyCView.Context.NbActiveLight = Min (MyContext.NumberOfActivatedLights(),
                                       myGraphicDriver->InquireLightLimit());
  if (MyCView.Context.NbActiveLight < 1)
  {
    myGraphicDriver->SetLight (MyCView);
    return;
  }

  // parcing of light sources
  MyCView.Context.ActiveLight = new Graphic3d_CLight[MyCView.Context.NbActiveLight];
  for (Standard_Integer aLightIter = 0; aLightIter < MyCView.Context.NbActiveLight; ++aLightIter)
  {
    MyCView.Context.ActiveLight[aLightIter] = MyContext.ActivatedLight (aLightIter + 1)->CLight();
  }
  myGraphicDriver->SetLight (MyCView);
  delete[] MyCView.Context.ActiveLight;
  MyCView.Context.ActiveLight = NULL;
}

// =======================================================================
// function : UpdatePlanes
// purpose  :
// =======================================================================
void Visual3d_View::UpdatePlanes()
{
  MyCView.Context.ClipPlanes = MyContext.ClipPlanes();
  if (IsDeleted() || !IsDefined())
  {
    return;
  }

  myGraphicDriver->SetClipPlanes (MyCView);
}

// =======================================================================
// function : SetBackground
// purpose  :
// =======================================================================
void Visual3d_View::SetBackground (const Aspect_Background& theBack)
{
  if (IsDeleted())
  {
    return;
  }
  else if (!IsDefined())
  {
    Visual3d_ViewDefinitionError::Raise ("Window not defined");
  }

  // At this level, only GL can update the background.
  // It is not necessary to call MyWindow->SetBackground (ABack); as
  // this method starts update of window background by X
  // (if the windowing is X)

  Standard_Real R, G, B;
  MyBackground = theBack;
  MyBackground.Color().Values (R, G, B, Quantity_TOC_RGB);
  MyCView.DefWindow.Background.r = float (R);
  MyCView.DefWindow.Background.g = float (G);
  MyCView.DefWindow.Background.b = float (B);

  myGraphicDriver->Background (MyCView);

  if (myViewManager != NULL)
  {
    Update (myViewManager->UpdateMode());
  }
}

// =======================================================================
// function : SetGradientBackground
// purpose  :
// =======================================================================
void Visual3d_View::SetGradientBackground (const Aspect_GradientBackground& theBack,
                                           const Standard_Boolean           theToUpdate)
{
  if (IsDeleted())
  {
    return;
  }
  else if (!IsDefined())
  {
    Visual3d_ViewDefinitionError::Raise ("Window not defined");
  }

  MyGradientBackground = theBack;
  Quantity_Color aCol1, aCol2;
  MyGradientBackground.Colors (aCol1, aCol2);
  myGraphicDriver->GradientBackground (MyCView, aCol1, aCol2, MyGradientBackground.BgGradientFillMethod());

  if (theToUpdate)
  {
     Update (Aspect_TOU_ASAP);
  }
  else if (myViewManager != NULL)
  {
    Update (myViewManager->UpdateMode());
  }
}

// =======================================================================
// function : SetBackgroundImage
// purpose  :
// =======================================================================
void Visual3d_View::SetBackgroundImage (const Standard_CString  theFileName,
                                        const Aspect_FillMethod theFillStyle,
                                        const Standard_Boolean  theToUpdate)
{
  if (IsDeleted())
  {
    return;
  }
  if (!IsDefined())
  {
    Visual3d_ViewDefinitionError::Raise ("Window not defined");
  }

  myGraphicDriver->BackgroundImage (theFileName, MyCView, theFillStyle);

  Update (theToUpdate ? Aspect_TOU_ASAP : myViewManager->UpdateMode());
}

// =======================================================================
// function : SetBgImageStyle
// purpose  :
// =======================================================================
void Visual3d_View::SetBgImageStyle (const Aspect_FillMethod theFillStyle,
                                     const Standard_Boolean  theToUpdate)
{
  if (IsDeleted())
  {
    return;
  }
  if (!IsDefined())
  {
    Visual3d_ViewDefinitionError::Raise ("Window not defined");
  }

  myGraphicDriver->SetBgImageStyle (MyCView, theFillStyle);

  Update (theToUpdate ? Aspect_TOU_ASAP : myViewManager->UpdateMode());
}

// =======================================================================
// function : Background
// purpose  :
// =======================================================================
Aspect_Background Visual3d_View::Background() const
{
  return MyBackground;
}

// =======================================================================
// function : SetBgGradientStyle
// purpose  :
// =======================================================================
void Visual3d_View::SetBgGradientStyle (const Aspect_GradientFillMethod theFillStyle,
                                        const Standard_Boolean          theToUpdate)
{
  if (IsDeleted())
  {
    return;
  }
  else if (!IsDefined())
  {
    Visual3d_ViewDefinitionError::Raise ("Window not defined");
  }

  myGraphicDriver->SetBgGradientStyle (MyCView, theFillStyle);

  Update (theToUpdate ? Aspect_TOU_ASAP : myViewManager->UpdateMode());
}

// =======================================================================
// function : GradientBackground
// purpose  :
// =======================================================================
Aspect_GradientBackground Visual3d_View::GradientBackground() const
{
  return MyGradientBackground;
}

// =======================================================================
// function : DefaultCamera
// purpose  :
// =======================================================================
const Handle(Graphic3d_Camera)& Visual3d_View::DefaultCamera() const
{
  return myDefaultCamera;
}

// =======================================================================
// function : Camera
// purpose  :
// =======================================================================
const Handle(Graphic3d_Camera)& Visual3d_View::Camera() const
{
  return MyCView.Context.Camera;
}

// =======================================================================
// function : SetCamera
// purpose  :
// =======================================================================
void Visual3d_View::SetCamera (const Handle(Graphic3d_Camera)& theCamera)
{
  MyCView.Context.Camera = theCamera;

  myGraphicDriver->SetCamera (MyCView);

  Update (myViewManager->UpdateMode());
}

// =======================================================================
// function : SetViewOrientationDefault
// purpose  :
// =======================================================================
void Visual3d_View::SetViewOrientationDefault()
{
  if (myDefaultCamera.IsNull())
  {
    myDefaultCamera = new Graphic3d_Camera();
  }

  myDefaultCamera->CopyOrientationData (MyCView.Context.Camera);
}

// =======================================================================
// function : ViewOrientationReset
// purpose  :
// =======================================================================
void Visual3d_View::ViewOrientationReset()
{
  if (IsDeleted())
  {
    return;
  }

  if (!myDefaultCamera.IsNull())
  {
    MyCView.Context.Camera->CopyOrientationData (myDefaultCamera);
  }

  Update (myViewManager->UpdateMode());
}

// =======================================================================
// function : SetViewMappingDefault
// purpose  :
// =======================================================================
void Visual3d_View::SetViewMappingDefault()
{
  if (myDefaultCamera.IsNull())
  {
    myDefaultCamera = new Graphic3d_Camera();
  }
  myDefaultCamera->CopyMappingData (MyCView.Context.Camera);
}

// =======================================================================
// function : ViewMappingReset
// purpose  :
// =======================================================================
void Visual3d_View::ViewMappingReset()
{
  if (IsDeleted())
  {
    return;
  }

  if (!myDefaultCamera.IsNull())
  {
    MyCView.Context.Camera->CopyMappingData (myDefaultCamera);
  }

  Update (myViewManager->UpdateMode());
}

// =======================================================================
// function : SetContext
// purpose  :
// =======================================================================
void Visual3d_View::SetContext (const Visual3d_ContextView& theViewCtx)
{
  if (IsDeleted())
  {
    return;
  }

  // To manage display only in case of change of visualisation mode
  const bool isVisModeChanged = theViewCtx.Visualization() != MyContext.Visualization();
  const bool isModelChanged   = theViewCtx.Model()         != MyContext.Model();

  // To manage antialiasing only in case of change
  const Standard_Boolean anAliasingModeOld = MyContext.AliasingIsOn();
  const Standard_Boolean anAliasingModeNew = theViewCtx.AliasingIsOn();

  // To manage the depth cueing only in case of change
  const Standard_Boolean aDepthCueingModeOld = MyContext.DepthCueingIsOn();
  const Standard_Boolean aDepthCueingModeNew = theViewCtx.DepthCueingIsOn();

  const Standard_Real aDepthCueingFrontPlaneOld = MyContext.DepthCueingFrontPlane();
  const Standard_Real aDepthCueingFrontPlaneNew = theViewCtx.DepthCueingFrontPlane();
  const Standard_Real aDepthCueingBackPlaneOld  = MyContext.DepthCueingBackPlane();
  const Standard_Real aDepthCueingBackPlaneNew  = theViewCtx.DepthCueingBackPlane();

  // To manage the Zclipping only in case of change
  const Standard_Boolean aFrontZClippingModeOld = MyContext.FrontZClippingIsOn();
  const Standard_Boolean aFrontZClippingModeNew = theViewCtx.FrontZClippingIsOn();
  const Standard_Boolean aBackZClippingModeOld  = MyContext.BackZClippingIsOn();
  const Standard_Boolean aBackZClippingModeNew  = theViewCtx.BackZClippingIsOn();

  const Standard_Real aZClippingFrontPlaneOld = MyContext.ZClippingFrontPlane();
  const Standard_Real aZClippingFrontPlaneNew = theViewCtx.ZClippingFrontPlane();
  const Standard_Real aZClippingBackPlaneOld  = MyContext.ZClippingBackPlane();
  const Standard_Real aZClippingBackPlaneNew  = theViewCtx.ZClippingBackPlane();

  const bool isTextEnvChanged    = theViewCtx.TextureEnv()    != MyContext.TextureEnv();
  const bool isSurfDetailChanged = theViewCtx.SurfaceDetail() != MyContext.SurfaceDetail();

  MyContext = theViewCtx;

  UpdateView();

  Standard_Boolean toWait = Standard_False; // => immediate update
  if (IsDefined())
  {
    // management of visualization modes and types of shading.
    if (isVisModeChanged
     || isModelChanged)
    {
      myGraphicDriver->SetVisualisation (MyCView);
    }

    // management of antialiasing
    if (anAliasingModeOld != anAliasingModeNew)
    {
      myGraphicDriver->AntiAliasing (MyCView, anAliasingModeNew);
    }

    // management of depth_cueing
    if (aDepthCueingModeOld       != aDepthCueingModeNew
     || aDepthCueingFrontPlaneOld != aDepthCueingFrontPlaneNew
     || aDepthCueingBackPlaneOld  != aDepthCueingBackPlaneNew)
    {
      if (aDepthCueingModeNew
      &&  aDepthCueingBackPlaneNew >= aDepthCueingFrontPlaneNew)
      {
        Visual3d_DepthCueingDefinitionError::Raise ("Bad value for DepthCueingPlanes position");
      }
      myGraphicDriver->DepthCueing (MyCView, aDepthCueingModeNew);
    }

    // management of Zclipping
    if (aFrontZClippingModeOld  != aFrontZClippingModeNew
     || aBackZClippingModeOld   != aBackZClippingModeNew
     || aZClippingFrontPlaneOld != aZClippingFrontPlaneNew
     || aZClippingBackPlaneOld  != aZClippingBackPlaneNew)
    {
      if (aBackZClippingModeNew
       && aFrontZClippingModeNew
       && aZClippingBackPlaneNew >= aZClippingFrontPlaneNew)
      {
        Visual3d_ZClippingDefinitionError::Raise ("Bad value for ZClippingPlanes position");
      }

      myGraphicDriver->ClipLimit (MyCView, toWait);
    }

    // management of textures
    if (isTextEnvChanged
     || isSurfDetailChanged)
    {
      myGraphicDriver->Environment (MyCView);
    }

    UpdatePlanes(); // Update of planes of model clipping
    UpdateLights(); // Update of light sources
  }

  if (isVisModeChanged)
  {
    // Change of context =>
    // Remove structures that cannot be displayed in the new visualisation mode.
    // It is not necessary to warn ViewManager as this structure should not disappear from
    // the list of structures displayed in it.
    NCollection_Sequence<Handle(Graphic3d_Structure)> aStructs;
    for (Graphic3d_MapIteratorOfMapOfStructure aStructIter (myStructsDisplayed); aStructIter.More(); aStructIter.Next())
    {
      const Handle(Graphic3d_Structure)& aStruct  = aStructIter.Key();
      const Visual3d_TypeOfAnswer        anAnswer = AcceptDisplay (aStruct);
      if (anAnswer == Visual3d_TOA_NO
       || anAnswer == Visual3d_TOA_COMPUTE)
      {
        aStructs.Append (aStruct);
      }
    }
    for (NCollection_Sequence<Handle(Graphic3d_Structure)>::Iterator aStructIter (aStructs); aStructIter.More(); aStructIter.Next())
    {
      Erase (aStructIter.ChangeValue(), Aspect_TOU_WAIT);
    }
    aStructs.Clear();

    // Change of context =>
    // Display structures that can be displayed with the new visualisation mode.
    // All structures with status Displayed are removed from the ViewManager
    // and displayed in the view directly, if the structure is not already
    // displayed and if the view accepts it in its context.
    Graphic3d_MapOfStructure aMapDisplayed;
    myViewManager->DisplayedStructures (aMapDisplayed);
    for (Graphic3d_MapIteratorOfMapOfStructure aStructIter (aMapDisplayed); aStructIter.More(); aStructIter.Next())
    {
      const Handle(Graphic3d_Structure)& aStruct = aStructIter.Key();
      if (IsDisplayed (aStruct))
      {
        continue;
      }

      const Visual3d_TypeOfAnswer anAnswer = AcceptDisplay (aStruct);
      if (anAnswer == Visual3d_TOA_YES
       || anAnswer == Visual3d_TOA_COMPUTE)
      {
        aStructs.Append (aStruct);
      }
    }

    for (NCollection_Sequence<Handle(Graphic3d_Structure)>::Iterator aStructIter (aStructs); aStructIter.More(); aStructIter.Next())
    {
      Display (aStructIter.ChangeValue(), Aspect_TOU_WAIT);
    }
  }

  Update (myViewManager->UpdateMode());
}

// =======================================================================
// function : Context
// purpose  :
// =======================================================================
const Visual3d_ContextView& Visual3d_View::Context() const
{
  return MyContext;
}

// =======================================================================
// function : DisplayedStructures
// purpose  :
// =======================================================================
void Visual3d_View::DisplayedStructures (Graphic3d_MapOfStructure& theStructures) const
{
  if (IsDeleted())
  {
    return;
  }

  for (Graphic3d_MapIteratorOfMapOfStructure aStructIter (myStructsDisplayed); aStructIter.More(); aStructIter.Next())
  {
    theStructures.Add (aStructIter.Key());
  }
}

// =======================================================================
// function : Activate
// purpose  :
// =======================================================================
void Visual3d_View::Activate()
{
  if (IsDeleted())
  {
    return;
  }
  else if (!IsDefined())
  {
    Visual3d_ViewDefinitionError::Raise ("Window not defined");
  }

  if (!IsActive())
  {
    myGraphicDriver->ActivateView (MyCView);
    myGraphicDriver->Background   (MyCView);
    myGraphicDriver->Transparency (MyCView, myViewManager->Transparency());

    MyCView.Active = 1;

    // Activation of a new view =>
    // Display structures that can be displayed in this new view.
    // All structures with status
    // Displayed in ViewManager are returned and displayed in
    // the view directly, if the structure is not already
    // displayed and if the view accepts it in its context.
    Graphic3d_MapOfStructure aDisplayedStructs;
    myViewManager->DisplayedStructures (aDisplayedStructs);
    for (Graphic3d_MapIteratorOfMapOfStructure aStructIter (aDisplayedStructs); aStructIter.More(); aStructIter.Next())
    {
      const Handle(Graphic3d_Structure)& aStruct = aStructIter.Key();
      if (IsDisplayed (aStruct))
      {
        continue;
      }

      // If the structure can be displayed in the new context of the view, it is displayed.
      const Visual3d_TypeOfAnswer anAnswer = AcceptDisplay (aStruct);
      if (anAnswer == Visual3d_TOA_YES
       || anAnswer == Visual3d_TOA_COMPUTE)
      {
        Display (aStruct, Aspect_TOU_WAIT);
      }
    }
  }

  // If the activation/desactivation of ZBuffer should be automatic
  // depending on the presence or absence of facets.
  if (myViewManager->ZBufferAuto())
  {
    const Standard_Boolean containsFacet = ContainsFacet();
    const Standard_Boolean hasZBuffer    = ZBufferIsActivated();
    if (containsFacet && !hasZBuffer)
    {
      SetZBufferActivity (1); // If the view contains facets and if ZBuffer is not active
    }
    else if (!containsFacet && hasZBuffer)
    {
      SetZBufferActivity (0); // If the view does not contain facets and if ZBuffer is active
    }
  }

  Update (myViewManager->UpdateMode());
}

// =======================================================================
// function : IsActive
// purpose  :
// =======================================================================
Standard_Boolean Visual3d_View::IsActive() const
{
  return !IsDeleted()
      &&  MyCView.Active;
}

// =======================================================================
// function : Deactivate
// purpose  :
// =======================================================================
void Visual3d_View::Deactivate()
{
  if (IsDeleted())
  {
    return;
  }
  else if (!IsDefined())
  {
    Visual3d_ViewDefinitionError::Raise ("Window not defined");
  }

  if (IsActive())
  {
    myGraphicDriver->DeactivateView (MyCView);

    // Deactivation of a view =>
    // Removal of structures displayed in this view.
    // All structures with status
    // Displayed in ViewManager are returned and removed from
    // the view directly, if the structure is not already
    // displayed and if the view accepts it in its context.
    Graphic3d_MapOfStructure aDisplayedStructs;
    myViewManager->DisplayedStructures (aDisplayedStructs);
    for (Graphic3d_MapIteratorOfMapOfStructure aStructIter (aDisplayedStructs); aStructIter.More(); aStructIter.Next())
    {
      const Handle(Graphic3d_Structure)& aStruct = aStructIter.Key();
      if (IsDisplayed (aStruct))
      {
        continue;
      }

      const Visual3d_TypeOfAnswer anAnswer = AcceptDisplay (aStruct);
      if (anAnswer == Visual3d_TOA_YES
       || anAnswer == Visual3d_TOA_COMPUTE)
      {
        Erase (aStruct, Aspect_TOU_WAIT);
      }
    }

    Update (myViewManager->UpdateMode());
    MyCView.Active = 0; // No action currently possible in the view
  }
}

// =======================================================================
// function : Redraw
// purpose  :
// =======================================================================
void Visual3d_View::Redraw()
{
  Redraw (myViewManager->UnderLayer(), myViewManager->OverLayer(), 0, 0, 0, 0);
}

// =======================================================================
// function : Redraw
// purpose  :
// =======================================================================
void Visual3d_View::Redraw (const Standard_Integer theX,
                            const Standard_Integer theY,
                            const Standard_Integer theWidth,
                            const Standard_Integer theHeight)
{
  Redraw (myViewManager->UnderLayer(), myViewManager->OverLayer(),
          theX, theY, theWidth, theHeight);
}

// =======================================================================
// function : Redraw
// purpose  :
// =======================================================================
void Visual3d_View::Redraw (const Handle(Visual3d_Layer)& theUnderLayer,
                            const Handle(Visual3d_Layer)& theOverLayer)
{
  Redraw (theUnderLayer, theOverLayer, 0, 0, 0, 0);
}

// =======================================================================
// function : Redraw
// purpose  :
// =======================================================================
void Visual3d_View::Redraw (const Handle(Visual3d_Layer)& theUnderLayer,
                            const Handle(Visual3d_Layer)& theOverLayer,
                            const Standard_Integer        theX,
                            const Standard_Integer        theY,
                            const Standard_Integer        theWidth,
                            const Standard_Integer        theHeight)
{
  if (IsDeleted()
   || !IsDefined()
   || !IsActive()
   || !MyWindow->IsMapped())
  {
    return;
  }

  Aspect_CLayer2d anOverCLayer, anUnderCLayer;
  anOverCLayer.ptrLayer = anUnderCLayer.ptrLayer = NULL;
  if (!theOverLayer .IsNull()) anOverCLayer  = theOverLayer ->CLayer();
  if (!theUnderLayer.IsNull()) anUnderCLayer = theUnderLayer->CLayer();

  for (Standard_Integer aRetryIter = 0; aRetryIter < 2; ++aRetryIter)
  {
    if (myGraphicDriver->IsDeviceLost())
    {
      myViewManager->RecomputeStructures();
      myViewManager->RecomputeStructures (myImmediateStructures);
      myGraphicDriver->ResetDeviceLostFlag();
    }

    // set up Z buffer state before redrawing
    if (myViewManager->ZBufferAuto())
    {
      const Standard_Boolean hasFacet   = ContainsFacet();
      const Standard_Boolean hasZBuffer = ZBufferIsActivated();
      // if the view contains facets and if ZBuffer is not active
      if (hasFacet && !hasZBuffer)
      {
        SetZBufferActivity (1);
      }
      // if the view contains only facets and if ZBuffer is active
      if (!hasFacet && hasZBuffer)
      {
        SetZBufferActivity (0);
      }
    }

    if (myStructuresUpdated)
    {
      AutoZFit();
      myStructuresUpdated = Standard_False;
    }

    myGraphicDriver->Redraw (MyCView, anUnderCLayer, anOverCLayer, theX, theY, theWidth, theHeight);
    if (!myGraphicDriver->IsDeviceLost())
    {
      return;
    }
  }
}

// =======================================================================
// function : RedrawImmediate
// purpose  :
// =======================================================================
void Visual3d_View::RedrawImmediate()
{
  RedrawImmediate (myViewManager->UnderLayer(), myViewManager->OverLayer());
}

// =======================================================================
// function : RedrawImmediate
// purpose  :
// =======================================================================
void Visual3d_View::RedrawImmediate (const Handle(Visual3d_Layer)& theUnderLayer,
                                     const Handle(Visual3d_Layer)& theOverLayer)
{
  if (IsDeleted()
   || !IsDefined()
   || !IsActive()
   || !MyWindow->IsMapped())
  {
    return;
  }

  Aspect_CLayer2d anOverCLayer, anUnderCLayer;
  anOverCLayer.ptrLayer = anUnderCLayer.ptrLayer = NULL;
  if (!theOverLayer .IsNull()) anOverCLayer  = theOverLayer ->CLayer();
  if (!theUnderLayer.IsNull()) anUnderCLayer = theUnderLayer->CLayer();
  myGraphicDriver->RedrawImmediate (MyCView, anUnderCLayer, anOverCLayer);
}

// =======================================================================
// function : Invalidate
// purpose  :
// =======================================================================
void Visual3d_View::Invalidate()
{
  myGraphicDriver->Invalidate (MyCView);
}

// =======================================================================
// function : Update
// purpose  :
// =======================================================================
void Visual3d_View::Update (Aspect_TypeOfUpdate theUpdateMode)
{
  myStructuresUpdated = Standard_True;
  if (theUpdateMode == Aspect_TOU_ASAP)
  {
    Compute();
    Redraw (myViewManager->UnderLayer(), myViewManager->OverLayer(), 0, 0, 0, 0);
  }
}

// =======================================================================
// function : Update
// purpose  :
// =======================================================================
void Visual3d_View::Update (const Handle(Visual3d_Layer)& theUnderLayer,
                            const Handle(Visual3d_Layer)& theOverLayer)
{
  Compute();
  myStructuresUpdated = Standard_True;
  Redraw (theUnderLayer, theOverLayer, 0, 0, 0, 0);
}

// ========================================================================
// function : SetAutoZFitMode
// purpose  :
// ========================================================================
void Visual3d_View::SetAutoZFitMode (const Standard_Boolean theIsOn,
                                     const Standard_Real    theScaleFactor)
{
  Standard_ASSERT_RAISE (theScaleFactor > 0.0, "Zero or negative scale factor is not allowed.");
  myAutoZFitScaleFactor = theScaleFactor;
  myAutoZFitIsOn = theIsOn;
}

// ========================================================================
// function : AutoZFitMode
// purpose  :
// ========================================================================
Standard_Boolean Visual3d_View::AutoZFitMode() const
{
  return myAutoZFitIsOn;
}

// ========================================================================
// function : AutoZFitScaleFactor
// purpose  :
// ========================================================================
Standard_Real Visual3d_View::AutoZFitScaleFactor() const
{
  return myAutoZFitScaleFactor;
}

// ========================================================================
// function : AutoZFit
// purpose  :
// ========================================================================
void Visual3d_View::AutoZFit()
{
  if (!AutoZFitMode())
  {
    return;
  }

  ZFitAll (myAutoZFitScaleFactor);
}

// ========================================================================
// function : ZFitAll
// purpose  :
// ========================================================================
void Visual3d_View::ZFitAll (const Standard_Real theScaleFactor)
{
  Standard_Real aMinMax[6];    // applicative min max boundaries
  MinMaxValues (aMinMax[0], aMinMax[1], aMinMax[2],
    aMinMax[3], aMinMax[4], aMinMax[5],
    Standard_False);

  Standard_Real aGraphicBB[6]; // real graphical boundaries (not accounting infinite flag).
  MinMaxValues (aGraphicBB[0], aGraphicBB[1], aGraphicBB[2],
    aGraphicBB[3], aGraphicBB[4], aGraphicBB[5],
    Standard_True);

  Bnd_Box aMinMaxBox;
  Bnd_Box aGraphicBox;

  aMinMaxBox.Update (aMinMax[0], aMinMax[1], aMinMax[2],
    aMinMax[3], aMinMax[4], aMinMax[5]);

  aGraphicBox.Update (aGraphicBB[0], aGraphicBB[1], aGraphicBB[2],
    aGraphicBB[3], aGraphicBB[4], aGraphicBB[5]);

  const Handle(Graphic3d_Camera)& aCamera = MyCView.Context.Camera;
  aCamera->ZFitAll (theScaleFactor, aMinMaxBox, aGraphicBox);
}

// ========================================================================
// function : AcceptDisplay
// purpose  :
// ========================================================================
Visual3d_TypeOfAnswer Visual3d_View::AcceptDisplay (const Handle(Graphic3d_Structure)& theStruct) const
{
  const Visual3d_TypeOfVisualization ViewType = MyContext.Visualization();
  switch (theStruct->Visual())
  {
    case Graphic3d_TOS_ALL:
    {
      return Visual3d_TOA_YES; // The structure accepts any type of view
    }
    case Graphic3d_TOS_SHADING:
    {
      return ViewType == Visual3d_TOV_SHADING
           ? Visual3d_TOA_YES
           : Visual3d_TOA_NO;
    }
    case Graphic3d_TOS_WIREFRAME:
    {
      return ViewType == Visual3d_TOV_WIREFRAME
           ? Visual3d_TOA_YES
           : Visual3d_TOA_NO;
    }
    case Graphic3d_TOS_COMPUTED:
    {
      return (ViewType == Visual3d_TOV_SHADING || ViewType == Visual3d_TOV_WIREFRAME)
           ?  Visual3d_TOA_COMPUTE
           :  Visual3d_TOA_NO;
    }
  }
  return Visual3d_TOA_NO;
}

// ========================================================================
// function : ChangeDisplayPriority
// purpose  :
// ========================================================================
void Visual3d_View::ChangeDisplayPriority (const Handle(Graphic3d_Structure)& theStruct,
                                           const Standard_Integer           /*theOldPriority*/,
                                           const Standard_Integer             theNewPriority)
{
  if (IsDeleted()
  || !IsDefined()
  || !IsActive()
  || !IsDisplayed (theStruct))
  {
    return;
  }

  if (!myIsInComputedMode)
  {
    myGraphicDriver->ChangePriority (*theStruct->CStructure(), MyCView, theNewPriority);
    return;
  }

  const Standard_Integer      anIndex  = IsComputed (theStruct);
  const Graphic3d_CStructure& aCStruct = anIndex != 0
                                       ? *(myStructsComputed.Value (anIndex)->CStructure())
                                       : *theStruct->CStructure();
  myGraphicDriver->ChangePriority (aCStruct, MyCView, theNewPriority);
}

// ========================================================================
// function : Clear
// purpose  :
// ========================================================================
void Visual3d_View::Clear (const Handle(Graphic3d_Structure)& theStruct,
                           const Standard_Boolean             theWithDestruction)
{
  const Standard_Integer anIndex = IsComputed (theStruct);
  if (anIndex != 0)
  {
    const Handle(Graphic3d_Structure)& aCompStruct = myStructsComputed.Value (anIndex);
    aCompStruct->GraphicClear (theWithDestruction);
    aCompStruct->SetHLRValidation (Standard_False);
  }
}

// ========================================================================
// function : Connect
// purpose  :
// ========================================================================
void Visual3d_View::Connect (const Handle(Graphic3d_Structure)& theMother,
                             const Handle(Graphic3d_Structure)& theDaughter)
{
  Standard_Integer anIndexM = IsComputed (theMother);
  Standard_Integer anIndexD = IsComputed (theDaughter);
  if (anIndexM != 0
   && anIndexD != 0)
  {
    const Handle(Graphic3d_Structure)& aStructM = myStructsComputed.Value (anIndexM);
    const Handle(Graphic3d_Structure)& aStructD = myStructsComputed.Value (anIndexD);
    aStructM->GraphicConnect (aStructD);
  }
}

// ========================================================================
// function : Disconnect
// purpose  :
// ========================================================================
void Visual3d_View::Disconnect (const Handle(Graphic3d_Structure)& theMother,
                                const Handle(Graphic3d_Structure)& theDaughter)
{
  Standard_Integer anIndexM = IsComputed (theMother);
  Standard_Integer anIndexD = IsComputed (theDaughter);
  if (anIndexM != 0
   && anIndexD != 0)
  {
    const Handle(Graphic3d_Structure)& aStructM = myStructsComputed.Value (anIndexM);
    const Handle(Graphic3d_Structure)& aStructD = myStructsComputed.Value (anIndexD);
    aStructM->GraphicDisconnect (aStructD);
  }
}

// ========================================================================
// function : DisplayImmediate
// purpose  :
// ========================================================================
Standard_Boolean Visual3d_View::DisplayImmediate (const Handle(Graphic3d_Structure)& theStructure,
                                                  const Standard_Boolean             theIsSingleView)
{
  if (!myImmediateStructures.Add (theStructure))
  {
    return Standard_False;
  }

  if (theIsSingleView)
  {
    Handle(Visual3d_HSequenceOfView) aViews = myViewManager->DefinedView();

    for (int i=1;i<=aViews->Length();i++)
    {
      if (aViews->Value(i).Access() != this)
      {
        aViews->Value(i)->EraseImmediate (theStructure);
      }
    }
  }

  myGraphicDriver->DisplayImmediateStructure (MyCView, *theStructure->CStructure());
  return Standard_True;
}

// ========================================================================
// function : EraseImmediate
// purpose  :
// ========================================================================
Standard_Boolean Visual3d_View::EraseImmediate (const Handle(Graphic3d_Structure)& theStructure)
{
  const Standard_Boolean isErased = myImmediateStructures.Remove (theStructure);
  if (isErased)
  {
    myGraphicDriver->EraseImmediateStructure (MyCView, *theStructure->CStructure());
  }

  return isErased;
}

// ========================================================================
// function : ClearImmediate
// purpose  :
// ========================================================================
Standard_Boolean Visual3d_View::ClearImmediate()
{
  if (myImmediateStructures.IsEmpty())
  {
    return Standard_False;
  }

  for (Graphic3d_MapIteratorOfMapOfStructure anIter (myImmediateStructures); anIter.More(); anIter.Next())
  {
    myGraphicDriver->EraseImmediateStructure (MyCView, *anIter.Key()->CStructure());
  }
  myImmediateStructures.Clear();
  return Standard_True;
}

// ========================================================================
// function : Display
// purpose  :
// ========================================================================
void Visual3d_View::Display (const Handle(Graphic3d_Structure)& theStruct)
{
  Display (theStruct, myViewManager->UpdateMode());
}

// ========================================================================
// function : Display
// purpose  :
// ========================================================================
void Visual3d_View::Display (const Handle(Graphic3d_Structure)& theStruct,
                             const Aspect_TypeOfUpdate          theUpdateMode)
{
  if (IsDeleted()
  || !IsDefined()
  || !IsActive())
  {
    return;
  }

  // If Display on a structure present in the list of calculated structures while it is not
  // or more, of calculated type =>
  // - removes it as well as the associated old computed
  // THis happens when hlhsr becomes again of type e non computed after SetVisual.
  Standard_Integer anIndex = IsComputed (theStruct);
  if (anIndex != 0
   && theStruct->Visual() != Graphic3d_TOS_COMPUTED)
  {
    myStructsToCompute.Remove (anIndex);
    myStructsComputed .Remove (anIndex);
    anIndex = 0;
  }

  Visual3d_TypeOfAnswer anAnswer = AcceptDisplay (theStruct);
  if (anAnswer == Visual3d_TOA_NO)
  {
    return;
  }

  if (!ComputedMode())
  {
    anAnswer = Visual3d_TOA_YES;
  }

  if (anAnswer == Visual3d_TOA_YES)
  {
    if (IsDisplayed (theStruct))
    {
      return;
    }

    theStruct->CalculateBoundBox();
    myGraphicDriver->DisplayStructure (MyCView, *theStruct->CStructure(), theStruct->DisplayPriority());
    myStructsDisplayed.Add (theStruct);
    Update (theUpdateMode);
    return;
  }
  else if (anAnswer != Visual3d_TOA_COMPUTE)
  {
    return;
  }

  if (anIndex != 0)
  {
    // Already computed, is COMPUTED still valid?
    const Handle(Graphic3d_Structure)& anOldStruct = myStructsComputed.Value (anIndex);
    if (anOldStruct->HLRValidation())
    {
      // Case COMPUTED valid
      // to be displayed
      if (!IsDisplayed (theStruct))
      {
        myStructsDisplayed.Add (theStruct);
        myGraphicDriver->DisplayStructure (MyCView, *anOldStruct->CStructure(), theStruct->DisplayPriority());
        Update (theUpdateMode);
      }
      return;
    }
    else
    {
      // Case COMPUTED invalid
      // Is there another valid representation?
      // Find in the sequence of already calculated structures
      // 1/ Structure having the same Owner as <AStructure>
      // 2/ That is not <AStructure>
      // 3/ The COMPUTED which of is valid
      const Standard_Integer aNewIndex = HaveTheSameOwner (theStruct);
      if (aNewIndex != 0)
      {
        // Case of COMPUTED invalid, WITH a valid of replacement
        // to be displayed
        if (!IsDisplayed (theStruct))
        {
          const Handle(Graphic3d_Structure)& aNewStruct = myStructsComputed.Value (aNewIndex);
          myStructsComputed.SetValue (anIndex, aNewStruct);
          myStructsDisplayed.Add (theStruct);
          myGraphicDriver->DisplayStructure (MyCView, *aNewStruct->CStructure(), theStruct->DisplayPriority());
          Update (theUpdateMode);
        }
        return;
      }
      else
      {
        // Case COMPUTED invalid, WITHOUT a valid of replacement
        // COMPUTED is removed if displayed
        if (IsDisplayed (theStruct))
        {
          myGraphicDriver->EraseStructure (MyCView, *anOldStruct->CStructure());
        }
      }
    }
  }

  // Compute + Validation
  Handle(Graphic3d_Structure) aStruct;
  TColStd_Array2OfReal aTrsf (0, 3, 0, 3);
  theStruct->Transform (aTrsf);
  if (anIndex != 0)
  {
    TColStd_Array2OfReal anIdent (0, 3, 0, 3);
    for (Standard_Integer ii = 0; ii <= 3; ++ii)
    {
      for (Standard_Integer jj = 0; jj <= 3; ++jj)
      {
        anIdent (ii, jj) = (ii == jj ? 1.0 : 0.0);
      }
    }

    aStruct = myStructsComputed.Value (anIndex);
    aStruct->SetTransform (anIdent, Graphic3d_TOC_REPLACE);
    if (theStruct->IsTransformed())
    {
      theStruct->Compute (this, aTrsf, aStruct);
    }
    else
    {
      theStruct->Compute (this, aStruct);
    }
  }
  else
  {
    aStruct = theStruct->IsTransformed()
            ? theStruct->Compute (this, aTrsf)
            : theStruct->Compute (this);
  }

  aStruct->SetHLRValidation (Standard_True);

  // TOCOMPUTE and COMPUTED associated to sequences are added
  myStructsToCompute.Append (theStruct);
  myStructsComputed .Append (aStruct);

  // The previous are removed if necessary
  if (anIndex != 0)
  {
    myStructsToCompute.Remove (anIndex);
    myStructsComputed .Remove (anIndex);
  }

  // Of which type will be the computed?
  const Visual3d_TypeOfVisualization aViewType = MyContext.Visualization();
  const Standard_Boolean toComputeWireframe = aViewType == Visual3d_TOV_WIREFRAME
                                           && theStruct->ComputeVisual() != Graphic3d_TOS_SHADING;
  const Standard_Boolean toComputeShading   = aViewType == Visual3d_TOV_SHADING
                                           && theStruct->ComputeVisual() != Graphic3d_TOS_WIREFRAME;
  if (!toComputeShading && !toComputeWireframe)
  {
    anAnswer = Visual3d_TOA_NO;
  }
  else
  {
    aStruct->SetVisual (toComputeWireframe ? Graphic3d_TOS_WIREFRAME : Graphic3d_TOS_SHADING);
    anAnswer = AcceptDisplay (aStruct);
  }

  if (theStruct->IsHighlighted())
  {
    aStruct->SetHighlightColor (theStruct->HighlightColor());
    aStruct->GraphicHighlight (Aspect_TOHM_COLOR);
  }

  // It is displayed only if the calculated structure
  // has a proper type corresponding to the one of the view.
  if (anAnswer != Visual3d_TOA_NO)
  {
    if (!IsDisplayed (theStruct))
    {
      myStructsDisplayed.Add (theStruct);
    }
    myGraphicDriver->DisplayStructure (MyCView, *aStruct->CStructure(), theStruct->DisplayPriority());
    Update (theUpdateMode);
  }
}

// ========================================================================
// function : Erase
// purpose  :
// ========================================================================
void Visual3d_View::Erase (const Handle(Graphic3d_Structure)& theStruct)
{
  if (!IsDeleted())
  {
    Erase (theStruct, myViewManager->UpdateMode());
  }
}

// ========================================================================
// function : Erase
// purpose  :
// ========================================================================
void Visual3d_View::Erase (const Handle(Graphic3d_Structure)& theStruct,
                           const Aspect_TypeOfUpdate          theUpdateMode)
{
  if (IsDeleted()
  || !IsDisplayed (theStruct))
  {
    return;
  }

  Visual3d_TypeOfAnswer anAnswer = AcceptDisplay (theStruct);
  if (!ComputedMode())
  {
    anAnswer = Visual3d_TOA_YES;
  }

  if (anAnswer != Visual3d_TOA_COMPUTE)
  {
    myGraphicDriver->EraseStructure (MyCView, *theStruct->CStructure());
  }
  else if (anAnswer == Visual3d_TOA_COMPUTE
       && myIsInComputedMode)
  {
    const Standard_Integer anIndex = IsComputed (theStruct);
    if (anIndex != 0)
    {
      const Handle(Graphic3d_Structure)& aCompStruct = myStructsComputed.ChangeValue (anIndex);
      myGraphicDriver->EraseStructure (MyCView, *aCompStruct->CStructure());
    }
  }
  myStructsDisplayed.Remove (theStruct);
  Update (theUpdateMode);
}

// ========================================================================
// function : Highlight
// purpose  :
// ========================================================================
void Visual3d_View::Highlight (const Handle(Graphic3d_Structure)& theStruct,
                               const Aspect_TypeOfHighlightMethod theMethod)
{
  const Standard_Integer anIndex = IsComputed (theStruct);
  if (anIndex != 0)
  {
    const Handle(Graphic3d_Structure)& aCompStruct = myStructsComputed.ChangeValue (anIndex);
    aCompStruct->SetHighlightColor (theStruct->HighlightColor());
    aCompStruct->GraphicHighlight (theMethod);
  }
}

// ========================================================================
// function : SetTransform
// purpose  :
// ========================================================================
void Visual3d_View::SetTransform (const Handle(Graphic3d_Structure)& theStruct,
                                  const TColStd_Array2OfReal&        theTrsf)
{
  const Standard_Integer anIndex = IsComputed (theStruct);
  if (anIndex != 0)
  {
    // Test is somewhat light !
    // trsf is transferred only if it is :
    // a translation
    // a scale
    if (theTrsf (0, 1) != 0.0 || theTrsf (0, 2) != 0.0
     || theTrsf (1, 0) != 0.0 || theTrsf (1, 2) != 0.0
     || theTrsf (2, 0) != 0.0 || theTrsf (2, 1) != 0.0)
    {
      ReCompute (theStruct);
    }
    else
    {
      const Handle(Graphic3d_Structure)& aCompStruct = myStructsComputed.ChangeValue (anIndex);
      aCompStruct->GraphicTransform (theTrsf);
    }
  }

  theStruct->CalculateBoundBox();
  if (!theStruct->IsMutable()
   && !theStruct->CStructure()->IsForHighlight
   && !theStruct->CStructure()->IsInfinite)
  {
    const Standard_Integer aLayerId = theStruct->GetZLayer();
    myGraphicDriver->InvalidateBVHData (MyCView, aLayerId);
  }
}

// ========================================================================
// function : UnHighlight
// purpose  :
// ========================================================================
void Visual3d_View::UnHighlight (const Handle(Graphic3d_Structure)& theStruct)
{
  Standard_Integer anIndex = IsComputed (theStruct);
  if (anIndex != 0)
  {
    const Handle(Graphic3d_Structure)& aCompStruct = myStructsComputed.ChangeValue (anIndex);
    aCompStruct->GraphicUnHighlight();
  }
}

// ========================================================================
// function : IsComputed
// purpose  :
// ========================================================================
Standard_Integer Visual3d_View::IsComputed (const Handle(Graphic3d_Structure)& theStruct) const
{
  const Standard_Integer aStructId  = theStruct->Identification();
  const Standard_Integer aNbStructs = myStructsToCompute.Length();
  for (Standard_Integer aStructIter = 1; aStructIter <= aNbStructs; ++aStructIter)
  {
    const Handle(Graphic3d_Structure)& aStruct = myStructsToCompute.Value (aStructIter);
    if (aStruct->Identification() == aStructId)
    {
      return aStructIter;
    }
  }
  return 0;
}

// ========================================================================
// function : IsDisplayed
// purpose  :
// ========================================================================
Standard_Boolean Visual3d_View::IsDisplayed (const Handle(Graphic3d_Structure)& theStruct) const
{
  return !IsDeleted()
      &&  myStructsDisplayed.Contains (theStruct);
}

// ========================================================================
// function : ContainsFacet
// purpose  :
// ========================================================================
Standard_Boolean Visual3d_View::ContainsFacet() const
{
  return ContainsFacet (myStructsDisplayed);
}

// ========================================================================
// function : ContainsFacet
// purpose  :
// ========================================================================
Standard_Boolean Visual3d_View::ContainsFacet (const Graphic3d_MapOfStructure& theSet) const
{
  for (Graphic3d_MapIteratorOfMapOfStructure aStructIter (theSet); aStructIter.More(); aStructIter.Next())
  {
    if (aStructIter.Key()->ContainsFacet())
    {
      return Standard_True;
    }
  }
  return Standard_False;
}

// ========================================================================
// function : MinMaxValues
// purpose  :
// ========================================================================
void Visual3d_View::MinMaxValues (Standard_Real& theXMin,
                                  Standard_Real& theYMin,
                                  Standard_Real& theZMin,
                                  Standard_Real& theXMax,
                                  Standard_Real& theYMax,
                                  Standard_Real& theZMax,
                                  const Standard_Boolean theToIgnoreInfiniteFlag) const
{
  MinMaxValues (myStructsDisplayed,
                theXMin, theYMin, theZMin,
                theXMax, theYMax, theZMax,
                theToIgnoreInfiniteFlag);
}

// ========================================================================
// function : MinMaxValues
// purpose  :
// ========================================================================
void Visual3d_View::MinMaxValues (const Graphic3d_MapOfStructure& theSet,
                                  Standard_Real& theXMin,
                                  Standard_Real& theYMin,
                                  Standard_Real& theZMin,
                                  Standard_Real& theXMax,
                                  Standard_Real& theYMax,
                                  Standard_Real& theZMax,
                                  const Standard_Boolean theToIgnoreInfiniteFlag) const
{
  if (theSet.IsEmpty ())
  {
    theXMin = RealFirst();
    theYMin = RealFirst();
    theZMin = RealFirst();

    theXMax = RealLast();
    theYMax = RealLast();
    theZMax = RealLast();
  }
  else
  {
    Standard_Real aXm, aYm, aZm, aXM, aYM, aZM;
    Graphic3d_MapIteratorOfMapOfStructure anIterator (theSet);

    theXMin = RealLast();
    theYMin = RealLast();
    theZMin = RealLast();

    theXMax = RealFirst ();
    theYMax = RealFirst ();
    theZMax = RealFirst ();

    for (anIterator.Initialize (theSet); anIterator.More(); anIterator.Next())
    {
      const Handle(Graphic3d_Structure)& aStructure = anIterator.Key();

      if (!aStructure->IsVisible())
        continue;

      if (aStructure->IsInfinite() && !theToIgnoreInfiniteFlag)
      {
        //XMin, YMin .... ZMax are initialized by means of infinite line data
        aStructure->MinMaxValues (aXm, aYm, aZm, aXM, aYM, aZM, Standard_False);
        if (aXm != RealFirst() && aXm < theXMin)
        {
          theXMin = aXm;
        }
        if (aYm != RealFirst() && aYm < theYMin)
        {
          theYMin = aYm;
        }
        if (aZm != RealFirst() && aZm < theZMin)
        {
          theZMin = aZm;
        }
        if (aXM != RealLast() && aXM > theXMax)
        {
          theXMax = aXM;
        }
        if (aYM != RealLast() && aYM > theYMax)
        {
          theYMax = aYM;
        }
        if (aZM != RealLast() && aZM > theZMax)
        {
          theZMax = aZM;
        }
      }

      // Only non-empty and non-infinite structures
      // are taken into account for calculation of MinMax
      if ((!aStructure->IsInfinite() || theToIgnoreInfiniteFlag) && !aStructure->IsEmpty())
      {
        aStructure->MinMaxValues (aXm, aYm, aZm, aXM, aYM, aZM, theToIgnoreInfiniteFlag);

        /* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */
        //"FitAll" operation ignores object with transform persitence parameter
        if(aStructure->TransformPersistenceMode() == Graphic3d_TMF_None )
        {
          theXMin = Min (aXm, theXMin);
          theYMin = Min (aYm, theYMin);
          theZMin = Min (aZm, theZMin);
          theXMax = Max (aXM, theXMax);
          theYMax = Max (aYM, theYMax);
          theZMax = Max (aZM, theZMax);
        }
      }
    }

    // The following cases are relevant
    // For exemple if all structures are empty or infinite
    if (theXMax < theXMin) { aXm = theXMin; theXMin = theXMax; theXMax = aXm; }
    if (theYMax < theYMin) { aYm = theYMin; theYMin = theYMax; theYMax = aYm; }
    if (theZMax < theZMin) { aZm = theZMin; theZMin = theZMax; theZMax = aZm; }
  }
}

// ========================================================================
// function : MinMaxValues
// purpose  :
// ========================================================================
void Visual3d_View::MinMaxValues (Standard_Real& theXMin,
                                  Standard_Real& theYMin,
                                  Standard_Real& theXMax,
                                  Standard_Real& theYMax,
                                  const Standard_Boolean theToIgnoreInfiniteFlag) const
{
  MinMaxValues (myStructsDisplayed,
                theXMin, theYMin,
                theXMax, theYMax,
                theToIgnoreInfiniteFlag);
}

// ========================================================================
// function : MinMaxValues
// purpose  :
// ========================================================================
void Visual3d_View::MinMaxValues (const Graphic3d_MapOfStructure& theSet,
                                  Standard_Real& theXMin,
                                  Standard_Real& theYMin,
                                  Standard_Real& theXMax,
                                  Standard_Real& theYMax,
                                  const Standard_Boolean theToIgnoreInfiniteFlag) const
{
  Standard_Real aXm, aYm, aZm, aXM, aYM, aZM;
  Standard_Real aXp, aYp, aZp;

  MinMaxValues (theSet, aXm, aYm, aZm, aXM, aYM, aZM, theToIgnoreInfiniteFlag);

  Projects (aXm, aYm, aZm, aXp, aYp, aZp);
  theXMin = aXp;
  theYMin = aYp;

  Projects (aXM, aYM, aZM, aXp, aYp, aZp);
  theXMax = aXp;
  theYMax = aYp;

  if (theXMax < theXMin) { aXp = theXMax; theXMax = theXMin; theXMin = aXp; }
  if (theYMax < theYMin) { aYp = theYMax; theYMax = theYMin; theYMin = aYp; }
}

// =======================================================================
// function : NumberOfDisplayedStructures
// purpose  :
// =======================================================================
Standard_Integer Visual3d_View::NumberOfDisplayedStructures() const
{
  return myStructsDisplayed.Extent();
}

// =======================================================================
// function : Projects
// purpose  :
// =======================================================================
void Visual3d_View::Projects (const Standard_Real theX,
                              const Standard_Real theY,
                              const Standard_Real theZ,
                              Standard_Real& thePX,
                              Standard_Real& thePY,
                              Standard_Real& thePZ) const
{
  const Handle(Graphic3d_Camera)& aCamera = MyCView.Context.Camera;

  gp_XYZ aViewSpaceDimensions = aCamera->ViewDimensions();
  Standard_Real aXSize = aViewSpaceDimensions.X();
  Standard_Real aYSize = aViewSpaceDimensions.Y();
  Standard_Real aZSize = aViewSpaceDimensions.Z();

  gp_Pnt aPoint = aCamera->Project (gp_Pnt (theX, theY, theZ));

  // NDC [-1, 1] --> PROJ [ -size / 2, +size / 2 ]
  thePX = aPoint.X() * aXSize * 0.5;
  thePY = aPoint.Y() * aYSize * 0.5;
  thePZ = aPoint.Z() * aZSize * 0.5;
}

// =======================================================================
// function : Identification
// purpose  :
// =======================================================================
Standard_Integer Visual3d_View::Identification() const
{
  return MyCView.ViewId;
}

// =======================================================================
// function : ZBufferIsActivated
// purpose  :
// =======================================================================
Standard_Boolean Visual3d_View::ZBufferIsActivated() const
{
  if (IsDeleted()
  || !IsDefined()
  || !IsActive())
  {
    return Standard_False;
  }

  if (MyCView.Context.ZBufferActivity == -1)
  {
    // not forced by the programmer => depends on the type of visualisation
    return MyContext.Visualization () == Visual3d_TOV_SHADING;
  }
  return MyCView.Context.ZBufferActivity != 0; //  0 or 1 => forced by the programmer
}

// =======================================================================
// function : SetTransparency
// purpose  :
// =======================================================================
void Visual3d_View::SetTransparency (const Standard_Boolean theActivity)
{
  if (IsDeleted()
  || !IsDefined()
  || !IsActive())
  {
    return;
  }

  myGraphicDriver->Transparency (MyCView, theActivity);
}

// =======================================================================
// function : SetZBufferActivity
// purpose  :
// =======================================================================
void Visual3d_View::SetZBufferActivity (const Standard_Integer theActivity)
{
  if (IsDeleted()
  ||  MyCView.Context.ZBufferActivity == theActivity
  || !IsDefined()
  || !IsActive())
  {
    return;
  }

  MyCView.Context.ZBufferActivity = theActivity;
  myGraphicDriver->SetVisualisation (MyCView);
}

// =======================================================================
// function : UpdateView
// purpose  :
// =======================================================================
void Visual3d_View::UpdateView()
{
  MyCView.Context.Aliasing        = MyContext.AliasingIsOn();
  MyCView.Context.BackZClipping   = MyContext.BackZClippingIsOn();
  MyCView.Context.FrontZClipping  = MyContext.FrontZClippingIsOn();
  MyCView.Context.DepthCueing     = MyContext.DepthCueingIsOn();

  MyCView.Context.ZClipFrontPlane = float (MyContext.ZClippingFrontPlane());
  MyCView.Context.ZClipBackPlane  = float (MyContext.ZClippingBackPlane());
  MyCView.Context.DepthFrontPlane = float (MyContext.DepthCueingFrontPlane());
  MyCView.Context.DepthBackPlane  = float (MyContext.DepthCueingBackPlane());

  MyCView.Context.Model           = MyContext.Model();
  MyCView.Context.Visualization   = MyContext.Visualization();

  MyCView.Context.TextureEnv      = MyContext.TextureEnv();
  MyCView.Context.SurfaceDetail   = MyContext.SurfaceDetail();
}

// =======================================================================
// function : Compute
// purpose  :
// =======================================================================
void Visual3d_View::Compute()
{
  // force HLRValidation to False on all structures calculated in the view
  const Standard_Integer aNbCompStructs = myStructsComputed.Length();
  for (Standard_Integer aStructIter = 1; aStructIter <= aNbCompStructs; ++aStructIter)
  {
    myStructsComputed.Value (aStructIter)->SetHLRValidation (Standard_False);
  }

  if (!ComputedMode())
  {
    return;
  }

  // Change of orientation or of projection type =>
  // Remove structures that were calculated for the previous orientation.
  // Recalculation of new structures.
  NCollection_Sequence<Handle(Graphic3d_Structure)> aStructsSeq;
  for (Graphic3d_MapIteratorOfMapOfStructure aStructIter (myStructsDisplayed); aStructIter.More(); aStructIter.Next())
  {
    const Visual3d_TypeOfAnswer anAnswer = AcceptDisplay (aStructIter.Key());
    if (anAnswer == Visual3d_TOA_COMPUTE)
    {
      aStructsSeq.Append (aStructIter.Key()); // if the structure was calculated, it is recalculated
    }
  }

  for (NCollection_Sequence<Handle(Graphic3d_Structure)>::Iterator aStructIter (aStructsSeq); aStructIter.More(); aStructIter.Next())
  {
    Display (aStructIter.ChangeValue(), Aspect_TOU_WAIT);
  }
}

// =======================================================================
// function : ReCompute
// purpose  :
// =======================================================================
void Visual3d_View::ReCompute (const Handle(Graphic3d_Structure)& theStruct)
{
  theStruct->CalculateBoundBox();
  if (!theStruct->IsMutable()
   && !theStruct->CStructure()->IsForHighlight
   && !theStruct->CStructure()->IsInfinite)
  {
    const Standard_Integer aLayerId = theStruct->DisplayPriority();
    myGraphicDriver->InvalidateBVHData(MyCView, aLayerId);
  }

  if (!ComputedMode()
   ||  IsDeleted()
   || !IsDefined()
   || !IsActive()
   || !MyWindow->IsMapped()
   || !theStruct->IsDisplayed())
  {
    return;
  }

  const Visual3d_TypeOfAnswer anAnswer = AcceptDisplay (theStruct);
  if (anAnswer != Visual3d_TOA_COMPUTE)
  {
    return;
  }

  const Standard_Integer anIndex = IsComputed (theStruct);
  if (anIndex == 0)
  {
    return;
  }

  // compute + validation
  TColStd_Array2OfReal anIdent (0, 3, 0, 3);
  for (Standard_Integer aRow = 0; aRow <= 3; ++aRow)
  {
    for (Standard_Integer aCol = 0; aCol <= 3; ++aCol)
    {
      anIdent (aRow, aCol) = (aRow == aCol ? 1.0 : 0.0);
    }
  }
  TColStd_Array2OfReal aTrsf (0, 3, 0, 3);
  theStruct->Transform (aTrsf);

  Handle(Graphic3d_Structure) aCompStructOld = myStructsComputed.ChangeValue (anIndex);
  Handle(Graphic3d_Structure) aCompStruct    = aCompStructOld;
  aCompStruct->SetTransform (anIdent, Graphic3d_TOC_REPLACE);
  theStruct->IsTransformed() ? theStruct->Compute (this, aTrsf, aCompStruct)
                             : theStruct->Compute (this,        aCompStruct);
  aCompStruct->SetHLRValidation (Standard_True);

  // of which type will be the computed?
  const Visual3d_TypeOfVisualization aViewType = MyContext.Visualization();
  const Standard_Boolean toComputeWireframe = aViewType == Visual3d_TOV_WIREFRAME
                                           && theStruct->ComputeVisual() != Graphic3d_TOS_SHADING;
  const Standard_Boolean toComputeShading   = aViewType == Visual3d_TOV_SHADING
                                           && theStruct->ComputeVisual() != Graphic3d_TOS_WIREFRAME;
  if (toComputeWireframe)
  {
    aCompStruct->SetVisual (Graphic3d_TOS_WIREFRAME);
  }
  else if (toComputeShading)
  {
    aCompStruct->SetVisual (Graphic3d_TOS_SHADING);
  }

  if (theStruct->IsHighlighted())
  {
    aCompStruct->SetHighlightColor (theStruct->HighlightColor());
    aCompStruct->GraphicHighlight (Aspect_TOHM_COLOR);
  }

  // The previous calculation is removed and the new one is dislayed
  myGraphicDriver->EraseStructure   (MyCView, *aCompStructOld->CStructure());
  myGraphicDriver->DisplayStructure (MyCView, *aCompStruct->CStructure(), theStruct->DisplayPriority());

  // why not just replace existing items?
  //myStructsToCompute.ChangeValue (anIndex) = theStruct;
  //myStructsComputed .ChangeValue (anIndex) = aCompStruct;

  // hlhsr and the new associated compute are added
  myStructsToCompute.Append (theStruct);
  myStructsComputed .Append (aCompStruct);

  // hlhsr and the new associated compute are removed
  myStructsToCompute.Remove (anIndex);
  myStructsComputed .Remove (anIndex);
}

// =======================================================================
// function : GraphicDriver
// purpose  :
// =======================================================================
const Handle(Graphic3d_GraphicDriver)& Visual3d_View::GraphicDriver() const
{
  return myGraphicDriver;
}

// =======================================================================
// function : HaveTheSameOwner
// purpose  :
// =======================================================================
Standard_Integer Visual3d_View::HaveTheSameOwner (const Handle(Graphic3d_Structure)& theStruct) const
{
  // Find in the sequence of already calculated structures
  // 1/ Structure with the same Owner as <AStructure>
  // 2/ Which is not <AStructure>
  // 3/ COMPUTED which of is valid
  const Standard_Integer aNbToCompStructs = myStructsToCompute.Length();
  for (Standard_Integer aStructIter = 1; aStructIter <= aNbToCompStructs; ++aStructIter)
  {
    const Handle(Graphic3d_Structure)& aStructToComp = myStructsToCompute.Value (aStructIter);
    if (aStructToComp->Owner()          == theStruct->Owner()
     && aStructToComp->Identification() != theStruct->Identification())
    {
      const Handle(Graphic3d_Structure)& aStructComp = myStructsComputed.Value (aStructIter);
      if (aStructComp->HLRValidation())
      {
        return aStructIter;
      }
    }
  }
  return 0;
}

// =======================================================================
// function : CView
// purpose  :
// =======================================================================
Standard_Address Visual3d_View::CView() const
{
  return Standard_Address (&MyCView);
}

// =======================================================================
// function : ZBufferTriedronSetup
// purpose  :
// =======================================================================
void Visual3d_View::ZBufferTriedronSetup (const Quantity_NameOfColor theXColor,
                                          const Quantity_NameOfColor theYColor,
                                          const Quantity_NameOfColor theZColor,
                                          const Standard_Real        theSizeRatio,
                                          const Standard_Real        theAxisDiametr,
                                          const Standard_Integer     theNbFacettes)
{
  myGraphicDriver->ZBufferTriedronSetup (theXColor, theYColor, theZColor,
                                         theSizeRatio, theAxisDiametr, theNbFacettes);
}

// =======================================================================
// function : TriedronDisplay
// purpose  :
// =======================================================================
void Visual3d_View::TriedronDisplay (const Aspect_TypeOfTriedronPosition thePosition,
                                     const Quantity_NameOfColor          theColor,
                                     const Standard_Real                 theScale,
                                     const Standard_Boolean              theAsWireframe)
{
  myGraphicDriver->TriedronDisplay (MyCView, thePosition, theColor, theScale, theAsWireframe);
}

// =======================================================================
// function : TriedronErase
// purpose  :
// =======================================================================
void Visual3d_View::TriedronErase()
{
  myGraphicDriver->TriedronErase (MyCView);
}

// =======================================================================
// function : TriedronEcho
// purpose  :
// =======================================================================
void Visual3d_View::TriedronEcho (const Aspect_TypeOfTriedronEcho theType)
{
  myGraphicDriver->TriedronEcho (MyCView, theType);
}

static Standard_Boolean checkFloat (const Standard_Real theValue)
{
  return theValue > -FLT_MAX
      && theValue <  FLT_MAX;
}

static void SetMinMaxValuesCallback (Visual3d_View* theView)
{
  Graphic3d_Vec3d aMin, aMax;
  theView->MinMaxValues (aMin.x(), aMin.y(), aMin.z(),
                         aMax.x(), aMax.y(), aMax.z());
  if (checkFloat (aMin.x()) && checkFloat (aMin.y()) && checkFloat (aMin.z())
   && checkFloat (aMax.x()) && checkFloat (aMax.y()) && checkFloat (aMax.z()))
  {
    const Handle(Graphic3d_GraphicDriver)& aDriver = theView->GraphicDriver();
    aDriver->GraduatedTrihedronMinMaxValues ((Standard_ShortReal )aMin.x(), (Standard_ShortReal )aMin.y(), (Standard_ShortReal )aMin.z(),
                                             (Standard_ShortReal )aMax.x(), (Standard_ShortReal )aMax.y(), (Standard_ShortReal )aMax.z());
  }
}

// =======================================================================
// function : GetGraduatedTrihedron
// purpose  :
// =======================================================================
Standard_Boolean Visual3d_View::GetGraduatedTrihedron (TCollection_ExtendedString& theXName,
                                                       TCollection_ExtendedString& theYName,
                                                       TCollection_ExtendedString& theZName,
                                                       Standard_Boolean&           theToDrawXName,
                                                       Standard_Boolean&           theToDrawYName,
                                                       Standard_Boolean&           theToDrawZName,
                                                       Standard_Boolean&           theToDrawXValues,
                                                       Standard_Boolean&           theToDrawYValues,
                                                       Standard_Boolean&           theToDrawZValues,
                                                       Standard_Boolean&           theToDrawGrid,
                                                       Standard_Boolean&           theToDrawAxes,
                                                       Standard_Integer&           theNbX,
                                                       Standard_Integer&           theNbY,
                                                       Standard_Integer&           theNbZ,
                                                       Standard_Integer&           theXOffset,
                                                       Standard_Integer&           theYOffset,
                                                       Standard_Integer&           theZOffset,
                                                       Standard_Integer&           theXAxisOffset,
                                                       Standard_Integer&           theYAxisOffset,
                                                       Standard_Integer&           theZAxisOffset,
                                                       Standard_Boolean&           theToDrawXTickMarks,
                                                       Standard_Boolean&           theToDrawYTickMarks,
                                                       Standard_Boolean&           theToDrawZTickMarks,
                                                       Standard_Integer&           theXTickMarkLength,
                                                       Standard_Integer&           theYTickMarkLength,
                                                       Standard_Integer&           theZTickMarkLength,
                                                       Quantity_Color&             theGridColor,
                                                       Quantity_Color&             theXNameColor,
                                                       Quantity_Color&             theYNameColor,
                                                       Quantity_Color&             theZNameColor,
                                                       Quantity_Color&             theXColor,
                                                       Quantity_Color&             theYColor,
                                                       Quantity_Color&             theZColor,
                                                       TCollection_AsciiString&    theFontOfNames,
                                                       Font_FontAspect&            theStyleOfNames,
                                                       Standard_Integer&           theSizeOfNames,
                                                       TCollection_AsciiString&    theFontOfValues,
                                                       Font_FontAspect&            theStyleOfValues,
                                                       Standard_Integer&           theSizeOfValues) const
{
  if (!MyGTrihedron.ptrVisual3dView)
  {
    return Standard_False;
  }

  theXName = MyGTrihedron.xname;
  theYName = MyGTrihedron.yname;
  theZName = MyGTrihedron.zname;
  theToDrawXName   = MyGTrihedron.xdrawname;
  theToDrawYName   = MyGTrihedron.ydrawname;
  theToDrawZName   = MyGTrihedron.zdrawname;
  theToDrawXValues = MyGTrihedron.xdrawvalues;
  theToDrawYValues = MyGTrihedron.ydrawvalues;
  theToDrawZValues = MyGTrihedron.zdrawvalues;
  theToDrawGrid    = MyGTrihedron.drawgrid;
  theToDrawAxes    = MyGTrihedron.drawaxes;
  theNbX = MyGTrihedron.nbx;
  theNbY = MyGTrihedron.nby;
  theNbZ = MyGTrihedron.nbz;
  theXOffset     = MyGTrihedron.xoffset;
  theYOffset     = MyGTrihedron.yoffset;
  theZOffset     = MyGTrihedron.zoffset;
  theXAxisOffset = MyGTrihedron.xaxisoffset;
  theYAxisOffset = MyGTrihedron.yaxisoffset;
  theZAxisOffset = MyGTrihedron.zaxisoffset;
  theToDrawXTickMarks = MyGTrihedron.xdrawtickmarks;
  theToDrawYTickMarks = MyGTrihedron.ydrawtickmarks;
  theToDrawZTickMarks = MyGTrihedron.zdrawtickmarks;
  theXTickMarkLength  = MyGTrihedron.xtickmarklength;
  theYTickMarkLength  = MyGTrihedron.ytickmarklength;
  theZTickMarkLength  = MyGTrihedron.ztickmarklength;
  theGridColor  = MyGTrihedron.gridcolor;
  theXNameColor = MyGTrihedron.xnamecolor;
  theYNameColor = MyGTrihedron.ynamecolor;
  theZNameColor = MyGTrihedron.znamecolor;
  theXColor     = MyGTrihedron.xcolor;
  theYColor     = MyGTrihedron.ycolor;
  theZColor     = MyGTrihedron.zcolor;
  theFontOfNames   = MyGTrihedron.fontOfNames;
  theStyleOfNames  = MyGTrihedron.styleOfNames;
  theSizeOfNames   = MyGTrihedron.sizeOfNames;
  theFontOfValues  = MyGTrihedron.fontOfValues;
  theStyleOfValues = MyGTrihedron.styleOfValues;
  theSizeOfValues  = MyGTrihedron.sizeOfValues;
  return Standard_True;
}

// =======================================================================
// function : GraduatedTrihedronDisplay
// purpose  :
// =======================================================================
void Visual3d_View::GraduatedTrihedronDisplay (const TCollection_ExtendedString& theXName,
                                               const TCollection_ExtendedString& theYName,
                                               const TCollection_ExtendedString& theZName,
                                               const Standard_Boolean theToDrawXName,
                                               const Standard_Boolean theToDrawYName,
                                               const Standard_Boolean theToDrawZName,
                                               const Standard_Boolean theToDrawXValues,
                                               const Standard_Boolean theToDrawYValues,
                                               const Standard_Boolean theToDrawZValues,
                                               const Standard_Boolean theToDrawGrid,
                                               const Standard_Boolean theToDrawAxes,
                                               const Standard_Integer theNbX,
                                               const Standard_Integer theNbY,
                                               const Standard_Integer theNbZ,
                                               const Standard_Integer theXOffset,
                                               const Standard_Integer theYOffset,
                                               const Standard_Integer theZOffset,
                                               const Standard_Integer theXAxisOffset,
                                               const Standard_Integer theYAxisOffset,
                                               const Standard_Integer theZAxisOffset,
                                               const Standard_Boolean theToDrawXTickMarks,
                                               const Standard_Boolean theToDrawYTickMarks,
                                               const Standard_Boolean theToDrawZTickMarks,
                                               const Standard_Integer theXTickMarkLength,
                                               const Standard_Integer theYTickMarkLength,
                                               const Standard_Integer theZTickMarkLength,
                                               const Quantity_Color&  theGridColor,
                                               const Quantity_Color&  theXNameColor,
                                               const Quantity_Color&  theYNameColor,
                                               const Quantity_Color&  theZNameColor,
                                               const Quantity_Color&  theXColor,
                                               const Quantity_Color&  theYColor,
                                               const Quantity_Color&  theZColor,
                                               const TCollection_AsciiString& theFontOfNames,
                                               const Font_FontAspect  theStyleOfNames,
                                               const Standard_Integer theSizeOfNames,
                                               const TCollection_AsciiString& theFontOfValues,
                                               const Font_FontAspect  theStyleOfValues,
                                               const Standard_Integer theSizeOfValues)
{
  MyGTrihedron.xname = theXName;
  MyGTrihedron.yname = theYName;
  MyGTrihedron.zname = theZName;
  MyGTrihedron.xdrawname = theToDrawXName;
  MyGTrihedron.ydrawname = theToDrawYName;
  MyGTrihedron.zdrawname = theToDrawZName;
  MyGTrihedron.xdrawvalues = theToDrawXValues;
  MyGTrihedron.ydrawvalues = theToDrawYValues;
  MyGTrihedron.zdrawvalues = theToDrawZValues;
  MyGTrihedron.drawgrid = theToDrawGrid;
  MyGTrihedron.drawaxes = theToDrawAxes;
  MyGTrihedron.nbx = theNbX;
  MyGTrihedron.nby = theNbY;
  MyGTrihedron.nbz = theNbZ;
  MyGTrihedron.xoffset = theXOffset;
  MyGTrihedron.yoffset = theYOffset;
  MyGTrihedron.zoffset = theZOffset;
  MyGTrihedron.xaxisoffset = theXAxisOffset;
  MyGTrihedron.yaxisoffset = theYAxisOffset;
  MyGTrihedron.zaxisoffset = theZAxisOffset;
  MyGTrihedron.xdrawtickmarks = theToDrawXTickMarks;
  MyGTrihedron.ydrawtickmarks = theToDrawYTickMarks;
  MyGTrihedron.zdrawtickmarks = theToDrawZTickMarks;
  MyGTrihedron.xtickmarklength = theXTickMarkLength;
  MyGTrihedron.ytickmarklength = theYTickMarkLength;
  MyGTrihedron.ztickmarklength = theZTickMarkLength;
  MyGTrihedron.gridcolor  = theGridColor;
  MyGTrihedron.xnamecolor = theXNameColor;
  MyGTrihedron.ynamecolor = theYNameColor;
  MyGTrihedron.znamecolor = theZNameColor;
  MyGTrihedron.xcolor = theXColor;
  MyGTrihedron.ycolor = theYColor;
  MyGTrihedron.zcolor = theZColor;
  MyGTrihedron.fontOfNames   = theFontOfNames;
  MyGTrihedron.styleOfNames  = theStyleOfNames;
  MyGTrihedron.sizeOfNames   = theSizeOfNames;
  MyGTrihedron.fontOfValues  = theFontOfValues;
  MyGTrihedron.styleOfValues = theStyleOfValues;
  MyGTrihedron.sizeOfValues  = theSizeOfValues;

  MyGTrihedron.ptrVisual3dView = this;
  MyGTrihedron.cbCubicAxes     = SetMinMaxValuesCallback;
  myGraphicDriver->GraduatedTrihedronDisplay (MyCView, MyGTrihedron);
}

// =======================================================================
// function : GraduatedTrihedronErase
// purpose  :
// =======================================================================
void Visual3d_View::GraduatedTrihedronErase()
{
  MyGTrihedron.ptrVisual3dView = NULL;
  myGraphicDriver->GraduatedTrihedronErase (MyCView);
}

// =======================================================================
// function : UnderLayer
// purpose  :
// =======================================================================
const Handle(Visual3d_Layer)& Visual3d_View::UnderLayer() const
{
  return myViewManager->UnderLayer();
}

// =======================================================================
// function : OverLayer
// purpose  :
// =======================================================================
const Handle(Visual3d_Layer)& Visual3d_View::OverLayer() const
{
  return myViewManager->OverLayer();
}

// =======================================================================
// function : LightLimit
// purpose  :
// =======================================================================
Standard_Integer Visual3d_View::LightLimit() const
{
  return myGraphicDriver->InquireLightLimit();
}

// =======================================================================
// function : PlaneLimit
// purpose  :
// =======================================================================
Standard_Integer Visual3d_View::PlaneLimit() const
{
  return myGraphicDriver->InquirePlaneLimit();
}

// =======================================================================
// function : ViewManager
// purpose  :
// =======================================================================
Handle(Visual3d_ViewManager) Visual3d_View::ViewManager() const
{
  return myViewManager;
}

// =======================================================================
// function : SetComputedMode
// purpose  :
// =======================================================================
void Visual3d_View::SetComputedMode (const Standard_Boolean theMode)
{
  if (( theMode &&  myIsInComputedMode)
   || (!theMode && !myIsInComputedMode))
  {
    return;
  }

  myIsInComputedMode = theMode;
  if (!myIsInComputedMode)
  {
    for (Graphic3d_MapIteratorOfMapOfStructure aStructIter (myStructsDisplayed); aStructIter.More(); aStructIter.Next())
    {
      const Handle(Graphic3d_Structure)& aStruct  = aStructIter.Key();
      const Visual3d_TypeOfAnswer        anAnswer = AcceptDisplay (aStruct);
      if (anAnswer != Visual3d_TOA_COMPUTE)
      {
        continue;
      }

      const Standard_Integer anIndex = IsComputed (aStruct);
      if (anIndex != 0)
      {
        const Handle(Graphic3d_Structure)& aStructComp = myStructsComputed.Value (anIndex);
        myGraphicDriver->EraseStructure   (MyCView, *aStructComp->CStructure());
        myGraphicDriver->DisplayStructure (MyCView, *aStruct->CStructure(), aStruct->DisplayPriority());
      }
    }
    return;
  }

  for (Graphic3d_MapIteratorOfMapOfStructure aDispStructIter (myStructsDisplayed); aDispStructIter.More(); aDispStructIter.Next())
  {
    Handle(Graphic3d_Structure) aStruct = aDispStructIter.Key();
    const Visual3d_TypeOfAnswer anAnswer = AcceptDisplay (aStruct);
    if (anAnswer != Visual3d_TOA_COMPUTE)
    {
      continue;
    }

    const Standard_Integer anIndex = IsComputed (aStruct);
    if (anIndex != 0)
    {
      myGraphicDriver->EraseStructure   (MyCView, *aStruct->CStructure());
      myGraphicDriver->DisplayStructure (MyCView, *(myStructsComputed.Value (anIndex)->CStructure()), aStruct->DisplayPriority());

      Display (aStruct, Aspect_TOU_WAIT);
      if (aStruct->IsHighlighted())
      {
        const Handle(Graphic3d_Structure)& aCompStruct = myStructsComputed.Value (anIndex);
        if (!aCompStruct->IsHighlighted())
        {
          aCompStruct->SetHighlightColor (aStruct->HighlightColor());
          aCompStruct->GraphicHighlight (Aspect_TOHM_COLOR);
        }
      }
    }
    else
    {
      TColStd_Array2OfReal aTrsf (0, 3, 0, 3);
      aStruct->Transform (aTrsf);
      Handle(Graphic3d_Structure) aCompStruct = aStruct->IsTransformed() ? aStruct->Compute (this, aTrsf) : aStruct->Compute (this);
      aCompStruct->SetHLRValidation (Standard_True);

      const Visual3d_TypeOfVisualization aViewType = MyContext.Visualization();
      const Standard_Boolean toComputeWireframe = aViewType == Visual3d_TOV_WIREFRAME
                                                && aStruct->ComputeVisual() != Graphic3d_TOS_SHADING;
      const Standard_Boolean toComputeShading   = aViewType == Visual3d_TOV_SHADING
                                                && aStruct->ComputeVisual() != Graphic3d_TOS_WIREFRAME;
      if (toComputeWireframe) aCompStruct->SetVisual (Graphic3d_TOS_WIREFRAME);
      if (toComputeShading  ) aCompStruct->SetVisual (Graphic3d_TOS_SHADING);

      if (aStruct->IsHighlighted())
      {
        aCompStruct->SetHighlightColor (aStruct->HighlightColor());
        aCompStruct->GraphicHighlight (Aspect_TOHM_COLOR);
      }

      Standard_Boolean hasResult = Standard_False;
      const Standard_Integer aNbToCompute = myStructsToCompute.Length();
      const Standard_Integer aStructId    = aStruct->Identification();
      for (Standard_Integer aToCompStructIter = 1; aToCompStructIter <= aNbToCompute; ++aToCompStructIter)
      {
        if (myStructsToCompute.Value (aToCompStructIter)->Identification() == aStructId)
        {
          hasResult = Standard_True;
          myStructsComputed.ChangeValue (aToCompStructIter) = aCompStruct;
          break;
        }
      }

      if (!hasResult)
      {
        myStructsToCompute.Append (aStruct);
        myStructsComputed .Append (aCompStruct);
      }

      myGraphicDriver->EraseStructure   (MyCView, *aStruct->CStructure());
      myGraphicDriver->DisplayStructure (MyCView, *aCompStruct->CStructure(), aStruct->DisplayPriority());
    }
  }
  Update (myViewManager->UpdateMode());
}

// =======================================================================
// function : ComputedMode
// purpose  :
// =======================================================================
Standard_Boolean Visual3d_View::ComputedMode() const
{
  return myIsInComputedMode;
}

// =======================================================================
// function : SetBackFacingModel
// purpose  :
// =======================================================================
void Visual3d_View::SetBackFacingModel (const Visual3d_TypeOfBackfacingModel theModel)
{
  switch (theModel)
  {
    default:
    case Visual3d_TOBM_AUTOMATIC:
      MyCView.Backfacing = 0;
      break;
    case Visual3d_TOBM_FORCE:
      MyCView.Backfacing = 1;
      break;
    case Visual3d_TOBM_DISABLE:
      MyCView.Backfacing = -1;
      break;
  }
  myGraphicDriver->SetBackFacingModel (MyCView);
}

// =======================================================================
// function : BackFacingModel
// purpose  :
// =======================================================================
Visual3d_TypeOfBackfacingModel Visual3d_View::BackFacingModel() const
{
  switch (MyCView.Backfacing)
  {
    case 0: return Visual3d_TOBM_AUTOMATIC;
    case 1: return Visual3d_TOBM_FORCE;
  }
  return Visual3d_TOBM_DISABLE;
}

// =======================================================================
// function : EnableDepthTest
// purpose  :
// =======================================================================
void Visual3d_View::EnableDepthTest (const Standard_Boolean theToEnable) const
{
  myGraphicDriver->SetDepthTestEnabled (MyCView, theToEnable);
}

// =======================================================================
// function : IsDepthTestEnabled
// purpose  :
// =======================================================================
Standard_Boolean Visual3d_View::IsDepthTestEnabled() const
{
  return myGraphicDriver->IsDepthTestEnabled (MyCView);
}

// =======================================================================
// function : ReadDepths
// purpose  :
// =======================================================================
void Visual3d_View::ReadDepths (const Standard_Integer theX,
                                const Standard_Integer theY,
                                const Standard_Integer theWidth,
                                const Standard_Integer theHeight,
                                const Standard_Address theBuffer) const
{
  myGraphicDriver->ReadDepths (MyCView, theX, theY, theWidth, theHeight, theBuffer);
}

// =======================================================================
// function : FBOCreate
// purpose  :
// =======================================================================
Graphic3d_PtrFrameBuffer Visual3d_View::FBOCreate(const Standard_Integer theWidth,
                                                  const Standard_Integer theHeight)
{
  return myGraphicDriver->FBOCreate( MyCView, theWidth, theHeight );
}

// =======================================================================
// function : FBORelease
// purpose  :
// =======================================================================
void Visual3d_View::FBORelease(Graphic3d_PtrFrameBuffer& theFBOPtr)
{
  myGraphicDriver->FBORelease( MyCView, theFBOPtr );
}

// =======================================================================
// function : FBOGetDimensions
// purpose  :
// =======================================================================
void Visual3d_View::FBOGetDimensions(const Graphic3d_PtrFrameBuffer theFBOPtr,
                                     Standard_Integer& theWidth,    Standard_Integer& theHeight,
                                     Standard_Integer& theWidthMax, Standard_Integer& theHeightMax)
{
  myGraphicDriver->FBOGetDimensions( MyCView, theFBOPtr,
                                     theWidth, theHeight,
                                     theWidthMax, theHeightMax );
}

// =======================================================================
// function : FBOChangeViewport
// purpose  :
// =======================================================================
void Visual3d_View::FBOChangeViewport(Graphic3d_PtrFrameBuffer& theFBOPtr,
                                      const Standard_Integer theWidth, const Standard_Integer theHeight)
{
  myGraphicDriver->FBOChangeViewport( MyCView, theFBOPtr,
                                     theWidth, theHeight );
}

// =======================================================================
// function : BufferDump
// purpose  :
// =======================================================================
Standard_Boolean Visual3d_View::BufferDump (Image_PixMap&               theImage,
                                            const Graphic3d_BufferType& theBufferType)
{
  return myGraphicDriver->BufferDump (MyCView, theImage, theBufferType);
}

// =======================================================================
// function : EnableGLLight
// purpose  :
// =======================================================================
void Visual3d_View::EnableGLLight( const Standard_Boolean enable ) const
{
  myGraphicDriver->SetGLLightEnabled( MyCView, enable );
}

// =======================================================================
// function : IsGLLightEnabled
// purpose  :
// =======================================================================
Standard_Boolean Visual3d_View::IsGLLightEnabled() const
{
  return myGraphicDriver->IsGLLightEnabled( MyCView );
}

// =======================================================================
// function : Export
// purpose  :
// =======================================================================
Standard_Boolean Visual3d_View::Export (const Standard_CString       theFileName,
                                        const Graphic3d_ExportFormat theFormat,
                                        const Graphic3d_SortType     theSortType,
                                        const Standard_Real          thePrecision,
                                        const Standard_Address       theProgressBarFunc,
                                        const Standard_Address       theProgressObject) const
{
  Handle(Visual3d_Layer) anUnderLayer = myViewManager->UnderLayer();
  Handle(Visual3d_Layer) anOverLayer  = myViewManager->OverLayer();

  Aspect_CLayer2d anOverCLayer;
  Aspect_CLayer2d anUnderCLayer;
  anOverCLayer.ptrLayer = anUnderCLayer.ptrLayer = NULL;

  if (!anOverLayer.IsNull())
    anOverCLayer = anOverLayer->CLayer();
  if (!anUnderLayer.IsNull())
    anUnderCLayer = anUnderLayer->CLayer();

  Standard_Integer aWidth, aHeight;
  Window()->Size (aWidth, aHeight);

  return myGraphicDriver->Export (theFileName, theFormat, theSortType,
                                  aWidth, aHeight, MyCView, anUnderCLayer, anOverCLayer,
                                  thePrecision, theProgressBarFunc, theProgressObject);
}

// =======================================================================
// function : SetZLayerSettings
// purpose  :
// =======================================================================
void Visual3d_View::SetZLayerSettings (const Standard_Integer theLayerId,
                                       const Graphic3d_ZLayerSettings& theSettings)
{
  myGraphicDriver->SetZLayerSettings (MyCView, theLayerId, theSettings);
}

// =======================================================================
// function : AddZLayer
// purpose  :
// =======================================================================
void Visual3d_View::AddZLayer (const Standard_Integer theLayerId)
{
  myGraphicDriver->AddZLayer (MyCView, theLayerId);
}

// =======================================================================
// function : RemoveZLayer
// purpose  :
// =======================================================================
void Visual3d_View::RemoveZLayer (const Standard_Integer theLayerId)
{
  myGraphicDriver->RemoveZLayer (MyCView, theLayerId);
}

// =======================================================================
// function : ChangeZLayer
// purpose  :
// =======================================================================
void Visual3d_View::ChangeZLayer (const Handle(Graphic3d_Structure)& theStructure,
                                  const Standard_Integer theLayerId)
{
  myGraphicDriver->ChangeZLayer (*(theStructure->CStructure()), MyCView, theLayerId);
}

// =======================================================================
// function : Print
// purpose  :
// =======================================================================
Standard_Boolean Visual3d_View::Print (const Aspect_Handle    thePrintDC, 
                                       const Standard_Boolean theToShowBackground,
                                       const Standard_CString theFilename,
                                       const Aspect_PrintAlgo thePrintAlgorithm,
                                       const Standard_Real    theScaleFactor) const
{
  return Print (myViewManager->UnderLayer(),
                myViewManager->OverLayer(),
                thePrintDC, theToShowBackground,
                theFilename, thePrintAlgorithm, 
                theScaleFactor);
}

// =======================================================================
// function : Print
// purpose  :
// =======================================================================
Standard_Boolean Visual3d_View::Print (const Handle(Visual3d_Layer)& theUnderLayer,
                                       const Handle(Visual3d_Layer)& theOverLayer,
                                       const Aspect_Handle           thePrintDC,
                                       const Standard_Boolean        theToShowBackground,
                                       const Standard_CString        theFilename,
                                       const Aspect_PrintAlgo        thePrintAlgorithm,
                                       const Standard_Real           theScaleFactor) const
{
  if (IsDeleted()
  || !IsDefined()
  || !IsActive()
  || !MyWindow->IsMapped())
  {
    return Standard_False;
  }

  Aspect_CLayer2d anOverCLayer;
  Aspect_CLayer2d anUnderCLayer;
  anOverCLayer.ptrLayer = anUnderCLayer.ptrLayer = NULL;
  if (!theOverLayer.IsNull())  anOverCLayer  = theOverLayer->CLayer();
  if (!theUnderLayer.IsNull()) anUnderCLayer = theUnderLayer->CLayer();
  return myGraphicDriver->Print (MyCView, anUnderCLayer, anOverCLayer,
                                 thePrintDC, theToShowBackground, theFilename,
                                 thePrintAlgorithm, theScaleFactor);
}
