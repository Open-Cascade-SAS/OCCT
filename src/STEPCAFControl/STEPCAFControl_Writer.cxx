// Created on: 2000-08-15
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

// CURRENT LIMITATIONS: 
// when val props and names assigned to instance of 
// component in assembly, it is in fact supposed that only one CDSR corresponds
// to such shape. This can be wrong and should be handled more carefully
// (analysis of SDRs which the CDSR links should be done)
// Names and validation props are supported for top-level shapes only

#include <STEPCAFControl_Writer.hxx>

#include <BRep_Builder.hxx>
#include <GeomToStep_MakeAxis2Placement3d.hxx>
#include <GeomToStep_MakeCartesianPoint.hxx>
#include <HeaderSection_FileSchema.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Static.hxx>
#include <Message.hxx>
#include <Message_ProgressScope.hxx>
#include <MoniTool_DataMapIteratorOfDataMapOfShapeTransient.hxx>
#include <OSD_Path.hxx>
#include <StepAP214_Protocol.hxx>
#include <StepAP242_DraughtingModelItemAssociation.hxx>
#include <StepAP242_GeometricItemSpecificUsage.hxx>
#include <StepBasic_ConversionBasedUnitAndLengthUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndPlaneAngleUnit.hxx>
#include <StepBasic_DerivedUnit.hxx>
#include <StepBasic_DerivedUnitElement.hxx>
#include <StepBasic_HArray1OfDerivedUnitElement.hxx>
#include <StepBasic_MeasureValueMember.hxx>
#include <StepBasic_Product.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepBasic_SiUnitAndLengthUnit.hxx>
#include <StepBasic_SiUnitAndMassUnit.hxx>
#include <StepBasic_SiUnitAndPlaneAngleUnit.hxx>
#include <STEPCAFControl_ActorWrite.hxx>
#include <STEPCAFControl_Controller.hxx>
#include <STEPCAFControl_ExternFile.hxx>
#include <STEPConstruct.hxx>
#include <STEPConstruct_ExternRefs.hxx>
#include <STEPCAFControl_GDTProperty.hxx>
#include <STEPConstruct_Styles.hxx>
#include <STEPConstruct_ValidationProps.hxx>
#include <STEPControl_StepModelType.hxx>
#include <StepData_Factors.hxx>
#include <StepData_Logical.hxx>
#include <StepData_StepModel.hxx>
#include <StepDimTol_AngularityTolerance.hxx>
#include <StepDimTol_CircularRunoutTolerance.hxx>
#include <StepDimTol_CoaxialityTolerance.hxx>
#include <StepDimTol_ConcentricityTolerance.hxx>
#include <StepDimTol_Datum.hxx>
#include <StepDimTol_DatumFeature.hxx>
#include <StepDimTol_DatumReference.hxx>
#include <StepDimTol_DatumSystem.hxx>
#include <StepDimTol_DatumSystemOrReference.hxx>
#include <StepDimTol_GeometricToleranceType.hxx>
#include <StepDimTol_GeometricToleranceWithDatumReference.hxx>
#include <StepDimTol_GeometricToleranceWithModifiers.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthDatRef.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthMaxTol.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthMod.hxx>
#include <StepDimTol_HArray1OfDatumReference.hxx>
#include <StepDimTol_HArray1OfDatumReferenceElement.hxx>
#include <StepDimTol_HArray1OfDatumReferenceModifier.hxx>
#include <StepDimTol_HArray1OfDatumSystemOrReference.hxx>
#include <StepDimTol_ModifiedGeometricTolerance.hxx>
#include <StepDimTol_ParallelismTolerance.hxx>
#include <StepDimTol_PerpendicularityTolerance.hxx>
#include <StepDimTol_PlacedDatumTargetFeature.hxx>
#include <StepDimTol_RunoutZoneDefinition.hxx>
#include <StepDimTol_RunoutZoneOrientation.hxx>
#include <StepDimTol_SymmetryTolerance.hxx>
#include <StepDimTol_ToleranceZone.hxx>
#include <StepDimTol_ToleranceZoneForm.hxx>
#include <StepDimTol_TotalRunoutTolerance.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_Direction.hxx>
#include <StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext.hxx>
#include <StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx.hxx>
#include <StepGeom_Plane.hxx>
#include <StepRepr_CompGroupShAspAndCompShAspAndDatumFeatAndShAsp.hxx>
#include <StepRepr_CompositeShapeAspect.hxx>
#include <StepRepr_ConstructiveGeometryRepresentation.hxx>
#include <StepRepr_ConstructiveGeometryRepresentationRelationship.hxx>
#include <StepRepr_DerivedShapeAspect.hxx>
#include <StepRepr_DescriptiveRepresentationItem.hxx>
#include <StepRepr_FeatureForDatumTargetRelationship.hxx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <StepRepr_MeasureRepresentationItem.hxx>
#include <StepRepr_NextAssemblyUsageOccurrence.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_PropertyDefinition.hxx>
#include <StepRepr_Representation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_ReprItemAndLengthMeasureWithUnit.hxx>
#include <StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI.hxx>
#include <StepRepr_ReprItemAndPlaneAngleMeasureWithUnit.hxx>
#include <StepRepr_ReprItemAndPlaneAngleMeasureWithUnitAndQRI.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepRepr_ShapeAspectDerivingRelationship.hxx>
#include <StepRepr_ShapeAspectRelationship.hxx>
#include <StepRepr_SpecifiedHigherUsageOccurrence.hxx>
#include <StepRepr_ValueRange.hxx>
#include <StepShape_AdvancedFace.hxx>
#include <StepShape_AngularLocation.hxx>
#include <StepShape_AngularSize.hxx>
#include <StepShape_ConnectedFaceSet.hxx>
#include <StepShape_ContextDependentShapeRepresentation.hxx>
#include <StepShape_DimensionalCharacteristicRepresentation.hxx>
#include <StepShape_DimensionalLocation.hxx>
#include <StepShape_DimensionalLocationWithPath.hxx>
#include <StepShape_DimensionalSize.hxx>
#include <StepShape_DimensionalSizeWithPath.hxx>
#include <StepShape_EdgeCurve.hxx>
#include <StepShape_EdgeLoop.hxx>
#include <StepShape_FaceBound.hxx>
#include <StepShape_LimitsAndFits.hxx>
#include <StepShape_OrientedEdge.hxx>
#include <StepShape_PlusMinusTolerance.hxx>
#include <StepShape_QualifiedRepresentationItem.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <StepShape_ShapeDimensionRepresentation.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <StepShape_ShapeRepresentationWithParameters.hxx>
#include <StepShape_ToleranceValue.hxx>
#include <StepShape_TypeQualifier.hxx>
#include <StepShape_ValueFormatTypeQualifier.hxx>
#include <StepVisual_AnnotationPlane.hxx>
#include <StepVisual_CurveStyle.hxx>
#include <StepVisual_DraughtingCallout.hxx>
#include <StepVisual_DraughtingModel.hxx>
#include <StepVisual_HArray1OfPresentationStyleAssignment.hxx>
#include <StepVisual_HArray1OfPresentationStyleSelect.hxx>
#include <StepVisual_Invisibility.hxx>
#include <StepVisual_InvisibleItem.hxx>
#include <StepVisual_MechanicalDesignGeometricPresentationRepresentation.hxx>
#include <StepVisual_NullStyleMember.hxx>
#include <StepVisual_PointStyle.hxx>
#include <StepVisual_PresentationLayerAssignment.hxx>
#include <StepVisual_PresentationRepresentation.hxx>
#include <StepVisual_PresentationStyleAssignment.hxx>
#include <StepVisual_PresentationStyleByContext.hxx>
#include <StepVisual_StyledItem.hxx>
#include <StepVisual_SurfaceStyleUsage.hxx>
#include <StepVisual_TessellatedAnnotationOccurrence.hxx>
#include <StepVisual_TessellatedGeometricSet.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_MapOfAsciiString.hxx>
#include <TColStd_MapOfTransient.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDocStd_Document.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_TransientListBinder.hxx>
#include <TransferBRep.hxx>
#include <TransferBRep_ShapeMapper.hxx>
#include <XCAFDimTolObjects_DatumObject.hxx>
#include <XCAFDimTolObjects_DimensionFormVariance.hxx>
#include <XCAFDimTolObjects_DimensionGrade.hxx>
#include <XCAFDimTolObjects_DimensionObject.hxx>
#include <XCAFDimTolObjects_DimensionModif.hxx>
#include <XCAFDimTolObjects_DimensionModifiersSequence.hxx>
#include <XCAFDimTolObjects_DimensionQualifier.hxx>
#include <XCAFDimTolObjects_GeomToleranceObject.hxx>
#include <XCAFDoc.hxx>
#include <XCAFDoc_Area.hxx>
#include <XCAFDoc_Centroid.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_Datum.hxx>
#include <XCAFDoc_Dimension.hxx>
#include <XCAFDoc_DimTol.hxx>
#include <XCAFDoc_DimTolTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_GeomTolerance.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <XCAFDoc_LengthUnit.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <XCAFDoc_MaterialTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_Volume.hxx>
#include <XCAFDoc_VisMaterialTool.hxx>
#include <XCAFPrs_IndexedDataMapOfShapeStyle.hxx>
#include <XCAFPrs_Style.hxx>
#include <XSAlgo.hxx>
#include <XSAlgo_AlgoContainer.hxx>
#include <XSControl_TransferWriter.hxx>
#include <XSControl_WorkSession.hxx>
#include <UnitsMethods.hxx>

//=======================================================================
//function : GetLabelName
//purpose  : auxiliary function: take name of label and append it to str
//=======================================================================
static Standard_Boolean GetLabelName(const TDF_Label& theL,
                                     Handle(TCollection_HAsciiString)& theStr)
{
  Handle(TDataStd_Name) anAttrName;
  if (!theL.FindAttribute(TDataStd_Name::GetID(), anAttrName))
  {
    return Standard_False;
  }
  TCollection_ExtendedString aName = anAttrName->Get();
  if (aName.IsEmpty())
    return Standard_False;

  // set name, removing spaces around it
  TCollection_AsciiString aBuffer(aName);
  aBuffer.LeftAdjust();
  aBuffer.RightAdjust();
  theStr->AssignCat(aBuffer.ToCString());
  return Standard_True;
}

//=======================================================================
//function : STEPCAFControl_Writer
//purpose  :
//=======================================================================
STEPCAFControl_Writer::STEPCAFControl_Writer() :
  myColorMode(Standard_True),
  myNameMode(Standard_True),
  myLayerMode(Standard_True),
  myPropsMode(Standard_True),
  mySHUOMode(Standard_True),
  myGDTMode(Standard_True),
  myMatMode(Standard_True)
{
  STEPCAFControl_Controller::Init();
  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession;
  Init(aWS);
}

//=======================================================================
//function : STEPCAFControl_Writer
//purpose  :
//=======================================================================
STEPCAFControl_Writer::STEPCAFControl_Writer(const Handle(XSControl_WorkSession)& theWS,
                                             const Standard_Boolean theScratch) :
  myColorMode(Standard_True),
  myNameMode(Standard_True),
  myLayerMode(Standard_True),
  myPropsMode(Standard_True),
  mySHUOMode(Standard_True),
  myGDTMode(Standard_True),
  myMatMode(Standard_True)
{
  STEPCAFControl_Controller::Init();
  Init(theWS, theScratch);
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================
void STEPCAFControl_Writer::Init(const Handle(XSControl_WorkSession)& theWS,
                                 const Standard_Boolean theScratch)
{
  theWS->SelectNorm("STEP");
  myWriter.SetWS(theWS, theScratch);
  myFiles.Clear();
  myLabEF.Clear();
  myLabels.Clear();
  myPureRefLabels.Clear();
  myRootLabels.Clear();
  myGDTPresentationDM = new StepVisual_DraughtingModel();
  myGDTPrsCurveStyle = new StepVisual_HArray1OfPresentationStyleAssignment(1, 1);
}

//=======================================================================
//function : Write
//purpose  :
//=======================================================================
IFSelect_ReturnStatus STEPCAFControl_Writer::Write(const Standard_CString theFileName)
{
  IFSelect_ReturnStatus aStatus = myWriter.Write(theFileName);
  if (aStatus != IFSelect_RetDone)
  {
    return aStatus;
  }

  // get directory name of the main file
  TCollection_AsciiString aDirPath;
  {
    OSD_Path aMainFile(theFileName);
    aMainFile.SetName("");
    aMainFile.SetExtension("");
    aMainFile.SystemName(aDirPath);
  }

  for (NCollection_DataMap<TCollection_AsciiString, Handle(STEPCAFControl_ExternFile)>::Iterator anExtFileIter(myFiles);
       anExtFileIter.More(); anExtFileIter.Next())
  {
    const Handle(STEPCAFControl_ExternFile)& anExtFile = anExtFileIter.Value();
    if (anExtFile->GetWriteStatus() != IFSelect_RetVoid)
    {
      continue;
    }

    // construct extern file name
    TCollection_AsciiString aFileName = OSD_Path::AbsolutePath(aDirPath, anExtFile->GetName()->String());
    if (aFileName.Length() <= 0)
    {
      aFileName = anExtFile->GetName()->String();
    }
    Message::SendTrace() << "Writing external file: " << aFileName << "\n";

    const IFSelect_ReturnStatus anExtStatus = anExtFile->GetWS()->SendAll(aFileName.ToCString());
    anExtFile->SetWriteStatus(anExtStatus);
    if (anExtStatus != IFSelect_RetDone)
    {
      aStatus = anExtStatus;
    }
  }

  return aStatus;
}

//=======================================================================
//function : prepareUnit
//purpose  :
//=======================================================================
void STEPCAFControl_Writer::prepareUnit(const TDF_Label& theLabel,
                                        const Handle(StepData_StepModel)& theModel,
                                        StepData_Factors& theLocalFactors)
{
  Handle(XCAFDoc_LengthUnit) aLengthAttr;
  if (!theLabel.IsNull() &&
      theLabel.Root().FindAttribute(XCAFDoc_LengthUnit::GetID(), aLengthAttr))
  {
    theModel->SetLocalLengthUnit(aLengthAttr->GetUnitValue() * 1000); // convert to mm
    theLocalFactors.SetCascadeUnit(aLengthAttr->GetUnitValue() * 1000);
  }
  else
  {
    XSAlgo::AlgoContainer()->PrepareForTransfer(); // update unit info
    theModel->SetLocalLengthUnit(UnitsMethods::GetCasCadeLengthUnit());
    theLocalFactors.SetCascadeUnit(UnitsMethods::GetCasCadeLengthUnit());
  }
}

//=======================================================================
//function : WriteStream
//purpose  :
//=======================================================================
IFSelect_ReturnStatus STEPCAFControl_Writer::WriteStream(std::ostream& theStream)
{
  if (!myFiles.IsEmpty())
  {
    // writing external files is unsupported via stream interface
    return IFSelect_RetError;
  }

  return myWriter.WriteStream(theStream);
}

//=======================================================================
//function : Transfer
//purpose  :
//=======================================================================
Standard_Boolean STEPCAFControl_Writer::Transfer(const Handle(TDocStd_Document)& theDoc,
                                                 const STEPControl_StepModelType theMode,
                                                 const Standard_CString theMulti,
                                                 const Message_ProgressRange& theProgress)
{
  const Handle(StepData_StepModel) aModel = Handle(StepData_StepModel)::DownCast(myWriter.WS()->Model());
  aModel->InternalParameters.InitFromStatic();
  return Transfer(theDoc, aModel->InternalParameters, theMode, theMulti, theProgress);
}

//=======================================================================
//function : Transfer
//purpose  :
//=======================================================================
Standard_Boolean STEPCAFControl_Writer::Transfer(const Handle(TDocStd_Document)& theDoc,
                                                 const StepData_ConfParameters& theParams,
                                                 const STEPControl_StepModelType theMode,
                                                 const Standard_CString theMulti,
                                                 const Message_ProgressRange& theProgress)
{
  Handle(XCAFDoc_ShapeTool) aShTool = XCAFDoc_DocumentTool::ShapeTool(theDoc->Main());
  if (aShTool.IsNull())
    return Standard_False;

  myRootLabels.Clear();
  myRootLabels.Add(theDoc->Main().Root());
  TDF_LabelSequence aLabels;
  aShTool->GetFreeShapes(aLabels);
  const Handle(StepData_StepModel) aModel = Handle(StepData_StepModel)::DownCast(myWriter.WS()->Model());
  aModel->InternalParameters = theParams;
  return transfer(myWriter, aLabels, theMode, theMulti, Standard_False, theProgress);
}

//=======================================================================
//function : Transfer
//purpose  :
//=======================================================================
Standard_Boolean STEPCAFControl_Writer::Transfer(const TDF_Label& theLabel,
                                                 const STEPControl_StepModelType theMode,
                                                 const Standard_CString theIsMulti,
                                                 const Message_ProgressRange& theProgress)
{
  const Handle(StepData_StepModel) aModel = Handle(StepData_StepModel)::DownCast(myWriter.WS()->Model());
  aModel->InternalParameters.InitFromStatic();
  return Transfer(theLabel, aModel->InternalParameters, theMode, theIsMulti, theProgress);
}

//=======================================================================
//function : Transfer
//purpose  :
//=======================================================================
Standard_Boolean STEPCAFControl_Writer::Transfer(const TDF_Label& theLabel,
                                                 const StepData_ConfParameters& theParams,
                                                 const STEPControl_StepModelType theMode,
                                                 const Standard_CString theIsMulti,
                                                 const Message_ProgressRange& theProgress)
{
  if (theLabel.IsNull())
  {
    return Standard_False;
  }
  TDF_LabelSequence aLabels;
  aLabels.Append(theLabel);
  myRootLabels.Clear();
  myRootLabels.Add(theLabel.Root());
  const Handle(StepData_StepModel) aModel = Handle(StepData_StepModel)::DownCast(myWriter.WS()->Model());
  aModel->InternalParameters = theParams;
  return transfer(myWriter, aLabels, theMode, theIsMulti, Standard_False, theProgress);
}

//=======================================================================
//function : Transfer
//purpose  :
//=======================================================================
Standard_Boolean STEPCAFControl_Writer::Transfer(const TDF_LabelSequence& theLabels,
                                                 const STEPControl_StepModelType theMode,
                                                 const Standard_CString theIsMulti,
                                                 const Message_ProgressRange& theProgress)
{
  const Handle(StepData_StepModel) aModel = Handle(StepData_StepModel)::DownCast(myWriter.WS()->Model());
  aModel->InternalParameters.InitFromStatic();
  return Transfer(theLabels, aModel->InternalParameters, theMode, theIsMulti, theProgress);
}

//=======================================================================
//function : Transfer
//purpose  :
//=======================================================================
Standard_Boolean STEPCAFControl_Writer::Transfer(const TDF_LabelSequence& theLabels,
                                                 const StepData_ConfParameters& theParams,
                                                 const STEPControl_StepModelType theMode,
                                                 const Standard_CString theIsMulti,
                                                 const Message_ProgressRange& theProgress)
{
  myRootLabels.Clear();
  for (TDF_LabelSequence::Iterator aLabelIter(theLabels);
       aLabelIter.More(); aLabelIter.Next())
  {
    const TDF_Label& aLabel = aLabelIter.Value();
    if (!aLabel.IsNull())
    {
      myRootLabels.Add(aLabel.Root());
    }
  }
  const Handle(StepData_StepModel) aModel = Handle(StepData_StepModel)::DownCast(myWriter.WS()->Model());
  aModel->InternalParameters = theParams;
  return transfer(myWriter, theLabels, theMode, theIsMulti, Standard_False, theProgress);
}

//=======================================================================
//function : Perform
//purpose  :
//=======================================================================
Standard_Boolean STEPCAFControl_Writer::Perform(const Handle(TDocStd_Document)& theDoc,
                                                const Standard_CString theFileName,
                                                const Message_ProgressRange& theProgress)
{
  if (!Transfer(theDoc, STEPControl_AsIs, 0L, theProgress))
    return Standard_False;
  return Write(theFileName) == IFSelect_RetDone;
}

//=======================================================================
//function : Perform
//purpose  :
//=======================================================================
Standard_Boolean STEPCAFControl_Writer::Perform(const Handle(TDocStd_Document)& theDoc,
                                                const Standard_CString theFileName,
                                                const StepData_ConfParameters& theParams,
                                                const Message_ProgressRange& theProgress)
{
  if (!Transfer(theDoc, theParams, STEPControl_AsIs, 0L, theProgress))
    return Standard_False;
  return Write(theFileName) == IFSelect_RetDone;
}

//=======================================================================
//function : Perform
//purpose  :
//=======================================================================
Standard_Boolean STEPCAFControl_Writer::Perform(const Handle(TDocStd_Document)& theDoc,
                                                const TCollection_AsciiString& theFileName,
                                                const Message_ProgressRange& theProgress)
{
  if (!Transfer(theDoc, STEPControl_AsIs, 0L, theProgress))
    return Standard_False;
  return Write(theFileName.ToCString()) == IFSelect_RetDone;
}

//=======================================================================
//function : ExternFile
//purpose  :
//=======================================================================
Standard_Boolean STEPCAFControl_Writer::ExternFile(const TDF_Label& theLabel,
                                                   Handle(STEPCAFControl_ExternFile)& theExtFile) const
{
  theExtFile.Nullify();
  if (!myLabEF.IsBound(theLabel))
    return Standard_False;
  theExtFile = myLabEF.Find(theLabel);
  return Standard_True;
}

//=======================================================================
//function : ExternFile
//purpose  :
//=======================================================================
Standard_Boolean STEPCAFControl_Writer::ExternFile(const Standard_CString theName,
                                                   Handle(STEPCAFControl_ExternFile)& theExtFile) const
{
  theExtFile.Nullify();
  if (!myFiles.IsEmpty() || !myFiles.IsBound(theName))
    return Standard_False;
  theExtFile = myFiles.Find(theName);
  return Standard_True;
}

//=======================================================================
//function : Transfer
//purpose  :
//=======================================================================
Standard_Boolean STEPCAFControl_Writer::transfer(STEPControl_Writer& theWriter,
                                                 const TDF_LabelSequence& theLabels,
                                                 const STEPControl_StepModelType theMode,
                                                 const Standard_CString theIsMulti,
                                                 const Standard_Boolean theIsExternFile,
                                                 const Message_ProgressRange& theProgress)
{
  if (theLabels.IsEmpty())
    return Standard_False;

  Handle(STEPCAFControl_ActorWrite) anActor =
    Handle(STEPCAFControl_ActorWrite)::DownCast(theWriter.WS()->NormAdaptor()->ActorWrite());

  StepData_Factors aLocalFactors;
  const Handle(StepData_StepModel) aModel = Handle(StepData_StepModel)::DownCast(theWriter.WS()->Model());
  prepareUnit(theLabels.First(), aModel, aLocalFactors); // set local length unit to the model
  // translate free top-level shapes of the DECAF document
  TDF_LabelSequence aSubLabels;
  Message_ProgressScope aPS(theProgress, "Labels", theLabels.Length());
  // Iterate on requested shapes
  for (TDF_LabelSequence::Iterator aLabelIter(theLabels);
       aLabelIter.More() && aPS.More(); aLabelIter.Next())
  {
    Message_ProgressRange aRange = aPS.Next();
    TDF_Label aCurL = aLabelIter.Value();
    if (myLabels.IsBound(aCurL))
      continue; // already processed

    TopoDS_Shape aCurShape = XCAFDoc_ShapeTool::GetShape(aCurL);
    if (aCurShape.IsNull())
      continue;
    TopoDS_Shape aShapeForBind = aCurShape;
    // write shape either as a whole, or as multifile (with extern refs)
    if (!theIsMulti)
    {
      anActor->SetStdMode(Standard_False);

      TDF_LabelSequence aCompLabels;

      //For case when only part of assembly structure should be written in the document
      //if specified label is component of the assembly then
      //in order to save location of this component in the high-level assembly
      //and save name of high-level assembly it is necessary to represent structure of high-level assembly
      //as assembly with one component specified by current label.
      //For that compound containing only specified component is binded to the label of the high-level assembly.
      //The such way full structure of high-level assembly was replaced on the assembly containing one component.
      //For case when free shape reference is (located root) also create an auxiliary assembly.
      if (XCAFDoc_ShapeTool::IsReference(aCurL))
      {
        TopoDS_Compound aComp;
        BRep_Builder aB;
        aB.MakeCompound(aComp);
        aB.Add(aComp, aCurShape);
        aCurShape = aComp;
        myPureRefLabels.Bind(aCurL, aComp);
        aCompLabels.Append(aCurL);
        TDF_Label aRefL;
        if (XCAFDoc_ShapeTool::GetReferredShape(aCurL, aRefL))
        {
          if (XCAFDoc_ShapeTool::IsAssembly(aRefL))
            XCAFDoc_ShapeTool::GetComponents(aRefL, aCompLabels, Standard_True);
        }
        if (!XCAFDoc_ShapeTool::IsFree(aCurL))
          aCurL = aCurL.Father();
      }
      else
      {
        // fill sequence of (sub) shapes for which attributes should be written
        // and set actor to handle assemblies in a proper way
        if (XCAFDoc_ShapeTool::IsAssembly(aCurL))
          XCAFDoc_ShapeTool::GetComponents(aCurL, aCompLabels, Standard_True);
      }

      for (TDF_LabelSequence::Iterator aCompIter(aCompLabels); aCompIter.More(); aCompIter.Next())
      {
        const TDF_Label aCurSubShL = aCompIter.Value();
        if (myLabels.IsBound(aCurSubShL))
        {
          continue;
        }
        const TopoDS_Shape aCurSh = XCAFDoc_ShapeTool::GetShape(aCurSubShL);
        if (aCurSh.IsNull())
        {
          continue;
        }
        myLabels.Bind(aCurSubShL, aCurSh);
        aSubLabels.Append(aCurSubShL);
        TDF_Label aRefL;
        if (!XCAFDoc_ShapeTool::GetReferredShape(aCurSubShL, aRefL))
        {
          continue;
        }
        if (!myLabels.IsBound(aRefL))
        {
          TopoDS_Shape aRefSh = XCAFDoc_ShapeTool::GetShape(aRefL);
          myLabels.Bind(aRefL, aRefSh);
          aSubLabels.Append(aRefL);
          if (XCAFDoc_ShapeTool::IsAssembly(aRefL))
          {
            anActor->RegisterAssembly(aRefSh);
          }
        }
      }
      myLabels.Bind(aCurL, aShapeForBind);
      aSubLabels.Append(aCurL);

      if (XCAFDoc_ShapeTool::IsAssembly(aCurL) || XCAFDoc_ShapeTool::IsReference(aCurL))
        anActor->RegisterAssembly(aCurShape);

      theWriter.Transfer(aCurShape, theMode, aModel->InternalParameters, Standard_False, aRange);
      anActor->SetStdMode(Standard_True); // restore default behaviour
    }
    else
    {
      // translate final solids
      Message_ProgressScope aPS1(aRange, NULL, 2);
      TopoDS_Shape aSass = transferExternFiles(aCurL, theMode, aSubLabels, aLocalFactors, theIsMulti, aPS1.Next());
      if (aPS1.UserBreak())
        return Standard_False;

      StepData_ConfParameters::WriteMode_Assembly assemblymode = aModel->InternalParameters.WriteAssembly;
      aModel->InternalParameters.WriteAssembly = StepData_ConfParameters::WriteMode_Assembly_On;
      theWriter.Transfer(aSass, STEPControl_AsIs, aModel->InternalParameters, Standard_True, aPS1.Next());
      aModel->InternalParameters.WriteAssembly = assemblymode;
    }
  }
  if (aPS.UserBreak())
    return Standard_False;

  theWriter.WS()->ComputeGraph(Standard_True); // Setting to clear and regenerate graph

  // write names
  if (GetNameMode())
    writeNames(theWriter.WS(), aSubLabels);

  if (!theIsMulti)
  {
    // write colors
    if (GetColorMode())
      writeColors(theWriter.WS(), aSubLabels);

    // write layers
    if (GetLayerMode())
      writeLayers(theWriter.WS(), aSubLabels);

    // write SHUO entities
    if (GetSHUOMode() && !theIsExternFile)
      // do not store SHUO for extern reference for the moment
      writeSHUOs(theWriter.WS(), aSubLabels);

    // write G&DTs
    if (GetDimTolMode())
    {
      if (aModel->InternalParameters.WriteSchema == 5)
      {
        writeDGTsAP242(theWriter.WS(), aSubLabels, aLocalFactors);
      }
      else
      {
        writeDGTs(theWriter.WS(), aSubLabels);
      }
    }

    // write Materials
    if (GetMaterialMode())
      writeMaterials(theWriter.WS(), aSubLabels);

    // register all MDGPRs in model
    for (MoniTool_DataMapIteratorOfDataMapOfShapeTransient anItr(myMapCompMDGPR);
         anItr.More(); anItr.Next())
    {
      aModel->AddWithRefs(anItr.Value());
    }
  }

  if (theIsMulti)
  { // external refs
    writeExternRefs(theWriter.WS(), aSubLabels);
  }

  // write validation props
  if (GetPropsMode())
    writeValProps(theWriter.WS(), aSubLabels, theIsMulti);

  // refresh graph
  theWriter.WS()->ComputeGraph(Standard_True);

  // Write names for the sub-shapes
  if (aModel->InternalParameters.WriteSubshapeNames != 0)
  {
    const Handle(XSControl_TransferWriter)& TW = this->ChangeWriter().WS()->TransferWriter();
    const Handle(Transfer_FinderProcess)& aFP = TW->FinderProcess();

    // Iterate on requested sub shapes
    for (TDF_LabelSequence::Iterator aLabelIter(aSubLabels);
         aLabelIter.More(); aLabelIter.Next())
    {
      const TDF_Label& aCurL = aLabelIter.Value();

      for (TDF_ChildIterator aChildIter(aCurL, Standard_True); aChildIter.More(); aChildIter.Next())
      {
        const TDF_Label& aSubL = aChildIter.Value();

        // Access name recorded in OCAF TDataStd_Name attribute
        Handle(TCollection_HAsciiString) aHSubName = new TCollection_HAsciiString;
        if (!GetLabelName(aSubL, aHSubName))
          continue;

        // Access topological data
        TopoDS_Shape aSubS = XCAFDoc_ShapeTool::GetShape(aSubL);
        if (aSubS.IsNull())
          continue;

        // Access the correspondent STEP Representation Item
        Handle(StepRepr_RepresentationItem) aRI;
        Handle(TransferBRep_ShapeMapper) aShMapper = TransferBRep::ShapeMapper(aFP, aSubS);
        if (!aFP->FindTypedTransient(aShMapper, STANDARD_TYPE(StepRepr_RepresentationItem), aRI))
          continue;

        // Record the name
        aRI->SetName(aHSubName);
      }
    }
  }

  return Standard_True;
}

//=======================================================================
//function : transferExternFiles
//purpose  :
//=======================================================================
TopoDS_Shape STEPCAFControl_Writer::transferExternFiles(const TDF_Label& theLabel,
                                                        const STEPControl_StepModelType theMode,
                                                        TDF_LabelSequence& theLabels,
                                                        const StepData_Factors& theLocalFactors,
                                                        const Standard_CString thePrefix,
                                                        const Message_ProgressRange& theProgress)
{
  // if label already translated, just return the shape
  if (myLabels.IsBound(theLabel))
  {
    return myLabels.Find(theLabel);
  }

  TopoDS_Compound aComp;
  BRep_Builder aBuilder;
  aBuilder.MakeCompound(aComp);
  // if not assembly, write to separate file
  if (!XCAFDoc_ShapeTool::IsAssembly(theLabel) && !XCAFDoc_ShapeTool::IsComponent(theLabel))
  {
    theLabels.Append(theLabel);
    // prepare for transfer
    Handle(XSControl_WorkSession) aNewWS = new XSControl_WorkSession;
    aNewWS->SelectNorm("STEP");
    STEPControl_Writer aStepWriter(aNewWS, Standard_True);
    TDF_LabelSequence aLabelSeq;
    aLabelSeq.Append(theLabel);

    // construct the name for extern file
    Handle(TCollection_HAsciiString) aBaseName = new TCollection_HAsciiString;
    if (thePrefix && thePrefix[0]) aBaseName->AssignCat(thePrefix);
    GetLabelName(theLabel, aBaseName);
    Handle(TCollection_HAsciiString) aNewName = new TCollection_HAsciiString(aBaseName);
    aNewName->AssignCat(".stp");
    if (myFiles.IsBound(aNewName->ToCString()))
    { // avoid confusions
      for (Standard_Integer k = 1; k < 32000; k++)
      {
        aNewName = new TCollection_HAsciiString(aBaseName);
        aNewName->AssignCat("_");
        aNewName->AssignCat(TCollection_AsciiString(k).ToCString());
        aNewName->AssignCat(".stp");
        if (!myFiles.IsBound(aNewName->ToCString())) break;
      }
    }

    // translate and record extern file
    Handle(STEPCAFControl_ExternFile) anExtFile = new STEPCAFControl_ExternFile;
    anExtFile->SetWS(aNewWS);
    anExtFile->SetName(aNewName);
    anExtFile->SetLabel(theLabel);
    StepData_ConfParameters::WriteMode_Assembly anAssemblymode = aStepWriter.Model()->InternalParameters.WriteAssembly;
    aStepWriter.Model()->InternalParameters.WriteAssembly = StepData_ConfParameters::WriteMode_Assembly_Off;
    const Standard_CString anIsMulti = 0;
    anExtFile->SetTransferStatus(transfer(aStepWriter, aLabelSeq, theMode, anIsMulti, Standard_True, theProgress));
    aStepWriter.Model()->InternalParameters.WriteAssembly = anAssemblymode;
    myLabEF.Bind(theLabel, anExtFile);
    myFiles.Bind(aNewName->ToCString(), anExtFile);

    // return empty compound as replacement for the shape
    myLabels.Bind(theLabel, aComp);
    return aComp;
  }
  TDF_LabelSequence aCompLabels;
  TDF_Label aLabel = theLabel;
  //if specified shape is component then high-level assembly is considered
  //to get valid structure with location
  if (XCAFDoc_ShapeTool::IsComponent(theLabel))
  {
    aCompLabels.Append(theLabel);
    aLabel = theLabel.Father();
  }
  // else iterate on components add create structure of empty compounds
  // representing the assembly
  else if (XCAFDoc_ShapeTool::IsAssembly(theLabel))
    XCAFDoc_ShapeTool::GetComponents(theLabel, aCompLabels, Standard_False);

  theLabels.Append(aLabel);
  Message_ProgressScope aPS(theProgress, NULL, aCompLabels.Length());
  // Iterate on requested shapes
  for (TDF_LabelSequence::Iterator aLabelIter(aCompLabels);
       aLabelIter.More() && aPS.More(); aLabelIter.Next())
  {
    const TDF_Label& aCurL = aLabelIter.Value();
    TDF_Label aRefL;
    if (!XCAFDoc_ShapeTool::GetReferredShape(aCurL, aRefL))
      continue;
    TopoDS_Shape aShComp = transferExternFiles(aRefL, theMode, theLabels, theLocalFactors, thePrefix, aPS.Next());
    aShComp.Location(XCAFDoc_ShapeTool::GetLocation(aCurL));
    aBuilder.Add(aComp, aShComp);
  }
  myLabels.Bind(aLabel, aComp);
  return aComp;
}

//=======================================================================
//function : writeExternRefs
//purpose  :
//=======================================================================
Standard_Boolean STEPCAFControl_Writer::writeExternRefs(const Handle(XSControl_WorkSession)& theWS,
                                                        const TDF_LabelSequence& theLabels) const
{
  if (theLabels.IsEmpty())
    return Standard_False;

  const Handle(XSControl_TransferWriter)& aTW = theWS->TransferWriter();
  const Handle(Transfer_FinderProcess)& aFP = aTW->FinderProcess();
  STEPConstruct_ExternRefs anEFTool(theWS);
  Handle(StepData_StepModel) aStepModel = Handle(StepData_StepModel)::DownCast(theWS->Model());
  Standard_Integer aStepSchema = aStepModel->InternalParameters.WriteSchema;
  // Iterate on requested shapes
  for (TDF_LabelSequence::Iterator aLabelIter(theLabels);
       aLabelIter.More(); aLabelIter.Next())
  {
    const TDF_Label& aLab = aLabelIter.Value();
    if (XCAFDoc_ShapeTool::IsAssembly(aLab)) continue; // skip assemblies

    // get extern file
    Handle(STEPCAFControl_ExternFile) anExtFile;
    if (!ExternFile(aLab, anExtFile))
      continue; // should never be

    // find SDR
    if (!myLabels.IsBound(aLab))
      continue; // not recorded as translated, skip
    TopoDS_Shape aShape = myLabels.Find(aLab);

    Handle(StepShape_ShapeDefinitionRepresentation) aSDR;
    Handle(TransferBRep_ShapeMapper) mapper = TransferBRep::ShapeMapper(aFP, aShape);
    if (!aFP->FindTypedTransient(mapper, STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation), aSDR))
    {
      Message::SendTrace() << "Warning: Cannot find SDR for " << aShape.TShape()->DynamicType()->Name() << "\n";
      continue;
    }

    // add extern ref
    const char* aStepFormat = (aStepSchema == 3 ? "STEP AP203" : "STEP AP214");
    // try to get PD from SDR
    StepRepr_RepresentedDefinition aRD = aSDR->Definition();
    Handle(StepRepr_PropertyDefinition) aPropDef = aRD.PropertyDefinition();
    if (aPropDef.IsNull())
    {
      Message::SendTrace() << "Warning: STEPCAFControl_Writer::writeExternRefs StepRepr_PropertyDefinition is null for " << aShape.TShape()->DynamicType()->Name() << "\n";
      continue;
    }
    StepRepr_CharacterizedDefinition aCharDef = aPropDef->Definition();
    Handle(StepBasic_ProductDefinition) aPD = aCharDef.ProductDefinition();
    if (aPD.IsNull())
    {
      Message::SendTrace() << "Warning: STEPCAFControl_Writer::writeExternRefs StepBasic_ProductDefinition is null for " << aShape.TShape()->DynamicType()->Name() << "\n";
      continue;
    }
    anEFTool.AddExternRef(anExtFile->GetName()->ToCString(), aPD, aStepFormat);
  }
  anEFTool.WriteExternRefs(aStepSchema);
  return Standard_True;
}

//=======================================================================
//function : FindEntities
//purpose  : auxiliary
//=======================================================================
static Standard_Integer FindEntities(const Handle(Transfer_FinderProcess)& theFP,
                                     const TopoDS_Shape& theShape,
                                     TopLoc_Location& theLocation,
                                     TColStd_SequenceOfTransient& theSeqRI)
{
  Handle(StepRepr_RepresentationItem) anItem = STEPConstruct::FindEntity(theFP, theShape, theLocation);

  if (!anItem.IsNull())
  {
    theSeqRI.Append(anItem);
    return 1;
  }

  // may be S was splited during shape processing
  Handle(TransferBRep_ShapeMapper) aMapper = TransferBRep::ShapeMapper(theFP, theShape);
  Handle(Transfer_Binder) aBinder = theFP->Find(aMapper);
  if (aBinder.IsNull())
    return 0;

  Handle(Transfer_TransientListBinder) aTransientListBinder =
    //Handle(Transfer_TransientListBinder)::DownCast( bnd->Next(Standard_True) );
    Handle(Transfer_TransientListBinder)::DownCast(aBinder);
  Standard_Integer aResCount = 0;
  if (aTransientListBinder.IsNull() && theShape.ShapeType() == TopAbs_COMPOUND)
  {
    for (TopoDS_Iterator anIter(theShape); anIter.More(); anIter.Next())
    {
      Handle(StepRepr_RepresentationItem) aLocalItem =
        STEPConstruct::FindEntity(theFP, anIter.Value(), theLocation);
      if (aLocalItem.IsNull())
        continue;
      aResCount++;
      theSeqRI.Append(aLocalItem);
    }
  }
  else if (!aTransientListBinder.IsNull())
  {
    const Standard_Integer aNbTransient = aTransientListBinder->NbTransients();
    for (Standard_Integer anInd = 1; anInd <= aNbTransient; anInd++)
    {
      Handle(Standard_Transient) anEntity = aTransientListBinder->Transient(anInd);
      anItem = Handle(StepRepr_RepresentationItem)::DownCast(anEntity);
      if (anItem.IsNull())
        continue;
      aResCount++;
      theSeqRI.Append(anItem);
    }
  }
  return aResCount;
}

//=======================================================================
//function : getStyledItem
//purpose  : auxiliary
//=======================================================================
static Standard_Boolean getStyledItem(const TopoDS_Shape& theShape,
                                      const Handle(XCAFDoc_ShapeTool)& theShapeTool,
                                      const STEPConstruct_Styles& theStyles,
                                      Handle(StepVisual_StyledItem)& theResSelItem,
                                      const MoniTool_DataMapOfShapeTransient& theMapCompMDGPR)
{
  const TDF_Label aTopShL = theShapeTool->FindShape(theShape, Standard_False);
  TopoDS_Shape aTopLevSh = theShapeTool->GetShape(aTopShL);
  Standard_Boolean anIsFound = Standard_False;
  if (aTopLevSh.IsNull() || !theMapCompMDGPR.IsBound(aTopLevSh))
  {
    return Standard_False;
  }
  Handle(StepVisual_PresentationRepresentation) aMDGPR =
    Handle(StepVisual_PresentationRepresentation)::DownCast(theMapCompMDGPR.Find(aTopLevSh));
  Handle(StepRepr_HArray1OfRepresentationItem) anSelItmHArr = aMDGPR->Items();
  if (anSelItmHArr.IsNull())
  {
    return Standard_False;
  }
  // Search for PSA of Manifold solid
  TColStd_SequenceOfTransient aNewSeqRI;
  Standard_Boolean isFilled = Standard_False;
  for (StepRepr_HArray1OfRepresentationItem::Iterator anIter(anSelItmHArr->Array1());
       anIter.More() && !anIsFound; anIter.Next())
  {
    const Handle(StepVisual_StyledItem)& aSelItm =
      Handle(StepVisual_StyledItem)::DownCast(anIter.Value());

    if (aSelItm.IsNull())
    {
      continue;
    }
    // Check that it is a styled item for manifold solid brep
    if (!isFilled)
    {
      TopLoc_Location aLoc;
      FindEntities(theStyles.FinderProcess(), aTopLevSh, aLoc, aNewSeqRI);
      isFilled = Standard_True;
    }
    if (!aNewSeqRI.IsEmpty())
    {
      const Handle(StepRepr_RepresentationItem)& anItem = aSelItm->Item();
      Standard_Boolean isSameMonSolBR = Standard_False;
      for (TColStd_SequenceOfTransient::Iterator aIterRI(aNewSeqRI);
           aIterRI.More(); aIterRI.Next())
      {
        if (!anItem.IsNull() && anItem == aIterRI.Value())
        {
          isSameMonSolBR = Standard_True;
          break;
        }
      }
      if (!isSameMonSolBR)
        continue;
    }
    for (StepVisual_HArray1OfPresentationStyleAssignment::Iterator aStyleIter(aSelItm->Styles()->Array1());
         aStyleIter.More() && !anIsFound; aStyleIter.Next())
    {
      const Handle(StepVisual_PresentationStyleAssignment)& aFatherPSA = aStyleIter.Value();
      // check for PSA for top-level (not Presentation style by context for NAUO)
      if (aFatherPSA.IsNull() || aFatherPSA->IsKind(STANDARD_TYPE(StepVisual_PresentationStyleByContext)))
        continue;
      theResSelItem = aSelItm;
      anIsFound = Standard_True;
    }
  }
  return anIsFound;
}

//=======================================================================
//function : setDefaultInstanceColor
//purpose  : auxiliary
//=======================================================================
static Standard_Boolean setDefaultInstanceColor(const Handle(StepVisual_StyledItem)& theStyleItem,
                                                Handle(StepVisual_PresentationStyleAssignment)& thePSA)
{
  Standard_Boolean anIsFound = Standard_False;
  for (StepVisual_HArray1OfPresentationStyleAssignment::Iterator aStyleIter(theStyleItem->Styles()->Array1());
       aStyleIter.More() && !anIsFound; aStyleIter.Next())
  {
    const Handle(StepVisual_PresentationStyleAssignment)& aFatherPSA = aStyleIter.Value();
    // check for PSA for top-level (not Presentation style by context for NAUO)
    if (aFatherPSA.IsNull() || aFatherPSA->IsKind(STANDARD_TYPE(StepVisual_PresentationStyleByContext)))
      return Standard_False;

    // get style select from father PSA
    if (aFatherPSA->NbStyles() > 0)
    {
      Handle(StepVisual_HArray1OfPresentationStyleSelect) aFatherStyles =
        new StepVisual_HArray1OfPresentationStyleSelect(1, aFatherPSA->NbStyles());
      Standard_Integer aSettingInd = 1;
      for (StepVisual_HArray1OfPresentationStyleSelect::Iterator aFatherStyleIter(aFatherPSA->Styles()->Array1());
           aFatherStyleIter.More(); aFatherStyleIter.Next())
      {
        StepVisual_PresentationStyleSelect aPSS;
        const StepVisual_PresentationStyleSelect& anOlDPSS = aFatherStyleIter.Value();
        if (!anOlDPSS.PointStyle().IsNull())
          aPSS.SetValue(anOlDPSS.PointStyle());
        else if (!anOlDPSS.CurveStyle().IsNull())
          aPSS.SetValue(anOlDPSS.CurveStyle());
        else if (!anOlDPSS.SurfaceStyleUsage().IsNull())
          aPSS.SetValue(anOlDPSS.SurfaceStyleUsage());
        else
        {
          anIsFound = Standard_False;
          break;
        }
        aFatherStyles->SetValue(aSettingInd++, anOlDPSS);
        anIsFound = Standard_True;
      }
      // init PSA of NAUO
      if (anIsFound)
      {
        thePSA->Init(aFatherStyles);
      }
    }
  }
  return anIsFound;
}

//=======================================================================
//function : MakeSTEPStyles
//purpose  : auxiliary
//=======================================================================
static void MakeSTEPStyles(STEPConstruct_Styles& theStyles,
                           const TopoDS_Shape& theShape,
                           const XCAFPrs_IndexedDataMapOfShapeStyle& theSettings,
                           Handle(StepVisual_StyledItem)& theOverride,
                           TopTools_MapOfShape& theMap,
                           const MoniTool_DataMapOfShapeTransient& theMapCompMDGPR,
                           STEPConstruct_DataMapOfAsciiStringTransient& theDPDCs,
                           STEPConstruct_DataMapOfPointTransient& theColRGBs,
                           const Handle(XCAFDoc_ShapeTool)& theShTool,
                           const XCAFPrs_Style* theInherit = 0,
                           const Standard_Boolean theIsComponent = Standard_False)
{
  // skip already processed shapes
  if (!theMap.Add(theShape))
    return;

  // check if shape has its own style (r inherits from ancestor)
  XCAFPrs_Style aStyle;
  if (theInherit) aStyle = *theInherit;
  if (theSettings.Contains(theShape))
  {
    const XCAFPrs_Style& anOwnStyle = theSettings.FindFromKey(theShape);
    if (!anOwnStyle.IsVisible())
      aStyle.SetVisibility(Standard_False);
    if (anOwnStyle.IsSetColorCurv())
      aStyle.SetColorCurv(anOwnStyle.GetColorCurv());
    if (anOwnStyle.IsSetColorSurf())
      aStyle.SetColorSurf(anOwnStyle.GetColorSurfRGBA());
  }

  // translate colors to STEP
  Handle(StepVisual_Colour) aSurfColor, aCurvColor;
  Standard_Real aRenderTransp = 0.0;
  if (aStyle.IsSetColorSurf())
  {
    Quantity_ColorRGBA aSurfCol = aStyle.GetColorSurfRGBA();
    aRenderTransp = 1.0 - aSurfCol.Alpha();
    aSurfColor = theStyles.EncodeColor(aSurfCol.GetRGB(), theDPDCs, theColRGBs);
  }
  if (aStyle.IsSetColorCurv())
    aCurvColor = theStyles.EncodeColor(aStyle.GetColorCurv(), theDPDCs, theColRGBs);

  Standard_Boolean aHasOwn = (!aSurfColor.IsNull() ||
                              !aCurvColor.IsNull() ||
                              !aStyle.IsVisible());

  // find target item and assign style to it
  Handle(StepVisual_StyledItem) aSTEPstyle = theOverride;
  if (aHasOwn)
  {
    if (theShape.ShapeType() != TopAbs_COMPOUND || theIsComponent)
    { // skip compounds, let subshapes inherit its colors
      TopLoc_Location aLocation;
      TColStd_SequenceOfTransient aSeqRI;
      Standard_Integer aNbEntities = FindEntities(theStyles.FinderProcess(), theShape, aLocation, aSeqRI);
      if (aNbEntities <= 0)
        Message::SendTrace() << "Warning: Cannot find RI for " << theShape.TShape()->DynamicType()->Name() << "\n";
      //Get overridden style gka 10.06.03
      if (theIsComponent && aNbEntities > 0)
        getStyledItem(theShape, theShTool, theStyles, theOverride, theMapCompMDGPR);

      for (TColStd_SequenceOfTransient::Iterator anEntIter(aSeqRI);
           anEntIter.More(); anEntIter.Next())
      {
        const Handle(StepRepr_RepresentationItem)& anItem =
          Handle(StepRepr_RepresentationItem)::DownCast(anEntIter.Value());
        Handle(StepVisual_PresentationStyleAssignment) aPSA;
        if (aStyle.IsVisible() || !aSurfColor.IsNull() || !aCurvColor.IsNull())
        {
          aPSA = theStyles.MakeColorPSA(anItem, aSurfColor, aCurvColor, aSurfColor, aRenderTransp, theIsComponent);
        }
        else
        {
          // default white color
          aSurfColor = theStyles.EncodeColor(Quantity_Color(Quantity_NOC_WHITE), theDPDCs, theColRGBs);
          aPSA = theStyles.MakeColorPSA(anItem, aSurfColor, aCurvColor, aSurfColor, 0.0, theIsComponent);
          if (theIsComponent)
            setDefaultInstanceColor(theOverride, aPSA);

        } // end of component case

        aSTEPstyle = theStyles.AddStyle(anItem, aPSA, theOverride);
        aHasOwn = Standard_False;
      }
    }
  }

  // iterate on subshapes (except vertices :)
  if (theShape.ShapeType() == TopAbs_EDGE)
    return;
  if (theIsComponent)
  {
    return;
  }
  for (TopoDS_Iterator anIter(theShape); anIter.More(); anIter.Next())
  {
    MakeSTEPStyles(theStyles, anIter.Value(), theSettings, aSTEPstyle,
                   theMap, theMapCompMDGPR, theDPDCs, theColRGBs, theShTool,
                   (aHasOwn ? &aStyle : 0));
  }
}

//=======================================================================
//function : writeColors
//purpose  :
//=======================================================================
Standard_Boolean STEPCAFControl_Writer::writeColors(const Handle(XSControl_WorkSession)& theWS,
                                                    const TDF_LabelSequence& theLabels)
{
  if (theLabels.IsEmpty())
    return Standard_False;

  STEPConstruct_Styles Styles(theWS);
  STEPConstruct_DataMapOfAsciiStringTransient DPDCs;
  STEPConstruct_DataMapOfPointTransient ColRGBs;
  // Iterate on requested shapes
  for (TDF_LabelSequence::Iterator aLabelIter(theLabels);
       aLabelIter.More(); aLabelIter.Next())
  {
    const TDF_Label aLabel = aLabelIter.Value();
    // Iterate on shapes in the document
    Handle(XCAFDoc_ShapeTool) aSTool = XCAFDoc_DocumentTool::ShapeTool(aLabel);
    // Skip assemblies: colors assigned to assemblies and their instances
    // are not supported (it is not clear how to encode that in STEP)
    if (XCAFDoc_ShapeTool::IsAssembly(aLabel))
    {
      Message::SendTrace() << "Warning: Cannot write color  for Assembly" << "\n";
      Message::SendTrace() << "Info: Check for colors assigned to components in assembly" << "\n";
      // PTV 22.01.2003 Write color for instances.
      TDF_LabelSequence compLabels;
      if (!aSTool->GetComponents(aLabel, compLabels))
        continue;
      writeColors(theWS, compLabels);
      continue;
    }
    Styles.ClearStyles();

    // get a target shape and try to find corresponding context
    // (all the colors set under that label will be put into that context)
    TopoDS_Shape aShape;
    if (!XCAFDoc_ShapeTool::GetShape(aLabel, aShape))
      continue;
    Standard_Boolean anIsComponent = aSTool->IsComponent(aLabel) || myPureRefLabels.IsBound(aLabel);
    TopoDS_Shape aTopSh = aShape;
    Handle(StepRepr_RepresentationContext) aContext = Styles.FindContext(aShape);
    if (anIsComponent)
    {
      TDF_Label aTopShL = aSTool->FindShape(aShape, Standard_False);
      if (aTopShL.IsNull())
        continue;
      aTopSh = aSTool->GetShape(aTopShL);
      aContext = Styles.FindContext(aTopSh);
    }
    if (aContext.IsNull())
      continue;

    // collect settings set on that label
    XCAFPrs_IndexedDataMapOfShapeStyle aSettings;
    TDF_LabelSequence aSeq;
    aSeq.Append(aLabel);
    XCAFDoc_ShapeTool::GetSubShapes(aLabel, aSeq);
    Standard_Boolean anIsVisible = Standard_True;
    for (TDF_LabelSequence::Iterator aSeqIter(aSeq);
         aSeqIter.More(); aSeqIter.Next())
    {
      const TDF_Label& aSeqValue = aSeqIter.Value();
      XCAFPrs_Style aStyle;
      Quantity_ColorRGBA aColor;
      if (aSeqValue == aLabel)
      {
        // check for invisible status of object on label
        if (!XCAFDoc_ColorTool::IsVisible(aSeqValue))
        {
          anIsVisible = Standard_False;
          aStyle.SetVisibility(Standard_False);
        }
      }
      if (XCAFDoc_ColorTool::GetColor(aSeqValue, XCAFDoc_ColorGen, aColor))
      {
        aStyle.SetColorCurv(aColor.GetRGB());
        aStyle.SetColorSurf(aColor);
      }
      if (XCAFDoc_ColorTool::GetColor(aSeqValue, XCAFDoc_ColorSurf, aColor))
        aStyle.SetColorSurf(aColor);
      if (XCAFDoc_ColorTool::GetColor(aSeqValue, XCAFDoc_ColorCurv, aColor))
        aStyle.SetColorCurv(aColor.GetRGB());
      if (!aStyle.IsSetColorSurf())
      {
        Handle(XCAFDoc_VisMaterial) aVisMat = XCAFDoc_VisMaterialTool::GetShapeMaterial(aSeqValue);
        if (!aVisMat.IsNull() && !aVisMat->IsEmpty())
        {
          // only color can be stored in STEP
          aStyle.SetColorSurf(aVisMat->BaseColor());
        }
      }
      if (!aStyle.IsSetColorCurv() && !aStyle.IsSetColorSurf() && anIsVisible) continue;

      TopoDS_Shape aSub = XCAFDoc_ShapeTool::GetShape(aSeqValue);
      XCAFPrs_Style* aMapStyle = aSettings.ChangeSeek(aSub);
      if (aMapStyle == NULL)
        aSettings.Add(aSub, aStyle);
      else
        *aMapStyle = aStyle;
    }

    if (aSettings.Extent() <= 0)
      continue;

    // iterate on subshapes and create STEP styles
    Handle(StepVisual_StyledItem) anOverride;
    TopTools_MapOfShape aMap;

    MakeSTEPStyles(Styles, aShape, aSettings, anOverride,
                   aMap, myMapCompMDGPR, DPDCs, ColRGBs, aSTool, 0, anIsComponent);

    const Handle(XSControl_TransferWriter)& aTW = theWS->TransferWriter();
    const Handle(Transfer_FinderProcess)& aFP = aTW->FinderProcess();
    Handle(StepData_StepModel) aStepModel = Handle(StepData_StepModel)::DownCast(aFP->Model());

    // create MDGPR and record it in model
    Handle(StepVisual_MechanicalDesignGeometricPresentationRepresentation) aMDGPR;

    if (!anIsComponent)
    {
      if (myMapCompMDGPR.IsBound(aTopSh))
      {
        Message::SendTrace() << "Error: Current Top-Level shape have MDGPR already " << "\n";
      }
      Styles.CreateMDGPR(aContext, aMDGPR, aStepModel);
      if (!aMDGPR.IsNull())
        myMapCompMDGPR.Bind(aTopSh, aMDGPR);
    }
    else
    {
      // create SDR and add to model.
      Handle(TransferBRep_ShapeMapper) aMapper = TransferBRep::ShapeMapper(aFP, aShape);
      Handle(StepShape_ContextDependentShapeRepresentation) aCDSR;
      if (aFP->FindTypedTransient(aMapper,
          STANDARD_TYPE(StepShape_ContextDependentShapeRepresentation),
          aCDSR))
      {
        // create SDR for NAUO
        Handle(StepRepr_ProductDefinitionShape) aNullPDS; // important to be NULL
        Styles.CreateNAUOSRD(aContext, aCDSR, aNullPDS);

        // search for MDGPR of the component top-level shape
        if (myMapCompMDGPR.IsBound(aTopSh))
        {
          aMDGPR = Handle(StepVisual_MechanicalDesignGeometricPresentationRepresentation)::DownCast(myMapCompMDGPR.Find(aTopSh));
        }
        else
        {
          aMDGPR = new StepVisual_MechanicalDesignGeometricPresentationRepresentation;
          Handle(TCollection_HAsciiString) aReprName = new TCollection_HAsciiString("");
          aMDGPR->SetName(aReprName);
          aMDGPR->SetContextOfItems(aContext);
          myMapCompMDGPR.Bind(aTopSh, aMDGPR);
        }
        Handle(StepRepr_HArray1OfRepresentationItem) anOldItems = aMDGPR->Items();
        Standard_Integer oldLengthlen = 0;
        if (!anOldItems.IsNull())
          oldLengthlen = anOldItems->Length();
        const Standard_Integer aNbIt = oldLengthlen + Styles.NbStyles();
        if (!aNbIt)
          continue;
        Handle(StepRepr_HArray1OfRepresentationItem) aNewItems =
          new StepRepr_HArray1OfRepresentationItem(1, aNbIt);
        Standard_Integer anElemInd = 1;
        for (Standard_Integer aStyleInd = 1; aStyleInd <= oldLengthlen; aStyleInd++)
        {
          aNewItems->SetValue(anElemInd++, anOldItems->Value(aStyleInd));
        }
        for (Standard_Integer aStyleInd = 1; aStyleInd <= Styles.NbStyles(); aStyleInd++)
        {
          aNewItems->SetValue(anElemInd++, Styles.Style(aStyleInd));
        }

        if (aNewItems->Length() > 0)
          aMDGPR->SetItems(aNewItems);
      } //end of work with CDSR
    }
    if (!anIsVisible)
    {
      // create invisibility item and refer for styledItem
      Handle(StepVisual_Invisibility) anInvisibility = new StepVisual_Invisibility();
      Handle(StepVisual_HArray1OfInvisibleItem) anInvisibilitySeq =
        new StepVisual_HArray1OfInvisibleItem(1, Styles.NbStyles());
      // put all style item into the harray
      for (Standard_Integer aStyleInd = 1; aStyleInd <= Styles.NbStyles(); aStyleInd++)
      {
        Handle(StepRepr_RepresentationItem) aStyledItm = Styles.Style(aStyleInd);
        StepVisual_InvisibleItem anInvItem;
        anInvItem.SetValue(aStyledItm);
        anInvisibilitySeq->SetValue(aStyleInd, anInvItem);
      }
      // set the invisibility of items
      anInvisibility->Init(anInvisibilitySeq);
      theWS->Model()->AddWithRefs(anInvisibility);
    }
  }
  return Standard_True;
}

//=======================================================================
//function : writeNames
//purpose  :
//=======================================================================
Standard_Boolean STEPCAFControl_Writer::writeNames(const Handle(XSControl_WorkSession)& theWS,
                                                   const TDF_LabelSequence& theLabels) const
{
  if (theLabels.IsEmpty())
    return Standard_False;

  // get working data
  const Handle(XSControl_TransferWriter)& aTW = theWS->TransferWriter();
  const Handle(Transfer_FinderProcess)& aFP = aTW->FinderProcess();

  // Iterate on requested shapes
  for (TDF_LabelSequence::Iterator aLabelIter(theLabels);
       aLabelIter.More(); aLabelIter.Next())
  {
    const TDF_Label& aLabel = aLabelIter.Value();
    // find target STEP entity for the current shape
    if (!myLabels.IsBound(aLabel))
      continue; // not recorded as translated, skip
    // get name
    Handle(TCollection_HAsciiString) aHName = new TCollection_HAsciiString;
    if (!GetLabelName(aLabel, aHName))
    {
      continue;
    }
    const TopoDS_Shape& aShape = myLabels.Find(aLabel);
    Handle(StepShape_ShapeDefinitionRepresentation) aSDR;
    Handle(StepShape_ContextDependentShapeRepresentation) aCDSR;
    Standard_Boolean isComponent = XCAFDoc_ShapeTool::IsComponent(aLabel) || myPureRefLabels.IsBound(aLabel);
    Handle(TransferBRep_ShapeMapper) aMapper = TransferBRep::ShapeMapper(aFP, aShape);
    if (isComponent && aFP->FindTypedTransient(aMapper, STANDARD_TYPE(StepShape_ContextDependentShapeRepresentation), aCDSR))
    {
      Handle(StepRepr_ProductDefinitionShape) aPDS = aCDSR->RepresentedProductRelation();
      Handle(StepBasic_ProductDefinitionRelationship) aNAUO = aPDS->Definition().ProductDefinitionRelationship();
      if (!aNAUO.IsNull())
        aNAUO->SetName(aHName);
      TopoDS_Shape anInternalAssembly;
      if (myPureRefLabels.Find(aLabel, anInternalAssembly))
      {
        Handle(TransferBRep_ShapeMapper) aMapperOfInternalShape = TransferBRep::ShapeMapper(aFP, anInternalAssembly);
        aFP->FindTypedTransient(aMapperOfInternalShape, STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation), aSDR);
      }
    }
    if (!aSDR.IsNull() ||
        (aCDSR.IsNull() && aFP->FindTypedTransient(aMapper, STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation), aSDR)))
    {
      // set the name to the PRODUCT
      Handle(StepRepr_PropertyDefinition) aPropD = aSDR->Definition().PropertyDefinition();
      if (aPropD.IsNull())
        continue;
      Handle(StepBasic_ProductDefinition) aPD = aPropD->Definition().ProductDefinition();
      if (aPD.IsNull())
        continue;
      Handle(StepBasic_Product) aProd = aPD->Formation()->OfProduct();

      aProd->SetId(aHName);
      aProd->SetName(aHName);
    }
    else
    {
      Message::SendTrace() << "Warning: Cannot find RI for " << aShape.TShape()->DynamicType()->Name() << "\n";
      continue;
    }
  }
  return Standard_True;
}

//=======================================================================
//function : WritePropsForLabel
//purpose  :
//=======================================================================
static Standard_Boolean WritePropsForLabel(const Handle(XSControl_WorkSession)& theWS,
                                           const STEPCAFControl_DataMapOfLabelShape& theLabels,
                                           const TDF_Label& theLabel,
                                           const Standard_CString theIsMulti)
{
  if (theLabel.IsNull())
    return Standard_False;

  STEPConstruct_ValidationProps aProps(theWS);

  TopoDS_Shape aShape = XCAFDoc_ShapeTool::GetShape(theLabel);
  if (aShape.IsNull())
    return Standard_False;

  if (!theIsMulti || XCAFDoc_ShapeTool::IsAssembly(theLabel))
  {
    // write area
    Handle(XCAFDoc_Area) anArea;
    theLabel.FindAttribute(XCAFDoc_Area::GetID(), anArea);
    if (!anArea.IsNull())
    {
      aProps.AddArea(aShape, anArea->Get());
    }
    // write volume
    Handle(XCAFDoc_Volume) aVolume;
    theLabel.FindAttribute(XCAFDoc_Volume::GetID(), aVolume);
    if (!aVolume.IsNull())
    {
      aProps.AddVolume(aShape, aVolume->Get());
    }
  }
  // write centroid
  Handle(XCAFDoc_Centroid) aCentroid;
  theLabel.FindAttribute(XCAFDoc_Centroid::GetID(), aCentroid);
  if (!aCentroid.IsNull())
  {
    aProps.AddCentroid(aShape, aCentroid->Get());
  }

  if (XCAFDoc_ShapeTool::IsCompound(theLabel) || XCAFDoc_ShapeTool::IsAssembly(theLabel))
  {
    if (theLabel.HasChild())
    {
      for (Standard_Integer aChildInd = 1; aChildInd <= theLabel.NbChildren(); aChildInd++)
      {
        WritePropsForLabel(theWS, theLabels, theLabel.FindChild(aChildInd), theIsMulti);
      }
    }
  }

  return Standard_True;
}

//=======================================================================
//function : writeValProps
//purpose  :
//=======================================================================
Standard_Boolean STEPCAFControl_Writer::writeValProps(const Handle(XSControl_WorkSession)& theWS,
                                                      const TDF_LabelSequence& theLabels,
                                                      const Standard_CString theIsMulti) const
{
  if (theLabels.IsEmpty())
    return Standard_False;

  // Iterate on requested shapes
  for (TDF_LabelSequence::Iterator aLabelIter(theLabels);
       aLabelIter.More(); aLabelIter.Next())
  {
    const TDF_Label& aLabel = aLabelIter.Value();

    WritePropsForLabel(theWS, myLabels, aLabel, theIsMulti);
  }

  return Standard_True;
}

//=======================================================================
//function : writeLayers
//purpose  :
//=======================================================================
Standard_Boolean STEPCAFControl_Writer::writeLayers(const Handle(XSControl_WorkSession)& theWS,
                                                    const TDF_LabelSequence& theLabels) const
{

  if (theLabels.IsEmpty())
    return Standard_False;

  // get working data
  const Handle(Interface_InterfaceModel)& aModel = theWS->Model();
  const Handle(XSControl_TransferWriter)& aTW = theWS->TransferWriter();
  const Handle(Transfer_FinderProcess)& aFP = aTW->FinderProcess();
  TDF_LabelSequence aGlobalLayerLS;
  // Iterate on requested shapes collect Tools
  for (TDF_LabelMap::Iterator aLabelIter(myRootLabels);
       aLabelIter.More(); aLabelIter.Next())
  {
    const TDF_Label& aLabel = aLabelIter.Value();
    Handle(XCAFDoc_LayerTool) aLTool = XCAFDoc_DocumentTool::LayerTool(aLabel);
    TDF_LabelSequence aLayerLS;
    aLTool->GetLayerLabels(aLayerLS);
    aGlobalLayerLS.Append(aLayerLS);
  }
  for (TDF_LabelSequence::Iterator aLayerIter(aGlobalLayerLS);
       aLayerIter.More(); aLayerIter.Next())
  {
    const TDF_Label& aLayerL = aLayerIter.Value();
    // get labels of shapes in that layer
    TDF_LabelSequence aShapeLabels;
    XCAFDoc_LayerTool::GetShapesOfLayer(aLayerL, aShapeLabels);
    if (aShapeLabels.IsEmpty())
      continue;

    // name of layer: if not set, is considered as being empty
    Handle(TCollection_HAsciiString) aHName = new TCollection_HAsciiString;
    GetLabelName(aLayerL, aHName);

    // Find target STEP entity for each shape and add to StepVisual_PresentationLayerAssignment items.
    TColStd_SequenceOfTransient aSeqRI;
    for (TDF_LabelSequence::Iterator aShapeIter(aShapeLabels);
         aShapeIter.More(); aShapeIter.Next())
    {
      const TDF_Label& aShLabel = aShapeIter.Value();
      if (aShLabel.IsNull())
        continue;

      // there is no way to assign layer to instance in STEP
      if (XCAFDoc_ShapeTool::IsAssembly(aShLabel) ||
          XCAFDoc_ShapeTool::IsReference(aShLabel))
      {
        continue;
      }
      // check that the shape is one of (uub)labels written during current transfer
      Standard_Boolean anIsWritten = Standard_False;
      for (TDF_LabelSequence::Iterator aLabelIter(theLabels);
           aLabelIter.More(); aLabelIter.Next())
      {
        if (aShLabel.IsDescendant(aLabelIter.Value()))
        {
          anIsWritten = Standard_True;
          break;
        }
      }
      if (!anIsWritten)
      {
        continue;
      }

      // get target STEP entity
      TopoDS_Shape anOneShape = XCAFDoc_ShapeTool::GetShape(aShLabel);

      TopLoc_Location aLoc;
      Standard_Integer aNb = FindEntities(aFP, anOneShape, aLoc, aSeqRI);
      if (aNb <= 0)
      {
        Message::SendTrace() << "Warning: Cannot find RI for " << anOneShape.TShape()->DynamicType()->Name() << "\n";
      }
    }
    if (aSeqRI.IsEmpty())
      continue;

    // analyze visibility
    Handle(StepVisual_PresentationLayerAssignment) aStepLayerAs = new StepVisual_PresentationLayerAssignment;
    Handle(TCollection_HAsciiString) aDescr;
    Handle(TDataStd_UAttribute) aUAttr;
    Standard_Boolean isLinv = Standard_False;
    if (aLayerL.FindAttribute(XCAFDoc::InvisibleGUID(), aUAttr))
    {
      aDescr = new TCollection_HAsciiString("invisible");
      Message::SendTrace() << "\tLayer \"" << aHName->String().ToCString() << "\" is invisible" << "\n";
      isLinv = Standard_True;
    }
    else
    {
      aDescr = new TCollection_HAsciiString("visible");
    }

    // create layer entity
    Standard_Integer anSetStyleInd = 1;
    Handle(StepVisual_HArray1OfLayeredItem) aHArrayOfLItem =
      new StepVisual_HArray1OfLayeredItem(1, aSeqRI.Length());
    for (TColStd_SequenceOfTransient::Iterator aRIIter(aSeqRI);
         aRIIter.More(); aRIIter.Next())
    {
      StepVisual_LayeredItem aLI;
      aLI.SetValue(aRIIter.Value());
      aHArrayOfLItem->SetValue(anSetStyleInd++, aLI);
    }
    aStepLayerAs->Init(aHName, aDescr, aHArrayOfLItem);
    aModel->AddWithRefs(aStepLayerAs);
    // PTV 23.01.2003 add the invisibility AFTER adding layer into the model.
    // add the invisibility for the layer
    if (isLinv)
    {
      // Invisibility Item for containing invisible layers.
      Handle(StepVisual_HArray1OfInvisibleItem) aHInvsblItm = new StepVisual_HArray1OfInvisibleItem(1, 1);
      StepVisual_InvisibleItem aInvIt;
      aInvIt.SetValue(aStepLayerAs);
      aHInvsblItm->SetValue(1, aInvIt);

      Handle(StepVisual_Invisibility) aInvsblt = new StepVisual_Invisibility();
      aInvsblt->Init(aHInvsblItm);
      aModel->AddWithRefs(aInvsblt);
    }
  }
  return Standard_True;
}

//=======================================================================
//function : getSHUOstyle
//purpose  : auxiliary
//=======================================================================
static Standard_Boolean getSHUOstyle(const TDF_Label& theSHUOlab,
                                     XCAFPrs_Style& theSHUOstyle)
{
  Quantity_Color aColor;
  if (!XCAFDoc_ColorTool::IsVisible(theSHUOlab))
  {
    theSHUOstyle.SetVisibility(Standard_False);
  }
  else
  {
    if (XCAFDoc_ColorTool::GetColor(theSHUOlab, XCAFDoc_ColorGen, aColor))
    {
      theSHUOstyle.SetColorCurv(aColor);
      theSHUOstyle.SetColorSurf(aColor);
    }
    if (XCAFDoc_ColorTool::GetColor(theSHUOlab, XCAFDoc_ColorSurf, aColor))
      theSHUOstyle.SetColorSurf(aColor);
    if (XCAFDoc_ColorTool::GetColor(theSHUOlab, XCAFDoc_ColorCurv, aColor))
      theSHUOstyle.SetColorCurv(aColor);
    if (!theSHUOstyle.IsSetColorSurf())
    {
      Handle(XCAFDoc_VisMaterial) aVisMat = XCAFDoc_VisMaterialTool::GetShapeMaterial(theSHUOlab);
      if (!aVisMat.IsNull()
          && !aVisMat->IsEmpty())
      {
        // only color can be stored in STEP
        theSHUOstyle.SetColorSurf(aVisMat->BaseColor());
      }
    }
  }
  if (!theSHUOstyle.IsSetColorCurv() &&
      !theSHUOstyle.IsSetColorSurf() &&
      theSHUOstyle.IsVisible())
  {
    return Standard_False;
  }
  return Standard_True;
}

//=======================================================================
//function : getProDefinitionOfNAUO
//purpose  : auxiliary
//=======================================================================
static Standard_Boolean getProDefinitionOfNAUO(const Handle(XSControl_WorkSession)& theWS,
                                               const TopoDS_Shape& theShape,
                                               Handle(StepBasic_ProductDefinition)& thePD,
                                               Handle(StepRepr_NextAssemblyUsageOccurrence)& theNAUO,
                                               Standard_Boolean theIsRelating)
{
  if (theShape.IsNull())
    return Standard_False;
  // get CDSR
  const Handle(XSControl_TransferWriter)& aTW = theWS->TransferWriter();
  const Handle(Transfer_FinderProcess)& aFP = aTW->FinderProcess();
  Handle(StepShape_ContextDependentShapeRepresentation) aCDSR;
  Handle(TransferBRep_ShapeMapper) aMapper = TransferBRep::ShapeMapper(aFP, theShape);
  if (!aFP->FindTypedTransient(aMapper,
      STANDARD_TYPE(StepShape_ContextDependentShapeRepresentation),
      aCDSR))
  {
    return Standard_False;
  }
  // get PDS of NAUO
  Handle(StepRepr_ProductDefinitionShape) aPDS = aCDSR->RepresentedProductRelation();
  if (aPDS.IsNull())
    return Standard_False;
  // get the NAUO entity
  Interface_Graph aGraph = theWS->HGraph()->Graph();
  for (Interface_EntityIterator aShareds = aGraph.Shareds(aPDS); aShareds.More(); aShareds.Next())
  {
    theNAUO = Handle(StepRepr_NextAssemblyUsageOccurrence)::DownCast(aShareds.Value());
    if (!theNAUO.IsNull())
    {
      break;
    }
  }
  if (theNAUO.IsNull())
    return Standard_False;
  // get Relatinf or Related product definition
  if (!theIsRelating)
    thePD = theNAUO->RelatedProductDefinition();
  else
    thePD = theNAUO->RelatingProductDefinition();
  if (thePD.IsNull())
    return Standard_False;
  return Standard_True;
}

//=======================================================================
//function : writeSHUO
//purpose  : auxiliary
//=======================================================================
static Standard_Boolean writeSHUO(const Handle(XCAFDoc_GraphNode)& theSHUO,
                                  const Handle(XSControl_WorkSession)& theWS,
                                  Handle(StepRepr_SpecifiedHigherUsageOccurrence)& theTopSHUO,
                                  TopoDS_Shape& theNAUOShape,
                                  Handle(StepBasic_ProductDefinition)& theRelatingPD,
                                  Standard_Boolean& theIsDeepest)
{
  // set the ProductDefinitionRelationship descriptin information as empty strings.
  Handle(TCollection_HAsciiString) anEmptyString = new TCollection_HAsciiString("");

  TDF_LabelSequence aNextUsageLabs;
  XCAFDoc_ShapeTool::GetSHUONextUsage(theSHUO->Label(), aNextUsageLabs);
  Handle(XCAFDoc_GraphNode) aNuSHUO;
  if (theTopSHUO.IsNull())
  {
    // the top SHUO
    if (aNextUsageLabs.Length() < 1)
      return Standard_False;
    XCAFDoc_ShapeTool::GetSHUO(aNextUsageLabs.Value(1), aNuSHUO);
    if (aNuSHUO.IsNull())
      return Standard_False;
    // get relating product definition
    TopoDS_Shape aTopCompShape = XCAFDoc_ShapeTool::GetShape(theSHUO->Label().Father());
    Handle(StepRepr_NextAssemblyUsageOccurrence) aRelatingNAUO;
    if (!getProDefinitionOfNAUO(theWS, aTopCompShape,
        theRelatingPD, aRelatingNAUO, Standard_True))
    {
      return Standard_False;
    }
    // get related product definition
    TopoDS_Shape aNUShape = XCAFDoc_ShapeTool::GetShape(aNuSHUO->Label().Father());
    Handle(StepBasic_ProductDefinition) aRelatedPD;
    Handle(StepRepr_NextAssemblyUsageOccurrence) aRelatedNAUO;
    if (!getProDefinitionOfNAUO(theWS, aNUShape,
        aRelatedPD, aRelatedNAUO, Standard_False))
    {
      return Standard_False;
    }

    theTopSHUO = new StepRepr_SpecifiedHigherUsageOccurrence;
    // create deepest shuo EmptyString
    theTopSHUO->Init(/*id*/anEmptyString, /*name*/anEmptyString,
                     /*no description*/Standard_False,/*description*/anEmptyString,
                     theRelatingPD, aRelatedPD,
                     /*no ACURefDesignator*/Standard_False,/*ACURefDesignator*/anEmptyString,
                     /*upper_usage*/aRelatingNAUO, /*next_usage*/aRelatedNAUO);
    // write the other SHUO.
    if (!writeSHUO(aNuSHUO, theWS, theTopSHUO, theNAUOShape, theRelatingPD, theIsDeepest))
    {
      theTopSHUO.Nullify();
      return Standard_False;
    }

    return Standard_True;
  }
  //   Handle(XCAFDoc_GraphNode) NuSHUO;
  if (aNextUsageLabs.Length() > 0)
  {
    // store SHUO recursive
    if (aNextUsageLabs.Length() > 1)
      Message::SendTrace() << "Warning: store only one next_usage of current SHUO" << "\n";
    XCAFDoc_ShapeTool::GetSHUO(aNextUsageLabs.Value(1), aNuSHUO);
    Handle(StepRepr_SpecifiedHigherUsageOccurrence) aNUEntSHUO =
      new StepRepr_SpecifiedHigherUsageOccurrence;
    if (!writeSHUO(aNuSHUO, theWS, aNUEntSHUO, theNAUOShape, theRelatingPD, theIsDeepest))
      return Standard_False;

    // store the deepest SHUO to the dociment
    TopoDS_Shape aNUSh, aUUSh;
    aNUSh = XCAFDoc_ShapeTool::GetShape(aNuSHUO->Label().Father());
    aUUSh = XCAFDoc_ShapeTool::GetShape(theSHUO->Label().Father());
    // get relating PD with upper_usage and related PD with next_usage
    Handle(StepBasic_ProductDefinition) nullPD;// no need to use,case have shared <theRelatingPD>
    Handle(StepBasic_ProductDefinition) aRelatedPD;
    Handle(StepRepr_NextAssemblyUsageOccurrence) UUNAUO, NUNAUO;
    if (!getProDefinitionOfNAUO(theWS, aUUSh, nullPD, UUNAUO, Standard_True) ||
        !getProDefinitionOfNAUO(theWS, aNUSh, aRelatedPD, NUNAUO, Standard_False))
    {
      Message::SendTrace() << "Warning: cannot get related or relating PD" << "\n";
      return Standard_False;
    }
    aNUEntSHUO->Init(/*id*/anEmptyString, /*name*/anEmptyString,
                     /*no description*/Standard_False,/*description*/anEmptyString,
                     theRelatingPD, aRelatedPD,
                     /*no ACURefDesignator*/Standard_False,/*ACURefDesignator*/anEmptyString,
                     /*upper_usage*/theTopSHUO, /*next_usage*/NUNAUO);
    if (theIsDeepest)
    {
      theIsDeepest = Standard_False;
    }
    theWS->Model()->AddWithRefs(aNUEntSHUO);
    return Standard_True;
  } // end of recurse storing

  // get shape
  TDF_Label aShapeL = theSHUO->Label().Father();
  theNAUOShape = XCAFDoc_ShapeTool::GetShape(aShapeL);
  // return to the deepest level from SHUO shape level
  // it is because SHUO is attribute on deep level and shape level.
  theIsDeepest = Standard_True;
  return Standard_True;
}

//=======================================================================
//function : createSHUOStyledItem
//purpose  : auxiliary
//=======================================================================
static Standard_Boolean createSHUOStyledItem(const XCAFPrs_Style& theStyle,
                                             const Handle(StepRepr_ProductDefinitionShape)& thePDS,
                                             const Handle(XSControl_WorkSession)& theWS,
                                             const TopoDS_Shape& theShape,
                                             const Handle(XCAFDoc_ShapeTool)& theSTool,
                                             MoniTool_DataMapOfShapeTransient& theMapCompMDGPR)
{
  // create styled item for the indicated SHUO and store to the model
  STEPConstruct_Styles aStyles(theWS);
  // translate colors to STEP
  Handle(StepVisual_Colour) aSurfColor, aCurvColor;
  Standard_Real aRenderTransp = 0.0;
  if (theStyle.IsSetColorSurf())
  {
    Quantity_ColorRGBA aSurfCol = theStyle.GetColorSurfRGBA();
    aRenderTransp = 1.0 - aSurfCol.Alpha();
    aSurfColor = aStyles.EncodeColor(aSurfCol.GetRGB());
  }
  if (theStyle.IsSetColorCurv())
    aCurvColor = aStyles.EncodeColor(theStyle.GetColorCurv());
  Standard_Boolean isComponent = Standard_True;// cause need to get PSBC
  Handle(StepRepr_RepresentationItem) anItem;
  // set default color for invisible SHUO.
  Standard_Boolean isSetDefaultColor = Standard_False;
  if (aSurfColor.IsNull() && aCurvColor.IsNull() && !theStyle.IsVisible())
  {
    aSurfColor = aStyles.EncodeColor(Quantity_Color(Quantity_NOC_WHITE));
    isSetDefaultColor = Standard_True;
  }
  Handle(StepVisual_PresentationStyleAssignment) aPSA =
    aStyles.MakeColorPSA(anItem, aSurfColor, aCurvColor, aSurfColor, aRenderTransp, isComponent);
  Handle(StepVisual_StyledItem) anOverride; //null styled item

  // find the repr item of the shape
  const Handle(XSControl_TransferWriter)& aTW = theWS->TransferWriter();
  const Handle(Transfer_FinderProcess)& aFP = aTW->FinderProcess();
  Handle(StepData_StepModel) aStepModel = Handle(StepData_StepModel)::DownCast(aFP->Model());
  Handle(TransferBRep_ShapeMapper) aMapper = TransferBRep::ShapeMapper(aFP, theShape);
  Handle(StepShape_ContextDependentShapeRepresentation) aCDSR;
  aFP->FindTypedTransient(aMapper,
                          STANDARD_TYPE(StepShape_ContextDependentShapeRepresentation),
                          aCDSR);
  if (aCDSR.IsNull())
    return Standard_False;
  // find context
  Handle(StepRepr_RepresentationContext) aContext = aStyles.FindContext(theShape);
  TopoDS_Shape aTopSh = theShape;
  if (aContext.IsNull())
  {
    TDF_Label aTopShL = theSTool->FindShape(theShape, Standard_False);
    if (aTopShL.IsNull())
      return Standard_False;
    aTopSh = XCAFDoc_ShapeTool::GetShape(aTopShL);
    aContext = aStyles.FindContext(aTopSh);
  }
  if (aContext.IsNull())
    return Standard_False;
  // get representation item of the shape
  TopLoc_Location aLocation;
  TColStd_SequenceOfTransient aSeqRI;
  FindEntities(aFP, theShape, aLocation, aSeqRI);
  if (aSeqRI.Length() <= 0)
    Message::SendTrace() << "Warning: Cannot find RI for " << theShape.TShape()->DynamicType()->Name() << "\n";
  anItem = Handle(StepRepr_RepresentationItem)::DownCast(aSeqRI(1));
  //get overridden styled item
  getStyledItem(theShape, theSTool, aStyles, anOverride, theMapCompMDGPR);

  // get STEP STYLED ITEM
  Handle(StepVisual_StyledItem) aSTEPstyle = aStyles.AddStyle(anItem, aPSA, anOverride);
  // create SR, SDR and all necessary references between them and ST, PDS, PSBC, GRC
  aStyles.CreateNAUOSRD(aContext, aCDSR, thePDS);

  // add step styled item of SHUO to the model
  // do it by additing styled item to the MDGPR
  if (!aTopSh.IsNull() && !theMapCompMDGPR.IsBound(aTopSh))
  {
    // create MDGPR and record it in model
    Message::SendTrace() << "Warning: " << __FILE__ << ": Create new MDGPR for SHUO instance" << "\n";
    Handle(StepVisual_MechanicalDesignGeometricPresentationRepresentation) aMDGPR;
    aStyles.CreateMDGPR(aContext, aMDGPR, aStepModel);
    if (!aMDGPR.IsNull())
      theMapCompMDGPR.Bind(aTopSh, aMDGPR);
  }
  else if (!aTopSh.IsNull() && theMapCompMDGPR.IsBound(aTopSh))
  {
    // get MDGPR of the top-level shape
    Handle(StepVisual_PresentationRepresentation) aMDGPR =
      Handle(StepVisual_PresentationRepresentation)::DownCast(theMapCompMDGPR.Find(aTopSh));
    // get old styled items to not lose it
    Handle(StepRepr_HArray1OfRepresentationItem) anOldItems = aMDGPR->Items();
    Standard_Integer anOldLengthlen = 0;
    if (!anOldItems.IsNull())
      anOldLengthlen = anOldItems->Length();
    // create new array of styled items by an olds and new one
    Handle(StepRepr_HArray1OfRepresentationItem) aNewItems =
      new StepRepr_HArray1OfRepresentationItem(1, anOldLengthlen + 1);
    Standard_Integer aSetStyleInd = 1;
    for (StepRepr_HArray1OfRepresentationItem::Iterator aStyleIter(anOldItems->Array1());
         aStyleIter.More(); aStyleIter.Next())
    {
      aNewItems->SetValue(aSetStyleInd++, aStyleIter.Value());
    }
    aNewItems->SetValue(aSetStyleInd++, aSTEPstyle);
    // init MDGPR be new array of styled items
    if (aNewItems->Length() > 0)
      aMDGPR->SetItems(aNewItems);
  }
  else
  {
    theWS->Model()->AddWithRefs(aSTEPstyle); // add as root to the model, but it is not good
    Message::SendTrace() << "Warning: " << __FILE__ << ": adds styled item of SHUO as root, cause cannot find MDGPR" << "\n";
  }
  // create invisibility item for the styled item
  if (!theStyle.IsVisible())
  {
    if (isSetDefaultColor)
    {
      // try to set default color from top-level shape

      setDefaultInstanceColor(anOverride, aPSA);
    }
    // create invisibility item and refer for styledItem
    Handle(StepVisual_Invisibility) aInvsblt = new StepVisual_Invisibility();
    Handle(StepVisual_HArray1OfInvisibleItem) aHInvsblItm =
      new StepVisual_HArray1OfInvisibleItem(1, 1);
    // put all style item into the harray
    StepVisual_InvisibleItem anInvItem;
    anInvItem.SetValue(aSTEPstyle);
    aHInvsblItm->SetValue(1, anInvItem);
    aInvsblt->Init(aHInvsblItm);
    theWS->Model()->AddWithRefs(aInvsblt);
  }

  return Standard_True;
}

//=======================================================================
//function : writeSHUOs
//purpose  :
//=======================================================================
Standard_Boolean STEPCAFControl_Writer::writeSHUOs(const Handle(XSControl_WorkSession)& theWS,
                                                   const TDF_LabelSequence& theLabels)
{
  if (theLabels.IsEmpty())
    return Standard_False;

  // map of transferred SHUO
  TColStd_MapOfTransient aMapOfMainSHUO;
  // Iterate on requested shapes
  for (TDF_LabelSequence::Iterator aLabelIter(theLabels);
       aLabelIter.More(); aLabelIter.Next())
  {
    const TDF_Label& aLabel = aLabelIter.Value();
    if (!myLabels.IsBound(aLabel))
      continue; // not recorded as translated, skip
    if (!XCAFDoc_ShapeTool::IsAssembly(aLabel))
    {
      continue;
    }
    TDF_LabelSequence aLabelSeq;
    XCAFDoc_ShapeTool::GetComponents(aLabel, aLabelSeq);
    // iterates on components of assembly
    for (TDF_LabelSequence::Iterator aCompIter(aLabelSeq);
         aCompIter.More(); aCompIter.Next())
    {
      const TDF_Label& aCompL = aCompIter.Value();
      TDF_AttributeSequence anAttrSeq;
      XCAFDoc_ShapeTool::GetAllComponentSHUO(aCompL, anAttrSeq);
      // work with SHUO
      for (TDF_AttributeSequence::Iterator anAttrIter(anAttrSeq);
           anAttrIter.More(); anAttrIter.Next())
      {
        Handle(XCAFDoc_GraphNode) aSHUO =
          Handle(XCAFDoc_GraphNode)::DownCast(anAttrIter.Value());
        // take label of SHUO
        TDF_Label aSHUOlab = aSHUO->Label();
        TDF_LabelSequence aUpLabels;
        // check is it SHUO of upper_usage
        XCAFDoc_ShapeTool::GetSHUOUpperUsage(aSHUOlab, aUpLabels);
        if (aUpLabels.Length() > 0)
          continue; // transfer only main SHUO
        if (aMapOfMainSHUO.Contains(aSHUO))
          continue; // do not try to transfer SHUO twice
        aMapOfMainSHUO.Add(aSHUO);
        // check if it is styled SHUO
        XCAFPrs_Style aSHUOstyle;
        if (!getSHUOstyle(aSHUOlab, aSHUOstyle))
        {
          Message::SendTrace() << "Warning: " << __FILE__ << ": do not store SHUO without any style to the STEP model" << "\n";
          continue;
        }
        // write SHUO to the model amd then add structure type.
        TopoDS_Shape aNAUOShape; // shape of the deepest NAUO in the SHUO structure
        Standard_Boolean isDeepest = Standard_False;
        Handle(StepRepr_SpecifiedHigherUsageOccurrence) anEntOfSHUO;
        Handle(StepBasic_ProductDefinition) aRelatingPD;
        // create the top SHUO and all other.
        writeSHUO(aSHUO, theWS, anEntOfSHUO, aNAUOShape, aRelatingPD, isDeepest);
        if (anEntOfSHUO.IsNull() || aNAUOShape.IsNull())
        {
          Message::SendTrace() << "Warning: " << __FILE__ << ": Cannot store SHUO" << "\n";
          continue;
        }
        // create new Product Definition Shape for TOP SHUO
        Message::SendTrace() << "Info: " << __FILE__ << ": Create NEW PDS for current SHUO " << "\n";
        Handle(StepRepr_ProductDefinitionShape) aPDS = new StepRepr_ProductDefinitionShape;
        Handle(TCollection_HAsciiString) aPDSname = new TCollection_HAsciiString("SHUO");
        Handle(TCollection_HAsciiString) aDescrStr = new TCollection_HAsciiString("");
        StepRepr_CharacterizedDefinition aCharDef;
        aCharDef.SetValue(anEntOfSHUO);
        aPDS->Init(aPDSname, Standard_False, aDescrStr, aCharDef);

        // create styled item for SHUO and add to the model
        createSHUOStyledItem(aSHUOstyle, aPDS, theWS, aNAUOShape, XCAFDoc_DocumentTool::ShapeTool(aLabel), myMapCompMDGPR);

      } // end work with SHUO
    } // end of an assembly components
  } // end of iterates on indicated labels
  return Standard_True;
}

//=======================================================================
//function : FindPDSforDGT
//purpose  : auxiliary: find PDS for AdvancedFace or EdgeCurve for creation
//                     needed ShapeAspect in D&GT structure
//=======================================================================
static Standard_Boolean FindPDSforDGT(const Interface_Graph& theGraph,
                                      const Handle(Standard_Transient)& theEnt,
                                      Handle(StepRepr_ProductDefinitionShape)& thePDS,
                                      Handle(StepRepr_RepresentationContext)& theRC,
                                      Handle(StepShape_AdvancedFace)& theAF,
                                      Handle(StepShape_EdgeCurve)& theEC)
{
  if (theEnt.IsNull())
    return Standard_False;
  if (!theEnt->IsKind(STANDARD_TYPE(StepShape_EdgeCurve)) &&
      !theEnt->IsKind(STANDARD_TYPE(StepShape_AdvancedFace)))
  {
    return Standard_False;
  }

  theAF = Handle(StepShape_AdvancedFace)::DownCast(theEnt);
  if (theAF.IsNull())
  {
    theEC = Handle(StepShape_EdgeCurve)::DownCast(theEnt);
    for (Interface_EntityIterator aSharingsIter = theGraph.Sharings(theEC);
         aSharingsIter.More() && theAF.IsNull(); aSharingsIter.Next())
    {
      Handle(StepShape_OrientedEdge) anOE = Handle(StepShape_OrientedEdge)::DownCast(aSharingsIter.Value());
      if (anOE.IsNull())
        continue;
      for (Interface_EntityIterator aSubs1 = theGraph.Sharings(anOE);
           aSubs1.More() && theAF.IsNull(); aSubs1.Next())
      {
        Handle(StepShape_EdgeLoop) aEL = Handle(StepShape_EdgeLoop)::DownCast(aSubs1.Value());
        if (aEL.IsNull())
          continue;
        for (Interface_EntityIterator aSubs2 = theGraph.Sharings(aEL);
             aSubs2.More() && theAF.IsNull(); aSubs2.Next())
        {
          Handle(StepShape_FaceBound) aFB = Handle(StepShape_FaceBound)::DownCast(aSubs2.Value());
          if (aFB.IsNull())
            continue;
          for (Interface_EntityIterator aSubs3 = theGraph.Sharings(aFB);
               aSubs3.More() && theAF.IsNull(); aSubs3.Next())
          {
            theAF = Handle(StepShape_AdvancedFace)::DownCast(aSubs3.Value());
          }
        }
      }
    }
  }
  if (theAF.IsNull())
    return Standard_False;

  for (Interface_EntityIterator aSharingsIter = theGraph.Sharings(theAF);
       aSharingsIter.More() && thePDS.IsNull(); aSharingsIter.Next())
  {
    Handle(StepShape_ConnectedFaceSet) aCFS =
      Handle(StepShape_ConnectedFaceSet)::DownCast(aSharingsIter.Value());
    if (aCFS.IsNull())
      continue;
    for (Interface_EntityIterator aSubs1 = theGraph.Sharings(aCFS);
         aSubs1.More() && thePDS.IsNull(); aSubs1.Next())
    {
      Handle(StepRepr_RepresentationItem) aRI =
        Handle(StepRepr_RepresentationItem)::DownCast(aSubs1.Value());
      if (aRI.IsNull())
        continue;
      for (Interface_EntityIterator aSubs2 = theGraph.Sharings(aRI);
           aSubs2.More() && thePDS.IsNull(); aSubs2.Next())
      {
        Handle(StepShape_ShapeRepresentation) aSR =
          Handle(StepShape_ShapeRepresentation)::DownCast(aSubs2.Value());
        if (aSR.IsNull())
          continue;
        theRC = aSR->ContextOfItems();
        for (Interface_EntityIterator aSubs3 = theGraph.Sharings(aSR);
             aSubs3.More() && thePDS.IsNull(); aSubs3.Next())
        {
          Handle(StepShape_ShapeDefinitionRepresentation) aSDR =
            Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(aSubs3.Value());
          if (aSDR.IsNull())
            continue;
          Handle(StepRepr_PropertyDefinition) aPropD = aSDR->Definition().PropertyDefinition();
          if (aPropD.IsNull())
            continue;
          thePDS = Handle(StepRepr_ProductDefinitionShape)::DownCast(aPropD);
        }
      }
    }
  }
  return Standard_True;
}

//=======================================================================
//function : FindPDS
//purpose  : auxiliary: find Product_definition_shape entity for given entity
//=======================================================================
static Handle(StepRepr_ProductDefinitionShape) FindPDS(const Interface_Graph& theGraph,
                                                       const Handle(Standard_Transient)& theEnt,
                                                       Handle(StepRepr_RepresentationContext)& theRC)
{
  if (theEnt.IsNull())
    return NULL;
  Handle(StepRepr_ProductDefinitionShape) aPDS;

  // try to find shape_representation in sharings
  for (Interface_EntityIterator anIter = theGraph.Sharings(theEnt);
       anIter.More() && aPDS.IsNull(); anIter.Next())
  {
    Handle(StepShape_ShapeRepresentation) aSR = Handle(StepShape_ShapeRepresentation)::DownCast(anIter.Value());
    if (aSR.IsNull())
      continue;
    theRC = aSR->ContextOfItems();
    Interface_EntityIterator aSDRIt = theGraph.Sharings(aSR);
    for (aSDRIt.Start(); aSDRIt.More() && aPDS.IsNull(); aSDRIt.Next())
    {
      Handle(StepShape_ShapeDefinitionRepresentation) aSDR =
        Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(aSDRIt.Value());
      if (aSDR.IsNull()) continue;
      Handle(StepRepr_PropertyDefinition) aPropD = aSDR->Definition().PropertyDefinition();
      if (aPropD.IsNull()) continue;
      aPDS = Handle(StepRepr_ProductDefinitionShape)::DownCast(aPropD);
    }
  }
  if (!aPDS.IsNull())
    return aPDS;

  for (Interface_EntityIterator anIter = theGraph.Sharings(theEnt);
       anIter.More(); anIter.Next())
  {
    if (anIter.Value()->IsKind(STANDARD_TYPE(StepShape_TopologicalRepresentationItem)) ||
        anIter.Value()->IsKind(STANDARD_TYPE(StepGeom_GeometricRepresentationItem)))
    {
      aPDS = FindPDS(theGraph, anIter.Value(), theRC);
      if (!aPDS.IsNull())
        return aPDS;
    }
  }
  return aPDS;
}

//=======================================================================
//function : GetUnit
//purpose  : auxiliary
//=======================================================================
static StepBasic_Unit GetUnit(const Handle(StepRepr_RepresentationContext)& theRC,
                              const Standard_Boolean theIsAngle = Standard_False)
{
  StepBasic_Unit aUnit;
  Handle(StepBasic_NamedUnit) aCurrentUnit;
  if (theIsAngle)
  {
    Handle(StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext) aCtx =
      Handle(StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext)::DownCast(theRC);
    if (!aCtx.IsNull())
    {
      for (StepBasic_HArray1OfNamedUnit::Iterator aUnitIter(aCtx->Units()->Array1());
           aUnitIter.More(); aUnitIter.Next())
      {
        const Handle(StepBasic_NamedUnit)& aCurUnitValue = aUnitIter.Value();
        if (aCurUnitValue->IsKind(STANDARD_TYPE(StepBasic_SiUnitAndPlaneAngleUnit)) ||
            aCurUnitValue->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndPlaneAngleUnit)))
        {
          aCurrentUnit = aCurUnitValue;
          break;
        }
      }
    }
    if (aCurrentUnit.IsNull())
    {
      Handle(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx) aCtx1 =
        Handle(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx)::DownCast(theRC);
      if (!aCtx1.IsNull())
      {
        for (StepBasic_HArray1OfNamedUnit::Iterator aUnitIter(aCtx1->Units()->Array1());
             aUnitIter.More(); aUnitIter.Next())
        {
          const Handle(StepBasic_NamedUnit)& aCurUnitValue = aUnitIter.Value();
          if (aCurUnitValue->IsKind(STANDARD_TYPE(StepBasic_SiUnitAndPlaneAngleUnit)) ||
              aCurUnitValue->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndPlaneAngleUnit)))
          {
            aCurrentUnit = aCurUnitValue;
            break;
          }
        }
      }
    }
    if (aCurrentUnit.IsNull())
      aCurrentUnit = new StepBasic_SiUnitAndPlaneAngleUnit;
  }
  else
  {
    Handle(StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext) aCtx =
      Handle(StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext)::DownCast(theRC);
    if (!aCtx.IsNull())
    {
      for (StepBasic_HArray1OfNamedUnit::Iterator aUnitIter(aCtx->Units()->Array1());
           aUnitIter.More(); aUnitIter.Next())
      {
        const Handle(StepBasic_NamedUnit)& aCurUnitValue = aUnitIter.Value();
        if (aCurUnitValue->IsKind(STANDARD_TYPE(StepBasic_SiUnitAndLengthUnit)) ||
            aCurUnitValue->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndLengthUnit)))
        {
          aCurrentUnit = aCurUnitValue;
          break;
        }
      }
    }
    if (aCurrentUnit.IsNull())
    {
      Handle(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx) aCtx1 =
        Handle(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx)::DownCast(theRC);
      if (!aCtx1.IsNull())
      {
        for (StepBasic_HArray1OfNamedUnit::Iterator aUnitIter(aCtx1->Units()->Array1());
             aUnitIter.More(); aUnitIter.Next())
        {
          const Handle(StepBasic_NamedUnit)& aCurUnitValue = aUnitIter.Value();
          if (aCurUnitValue->IsKind(STANDARD_TYPE(StepBasic_SiUnitAndLengthUnit)) ||
              aCurUnitValue->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndLengthUnit)))
          {
            aCurrentUnit = aCurUnitValue;
            break;
          }
        }
      }
    }
    if (aCurrentUnit.IsNull())
      aCurrentUnit = new StepBasic_SiUnitAndLengthUnit;
  }

  aUnit.SetValue(aCurrentUnit);
  return aUnit;
}

//=======================================================================
//function : CreateDimValue
//purpose  : auxiliary
//======================================================================
static Handle(StepRepr_ReprItemAndMeasureWithUnit) CreateDimValue(const Standard_Real theValue,
                                                                  const StepBasic_Unit& theUnit,
                                                                  const Handle(TCollection_HAsciiString)& theName,
                                                                  const Standard_CString theMeasureName,
                                                                  const Standard_Boolean theIsAngle,
                                                                  const Standard_Boolean theIsQualified = Standard_False,
                                                                  const Handle(StepShape_QualifiedRepresentationItem)& theQRI = NULL)
{
  Handle(StepRepr_RepresentationItem) aReprItem = new StepRepr_RepresentationItem();
  aReprItem->Init(new TCollection_HAsciiString(theName));
  Handle(StepBasic_MeasureWithUnit) aMWU = new StepBasic_MeasureWithUnit();
  Handle(StepBasic_MeasureValueMember) aValueMember = new StepBasic_MeasureValueMember();
  aValueMember->SetName(theMeasureName);
  aValueMember->SetReal(theValue);
  aMWU->Init(aValueMember, theUnit);
  if (theIsQualified)
  {
    if (theIsAngle)
    {
      // Angle & with qualifiers
      Handle(StepRepr_ReprItemAndPlaneAngleMeasureWithUnitAndQRI) anItem =
        new StepRepr_ReprItemAndPlaneAngleMeasureWithUnitAndQRI();
      anItem->Init(aMWU, aReprItem, theQRI);
      return anItem;
    }
    else
    {
      // Length & with qualifiers
      Handle(StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI) anItem =
        new StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI();
      anItem->Init(aMWU, aReprItem, theQRI);
      return anItem;
    }
  }
  else
  {
    if (theIsAngle)
    {
      // Angle & without qualifiers
      Handle(StepRepr_ReprItemAndPlaneAngleMeasureWithUnit) anItem =
        new StepRepr_ReprItemAndPlaneAngleMeasureWithUnit();
      anItem->Init(aMWU, aReprItem);
      return anItem;
    }
    else
    {
      // Length & without qualifiers
      Handle(StepRepr_ReprItemAndLengthMeasureWithUnit) anItem =
        new StepRepr_ReprItemAndLengthMeasureWithUnit();
      anItem->Init(aMWU, aReprItem);
      return anItem;
    }
  }
}

//=======================================================================
//function : writeShapeAspect
//purpose  : auxiliary (write Shape_Aspect entity for given shape)
//=======================================================================
Handle(StepRepr_ShapeAspect) STEPCAFControl_Writer::writeShapeAspect(const Handle(XSControl_WorkSession)& theWS,
                                                                     const TDF_Label theLabel,
                                                                     const TopoDS_Shape& theShape,
                                                                     Handle(StepRepr_RepresentationContext)& theRC,
                                                                     Handle(StepAP242_GeometricItemSpecificUsage)& theGISU)
{
  // Get working data
  const Handle(Interface_InterfaceModel)& aModel = theWS->Model();
  const Handle(XSControl_TransferWriter)& aTW = theWS->TransferWriter();
  const Handle(Transfer_FinderProcess)& aFP = aTW->FinderProcess();
  const Handle(Interface_HGraph) aHGraph = theWS->HGraph();
  if (aHGraph.IsNull())
    return NULL;
  Interface_Graph aGraph = aHGraph->Graph();

  TopLoc_Location aLoc;
  TColStd_SequenceOfTransient aSeqRI;
  FindEntities(aFP, theShape, aLoc, aSeqRI);
  if (aSeqRI.Length() <= 0)
  {
    Message::SendTrace() << "Warning: Cannot find RI for " << theShape.TShape()->DynamicType()->Name() << "\n";
    return NULL;
  }

  Handle(StepRepr_ProductDefinitionShape) aPDS;
  Handle(StepRepr_RepresentationContext) aRC;
  Handle(Standard_Transient) anEnt = aSeqRI.Value(1);
  aPDS = FindPDS(aGraph, anEnt, aRC);
  if (aPDS.IsNull())
    return NULL;

  theRC = aRC;
  // Shape_Aspect
  Handle(TCollection_HAsciiString) aName = new TCollection_HAsciiString();
  Handle(TDataStd_Name) aNameAttr;
  if (theLabel.FindAttribute(TDataStd_Name::GetID(), aNameAttr))
  {
    aName = new TCollection_HAsciiString(TCollection_AsciiString(aNameAttr->Get()));
    Standard_Integer aFirstSpace = aName->Search(" ");
    if (aFirstSpace != -1)
      aName = aName->SubString(aFirstSpace + 1, aName->Length());
    else
      aName = new TCollection_HAsciiString();
  }
  Handle(TCollection_HAsciiString) aDescription = new TCollection_HAsciiString();
  Handle(StepRepr_ShapeAspect) aSA = new StepRepr_ShapeAspect;
  aSA->Init(aName, aDescription, aPDS, StepData_LTrue);

  // Geometric_Item_Specific_Usage
  Handle(StepAP242_GeometricItemSpecificUsage) aGISU = new StepAP242_GeometricItemSpecificUsage();
  StepAP242_ItemIdentifiedRepresentationUsageDefinition aDefinition;
  aDefinition.SetValue(aSA);
  Handle(StepRepr_HArray1OfRepresentationItem) anReprItems = new StepRepr_HArray1OfRepresentationItem(1, 1);
  Handle(StepRepr_RepresentationItem) anIdentifiedItem = Handle(StepRepr_RepresentationItem)::DownCast(anEnt);
  anReprItems->SetValue(1, anIdentifiedItem);
  Handle(StepShape_ShapeDefinitionRepresentation) aSDR;
  for (Interface_EntityIterator aSharingIter = aGraph.Sharings(aPDS);
       aSharingIter.More() && aSDR.IsNull(); aSharingIter.Next())
  {
    const Handle(Standard_Transient)& anEntity = aSharingIter.Value();
    aSDR = Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(anEntity);
  }
  if (aSDR.IsNull())
    return NULL;

  // Set entities to model
  aGISU->Init(aName, aDescription, aDefinition, aSDR->UsedRepresentation(), anReprItems);
  aModel->AddWithRefs(aSA);
  aModel->AddWithRefs(aGISU);
  theGISU = aGISU;
  return aSA;
}

//=======================================================================
//function : writePresentation
//purpose  : auxiliary (write annotation plane and presentation)
//======================================================================
void STEPCAFControl_Writer::writePresentation(const Handle(XSControl_WorkSession)& theWS,
                                              const TopoDS_Shape& thePresentation,
                                              const Handle(TCollection_HAsciiString)& thePrsName,
                                              const Standard_Boolean theHasSemantic,
                                              const Standard_Boolean theHasPlane,
                                              const gp_Ax2& theAnnotationPlane,
                                              const gp_Pnt& theTextPosition,
                                              const Handle(Standard_Transient)& theDimension,
                                              const StepData_Factors& theLocalFactors)
{
  if (thePresentation.IsNull())
    return;
  // Get working data
  const Handle(Interface_InterfaceModel)& aModel = theWS->Model();

  // Presentation
  Handle(StepVisual_TessellatedGeometricSet) aGeomSet = STEPCAFControl_GDTProperty::GetTessellation(thePresentation);
  if (aGeomSet.IsNull())
  {
    return;
  }
  Handle(StepVisual_TessellatedAnnotationOccurrence) aTAO = new StepVisual_TessellatedAnnotationOccurrence();
  aTAO->Init(new TCollection_HAsciiString(), myGDTPrsCurveStyle, aGeomSet);
  StepVisual_DraughtingCalloutElement aDCElement;
  aDCElement.SetValue(aTAO);
  Handle(StepVisual_HArray1OfDraughtingCalloutElement) aTAOs = new StepVisual_HArray1OfDraughtingCalloutElement(1, 1);
  aTAOs->SetValue(1, aDCElement);
  Handle(StepVisual_DraughtingCallout) aDCallout = new StepVisual_DraughtingCallout();
  Handle(TCollection_HAsciiString) aPrsName = thePrsName.IsNull() ? new TCollection_HAsciiString() : thePrsName;
  aDCallout->Init(aPrsName, aTAOs);
  Handle(StepRepr_HArray1OfRepresentationItem) aDCsForDMIA = new StepRepr_HArray1OfRepresentationItem(1, 1);
  aDCsForDMIA->SetValue(1, aDCallout);
  myGDTAnnotations.Append(aDCallout);
  StepAP242_ItemIdentifiedRepresentationUsageDefinition aDimension;
  aDimension.SetValue(theDimension);
  Handle(TCollection_HAsciiString) aDMIAName;
  if (theHasSemantic)
    aDMIAName = new TCollection_HAsciiString("PMI representation to presentation link");
  else
    aDMIAName = new TCollection_HAsciiString();
  Handle(StepAP242_DraughtingModelItemAssociation) aDMIA =
    new StepAP242_DraughtingModelItemAssociation();
  aDMIA->Init(aDMIAName, new TCollection_HAsciiString(), aDimension, myGDTPresentationDM, aDCsForDMIA);
  aModel->AddWithRefs(aDMIA);

  if (!theHasPlane)
    return;

  // Annotation plane
  // Presentation Style
  Handle(StepVisual_NullStyleMember) aNullStyle = new StepVisual_NullStyleMember();
  aNullStyle->SetEnumText(0, ".NULL.");
  StepVisual_PresentationStyleSelect aStyleItem;
  aStyleItem.SetValue(aNullStyle);
  Handle(StepVisual_HArray1OfPresentationStyleSelect) aStyles = new StepVisual_HArray1OfPresentationStyleSelect(1, 1);
  aStyles->SetValue(1, aStyleItem);
  Handle(StepVisual_PresentationStyleAssignment) aPrsStyle = new StepVisual_PresentationStyleAssignment();
  aPrsStyle->Init(aStyles);
  Handle(StepVisual_HArray1OfPresentationStyleAssignment) aPrsStyles =
    new StepVisual_HArray1OfPresentationStyleAssignment(1, 1);
  aPrsStyles->SetValue(1, aPrsStyle);
  // Plane
  Handle(StepGeom_Plane) aPlane = new StepGeom_Plane();
  GeomToStep_MakeAxis2Placement3d anAxisMaker(theAnnotationPlane, theLocalFactors);
  const Handle(StepGeom_Axis2Placement3d)& anAxis = anAxisMaker.Value();
  // Set text position to plane origin
  Handle(StepGeom_CartesianPoint) aTextPos = new StepGeom_CartesianPoint();
  Handle(TColStd_HArray1OfReal) aCoords = new TColStd_HArray1OfReal(1, 3);
  for (Standard_Integer i = 1; i <= 3; i++)
    aCoords->SetValue(i, theTextPosition.Coord(i));
  aTextPos->Init(new TCollection_HAsciiString(), aCoords);
  anAxis->SetLocation(aTextPos);
  aPlane->Init(new TCollection_HAsciiString(), anAxis);
  // Annotation plane element
  StepVisual_AnnotationPlaneElement aPlaneElement;
  aPlaneElement.SetValue(aDCallout);
  Handle(StepVisual_HArray1OfAnnotationPlaneElement) aDCsForAnnPln = new StepVisual_HArray1OfAnnotationPlaneElement(1, 1);
  aDCsForAnnPln->SetValue(1, aPlaneElement);
  // Init AnnotationPlane entity
  Handle(StepVisual_AnnotationPlane) anAnnPlane = new StepVisual_AnnotationPlane();
  anAnnPlane->Init(new TCollection_HAsciiString(), aPrsStyles, aPlane, aDCsForAnnPln);
  myGDTAnnotations.Append(anAnnPlane);
  aModel->AddWithRefs(anAnnPlane);
}

//=======================================================================
//function : writeDatumAP242
//purpose  : auxiliary (write Datum entity for given shape or write all
//           necessary entities and link them to already written datum
//           in case of multiple features association)
//=======================================================================
Handle(StepDimTol_Datum) STEPCAFControl_Writer::writeDatumAP242(const Handle(XSControl_WorkSession)& theWS,
                                                                const TDF_LabelSequence& theShapeL,
                                                                const TDF_Label& theDatumL,
                                                                const Standard_Boolean theIsFirstDTarget,
                                                                const Handle(StepDimTol_Datum)& theWrittenDatum,
                                                                const StepData_Factors& theLocalFactors)
{
  // Get working data
  const Handle(Interface_InterfaceModel)& aModel = theWS->Model();
  const Handle(XSControl_TransferWriter)& aTW = theWS->TransferWriter();
  const Handle(Transfer_FinderProcess)& aFP = aTW->FinderProcess();
  const Handle(Interface_HGraph) aHGraph = theWS->HGraph();
  if (aHGraph.IsNull())
    return NULL;
  Interface_Graph aGraph = aHGraph->Graph();

  Handle(StepRepr_ShapeAspect) aSA;
  Handle(StepRepr_RepresentationContext) aRC;
  Handle(StepRepr_ProductDefinitionShape) aPDS;
  NCollection_Sequence<Handle(StepRepr_ShapeAspect)> aSASeq;
  Handle(StepAP242_GeometricItemSpecificUsage) aGISU;
  Standard_Integer aSANum = 0, aGISUNum = 0;
  // Link with datum feature
  for (TDF_LabelSequence::Iterator aLabelIter(theShapeL);
       aLabelIter.More(); aLabelIter.Next())
  {
    Handle(Standard_Transient) anEnt;
    TopLoc_Location aLoc;
    TColStd_SequenceOfTransient aSeqRI;

    TopoDS_Shape aShape = XCAFDoc_ShapeTool::GetShape(aLabelIter.Value());
    FindEntities(aFP, aShape, aLoc, aSeqRI);
    if (aSeqRI.Length() <= 0)
    {
      Message::SendTrace() << "Warning: Cannot find RI for " << aShape.TShape()->DynamicType()->Name() << "\n";
      continue;
    }
    anEnt = aSeqRI.Value(1);
    aPDS = FindPDS(aGraph, anEnt, aRC);
    if (aPDS.IsNull())
      continue;

    Handle(StepRepr_ShapeAspect) aCurrentSA = writeShapeAspect(theWS, theDatumL, aShape, aRC, aGISU);
    if (aCurrentSA.IsNull())
      continue;
    aSASeq.Append(aCurrentSA);
    aSANum = aModel->Number(aCurrentSA);
    aGISUNum = aModel->Number(aGISU);
  }
  if (aPDS.IsNull())
  {
    // Workaround for datums without shape
    aPDS = myGDTCommonPDS;
    Interface_EntityIterator aSDRIt = aGraph.Sharings(aPDS);
    Handle(StepShape_ShapeDefinitionRepresentation) aSDR;
    for (aSDRIt.Start(); aSDRIt.More() && aSDR.IsNull(); aSDRIt.Next())
      aSDR = Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(aSDRIt.Value());
    if (!aSDR.IsNull())
    {
      Handle(StepRepr_Representation) aRepr = aSDR->UsedRepresentation();
      if (!aRepr.IsNull())
        aRC = aRepr->ContextOfItems();
    }
  }

  // Find if datum has datum targets and get common datum attributes
  Handle(XCAFDoc_Datum) aDatumAttr;
  if (!theDatumL.FindAttribute(XCAFDoc_Datum::GetID(), aDatumAttr))
    return NULL;
  Handle(XCAFDimTolObjects_DatumObject) anObject = aDatumAttr->GetObject();
  if (anObject.IsNull())
    return NULL;
  Standard_Boolean isSimpleDatum = !anObject->IsDatumTarget();
  Handle(TCollection_HAsciiString) anIdentifier = anObject->GetName();
  Handle(TCollection_HAsciiString) aTargetId = (anObject->GetDatumTargetNumber() == 0 ?
                                                new TCollection_HAsciiString() : new TCollection_HAsciiString(anObject->GetDatumTargetNumber()));

  // If datum type is area, but there is no area in object, write as simple datum
  if (anObject->GetDatumTargetType() == XCAFDimTolObjects_DatumTargetType_Area &&
      anObject->GetDatumTarget().IsNull())
    isSimpleDatum = Standard_True;

  // Simple datum
  if (isSimpleDatum)
  {
    if (aSASeq.Length() == 0)
    {
      // Create empty datum with name and presentation only
      Handle(StepDimTol_DatumFeature) aDF = new StepDimTol_DatumFeature();
      aDF->Init(new TCollection_HAsciiString(), new TCollection_HAsciiString(), aPDS, StepData_LTrue);
      aSA = aDF;
      aModel->AddWithRefs(aDF);
    }
    else if (aSASeq.Length() == 1)
    {
      Handle(StepDimTol_DatumFeature) aDF = new StepDimTol_DatumFeature();
      aDF->Init(new TCollection_HAsciiString(), new TCollection_HAsciiString(), aPDS, StepData_LTrue);
      aModel->ReplaceEntity(aSANum, aDF);
      aSA = aDF;
      StepAP242_ItemIdentifiedRepresentationUsageDefinition aDefinition;
      aDefinition.SetValue(aDF);
      aGISU->SetDefinition(aDefinition);
      aModel->ReplaceEntity(aGISUNum, aGISU);
    }
    else if (aSASeq.Length() > 1)
    {
      Handle(StepRepr_CompShAspAndDatumFeatAndShAsp) aDF = new StepRepr_CompShAspAndDatumFeatAndShAsp();
      aDF->Init(new TCollection_HAsciiString(), new TCollection_HAsciiString(), aPDS, StepData_LTrue);
      for (Standard_Integer i = 1; i <= aSASeq.Length(); i++)
      {
        Handle(StepRepr_ShapeAspectRelationship) aSAR = new StepRepr_ShapeAspectRelationship();
        aSAR->Init(new TCollection_HAsciiString(), Standard_False, new TCollection_HAsciiString(), aDF, aSASeq.Value(i));
        aModel->AddWithRefs(aSAR);
      }
      aSA = aDF;
      aModel->AddWithRefs(aDF);
    }
  }
  // Datum with datum targets
  else
  {
    XCAFDimTolObjects_DatumTargetType aDatumType = anObject->GetDatumTargetType();
    Handle(StepDimTol_DatumTarget) aDatumTarget;
    // Note: the given way to write such datum type may be incorrect (too little information)
    if (aDatumType == XCAFDimTolObjects_DatumTargetType_Area)
    {
      TopoDS_Shape aDTShape = anObject->GetDatumTarget();
      Handle(StepAP242_GeometricItemSpecificUsage) anAreaGISU;
      Handle(StepRepr_ShapeAspect) anAreaSA = writeShapeAspect(theWS, theDatumL, aDTShape, aRC, anAreaGISU);
      aSANum = aModel->Number(anAreaSA);
      aGISUNum = aModel->Number(anAreaGISU);
      Handle(StepDimTol_DatumTarget) aDT = new StepDimTol_DatumTarget();
      aDT->Init(new TCollection_HAsciiString(), new TCollection_HAsciiString("area"), anAreaSA->OfShape(),
                StepData_LTrue, aTargetId);
      aModel->ReplaceEntity(aSANum, aDT);
      StepAP242_ItemIdentifiedRepresentationUsageDefinition aDefinition;
      aDefinition.SetValue(aDT);
      anAreaGISU->SetDefinition(aDefinition);
      aModel->ReplaceEntity(aGISUNum, anAreaGISU);
    }
    else
    {
      Handle(StepDimTol_PlacedDatumTargetFeature) aPDTF = new StepDimTol_PlacedDatumTargetFeature();
      aPDTF->Init(new TCollection_HAsciiString(), STEPCAFControl_GDTProperty::GetDatumTargetName(aDatumType),
                  aPDS, StepData_LTrue, aTargetId);
      aModel->AddWithRefs(aPDTF);
      aDatumTarget = aPDTF;
      // Datum targets
      Handle(StepRepr_PropertyDefinition) aPD = new StepRepr_PropertyDefinition();
      StepRepr_CharacterizedDefinition aCDefinition;
      aCDefinition.SetValue(aPDTF);
      aPD->Init(new TCollection_HAsciiString(), Standard_False, NULL, aCDefinition);
      if (anObject->HasDatumTargetParams())
      {
        // write all parameters of datum target
        Handle(StepShape_ShapeRepresentationWithParameters) aSRWP = new StepShape_ShapeRepresentationWithParameters();
        // Common for all datum targets
        StepBasic_Unit aUnit = GetUnit(aRC);
        gp_Ax2 aDTAxis = anObject->GetDatumTargetAxis();
        GeomToStep_MakeAxis2Placement3d anAxisMaker(aDTAxis, theLocalFactors);
        Handle(StepGeom_Axis2Placement3d) anA2P3D = anAxisMaker.Value();
        anA2P3D->SetName(new TCollection_HAsciiString("orientation"));
        Handle(StepRepr_HArray1OfRepresentationItem) anItems;
        // Process each datum target type
        if (aDatumType == XCAFDimTolObjects_DatumTargetType_Point)
        {
          anItems = new StepRepr_HArray1OfRepresentationItem(1, 1);
        }
        else
        {
          Handle(TCollection_HAsciiString) aTargetValueName;
          if (aDatumType == XCAFDimTolObjects_DatumTargetType_Line)
          {
            anItems = new StepRepr_HArray1OfRepresentationItem(1, 2);
            aTargetValueName = new TCollection_HAsciiString("target length");
          }
          else if (aDatumType == XCAFDimTolObjects_DatumTargetType_Rectangle)
          {
            anItems = new StepRepr_HArray1OfRepresentationItem(1, 3);
            aTargetValueName = new TCollection_HAsciiString("target length");
            // Additional value
            Handle(StepRepr_ReprItemAndMeasureWithUnit) aTargetValue = CreateDimValue(anObject->GetDatumTargetWidth(),
                                                                                      aUnit, new TCollection_HAsciiString("target width"), "POSITIVE_LENGTH_MEASURE", Standard_False);
            anItems->SetValue(2, aTargetValue);
            aModel->AddWithRefs(aTargetValue);
          }
          else if (aDatumType == XCAFDimTolObjects_DatumTargetType_Circle)
          {
            anItems = new StepRepr_HArray1OfRepresentationItem(1, 2);
            aTargetValueName = new TCollection_HAsciiString("target diameter");
          }
          // Value
          Handle(StepRepr_ReprItemAndMeasureWithUnit) aTargetValue = CreateDimValue(anObject->GetDatumTargetLength(),
                                                                                    aUnit, aTargetValueName, "POSITIVE_LENGTH_MEASURE", Standard_False);
          anItems->SetValue(1, aTargetValue);
          aModel->AddWithRefs(aTargetValue);
        }
        anItems->SetValue(anItems->Length(), anA2P3D);
        aSRWP->Init(new TCollection_HAsciiString(), anItems, aRC);
        // Create and write auxiliary entities
        Handle(StepShape_ShapeDefinitionRepresentation) aSDR = new StepShape_ShapeDefinitionRepresentation();
        StepRepr_RepresentedDefinition aRDefinition;
        aRDefinition.SetValue(aPD);
        aSDR->Init(aRDefinition, aSRWP);
        aModel->AddWithRefs(aPD);
        aModel->AddWithRefs(aSRWP);
        aModel->AddWithRefs(aSDR);
      }
    }
    // Link datum target to datum feature
    // if aSASeq.Length() == 0 nothing to do
    if (aSASeq.Length() == 1)
    {
      Handle(StepRepr_FeatureForDatumTargetRelationship) aFFDTR = new StepRepr_FeatureForDatumTargetRelationship();
      aFFDTR->Init(new TCollection_HAsciiString(), Standard_False, NULL, aSASeq.Value(1), aDatumTarget);
      aModel->AddWithRefs(aFFDTR);
    }
    else if (aSASeq.Length() > 1)
    {
      Handle(StepRepr_CompositeShapeAspect) aCompSA = new StepRepr_CompositeShapeAspect();
      aCompSA->Init(new TCollection_HAsciiString(), new TCollection_HAsciiString(), aPDS, aSASeq.Value(1)->ProductDefinitional());
      for (Standard_Integer i = 1; i <= aSASeq.Length(); i++)
      {
        Handle(StepRepr_ShapeAspectRelationship) aSAR = new StepRepr_ShapeAspectRelationship();
        aSAR->Init(new TCollection_HAsciiString(), Standard_False, new TCollection_HAsciiString(), aCompSA, aSASeq.Value(i));
        aModel->AddWithRefs(aSAR);
      }
      Handle(StepRepr_FeatureForDatumTargetRelationship) aFFDTR = new StepRepr_FeatureForDatumTargetRelationship();
      aFFDTR->Init(new TCollection_HAsciiString(), Standard_False, NULL, aCompSA, aDatumTarget);
      aModel->AddWithRefs(aFFDTR);
    }
    aSA = aDatumTarget;
  }

  // Datum
  Handle(StepDimTol_Datum) aDatum = theWrittenDatum;
  if (theIsFirstDTarget)
  {
    aDatum = new StepDimTol_Datum();
    aDatum->Init(new TCollection_HAsciiString(), new TCollection_HAsciiString(), aPDS, StepData_LFalse, anIdentifier);
    aModel->AddWithRefs(aDatum);
  }

  // Shape_Aspect_Relationship
  if (!aSA.IsNull())
  {
    Handle(StepRepr_ShapeAspectRelationship) aSAR = new StepRepr_ShapeAspectRelationship();
    aSAR->Init(new TCollection_HAsciiString(), Standard_False, NULL, aSA, aDatum);
    aModel->AddWithRefs(aSAR);
  }

  //Annotation plane and Presentation
  writePresentation(theWS, anObject->GetPresentation(), anObject->GetPresentationName(), Standard_True, anObject->HasPlane(),
                    anObject->GetPlane(), anObject->GetPointTextAttach(), aSA, theLocalFactors);

  return aDatum;
}

//=======================================================================
//function : WriteDimValues
//purpose  : auxiliary (write all data for given dimension: values,
//           qualifiers, modifiers, orientation and tolerance class)
//======================================================================
static void WriteDimValues(const Handle(XSControl_WorkSession)& theWS,
                           const Handle(XCAFDimTolObjects_DimensionObject)& theObject,
                           const Handle(StepRepr_RepresentationContext)& theRC,
                           const StepShape_DimensionalCharacteristic& theDimension,
                           const StepData_Factors& theLocalFactors)
{
  // Get working data
  const Handle(Interface_InterfaceModel)& aModel = theWS->Model();
  XCAFDimTolObjects_DimensionModifiersSequence aModifiers = theObject->GetModifiers();
  const Handle(Standard_Transient)& aDim = theDimension.Value();
  Standard_Boolean isAngle = aDim->IsKind(STANDARD_TYPE(StepShape_AngularLocation)) ||
    aDim->IsKind(STANDARD_TYPE(StepShape_AngularSize));

  // Unit
  StepBasic_Unit aUnit = GetUnit(theRC, isAngle);
  Standard_CString aMeasureName;
  if (isAngle)
    aMeasureName = "POSITIVE_PLANE_ANGLE_MEASURE";
  else
    aMeasureName = "POSITIVE_LENGTH_MEASURE";

  // Values
  Handle(StepRepr_HArray1OfRepresentationItem) aValues;
  Standard_Integer aNbItems = 1, aValIt = 1;
  if (theObject->IsDimWithRange())
    aNbItems += 2;
  if (aModifiers.Length() > 0)
    aNbItems++;
  if (theObject->GetType() == XCAFDimTolObjects_DimensionType_Location_Oriented)
    aNbItems++;
  aNbItems += theObject->NbDescriptions();
  aValues = new StepRepr_HArray1OfRepresentationItem(1, aNbItems);

  // Nominal value
  Standard_Real aNominal = theObject->GetValue();
  Standard_Integer aLeftNbDigits, aRightNbDigits;
  theObject->GetNbOfDecimalPlaces(aLeftNbDigits, aRightNbDigits);
  Standard_Integer aNbQualifiers = 0;
  if (theObject->HasQualifier() && !isAngle)
    aNbQualifiers++;
  if (aLeftNbDigits > 0 || aRightNbDigits > 0)
    aNbQualifiers++;
  // With qualifiers
  if (aNbQualifiers > 0)
  {
    Handle(StepShape_QualifiedRepresentationItem) aQRI = new StepShape_QualifiedRepresentationItem();
    Handle(StepShape_HArray1OfValueQualifier) aQualifiers = new StepShape_HArray1OfValueQualifier(1, aNbQualifiers);
    // Type qualifier
    if (theObject->HasQualifier() && !isAngle)
    {
      StepShape_ValueQualifier anItem;
      Handle(StepShape_TypeQualifier) aType = new StepShape_TypeQualifier();
      XCAFDimTolObjects_DimensionQualifier aQualifier = theObject->GetQualifier();
      aType->Init(STEPCAFControl_GDTProperty::GetDimQualifierName(aQualifier));
      aModel->AddWithRefs(aType);
      anItem.SetValue(aType);
      aQualifiers->SetValue(1, anItem);
    }
    // Number of decimal places
    if (aLeftNbDigits > 0 || aRightNbDigits > 0)
    {
      StepShape_ValueQualifier anItem;
      Handle(StepShape_ValueFormatTypeQualifier) aType = new StepShape_ValueFormatTypeQualifier();
      Handle(TCollection_HAsciiString) aFormatType = new TCollection_HAsciiString("NR2 ");
      aFormatType->AssignCat(new TCollection_HAsciiString(aLeftNbDigits));
      aFormatType->AssignCat(new TCollection_HAsciiString("."));
      aFormatType->AssignCat(new TCollection_HAsciiString(aRightNbDigits));
      aType->Init(aFormatType);
      aModel->AddWithRefs(aType);
      anItem.SetValue(aType);
      aQualifiers->SetValue(aNbQualifiers, anItem);
    }
    // Set qualifiers
    aQRI->SetQualifiers(aQualifiers);
    Handle(StepRepr_ReprItemAndMeasureWithUnit) anItem =
      CreateDimValue(aNominal, aUnit,
                     new TCollection_HAsciiString("nominal value"), aMeasureName, isAngle, Standard_True, aQRI);
    aValues->SetValue(aValIt, anItem);
    aValIt++;
  }
  // Without qualifiers
  else
  {
    Handle(StepRepr_ReprItemAndMeasureWithUnit) anItem =
      CreateDimValue(aNominal, aUnit,
                     new TCollection_HAsciiString("nominal value"), aMeasureName, isAngle);
    aValues->SetValue(aValIt, anItem);
    aValIt++;
  }

  // Ranges
  if (theObject->IsDimWithRange())
  {
    Handle(StepRepr_ReprItemAndMeasureWithUnit) aLowerItem =
      CreateDimValue(theObject->GetLowerBound(), aUnit,
                     new TCollection_HAsciiString("lower limit"), aMeasureName, isAngle);
    Handle(StepRepr_ReprItemAndMeasureWithUnit) anUpperItem =
      CreateDimValue(theObject->GetUpperBound(), aUnit,
                     new TCollection_HAsciiString("upper limit"), aMeasureName, isAngle);
    aValues->SetValue(aValIt, aLowerItem);
    aValIt++;
    aValues->SetValue(aValIt, anUpperItem);
    aValIt++;
  }

  // Modifiers
  if (aModifiers.Length() > 0)
  {
    Handle(StepRepr_CompoundRepresentationItem) aCompoundRI = new StepRepr_CompoundRepresentationItem();
    Handle(StepRepr_HArray1OfRepresentationItem) aModifItems =
      new StepRepr_HArray1OfRepresentationItem(1, aModifiers.Length());
    for (Standard_Integer i = 1; i <= aModifiers.Length(); i++)
    {
      XCAFDimTolObjects_DimensionModif aModif = aModifiers.Value(i);
      Handle(StepRepr_DescriptiveRepresentationItem) aModifItem =
        new StepRepr_DescriptiveRepresentationItem();
      aModifItem->Init(new TCollection_HAsciiString(), STEPCAFControl_GDTProperty::GetDimModifierName(aModif));
      aModel->AddWithRefs(aModifItem);
      aModifItems->SetValue(i, aModifItem);
    }
    aCompoundRI->Init(new TCollection_HAsciiString(), aModifItems);
    aValues->SetValue(aValIt, aCompoundRI);
    aValIt++;
  }

  // Orientation
  if (theObject->GetType() == XCAFDimTolObjects_DimensionType_Location_Oriented)
  {
    Handle(StepGeom_Axis2Placement3d) anOrientation = new StepGeom_Axis2Placement3d();
    gp_Dir aDir;
    theObject->GetDirection(aDir);
    GeomToStep_MakeCartesianPoint MkPoint(gp_Pnt(0, 0, 0), theLocalFactors.LengthFactor());
    const Handle(StepGeom_CartesianPoint)& aLoc = MkPoint.Value();
    Handle(StepGeom_Direction) anAxis = new StepGeom_Direction();
    Handle(TColStd_HArray1OfReal) aCoords = new TColStd_HArray1OfReal(1, 3);
    aCoords->SetValue(1, aDir.X());
    aCoords->SetValue(2, aDir.Y());
    aCoords->SetValue(3, aDir.Z());
    anAxis->Init(new TCollection_HAsciiString(), aCoords);
    anOrientation->Init(new TCollection_HAsciiString("orientation"), aLoc, Standard_True, anAxis, Standard_False, NULL);
    aValues->SetValue(aValIt, anOrientation);
    aValIt++;
  }

  // Descriptions
  if (theObject->HasDescriptions())
  {
    for (Standard_Integer i = 0; i < theObject->NbDescriptions(); i++)
    {
      Handle(StepRepr_DescriptiveRepresentationItem) aDRI = new StepRepr_DescriptiveRepresentationItem();
      aDRI->Init(theObject->GetDescriptionName(i), theObject->GetDescription(i));
      aValues->SetValue(aValIt, aDRI);
      aValIt++;
    }
  }

  for (Standard_Integer i = 1; i <= aValues->Length(); i++)
  {
    aModel->AddWithRefs(aValues->Value(i));
  }

  // Create resulting Shape_Dimension_Representation
  Handle(StepShape_ShapeDimensionRepresentation) aSDR = new StepShape_ShapeDimensionRepresentation();
  aSDR->Init(new TCollection_HAsciiString(), aValues, theRC);
  aModel->AddWithRefs(aSDR);
  Handle(StepShape_DimensionalCharacteristicRepresentation) aDCR = new StepShape_DimensionalCharacteristicRepresentation();
  aDCR->Init(theDimension, aSDR);
  aModel->AddWithRefs(aDCR);

  // Plus_Minus_Tolerance
  if (theObject->IsDimWithPlusMinusTolerance())
  {
    Handle(TCollection_HAsciiString) aDummyName = new TCollection_HAsciiString(aMeasureName);
    aDummyName = aDummyName->SubString(9, aDummyName->Length()); //delete "POSITIVE_"
    aMeasureName = aDummyName->ToCString();
    Standard_Real aLowerTolValue = -theObject->GetLowerTolValue(),
      anUpperTolValue = theObject->GetUpperTolValue();
    // Upper
    Handle(StepBasic_MeasureWithUnit) anUpperMWU = new StepBasic_MeasureWithUnit();
    Handle(StepBasic_MeasureValueMember) anUpperValue = new StepBasic_MeasureValueMember();
    anUpperValue->SetName(aMeasureName);
    anUpperValue->SetReal(anUpperTolValue);
    anUpperMWU->Init(anUpperValue, aUnit);
    aModel->AddWithRefs(anUpperMWU);
    // Lower
    Handle(StepBasic_MeasureWithUnit) aLowerMWU = new StepBasic_MeasureWithUnit();
    Handle(StepBasic_MeasureValueMember) aLowerValue = new StepBasic_MeasureValueMember();
    aLowerValue->SetName(aMeasureName);
    aLowerValue->SetReal(aLowerTolValue);
    aLowerMWU->Init(aLowerValue, aUnit);
    aModel->AddWithRefs(aLowerMWU);
    // Tolerance
    Handle(StepShape_ToleranceValue) aTolValue = new StepShape_ToleranceValue();
    aTolValue->Init(aLowerMWU, anUpperMWU);
    aModel->AddWithRefs(aTolValue);
    StepShape_ToleranceMethodDefinition aMethod;
    aMethod.SetValue(aTolValue);
    Handle(StepShape_PlusMinusTolerance) aPlusMinusTol = new StepShape_PlusMinusTolerance();
    aPlusMinusTol->Init(aMethod, theDimension);
    aModel->AddWithRefs(aPlusMinusTol);
  }
  // Tolerance class
  if (theObject->IsDimWithClassOfTolerance())
  {
    Standard_Boolean isHole;
    XCAFDimTolObjects_DimensionFormVariance aFormVariance;
    XCAFDimTolObjects_DimensionGrade aGrade;
    if (!theObject->GetClassOfTolerance(isHole, aFormVariance, aGrade))
      return;
    Handle(StepShape_LimitsAndFits) aLAF = STEPCAFControl_GDTProperty::GetLimitsAndFits(isHole, aFormVariance, aGrade);
    aModel->AddWithRefs(aLAF);
    StepShape_ToleranceMethodDefinition aMethod;
    aMethod.SetValue(aLAF);
    Handle(StepShape_PlusMinusTolerance) aPlusMinusTol = new StepShape_PlusMinusTolerance();
    aPlusMinusTol->Init(aMethod, theDimension);
    aModel->AddWithRefs(aPlusMinusTol);
  }
}

//=======================================================================
//function : WriteDerivedGeometry
//purpose  : auxiliary (write connection point for dimensions)
//======================================================================
static void WriteDerivedGeometry(const Handle(XSControl_WorkSession)& theWS,
                                 const Handle(XCAFDimTolObjects_DimensionObject)& theObject,
                                 const Handle(StepRepr_ConstructiveGeometryRepresentation)& theRepr,
                                 Handle(StepRepr_ShapeAspect)& theFirstSA,
                                 Handle(StepRepr_ShapeAspect)& theSecondSA,
                                 NCollection_Vector<Handle(StepGeom_CartesianPoint)>& thePnts,
                                 const StepData_Factors& theLocalFactors)
{
  const Handle(Interface_InterfaceModel)& aModel = theWS->Model();
  // First point
  if (theObject->HasPoint())
  {
    GeomToStep_MakeCartesianPoint aPointMaker(theObject->GetPoint(), theLocalFactors.LengthFactor());
    Handle(StepGeom_CartesianPoint) aPoint = aPointMaker.Value();
    thePnts.Append(aPoint);
    Handle(StepRepr_DerivedShapeAspect) aDSA = new StepRepr_DerivedShapeAspect();
    aDSA->Init(new TCollection_HAsciiString(), new TCollection_HAsciiString(), theFirstSA->OfShape(), StepData_LFalse);
    Handle(StepAP242_GeometricItemSpecificUsage) aGISU = new StepAP242_GeometricItemSpecificUsage();
    StepAP242_ItemIdentifiedRepresentationUsageDefinition aDefinition;
    aDefinition.SetValue(aDSA);
    Handle(StepRepr_HArray1OfRepresentationItem) anItem = new StepRepr_HArray1OfRepresentationItem(1, 1);
    anItem->SetValue(1, aPoint);
    aGISU->Init(new TCollection_HAsciiString(), new TCollection_HAsciiString(), aDefinition, theRepr, anItem);
    Handle(StepRepr_ShapeAspectDerivingRelationship) aSADR = new StepRepr_ShapeAspectDerivingRelationship();
    aSADR->Init(new TCollection_HAsciiString(), Standard_False, new TCollection_HAsciiString(), aDSA, theFirstSA);
    theFirstSA = aDSA;
    aModel->AddWithRefs(aGISU);
    aModel->AddWithRefs(aSADR);
  }

  // Second point (for locations)
  if (theObject->HasPoint2())
  {
    GeomToStep_MakeCartesianPoint aPointMaker(theObject->GetPoint2(), theLocalFactors.LengthFactor());
    Handle(StepGeom_CartesianPoint) aPoint = aPointMaker.Value();
    thePnts.Append(aPoint);
    Handle(StepRepr_DerivedShapeAspect) aDSA = new StepRepr_DerivedShapeAspect();
    aDSA->Init(new TCollection_HAsciiString(), new TCollection_HAsciiString(), theFirstSA->OfShape(), StepData_LFalse);
    Handle(StepAP242_GeometricItemSpecificUsage) aGISU = new StepAP242_GeometricItemSpecificUsage();
    StepAP242_ItemIdentifiedRepresentationUsageDefinition aDefinition;
    aDefinition.SetValue(aDSA);
    Handle(StepRepr_HArray1OfRepresentationItem) anItem = new StepRepr_HArray1OfRepresentationItem(1, 1);
    anItem->SetValue(1, aPoint);
    aGISU->Init(new TCollection_HAsciiString(), new TCollection_HAsciiString(), aDefinition, theRepr, anItem);
    Handle(StepRepr_ShapeAspectDerivingRelationship) aSADR = new StepRepr_ShapeAspectDerivingRelationship();
    aSADR->Init(new TCollection_HAsciiString(), Standard_False, new TCollection_HAsciiString(), aDSA, theSecondSA);
    theSecondSA = aDSA;
    aModel->AddWithRefs(aGISU);
    aModel->AddWithRefs(aSADR);
  }
}

//=======================================================================
//function : WriteDatumSystem
//purpose  : auxiliary (write Write datum system for given
//           geometric_tolerance)
//======================================================================
static Handle(StepDimTol_HArray1OfDatumSystemOrReference) WriteDatumSystem(const Handle(XSControl_WorkSession)& theWS,
                                                                           const TDF_Label theGeomTolL,
                                                                           const TDF_LabelSequence& theDatumSeq,
                                                                           const STEPConstruct_DataMapOfAsciiStringTransient& theDatumMap,
                                                                           const Handle(StepRepr_RepresentationContext)& theRC,
                                                                           const StepData_Factors& theLocalFactors)
{
  // Get working data
  const Handle(Interface_InterfaceModel)& aModel = theWS->Model();
  const Handle(Interface_HGraph) aHGraph = theWS->HGraph();
  if (aHGraph.IsNull())
    return NULL;
  Interface_Graph aGraph = aHGraph->Graph();
  Handle(XCAFDoc_GeomTolerance) aGTAttr;
  if (!theGeomTolL.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aGTAttr))
    return NULL;
  Handle(XCAFDimTolObjects_GeomToleranceObject) anObject = aGTAttr->GetObject();
  if (anObject.IsNull())
    return NULL;

  // Unit
  StepBasic_Unit aUnit = GetUnit(theRC);

  XCAFDimTolObjects_DatumObjectSequence aDatums;
  Standard_Integer aMaxDatumNum = 0;

  for (TDF_LabelSequence::Iterator aDatumIter(theDatumSeq);
       aDatumIter.More(); aDatumIter.Next())
  {
    Handle(XCAFDoc_Datum) aDatumAttr;
    if (!aDatumIter.Value().FindAttribute(XCAFDoc_Datum::GetID(), aDatumAttr))
      continue;
    Handle(XCAFDimTolObjects_DatumObject) aDatumObj = aDatumAttr->GetObject();
    if (aDatumObj.IsNull())
      continue;
    aDatums.Append(aDatumObj);
    aMaxDatumNum = Max(aMaxDatumNum, aDatumObj->GetPosition());
  }
  if (aMaxDatumNum == 0)
    return NULL;

  Handle(StepDimTol_HArray1OfDatumReferenceCompartment) aConstituents =
    new StepDimTol_HArray1OfDatumReferenceCompartment(1, aMaxDatumNum);
  // Auxiliary datum to initialize attributes in Datum_System
  Handle(StepDimTol_Datum) aFirstDatum;
  for (Standard_Integer aConstituentsNum = 1;
       aConstituentsNum <= aMaxDatumNum; aConstituentsNum++)
  {
    // Collect datums with i-th position
    XCAFDimTolObjects_DatumObjectSequence aDatumSeqPos;
    for (XCAFDimTolObjects_DatumObjectSequence::Iterator aDatumIter(aDatums);
         aDatumIter.More(); aDatumIter.Next())
    {
      if (aDatumIter.Value()->GetPosition() == aConstituentsNum)
      {
        aDatumSeqPos.Append(aDatumIter.Value());
      }
    }
    if (aDatumSeqPos.Length() < 1)
      continue;
    // Initialize Datum_Reference_Compartment
    StepDimTol_DatumOrCommonDatum aDatumRef;
    Handle(StepDimTol_DatumReferenceCompartment) aCompartment =
      new StepDimTol_DatumReferenceCompartment();
    Handle(StepDimTol_HArray1OfDatumReferenceModifier) aModifiers;
    if (aDatumSeqPos.Length() == 1)
    {
      // Datum entity
      Handle(Standard_Transient) aFDValue;
      if (theDatumMap.Find(aDatumSeqPos.Value(1)->GetName()->String(), aFDValue) && !aFDValue.IsNull())
        aFirstDatum = Handle(StepDimTol_Datum)::DownCast(aFDValue);
      aDatumRef.SetValue(aFirstDatum);
      // Modifiers
      XCAFDimTolObjects_DatumModifiersSequence aSimpleModifiers = aDatumSeqPos.Value(1)->GetModifiers();
      XCAFDimTolObjects_DatumModifWithValue aModifWithVal;
      Standard_Real aValue = 0;
      aDatumSeqPos.Value(1)->GetModifierWithValue(aModifWithVal, aValue);
      aModifiers = STEPCAFControl_GDTProperty::GetDatumRefModifiers(aSimpleModifiers, aModifWithVal, aValue, aUnit);
      // Add Datum_Reference_Modifier_With_Value
      if (!aModifiers.IsNull())
      {
        Handle(StepDimTol_DatumReferenceModifierWithValue) aDRMWV =
          aModifiers->Value(aModifiers->Length()).DatumReferenceModifierWithValue();
        if (!aDRMWV.IsNull())
        {
          aModel->AddWithRefs(aDRMWV);
        }
      }
    }
    else
    {
      Standard_Integer aSetInd = 1;
      Handle(StepDimTol_HArray1OfDatumReferenceElement) aCommonDatumList =
        new StepDimTol_HArray1OfDatumReferenceElement(1, aDatumSeqPos.Length());
      for (XCAFDimTolObjects_DatumObjectSequence::Iterator aDatumIter(aDatumSeqPos);
           aDatumIter.More(); aDatumIter.Next())
      {
        // Datum entity
        const Handle(XCAFDimTolObjects_DatumObject)& aDatumObj = aDatumIter.Value();
        Handle(StepDimTol_Datum) aDatum;
        Handle(Standard_Transient) aDValue;
        if (theDatumMap.Find(aDatumObj->GetName()->String(), aDValue))
          aDatum = Handle(StepDimTol_Datum)::DownCast(aDValue);
        StepDimTol_DatumOrCommonDatum anElemDatumRef;
        anElemDatumRef.SetValue(aDatum);
        if (aFirstDatum.IsNull())
          aFirstDatum = aDatum;
        // Modifiers
        XCAFDimTolObjects_DatumModifiersSequence aSimpleModifiers = aDatumObj->GetModifiers();
        XCAFDimTolObjects_DatumModifWithValue aModifWithVal;
        Standard_Real aValue = 0;
        aDatumObj->GetModifierWithValue(aModifWithVal, aValue);
        Handle(StepDimTol_HArray1OfDatumReferenceModifier) anElemModifiers =
          STEPCAFControl_GDTProperty::GetDatumRefModifiers(aSimpleModifiers, aModifWithVal, aValue, aUnit);
        // Add Datum_Reference_Modifier_With_Value
        if (!anElemModifiers.IsNull())
        {
          Handle(StepDimTol_DatumReferenceModifierWithValue) aDRMWV =
            anElemModifiers->Value(anElemModifiers->Length()).DatumReferenceModifierWithValue();
          if (!aDRMWV.IsNull())
          {
            aModel->AddWithRefs(aDRMWV);
          }
        }
        // Datum_Reference_Element
        Handle(StepDimTol_DatumReferenceElement) anElement = new StepDimTol_DatumReferenceElement();
        anElement->Init(new TCollection_HAsciiString(), new TCollection_HAsciiString(), aDatum->OfShape(),
                        aDatum->ProductDefinitional(), anElemDatumRef, !anElemModifiers.IsNull(), anElemModifiers);
        aModel->AddWithRefs(anElement);
        aCommonDatumList->SetValue(aSetInd++, anElement);
      }
      aDatumRef.SetValue(aCommonDatumList);
    }
    aCompartment->Init(new TCollection_HAsciiString(), new TCollection_HAsciiString(), aFirstDatum->OfShape(),
                       aFirstDatum->ProductDefinitional(), aDatumRef, !aModifiers.IsNull(), aModifiers);
    aModel->AddWithRefs(aCompartment);
    aConstituents->SetValue(aConstituentsNum, aCompartment);
  }
  // Remove null elements from aConstituents
  Standard_Integer aNbConstituents = 0;
  for (StepDimTol_HArray1OfDatumReferenceCompartment::Iterator aConstituentIter(aConstituents->Array1());
       aConstituentIter.More(); aConstituentIter.Next())
  {
    if (!aConstituentIter.Value().IsNull())
    {
      aNbConstituents++;
    }
  }
  Handle(StepDimTol_HArray1OfDatumReferenceCompartment) aResConstituents =
    new StepDimTol_HArray1OfDatumReferenceCompartment(1, aNbConstituents);
  Standard_Integer aConstituentsIt = 0;
  for (StepDimTol_HArray1OfDatumReferenceCompartment::Iterator aConstituentIter(aConstituents->Array1());
       aConstituentIter.More(); aConstituentIter.Next())
  {
    if (!aConstituentIter.Value().IsNull())
    {
      aConstituentsIt++;
      aResConstituents->SetValue(aConstituentsIt, aConstituentIter.Value());
    }
  }

  Handle(StepDimTol_HArray1OfDatumSystemOrReference) aDatumSystem;
  Handle(StepDimTol_DatumSystem) aDS = new StepDimTol_DatumSystem();
  aDS->Init(new TCollection_HAsciiString(), new TCollection_HAsciiString(), aFirstDatum->OfShape(),
            aFirstDatum->ProductDefinitional(), aResConstituents);
  aModel->AddWithRefs(aDS);
  StepDimTol_DatumSystemOrReference anArrayValue;
  anArrayValue.SetValue(aDS);
  aDatumSystem = new StepDimTol_HArray1OfDatumSystemOrReference(1, 1);
  aDatumSystem->SetValue(1, anArrayValue);

  // Axis
  if (anObject->HasAxis())
  {
    GeomToStep_MakeAxis2Placement3d anAxisMaker(anObject->GetAxis(), theLocalFactors);
    Handle(StepGeom_Axis2Placement3d) anAxis = anAxisMaker.Value();
    anAxis->SetName(new TCollection_HAsciiString("orientation"));
    Handle(StepAP242_GeometricItemSpecificUsage) aGISU = new StepAP242_GeometricItemSpecificUsage();
    StepAP242_ItemIdentifiedRepresentationUsageDefinition aDefinition;
    aDefinition.SetValue(aDS);
    Handle(StepRepr_HArray1OfRepresentationItem) anReprItems = new StepRepr_HArray1OfRepresentationItem(1, 1);
    Handle(StepRepr_RepresentationItem) anIdentifiedItem = anAxis;
    anReprItems->SetValue(1, anIdentifiedItem);
    Handle(StepShape_ShapeDefinitionRepresentation) aSDR;
    for (Interface_EntityIterator aSharingsIter = aGraph.Sharings(aFirstDatum->OfShape());
         aSharingsIter.More() && aSDR.IsNull(); aSharingsIter.Next())
    {
      aSDR = Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(aSharingsIter.Value());
    }
    if (aSDR.IsNull())
      return aDatumSystem;

    aGISU->Init(new TCollection_HAsciiString(), new TCollection_HAsciiString(),
                aDefinition, aSDR->UsedRepresentation(), anReprItems);
    aModel->AddWithRefs(anAxis);
    aModel->AddWithRefs(aGISU);
  }

  return aDatumSystem;
}

//=======================================================================
//function : writeToleranceZone
//purpose  : auxiliary (write tolerace zones)
//=======================================================================
void STEPCAFControl_Writer::writeToleranceZone(const Handle(XSControl_WorkSession)& theWS,
                                               const Handle(XCAFDimTolObjects_GeomToleranceObject)& theObject,
                                               const Handle(StepDimTol_GeometricTolerance)& theEntity,
                                               const Handle(StepRepr_RepresentationContext)& theRC)
{
  // Get working data
  const Handle(Interface_InterfaceModel)& aModel = theWS->Model();
  if (theEntity.IsNull() || theObject.IsNull())
    return;

  // Return if there is no tolerance zones
  if (theObject->GetTypeOfValue() == XCAFDimTolObjects_GeomToleranceTypeValue_None &&
      theObject->GetZoneModifier() != XCAFDimTolObjects_GeomToleranceZoneModif_Runout)
  {
    return;
  }

  // Create Tolerance_Zone
  Handle(StepDimTol_ToleranceZoneForm) aForm = new StepDimTol_ToleranceZoneForm();
  aModel->AddWithRefs(aForm);
  aForm->Init(STEPCAFControl_GDTProperty::GetTolValueType(theObject->GetTypeOfValue()));
  Handle(StepDimTol_HArray1OfToleranceZoneTarget) aZoneTargetArray = new StepDimTol_HArray1OfToleranceZoneTarget(1, 1);
  StepDimTol_ToleranceZoneTarget aTarget;
  aTarget.SetValue(theEntity);
  aZoneTargetArray->SetValue(1, aTarget);
  Handle(StepDimTol_ToleranceZone) aZone = new StepDimTol_ToleranceZone();
  aZone->Init(new TCollection_HAsciiString(), new TCollection_HAsciiString(),
              theEntity->TolerancedShapeAspect().ShapeAspect()->OfShape(), StepData_LFalse,
              aZoneTargetArray, aForm);
  aModel->AddWithRefs(aZone);

  // Runout_Tolerance_Zone
  Handle(StepBasic_PlaneAngleMeasureWithUnit) aPAMWU = new StepBasic_PlaneAngleMeasureWithUnit();
  Handle(StepBasic_MeasureValueMember) aValueMember = new StepBasic_MeasureValueMember();
  aValueMember->SetName("PLANE_ANGLE_MEASURE");
  aValueMember->SetReal(theObject->GetValueOfZoneModifier());
  aPAMWU->Init(aValueMember, GetUnit(theRC, Standard_True));
  Handle(StepDimTol_RunoutZoneOrientation) anOrientation = new StepDimTol_RunoutZoneOrientation();
  anOrientation->Init(aPAMWU);
  Handle(StepDimTol_RunoutZoneDefinition) aDefinition = new StepDimTol_RunoutZoneDefinition();
  aDefinition->Init(aZone, NULL, anOrientation);
  aModel->AddWithRefs(aDefinition);
  aModel->AddWithRefs(anOrientation);
  aModel->AddWithRefs(aPAMWU);
}

//=======================================================================
//function : writeGeomTolerance
//purpose  : auxiliary (write Geometric_Tolerance entity for given shapes,
//           label and datum system)
//======================================================================
void STEPCAFControl_Writer::writeGeomTolerance(const Handle(XSControl_WorkSession)& theWS,
                                               const TDF_LabelSequence& theShapeSeqL,
                                               const TDF_Label& theGeomTolL,
                                               const Handle(StepDimTol_HArray1OfDatumSystemOrReference)& theDatumSystem,
                                               const Handle(StepRepr_RepresentationContext)& theRC,
                                               const StepData_Factors& theLocalFactors)
{
  // Get working data
  const Handle(Interface_InterfaceModel)& aModel = theWS->Model();
  Handle(XCAFDoc_GeomTolerance) aGTAttr;
  if (!theGeomTolL.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aGTAttr))
    return;
  Handle(XCAFDimTolObjects_GeomToleranceObject) anObject = aGTAttr->GetObject();
  if (anObject.IsNull())
    return;

  // Value
  Handle(StepBasic_LengthMeasureWithUnit) aLMWU = new StepBasic_LengthMeasureWithUnit();
  StepBasic_Unit aUnit = GetUnit(theRC);
  Handle(StepBasic_MeasureValueMember) aValueMember = new StepBasic_MeasureValueMember();
  aValueMember->SetName("LENGTH_MEASURE");
  aValueMember->SetReal(anObject->GetValue());
  aLMWU->Init(aValueMember, aUnit);
  aModel->AddWithRefs(aLMWU);

  // Geometric_Tolerance target
  Handle(StepRepr_ShapeAspect) aMainSA;
  Handle(StepRepr_RepresentationContext) dummyRC;
  Handle(StepAP242_GeometricItemSpecificUsage) dummyGISU;
  if (theShapeSeqL.Length() == 1)
  {
    TopoDS_Shape aShape = XCAFDoc_ShapeTool::GetShape(theShapeSeqL.Value(1));
    aMainSA = writeShapeAspect(theWS, theGeomTolL, aShape, dummyRC, dummyGISU);
    aModel->AddWithRefs(aMainSA);
  }
  else
  {
    Handle(StepRepr_CompositeShapeAspect) aCSA;
    for (TDF_LabelSequence::Iterator aShIter(theShapeSeqL);
         aShIter.More(); aShIter.Next())
    {
      TopoDS_Shape aShape = XCAFDoc_ShapeTool::GetShape(aShIter.Value());
      Handle(StepRepr_ShapeAspect) aSA = writeShapeAspect(theWS, theGeomTolL, aShape, dummyRC, dummyGISU);
      if (aSA.IsNull())
        continue;
      if (aCSA.IsNull())
      {
        aCSA = new StepRepr_CompositeShapeAspect();
        aCSA->Init(aSA->Name(), aSA->Description(), aSA->OfShape(), aSA->ProductDefinitional());
        aModel->AddWithRefs(aCSA);
      }
      Handle(StepRepr_ShapeAspectRelationship) aSAR = new StepRepr_ShapeAspectRelationship();
      aSAR->Init(new TCollection_HAsciiString(), Standard_False, NULL, aCSA, aSA);
      aModel->AddWithRefs(aSAR);
    }
    aMainSA = aCSA;
  }
  StepDimTol_GeometricToleranceTarget aGTTarget;
  aGTTarget.SetValue(aMainSA);

  Standard_Boolean isWithModif = Standard_False,
    isWithDatRef = Standard_False,
    isWithMaxTol = Standard_False;
  // Modifiers
  // Simple modifiers
  XCAFDimTolObjects_GeomToleranceModifiersSequence aModifiers = anObject->GetModifiers();
  Handle(StepDimTol_HArray1OfGeometricToleranceModifier) aModifArray;
  Handle(StepBasic_LengthMeasureWithUnit) aMaxLMWU;
  Standard_Integer aModifNb = aModifiers.Length();
  if (anObject->GetMaterialRequirementModifier() != XCAFDimTolObjects_GeomToleranceMatReqModif_None)
    aModifNb++;
  for (Standard_Integer i = 1; i <= aModifiers.Length(); i++)
  {
    if (aModifiers.Value(i) == XCAFDimTolObjects_GeomToleranceModif_All_Around ||
        aModifiers.Value(i) == XCAFDimTolObjects_GeomToleranceModif_All_Over)
    {
      aModifNb--;
    }
  }
  if (aModifNb > 0)
  {
    isWithModif = Standard_True;
    aModifArray = new StepDimTol_HArray1OfGeometricToleranceModifier(1, aModifNb);
    Standard_Integer k = 1;
    for (Standard_Integer i = 1; i <= aModifiers.Length(); i++)
    {
      if (aModifiers.Value(i) == XCAFDimTolObjects_GeomToleranceModif_All_Around ||
          aModifiers.Value(i) == XCAFDimTolObjects_GeomToleranceModif_All_Over)
      {
        continue;
      }
      StepDimTol_GeometricToleranceModifier aModif =
        STEPCAFControl_GDTProperty::GetGeomToleranceModifier(aModifiers.Value(i));
      aModifArray->SetValue(k, aModif);
      k++;
    }
    if (anObject->GetMaterialRequirementModifier() == XCAFDimTolObjects_GeomToleranceMatReqModif_L)
    {
      aModifArray->SetValue(aModifNb, StepDimTol_GTMLeastMaterialRequirement);
    }
    else if (anObject->GetMaterialRequirementModifier() == XCAFDimTolObjects_GeomToleranceMatReqModif_M)
    {
      aModifArray->SetValue(aModifNb, StepDimTol_GTMMaximumMaterialRequirement);
    }
    // Modifier with value
    if (anObject->GetMaxValueModifier() != 0)
    {
      isWithMaxTol = Standard_True;
      aMaxLMWU = new StepBasic_LengthMeasureWithUnit();
      Handle(StepBasic_MeasureValueMember) aModifierValueMember = new StepBasic_MeasureValueMember();
      aModifierValueMember->SetName("LENGTH_MEASURE");
      aModifierValueMember->SetReal(anObject->GetMaxValueModifier());
      aMaxLMWU->Init(aModifierValueMember, aUnit);
      aModel->AddWithRefs(aMaxLMWU);
    }
  }

  // Datum Reference
  isWithDatRef = !theDatumSystem.IsNull();

  // Collect all attributes
  Handle(TCollection_HAsciiString) aName = new TCollection_HAsciiString(),
    aDescription = new TCollection_HAsciiString();
  Handle(StepDimTol_GeometricToleranceWithDatumReference) aGTWDR =
    new StepDimTol_GeometricToleranceWithDatumReference();
  aGTWDR->SetDatumSystem(theDatumSystem);
  Handle(StepDimTol_GeometricToleranceWithModifiers) aGTWM =
    new StepDimTol_GeometricToleranceWithModifiers();
  aGTWM->SetModifiers(aModifArray);
  StepDimTol_GeometricToleranceType aType =
    STEPCAFControl_GDTProperty::GetGeomToleranceType(anObject->GetType());

  // Init and write necessary subtype of Geometric_Tolerance entity
  Handle(StepDimTol_GeometricTolerance) aGeomTol;
  if (isWithModif)
  {
    if (isWithMaxTol)
    {
      if (isWithDatRef)
      {
        // Geometric_Tolerance & Geometric_Tolerance_With_Datum_Reference &
        // Geometric_Tolerance_With_Maximum_Tolerance & Geometric_Tolerance_With_Modifiers
        Handle(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol) aResult =
          new StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol();
        aResult->Init(aName, aDescription, aLMWU, aGTTarget, aGTWDR, aGTWM, aMaxLMWU, aType);
        aGeomTol = aResult;
      }
      else
      {
        // Geometric_Tolerance & Geometric_Tolerance_With_Maximum_Tolerance & Geometric_Tolerance_With_Modifiers
        Handle(StepDimTol_GeoTolAndGeoTolWthMaxTol) aResult =
          new StepDimTol_GeoTolAndGeoTolWthMaxTol();
        aResult->Init(aName, aDescription, aLMWU, aGTTarget, aGTWM, aMaxLMWU, aType);
        aGeomTol = aResult;
      }
    }
    else
    {
      if (isWithDatRef)
      {
        // Geometric_Tolerance & Geometric_Tolerance_With_Datum_Reference & Geometric_Tolerance_With_Modifiers
        Handle(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod) aResult =
          new StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod();
        aResult->Init(aName, aDescription, aLMWU, aGTTarget, aGTWDR, aGTWM, aType);
        aGeomTol = aResult;
      }
      else
      {
        // Geometric_Tolerance & Geometric_Tolerance_With_Modifiers
        Handle(StepDimTol_GeoTolAndGeoTolWthMod) aResult =
          new StepDimTol_GeoTolAndGeoTolWthMod();
        aResult->Init(aName, aDescription, aLMWU, aGTTarget, aGTWM, aType);
        aGeomTol = aResult;
      }
    }
  }
  else
  {
    if (isWithDatRef)
    {
      // Geometric_Tolerance & Geometric_Tolerance_With_Datum_Reference
      Handle(StepDimTol_GeoTolAndGeoTolWthDatRef) aResult =
        new StepDimTol_GeoTolAndGeoTolWthDatRef();
      aResult->Init(aName, aDescription, aLMWU, aGTTarget, aGTWDR, aType);
      aGeomTol = aResult;
    }
    else
    {
      // Geometric_Tolerance
      Handle(StepDimTol_GeometricTolerance) aResult =
        STEPCAFControl_GDTProperty::GetGeomTolerance(anObject->GetType());
      if (!aResult.IsNull())
      {
        aResult->Init(aName, aDescription, aLMWU, aGTTarget);
        aGeomTol = aResult;
      }
    }
  }
  aModel->AddWithRefs(aGeomTol);
  writeToleranceZone(theWS, anObject, aGeomTol, theRC);
  //Annotation plane and Presentation
  writePresentation(theWS, anObject->GetPresentation(), anObject->GetPresentationName(), Standard_True, anObject->HasPlane(),
                    anObject->GetPlane(), anObject->GetPointTextAttach(), aGeomTol, theLocalFactors);
}

//=======================================================================
//function : writeDGTs
//purpose  :
//=======================================================================
Standard_Boolean STEPCAFControl_Writer::writeDGTs(const Handle(XSControl_WorkSession)& theWS,
                                                  const TDF_LabelSequence& theLabels) const
{

  if (theLabels.IsEmpty())
    return Standard_False;

  // get working data
  const Handle(Interface_InterfaceModel)& aModel = theWS->Model();
  const Handle(XSControl_TransferWriter)& aTW = theWS->TransferWriter();
  const Handle(Transfer_FinderProcess)& aFP = aTW->FinderProcess();

  const Handle(Interface_HGraph) aHGraph = theWS->HGraph();
  if (aHGraph.IsNull())
    return Standard_False;

  Interface_Graph aGraph = aHGraph->Graph();
  STEPConstruct_DataMapOfAsciiStringTransient aDatumMap;

  TDF_LabelSequence aDGTLabels;
  // Iterate on requested shapes collect Tools
  for (TDF_LabelMap::Iterator aLabelIter(myRootLabels);
       aLabelIter.More(); aLabelIter.Next())
  {
    const TDF_Label& aLabel = aLabelIter.Value();
    Handle(XCAFDoc_DimTolTool) aDGTTool = XCAFDoc_DocumentTool::DimTolTool(aLabel);
    TDF_LabelSequence aDGTLS;
    aDGTTool->GetDatumLabels(aDGTLS);
    aDGTLabels.Append(aDGTLS);
  }

  if (aDGTLabels.IsEmpty())
    return Standard_False;

  for (TDF_LabelSequence::Iterator aDGTIter(aDGTLabels);
       aDGTIter.More(); aDGTIter.Next())
  {
    const TDF_Label& aDatumL = aDGTIter.Value();
    TDF_LabelSequence aShapeL;
    TDF_LabelSequence aNullSeq;
    if (!XCAFDoc_DimTolTool::GetRefShapeLabel(aDatumL, aShapeL, aNullSeq)) continue;
    // find target shape
    TopoDS_Shape aShape = XCAFDoc_ShapeTool::GetShape(aShapeL.Value(1));
    TopLoc_Location aLoc;
    TColStd_SequenceOfTransient aSeqRI;
    FindEntities(aFP, aShape, aLoc, aSeqRI);
    if (aSeqRI.IsEmpty())
    {
      Message::SendTrace() << "Warning: Cannot find RI for " << aShape.TShape()->DynamicType()->Name() << "\n";
      continue;
    }
    Handle(StepRepr_ProductDefinitionShape) aPDS;
    Handle(StepRepr_RepresentationContext) aRC;
    Handle(Standard_Transient) anEnt = aSeqRI.Value(1);
    Handle(StepShape_AdvancedFace) anAF;
    Handle(StepShape_EdgeCurve) anEC;
    FindPDSforDGT(aGraph, anEnt, aPDS, aRC, anAF, anEC);
    if (aPDS.IsNull())
      continue;
    Handle(XCAFDoc_Datum) aDatumAttr;
    if (!aDatumL.FindAttribute(XCAFDoc_Datum::GetID(), aDatumAttr))
      continue;
    Handle(TCollection_HAsciiString) aName = aDatumAttr->GetName();
    Handle(TCollection_HAsciiString) anIdentification = aDatumAttr->GetIdentification();
    Handle(TCollection_HAsciiString) aDescription = aDatumAttr->GetDescription();
    if (aDescription.IsNull())
    {
      aDescription = new TCollection_HAsciiString();
    }
    Handle(StepDimTol_DatumFeature) aDF = new StepDimTol_DatumFeature;
    Handle(StepDimTol_Datum) aDatum = new StepDimTol_Datum;
    aDF->Init(aName, new TCollection_HAsciiString, aPDS, StepData_LTrue);
    aModel->AddWithRefs(aDF);
    aDatum->Init(aName, new TCollection_HAsciiString, aPDS, StepData_LFalse, anIdentification);
    aModel->AddWithRefs(aDatum);
    Handle(StepRepr_ShapeAspectRelationship) aSAR = new StepRepr_ShapeAspectRelationship;
    aSAR->SetName(aName);
    aSAR->SetRelatingShapeAspect(aDF);
    aSAR->SetRelatedShapeAspect(aDatum);
    aModel->AddWithRefs(aSAR);
    // write chain for DatumFeature
    StepRepr_CharacterizedDefinition aCD;
    aCD.SetValue(aDF);
    Handle(StepRepr_PropertyDefinition) aPropD = new StepRepr_PropertyDefinition;
    aPropD->Init(aName, Standard_True, aDescription, aCD);
    aModel->AddWithRefs(aPropD);
    StepRepr_RepresentedDefinition aRD;
    aRD.SetValue(aPropD);
    Handle(StepShape_ShapeRepresentation) aSR = new StepShape_ShapeRepresentation;
    Handle(StepRepr_HArray1OfRepresentationItem) aHARI =
      new StepRepr_HArray1OfRepresentationItem(1, 1);
    aHARI->SetValue(1, anAF);
    aSR->Init(aName, aHARI, aRC);
    Handle(StepShape_ShapeDefinitionRepresentation) aSDR = new StepShape_ShapeDefinitionRepresentation;
    aSDR->Init(aRD, aSR);
    aModel->AddWithRefs(aSDR);
    // write chain for Datum
    StepRepr_CharacterizedDefinition aCD1;
    aCD1.SetValue(aDatum);
    Handle(StepRepr_PropertyDefinition) aPropD1 = new StepRepr_PropertyDefinition;
    aPropD1->Init(aName, Standard_True, aDescription, aCD1);
    aModel->AddWithRefs(aPropD1);
    StepRepr_RepresentedDefinition aRD1;
    aRD1.SetValue(aPropD1);
    Handle(StepShape_ShapeRepresentation) aSR1 = new StepShape_ShapeRepresentation;
    Handle(StepRepr_HArray1OfRepresentationItem) aHARI1 =
      new StepRepr_HArray1OfRepresentationItem(1, 1);
    aHARI1->SetValue(1, anAF->FaceGeometry());
    aSR1->Init(aName, aHARI1, aRC);
    aModel->AddWithRefs(aSR1);
    Handle(StepShape_ShapeDefinitionRepresentation) aSDR1 = new StepShape_ShapeDefinitionRepresentation;
    aSDR1->Init(aRD1, aSR1);
    aModel->AddWithRefs(aSDR1);
    // add created Datum into Map
    TCollection_AsciiString aStmp(aName->ToCString());
    aStmp.AssignCat(aDescription->ToCString());
    aStmp.AssignCat(anIdentification->ToCString());
    aDatumMap.Bind(aStmp, aDatum);
  }

  // write Tolerances and Dimensions
  aDGTLabels.Clear();
  // Iterate on requested shapes collect Tools
  for (TDF_LabelMap::Iterator aLabelIter(myRootLabels);
       aLabelIter.More(); aLabelIter.Next())
  {
    const TDF_Label& aLabel = aLabelIter.Value();
    Handle(XCAFDoc_DimTolTool) aDGTTool = XCAFDoc_DocumentTool::DimTolTool(aLabel);
    TDF_LabelSequence aaDGTLS;
    aDGTTool->GetDimTolLabels(aDGTLabels);
    aDGTLabels.Append(aaDGTLS);
  }

  if (aDGTLabels.IsEmpty())
    return Standard_False;
  for (TDF_LabelSequence::Iterator aDGTIter(aDGTLabels);
       aDGTIter.More(); aDGTIter.Next())
  {
    const TDF_Label& aDimTolL = aDGTIter.Value();
    TDF_LabelSequence aShapeL;
    TDF_LabelSequence aNullSeq;
    if (!XCAFDoc_DimTolTool::GetRefShapeLabel(aDimTolL, aShapeL, aNullSeq))
      continue;
    // find target shape
    TopoDS_Shape aShape = XCAFDoc_ShapeTool::GetShape(aShapeL.Value(1));
    TopLoc_Location aLoc;
    TColStd_SequenceOfTransient seqRI;
    FindEntities(aFP, aShape, aLoc, seqRI);
    if (seqRI.IsEmpty())
    {
      Message::SendTrace() << "Warning: Cannot find RI for " << aShape.TShape()->DynamicType()->Name() << "\n";
      continue;
    }
    Handle(StepRepr_ProductDefinitionShape) aPDS;
    Handle(StepRepr_RepresentationContext) aRC;
    Handle(Standard_Transient) anEnt = seqRI.Value(1);
    Handle(StepShape_AdvancedFace) anAF;
    Handle(StepShape_EdgeCurve) anEC;
    FindPDSforDGT(aGraph, anEnt, aPDS, aRC, anAF, anEC);
    if (aPDS.IsNull())
      continue;

    Handle(XCAFDoc_DimTol) aDimTolAttr;
    if (!aDimTolL.FindAttribute(XCAFDoc_DimTol::GetID(), aDimTolAttr))
      continue;
    Standard_Integer aKind = aDimTolAttr->GetKind();
    Handle(TColStd_HArray1OfReal) aVal = aDimTolAttr->GetVal();
    Handle(TCollection_HAsciiString) aName = aDimTolAttr->GetName();
    Handle(TCollection_HAsciiString) aDescription = aDimTolAttr->GetDescription();

    // common part of writing D&GT entities
    StepRepr_CharacterizedDefinition aCD;
    Handle(StepRepr_ShapeAspect) aSA = new StepRepr_ShapeAspect;
    aSA->Init(aName, new TCollection_HAsciiString, aPDS, StepData_LTrue);
    aModel->AddWithRefs(aSA);
    aCD.SetValue(aSA);
    Handle(StepRepr_PropertyDefinition) aPropD = new StepRepr_PropertyDefinition;
    aPropD->Init(aName, Standard_True, aDescription, aCD);
    aModel->AddWithRefs(aPropD);
    StepRepr_RepresentedDefinition aRD;
    aRD.SetValue(aPropD);
    Handle(StepShape_ShapeRepresentation) aSR = new StepShape_ShapeRepresentation;
    Handle(StepRepr_HArray1OfRepresentationItem) aHARI =
      new StepRepr_HArray1OfRepresentationItem(1, 1);
    if (aKind < 20)
      aHARI->SetValue(1, anEC);
    else
      aHARI->SetValue(1, anAF);
    aSR->Init(aName, aHARI, aRC);
    Handle(StepShape_ShapeDefinitionRepresentation) aSDR = new StepShape_ShapeDefinitionRepresentation;
    aSDR->Init(aRD, aSR);
    aModel->AddWithRefs(aSDR);
    // define aUnit for creation LengthMeasureWithUnit (common for all)
    StepBasic_Unit aUnit;
    aUnit = GetUnit(aRC);

    // specific part of writing D&GT entities
    if (aKind < 20)
    { //dimension
      Handle(StepShape_DimensionalSize) aDimSize = new StepShape_DimensionalSize;
      aDimSize->Init(aSA, aDescription);
      aModel->AddWithRefs(aDimSize);
      if (aVal->Length() > 1)
      {
        // create MeasureWithUnits
        Handle(StepBasic_MeasureValueMember) aMVM1 = new StepBasic_MeasureValueMember;
        aMVM1->SetName("POSITIVE_LENGTH_MEASURE");
        aMVM1->SetReal(aVal->Value(1));
        Handle(StepBasic_MeasureWithUnit) aMWU1 = new StepBasic_MeasureWithUnit;
        aMWU1->Init(aMVM1, aUnit);
        Handle(StepBasic_MeasureValueMember) aMVM2 = new StepBasic_MeasureValueMember;
        aMVM2->SetName("POSITIVE_LENGTH_MEASURE");
        aMVM2->SetReal(aVal->Value(2));
        Handle(StepBasic_MeasureWithUnit) aMWU2 = new StepBasic_MeasureWithUnit;
        aMWU2->Init(aMVM2, aUnit);
        Handle(StepRepr_RepresentationItem) aRI1 = new StepRepr_RepresentationItem;
        aRI1->Init(new TCollection_HAsciiString("lower limit"));
        Handle(StepRepr_RepresentationItem) aRI2 = new StepRepr_RepresentationItem;
        aRI2->Init(new TCollection_HAsciiString("upper limit"));
        Handle(StepRepr_ReprItemAndLengthMeasureWithUnit) aRILMU1 =
          new StepRepr_ReprItemAndLengthMeasureWithUnit;
        aRILMU1->Init(aMWU1, aRI1);
        Handle(StepRepr_ReprItemAndLengthMeasureWithUnit) aRILMU2 =
          new StepRepr_ReprItemAndLengthMeasureWithUnit;
        aRILMU2->Init(aMWU2, aRI2);
        aModel->AddWithRefs(aRILMU1);
        aModel->AddWithRefs(aRILMU2);
        Handle(StepRepr_HArray1OfRepresentationItem) aHARIVR =
          new StepRepr_HArray1OfRepresentationItem(1, 2);
        aHARIVR->SetValue(1, aRILMU1);
        aHARIVR->SetValue(2, aRILMU2);
        Handle(StepRepr_ValueRange) aVR = new StepRepr_ValueRange;
        //VR->Init(aName,CID);
        aVR->Init(aName, aHARIVR);
        aModel->AddEntity(aVR);
        Handle(StepShape_ShapeDimensionRepresentation) aSDimR =
          new StepShape_ShapeDimensionRepresentation;
        Handle(StepRepr_HArray1OfRepresentationItem) aHARI1 =
          new StepRepr_HArray1OfRepresentationItem(1, 1);
        aHARI1->SetValue(1, aVR);
        aSDimR->Init(aName, aHARI1, aRC);
        aModel->AddWithRefs(aSDimR);
        Handle(StepShape_DimensionalCharacteristicRepresentation) aDimCharR =
          new StepShape_DimensionalCharacteristicRepresentation;
        StepShape_DimensionalCharacteristic aDimChar;
        aDimChar.SetValue(aDimSize);
        aDimCharR->Init(aDimChar, aSDimR);
        aModel->AddEntity(aDimCharR);
      }
    }
    else if (aKind < 50)
    { //tolerance
      if (aKind < 35)
      { // tolerance with datum system
        TDF_LabelSequence aDatumLabels;
        XCAFDoc_DimTolTool::GetDatumOfTolerLabels(aDimTolL, aDatumLabels);
        Standard_Integer aSetDatumInd = 1;
        Handle(StepDimTol_HArray1OfDatumReference) aHADR =
          new StepDimTol_HArray1OfDatumReference(1, aDatumLabels.Length());
        for (TDF_LabelSequence::Iterator aDatumIter(aDatumLabels);
             aDatumIter.More(); aDatumIter.Next(), aSetDatumInd++)
        {
          Handle(XCAFDoc_Datum) aDatumAttr;
          const TDF_Label& aDatumL = aDatumIter.Value();
          if (!aDatumL.FindAttribute(XCAFDoc_Datum::GetID(), aDatumAttr))
            continue;
          Handle(TCollection_HAsciiString) aNameD = aDatumAttr->GetName();
          Handle(TCollection_HAsciiString) aDescriptionD = aDatumAttr->GetDescription();
          Handle(TCollection_HAsciiString) anIdentificationD = aDatumAttr->GetIdentification();
          TCollection_AsciiString aStmp(aNameD->ToCString());
          aStmp.AssignCat(aDescriptionD->ToCString());
          aStmp.AssignCat(anIdentificationD->ToCString());
          if (aDatumMap.IsBound(aStmp))
          {
            Handle(StepDimTol_Datum) aDatum =
              Handle(StepDimTol_Datum)::DownCast(aDatumMap.Find(aStmp));
            Handle(StepDimTol_DatumReference) aDR = new StepDimTol_DatumReference;
            aDR->Init(aSetDatumInd, aDatum);
            aModel->AddWithRefs(aDR);
            aHADR->SetValue(aSetDatumInd, aDR);
          }
        }
        // create LengthMeasureWithUnit
        Handle(StepBasic_MeasureValueMember) aMVM = new StepBasic_MeasureValueMember;
        aMVM->SetName("LENGTH_MEASURE");
        aMVM->SetReal(aVal->Value(1));
        Handle(StepBasic_LengthMeasureWithUnit) aLMWU = new StepBasic_LengthMeasureWithUnit;
        aLMWU->Init(aMVM, aUnit);
        // create tolerance by it's type
        if (aKind < 24)
        {
          Handle(StepDimTol_GeometricToleranceWithDatumReference) aGTWDR =
            new StepDimTol_GeometricToleranceWithDatumReference;
          aGTWDR->SetDatumSystem(aHADR);
          Handle(StepDimTol_ModifiedGeometricTolerance) aMGT =
            new StepDimTol_ModifiedGeometricTolerance;
          if (aKind == 21)
            aMGT->SetModifier(StepDimTol_MaximumMaterialCondition);
          else if (aKind == 22)
            aMGT->SetModifier(StepDimTol_LeastMaterialCondition);
          else if (aKind == 23)
            aMGT->SetModifier(StepDimTol_RegardlessOfFeatureSize);
          Handle(StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol) aGTComplex =
            new StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol;
          aGTComplex->Init(aName, aDescription, aLMWU, aSA, aGTWDR, aMGT);
          aModel->AddWithRefs(aGTComplex);
        }
        else if (aKind == 24)
        {
          Handle(StepDimTol_AngularityTolerance) aToler =
            new StepDimTol_AngularityTolerance;
          aToler->Init(aName, aDescription, aLMWU, aSA, aHADR);
          aModel->AddWithRefs(aToler);
        }
        else if (aKind == 25)
        {
          Handle(StepDimTol_CircularRunoutTolerance) aToler =
            new StepDimTol_CircularRunoutTolerance;
          aToler->Init(aName, aDescription, aLMWU, aSA, aHADR);
          aModel->AddWithRefs(aToler);
        }
        else if (aKind == 26)
        {
          Handle(StepDimTol_CoaxialityTolerance) aToler =
            new StepDimTol_CoaxialityTolerance;
          aToler->Init(aName, aDescription, aLMWU, aSA, aHADR);
          aModel->AddWithRefs(aToler);
        }
        else if (aKind == 27)
        {
          Handle(StepDimTol_ConcentricityTolerance) aToler =
            new StepDimTol_ConcentricityTolerance;
          aToler->Init(aName, aDescription, aLMWU, aSA, aHADR);
          aModel->AddWithRefs(aToler);
        }
        else if (aKind == 28)
        {
          Handle(StepDimTol_ParallelismTolerance) aToler =
            new StepDimTol_ParallelismTolerance;
          aToler->Init(aName, aDescription, aLMWU, aSA, aHADR);
          aModel->AddWithRefs(aToler);
        }
        else if (aKind == 29)
        {
          Handle(StepDimTol_PerpendicularityTolerance) aToler =
            new StepDimTol_PerpendicularityTolerance;
          aToler->Init(aName, aDescription, aLMWU, aSA, aHADR);
          aModel->AddWithRefs(aToler);
        }
        else if (aKind == 30)
        {
          Handle(StepDimTol_SymmetryTolerance) aToler =
            new StepDimTol_SymmetryTolerance;
          aToler->Init(aName, aDescription, aLMWU, aSA, aHADR);
          aModel->AddWithRefs(aToler);
        }
        else if (aKind == 31)
        {
          Handle(StepDimTol_TotalRunoutTolerance) aToler =
            new StepDimTol_TotalRunoutTolerance;
          aToler->Init(aName, aDescription, aLMWU, aSA, aHADR);
          aModel->AddWithRefs(aToler);
        }
      }
    }
  }

  return Standard_True;
}

//=======================================================================
//function : writeDGTsAP242
//purpose  :
//=======================================================================
Standard_Boolean STEPCAFControl_Writer::writeDGTsAP242(const Handle(XSControl_WorkSession)& theWS,
                                                       const TDF_LabelSequence& theLabels,
                                                       const StepData_Factors& theLocalFactors)
{
  (void)theLabels;
  // Get working data
  const Handle(Interface_InterfaceModel)& aModel = theWS->Model();

  const Handle(Interface_HGraph) aHGraph = theWS->HGraph();
  if (aHGraph.IsNull())
    return Standard_False;

  Interface_Graph aGraph = aHGraph->Graph();

  // Common entities for presentation
  STEPConstruct_Styles aStyles(theWS);
  Handle(StepVisual_Colour) aCurvColor = aStyles.EncodeColor(Quantity_NOC_WHITE);
  Handle(StepRepr_RepresentationItem) anItem = NULL;
  myGDTPrsCurveStyle->SetValue(1, aStyles.MakeColorPSA(anItem, aCurvColor, aCurvColor, aCurvColor, 0.0));
  for (Interface_EntityIterator aModelIter = aModel->Entities();
       aModelIter.More() && myGDTCommonPDS.IsNull(); aModelIter.Next())
  {
    myGDTCommonPDS = Handle(StepRepr_ProductDefinitionShape)::DownCast(aModelIter.Value());
  }

  STEPConstruct_DataMapOfAsciiStringTransient aDatumMap;
  Handle(StepRepr_RepresentationContext) aRC;

  TDF_LabelSequence aDGTLabels;
  // Iterate on requested shapes collect Tools
  for (TDF_LabelMap::Iterator aLabelIter(myRootLabels);
       aLabelIter.More(); aLabelIter.Next())
  {
    const TDF_Label& aLabel = aLabelIter.Value();
    Handle(XCAFDoc_DimTolTool) aDGTTool = XCAFDoc_DocumentTool::DimTolTool(aLabel);
    TDF_LabelSequence aaDGTLS;
    aDGTTool->GetDatumLabels(aDGTLabels);
    aDGTLabels.Append(aaDGTLS);
  }

  // Find all shapes with datums
  TColStd_MapOfAsciiString aNameIdMap;
  for (TDF_LabelSequence::Iterator aDGTIter(aDGTLabels);
       aDGTIter.More(); aDGTIter.Next())
  {
    const TDF_Label& aDatumL = aDGTIter.Value();
    TDF_LabelSequence aShapeL, aNullSeq;
    XCAFDoc_DimTolTool::GetRefShapeLabel(aDatumL, aShapeL, aNullSeq);
    Handle(XCAFDoc_Datum) aDatumAttr;
    aDatumL.FindAttribute(XCAFDoc_Datum::GetID(), aDatumAttr);
    Handle(XCAFDimTolObjects_DatumObject) anObject = aDatumAttr->GetObject();
    TCollection_AsciiString aDatumName = anObject->GetName()->String();
    TCollection_AsciiString aDatumTargetId = TCollection_AsciiString(anObject->GetDatumTargetNumber());
    if (!aNameIdMap.Add(aDatumName.Cat(aDatumTargetId)))
      continue;
    Handle(Standard_Transient) aWrittenDatum;
    Standard_Boolean isFirstDT = !aDatumMap.Find(aDatumName, aWrittenDatum);
    Handle(StepDimTol_Datum) aDatum = writeDatumAP242(theWS, aShapeL, aDatumL, isFirstDT,
                                                      Handle(StepDimTol_Datum)::DownCast(aWrittenDatum),
                                                      theLocalFactors);
    // Add created Datum into Map
    aDatumMap.Bind(aDatumName, aDatum);
  }

  // write Dimensions
  aDGTLabels.Clear();
  for (TDF_LabelMap::Iterator aLabelIter(myRootLabels);
       aLabelIter.More(); aLabelIter.Next())
  {
    const TDF_Label& aLabel = aLabelIter.Value();
    Handle(XCAFDoc_DimTolTool) aDGTTool = XCAFDoc_DocumentTool::DimTolTool(aLabel);
    TDF_LabelSequence aaDGTLS;
    aDGTTool->GetDimensionLabels(aDGTLabels);
    aDGTLabels.Append(aaDGTLS);
  }

  // Auxiliary entities for derived geometry
  Handle(StepRepr_ConstructiveGeometryRepresentation) aCGRepr =
    new StepRepr_ConstructiveGeometryRepresentation();
  Handle(StepRepr_ConstructiveGeometryRepresentationRelationship) aCGReprRel =
    new StepRepr_ConstructiveGeometryRepresentationRelationship();
  NCollection_Vector<Handle(StepGeom_CartesianPoint)> aConnectionPnts;
  Handle(StepRepr_RepresentationContext) dummyRC;
  Handle(StepAP242_GeometricItemSpecificUsage) dummyGISU;
  for (TDF_LabelSequence::Iterator aDGTIter(aDGTLabels);
       aDGTIter.More(); aDGTIter.Next())
  {
    const TDF_Label& aDimensionL = aDGTIter.Value();
    TDF_LabelSequence aFirstShapeL, aSecondShapeL;
    Handle(XCAFDoc_Dimension) aDimAttr;
    if (!aDimensionL.FindAttribute(XCAFDoc_Dimension::GetID(), aDimAttr))
      continue;
    Handle(XCAFDimTolObjects_DimensionObject) anObject = aDimAttr->GetObject();
    if (anObject.IsNull())
      continue;
    if (anObject->GetType() == XCAFDimTolObjects_DimensionType_CommonLabel)
    {
      Handle(StepRepr_ShapeAspect) aSA = new StepRepr_ShapeAspect();
      aSA->Init(new TCollection_HAsciiString(), new TCollection_HAsciiString(), myGDTCommonPDS, StepData_LTrue);
      aModel->AddWithRefs(aSA);
      writePresentation(theWS, anObject->GetPresentation(), anObject->GetPresentationName(), anObject->HasPlane(),
                        Standard_False, anObject->GetPlane(), anObject->GetPointTextAttach(), aSA, theLocalFactors);
    }

    if (!XCAFDoc_DimTolTool::GetRefShapeLabel(aDimensionL, aFirstShapeL, aSecondShapeL))
      continue;

    // Write links with shapes
    Handle(StepRepr_ShapeAspect) aFirstSA, aSecondSA;
    if (aFirstShapeL.Length() == 1)
    {
      TopoDS_Shape aShape = XCAFDoc_ShapeTool::GetShape(aFirstShapeL.Value(1));
      aFirstSA = writeShapeAspect(theWS, aDimensionL, aShape, dummyRC, dummyGISU);
      if (aRC.IsNull() && !dummyRC.IsNull())
        aRC = dummyRC;
    }
    else if (aFirstShapeL.Length() > 1)
    {
      Handle(StepRepr_CompositeShapeAspect) aCSA;
      for (Standard_Integer shIt = 1; shIt <= aFirstShapeL.Length(); shIt++)
      {
        TopoDS_Shape aShape = XCAFDoc_ShapeTool::GetShape(aFirstShapeL.Value(shIt));
        Handle(StepRepr_ShapeAspect) aSA = writeShapeAspect(theWS, aDimensionL, aShape, dummyRC, dummyGISU);
        if (aSA.IsNull())
          continue;
        if (aCSA.IsNull())
        {
          aCSA = new StepRepr_CompositeShapeAspect();
          aCSA->Init(aSA->Name(), aSA->Description(), aSA->OfShape(), aSA->ProductDefinitional());
          aModel->AddWithRefs(aCSA);
        }
        Handle(StepRepr_ShapeAspectRelationship) aSAR = new StepRepr_ShapeAspectRelationship();
        aSAR->Init(new TCollection_HAsciiString(), Standard_False, new TCollection_HAsciiString(), aCSA, aSA);
        aModel->AddWithRefs(aSAR);
        if (aRC.IsNull() && !dummyRC.IsNull())
          aRC = dummyRC;
      }
      aFirstSA = aCSA;
    }
    if (aSecondShapeL.Length() == 1)
    {
      TopoDS_Shape aShape = XCAFDoc_ShapeTool::GetShape(aSecondShapeL.Value(1));
      aSecondSA = writeShapeAspect(theWS, aDimensionL, aShape, dummyRC, dummyGISU);
      if (aRC.IsNull() && !dummyRC.IsNull())
        aRC = dummyRC;
    }
    else if (aSecondShapeL.Length() > 1)
    {
      Handle(StepRepr_CompositeShapeAspect) aCSA;
      for (Standard_Integer shIt = 1; shIt <= aSecondShapeL.Length(); shIt++)
      {
        TopoDS_Shape aShape = XCAFDoc_ShapeTool::GetShape(aSecondShapeL.Value(shIt));
        Handle(StepRepr_ShapeAspect) aSA = writeShapeAspect(theWS, aDimensionL, aShape, dummyRC, dummyGISU);
        if (aCSA.IsNull() && !aSA.IsNull())
        {
          aCSA = new StepRepr_CompositeShapeAspect();
        }
        aCSA->Init(aSA->Name(), aSA->Description(), aSA->OfShape(), aSA->ProductDefinitional());
        if (!aSA.IsNull())
        {
          Handle(StepRepr_ShapeAspectRelationship) aSAR = new StepRepr_ShapeAspectRelationship();
          aSAR->Init(new TCollection_HAsciiString(), Standard_False, new TCollection_HAsciiString(), aCSA, aSA);
          aModel->AddWithRefs(aSAR);
        }
        if (aRC.IsNull() && !dummyRC.IsNull())
          aRC = dummyRC;
      }
      aSecondSA = aCSA;
    }

    if (anObject->GetType() == XCAFDimTolObjects_DimensionType_DimensionPresentation)
    {
      writePresentation(theWS, anObject->GetPresentation(), anObject->GetPresentationName(), anObject->HasPlane(),
                        Standard_False, anObject->GetPlane(), anObject->GetPointTextAttach(), aFirstSA, theLocalFactors);
      continue;
    }

    // Write dimensions
    StepShape_DimensionalCharacteristic aDimension;
    if (anObject->HasPoint() || anObject->HasPoint2())
      WriteDerivedGeometry(theWS, anObject, aCGRepr, aFirstSA, aSecondSA, aConnectionPnts, theLocalFactors);
    XCAFDimTolObjects_DimensionType aDimType = anObject->GetType();
    if (STEPCAFControl_GDTProperty::IsDimensionalLocation(aDimType))
    {
      // Dimensional_Location
      Handle(StepShape_DimensionalLocation) aDim = new StepShape_DimensionalLocation();
      aDim->Init(STEPCAFControl_GDTProperty::GetDimTypeName(aDimType), Standard_False, NULL, aFirstSA, aSecondSA);
      aDimension.SetValue(aDim);
    }
    else if (aDimType == XCAFDimTolObjects_DimensionType_Location_Angular)
    {
      // Angular_Location
      Handle(StepShape_AngularLocation) aDim = new StepShape_AngularLocation();
      StepShape_AngleRelator aRelator = StepShape_Equal;
      if (anObject->HasQualifier())
      {
        XCAFDimTolObjects_AngularQualifier aQualifier = anObject->GetAngularQualifier();
        switch (aQualifier)
        {
          case XCAFDimTolObjects_AngularQualifier_Small: aRelator = StepShape_Small;
            break;
          case XCAFDimTolObjects_AngularQualifier_Large: aRelator = StepShape_Large;
            break;
          default: aRelator = StepShape_Equal;
        }
      }
      aDim->Init(new TCollection_HAsciiString(), Standard_False, NULL, aFirstSA, aSecondSA, aRelator);
      aDimension.SetValue(aDim);
    }
    else if (aDimType == XCAFDimTolObjects_DimensionType_Location_WithPath)
    {
      // Dimensional_Location_With_Path
      Handle(StepShape_DimensionalLocationWithPath) aDim = new StepShape_DimensionalLocationWithPath();
      Handle(StepRepr_ShapeAspect) aPathSA = writeShapeAspect(theWS, aDimensionL, anObject->GetPath(), dummyRC, dummyGISU);
      aDim->Init(new TCollection_HAsciiString(), Standard_False, NULL, aFirstSA, aSecondSA, aPathSA);
      aDimension.SetValue(aDim);
    }
    else if (STEPCAFControl_GDTProperty::IsDimensionalSize(aDimType))
    {
      // Dimensional_Size
      Handle(StepShape_DimensionalSize) aDim = new StepShape_DimensionalSize();
      aDim->Init(aFirstSA, STEPCAFControl_GDTProperty::GetDimTypeName(aDimType));
      aDimension.SetValue(aDim);
    }
    else if (aDimType == XCAFDimTolObjects_DimensionType_Size_Angular)
    {
      // Angular_Size
      Handle(StepShape_AngularSize) aDim = new StepShape_AngularSize();
      StepShape_AngleRelator aRelator = StepShape_Equal;
      if (anObject->HasQualifier())
      {
        XCAFDimTolObjects_AngularQualifier aQualifier = anObject->GetAngularQualifier();
        switch (aQualifier)
        {
          case XCAFDimTolObjects_AngularQualifier_Small: aRelator = StepShape_Small;
            break;
          case XCAFDimTolObjects_AngularQualifier_Large: aRelator = StepShape_Large;
            break;
          default: aRelator = StepShape_Equal;
        }
      }
      aDim->Init(aFirstSA, new TCollection_HAsciiString(), aRelator);
      aDimension.SetValue(aDim);
    }
    else if (aDimType == XCAFDimTolObjects_DimensionType_Size_WithPath)
    {
      // Dimensional_Size_With_Path
      Handle(StepShape_DimensionalSizeWithPath) aDim = new StepShape_DimensionalSizeWithPath();
      Handle(StepRepr_ShapeAspect) aPathSA = writeShapeAspect(theWS, aDimensionL, anObject->GetPath(), dummyRC, dummyGISU);
      aDim->Init(aFirstSA, new TCollection_HAsciiString(), aPathSA);
      aDimension.SetValue(aDim);
    }

    // Write values
    WriteDimValues(theWS, anObject, aRC, aDimension, theLocalFactors);
    //Annotation plane and Presentation
    writePresentation(theWS, anObject->GetPresentation(), anObject->GetPresentationName(), Standard_True, anObject->HasPlane(),
                      anObject->GetPlane(), anObject->GetPointTextAttach(), aDimension.Value(), theLocalFactors);
  }
  // Write Derived geometry
  if (aConnectionPnts.Length() > 0)
  {
    Handle(StepRepr_HArray1OfRepresentationItem) anItems = new StepRepr_HArray1OfRepresentationItem(1, aConnectionPnts.Length());
    for (Standard_Integer i = 0; i < aConnectionPnts.Length(); i++)
      anItems->SetValue(i + 1, aConnectionPnts(i));
    aCGRepr->Init(new TCollection_HAsciiString(), anItems, dummyRC);
    aCGReprRel->Init(new TCollection_HAsciiString(), new TCollection_HAsciiString(), dummyGISU->UsedRepresentation(), aCGRepr);
    aModel->AddWithRefs(aCGReprRel);
  }

  // write Geometric Tolerances
  aDGTLabels.Clear();
  for (TDF_LabelMap::Iterator aLabelIter(myRootLabels);
       aLabelIter.More(); aLabelIter.Next())
  {
    const TDF_Label& aLabel = aLabelIter.Value();
    Handle(XCAFDoc_DimTolTool) aDGTTool = XCAFDoc_DocumentTool::DimTolTool(aLabel);
    TDF_LabelSequence aaDGTLS;
    aDGTTool->GetGeomToleranceLabels(aDGTLabels);
    aDGTLabels.Append(aaDGTLS);
  }
  for (TDF_LabelSequence::Iterator aDGTIter(aDGTLabels);
       aDGTIter.More(); aDGTIter.Next())
  {
    const TDF_Label aGeomTolL = aDGTIter.Value();
    TDF_LabelSequence aFirstShapeL, aNullSeqL;
    if (!XCAFDoc_DimTolTool::GetRefShapeLabel(aGeomTolL, aFirstShapeL, aNullSeqL))
      continue;
    TDF_LabelSequence aDatumSeq;
    XCAFDoc_DimTolTool::GetDatumWithObjectOfTolerLabels(aGeomTolL, aDatumSeq);
    Handle(StepDimTol_HArray1OfDatumSystemOrReference) aDatumSystem;
    if (aDatumSeq.Length() > 0)
      aDatumSystem = WriteDatumSystem(theWS, aGeomTolL, aDatumSeq, aDatumMap, aRC, theLocalFactors);
    writeGeomTolerance(theWS, aFirstShapeL, aGeomTolL, aDatumSystem, aRC, theLocalFactors);
  }

  // Write Draughting model for Annotation Planes
  if (myGDTAnnotations.Length() == 0)
    return Standard_True;

  Handle(StepRepr_HArray1OfRepresentationItem) aItems =
    new StepRepr_HArray1OfRepresentationItem(1, myGDTAnnotations.Length());
  for (Standard_Integer i = 1; i <= aItems->Length(); i++)
  {
    aItems->SetValue(i, myGDTAnnotations.Value(i - 1));
  }
  myGDTPresentationDM->Init(new TCollection_HAsciiString(), aItems, aRC);
  aModel->AddWithRefs(myGDTPresentationDM);

  return Standard_True;
}

//=======================================================================
//function : FindPDSforRI
//purpose  : auxiliary:
//=======================================================================
static Standard_Boolean FindPDSforRI(const Interface_Graph& theGraph,
                                     const Handle(Standard_Transient)& theEnt,
                                     Handle(StepRepr_ProductDefinitionShape)& thePDS,
                                     Handle(StepRepr_RepresentationContext)& theRC)
{
  if (theEnt.IsNull() || !theEnt->IsKind(STANDARD_TYPE(StepRepr_RepresentationItem)))
    return Standard_False;
  for (Interface_EntityIterator aSharingIter = theGraph.Sharings(theEnt);
       aSharingIter.More() && thePDS.IsNull(); aSharingIter.Next())
  {
    Handle(StepShape_ShapeRepresentation) aSR =
      Handle(StepShape_ShapeRepresentation)::DownCast(aSharingIter.Value());
    if (aSR.IsNull())
      continue;
    theRC = aSR->ContextOfItems();
    for (Interface_EntityIterator aSubs1 = theGraph.Sharings(aSR);
         aSubs1.More() && thePDS.IsNull(); aSubs1.Next())
    {
      Handle(StepShape_ShapeDefinitionRepresentation) aSDR =
        Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(aSubs1.Value());
      if (aSDR.IsNull())
        continue;
      Handle(StepRepr_PropertyDefinition) aPropD = aSDR->Definition().PropertyDefinition();
      if (aPropD.IsNull())
        continue;
      thePDS = Handle(StepRepr_ProductDefinitionShape)::DownCast(aPropD);
    }
  }
  return Standard_True;
}

//=======================================================================
//function : writeMaterials
//purpose  :
//=======================================================================
Standard_Boolean STEPCAFControl_Writer::writeMaterials(const Handle(XSControl_WorkSession)& theWS,
                                                       const TDF_LabelSequence& theLabels) const
{

  if (theLabels.IsEmpty())
    return Standard_False;

  // get working data
  const Handle(Interface_InterfaceModel)& aModel = theWS->Model();
  const Handle(XSControl_TransferWriter)& aTW = theWS->TransferWriter();
  const Handle(Transfer_FinderProcess)& aFP = aTW->FinderProcess();

  const Handle(Interface_HGraph) aHGraph = theWS->HGraph();
  if (aHGraph.IsNull())
    return Standard_False;

  Interface_Graph aGraph = theWS->HGraph()->Graph();

  STEPConstruct_DataMapOfAsciiStringTransient aMapDRI, aMapMRI;
  TDF_LabelSequence aTopLabels;
  // Iterate on requested shapes collect Tools
  for (TDF_LabelMap::Iterator aLabelIter(myRootLabels);
       aLabelIter.More(); aLabelIter.Next())
  {
    const TDF_Label& aLabel = aLabelIter.Value();
    Handle(XCAFDoc_ShapeTool) aShTool = XCAFDoc_DocumentTool::ShapeTool(aLabel);
    TDF_LabelSequence aTopInterLabels;
    aShTool->GetShapes(aTopInterLabels);
    aTopLabels.Append(aTopInterLabels);
  }
  for (TDF_LabelSequence::Iterator aTopLIter(aTopLabels);
       aTopLIter.More(); aTopLIter.Next())
  {
    const TDF_Label& aShL = aTopLIter.Value();
    Handle(TDataStd_TreeNode) aNode;
    if (!aShL.FindAttribute(XCAFDoc::MaterialRefGUID(), aNode) || !aNode->HasFather())
    {
      continue;
    }
    // find PDS for current shape
    TopoDS_Shape aShape = XCAFDoc_ShapeTool::GetShape(aShL);
    TopLoc_Location aLocation;
    TColStd_SequenceOfTransient aSeqRI;
    FindEntities(aFP, aShape, aLocation, aSeqRI);
    if (aSeqRI.Length() <= 0) continue;
    Handle(StepRepr_ProductDefinitionShape) aPDS;
    Handle(StepRepr_RepresentationContext) aRC;
    Handle(Standard_Transient) anEnt = aSeqRI.Value(1);
    FindPDSforRI(aGraph, anEnt, aPDS, aRC);
    if (aPDS.IsNull())
      continue;
    Handle(StepBasic_ProductDefinition) aProdDef =
      aPDS->Definition().ProductDefinition();
    if (aProdDef.IsNull())
      continue;
    // write material entities
    TDF_Label aMatL = aNode->Father()->Label();
    Handle(TCollection_HAsciiString) aName;
    Handle(TCollection_HAsciiString) aDescription;
    Standard_Real aDensity;
    Handle(TCollection_HAsciiString) aDensName;
    Handle(TCollection_HAsciiString) aDensValType;
    Handle(StepRepr_Representation) aRepDRI;
    Handle(StepRepr_Representation) aRepMRI;
    if (XCAFDoc_MaterialTool::GetMaterial(aMatL, aName, aDescription, aDensity, aDensName, aDensValType))
    {
      if (aName->Length() == 0)
        continue;
      TCollection_AsciiString aKey(aName->ToCString());
      if (aMapDRI.IsBound(aKey))
      {
        aRepDRI = Handle(StepRepr_Representation)::DownCast(aMapDRI.Find(aKey));
        if (aMapMRI.IsBound(aKey))
        {
          aRepMRI = Handle(StepRepr_Representation)::DownCast(aMapMRI.Find(aKey));
        }
      }
      else
      {
        // write DRI
        Handle(StepRepr_DescriptiveRepresentationItem) aDRI = new StepRepr_DescriptiveRepresentationItem;
        aDRI->Init(aName, aDescription);
        Handle(StepRepr_HArray1OfRepresentationItem) aHARI = new StepRepr_HArray1OfRepresentationItem(1, 1);
        aHARI->SetValue(1, aDRI);
        aRepDRI = new StepRepr_Representation();
        aRepDRI->Init(new TCollection_HAsciiString("material name"), aHARI, aRC);
        aModel->AddWithRefs(aRepDRI);
        // write MRI
        if (aDensity > 0)
        {
          // mass
          Handle(StepBasic_SiUnitAndMassUnit) aSMU = new StepBasic_SiUnitAndMassUnit;
          aSMU->SetName(StepBasic_sunGram);
          Handle(StepBasic_DerivedUnitElement) aDUE1 = new StepBasic_DerivedUnitElement;
          aDUE1->Init(aSMU, 3.0);
          // length
          Handle(StepBasic_SiUnitAndLengthUnit) aSLU = new StepBasic_SiUnitAndLengthUnit;
          aSLU->Init(Standard_True, StepBasic_spCenti, StepBasic_sunMetre);
          Handle(StepBasic_DerivedUnitElement) aDUE2 = new StepBasic_DerivedUnitElement;
          aDUE2->Init(aSLU, 2.0);
          // other
          Handle(StepBasic_HArray1OfDerivedUnitElement) aHADUE = new StepBasic_HArray1OfDerivedUnitElement(1, 2);
          aHADUE->SetValue(1, aDUE1);
          aHADUE->SetValue(2, aDUE2);
          Handle(StepBasic_DerivedUnit) aDU = new StepBasic_DerivedUnit;
          aDU->Init(aHADUE);
          aModel->AddWithRefs(aDU);
          StepBasic_Unit aUnit;
          aUnit.SetValue(aDU);
          Handle(StepBasic_MeasureValueMember) aMVM = new StepBasic_MeasureValueMember;
          aMVM->SetName(aDensValType->ToCString());
          aMVM->SetReal(aDensity);
          Handle(StepRepr_MeasureRepresentationItem) aMRI = new StepRepr_MeasureRepresentationItem;
          aMRI->Init(aDensName, aMVM, aUnit);
          aHARI = new StepRepr_HArray1OfRepresentationItem(1, 1);
          aHARI->SetValue(1, aMRI);
          aRepMRI = new StepRepr_Representation();
          aRepMRI->Init(new TCollection_HAsciiString("density"), aHARI, aRC);
          aModel->AddWithRefs(aRepMRI);
        }
        //WriteNewMaterial(Model,aName,aDescription,aDensity,aDensName,aDensValType,RC,RepDRI,RepMRI);
        aMapDRI.Bind(aKey, aRepDRI);
        if (!aRepMRI.IsNull())
          aMapMRI.Bind(aKey, aRepMRI);
      }
    }

    if (!aRepDRI.IsNull())
    {
      StepRepr_CharacterizedDefinition aCD1;
      aCD1.SetValue(aProdDef);
      Handle(StepRepr_PropertyDefinition) aPropD1 = new StepRepr_PropertyDefinition;
      aPropD1->Init(new TCollection_HAsciiString("material property"), Standard_True,
                    new TCollection_HAsciiString("material name"), aCD1);
      aModel->AddWithRefs(aPropD1);
      StepRepr_RepresentedDefinition aRD1;
      aRD1.SetValue(aPropD1);
      Handle(StepRepr_PropertyDefinitionRepresentation) aPDR1 =
        new StepRepr_PropertyDefinitionRepresentation;
      aPDR1->Init(aRD1, aRepDRI);
      aModel->AddWithRefs(aPDR1);

      if (!aRepMRI.IsNull())
      {
        StepRepr_CharacterizedDefinition aCD2;
        aCD2.SetValue(aProdDef);
        Handle(StepRepr_PropertyDefinition) aPropD2 = new StepRepr_PropertyDefinition;
        aPropD2->Init(new TCollection_HAsciiString("material property"), Standard_True,
                      new TCollection_HAsciiString("density"), aCD2);
        aModel->AddWithRefs(aPropD2);
        StepRepr_RepresentedDefinition aRD2;
        aRD2.SetValue(aPropD2);
        Handle(StepRepr_PropertyDefinitionRepresentation) aPDR2 =
          new StepRepr_PropertyDefinitionRepresentation;
        aPDR2->Init(aRD2, aRepMRI);
        aModel->AddWithRefs(aPDR2);
      }
    }
  }

  return Standard_True;
}
