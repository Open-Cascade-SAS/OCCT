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
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_Text.hxx>
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
  myShadingModel (Graphic3d_TOSM_VERTEX),
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
                        const Standard_Real                theViewSize,
                        const V3d_TypeOfOrientation        theViewProj,
                        const Quantity_Color&              theViewBackground,
                        const V3d_TypeOfVisualization      theVisualization,
                        const Graphic3d_TypeOfShadingModel theShadingModel,
                        const Standard_Boolean             theComputedMode,
                        const Standard_Boolean             theDefaultComputedMode)
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
  if (theView->SetImmediateUpdate (Standard_False))
  {
    theView->Redraw();
    theView->SetImmediateUpdate (Standard_True);
  }
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
//function : InsertLayerBefore
//purpose  :
//=======================================================================
Standard_Boolean V3d_Viewer::InsertLayerBefore (Graphic3d_ZLayerId& theNewLayerId,
                                                const Graphic3d_ZLayerSettings& theSettings,
                                                const Graphic3d_ZLayerId theLayerAfter)
{
  if (myZLayerGenId.Next (theNewLayerId))
  {
    myLayerIds.Add (theNewLayerId);
    myDriver->InsertLayerBefore (theNewLayerId, theSettings, theLayerAfter);
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : InsertLayerAfter
//purpose  :
//=======================================================================
Standard_Boolean V3d_Viewer::InsertLayerAfter (Graphic3d_ZLayerId& theNewLayerId,
                                               const Graphic3d_ZLayerSettings& theSettings,
                                               const Graphic3d_ZLayerId theLayerBefore)
{
  if (myZLayerGenId.Next (theNewLayerId))
  {
    myLayerIds.Add (theNewLayerId);
    myDriver->InsertLayerAfter (theNewLayerId, theSettings, theLayerBefore);
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : RemoveZLayer
//purpose  : 
//=======================================================================
Standard_Boolean V3d_Viewer::RemoveZLayer (const Graphic3d_ZLayerId theLayerId)
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
void V3d_Viewer::SetZLayerSettings (const Graphic3d_ZLayerId theLayerId, const Graphic3d_ZLayerSettings& theSettings)
{
  myDriver->SetZLayerSettings (theLayerId, theSettings);
}

//=======================================================================
//function : ZLayerSettings
//purpose  :
//=======================================================================
const Graphic3d_ZLayerSettings& V3d_Viewer::ZLayerSettings (const Graphic3d_ZLayerId theLayerId) const
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

  Handle(V3d_DirectionalLight) aDirLight  = new V3d_DirectionalLight (V3d_Zneg, Quantity_NOC_WHITE, Standard_True);
  Handle(V3d_AmbientLight)     anAmbLight = new V3d_AmbientLight (Quantity_NOC_WHITE);
  AddLight (aDirLight);
  AddLight (anAmbLight);
  SetLightOn (aDirLight);
  SetLightOn (anAmbLight);
}

//=======================================================================
//function : SetPrivilegedPlane
//purpose  :
//=======================================================================
void V3d_Viewer::SetPrivilegedPlane (const gp_Ax3& thePlane)
{
  myPrivilegedPlane = thePlane;
  Grid()->SetDrawMode(Grid()->DrawMode());
  for (V3d_ListOfView::Iterator anActiveViewIter (myActiveViews); anActiveViewIter.More(); anActiveViewIter.Next())
  {
    anActiveViewIter.Value()->SetGrid (myPrivilegedPlane, Grid());
  }

  if (myDisplayPlane)
  {
    DisplayPrivilegedPlane (Standard_True, myDisplayPlaneLength);
  }
}

//=======================================================================
//function : DisplayPrivilegedPlane
//purpose  :
//=======================================================================
void V3d_Viewer::DisplayPrivilegedPlane (const Standard_Boolean theOnOff, const Standard_Real theSize)
{
  myDisplayPlane = theOnOff;
  myDisplayPlaneLength = theSize;

  if (!myDisplayPlane)
  {
    if (!myPlaneStructure.IsNull())
	{
      myPlaneStructure->Erase();
    }
    return;
  }

  if (myPlaneStructure.IsNull())
  {
    myPlaneStructure = new Graphic3d_Structure (StructureManager());
    myPlaneStructure->SetInfiniteState (Standard_True);
    myPlaneStructure->Display();
  }
  else
  {
    myPlaneStructure->Clear();
  }

  Handle(Graphic3d_Group) aGroup = myPlaneStructure->NewGroup();

  Handle(Graphic3d_AspectLine3d) aLineAttrib = new Graphic3d_AspectLine3d (Quantity_NOC_GRAY60, Aspect_TOL_SOLID, 1.0);
  aGroup->SetGroupPrimitivesAspect (aLineAttrib);

  Handle(Graphic3d_AspectText3d) aTextAttrib = new Graphic3d_AspectText3d();
  aTextAttrib->SetColor (Quantity_Color (Quantity_NOC_ROYALBLUE1));
  aGroup->SetGroupPrimitivesAspect (aTextAttrib);

  Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments (6);

  const gp_Pnt& p0 = myPrivilegedPlane.Location();

  const gp_Pnt pX (p0.XYZ() + myDisplayPlaneLength * myPrivilegedPlane.XDirection().XYZ());
  aPrims->AddVertex (p0);
  aPrims->AddVertex (pX);
  Handle(Graphic3d_Text) aText = new Graphic3d_Text (1.0f / 81.0f);
  aText->SetText ("X");
  aText->SetPosition (pX);
  aGroup->AddText (aText);

  const gp_Pnt pY (p0.XYZ() + myDisplayPlaneLength * myPrivilegedPlane.YDirection().XYZ());
  aPrims->AddVertex (p0);
  aPrims->AddVertex (pY);
  aText = new Graphic3d_Text (1.0f / 81.0f);
  aText->SetText ("Y");
  aText->SetPosition (pY);
  aGroup->AddText (aText);

  const gp_Pnt pZ (p0.XYZ() + myDisplayPlaneLength * myPrivilegedPlane.Direction().XYZ());
  aPrims->AddVertex (p0);
  aPrims->AddVertex (pZ);
  aText = new Graphic3d_Text (1.0f / 81.0f);
  aText->SetText ("Z");
  aText->SetPosition (pZ);
  aGroup->AddText (aText);

  aGroup->AddPrimitiveArray (aPrims);

  myPlaneStructure->Display();
}

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void V3d_Viewer::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)
  
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myDriver.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myStructureManager.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myZLayerGenId)
  
  for (V3d_ListOfView::Iterator anIter (myDefinedViews); anIter.More(); anIter.Next())
  {
    const Handle(V3d_View)& aDefinedView = anIter.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, aDefinedView.get())
  }

  for (V3d_ListOfView::Iterator anIter (myActiveViews); anIter.More(); anIter.Next())
  {
    const Handle(V3d_View)& anActiveView = anIter.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, anActiveView.get())
  }
    
  for (V3d_ListOfLight::Iterator anIter (myDefinedLights); anIter.More(); anIter.Next())
  {
    const Handle(Graphic3d_CLight)& aDefinedLight = anIter.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, aDefinedLight.get())
  }

  for (V3d_ListOfLight::Iterator anIter (myActiveLights); anIter.More(); anIter.Next())
  {
    const Handle(Graphic3d_CLight)& anActiveLight = anIter.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, anActiveLight.get())
  }

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myBackground)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myGradientBackground)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myViewSize)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myViewProj)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myVisualization)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myShadingModel)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myDefaultTypeOfView)
  
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myDefaultRenderingParams)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myComputedMode)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myDefaultComputedMode)

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myPrivilegedPlane)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myPlaneStructure.get())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myDisplayPlane)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myDisplayPlaneLength)
  
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myRGrid.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myCGrid.get())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myGridType)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myGridEcho)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myGridEchoStructure.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myGridEchoGroup.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myGridEchoAspect.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myGridEchoLastVert)
}
