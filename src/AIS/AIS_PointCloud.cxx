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
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <PrsMgr_ModedPresentation.hxx>
#include <PrsMgr_Presentations.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <Select3D_SensitivePrimitiveArray.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <StdPrs_BndBox.hxx>


IMPLEMENT_STANDARD_RTTIEXT(AIS_PointCloud,AIS_InteractiveObject)

//==================================================
// Function: AIS_PointCloud
// Purpose : Constructor
//==================================================
AIS_PointCloud::AIS_PointCloud()
{
  // override default point style to Aspect_TOM_POINT
  myDrawer->SetPointAspect (new Prs3d_PointAspect (Aspect_TOM_POINT, Quantity_NOC_YELLOW, 1.0));

  SetDisplayMode (AIS_PointCloud::DM_Points);
  SetHilightMode (AIS_PointCloud::DM_BndBox);
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

  if (!myDrawer->HasOwnPointAspect())
  {
    myDrawer->SetPointAspect (new Prs3d_PointAspect (Aspect_TOM_POINT, theColor, 1.0));
    if (myDrawer->HasLink())
    {
      *myDrawer->PointAspect()->Aspect() = *myDrawer->Link()->PointAspect()->Aspect();
    }
  }
  if (!myDrawer->HasOwnShadingAspect())
  {
    myDrawer->SetShadingAspect (new Prs3d_ShadingAspect());
    if (myDrawer->HasLink())
    {
      *myDrawer->ShadingAspect()->Aspect() = *myDrawer->Link()->ShadingAspect()->Aspect();
    }
  }

  // Override color
  myDrawer->ShadingAspect()->SetColor (theColor);
  myDrawer->PointAspect()  ->SetColor (theColor);

  const PrsMgr_Presentations&        aPrsList     = Presentations();
  Handle(Graphic3d_AspectMarker3d)   aPointAspect = myDrawer->PointAspect()->Aspect();
  Handle(Graphic3d_AspectFillArea3d) anAreaAspect = myDrawer->ShadingAspect()->Aspect();
  for (Standard_Integer aPrsIt = 1; aPrsIt <= aPrsList.Length(); ++aPrsIt)
  {
    const PrsMgr_ModedPresentation& aPrsModed = aPrsList.Value (aPrsIt);
    if (aPrsModed.Mode() != AIS_PointCloud::DM_Points)
    {
      continue;
    }

    const Handle(Prs3d_Presentation)& aPrs = aPrsModed.Presentation()->Presentation();
    for (Graphic3d_SequenceOfGroup::Iterator aGroupIt (aPrs->Groups()); aGroupIt.More(); aGroupIt.Next())
    {
      const Handle(Graphic3d_Group)& aGroup = aGroupIt.Value();
      if (aGroup->IsGroupPrimitivesAspectSet (Graphic3d_ASPECT_MARKER))
      {
        aGroup->SetGroupPrimitivesAspect (aPointAspect);
      }
      if (aGroup->IsGroupPrimitivesAspectSet (Graphic3d_ASPECT_FILL_AREA))
      {
        aGroup->SetGroupPrimitivesAspect (anAreaAspect);
      }
    }
  }
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

  if (!HasWidth())
  {
    myDrawer->SetPointAspect (Handle(Prs3d_PointAspect)());
  }
  else
  {
    myDrawer->PointAspect()->SetColor (myDrawer->HasLink()
                                     ? myDrawer->Link()->PointAspect()->Aspect()->Color()
                                     : Quantity_Color (Quantity_NOC_YELLOW));
  }

  if (HasMaterial()
   || IsTransparent())
  {
    Graphic3d_MaterialAspect aDefaultMat (Graphic3d_NOM_BRASS);
    Graphic3d_MaterialAspect aMat = aDefaultMat;
    if (HasMaterial() || myDrawer->HasLink())
    {
      aMat = AIS_GraphicTool::GetMaterial (HasMaterial() ? myDrawer : myDrawer->Link());
    }
    if (HasMaterial())
    {
      Quantity_Color aColor = aDefaultMat.AmbientColor();
      if (myDrawer->HasLink())
      {
        aColor = myDrawer->Link()->ShadingAspect()->Color (myCurrentFacingModel);
      }
      aMat.SetColor (aColor);
    }
    if (IsTransparent())
    {
      Standard_Real aTransp = myDrawer->ShadingAspect()->Transparency (myCurrentFacingModel);
      aMat.SetTransparency (Standard_ShortReal(aTransp));
    }
    myDrawer->ShadingAspect()->SetMaterial (aMat, myCurrentFacingModel);
  }
  else
  {
    myDrawer->SetShadingAspect (Handle(Prs3d_ShadingAspect)());
  }
  myDrawer->SetPointAspect (Handle(Prs3d_PointAspect)());

  // modify shading presentation without re-computation
  const PrsMgr_Presentations&        aPrsList   = Presentations();
  Handle(Graphic3d_AspectFillArea3d) anAreaAsp  = myDrawer->Link()->ShadingAspect()->Aspect();
  Handle(Graphic3d_AspectMarker3d)   aMarkerAsp = myDrawer->Link()->PointAspect()->Aspect();
  for (Standard_Integer aPrsIt = 1; aPrsIt <= aPrsList.Length(); ++aPrsIt)
  {
    const PrsMgr_ModedPresentation& aPrsModed = aPrsList.Value (aPrsIt);
    if (aPrsModed.Mode() != AIS_PointCloud::DM_Points)
    {
      continue;
    }

    const Handle(Prs3d_Presentation)& aPrs = aPrsModed.Presentation()->Presentation();
    for (Graphic3d_SequenceOfGroup::Iterator aGroupIt (aPrs->Groups()); aGroupIt.More(); aGroupIt.Next())
    {
      const Handle(Graphic3d_Group)& aGroup = aGroupIt.Value();

      // Check if aspect of given type is set for the group,
      // because setting aspect for group with no already set aspect
      // can lead to loss of presentation data
      if (aGroup->IsGroupPrimitivesAspectSet (Graphic3d_ASPECT_FILL_AREA))
      {
        aGroup->SetGroupPrimitivesAspect (anAreaAsp);
      }
      if (aGroup->IsGroupPrimitivesAspectSet (Graphic3d_ASPECT_MARKER))
      {
        aGroup->SetGroupPrimitivesAspect (aMarkerAsp);
      }
    }
  }
}

//=======================================================================
//function : SetMaterial
//purpose  :
//=======================================================================
void AIS_PointCloud::SetMaterial (const Graphic3d_MaterialAspect& theMat)
{
  if (!myDrawer->HasOwnShadingAspect())
  {
    myDrawer->SetShadingAspect (new Prs3d_ShadingAspect());
    if (myDrawer->HasLink())
    {
      *myDrawer->ShadingAspect()->Aspect() = *myDrawer->Link()->ShadingAspect()->Aspect();
    }
  }
  hasOwnMaterial = Standard_True;

  myDrawer->ShadingAspect()->SetMaterial (theMat, myCurrentFacingModel);
  if (HasColor())
  {
    myDrawer->ShadingAspect()->SetColor (myDrawer->Color(), myCurrentFacingModel);
  }
  myDrawer->ShadingAspect()->SetTransparency (myDrawer->Transparency(), myCurrentFacingModel);

  // modify shading presentation without re-computation
  const PrsMgr_Presentations&        aPrsList  = Presentations();
  Handle(Graphic3d_AspectFillArea3d) anAreaAsp = myDrawer->ShadingAspect()->Aspect();
  for (Standard_Integer aPrsIt = 1; aPrsIt <= aPrsList.Length(); ++aPrsIt)
  {
    const PrsMgr_ModedPresentation& aPrsModed = aPrsList.Value (aPrsIt);
    if (aPrsModed.Mode() != AIS_PointCloud::DM_Points)
    {
      continue;
    }

    const Handle(Prs3d_Presentation)& aPrs = aPrsModed.Presentation()->Presentation();
    for (Graphic3d_SequenceOfGroup::Iterator aGroupIt (aPrs->Groups()); aGroupIt.More(); aGroupIt.Next())
    {
      const Handle(Graphic3d_Group)& aGroup = aGroupIt.Value();
      if (aGroup->IsGroupPrimitivesAspectSet (Graphic3d_ASPECT_FILL_AREA))
      {
        aGroup->SetGroupPrimitivesAspect (anAreaAsp);
      }
    }
  }
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

  if (HasColor()
   || IsTransparent())
  {
    Graphic3d_MaterialAspect aDefaultMat (Graphic3d_NOM_BRASS);
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
  else
  {
    myDrawer->SetShadingAspect (Handle(Prs3d_ShadingAspect)());
  }
  hasOwnMaterial = Standard_False;

  // modify shading presentation without re-computation
  const PrsMgr_Presentations&        aPrsList  = Presentations();
  Handle(Graphic3d_AspectFillArea3d) anAreaAsp = myDrawer->ShadingAspect()->Aspect();
  for (Standard_Integer aPrsIt = 1; aPrsIt <= aPrsList.Length(); ++aPrsIt)
  {
    const PrsMgr_ModedPresentation& aPrsModed = aPrsList.Value (aPrsIt);
    if (aPrsModed.Mode() != AIS_PointCloud::DM_Points)
    {
      continue;
    }

    const Handle(Prs3d_Presentation)& aPrs = aPrsModed.Presentation()->Presentation();
    for (Graphic3d_SequenceOfGroup::Iterator aGroupIt (aPrs->Groups()); aGroupIt.More(); aGroupIt.Next())
    {
      const Handle(Graphic3d_Group)& aGroup = aGroupIt.Value();
      if (aGroup->IsGroupPrimitivesAspectSet (Graphic3d_ASPECT_FILL_AREA))
      {
        aGroup->SetGroupPrimitivesAspect (anAreaAsp);
      }
    }
  }
}

//=======================================================================
//function : Compute
//purpose  :
//=======================================================================
void AIS_PointCloud::Compute (const Handle(PrsMgr_PresentationManager3d)& /*thePrsMgr*/,
                              const Handle(Prs3d_Presentation)&           thePrs,
                              const Standard_Integer                      theMode)
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

      Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup (thePrs);
      aGroup->SetGroupPrimitivesAspect (myDrawer->PointAspect()->Aspect());
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

      StdPrs_BndBox::Add (thePrs, aBndBox, myDrawer);
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
    {
      const Handle(Graphic3d_ArrayOfPoints) aPoints = GetPoints();
      if (!aPoints.IsNull()
       && !aPoints->Attributes().IsNull())
      {
        // split large point clouds into several groups
        const Standard_Integer aNbGroups = aPoints->Attributes()->NbElements > 500000 ? 8 : 1;
        Handle(Select3D_SensitivePrimitiveArray) aSensitive = new Select3D_SensitivePrimitiveArray (anOwner);
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
