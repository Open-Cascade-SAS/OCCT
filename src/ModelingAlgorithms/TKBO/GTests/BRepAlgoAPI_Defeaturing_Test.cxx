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
#include <BRepAlgoAPI_Defeaturing.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
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
