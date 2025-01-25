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

#include <RWObj_CafWriter.hxx>

#include <Message.hxx>
#include <Message_LazyProgressScope.hxx>
#include <OSD_OpenFile.hxx>
#include <OSD_Path.hxx>
#include <RWMesh_FaceIterator.hxx>
#include <RWObj_ObjMaterialMap.hxx>
#include <RWObj_ObjWriterContext.hxx>
#include <Standard_CLocaleSentry.hxx>
#include <TDocStd_Document.hxx>
#include <TDataStd_Name.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFPrs_DocumentExplorer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(RWObj_CafWriter, Standard_Transient)

namespace
{
//! Trivial cast.
inline Graphic3d_Vec3 objXyzToVec(const gp_XYZ& thePnt)
{
  return Graphic3d_Vec3((float)thePnt.X(), (float)thePnt.Y(), (float)thePnt.Z());
}

//! Trivial cast.
inline Graphic3d_Vec2 objXyToVec(const gp_XY& thePnt)
{
  return Graphic3d_Vec2((float)thePnt.X(), (float)thePnt.Y());
}

//! Read name attribute.
static TCollection_AsciiString readNameAttribute(const TDF_Label& theRefLabel)
{
  Handle(TDataStd_Name) aNodeName;
  if (!theRefLabel.FindAttribute(TDataStd_Name::GetID(), aNodeName))
  {
    return TCollection_AsciiString();
  }
  return TCollection_AsciiString(aNodeName->Get());
}
} // namespace

//=================================================================================================

RWObj_CafWriter::RWObj_CafWriter(const TCollection_AsciiString& theFile)
    : myFile(theFile)
{
  // OBJ file format doesn't define length units;
  // Y-up coordinate system is most commonly used (but also undefined)
  // myCSTrsf.SetOutputCoordinateSystem (RWMesh_CoordinateSystem_negZfwd_posYup);
}

//=================================================================================================

RWObj_CafWriter::~RWObj_CafWriter()
{
  //
}

//=================================================================================================

Standard_Boolean RWObj_CafWriter::toSkipFaceMesh(const RWMesh_FaceIterator& theFaceIter)
{
  return theFaceIter.IsEmptyMesh();
}

// =======================================================================
// function : Perform
// purpose  :
// =======================================================================
bool RWObj_CafWriter::Perform(const Handle(TDocStd_Document)&             theDocument,
                              const TColStd_IndexedDataMapOfStringString& theFileInfo,
                              const Message_ProgressRange&                theProgress)
{
  TDF_LabelSequence         aRoots;
  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(theDocument->Main());
  aShapeTool->GetFreeShapes(aRoots);
  return Perform(theDocument, aRoots, NULL, theFileInfo, theProgress);
}

// =======================================================================
// function : Perform
// purpose  :
// =======================================================================
bool RWObj_CafWriter::Perform(const Handle(TDocStd_Document)&             theDocument,
                              const TDF_LabelSequence&                    theRootLabels,
                              const TColStd_MapOfAsciiString*             theLabelFilter,
                              const TColStd_IndexedDataMapOfStringString& theFileInfo,
                              const Message_ProgressRange&                theProgress)
{
  TCollection_AsciiString aFolder, aFileName, aFullFileNameBase, aShortFileNameBase, aFileExt;
  OSD_Path::FolderAndFileFromPath(myFile, aFolder, aFileName);
  OSD_Path::FileNameAndExtension(aFileName, aShortFileNameBase, aFileExt);

  Standard_Real aLengthUnit = 1.;
  if (XCAFDoc_DocumentTool::GetLengthUnit(theDocument, aLengthUnit))
  {
    myCSTrsf.SetInputLengthUnit(aLengthUnit);
  }

  if (theRootLabels.IsEmpty() || (theLabelFilter != NULL && theLabelFilter->IsEmpty()))
  {
    Message::SendFail("Nothing to export into OBJ file");
    return false;
  }

  Standard_Integer aNbNodesAll = 0, aNbElemsAll = 0;
  Standard_Real    aNbPEntities    = 0; // steps for progress range
  bool             toCreateMatFile = false;
  for (XCAFPrs_DocumentExplorer aDocExplorer(theDocument,
                                             theRootLabels,
                                             XCAFPrs_DocumentExplorerFlags_OnlyLeafNodes);
       aDocExplorer.More();
       aDocExplorer.Next())
  {
    const XCAFPrs_DocumentNode& aDocNode = aDocExplorer.Current();
    if (theLabelFilter != NULL && !theLabelFilter->Contains(aDocNode.Id))
    {
      continue;
    }

    for (RWMesh_FaceIterator aFaceIter(aDocNode.RefLabel, aDocNode.Location, true, aDocNode.Style);
         aFaceIter.More();
         aFaceIter.Next())
    {
      if (toSkipFaceMesh(aFaceIter))
      {
        continue;
      }

      addFaceInfo(aFaceIter, aNbNodesAll, aNbElemsAll, aNbPEntities, toCreateMatFile);
    }
  }
  if (aNbNodesAll == 0 || aNbElemsAll == 0)
  {
    Message::SendFail("No mesh data to save");
    return false;
  }

  TCollection_AsciiString       aMatFileNameShort = aShortFileNameBase + ".mtl";
  const TCollection_AsciiString aMatFileNameFull =
    !aFolder.IsEmpty() ? aFolder + aMatFileNameShort : aMatFileNameShort;
  if (!toCreateMatFile)
  {
    aMatFileNameShort.Clear();
  }

  Standard_CLocaleSentry aLocaleSentry;
  RWObj_ObjWriterContext anObjFile(myFile);
  RWObj_ObjMaterialMap   aMatMgr(aMatFileNameFull);
  aMatMgr.SetDefaultStyle(myDefaultStyle);
  if (!anObjFile.IsOpened()
      || !anObjFile.WriteHeader(aNbNodesAll, aNbElemsAll, aMatFileNameShort, theFileInfo))
  {
    return false;
  }

  int aRootDepth = 0;
  if (theRootLabels.Size() == 1)
  {
    TDF_Label aRefLabel = theRootLabels.First();
    XCAFDoc_ShapeTool::GetReferredShape(theRootLabels.First(), aRefLabel);
    TCollection_AsciiString aRootName = readNameAttribute(aRefLabel);
    if (aRootName.EndsWith(".obj"))
    {
      // workaround import/export of .obj file
      aRootDepth = 1;
    }
  }

  // simple global progress sentry - ignores size of node and index data
  const Standard_Real       aPatchStep = 2048.0; // about 100 KiB
  Message_LazyProgressScope aPSentry(theProgress, "OBJ export", aNbPEntities, aPatchStep);

  bool isDone = true;
  for (XCAFPrs_DocumentExplorer aDocExplorer(theDocument,
                                             theRootLabels,
                                             XCAFPrs_DocumentExplorerFlags_OnlyLeafNodes);
       aDocExplorer.More() && !aPSentry.IsAborted();
       aDocExplorer.Next())
  {
    const XCAFPrs_DocumentNode& aDocNode = aDocExplorer.Current();
    if (theLabelFilter != NULL && !theLabelFilter->Contains(aDocNode.Id))
    {
      continue;
    }

    TCollection_AsciiString aName = readNameAttribute(aDocNode.RefLabel);
    for (int aParentIter = aDocExplorer.CurrentDepth() - 1; aParentIter >= aRootDepth;
         --aParentIter)
    {
      const TCollection_AsciiString aParentName =
        readNameAttribute(aDocExplorer.Current(aParentIter).RefLabel);
      if (!aParentName.IsEmpty())
      {
        aName = aParentName + "/" + aName;
      }
    }

    if (!writeShape(anObjFile,
                    aMatMgr,
                    aPSentry,
                    aDocNode.RefLabel,
                    aDocNode.Location,
                    aDocNode.Style,
                    aName))
    {
      isDone = false;
      break;
    }
  }

  const bool isClosed = anObjFile.Close();
  if (isDone && !isClosed)
  {
    Message::SendFail(TCollection_AsciiString("Failed to write OBJ file\n") + myFile);
    return false;
  }
  return isDone && !aPSentry.IsAborted();
}

// =======================================================================
// function : addFaceInfo
// purpose  :
// =======================================================================
void RWObj_CafWriter::addFaceInfo(const RWMesh_FaceIterator& theFace,
                                  Standard_Integer&          theNbNodes,
                                  Standard_Integer&          theNbElems,
                                  Standard_Real&             theNbProgressSteps,
                                  Standard_Boolean&          theToCreateMatFile)
{
  theNbNodes += theFace.NbNodes();
  theNbElems += theFace.NbTriangles();

  theNbProgressSteps += theFace.NbNodes();
  theNbProgressSteps += theFace.NbTriangles();
  if (theFace.HasNormals())
  {
    theNbProgressSteps += theFace.NbNodes();
  }
  if (theFace.HasTexCoords()) //&& !theFace.FaceStyle().Texture().IsEmpty()
  {
    theNbProgressSteps += theFace.NbNodes();
  }

  theToCreateMatFile =
    theToCreateMatFile || theFace.HasFaceColor()
    || (!theFace.FaceStyle().BaseColorTexture().IsNull() && theFace.HasTexCoords());
}

// =======================================================================
// function : writeShape
// purpose  :
// =======================================================================
bool RWObj_CafWriter::writeShape(RWObj_ObjWriterContext&        theWriter,
                                 RWObj_ObjMaterialMap&          theMatMgr,
                                 Message_LazyProgressScope&     thePSentry,
                                 const TDF_Label&               theLabel,
                                 const TopLoc_Location&         theParentTrsf,
                                 const XCAFPrs_Style&           theParentStyle,
                                 const TCollection_AsciiString& theName)
{
  bool toCreateGroup = true;
  for (RWMesh_FaceIterator aFaceIter(theLabel, theParentTrsf, true, theParentStyle);
       aFaceIter.More() && !thePSentry.IsAborted();
       aFaceIter.Next())
  {
    if (toSkipFaceMesh(aFaceIter))
    {
      continue;
    }

    ++theWriter.NbFaces;
    {
      const bool hasNormals = aFaceIter.HasNormals();
      // clang-format off
      const bool hasTexCoords = aFaceIter.HasTexCoords(); //&& !aFaceIter.FaceStyle().Texture().IsEmpty();
      // clang-format on
      if (theWriter.NbFaces != 1)
      {
        toCreateGroup = toCreateGroup || hasNormals != theWriter.HasNormals()
                        || hasTexCoords != theWriter.HasTexCoords();
      }
      theWriter.SetNormals(hasNormals);
      theWriter.SetTexCoords(hasTexCoords);
    }

    if (toCreateGroup && !theWriter.WriteGroup(theName))
    {
      return false;
    }
    toCreateGroup = false;

    TCollection_AsciiString aMatName;
    if (aFaceIter.HasFaceColor() || !aFaceIter.FaceStyle().BaseColorTexture().IsNull())
    {
      aMatName = theMatMgr.AddMaterial(aFaceIter.FaceStyle());
    }
    if (aMatName != theWriter.ActiveMaterial())
    {
      theWriter.WriteActiveMaterial(aMatName);
    }

    // write nodes
    if (!writePositions(theWriter, thePSentry, aFaceIter))
    {
      return false;
    }

    // write normals
    if (theWriter.HasNormals() && !writeNormals(theWriter, thePSentry, aFaceIter))
    {
      return false;
    }

    if (theWriter.HasTexCoords() && !writeTextCoords(theWriter, thePSentry, aFaceIter))
    {
      return false;
    }

    if (!writeIndices(theWriter, thePSentry, aFaceIter))
    {
      return false;
    }
    theWriter.FlushFace(aFaceIter.NbNodes());
  }
  return true;
}

// =======================================================================
// function : writePositions
// purpose  :
// =======================================================================
bool RWObj_CafWriter::writePositions(RWObj_ObjWriterContext&    theWriter,
                                     Message_LazyProgressScope& thePSentry,
                                     const RWMesh_FaceIterator& theFace)
{
  const Standard_Integer aNodeUpper = theFace.NodeUpper();
  for (Standard_Integer aNodeIter = theFace.NodeLower();
       aNodeIter <= aNodeUpper && thePSentry.More();
       ++aNodeIter, thePSentry.Next())
  {
    gp_XYZ aNode = theFace.NodeTransformed(aNodeIter).XYZ();
    myCSTrsf.TransformPosition(aNode);
    if (!theWriter.WriteVertex(objXyzToVec(aNode)))
    {
      return false;
    }
  }
  return true;
}

// =======================================================================
// function : writeNormals
// purpose  :
// =======================================================================
bool RWObj_CafWriter::writeNormals(RWObj_ObjWriterContext&    theWriter,
                                   Message_LazyProgressScope& thePSentry,
                                   const RWMesh_FaceIterator& theFace)
{
  const Standard_Integer aNodeUpper = theFace.NodeUpper();
  for (Standard_Integer aNodeIter = theFace.NodeLower();
       aNodeIter <= aNodeUpper && thePSentry.More();
       ++aNodeIter, thePSentry.Next())
  {
    const gp_Dir   aNormal   = theFace.NormalTransformed(aNodeIter);
    Graphic3d_Vec3 aNormVec3 = objXyzToVec(aNormal.XYZ());
    myCSTrsf.TransformNormal(aNormVec3);
    if (!theWriter.WriteNormal(aNormVec3))
    {
      return false;
    }
  }
  return true;
}

// =======================================================================
// function : writeTextCoords
// purpose  :
// =======================================================================
bool RWObj_CafWriter::writeTextCoords(RWObj_ObjWriterContext&    theWriter,
                                      Message_LazyProgressScope& thePSentry,
                                      const RWMesh_FaceIterator& theFace)
{
  const Standard_Integer aNodeUpper = theFace.NodeUpper();
  for (Standard_Integer aNodeIter = theFace.NodeLower();
       aNodeIter <= aNodeUpper && thePSentry.More();
       ++aNodeIter, thePSentry.Next())
  {
    gp_Pnt2d aTexCoord = theFace.NodeTexCoord(aNodeIter);
    if (!theWriter.WriteTexCoord(objXyToVec(aTexCoord.XY())))
    {
      return false;
    }
  }
  return true;
}

// =======================================================================
// function : writeIndices
// purpose  :
// =======================================================================
bool RWObj_CafWriter::writeIndices(RWObj_ObjWriterContext&    theWriter,
                                   Message_LazyProgressScope& thePSentry,
                                   const RWMesh_FaceIterator& theFace)
{
  const Standard_Integer anElemLower = theFace.ElemLower();
  const Standard_Integer anElemUpper = theFace.ElemUpper();
  for (Standard_Integer anElemIter = anElemLower; anElemIter <= anElemUpper && thePSentry.More();
       ++anElemIter, thePSentry.Next())
  {
    const Poly_Triangle aTri = theFace.TriangleOriented(anElemIter);
    if (!theWriter.WriteTriangle(Graphic3d_Vec3i(aTri(1), aTri(2), aTri(3))
                                 - Graphic3d_Vec3i(anElemLower)))
    {
      return false;
    }
  }
  return true;
}
