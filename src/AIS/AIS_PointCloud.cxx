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

//=======================================================================
//function : AIS_PointCloudOwner
//purpose  :
//=======================================================================
AIS_PointCloudOwner::AIS_PointCloudOwner (const Handle(AIS_PointCloud)& theOrigin)
: SelectMgr_EntityOwner ((const Handle(SelectMgr_SelectableObject)& )theOrigin,  5),
  myDetPoints (new TColStd_HPackedMapOfInteger()),
  mySelPoints (new TColStd_HPackedMapOfInteger())
{
  //
}

//=======================================================================
//function : ~AIS_PointCloudOwner
//purpose  :
//=======================================================================
AIS_PointCloudOwner::~AIS_PointCloudOwner()
{
  //
}

//=======================================================================
//function : HilightWithColor
//purpose  :
//=======================================================================
Standard_Boolean AIS_PointCloudOwner::IsForcedHilight() const
{
  return true;
}

//=======================================================================
//function : HilightWithColor
//purpose  :
//=======================================================================
void AIS_PointCloudOwner::HilightWithColor (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                            const Handle(Prs3d_Drawer)& theStyle,
                                            const Standard_Integer )
{
  Handle(AIS_PointCloud) anObj = Handle(AIS_PointCloud)::DownCast (Selectable());
  if (anObj.IsNull())
  {
    throw Standard_ProgramError ("Internal Error within AIS_PointCloud::PointsOwner!");
  }

  const Handle(TColStd_HPackedMapOfInteger)& aMap = thePrsMgr->IsImmediateModeOn()
                                                  ? myDetPoints
                                                  : mySelPoints;
  Handle(Prs3d_Presentation) aPrs = thePrsMgr->IsImmediateModeOn()
                                  ? anObj->GetHilightPresentation(thePrsMgr)
                                  : anObj->GetSelectPresentation (thePrsMgr);
  const Graphic3d_ZLayerId aZLayer = theStyle->ZLayer() != -1
                                   ? theStyle->ZLayer()
                                   : (thePrsMgr->IsImmediateModeOn() ? Graphic3d_ZLayerId_Top : anObj->ZLayer());
  aMap->ChangeMap().Clear();
  for (SelectMgr_SequenceOfSelection::Iterator aSelIter (anObj->Selections()); aSelIter.More(); aSelIter.Next())
  {
    const Handle(SelectMgr_Selection)& aSel = aSelIter.Value();
    for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator aSelEntIter (aSel->Entities()); aSelEntIter.More(); aSelEntIter.Next())
    {
      const Handle(SelectMgr_SensitiveEntity)& aSelEnt = aSelEntIter.Value();
      if (aSelEnt->BaseSensitive()->OwnerId() == this)
      {
        if (Handle(Select3D_SensitivePrimitiveArray) aSensitive = Handle(Select3D_SensitivePrimitiveArray)::DownCast (aSelEnt->BaseSensitive()))
        {
          aMap->ChangeMap() = aSensitive->LastDetectedElementMap()->Map();
          if (aSensitive->LastDetectedElement() != -1)
          {
            aMap->ChangeMap().Add (aSensitive->LastDetectedElement());
          }
          break;
        }
      }
    }
  }

  aPrs->Clear();
  if (aPrs->GetZLayer() != aZLayer)
  {
    aPrs->SetZLayer (aZLayer);
  }
  if (aMap->Map().IsEmpty())
  {
    return;
  }

  const Handle(Graphic3d_ArrayOfPoints) anAllPoints = anObj->GetPoints();
  if (anAllPoints.IsNull())
  {
    return;
  }

  Handle(Graphic3d_ArrayOfPoints) aPoints = new Graphic3d_ArrayOfPoints (aMap->Map().Extent());
  for (TColStd_PackedMapOfInteger::Iterator aPntIter (aMap->Map()); aPntIter.More(); aPntIter.Next())
  {
    const gp_Pnt aPnt = anAllPoints->Vertice (aPntIter.Key() + 1);
    aPoints->AddVertex (aPnt);
  }

  Handle(Graphic3d_Group) aGroup = aPrs->NewGroup();
  aGroup->SetGroupPrimitivesAspect (theStyle->PointAspect()->Aspect());
  aGroup->AddPrimitiveArray (aPoints);
  if (thePrsMgr->IsImmediateModeOn())
  {
    thePrsMgr->AddToImmediateList (aPrs);
  }
  else
  {
    aPrs->Display();
  }
}

//=======================================================================
//function : Unhilight
//purpose  :
//=======================================================================
void AIS_PointCloudOwner::Unhilight (const Handle(PrsMgr_PresentationManager)& , const Standard_Integer )
{
  if (Handle(Prs3d_Presentation) aPrs = Selectable()->GetSelectPresentation (Handle(PrsMgr_PresentationManager)()))
  {
    aPrs->Erase();
  }
}

//=======================================================================
//function : Clear
//purpose  :
//=======================================================================
void AIS_PointCloudOwner::Clear (const Handle(PrsMgr_PresentationManager)& thePrsMgr, const Standard_Integer theMode)
{
  SelectMgr_EntityOwner::Clear (thePrsMgr, theMode);
}

//==================================================
// Function: AIS_PointCloud
// Purpose : Constructor
//==================================================
AIS_PointCloud::AIS_PointCloud()
{
  myDrawer->SetupOwnShadingAspect();
  myDrawer->ShadingAspect()->Aspect()->SetMarkerType (Aspect_TOM_POINT);

  SetDisplayMode (AIS_PointCloud::DM_Points);
  SetHilightMode (AIS_PointCloud::DM_BndBox);

  myDynHilightDrawer->SetPointAspect (new Prs3d_PointAspect (Aspect_TOM_PLUS, Quantity_NOC_CYAN1, 1.0));
}

//=======================================================================
//function : GetPoints
//purpose  :
//=======================================================================
const Handle(Graphic3d_ArrayOfPoints) AIS_PointCloud::GetPoints() const
{
  return myPoints;
}

//=======================================================================
//function : GetBoundingBox
//purpose  :
//=======================================================================
Bnd_Box AIS_PointCloud::GetBoundingBox() const
{
  return myBndBox;
}

//! Auxiliary method
static inline Bnd_Box getBoundingBox (const Handle(Graphic3d_ArrayOfPoints)& thePoints)
{
  Bnd_Box aBndBox;
  if (thePoints.IsNull())
  {
    return aBndBox;
  }

  const Standard_Integer aNbVertices = thePoints->VertexNumber();
  for (Standard_Integer aVertIter = 1; aVertIter <= aNbVertices; ++aVertIter)
  {
    aBndBox.Add (thePoints->Vertice (aVertIter));
  }
  return aBndBox;
}

//=======================================================================
//function : SetPoints
//purpose  :
//=======================================================================
void AIS_PointCloud::SetPoints (const Handle(Graphic3d_ArrayOfPoints)& thePoints)
{
  myPoints = thePoints;
  myBndBox = getBoundingBox (thePoints);
}

//=======================================================================
//function : SetPoints
//purpose  :
//=======================================================================
void AIS_PointCloud::SetPoints (const Handle(TColgp_HArray1OfPnt)&     theCoords,
                                const Handle(Quantity_HArray1OfColor)& theColors,
                                const Handle(TColgp_HArray1OfDir)&     theNormals)
{
  myPoints.Nullify();
  myBndBox.SetVoid();
  if (theCoords.IsNull())
  {
    return;
  }

  const Standard_Integer aNbPoints = theCoords->Length();
  if ((!theNormals.IsNull() && theNormals->Length() != aNbPoints)
   || (!theColors.IsNull()  && theColors->Length()  != aNbPoints))
  {
    // invalid input
    return;
  }

  const Standard_Boolean hasColors  = !theColors.IsNull()  && theColors->Length()  == aNbPoints;
  const Standard_Boolean hasNormals = !theNormals.IsNull() && theNormals->Length() == aNbPoints;

  const Standard_Integer aDiffColors  = hasColors  ? (theColors->Lower()  - theCoords->Lower()) : 0;
  const Standard_Integer aDiffNormals = hasNormals ? (theNormals->Lower() - theCoords->Lower()) : 0;

  myPoints = new Graphic3d_ArrayOfPoints (aNbPoints, hasColors, hasNormals);
  for (Standard_Integer aPntIter = theCoords->Lower(); aPntIter <= theCoords->Upper(); ++aPntIter)
  {
    myPoints->AddVertex (theCoords->Value (aPntIter));
    if (hasColors)
    {
      myPoints->SetVertexColor (myPoints->VertexNumber(),
                                theColors->Value (aPntIter + aDiffColors));
    }
    if (hasNormals)
    {
      myPoints->SetVertexNormal (myPoints->VertexNumber(),
                                 theNormals->Value (aPntIter + aDiffNormals));
    }
  }
  myBndBox = getBoundingBox (myPoints);
}

//=======================================================================
//function : SetColor
//purpose  :
//=======================================================================
void AIS_PointCloud::SetColor (const Quantity_Color& theColor)
{
  AIS_InteractiveObject::SetColor(theColor);

  myDrawer->ShadingAspect()->SetColor (theColor);
  SynchronizeAspects();
}

//=======================================================================
//function : UnsetColor
//purpose  :
//=======================================================================
void AIS_PointCloud::UnsetColor()
{
  if (!HasColor())
  {
    return;
  }

  AIS_InteractiveObject::UnsetColor();
  {
    Graphic3d_MaterialAspect aDefaultMat (Graphic3d_NameOfMaterial_Brass);
    Graphic3d_MaterialAspect aMat = aDefaultMat;
    Quantity_Color aColor = aDefaultMat.Color();
    if (myDrawer->HasLink())
    {
      aColor = myDrawer->Link()->ShadingAspect()->Color (myCurrentFacingModel);
    }
    if (HasMaterial() || myDrawer->HasLink())
    {
      aMat = AIS_GraphicTool::GetMaterial (HasMaterial() ? myDrawer : myDrawer->Link());
    }
    if (HasMaterial())
    {
      aMat.SetColor (aColor);
    }
    if (IsTransparent())
    {
      Standard_Real aTransp = myDrawer->ShadingAspect()->Transparency (myCurrentFacingModel);
      aMat.SetTransparency (Standard_ShortReal(aTransp));
    }
    myDrawer->ShadingAspect()->SetMaterial (aMat, myCurrentFacingModel);
    myDrawer->ShadingAspect()->Aspect()->SetInteriorColor (aColor);
  }

  SynchronizeAspects();
}

//=======================================================================
//function : SetMaterial
//purpose  :
//=======================================================================
void AIS_PointCloud::SetMaterial (const Graphic3d_MaterialAspect& theMat)
{
  hasOwnMaterial = Standard_True;

  myDrawer->ShadingAspect()->SetMaterial (theMat, myCurrentFacingModel);
  if (HasColor())
  {
    myDrawer->ShadingAspect()->SetColor (myDrawer->Color(), myCurrentFacingModel);
  }
  myDrawer->ShadingAspect()->SetTransparency (myDrawer->Transparency(), myCurrentFacingModel);
  SynchronizeAspects();
}

//=======================================================================
//function : UnsetMaterial
//purpose  :
//=======================================================================
void AIS_PointCloud::UnsetMaterial()
{
  if (!HasMaterial())
  {
    return;
  }

  {
    Graphic3d_MaterialAspect aDefaultMat (Graphic3d_NameOfMaterial_Brass);
    myDrawer->ShadingAspect()->SetMaterial (myDrawer->HasLink() ?
                                            myDrawer->Link()->ShadingAspect()->Material (myCurrentFacingModel) :
                                            aDefaultMat,
                                            myCurrentFacingModel);
    if (HasColor())
    {
      myDrawer->ShadingAspect()->SetColor        (myDrawer->Color(),        myCurrentFacingModel);
      myDrawer->ShadingAspect()->SetTransparency (myDrawer->Transparency(), myCurrentFacingModel);
    }
  }
  hasOwnMaterial = Standard_False;
  SynchronizeAspects();
}

//=======================================================================
//function : Compute
//purpose  :
//=======================================================================
void AIS_PointCloud::Compute (const Handle(PrsMgr_PresentationManager)& ,
                              const Handle(Prs3d_Presentation)& thePrs,
                              const Standard_Integer theMode)
{
  switch (theMode)
  {
    case AIS_PointCloud::DM_Points:
    {
      const Handle(Graphic3d_ArrayOfPoints) aPoints = GetPoints();
      if (aPoints.IsNull())
      {
        return;
      }

      Handle(Graphic3d_Group) aGroup = thePrs->NewGroup();
      aGroup->SetGroupPrimitivesAspect (myDrawer->ShadingAspect()->Aspect());
      aGroup->AddPrimitiveArray (aPoints);
      break;
    }
    case AIS_PointCloud::DM_BndBox:
    {
      Bnd_Box aBndBox = GetBoundingBox();
      if (aBndBox.IsVoid())
      {
        return;
      }

      Prs3d_BndBox::Add (thePrs, aBndBox, myDrawer);
      break;
    }
  }
}

//=======================================================================
//function : ComputeSelection
//purpose  :
//=======================================================================
void AIS_PointCloud::ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                       const Standard_Integer             theMode)
{
  Handle(SelectMgr_EntityOwner) anOwner = new SelectMgr_EntityOwner (this);
  switch (theMode)
  {
    case SM_Points:
    case SM_SubsetOfPoints:
    {
      const Handle(Graphic3d_ArrayOfPoints) aPoints = GetPoints();
      if (!aPoints.IsNull()
       && !aPoints->Attributes().IsNull())
      {
        if (theMode == SM_SubsetOfPoints)
        {
          anOwner = new AIS_PointCloudOwner (this);
        }

        // split large point clouds into several groups
        const Standard_Integer aNbGroups = aPoints->Attributes()->NbElements > 500000 ? 8 : 1;
        Handle(Select3D_SensitivePrimitiveArray) aSensitive = new Select3D_SensitivePrimitiveArray (anOwner);
        aSensitive->SetDetectElements (true);
        aSensitive->SetDetectElementMap (theMode == SM_SubsetOfPoints);
        aSensitive->SetSensitivityFactor (8);
        aSensitive->InitPoints (aPoints->Attributes(), aPoints->Indices(), TopLoc_Location(), true, aNbGroups);
        aSensitive->BVH();
        theSelection->Add (aSensitive);
        return;
      }
      break;
    }
    case SM_BndBox:
    {
      break;
    }
    default:
    {
      return;
    }
  }

  Bnd_Box aBndBox = GetBoundingBox();
  if (aBndBox.IsVoid())
  {
    return;
  }
  Handle(Select3D_SensitiveBox) aSensBox = new Select3D_SensitiveBox (anOwner, aBndBox);
  theSelection->Add (aSensBox);
}
