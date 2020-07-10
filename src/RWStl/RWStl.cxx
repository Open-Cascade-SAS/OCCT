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
#include <OSD_OpenFile.hxx>
#include <RWStl_Reader.hxx>

namespace
{

  static const Standard_Integer THE_STL_SIZEOF_FACET = 50;
  static const Standard_Integer IND_THRESHOLD = 1000; // increment the indicator every 1k triangles
  static const size_t THE_BUFFER_SIZE = 1024; // The length of buffer to read (in bytes)

  //! Writing a Little Endian 32 bits integer
  inline static void convertInteger (const Standard_Integer theValue,
                                     Standard_Character* theResult)
  {
    union
    {
      Standard_Integer i;
      Standard_Character c[4];
    } anUnion;
    anUnion.i = theValue;

    theResult[0] = anUnion.c[0];
    theResult[1] = anUnion.c[1];
    theResult[2] = anUnion.c[2];
    theResult[3] = anUnion.c[3];
  }

  //! Writing a Little Endian 32 bits float
  inline static void convertDouble (const Standard_Real theValue,
                                    Standard_Character* theResult)
  {
    union
    {
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
    virtual Standard_Integer AddNode (const gp_XYZ& thePnt) Standard_OVERRIDE
    {
      myNodes.Append (thePnt);
      return myNodes.Size();
    }

    //! Add new triangle
    virtual void AddTriangle (Standard_Integer theNode1, Standard_Integer theNode2, Standard_Integer theNode3) Standard_OVERRIDE
    {
      myTriangles.Append (Poly_Triangle (theNode1, theNode2, theNode3));
    }

    //! Creates Poly_Triangulation from collected data
    Handle(Poly_Triangulation) GetTriangulation()
    {
      if (myTriangles.IsEmpty())
        return Handle(Poly_Triangulation)();

      Handle(Poly_Triangulation) aPoly = new Poly_Triangulation (myNodes.Length(), myTriangles.Length(), Standard_False);
      for (Standard_Integer aNodeIter = 0; aNodeIter < myNodes.Size(); ++aNodeIter)
      {
        aPoly->ChangeNode (aNodeIter + 1) = myNodes (aNodeIter);
      }

      for (Standard_Integer aTriIter = 0; aTriIter < myTriangles.Size(); ++aTriIter)
      {
        aPoly->ChangeTriangle (aTriIter + 1) = myTriangles (aTriIter);
      }

      return aPoly;
    }

  private:
    NCollection_Vector<gp_XYZ> myNodes;
    NCollection_Vector<Poly_Triangle> myTriangles;
  };

}

//=============================================================================
//function : Read
//purpose  :
//=============================================================================
Handle(Poly_Triangulation) RWStl::ReadFile (const Standard_CString theFile,
                                            const Message_ProgressRange& theProgress)
{
  Reader aReader;
  aReader.Read (theFile, theProgress);
  // note that returned bool value is ignored intentionally -- even if something went wrong,
  // but some data have been read, we at least will return these data
  return aReader.GetTriangulation();
}

//=============================================================================
//function : ReadFile
//purpose  :
//=============================================================================
Handle(Poly_Triangulation) RWStl::ReadFile (const OSD_Path& theFile,
                                            const Message_ProgressRange& theProgress)
{
  OSD_File aFile(theFile);
  if (!aFile.Exists())
  {
    return Handle(Poly_Triangulation)();
  }

  TCollection_AsciiString aPath;
  theFile.SystemName (aPath);
  return ReadFile (aPath.ToCString(), theProgress);
}

//=============================================================================
//function : ReadBinary
//purpose  :
//=============================================================================
Handle(Poly_Triangulation) RWStl::ReadBinary (const OSD_Path& theFile,
                                              const Message_ProgressRange& theProgress)
{
  OSD_File aFile(theFile);
  if (!aFile.Exists())
  {
    return Handle(Poly_Triangulation)();
  }

  TCollection_AsciiString aPath;
  theFile.SystemName (aPath);

  std::filebuf aBuf;
  OSD_OpenStream (aBuf, aPath, std::ios::in | std::ios::binary);
  if (!aBuf.is_open())
  {
    return Handle(Poly_Triangulation)();
  }
  Standard_IStream aStream (&aBuf);

  Reader aReader;
  if (!aReader.ReadBinary (aStream, theProgress))
  {
    return Handle(Poly_Triangulation)();
  }

  return aReader.GetTriangulation();
}

//=============================================================================
//function : ReadAscii
//purpose  :
//=============================================================================
Handle(Poly_Triangulation) RWStl::ReadAscii (const OSD_Path& theFile,
                                             const Message_ProgressRange& theProgress)
{
  OSD_File aFile (theFile);
  if (!aFile.Exists())
  {
    return Handle(Poly_Triangulation)();
  }

  TCollection_AsciiString aPath;
  theFile.SystemName (aPath);

  std::filebuf aBuf;
  OSD_OpenStream (aBuf, aPath, std::ios::in | std::ios::binary);
  if (!aBuf.is_open())
  {
    return Handle(Poly_Triangulation)();
  }
  Standard_IStream aStream (&aBuf);

  // get length of file to feed progress indicator
  aStream.seekg (0, aStream.end);
  std::streampos theEnd = aStream.tellg();
  aStream.seekg (0, aStream.beg);

  Reader aReader;
  Standard_ReadLineBuffer aBuffer (THE_BUFFER_SIZE);
  if (!aReader.ReadAscii (aStream, aBuffer, theEnd, theProgress))
  {
    return Handle(Poly_Triangulation)();
  }

  return aReader.GetTriangulation();
}

//=============================================================================
//function : Write
//purpose  :
//=============================================================================
Standard_Boolean RWStl::WriteBinary (const Handle(Poly_Triangulation)& theMesh,
                                     const OSD_Path& thePath,
                                     const Message_ProgressRange& theProgress)
{
  if (theMesh.IsNull() || theMesh->NbTriangles() <= 0)
  {
    return Standard_False;
  }

  TCollection_AsciiString aPath;
  thePath.SystemName (aPath);

  FILE* aFile = OSD_OpenFile (aPath, "wb");
  if (aFile == NULL)
  {
    return Standard_False;
  }

  Standard_Boolean isOK = writeBinary (theMesh, aFile, theProgress);

  fclose (aFile);
  return isOK;
}

//=============================================================================
//function : Write
//purpose  :
//=============================================================================
Standard_Boolean RWStl::WriteAscii (const Handle(Poly_Triangulation)& theMesh,
                                    const OSD_Path& thePath,
                                    const Message_ProgressRange& theProgress)
{
  if (theMesh.IsNull() || theMesh->NbTriangles() <= 0)
  {
    return Standard_False;
  }

  TCollection_AsciiString aPath;
  thePath.SystemName (aPath);

  FILE* aFile = OSD_OpenFile (aPath, "w");
  if (aFile == NULL)
  {
    return Standard_False;
  }

  Standard_Boolean isOK = writeASCII (theMesh, aFile, theProgress);
  fclose (aFile);
  return isOK;
}

//=============================================================================
//function : writeASCII
//purpose  :
//=============================================================================
Standard_Boolean RWStl::writeASCII (const Handle(Poly_Triangulation)& theMesh,
                                    FILE* theFile,
                                    const Message_ProgressRange& theProgress)
{
  // note that space after 'solid' is necessary for many systems
  if (fwrite ("solid \n", 1, 7, theFile) != 7)
  {
    return Standard_False;
  }

  char aBuffer[512];
  memset (aBuffer, 0, sizeof(aBuffer));

  const Standard_Integer NBTriangles = theMesh->NbTriangles();
  Message_ProgressScope aPS (theProgress, "Triangles", NBTriangles);

  const TColgp_Array1OfPnt& aNodes = theMesh->Nodes();
  const Poly_Array1OfTriangle& aTriangles = theMesh->Triangles();
  Standard_Integer anElem[3] = {0, 0, 0};
  for (Standard_Integer aTriIter = 1; aTriIter <= NBTriangles; ++aTriIter)
  {
    const Poly_Triangle& aTriangle = aTriangles (aTriIter);
    aTriangle.Get (anElem[0], anElem[1], anElem[2]);

    const gp_Pnt aP1 = aNodes (anElem[0]);
    const gp_Pnt aP2 = aNodes (anElem[1]);
    const gp_Pnt aP3 = aNodes (anElem[2]);

    const gp_Vec aVec1 (aP1, aP2);
    const gp_Vec aVec2 (aP1, aP3);
    gp_Vec aVNorm = aVec1.Crossed (aVec2);
    if (aVNorm.SquareMagnitude() > gp::Resolution())
    {
      aVNorm.Normalize();
    }
    else
    {
      aVNorm.SetCoord (0.0, 0.0, 0.0);
    }

    Sprintf (aBuffer,
          " facet normal % 12e % 12e % 12e\n"
          "   outer loop\n"
          "     vertex % 12e % 12e % 12e\n"
          "     vertex % 12e % 12e % 12e\n"
          "     vertex % 12e % 12e % 12e\n"
          "   endloop\n"
          " endfacet\n",
          aVNorm.X(), aVNorm.Y(), aVNorm.Z(),
          aP1.X(), aP1.Y(), aP1.Z(),
          aP2.X(), aP2.Y(), aP2.Z(),
          aP3.X(), aP3.Y(), aP3.Z());

    if (fprintf (theFile, "%s", aBuffer) < 0)
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

  if (fwrite ("endsolid\n", 1, 9, theFile) != 9)
  {
    return Standard_False;
  }

  return Standard_True;
}

//=============================================================================
//function : writeBinary
//purpose  :
//=============================================================================
Standard_Boolean RWStl::writeBinary (const Handle(Poly_Triangulation)& theMesh,
                                     FILE* theFile,
                                     const Message_ProgressRange& theProgress)
{
  char aHeader[80] = "STL Exported by OpenCASCADE [www.opencascade.com]";
  if (fwrite (aHeader, 1, 80, theFile) != 80)
  {
    return Standard_False;
  }
  
  const Standard_Integer aNBTriangles = theMesh->NbTriangles();
  Message_ProgressScope aPS (theProgress, "Triangles", aNBTriangles);

  const Standard_Size aNbChunkTriangles = 4096;
  const Standard_Size aChunkSize = aNbChunkTriangles * THE_STL_SIZEOF_FACET;
  NCollection_Array1<Standard_Character> aData (1, aChunkSize);
  Standard_Character* aDataChunk = &aData.ChangeFirst();

  const TColgp_Array1OfPnt& aNodes = theMesh->Nodes();
  const Poly_Array1OfTriangle& aTriangles = theMesh->Triangles();

  Standard_Character aConv[4];
  convertInteger (aNBTriangles, aConv);
  if (fwrite (aConv, 1, 4, theFile) != 4)
  {
    return Standard_False;
  }

  Standard_Size aByteCount = 0;
  for (Standard_Integer aTriIter = 1; aTriIter <= aNBTriangles; ++aTriIter)
  {
    Standard_Integer id[3];
    const Poly_Triangle& aTriangle = aTriangles (aTriIter);
    aTriangle.Get (id[0], id[1], id[2]);

    const gp_Pnt aP1 = aNodes (id[0]);
    const gp_Pnt aP2 = aNodes (id[1]);
    const gp_Pnt aP3 = aNodes (id[2]);

    gp_Vec aVec1 (aP1, aP2);
    gp_Vec aVec2 (aP1, aP3);
    gp_Vec aVNorm = aVec1.Crossed(aVec2);
    if (aVNorm.SquareMagnitude() > gp::Resolution())
    {
      aVNorm.Normalize();
    }
    else
    {
      aVNorm.SetCoord (0.0, 0.0, 0.0);
    }

    convertDouble (aVNorm.X(), &aDataChunk[aByteCount]); aByteCount += 4;
    convertDouble (aVNorm.Y(), &aDataChunk[aByteCount]); aByteCount += 4;
    convertDouble (aVNorm.Z(), &aDataChunk[aByteCount]); aByteCount += 4;

    convertDouble (aP1.X(), &aDataChunk[aByteCount]); aByteCount += 4;
    convertDouble (aP1.Y(), &aDataChunk[aByteCount]); aByteCount += 4;
    convertDouble (aP1.Z(), &aDataChunk[aByteCount]); aByteCount += 4;

    convertDouble (aP2.X(), &aDataChunk[aByteCount]); aByteCount += 4;
    convertDouble (aP2.Y(), &aDataChunk[aByteCount]); aByteCount += 4;
    convertDouble (aP2.Z(), &aDataChunk[aByteCount]); aByteCount += 4;

    convertDouble (aP3.X(), &aDataChunk[aByteCount]); aByteCount += 4;
    convertDouble (aP3.Y(), &aDataChunk[aByteCount]); aByteCount += 4;
    convertDouble (aP3.Z(), &aDataChunk[aByteCount]); aByteCount += 4;

    aDataChunk[aByteCount] = 0; aByteCount += 1;
    aDataChunk[aByteCount] = 0; aByteCount += 1;

    // Chunk is filled. Dump it to the file.
    if (aByteCount == aChunkSize)
    {
      if (fwrite (aDataChunk, 1, aChunkSize, theFile) != aChunkSize)
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
    if (fwrite (aDataChunk, 1, aByteCount, theFile) != aByteCount)
    {
      return Standard_False;
    }
  }

  return Standard_True;
}
