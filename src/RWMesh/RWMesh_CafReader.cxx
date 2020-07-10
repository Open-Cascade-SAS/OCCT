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
#include <Message_ProgressScope.hxx>
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
#include <XCAFDoc_VisMaterialTool.hxx>

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
                                            const Message_ProgressRange& theProgress,
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
  if (theToProbe || theProgress.UserBreak())
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

  Message::SendInfo (TCollection_AsciiString ("Mesh ") + theFile
                   + "\n[" + aNbNodes + " nodes] [" + aNbElems + " 2d elements]"
                   + "\n[" + (!isDone ? "PARTIALLY " : "") + "read in " + aLoadingTimer.ElapsedTime() + " s]");
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
  CafDocumentTools aTools;
  aTools.ShapeTool = XCAFDoc_DocumentTool::ShapeTool (myXdeDoc->Main());
  aTools.ColorTool = XCAFDoc_DocumentTool::ColorTool (myXdeDoc->Main());
  aTools.VisMaterialTool = XCAFDoc_DocumentTool::VisMaterialTool (myXdeDoc->Main());
  for (TopTools_SequenceOfShape::Iterator aRootIter (myRootShapes); aRootIter.More(); aRootIter.Next())
  {
    addShapeIntoDoc (aTools, aRootIter.Value(), TDF_Label(), aRootName);
  }
  XCAFDoc_DocumentTool::ShapeTool (myXdeDoc->Main())->UpdateAssemblies();
  XCAFDoc_ShapeTool::SetAutoNaming (wasAutoNaming);
}

// =======================================================================
// function : setShapeName
// purpose  :
// =======================================================================
void RWMesh_CafReader::setShapeName (const TDF_Label& theLabel,
                                     const TopAbs_ShapeEnum theShapeType,
                                     const TCollection_AsciiString& theName,
                                     const TDF_Label& theParentLabel,
                                     const TCollection_AsciiString& theParentName)
{
  if (!theName.IsEmpty())
  {
    TDataStd_Name::Set (theLabel, theName);
  }
  else if (!theParentLabel.IsNull())
  {
    TDataStd_Name::Set (theLabel, shapeTypeToString (theShapeType));
  }
  else if (theParentLabel.IsNull()
       && !theParentName.IsEmpty())
  {
    TDataStd_Name::Set (theLabel, theParentName);
  }
}

// =======================================================================
// function : setShapeStyle
// purpose  :
// =======================================================================
void RWMesh_CafReader::setShapeStyle (const CafDocumentTools& theTools,
                                      const TDF_Label& theLabel,
                                      const XCAFPrs_Style& theStyle)
{
  if (theStyle.IsSetColorSurf())
  {
    theTools.ColorTool->SetColor (theLabel, theStyle.GetColorSurfRGBA(), XCAFDoc_ColorSurf);
  }
  if (theStyle.IsSetColorCurv())
  {
    theTools.ColorTool->SetColor (theLabel, theStyle.GetColorCurv(), XCAFDoc_ColorCurv);
  }
  if (!theStyle.Material().IsNull())
  {
    TDF_Label aMaterialLabel = theStyle.Material()->Label();
    if (aMaterialLabel.IsNull())
    {
      const TCollection_AsciiString aMatName = !theStyle.Material()->RawName().IsNull()
                                             ?  theStyle.Material()->RawName()->String()
                                             :  "";
      aMaterialLabel = theTools.VisMaterialTool->AddMaterial (theStyle.Material(), aMatName);
    }
    theTools.VisMaterialTool->SetShapeMaterial (theLabel, aMaterialLabel);
  }
}

// =======================================================================
// function : setShapeNamedData
// purpose  :
// =======================================================================
void RWMesh_CafReader::setShapeNamedData (const CafDocumentTools& ,
                                          const TDF_Label& theLabel,
                                          const Handle(TDataStd_NamedData)& theNameData)
{
  if (theNameData.IsNull())
  {
    return;
  }

  const TDF_Label aNameDataLabel = theNameData->Label();
  Handle(TDataStd_NamedData) anOtherNamedData;
  if (theLabel.FindAttribute (theNameData->ID(), anOtherNamedData))
  {
    if (anOtherNamedData->Label() != aNameDataLabel)
    {
      Message::SendAlarm ("Error! Different NamedData is already set to shape");
    }
  }
  else
  {
    if (aNameDataLabel.IsNull())
    {
      theLabel.AddAttribute (theNameData);
    }
    else
    {
      Message::SendAlarm ("Error! Skipped NamedData instance shared across shapes");
    }
  }
}

// =======================================================================
// function : addShapeIntoDoc
// purpose  :
// =======================================================================
Standard_Boolean RWMesh_CafReader::addShapeIntoDoc (CafDocumentTools& theTools,
                                                    const TopoDS_Shape& theShape,
                                                    const TDF_Label& theLabel,
                                                    const TCollection_AsciiString& theParentName)
{
  if (theShape.IsNull()
   || myXdeDoc.IsNull())
  {
    return Standard_False;
  }

  const TopAbs_ShapeEnum aShapeType = theShape.ShapeType();
  TopoDS_Shape aShapeToAdd = theShape;
  const TopoDS_Shape aShapeNoLoc = theShape.Located (TopLoc_Location());
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

    if (toMakeAssembly)
    {
      // create an empty Compound to add as assembly, so that we can add children one-by-one via AddComponent()
      TopoDS_Compound aCompound;
      BRep_Builder aBuilder;
      aBuilder.MakeCompound (aCompound);
      aCompound.Location (theShape.Location());
      aShapeToAdd = aCompound;
    }
  }

  TDF_Label aNewLabel, anOldLabel;
  if (theLabel.IsNull())
  {
    // add new shape
    aNewLabel = theTools.ShapeTool->AddShape (aShapeToAdd, toMakeAssembly);
  }
  else if (theTools.ShapeTool->IsAssembly (theLabel))
  {
    // add shape as component
    if (theTools.ComponentMap.Find (aShapeNoLoc, anOldLabel))
    {
      aNewLabel = theTools.ShapeTool->AddComponent (theLabel, anOldLabel, theShape.Location());
    }
    else
    {
      aNewLabel = theTools.ShapeTool->AddComponent (theLabel, aShapeToAdd, toMakeAssembly);

      TDF_Label aRefLabel = aNewLabel;
      theTools.ShapeTool->GetReferredShape (aNewLabel, aRefLabel);
      if (!aRefLabel.IsNull())
      {
        theTools.ComponentMap.Bind (aShapeNoLoc, aRefLabel);
      }
    }
  }
  else
  {
    // add shape as sub-shape
    aNewLabel = theTools.ShapeTool->AddSubShape (theLabel, theShape);
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
  theTools.ShapeTool->GetReferredShape (aNewLabel, aNewRefLabel);

  RWMesh_NodeAttributes aRefShapeAttribs;
  myAttribMap.Find (aShapeNoLoc, aRefShapeAttribs);

  bool hasProductName = false;
  if (aNewLabel != aNewRefLabel)
  {
    // put attributes to the Instance (overrides Product attributes)
    RWMesh_NodeAttributes aShapeAttribs;
    if (!theShape.Location().IsIdentity()
      && myAttribMap.Find (theShape, aShapeAttribs))
    {
      if (!aShapeAttribs.Style.IsEqual (aRefShapeAttribs.Style))
      {
        setShapeStyle (theTools, aNewLabel, aShapeAttribs.Style);
      }
      if (aShapeAttribs.NamedData != aRefShapeAttribs.NamedData)
      {
        setShapeNamedData (theTools, aNewLabel, aShapeAttribs.NamedData);
      }
      setShapeName (aNewLabel, aShapeType, aShapeAttribs.Name, theLabel, theParentName);
      if (aRefShapeAttribs.Name.IsEmpty()
      && !aShapeAttribs.Name.IsEmpty())
      {
        // it is not nice having unnamed Product, so copy name from first Instance (probably the only one)
        hasProductName = true;
        setShapeName (aNewRefLabel, aShapeType, aShapeAttribs.Name, theLabel, theParentName);
      }
    }
    else
    {
      // copy name from Product
      setShapeName (aNewLabel, aShapeType, aRefShapeAttribs.Name, theLabel, theParentName);
    }
  }

  if (!anOldLabel.IsNull())
  {
    // already defined in the document
    return Standard_True;
  }

  // put attributes to the Product (shared across Instances)
  if (!hasProductName)
  {
    setShapeName (aNewRefLabel, aShapeType, aRefShapeAttribs.Name, theLabel, theParentName);
  }
  setShapeStyle (theTools, aNewRefLabel, aRefShapeAttribs.Style);
  setShapeNamedData (theTools, aNewRefLabel, aRefShapeAttribs.NamedData);

  if (theTools.ShapeTool->IsAssembly (aNewRefLabel))
  {
    // store sub-shapes (iterator is set to not inherit Location of parent object)
    TCollection_AsciiString aDummyName;
    for (TopoDS_Iterator aSubShapeIter (theShape, Standard_True, Standard_False); aSubShapeIter.More(); aSubShapeIter.Next())
    {
      addShapeIntoDoc (theTools, aSubShapeIter.Value(), aNewRefLabel, aDummyName);
    }
  }
  else
  {
    // store a plain list of sub-shapes in case if they have custom attributes (usually per-face color)
    RWMesh_NodeAttributes aSubShapeAttribs;
    for (TopoDS_Iterator aSubShapeIter (theShape, Standard_True, Standard_False); aSubShapeIter.More(); aSubShapeIter.Next())
    {
      const TopoDS_Shape& aSubShape = aSubShapeIter.Value();
      if (myAttribMap.Find (aSubShape.Located (TopLoc_Location()), aSubShapeAttribs))
      {
        addSubShapeIntoDoc (theTools, aSubShape, aNewRefLabel, aSubShapeAttribs);
      }
    }
  }
  return Standard_True;
}

// =======================================================================
// function : addSubShapeIntoDoc
// purpose  :
// =======================================================================
Standard_Boolean RWMesh_CafReader::addSubShapeIntoDoc (CafDocumentTools& theTools,
                                                       const TopoDS_Shape& theShape,
                                                       const TDF_Label& theParentLabel,
                                                       const RWMesh_NodeAttributes& theAttribs)
{
  if (theShape.IsNull()
   || myXdeDoc.IsNull())
  {
    return Standard_False;
  }

  const TopAbs_ShapeEnum aShapeType = theShape.ShapeType();
  TDF_Label aNewLabel = theTools.ShapeTool->AddSubShape (theParentLabel, theShape);
  if (aNewLabel.IsNull())
  {
    return Standard_False;
  }

  {
    Handle(XCAFDoc_ShapeMapTool) aShapeMapTool = XCAFDoc_ShapeMapTool::Set (aNewLabel);
    aShapeMapTool->SetShape (theShape);
  }

  // if new label is a reference get referred shape
  TDF_Label aNewRefLabel = aNewLabel;
  theTools.ShapeTool->GetReferredShape (aNewLabel, aNewRefLabel);

  // put attributes to the Product (shared across Instances)
  static const TCollection_AsciiString anEmptyString;
  setShapeName (aNewRefLabel, aShapeType, theAttribs.Name, TDF_Label(), anEmptyString);
  setShapeStyle (theTools, aNewRefLabel, theAttribs.Style);
  setShapeNamedData (theTools, aNewRefLabel, theAttribs.NamedData);

  RWMesh_NodeAttributes aSubShapeAttribs;
  for (TopoDS_Iterator aSubShapeIter (theShape, Standard_True, Standard_False); aSubShapeIter.More(); aSubShapeIter.Next())
  {
    const TopoDS_Shape& aSubShape = aSubShapeIter.Value();
    if (myAttribMap.Find (aSubShape.Located (TopLoc_Location()), aSubShapeAttribs))
    {
      addSubShapeIntoDoc (theTools, aSubShape, theParentLabel, aSubShapeAttribs);
    }
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
