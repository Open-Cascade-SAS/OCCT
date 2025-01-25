// Copyright (c) 2015-2021 OPEN CASCADE SAS
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

#include <RWObj_ObjWriterContext.hxx>

#include <Message.hxx>
#include <NCollection_IndexedMap.hxx>
#include <OSD_OpenFile.hxx>

// =======================================================================
// function : splitLines
// purpose  :
// =======================================================================
static void splitLines(const TCollection_AsciiString&                   theString,
                       NCollection_IndexedMap<TCollection_AsciiString>& theLines)
{
  if (theString.IsEmpty())
  {
    return;
  }

  Standard_Integer aLineFrom = 1;
  for (Standard_Integer aCharIter = 1;; ++aCharIter)
  {
    const char aChar = theString.Value(aCharIter);
    if (aChar != '\r' && aChar != '\n' && aCharIter != theString.Length())
    {
      continue;
    }

    if (aLineFrom != aCharIter)
    {
      TCollection_AsciiString aLine = theString.SubString(aLineFrom, aCharIter);
      aLine.RightAdjust();
      theLines.Add(aLine);
    }

    if (aCharIter == theString.Length())
    {
      break;
    }
    else if (aChar == '\r' && theString.Value(aCharIter + 1) == '\n')
    {
      // CRLF
      ++aCharIter;
    }
    aLineFrom = aCharIter + 1;
  }
}

// ================================================================
// Function : RWObj_ObjWriterContext
// Purpose  :
// ================================================================
RWObj_ObjWriterContext::RWObj_ObjWriterContext(const TCollection_AsciiString& theName)
    : NbFaces(0),
      myFile(OSD_OpenFile(theName.ToCString(), "wb")),
      myName(theName),
      myElemPosFirst(1, 1, 1, 1),
      myElemNormFirst(1, 1, 1, 1),
      myElemUVFirst(1, 1, 1, 1),
      myHasNormals(false),
      myHasTexCoords(false)
{
  if (myFile == NULL)
  {
    Message::SendFail(TCollection_AsciiString("File cannot be created\n") + theName);
    return;
  }
}

// ================================================================
// Function : ~RWObj_ObjWriterContext
// Purpose  :
// ================================================================
RWObj_ObjWriterContext::~RWObj_ObjWriterContext()
{
  if (myFile != NULL)
  {
    ::fclose(myFile);
    Message::SendFail(TCollection_AsciiString("File cannot be written\n") + myName);
  }
}

// ================================================================
// Function : Close
// Purpose  :
// ================================================================
bool RWObj_ObjWriterContext::Close()
{
  bool isOk = ::fclose(myFile) == 0;
  myFile    = NULL;
  return isOk;
}

// ================================================================
// Function : WriteHeader
// Purpose  :
// ================================================================
bool RWObj_ObjWriterContext::WriteHeader(const Standard_Integer                      theNbNodes,
                                         const Standard_Integer                      theNbElems,
                                         const TCollection_AsciiString&              theMatLib,
                                         const TColStd_IndexedDataMapOfStringString& theFileInfo)
{
  bool isOk = ::Fprintf(myFile,
                        "# Exported by Open CASCADE Technology [dev.opencascade.org]\n"
                        "#  Vertices: %d\n"
                        "#     Faces: %d\n",
                        theNbNodes,
                        theNbElems)
              != 0;
  for (TColStd_IndexedDataMapOfStringString::Iterator aKeyValueIter(theFileInfo);
       aKeyValueIter.More();
       aKeyValueIter.Next())
  {
    NCollection_IndexedMap<TCollection_AsciiString> aKeyLines, aValLines;
    splitLines(aKeyValueIter.Key(), aKeyLines);
    splitLines(aKeyValueIter.Value(), aValLines);
    for (Standard_Integer aLineIter = 1; aLineIter <= aKeyLines.Extent(); ++aLineIter)
    {
      const TCollection_AsciiString& aLine = aKeyLines.FindKey(aLineIter);
      isOk = isOk && ::Fprintf(myFile, aLineIter > 1 ? "\n# %s" : "# %s", aLine.ToCString()) != 0;
    }
    isOk = isOk && ::Fprintf(myFile, !aKeyLines.IsEmpty() ? ":" : "# ") != 0;
    for (Standard_Integer aLineIter = 1; aLineIter <= aValLines.Extent(); ++aLineIter)
    {
      const TCollection_AsciiString& aLine = aValLines.FindKey(aLineIter);
      isOk = isOk && ::Fprintf(myFile, aLineIter > 1 ? "\n# %s" : " %s", aLine.ToCString()) != 0;
    }
    isOk = isOk && ::Fprintf(myFile, "\n") != 0;
  }

  if (!theMatLib.IsEmpty())
  {
    isOk = isOk && ::Fprintf(myFile, "mtllib %s\n", theMatLib.ToCString()) != 0;
  }
  return isOk;
}

// ================================================================
// Function : WriteActiveMaterial
// Purpose  :
// ================================================================
bool RWObj_ObjWriterContext::WriteActiveMaterial(const TCollection_AsciiString& theMaterial)
{
  myActiveMaterial = theMaterial;
  return !theMaterial.IsEmpty() ? Fprintf(myFile, "usemtl %s\n", theMaterial.ToCString()) != 0
                                : Fprintf(myFile, "usemtl\n") != 0;
}

// ================================================================
// Function : WriteTriangle
// Purpose  :
// ================================================================
bool RWObj_ObjWriterContext::WriteTriangle(const Graphic3d_Vec3i& theTri)
{
  const Graphic3d_Vec3i aTriPos = theTri + myElemPosFirst.xyz();
  if (myHasNormals)
  {
    const Graphic3d_Vec3i aTriNorm = theTri + myElemNormFirst.xyz();
    if (myHasTexCoords)
    {
      const Graphic3d_Vec3i aTriUv = theTri + myElemUVFirst.xyz();
      return Fprintf(myFile,
                     "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
                     aTriPos[0],
                     aTriUv[0],
                     aTriNorm[0],
                     aTriPos[1],
                     aTriUv[1],
                     aTriNorm[1],
                     aTriPos[2],
                     aTriUv[2],
                     aTriNorm[2])
             != 0;
    }
    else
    {
      return Fprintf(myFile,
                     "f %d//%d %d//%d %d//%d\n",
                     aTriPos[0],
                     aTriNorm[0],
                     aTriPos[1],
                     aTriNorm[1],
                     aTriPos[2],
                     aTriNorm[2])
             != 0;
    }
  }
  if (myHasTexCoords)
  {
    const Graphic3d_Vec3i aTriUv = theTri + myElemUVFirst.xyz();
    return Fprintf(myFile,
                   "f %d/%d %d/%d %d/%d\n",
                   aTriPos[0],
                   aTriUv[0],
                   aTriPos[1],
                   aTriUv[1],
                   aTriPos[2],
                   aTriUv[2])
           != 0;
  }
  else
  {
    return Fprintf(myFile, "f %d %d %d\n", aTriPos[0], aTriPos[1], aTriPos[2]) != 0;
  }
}

// ================================================================
// Function : WriteQuad
// Purpose  :
// ================================================================
bool RWObj_ObjWriterContext::WriteQuad(const Graphic3d_Vec4i& theQuad)
{
  const Graphic3d_Vec4i aQPos = theQuad + myElemPosFirst;
  if (myHasNormals)
  {
    const Graphic3d_Vec4i aQNorm = theQuad + myElemNormFirst;
    if (myHasTexCoords)
    {
      const Graphic3d_Vec4i aQTex = theQuad + myElemUVFirst;
      return Fprintf(myFile,
                     "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n",
                     aQPos[0],
                     aQTex[0],
                     aQNorm[0],
                     aQPos[1],
                     aQTex[1],
                     aQNorm[1],
                     aQPos[2],
                     aQTex[2],
                     aQNorm[2],
                     aQPos[3],
                     aQTex[3],
                     aQNorm[3])
             != 0;
    }
    else
    {
      return Fprintf(myFile,
                     "f %d//%d %d//%d %d//%d %d//%d\n",
                     aQPos[0],
                     aQNorm[0],
                     aQPos[1],
                     aQNorm[1],
                     aQPos[2],
                     aQNorm[2],
                     aQPos[3],
                     aQNorm[3])
             != 0;
    }
  }
  if (myHasTexCoords)
  {
    const Graphic3d_Vec4i aQTex = theQuad + myElemUVFirst;
    return Fprintf(myFile,
                   "f %d/%d %d/%d %d/%d %d/%d\n",
                   aQPos[0],
                   aQTex[0],
                   aQPos[1],
                   aQTex[1],
                   aQPos[2],
                   aQTex[2],
                   aQPos[3],
                   aQTex[3])
           != 0;
  }
  else
  {
    return Fprintf(myFile, "f %d %d %d %d\n", aQPos[0], aQPos[1], aQPos[2], aQPos[3]) != 0;
  }
}

// ================================================================
// Function : WriteVertex
// Purpose  :
// ================================================================
bool RWObj_ObjWriterContext::WriteVertex(const Graphic3d_Vec3& theValue)
{
  return Fprintf(myFile, "v %f %f %f\n", theValue.x(), theValue.y(), theValue.z()) != 0;
}

// ================================================================
// Function : WriteNormal
// Purpose  :
// ================================================================
bool RWObj_ObjWriterContext::WriteNormal(const Graphic3d_Vec3& theValue)
{
  return Fprintf(myFile, "vn %f %f %f\n", theValue.x(), theValue.y(), theValue.z()) != 0;
}

// ================================================================
// Function : WriteTexCoord
// Purpose  :
// ================================================================
bool RWObj_ObjWriterContext::WriteTexCoord(const Graphic3d_Vec2& theValue)
{
  return Fprintf(myFile, "vt %f %f\n", theValue.x(), theValue.y()) != 0;
}

// ================================================================
// Function : WriteGroup
// Purpose  :
// ================================================================
bool RWObj_ObjWriterContext::WriteGroup(const TCollection_AsciiString& theValue)
{
  return !theValue.IsEmpty() ? Fprintf(myFile, "g %s\n", theValue.ToCString()) != 0
                             : Fprintf(myFile, "g\n") != 0;
}

// ================================================================
// Function : FlushFace
// Purpose  :
// ================================================================
void RWObj_ObjWriterContext::FlushFace(Standard_Integer theNbNodes)
{
  Graphic3d_Vec4i aShift(theNbNodes, theNbNodes, theNbNodes, theNbNodes);
  myElemPosFirst += aShift;
  if (myHasNormals)
  {
    myElemNormFirst += aShift;
  }
  if (myHasTexCoords)
  {
    myElemUVFirst += aShift;
  }
}
