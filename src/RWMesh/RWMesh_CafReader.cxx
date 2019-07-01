// Author: Kirill Gavrilov
// Copyright (c) 2016-2019 OPEN CASCADE SAS
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

#include <RWMesh_CafReader.hxx>

#include <XCAFPrs_DocumentExplorer.hxx>

#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_ProgressIndicator.hxx>
#include <BRep_Builder.hxx>
#include <OSD_Path.hxx>
#include <OSD_Timer.hxx>
#include <TDataStd_Name.hxx>
#include <TDocStd_Document.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_ColorType.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeMapTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(RWMesh_CafReader, Standard_Transient)

// =======================================================================
// function : RWMesh_CafReader
// purpose  :
// =======================================================================
RWMesh_CafReader::RWMesh_CafReader()
: myToFillDoc (Standard_True),
  myToFillIncomplete (Standard_True),
  myMemoryLimitMiB (-1),
  myExtraStatus (RWMesh_CafReaderStatusEx_NONE)
{
  //
}

// =======================================================================
// function : ~RWMesh_CafReader
// purpose  :
// =======================================================================
RWMesh_CafReader::~RWMesh_CafReader()
{
  //
}

// =======================================================================
// function : SingleShape
// purpose  :
// =======================================================================
TopoDS_Shape RWMesh_CafReader::SingleShape() const
{
  if (myRootShapes.Size() > 1)
  {
    BRep_Builder aBuilder;
    TopoDS_Compound aCompound;
    aBuilder.MakeCompound (aCompound);
    for (TopTools_SequenceOfShape::Iterator aRootIter (myRootShapes); aRootIter.More(); aRootIter.Next())
    {
      aBuilder.Add (aCompound, aRootIter.Value());
    }
    return aCompound;
  }
  else if (!myRootShapes.IsEmpty())
  {
    return myRootShapes.First();
  }
  return TopoDS_Shape();
}

// =======================================================================
// function : perform
// purpose  :
// =======================================================================
Standard_Boolean RWMesh_CafReader::perform (const TCollection_AsciiString& theFile,
                                            const Handle(Message_ProgressIndicator)& theProgress,
                                            const Standard_Boolean theToProbe)
{
  Standard_Integer aNewRootsLower = 1;
  if (!myXdeDoc.IsNull())
  {
    TDF_LabelSequence aRootLabels;
    XCAFDoc_DocumentTool::ShapeTool (myXdeDoc->Main())->GetFreeShapes (aRootLabels);
    aNewRootsLower = aRootLabels.Upper() + 1;
  }

  OSD_Timer aLoadingTimer;
  aLoadingTimer.Start();
  const Standard_Boolean isDone = performMesh (theFile, theProgress, theToProbe);
  if (theToProbe
   || (!theProgress.IsNull() && theProgress->UserBreak()))
  {
    return isDone;
  }
  else if (!isDone)
  {
    if (!myToFillIncomplete)
    {
      return Standard_False;
    }

    myExtraStatus |= RWMesh_CafReaderStatusEx_Partial;
  }

  TopLoc_Location aDummyLoc;
  Standard_Integer aNbNodes = 0, aNbElems = 0, aNbFaces = 0;
  for (TopTools_SequenceOfShape::Iterator aRootIter (myRootShapes); aRootIter.More(); aRootIter.Next())
  {
    for (TopExp_Explorer aFaceIter (aRootIter.Value(), TopAbs_FACE); aFaceIter.More(); aFaceIter.Next())
    {
      const TopoDS_Face& aFace = TopoDS::Face (aFaceIter.Current());
      if (const Handle(Poly_Triangulation)& aPolyTri = BRep_Tool::Triangulation (aFace, aDummyLoc))
      {
        ++aNbFaces;
        aNbNodes += aPolyTri->NbNodes();
        aNbElems += aPolyTri->NbTriangles();
      }
    }
  }
  if (!isDone && aNbElems < 100)
  {
    return Standard_False;
  }

  fillDocument();
  generateNames (theFile, aNewRootsLower, Standard_False);

  aLoadingTimer.Stop();

  Message::DefaultMessenger()->Send (TCollection_AsciiString ("Mesh ") + theFile
                                   + "\n[" + aNbNodes + " nodes] [" + aNbElems + " 2d elements]"
                                   + "\n[" + (!isDone ? "PARTIALLY " : "") + "read in " + aLoadingTimer.ElapsedTime() + " s]", Message_Info);
  return Standard_True;
}

// =======================================================================
// function : fillDocument
// purpose  :
// =======================================================================
void RWMesh_CafReader::fillDocument()
{
  if (!myToFillDoc
    || myXdeDoc.IsNull()
    || myRootShapes.IsEmpty())
  {
    return;
  }

  const Standard_Boolean wasAutoNaming = XCAFDoc_ShapeTool::AutoNaming();
  XCAFDoc_ShapeTool::SetAutoNaming (Standard_False);
  const TCollection_AsciiString aRootName; // = generateRootName (theFile);
  for (TopTools_SequenceOfShape::Iterator aRootIter (myRootShapes); aRootIter.More(); aRootIter.Next())
  {
    addShapeIntoDoc (aRootIter.Value(), TDF_Label(), aRootName);
  }
  XCAFDoc_DocumentTool::ShapeTool (myXdeDoc->Main())->UpdateAssemblies();
  XCAFDoc_ShapeTool::SetAutoNaming (wasAutoNaming);
}

// =======================================================================
// function : addShapeIntoDoc
// purpose  :
// =======================================================================
Standard_Boolean RWMesh_CafReader::addShapeIntoDoc (const TopoDS_Shape& theShape,
                                                    const TDF_Label& theLabel,
                                                    const TCollection_AsciiString& theParentName)
{
  if (theShape.IsNull()
   || myXdeDoc.IsNull())
  {
    return Standard_False;
  }

  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool (myXdeDoc->Main());

  const TopAbs_ShapeEnum aShapeType = theShape.ShapeType();
  TopoDS_Shape aShapeToAdd = theShape;
  Standard_Boolean toMakeAssembly = Standard_False;
  if (theShape.ShapeType() == TopAbs_COMPOUND)
  {
    RWMesh_NodeAttributes aSubFaceAttribs;
    for (TopoDS_Iterator aSubShapeIter (theShape, Standard_True, Standard_False); !toMakeAssembly && aSubShapeIter.More(); aSubShapeIter.Next())
    {
      if (aSubShapeIter.Value().ShapeType() != TopAbs_FACE)
      {
        toMakeAssembly = Standard_True;
        break;
      }

      const TopoDS_Face& aFace = TopoDS::Face (aSubShapeIter.Value());
      toMakeAssembly = toMakeAssembly
                    || (myAttribMap.Find (aFace, aSubFaceAttribs) && !aSubFaceAttribs.Name.IsEmpty());
    }

    // create empty compound to add as assembly
    if (toMakeAssembly)
    {
      TopoDS_Compound aCompound;
      BRep_Builder aBuilder;
      aBuilder.MakeCompound (aCompound);
      aCompound.Location (theShape.Location());
      aShapeToAdd = aCompound;
    }
  }

  TDF_Label aNewLabel;
  if (theLabel.IsNull())
  {
    // add new shape
    aNewLabel = aShapeTool->AddShape (aShapeToAdd, toMakeAssembly);
  }
  else if (aShapeTool->IsAssembly (theLabel))
  {
    // add shape as component
    aNewLabel = aShapeTool->AddComponent (theLabel, aShapeToAdd, toMakeAssembly);
  }
  else
  {
    // add shape as sub-shape
    aNewLabel = aShapeTool->AddSubShape (theLabel, theShape);
    if (!aNewLabel.IsNull())
    {
      Handle(XCAFDoc_ShapeMapTool) aShapeMapTool = XCAFDoc_ShapeMapTool::Set (aNewLabel);
      aShapeMapTool->SetShape (theShape);
    }
  }
  if (aNewLabel.IsNull())
  {
    return Standard_False;
  }

  // if new label is a reference get referred shape
  TDF_Label aNewRefLabel = aNewLabel;
  aShapeTool->GetReferredShape (aNewLabel, aNewRefLabel);

  // store name
  RWMesh_NodeAttributes aShapeAttribs;
  myAttribMap.Find (theShape, aShapeAttribs);
  if (aShapeAttribs.Name.IsEmpty())
  {
    if (theLabel.IsNull())
    {
      aShapeAttribs.Name = theParentName;
    }
    if (aShapeAttribs.Name.IsEmpty()
    && !theLabel.IsNull())
    {
      aShapeAttribs.Name = shapeTypeToString (aShapeType);
    }
  }
  if (!aShapeAttribs.Name.IsEmpty())
  {
    TDataStd_Name::Set (aNewRefLabel, aShapeAttribs.Name);
  }

  // store color
  Handle(XCAFDoc_ColorTool) aColorTool = XCAFDoc_DocumentTool::ColorTool (myXdeDoc->Main());
  if (aShapeAttribs.Style.IsSetColorSurf())
  {
    aColorTool->SetColor (aNewRefLabel, aShapeAttribs.Style.GetColorSurfRGBA(), XCAFDoc_ColorSurf);
  }
  if (aShapeAttribs.Style.IsSetColorCurv())
  {
    aColorTool->SetColor (aNewRefLabel, aShapeAttribs.Style.GetColorCurv(), XCAFDoc_ColorCurv);
  }

  // store sub-shapes (iterator is set to ignore Location)
  TCollection_AsciiString aDummyName;
  for (TopoDS_Iterator aSubShapeIter (theShape, Standard_True, Standard_False); aSubShapeIter.More(); aSubShapeIter.Next())
  {
    addShapeIntoDoc (aSubShapeIter.Value(), aNewRefLabel, aDummyName);
  }
  return Standard_True;
}

// =======================================================================
// function : generateNames
// purpose  :
// =======================================================================
void RWMesh_CafReader::generateNames (const TCollection_AsciiString& theFile,
                                      const Standard_Integer theRootLower,
                                      const Standard_Boolean theWithSubLabels)
{
  if (myXdeDoc.IsNull())
  {
    return;
  }

  TCollection_AsciiString aDummyFolder, aFileName;
  OSD_Path::FolderAndFileFromPath (theFile, aDummyFolder, aFileName);
  const TCollection_AsciiString aRootName = myRootPrefix + aFileName;

  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool (myXdeDoc->Main());
  TDF_LabelSequence aRootLabels;
  aShapeTool->GetFreeShapes (aRootLabels);
  if (aRootLabels.Upper() < theRootLower)
  {
    return;
  }

  // replace empty names
  Handle(TDataStd_Name) aNodeName;
  Standard_Integer aRootIndex = aRootLabels.Lower();
  TDF_LabelSequence aNewRootLabels;
  for (TDF_LabelSequence::Iterator aRootIter (aRootLabels); aRootIter.More(); ++aRootIndex, aRootIter.Next())
  {
    if (aRootIndex < theRootLower)
    {
      continue;
    }
    else if (theWithSubLabels)
    {
      aNewRootLabels.Append (aRootIter.Value());
    }

    const TDF_Label aLabel = aRootIter.Value();
    TDF_Label aRefLab = aLabel;
    XCAFDoc_ShapeTool::GetReferredShape (aLabel, aRefLab);
    if (!aRefLab.FindAttribute (TDataStd_Name::GetID(), aNodeName))
    {
      TDataStd_Name::Set (aRefLab, aRootName);
    }
    if (aLabel != aRefLab
    && !aLabel.FindAttribute (TDataStd_Name::GetID(), aNodeName))
    {
      TDataStd_Name::Set (aLabel, aRootName);
    }
  }

  if (theWithSubLabels)
  {
    for (XCAFPrs_DocumentExplorer aDocIter (myXdeDoc, aNewRootLabels, XCAFPrs_DocumentExplorerFlags_NoStyle);
         aDocIter.More(); aDocIter.Next())
    {
      if (aDocIter.CurrentDepth() == 0
       || aDocIter.Current().RefLabel.FindAttribute (TDataStd_Name::GetID(), aNodeName))
      {
        continue;
      }

      const TopoDS_Shape aShape = XCAFDoc_ShapeTool::GetShape (aDocIter.Current().RefLabel);
      if (!aShape.IsNull())
      {
        TDataStd_Name::Set (aDocIter.Current().RefLabel, shapeTypeToString (aShape.ShapeType()));
      }
    }
  }
}
