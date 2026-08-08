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

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBndLib.hxx>
#include <BRepExtrema_ShapeProximity.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>
#include <BRep_Tool.hxx>
#include <Bnd_Box.hxx>
#include <Message_ProgressIndicator.hxx>
#include <Message_ProgressScope.hxx>
#include <OSD_Directory.hxx>
#include <OSD_File.hxx>
#include <OSD_OpenMode.hxx>
#include <OSD_Path.hxx>
#include <OSD_Protection.hxx>
#include <StlAPI_Reader.hxx>
#include <StlAPI_Writer.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_Sequence.hxx>
#include <Poly_Triangulation.hxx>
#include <RWStl.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <gp_Pln.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <cstring>
#include <sstream>

#include <gtest/gtest.h>

namespace
{
class ProgressObserver : public Message_ProgressIndicator
{
public:
  const NCollection_Sequence<TCollection_AsciiString>& Messages() const { return myMessages; }

protected:
  void Show(const Message_ProgressScope& theScope, const bool) override
  {
    TCollection_AsciiString aMessage("Progress: ");
    aMessage += static_cast<int>(100.0 * GetPosition() + 0.5);
    aMessage += "%";
    if (theScope.Name() != nullptr)
    {
      aMessage += " ";
      aMessage += theScope.Name();
      aMessage += ": ";
      aMessage += static_cast<int>(theScope.Value() + 0.5);
      aMessage += " / ";
      aMessage += static_cast<int>(theScope.MaxValue() + 0.5);
    }
    myMessages.Append(aMessage);
  }

private:
  NCollection_Sequence<TCollection_AsciiString> myMessages;
};

static TopoDS_Shape MakeRegularTriangulatedFace()
{
  const int   aNbU  = 100;
  const int   aNbV  = 100;
  TopoDS_Face aFace = BRepBuilderAPI_MakeFace(gp_Pln(), 0.0, 100.0, 0.0, 100.0).Face();
  occ::handle<Poly_Triangulation> aTriangulation =
    new Poly_Triangulation((aNbU + 1) * (aNbV + 1), 2 * aNbU * aNbV, false);
  for (int iU = 0; iU <= aNbU; ++iU)
  {
    for (int iV = 0; iV <= aNbV; ++iV)
    {
      const int aNode = 1 + iU * (aNbV + 1) + iV;
      aTriangulation->SetNode(aNode, gp_Pnt(iU, iV, 0.0));
    }
  }
  int aTriangle = 1;
  for (int iU = 0; iU < aNbU; ++iU)
  {
    for (int iV = 0; iV < aNbV; ++iV)
    {
      const int aBase = 1 + iU * (aNbV + 1) + iV;
      aTriangulation->SetTriangle(aTriangle++, Poly_Triangle(aBase, aBase + aNbV + 2, aBase + 1));
      aTriangulation->SetTriangle(aTriangle++,
                                  Poly_Triangle(aBase, aBase + aNbV + 1, aBase + aNbV + 2));
    }
  }
  BRep_Builder aBuilder;
  aBuilder.UpdateFace(aFace, aTriangulation);
  return aFace;
}

static TCollection_AsciiString MakeTemporaryStlFile(OSD_Directory& theDirectory, OSD_Path& thePath)
{
  theDirectory.Path(thePath);
  thePath.SetName("draw_shape_write_stl");
  thePath.SetExtension(".stl");

  TCollection_AsciiString aSystemName;
  thePath.SystemName(aSystemName);
  return aSystemName;
}

static int CountFaces(const TopoDS_Shape& theShape)
{
  int aNbFaces = 0;
  for (TopExp_Explorer anExplorer(theShape, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    ++aNbFaces;
  }
  return aNbFaces;
}

static int CountTriangles(const TopoDS_Shape& theShape)
{
  int aNbTriangles = 0;
  for (TopExp_Explorer anExplorer(theShape, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    TopLoc_Location                 aLocation;
    occ::handle<Poly_Triangulation> aTriangulation =
      BRep_Tool::Triangulation(TopoDS::Face(anExplorer.Current()), aLocation);
    if (!aTriangulation.IsNull())
    {
      aNbTriangles += aTriangulation->NbTriangles();
    }
  }
  return aNbTriangles;
}

static TCollection_AsciiString MakeTemporaryStlFile(OSD_Directory&    theDirectory,
                                                    const char* const theName,
                                                    OSD_Path&         thePath)
{
  theDirectory.Path(thePath);
  thePath.SetName(theName);
  thePath.SetExtension(".stl");

  TCollection_AsciiString aSystemName;
  thePath.SystemName(aSystemName);
  return aSystemName;
}

static void WriteTemporaryFile(const OSD_Path& thePath, const void* theBuffer, const int theLength)
{
  OSD_File aFile(thePath);
  aFile.Build(OSD_WriteOnly, OSD_Protection());
  if (theLength > 0)
  {
    aFile.Write(const_cast<void*>(theBuffer), theLength);
  }
  aFile.Close();
}

static void WriteTemporaryFile(const OSD_Path& thePath, const TCollection_AsciiString& theBuffer)
{
  WriteTemporaryFile(thePath, (void*)theBuffer.ToCString(), theBuffer.Length());
}

static void WriteBinaryStl(const OSD_Path& thePath, const bool theHasFacet)
{
  unsigned char aBuffer[134] = {};
  std::memcpy(aBuffer, "stl ", 4);
  if (theHasFacet)
  {
    aBuffer[80] = 1;

    const float aNormal[3]  = {0.0f, 0.0f, 1.0f};
    const float aVertex1[3] = {0.0f, 0.0f, 0.0f};
    const float aVertex2[3] = {1.0f, 0.0f, 0.0f};
    const float aVertex3[3] = {0.0f, 1.0f, 0.0f};
    std::memcpy(aBuffer + 84, aNormal, sizeof(aNormal));
    std::memcpy(aBuffer + 96, aVertex1, sizeof(aVertex1));
    std::memcpy(aBuffer + 108, aVertex2, sizeof(aVertex2));
    std::memcpy(aBuffer + 120, aVertex3, sizeof(aVertex3));
  }
  WriteTemporaryFile(thePath, aBuffer, theHasFacet ? 134 : 84);
}

static bool ReadStl(const TCollection_AsciiString& theFileName, TopoDS_Shape& theShape)
{
  StlAPI_Reader aReader;
  return aReader.Read(theShape, theFileName.ToCString());
}

static TopoDS_Shape RoundTripStl(const TopoDS_Shape& theShape, const bool theAsciiMode = false)
{
  std::stringstream aStream;
  StlAPI_Writer     aWriter;
  aWriter.ASCIIMode() = theAsciiMode;
  if (!aWriter.Write(theShape, aStream))
  {
    return TopoDS_Shape();
  }

  aStream.seekg(0);
  TopoDS_Shape  aRestoredShape;
  StlAPI_Reader aReader;
  if (!aReader.Read(aRestoredShape, aStream))
  {
    return TopoDS_Shape();
  }
  return aRestoredShape;
}

static TopoDS_Shape RoundTripStlMesh(const TopoDS_Shape& theShape)
{
  std::stringstream aStream;
  StlAPI_Writer     aWriter;
  aWriter.ASCIIMode() = false;
  if (!aWriter.Write(theShape, aStream))
  {
    return TopoDS_Shape();
  }

  aStream.seekg(0);
  occ::handle<Poly_Triangulation> aMesh = RWStl::ReadStream(aStream);
  if (aMesh.IsNull())
  {
    return TopoDS_Shape();
  }

  BRep_Builder aBuilder;
  TopoDS_Face  aFace;
  aBuilder.MakeFace(aFace);
  aBuilder.UpdateFace(aFace, aMesh);
  return aFace;
}
} // namespace

// de_mesh/shape_write_stl/B1: the DRAW writestl path reports progress and
// produces a readable STL file from a generated triangulated shape.
TEST(DEMesh_ShapeWriteStl_Test, B1_WriteProgressAndReadBack)
{
  const TopoDS_Shape aShape = MakeRegularTriangulatedFace();
  ASSERT_FALSE(aShape.IsNull());

  OSD_Directory                 aDirectory = OSD_Directory::BuildTemporary();
  OSD_Path                      aPath;
  const TCollection_AsciiString aFileName = MakeTemporaryStlFile(aDirectory, aPath);
  occ::handle<ProgressObserver> aProgress = new ProgressObserver();
  StlAPI_Writer                 aWriter;
  aWriter.ASCIIMode() = false;

  EXPECT_TRUE(aWriter.Write(aShape, aFileName.ToCString(), aProgress->Start()));
  const char* const anExpected[] = {
    "Progress: 0%",
    "Progress: 5% Triangles: 1000 / 20000",
    "Progress: 10% Triangles: 2000 / 20000",
    "Progress: 15% Triangles: 3000 / 20000",
    "Progress: 20% Triangles: 4000 / 20000",
    "Progress: 25% Triangles: 5000 / 20000",
    "Progress: 30% Triangles: 6000 / 20000",
    "Progress: 35% Triangles: 7000 / 20000",
    "Progress: 40% Triangles: 8000 / 20000",
    "Progress: 45% Triangles: 9000 / 20000",
    "Progress: 50% Triangles: 10000 / 20000",
    "Progress: 55% Triangles: 11000 / 20000",
    "Progress: 60% Triangles: 12000 / 20000",
    "Progress: 65% Triangles: 13000 / 20000",
    "Progress: 70% Triangles: 14000 / 20000",
    "Progress: 75% Triangles: 15000 / 20000",
    "Progress: 80% Triangles: 16000 / 20000",
    "Progress: 85% Triangles: 17000 / 20000",
    "Progress: 90% Triangles: 18000 / 20000",
    "Progress: 95% Triangles: 19000 / 20000",
    "Progress: 100% Triangles: 20000 / 20000",
  };
  ASSERT_EQ(aProgress->Messages().Length(),
            static_cast<int>(sizeof(anExpected) / sizeof(*anExpected)));
  for (int anIndex = 1; anIndex <= aProgress->Messages().Length(); ++anIndex)
  {
    EXPECT_STREQ(aProgress->Messages().Value(anIndex).ToCString(), anExpected[anIndex - 1]);
  }
  EXPECT_DOUBLE_EQ(aProgress->GetPosition(), 1.0);

  OSD_File aFile(aPath);
  ASSERT_TRUE(aFile.Exists());

  TopoDS_Shape  aRestored;
  StlAPI_Reader aReader;
  EXPECT_TRUE(aReader.Read(aRestored, aFileName.ToCString()));
  EXPECT_FALSE(aRestored.IsNull());
  EXPECT_GT(CountFaces(aRestored), 0);

  aFile.Remove();
}

// de_mesh/stl_read/D1: the DRAW reader accepts ASCII STL with both line endings,
// a missing final EOL, and a binary STL, while rejecting empty files.
TEST(DEMesh_StlRead_Test, D1_ReadAsciiAndBinaryBoundaryCases)
{
  const TCollection_AsciiString aMinimalAsciiStl("solid\n"
                                                 "facet normal 0 0 1\n"
                                                 "outer loop\n"
                                                 "vertex 0 0 0\n"
                                                 "vertex 1 0 0\n"
                                                 "vertex 0 1 0\n"
                                                 "endloop\n"
                                                 "endfacet\n"
                                                 "endsolid");

  OSD_Directory aDirectory = OSD_Directory::BuildTemporary();

  OSD_Path                      aDosPath;
  const TCollection_AsciiString aDosName =
    MakeTemporaryStlFile(aDirectory, "draw_stl_one_ascii_dos", aDosPath);
  const TCollection_AsciiString aDosAscii("solid\r\n"
                                          "facet normal 0 0 1\r\n"
                                          "outer loop\r\n"
                                          "vertex 0 0 0\r\n"
                                          "vertex 1 0 0\r\n"
                                          "vertex 0 1 0\r\n"
                                          "endloop\r\n"
                                          "endfacet\r\n"
                                          "endsolid\r\n");
  WriteTemporaryFile(aDosPath, aDosAscii);
  TopoDS_Shape aDosShape;
  ASSERT_TRUE(ReadStl(aDosName, aDosShape));
  EXPECT_EQ(CountFaces(aDosShape), 1);

  OSD_Path                      aUnixPath;
  const TCollection_AsciiString aUnixName =
    MakeTemporaryStlFile(aDirectory, "draw_stl_one_ascii_unix", aUnixPath);
  WriteTemporaryFile(aUnixPath, aMinimalAsciiStl + "\n");
  TopoDS_Shape aUnixShape;
  ASSERT_TRUE(ReadStl(aUnixName, aUnixShape));
  EXPECT_EQ(CountFaces(aUnixShape), 1);

  OSD_Path                      aNoEolPath;
  const TCollection_AsciiString aNoEolName =
    MakeTemporaryStlFile(aDirectory, "draw_stl_one_ascii_noeol", aNoEolPath);
  WriteTemporaryFile(aNoEolPath, aMinimalAsciiStl);
  TopoDS_Shape aNoEolShape;
  ASSERT_TRUE(ReadStl(aNoEolName, aNoEolShape));
  EXPECT_EQ(CountFaces(aNoEolShape), 1);

  const TCollection_AsciiString aZeroAsciiStl("solid \nendsolid");
  OSD_Path                      aZeroAsciiPath;
  const TCollection_AsciiString aZeroAsciiName =
    MakeTemporaryStlFile(aDirectory, "draw_stl_zero_ascii", aZeroAsciiPath);
  WriteTemporaryFile(aZeroAsciiPath, aZeroAsciiStl);
  TopoDS_Shape aZeroAsciiShape;
  EXPECT_FALSE(ReadStl(aZeroAsciiName, aZeroAsciiShape));
  EXPECT_TRUE(aZeroAsciiShape.IsNull());

  OSD_Path                      aBinaryPath;
  const TCollection_AsciiString aBinaryName =
    MakeTemporaryStlFile(aDirectory, "draw_stl_one_binary", aBinaryPath);
  WriteBinaryStl(aBinaryPath, true);
  TopoDS_Shape aBinaryShape;
  ASSERT_TRUE(ReadStl(aBinaryName, aBinaryShape));
  EXPECT_EQ(CountFaces(aBinaryShape), 1);

  OSD_Path                      aZeroBinaryPath;
  const TCollection_AsciiString aZeroBinaryName =
    MakeTemporaryStlFile(aDirectory, "draw_stl_zero_binary", aZeroBinaryPath);
  WriteBinaryStl(aZeroBinaryPath, false);
  TopoDS_Shape aZeroBinaryShape;
  EXPECT_FALSE(ReadStl(aZeroBinaryName, aZeroBinaryShape));
  EXPECT_TRUE(aZeroBinaryShape.IsNull());

  OSD_Path                      aEmptyPath;
  const TCollection_AsciiString aEmptyName =
    MakeTemporaryStlFile(aDirectory, "draw_stl_empty", aEmptyPath);
  const char* anEmptyBuffer = "";
  WriteTemporaryFile(aEmptyPath, anEmptyBuffer, 0);
  TopoDS_Shape anEmptyShape;
  EXPECT_FALSE(ReadStl(aEmptyName, anEmptyShape));
  EXPECT_TRUE(anEmptyShape.IsNull());

  OSD_File aFile(aDosPath);
  aFile.Remove();
  OSD_File(aUnixPath).Remove();
  OSD_File(aNoEolPath).Remove();
  OSD_File(aZeroAsciiPath).Remove();
  OSD_File(aBinaryPath).Remove();
  OSD_File(aZeroBinaryPath).Remove();
  OSD_File(aEmptyPath).Remove();
}

// bugs/modalg_7/bug30829: STL round-trips remain usable by shape proximity.
TEST(DEMesh_ShapeWriteStl_Test, ModalgBug_30829_ProximityAfterStlRoundTrip)
{
  const TopoDS_Shape       aBox      = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  const TopoDS_Shape       aCylinder = BRepPrimAPI_MakeCylinder(5.0, 10.0).Shape();
  BRepMesh_IncrementalMesh aBoxMesher(aBox, 0.1);
  BRepMesh_IncrementalMesh aCylinderMesher(aCylinder, 0.1);

  const TopoDS_Shape aRestoredBox      = RoundTripStlMesh(aBox);
  const TopoDS_Shape aRestoredCylinder = RoundTripStlMesh(aCylinder);
  ASSERT_FALSE(aRestoredBox.IsNull());
  ASSERT_FALSE(aRestoredCylinder.IsNull());

  BRepExtrema_ShapeProximity aProximity(0.0);
  ASSERT_TRUE(aProximity.LoadShape1(aRestoredBox));
  ASSERT_TRUE(aProximity.LoadShape2(aRestoredCylinder));
  aProximity.Perform();
  ASSERT_TRUE(aProximity.IsDone());
  EXPECT_GT(aProximity.ElementSet1()->Size(), 0);
  EXPECT_GT(aProximity.ElementSet2()->Size(), 0);
}

// bugs/stlvrml/bug26338: binary STL preserves the locations of compound parts.
TEST(DEMesh_ShapeWriteStl_Test, StlBug_26338_CompoundLocations)
{
  const TopoDS_Shape aFirstBox = BRepPrimAPI_MakeBox(5.0, 5.0, 5.0).Shape();
  gp_Trsf            aTranslation;
  aTranslation.SetTranslation(gp_Vec(10.0, 10.0, 10.0));
  const TopoDS_Shape aSecondBox = BRepBuilderAPI_Transform(aFirstBox, aTranslation).Shape();

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aFirstBox);
  aBuilder.Add(aCompound, aSecondBox);
  BRepMesh_IncrementalMesh aMesher(aCompound, 1.0);

  const TopoDS_Shape aRestored = RoundTripStl(aCompound);
  ASSERT_FALSE(aRestored.IsNull());
  Bnd_Box aBounds;
  BRepBndLib::Add(aRestored, aBounds);
  double aXMin = 0.0;
  double aYMin = 0.0;
  double aZMin = 0.0;
  double aXMax = 0.0;
  double aYMax = 0.0;
  double aZMax = 0.0;
  aBounds.Get(aXMin, aYMin, aZMin, aXMax, aYMax, aZMax);
  EXPECT_NEAR(aXMin, 0.0, 1.0e-6);
  EXPECT_NEAR(aYMin, 0.0, 1.0e-6);
  EXPECT_NEAR(aZMin, 0.0, 1.0e-6);
  EXPECT_NEAR(aXMax, 15.0, 1.0e-6);
  EXPECT_NEAR(aYMax, 15.0, 1.0e-6);
  EXPECT_NEAR(aZMax, 15.0, 1.0e-6);
}

// bugs/stlvrml/bug30389: STL writing skips an untriangulated face and keeps the
// ten triangles generated for the five meshed faces.
TEST(DEMesh_ShapeWriteStl_Test, StlBug_30389_MissingFaceTriangulation)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape();
  BRep_Builder       aBuilder;
  TopoDS_Compound    aMeshedFaces;
  aBuilder.MakeCompound(aMeshedFaces);
  int aFaceIndex = 0;
  for (TopExp_Explorer anExplorer(aBox, TopAbs_FACE); anExplorer.More() && aFaceIndex < 5;
       anExplorer.Next(), ++aFaceIndex)
  {
    aBuilder.Add(aMeshedFaces, anExplorer.Current());
  }
  ASSERT_EQ(aFaceIndex, 5);
  BRepMesh_IncrementalMesh aMesher(aMeshedFaces, 0.1);

  const TopoDS_Shape aRestored = RoundTripStl(aBox);
  ASSERT_FALSE(aRestored.IsNull());
  EXPECT_EQ(CountFaces(aRestored), 10);
}

// bugs/xde/bug25357: STL writing does not remesh an already triangulated torus.
TEST(DEMesh_ShapeWriteStl_Test, XdeBug_25357_PreserveTriangulation)
{
  const TopoDS_Shape       aTorus = BRepPrimAPI_MakeTorus(10.0, 8.0).Shape();
  BRepMesh_IncrementalMesh aMesher(aTorus, 0.5);
  const int                aTrianglesBefore = CountTriangles(aTorus);
  ASSERT_GT(aTrianglesBefore, 0);

  std::stringstream aStream;
  StlAPI_Writer     aWriter;
  aWriter.ASCIIMode() = false;
  EXPECT_TRUE(aWriter.Write(aTorus, aStream));
  EXPECT_FALSE(aStream.str().empty());
  EXPECT_EQ(CountTriangles(aTorus), aTrianglesBefore);
}
