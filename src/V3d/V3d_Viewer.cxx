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

#include <V3d_Viewer.hxx>

#include <Aspect_Grid.hxx>
#include <Aspect_IdentDefinitionError.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Structure.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Type.hxx>
#include <V3d.hxx>
#include <V3d_BadValue.hxx>
#include <V3d_CircularGrid.hxx>
#include <V3d_AmbientLight.hxx>
#include <V3d_DirectionalLight.hxx>
#include <V3d_RectangularGrid.hxx>
#include <V3d_View.hxx>

IMPLEMENT_STANDARD_RTTIEXT(V3d_Viewer, Standard_Transient)

// ========================================================================
// function : V3d_Viewer
// purpose  :
// ========================================================================
V3d_Viewer::V3d_Viewer (const Handle(Graphic3d_GraphicDriver)& theDriver)
: myDriver (theDriver),
  myStructureManager (new Graphic3d_StructureManager (theDriver)),
  myZLayerGenId (1, IntegerLast()),
  myBackground (Quantity_NOC_GRAY30),
  myViewSize (1000.0),
  myViewProj (V3d_XposYnegZpos),
  myVisualization (V3d_ZBUFFER),
  myShadingModel (V3d_GOURAUD),
  myDefaultTypeOfView (V3d_ORTHOGRAPHIC),
  myComputedMode (Standard_True),
  myDefaultComputedMode (Standard_False),
  myPrivilegedPlane (gp_Ax3 (gp_Pnt (0.,0.,0), gp_Dir (0.,0.,1.), gp_Dir (1.,0.,0.))),
  myDisplayPlane (Standard_False),
  myDisplayPlaneLength (1000.0),
  myGridType (Aspect_GT_Rectangular),
  myGridEcho (Standard_True),
  myGridEchoLastVert (ShortRealLast(), ShortRealLast(), ShortRealLast())
{
  myRGrid = new V3d_RectangularGrid (this, Quantity_Color (Quantity_NOC_GRAY50), Quantity_Color (Quantity_NOC_GRAY70));
  myCGrid = new V3d_CircularGrid    (this, Quantity_Color (Quantity_NOC_GRAY50), Quantity_Color (Quantity_NOC_GRAY70));
}

// ========================================================================
// function : V3d_Viewer
// purpose  :
// ========================================================================
V3d_Viewer::V3d_Viewer (const Handle(Graphic3d_GraphicDriver)& theDriver,
                        const Standard_ExtString ,
                        const Standard_CString ,
                        const Standard_Real           theViewSize,
                        const V3d_TypeOfOrientation   theViewProj,
                        const Quantity_Color&         theViewBackground,
                        const V3d_TypeOfVisualization theVisualization,
                        const V3d_TypeOfShadingModel  theShadingModel,
                        const Standard_Boolean        theComputedMode,
                        const Standard_Boolean        theDefaultComputedMode)
: myDriver (theDriver),
  myStructureManager (new Graphic3d_StructureManager (theDriver)),
  myZLayerGenId (1, IntegerLast()),
  myBackground (theViewBackground),
  myViewSize (theViewSize),
  myViewProj (theViewProj),
  myVisualization (theVisualization),
  myShadingModel (theShadingModel),
  myDefaultTypeOfView (V3d_ORTHOGRAPHIC),
  myComputedMode (theComputedMode),
  myDefaultComputedMode (theDefaultComputedMode),
  myPrivilegedPlane (gp_Ax3 (gp_Pnt (0.,0.,0), gp_Dir (0.,0.,1.), gp_Dir (1.,0.,0.))),
  myDisplayPlane (Standard_False),
  myDisplayPlaneLength (theViewSize),
  myGridType (Aspect_GT_Rectangular),
  myGridEcho (Standard_True),
  myGridEchoLastVert (ShortRealLast(), ShortRealLast(), ShortRealLast())
{
  myRGrid = new V3d_RectangularGrid (this, Quantity_Color (Quantity_NOC_GRAY50), Quantity_Color (Quantity_NOC_GRAY70));
  myCGrid = new V3d_CircularGrid    (this, Quantity_Color (Quantity_NOC_GRAY50), Quantity_Color (Quantity_NOC_GRAY70));
  SetDefaultViewSize (theViewSize);
}

// ========================================================================
// function : CreateView
// purpose  :
// ========================================================================
Handle(V3d_View) V3d_Viewer::CreateView ()
{
  return new V3d_View(this, myDefaultTypeOfView);
}

// ========================================================================
// function : SetViewOn
// purpose  :
// ========================================================================
void V3d_Viewer::SetViewOn()
{
  for (V3d_ListOfView::Iterator aDefViewIter (myDefinedViews); aDefViewIter.More(); aDefViewIter.Next())
  {
    SetViewOn (aDefViewIter.Value());
  }
}

// ========================================================================
// function : SetViewOff
// purpose  :
// ========================================================================
void V3d_Viewer::SetViewOff()
{
  for (V3d_ListOfView::Iterator aDefViewIter (myDefinedViews); aDefViewIter.More(); aDefViewIter.Next())
  {
    SetViewOff (aDefViewIter.Value());
  }
}

// ========================================================================
// function : SetViewOn
// purpose  :
// ========================================================================
void V3d_Viewer::SetViewOn (const Handle(V3d_View)& theView)
{
  Handle(Graphic3d_CView) aViewImpl = theView->View();
  if (!aViewImpl->IsDefined() || myActiveViews.Contains (theView))
  {
    return;
  }

  myActiveViews.Append (theView);
  aViewImpl->Activate();
  for (V3d_ListOfLight::Iterator anActiveLightIter (myActiveLights); anActiveLightIter.More(); anActiveLightIter.Next())
  {
    theView->SetLightOn (anActiveLightIter.Value());
  }

  theView->SetGrid (myPrivilegedPlane, Grid ());
  theView->SetGridActivity (Grid ()->IsActive ());
  theView->Redraw();
}

// ========================================================================
// function : SetViewOff
// purpose  :
// ========================================================================
void V3d_Viewer::SetViewOff (const Handle(V3d_View)& theView)
{
  Handle(Graphic3d_CView) aViewImpl = theView->View();
  if (aViewImpl->IsDefined() && myActiveViews.Contains (theView))
  {
    myActiveViews.Remove (theView);
    aViewImpl->Deactivate() ;
  }
}

// ========================================================================
// function : Redraw
// purpose  :
// ========================================================================
void V3d_Viewer::Redraw() const
{
  for (V3d_ListOfView::Iterator aDefViewIter (myDefinedViews); aDefViewIter.More(); aDefViewIter.Next())
  {
    aDefViewIter.Value()->Redraw();
  }
}

// ========================================================================
// function : RedrawImmediate
// purpose  :
// ========================================================================
void V3d_Viewer::RedrawImmediate() const
{
  for (V3d_ListOfView::Iterator aDefViewIter (myDefinedViews); aDefViewIter.More(); aDefViewIter.Next())
  {
    aDefViewIter.Value()->RedrawImmediate();
  }
}

// ========================================================================
// function : Invalidate
// purpose  :
// ========================================================================
void V3d_Viewer::Invalidate() const
{
  for (V3d_ListOfView::Iterator aDefViewIter (myDefinedViews); aDefViewIter.More(); aDefViewIter.Next())
  {
    aDefViewIter.Value()->Invalidate();
  }
}

// ========================================================================
// function : Remove
// purpose  :
// ========================================================================
void V3d_Viewer::Remove()
{
  myStructureManager->Remove();
}

// ========================================================================
// function : Erase
// purpose  :
// ========================================================================
void V3d_Viewer::Erase() const
{
  myStructureManager->Erase();
}

// ========================================================================
// function : UnHighlight
// purpose  :
// ========================================================================
void V3d_Viewer::UnHighlight() const
{
  myStructureManager->UnHighlight();
}

void V3d_Viewer::SetDefaultViewSize (const Standard_Real theSize)
{
  if (theSize <= 0.0)
    throw V3d_BadValue("V3d_Viewer::SetDefaultViewSize, bad size");
  myViewSize = theSize;
}

// ========================================================================
// function : IfMoreViews
// purpose  :
// ========================================================================
Standard_Boolean V3d_Viewer::IfMoreViews() const
{
  return myDefinedViews.Size() < myStructureManager->MaxNumOfViews();
}

// ========================================================================
// function : AddView
// purpose  :
// ========================================================================
void V3d_Viewer::AddView (const Handle(V3d_View)& theView)
{
  if (!myDefinedViews.Contains (theView))
  {
    myDefinedViews.Append (theView);
  }
}

// ========================================================================
// function : DelView
// purpose  :
// ========================================================================
void V3d_Viewer::DelView (const Handle(V3d_View)& theView)
{
  myActiveViews.Remove (theView);
  myDefinedViews.Remove (theView);
}

//=======================================================================
//function : AddZLayer
//purpose  :
//=======================================================================
Standard_Boolean V3d_Viewer::AddZLayer (Standard_Integer& theLayerId)
{
  try
  {
    OCC_CATCH_SIGNALS
    theLayerId = myZLayerGenId.Next();
  }
  catch (Aspect_IdentDefinitionError)
  {
    // new index can't be generated
    return Standard_False;
  }

  myLayerIds.Add (theLayerId);
  myDriver->AddZLayer (theLayerId);

  return Standard_True;
}

//=======================================================================
//function : RemoveZLayer
//purpose  : 
//=======================================================================
Standard_Boolean V3d_Viewer::RemoveZLayer (const Standard_Integer theLayerId)
{
  if (!myLayerIds.Contains (theLayerId)
    || theLayerId < myZLayerGenId.Lower()
    || theLayerId > myZLayerGenId.Upper())
  {
    return Standard_False;
  }

  myDriver->RemoveZLayer (theLayerId);
  myLayerIds.Remove  (theLayerId);
  myZLayerGenId.Free (theLayerId);

  return Standard_True;
}

//=======================================================================
//function : GetAllZLayers
//purpose  :
//=======================================================================
void V3d_Viewer::GetAllZLayers (TColStd_SequenceOfInteger& theLayerSeq) const
{
  myDriver->ZLayers (theLayerSeq);
}

//=======================================================================
//function : SetZLayerSettings
//purpose  :
//=======================================================================
void V3d_Viewer::SetZLayerSettings (const Standard_Integer theLayerId, const Graphic3d_ZLayerSettings& theSettings)
{
  myDriver->SetZLayerSettings (theLayerId, theSettings);
}

//=======================================================================
//function : ZLayerSettings
//purpose  :
//=======================================================================
Graphic3d_ZLayerSettings V3d_Viewer::ZLayerSettings (const Standard_Integer theLayerId)
{
  return myDriver->ZLayerSettings (theLayerId);
}

//=======================================================================
//function : UpdateLights
//purpose  :
//=======================================================================
void V3d_Viewer::UpdateLights()
{
  for (V3d_ListOfView::Iterator anActiveViewIter (myActiveViews); anActiveViewIter.More(); anActiveViewIter.Next())
  {
    anActiveViewIter.Value()->UpdateLights();
  }
}

//=======================================================================
//function : SetLightOn
//purpose  :
//=======================================================================
void V3d_Viewer::SetLightOn (const Handle(V3d_Light)& theLight)
{
  if (!myActiveLights.Contains (theLight))
  {
    myActiveLights.Append (theLight);
  }

  for (V3d_ListOfView::Iterator anActiveViewIter (myActiveViews); anActiveViewIter.More(); anActiveViewIter.Next())
  {
    anActiveViewIter.Value()->SetLightOn (theLight);
  }
}

//=======================================================================
//function : SetLightOff
//purpose  :
//=======================================================================
void V3d_Viewer::SetLightOff (const Handle(V3d_Light)& theLight)
{
  myActiveLights.Remove (theLight);
  for (V3d_ListOfView::Iterator anActiveViewIter (myActiveViews); anActiveViewIter.More(); anActiveViewIter.Next())
  {
    anActiveViewIter.Value()->SetLightOff (theLight);
  }
}

//=======================================================================
//function : SetLightOn
//purpose  :
//=======================================================================
void V3d_Viewer::SetLightOn()
{
  for (V3d_ListOfLight::Iterator aDefLightIter (myDefinedLights); aDefLightIter.More(); aDefLightIter.Next())
  {
    if (!myActiveLights.Contains (aDefLightIter.Value()))
    {
      myActiveLights.Append (aDefLightIter.Value());
      for (V3d_ListOfView::Iterator anActiveViewIter (myActiveViews); anActiveViewIter.More(); anActiveViewIter.Next())
      {
        anActiveViewIter.Value()->SetLightOn (aDefLightIter.Value());
      }
    }
  }
}

//=======================================================================
//function : SetLightOff
//purpose  :
//=======================================================================
void V3d_Viewer::SetLightOff()
{
  for (V3d_ListOfLight::Iterator anActiveLightIter (myActiveLights); anActiveLightIter.More(); anActiveLightIter.Next())
  {
    for (V3d_ListOfView::Iterator anActiveViewIter (myActiveViews); anActiveViewIter.More(); anActiveViewIter.Next())
    {
      anActiveViewIter.Value()->SetLightOff (anActiveLightIter.Value());
    }
  }
  myActiveLights.Clear();
}

//=======================================================================
//function : IsGlobalLight
//purpose  :
//=======================================================================
Standard_Boolean V3d_Viewer::IsGlobalLight (const Handle(V3d_Light)& theLight) const
{
  return myActiveLights.Contains (theLight);
}

//=======================================================================
//function : AddLight
//purpose  :
//=======================================================================
void V3d_Viewer::AddLight (const Handle(V3d_Light)& theLight)
{
  if (!myDefinedLights.Contains (theLight))
  {
    myDefinedLights.Append (theLight);
  }
}

//=======================================================================
//function : DelLight
//purpose  :
//=======================================================================
void V3d_Viewer::DelLight (const Handle(V3d_Light)& theLight)
{
  SetLightOff (theLight);
  myDefinedLights.Remove (theLight);
}

//=======================================================================
//function : SetDefaultLights
//purpose  :
//=======================================================================
void V3d_Viewer::SetDefaultLights()
{
  while (!myDefinedLights.IsEmpty())
  {
    Handle(V3d_Light) aLight = myDefinedLights.First();
    DelLight (aLight);
  }

  SetLightOn (new V3d_DirectionalLight (this, V3d_Zneg, Quantity_NOC_WHITE, Standard_True));
  SetLightOn (new V3d_AmbientLight (this));
}
