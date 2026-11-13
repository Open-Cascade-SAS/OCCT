// Copyright (c) 2026 OPEN CASCADE SAS
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

#include "BOPTest_Utilities.pxx"

#include <BRepAdaptor_Surface.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Defeaturing.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRep_Tool.hxx>

#include <TopExp.hxx>
#include <TopTools_ShapeMapHasher.hxx>

namespace
{
typedef NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> ShapeMap;

const double THE_WEDGE_ANGLE  = 15.0; // degrees between the two faces of the wedge
const double THE_WEDGE_LENGTH = 60.0;
const double THE_WEDGE_DEPTH  = 5.0;
const double THE_ROUND_RADIUS = 5.0;

const double THE_BLANK_HALF_SIZE = 30.0;
const double THE_BLANK_DEPTH     = 10.0;
const double THE_POCKET_RADIUS   = 20.0;
const double THE_POCKET_TILT     = 7.0; // degrees
const double THE_TOOL_HEIGHT     = 5.0;
const double THE_BLEND_RADIUS    = 0.5;

//! Expected number of blend faces of the model: four free form ones along the
//! elliptical top edge and the two cusps, two toroidal ones along the bottom
//! arc and two cylindrical ones along the straight bottom segment.
const int THE_BLEND_COUNT = 8;

int CountFaces(const TopoDS_Shape& theShape)
{
  ShapeMap aMap;
  TopExp::MapShapes(theShape, TopAbs_FACE, aMap);
  return aMap.Extent();
}

//! A wedge with a round on its sharp edge.
struct WedgeModel
{
  TopoDS_Shape myWedge;   //!< wedge with a sharp edge
  TopoDS_Shape myRounded; //!< the same wedge with the edge rounded
  TopoDS_Face  myRound;   //!< the round face of myRounded
};

//! Build a wedge of the given angle and put a round on its sharp edge.
//!
//! The round is short, so its bounding box is much smaller than the distance
//! from it to the sharp edge it replaces: the faces of the wedge have to be
//! extended by several times the size of the round before they meet each other
//! again. With the angle and the radius used here the round measures about 12,
//! while its tangency lines stand about 38 away from the edge.
WedgeModel MakeRoundedWedge()
{
  const double aHalfAngle = 0.5 * THE_WEDGE_ANGLE * M_PI / 180.0;

  BRepBuilderAPI_MakePolygon aProfile;
  aProfile.Add(gp_Pnt(0.0, 0.0, 0.0));
  aProfile.Add(gp_Pnt(THE_WEDGE_LENGTH, 0.0, 0.0));
  aProfile.Add(gp_Pnt(THE_WEDGE_LENGTH, 0.0, THE_WEDGE_LENGTH * tan(2.0 * aHalfAngle)));
  aProfile.Close();

  WedgeModel        aModel;
  const TopoDS_Face aFace = BRepBuilderAPI_MakeFace(aProfile.Wire()).Face();
  aModel.myWedge          = BRepPrimAPI_MakePrism(aFace, gp_Vec(0.0, THE_WEDGE_DEPTH, 0.0)).Shape();

  // The sharp edge of the wedge runs along Y through the origin.
  TopoDS_Edge anEdge;
  ShapeMap    anEdges;
  TopExp::MapShapes(aModel.myWedge, TopAbs_EDGE, anEdges);
  for (int anIndex = 1; anIndex <= anEdges.Extent() && anEdge.IsNull(); ++anIndex)
  {
    const TopoDS_Edge& aCandidate = TopoDS::Edge(anEdges(anIndex));
    TopoDS_Vertex      aV1, aV2;
    TopExp::Vertices(aCandidate, aV1, aV2);
    const gp_Pnt aP1 = BRep_Tool::Pnt(aV1);
    const gp_Pnt aP2 = BRep_Tool::Pnt(aV2);
    if (aP1.X() < Precision::Confusion() && aP1.Z() < Precision::Confusion()
        && aP2.X() < Precision::Confusion() && aP2.Z() < Precision::Confusion())
    {
      anEdge = aCandidate;
    }
  }
  if (anEdge.IsNull())
  {
    return aModel;
  }

  BRepFilletAPI_MakeFillet aFilletMaker(aModel.myWedge);
  aFilletMaker.Add(THE_ROUND_RADIUS, anEdge);
  aFilletMaker.Build();
  if (!aFilletMaker.IsDone())
  {
    return aModel;
  }
  aModel.myRounded = aFilletMaker.Shape();

  ShapeMap aFaces;
  TopExp::MapShapes(aModel.myRounded, TopAbs_FACE, aFaces);
  for (int anIndex = 1; anIndex <= aFaces.Extent(); ++anIndex)
  {
    const TopoDS_Face&        aCandidate = TopoDS::Face(aFaces(anIndex));
    const BRepAdaptor_Surface aSurface(aCandidate);
    if (aSurface.GetType() == GeomAbs_Cylinder
        && std::abs(aSurface.Cylinder().Radius() - THE_ROUND_RADIUS) < Precision::Confusion())
    {
      aModel.myRound = aCandidate;
      break;
    }
  }
  return aModel;
}

//! A pocket reproducing the shape of a valve recess of a piston crown, which
//! is the configuration the feature removal used to choke on.
struct PocketModel
{
  TopoDS_Shape                   myPocket;  //!< pocket with sharp edges
  TopoDS_Shape                   myBlended; //!< the same pocket, all its edges blended
  NCollection_List<TopoDS_Shape> myBlends;  //!< the blend faces of myBlended
};

//! Build a blended pocket whose wall is split into two faces.
//!
//! A blank with a flat top face is cut by a cylinder whose axis is tilted, so
//! that the base plane of the cylinder crosses the top face right at the axis.
//! The pocket is therefore a half lens which fades to zero depth at two cusps,
//! exactly like a valve recess machined into a piston crown.
//!
//! The cutting cylinder is built from two halves, so that the wall of the
//! pocket consists of two faces lying on one and the same cylindrical surface
//! and sharing a generatrix. This is the essential ingredient: the only edge
//! of either wall face which does not belong to a blend is that shared
//! generatrix, and it falls inside the split of the reconstructed floor
//! instead of bounding it.
//!
//! @param theSplitAngle position of the shared generatrix, in degrees from the
//!                      deepest point of the pocket
PocketModel MakeSplitWallPocket(const double theSplitAngle)
{
  const double aTilt = THE_POCKET_TILT * M_PI / 180.0;

  // The axis leans towards +X, so the pocket is deepest at +X and the
  // reference direction of the cylinder points at that deepest generatrix.
  const gp_Pnt anApex(0.0, 0.0, 0.0);
  const gp_Dir anAxis(sin(aTilt), 0.0, cos(aTilt));
  const gp_Dir aRefDir(cos(aTilt), 0.0, -sin(aTilt));

  const TopoDS_Shape aBlank =
    BRepPrimAPI_MakeBox(gp_Pnt(-THE_BLANK_HALF_SIZE, -THE_BLANK_HALF_SIZE, -THE_BLANK_DEPTH),
                        gp_Pnt(THE_BLANK_HALF_SIZE, THE_BLANK_HALF_SIZE, 0.0))
      .Shape();

  gp_Ax2 aFirstHalf(anApex, anAxis, aRefDir);
  aFirstHalf.Rotate(gp_Ax1(anApex, anAxis), theSplitAngle * M_PI / 180.0);
  gp_Ax2 aSecondHalf = aFirstHalf;
  aSecondHalf.Rotate(gp_Ax1(anApex, anAxis), M_PI);

  const TopoDS_Shape aHalf1 =
    BRepPrimAPI_MakeCylinder(aFirstHalf, THE_POCKET_RADIUS, THE_TOOL_HEIGHT, M_PI).Shape();
  const TopoDS_Shape aHalf2 =
    BRepPrimAPI_MakeCylinder(aSecondHalf, THE_POCKET_RADIUS, THE_TOOL_HEIGHT, M_PI).Shape();
  const TopoDS_Shape aTool = BRepAlgoAPI_Fuse(aHalf1, aHalf2).Shape();

  PocketModel aModel;
  aModel.myPocket = BRepAlgoAPI_Cut(aBlank, aTool).Shape();

  // Blend every edge introduced by the cut, i.e. every edge which the blank
  // does not already have.
  ShapeMap aBlankEdges;
  TopExp::MapShapes(aBlank, TopAbs_EDGE, aBlankEdges);
  ShapeMap aPocketEdges;
  TopExp::MapShapes(aModel.myPocket, TopAbs_EDGE, aPocketEdges);

  BRepFilletAPI_MakeFillet aFilletMaker(aModel.myPocket);
  for (int anIndex = 1; anIndex <= aPocketEdges.Extent(); ++anIndex)
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(aPocketEdges(anIndex));
    if (BRep_Tool::Degenerated(anEdge) || aBlankEdges.Contains(anEdge))
    {
      continue;
    }
    aFilletMaker.Add(THE_BLEND_RADIUS, anEdge);
  }
  aFilletMaker.Build();
  if (!aFilletMaker.IsDone())
  {
    return aModel;
  }
  aModel.myBlended = aFilletMaker.Shape();

  // Collect the blends: the wall and the floor are the only faces left on a
  // surface of the cutting cylinder, everything else of the pocket is planar.
  ShapeMap aBlendedFaces;
  TopExp::MapShapes(aModel.myBlended, TopAbs_FACE, aBlendedFaces);
  for (int anIndex = 1; anIndex <= aBlendedFaces.Extent(); ++anIndex)
  {
    const TopoDS_Face&        aFace = TopoDS::Face(aBlendedFaces(anIndex));
    const BRepAdaptor_Surface aSurface(aFace);
    const bool                isBlend =
      aSurface.GetType() == GeomAbs_BSplineSurface
      || (aSurface.GetType() == GeomAbs_Torus
          && std::abs(aSurface.Torus().MinorRadius() - THE_BLEND_RADIUS) < Precision::Confusion())
      || (aSurface.GetType() == GeomAbs_Cylinder
          && std::abs(aSurface.Cylinder().Radius() - THE_BLEND_RADIUS) < Precision::Confusion());
    if (isBlend)
    {
      aModel.myBlends.Append(aFace);
    }
  }
  return aModel;
}

//! Remove the blends of the model and check that the sharp pocket is restored.
void CheckBlendsRemoved(const PocketModel& theModel)
{
  ASSERT_FALSE(theModel.myBlended.IsNull()) << "failed to blend the pocket";
  ASSERT_EQ(THE_BLEND_COUNT, theModel.myBlends.Extent());

  BRepAlgoAPI_Defeaturing aDefeaturing;
  aDefeaturing.SetShape(theModel.myBlended);
  aDefeaturing.AddFacesToRemove(theModel.myBlends);
  aDefeaturing.Build();

  ASSERT_TRUE(aDefeaturing.IsDone());
  EXPECT_FALSE(aDefeaturing.HasWarnings()) << "the blends were not removed";

  const TopoDS_Shape aResult = aDefeaturing.Shape();
  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());

  // Removing the blends must give back exactly the pocket they were built on,
  // in particular the pocket must not be sealed by the removal.
  EXPECT_NEAR(BOPTest_Utilities::GetVolume(theModel.myPocket),
              BOPTest_Utilities::GetVolume(aResult),
              1.0e-3);

  // The two halves of the wall and of the floor are rebuilt as single faces,
  // so the four sides and the bottom of the blank are joined by the top face,
  // the wall and the floor.
  EXPECT_EQ(8, CountFaces(aResult));
}
} // namespace

// The extension of the adjacent faces used to be sized by the bounding box of
// the feature alone, which is far too short for the faces of a wedge this
// sharp to reach each other. Trimming of the extended faces then degenerated,
// the untrimmed extensions leaked into the reconstruction and no solid could
// be built.
TEST(BRepAlgoAPI_DefeaturingTest, WedgeRound_ExtensionShorterThanTheGap_RoundRemoved)
{
  const WedgeModel aModel = MakeRoundedWedge();
  ASSERT_FALSE(aModel.myRounded.IsNull()) << "failed to round the wedge";
  ASSERT_FALSE(aModel.myRound.IsNull()) << "the round was not found";

  BRepAlgoAPI_Defeaturing aDefeaturing;
  aDefeaturing.SetShape(aModel.myRounded);
  aDefeaturing.AddFaceToRemove(aModel.myRound);
  aDefeaturing.Build();

  ASSERT_TRUE(aDefeaturing.IsDone());
  EXPECT_FALSE(aDefeaturing.HasWarnings()) << "the round was not removed";

  const TopoDS_Shape aResult = aDefeaturing.Shape();
  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());

  // Removing the round must give back the sharp wedge it was built on.
  EXPECT_NEAR(BOPTest_Utilities::GetVolume(aModel.myWedge),
              BOPTest_Utilities::GetVolume(aResult),
              1.0e-3);
  EXPECT_EQ(CountFaces(aModel.myWedge), CountFaces(aResult));
}

// The wall is split on the deepest generatrix of the pocket. The split of the
// reconstructed floor used to be rejected as reversed, which left the floor
// unsupported and the blends in place.
TEST(BRepAlgoAPI_DefeaturingTest, SplitWallPocket_SeamOnDeepestGeneratrix_BlendsRemoved)
{
  CheckBlendsRemoved(MakeSplitWallPocket(0.0));
}

// The wall is split away from the deepest generatrix. Here the blends were
// removed without any warning, but the cell freed by them was merged into the
// body and silently sealed the pocket.
TEST(BRepAlgoAPI_DefeaturingTest, SplitWallPocket_SeamOffCentre_PocketNotSealed)
{
  CheckBlendsRemoved(MakeSplitWallPocket(30.0));
}
