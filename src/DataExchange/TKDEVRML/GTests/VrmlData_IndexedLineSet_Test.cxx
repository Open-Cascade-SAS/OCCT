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
#include <VrmlData_IndexedLineSet.hxx>
#include <VrmlData_Scene.hxx>

#include <gtest/gtest.h>

#include <array>
#include <sstream>
#include <string>

TEST(VrmlData_IndexedLineSetTest, RejectsInvalidCoordinateIndex)
{
  const std::string                  aContent = "#VRML V2.0 utf8\n"
                                                "Shape { geometry IndexedLineSet { "
                                                "coord Coordinate { point [ 0 0 0, 1 0 0, 0 1 0 ] } "
                                                "coordIndex [ 0, 1, 3, -1 ] } }\n";
  const occ::handle<DEVRML_Provider> aProvider =
    new DEVRML_Provider(new DEVRML_ConfigurationNode());
  std::istringstream          anInput(aContent);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("indexed-line-set.vrml", anInput));

  TopoDS_Shape aShape;
  EXPECT_FALSE(aProvider->Read(aStreams, aShape));
  EXPECT_TRUE(aShape.IsNull());
}

TEST(VrmlData_IndexedLineSetTest, RejectsNegativeCoordinateIndex)
{
  VrmlData_Scene                         aScene;
  const std::array<gp_XYZ, 2>            aPoints = {gp_XYZ(0.0, 0.0, 0.0), gp_XYZ(1.0, 0.0, 0.0)};
  const occ::handle<VrmlData_Coordinate> aCoordinates =
    new VrmlData_Coordinate(aScene, "coordinates", aPoints.size(), aPoints.data());
  const std::array<int, 3>  aPolyline  = {2, 0, -2};
  std::array<const int*, 1> aPolylines = {aPolyline.data()};

  VrmlData_IndexedLineSet aLineSet(aScene, "lines");
  aLineSet.SetCoordinates(aCoordinates);
  aLineSet.SetPolygons(aPolylines.size(), aPolylines.data());
  EXPECT_TRUE(aLineSet.TShape().IsNull());
}
