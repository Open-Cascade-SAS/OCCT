// Copyright (c) 2022 OPEN CASCADE SAS
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

#include <RWPly_PlyWriterContext.hxx>

#include <Message.hxx>
#include <NCollection_IndexedMap.hxx>
#include <OSD_FileSystem.hxx>

// =======================================================================
// function : splitLines
// purpose  :
// =======================================================================
static void splitLines (const TCollection_AsciiString& theString,
                        NCollection_IndexedMap<TCollection_AsciiString>& theLines)
{
  if (theString.IsEmpty())
  {
    return;
  }

  Standard_Integer aLineFrom = 1;
  for (Standard_Integer aCharIter = 1;; ++aCharIter)
  {
    const char aChar = theString.Value (aCharIter);
    if (aChar != '\r'
     && aChar != '\n'
     && aCharIter != theString.Length())
    {
      continue;
    }

    if (aLineFrom != aCharIter)
    {
      TCollection_AsciiString aLine = theString.SubString (aLineFrom, aCharIter);
      aLine.RightAdjust();
      theLines.Add (aLine);
    }

    if (aCharIter == theString.Length())
    {
      break;
    }
    else if (aChar == '\r'
          && theString.Value (aCharIter + 1) == '\n')
    {
      // CRLF
      ++aCharIter;
    }
    aLineFrom = aCharIter + 1;
  }
}

// ================================================================
// Function : RWPly_PlyWriterContext
// Purpose  :
// ================================================================
RWPly_PlyWriterContext::RWPly_PlyWriterContext()
: myNbHeaderVerts (0),
  myNbHeaderElems (0),
  myNbVerts (0),
  myNbElems (0),
  mySurfId (0),
  myVertOffset (0),
  myIsDoublePrec (false),
  myHasNormals   (false),
  myHasColors    (false),
  myHasTexCoords (false),
  myHasSurfId    (false)
{
  //
}

// ================================================================
// Function : ~RWPly_PlyWriterContext
// Purpose  :
// ================================================================
RWPly_PlyWriterContext::~RWPly_PlyWriterContext()
{
  Close();
}

// ================================================================
// Function : Open
// Purpose  :
// ================================================================
bool RWPly_PlyWriterContext::Open (const TCollection_AsciiString& theName,
                                   const std::shared_ptr<std::ostream>& theStream)
{
  myName = theName;
  myNbHeaderVerts = myNbHeaderElems = 0;
  myNbVerts = myNbElems = 0;
  if (theStream.get() != nullptr)
  {
    myStream = theStream;
    return true;
  }

  const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  myStream = aFileSystem->OpenOStream (theName, std::ios::out | std::ios::binary);
  if (myStream.get() == NULL || !myStream->good())
  {
    myStream.reset();
    Message::SendFail() << "Error: file cannot be created\n" << theName;
    return false;
  }
  return true;
}

// ================================================================
// Function : Close
// Purpose  :
// ================================================================
bool RWPly_PlyWriterContext::Close (bool theIsAborted)
{
  if (myStream.get() == nullptr)
  {
    return false;
  }

  myStream->flush();
  bool aResult = myStream->good();
  if (!aResult)
  {
    Message::SendFail() << "Error: file cannot be written\n" << myName;
  }
  else if (!theIsAborted)
  {
    if (myNbVerts != myNbHeaderVerts)
    {
      Message::SendFail() << "Error: written less number of vertices (" << myNbVerts << ") than specified in PLY header (" << myNbHeaderVerts << ")";
    }
    else if (myNbElems != myNbHeaderElems)
    {
      Message::SendFail() << "Error: written less number of elements (" << myNbElems << ") than specified in PLY header (" << myNbHeaderElems << ")";
    }
  }
  myStream.reset();
  return aResult;
}

// ================================================================
// Function : WriteHeader
// Purpose  :
// ================================================================
bool RWPly_PlyWriterContext::WriteHeader (const Standard_Integer theNbNodes,
                                          const Standard_Integer theNbElems,
                                          const TColStd_IndexedDataMapOfStringString& theFileInfo)
{
  if (myStream.get() == nullptr)
  {
    return false;
  }

  myNbHeaderVerts = theNbNodes;
  myNbHeaderElems = theNbElems;
  *myStream << "ply\n"
               "format ascii 1.0\n"
               "comment Exported by Open CASCADE Technology [dev.opencascade.org]\n";
  for (TColStd_IndexedDataMapOfStringString::Iterator aKeyValueIter (theFileInfo); aKeyValueIter.More(); aKeyValueIter.Next())
  {
    NCollection_IndexedMap<TCollection_AsciiString> aKeyLines, aValLines;
    splitLines (aKeyValueIter.Key(),   aKeyLines);
    splitLines (aKeyValueIter.Value(), aValLines);
    for (Standard_Integer aLineIter = 1; aLineIter <= aKeyLines.Extent(); ++aLineIter)
    {
      const TCollection_AsciiString& aLine = aKeyLines.FindKey (aLineIter);
      *myStream << (aLineIter > 1 ? "\n" : "") << "comment " << aLine;
    }
    *myStream << (!aKeyLines.IsEmpty() ? ":" : "comment ");
    for (Standard_Integer aLineIter = 1; aLineIter <= aValLines.Extent(); ++aLineIter)
    {
      const TCollection_AsciiString& aLine = aValLines.FindKey (aLineIter);
      *myStream << (aLineIter > 1 ? "\n" : "") << "comment " << aLine;
    }
    *myStream << "\n";
  }

  *myStream << "element vertex " << theNbNodes<< "\n";
  if (myIsDoublePrec)
  {
    *myStream << "property double x\n"
                 "property double y\n"
                 "property double z\n";
  }
  else
  {
    *myStream << "property float x\n"
                 "property float y\n"
                 "property float z\n";
  }
  if (myHasNormals)
  {
    *myStream << "property float nx\n"
                 "property float ny\n"
                 "property float nz\n";
  }
  if (myHasTexCoords)
  {
    *myStream << "property float s\n"
                 "property float t\n";
  }
  if (myHasColors)
  {
    *myStream << "property uchar red\n"
                 "property uchar green\n"
                 "property uchar blue\n";
  }

  if (theNbElems > 0)
  {
    *myStream << "element face " << theNbElems << "\n"
                 "property list uchar uint vertex_indices\n";
    if (myHasSurfId)
    {
      *myStream << "property uint SurfaceID\n";
    }
  }

  *myStream << "end_header\n";
  return myStream->good();
}

// ================================================================
// Function : WriteVertex
// Purpose  :
// ================================================================
bool RWPly_PlyWriterContext::WriteVertex (const gp_Pnt& thePoint,
                                          const Graphic3d_Vec3& theNorm,
                                          const Graphic3d_Vec2& theUV,
                                          const Graphic3d_Vec4ub& theColor)
{
  if (myStream.get() == nullptr)
  {
    return false;
  }

  if (myIsDoublePrec)
  {
    *myStream << (double )thePoint.X() << " " << (double )thePoint.Y() << " " << (double )thePoint.Z();
  }
  else
  {
    *myStream << (float )thePoint.X() << " " << (float )thePoint.Y() << " " << (float )thePoint.Z();
  }
  if (myHasNormals)
  {
    *myStream << " " << (float )theNorm.x() << " " << (float )theNorm.y() << " " << (float )theNorm.z();
  }
  if (myHasTexCoords)
  {
    *myStream << " " << (float )theUV.x() << " " << (float )theUV.y();
  }
  if (myHasColors)
  {
    *myStream << " " << (int )theColor.r() << " " << (int )theColor.g() << " " << (int )theColor.b();
  }
  *myStream << "\n";
  if (++myNbVerts > myNbHeaderVerts)
  {
    throw Standard_OutOfRange ("RWPly_PlyWriterContext::WriteVertex() - number of vertices is greater than defined");
  }
  return myStream->good();
}

// ================================================================
// Function : WriteTriangle
// Purpose  :
// ================================================================
bool RWPly_PlyWriterContext::WriteTriangle (const Graphic3d_Vec3i& theTri)
{
  if (myStream.get() == nullptr)
  {
    return false;
  }

  const Graphic3d_Vec3i aTri = Graphic3d_Vec3i(myVertOffset) + theTri;
  *myStream << "3 " << aTri[0] << " " << aTri[1] << " " << aTri[2];
  if (myHasSurfId)
  {
    *myStream << " " << mySurfId;
  }
  *myStream << "\n";
  if (++myNbElems > myNbHeaderElems)
  {
    throw Standard_OutOfRange ("RWPly_PlyWriterContext::WriteTriangle() - number of elements is greater than defined");
  }
  return myStream->good();
}

// ================================================================
// Function : WriteQuad
// Purpose  :
// ================================================================
bool RWPly_PlyWriterContext::WriteQuad (const Graphic3d_Vec4i& theQuad)
{
  if (myStream.get() == nullptr)
  {
    return false;
  }

  const Graphic3d_Vec4i aQuad = Graphic3d_Vec4i(myVertOffset) + theQuad;
  *myStream << "4 " << aQuad[0] << " " << aQuad[1] << " " << aQuad[2] << " " << aQuad[3];
  if (myHasSurfId)
  {
    *myStream << " " << mySurfId;
  }
  *myStream << "\n";
  if (++myNbElems > myNbHeaderElems)
  {
    throw Standard_OutOfRange ("RWPly_PlyWriterContext::WriteQuad() - number of elements is greater than defined");
  }
  return myStream->good();
}
