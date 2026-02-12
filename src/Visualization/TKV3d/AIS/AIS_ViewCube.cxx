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
#include <NCollection_Sequence.hxx>
#include <V3d.hxx>
#include <V3d_View.hxx>
class SelectMgr_EntityOwner;

IMPLEMENT_STANDARD_RTTIEXT(AIS_ViewCube, AIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(AIS_ViewCubeOwner, SelectMgr_EntityOwner)
IMPLEMENT_STANDARD_RTTIEXT(AIS_ViewCubeSensitive, Select3D_SensitivePrimitiveArray)

namespace
{
constexpr int THE_NB_ROUND_SPLITS   = 8;
constexpr int THE_NB_DISK_SLICES    = 20;
constexpr int THE_NB_ARROW_FACETTES = 20;

//! Return the number of non-zero components.
static int nbDirectionComponents(const gp_Dir& theDir)
{
  int aNbComps = 0;
  for (int aCompIter = 1; aCompIter <= 3; ++aCompIter)
  {
    if (std::abs(theDir.Coord(aCompIter)) > gp::Resolution())
    {
      ++aNbComps;
    }
  }
  return aNbComps;
}
} // namespace

//=================================================================================================

AIS_ViewCubeSensitive::AIS_ViewCubeSensitive(const occ::handle<SelectMgr_EntityOwner>& theOwner,
                                             const occ::handle<Graphic3d_ArrayOfTriangles>& theTris)
    : Select3D_SensitivePrimitiveArray(theOwner)
{
  InitTriangulation(theTris->Attributes(), theTris->Indices(), TopLoc_Location());
}

//=================================================================================================

bool AIS_ViewCubeSensitive::Matches(SelectBasics_SelectingVolumeManager& theMgr,
                                    SelectBasics_PickResult&             thePickResult)
{
  return isValidRay(theMgr) && Select3D_SensitivePrimitiveArray::Matches(theMgr, thePickResult);
}

//=================================================================================================

bool AIS_ViewCubeSensitive::isValidRay(const SelectBasics_SelectingVolumeManager& theMgr) const
{
  if (theMgr.GetActiveSelectionType() != SelectMgr_SelectionType_Point)
  {
    // disallow rectangular selection
    return false;
  }

  if (AIS_ViewCubeOwner* anOwner = dynamic_cast<AIS_ViewCubeOwner*>(myOwnerId.get()))
  {
    const double anAngleToler = 10.0 * M_PI / 180.0;
    const gp_Dir aRay         = theMgr.GetViewRayDirection();
    const gp_Dir aDir         = V3d::GetProjAxis(anOwner->MainOrientation());
    return !aRay.IsNormal(aDir, anAngleToler);
  }
  return true;
}

//=================================================================================================

bool AIS_ViewCube::IsBoxSide(V3d_TypeOfOrientation theOrient)
{
  return nbDirectionComponents(V3d::GetProjAxis(theOrient)) == 1;
}

//=================================================================================================

bool AIS_ViewCube::IsBoxEdge(V3d_TypeOfOrientation theOrient)
{
  return nbDirectionComponents(V3d::GetProjAxis(theOrient)) == 2;
}

//=================================================================================================

bool AIS_ViewCube::IsBoxCorner(V3d_TypeOfOrientation theOrient)
{
  return nbDirectionComponents(V3d::GetProjAxis(theOrient)) == 3;
}

//=================================================================================================

AIS_ViewCube::AIS_ViewCube()
    : myBoxEdgeAspect(new Prs3d_ShadingAspect()),
      myBoxCornerAspect(new Prs3d_ShadingAspect()),
      mySize(1.0),
      myBoxEdgeMinSize(2.0),
      myBoxEdgeGap(0.0),
      myBoxFacetExtension(1.0),
      myAxesPadding(1.0),
      myAxesRadius(1.0),
      myAxesConeRadius(3.0),
      myAxesSphereRadius(4.0),
      myCornerMinSize(2.0),
      myRoundRadius(0.0),
      myToDisplayAxes(true),
      myToDisplayEdges(true),
      myToDisplayVertices(true),
      myIsYup(false),
      myViewAnimation(new AIS_AnimationCamera("AIS_ViewCube", occ::handle<V3d_View>())),
      myStartState(new Graphic3d_Camera()),
      myEndState(new Graphic3d_Camera()),
      myToAutoStartAnim(true),
      myIsFixedAnimation(true),
      myToFitSelected(true),
      myToResetCameraUp(false)
{
  myViewAnimation->SetOwnDuration(0.5);
  myInfiniteState = true;
  myIsMutable     = true;
  myDrawer->SetZLayer(Graphic3d_ZLayerId_Topmost);
  myTransformPersistence = new Graphic3d_TransformPers(Graphic3d_TMF_TriedronPers,
                                                       Aspect_TOTP_LEFT_LOWER,
                                                       NCollection_Vec2<int>(100, 100));

  myDrawer->SetTextAspect(new Prs3d_TextAspect());
  myDrawer->SetShadingAspect(new Prs3d_ShadingAspect());

  myDynHilightDrawer = new Prs3d_Drawer();
  myDynHilightDrawer->SetLink(myDrawer);
  myDynHilightDrawer->SetShadingAspect(new Prs3d_ShadingAspect());

  setDefaultAttributes();
  setDefaultHighlightAttributes();

  // setup default labels
  myBoxSideLabels.Bind(V3d_TypeOfOrientation_Zup_Front, "FRONT");
  myBoxSideLabels.Bind(V3d_TypeOfOrientation_Zup_Back, "BACK");
  myBoxSideLabels.Bind(V3d_TypeOfOrientation_Zup_Top, "TOP");
  myBoxSideLabels.Bind(V3d_TypeOfOrientation_Zup_Bottom, "BOTTOM");
  myBoxSideLabels.Bind(V3d_TypeOfOrientation_Zup_Left, "LEFT");
  myBoxSideLabels.Bind(V3d_TypeOfOrientation_Zup_Right, "RIGHT");

  myAxesLabels.Bind(Prs3d_DatumParts_XAxis, "X");
  myAxesLabels.Bind(Prs3d_DatumParts_YAxis, "Y");
  myAxesLabels.Bind(Prs3d_DatumParts_ZAxis, "Z");

  // define default size
  SetSize(70.0);
}

//=================================================================================================

void AIS_ViewCube::setDefaultAttributes()
{
  myDrawer->TextAspect()->SetHorizontalJustification(Graphic3d_HTA_CENTER);
  myDrawer->TextAspect()->SetVerticalJustification(Graphic3d_VTA_CENTER);
  myDrawer->TextAspect()->SetColor(Quantity_NOC_BLACK);
  myDrawer->TextAspect()->SetFont(Font_NOF_SANS_SERIF);
  myDrawer->TextAspect()->SetHeight(16.0);
  // clang-format off
  myDrawer->TextAspect()->Aspect()->SetTextZoomable (true); // the whole object is drawn within transformation-persistence
  // this should be forced back-face culling regardless Closed flag
  // clang-format on
  myDrawer->TextAspect()->Aspect()->SetFaceCulling(Graphic3d_TypeOfBackfacingModel_BackCulled);

  Graphic3d_MaterialAspect aMat(Graphic3d_NameOfMaterial_UserDefined);
  aMat.SetColor(Quantity_NOC_WHITE);
  aMat.SetAmbientColor(Quantity_NOC_GRAY60);

  const occ::handle<Graphic3d_AspectFillArea3d>& aShading = myDrawer->ShadingAspect()->Aspect();
  aShading->SetInteriorStyle(Aspect_IS_SOLID);
  // this should be forced back-face culling regardless Closed flag
  aShading->SetFaceCulling(Graphic3d_TypeOfBackfacingModel_BackCulled);
  aShading->SetInteriorColor(aMat.Color());
  aShading->SetFrontMaterial(aMat);
  myDrawer->SetFaceBoundaryDraw(false);

  *myBoxEdgeAspect->Aspect() = *aShading;
  myBoxEdgeAspect->SetColor(Quantity_NOC_GRAY30);
  *myBoxCornerAspect->Aspect() = *aShading;
  myBoxCornerAspect->SetColor(Quantity_NOC_GRAY30);
}

//=================================================================================================

void AIS_ViewCube::setDefaultHighlightAttributes()
{
  myDynHilightDrawer->SetShadingAspect(new Prs3d_ShadingAspect());
  myDynHilightDrawer->ShadingAspect()->Aspect()->SetShadingModel(
    Graphic3d_TypeOfShadingModel_Unlit);
  myDynHilightDrawer->ShadingAspect()->SetColor(Quantity_NOC_CYAN1);
  myDynHilightDrawer->SetZLayer(Graphic3d_ZLayerId_Topmost);
  myDynHilightDrawer->SetColor(Quantity_NOC_CYAN1);
}

//=================================================================================================

void AIS_ViewCube::SetYup(bool theIsYup, bool theToUpdateLabels)
{
  if (myIsYup == theIsYup)
  {
    return;
  }

  myIsYup = theIsYup;

  static const V3d_TypeOfOrientation THE_ZUP_ORI_LIST[6] = {V3d_TypeOfOrientation_Zup_Front,
                                                            V3d_TypeOfOrientation_Zup_Back,
                                                            V3d_TypeOfOrientation_Zup_Top,
                                                            V3d_TypeOfOrientation_Zup_Bottom,
                                                            V3d_TypeOfOrientation_Zup_Left,
                                                            V3d_TypeOfOrientation_Zup_Right};
  static const V3d_TypeOfOrientation THE_YUP_ORI_LIST[6] = {V3d_TypeOfOrientation_Yup_Front,
                                                            V3d_TypeOfOrientation_Yup_Back,
                                                            V3d_TypeOfOrientation_Yup_Top,
                                                            V3d_TypeOfOrientation_Yup_Bottom,
                                                            V3d_TypeOfOrientation_Yup_Left,
                                                            V3d_TypeOfOrientation_Yup_Right};
  if (theToUpdateLabels)
  {
    NCollection_Array1<TCollection_AsciiString> aLabels(0, 5);
    for (int aLabelIter = 0; aLabelIter < 6; ++aLabelIter)
    {
      myBoxSideLabels.Find(!myIsYup ? THE_YUP_ORI_LIST[aLabelIter] : THE_ZUP_ORI_LIST[aLabelIter],
                           aLabels.ChangeValue(aLabelIter));
    }
    for (int aLabelIter = 0; aLabelIter < 6; ++aLabelIter)
    {
      myBoxSideLabels.Bind(myIsYup ? THE_YUP_ORI_LIST[aLabelIter] : THE_ZUP_ORI_LIST[aLabelIter],
                           aLabels.Value(aLabelIter));
    }
  }

  SetToUpdate();
}

//=================================================================================================

void AIS_ViewCube::ResetStyles()
{
  UnsetAttributes();
  UnsetHilightAttributes();

  myBoxEdgeMinSize = 2.0;
  myCornerMinSize  = 2.0;
  myBoxEdgeGap     = 0.0;
  myRoundRadius    = 0.0;

  myToDisplayAxes     = true;
  myToDisplayEdges    = true;
  myToDisplayVertices = true;

  myBoxFacetExtension = 1.0;
  myAxesPadding       = 1.0;
  SetSize(70.0);
}

//=================================================================================================

void AIS_ViewCube::SetSize(double theValue, bool theToAdaptAnother)
{
  const bool isNewSize = std::abs(mySize - theValue) > Precision::Confusion();
  mySize               = theValue;
  if (theToAdaptAnother)
  {
    if (myBoxFacetExtension > 0.0)
    {
      SetBoxFacetExtension(mySize * 0.15);
    }
    if (myAxesPadding > 0.0)
    {
      SetAxesPadding(mySize * 0.1);
    }
    SetFontHeight(mySize * 0.16);
  }
  if (isNewSize)
  {
    SetToUpdate();
  }
}

//=================================================================================================

void AIS_ViewCube::SetRoundRadius(const double theValue)
{
  Standard_OutOfRange_Raise_if(theValue < 0.0 || theValue > 0.5,
                               "AIS_ViewCube::SetRoundRadius(): theValue should be in [0; 0.5]");
  if (std::abs(myRoundRadius - theValue) > Precision::Confusion())
  {
    myRoundRadius = theValue;
    SetToUpdate();
  }
}

//=================================================================================================

void AIS_ViewCube::createRoundRectangleTriangles(
  const occ::handle<Graphic3d_ArrayOfTriangles>& theTris,
  int&                                           theNbNodes,
  int&                                           theNbTris,
  const gp_XY&                                   theSize,
  double                                         theRadius,
  const gp_Trsf&                                 theTrsf)
{
  const double aRadius = std::min(theRadius, std::min(theSize.X(), theSize.Y()) * 0.5);
  const gp_XY  aHSize(theSize.X() * 0.5 - aRadius, theSize.Y() * 0.5 - aRadius);
  const gp_Dir aNorm      = gp::DZ().Transformed(theTrsf);
  const int    aVertFirst = !theTris.IsNull() ? theTris->VertexNumber() : 0;
  if (aRadius > 0.0)
  {
    const int aNbNodes = (THE_NB_ROUND_SPLITS + 1) * 4 + 1;
    theNbNodes += aNbNodes;
    theNbTris += aNbNodes;
    if (theTris.IsNull())
    {
      return;
    }

    theTris->AddVertex(gp_Pnt(0.0, 0.0, 0.0).Transformed(theTrsf));
    for (int aNodeIter = 0; aNodeIter <= THE_NB_ROUND_SPLITS; ++aNodeIter)
    {
      const double anAngle =
        NCollection_Lerp<double>::Interpolate(M_PI * 0.5,
                                              0.0,
                                              double(aNodeIter) / double(THE_NB_ROUND_SPLITS));
      theTris->AddVertex(gp_Pnt(aHSize.X() + aRadius * std::cos(anAngle),
                                aHSize.Y() + aRadius * std::sin(anAngle),
                                0.0)
                           .Transformed(theTrsf));
    }
    for (int aNodeIter = 0; aNodeIter <= THE_NB_ROUND_SPLITS; ++aNodeIter)
    {
      const double anAngle =
        NCollection_Lerp<double>::Interpolate(0.0,
                                              -M_PI * 0.5,
                                              double(aNodeIter) / double(THE_NB_ROUND_SPLITS));
      theTris->AddVertex(gp_Pnt(aHSize.X() + aRadius * std::cos(anAngle),
                                -aHSize.Y() + aRadius * std::sin(anAngle),
                                0.0)
                           .Transformed(theTrsf));
    }
    for (int aNodeIter = 0; aNodeIter <= THE_NB_ROUND_SPLITS; ++aNodeIter)
    {
      const double anAngle =
        NCollection_Lerp<double>::Interpolate(-M_PI * 0.5,
                                              -M_PI,
                                              double(aNodeIter) / double(THE_NB_ROUND_SPLITS));
      theTris->AddVertex(gp_Pnt(-aHSize.X() + aRadius * std::cos(anAngle),
                                -aHSize.Y() + aRadius * std::sin(anAngle),
                                0.0)
                           .Transformed(theTrsf));
    }
    for (int aNodeIter = 0; aNodeIter <= THE_NB_ROUND_SPLITS; ++aNodeIter)
    {
      const double anAngle =
        NCollection_Lerp<double>::Interpolate(-M_PI,
                                              -M_PI * 1.5,
                                              double(aNodeIter) / double(THE_NB_ROUND_SPLITS));
      theTris->AddVertex(gp_Pnt(-aHSize.X() + aRadius * std::cos(anAngle),
                                aHSize.Y() + aRadius * std::sin(anAngle),
                                0.0)
                           .Transformed(theTrsf));
    }

    // split triangle fan
    theTris->AddTriangleFanEdges(aVertFirst + 1, theTris->VertexNumber(), true);
  }
  else
  {
    theNbNodes += 4;
    theNbTris += 2;
    if (theTris.IsNull())
    {
      return;
    }

    theTris->AddVertex(gp_Pnt(-aHSize.X(), -aHSize.Y(), 0.0).Transformed(theTrsf));
    theTris->AddVertex(gp_Pnt(-aHSize.X(), aHSize.Y(), 0.0).Transformed(theTrsf));
    theTris->AddVertex(gp_Pnt(aHSize.X(), aHSize.Y(), 0.0).Transformed(theTrsf));
    theTris->AddVertex(gp_Pnt(aHSize.X(), -aHSize.Y(), 0.0).Transformed(theTrsf));
    theTris->AddQuadTriangleEdges(aVertFirst + 1, aVertFirst + 2, aVertFirst + 3, aVertFirst + 4);
  }

  for (int aVertIter = aVertFirst + 1; aVertIter <= theTris->VertexNumber(); ++aVertIter)
  {
    theTris->SetVertexNormal(aVertIter, -aNorm);
  }
}

//=================================================================================================

void AIS_ViewCube::createBoxPartTriangles(const occ::handle<Graphic3d_ArrayOfTriangles>& theTris,
                                          int&                                           theNbNodes,
                                          int&                                           theNbTris,
                                          V3d_TypeOfOrientation theDir) const
{
  if (IsBoxSide(theDir))
  {
    createBoxSideTriangles(theTris, theNbNodes, theNbTris, theDir);
  }
  else if (IsBoxEdge(theDir) && myToDisplayEdges)
  {
    createBoxEdgeTriangles(theTris, theNbNodes, theNbTris, theDir);
  }
  else if (IsBoxCorner(theDir) && myToDisplayVertices)
  {
    createBoxCornerTriangles(theTris, theNbNodes, theNbTris, theDir);
  }
}

//=================================================================================================

void AIS_ViewCube::createBoxSideTriangles(const occ::handle<Graphic3d_ArrayOfTriangles>& theTris,
                                          int&                                           theNbNodes,
                                          int&                                           theNbTris,
                                          V3d_TypeOfOrientation theDirection) const
{
  const gp_Dir aDir = V3d::GetProjAxis(theDirection);
  const gp_Pnt aPos = aDir.XYZ() * (mySize * 0.5 + myBoxFacetExtension);
  const gp_Ax2 aPosition(aPos, aDir.Reversed());

  gp_Ax3  aSystem(aPosition);
  gp_Trsf aTrsf;
  aTrsf.SetTransformation(aSystem, gp_Ax3());

  createRoundRectangleTriangles(theTris,
                                theNbNodes,
                                theNbTris,
                                gp_XY(mySize, mySize),
                                myRoundRadius * mySize,
                                aTrsf);
}

//=================================================================================================

void AIS_ViewCube::createBoxEdgeTriangles(const occ::handle<Graphic3d_ArrayOfTriangles>& theTris,
                                          int&                                           theNbNodes,
                                          int&                                           theNbTris,
                                          V3d_TypeOfOrientation theDirection) const
{
  const double aThickness =
    std::max(myBoxFacetExtension * gp_XY(1.0, 1.0).Modulus() - myBoxEdgeGap, myBoxEdgeMinSize);

  const gp_Dir aDir = V3d::GetProjAxis(theDirection);
  const gp_Pnt aPos =
    aDir.XYZ()
    * (mySize * 0.5 * gp_XY(1.0, 1.0).Modulus() + myBoxFacetExtension * std::cos(M_PI_4));
  const gp_Ax2 aPosition(aPos, aDir.Reversed());

  gp_Ax3  aSystem(aPosition);
  gp_Trsf aTrsf;
  aTrsf.SetTransformation(aSystem, gp_Ax3());

  createRoundRectangleTriangles(theTris,
                                theNbNodes,
                                theNbTris,
                                gp_XY(aThickness, mySize),
                                myRoundRadius * mySize,
                                aTrsf);
}

//=================================================================================================

void AIS_ViewCube::createBoxCornerTriangles(const occ::handle<Graphic3d_ArrayOfTriangles>& theTris,
                                            int&                  theNbNodes,
                                            int&                  theNbTris,
                                            V3d_TypeOfOrientation theDir) const
{
  const double aHSize     = mySize * 0.5;
  const gp_Dir aDir       = V3d::GetProjAxis(theDir);
  const gp_XYZ aHSizeDir  = aDir.XYZ() * (aHSize * gp_Vec(1.0, 1.0, 1.0).Magnitude());
  const int    aVertFirst = !theTris.IsNull() ? theTris->VertexNumber() : 0;
  if (myRoundRadius > 0.0)
  {
    theNbNodes += THE_NB_DISK_SLICES + 1;
    theNbTris += THE_NB_DISK_SLICES + 1;
    if (theTris.IsNull())
    {
      return;
    }

    const double anEdgeHWidth = myBoxFacetExtension * gp_XY(1.0, 1.0).Modulus() * 0.5;
    const double aHeight      = anEdgeHWidth * std::sqrt(2.0 / 3.0); // tetrahedron height
    const gp_Pnt aPos         = aDir.XYZ() * (aHSize * gp_Vec(1.0, 1.0, 1.0).Magnitude() + aHeight);
    const gp_Ax2 aPosition(aPos, aDir.Reversed());
    gp_Ax3       aSystem(aPosition);
    gp_Trsf      aTrsf;
    aTrsf.SetTransformation(aSystem, gp_Ax3());
    const double aRadius = std::max(myBoxFacetExtension * 0.5 / std::cos(M_PI_4), myCornerMinSize);

    theTris->AddVertex(gp_Pnt(0.0, 0.0, 0.0).Transformed(aTrsf));
    for (int aNodeIter = 0; aNodeIter < THE_NB_DISK_SLICES; ++aNodeIter)
    {
      const double anAngle =
        NCollection_Lerp<double>::Interpolate(2.0 * M_PI,
                                              0.0,
                                              double(aNodeIter) / double(THE_NB_DISK_SLICES));
      theTris->AddVertex(
        gp_Pnt(aRadius * std::cos(anAngle), aRadius * std::sin(anAngle), 0.0).Transformed(aTrsf));
    }
    theTris->AddTriangleFanEdges(aVertFirst + 1, theTris->VertexNumber(), true);
  }
  else
  {
    theNbNodes += 3;
    theNbTris += 1;
    if (theTris.IsNull())
    {
      return;
    }

    theTris->AddVertex(aHSizeDir + myBoxFacetExtension * gp_Dir(aDir.X(), 0.0, 0.0).XYZ());
    theTris->AddVertex(aHSizeDir + myBoxFacetExtension * gp_Dir(0.0, aDir.Y(), 0.0).XYZ());
    theTris->AddVertex(aHSizeDir + myBoxFacetExtension * gp_Dir(0.0, 0.0, aDir.Z()).XYZ());

    const gp_XYZ aNode1   = theTris->Vertice(aVertFirst + 1).XYZ();
    const gp_XYZ aNode2   = theTris->Vertice(aVertFirst + 2).XYZ();
    const gp_XYZ aNode3   = theTris->Vertice(aVertFirst + 3).XYZ();
    const gp_XYZ aNormTri = ((aNode2 - aNode1).Crossed(aNode3 - aNode1));
    if (aNormTri.Dot(aDir.XYZ()) < 0.0)
    {
      theTris->AddTriangleEdges(aVertFirst + 1, aVertFirst + 3, aVertFirst + 2);
    }
    else
    {
      theTris->AddTriangleEdges(aVertFirst + 1, aVertFirst + 2, aVertFirst + 3);
    }
  }

  for (int aVertIter = aVertFirst + 1; aVertIter <= theTris->VertexNumber(); ++aVertIter)
  {
    theTris->SetVertexNormal(aVertIter, aDir);
  }
}

//=================================================================================================

void AIS_ViewCube::Compute(const occ::handle<PrsMgr_PresentationManager>&,
                           const occ::handle<Prs3d_Presentation>& thePrs,
                           const int                              theMode)
{
  thePrs->SetInfiniteState(true);
  if (theMode != 0)
  {
    return;
  }

  const gp_Pnt aLocation =
    (mySize * 0.5 + myBoxFacetExtension + myAxesPadding) * gp_XYZ(-1.0, -1.0, -1.0);

  // Display axes
  if (myToDisplayAxes)
  {
    const double anAxisSize = mySize + 2.0 * myBoxFacetExtension + myAxesPadding;
    const occ::handle<Prs3d_DatumAspect>& aDatumAspect = myDrawer->DatumAspect();
    for (int anAxisIter = Prs3d_DatumParts_XAxis; anAxisIter <= Prs3d_DatumParts_ZAxis;
         ++anAxisIter)
    {
      const Prs3d_DatumParts aPart = (Prs3d_DatumParts)anAxisIter;
      if (!aDatumAspect->DrawDatumPart(aPart))
      {
        continue;
      }

      gp_Ax1 anAx1;
      switch (aPart)
      {
        case Prs3d_DatumParts_XAxis:
          anAx1 = gp_Ax1(aLocation, gp::DX());
          break;
        case Prs3d_DatumParts_YAxis:
          anAx1 = gp_Ax1(aLocation, gp::DY());
          break;
        case Prs3d_DatumParts_ZAxis:
          anAx1 = gp_Ax1(aLocation, gp::DZ());
          break;
        default:
          break;
      }

      occ::handle<Graphic3d_Group> anAxisGroup = thePrs->NewGroup();
      anAxisGroup->SetClosed(true);
      anAxisGroup->SetGroupPrimitivesAspect(aDatumAspect->ShadingAspect(aPart)->Aspect());

      const double                            anArrowLength = 0.2 * anAxisSize;
      occ::handle<Graphic3d_ArrayOfTriangles> aTriangleArray =
        Prs3d_Arrow::DrawShaded(anAx1,
                                myAxesRadius,
                                anAxisSize,
                                myAxesConeRadius,
                                anArrowLength,
                                THE_NB_ARROW_FACETTES);
      anAxisGroup->AddPrimitiveArray(aTriangleArray);

      TCollection_AsciiString anAxisLabel;
      if (aDatumAspect->ToDrawLabels() && myAxesLabels.Find(aPart, anAxisLabel)
          && !anAxisLabel.IsEmpty())
      {
        occ::handle<Graphic3d_Group> anAxisLabelGroup = thePrs->NewGroup();
        gp_Pnt                       aTextOrigin =
          anAx1.Location().Translated(gp_Vec(anAx1.Direction().X() * (anAxisSize + anArrowLength),
                                             anAx1.Direction().Y() * (anAxisSize + anArrowLength),
                                             anAx1.Direction().Z() * (anAxisSize + anArrowLength)));
        Prs3d_Text::Draw(anAxisLabelGroup,
                         aDatumAspect->TextAspect(aPart),
                         TCollection_ExtendedString(anAxisLabel),
                         aTextOrigin);
      }
    }

    // Display center
    {
      occ::handle<Graphic3d_Group> aGroup = thePrs->NewGroup();
      aGroup->SetClosed(true);
      occ::handle<Prs3d_ShadingAspect> anAspectCen = new Prs3d_ShadingAspect();
      anAspectCen->SetColor(Quantity_NOC_WHITE);
      aGroup->SetGroupPrimitivesAspect(anAspectCen->Aspect());
      Prs3d_ToolSphere aTool(myAxesSphereRadius, THE_NB_DISK_SLICES, THE_NB_DISK_SLICES);
      gp_Trsf          aTrsf;
      aTrsf.SetTranslation(gp_Vec(gp::Origin(), aLocation));
      occ::handle<Graphic3d_ArrayOfTriangles> aCenterArray;
      aTool.FillArray(aCenterArray, aTrsf);
      aGroup->AddPrimitiveArray(aCenterArray);
    }
  }

  // Display box sides
  {
    int aNbNodes = 0, aNbTris = 0;
    for (int aPartIter = V3d_Xpos; aPartIter <= int(V3d_Zneg); ++aPartIter)
    {
      createBoxPartTriangles(occ::handle<Graphic3d_ArrayOfTriangles>(),
                             aNbNodes,
                             aNbTris,
                             (V3d_TypeOfOrientation)aPartIter);
    }
    if (aNbNodes > 0)
    {
      occ::handle<Graphic3d_ArrayOfTriangles> aTris =
        new Graphic3d_ArrayOfTriangles(aNbNodes, aNbTris * 3, Graphic3d_ArrayFlags_VertexNormal);
      occ::handle<Graphic3d_ArrayOfSegments> aSegs;
      if (myDrawer->FaceBoundaryDraw())
      {
        aSegs = new Graphic3d_ArrayOfSegments(aNbNodes, aNbNodes * 2, Graphic3d_ArrayFlags_None);
      }
      aNbNodes = aNbTris = 0;
      for (int aPartIter = V3d_Xpos; aPartIter <= int(V3d_Zneg); ++aPartIter)
      {
        int       aTriNodesFrom = aTris->VertexNumber();
        const int aTriFrom      = aNbTris;
        createBoxPartTriangles(aTris, aNbNodes, aNbTris, (V3d_TypeOfOrientation)aPartIter);
        if (aSegs.IsNull())
        {
          continue;
        }

        const int aFirstNode = aSegs->VertexNumber();
        // clang-format off
        for (int aVertIter = (aNbTris - aTriFrom) > 2 ? aTriNodesFrom + 2 : aTriNodesFrom + 1; // skip triangle fan center
                                                               // clang-format on
            aVertIter <= aTris->VertexNumber();
            ++aVertIter)
        {
          aSegs->AddVertex(aTris->Vertice(aVertIter));
        }
        aSegs->AddPolylineEdges(aFirstNode + 1, aSegs->VertexNumber(), true);
      }

      {
        occ::handle<Graphic3d_Group> aGroupSides = thePrs->NewGroup();
        aGroupSides->SetClosed(true);
        aGroupSides->SetGroupPrimitivesAspect(myDrawer->ShadingAspect()->Aspect());
        aGroupSides->AddPrimitiveArray(aTris);
      }

      if (!aSegs.IsNull())
      {
        occ::handle<Graphic3d_Group> aGroupSegs = thePrs->NewGroup();
        aGroupSegs->SetGroupPrimitivesAspect(myDrawer->FaceBoundaryAspect()->Aspect());
        aGroupSegs->AddPrimitiveArray(aSegs);
      }
    }

    // Display box sides labels
    occ::handle<Graphic3d_Group> aTextGroup = thePrs->NewGroup();
    aTextGroup->SetGroupPrimitivesAspect(myDrawer->TextAspect()->Aspect());
    for (int aPartIter = V3d_Xpos; aPartIter <= int(V3d_Zneg); ++aPartIter)
    {
      const V3d_TypeOfOrientation anOrient = (V3d_TypeOfOrientation)aPartIter;

      TCollection_AsciiString aLabel;
      if (!myBoxSideLabels.Find(anOrient, aLabel) || aLabel.IsEmpty())
      {
        continue;
      }

      const gp_Dir aDir = V3d::GetProjAxis(anOrient);
      gp_Dir       anUp = myIsYup ? gp::DY() : gp::DZ();
      if (myIsYup)
      {
        if (anOrient == V3d_Ypos || anOrient == V3d_Yneg)
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

      const double anOffset = 2.0; // extra offset to avoid overlapping with triangulation
      const gp_Pnt aPos     = aDir.XYZ() * (mySize * 0.5 + myBoxFacetExtension + anOffset);
      const gp_Ax2 aPosition(aPos, aDir, anUp.Crossed(aDir));

      occ::handle<Graphic3d_Text> aText =
        new Graphic3d_Text((float)myDrawer->TextAspect()->Height());
      aText->SetText(aLabel);
      aText->SetOrientation(aPosition);
      aText->SetOwnAnchorPoint(false);
      aText->SetHorizontalAlignment(myDrawer->TextAspect()->HorizontalJustification());
      aText->SetVerticalAlignment(myDrawer->TextAspect()->VerticalJustification());
      aTextGroup->AddText(aText);
    }
  }

  // Display box edges
  {
    int aNbNodes = 0, aNbTris = 0;
    for (int aPartIter = V3d_XposYpos; aPartIter <= int(V3d_YposZneg); ++aPartIter)
    {
      createBoxPartTriangles(occ::handle<Graphic3d_ArrayOfTriangles>(),
                             aNbNodes,
                             aNbTris,
                             (V3d_TypeOfOrientation)aPartIter);
    }
    if (aNbNodes > 0)
    {
      occ::handle<Graphic3d_ArrayOfTriangles> aTris =
        new Graphic3d_ArrayOfTriangles(aNbNodes, aNbTris * 3, Graphic3d_ArrayFlags_VertexNormal);
      aNbNodes = aNbTris = 0;
      for (int aPartIter = V3d_XposYpos; aPartIter <= int(V3d_YposZneg); ++aPartIter)
      {
        const V3d_TypeOfOrientation anOrient = (V3d_TypeOfOrientation)aPartIter;
        createBoxPartTriangles(aTris, aNbNodes, aNbTris, anOrient);
      }

      occ::handle<Graphic3d_Group> aGroupEdges = thePrs->NewGroup();
      aGroupEdges->SetClosed(true);
      aGroupEdges->SetGroupPrimitivesAspect(myBoxEdgeAspect->Aspect());
      aGroupEdges->AddPrimitiveArray(aTris);
    }
  }

  // Display box corners
  {
    int aNbNodes = 0, aNbTris = 0;
    for (int aPartIter = V3d_XposYposZpos; aPartIter <= int(V3d_XnegYnegZneg); ++aPartIter)
    {
      createBoxPartTriangles(occ::handle<Graphic3d_ArrayOfTriangles>(),
                             aNbNodes,
                             aNbTris,
                             (V3d_TypeOfOrientation)aPartIter);
    }
    if (aNbNodes > 0)
    {
      occ::handle<Graphic3d_ArrayOfTriangles> aTris =
        new Graphic3d_ArrayOfTriangles(aNbNodes, aNbTris * 3, Graphic3d_ArrayFlags_VertexNormal);
      aNbNodes = aNbTris = 0;
      for (int aPartIter = V3d_XposYposZpos; aPartIter <= int(V3d_XnegYnegZneg); ++aPartIter)
      {
        const V3d_TypeOfOrientation anOrient = (V3d_TypeOfOrientation)aPartIter;
        createBoxPartTriangles(aTris, aNbNodes, aNbTris, anOrient);
      }

      occ::handle<Graphic3d_Group> aGroupCorners = thePrs->NewGroup();
      aGroupCorners->SetClosed(true);
      aGroupCorners->SetGroupPrimitivesAspect(myBoxCornerAspect->Aspect());
      aGroupCorners->AddPrimitiveArray(aTris);
    }
  }
}

//=================================================================================================

void AIS_ViewCube::ComputeSelection(const occ::handle<SelectMgr_Selection>& theSelection,
                                    const int                               theMode)
{
  if (theMode != 0)
  {
    return;
  }

  for (int aPartIter = 0; aPartIter <= int(V3d_XnegYnegZneg); ++aPartIter)
  {
    const V3d_TypeOfOrientation anOri    = (V3d_TypeOfOrientation)aPartIter;
    int                         aNbNodes = 0, aNbTris = 0;
    createBoxPartTriangles(occ::handle<Graphic3d_ArrayOfTriangles>(), aNbNodes, aNbTris, anOri);
    if (aNbNodes <= 0)
    {
      continue;
    }

    occ::handle<Graphic3d_ArrayOfTriangles> aTris =
      new Graphic3d_ArrayOfTriangles(aNbNodes, aNbTris * 3, Graphic3d_ArrayFlags_None);
    aNbNodes = aNbTris = 0;
    createBoxPartTriangles(aTris, aNbNodes, aNbTris, anOri);

    int aSensitivity = 2;
    if (IsBoxCorner(anOri))
    {
      aSensitivity = 8;
    }
    else if (IsBoxEdge(anOri))
    {
      aSensitivity = 4;
    }
    occ::handle<AIS_ViewCubeOwner>     anOwner  = new AIS_ViewCubeOwner(this, anOri);
    occ::handle<AIS_ViewCubeSensitive> aTriSens = new AIS_ViewCubeSensitive(anOwner, aTris);
    aTriSens->SetSensitivityFactor(aSensitivity);
    theSelection->Add(aTriSens);
  }
}

//=================================================================================================

double AIS_ViewCube::Duration() const
{
  return myViewAnimation->OwnDuration();
}

//=================================================================================================

void AIS_ViewCube::SetDuration(double theDurationSec)
{
  myViewAnimation->SetOwnDuration(theDurationSec);
}

//=================================================================================================

bool AIS_ViewCube::HasAnimation() const
{
  return !myViewAnimation->IsStopped();
}

//=================================================================================================

void AIS_ViewCube::viewFitAll(const occ::handle<V3d_View>&         theView,
                              const occ::handle<Graphic3d_Camera>& theCamera)
{
  Bnd_Box aBndBox = myToFitSelected ? GetContext()->BoundingBoxOfSelection(theView)
                                    : theView->View()->MinMaxValues();
  if (aBndBox.IsVoid() && myToFitSelected)
  {
    aBndBox = theView->View()->MinMaxValues();
  }
  if (!aBndBox.IsVoid())
  {
    theView->FitMinMax(theCamera, aBndBox, 0.01, 10.0 * Precision::Confusion());
  }
}

//=================================================================================================

void AIS_ViewCube::StartAnimation(const occ::handle<AIS_ViewCubeOwner>& theOwner)
{
  occ::handle<V3d_View> aView = GetContext()->LastActiveView();
  if (theOwner.IsNull() || aView.IsNull())
  {
    return;
  }

  myStartState->Copy(aView->Camera());
  myEndState->Copy(aView->Camera());

  {
    {
      occ::handle<Graphic3d_Camera> aBackupCamera      = aView->Camera();
      const bool                    wasImmediateUpdate = aView->SetImmediateUpdate(false);
      aView->SetCamera(myEndState);
      aView->SetProj(theOwner->MainOrientation(), myIsYup);
      aView->SetCamera(aBackupCamera);
      aView->SetImmediateUpdate(wasImmediateUpdate);
    }

    const gp_Dir aNewDir = myEndState->Direction();
    if (!myToResetCameraUp && !aNewDir.IsEqual(myStartState->Direction(), Precision::Angular()))
    {
      // find the Up direction closest to current instead of default one
      const gp_Ax1 aNewDirAx1(gp::Origin(), aNewDir);
      const gp_Dir anOldUp     = myStartState->Up();
      const gp_Dir anUpList[4] = {
        myEndState->Up(),
        myEndState->Up().Rotated(aNewDirAx1, M_PI_2),
        myEndState->Up().Rotated(aNewDirAx1, M_PI),
        myEndState->Up().Rotated(aNewDirAx1, M_PI * 1.5),
      };

      double aBestAngle = Precision::Infinite();
      gp_Dir anUpBest;
      for (int anUpIter = 0; anUpIter < 4; ++anUpIter)
      {
        double anAngle = anUpList[anUpIter].Angle(anOldUp);
        if (aBestAngle > anAngle)
        {
          aBestAngle = anAngle;
          anUpBest   = anUpList[anUpIter];
        }
      }
      myEndState->SetUp(anUpBest);
    }

    viewFitAll(aView, myEndState);
  }

  myViewAnimation->SetView(aView);
  myViewAnimation->SetCameraStart(myStartState);
  myViewAnimation->SetCameraEnd(myEndState);
  myViewAnimation->StartTimer(0.0, 1.0, true, false);
}

//=================================================================================================

bool AIS_ViewCube::updateAnimation()
{
  const double aPts = myViewAnimation->UpdateTimer();
  if (aPts >= myViewAnimation->OwnDuration())
  {
    myViewAnimation->Stop();
    onAnimationFinished();
    myViewAnimation->SetView(occ::handle<V3d_View>());
    return false;
  }
  return true;
}

//=================================================================================================

bool AIS_ViewCube::UpdateAnimation(const bool theToUpdate)
{
  occ::handle<V3d_View> aView = myViewAnimation->View();
  if (!HasAnimation() || !updateAnimation())
  {
    return false;
  }

  if (theToUpdate && !aView.IsNull())
  {
    aView->IsInvalidated() ? aView->Redraw() : aView->RedrawImmediate();
  }

  onAfterAnimation();
  return true;
}

//=================================================================================================

void AIS_ViewCube::HandleClick(const occ::handle<AIS_ViewCubeOwner>& theOwner)
{
  if (!myToAutoStartAnim)
  {
    return;
  }

  StartAnimation(theOwner);
  if (!myIsFixedAnimation)
  {
    return;
  }
  for (; HasAnimation();)
  {
    UpdateAnimation(true);
  }
}

//=================================================================================================

void AIS_ViewCube::HilightOwnerWithColor(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                                         const occ::handle<Prs3d_Drawer>&               theStyle,
                                         const occ::handle<SelectMgr_EntityOwner>&      theOwner)
{
  if (theOwner.IsNull() || !thePrsMgr->IsImmediateModeOn())
  {
    return;
  }

  const AIS_ViewCubeOwner* aCubeOwner = dynamic_cast<AIS_ViewCubeOwner*>(theOwner.get());
  if (aCubeOwner == nullptr)
  {
    return;
  }

  const Graphic3d_ZLayerId aLayer =
    theStyle->ZLayer() != Graphic3d_ZLayerId_UNKNOWN ? theStyle->ZLayer() : myDrawer->ZLayer();
  occ::handle<Prs3d_Presentation> aHiPrs = GetHilightPresentation(thePrsMgr);
  aHiPrs->Clear();
  aHiPrs->CStructure()->ViewAffinity = myViewAffinity;
  aHiPrs->SetTransformPersistence(TransformPersistence());
  aHiPrs->SetZLayer(aLayer);

  occ::handle<Graphic3d_Group> aGroup = aHiPrs->NewGroup();
  aGroup->SetGroupPrimitivesAspect(theStyle->ShadingAspect()->Aspect());
  int aNbNodes = 0, aNbTris = 0;
  createBoxPartTriangles(occ::handle<Graphic3d_ArrayOfTriangles>(),
                         aNbNodes,
                         aNbTris,
                         aCubeOwner->MainOrientation());
  if (aNbNodes > 0)
  {
    occ::handle<Graphic3d_ArrayOfTriangles> aTris =
      new Graphic3d_ArrayOfTriangles(aNbNodes, aNbTris * 3, Graphic3d_ArrayFlags_None);
    aNbNodes = aNbTris = 0;
    createBoxPartTriangles(aTris, aNbNodes, aNbTris, aCubeOwner->MainOrientation());
    aGroup->AddPrimitiveArray(aTris);
  }

  if (thePrsMgr->IsImmediateModeOn())
  {
    thePrsMgr->AddToImmediateList(aHiPrs);
  }
}

//=================================================================================================

void AIS_ViewCube::HilightSelected(
  const occ::handle<PrsMgr_PresentationManager>&,
  const NCollection_Sequence<occ::handle<SelectMgr_EntityOwner>>& theSeq)
{
  // this method should never be called since AIS_InteractiveObject::HandleClick() has been
  // overridden
  if (theSeq.Size() == 1)
  {
    // HandleClick (Handle(AIS_ViewCubeOwner)::DownCast (theSeq.First()));
  }
}
