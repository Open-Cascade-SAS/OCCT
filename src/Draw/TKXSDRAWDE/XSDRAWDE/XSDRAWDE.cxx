// Copyright (c) 2023 OPEN CASCADE SAS
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

#include <XSDRAWDE.hxx>

#include <BRepMesh_IncrementalMesh.hxx>
#include <DBRep.hxx>
#include <DDocStd.hxx>
#include <DDocStd_DrawDocument.hxx>
#include <DE_ConfigurationContext.hxx>
#include <DE_Provider.hxx>
#include <DE_Wrapper.hxx>
#include <DEBREP_ConfigurationNode.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_PluginMacro.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <Message.hxx>
#include <TDataStd_Name.hxx>
#include <TDocStd_Application.hxx>
#include <TopoDS_Shape.hxx>
#include <XSControl_WorkSession.hxx>
#include <XSDRAW.hxx>

#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDF_Tool.hxx>
#include <XCAFDoc_DimTolTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_Dimension.hxx>
#include <XCAFDoc_Editor.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_ViewTool.hxx>
#include <XCAFDoc_View.hxx>
#include <XCAFView_Object.hxx>

namespace
{

// Exports GT&D and view data from the document as geometry.
class DataAsGeomExporter
{
public:
  // Constructor that initializes the exporter with a document.
  // @param theDoc The document to be used for exporting GT&D and view data as geometry.
  DataAsGeomExporter(const Handle(TDocStd_Document)& theDoc);

  // Performs the export operation.
  // @return True if at least one GT&D or view data was successfully exported,
  //         false if no data were found or exported.
  bool Perform();

  // Returns the document associated with this exporter.
  // @return The document associated with this exporter.
  Handle(TDocStd_Document) GetDocument() const { return myDoc; }

private:
  // Extracts GT&D presentations from the document.
  // @param theDoc The document from which to extract GT&D presentations.
  // @return A map containing the GT&D presentations: labels as keys and shapes as values.
  NCollection_IndexedDataMap<TDF_Label, TopoDS_Shape> ExtractGTDPresentations() const;

  // Converts the indexed data map of presentations to a compound shape.
  // @param thePresentations The indexed data map containing labels and shapes.
  // @return A compound shape containing all the shapes from the presentations.
  static TopoDS_Compound ToCompound(
    const NCollection_IndexedDataMap<TDF_Label, TopoDS_Shape>& thePresentations);

  // Adds GT&D geometry to the document.
  // @param theDoc The document to which the GT&D geometry will be added.
  // @param aCompound The compound shape containing the GT&D geometry.
  // @return The label of the added GT&D geometry, or an empty label if the operation fails.
  TDF_Label AddGTDGeometry(const TopoDS_Compound& aCompound);

  // Maps the labels of the GT&D presentations to the new geometry labels.
  // @param thePresentations The indexed data map containing labels and shapes of the GT&D
  //        presentations.
  // @param theGeometryRoot The root label of the GT&D geometry in the document.
  // @return A data map where keys are original labels of the GT&D and values are new labels of
  //         their respective geometry representations.
  static NCollection_DataMap<TDF_Label, TDF_Label> MapLabels(
    const NCollection_IndexedDataMap<TDF_Label, TopoDS_Shape>& thePresentations,
    const TDF_Label&                                           theGeometryRoot);

  // Sets the source GT&D entries, reference shape labels, and name attributes in the geometry
  // labels.
  // @param theDoc The document to which the GT&D geometry is added.
  // @param aLabelMap A map of original labels to new labels for GT&D geometry.
  void SetSourceGTDAttributes(const NCollection_DataMap<TDF_Label, TDF_Label>& aLabelMap);

  // Retrieve or create a named data attribute for a given label.
  // @param theLabel The label for which to retrieve or create the named data attribute.
  // @return A handle to the named data attribute associated with the label.
  static Handle(TDataStd_NamedData) GetNamedDataAttribute(const TDF_Label& theLabel);

  // Retuns a string containing the entries of the labels in the sequence.
  // The entries are separated by a space.
  // @param theLabels The sequence of labels from which to extract entries.
  // @return A string containing the entries of the labels, separated by spaces.
  //         If theLabels is empty or contains only empty labels, returns an empty string.
  TCollection_ExtendedString GetEntriesString(const TDF_LabelSequence& theLabels);

  // Sets the label name attribute for a given label.
  // If the name attribute does not exist, it creates a new one.
  // @param theLabel The label for which to set the name attribute.
  // @param theName The name to set in the label's name attribute.
  static void SetLabelName(const TDF_Label& theLabel, const TCollection_ExtendedString& theName);

  // For each shape in the document, set the entry as a named data attribute.
  // This function iterates through all shapes in the document and sets a named data attribute
  // containing the entry of each shape.
  // @param theDoc The document containing the shapes.
  void SetShapeEntriesAsData();

  // Creates a shape for views in the document.
  // It consists of top-level label containing children labels, one for each non-null view.
  // @return A TDF_Label representing the compound of views. If there are no views, returns an empty
  //         label.
  TDF_Label CreateShapeLabelForViews() const;

  // Sets views as geometry in the document.
  // This function iterates through all views in the document and creates shape labels for them,
  // and sets source view data as named data attributes for new shape labels.
  bool SetViewsAsGeometry();

  // Generates mesh for all the shape in the shape tool.
  void GenerateMesh();

  // Recursively expands source shapes in the document until faces are reached.
  void ExpandSourceShapes(const TDF_Label& theRoot);

private:
  Handle(TDocStd_Document)   myDoc;        //!< The document associated with this exporter.
  TDF_Label                  myMainLabel;  //!< The main label of the document.
  Handle(XCAFDoc_ShapeTool)  myShapeTool;  //!< Tool for handling shapes in the document.
  Handle(XCAFDoc_DimTolTool) myDimTolTool; //!< Tool for handling GT&D data in the document.
  Handle(XCAFDoc_ViewTool)   myViewTool;   //!< Tool for handling views in the document.

  // clang-format off
  // Constants for GT&D geometry and attributes.
  static constexpr char* GTD_GEOM_NAME       = "gtd_geometry";  //!< Name for GT&D geometry top-level label.
  static constexpr char* GTD_SRC_ENTRY       = "gtd_src_entry"; //!< Named attribute: source entry.
  static constexpr char* GTD_DIM_LINK_1      = "gtd_src_dim_link_first";  //!< Named attribute: first GT&D link.
  static constexpr char* GTD_DIM_LINK_2      = "gtd_src_dim_link_second"; //!< Named attribute: second GT&D link.
  static constexpr char* GTD_SRC_NAME        = "gtd_src_name"; //!< Named attribute: source name.
  static constexpr char* GTD_SHAPE_SRC_ENTRY = "source_entry"; //!< Named attribute: source entry for shapes.

  // Constants for view geometry.
  static constexpr char* VIEW_GEOM_NAME      = "view_geometry"; //!< Name for view geometry top-level label.
  static constexpr char* VIEW_SRC_ENTRY      = "view_src_entry"; //!< Named attribute: source entry for views.
  static constexpr char* VIEW_REF_SHAPES     = "view_ref_shapes"; //!< Named attribute: reference shapes for views.
  static constexpr char* VIEW_REF_GTD        = "view_ref_gtd"; //!< Named attribute: reference GT&D for views.
  static constexpr char* VIEW_CAM_POS        = "view_camera_pos"; //!< Named attribute: camera position for views.
  static constexpr char* VIEW_CAM_DIR        = "view_camera_dir"; //!< Named attribute: camera direction for views.
  static constexpr char* VIEW_CAM_UP         = "view_camera_up"; //!< Named attribute: camera up vector for views.
  // clang-format on
};

//=================================================================================================

DataAsGeomExporter::DataAsGeomExporter(const Handle(TDocStd_Document)& theDoc)
    : myDoc(theDoc)
{
  if (myDoc.IsNull())
  {
    return;
  }

  // Get main document label.
  myMainLabel = myDoc->Main();
  if (myMainLabel.IsNull())
  {
    return;
  }

  // Initialize tools for shape, dimension tolerance, and view.
  myShapeTool  = XCAFDoc_DocumentTool::ShapeTool(myMainLabel);
  myDimTolTool = XCAFDoc_DocumentTool::DimTolTool(myMainLabel);
  myViewTool   = XCAFDoc_DocumentTool::ViewTool(myMainLabel);
}

//=================================================================================================

bool DataAsGeomExporter::Perform()
{
  if (myShapeTool.IsNull())
  {
    return false;
  }

  // Set entries for all shapes in the document.
  SetShapeEntriesAsData();

  bool isAnyDataExported = false;
  if (!myDimTolTool.IsNull())
  {
    // Extract GT&D data from the document.
    const NCollection_IndexedDataMap<TDF_Label, TopoDS_Shape> aPresentations =
      ExtractGTDPresentations();
    if (!aPresentations.IsEmpty())
    {
      isAnyDataExported = true;

      // Create a compound to hold all GT&D shapes.
      const TopoDS_Compound aCompound = ToCompound(aPresentations);
      // Add the compound shape to the document.
      const TDF_Label aNewRoot = AddGTDGeometry(aCompound);
      // Map new labels to original labels based on shape identity.
      const NCollection_DataMap<TDF_Label, TDF_Label> aLabelMap =
        MapLabels(aPresentations, aNewRoot);
      // Set source GT&D entries in the geometry labels.
      SetSourceGTDAttributes(aLabelMap);
    }
  }

  if (!myViewTool.IsNull())
  {
    // Set views as geometry.
    isAnyDataExported |= SetViewsAsGeometry();
  }

  if (isAnyDataExported)
  {
    // Generate mesh for all shapes in the shape tool.
    GenerateMesh();
    // Expand source shapes to ensure they are fully represented in the document.
    ExpandSourceShapes(myShapeTool->Label());
  }

  return isAnyDataExported;
}

//==================================================================================================

NCollection_IndexedDataMap<TDF_Label, TopoDS_Shape> DataAsGeomExporter::ExtractGTDPresentations()
  const
{
  NCollection_IndexedDataMap<TDF_Label, TopoDS_Shape> aPresentations;
  myDimTolTool->GetGDTPresentations(aPresentations);
  return aPresentations;
}

//=================================================================================================

TopoDS_Compound DataAsGeomExporter::ToCompound(
  const NCollection_IndexedDataMap<TDF_Label, TopoDS_Shape>& thePresentations)
{
  if (thePresentations.IsEmpty())
  {
    return TopoDS_Compound();
  }

  TopoDS_Compound aCompound;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aCompound);
  for (NCollection_IndexedDataMap<TDF_Label, TopoDS_Shape>::Iterator aIt(thePresentations);
       aIt.More();
       aIt.Next())
  {
    const TopoDS_Shape& aShape = aIt.Value();
    if (!aShape.IsNull())
    {
      aBuilder.Add(aCompound, aShape);
    }
  }
  return aCompound;
}

//=================================================================================================

TDF_Label DataAsGeomExporter::AddGTDGeometry(const TopoDS_Compound& aCompound)
{
  if (aCompound.IsNull())
  {
    return TDF_Label();
  }

  // Add the compound shape to the document.
  TDF_Label aNewRoot = myShapeTool->AddShape(aCompound, true, false);
  if (aNewRoot.IsNull())
  {
    return TDF_Label();
  }
  SetLabelName(aNewRoot, TCollection_ExtendedString(GTD_GEOM_NAME));

  return aNewRoot;
}

//=================================================================================================

NCollection_DataMap<TDF_Label, TDF_Label> DataAsGeomExporter::MapLabels(
  const NCollection_IndexedDataMap<TDF_Label, TopoDS_Shape>& thePresentations,
  const TDF_Label&                                           theGeometryRoot)
{
  NCollection_DataMap<TDF_Label, TDF_Label> aLabelMap;

  for (TDF_ChildIterator aChildIt(theGeometryRoot); aChildIt.More(); aChildIt.Next())
  {
    const TDF_Label aNewLabel = aChildIt.Value();
    if (aNewLabel.IsNull())
    {
      continue;
    }
    const TopoDS_Shape aNewShape = XCAFDoc_ShapeTool::GetShape(aNewLabel);

    for (NCollection_IndexedDataMap<TDF_Label, TopoDS_Shape>::Iterator aIt(thePresentations);
         aIt.More();
         aIt.Next())
    {
      const TDF_Label&    anOriginalLabel = aIt.Key();
      const TopoDS_Shape& anOriginalShape = aIt.Value();

      if (anOriginalShape.IsPartner(aNewShape))
      {
        // If the shapes match, map the new label to the original label
        aLabelMap.Bind(anOriginalLabel, aNewLabel);
        break;
      }
    }
  }
  return aLabelMap;
}

//=================================================================================================

Handle(TDataStd_NamedData) DataAsGeomExporter::GetNamedDataAttribute(const TDF_Label& theLabel)
{
  Handle(TDataStd_NamedData) aNamedData;
  if (!theLabel.FindAttribute(TDataStd_NamedData::GetID(), aNamedData))
  {
    // If the named data attribute does not exist, create it.
    aNamedData = new TDataStd_NamedData();
    // If the named data attribute did not exist, add it to the label.
    theLabel.AddAttribute(aNamedData);
  }
  return aNamedData;
}

//=================================================================================================

TCollection_ExtendedString DataAsGeomExporter::GetEntriesString(const TDF_LabelSequence& theLabels)
{
  constexpr char*            separator = ", ";
  TCollection_ExtendedString aResult;
  for (const auto& aLabel : theLabels)
  {
    if (aLabel.IsNull())
    {
      continue;
    }

    TCollection_AsciiString anEntry;
    TDF_Tool::Entry(aLabel, anEntry);
    if (!anEntry.IsEmpty())
    {
      if (!aResult.IsEmpty())
      {
        aResult += separator;
      }
      aResult += TCollection_ExtendedString(anEntry);
    }
  }
  return aResult;
}

//=================================================================================================

void DataAsGeomExporter::SetLabelName(const TDF_Label&                  theLabel,
                                      const TCollection_ExtendedString& theName)
{
  Handle(TDataStd_Name) aNameAttr;
  if (!theLabel.FindAttribute(TDataStd_Name::GetID(), aNameAttr))
  {
    aNameAttr = new TDataStd_Name();
    theLabel.AddAttribute(aNameAttr);
  }
  aNameAttr->Set(theName);
}

//=================================================================================================

void DataAsGeomExporter::SetSourceGTDAttributes(
  const NCollection_DataMap<TDF_Label, TDF_Label>& theLabelMap)
{
  for (NCollection_DataMap<TDF_Label, TDF_Label>::Iterator aIt(theLabelMap); aIt.More(); aIt.Next())
  {
    const TDF_Label& aOriginalLabel = aIt.Key();
    const TDF_Label& aGeometryLabel = aIt.Value();

    if (aOriginalLabel.IsNull() || aGeometryLabel.IsNull())
    {
      continue;
    }

    Handle(TDataStd_NamedData) aNamedData = GetNamedDataAttribute(aGeometryLabel);

    // 1. Set the source entry as named data attribute.
    // Get the original label entry.
    TCollection_AsciiString anOriginalLabelEntry;
    TDF_Tool::Entry(aOriginalLabel, anOriginalLabelEntry);
    // Create and set the named data attribute.
    aNamedData->SetString(TCollection_ExtendedString(GTD_SRC_ENTRY), anOriginalLabelEntry);

    // 2. set the source dimension links as named data attributes.
    // Get the reference shapes for the original label.
    TDF_LabelSequence aRefShapeLabelsFirst;
    TDF_LabelSequence aRefShapeLabelsSecond;
    if (myDimTolTool->GetRefShapeLabel(aOriginalLabel, aRefShapeLabelsFirst, aRefShapeLabelsSecond))
    {
      // Set the first reference shapes as a named data attribute.
      if (const TCollection_ExtendedString aRefShapeLabelsFirstStr =
            GetEntriesString(aRefShapeLabelsFirst);
          !aRefShapeLabelsFirstStr.IsEmpty())
      {
        aNamedData->SetString(TCollection_ExtendedString(GTD_DIM_LINK_1), aRefShapeLabelsFirstStr);
      }

      // Set the second reference shapes as a named data attribute.
      if (const TCollection_ExtendedString aRefShapeLabelsSecondStr =
            GetEntriesString(aRefShapeLabelsSecond);
          !aRefShapeLabelsSecondStr.IsEmpty())
      {
        aNamedData->SetString(TCollection_ExtendedString(GTD_DIM_LINK_2), aRefShapeLabelsSecondStr);
      }
    }

    // 3. Set the source name attribute as a named data attribute.
    if (Handle(TDataStd_Name) aSourceNameAttr;
        aOriginalLabel.FindAttribute(TDataStd_Name::GetID(), aSourceNameAttr))
    {
      // If the name attribute exists, set it as a named data attribute.
      TCollection_ExtendedString aNameStr = aSourceNameAttr->Get();
      if (!aNameStr.IsEmpty())
      {
        aNamedData->SetString(TCollection_ExtendedString(GTD_SRC_NAME), aNameStr);
      }
    }
  }
}

//=================================================================================================

void DataAsGeomExporter::SetShapeEntriesAsData()
{
  for (TDF_ChildIterator aChildIt(myShapeTool->Label(), true); aChildIt.More(); aChildIt.Next())
  {
    const TDF_Label& aShapeLabel = aChildIt.Value();
    if (aShapeLabel.IsNull())
    {
      continue;
    }

    TCollection_AsciiString anEntry;
    TDF_Tool::Entry(aShapeLabel, anEntry);

    // Get NamedData attribute from the label.
    Handle(TDataStd_NamedData) aNamedData = GetNamedDataAttribute(aShapeLabel);
    aNamedData->SetString(TCollection_ExtendedString(GTD_SHAPE_SRC_ENTRY), anEntry);
  }
}

//=================================================================================================

TDF_Label DataAsGeomExporter::CreateShapeLabelForViews() const
{
  TDF_ChildIterator aViewIt(myViewTool->Label(), false);
  if (!aViewIt.More())
  {
    return TDF_Label(); // No views to process.
  }

  // Create a compound to hold all view shapes.
  TopoDS_Compound aCompound;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aCompound);
  for (; aViewIt.More(); aViewIt.Next())
  {
    const TDF_Label& aViewLabel = aViewIt.Value();
    if (aViewLabel.IsNull())
    {
      continue; // Skip null labels
    }

    // Get the shape associated with the view label.
    TopoDS_Compound aSubCompound;
    aBuilder.MakeCompound(aSubCompound);
    // Add the shape to the compound.
    aBuilder.Add(aCompound, aSubCompound);
  }

  const TDF_Label aShapeLabel = myShapeTool->AddShape(aCompound, true, false);
  if (!aShapeLabel.IsNull())
  {
    SetLabelName(aShapeLabel, TCollection_ExtendedString(VIEW_GEOM_NAME));
  }

  return aShapeLabel;
}

//=================================================================================================

bool DataAsGeomExporter::SetViewsAsGeometry()
{
  TDF_Label aShapeLabel = CreateShapeLabelForViews();
  if (aShapeLabel.IsNull())
  {
    return false; // No views to process or create shape label failed.
  }

  TDF_ChildIterator aViewIt(myViewTool->Label(), false);
  TDF_ChildIterator aShapeIt(aShapeLabel, false);
  for (; aViewIt.More() && aShapeIt.More(); aViewIt.Next(), aShapeIt.Next())
  {
    const TDF_Label& aViewLabel = aViewIt.Value();
    if (aViewLabel.IsNull())
    {
      continue;
    }

    const TDF_Label& aSubShapeLabel = aShapeIt.Value();

    Handle(TDataStd_NamedData) aSubShapeNamedData = GetNamedDataAttribute(aSubShapeLabel);

    // All label entry.
    TCollection_AsciiString anEntry;
    TDF_Tool::Entry(aViewLabel, anEntry);
    aSubShapeNamedData->SetString(TCollection_ExtendedString(VIEW_SRC_ENTRY), anEntry);

    // Add ref shapes entries as named data attributes.
    TDF_LabelSequence aRefShapes;
    if (myViewTool->GetRefShapeLabel(aViewLabel, aRefShapes))
    {
      // Set the reference shapes as a named data attribute.
      if (const TCollection_ExtendedString aRefShapesStr = GetEntriesString(aRefShapes);
          !aRefShapesStr.IsEmpty())
      {
        aSubShapeNamedData->SetString(TCollection_ExtendedString(VIEW_REF_SHAPES), aRefShapesStr);
      }
    }

    // Add ref GDT labels entries as named data attributes.
    TDF_LabelSequence aGDTLabels;
    if (myViewTool->GetRefGDTLabel(aViewLabel, aGDTLabels))
    {
      // Set the GDT labels as a named data attribute.
      if (const TCollection_ExtendedString aGDTLabelsStr = GetEntriesString(aGDTLabels);
          !aGDTLabelsStr.IsEmpty())
      {
        aSubShapeNamedData->SetString(TCollection_ExtendedString(VIEW_REF_GTD), aGDTLabelsStr);
      }
    }

    // Add camera position as a named data attribute.
    Handle(XCAFDoc_View) aViewAttr;
    if (aViewLabel.FindAttribute(XCAFDoc_View::GetID(), aViewAttr))
    {
      const Handle(XCAFView_Object) aViewObj = aViewAttr->GetObject();

      // Position of the camera in the view.
      const gp_Pnt                  aCameraPos      = aViewObj->ProjectionPoint();
      Handle(TColStd_HArray1OfReal) aCameraPosArray = new TColStd_HArray1OfReal(1, 3);
      aCameraPosArray->SetValue(1, aCameraPos.X());
      aCameraPosArray->SetValue(2, aCameraPos.Y());
      aCameraPosArray->SetValue(3, aCameraPos.Z());
      aSubShapeNamedData->SetArrayOfReals(TCollection_ExtendedString(VIEW_CAM_POS),
                                          aCameraPosArray);

      // Direction of the camera in the view.
      const gp_Dir                  aCameraDir      = aViewObj->ViewDirection();
      Handle(TColStd_HArray1OfReal) aCameraDirArray = new TColStd_HArray1OfReal(1, 3);
      aCameraDirArray->SetValue(1, aCameraDir.X());
      aCameraDirArray->SetValue(2, aCameraDir.Y());
      aCameraDirArray->SetValue(3, aCameraDir.Z());
      aSubShapeNamedData->SetArrayOfReals(TCollection_ExtendedString(VIEW_CAM_DIR),
                                          aCameraDirArray);

      // Up direction of the camera in the view.
      const gp_Dir                  anUpDir      = aViewObj->UpDirection();
      Handle(TColStd_HArray1OfReal) anUpDirArray = new TColStd_HArray1OfReal(1, 3);
      anUpDirArray->SetValue(1, anUpDir.X());
      anUpDirArray->SetValue(2, anUpDir.Y());
      anUpDirArray->SetValue(3, anUpDir.Z());
      aSubShapeNamedData->SetArrayOfReals(TCollection_ExtendedString(VIEW_CAM_UP), anUpDirArray);
    }
  }

  return true;
}

//=================================================================================================

void DataAsGeomExporter::GenerateMesh()
{
  TopoDS_Shape aShape = myShapeTool->GetOneShape();
  if (aShape.IsNull())
  {
    return; // No shape to process.
  }

  BRepMesh_IncrementalMesh aMeshBuilder(aShape, 0.1, Standard_False, 0.5, Standard_True);
  aMeshBuilder.Perform();
}

//=================================================================================================

void DataAsGeomExporter::ExpandSourceShapes(const TDF_Label& theRoot)
{
  // Iterate through all shapes in the document and expand their source shapes.
  for (TDF_ChildIterator aChildIt(theRoot); aChildIt.More(); aChildIt.Next())
  {
    const TDF_Label& aShapeLabel = aChildIt.Value();
    if (aShapeLabel.IsNull())
    {
      continue; // Skip null labels
    }

    TCollection_ExtendedString aName;
    if (Handle(TDataStd_Name) aNameAttr;
        aShapeLabel.FindAttribute(TDataStd_Name::GetID(), aNameAttr))
    {
      aName = aNameAttr->Get();
      if (aName == GTD_GEOM_NAME || aName == VIEW_GEOM_NAME)
      {
        continue; // Skip GT&D and view geometry labels
      }
    }

    if (myShapeTool->IsAssembly(aShapeLabel))
    {
      // If the label is an assembly, recursively expand source shapes for all child labels.
      ExpandSourceShapes(aShapeLabel);
    }
    else if (myShapeTool->IsSimpleShape(aShapeLabel))
    {
      // Shapes should only be expanded until faces, the rest should be skipped.
      const TopoDS_Shape aShape = myShapeTool->GetShape(aShapeLabel);
      if (aShape.IsNull() || aShape.ShapeType() >= TopAbs_FACE)
      {
        continue;
      }
      // Expand the source shapes for the current label.
      XCAFDoc_Editor::Expand(aShapeLabel, aShapeLabel, true);
    }
  }
}

} // namespace

//=================================================================================================

static Standard_Integer DumpConfiguration(Draw_Interpretor& theDI,
                                          Standard_Integer  theNbArgs,
                                          const char**      theArgVec)
{
  Handle(DE_Wrapper)        aConf = DE_Wrapper::GlobalWrapper();
  TCollection_AsciiString   aPath;
  Standard_Boolean          aIsRecursive    = Standard_True;
  Standard_Boolean          isHandleFormat  = Standard_False;
  Standard_Boolean          isHandleVendors = Standard_False;
  TColStd_ListOfAsciiString aFormats;
  TColStd_ListOfAsciiString aVendors;
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if ((anArg == "-path") && (anArgIter + 1 < theNbArgs))
    {
      ++anArgIter;
      aPath = theArgVec[anArgIter];
    }
    else if ((anArg == "-recursive") && (anArgIter + 1 < theNbArgs)
             && Draw::ParseOnOff(theArgVec[anArgIter + 1], aIsRecursive))
    {
      ++anArgIter;
    }
    else if (anArg == "-format")
    {
      isHandleFormat  = Standard_True;
      isHandleVendors = Standard_False;
    }
    else if (anArg == "-vendor")
    {
      isHandleFormat  = Standard_False;
      isHandleVendors = Standard_True;
    }
    else if (isHandleFormat)
    {
      aFormats.Append(theArgVec[anArgIter]);
    }
    else if (isHandleVendors)
    {
      aVendors.Append(theArgVec[anArgIter]);
    }
    else if (!isHandleFormat && !isHandleVendors)
    {
      theDI << "Syntax error at argument '" << theArgVec[anArgIter] << "'\n";
      return 1;
    }
  }
  Standard_Boolean aStat = Standard_True;
  if (!aPath.IsEmpty())
  {
    aStat = aConf->Save(aPath, aIsRecursive, aFormats, aVendors);
  }
  else
  {
    theDI << aConf->Save(aIsRecursive, aFormats, aVendors) << "\n";
  }
  if (!aStat)
  {
    return 1;
  }
  return 0;
}

//=================================================================================================

static Standard_Integer CompareConfiguration(Draw_Interpretor& theDI,
                                             Standard_Integer  theNbArgs,
                                             const char**      theArgVec)
{
  if (theNbArgs > 5)
  {
    theDI.PrintHelp(theArgVec[0]);
    return 1;
  }
  Handle(DE_ConfigurationContext) aResourceFirst = new DE_ConfigurationContext();
  if (!aResourceFirst->Load(theArgVec[1]))
  {
    theDI << "Error: Can't load first configuration\n";
    return 1;
  }
  Handle(DE_ConfigurationContext) aResourceSecond = new DE_ConfigurationContext();
  if (!aResourceSecond->Load(theArgVec[2]))
  {
    theDI << "Error: Can't load second configuration\n";
    return 1;
  }
  const DE_ResourceMap& aResourceMapFirst  = aResourceFirst->GetInternalMap();
  const DE_ResourceMap& aResourceMapSecond = aResourceSecond->GetInternalMap();
  Standard_Integer      anDiffers          = 0;
  for (DE_ResourceMap::Iterator anOrigIt(aResourceMapFirst); anOrigIt.More(); anOrigIt.Next())
  {
    const TCollection_AsciiString& anOrigValue = anOrigIt.Value();
    const TCollection_AsciiString& anOrigKey   = anOrigIt.Key();
    TCollection_AsciiString        aCompValue;
    if (!aResourceMapSecond.Find(anOrigKey, aCompValue))
    {
      Message::SendWarning() << "Second configuration don't have the next scope : " << anOrigKey;
      anDiffers++;
    }
    if (!aCompValue.IsEqual(anOrigValue))
    {
      Message::SendWarning() << "Configurations have differs value with the next scope :"
                             << anOrigKey << " First value : " << anOrigValue
                             << " Second value : " << aCompValue;
      anDiffers++;
    }
  }
  TCollection_AsciiString aMessage;
  if (aResourceMapFirst.Extent() != aResourceMapSecond.Extent() || anDiffers > 0)
  {
    theDI << "Error: Configurations are not same : " << " Differs count : " << anDiffers
          << " Count of first's scopes : " << aResourceMapFirst.Extent()
          << " Count of second's scopes : " << aResourceMapSecond.Extent() << "\n";
    return 1;
  }
  return 0;
}

//=================================================================================================

static Standard_Integer LoadConfiguration(Draw_Interpretor& theDI,
                                          Standard_Integer  theNbArgs,
                                          const char**      theArgVec)
{
  if (theNbArgs > 4)
  {
    theDI.PrintHelp(theArgVec[0]);
    return 1;
  }
  Handle(DE_Wrapper)      aConf        = DE_Wrapper::GlobalWrapper();
  TCollection_AsciiString aString      = theArgVec[1];
  Standard_Boolean        aIsRecursive = Standard_True;
  if (theNbArgs == 4)
  {
    TCollection_AsciiString anArg = theArgVec[2];
    anArg.LowerCase();
    if (!(anArg == "-recursive") || !Draw::ParseOnOff(theArgVec[3], aIsRecursive))
    {
      theDI << "Syntax error at argument '" << theArgVec[3] << "'\n";
      return 1;
    }
  }
  if (!aConf->Load(aString, aIsRecursive))
  {
    theDI << "Error: configuration is incorrect\n";
    return 1;
  }
  return 0;
}

//=================================================================================================

static Standard_Integer ReadFile(Draw_Interpretor& theDI,
                                 Standard_Integer  theNbArgs,
                                 const char**      theArgVec)
{
  if (theNbArgs > 6)
  {
    theDI.PrintHelp(theArgVec[0]);
    return 1;
  }
  TCollection_AsciiString     aDocShapeName;
  TCollection_AsciiString     aFilePath;
  Handle(TDocStd_Document)    aDoc;
  Handle(TDocStd_Application) anApp = DDocStd::GetApplication();
  TCollection_AsciiString     aConfString;
  Standard_Boolean            isNoDoc = (TCollection_AsciiString(theArgVec[0]) == "readfile");
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if ((anArg == "-conf") && (anArgIter + 1 < theNbArgs))
    {
      ++anArgIter;
      aConfString = theArgVec[anArgIter];
    }
    else if (aDocShapeName.IsEmpty())
    {
      aDocShapeName             = theArgVec[anArgIter];
      Standard_CString aNameVar = aDocShapeName.ToCString();
      if (!isNoDoc)
      {
        DDocStd::GetDocument(aNameVar, aDoc, Standard_False);
      }
    }
    else if (aFilePath.IsEmpty())
    {
      aFilePath = theArgVec[anArgIter];
    }
    else
    {
      theDI << "Syntax error at argument '" << theArgVec[anArgIter] << "'\n";
      return 1;
    }
  }
  if (aDocShapeName.IsEmpty() || aFilePath.IsEmpty())
  {
    theDI << "Syntax error: wrong number of arguments\n";
    return 1;
  }
  if (aDoc.IsNull() && !isNoDoc)
  {
    anApp->NewDocument(TCollection_ExtendedString("BinXCAF"), aDoc);
    Handle(DDocStd_DrawDocument) aDrawDoc = new DDocStd_DrawDocument(aDoc);
    TDataStd_Name::Set(aDoc->GetData()->Root(), theArgVec[1]);
    Draw::Set(theArgVec[1], aDrawDoc);
  }

  Handle(DE_Wrapper)            aConf = DE_Wrapper::GlobalWrapper()->Copy();
  Handle(XSControl_WorkSession) aWS   = XSDRAW::Session();
  Standard_Boolean              aStat = Standard_True;
  if (!aConfString.IsEmpty())
  {
    aStat = aConf->Load(aConfString);
  }
  if (aStat)
  {
    TopoDS_Shape aShape;
    aStat = isNoDoc ? aConf->Read(aFilePath, aShape, aWS) : aConf->Read(aFilePath, aDoc, aWS);
    if (isNoDoc && aStat)
    {
      DBRep::Set(aDocShapeName.ToCString(), aShape);
    }
  }
  if (!aStat)
  {
    return 1;
  }
  XSDRAW::CollectActiveWorkSessions(aFilePath);
  return 0;
}

//=================================================================================================

static Standard_Integer WriteFile(Draw_Interpretor& theDI,
                                  Standard_Integer  theNbArgs,
                                  const char**      theArgVec)
{
  if (theNbArgs > 6)
  {
    theDI.PrintHelp(theArgVec[0]);
    return 1;
  }
  TCollection_AsciiString  aDocShapeName;
  TCollection_AsciiString  aFilePath;
  Handle(TDocStd_Document) aDoc;
  TCollection_AsciiString  aConfString;
  Standard_Boolean         isNoDoc = (TCollection_AsciiString(theArgVec[0]) == "writefile");
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if ((anArg == "-conf") && (anArgIter + 1 < theNbArgs))
    {
      ++anArgIter;
      aConfString = theArgVec[anArgIter];
    }
    else if (aDocShapeName.IsEmpty())
    {
      aDocShapeName             = theArgVec[anArgIter];
      Standard_CString aNameVar = aDocShapeName.ToCString();
      if (!isNoDoc)
      {
        DDocStd::GetDocument(aNameVar, aDoc, Standard_False);
      }
    }
    else if (aFilePath.IsEmpty())
    {
      aFilePath = theArgVec[anArgIter];
    }
    else
    {
      theDI << "Syntax error at argument '" << theArgVec[anArgIter] << "'\n";
      return 1;
    }
  }
  if (aDocShapeName.IsEmpty() || aFilePath.IsEmpty())
  {
    theDI << "Syntax error: wrong number of arguments\n";
    return 1;
  }
  if (aDoc.IsNull() && !isNoDoc)
  {
    theDI << "Error: incorrect document\n";
    return 1;
  }
  Handle(DE_Wrapper)            aConf = DE_Wrapper::GlobalWrapper()->Copy();
  Handle(XSControl_WorkSession) aWS   = XSDRAW::Session();
  Standard_Boolean              aStat = Standard_True;
  if (!aConfString.IsEmpty())
  {
    aStat = aConf->Load(aConfString);
  }
  if (aStat)
  {
    if (isNoDoc)
    {
      TopoDS_Shape aShape = DBRep::Get(aDocShapeName);
      if (aShape.IsNull())
      {
        theDI << "Error: incorrect shape " << aDocShapeName << "\n";
        return 1;
      }
      aStat = aConf->Write(aFilePath, aShape, aWS);
    }
    else
    {
      aStat = aConf->Write(aFilePath, aDoc, aWS);
    }
  }
  if (!aStat)
  {
    return 1;
  }
  XSDRAW::CollectActiveWorkSessions(aFilePath);
  return 0;
}

//=================================================================================================

static Standard_Integer ConvertGTD(Draw_Interpretor& theDI,
                                   Standard_Integer  theNbArgs,
                                   const char**      theArgVec)
{
  if (theNbArgs != 3)
  {
    theDI.PrintHelp(theArgVec[0]);
    return 1;
  }

  const TCollection_AsciiString aSourcePath  = theArgVec[1];
  const TCollection_AsciiString anOutputPath = theArgVec[2];

  Handle(TDocStd_Document) aDocument;
  DDocStd::GetApplication()->NewDocument(TCollection_ExtendedString("BinXCAF"), aDocument);
  if (aDocument.IsNull())
  {
    return 1;
  }

  const Handle(DE_Wrapper) aDEWrapper = DE_Wrapper::GlobalWrapper()->Copy();
  if (!aDEWrapper->Read(aSourcePath, aDocument))
  {
    return 1;
  }

  // Create a DataAsGeomExporter instance to handle GT&D data extraction and geometry creation.
  DataAsGeomExporter aExporter(aDocument);
  if (aExporter.Perform())
  {
    aDEWrapper->Write(anOutputPath, aExporter.GetDocument());
  }

  return 0;
}

//=================================================================================================

void XSDRAWDE::Factory(Draw_Interpretor& theDI)
{
  static Standard_Boolean aIsActivated = Standard_False;
  if (aIsActivated)
  {
    return;
  }
  aIsActivated = Standard_True;

  Standard_CString aGroup = "XDE translation commands";
  theDI.Add("DumpConfiguration",
            "DumpConfiguration [-path <path>] [-recursive {on|off}] [-format fmt1 fmt2 ...] "
            "[-vendor vend1 vend2 ...]\n"
            "\n\t\t: Dump special resource generated from global configuration."
            "\n\t\t:   '-path' - save resource configuration to the file"
            "\n\t\t:   '-recursive' - flag to generate a resource from providers. Default is On. "
            "Off disables other options"
            "\n\t\t:   '-format' - flag to generate a resource for chosen formats. If list is "
            "empty, generate it for all"
            "\n\t\t:   '-vendor' - flag to generate a resource for chosen vendors. If list is "
            "empty, generate it for all",
            __FILE__,
            DumpConfiguration,
            aGroup);
  theDI.Add(
    "LoadConfiguration",
    "LoadConfiguration conf [-recursive {on|off}]\n"
    "\n\t\t:   'conf' - path to the resource file or string value in the special format"
    "\n\t\t:   '-recursive' - flag to generate a resource for all providers. Default is true"
    "\n\t\t: Configure global configuration according special resource",
    __FILE__,
    LoadConfiguration,
    aGroup);
  theDI.Add(
    "CompareConfiguration",
    "CompareConfiguration conf1 conf2\n"
    "\n\t\t:   'conf1' - path to the first resource file or string value in the special format"
    "\n\t\t:   'conf2' - path to the second resource file or string value in the special format"
    "\n\t\t: Compare two configurations",
    __FILE__,
    CompareConfiguration,
    aGroup);
  theDI.Add("ReadFile",
            "ReadFile docName filePath [-conf <value|path>]\n"
            "\n\t\t: Read CAD file to document with registered format's providers. Use global "
            "configuration by default.",
            __FILE__,
            ReadFile,
            aGroup);
  theDI.Add("readfile",
            "readfile shapeName filePath [-conf <value|path>]\n"
            "\n\t\t: Read CAD file to shape with registered format's providers. Use global "
            "configuration by default.",
            __FILE__,
            ReadFile,
            aGroup);
  theDI.Add("WriteFile",
            "WriteFile docName filePath [-conf <value|path>]\n"
            "\n\t\t: Write CAD file to document with registered format's providers. Use global "
            "configuration by default.",
            __FILE__,
            WriteFile,
            aGroup);
  theDI.Add("writefile",
            "writefile shapeName filePath [-conf <value|path>]\n"
            "\n\t\t: Write CAD file to shape with registered format's providers. Use global "
            "configuration by default.",
            __FILE__,
            WriteFile,
            aGroup);

  theDI.Add("ConvertGTD",
            "ConvertGTD sourcePath outputPath\n"
            "\n\t\t: Reads GT&D presentations from the source file and saves them as a "
            "compound shape in the output file."
            "\n\t\t:   'sourcePath' - path to the source file containing GT&D presentations"
            "\n\t\t:   'outputPath' - path to the output file where GT&D geometry will be saved",
            __FILE__,
            ConvertGTD,
            aGroup);

  // Load XSDRAW session for pilot activation
  XSDRAW::LoadDraw(theDI);

  // Workaround to force load TKDECascade lib
  DEBREP_ConfigurationNode aTmpObj;
  (void)aTmpObj;
}

// Declare entry point PLUGINFACTORY
DPLUGIN(XSDRAWDE)
