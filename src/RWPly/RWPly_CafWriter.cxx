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

#include <RWPly_CafWriter.hxx>

#include <Message.hxx>
#include <Message_LazyProgressScope.hxx>
#include <OSD_Path.hxx>
#include <RWMesh_FaceIterator.hxx>
#include <RWMesh_MaterialMap.hxx>
#include <RWPly_PlyWriterContext.hxx>
#include <Standard_CLocaleSentry.hxx>
#include <TDocStd_Document.hxx>
#include <TDataStd_Name.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFPrs_DocumentExplorer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(RWPly_CafWriter, Standard_Transient)

//================================================================
// Function : Constructor
// Purpose  :
//================================================================
RWPly_CafWriter::RWPly_CafWriter (const TCollection_AsciiString& theFile)
: myFile (theFile),
  myIsDoublePrec (false),
  myHasNormals (true),
  myHasColors (true),
  myHasTexCoords (false),
  myHasPartId (true),
  myHasFaceId (false)
{
  //
}

//================================================================
// Function : Destructor
// Purpose  :
//================================================================
RWPly_CafWriter::~RWPly_CafWriter()
{
  //
}

//================================================================
// Function : toSkipFaceMesh
// Purpose  :
//================================================================
Standard_Boolean RWPly_CafWriter::toSkipFaceMesh (const RWMesh_FaceIterator& theFaceIter)
{
  return theFaceIter.IsEmptyMesh();
}

// =======================================================================
// function : Perform
// purpose  :
// =======================================================================
bool RWPly_CafWriter::Perform (const Handle(TDocStd_Document)& theDocument,
                               const TColStd_IndexedDataMapOfStringString& theFileInfo,
                               const Message_ProgressRange& theProgress)
{
  TDF_LabelSequence aRoots;
  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool (theDocument->Main());
  aShapeTool->GetFreeShapes (aRoots);
  return Perform (theDocument, aRoots, NULL, theFileInfo, theProgress);
}

// =======================================================================
// function : Perform
// purpose  :
// =======================================================================
bool RWPly_CafWriter::Perform (const Handle(TDocStd_Document)& theDocument,
                               const TDF_LabelSequence& theRootLabels,
                               const TColStd_MapOfAsciiString* theLabelFilter,
                               const TColStd_IndexedDataMapOfStringString& theFileInfo,
                               const Message_ProgressRange& theProgress)
{
  TCollection_AsciiString aFolder, aFileName, aFullFileNameBase, aShortFileNameBase, aFileExt;
  OSD_Path::FolderAndFileFromPath (myFile, aFolder, aFileName);
  OSD_Path::FileNameAndExtension (aFileName, aShortFileNameBase, aFileExt);

  Standard_Real aLengthUnit = 1.;
  if (XCAFDoc_DocumentTool::GetLengthUnit(theDocument, aLengthUnit))
  {
    myCSTrsf.SetInputLengthUnit(aLengthUnit);
  }

  if (theRootLabels.IsEmpty()
  || (theLabelFilter != NULL && theLabelFilter->IsEmpty()))
  {
    Message::SendFail ("Nothing to export into PLY file");
    return false;
  }

  Standard_Integer aNbNodesAll = 0, aNbElemsAll = 0;
  Standard_Real aNbPEntities = 0; // steps for progress range
  for (XCAFPrs_DocumentExplorer aDocExplorer (theDocument, theRootLabels, XCAFPrs_DocumentExplorerFlags_OnlyLeafNodes);
       aDocExplorer.More(); aDocExplorer.Next())
  {
    const XCAFPrs_DocumentNode& aDocNode = aDocExplorer.Current();
    if (theLabelFilter != NULL
    && !theLabelFilter->Contains (aDocNode.Id))
    {
      continue;
    }

    for (RWMesh_FaceIterator aFaceIter (aDocNode.RefLabel, aDocNode.Location, true, aDocNode.Style); aFaceIter.More(); aFaceIter.Next())
    {
      if (toSkipFaceMesh (aFaceIter))
      {
        continue;
      }

      addFaceInfo (aFaceIter, aNbNodesAll, aNbElemsAll);
      aNbPEntities += aNbNodesAll + aNbElemsAll;
    }
  }
  if (aNbNodesAll == 0)
  {
    Message::SendFail ("No mesh data to save");
    return false;
  }

  Standard_CLocaleSentry  aLocaleSentry;
  RWPly_PlyWriterContext  aPlyCtx;
  aPlyCtx.SetDoublePrecision (myIsDoublePrec);
  aPlyCtx.SetNormals (myHasNormals);
  aPlyCtx.SetColors (myHasColors);
  aPlyCtx.SetTexCoords (myHasTexCoords);
  aPlyCtx.SetSurfaceId (myHasPartId || myHasFaceId);
  if (!aPlyCtx.Open (myFile)
   || !aPlyCtx.WriteHeader (aNbNodesAll, aNbElemsAll, theFileInfo))
  {
    return false;
  }

  // simple global progress sentry
  const Standard_Real aPatchStep = 2048.0;
  Message_LazyProgressScope aPSentry (theProgress, "PLY export", aNbPEntities, aPatchStep);

  bool isDone = true;
  for (Standard_Integer aStepIter = 0; aStepIter < 2; ++aStepIter)
  {
    aPlyCtx.SetSurfaceId (0);
    for (XCAFPrs_DocumentExplorer aDocExplorer (theDocument, theRootLabels, XCAFPrs_DocumentExplorerFlags_OnlyLeafNodes);
         aDocExplorer.More() && !aPSentry.IsAborted(); aDocExplorer.Next())
    {
      const XCAFPrs_DocumentNode& aDocNode = aDocExplorer.Current();
      if (theLabelFilter != NULL
      && !theLabelFilter->Contains (aDocNode.Id))
      {
        continue;
      }

      if (myHasPartId)
      {
        aPlyCtx.SetSurfaceId (aPlyCtx.SurfaceId() + 1);
      }
      if (!writeShape (aPlyCtx, aPSentry, aStepIter, aDocNode.RefLabel, aDocNode.Location, aDocNode.Style))
      {
        isDone = false;
        break;
      }
    }
  }

  const bool isClosed = aPlyCtx.Close();
  if (isDone && !isClosed)
  {
    Message::SendFail (TCollection_AsciiString ("Failed to write PLY file\n") + myFile);
    return false;
  }
  return isDone && !aPSentry.IsAborted();
}

// =======================================================================
// function : addFaceInfo
// purpose  :
// =======================================================================
void RWPly_CafWriter::addFaceInfo (const RWMesh_FaceIterator& theFace,
                                   Standard_Integer& theNbNodes,
                                   Standard_Integer& theNbElems)
{
  theNbNodes += theFace.NbNodes();
  theNbElems += theFace.NbTriangles();
}

// =======================================================================
// function : writeShape
// purpose  :
// =======================================================================
bool RWPly_CafWriter::writeShape (RWPly_PlyWriterContext& theWriter,
                                  Message_LazyProgressScope& thePSentry,
                                  const Standard_Integer theWriteStep,
                                  const TDF_Label& theLabel,
                                  const TopLoc_Location& theParentTrsf,
                                  const XCAFPrs_Style& theParentStyle)
{
  for (RWMesh_FaceIterator aFaceIter (theLabel, theParentTrsf, true, theParentStyle); aFaceIter.More() && !thePSentry.IsAborted(); aFaceIter.Next())
  {
    if (toSkipFaceMesh (aFaceIter))
    {
      continue;
    }

    if (theWriteStep == 0
     && !writeNodes (theWriter, thePSentry, aFaceIter))
    {
      return false;
    }
    if (theWriteStep == 1
     && !writeIndices (theWriter, thePSentry, aFaceIter))
    {
      return false;
    }
  }
  return true;
}

// =======================================================================
// function : writeNodes
// purpose  :
// =======================================================================
bool RWPly_CafWriter::writeNodes (RWPly_PlyWriterContext&    theWriter,
                                  Message_LazyProgressScope& thePSentry,
                                  const RWMesh_FaceIterator& theFace)
{
  const Standard_Integer aNodeUpper = theFace.NodeUpper();
  Graphic3d_Vec3 aNormVec;
  Graphic3d_Vec2 aTexVec;
  Graphic3d_Vec4ub aColorVec (255);
  if (theFace.HasFaceColor())
  {
    //Graphic3d_Vec4 aColorF = Quantity_ColorRGBA::Convert_LinearRGB_To_sRGB (theFace.FaceColor());
    Graphic3d_Vec4 aColorF = theFace.FaceColor();
    aColorVec.SetValues ((unsigned char )int(aColorF.r() * 255.0f),
                         (unsigned char )int(aColorF.g() * 255.0f),
                         (unsigned char )int(aColorF.b() * 255.0f),
                         (unsigned char )int(aColorF.a() * 255.0f));
  }
  for (Standard_Integer aNodeIter = theFace.NodeLower(); aNodeIter <= aNodeUpper && thePSentry.More(); ++aNodeIter, thePSentry.Next())
  {
    gp_XYZ aNode = theFace.NodeTransformed (aNodeIter).XYZ();
    myCSTrsf.TransformPosition (aNode);
    if (theFace.HasNormals())
    {
      gp_Dir aNorm = theFace.NormalTransformed (aNodeIter);
      aNormVec.SetValues ((float )aNorm.X(), (float )aNorm.Y(), (float )aNorm.Z());
      myCSTrsf.TransformNormal (aNormVec);
    }
    if (theFace.HasTexCoords())
    {
      const gp_Pnt2d aUV = theFace.NodeTexCoord (aNodeIter);
      aTexVec.SetValues ((float )aUV.X(), (float )aUV.Y());
    }

    if (!theWriter.WriteVertex (aNode, aNormVec, aTexVec, aColorVec))
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
bool RWPly_CafWriter::writeIndices (RWPly_PlyWriterContext&    theWriter,
                                    Message_LazyProgressScope& thePSentry,
                                    const RWMesh_FaceIterator& theFace)
{
  if (myHasFaceId)
  {
    theWriter.SetSurfaceId (theWriter.SurfaceId() + 1);
  }

  const Standard_Integer anElemLower = theFace.ElemLower();
  const Standard_Integer anElemUpper = theFace.ElemUpper();
  for (Standard_Integer anElemIter = anElemLower; anElemIter <= anElemUpper && thePSentry.More(); ++anElemIter, thePSentry.Next())
  {
    const Poly_Triangle aTri = theFace.TriangleOriented (anElemIter);
    if (!theWriter.WriteTriangle (Graphic3d_Vec3i (aTri(1), aTri(2), aTri(3)) - Graphic3d_Vec3i (anElemLower)))
    {
      return false;
    }
  }

  theWriter.SetVertexOffset (theWriter.VertexOffset() + theFace.NbNodes());
  return true;
}
