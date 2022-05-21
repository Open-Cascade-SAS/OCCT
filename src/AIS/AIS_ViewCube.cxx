// Created on: 2017-07-25
// Created by: Anastasia BOBYLEVA
// Copyright (c) 2017-2019 OPEN CASCADE SAS
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

#include <AIS_ViewCube.hxx>

#include <AIS_AnimationCamera.hxx>
#include <AIS_InteractiveContext.hxx>
#include <gp_Ax2.hxx>
#include <Graphic3d_Text.hxx>
#include <NCollection_Lerp.hxx>
#include <Prs3d.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_Text.hxx>
#include <Prs3d_ToolSphere.hxx>
#include <SelectMgr_SequenceOfOwner.hxx>
#include <V3d.hxx>
#include <V3d_View.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_ViewCube, AIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(AIS_ViewCubeOwner, SelectMgr_EntityOwner)
IMPLEMENT_STANDARD_RTTIEXT(AIS_ViewCubeSensitive, Select3D_SensitivePrimitiveArray)

namespace
{
  static const Standard_Integer THE_NB_ROUND_SPLITS = 8;
  static const Standard_Integer THE_NB_DISK_SLICES = 20;
  static const Standard_Integer THE_NB_ARROW_FACETTES = 20;

  //! Return the number of non-zero components.
  static Standard_Integer nbDirectionComponents (const gp_Dir& theDir)
  {
    Standard_Integer aNbComps = 0;
    for (Standard_Integer aCompIter = 1; aCompIter <= 3; ++aCompIter)
    {
      if (Abs (theDir.Coord (aCompIter)) > gp::Resolution())
      {
        ++aNbComps;
      }
    }
    return aNbComps;
  }
}

//=======================================================================
//function : AIS_ViewCubeSensitive
//purpose  :
//=======================================================================
AIS_ViewCubeSensitive::AIS_ViewCubeSensitive (const Handle(SelectMgr_EntityOwner)& theOwner,
                                              const Handle(Graphic3d_ArrayOfTriangles)& theTris)
: Select3D_SensitivePrimitiveArray (theOwner)
{
  InitTriangulation (theTris->Attributes(), theTris->Indices(), TopLoc_Location());
}

//=======================================================================
//function : Matches
//purpose  :
//=======================================================================
Standard_Boolean AIS_ViewCubeSensitive::Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                                 SelectBasics_PickResult& thePickResult)
{
  return isValidRay (theMgr) && Select3D_SensitivePrimitiveArray::Matches (theMgr, thePickResult);
}

//=======================================================================
//function : isValidRay
//purpose  :
//=======================================================================
bool AIS_ViewCubeSensitive::isValidRay (const SelectBasics_SelectingVolumeManager& theMgr) const
{
  if (theMgr.GetActiveSelectionType() != SelectMgr_SelectionType_Point)
  {
    // disallow rectangular selection
    return false;
  }

  if (AIS_ViewCubeOwner* anOwner = dynamic_cast<AIS_ViewCubeOwner* >(myOwnerId.get()))
  {
    const Standard_Real anAngleToler = 10.0 * M_PI / 180.0;
    const gp_Dir aRay = theMgr.GetViewRayDirection();
    const gp_Dir aDir = V3d::GetProjAxis (anOwner->MainOrientation());
    return !aRay.IsNormal (aDir, anAngleToler);
  }
  return true;
}

//=======================================================================
//function : IsBoxSide
//purpose  :
//=======================================================================
bool AIS_ViewCube::IsBoxSide (V3d_TypeOfOrientation theOrient)
{
  return nbDirectionComponents (V3d::GetProjAxis (theOrient)) == 1;
}

//=======================================================================
//function : IsBoxEdge
//purpose  :
//=======================================================================
bool AIS_ViewCube::IsBoxEdge (V3d_TypeOfOrientation theOrient)
{
  return nbDirectionComponents (V3d::GetProjAxis (theOrient)) == 2;
}

//=======================================================================
//function : IsBoxCorner
//purpose  :
//=======================================================================
bool AIS_ViewCube::IsBoxCorner (V3d_TypeOfOrientation theOrient)
{
  return nbDirectionComponents (V3d::GetProjAxis (theOrient)) == 3;
}

//=======================================================================
//function : AIS_ViewCube
//purpose  :
//=======================================================================
AIS_ViewCube::AIS_ViewCube()
: myBoxEdgeAspect (new Prs3d_ShadingAspect()),
  myBoxCornerAspect (new Prs3d_ShadingAspect()),
  mySize (1.0),
  myBoxEdgeMinSize (2.0),
  myBoxEdgeGap (0.0),
  myBoxFacetExtension (1.0),
  myAxesPadding (1.0),
  myAxesRadius (1.0),
  myAxesConeRadius (3.0),
  myAxesSphereRadius (4.0),
  myCornerMinSize (2.0),
  myRoundRadius  (0.0),
  myToDisplayAxes (true),
  myToDisplayEdges (true),
  myToDisplayVertices (true),
  myIsYup (false),
  myViewAnimation (new AIS_AnimationCamera ("AIS_ViewCube", Handle(V3d_View)())),
  myStartState(new Graphic3d_Camera()),
  myEndState  (new Graphic3d_Camera()),
  myToAutoStartAnim (true),
  myIsFixedAnimation (true),
  myToFitSelected (true),
  myToResetCameraUp (false)
{
  myViewAnimation->SetOwnDuration (0.5);
  myInfiniteState = true;
  myIsMutable = true;
  myDrawer->SetZLayer (Graphic3d_ZLayerId_Topmost);
  myTransformPersistence = new Graphic3d_TransformPers (Graphic3d_TMF_TriedronPers, Aspect_TOTP_LEFT_LOWER, Graphic3d_Vec2i (100, 100));

  myDrawer->SetTextAspect  (new Prs3d_TextAspect());
  myDrawer->SetShadingAspect (new Prs3d_ShadingAspect());

  myDynHilightDrawer = new Prs3d_Drawer();
  myDynHilightDrawer->SetLink (myDrawer);
  myDynHilightDrawer->SetShadingAspect (new Prs3d_ShadingAspect());

  setDefaultAttributes();
  setDefaultHighlightAttributes();

  // setup default labels
  myBoxSideLabels.Bind (V3d_TypeOfOrientation_Zup_Front,  "FRONT");
  myBoxSideLabels.Bind (V3d_TypeOfOrientation_Zup_Back,   "BACK");
  myBoxSideLabels.Bind (V3d_TypeOfOrientation_Zup_Top,    "TOP");
  myBoxSideLabels.Bind (V3d_TypeOfOrientation_Zup_Bottom, "BOTTOM");
  myBoxSideLabels.Bind (V3d_TypeOfOrientation_Zup_Left,   "LEFT");
  myBoxSideLabels.Bind (V3d_TypeOfOrientation_Zup_Right,  "RIGHT");

  myAxesLabels.Bind (Prs3d_DatumParts_XAxis, "X");
  myAxesLabels.Bind (Prs3d_DatumParts_YAxis, "Y");
  myAxesLabels.Bind (Prs3d_DatumParts_ZAxis, "Z");

  // define default size
  SetSize (70.0);
}

//=======================================================================
//function : setDefaultAttributes
//purpose  :
//=======================================================================
void AIS_ViewCube::setDefaultAttributes()
{
  myDrawer->TextAspect()->SetHorizontalJustification(Graphic3d_HTA_CENTER);
  myDrawer->TextAspect()->SetVerticalJustification  (Graphic3d_VTA_CENTER);
  myDrawer->TextAspect()->SetColor (Quantity_NOC_BLACK);
  myDrawer->TextAspect()->SetFont (Font_NOF_SANS_SERIF);
  myDrawer->TextAspect()->SetHeight (16.0);
  myDrawer->TextAspect()->Aspect()->SetTextZoomable (true); // the whole object is drawn within transformation-persistence
  // this should be forced back-face culling regardless Closed flag
  myDrawer->TextAspect()->Aspect()->SetFaceCulling (Graphic3d_TypeOfBackfacingModel_BackCulled);

  Graphic3d_MaterialAspect aMat (Graphic3d_NameOfMaterial_UserDefined);
  aMat.SetColor (Quantity_NOC_WHITE);
  aMat.SetAmbientColor (Quantity_NOC_GRAY60);

  const Handle(Graphic3d_AspectFillArea3d)& aShading = myDrawer->ShadingAspect()->Aspect();
  aShading->SetInteriorStyle (Aspect_IS_SOLID);
  // this should be forced back-face culling regardless Closed flag
  aShading->SetFaceCulling (Graphic3d_TypeOfBackfacingModel_BackCulled);
  aShading->SetInteriorColor (aMat.Color());
  aShading->SetFrontMaterial (aMat);
  myDrawer->SetFaceBoundaryDraw (false);

  *myBoxEdgeAspect  ->Aspect() = *aShading;
  myBoxEdgeAspect->SetColor (Quantity_NOC_GRAY30);
  *myBoxCornerAspect->Aspect() = *aShading;
  myBoxCornerAspect->SetColor (Quantity_NOC_GRAY30);
}

//=======================================================================
//function : setDefaultHighlightAttributes
//purpose  :
//=======================================================================
void AIS_ViewCube::setDefaultHighlightAttributes()
{
  Graphic3d_MaterialAspect aHighlightMaterial;
  aHighlightMaterial.SetAmbientColor (Quantity_NOC_BLACK);
  aHighlightMaterial.SetDiffuseColor (Quantity_NOC_BLACK);
  aHighlightMaterial.SetSpecularColor(Quantity_NOC_BLACK);
  aHighlightMaterial.SetEmissiveColor(Quantity_NOC_BLACK);
  aHighlightMaterial.SetMaterialType (Graphic3d_MATERIAL_ASPECT);
  myDynHilightDrawer->SetShadingAspect (new Prs3d_ShadingAspect());
  myDynHilightDrawer->ShadingAspect()->SetMaterial (aHighlightMaterial);
  myDynHilightDrawer->ShadingAspect()->SetColor (Quantity_NOC_CYAN1);
  myDynHilightDrawer->SetZLayer (Graphic3d_ZLayerId_Topmost);
  myDynHilightDrawer->SetColor (Quantity_NOC_CYAN1);
}

//=======================================================================
//function : SetYup
//purpose  :
//=======================================================================
void AIS_ViewCube::SetYup (Standard_Boolean theIsYup,
                           Standard_Boolean theToUpdateLabels)
{
  if (myIsYup == theIsYup)
  {
    return;
  }

  myIsYup = theIsYup;

  static const V3d_TypeOfOrientation THE_ZUP_ORI_LIST[6] =
  {
    V3d_TypeOfOrientation_Zup_Front, V3d_TypeOfOrientation_Zup_Back,
    V3d_TypeOfOrientation_Zup_Top,   V3d_TypeOfOrientation_Zup_Bottom,
    V3d_TypeOfOrientation_Zup_Left,  V3d_TypeOfOrientation_Zup_Right
  };
  static const V3d_TypeOfOrientation THE_YUP_ORI_LIST[6] =
  {
    V3d_TypeOfOrientation_Yup_Front, V3d_TypeOfOrientation_Yup_Back,
    V3d_TypeOfOrientation_Yup_Top,   V3d_TypeOfOrientation_Yup_Bottom,
    V3d_TypeOfOrientation_Yup_Left,  V3d_TypeOfOrientation_Yup_Right
  };
  if (theToUpdateLabels)
  {
    NCollection_Array1<TCollection_AsciiString> aLabels (0, 5);
    for (Standard_Integer aLabelIter = 0; aLabelIter < 6; ++aLabelIter)
    {
      myBoxSideLabels.Find (!myIsYup ? THE_YUP_ORI_LIST[aLabelIter] : THE_ZUP_ORI_LIST[aLabelIter],
                            aLabels.ChangeValue (aLabelIter));
    }
    for (Standard_Integer aLabelIter = 0; aLabelIter < 6; ++aLabelIter)
    {
      myBoxSideLabels.Bind (myIsYup ? THE_YUP_ORI_LIST[aLabelIter] : THE_ZUP_ORI_LIST[aLabelIter],
                            aLabels.Value (aLabelIter));
    }
  }

  SetToUpdate();
}

//=======================================================================
//function : ResetStyles
//purpose  :
//=======================================================================
void AIS_ViewCube::ResetStyles()
{
  UnsetAttributes();
  UnsetHilightAttributes();

  myBoxEdgeMinSize = 2.0;
  myCornerMinSize  = 2.0;
  myBoxEdgeGap     = 0.0;
  myRoundRadius    = 0.0;

  myToDisplayAxes = true;
  myToDisplayEdges = true;
  myToDisplayVertices = true;

  myBoxFacetExtension = 1.0;
  myAxesPadding = 1.0;
  SetSize (70.0);
}

//=======================================================================
//function : SetSize
//purpose  :
//=======================================================================
void AIS_ViewCube::SetSize (Standard_Real theValue,
                            Standard_Boolean theToAdaptAnother)
{
  const bool isNewSize = Abs (mySize - theValue) > Precision::Confusion();
  mySize = theValue;
  if (theToAdaptAnother)
  {
    if (myBoxFacetExtension > 0.0)
    {
      SetBoxFacetExtension (mySize * 0.15);
    }
    if (myAxesPadding > 0.0)
    {
      SetAxesPadding (mySize * 0.1);
    }
    SetFontHeight (mySize * 0.16);
  }
  if (isNewSize)
  {
    SetToUpdate();
  }
}

//=======================================================================
//function : SetRoundRadius
//purpose  :
//=======================================================================
void AIS_ViewCube::SetRoundRadius (const Standard_Real theValue)
{
  Standard_OutOfRange_Raise_if (theValue < 0.0 || theValue > 0.5,
                                "AIS_ViewCube::SetRoundRadius(): theValue should be in [0; 0.5]");
  if (Abs (myRoundRadius - theValue) > Precision::Confusion())
  {
    myRoundRadius = theValue;
    SetToUpdate();
  }
}

//=======================================================================
//function : createRoundRectangleTriangles
//purpose  :
//=======================================================================
void AIS_ViewCube::createRoundRectangleTriangles (const Handle(Graphic3d_ArrayOfTriangles)& theTris,
                                                  Standard_Integer& theNbNodes,
                                                  Standard_Integer& theNbTris,
                                                  const gp_XY& theSize,
                                                  Standard_Real theRadius,
                                                  const gp_Trsf& theTrsf)
{
  const Standard_Real aRadius = Min (theRadius, Min (theSize.X(), theSize.Y()) * 0.5);
  const gp_XY  aHSize (theSize.X() * 0.5 - aRadius, theSize.Y() * 0.5 - aRadius);
  const gp_Dir aNorm = gp::DZ().Transformed (theTrsf);
  const Standard_Integer aVertFirst = !theTris.IsNull() ? theTris->VertexNumber() : 0;
  if (aRadius > 0.0)
  {
    const Standard_Integer aNbNodes = (THE_NB_ROUND_SPLITS + 1) * 4 + 1;
    theNbNodes += aNbNodes;
    theNbTris  += aNbNodes;
    if (theTris.IsNull())
    {
      return;
    }

    theTris->AddVertex (gp_Pnt (0.0, 0.0, 0.0).Transformed (theTrsf));
    for (Standard_Integer aNodeIter = 0; aNodeIter <= THE_NB_ROUND_SPLITS; ++aNodeIter)
    {
      const Standard_Real anAngle = NCollection_Lerp<Standard_Real>::Interpolate (M_PI * 0.5, 0.0, Standard_Real(aNodeIter) / Standard_Real(THE_NB_ROUND_SPLITS));
      theTris->AddVertex (gp_Pnt (aHSize.X() + aRadius * Cos (anAngle), aHSize.Y() + aRadius * Sin (anAngle), 0.0).Transformed (theTrsf));
    }
    for (Standard_Integer aNodeIter = 0; aNodeIter <= THE_NB_ROUND_SPLITS; ++aNodeIter)
    {
      const Standard_Real anAngle = NCollection_Lerp<Standard_Real>::Interpolate (0.0, -M_PI * 0.5, Standard_Real(aNodeIter) / Standard_Real(THE_NB_ROUND_SPLITS));
      theTris->AddVertex (gp_Pnt (aHSize.X() + aRadius * Cos (anAngle), -aHSize.Y() + aRadius * Sin (anAngle), 0.0).Transformed (theTrsf));
    }
    for (Standard_Integer aNodeIter = 0; aNodeIter <= THE_NB_ROUND_SPLITS; ++aNodeIter)
    {
      const Standard_Real anAngle = NCollection_Lerp<Standard_Real>::Interpolate (-M_PI * 0.5, -M_PI, Standard_Real(aNodeIter) / Standard_Real(THE_NB_ROUND_SPLITS));
      theTris->AddVertex (gp_Pnt (-aHSize.X() + aRadius * Cos (anAngle), -aHSize.Y() + aRadius * Sin (anAngle), 0.0).Transformed (theTrsf));
    }
    for (Standard_Integer aNodeIter = 0; aNodeIter <= THE_NB_ROUND_SPLITS; ++aNodeIter)
    {
      const Standard_Real anAngle = NCollection_Lerp<Standard_Real>::Interpolate (-M_PI, -M_PI * 1.5, Standard_Real(aNodeIter) / Standard_Real(THE_NB_ROUND_SPLITS));
      theTris->AddVertex (gp_Pnt (-aHSize.X() + aRadius * Cos (anAngle), aHSize.Y() + aRadius * Sin (anAngle), 0.0).Transformed (theTrsf));
    }

    // split triangle fan
    theTris->AddTriangleFanEdges (aVertFirst + 1, theTris->VertexNumber(), true);
  }
  else
  {
    theNbNodes += 4;
    theNbTris  += 2;
    if (theTris.IsNull())
    {
      return;
    }

    theTris->AddVertex (gp_Pnt (-aHSize.X(), -aHSize.Y(), 0.0).Transformed (theTrsf));
    theTris->AddVertex (gp_Pnt (-aHSize.X(),  aHSize.Y(), 0.0).Transformed (theTrsf));
    theTris->AddVertex (gp_Pnt ( aHSize.X(),  aHSize.Y(), 0.0).Transformed (theTrsf));
    theTris->AddVertex (gp_Pnt ( aHSize.X(), -aHSize.Y(), 0.0).Transformed (theTrsf));
    theTris->AddQuadTriangleEdges (aVertFirst + 1, aVertFirst + 2, aVertFirst + 3, aVertFirst + 4);
  }

  for (Standard_Integer aVertIter = aVertFirst + 1; aVertIter <= theTris->VertexNumber(); ++aVertIter)
  {
    theTris->SetVertexNormal (aVertIter, -aNorm);
  }
}

//=======================================================================
//function : createBoxPartTriangles
//purpose  :
//=======================================================================
void AIS_ViewCube::createBoxPartTriangles (const Handle(Graphic3d_ArrayOfTriangles)& theTris,
                                           Standard_Integer& theNbNodes,
                                           Standard_Integer& theNbTris,
                                           V3d_TypeOfOrientation theDir) const
{
  if (IsBoxSide (theDir))
  {
    createBoxSideTriangles (theTris, theNbNodes, theNbTris, theDir);
  }
  else if (IsBoxEdge (theDir)
        && myToDisplayEdges)
  {
    createBoxEdgeTriangles (theTris, theNbNodes, theNbTris, theDir);
  }
  else if (IsBoxCorner (theDir)
        && myToDisplayVertices)
  {
    createBoxCornerTriangles (theTris, theNbNodes, theNbTris, theDir);
  }
}

//=======================================================================
//function : createBoxSideTriangles
//purpose  :
//=======================================================================
void AIS_ViewCube::createBoxSideTriangles (const Handle(Graphic3d_ArrayOfTriangles)& theTris,
                                           Standard_Integer& theNbNodes,
                                           Standard_Integer& theNbTris,
                                           V3d_TypeOfOrientation theDirection) const
{
  const gp_Dir aDir = V3d::GetProjAxis (theDirection);
  const gp_Pnt aPos = aDir.XYZ() * (mySize * 0.5 + myBoxFacetExtension);
  const gp_Ax2 aPosition (aPos, aDir.Reversed());

  gp_Ax3 aSystem (aPosition);
  gp_Trsf aTrsf;
  aTrsf.SetTransformation (aSystem, gp_Ax3());

  createRoundRectangleTriangles (theTris, theNbNodes, theNbTris,
                                 gp_XY (mySize, mySize), myRoundRadius * mySize, aTrsf);
}

//=======================================================================
//function : createBoxEdgeTriangles
//purpose  :
//=======================================================================
void AIS_ViewCube::createBoxEdgeTriangles (const Handle(Graphic3d_ArrayOfTriangles)& theTris,
                                           Standard_Integer& theNbNodes,
                                           Standard_Integer& theNbTris,
                                           V3d_TypeOfOrientation theDirection) const
{
  const Standard_Real aThickness = Max (myBoxFacetExtension * gp_XY (1.0, 1.0).Modulus() - myBoxEdgeGap, myBoxEdgeMinSize);

  const gp_Dir aDir = V3d::GetProjAxis (theDirection);
  const gp_Pnt aPos = aDir.XYZ() * (mySize * 0.5 * gp_XY (1.0, 1.0).Modulus() + myBoxFacetExtension * Cos (M_PI_4));
  const gp_Ax2 aPosition (aPos, aDir.Reversed());

  gp_Ax3 aSystem (aPosition);
  gp_Trsf aTrsf;
  aTrsf.SetTransformation (aSystem, gp_Ax3());

  createRoundRectangleTriangles (theTris, theNbNodes, theNbTris,
                                 gp_XY (aThickness, mySize), myRoundRadius * mySize, aTrsf);
}

//=======================================================================
//function : createBoxCornerTriangles
//purpose  :
//=======================================================================
void AIS_ViewCube::createBoxCornerTriangles (const Handle(Graphic3d_ArrayOfTriangles)& theTris,
                                             Standard_Integer& theNbNodes,
                                             Standard_Integer& theNbTris,
                                             V3d_TypeOfOrientation theDir) const
{
  const Standard_Real aHSize = mySize * 0.5;
  const gp_Dir aDir = V3d::GetProjAxis (theDir);
  const gp_XYZ aHSizeDir = aDir.XYZ() * (aHSize * gp_Vec (1.0, 1.0, 1.0).Magnitude());
  const Standard_Integer aVertFirst = !theTris.IsNull() ? theTris->VertexNumber() : 0;
  if (myRoundRadius > 0.0)
  {
    theNbNodes += THE_NB_DISK_SLICES + 1;
    theNbTris  += THE_NB_DISK_SLICES + 1;
    if (theTris.IsNull())
    {
      return;
    }

    const Standard_Real anEdgeHWidth = myBoxFacetExtension * gp_XY (1.0, 1.0).Modulus() * 0.5;
    const Standard_Real aHeight = anEdgeHWidth * Sqrt (2.0 / 3.0); // tetrahedron height
    const gp_Pnt aPos = aDir.XYZ() * (aHSize * gp_Vec (1.0, 1.0, 1.0).Magnitude() + aHeight);
    const gp_Ax2 aPosition (aPos, aDir.Reversed());
    gp_Ax3 aSystem (aPosition);
    gp_Trsf aTrsf;
    aTrsf.SetTransformation (aSystem, gp_Ax3());
    const Standard_Real aRadius = Max (myBoxFacetExtension * 0.5 / Cos (M_PI_4), myCornerMinSize);

    theTris->AddVertex (gp_Pnt (0.0, 0.0, 0.0).Transformed (aTrsf));
    for (Standard_Integer aNodeIter = 0; aNodeIter < THE_NB_DISK_SLICES; ++aNodeIter)
    {
      const Standard_Real anAngle = NCollection_Lerp<Standard_Real>::Interpolate (2.0 * M_PI, 0.0, Standard_Real(aNodeIter) / Standard_Real(THE_NB_DISK_SLICES));
      theTris->AddVertex (gp_Pnt (aRadius * Cos (anAngle), aRadius * Sin (anAngle), 0.0).Transformed (aTrsf));
    }
    theTris->AddTriangleFanEdges (aVertFirst + 1, theTris->VertexNumber(), true);
  }
  else
  {
    theNbNodes += 3;
    theNbTris  += 1;
    if (theTris.IsNull())
    {
      return;
    }

    theTris->AddVertex (aHSizeDir + myBoxFacetExtension * gp_Dir (aDir.X(), 0.0, 0.0).XYZ());
    theTris->AddVertex (aHSizeDir + myBoxFacetExtension * gp_Dir (0.0, aDir.Y(), 0.0).XYZ());
    theTris->AddVertex (aHSizeDir + myBoxFacetExtension * gp_Dir (0.0, 0.0, aDir.Z()).XYZ());

    const gp_XYZ aNode1 = theTris->Vertice (aVertFirst + 1).XYZ();
    const gp_XYZ aNode2 = theTris->Vertice (aVertFirst + 2).XYZ();
    const gp_XYZ aNode3 = theTris->Vertice (aVertFirst + 3).XYZ();
    const gp_XYZ aNormTri = ((aNode2 - aNode1).Crossed (aNode3 - aNode1));
    if (aNormTri.Dot (aDir.XYZ()) < 0.0)
    {
      theTris->AddTriangleEdges (aVertFirst + 1, aVertFirst + 3, aVertFirst + 2);
    }
    else
    {
      theTris->AddTriangleEdges (aVertFirst + 1, aVertFirst + 2, aVertFirst + 3);
    }
  }

  for (Standard_Integer aVertIter = aVertFirst + 1; aVertIter <= theTris->VertexNumber(); ++aVertIter)
  {
    theTris->SetVertexNormal (aVertIter, aDir);
  }
}

//=======================================================================
//function : Compute
//purpose  :
//=======================================================================
void AIS_ViewCube::Compute (const Handle(PrsMgr_PresentationManager)& ,
                            const Handle(Prs3d_Presentation)& thePrs,
                            const Standard_Integer theMode)
{
  thePrs->SetInfiniteState (true);
  if (theMode != 0)
  {
    return;
  }

  const gp_Pnt aLocation = (mySize * 0.5 + myBoxFacetExtension + myAxesPadding) * gp_XYZ (-1.0, -1.0, -1.0);

  // Display axes
  if (myToDisplayAxes)
  {
    const Standard_Real anAxisSize = mySize + 2.0 * myBoxFacetExtension + myAxesPadding;
    const Handle(Prs3d_DatumAspect)& aDatumAspect = myDrawer->DatumAspect();
    for (Standard_Integer anAxisIter = Prs3d_DatumParts_XAxis; anAxisIter <= Prs3d_DatumParts_ZAxis; ++anAxisIter)
    {
      const Prs3d_DatumParts aPart = (Prs3d_DatumParts )anAxisIter;
      if (!aDatumAspect->DrawDatumPart (aPart))
      {
        continue;
      }

      gp_Ax1 anAx1;
      switch (aPart)
      {
        case Prs3d_DatumParts_XAxis: anAx1 = gp_Ax1 (aLocation, gp::DX()); break;
        case Prs3d_DatumParts_YAxis: anAx1 = gp_Ax1 (aLocation, gp::DY()); break;
        case Prs3d_DatumParts_ZAxis: anAx1 = gp_Ax1 (aLocation, gp::DZ()); break;
        default: break;
      }

      Handle(Graphic3d_Group) anAxisGroup = thePrs->NewGroup();
      anAxisGroup->SetClosed (true);
      anAxisGroup->SetGroupPrimitivesAspect (aDatumAspect->ShadingAspect (aPart)->Aspect());

      const Standard_Real anArrowLength = 0.2 * anAxisSize;
      Handle(Graphic3d_ArrayOfTriangles) aTriangleArray = Prs3d_Arrow::DrawShaded (anAx1, myAxesRadius, anAxisSize, myAxesConeRadius, anArrowLength, THE_NB_ARROW_FACETTES);
      anAxisGroup->AddPrimitiveArray (aTriangleArray);

      TCollection_AsciiString anAxisLabel;
      if (aDatumAspect->ToDrawLabels()
      &&  myAxesLabels.Find (aPart, anAxisLabel)
      && !anAxisLabel.IsEmpty())
      {
        Handle(Graphic3d_Group) anAxisLabelGroup = thePrs->NewGroup();
        gp_Pnt aTextOrigin = anAx1.Location().Translated (gp_Vec (anAx1.Direction().X() * (anAxisSize + anArrowLength),
                                                                  anAx1.Direction().Y() * (anAxisSize + anArrowLength),
                                                                  anAx1.Direction().Z() * (anAxisSize + anArrowLength)));
        Prs3d_Text::Draw (anAxisLabelGroup, aDatumAspect->TextAspect (aPart), TCollection_ExtendedString (anAxisLabel), aTextOrigin);
      }
    }

    // Display center
    {
      Handle(Graphic3d_Group) aGroup = thePrs->NewGroup();
      aGroup->SetClosed (true);
      Handle(Prs3d_ShadingAspect) anAspectCen = new Prs3d_ShadingAspect();
      anAspectCen->SetColor (Quantity_NOC_WHITE);
      aGroup->SetGroupPrimitivesAspect (anAspectCen->Aspect());
      Prs3d_ToolSphere aTool (myAxesSphereRadius, THE_NB_DISK_SLICES, THE_NB_DISK_SLICES);
      gp_Trsf aTrsf;
      aTrsf.SetTranslation (gp_Vec (gp::Origin(), aLocation));
      Handle(Graphic3d_ArrayOfTriangles) aCenterArray;
      aTool.FillArray (aCenterArray, aTrsf);
      aGroup->AddPrimitiveArray (aCenterArray);
    }
  }

  // Display box sides
  {
    Standard_Integer aNbNodes = 0, aNbTris = 0;
    for (Standard_Integer aPartIter = V3d_Xpos; aPartIter <= Standard_Integer(V3d_Zneg); ++aPartIter)
    {
      createBoxPartTriangles (Handle(Graphic3d_ArrayOfTriangles)(), aNbNodes, aNbTris, (V3d_TypeOfOrientation )aPartIter);
    }
    if (aNbNodes > 0)
    {
      Handle(Graphic3d_ArrayOfTriangles) aTris = new Graphic3d_ArrayOfTriangles (aNbNodes, aNbTris * 3, Graphic3d_ArrayFlags_VertexNormal);
      Handle(Graphic3d_ArrayOfSegments) aSegs;
      if (myDrawer->FaceBoundaryDraw())
      {
        aSegs = new Graphic3d_ArrayOfSegments (aNbNodes, aNbNodes * 2, Graphic3d_ArrayFlags_None);
      }
      aNbNodes = aNbTris = 0;
      for (Standard_Integer aPartIter = V3d_Xpos; aPartIter <= Standard_Integer(V3d_Zneg); ++aPartIter)
      {
        Standard_Integer aTriNodesFrom = aTris->VertexNumber();
        const Standard_Integer aTriFrom = aNbTris;
        createBoxPartTriangles (aTris, aNbNodes, aNbTris, (V3d_TypeOfOrientation )aPartIter);
        if (aSegs.IsNull())
        {
          continue;
        }

        const Standard_Integer aFirstNode = aSegs->VertexNumber();
        for (Standard_Integer aVertIter = (aNbTris - aTriFrom) > 2 ? aTriNodesFrom + 2 : aTriNodesFrom + 1; // skip triangle fan center
             aVertIter <= aTris->VertexNumber(); ++aVertIter)
        {
          aSegs->AddVertex (aTris->Vertice (aVertIter));
        }
        aSegs->AddPolylineEdges (aFirstNode + 1, aSegs->VertexNumber(), true);
      }

      {
        Handle(Graphic3d_Group) aGroupSides = thePrs->NewGroup();
        aGroupSides->SetClosed (true);
        aGroupSides->SetGroupPrimitivesAspect (myDrawer->ShadingAspect()->Aspect());
        aGroupSides->AddPrimitiveArray (aTris);
      }

      if (!aSegs.IsNull())
      {
        Handle(Graphic3d_Group) aGroupSegs = thePrs->NewGroup();
        aGroupSegs->SetGroupPrimitivesAspect (myDrawer->FaceBoundaryAspect()->Aspect());
        aGroupSegs->AddPrimitiveArray (aSegs);
      }
    }

    // Display box sides labels
    Handle(Graphic3d_Group) aTextGroup = thePrs->NewGroup();
    aTextGroup->SetGroupPrimitivesAspect (myDrawer->TextAspect()->Aspect());
    for (Standard_Integer aPartIter = V3d_Xpos; aPartIter <= Standard_Integer(V3d_Zneg); ++aPartIter)
    {
      const V3d_TypeOfOrientation anOrient = (V3d_TypeOfOrientation )aPartIter;

      TCollection_AsciiString aLabel;
      if (!myBoxSideLabels.Find (anOrient, aLabel)
        || aLabel.IsEmpty())
      {
        continue;
      }

      const gp_Dir aDir = V3d::GetProjAxis (anOrient);
      gp_Dir anUp = myIsYup ? gp::DY() : gp::DZ();
      if (myIsYup)
      {
        if (anOrient == V3d_Ypos
          || anOrient == V3d_Yneg)
        {
          anUp = -gp::DZ();
        }
      }
      else
      {
        if (anOrient == V3d_Zpos)
        {
          anUp = gp::DY();
        }
        else if (anOrient == V3d_Zneg)
        {
          anUp = -gp::DY();
        }
      }

      const Standard_Real anOffset = 2.0; // extra offset to avoid overlapping with triangulation
      const gp_Pnt aPos = aDir.XYZ() * (mySize * 0.5 + myBoxFacetExtension + anOffset);
      const gp_Ax2 aPosition (aPos, aDir, anUp.Crossed (aDir));

      Handle(Graphic3d_Text) aText = new Graphic3d_Text ((Standard_ShortReal)myDrawer->TextAspect()->Height());
      aText->SetText (aLabel);
      aText->SetOrientation (aPosition);
      aText->SetOwnAnchorPoint (false);
      aText->SetHorizontalAlignment(myDrawer->TextAspect()->HorizontalJustification());
      aText->SetVerticalAlignment  (myDrawer->TextAspect()->VerticalJustification());
      aTextGroup->AddText (aText);
    }
  }

  // Display box edges
  {
    Standard_Integer aNbNodes = 0, aNbTris = 0;
    for (Standard_Integer aPartIter = V3d_XposYpos; aPartIter <= Standard_Integer(V3d_YposZneg); ++aPartIter)
    {
      createBoxPartTriangles (Handle(Graphic3d_ArrayOfTriangles)(), aNbNodes, aNbTris, (V3d_TypeOfOrientation )aPartIter);
    }
    if (aNbNodes > 0)
    {
      Handle(Graphic3d_ArrayOfTriangles) aTris = new Graphic3d_ArrayOfTriangles (aNbNodes, aNbTris * 3, Graphic3d_ArrayFlags_VertexNormal);
      aNbNodes = aNbTris = 0;
      for (Standard_Integer aPartIter = V3d_XposYpos; aPartIter <= Standard_Integer(V3d_YposZneg); ++aPartIter)
      {
        const V3d_TypeOfOrientation anOrient = (V3d_TypeOfOrientation )aPartIter;
        createBoxPartTriangles (aTris, aNbNodes, aNbTris, anOrient);
      }

      Handle(Graphic3d_Group) aGroupEdges = thePrs->NewGroup();
      aGroupEdges->SetClosed (true);
      aGroupEdges->SetGroupPrimitivesAspect (myBoxEdgeAspect->Aspect());
      aGroupEdges->AddPrimitiveArray (aTris);
    }
  }

  // Display box corners
  {
    Standard_Integer aNbNodes = 0, aNbTris = 0;
    for (Standard_Integer aPartIter = V3d_XposYposZpos; aPartIter <= Standard_Integer(V3d_XnegYnegZneg); ++aPartIter)
    {
      createBoxPartTriangles (Handle(Graphic3d_ArrayOfTriangles)(), aNbNodes, aNbTris, (V3d_TypeOfOrientation )aPartIter);
    }
    if (aNbNodes > 0)
    {
      Handle(Graphic3d_ArrayOfTriangles) aTris = new Graphic3d_ArrayOfTriangles (aNbNodes, aNbTris * 3, Graphic3d_ArrayFlags_VertexNormal);
      aNbNodes = aNbTris = 0;
      for (Standard_Integer aPartIter = V3d_XposYposZpos; aPartIter <= Standard_Integer(V3d_XnegYnegZneg); ++aPartIter)
      {
        const V3d_TypeOfOrientation anOrient = (V3d_TypeOfOrientation )aPartIter;
        createBoxPartTriangles (aTris, aNbNodes, aNbTris, anOrient);
      }

      Handle(Graphic3d_Group) aGroupCorners = thePrs->NewGroup();
      aGroupCorners->SetClosed (true);
      aGroupCorners->SetGroupPrimitivesAspect (myBoxCornerAspect->Aspect());
      aGroupCorners->AddPrimitiveArray (aTris);
    }
  }
}

//=======================================================================
//function : ComputeSelection
//purpose  :
//=======================================================================
void AIS_ViewCube::ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                     const Standard_Integer theMode)
{
  if (theMode != 0)
  {
    return;
  }

  for (Standard_Integer aPartIter = 0; aPartIter <= Standard_Integer(V3d_XnegYnegZneg); ++aPartIter)
  {
    const V3d_TypeOfOrientation anOri = (V3d_TypeOfOrientation )aPartIter;
    Standard_Integer aNbNodes = 0, aNbTris = 0;
    createBoxPartTriangles (Handle(Graphic3d_ArrayOfTriangles)(), aNbNodes, aNbTris, anOri);
    if (aNbNodes <= 0)
    {
      continue;
    }

    Handle(Graphic3d_ArrayOfTriangles) aTris = new Graphic3d_ArrayOfTriangles (aNbNodes, aNbTris * 3, Graphic3d_ArrayFlags_None);
    aNbNodes = aNbTris = 0;
    createBoxPartTriangles (aTris, aNbNodes, aNbTris, anOri);

    Standard_Integer aSensitivity = 2;
    if (IsBoxCorner (anOri))
    {
      aSensitivity = 8;
    }
    else if (IsBoxEdge (anOri))
    {
      aSensitivity = 4;
    }
    Handle(AIS_ViewCubeOwner) anOwner = new AIS_ViewCubeOwner (this, anOri);
    Handle(AIS_ViewCubeSensitive) aTriSens = new AIS_ViewCubeSensitive (anOwner, aTris);
    aTriSens->SetSensitivityFactor (aSensitivity);
    theSelection->Add (aTriSens);
  }
}

//=======================================================================
//function : Duration
//purpose  :
//=======================================================================
Standard_Real AIS_ViewCube::Duration() const
{
  return myViewAnimation->OwnDuration();
}

//=======================================================================
//function : SetDuration
//purpose  :
//=======================================================================
void AIS_ViewCube::SetDuration (Standard_Real theDurationSec)
{
  myViewAnimation->SetOwnDuration (theDurationSec);
}

//=======================================================================
//function : HasAnimation
//purpose  :
//=======================================================================
Standard_Boolean AIS_ViewCube::HasAnimation() const
{
  return !myViewAnimation->IsStopped();
}

//=======================================================================
//function : viewFitAll
//purpose  :
//=======================================================================
void AIS_ViewCube::viewFitAll (const Handle(V3d_View)& theView,
                               const Handle(Graphic3d_Camera)& theCamera)
{
  Bnd_Box aBndBox = myToFitSelected ? GetContext()->BoundingBoxOfSelection (theView) : theView->View()->MinMaxValues();
  if (aBndBox.IsVoid()
   && myToFitSelected)
  {
    aBndBox = theView->View()->MinMaxValues();
  }
  if (!aBndBox.IsVoid())
  {
    theView->FitMinMax (theCamera, aBndBox, 0.01, 10.0 * Precision::Confusion());
  }
}

//=======================================================================
//function : StartAnimation
//purpose  :
//=======================================================================
void AIS_ViewCube::StartAnimation (const Handle(AIS_ViewCubeOwner)& theOwner)
{
  Handle(V3d_View) aView = GetContext()->LastActiveView();
  if (theOwner.IsNull()
   || aView.IsNull())
  {
    return;
  }

  myStartState->Copy (aView->Camera());
  myEndState  ->Copy (aView->Camera());

  {
    {
      Handle(Graphic3d_Camera) aBackupCamera = aView->Camera();
      const bool wasImmediateUpdate = aView->SetImmediateUpdate (false);
      aView->SetCamera (myEndState);
      aView->SetProj (theOwner->MainOrientation(), myIsYup);
      aView->SetCamera (aBackupCamera);
      aView->SetImmediateUpdate (wasImmediateUpdate);
    }

    const gp_Dir aNewDir = myEndState->Direction();
    if (!myToResetCameraUp
     && !aNewDir.IsEqual (myStartState->Direction(), Precision::Angular()))
    {
      // find the Up direction closest to current instead of default one
      const gp_Ax1 aNewDirAx1 (gp::Origin(), aNewDir);
      const gp_Dir anOldUp = myStartState->Up();
      const gp_Dir anUpList[4] =
      {
        myEndState->Up(),
        myEndState->Up().Rotated (aNewDirAx1, M_PI_2),
        myEndState->Up().Rotated (aNewDirAx1, M_PI),
        myEndState->Up().Rotated (aNewDirAx1, M_PI * 1.5),
      };

      Standard_Real aBestAngle = Precision::Infinite();
      gp_Dir anUpBest;
      for (Standard_Integer anUpIter = 0; anUpIter < 4; ++anUpIter)
      {
        Standard_Real anAngle = anUpList[anUpIter].Angle (anOldUp);
        if (aBestAngle > anAngle)
        {
          aBestAngle = anAngle;
          anUpBest = anUpList[anUpIter];
        }
      }
      myEndState->SetUp (anUpBest);
    }

    viewFitAll (aView, myEndState);
  }

  myViewAnimation->SetView (aView);
  myViewAnimation->SetCameraStart (myStartState);
  myViewAnimation->SetCameraEnd   (myEndState);
  myViewAnimation->StartTimer (0.0, 1.0, true, false);
}

//=======================================================================
//function : updateAnimation
//purpose  :
//=======================================================================
Standard_Boolean AIS_ViewCube::updateAnimation()
{
  const Standard_Real aPts = myViewAnimation->UpdateTimer();
  if (aPts >= myViewAnimation->OwnDuration())
  {
    myViewAnimation->Stop();
    onAnimationFinished();
    myViewAnimation->SetView (Handle(V3d_View)());
    return Standard_False;
  }
  return Standard_True;
}

//=======================================================================
//function : UpdateAnimation
//purpose  :
//=======================================================================
Standard_Boolean AIS_ViewCube::UpdateAnimation (const Standard_Boolean theToUpdate)
{
  Handle(V3d_View) aView = myViewAnimation->View();
  if (!HasAnimation()
   || !updateAnimation())
  {
    return Standard_False;
  }

  if (theToUpdate
  && !aView.IsNull())
  {
    aView->IsInvalidated() ? aView->Redraw() : aView->RedrawImmediate();
  }

  onAfterAnimation();
  return Standard_True;
}

//=======================================================================
//function : HandleClick
//purpose  :
//=======================================================================
void AIS_ViewCube::HandleClick (const Handle(AIS_ViewCubeOwner)& theOwner)
{
  if (!myToAutoStartAnim)
  {
    return;
  }

  StartAnimation (theOwner);
  if (!myIsFixedAnimation)
  {
    return;
  }
  for (; HasAnimation(); )
  {
    UpdateAnimation (true);
  }
}

//=======================================================================
//function : HilightOwnerWithColor
//purpose  :
//=======================================================================
void AIS_ViewCube::HilightOwnerWithColor (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                          const Handle(Prs3d_Drawer)& theStyle,
                                          const Handle(SelectMgr_EntityOwner)& theOwner)
{
  if (theOwner.IsNull()
  || !thePrsMgr->IsImmediateModeOn())
  {
    return;
  }

  const Graphic3d_ZLayerId aLayer = theStyle->ZLayer() != Graphic3d_ZLayerId_UNKNOWN ? theStyle->ZLayer() : myDrawer->ZLayer();
  const AIS_ViewCubeOwner* aCubeOwner = dynamic_cast<AIS_ViewCubeOwner* >(theOwner.get());

  Handle(Prs3d_Presentation) aHiPrs = GetHilightPresentation (thePrsMgr);
  aHiPrs->Clear();
  aHiPrs->CStructure()->ViewAffinity = myViewAffinity;
  aHiPrs->SetTransformPersistence (TransformPersistence());
  aHiPrs->SetZLayer (aLayer);

  {
    Handle(Graphic3d_Group) aGroup = aHiPrs->NewGroup();
    aGroup->SetGroupPrimitivesAspect (theStyle->ShadingAspect()->Aspect());
    Standard_Integer aNbNodes = 0, aNbTris = 0;
    createBoxPartTriangles (Handle(Graphic3d_ArrayOfTriangles)(), aNbNodes, aNbTris, aCubeOwner->MainOrientation());
    if (aNbNodes > 0)
    {
      Handle(Graphic3d_ArrayOfTriangles) aTris = new Graphic3d_ArrayOfTriangles (aNbNodes, aNbTris * 3, Graphic3d_ArrayFlags_None);
      aNbNodes = aNbTris = 0;
      createBoxPartTriangles (aTris, aNbNodes, aNbTris, aCubeOwner->MainOrientation());
      aGroup->AddPrimitiveArray (aTris);
    }
  }

  if (thePrsMgr->IsImmediateModeOn())
  {
    thePrsMgr->AddToImmediateList (aHiPrs);
  }
}

//=======================================================================
//function : HilightSelected
//purpose  :
//=======================================================================
void AIS_ViewCube::HilightSelected (const Handle(PrsMgr_PresentationManager)& ,
                                    const SelectMgr_SequenceOfOwner& theSeq)
{
  // this method should never be called since AIS_InteractiveObject::HandleClick() has been overridden
  if (theSeq.Size() == 1)
  {
    //HandleClick (Handle(AIS_ViewCubeOwner)::DownCast (theSeq.First()));
  }
}
