// Author: Kirill Gavrilov
// Copyright (c) 2017-2019 OPEN CASCADE SAS
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

#include <RWObj_Reader.hxx>

#include <RWObj_MtlReader.hxx>

#include <BRepMesh_DataStructureOfDelaun.hxx>
#include <BRepMesh_Delaun.hxx>
#include <gp_XY.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_ProgressScope.hxx>
#include <NCollection_IncAllocator.hxx>
#include <OSD_OpenFile.hxx>
#include <OSD_Path.hxx>
#include <OSD_Timer.hxx>
#include <Precision.hxx>
#include <Standard_CLocaleSentry.hxx>
#include <Standard_ReadLineBuffer.hxx>

#include <algorithm>
#include <limits>

#if defined(_WIN32)
  #define ftell64(a)     _ftelli64(a)
  #define fseek64(a,b,c) _fseeki64(a,b,c)
#else
  #define ftell64(a)     ftello(a)
  #define fseek64(a,b,c) fseeko(a,b,c)
#endif

IMPLEMENT_STANDARD_RTTIEXT(RWObj_Reader, Standard_Transient)

namespace
{
  // The length of buffer to read (in bytes)
  static const size_t THE_BUFFER_SIZE = 4 * 1024;

  //! Simple wrapper.
  struct RWObj_ReaderFile
  {
    FILE*   File;
    int64_t FileLen;

    //! Constructor opening the file.
    RWObj_ReaderFile (const TCollection_AsciiString& theFile)
    : File (OSD_OpenFile (theFile.ToCString(), "rb")),
      FileLen (0)
    {
      if (this->File != NULL)
      {
        // determine length of file
        ::fseek64 (this->File, 0, SEEK_END);
        FileLen = ::ftell64 (this->File);
        ::fseek64 (this->File, 0, SEEK_SET);
      }
    }

    //! Destructor closing the file.
    ~RWObj_ReaderFile()
    {
      if (File != NULL)
      {
        ::fclose (File);
      }
    }
  };

  //! Return TRUE if given polygon has clockwise node order.
  static bool isClockwisePolygon (const Handle(BRepMesh_DataStructureOfDelaun)& theMesh,
                                  const IMeshData::VectorOfInteger& theIndexes)
  {
    double aPtSum = 0;
    const int aNbElemNodes = theIndexes.Size();
    for (int aNodeIter = theIndexes.Lower(); aNodeIter <= theIndexes.Upper(); ++aNodeIter)
    {
      int aNodeNext = theIndexes.Lower() + ((aNodeIter + 1) % aNbElemNodes);
      const BRepMesh_Vertex& aVert1 = theMesh->GetNode (theIndexes.Value (aNodeIter));
      const BRepMesh_Vertex& aVert2 = theMesh->GetNode (theIndexes.Value (aNodeNext));
      aPtSum += (aVert2.Coord().X() - aVert1.Coord().X())
              * (aVert2.Coord().Y() + aVert1.Coord().Y());
    }
    return aPtSum < 0.0;
  }
}

// ================================================================
// Function : Read
// Purpose  :
// ================================================================
RWObj_Reader::RWObj_Reader()
: myMemLimitBytes (Standard_Size(-1)),
  myMemEstim (0),
  myNbLines (0),
  myNbProbeNodes (0),
  myNbProbeElems (0),
  myNbElemsBig (0),
  myToAbort (false)
{
  //
}

// ================================================================
// Function : read
// Purpose  :
// ================================================================
Standard_Boolean RWObj_Reader::read (const TCollection_AsciiString& theFile,
                                     const Message_ProgressRange& theProgress,
                                     const Standard_Boolean theToProbe)
{
  myMemEstim = 0;
  myNbLines = 0;
  myNbProbeNodes = 0;
  myNbProbeElems = 0;
  myNbElemsBig = 0;
  myToAbort = false;
  myObjVerts.Reset();
  myObjVertsUV.Clear();
  myObjNorms.Clear();
  myPackedIndices.Clear();
  myMaterials.Clear();
  myFileComments.Clear();
  myExternalFiles.Clear();
  myActiveSubMesh = RWObj_SubMesh();

  // determine file location to load associated files
  TCollection_AsciiString aFileName;
  OSD_Path::FolderAndFileFromPath (theFile, myFolder, aFileName);
  myCurrElem.resize (1024, -1);

  Standard_CLocaleSentry aLocaleSentry;
  RWObj_ReaderFile aFile (theFile);
  if (aFile.File == NULL)
  {
    Message::SendFail (TCollection_AsciiString ("Error: file '") + theFile + "' is not found");
    return Standard_False;
  }

  // determine length of file
  const int64_t aFileLen = aFile.FileLen;
  if (aFileLen <= 0L)
  {
    Message::SendFail (TCollection_AsciiString ("Error: file '") + theFile + "' is empty");
    return Standard_False;
  }

  Standard_ReadLineBuffer aBuffer (THE_BUFFER_SIZE);
  aBuffer.SetMultilineMode (true);

  const Standard_Integer aNbMiBTotal  = Standard_Integer(aFileLen / (1024 * 1024));
  Standard_Integer       aNbMiBPassed = 0;
  Message_ProgressScope aPS (theProgress, "Reading text OBJ file", aNbMiBTotal);
  OSD_Timer aTimer;
  aTimer.Start();

  bool isStart = true;
  int64_t aPosition = 0;
  size_t aLineLen = 0;
  int64_t aReadBytes = 0;
  const char* aLine = NULL;
  for (;;)
  {
    aLine = aBuffer.ReadLine (aFile.File, aLineLen, aReadBytes);
    if (aLine == NULL)
    {
      break;
    }
    ++myNbLines;
    aPosition += aReadBytes;
    if (aTimer.ElapsedTime() > 1.0)
    {
      if (!aPS.More())
      {
        return false;
      }

      const Standard_Integer aNbMiBRead = Standard_Integer(aPosition / (1024 * 1024));
      aPS.Next (aNbMiBRead - aNbMiBPassed);
      aNbMiBPassed = aNbMiBRead;
      aTimer.Reset();
      aTimer.Start();
    }

    if (*aLine == '#')
    {
      if (isStart)
      {
        TCollection_AsciiString aComment (aLine + 1);
        aComment.LeftAdjust();
        aComment.RightAdjust();
        if (!aComment.IsEmpty())
        {
          if (!myFileComments.IsEmpty())
          {
            myFileComments += "\n";
          }
          myFileComments += aComment;
        }
      }
      continue;
    }
    else if (*aLine == '\n'
          || *aLine == '\0')
    {

      continue;
    }
    isStart = false;

    if (theToProbe)
    {
      if (::strncmp (aLine, "mtllib", 6) == 0)
      {
        readMaterialLib (IsSpace (aLine[6]) ? aLine + 7 : "");
      }
      else if (aLine[0] == 'v' && RWObj_Tools::isSpaceChar (aLine[1]))
      {
        ++myNbProbeNodes;
      }
      else if (aLine[0] == 'f' && RWObj_Tools::isSpaceChar (aLine[1]))
      {
        ++myNbProbeElems;
      }
      continue;
    }

    if (aLine[0] == 'v' && RWObj_Tools::isSpaceChar (aLine[1]))
    {
      ++myNbProbeNodes;
      pushVertex (aLine + 2);
    }
    else if (aLine[0] == 'v'
          && aLine[1] == 'n'
          && RWObj_Tools::isSpaceChar (aLine[2]))
    {
      pushNormal (aLine + 3);
    }
    else if (aLine[0] == 'v'
          && aLine[1] == 't'
          && RWObj_Tools::isSpaceChar (aLine[2]))
    {
      pushTexel (aLine + 3);
    }
    else if (aLine[0] == 'f' && RWObj_Tools::isSpaceChar (aLine[1]))
    {
      ++myNbProbeElems;
      pushIndices (aLine + 2);
    }
    else if (aLine[0] == 'g' && IsSpace (aLine[1]))
    {
      pushGroup (aLine + 2);
    }
    else if (aLine[0] == 's' && IsSpace (aLine[1]))
    {
      pushSmoothGroup (aLine + 2);
    }
    else if (aLine[0] == 'o' && IsSpace (aLine[1]))
    {
      pushObject (aLine + 2);
    }
    else if (::strncmp (aLine, "mtllib", 6) == 0)
    {
      readMaterialLib (IsSpace (aLine[6]) ? aLine + 7 : "");
    }
    else if (::strncmp (aLine, "usemtl", 6) == 0)
    {
      pushMaterial (IsSpace (aLine[6]) ? aLine + 7 : "");
    }

    if (!checkMemory())
    {
      addMesh (myActiveSubMesh, RWObj_SubMeshReason_NewObject);
      return false;
    }
  }

  // collect external references
  for (NCollection_DataMap<TCollection_AsciiString, RWObj_Material>::Iterator aMatIter (myMaterials); aMatIter.More(); aMatIter.Next())
  {
    const RWObj_Material& aMat = aMatIter.Value();
    if (!aMat.DiffuseTexture.IsEmpty())
    {
      myExternalFiles.Add (aMat.DiffuseTexture);
    }
    if (!aMat.SpecularTexture.IsEmpty())
    {
      myExternalFiles.Add (aMat.SpecularTexture);
    }
    if (!aMat.BumpTexture.IsEmpty())
    {
      myExternalFiles.Add (aMat.BumpTexture);
    }
  }

  // flush the last group
  if (!theToProbe)
  {
    addMesh (myActiveSubMesh, RWObj_SubMeshReason_NewObject);
  }
  if (myNbElemsBig != 0)
  {
    Message::SendWarning (TCollection_AsciiString("Warning: OBJ reader, ") + myNbElemsBig + " polygon(s) have been split into triangles");
  }

  return true;
}

// =======================================================================
// function : pushIndices
// purpose  :
// =======================================================================
void RWObj_Reader::pushIndices (const char* thePos)
{
  char* aNext = NULL;

  Standard_Integer aNbElemNodes = 0;
  for (Standard_Integer aNode = 0;; ++aNode)
  {
    Graphic3d_Vec3i a3Indices (-1, -1, -1);
    a3Indices[0] = strtol (thePos, &aNext, 10) - 1;
    if (aNext == thePos)
    {
      break;
    }

    // parse UV index
    thePos = aNext;
    if (*thePos == '/')
    {
      ++thePos;
      a3Indices[1] = strtol (thePos, &aNext, 10) - 1;
      thePos = aNext;

      // parse Normal index
      if (*thePos == '/')
      {
        ++thePos;
        a3Indices[2] = strtol (thePos, &aNext, 10) - 1;
        thePos = aNext;
      }
    }

    // handle negative indices
    if (a3Indices[0] < -1)
    {
      a3Indices[0] += myObjVerts.Upper() + 2;
    }
    if (a3Indices[1] < -1)
    {
      a3Indices[1] += myObjVertsUV.Upper() + 2;
    }
    if (a3Indices[2] < -1)
    {
      a3Indices[2] += myObjNorms.Upper() + 2;
    }

    Standard_Integer anIndex = -1;
    if (!myPackedIndices.Find (a3Indices, anIndex))
    {
      if (a3Indices[0] >= 0)
      {
        myMemEstim += sizeof(Graphic3d_Vec3);
      }
      if (a3Indices[1] >= 0)
      {
        myMemEstim += sizeof(Graphic3d_Vec2);
      }
      if (a3Indices[2] >= 0)
      {
        myMemEstim += sizeof(Graphic3d_Vec3);
      }
      myMemEstim += sizeof(Graphic3d_Vec4i) + sizeof(Standard_Integer); // naive map
      if (a3Indices[0] < myObjVerts.Lower() || a3Indices[0] > myObjVerts.Upper())
      {
        myToAbort = true;
        Message::SendFail (TCollection_AsciiString("Error: invalid OBJ syntax at line ") + myNbLines + ": vertex index is out of range");
        return;
      }

      anIndex = addNode (myObjVerts.Value (a3Indices[0]));
      myPackedIndices.Bind (a3Indices, anIndex);
      if (a3Indices[1] >= 0)
      {
        if (myObjVertsUV.IsEmpty())
        {
          Message::SendWarning (TCollection_AsciiString("Warning: invalid OBJ syntax at line ") + myNbLines
                              + ": UV index is specified but no UV nodes are defined");
        }
        else if (a3Indices[1] < myObjVertsUV.Lower() || a3Indices[1] > myObjVertsUV.Upper())
        {
          Message::SendWarning (TCollection_AsciiString("Warning: invalid OBJ syntax at line ") + myNbLines
                              + ": UV index is out of range");
          setNodeUV (anIndex,Graphic3d_Vec2 (0.0f, 0.0f));
        }
        else
        {
          setNodeUV (anIndex, myObjVertsUV.Value (a3Indices[1]));
        }
      }
      if (a3Indices[2] >= 0)
      {
        if (myObjNorms.IsEmpty())
        {
          Message::SendWarning (TCollection_AsciiString("Warning: invalid OBJ syntax at line ") + myNbLines
                              + ": Normal index is specified but no Normals nodes are defined");
        }
        else if (a3Indices[2] < myObjNorms.Lower() || a3Indices[2] > myObjNorms.Upper())
        {
          Message::SendWarning (TCollection_AsciiString("Warning: invalid OBJ syntax at line ") + myNbLines
                              + ": Normal index is out of range");
          setNodeNormal (anIndex, Graphic3d_Vec3 (0.0f, 0.0f, 1.0f));
        }
        else
        {
          setNodeNormal (anIndex, myObjNorms.Value (a3Indices[2]));
        }
      }
    }

    if (myCurrElem.size() < size_t(aNode))
    {
      myCurrElem.resize (aNode * 2, -1);
    }
    myCurrElem[aNode] = anIndex;
    aNbElemNodes = aNode + 1;

    if (*thePos == '\n'
     || *thePos == '\0')
    {
      break;
    }

    if (*thePos != ' ')
    {
      ++thePos;
    }
  }

  if (myCurrElem[0] < 0
   || myCurrElem[1] < 0
   || myCurrElem[2] < 0
   || aNbElemNodes  < 3)
  {
    return;
  }

  if (aNbElemNodes == 3)
  {
    myMemEstim += sizeof(Graphic3d_Vec4i);
    addElement (myCurrElem[0], myCurrElem[1], myCurrElem[2], -1);
  }
  else if (aNbElemNodes == 4)
  {
    myMemEstim += sizeof(Graphic3d_Vec4i);
    addElement (myCurrElem[0], myCurrElem[1], myCurrElem[2], myCurrElem[3]);
  }
  else
  {
    const NCollection_Array1<Standard_Integer> aCurrElemArray1 (myCurrElem[0], 1, aNbElemNodes);
    const Standard_Integer aNbAdded = triangulatePolygon (aCurrElemArray1);
    if (aNbAdded < 1)
    {
      return;
    }
    ++myNbElemsBig;
    myMemEstim += sizeof(Graphic3d_Vec4i) * aNbAdded;
  }
}

//================================================================
// Function : triangulatePolygonFan
// Purpose  :
//================================================================
Standard_Integer RWObj_Reader::triangulatePolygonFan (const NCollection_Array1<Standard_Integer>& theIndices)
{
  const Standard_Integer aNbElemNodes = theIndices.Size();
  for (Standard_Integer aNodeIter = 0; aNodeIter < aNbElemNodes - 2; ++aNodeIter)
  {
    Graphic3d_Vec4i aTriNodes (-1, -1, -1, -1);
    for (Standard_Integer aNodeInSubTriIter = 0; aNodeInSubTriIter < 3; ++aNodeInSubTriIter)
    {
      const Standard_Integer aCurrNodeIndex = (aNodeInSubTriIter == 0) ? 0 : (aNodeIter + aNodeInSubTriIter);
      aTriNodes[aNodeInSubTriIter] = theIndices.Value (theIndices.Lower() + aCurrNodeIndex);
    }
    addElement (aTriNodes[0], aTriNodes[1], aTriNodes[2], -1);
  }
  return aNbElemNodes - 2;
}

//================================================================
// Function : polygonCenter
// Purpose  :
//================================================================
gp_XYZ RWObj_Reader::polygonCenter (const NCollection_Array1<Standard_Integer>& theIndices)
{
  if (theIndices.Size() < 3)
  {
    return gp_XYZ (0.0, 0.0, 0.0);
  }
  else if (theIndices.Size() == 4)
  {
    gp_XYZ aCenter = getNode (theIndices.Value (theIndices.Lower() + 0)).XYZ()
                   + getNode (theIndices.Value (theIndices.Lower() + 2)).XYZ();
    aCenter /= 2.0;
    return aCenter;
  }

  gp_XYZ aCenter (0, 0, 0);
  for (NCollection_Array1<Standard_Integer>::Iterator aPntIter (theIndices); aPntIter.More(); aPntIter.Next())
  {
    aCenter += getNode (aPntIter.Value()).XYZ();
  }

  aCenter /= (Standard_Real )theIndices.Size();
  return aCenter;
}

//================================================================
// Function : polygonNormal
// Purpose  :
//================================================================
gp_XYZ RWObj_Reader::polygonNormal (const NCollection_Array1<Standard_Integer>& theIndices)
{
  const gp_XYZ aCenter = polygonCenter (theIndices);
  gp_XYZ aMaxDir = getNode (theIndices.First()).XYZ() - aCenter;
  gp_XYZ aNormal = (getNode (theIndices.Last()).XYZ() - aCenter).Crossed (aMaxDir);
  for (int aPntIter = theIndices.Lower(); aPntIter < theIndices.Upper(); ++aPntIter)
  {
    const gp_XYZ aTmpDir2 = getNode (theIndices.Value (aPntIter + 1)).XYZ() - aCenter;
    if (aTmpDir2.SquareModulus() > aMaxDir.SquareModulus())
    {
      aMaxDir = aTmpDir2;
    }

    const gp_XYZ aTmpDir1 = getNode (theIndices.Value (aPntIter)).XYZ() - aCenter;
    gp_XYZ aDelta = aTmpDir1.Crossed (aTmpDir2);
    if (aNormal.Dot (aDelta) < 0.0)
    {
      aDelta *= -1.0;
    }
    aNormal += aDelta;
  }

  const Standard_Real aMod = aNormal.Modulus();
  if (aMod > gp::Resolution())
  {
    aNormal /= aMod;
  }
  return aNormal;
}

//================================================================
// Function : triangulatePolygon
// Purpose  :
//================================================================
Standard_Integer RWObj_Reader::triangulatePolygon (const NCollection_Array1<Standard_Integer>& theIndices)
{
  const Standard_Integer aNbElemNodes = theIndices.Size();
  if (aNbElemNodes < 3)
  {
    return 0;
  }

  const gp_XYZ aPolygonNorm = polygonNormal (theIndices);

  // map polygon onto plane
  gp_XYZ aXDir;
  {
    const double aAbsXYZ[] = { Abs(aPolygonNorm.X()), Abs(aPolygonNorm.Y()), Abs(aPolygonNorm.Z()) };
    Standard_Integer aMinI = (aAbsXYZ[0] < aAbsXYZ[1]) ? 0 : 1;
    aMinI = (aAbsXYZ[aMinI] < aAbsXYZ[2]) ? aMinI : 2;
    const Standard_Integer aI1 = (aMinI + 1) % 3 + 1;
    const Standard_Integer aI2 = (aMinI + 2) % 3 + 1;
    aXDir.ChangeCoord (aMinI + 1) = 0;
    aXDir.ChangeCoord (aI1) =  aPolygonNorm.Coord (aI2);
    aXDir.ChangeCoord (aI2) = -aPolygonNorm.Coord (aI1);
  }
  const gp_XYZ aYDir = aPolygonNorm ^ aXDir;

  Handle(NCollection_IncAllocator) anAllocator = new NCollection_IncAllocator();
  Handle(BRepMesh_DataStructureOfDelaun) aMeshStructure = new BRepMesh_DataStructureOfDelaun (anAllocator);
  IMeshData::VectorOfInteger anIndexes (aNbElemNodes, anAllocator);
  for (Standard_Integer aNodeIter = 0; aNodeIter < aNbElemNodes; ++aNodeIter)
  {
    const Standard_Integer aNodeIndex = theIndices.Value (theIndices.Lower() + aNodeIter);
    const gp_XYZ aPnt3d = getNode (aNodeIndex).XYZ();
    gp_XY aPnt2d (aXDir * aPnt3d, aYDir * aPnt3d);
    BRepMesh_Vertex aVertex (aPnt2d, aNodeIndex, BRepMesh_Frontier);
    anIndexes.Append (aMeshStructure->AddNode (aVertex));
  }

  const bool isClockwiseOrdered = isClockwisePolygon (aMeshStructure, anIndexes);
  for (Standard_Integer aIdx = anIndexes.Lower(); aIdx <= anIndexes.Upper(); ++aIdx)
  {
    const Standard_Integer aPtIdx     = isClockwiseOrdered ? aIdx : (aIdx + 1) % anIndexes.Length();
    const Standard_Integer aNextPtIdx = isClockwiseOrdered ? (aIdx + 1) % anIndexes.Length() : aIdx;
    BRepMesh_Edge anEdge (anIndexes.Value (aPtIdx),
                          anIndexes.Value (aNextPtIdx),
                          BRepMesh_Frontier);
    aMeshStructure->AddLink (anEdge);
  }

  try
  {
    BRepMesh_Delaun aTriangulation (aMeshStructure, anIndexes);
    const IMeshData::MapOfInteger& aTriangles = aMeshStructure->ElementsOfDomain();
    if (aTriangles.Extent() < 1)
    {
      return triangulatePolygonFan (theIndices);
    }

    Standard_Integer aNbTrisAdded = 0;
    for (IMeshData::MapOfInteger::Iterator aTriIter (aTriangles); aTriIter.More(); aTriIter.Next())
    {
      const Standard_Integer aTriangleId = aTriIter.Key();
      const BRepMesh_Triangle& aTriangle = aMeshStructure->GetElement (aTriangleId);
      if (aTriangle.Movability() == BRepMesh_Deleted)
      {
        continue;
      }

      int aTri2d[3];
      aMeshStructure->ElementNodes (aTriangle, aTri2d);
      if (!isClockwiseOrdered)
      {
        std::swap (aTri2d[1], aTri2d[2]);
      }
      const BRepMesh_Vertex& aVertex1 = aMeshStructure->GetNode (aTri2d[0]);
      const BRepMesh_Vertex& aVertex2 = aMeshStructure->GetNode (aTri2d[1]);
      const BRepMesh_Vertex& aVertex3 = aMeshStructure->GetNode (aTri2d[2]);
      addElement (aVertex1.Location3d(), aVertex2.Location3d(), aVertex3.Location3d(), -1);
      ++aNbTrisAdded;
    }
    return aNbTrisAdded;
  }
  catch (Standard_Failure const& theFailure)
  {
    Message::SendWarning (TCollection_AsciiString ("Error: exception raised during polygon split\n[") + theFailure.GetMessageString() + "]");
  }
  return triangulatePolygonFan (theIndices);
}

// =======================================================================
// function : pushObject
// purpose  :
// =======================================================================
void RWObj_Reader::pushObject (const char* theObjectName)
{
  TCollection_AsciiString aNewObject;
  if (!RWObj_Tools::ReadName (theObjectName, aNewObject))
  {
    // empty group name is OK
  }
  if (addMesh (myActiveSubMesh, RWObj_SubMeshReason_NewObject))
  {
    myPackedIndices.Clear(); // vertices might be duplicated after this point...
  }
  myActiveSubMesh.Object = aNewObject;
}

// =======================================================================
// function : pushGroup
// purpose  :
// =======================================================================
void RWObj_Reader::pushGroup (const char* theGroupName)
{
  TCollection_AsciiString aNewGroup;
  if (!RWObj_Tools::ReadName (theGroupName, aNewGroup))
  {
    // empty group name is OK
  }
  if (addMesh (myActiveSubMesh, RWObj_SubMeshReason_NewGroup))
  {
    myPackedIndices.Clear(); // vertices might be duplicated after this point...
  }
  myActiveSubMesh.Group = aNewGroup;
}

// =======================================================================
// function : pushSmoothGroup
// purpose  :
// =======================================================================
void RWObj_Reader::pushSmoothGroup (const char* theSmoothGroupIndex)
{
  TCollection_AsciiString aNewSmoothGroup;
  RWObj_Tools::ReadName (theSmoothGroupIndex, aNewSmoothGroup);
  if (aNewSmoothGroup == "off"
   || aNewSmoothGroup == "0")
  {
    aNewSmoothGroup.Clear();
  }
  if (myActiveSubMesh.SmoothGroup.IsEqual (aNewSmoothGroup))
  {
    // Ignore duplicated statements to workaround some weird OBJ files.
    // Note that smooth groups are handled in different manner than groups and objects,
    // which always flushed even with equal names.
    return;
  }

  if (addMesh (myActiveSubMesh, RWObj_SubMeshReason_NewSmoothGroup))
  {
    myPackedIndices.Clear(); // vertices might be duplicated after this point...
  }
  myActiveSubMesh.SmoothGroup = aNewSmoothGroup;
}

// =======================================================================
// function : pushMaterial
// purpose  :
// =======================================================================
void RWObj_Reader::pushMaterial (const char* theMaterialName)
{
  TCollection_AsciiString aNewMat;
  if (!RWObj_Tools::ReadName (theMaterialName, aNewMat))
  {
    // empty material name is allowed by specs
  }
  else if (!myMaterials.IsBound (aNewMat))
  {
    Message::SendWarning (TCollection_AsciiString("Warning: use of undefined OBJ material at line ") + myNbLines);
    return;
  }
  if (myActiveSubMesh.Material.IsEqual (aNewMat))
  {
    return; // ignore
  }

  // implicitly create a new group to split materials
  if (addMesh (myActiveSubMesh, RWObj_SubMeshReason_NewMaterial))
  {
    myPackedIndices.Clear(); // vertices might be duplicated after this point...
  }
  myActiveSubMesh.Material = aNewMat;
}

// =======================================================================
// function : readMaterialLib
// purpose  :
// =======================================================================
void RWObj_Reader::readMaterialLib (const char* theFileName)
{
  TCollection_AsciiString aMatPath;
  if (!RWObj_Tools::ReadName (theFileName, aMatPath))
  {
    Message::SendWarning (TCollection_AsciiString("Warning: invalid OBJ syntax at line ") + myNbLines);
    return;
  }

  RWObj_MtlReader aMatReader (myMaterials);
  if (aMatReader.Read (myFolder, aMatPath))
  {
    myExternalFiles.Add (myFolder + aMatPath);
  }
}

// =======================================================================
// function : checkMemory
// purpose  :
// =======================================================================
bool RWObj_Reader::checkMemory()
{
  if (myMemEstim < myMemLimitBytes
   || myToAbort)
  {
    return true;
  }

  Message::SendFail (TCollection_AsciiString("Error: OBJ file content does not fit into ")
                   + Standard_Integer(myMemLimitBytes / (1024 * 1024)) + " MiB limit."
                   + "\nMesh data will be truncated.");
  myToAbort = true;
  return false;
}
