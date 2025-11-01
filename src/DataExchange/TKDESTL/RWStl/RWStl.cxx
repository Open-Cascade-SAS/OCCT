// Created on: 2017-06-13
// Created by: Alexander MALYSHEV
// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <RWStl.hxx>

#include <Message_ProgressScope.hxx>
#include <NCollection_Vector.hxx>
#include <OSD_File.hxx>
#include <OSD_FileSystem.hxx>
#include <OSD_OpenFile.hxx>
#include <RWStl_Reader.hxx>
#include <Standard_ReadLineBuffer.hxx>
#include <iomanip>
#include <iostream>
#include <cstring>

namespace
{

static const Standard_Integer THE_STL_SIZEOF_FACET = 50;
// clang-format off
  static const Standard_Integer IND_THRESHOLD = 1000; // increment the indicator every 1k triangles
// clang-format on
static const size_t THE_BUFFER_SIZE = 1024; // The length of buffer to read (in bytes)

//! Writing a Little Endian 32 bits integer
inline static void convertInteger(const Standard_Integer theValue, Standard_Character* theResult)
{
  union {
    Standard_Integer   i;
    Standard_Character c[4];
  } anUnion;

  anUnion.i = theValue;

  theResult[0] = anUnion.c[0];
  theResult[1] = anUnion.c[1];
  theResult[2] = anUnion.c[2];
  theResult[3] = anUnion.c[3];
}

//! Writing a Little Endian 32 bits float
inline static void convertDouble(const Standard_Real theValue, Standard_Character* theResult)
{
  union {
    Standard_ShortReal i;
    Standard_Character c[4];
  } anUnion;

  anUnion.i = (Standard_ShortReal)theValue;

  theResult[0] = anUnion.c[0];
  theResult[1] = anUnion.c[1];
  theResult[2] = anUnion.c[2];
  theResult[3] = anUnion.c[3];
}

class Reader : public RWStl_Reader
{
public:
  //! Add new node
  virtual Standard_Integer AddNode(const gp_XYZ& thePnt) Standard_OVERRIDE
  {
    myNodes.Append(thePnt);
    return myNodes.Size();
  }

  //! Add new triangle
  virtual void AddTriangle(Standard_Integer theNode1,
                           Standard_Integer theNode2,
                           Standard_Integer theNode3) Standard_OVERRIDE
  {
    myTriangles.Append(Poly_Triangle(theNode1, theNode2, theNode3));
  }

  //! Creates Poly_Triangulation from collected data
  Handle(Poly_Triangulation) GetTriangulation()
  {
    if (myTriangles.IsEmpty())
      return Handle(Poly_Triangulation)();

    Handle(Poly_Triangulation) aPoly =
      new Poly_Triangulation(myNodes.Length(), myTriangles.Length(), Standard_False);
    for (Standard_Integer aNodeIter = 0; aNodeIter < myNodes.Size(); ++aNodeIter)
    {
      aPoly->SetNode(aNodeIter + 1, myNodes[aNodeIter]);
    }

    for (Standard_Integer aTriIter = 0; aTriIter < myTriangles.Size(); ++aTriIter)
    {
      aPoly->SetTriangle(aTriIter + 1, myTriangles[aTriIter]);
    }

    return aPoly;
  }

protected:
  void Clear()
  {
    myNodes.Clear();
    myTriangles.Clear();
  }

private:
  NCollection_Vector<gp_XYZ>        myNodes;
  NCollection_Vector<Poly_Triangle> myTriangles;
};

class MultiDomainReader : public Reader
{
public:
  //! Add new solid
  //! Add triangulation to triangulation list for multi-domain case
  virtual void AddSolid() Standard_OVERRIDE
  {
    if (Handle(Poly_Triangulation) aCurrentTri = GetTriangulation())
    {
      myTriangulationList.Append(aCurrentTri);
    }
    Clear();
  }

  //! Returns triangulation list for multi-domain case
  NCollection_Sequence<Handle(Poly_Triangulation)>& ChangeTriangulationList()
  {
    return myTriangulationList;
  }

private:
  NCollection_Sequence<Handle(Poly_Triangulation)> myTriangulationList;
};

} // namespace

//=================================================================================================

Handle(Poly_Triangulation) RWStl::ReadFile(const Standard_CString       theFile,
                                           const Standard_Real          theMergeAngle,
                                           const Message_ProgressRange& theProgress)
{
  Reader aReader;
  aReader.SetMergeAngle(theMergeAngle);
  aReader.Read(theFile, theProgress);
  // note that returned bool value is ignored intentionally -- even if something went wrong,
  // but some data have been read, we at least will return these data
  return aReader.GetTriangulation();
}

//=================================================================================================

void RWStl::ReadFile(const Standard_CString                            theFile,
                     const Standard_Real                               theMergeAngle,
                     NCollection_Sequence<Handle(Poly_Triangulation)>& theTriangList,
                     const Message_ProgressRange&                      theProgress)
{
  MultiDomainReader aReader;
  aReader.SetMergeAngle(theMergeAngle);
  aReader.Read(theFile, theProgress);
  theTriangList.Clear();
  theTriangList.Append(aReader.ChangeTriangulationList());
}

//=================================================================================================

Handle(Poly_Triangulation) RWStl::ReadFile(const OSD_Path&              theFile,
                                           const Message_ProgressRange& theProgress)
{
  OSD_File aFile(theFile);
  if (!aFile.Exists())
  {
    return Handle(Poly_Triangulation)();
  }

  TCollection_AsciiString aPath;
  theFile.SystemName(aPath);
  return ReadFile(aPath.ToCString(), theProgress);
}

//=================================================================================================

Handle(Poly_Triangulation) RWStl::ReadBinary(const OSD_Path&              theFile,
                                             const Message_ProgressRange& theProgress)
{
  TCollection_AsciiString aPath;
  theFile.SystemName(aPath);

  const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::istream> aStream =
    aFileSystem->OpenIStream(aPath, std::ios::in | std::ios::binary);
  if (aStream.get() == NULL)
  {
    return Handle(Poly_Triangulation)();
  }

  Reader aReader;
  if (!aReader.ReadBinary(*aStream, theProgress))
  {
    return Handle(Poly_Triangulation)();
  }

  return aReader.GetTriangulation();
}

//=================================================================================================

Handle(Poly_Triangulation) RWStl::ReadAscii(const OSD_Path&              theFile,
                                            const Message_ProgressRange& theProgress)
{
  TCollection_AsciiString aPath;
  theFile.SystemName(aPath);

  const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::istream> aStream =
    aFileSystem->OpenIStream(aPath, std::ios::in | std::ios::binary);
  if (aStream.get() == NULL)
  {
    return Handle(Poly_Triangulation)();
  }

  // get length of file to feed progress indicator
  aStream->seekg(0, aStream->end);
  std::streampos theEnd = aStream->tellg();
  aStream->seekg(0, aStream->beg);

  Reader                  aReader;
  Standard_ReadLineBuffer aBuffer(THE_BUFFER_SIZE);
  if (!aReader.ReadAscii(*aStream, aBuffer, theEnd, theProgress))
  {
    return Handle(Poly_Triangulation)();
  }

  return aReader.GetTriangulation();
}

//=================================================================================================

Standard_Boolean RWStl::WriteBinary(const Handle(Poly_Triangulation)& theMesh,
                                    const OSD_Path&                   thePath,
                                    const Message_ProgressRange&      theProgress)
{
  if (theMesh.IsNull() || theMesh->NbTriangles() <= 0)
  {
    return Standard_False;
  }

  TCollection_AsciiString aPath;
  thePath.SystemName(aPath);

  std::ofstream aStream(aPath.ToCString(), std::ios::binary);
  if (!aStream.is_open())
  {
    return Standard_False;
  }

  return WriteBinary(theMesh, aStream, theProgress);
}

//=================================================================================================

Standard_Boolean RWStl::WriteAscii(const Handle(Poly_Triangulation)& theMesh,
                                   const OSD_Path&                   thePath,
                                   const Message_ProgressRange&      theProgress)
{
  if (theMesh.IsNull() || theMesh->NbTriangles() <= 0)
  {
    return Standard_False;
  }

  TCollection_AsciiString aPath;
  thePath.SystemName(aPath);

  std::ofstream aStream(aPath.ToCString());
  if (!aStream.is_open())
  {
    return Standard_False;
  }

  return WriteAscii(theMesh, aStream, theProgress);
}

//=================================================================================================

Standard_Boolean RWStl::WriteAscii(const Handle(Poly_Triangulation)& theMesh,
                                   Standard_OStream&                 theStream,
                                   const Message_ProgressRange&      theProgress)
{
  if (theMesh.IsNull() || theMesh->NbTriangles() <= 0)
  {
    return Standard_False;
  }

  // note that space after 'solid' is necessary for many systems
  theStream << "solid \n";
  if (theStream.fail())
  {
    return Standard_False;
  }

  const Standard_Integer NBTriangles = theMesh->NbTriangles();
  Message_ProgressScope  aPS(theProgress, "Triangles", NBTriangles);
  Standard_Integer       anElem[3] = {0, 0, 0};

  for (Standard_Integer aTriIter = 1; aTriIter <= NBTriangles; ++aTriIter)
  {
    const Poly_Triangle aTriangle = theMesh->Triangle(aTriIter);
    aTriangle.Get(anElem[0], anElem[1], anElem[2]);
    const gp_Pnt aP1 = theMesh->Node(anElem[0]);
    const gp_Pnt aP2 = theMesh->Node(anElem[1]);
    const gp_Pnt aP3 = theMesh->Node(anElem[2]);
    const gp_Vec aVec1(aP1, aP2);
    const gp_Vec aVec2(aP1, aP3);
    gp_Vec       aVNorm = aVec1.Crossed(aVec2);
    if (aVNorm.SquareMagnitude() > gp::Resolution())
    {
      aVNorm.Normalize();
    }
    else
    {
      aVNorm.SetCoord(0.0, 0.0, 0.0);
    }

    theStream << " facet normal " << std::scientific << std::setprecision(12) << aVNorm.X() << " "
              << aVNorm.Y() << " " << aVNorm.Z() << "\n"
              << "   outer loop\n"
              << "     vertex " << aP1.X() << " " << aP1.Y() << " " << aP1.Z() << "\n"
              << "     vertex " << aP2.X() << " " << aP2.Y() << " " << aP2.Z() << "\n"
              << "     vertex " << aP3.X() << " " << aP3.Y() << " " << aP3.Z() << "\n"
              << "   endloop\n"
              << " endfacet\n";

    if (theStream.fail())
    {
      return Standard_False;
    }

    // update progress only per 1k triangles
    if ((aTriIter % IND_THRESHOLD) == 0)
    {
      if (!aPS.More())
        return Standard_False;
      aPS.Next(IND_THRESHOLD);
    }
  }

  theStream << "endsolid\n";
  return !theStream.fail();
}

//=================================================================================================

Standard_Boolean RWStl::WriteBinary(const Handle(Poly_Triangulation)& theMesh,
                                    Standard_OStream&                 theStream,
                                    const Message_ProgressRange&      theProgress)
{
  if (theMesh.IsNull() || theMesh->NbTriangles() <= 0)
  {
    return Standard_False;
  }

  char aHeader[80] = "STL Exported by Open CASCADE Technology [dev.opencascade.org]";
  theStream.write(aHeader, 80);
  if (theStream.fail())
  {
    return Standard_False;
  }

  const Standard_Integer                 aNBTriangles = theMesh->NbTriangles();
  Message_ProgressScope                  aPS(theProgress, "Triangles", aNBTriangles);
  const Standard_Size                    aNbChunkTriangles = 4096;
  const Standard_Size                    aChunkSize = aNbChunkTriangles * THE_STL_SIZEOF_FACET;
  NCollection_Array1<Standard_Character> aData(1, aChunkSize);
  Standard_Character*                    aDataChunk = &aData.ChangeFirst();
  Standard_Character                     aConv[4];
  convertInteger(aNBTriangles, aConv);
  theStream.write(aConv, 4);
  if (theStream.fail())
  {
    return Standard_False;
  }

  Standard_Size aByteCount = 0;
  for (Standard_Integer aTriIter = 1; aTriIter <= aNBTriangles; ++aTriIter)
  {
    Standard_Integer    id[3];
    const Poly_Triangle aTriangle = theMesh->Triangle(aTriIter);
    aTriangle.Get(id[0], id[1], id[2]);
    const gp_Pnt aP1 = theMesh->Node(id[0]);
    const gp_Pnt aP2 = theMesh->Node(id[1]);
    const gp_Pnt aP3 = theMesh->Node(id[2]);
    gp_Vec       aVec1(aP1, aP2);
    gp_Vec       aVec2(aP1, aP3);
    gp_Vec       aVNorm = aVec1.Crossed(aVec2);
    if (aVNorm.SquareMagnitude() > gp::Resolution())
    {
      aVNorm.Normalize();
    }
    else
    {
      aVNorm.SetCoord(0.0, 0.0, 0.0);
    }
    convertDouble(aVNorm.X(), &aDataChunk[aByteCount]);
    aByteCount += 4;
    convertDouble(aVNorm.Y(), &aDataChunk[aByteCount]);
    aByteCount += 4;
    convertDouble(aVNorm.Z(), &aDataChunk[aByteCount]);
    aByteCount += 4;
    convertDouble(aP1.X(), &aDataChunk[aByteCount]);
    aByteCount += 4;
    convertDouble(aP1.Y(), &aDataChunk[aByteCount]);
    aByteCount += 4;
    convertDouble(aP1.Z(), &aDataChunk[aByteCount]);
    aByteCount += 4;
    convertDouble(aP2.X(), &aDataChunk[aByteCount]);
    aByteCount += 4;
    convertDouble(aP2.Y(), &aDataChunk[aByteCount]);
    aByteCount += 4;
    convertDouble(aP2.Z(), &aDataChunk[aByteCount]);
    aByteCount += 4;
    convertDouble(aP3.X(), &aDataChunk[aByteCount]);
    aByteCount += 4;
    convertDouble(aP3.Y(), &aDataChunk[aByteCount]);
    aByteCount += 4;
    convertDouble(aP3.Z(), &aDataChunk[aByteCount]);
    aByteCount += 4;
    aDataChunk[aByteCount]     = 0;
    aDataChunk[aByteCount + 1] = 0;
    aByteCount += 2;

    // Chunk is full, write it out.
    if (aByteCount == aChunkSize)
    {
      theStream.write(aDataChunk, aChunkSize);
      if (theStream.fail())
      {
        return Standard_False;
      }
      aByteCount = 0;
    }

    // update progress only per 1k triangles
    if ((aTriIter % IND_THRESHOLD) == 0)
    {
      if (!aPS.More())
        return Standard_False;
      aPS.Next(IND_THRESHOLD);
    }
  }

  // Write last part if necessary.
  if (aByteCount != aChunkSize)
  {
    theStream.write(aDataChunk, aByteCount);
    if (theStream.fail())
    {
      return Standard_False;
    }
  }

  return Standard_True;
}

//=================================================================================================

Handle(Poly_Triangulation) RWStl::ReadBinaryStream(Standard_IStream&            theStream,
                                                   const Standard_Real          theMergeAngle,
                                                   const Message_ProgressRange& theProgress)
{
  Reader aReader;
  aReader.SetMergeAngle(theMergeAngle);
  if (!aReader.ReadBinary(theStream, theProgress))
  {
    return Handle(Poly_Triangulation)();
  }
  return aReader.GetTriangulation();
}

//=================================================================================================

Handle(Poly_Triangulation) RWStl::ReadAsciiStream(Standard_IStream&            theStream,
                                                  const Standard_Real          theMergeAngle,
                                                  const Message_ProgressRange& theProgress)
{
  Reader aReader;
  aReader.SetMergeAngle(theMergeAngle);

  // get length of stream to feed progress indicator
  theStream.seekg(0, theStream.end);
  std::streampos theEnd = theStream.tellg();
  theStream.seekg(0, theStream.beg);

  Standard_ReadLineBuffer aBuffer(THE_BUFFER_SIZE);
  if (!aReader.ReadAscii(theStream, aBuffer, theEnd, theProgress))
  {
    return Handle(Poly_Triangulation)();
  }
  return aReader.GetTriangulation();
}

//=================================================================================================

Handle(Poly_Triangulation) RWStl::ReadStream(Standard_IStream&            theStream,
                                             const Standard_Real          theMergeAngle,
                                             const Message_ProgressRange& theProgress)
{
  // Try to detect ASCII vs Binary format by peeking at the first few bytes
  std::streampos            anOriginalPos = theStream.tellg();
  constexpr std::streamsize aHeaderSize   = 6;
  char                      aHeader[aHeaderSize];
  theStream.read(aHeader, aHeaderSize - 1);
  aHeader[aHeaderSize - 1] = '\0';
  theStream.seekg(anOriginalPos);

  bool isAscii = (strncmp(aHeader, "solid", 5) == 0);

  if (isAscii)
  {
    return RWStl::ReadAsciiStream(theStream, theMergeAngle, theProgress);
  }
  else
  {
    return RWStl::ReadBinaryStream(theStream, theMergeAngle, theProgress);
  }
}
