// Created on: 2014-08-13
// Created by: Maxim GLIBIN
// Copyright (c) 2014 OPEN CASCADE SAS
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

#include <AIS_PointCloud.hxx>

#include <AIS_GraphicTool.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <Select3D_SensitivePrimitiveArray.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <Prs3d_BndBox.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_PointCloudOwner, SelectMgr_EntityOwner)
IMPLEMENT_STANDARD_RTTIEXT(AIS_PointCloud, AIS_InteractiveObject)

//=================================================================================================

AIS_PointCloudOwner::AIS_PointCloudOwner(const occ::handle<AIS_PointCloud>& theOrigin)
    : SelectMgr_EntityOwner((const occ::handle<SelectMgr_SelectableObject>&)theOrigin, 5),
      myDetPoints(new TColStd_HPackedMapOfInteger()),
      mySelPoints(new TColStd_HPackedMapOfInteger())
{}

//=================================================================================================

AIS_PointCloudOwner::~AIS_PointCloudOwner()
= default;

//=================================================================================================

bool AIS_PointCloudOwner::IsForcedHilight() const
{
  return true;
}

//=================================================================================================

void AIS_PointCloudOwner::HilightWithColor(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                                           const occ::handle<Prs3d_Drawer>&               theStyle,
                                           const int)
{
  occ::handle<AIS_PointCloud> anObj = occ::down_cast<AIS_PointCloud>(Selectable());
  if (anObj.IsNull())
  {
    throw Standard_ProgramError("Internal Error within AIS_PointCloud::PointsOwner!");
  }

  const occ::handle<TColStd_HPackedMapOfInteger>& aMap =
    thePrsMgr->IsImmediateModeOn() ? myDetPoints : mySelPoints;
  occ::handle<Prs3d_Presentation> aPrs = thePrsMgr->IsImmediateModeOn()
                                           ? anObj->GetHilightPresentation(thePrsMgr)
                                           : anObj->GetSelectPresentation(thePrsMgr);
  const Graphic3d_ZLayerId        aZLayer =
    theStyle->ZLayer() != -1
             ? theStyle->ZLayer()
             : (thePrsMgr->IsImmediateModeOn() ? Graphic3d_ZLayerId_Top : anObj->ZLayer());
  aMap->ChangeMap().Clear();
  for (NCollection_Sequence<occ::handle<SelectMgr_Selection>>::Iterator aSelIter(
         anObj->Selections());
       aSelIter.More();
       aSelIter.Next())
  {
    const occ::handle<SelectMgr_Selection>& aSel = aSelIter.Value();
    for (NCollection_Vector<occ::handle<SelectMgr_SensitiveEntity>>::Iterator aSelEntIter(
           aSel->Entities());
         aSelEntIter.More();
         aSelEntIter.Next())
    {
      const occ::handle<SelectMgr_SensitiveEntity>& aSelEnt = aSelEntIter.Value();
      if (aSelEnt->BaseSensitive()->OwnerId() == this)
      {
        if (occ::handle<Select3D_SensitivePrimitiveArray> aSensitive =
              occ::down_cast<Select3D_SensitivePrimitiveArray>(aSelEnt->BaseSensitive()))
        {
          aMap->ChangeMap() = aSensitive->LastDetectedElementMap()->Map();
          if (aSensitive->LastDetectedElement() != -1)
          {
            aMap->ChangeMap().Add(aSensitive->LastDetectedElement());
          }
          break;
        }
      }
    }
  }

  aPrs->Clear();
  if (aPrs->GetZLayer() != aZLayer)
  {
    aPrs->SetZLayer(aZLayer);
  }
  if (aMap->Map().IsEmpty())
  {
    return;
  }

  const occ::handle<Graphic3d_ArrayOfPoints> anAllPoints = anObj->GetPoints();
  if (anAllPoints.IsNull())
  {
    return;
  }

  occ::handle<Graphic3d_ArrayOfPoints> aPoints = new Graphic3d_ArrayOfPoints(aMap->Map().Extent());
  for (TColStd_PackedMapOfInteger::Iterator aPntIter(aMap->Map()); aPntIter.More(); aPntIter.Next())
  {
    const gp_Pnt aPnt = anAllPoints->Vertice(aPntIter.Key() + 1);
    aPoints->AddVertex(aPnt);
  }

  occ::handle<Graphic3d_Group> aGroup = aPrs->NewGroup();
  aGroup->SetGroupPrimitivesAspect(theStyle->PointAspect()->Aspect());
  aGroup->AddPrimitiveArray(aPoints);
  if (thePrsMgr->IsImmediateModeOn())
  {
    thePrsMgr->AddToImmediateList(aPrs);
  }
  else
  {
    aPrs->Display();
  }
}

//=================================================================================================

void AIS_PointCloudOwner::Unhilight(const occ::handle<PrsMgr_PresentationManager>&, const int)
{
  if (occ::handle<Prs3d_Presentation> aPrs =
        Selectable()->GetSelectPresentation(occ::handle<PrsMgr_PresentationManager>()))
  {
    aPrs->Erase();
  }
}

//=================================================================================================

void AIS_PointCloudOwner::Clear(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                                const int                                      theMode)
{
  SelectMgr_EntityOwner::Clear(thePrsMgr, theMode);
}

//=================================================================================================

AIS_PointCloud::AIS_PointCloud()
{
  myDrawer->SetupOwnShadingAspect();
  myDrawer->ShadingAspect()->Aspect()->SetMarkerType(Aspect_TOM_POINT);

  SetDisplayMode(AIS_PointCloud::DM_Points);
  SetHilightMode(AIS_PointCloud::DM_BndBox);

  myDynHilightDrawer->SetPointAspect(
    new Prs3d_PointAspect(Aspect_TOM_PLUS, Quantity_NOC_CYAN1, 1.0));
}

//=================================================================================================

const occ::handle<Graphic3d_ArrayOfPoints> AIS_PointCloud::GetPoints() const
{
  return myPoints;
}

//=================================================================================================

Bnd_Box AIS_PointCloud::GetBoundingBox() const
{
  return myBndBox;
}

//! Auxiliary method
static inline Bnd_Box getBoundingBox(const occ::handle<Graphic3d_ArrayOfPoints>& thePoints)
{
  Bnd_Box aBndBox;
  if (thePoints.IsNull())
  {
    return aBndBox;
  }

  const int aNbVertices = thePoints->VertexNumber();
  for (int aVertIter = 1; aVertIter <= aNbVertices; ++aVertIter)
  {
    aBndBox.Add(thePoints->Vertice(aVertIter));
  }
  return aBndBox;
}

//=================================================================================================

void AIS_PointCloud::SetPoints(const occ::handle<Graphic3d_ArrayOfPoints>& thePoints)
{
  myPoints = thePoints;
  myBndBox = getBoundingBox(thePoints);
}

//=================================================================================================

void AIS_PointCloud::SetPoints(const occ::handle<NCollection_HArray1<gp_Pnt>>&         theCoords,
                               const occ::handle<NCollection_HArray1<Quantity_Color>>& theColors,
                               const occ::handle<NCollection_HArray1<gp_Dir>>&         theNormals)
{
  myPoints.Nullify();
  myBndBox.SetVoid();
  if (theCoords.IsNull())
  {
    return;
  }

  const int aNbPoints = theCoords->Length();
  if ((!theNormals.IsNull() && theNormals->Length() != aNbPoints)
      || (!theColors.IsNull() && theColors->Length() != aNbPoints))
  {
    // invalid input
    return;
  }

  const bool hasColors  = !theColors.IsNull() && theColors->Length() == aNbPoints;
  const bool hasNormals = !theNormals.IsNull() && theNormals->Length() == aNbPoints;

  const int aDiffColors  = hasColors ? (theColors->Lower() - theCoords->Lower()) : 0;
  const int aDiffNormals = hasNormals ? (theNormals->Lower() - theCoords->Lower()) : 0;

  myPoints = new Graphic3d_ArrayOfPoints(aNbPoints, hasColors, hasNormals);
  for (int aPntIter = theCoords->Lower(); aPntIter <= theCoords->Upper(); ++aPntIter)
  {
    myPoints->AddVertex(theCoords->Value(aPntIter));
    if (hasColors)
    {
      myPoints->SetVertexColor(myPoints->VertexNumber(), theColors->Value(aPntIter + aDiffColors));
    }
    if (hasNormals)
    {
      myPoints->SetVertexNormal(myPoints->VertexNumber(),
                                theNormals->Value(aPntIter + aDiffNormals));
    }
  }
  myBndBox = getBoundingBox(myPoints);
}

//=================================================================================================

void AIS_PointCloud::SetColor(const Quantity_Color& theColor)
{
  AIS_InteractiveObject::SetColor(theColor);

  myDrawer->ShadingAspect()->SetColor(theColor);
  SynchronizeAspects();
}

//=================================================================================================

void AIS_PointCloud::UnsetColor()
{
  if (!HasColor())
  {
    return;
  }

  AIS_InteractiveObject::UnsetColor();
  {
    Graphic3d_MaterialAspect aDefaultMat(Graphic3d_NameOfMaterial_Brass);
    Graphic3d_MaterialAspect aMat   = aDefaultMat;
    Quantity_Color           aColor = aDefaultMat.Color();
    if (myDrawer->HasLink())
    {
      aColor = myDrawer->Link()->ShadingAspect()->Color(myCurrentFacingModel);
    }
    if (HasMaterial() || myDrawer->HasLink())
    {
      aMat = AIS_GraphicTool::GetMaterial(HasMaterial() ? myDrawer : myDrawer->Link());
    }
    if (HasMaterial())
    {
      aMat.SetColor(aColor);
    }
    if (IsTransparent())
    {
      double aTransp = myDrawer->ShadingAspect()->Transparency(myCurrentFacingModel);
      aMat.SetTransparency(float(aTransp));
    }
    myDrawer->ShadingAspect()->SetMaterial(aMat, myCurrentFacingModel);
    myDrawer->ShadingAspect()->Aspect()->SetInteriorColor(aColor);
  }

  SynchronizeAspects();
}

//=================================================================================================

void AIS_PointCloud::SetMaterial(const Graphic3d_MaterialAspect& theMat)
{
  hasOwnMaterial = true;

  myDrawer->ShadingAspect()->SetMaterial(theMat, myCurrentFacingModel);
  if (HasColor())
  {
    myDrawer->ShadingAspect()->SetColor(myDrawer->Color(), myCurrentFacingModel);
  }
  myDrawer->ShadingAspect()->SetTransparency(myDrawer->Transparency(), myCurrentFacingModel);
  SynchronizeAspects();
}

//=================================================================================================

void AIS_PointCloud::UnsetMaterial()
{
  if (!HasMaterial())
  {
    return;
  }

  {
    Graphic3d_MaterialAspect aDefaultMat(Graphic3d_NameOfMaterial_Brass);
    myDrawer->ShadingAspect()->SetMaterial(
      myDrawer->HasLink() ? myDrawer->Link()->ShadingAspect()->Material(myCurrentFacingModel)
                          : aDefaultMat,
      myCurrentFacingModel);
    if (HasColor())
    {
      myDrawer->ShadingAspect()->SetColor(myDrawer->Color(), myCurrentFacingModel);
      myDrawer->ShadingAspect()->SetTransparency(myDrawer->Transparency(), myCurrentFacingModel);
    }
  }
  hasOwnMaterial = false;
  SynchronizeAspects();
}

//=================================================================================================

void AIS_PointCloud::Compute(const occ::handle<PrsMgr_PresentationManager>&,
                             const occ::handle<Prs3d_Presentation>& thePrs,
                             const int                              theMode)
{
  switch (theMode)
  {
    case AIS_PointCloud::DM_Points: {
      const occ::handle<Graphic3d_ArrayOfPoints> aPoints = GetPoints();
      if (aPoints.IsNull())
      {
        return;
      }

      occ::handle<Graphic3d_Group> aGroup = thePrs->NewGroup();
      aGroup->SetGroupPrimitivesAspect(myDrawer->ShadingAspect()->Aspect());
      aGroup->AddPrimitiveArray(aPoints);
      break;
    }
    case AIS_PointCloud::DM_BndBox: {
      Bnd_Box aBndBox = GetBoundingBox();
      if (aBndBox.IsVoid())
      {
        return;
      }

      Prs3d_BndBox::Add(thePrs, aBndBox, myDrawer);
      break;
    }
  }
}

//=================================================================================================

void AIS_PointCloud::ComputeSelection(const occ::handle<SelectMgr_Selection>& theSelection,
                                      const int                               theMode)
{
  occ::handle<SelectMgr_EntityOwner> anOwner = new SelectMgr_EntityOwner(this);
  switch (theMode)
  {
    case SM_Points:
    case SM_SubsetOfPoints: {
      const occ::handle<Graphic3d_ArrayOfPoints> aPoints = GetPoints();
      if (!aPoints.IsNull() && !aPoints->Attributes().IsNull())
      {
        if (theMode == SM_SubsetOfPoints)
        {
          anOwner = new AIS_PointCloudOwner(this);
        }

        // split large point clouds into several groups
        const int aNbGroups = aPoints->Attributes()->NbElements > 500000 ? 8 : 1;
        occ::handle<Select3D_SensitivePrimitiveArray> aSensitive =
          new Select3D_SensitivePrimitiveArray(anOwner);
        aSensitive->SetDetectElements(true);
        aSensitive->SetDetectElementMap(theMode == SM_SubsetOfPoints);
        aSensitive->SetSensitivityFactor(8);
        aSensitive->InitPoints(aPoints->Attributes(),
                               aPoints->Indices(),
                               TopLoc_Location(),
                               true,
                               aNbGroups);
        aSensitive->BVH();
        theSelection->Add(aSensitive);
        return;
      }
      break;
    }
    case SM_BndBox: {
      break;
    }
    default: {
      return;
    }
  }

  Bnd_Box aBndBox = GetBoundingBox();
  if (aBndBox.IsVoid())
  {
    return;
  }
  occ::handle<Select3D_SensitiveBox> aSensBox = new Select3D_SensitiveBox(anOwner, aBndBox);
  theSelection->Add(aSensBox);
}
