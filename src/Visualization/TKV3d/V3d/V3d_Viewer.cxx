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

#include <Aspect_IdentDefinitionError.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_Text.hxx>
#include <Standard_ErrorHandler.hxx>
#include <V3d.hxx>
#include <V3d_BadValue.hxx>
#include <V3d_CircularGrid.hxx>
#include <V3d_AmbientLight.hxx>
#include <V3d_DirectionalLight.hxx>
#include <V3d_RectangularGrid.hxx>
#include <V3d_View.hxx>

IMPLEMENT_STANDARD_RTTIEXT(V3d_Viewer, Standard_Transient)

//=================================================================================================

V3d_Viewer::V3d_Viewer(const occ::handle<Graphic3d_GraphicDriver>& theDriver)
    : myDriver(theDriver),
      myStructureManager(new Graphic3d_StructureManager(theDriver)),
      myZLayerGenId(1, IntegerLast()),
      myBackground(Quantity_NOC_GRAY30),
      myViewSize(1000.0),
      myViewProj(V3d_XposYnegZpos),
      myVisualization(V3d_ZBUFFER),
      myDefaultTypeOfView(V3d_ORTHOGRAPHIC),
      myComputedMode(true),
      myDefaultComputedMode(false),
      myPrivilegedPlane(gp_Ax3(gp_Pnt(0., 0., 0), gp_Dir(gp_Dir::D::Z), gp_Dir(gp_Dir::D::X))),
      myDisplayPlane(false),
      myDisplayPlaneLength(1000.0),
      myGridType(Aspect_GT_Rectangular),
      myGridEcho(true),
      myGridEchoLastVert(ShortRealLast(), ShortRealLast(), ShortRealLast())
{
  //
}

//=================================================================================================

occ::handle<V3d_View> V3d_Viewer::CreateView()
{
  return new V3d_View(this, myDefaultTypeOfView);
}

//=================================================================================================

void V3d_Viewer::SetViewOn()
{
  for (NCollection_List<occ::handle<V3d_View>>::Iterator aDefViewIter(myDefinedViews); aDefViewIter.More();
       aDefViewIter.Next())
  {
    SetViewOn(aDefViewIter.Value());
  }
}

//=================================================================================================

void V3d_Viewer::SetViewOff()
{
  for (NCollection_List<occ::handle<V3d_View>>::Iterator aDefViewIter(myDefinedViews); aDefViewIter.More();
       aDefViewIter.Next())
  {
    SetViewOff(aDefViewIter.Value());
  }
}

//=================================================================================================

void V3d_Viewer::SetViewOn(const occ::handle<V3d_View>& theView)
{
  occ::handle<Graphic3d_CView> aViewImpl = theView->View();
  if (!aViewImpl->IsDefined() || myActiveViews.Contains(theView))
  {
    return;
  }

  myActiveViews.Append(theView);
  aViewImpl->Activate();
  for (NCollection_List<occ::handle<Graphic3d_CLight>>::Iterator anActiveLightIter(myActiveLights); anActiveLightIter.More();
       anActiveLightIter.Next())
  {
    theView->SetLightOn(anActiveLightIter.Value());
  }
  if (occ::handle<Aspect_Grid> aGrid = Grid(false))
  {
    theView->SetGrid(myPrivilegedPlane, aGrid);
    theView->SetGridActivity(aGrid->IsActive());
  }
  if (theView->SetImmediateUpdate(false))
  {
    theView->Redraw();
    theView->SetImmediateUpdate(true);
  }
}

//=================================================================================================

void V3d_Viewer::SetViewOff(const occ::handle<V3d_View>& theView)
{
  occ::handle<Graphic3d_CView> aViewImpl = theView->View();
  if (aViewImpl->IsDefined() && myActiveViews.Contains(theView))
  {
    myActiveViews.Remove(theView);
    aViewImpl->Deactivate();
  }
}

//=================================================================================================

void V3d_Viewer::Redraw() const
{
  for (int aSubViewPass = 0; aSubViewPass < 2; ++aSubViewPass)
  {
    // redraw subviews first
    const bool isSubViewPass = (aSubViewPass == 0);
    for (const occ::handle<V3d_View>& aViewIter : myDefinedViews)
    {
      if (isSubViewPass && aViewIter->IsSubview())
      {
        aViewIter->Redraw();
      }
      else if (!isSubViewPass && !aViewIter->IsSubview())
      {
        aViewIter->Redraw();
      }
    }
  }
}

//=================================================================================================

void V3d_Viewer::RedrawImmediate() const
{
  for (int aSubViewPass = 0; aSubViewPass < 2; ++aSubViewPass)
  {
    // redraw subviews first
    const bool isSubViewPass = (aSubViewPass == 0);
    for (const occ::handle<V3d_View>& aViewIter : myDefinedViews)
    {
      if (isSubViewPass && aViewIter->IsSubview())
      {
        aViewIter->RedrawImmediate();
      }
      else if (!isSubViewPass && !aViewIter->IsSubview())
      {
        aViewIter->RedrawImmediate();
      }
    }
  }
}

//=================================================================================================

void V3d_Viewer::Invalidate() const
{
  for (NCollection_List<occ::handle<V3d_View>>::Iterator aDefViewIter(myDefinedViews); aDefViewIter.More();
       aDefViewIter.Next())
  {
    aDefViewIter.Value()->Invalidate();
  }
}

//=================================================================================================

void V3d_Viewer::Remove()
{
  myStructureManager->Remove();
}

//=================================================================================================

void V3d_Viewer::Erase() const
{
  myStructureManager->Erase();
}

//=================================================================================================

void V3d_Viewer::UnHighlight() const
{
  myStructureManager->UnHighlight();
}

void V3d_Viewer::SetDefaultViewSize(const double theSize)
{
  if (theSize <= 0.0)
    throw V3d_BadValue("V3d_Viewer::SetDefaultViewSize, bad size");
  myViewSize = theSize;
}

//=================================================================================================

bool V3d_Viewer::IfMoreViews() const
{
  return myDefinedViews.Size() < myStructureManager->MaxNumOfViews();
}

//=================================================================================================

void V3d_Viewer::AddView(const occ::handle<V3d_View>& theView)
{
  if (!myDefinedViews.Contains(theView))
  {
    myDefinedViews.Append(theView);
  }
}

//=================================================================================================

void V3d_Viewer::DelView(const V3d_View* theView)
{
  for (NCollection_List<occ::handle<V3d_View>>::Iterator aViewIter(myActiveViews); aViewIter.More(); aViewIter.Next())
  {
    if (aViewIter.Value() == theView)
    {
      myActiveViews.Remove(aViewIter);
      break;
    }
  }
  for (NCollection_List<occ::handle<V3d_View>>::Iterator aViewIter(myDefinedViews); aViewIter.More(); aViewIter.Next())
  {
    if (aViewIter.Value() == theView)
    {
      myDefinedViews.Remove(aViewIter);
      break;
    }
  }
}

//=================================================================================================

bool V3d_Viewer::InsertLayerBefore(Graphic3d_ZLayerId&             theNewLayerId,
                                               const Graphic3d_ZLayerSettings& theSettings,
                                               const Graphic3d_ZLayerId        theLayerAfter)
{
  if (myZLayerGenId.Next(theNewLayerId))
  {
    myLayerIds.Add(theNewLayerId);
    myDriver->InsertLayerBefore(theNewLayerId, theSettings, theLayerAfter);
    return true;
  }
  return false;
}

//=================================================================================================

bool V3d_Viewer::InsertLayerAfter(Graphic3d_ZLayerId&             theNewLayerId,
                                              const Graphic3d_ZLayerSettings& theSettings,
                                              const Graphic3d_ZLayerId        theLayerBefore)
{
  if (myZLayerGenId.Next(theNewLayerId))
  {
    myLayerIds.Add(theNewLayerId);
    myDriver->InsertLayerAfter(theNewLayerId, theSettings, theLayerBefore);
    return true;
  }
  return false;
}

//=================================================================================================

bool V3d_Viewer::RemoveZLayer(const Graphic3d_ZLayerId theLayerId)
{
  if (!myLayerIds.Contains(theLayerId) || theLayerId < myZLayerGenId.Lower()
      || theLayerId > myZLayerGenId.Upper())
  {
    return false;
  }

  myDriver->RemoveZLayer(theLayerId);
  myLayerIds.Remove(theLayerId);
  myZLayerGenId.Free(theLayerId);

  return true;
}

//=================================================================================================

void V3d_Viewer::GetAllZLayers(NCollection_Sequence<int>& theLayerSeq) const
{
  myDriver->ZLayers(theLayerSeq);
}

//=================================================================================================

void V3d_Viewer::SetZLayerSettings(const Graphic3d_ZLayerId        theLayerId,
                                   const Graphic3d_ZLayerSettings& theSettings)
{
  myDriver->SetZLayerSettings(theLayerId, theSettings);
}

//=================================================================================================

const Graphic3d_ZLayerSettings& V3d_Viewer::ZLayerSettings(
  const Graphic3d_ZLayerId theLayerId) const
{
  return myDriver->ZLayerSettings(theLayerId);
}

//=================================================================================================

void V3d_Viewer::UpdateLights()
{
  for (NCollection_List<occ::handle<V3d_View>>::Iterator anActiveViewIter(myActiveViews); anActiveViewIter.More();
       anActiveViewIter.Next())
  {
    anActiveViewIter.Value()->UpdateLights();
  }
}

//=================================================================================================

void V3d_Viewer::SetLightOn(const occ::handle<V3d_Light>& theLight)
{
  if (!myActiveLights.Contains(theLight))
  {
    myActiveLights.Append(theLight);
  }

  for (NCollection_List<occ::handle<V3d_View>>::Iterator anActiveViewIter(myActiveViews); anActiveViewIter.More();
       anActiveViewIter.Next())
  {
    anActiveViewIter.Value()->SetLightOn(theLight);
  }
}

//=================================================================================================

void V3d_Viewer::SetLightOff(const occ::handle<V3d_Light>& theLight)
{
  myActiveLights.Remove(theLight);
  for (NCollection_List<occ::handle<V3d_View>>::Iterator anActiveViewIter(myActiveViews); anActiveViewIter.More();
       anActiveViewIter.Next())
  {
    anActiveViewIter.Value()->SetLightOff(theLight);
  }
}

//=================================================================================================

void V3d_Viewer::SetLightOn()
{
  for (NCollection_List<occ::handle<Graphic3d_CLight>>::Iterator aDefLightIter(myDefinedLights); aDefLightIter.More();
       aDefLightIter.Next())
  {
    if (!myActiveLights.Contains(aDefLightIter.Value()))
    {
      myActiveLights.Append(aDefLightIter.Value());
      for (NCollection_List<occ::handle<V3d_View>>::Iterator anActiveViewIter(myActiveViews); anActiveViewIter.More();
           anActiveViewIter.Next())
      {
        anActiveViewIter.Value()->SetLightOn(aDefLightIter.Value());
      }
    }
  }
}

//=================================================================================================

void V3d_Viewer::SetLightOff()
{
  for (NCollection_List<occ::handle<Graphic3d_CLight>>::Iterator anActiveLightIter(myActiveLights); anActiveLightIter.More();
       anActiveLightIter.Next())
  {
    for (NCollection_List<occ::handle<V3d_View>>::Iterator anActiveViewIter(myActiveViews); anActiveViewIter.More();
         anActiveViewIter.Next())
    {
      anActiveViewIter.Value()->SetLightOff(anActiveLightIter.Value());
    }
  }
  myActiveLights.Clear();
}

//=================================================================================================

bool V3d_Viewer::IsGlobalLight(const occ::handle<V3d_Light>& theLight) const
{
  return myActiveLights.Contains(theLight);
}

//=================================================================================================

void V3d_Viewer::AddLight(const occ::handle<V3d_Light>& theLight)
{
  if (!myDefinedLights.Contains(theLight))
  {
    myDefinedLights.Append(theLight);
  }
}

//=================================================================================================

void V3d_Viewer::DelLight(const occ::handle<V3d_Light>& theLight)
{
  SetLightOff(theLight);
  myDefinedLights.Remove(theLight);
}

//=================================================================================================

void V3d_Viewer::SetDefaultLights()
{
  while (!myDefinedLights.IsEmpty())
  {
    occ::handle<V3d_Light> aLight = myDefinedLights.First();
    DelLight(aLight);
  }

  occ::handle<V3d_DirectionalLight> aDirLight = new V3d_DirectionalLight(V3d_Zneg, Quantity_NOC_WHITE);
  aDirLight->SetName("headlight");
  aDirLight->SetHeadlight(true);
  occ::handle<V3d_AmbientLight> anAmbLight = new V3d_AmbientLight(Quantity_NOC_WHITE);
  anAmbLight->SetName("amblight");
  AddLight(aDirLight);
  AddLight(anAmbLight);
  SetLightOn(aDirLight);
  SetLightOn(anAmbLight);
}

//=================================================================================================

void V3d_Viewer::SetPrivilegedPlane(const gp_Ax3& thePlane)
{
  myPrivilegedPlane         = thePlane;
  occ::handle<Aspect_Grid> aGrid = Grid(true);
  aGrid->SetDrawMode(aGrid->DrawMode()); // aGrid->UpdateDisplay();
  for (NCollection_List<occ::handle<V3d_View>>::Iterator anActiveViewIter(myActiveViews); anActiveViewIter.More();
       anActiveViewIter.Next())
  {
    anActiveViewIter.Value()->SetGrid(myPrivilegedPlane, aGrid);
  }

  if (myDisplayPlane)
  {
    DisplayPrivilegedPlane(true, myDisplayPlaneLength);
  }
}

//=================================================================================================

void V3d_Viewer::DisplayPrivilegedPlane(const bool theOnOff,
                                        const double    theSize)
{
  myDisplayPlane       = theOnOff;
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
    myPlaneStructure = new Graphic3d_Structure(StructureManager());
    myPlaneStructure->SetInfiniteState(true);
    myPlaneStructure->Display();
  }
  else
  {
    myPlaneStructure->Clear();
  }

  occ::handle<Graphic3d_Group> aGroup = myPlaneStructure->NewGroup();

  occ::handle<Graphic3d_AspectLine3d> aLineAttrib =
    new Graphic3d_AspectLine3d(Quantity_NOC_GRAY60, Aspect_TOL_SOLID, 1.0);
  aGroup->SetGroupPrimitivesAspect(aLineAttrib);

  occ::handle<Graphic3d_AspectText3d> aTextAttrib = new Graphic3d_AspectText3d();
  aTextAttrib->SetColor(Quantity_Color(Quantity_NOC_ROYALBLUE1));
  aGroup->SetGroupPrimitivesAspect(aTextAttrib);

  occ::handle<Graphic3d_ArrayOfSegments> aPrims = new Graphic3d_ArrayOfSegments(6);

  const gp_Pnt& p0 = myPrivilegedPlane.Location();

  const gp_Pnt pX(p0.XYZ() + myDisplayPlaneLength * myPrivilegedPlane.XDirection().XYZ());
  aPrims->AddVertex(p0);
  aPrims->AddVertex(pX);
  occ::handle<Graphic3d_Text> aText = new Graphic3d_Text(1.0f / 81.0f);
  aText->SetText("X");
  aText->SetPosition(pX);
  aGroup->AddText(aText);

  const gp_Pnt pY(p0.XYZ() + myDisplayPlaneLength * myPrivilegedPlane.YDirection().XYZ());
  aPrims->AddVertex(p0);
  aPrims->AddVertex(pY);
  aText = new Graphic3d_Text(1.0f / 81.0f);
  aText->SetText("Y");
  aText->SetPosition(pY);
  aGroup->AddText(aText);

  const gp_Pnt pZ(p0.XYZ() + myDisplayPlaneLength * myPrivilegedPlane.Direction().XYZ());
  aPrims->AddVertex(p0);
  aPrims->AddVertex(pZ);
  aText = new Graphic3d_Text(1.0f / 81.0f);
  aText->SetText("Z");
  aText->SetPosition(pZ);
  aGroup->AddText(aText);

  aGroup->AddPrimitiveArray(aPrims);

  myPlaneStructure->Display();
}

//=================================================================================================

occ::handle<Aspect_Grid> V3d_Viewer::Grid(Aspect_GridType theGridType, bool theToCreate)
{
  switch (theGridType)
  {
    case Aspect_GT_Circular: {
      if (myCGrid.IsNull() && theToCreate)
      {
        myCGrid = new V3d_CircularGrid(this,
                                       Quantity_Color(Quantity_NOC_GRAY50),
                                       Quantity_Color(Quantity_NOC_GRAY70));
      }
      return occ::handle<Aspect_Grid>(myCGrid);
    }
    case Aspect_GT_Rectangular: {
      if (myRGrid.IsNull() && theToCreate)
      {
        myRGrid = new V3d_RectangularGrid(this,
                                          Quantity_Color(Quantity_NOC_GRAY50),
                                          Quantity_Color(Quantity_NOC_GRAY70));
      }
      return occ::handle<Aspect_Grid>(myRGrid);
    }
  }
  return occ::handle<Aspect_Grid>();
}

//=================================================================================================

Aspect_GridDrawMode V3d_Viewer::GridDrawMode()
{
  occ::handle<Aspect_Grid> aGrid = Grid(false);
  return !aGrid.IsNull() ? aGrid->DrawMode() : Aspect_GDM_Lines;
}

//=================================================================================================

void V3d_Viewer::ActivateGrid(const Aspect_GridType theType, const Aspect_GridDrawMode theMode)
{
  if (occ::handle<Aspect_Grid> anOldGrid = Grid(false))
  {
    anOldGrid->Erase();
  }

  myGridType                = theType;
  occ::handle<Aspect_Grid> aGrid = Grid(true);
  aGrid->SetDrawMode(theMode);
  if (theMode != Aspect_GDM_None)
  {
    aGrid->Display();
  }
  aGrid->Activate();
  for (NCollection_List<occ::handle<V3d_View>>::Iterator anActiveViewIter(myActiveViews); anActiveViewIter.More();
       anActiveViewIter.Next())
  {
    anActiveViewIter.Value()->SetGrid(myPrivilegedPlane, aGrid);
  }
}

//=================================================================================================

void V3d_Viewer::DeactivateGrid()
{
  occ::handle<Aspect_Grid> aGrid = Grid(false);
  if (aGrid.IsNull())
  {
    return;
  }

  aGrid->Erase();
  aGrid->Deactivate();

  myGridType = Aspect_GT_Rectangular;
  for (NCollection_List<occ::handle<V3d_View>>::Iterator anActiveViewIter(myActiveViews); anActiveViewIter.More();
       anActiveViewIter.Next())
  {
    anActiveViewIter.Value()->SetGridActivity(false);
    if (myGridEcho && !myGridEchoStructure.IsNull())
    {
      myGridEchoStructure->Erase();
    }
  }
}

//=================================================================================================

bool V3d_Viewer::IsGridActive()
{
  occ::handle<Aspect_Grid> aGrid = Grid(false);
  return !aGrid.IsNull() && aGrid->IsActive();
}

//=================================================================================================

void V3d_Viewer::RectangularGridValues(double& theXOrigin,
                                       double& theYOrigin,
                                       double& theXStep,
                                       double& theYStep,
                                       double& theRotationAngle)
{
  Grid(Aspect_GT_Rectangular, true);
  theXOrigin       = myRGrid->XOrigin();
  theYOrigin       = myRGrid->YOrigin();
  theXStep         = myRGrid->XStep();
  theYStep         = myRGrid->YStep();
  theRotationAngle = myRGrid->RotationAngle();
}

//=================================================================================================

void V3d_Viewer::SetRectangularGridValues(const double theXOrigin,
                                          const double theYOrigin,
                                          const double theXStep,
                                          const double theYStep,
                                          const double theRotationAngle)
{
  Grid(Aspect_GT_Rectangular, true);
  myRGrid->SetGridValues(theXOrigin, theYOrigin, theXStep, theYStep, theRotationAngle);
  for (NCollection_List<occ::handle<V3d_View>>::Iterator anActiveViewIter(myActiveViews); anActiveViewIter.More();
       anActiveViewIter.Next())
  {
    anActiveViewIter.Value()->SetGrid(myPrivilegedPlane, myRGrid);
  }
}

//=================================================================================================

void V3d_Viewer::CircularGridValues(double&    theXOrigin,
                                    double&    theYOrigin,
                                    double&    theRadiusStep,
                                    int& theDivisionNumber,
                                    double&    theRotationAngle)
{
  Grid(Aspect_GT_Circular, true);
  theXOrigin        = myCGrid->XOrigin();
  theYOrigin        = myCGrid->YOrigin();
  theRadiusStep     = myCGrid->RadiusStep();
  theDivisionNumber = myCGrid->DivisionNumber();
  theRotationAngle  = myCGrid->RotationAngle();
}

//=================================================================================================

void V3d_Viewer::SetCircularGridValues(const double    theXOrigin,
                                       const double    theYOrigin,
                                       const double    theRadiusStep,
                                       const int theDivisionNumber,
                                       const double    theRotationAngle)
{
  Grid(Aspect_GT_Circular, true);
  myCGrid->SetGridValues(theXOrigin,
                         theYOrigin,
                         theRadiusStep,
                         theDivisionNumber,
                         theRotationAngle);
  for (NCollection_List<occ::handle<V3d_View>>::Iterator anActiveViewIter(myActiveViews); anActiveViewIter.More();
       anActiveViewIter.Next())
  {
    anActiveViewIter.Value()->SetGrid(myPrivilegedPlane, myCGrid);
  }
}

//=================================================================================================

void V3d_Viewer::RectangularGridGraphicValues(double& theXSize,
                                              double& theYSize,
                                              double& theOffSet)
{
  Grid(Aspect_GT_Rectangular, true);
  myRGrid->GraphicValues(theXSize, theYSize, theOffSet);
}

//=================================================================================================

void V3d_Viewer::SetRectangularGridGraphicValues(const double theXSize,
                                                 const double theYSize,
                                                 const double theOffSet)
{
  Grid(Aspect_GT_Rectangular, true);
  myRGrid->SetGraphicValues(theXSize, theYSize, theOffSet);
}

//=================================================================================================

void V3d_Viewer::CircularGridGraphicValues(double& theRadius, double& theOffSet)
{
  Grid(Aspect_GT_Circular, true);
  myCGrid->GraphicValues(theRadius, theOffSet);
}

//=================================================================================================

void V3d_Viewer::SetCircularGridGraphicValues(const double theRadius,
                                              const double theOffSet)
{
  Grid(Aspect_GT_Circular, true);
  myCGrid->SetGraphicValues(theRadius, theOffSet);
}

//=================================================================================================

void V3d_Viewer::SetGridEcho(const bool theToShowGrid)
{
  if (myGridEcho == theToShowGrid)
  {
    return;
  }

  myGridEcho = theToShowGrid;
  if (theToShowGrid || myGridEchoStructure.IsNull())
  {
    return;
  }

  myGridEchoStructure->Erase();
}

//=================================================================================================

void V3d_Viewer::SetGridEcho(const occ::handle<Graphic3d_AspectMarker3d>& theMarker)
{
  if (myGridEchoStructure.IsNull())
  {
    myGridEchoStructure = new Graphic3d_Structure(StructureManager());
    myGridEchoGroup     = myGridEchoStructure->NewGroup();
  }

  myGridEchoAspect = theMarker;
  myGridEchoGroup->SetPrimitivesAspect(theMarker);
}

//=================================================================================================

void V3d_Viewer::ShowGridEcho(const occ::handle<V3d_View>& theView, const Graphic3d_Vertex& theVertex)
{
  if (!myGridEcho)
  {
    return;
  }

  if (myGridEchoStructure.IsNull())
  {
    myGridEchoStructure = new Graphic3d_Structure(StructureManager());
    myGridEchoGroup     = myGridEchoStructure->NewGroup();

    myGridEchoAspect =
      new Graphic3d_AspectMarker3d(Aspect_TOM_STAR, Quantity_Color(Quantity_NOC_GRAY90), 3.0);
    myGridEchoGroup->SetPrimitivesAspect(myGridEchoAspect);
  }

  if (theVertex.X() == myGridEchoLastVert.X() && theVertex.Y() == myGridEchoLastVert.Y()
      && theVertex.Z() == myGridEchoLastVert.Z())
  {
    return;
  }

  myGridEchoLastVert = theVertex;
  myGridEchoGroup->Clear();
  myGridEchoGroup->SetPrimitivesAspect(myGridEchoAspect);

  occ::handle<Graphic3d_ArrayOfPoints> anArrayOfPoints = new Graphic3d_ArrayOfPoints(1);
  anArrayOfPoints->AddVertex(theVertex.X(), theVertex.Y(), theVertex.Z());
  myGridEchoGroup->AddPrimitiveArray(anArrayOfPoints);

  myGridEchoStructure->SetZLayer(Graphic3d_ZLayerId_Topmost);
  myGridEchoStructure->SetInfiniteState(true);
  myGridEchoStructure->CStructure()->ViewAffinity = new Graphic3d_ViewAffinity();
  myGridEchoStructure->CStructure()->ViewAffinity->SetVisible(false);
  myGridEchoStructure->CStructure()->ViewAffinity->SetVisible(theView->View()->Identification(),
                                                              true);
  myGridEchoStructure->Display();
}

//=================================================================================================

void V3d_Viewer::HideGridEcho(const occ::handle<V3d_View>& theView)
{
  if (myGridEchoStructure.IsNull())
  {
    return;
  }

  myGridEchoLastVert.SetCoord(ShortRealLast(), ShortRealLast(), ShortRealLast());
  const occ::handle<Graphic3d_ViewAffinity>& anAffinity =
    myGridEchoStructure->CStructure()->ViewAffinity;
  if (!anAffinity.IsNull() && anAffinity->IsVisible(theView->View()->Identification()))
  {
    myGridEchoStructure->Erase();
  }
}

//=================================================================================================

void V3d_Viewer::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myDriver.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myStructureManager.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myZLayerGenId)

  for (NCollection_List<occ::handle<V3d_View>>::Iterator anIter(myDefinedViews); anIter.More(); anIter.Next())
  {
    const occ::handle<V3d_View>& aDefinedView = anIter.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, aDefinedView.get())
  }

  for (NCollection_List<occ::handle<V3d_View>>::Iterator anIter(myActiveViews); anIter.More(); anIter.Next())
  {
    const occ::handle<V3d_View>& anActiveView = anIter.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, anActiveView.get())
  }

  for (NCollection_List<occ::handle<Graphic3d_CLight>>::Iterator anIter(myDefinedLights); anIter.More(); anIter.Next())
  {
    const occ::handle<Graphic3d_CLight>& aDefinedLight = anIter.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, aDefinedLight.get())
  }

  for (NCollection_List<occ::handle<Graphic3d_CLight>>::Iterator anIter(myActiveLights); anIter.More(); anIter.Next())
  {
    const occ::handle<Graphic3d_CLight>& anActiveLight = anIter.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, anActiveLight.get())
  }

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myBackground)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myGradientBackground)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myViewSize)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myViewProj)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myVisualization)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myDefaultTypeOfView)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myDefaultRenderingParams)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myComputedMode)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myDefaultComputedMode)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myPrivilegedPlane)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myPlaneStructure.get())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myDisplayPlane)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myDisplayPlaneLength)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myRGrid.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myCGrid.get())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myGridType)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myGridEcho)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myGridEchoStructure.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myGridEchoGroup.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myGridEchoAspect.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myGridEchoLastVert)
}
