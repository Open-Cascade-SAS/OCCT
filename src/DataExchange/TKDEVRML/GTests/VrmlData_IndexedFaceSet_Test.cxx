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

#include <DEVRML_ConfigurationNode.hxx>
#include <DEVRML_Provider.hxx>
#include <DE_Provider.hxx>
#include <TopoDS_Shape.hxx>
#include <VrmlData_Coordinate.hxx>
#include <VrmlData_IndexedFaceSet.hxx>
#include <VrmlData_Scene.hxx>

#include <gtest/gtest.h>

#include <array>
#include <sstream>
#include <string>

namespace
{
static bool readVrmlShape(const std::string& theContent, TopoDS_Shape& theShape)
{
  const occ::handle<DEVRML_Provider> aProvider =
    new DEVRML_Provider(new DEVRML_ConfigurationNode());
  std::istringstream          anInput(theContent);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("indexed-face-set.vrml", anInput));
  return aProvider->Read(aStreams, theShape);
}
} // namespace

TEST(VrmlData_IndexedFaceSetTest, RejectsInvalidCoordinateIndex)
{
  const std::string aContent = "#VRML V2.0 utf8\n"
                               "Shape { geometry IndexedFaceSet { "
                               "coord Coordinate { point [ 0 0 0, 1 0 0, 0 1 0 ] } "
                               "coordIndex [ 0, 1, 3, -1 ] } }\n";
  TopoDS_Shape      aShape;
  EXPECT_FALSE(readVrmlShape(aContent, aShape));
  EXPECT_TRUE(aShape.IsNull());
}

TEST(VrmlData_IndexedFaceSetTest, ComputesNormalsForInvalidNormalIndex)
{
  const std::string aContent = "#VRML V2.0 utf8\n"
                               "Shape { geometry IndexedFaceSet { "
                               "coord Coordinate { point [ 0 0 0, 1 0 0, 0 1 0 ] } "
                               "coordIndex [ 0, 1, 2, -1 ] "
                               "normal Normal { vector [ 0 0 1, 0 0 1, 0 0 1 ] } "
                               "normalIndex [ 0, 1, 3, -1 ] normalPerVertex TRUE } }\n";
  TopoDS_Shape      aShape;
  ASSERT_TRUE(readVrmlShape(aContent, aShape));
  EXPECT_FALSE(aShape.IsNull());
}

TEST(VrmlData_IndexedFaceSetTest, RejectsNegativeCoordinateIndex)
{
  VrmlData_Scene                         aScene;
  const std::array<gp_XYZ, 3>            aPoints = {gp_XYZ(0.0, 0.0, 0.0),
                                                    gp_XYZ(1.0, 0.0, 0.0),
                                                    gp_XYZ(0.0, 1.0, 0.0)};
  const occ::handle<VrmlData_Coordinate> aCoordinates =
    new VrmlData_Coordinate(aScene, "coordinates", aPoints.size(), aPoints.data());
  const std::array<int, 4>  aPolygon  = {3, 0, -2, 1};
  std::array<const int*, 1> aPolygons = {aPolygon.data()};

  VrmlData_IndexedFaceSet aFaceSet(aScene, "faces");
  aFaceSet.SetCoordinates(aCoordinates);
  aFaceSet.SetPolygons(aPolygons.size(), aPolygons.data());
  EXPECT_TRUE(aFaceSet.TShape().IsNull());
}
