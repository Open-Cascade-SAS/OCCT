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
#include <BinTools_Curve2dSet.hxx>
#include <BinTools_CurveSet.hxx>
#include <BinTools_LocationSet.hxx>
#include <BinTools_ShapeSet.hxx>
#include <BinTools_SurfaceSet.hxx>
#include <Standard_Failure.hxx>
#include <TopoDS_Vertex.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

#include <sstream>
#include <string>

namespace
{
//! Replaces the name of a table in a written set by a token of theLength characters.
std::string withLongTableName(const std::string& theWritten,
                              const std::string& theName,
                              const size_t       theLength)
{
  std::string       aDamaged = theWritten;
  const std::string aToken(theLength, 'A');
  const size_t      aPos = aDamaged.find(theName);
  EXPECT_NE(aPos, std::string::npos);
  if (aPos != std::string::npos)
  {
    aDamaged.replace(aPos, theName.size(), aToken);
  }
  return aDamaged;
}

//! The token lengths tried: just past the 255 byte buffer, and far past it.
const size_t THE_TOKEN_LENGTHS[] = {256, 300, 4096, 1 << 20};
} // namespace

// Each set reads the name of its table into a buffer of 255 characters, and a longer token must
// end the read with an exception instead of being written past the buffer.

TEST(BinTools_SetRead_Test, LocationSetLongTableName)
{
  std::ostringstream aWritten;
  BinTools_LocationSet().Write(aWritten);
  for (const size_t aLength : THE_TOKEN_LENGTHS)
  {
    std::istringstream   aDamaged(withLongTableName(aWritten.str(), "Locations", aLength));
    BinTools_LocationSet aSet;
    EXPECT_THROW(aSet.Read(aDamaged), Standard_Failure) << aLength;
  }
}

TEST(BinTools_SetRead_Test, Curve2dSetLongTableName)
{
  std::ostringstream aWritten;
  BinTools_Curve2dSet().Write(aWritten);
  for (const size_t aLength : THE_TOKEN_LENGTHS)
  {
    std::istringstream  aDamaged(withLongTableName(aWritten.str(), "Curve2ds", aLength));
    BinTools_Curve2dSet aSet;
    EXPECT_THROW(aSet.Read(aDamaged), Standard_Failure) << aLength;
  }
}

TEST(BinTools_SetRead_Test, CurveSetLongTableName)
{
  std::ostringstream aWritten;
  BinTools_CurveSet().Write(aWritten);
  for (const size_t aLength : THE_TOKEN_LENGTHS)
  {
    std::istringstream aDamaged(withLongTableName(aWritten.str(), "Curves", aLength));
    BinTools_CurveSet  aSet;
    EXPECT_THROW(aSet.Read(aDamaged), Standard_Failure) << aLength;
  }
}

TEST(BinTools_SetRead_Test, SurfaceSetLongTableName)
{
  std::ostringstream aWritten;
  BinTools_SurfaceSet().Write(aWritten);
  for (const size_t aLength : THE_TOKEN_LENGTHS)
  {
    std::istringstream  aDamaged(withLongTableName(aWritten.str(), "Surfaces", aLength));
    BinTools_SurfaceSet aSet;
    EXPECT_THROW(aSet.Read(aDamaged), Standard_Failure) << aLength;
  }
}

TEST(BinTools_SetRead_Test, ShapeSetLongTableName)
{
  TopoDS_Vertex aVertex;
  BRep_Builder().MakeVertex(aVertex, gp_Pnt(1.0, 2.0, 3.0), 1.0e-7);
  BinTools_ShapeSet aWriter;
  aWriter.Add(aVertex);
  std::ostringstream aWritten;
  aWriter.Write(aWritten);

  // the unchanged stream reads back
  std::istringstream aValid(aWritten.str());
  BinTools_ShapeSet  aReader;
  EXPECT_NO_THROW(aReader.Read(aValid));
  EXPECT_EQ(aReader.NbShapes(), 1);

  for (const size_t aLength : THE_TOKEN_LENGTHS)
  {
    std::istringstream aDamaged(withLongTableName(aWritten.str(), "TShapes", aLength));
    BinTools_ShapeSet  aSet;
    EXPECT_THROW(aSet.Read(aDamaged), Standard_Failure) << aLength;
  }
}
