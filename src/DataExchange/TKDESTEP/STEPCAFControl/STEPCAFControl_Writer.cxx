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
#include <TopoDS_Shape.hxx>
#include <Standard_Transient.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <OSD_Path.hxx>
#include <StepAP242_DraughtingModelItemAssociation.hxx>
#include <StepAP242_GeometricItemSpecificUsage.hxx>
#include <StepBasic_ConversionBasedUnitAndLengthUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndPlaneAngleUnit.hxx>
#include <StepBasic_DerivedUnit.hxx>
#include <StepBasic_DerivedUnitElement.hxx>
#include <StepBasic_GeneralProperty.hxx>
#include <StepBasic_GeneralPropertyAssociation.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
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
#include <StepDimTol_DatumReferenceElement.hxx>
#include <StepDimTol_DatumReferenceModifier.hxx>
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
#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_IntegerRepresentationItem.hxx>
#include <StepRepr_MeasureRepresentationItem.hxx>
#include <StepRepr_NextAssemblyUsageOccurrence.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_PropertyDefinition.hxx>
#include <StepRepr_RealRepresentationItem.hxx>
#include <StepRepr_Representation.hxx>
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
#include <StepVisual_PresentationStyleAssignment.hxx>
#include <StepVisual_PresentationStyleSelect.hxx>
#include <StepVisual_Invisibility.hxx>
#include <StepVisual_InvisibleItem.hxx>
#include <StepVisual_MechanicalDesignGeometricPresentationRepresentation.hxx>
#include <StepVisual_NullStyleMember.hxx>
#include <StepVisual_PointStyle.hxx>
#include <StepVisual_PresentationLayerAssignment.hxx>
#include <StepVisual_PresentationRepresentation.hxx>
#include <StepVisual_PresentationStyleByContext.hxx>
#include <StepVisual_StyledItem.hxx>
#include <StepVisual_SurfaceStyleUsage.hxx>
#include <StepVisual_TessellatedAnnotationOccurrence.hxx>
#include <StepVisual_TessellatedGeometricSet.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Map.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <NCollection_Sequence.hxx>
#include <TDocStd_Document.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
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
#include <XCAFPrs_Style.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <XSAlgo.hxx>
#include <XSAlgo_ShapeProcessor.hxx>
#include <XSControl_TransferWriter.hxx>
#include <XSControl_WorkSession.hxx>
#include <UnitsMethods.hxx>

//=======================================================================
// function : GetLabelName
// purpose  : auxiliary function: take name of label and append it to str
//=======================================================================
static bool GetLabelName(const TDF_Label& theL, occ::handle<TCollection_HAsciiString>& theStr)
{
  occ::handle<TDataStd_Name> anAttrName;
  if (!theL.FindAttribute(TDataStd_Name::GetID(), anAttrName))
  {
    return false;
  }
  TCollection_ExtendedString aName = anAttrName->Get();
  if (aName.IsEmpty())
    return false;

  // set name, removing spaces around it
  TCollection_AsciiString aBuffer(aName);
  aBuffer.LeftAdjust();
  aBuffer.RightAdjust();
  theStr->AssignCat(aBuffer.ToCString());
  return true;
}

//=================================================================================================

STEPCAFControl_Writer::STEPCAFControl_Writer()
    : myColorMode(true),
      myNameMode(true),
      myLayerMode(true),
      myPropsMode(true),
      myMetadataMode(true),
      mySHUOMode(true),
      myGDTMode(true),
      myMatMode(true),
      myVisMatMode(false),
      myIsCleanDuplicates(false)
{
  STEPCAFControl_Controller::Init();
  occ::handle<XSControl_WorkSession> aWS = new XSControl_WorkSession;
  Init(aWS);
}

//=================================================================================================

STEPCAFControl_Writer::STEPCAFControl_Writer(const occ::handle<XSControl_WorkSession>& theWS,
                                             const bool                                theScratch)
    : myColorMode(true),
      myNameMode(true),
      myLayerMode(true),
      myPropsMode(true),
      myMetadataMode(true),
      mySHUOMode(true),
      myGDTMode(true),
      myMatMode(true),
      myVisMatMode(false),
      myIsCleanDuplicates(false)
{
  STEPCAFControl_Controller::Init();
  Init(theWS, theScratch);
}

//=================================================================================================

void STEPCAFControl_Writer::Init(const occ::handle<XSControl_WorkSession>& theWS,
                                 const bool                                theScratch)
{
  theWS->SelectNorm("STEP");
  myWriter.SetWS(theWS, theScratch);
  myFiles.Clear();
  myLabEF.Clear();
  myLabels.Clear();
  myPureRefLabels.Clear();
  myRootLabels.Clear();
  myGDTPresentationDM = new StepVisual_DraughtingModel();
  myGDTPrsCurveStyle =
    new NCollection_HArray1<occ::handle<StepVisual_PresentationStyleAssignment>>(1, 1);
}

//=================================================================================================

IFSelect_ReturnStatus STEPCAFControl_Writer::Write(const char* const theFileName)
{
  if (myIsCleanDuplicates)
  {
    // remove duplicates
    myWriter.CleanDuplicateEntities();
  }

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

  for (NCollection_DataMap<TCollection_AsciiString,
                           occ::handle<STEPCAFControl_ExternFile>>::Iterator anExtFileIter(myFiles);
       anExtFileIter.More();
       anExtFileIter.Next())
  {
    const occ::handle<STEPCAFControl_ExternFile>& anExtFile = anExtFileIter.Value();
    if (anExtFile->GetWriteStatus() != IFSelect_RetVoid)
    {
      continue;
    }

    // construct extern file name
    TCollection_AsciiString aFileName =
      OSD_Path::AbsolutePath(aDirPath, anExtFile->GetName()->String());
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

//=================================================================================================

void STEPCAFControl_Writer::prepareUnit(const TDF_Label&                       theLabel,
                                        const occ::handle<StepData_StepModel>& theModel,
                                        StepData_Factors&                      theLocalFactors)
{
  occ::handle<XCAFDoc_LengthUnit> aLengthAttr;
  if (!theLabel.IsNull() && theLabel.Root().FindAttribute(XCAFDoc_LengthUnit::GetID(), aLengthAttr))
  {
    theModel->SetLocalLengthUnit(aLengthAttr->GetUnitValue() * 1000); // convert to mm
    theLocalFactors.SetCascadeUnit(aLengthAttr->GetUnitValue() * 1000);
  }
  else
  {
    XSAlgo_ShapeProcessor::PrepareForTransfer(); // update unit info
    theModel->SetLocalLengthUnit(UnitsMethods::GetCasCadeLengthUnit());
    theLocalFactors.SetCascadeUnit(UnitsMethods::GetCasCadeLengthUnit());
  }
}

//=================================================================================================

IFSelect_ReturnStatus STEPCAFControl_Writer::WriteStream(std::ostream& theStream)
{
  if (!myFiles.IsEmpty())
  {
    // writing external files is unsupported via stream interface
    return IFSelect_RetError;
  }

  return myWriter.WriteStream(theStream);
}

//=================================================================================================

bool STEPCAFControl_Writer::Transfer(const occ::handle<TDocStd_Document>& theDoc,
                                     const STEPControl_StepModelType      theMode,
                                     const char* const                    theMulti,
                                     const Message_ProgressRange&         theProgress)
{
  const occ::handle<StepData_StepModel> aModel =
    occ::down_cast<StepData_StepModel>(myWriter.WS()->Model());
  aModel->InternalParameters.InitFromStatic();
  return Transfer(theDoc, aModel->InternalParameters, theMode, theMulti, theProgress);
}

//=================================================================================================

bool STEPCAFControl_Writer::Transfer(const occ::handle<TDocStd_Document>& theDoc,
                                     const DESTEP_Parameters&             theParams,
                                     const STEPControl_StepModelType      theMode,
                                     const char* const                    theMulti,
                                     const Message_ProgressRange&         theProgress)
{
  occ::handle<XCAFDoc_ShapeTool> aShTool = XCAFDoc_DocumentTool::ShapeTool(theDoc->Main());
  if (aShTool.IsNull())
    return false;

  myRootLabels.Clear();
  myRootLabels.Add(theDoc->Main().Root());
  NCollection_Sequence<TDF_Label> aLabels;
  aShTool->GetFreeShapes(aLabels);
  const occ::handle<StepData_StepModel> aModel =
    occ::down_cast<StepData_StepModel>(myWriter.WS()->Model());
  aModel->InternalParameters = theParams;
  return transfer(myWriter, aLabels, theMode, theMulti, false, theProgress);
}

//=================================================================================================

bool STEPCAFControl_Writer::Transfer(const TDF_Label&                theLabel,
                                     const STEPControl_StepModelType theMode,
                                     const char* const               theIsMulti,
                                     const Message_ProgressRange&    theProgress)
{
  const occ::handle<StepData_StepModel> aModel =
    occ::down_cast<StepData_StepModel>(myWriter.WS()->Model());
  aModel->InternalParameters.InitFromStatic();
  return Transfer(theLabel, aModel->InternalParameters, theMode, theIsMulti, theProgress);
}

//=================================================================================================

bool STEPCAFControl_Writer::Transfer(const TDF_Label&                theLabel,
                                     const DESTEP_Parameters&        theParams,
                                     const STEPControl_StepModelType theMode,
                                     const char* const               theIsMulti,
                                     const Message_ProgressRange&    theProgress)
{
  if (theLabel.IsNull())
  {
    return false;
  }
  NCollection_Sequence<TDF_Label> aLabels;
  aLabels.Append(theLabel);
  myRootLabels.Clear();
  myRootLabels.Add(theLabel.Root());
  const occ::handle<StepData_StepModel> aModel =
    occ::down_cast<StepData_StepModel>(myWriter.WS()->Model());
  aModel->InternalParameters = theParams;
  return transfer(myWriter, aLabels, theMode, theIsMulti, false, theProgress);
}

//=================================================================================================

bool STEPCAFControl_Writer::Transfer(const NCollection_Sequence<TDF_Label>& theLabels,
                                     const STEPControl_StepModelType        theMode,
                                     const char* const                      theIsMulti,
                                     const Message_ProgressRange&           theProgress)
{
  const occ::handle<StepData_StepModel> aModel =
    occ::down_cast<StepData_StepModel>(myWriter.WS()->Model());
  aModel->InternalParameters.InitFromStatic();
  return Transfer(theLabels, aModel->InternalParameters, theMode, theIsMulti, theProgress);
}

//=================================================================================================

bool STEPCAFControl_Writer::Transfer(const NCollection_Sequence<TDF_Label>& theLabels,
                                     const DESTEP_Parameters&               theParams,
                                     const STEPControl_StepModelType        theMode,
                                     const char* const                      theIsMulti,
                                     const Message_ProgressRange&           theProgress)
{
  myRootLabels.Clear();
  for (NCollection_Sequence<TDF_Label>::Iterator aLabelIter(theLabels); aLabelIter.More();
       aLabelIter.Next())
  {
    const TDF_Label& aLabel = aLabelIter.Value();
    if (!aLabel.IsNull())
    {
      myRootLabels.Add(aLabel.Root());
    }
  }
  const occ::handle<StepData_StepModel> aModel =
    occ::down_cast<StepData_StepModel>(myWriter.WS()->Model());
  aModel->InternalParameters = theParams;
  return transfer(myWriter, theLabels, theMode, theIsMulti, false, theProgress);
}

//=================================================================================================

bool STEPCAFControl_Writer::Perform(const occ::handle<TDocStd_Document>& theDoc,
                                    const char* const                    theFileName,
                                    const Message_ProgressRange&         theProgress)
{
  if (!Transfer(theDoc, STEPControl_AsIs, nullptr, theProgress))
    return false;
  return Write(theFileName) == IFSelect_RetDone;
}

//=================================================================================================

bool STEPCAFControl_Writer::Perform(const occ::handle<TDocStd_Document>& theDoc,
                                    const char* const                    theFileName,
                                    const DESTEP_Parameters&             theParams,
                                    const Message_ProgressRange&         theProgress)
{
  if (!Transfer(theDoc, theParams, STEPControl_AsIs, nullptr, theProgress))
    return false;
  return Write(theFileName) == IFSelect_RetDone;
}

//=================================================================================================

bool STEPCAFControl_Writer::Perform(const occ::handle<TDocStd_Document>& theDoc,
                                    const TCollection_AsciiString&       theFileName,
                                    const Message_ProgressRange&         theProgress)
{
  if (!Transfer(theDoc, STEPControl_AsIs, nullptr, theProgress))
    return false;
  return Write(theFileName.ToCString()) == IFSelect_RetDone;
}

//=================================================================================================

bool STEPCAFControl_Writer::ExternFile(const TDF_Label&                        theLabel,
                                       occ::handle<STEPCAFControl_ExternFile>& theExtFile) const
{
  theExtFile.Nullify();
  if (!myLabEF.IsBound(theLabel))
    return false;
  theExtFile = myLabEF.Find(theLabel);
  return true;
}

//=================================================================================================

bool STEPCAFControl_Writer::ExternFile(const char* const                       theName,
                                       occ::handle<STEPCAFControl_ExternFile>& theExtFile) const
{
  theExtFile.Nullify();
  if (!myFiles.IsEmpty() || !myFiles.IsBound(theName))
    return false;
  theExtFile = myFiles.Find(theName);
  return true;
}

//=============================================================================

void STEPCAFControl_Writer::SetShapeFixParameters(
  const XSAlgo_ShapeProcessor::ParameterMap& theParameters)
{
  myWriter.SetShapeFixParameters(theParameters);
}

//=============================================================================

void STEPCAFControl_Writer::SetShapeFixParameters(
  XSAlgo_ShapeProcessor::ParameterMap&& theParameters)
{
  myWriter.SetShapeFixParameters(std::move(theParameters));
}

//=============================================================================

void STEPCAFControl_Writer::SetShapeFixParameters(
  const DE_ShapeFixParameters&               theParameters,
  const XSAlgo_ShapeProcessor::ParameterMap& theAdditionalParameters)
{
  myWriter.SetShapeFixParameters(theParameters, theAdditionalParameters);
}

//=============================================================================

const XSAlgo_ShapeProcessor::ParameterMap& STEPCAFControl_Writer::GetShapeFixParameters() const
{
  return myWriter.GetShapeFixParameters();
}

//=============================================================================

void STEPCAFControl_Writer::SetShapeProcessFlags(const ShapeProcess::OperationsFlags& theFlags)
{
  myWriter.SetShapeProcessFlags(theFlags);
}

//=============================================================================

const XSAlgo_ShapeProcessor::ProcessingFlags& STEPCAFControl_Writer::GetShapeProcessFlags() const
{
  return myWriter.GetShapeProcessFlags();
}

//=================================================================================================

bool STEPCAFControl_Writer::transfer(STEPControl_Writer&                    theWriter,
                                     const NCollection_Sequence<TDF_Label>& theLabels,
                                     const STEPControl_StepModelType        theMode,
                                     const char* const                      theIsMulti,
                                     const bool                             theIsExternFile,
                                     const Message_ProgressRange&           theProgress)
{
  if (theLabels.IsEmpty())
    return false;

  occ::handle<STEPCAFControl_ActorWrite> anActor =
    occ::down_cast<STEPCAFControl_ActorWrite>(theWriter.WS()->NormAdaptor()->ActorWrite());

  StepData_Factors                      aLocalFactors;
  const occ::handle<StepData_StepModel> aModel =
    occ::down_cast<StepData_StepModel>(theWriter.WS()->Model());
  prepareUnit(theLabels.First(), aModel, aLocalFactors); // set local length unit to the model
  // translate free top-level shapes of the DECAF document
  NCollection_Sequence<TDF_Label> aSubLabels;
  Message_ProgressScope           aPS(theProgress, "Labels", theLabels.Length());
  // Iterate on requested shapes
  for (NCollection_Sequence<TDF_Label>::Iterator aLabelIter(theLabels);
       aLabelIter.More() && aPS.More();
       aLabelIter.Next())
  {
    Message_ProgressRange aRange = aPS.Next();
    TDF_Label             aCurL  = aLabelIter.Value();
    if (myLabels.IsBound(aCurL))
      continue; // already processed

    TopoDS_Shape aCurShape = XCAFDoc_ShapeTool::GetShape(aCurL);
    if (aCurShape.IsNull())
      continue;
    TopoDS_Shape aShapeForBind = aCurShape;
    // write shape either as a whole, or as multifile (with extern refs)
    if (!theIsMulti)
    {
      anActor->SetStdMode(false);

      NCollection_Sequence<TDF_Label> aCompLabels;

      // For case when only part of assembly structure should be written in the document
      // if specified label is component of the assembly then
      // in order to save location of this component in the high-level assembly
      // and save name of high-level assembly it is necessary to represent structure of high-level
      // assembly as assembly with one component specified by current label. For that compound
      // containing only specified component is binded to the label of the high-level assembly. The
      // such way full structure of high-level assembly was replaced on the assembly containing one
      // component. For case when free shape reference is (located root) also create an auxiliary
      // assembly.
      if (XCAFDoc_ShapeTool::IsReference(aCurL))
      {
        TopoDS_Compound aComp;
        BRep_Builder    aB;
        aB.MakeCompound(aComp);
        aB.Add(aComp, aCurShape);
        aCurShape = aComp;
        myPureRefLabels.Bind(aCurL, aComp);
        aCompLabels.Append(aCurL);
        TDF_Label aRefL;
        if (XCAFDoc_ShapeTool::GetReferredShape(aCurL, aRefL))
        {
          if (XCAFDoc_ShapeTool::IsAssembly(aRefL))
            XCAFDoc_ShapeTool::GetComponents(aRefL, aCompLabels, true);
        }
        if (!XCAFDoc_ShapeTool::IsFree(aCurL))
          aCurL = aCurL.Father();
      }
      else
      {
        // fill sequence of (sub) shapes for which attributes should be written
        // and set actor to handle assemblies in a proper way
        if (XCAFDoc_ShapeTool::IsAssembly(aCurL))
          XCAFDoc_ShapeTool::GetComponents(aCurL, aCompLabels, true);
      }

      for (NCollection_Sequence<TDF_Label>::Iterator aCompIter(aCompLabels); aCompIter.More();
           aCompIter.Next())
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

      theWriter.Transfer(aCurShape, theMode, aModel->InternalParameters, false, aRange);
      anActor->SetStdMode(true); // restore default behaviour
    }
    else
    {
      // translate final solids
      Message_ProgressScope aPS1(aRange, nullptr, 2);
      TopoDS_Shape          aSass =
        transferExternFiles(aCurL, theMode, aSubLabels, aLocalFactors, theIsMulti, aPS1.Next());
      if (aPS1.UserBreak())
        return false;

      DESTEP_Parameters::WriteMode_Assembly assemblymode = aModel->InternalParameters.WriteAssembly;
      aModel->InternalParameters.WriteAssembly           = DESTEP_Parameters::WriteMode_Assembly_On;
      theWriter.Transfer(aSass, STEPControl_AsIs, aModel->InternalParameters, true, aPS1.Next());
      aModel->InternalParameters.WriteAssembly = assemblymode;
    }
  }
  if (aPS.UserBreak())
    return false;

  theWriter.WS()->ComputeGraph(true); // Setting to clear and regenerate graph

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
    for (NCollection_DataMap<TopoDS_Shape,
                             occ::handle<Standard_Transient>,
                             TopTools_ShapeMapHasher>::Iterator anItr(myMapCompMDGPR);
         anItr.More();
         anItr.Next())
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

  // write metadata
  if (GetMetadataMode())
    writeMetadata(theWriter.WS(), aSubLabels);

  // refresh graph
  theWriter.WS()->ComputeGraph(true);

  // Write names for the sub-shapes
  if (aModel->InternalParameters.WriteSubshapeNames != 0)
  {
    const occ::handle<XSControl_TransferWriter>& TW  = this->ChangeWriter().WS()->TransferWriter();
    const occ::handle<Transfer_FinderProcess>&   aFP = TW->FinderProcess();

    // Iterate on requested sub shapes
    for (NCollection_Sequence<TDF_Label>::Iterator aLabelIter(aSubLabels); aLabelIter.More();
         aLabelIter.Next())
    {
      const TDF_Label& aCurL = aLabelIter.Value();

      for (TDF_ChildIterator aChildIter(aCurL, true); aChildIter.More(); aChildIter.Next())
      {
        const TDF_Label& aSubL = aChildIter.Value();

        // Access name recorded in OCAF TDataStd_Name attribute
        occ::handle<TCollection_HAsciiString> aHSubName = new TCollection_HAsciiString;
        if (!GetLabelName(aSubL, aHSubName))
          continue;

        // Access topological data
        TopoDS_Shape aSubS = XCAFDoc_ShapeTool::GetShape(aSubL);
        if (aSubS.IsNull())
          continue;

        // Access the correspondent STEP Representation Item
        occ::handle<StepRepr_RepresentationItem> aRI;
        occ::handle<TransferBRep_ShapeMapper>    aShMapper = TransferBRep::ShapeMapper(aFP, aSubS);
        if (!aFP->FindTypedTransient(aShMapper, STANDARD_TYPE(StepRepr_RepresentationItem), aRI))
          continue;

        // Record the name
        aRI->SetName(aHSubName);
      }
    }
  }

  return true;
}

//=================================================================================================

TopoDS_Shape STEPCAFControl_Writer::transferExternFiles(const TDF_Label&                 theLabel,
                                                        const STEPControl_StepModelType  theMode,
                                                        NCollection_Sequence<TDF_Label>& theLabels,
                                                        const StepData_Factors& theLocalFactors,
                                                        const char* const       thePrefix,
                                                        const Message_ProgressRange& theProgress)
{
  // if label already translated, just return the shape
  if (myLabels.IsBound(theLabel))
  {
    return myLabels.Find(theLabel);
  }

  TopoDS_Compound aComp;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aComp);
  // if not assembly, write to separate file
  if (!XCAFDoc_ShapeTool::IsAssembly(theLabel) && !XCAFDoc_ShapeTool::IsComponent(theLabel))
  {
    theLabels.Append(theLabel);
    // prepare for transfer
    occ::handle<XSControl_WorkSession> aNewWS = new XSControl_WorkSession;
    aNewWS->SelectNorm("STEP");
    STEPControl_Writer              aStepWriter(aNewWS, true);
    NCollection_Sequence<TDF_Label> aLabelSeq;
    aLabelSeq.Append(theLabel);

    // construct the name for extern file
    occ::handle<TCollection_HAsciiString> aBaseName = new TCollection_HAsciiString;
    if (thePrefix && thePrefix[0])
      aBaseName->AssignCat(thePrefix);
    GetLabelName(theLabel, aBaseName);
    occ::handle<TCollection_HAsciiString> aNewName = new TCollection_HAsciiString(aBaseName);
    aNewName->AssignCat(".stp");
    if (myFiles.IsBound(aNewName->ToCString()))
    { // avoid confusions
      for (int k = 1; k < 32000; k++)
      {
        aNewName = new TCollection_HAsciiString(aBaseName);
        aNewName->AssignCat("_");
        aNewName->AssignCat(TCollection_AsciiString(k).ToCString());
        aNewName->AssignCat(".stp");
        if (!myFiles.IsBound(aNewName->ToCString()))
          break;
      }
    }

    // translate and record extern file
    occ::handle<STEPCAFControl_ExternFile> anExtFile = new STEPCAFControl_ExternFile;
    anExtFile->SetWS(aNewWS);
    anExtFile->SetName(aNewName);
    anExtFile->SetLabel(theLabel);
    DESTEP_Parameters::WriteMode_Assembly anAssemblymode =
      aStepWriter.Model()->InternalParameters.WriteAssembly;
    aStepWriter.Model()->InternalParameters.WriteAssembly =
      DESTEP_Parameters::WriteMode_Assembly_Off;
    const char* const anIsMulti = nullptr;
    anExtFile->SetTransferStatus(
      transfer(aStepWriter, aLabelSeq, theMode, anIsMulti, true, theProgress));
    aStepWriter.Model()->InternalParameters.WriteAssembly = anAssemblymode;
    myLabEF.Bind(theLabel, anExtFile);
    myFiles.Bind(aNewName->ToCString(), anExtFile);

    // return empty compound as replacement for the shape
    myLabels.Bind(theLabel, aComp);
    return aComp;
  }
  NCollection_Sequence<TDF_Label> aCompLabels;
  TDF_Label                       aLabel = theLabel;
  // if specified shape is component then high-level assembly is considered
  // to get valid structure with location
  if (XCAFDoc_ShapeTool::IsComponent(theLabel))
  {
    aCompLabels.Append(theLabel);
    aLabel = theLabel.Father();
  }
  // else iterate on components add create structure of empty compounds
  // representing the assembly
  else if (XCAFDoc_ShapeTool::IsAssembly(theLabel))
    XCAFDoc_ShapeTool::GetComponents(theLabel, aCompLabels, false);

  theLabels.Append(aLabel);
  Message_ProgressScope aPS(theProgress, nullptr, aCompLabels.Length());
  // Iterate on requested shapes
  for (NCollection_Sequence<TDF_Label>::Iterator aLabelIter(aCompLabels);
       aLabelIter.More() && aPS.More();
       aLabelIter.Next())
  {
    const TDF_Label& aCurL = aLabelIter.Value();
    TDF_Label        aRefL;
    if (!XCAFDoc_ShapeTool::GetReferredShape(aCurL, aRefL))
      continue;
    TopoDS_Shape aShComp =
      transferExternFiles(aRefL, theMode, theLabels, theLocalFactors, thePrefix, aPS.Next());
    aShComp.Location(XCAFDoc_ShapeTool::GetLocation(aCurL));
    aBuilder.Add(aComp, aShComp);
  }
  myLabels.Bind(aLabel, aComp);
  return aComp;
}

//=================================================================================================

bool STEPCAFControl_Writer::writeExternRefs(const occ::handle<XSControl_WorkSession>& theWS,
                                            const NCollection_Sequence<TDF_Label>& theLabels) const
{
  if (theLabels.IsEmpty())
    return false;

  const occ::handle<XSControl_TransferWriter>& aTW = theWS->TransferWriter();
  const occ::handle<Transfer_FinderProcess>&   aFP = aTW->FinderProcess();
  STEPConstruct_ExternRefs                     anEFTool(theWS);
  occ::handle<StepData_StepModel> aStepModel  = occ::down_cast<StepData_StepModel>(theWS->Model());
  int                             aStepSchema = aStepModel->InternalParameters.WriteSchema;
  // Iterate on requested shapes
  for (NCollection_Sequence<TDF_Label>::Iterator aLabelIter(theLabels); aLabelIter.More();
       aLabelIter.Next())
  {
    const TDF_Label& aLab = aLabelIter.Value();
    if (XCAFDoc_ShapeTool::IsAssembly(aLab))
      continue; // skip assemblies

    // get extern file
    occ::handle<STEPCAFControl_ExternFile> anExtFile;
    if (!ExternFile(aLab, anExtFile))
      continue; // should never be

    // find SDR
    const TopoDS_Shape* pShape = myLabels.Seek(aLab);
    if (!pShape)
      continue; // not recorded as translated, skip
    TopoDS_Shape aShape = *pShape;

    occ::handle<StepShape_ShapeDefinitionRepresentation> aSDR;
    occ::handle<TransferBRep_ShapeMapper> mapper = TransferBRep::ShapeMapper(aFP, aShape);
    if (!aFP->FindTypedTransient(mapper,
                                 STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation),
                                 aSDR))
    {
      Message::SendTrace() << "Warning: Cannot find SDR for "
                           << aShape.TShape()->DynamicType()->Name() << "\n";
      continue;
    }

    // add extern ref
    const char* aStepFormat = (aStepSchema == 3 ? "STEP AP203" : "STEP AP214");
    // try to get PD from SDR
    StepRepr_RepresentedDefinition           aRD      = aSDR->Definition();
    occ::handle<StepRepr_PropertyDefinition> aPropDef = aRD.PropertyDefinition();
    if (aPropDef.IsNull())
    {
      Message::SendTrace() << "Warning: STEPCAFControl_Writer::writeExternRefs "
                              "StepRepr_PropertyDefinition is null for "
                           << aShape.TShape()->DynamicType()->Name() << "\n";
      continue;
    }
    StepRepr_CharacterizedDefinition         aCharDef = aPropDef->Definition();
    occ::handle<StepBasic_ProductDefinition> aPD      = aCharDef.ProductDefinition();
    if (aPD.IsNull())
    {
      Message::SendTrace() << "Warning: STEPCAFControl_Writer::writeExternRefs "
                              "StepBasic_ProductDefinition is null for "
                           << aShape.TShape()->DynamicType()->Name() << "\n";
      continue;
    }
    anEFTool.AddExternRef(anExtFile->GetName()->ToCString(), aPD, aStepFormat);
  }
  anEFTool.WriteExternRefs(aStepSchema);
  return true;
}

//=================================================================================================

static int FindEntities(const occ::handle<Transfer_FinderProcess>&             theFP,
                        const TopoDS_Shape&                                    theShape,
                        TopLoc_Location&                                       theLocation,
                        NCollection_Sequence<occ::handle<Standard_Transient>>& theSeqRI)
{
  occ::handle<StepRepr_RepresentationItem> anItem =
    STEPConstruct::FindEntity(theFP, theShape, theLocation);

  if (!anItem.IsNull())
  {
    theSeqRI.Append(anItem);
    return 1;
  }

  // may be S was split during shape processing
  occ::handle<TransferBRep_ShapeMapper> aMapper = TransferBRep::ShapeMapper(theFP, theShape);
  occ::handle<Transfer_Binder>          aBinder = theFP->Find(aMapper);
  if (aBinder.IsNull())
    return 0;

  occ::handle<Transfer_TransientListBinder> aTransientListBinder =
    // occ::down_cast<Transfer_TransientListBinder>( bnd->Next(true) );
    occ::down_cast<Transfer_TransientListBinder>(aBinder);
  int aResCount = 0;
  if (aTransientListBinder.IsNull())
  {
    for (TopoDS_Iterator anIter(theShape); anIter.More(); anIter.Next())
    {
      occ::handle<StepRepr_RepresentationItem> aLocalItem =
        STEPConstruct::FindEntity(theFP, anIter.Value(), theLocation);
      if (aLocalItem.IsNull())
        continue;
      aResCount++;
      theSeqRI.Append(aLocalItem);
    }
  }
  else if (!aTransientListBinder.IsNull())
  {
    const int aNbTransient = aTransientListBinder->NbTransients();
    for (int anInd = 1; anInd <= aNbTransient; anInd++)
    {
      occ::handle<Standard_Transient> anEntity = aTransientListBinder->Transient(anInd);
      anItem = occ::down_cast<StepRepr_RepresentationItem>(anEntity);
      if (anItem.IsNull())
        continue;
      aResCount++;
      theSeqRI.Append(anItem);
    }
  }
  return aResCount;
}

//=================================================================================================

static bool getStyledItem(
  const TopoDS_Shape&                   theShape,
  const occ::handle<XCAFDoc_ShapeTool>& theShapeTool,
  const STEPConstruct_Styles&           theStyles,
  occ::handle<StepVisual_StyledItem>&   theResSelItem,
  const NCollection_DataMap<TopoDS_Shape, occ::handle<Standard_Transient>, TopTools_ShapeMapHasher>&
    theMapCompMDGPR)
{
  const TDF_Label aTopShL   = theShapeTool->FindShape(theShape, false);
  TopoDS_Shape    aTopLevSh = theShapeTool->GetShape(aTopShL);
  bool            anIsFound = false;
  if (aTopLevSh.IsNull() || !theMapCompMDGPR.IsBound(aTopLevSh))
  {
    return false;
  }
  occ::handle<StepVisual_PresentationRepresentation> aMDGPR =
    occ::down_cast<StepVisual_PresentationRepresentation>(theMapCompMDGPR.Find(aTopLevSh));
  occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> anSelItmHArr =
    aMDGPR->Items();
  if (anSelItmHArr.IsNull())
  {
    return false;
  }
  // Search for PSA of Manifold solid
  NCollection_Sequence<occ::handle<Standard_Transient>> aNewSeqRI;
  bool                                                  isFilled = false;
  for (NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>::Iterator anIter(
         anSelItmHArr->Array1());
       anIter.More() && !anIsFound;
       anIter.Next())
  {
    const occ::handle<StepVisual_StyledItem>& aSelItm =
      occ::down_cast<StepVisual_StyledItem>(anIter.Value());

    if (aSelItm.IsNull())
    {
      continue;
    }
    // Check that it is a styled item for manifold solid brep
    if (!isFilled)
    {
      TopLoc_Location aLoc;
      FindEntities(theStyles.FinderProcess(), aTopLevSh, aLoc, aNewSeqRI);
      isFilled = true;
    }
    if (!aNewSeqRI.IsEmpty())
    {
      const occ::handle<StepRepr_RepresentationItem>& anItem         = aSelItm->Item();
      bool                                            isSameMonSolBR = false;
      for (NCollection_Sequence<occ::handle<Standard_Transient>>::Iterator aIterRI(aNewSeqRI);
           aIterRI.More();
           aIterRI.Next())
      {
        if (!anItem.IsNull() && anItem == aIterRI.Value())
        {
          isSameMonSolBR = true;
          break;
        }
      }
      if (!isSameMonSolBR)
        continue;
    }
    for (NCollection_HArray1<occ::handle<StepVisual_PresentationStyleAssignment>>::Iterator
           aStyleIter(aSelItm->Styles()->Array1());
         aStyleIter.More() && !anIsFound;
         aStyleIter.Next())
    {
      const occ::handle<StepVisual_PresentationStyleAssignment>& aFatherPSA = aStyleIter.Value();
      // check for PSA for top-level (not Presentation style by context for NAUO)
      if (aFatherPSA.IsNull()
          || aFatherPSA->IsKind(STANDARD_TYPE(StepVisual_PresentationStyleByContext)))
        continue;
      theResSelItem = aSelItm;
      anIsFound     = true;
    }
  }
  return anIsFound;
}

//=================================================================================================

static bool setDefaultInstanceColor(const occ::handle<StepVisual_StyledItem>& theStyleItem,
                                    occ::handle<StepVisual_PresentationStyleAssignment>& thePSA)
{
  bool anIsFound = false;
  for (NCollection_HArray1<occ::handle<StepVisual_PresentationStyleAssignment>>::Iterator
         aStyleIter(theStyleItem->Styles()->Array1());
       aStyleIter.More() && !anIsFound;
       aStyleIter.Next())
  {
    const occ::handle<StepVisual_PresentationStyleAssignment>& aFatherPSA = aStyleIter.Value();
    // check for PSA for top-level (not Presentation style by context for NAUO)
    if (aFatherPSA.IsNull()
        || aFatherPSA->IsKind(STANDARD_TYPE(StepVisual_PresentationStyleByContext)))
      return false;

    // get style select from father PSA
    if (aFatherPSA->NbStyles() > 0)
    {
      occ::handle<NCollection_HArray1<StepVisual_PresentationStyleSelect>> aFatherStyles =
        new NCollection_HArray1<StepVisual_PresentationStyleSelect>(1, aFatherPSA->NbStyles());
      int aSettingInd = 1;
      for (NCollection_HArray1<StepVisual_PresentationStyleSelect>::Iterator aFatherStyleIter(
             aFatherPSA->Styles()->Array1());
           aFatherStyleIter.More();
           aFatherStyleIter.Next())
      {
        StepVisual_PresentationStyleSelect        aPSS;
        const StepVisual_PresentationStyleSelect& anOlDPSS = aFatherStyleIter.Value();
        if (!anOlDPSS.PointStyle().IsNull())
          aPSS.SetValue(anOlDPSS.PointStyle());
        else if (!anOlDPSS.CurveStyle().IsNull())
          aPSS.SetValue(anOlDPSS.CurveStyle());
        else if (!anOlDPSS.SurfaceStyleUsage().IsNull())
          aPSS.SetValue(anOlDPSS.SurfaceStyleUsage());
        else
        {
          anIsFound = false;
          break;
        }
        aFatherStyles->SetValue(aSettingInd++, anOlDPSS);
        anIsFound = true;
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

//=================================================================================================

static void MakeSTEPStyles(
  STEPConstruct_Styles& theStyles,
  const TopoDS_Shape&   theShape,
  const NCollection_IndexedDataMap<TopoDS_Shape, XCAFPrs_Style, TopTools_ShapeMapHasher>&
                                                          theSettings,
  occ::handle<StepVisual_StyledItem>&                     theOverride,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMap,
  const NCollection_DataMap<TopoDS_Shape, occ::handle<Standard_Transient>, TopTools_ShapeMapHasher>&
                                                                                 theMapCompMDGPR,
  NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>>& theDPDCs,
  NCollection_DataMap<gp_Pnt, occ::handle<Standard_Transient>>&                  theColRGBs,
  const occ::handle<XCAFDoc_ShapeTool>&                                          theShTool,
  const XCAFPrs_Style*                                                           theInherit,
  const bool                                                                     theIsComponent,
  const bool                                                                     theVisMaterialMode)
{
  // skip already processed shapes
  if (!theMap.Add(theShape))
    return;

  // check if shape has its own style (r inherits from ancestor)
  XCAFPrs_Style aStyle;
  if (theInherit)
    aStyle = *theInherit;
  if (theSettings.Contains(theShape))
  {
    const XCAFPrs_Style& anOwnStyle = theSettings.FindFromKey(theShape);
    if (!anOwnStyle.IsVisible())
      aStyle.SetVisibility(false);
    if (anOwnStyle.IsSetColorCurv())
      aStyle.SetColorCurv(anOwnStyle.GetColorCurv());
    if (anOwnStyle.IsSetColorSurf())
      aStyle.SetColorSurf(anOwnStyle.GetColorSurfRGBA());
    if (!anOwnStyle.Material().IsNull())
      aStyle.SetMaterial(anOwnStyle.Material());
  }

  // translate colors to STEP
  occ::handle<StepVisual_Colour> aSurfColor, aCurvColor;
  double                         aRenderTransp = 0.0;
  if (aStyle.IsSetColorSurf())
  {
    Quantity_ColorRGBA aSurfCol = aStyle.GetColorSurfRGBA();
    aRenderTransp               = 1.0 - aSurfCol.Alpha();
    aSurfColor = STEPConstruct_Styles::EncodeColor(aSurfCol.GetRGB(), theDPDCs, theColRGBs);
  }
  if (aStyle.IsSetColorCurv())
    aCurvColor = STEPConstruct_Styles::EncodeColor(aStyle.GetColorCurv(), theDPDCs, theColRGBs);

  bool aHasOwn = (!aSurfColor.IsNull() || !aCurvColor.IsNull() || !aStyle.IsVisible());

  // find target item and assign style to it
  occ::handle<StepVisual_StyledItem> aSTEPstyle = theOverride;
  if (aHasOwn)
  {
    if (theShape.ShapeType() != TopAbs_COMPOUND || theIsComponent)
    { // skip compounds, let subshapes inherit its colors
      TopLoc_Location                                       aLocation;
      NCollection_Sequence<occ::handle<Standard_Transient>> aSeqRI;
      int aNbEntities = FindEntities(theStyles.FinderProcess(), theShape, aLocation, aSeqRI);
      if (aNbEntities <= 0)
        Message::SendTrace() << "Warning: Cannot find RI for "
                             << theShape.TShape()->DynamicType()->Name() << "\n";
      // Get overridden style gka 10.06.03
      if (theIsComponent && aNbEntities > 0)
        getStyledItem(theShape, theShTool, theStyles, theOverride, theMapCompMDGPR);

      for (NCollection_Sequence<occ::handle<Standard_Transient>>::Iterator anEntIter(aSeqRI);
           anEntIter.More();
           anEntIter.Next())
      {
        const occ::handle<StepRepr_RepresentationItem>& anItem =
          occ::down_cast<StepRepr_RepresentationItem>(anEntIter.Value());
        occ::handle<StepVisual_PresentationStyleAssignment> aPSA;
        if (aStyle.IsVisible() || !aSurfColor.IsNull() || !aCurvColor.IsNull()
            || (theVisMaterialMode && !aStyle.Material().IsNull() && !aStyle.Material()->IsEmpty()))
        {
          STEPConstruct_RenderingProperties aRenderProps;
          if (theVisMaterialMode && !aStyle.Material().IsNull() && !aStyle.Material()->IsEmpty())
          {
            aRenderProps.Init(aStyle.Material());
          }
          else if (aRenderTransp > 0.0)
          {
            aRenderProps.Init(aStyle.GetColorSurfRGBA());
          }
          aPSA =
            theStyles.MakeColorPSA(anItem, aSurfColor, aCurvColor, aRenderProps, theIsComponent);
        }
        else
        {
          // default white color
          aSurfColor = STEPConstruct_Styles::EncodeColor(Quantity_Color(Quantity_NOC_WHITE),
                                                         theDPDCs,
                                                         theColRGBs);
          aPSA       = theStyles.MakeColorPSA(anItem,
                                        aSurfColor,
                                        aCurvColor,
                                        STEPConstruct_RenderingProperties(),
                                        theIsComponent);
          if (theIsComponent)
            setDefaultInstanceColor(theOverride, aPSA);

        } // end of component case

        aSTEPstyle = theStyles.AddStyle(anItem, aPSA, theOverride);
        aHasOwn    = false;
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
    MakeSTEPStyles(theStyles,
                   anIter.Value(),
                   theSettings,
                   aSTEPstyle,
                   theMap,
                   theMapCompMDGPR,
                   theDPDCs,
                   theColRGBs,
                   theShTool,
                   (aHasOwn ? &aStyle : nullptr),
                   false,
                   theVisMaterialMode);
  }
}

//=================================================================================================

bool STEPCAFControl_Writer::writeColors(const occ::handle<XSControl_WorkSession>& theWS,
                                        const NCollection_Sequence<TDF_Label>&    theLabels)
{
  if (theLabels.IsEmpty())
    return false;

  STEPConstruct_Styles                                                          Styles(theWS);
  NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>> DPDCs;
  NCollection_DataMap<gp_Pnt, occ::handle<Standard_Transient>>                  ColRGBs;
  // Iterate on requested shapes
  for (NCollection_Sequence<TDF_Label>::Iterator aLabelIter(theLabels); aLabelIter.More();
       aLabelIter.Next())
  {
    const TDF_Label aLabel = aLabelIter.Value();
    // Iterate on shapes in the document
    occ::handle<XCAFDoc_ShapeTool> aSTool = XCAFDoc_DocumentTool::ShapeTool(aLabel);
    // Skip assemblies: colors assigned to assemblies and their instances
    // are not supported (it is not clear how to encode that in STEP)
    if (XCAFDoc_ShapeTool::IsAssembly(aLabel))
    {
      Message::SendTrace() << "Warning: Cannot write color  for Assembly" << "\n";
      Message::SendTrace() << "Info: Check for colors assigned to components in assembly" << "\n";
      // PTV 22.01.2003 Write color for instances.
      NCollection_Sequence<TDF_Label> compLabels;
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
    bool         anIsComponent = aSTool->IsComponent(aLabel) || myPureRefLabels.IsBound(aLabel);
    TopoDS_Shape aTopSh        = aShape;
    occ::handle<StepRepr_RepresentationContext> aContext = Styles.FindContext(aShape);
    if (anIsComponent)
    {
      TDF_Label aTopShL = aSTool->FindShape(aShape, false);
      if (aTopShL.IsNull())
        continue;
      aTopSh   = aSTool->GetShape(aTopShL);
      aContext = Styles.FindContext(aTopSh);
    }
    if (aContext.IsNull())
      continue;

    // collect settings set on that label
    NCollection_IndexedDataMap<TopoDS_Shape, XCAFPrs_Style, TopTools_ShapeMapHasher> aSettings;
    NCollection_Sequence<TDF_Label>                                                  aSeq;
    aSeq.Append(aLabel);
    XCAFDoc_ShapeTool::GetSubShapes(aLabel, aSeq);
    bool anIsVisible = true;
    for (NCollection_Sequence<TDF_Label>::Iterator aSeqIter(aSeq); aSeqIter.More(); aSeqIter.Next())
    {
      const TDF_Label&   aSeqValue = aSeqIter.Value();
      XCAFPrs_Style      aStyle;
      Quantity_ColorRGBA aColor;
      if (aSeqValue == aLabel)
      {
        // check for invisible status of object on label
        if (!XCAFDoc_ColorTool::IsVisible(aSeqValue))
        {
          anIsVisible = false;
          aStyle.SetVisibility(false);
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
        occ::handle<XCAFDoc_VisMaterial> aVisMat =
          XCAFDoc_VisMaterialTool::GetShapeMaterial(aSeqValue);
        if (!aVisMat.IsNull() && !aVisMat->IsEmpty())
        {
          // only color can be stored in STEP
          aStyle.SetColorSurf(aVisMat->BaseColor());
        }
      }
      if (!aStyle.IsSetColorCurv() && !aStyle.IsSetColorSurf() && anIsVisible)
        continue;

      TopoDS_Shape   aSub      = XCAFDoc_ShapeTool::GetShape(aSeqValue);
      XCAFPrs_Style* aMapStyle = aSettings.ChangeSeek(aSub);
      if (aMapStyle == nullptr)
        aSettings.Add(aSub, aStyle);
      else
        *aMapStyle = aStyle;
    }

    if (aSettings.Extent() <= 0)
      continue;

    // iterate on subshapes and create STEP styles
    occ::handle<StepVisual_StyledItem>                     anOverride;
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMap;

    MakeSTEPStyles(Styles,
                   aShape,
                   aSettings,
                   anOverride,
                   aMap,
                   myMapCompMDGPR,
                   DPDCs,
                   ColRGBs,
                   aSTool,
                   nullptr,
                   anIsComponent,
                   GetVisualMaterialMode());

    const occ::handle<XSControl_TransferWriter>& aTW = theWS->TransferWriter();
    const occ::handle<Transfer_FinderProcess>&   aFP = aTW->FinderProcess();
    occ::handle<StepData_StepModel> aStepModel = occ::down_cast<StepData_StepModel>(aFP->Model());

    // create MDGPR and record it in model
    occ::handle<StepVisual_MechanicalDesignGeometricPresentationRepresentation> aMDGPR;

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
      occ::handle<TransferBRep_ShapeMapper> aMapper = TransferBRep::ShapeMapper(aFP, aShape);
      occ::handle<StepShape_ContextDependentShapeRepresentation> aCDSR;
      if (aFP->FindTypedTransient(aMapper,
                                  STANDARD_TYPE(StepShape_ContextDependentShapeRepresentation),
                                  aCDSR))
      {
        // create SDR for NAUO
        occ::handle<StepRepr_ProductDefinitionShape> aNullPDS; // important to be NULL
        Styles.CreateNAUOSRD(aContext, aCDSR, aNullPDS);

        // search for MDGPR of the component top-level shape
        if (myMapCompMDGPR.IsBound(aTopSh))
        {
          aMDGPR = occ::down_cast<StepVisual_MechanicalDesignGeometricPresentationRepresentation>(
            myMapCompMDGPR.Find(aTopSh));
        }
        else
        {
          aMDGPR = new StepVisual_MechanicalDesignGeometricPresentationRepresentation;
          occ::handle<TCollection_HAsciiString> aReprName = new TCollection_HAsciiString("");
          aMDGPR->SetName(aReprName);
          aMDGPR->SetContextOfItems(aContext);
          myMapCompMDGPR.Bind(aTopSh, aMDGPR);
        }
        occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> anOldItems =
          aMDGPR->Items();
        int oldLengthlen = 0;
        if (!anOldItems.IsNull())
          oldLengthlen = anOldItems->Length();
        const int aNbIt = oldLengthlen + Styles.NbStyles();
        if (!aNbIt)
          continue;
        occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> aNewItems =
          new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, aNbIt);
        int anElemInd = 1;
        for (int aStyleInd = 1; aStyleInd <= oldLengthlen; aStyleInd++)
        {
          aNewItems->SetValue(anElemInd++, anOldItems->Value(aStyleInd));
        }
        for (int aStyleInd = 1; aStyleInd <= Styles.NbStyles(); aStyleInd++)
        {
          aNewItems->SetValue(anElemInd++, Styles.Style(aStyleInd));
        }

        if (aNewItems->Length() > 0)
          aMDGPR->SetItems(aNewItems);
      } // end of work with CDSR
    }
    if (!anIsVisible)
    {
      // create invisibility item and refer for styledItem
      occ::handle<StepVisual_Invisibility> anInvisibility = new StepVisual_Invisibility();
      occ::handle<NCollection_HArray1<StepVisual_InvisibleItem>> anInvisibilitySeq =
        new NCollection_HArray1<StepVisual_InvisibleItem>(1, Styles.NbStyles());
      // put all style item into the harray
      for (int aStyleInd = 1; aStyleInd <= Styles.NbStyles(); aStyleInd++)
      {
        occ::handle<StepRepr_RepresentationItem> aStyledItm = Styles.Style(aStyleInd);
        StepVisual_InvisibleItem                 anInvItem;
        anInvItem.SetValue(aStyledItm);
        anInvisibilitySeq->SetValue(aStyleInd, anInvItem);
      }
      // set the invisibility of items
      anInvisibility->Init(anInvisibilitySeq);
      theWS->Model()->AddWithRefs(anInvisibility);
    }
  }
  return true;
}

//=================================================================================================

bool STEPCAFControl_Writer::writeNames(const occ::handle<XSControl_WorkSession>& theWS,
                                       const NCollection_Sequence<TDF_Label>&    theLabels) const
{
  if (theLabels.IsEmpty())
    return false;

  // get working data
  const occ::handle<XSControl_TransferWriter>& aTW = theWS->TransferWriter();
  const occ::handle<Transfer_FinderProcess>&   aFP = aTW->FinderProcess();

  // Iterate on requested shapes
  for (NCollection_Sequence<TDF_Label>::Iterator aLabelIter(theLabels); aLabelIter.More();
       aLabelIter.Next())
  {
    const TDF_Label& aLabel = aLabelIter.Value();
    // find target STEP entity for the current shape
    const TopoDS_Shape* pShape = myLabels.Seek(aLabel);
    if (!pShape)
      continue; // not recorded as translated, skip
    // get name
    occ::handle<TCollection_HAsciiString> aHName = new TCollection_HAsciiString;
    if (!GetLabelName(aLabel, aHName))
    {
      continue;
    }
    const TopoDS_Shape&                                        aShape = *pShape;
    occ::handle<StepShape_ShapeDefinitionRepresentation>       aSDR;
    occ::handle<StepShape_ContextDependentShapeRepresentation> aCDSR;
    bool isComponent = XCAFDoc_ShapeTool::IsComponent(aLabel) || myPureRefLabels.IsBound(aLabel);
    occ::handle<TransferBRep_ShapeMapper> aMapper = TransferBRep::ShapeMapper(aFP, aShape);
    if (isComponent
        && aFP->FindTypedTransient(aMapper,
                                   STANDARD_TYPE(StepShape_ContextDependentShapeRepresentation),
                                   aCDSR))
    {
      occ::handle<StepRepr_ProductDefinitionShape> aPDS = aCDSR->RepresentedProductRelation();
      occ::handle<StepBasic_ProductDefinitionRelationship> aNAUO =
        aPDS->Definition().ProductDefinitionRelationship();
      if (!aNAUO.IsNull())
        aNAUO->SetName(aHName);
      TopoDS_Shape anInternalAssembly;
      if (myPureRefLabels.Find(aLabel, anInternalAssembly))
      {
        occ::handle<TransferBRep_ShapeMapper> aMapperOfInternalShape =
          TransferBRep::ShapeMapper(aFP, anInternalAssembly);
        aFP->FindTypedTransient(aMapperOfInternalShape,
                                STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation),
                                aSDR);
      }
    }
    if (!aSDR.IsNull()
        || (aCDSR.IsNull()
            && aFP->FindTypedTransient(aMapper,
                                       STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation),
                                       aSDR)))
    {
      // set the name to the PRODUCT
      occ::handle<StepRepr_PropertyDefinition> aPropD = aSDR->Definition().PropertyDefinition();
      if (aPropD.IsNull())
        continue;
      occ::handle<StepBasic_ProductDefinition> aPD = aPropD->Definition().ProductDefinition();
      if (aPD.IsNull())
        continue;
      occ::handle<StepBasic_Product> aProd = aPD->Formation()->OfProduct();

      aProd->SetId(aHName);
      aProd->SetName(aHName);
    }
    else
    {
      Message::SendTrace() << "Warning: Cannot find RI for "
                           << aShape.TShape()->DynamicType()->Name() << "\n";
      continue;
    }
  }
  return true;
}

//=================================================================================================

bool STEPCAFControl_Writer::writeMetadata(const occ::handle<XSControl_WorkSession>& theWS,
                                          const NCollection_Sequence<TDF_Label>&    theLabels) const
{
  if (theLabels.IsEmpty())
  {
    return false;
  }

  // Iterate on requested shapes.
  for (NCollection_Sequence<TDF_Label>::Iterator aLabelIter(theLabels); aLabelIter.More();
       aLabelIter.Next())
  {
    writeMetadataForLabel(theWS, aLabelIter.Value());
  }

  return true;
}

//=================================================================================================

bool STEPCAFControl_Writer::writeMetadataForLabel(const occ::handle<XSControl_WorkSession>& theWS,
                                                  const TDF_Label& theLabel) const
{
  if (theLabel.IsNull())
  {
    return false;
  }

  // Get working data:
  const occ::handle<XSControl_TransferWriter>& aTW = theWS->TransferWriter();
  const occ::handle<Transfer_FinderProcess>&   aFP = aTW->FinderProcess();
  const occ::handle<StepData_StepModel>&       aModel =
    occ::down_cast<StepData_StepModel>(theWS->Model());

  // Check if label has metadata (NamedData):
  occ::handle<TDataStd_NamedData> aNamedData;
  if (!theLabel.FindAttribute(TDataStd_NamedData::GetID(), aNamedData))
    return false; // No metadata on this label

  // Find target STEP entity for the current shape:
  const TopoDS_Shape* pShape = myLabels.Seek(theLabel);
  if (!pShape)
    return false; // Not recorded as translated, skip

  const TopoDS_Shape&                                  aShape = *pShape;
  occ::handle<StepShape_ShapeDefinitionRepresentation> aSDR;
  const occ::handle<TransferBRep_ShapeMapper> aMapper = TransferBRep::ShapeMapper(aFP, aShape);
  if (!aFP->FindTypedTransient(aMapper,
                               STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation),
                               aSDR))
  {
    return false; // Cannot find STEP representation
  }

  // Get the product definition from the shape definition representation:
  const occ::handle<StepRepr_PropertyDefinition> aPropDef = aSDR->Definition().PropertyDefinition();
  if (aPropDef.IsNull())
    return false;
  const occ::handle<StepBasic_ProductDefinition> aProdDef =
    aPropDef->Definition().ProductDefinition();
  if (aProdDef.IsNull())
    return false;

  // Export string metadata.
  const NCollection_DataMap<TCollection_ExtendedString, TCollection_ExtendedString>& aStringMap =
    aNamedData->GetStringsContainer();
  for (NCollection_DataMap<TCollection_ExtendedString, TCollection_ExtendedString>::Iterator anIter(
         aStringMap);
       anIter.More();
       anIter.Next())
  {
    const TCollection_ExtendedString& aKey   = anIter.Key();
    const TCollection_ExtendedString& aValue = anIter.Value();

    // Create descriptive representation item for the value.
    const occ::handle<StepRepr_DescriptiveRepresentationItem> aDescrItem =
      new StepRepr_DescriptiveRepresentationItem();
    const occ::handle<TCollection_HAsciiString> aItemName  = new TCollection_HAsciiString(aKey);
    const occ::handle<TCollection_HAsciiString> aItemValue = new TCollection_HAsciiString(aValue);
    aDescrItem->SetName(aItemName);
    aDescrItem->SetDescription(aItemValue);

    writeMetadataRepresentationItem(aKey, aModel, aSDR, aProdDef, aDescrItem);
  }

  // Export integer metadata.
  const NCollection_DataMap<TCollection_ExtendedString, int>& aIntMap =
    aNamedData->GetIntegersContainer();
  for (NCollection_DataMap<TCollection_ExtendedString, int>::Iterator anIter(aIntMap);
       anIter.More();
       anIter.Next())
  {
    const TCollection_ExtendedString& aKey   = anIter.Key();
    const int                         aValue = anIter.Value();
    // Create integer representation item for the value.
    const occ::handle<StepRepr_IntegerRepresentationItem> aIntItem =
      new StepRepr_IntegerRepresentationItem();
    const occ::handle<TCollection_HAsciiString> aItemName = new TCollection_HAsciiString(aKey);
    aIntItem->Init(aItemName, aValue);

    writeMetadataRepresentationItem(aKey, aModel, aSDR, aProdDef, aIntItem);
  }

  // Export real metadata.
  const NCollection_DataMap<TCollection_ExtendedString, double>& aRealMap =
    aNamedData->GetRealsContainer();
  for (NCollection_DataMap<TCollection_ExtendedString, double>::Iterator anIter(aRealMap);
       anIter.More();
       anIter.Next())
  {
    const TCollection_ExtendedString& aKey   = anIter.Key();
    const double                      aValue = anIter.Value();
    // Create real representation item for the value.
    const occ::handle<StepRepr_RealRepresentationItem> aRealItem =
      new StepRepr_RealRepresentationItem();
    const occ::handle<TCollection_HAsciiString> aItemName = new TCollection_HAsciiString(aKey);
    aRealItem->Init(aItemName, aValue);

    writeMetadataRepresentationItem(aKey, aModel, aSDR, aProdDef, aRealItem);
  }

  // Process label children recursively:
  if (theLabel.HasChild())
  {
    for (int aChildInd = 1; aChildInd <= theLabel.NbChildren(); aChildInd++)
    {
      const TDF_Label& aChildLabel = theLabel.FindChild(aChildInd);
      writeMetadataForLabel(theWS, aChildLabel);
    }
  }

  return true;
}

//=================================================================================================

void STEPCAFControl_Writer::writeMetadataRepresentationItem(
  const TCollection_AsciiString&                              theKey,
  const occ::handle<StepData_StepModel>&                      theModel,
  const occ::handle<StepShape_ShapeDefinitionRepresentation>& theShapeDefRep,
  const occ::handle<StepBasic_ProductDefinition>&             theProdDef,
  const occ::handle<StepRepr_RepresentationItem>&             theItem) const
{
  // Empty string to use for empty values:
  const occ::handle<TCollection_HAsciiString> anEmptyStr = new TCollection_HAsciiString("");

  // Create property_definition:
  const occ::handle<StepRepr_PropertyDefinition> aMetaPropDef = new StepRepr_PropertyDefinition();
  const occ::handle<TCollection_HAsciiString>    aPropName = new TCollection_HAsciiString(theKey);
  const occ::handle<TCollection_HAsciiString>    aPropDesc =
    new TCollection_HAsciiString("user defined attribute");
  aMetaPropDef->SetName(aPropName);
  aMetaPropDef->SetDescription(aPropDesc);

  // Create a general_property:
  const occ::handle<StepBasic_GeneralProperty> aGeneralProp = new StepBasic_GeneralProperty();
  aGeneralProp->SetId(anEmptyStr);
  aGeneralProp->SetName(aPropName);
  aGeneralProp->SetDescription(anEmptyStr);

  // Create a general_property_association:
  const occ::handle<StepBasic_GeneralPropertyAssociation> aGeneralPropAssoc =
    new StepBasic_GeneralPropertyAssociation();
  aGeneralPropAssoc->SetName(anEmptyStr);
  aGeneralPropAssoc->SetDescription(anEmptyStr);
  aGeneralPropAssoc->SetPropertyDefinition(aMetaPropDef);
  aGeneralPropAssoc->SetGeneralProperty(aGeneralProp);

  // Set the definition to point to the product definition:
  StepRepr_CharacterizedDefinition aCharDef;
  aCharDef.SetValue(theProdDef);
  aMetaPropDef->SetDefinition(aCharDef);

  // Create property_definition_representation:
  const occ::handle<StepRepr_PropertyDefinitionRepresentation> aPropDefRepr =
    new StepRepr_PropertyDefinitionRepresentation();
  StepRepr_RepresentedDefinition aRepDef;
  aRepDef.SetValue(aMetaPropDef);
  aPropDefRepr->SetDefinition(aRepDef);

  // Create representation with descriptive_representation_item:
  const occ::handle<StepRepr_Representation>  aRepr     = new StepRepr_Representation();
  const occ::handle<TCollection_HAsciiString> aReprName = new TCollection_HAsciiString(theKey);
  aRepr->SetName(aReprName);

  // Add item to representation:
  const occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> aItems =
    new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, 1);
  aItems->SetValue(1, theItem);
  aRepr->SetItems(aItems);

  // Set representation context (reuse from shape representation):
  const occ::handle<StepRepr_RepresentationContext> aRC =
    theShapeDefRep->UsedRepresentation()->ContextOfItems();
  aRepr->SetContextOfItems(aRC);

  aPropDefRepr->SetUsedRepresentation(aRepr);

  // Add entities to the model:
  theModel->AddWithRefs(aMetaPropDef);
  theModel->AddWithRefs(aGeneralProp);
  theModel->AddWithRefs(aGeneralPropAssoc);
  theModel->AddWithRefs(aPropDefRepr);
  theModel->AddWithRefs(aRepr);
  theModel->AddWithRefs(theItem);
}

//=================================================================================================

static bool WritePropsForLabel(const occ::handle<XSControl_WorkSession>&           theWS,
                               const NCollection_DataMap<TDF_Label, TopoDS_Shape>& theLabels,
                               const TDF_Label&                                    theLabel,
                               const char* const                                   theIsMulti)
{
  if (theLabel.IsNull())
    return false;

  STEPConstruct_ValidationProps aProps(theWS);

  TopoDS_Shape aShape = XCAFDoc_ShapeTool::GetShape(theLabel);
  if (aShape.IsNull())
    return false;

  if (!theIsMulti || XCAFDoc_ShapeTool::IsAssembly(theLabel))
  {
    // write area
    occ::handle<XCAFDoc_Area> anArea;
    theLabel.FindAttribute(XCAFDoc_Area::GetID(), anArea);
    if (!anArea.IsNull())
    {
      aProps.AddArea(aShape, anArea->Get());
    }
    // write volume
    occ::handle<XCAFDoc_Volume> aVolume;
    theLabel.FindAttribute(XCAFDoc_Volume::GetID(), aVolume);
    if (!aVolume.IsNull())
    {
      aProps.AddVolume(aShape, aVolume->Get());
    }
  }
  // write centroid
  occ::handle<XCAFDoc_Centroid> aCentroid;
  theLabel.FindAttribute(XCAFDoc_Centroid::GetID(), aCentroid);
  if (!aCentroid.IsNull())
  {
    aProps.AddCentroid(aShape, aCentroid->Get());
  }

  if (XCAFDoc_ShapeTool::IsCompound(theLabel) || XCAFDoc_ShapeTool::IsAssembly(theLabel))
  {
    if (theLabel.HasChild())
    {
      for (int aChildInd = 1; aChildInd <= theLabel.NbChildren(); aChildInd++)
      {
        WritePropsForLabel(theWS, theLabels, theLabel.FindChild(aChildInd), theIsMulti);
      }
    }
  }

  return true;
}

//=================================================================================================

bool STEPCAFControl_Writer::writeValProps(const occ::handle<XSControl_WorkSession>& theWS,
                                          const NCollection_Sequence<TDF_Label>&    theLabels,
                                          const char* const theIsMulti) const
{
  if (theLabels.IsEmpty())
    return false;

  // Iterate on requested shapes
  for (NCollection_Sequence<TDF_Label>::Iterator aLabelIter(theLabels); aLabelIter.More();
       aLabelIter.Next())
  {
    const TDF_Label& aLabel = aLabelIter.Value();

    WritePropsForLabel(theWS, myLabels, aLabel, theIsMulti);
  }

  return true;
}

//=================================================================================================

bool STEPCAFControl_Writer::writeLayers(const occ::handle<XSControl_WorkSession>& theWS,
                                        const NCollection_Sequence<TDF_Label>&    theLabels) const
{

  if (theLabels.IsEmpty())
    return false;

  // get working data
  const occ::handle<Interface_InterfaceModel>& aModel = theWS->Model();
  const occ::handle<XSControl_TransferWriter>& aTW    = theWS->TransferWriter();
  const occ::handle<Transfer_FinderProcess>&   aFP    = aTW->FinderProcess();
  NCollection_Sequence<TDF_Label>              aGlobalLayerLS;
  // Iterate on requested shapes collect Tools
  for (NCollection_Map<TDF_Label>::Iterator aLabelIter(myRootLabels); aLabelIter.More();
       aLabelIter.Next())
  {
    const TDF_Label&                aLabel = aLabelIter.Value();
    occ::handle<XCAFDoc_LayerTool>  aLTool = XCAFDoc_DocumentTool::LayerTool(aLabel);
    NCollection_Sequence<TDF_Label> aLayerLS;
    aLTool->GetLayerLabels(aLayerLS);
    aGlobalLayerLS.Append(aLayerLS);
  }
  for (NCollection_Sequence<TDF_Label>::Iterator aLayerIter(aGlobalLayerLS); aLayerIter.More();
       aLayerIter.Next())
  {
    const TDF_Label& aLayerL = aLayerIter.Value();
    // get labels of shapes in that layer
    NCollection_Sequence<TDF_Label> aShapeLabels;
    XCAFDoc_LayerTool::GetShapesOfLayer(aLayerL, aShapeLabels);
    if (aShapeLabels.IsEmpty())
      continue;

    // name of layer: if not set, is considered as being empty
    occ::handle<TCollection_HAsciiString> aHName = new TCollection_HAsciiString;
    GetLabelName(aLayerL, aHName);

    // Find target STEP entity for each shape and add to StepVisual_PresentationLayerAssignment
    // items.
    NCollection_Sequence<occ::handle<Standard_Transient>> aSeqRI;
    for (NCollection_Sequence<TDF_Label>::Iterator aShapeIter(aShapeLabels); aShapeIter.More();
         aShapeIter.Next())
    {
      const TDF_Label& aShLabel = aShapeIter.Value();
      if (aShLabel.IsNull())
        continue;

      // there is no way to assign layer to instance in STEP
      if (XCAFDoc_ShapeTool::IsAssembly(aShLabel) || XCAFDoc_ShapeTool::IsReference(aShLabel))
      {
        continue;
      }
      // check that the shape is one of (uub)labels written during current transfer
      bool anIsWritten = false;
      for (NCollection_Sequence<TDF_Label>::Iterator aLabelIter(theLabels); aLabelIter.More();
           aLabelIter.Next())
      {
        if (aShLabel.IsDescendant(aLabelIter.Value()))
        {
          anIsWritten = true;
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
      int             aNb = FindEntities(aFP, anOneShape, aLoc, aSeqRI);
      if (aNb <= 0)
      {
        Message::SendTrace() << "Warning: Cannot find RI for "
                             << anOneShape.TShape()->DynamicType()->Name() << "\n";
      }
    }
    if (aSeqRI.IsEmpty())
      continue;

    // analyze visibility
    occ::handle<StepVisual_PresentationLayerAssignment> aStepLayerAs =
      new StepVisual_PresentationLayerAssignment;
    occ::handle<TCollection_HAsciiString> aDescr;
    occ::handle<TDataStd_UAttribute>      aUAttr;
    bool                                  isLinv = false;
    if (aLayerL.FindAttribute(XCAFDoc::InvisibleGUID(), aUAttr))
    {
      aDescr = new TCollection_HAsciiString("invisible");
      Message::SendTrace() << "\tLayer \"" << aHName->String().ToCString() << "\" is invisible"
                           << "\n";
      isLinv = true;
    }
    else
    {
      aDescr = new TCollection_HAsciiString("visible");
    }

    // create layer entity
    int                                                      anSetStyleInd = 1;
    occ::handle<NCollection_HArray1<StepVisual_LayeredItem>> aHArrayOfLItem =
      new NCollection_HArray1<StepVisual_LayeredItem>(1, aSeqRI.Length());
    for (NCollection_Sequence<occ::handle<Standard_Transient>>::Iterator aRIIter(aSeqRI);
         aRIIter.More();
         aRIIter.Next())
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
      occ::handle<NCollection_HArray1<StepVisual_InvisibleItem>> aHInvsblItm =
        new NCollection_HArray1<StepVisual_InvisibleItem>(1, 1);
      StepVisual_InvisibleItem aInvIt;
      aInvIt.SetValue(aStepLayerAs);
      aHInvsblItm->SetValue(1, aInvIt);

      occ::handle<StepVisual_Invisibility> aInvsblt = new StepVisual_Invisibility();
      aInvsblt->Init(aHInvsblItm);
      aModel->AddWithRefs(aInvsblt);
    }
  }
  return true;
}

//=================================================================================================

static bool getSHUOstyle(const TDF_Label& theSHUOlab, XCAFPrs_Style& theSHUOstyle)
{
  Quantity_Color aColor;
  if (!XCAFDoc_ColorTool::IsVisible(theSHUOlab))
  {
    theSHUOstyle.SetVisibility(false);
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
      occ::handle<XCAFDoc_VisMaterial> aVisMat =
        XCAFDoc_VisMaterialTool::GetShapeMaterial(theSHUOlab);
      if (!aVisMat.IsNull() && !aVisMat->IsEmpty())
      {
        // only color can be stored in STEP
        theSHUOstyle.SetColorSurf(aVisMat->BaseColor());
      }
    }
  }
  if (!theSHUOstyle.IsSetColorCurv() && !theSHUOstyle.IsSetColorSurf() && theSHUOstyle.IsVisible())
  {
    return false;
  }
  return true;
}

//=================================================================================================

static bool getProDefinitionOfNAUO(const occ::handle<XSControl_WorkSession>&          theWS,
                                   const TopoDS_Shape&                                theShape,
                                   occ::handle<StepBasic_ProductDefinition>&          thePD,
                                   occ::handle<StepRepr_NextAssemblyUsageOccurrence>& theNAUO,
                                   bool                                               theIsRelating)
{
  if (theShape.IsNull())
    return false;
  // get CDSR
  const occ::handle<XSControl_TransferWriter>&               aTW = theWS->TransferWriter();
  const occ::handle<Transfer_FinderProcess>&                 aFP = aTW->FinderProcess();
  occ::handle<StepShape_ContextDependentShapeRepresentation> aCDSR;
  occ::handle<TransferBRep_ShapeMapper> aMapper = TransferBRep::ShapeMapper(aFP, theShape);
  if (!aFP->FindTypedTransient(aMapper,
                               STANDARD_TYPE(StepShape_ContextDependentShapeRepresentation),
                               aCDSR))
  {
    return false;
  }
  // get PDS of NAUO
  occ::handle<StepRepr_ProductDefinitionShape> aPDS = aCDSR->RepresentedProductRelation();
  if (aPDS.IsNull())
    return false;
  // get the NAUO entity
  Interface_Graph aGraph = theWS->HGraph()->Graph();
  for (Interface_EntityIterator aShareds = aGraph.Shareds(aPDS); aShareds.More(); aShareds.Next())
  {
    theNAUO = occ::down_cast<StepRepr_NextAssemblyUsageOccurrence>(aShareds.Value());
    if (!theNAUO.IsNull())
    {
      break;
    }
  }
  if (theNAUO.IsNull())
    return false;
  // get Relatinf or Related product definition
  if (!theIsRelating)
    thePD = theNAUO->RelatedProductDefinition();
  else
    thePD = theNAUO->RelatingProductDefinition();
  if (thePD.IsNull())
    return false;
  return true;
}

//=================================================================================================

static bool writeSHUO(const occ::handle<XCAFDoc_GraphNode>&                 theSHUO,
                      const occ::handle<XSControl_WorkSession>&             theWS,
                      occ::handle<StepRepr_SpecifiedHigherUsageOccurrence>& theTopSHUO,
                      TopoDS_Shape&                                         theNAUOShape,
                      occ::handle<StepBasic_ProductDefinition>&             theRelatingPD,
                      bool&                                                 theIsDeepest)
{
  // set the ProductDefinitionRelationship descriptin information as empty strings.
  occ::handle<TCollection_HAsciiString> anEmptyString = new TCollection_HAsciiString("");

  NCollection_Sequence<TDF_Label> aNextUsageLabs;
  XCAFDoc_ShapeTool::GetSHUONextUsage(theSHUO->Label(), aNextUsageLabs);
  occ::handle<XCAFDoc_GraphNode> aNuSHUO;
  if (theTopSHUO.IsNull())
  {
    // the top SHUO
    if (aNextUsageLabs.Length() < 1)
      return false;
    XCAFDoc_ShapeTool::GetSHUO(aNextUsageLabs.Value(1), aNuSHUO);
    if (aNuSHUO.IsNull())
      return false;
    // get relating product definition
    TopoDS_Shape aTopCompShape = XCAFDoc_ShapeTool::GetShape(theSHUO->Label().Father());
    occ::handle<StepRepr_NextAssemblyUsageOccurrence> aRelatingNAUO;
    if (!getProDefinitionOfNAUO(theWS, aTopCompShape, theRelatingPD, aRelatingNAUO, true))
    {
      return false;
    }
    // get related product definition
    TopoDS_Shape aNUShape = XCAFDoc_ShapeTool::GetShape(aNuSHUO->Label().Father());
    occ::handle<StepBasic_ProductDefinition>          aRelatedPD;
    occ::handle<StepRepr_NextAssemblyUsageOccurrence> aRelatedNAUO;
    if (!getProDefinitionOfNAUO(theWS, aNUShape, aRelatedPD, aRelatedNAUO, false))
    {
      return false;
    }

    theTopSHUO = new StepRepr_SpecifiedHigherUsageOccurrence;
    // create deepest shuo EmptyString
    theTopSHUO->Init(/*id*/ anEmptyString,
                     /*name*/ anEmptyString,
                     /*no description*/ false,
                     /*description*/ anEmptyString,
                     theRelatingPD,
                     aRelatedPD,
                     /*no ACURefDesignator*/ false,
                     /*ACURefDesignator*/ anEmptyString,
                     /*upper_usage*/ aRelatingNAUO,
                     /*next_usage*/ aRelatedNAUO);
    // write the other SHUO.
    if (!writeSHUO(aNuSHUO, theWS, theTopSHUO, theNAUOShape, theRelatingPD, theIsDeepest))
    {
      theTopSHUO.Nullify();
      return false;
    }

    return true;
  }
  //   occ::handle<XCAFDoc_GraphNode> NuSHUO;
  if (aNextUsageLabs.Length() > 0)
  {
    // store SHUO recursive
    if (aNextUsageLabs.Length() > 1)
      Message::SendTrace() << "Warning: store only one next_usage of current SHUO" << "\n";
    XCAFDoc_ShapeTool::GetSHUO(aNextUsageLabs.Value(1), aNuSHUO);
    occ::handle<StepRepr_SpecifiedHigherUsageOccurrence> aNUEntSHUO =
      new StepRepr_SpecifiedHigherUsageOccurrence;
    if (!writeSHUO(aNuSHUO, theWS, aNUEntSHUO, theNAUOShape, theRelatingPD, theIsDeepest))
      return false;

    // store the deepest SHUO to the dociment
    TopoDS_Shape aNUSh, aUUSh;
    aNUSh = XCAFDoc_ShapeTool::GetShape(aNuSHUO->Label().Father());
    aUUSh = XCAFDoc_ShapeTool::GetShape(theSHUO->Label().Father());
    // get relating PD with upper_usage and related PD with next_usage
    occ::handle<StepBasic_ProductDefinition>
      nullPD; // no need to use,case have shared <theRelatingPD>
    occ::handle<StepBasic_ProductDefinition>          aRelatedPD;
    occ::handle<StepRepr_NextAssemblyUsageOccurrence> UUNAUO, NUNAUO;
    if (!getProDefinitionOfNAUO(theWS, aUUSh, nullPD, UUNAUO, true)
        || !getProDefinitionOfNAUO(theWS, aNUSh, aRelatedPD, NUNAUO, false))
    {
      Message::SendTrace() << "Warning: cannot get related or relating PD" << "\n";
      return false;
    }
    aNUEntSHUO->Init(/*id*/ anEmptyString,
                     /*name*/ anEmptyString,
                     /*no description*/ false,
                     /*description*/ anEmptyString,
                     theRelatingPD,
                     aRelatedPD,
                     /*no ACURefDesignator*/ false,
                     /*ACURefDesignator*/ anEmptyString,
                     /*upper_usage*/ theTopSHUO,
                     /*next_usage*/ NUNAUO);
    if (theIsDeepest)
    {
      theIsDeepest = false;
    }
    theWS->Model()->AddWithRefs(aNUEntSHUO);
    return true;
  } // end of recurse storing

  // get shape
  TDF_Label aShapeL = theSHUO->Label().Father();
  theNAUOShape      = XCAFDoc_ShapeTool::GetShape(aShapeL);
  // return to the deepest level from SHUO shape level
  // it is because SHUO is attribute on deep level and shape level.
  theIsDeepest = true;
  return true;
}

//=================================================================================================

static bool createSHUOStyledItem(
  const XCAFPrs_Style&                                theStyle,
  const occ::handle<StepRepr_ProductDefinitionShape>& thePDS,
  const occ::handle<XSControl_WorkSession>&           theWS,
  const TopoDS_Shape&                                 theShape,
  const occ::handle<XCAFDoc_ShapeTool>&               theSTool,
  NCollection_DataMap<TopoDS_Shape, occ::handle<Standard_Transient>, TopTools_ShapeMapHasher>&
    theMapCompMDGPR)
{
  // create styled item for the indicated SHUO and store to the model
  STEPConstruct_Styles aStyles(theWS);
  // translate colors to STEP
  occ::handle<StepVisual_Colour> aSurfColor, aCurvColor;
  double                         aRenderTransp = 0.0;
  if (theStyle.IsSetColorSurf())
  {
    Quantity_ColorRGBA aSurfCol = theStyle.GetColorSurfRGBA();
    aRenderTransp               = 1.0 - aSurfCol.Alpha();
    aSurfColor                  = STEPConstruct_Styles::EncodeColor(aSurfCol.GetRGB());
  }
  if (theStyle.IsSetColorCurv())
    aCurvColor = STEPConstruct_Styles::EncodeColor(theStyle.GetColorCurv());
  bool                                     isComponent = true; // cause need to get PSBC
  occ::handle<StepRepr_RepresentationItem> anItem;
  // set default color for invisible SHUO.
  bool isSetDefaultColor = false;
  if (aSurfColor.IsNull() && aCurvColor.IsNull() && !theStyle.IsVisible())
  {
    aSurfColor        = STEPConstruct_Styles::EncodeColor(Quantity_Color(Quantity_NOC_WHITE));
    isSetDefaultColor = true;
  }
  STEPConstruct_RenderingProperties aRenderProps;
  if (aRenderTransp > 0.0)
  {
    aRenderProps.Init(theStyle.GetColorSurfRGBA());
  }
  occ::handle<StepVisual_PresentationStyleAssignment> aPSA =
    aStyles.MakeColorPSA(anItem, aSurfColor, aCurvColor, aRenderProps, isComponent);
  occ::handle<StepVisual_StyledItem> anOverride; // null styled item

  // find the repr item of the shape
  const occ::handle<XSControl_TransferWriter>& aTW = theWS->TransferWriter();
  const occ::handle<Transfer_FinderProcess>&   aFP = aTW->FinderProcess();
  occ::handle<StepData_StepModel> aStepModel    = occ::down_cast<StepData_StepModel>(aFP->Model());
  occ::handle<TransferBRep_ShapeMapper> aMapper = TransferBRep::ShapeMapper(aFP, theShape);
  occ::handle<StepShape_ContextDependentShapeRepresentation> aCDSR;
  aFP->FindTypedTransient(aMapper,
                          STANDARD_TYPE(StepShape_ContextDependentShapeRepresentation),
                          aCDSR);
  if (aCDSR.IsNull())
    return false;
  // find context
  occ::handle<StepRepr_RepresentationContext> aContext = aStyles.FindContext(theShape);
  TopoDS_Shape                                aTopSh   = theShape;
  if (aContext.IsNull())
  {
    TDF_Label aTopShL = theSTool->FindShape(theShape, false);
    if (aTopShL.IsNull())
      return false;
    aTopSh   = XCAFDoc_ShapeTool::GetShape(aTopShL);
    aContext = aStyles.FindContext(aTopSh);
  }
  if (aContext.IsNull())
    return false;
  // get representation item of the shape
  TopLoc_Location                                       aLocation;
  NCollection_Sequence<occ::handle<Standard_Transient>> aSeqRI;
  FindEntities(aFP, theShape, aLocation, aSeqRI);
  if (aSeqRI.Length() <= 0)
    Message::SendTrace() << "Warning: Cannot find RI for "
                         << theShape.TShape()->DynamicType()->Name() << "\n";
  anItem = occ::down_cast<StepRepr_RepresentationItem>(aSeqRI(1));
  // get overridden styled item
  getStyledItem(theShape, theSTool, aStyles, anOverride, theMapCompMDGPR);

  // get STEP STYLED ITEM
  occ::handle<StepVisual_StyledItem> aSTEPstyle = aStyles.AddStyle(anItem, aPSA, anOverride);
  // create SR, SDR and all necessary references between them and ST, PDS, PSBC, GRC
  aStyles.CreateNAUOSRD(aContext, aCDSR, thePDS);

  // add step styled item of SHUO to the model
  // do it by additing styled item to the MDGPR
  if (!aTopSh.IsNull() && !theMapCompMDGPR.IsBound(aTopSh))
  {
    // create MDGPR and record it in model
    Message::SendTrace() << "Warning: " << __FILE__ << ": Create new MDGPR for SHUO instance"
                         << "\n";
    occ::handle<StepVisual_MechanicalDesignGeometricPresentationRepresentation> aMDGPR;
    aStyles.CreateMDGPR(aContext, aMDGPR, aStepModel);
    if (!aMDGPR.IsNull())
      theMapCompMDGPR.Bind(aTopSh, aMDGPR);
  }
  else if (!aTopSh.IsNull() && theMapCompMDGPR.IsBound(aTopSh))
  {
    // get MDGPR of the top-level shape
    occ::handle<StepVisual_PresentationRepresentation> aMDGPR =
      occ::down_cast<StepVisual_PresentationRepresentation>(theMapCompMDGPR.Find(aTopSh));
    // get old styled items to not lose it
    occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> anOldItems =
      aMDGPR->Items();
    int anOldLengthlen = 0;
    if (!anOldItems.IsNull())
      anOldLengthlen = anOldItems->Length();
    // create new array of styled items by an olds and new one
    occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> aNewItems =
      new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, anOldLengthlen + 1);
    int aSetStyleInd = 1;
    for (NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>::Iterator aStyleIter(
           anOldItems->Array1());
         aStyleIter.More();
         aStyleIter.Next())
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
    Message::SendTrace() << "Warning: " << __FILE__
                         << ": adds styled item of SHUO as root, cause cannot find MDGPR" << "\n";
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
    occ::handle<StepVisual_Invisibility> aInvsblt = new StepVisual_Invisibility();
    occ::handle<NCollection_HArray1<StepVisual_InvisibleItem>> aHInvsblItm =
      new NCollection_HArray1<StepVisual_InvisibleItem>(1, 1);
    // put all style item into the harray
    StepVisual_InvisibleItem anInvItem;
    anInvItem.SetValue(aSTEPstyle);
    aHInvsblItm->SetValue(1, anInvItem);
    aInvsblt->Init(aHInvsblItm);
    theWS->Model()->AddWithRefs(aInvsblt);
  }

  return true;
}

//=================================================================================================

bool STEPCAFControl_Writer::writeSHUOs(const occ::handle<XSControl_WorkSession>& theWS,
                                       const NCollection_Sequence<TDF_Label>&    theLabels)
{
  if (theLabels.IsEmpty())
    return false;

  // map of transferred SHUO
  NCollection_Map<occ::handle<Standard_Transient>> aMapOfMainSHUO;
  // Iterate on requested shapes
  for (NCollection_Sequence<TDF_Label>::Iterator aLabelIter(theLabels); aLabelIter.More();
       aLabelIter.Next())
  {
    const TDF_Label& aLabel = aLabelIter.Value();
    if (!myLabels.IsBound(aLabel))
      continue; // not recorded as translated, skip
    if (!XCAFDoc_ShapeTool::IsAssembly(aLabel))
    {
      continue;
    }
    NCollection_Sequence<TDF_Label> aLabelSeq;
    XCAFDoc_ShapeTool::GetComponents(aLabel, aLabelSeq);
    // iterates on components of assembly
    for (NCollection_Sequence<TDF_Label>::Iterator aCompIter(aLabelSeq); aCompIter.More();
         aCompIter.Next())
    {
      const TDF_Label&                                 aCompL = aCompIter.Value();
      NCollection_Sequence<occ::handle<TDF_Attribute>> anAttrSeq;
      XCAFDoc_ShapeTool::GetAllComponentSHUO(aCompL, anAttrSeq);
      // work with SHUO
      for (NCollection_Sequence<occ::handle<TDF_Attribute>>::Iterator anAttrIter(anAttrSeq);
           anAttrIter.More();
           anAttrIter.Next())
      {
        occ::handle<XCAFDoc_GraphNode> aSHUO =
          occ::down_cast<XCAFDoc_GraphNode>(anAttrIter.Value());
        // take label of SHUO
        TDF_Label                       aSHUOlab = aSHUO->Label();
        NCollection_Sequence<TDF_Label> aUpLabels;
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
          Message::SendTrace() << "Warning: " << __FILE__
                               << ": do not store SHUO without any style to the STEP model" << "\n";
          continue;
        }
        // write SHUO to the model amd then add structure type.
        TopoDS_Shape aNAUOShape; // shape of the deepest NAUO in the SHUO structure
        bool         isDeepest = false;
        occ::handle<StepRepr_SpecifiedHigherUsageOccurrence> anEntOfSHUO;
        occ::handle<StepBasic_ProductDefinition>             aRelatingPD;
        // create the top SHUO and all other.
        writeSHUO(aSHUO, theWS, anEntOfSHUO, aNAUOShape, aRelatingPD, isDeepest);
        if (anEntOfSHUO.IsNull() || aNAUOShape.IsNull())
        {
          Message::SendTrace() << "Warning: " << __FILE__ << ": Cannot store SHUO" << "\n";
          continue;
        }
        // create new Product Definition Shape for TOP SHUO
        Message::SendTrace() << "Info: " << __FILE__ << ": Create NEW PDS for current SHUO "
                             << "\n";
        occ::handle<StepRepr_ProductDefinitionShape> aPDS = new StepRepr_ProductDefinitionShape;
        occ::handle<TCollection_HAsciiString> aPDSname    = new TCollection_HAsciiString("SHUO");
        occ::handle<TCollection_HAsciiString> aDescrStr   = new TCollection_HAsciiString("");
        StepRepr_CharacterizedDefinition      aCharDef;
        aCharDef.SetValue(anEntOfSHUO);
        aPDS->Init(aPDSname, false, aDescrStr, aCharDef);

        // create styled item for SHUO and add to the model
        createSHUOStyledItem(aSHUOstyle,
                             aPDS,
                             theWS,
                             aNAUOShape,
                             XCAFDoc_DocumentTool::ShapeTool(aLabel),
                             myMapCompMDGPR);

      } // end work with SHUO
    } // end of an assembly components
  } // end of iterates on indicated labels
  return true;
}

//=======================================================================
// function : FindPDSforDGT
// purpose  : auxiliary: find PDS for AdvancedFace or EdgeCurve for creation
//                     needed ShapeAspect in D&GT structure
//=======================================================================
static bool FindPDSforDGT(const Interface_Graph&                        theGraph,
                          const occ::handle<Standard_Transient>&        theEnt,
                          occ::handle<StepRepr_ProductDefinitionShape>& thePDS,
                          occ::handle<StepRepr_RepresentationContext>&  theRC,
                          occ::handle<StepShape_AdvancedFace>&          theAF,
                          occ::handle<StepShape_EdgeCurve>&             theEC)
{
  if (theEnt.IsNull())
    return false;
  if (!theEnt->IsKind(STANDARD_TYPE(StepShape_EdgeCurve))
      && !theEnt->IsKind(STANDARD_TYPE(StepShape_AdvancedFace)))
  {
    return false;
  }

  theAF = occ::down_cast<StepShape_AdvancedFace>(theEnt);
  if (theAF.IsNull())
  {
    theEC = occ::down_cast<StepShape_EdgeCurve>(theEnt);
    for (Interface_EntityIterator aSharingsIter = theGraph.Sharings(theEC);
         aSharingsIter.More() && theAF.IsNull();
         aSharingsIter.Next())
    {
      occ::handle<StepShape_OrientedEdge> anOE =
        occ::down_cast<StepShape_OrientedEdge>(aSharingsIter.Value());
      if (anOE.IsNull())
        continue;
      for (Interface_EntityIterator aSubs1 = theGraph.Sharings(anOE);
           aSubs1.More() && theAF.IsNull();
           aSubs1.Next())
      {
        occ::handle<StepShape_EdgeLoop> aEL = occ::down_cast<StepShape_EdgeLoop>(aSubs1.Value());
        if (aEL.IsNull())
          continue;
        for (Interface_EntityIterator aSubs2 = theGraph.Sharings(aEL);
             aSubs2.More() && theAF.IsNull();
             aSubs2.Next())
        {
          occ::handle<StepShape_FaceBound> aFB =
            occ::down_cast<StepShape_FaceBound>(aSubs2.Value());
          if (aFB.IsNull())
            continue;
          for (Interface_EntityIterator aSubs3 = theGraph.Sharings(aFB);
               aSubs3.More() && theAF.IsNull();
               aSubs3.Next())
          {
            theAF = occ::down_cast<StepShape_AdvancedFace>(aSubs3.Value());
          }
        }
      }
    }
  }
  if (theAF.IsNull())
    return false;

  for (Interface_EntityIterator aSharingsIter = theGraph.Sharings(theAF);
       aSharingsIter.More() && thePDS.IsNull();
       aSharingsIter.Next())
  {
    occ::handle<StepShape_ConnectedFaceSet> aCFS =
      occ::down_cast<StepShape_ConnectedFaceSet>(aSharingsIter.Value());
    if (aCFS.IsNull())
      continue;
    for (Interface_EntityIterator aSubs1 = theGraph.Sharings(aCFS);
         aSubs1.More() && thePDS.IsNull();
         aSubs1.Next())
    {
      occ::handle<StepRepr_RepresentationItem> aRI =
        occ::down_cast<StepRepr_RepresentationItem>(aSubs1.Value());
      if (aRI.IsNull())
        continue;
      for (Interface_EntityIterator aSubs2 = theGraph.Sharings(aRI);
           aSubs2.More() && thePDS.IsNull();
           aSubs2.Next())
      {
        occ::handle<StepShape_ShapeRepresentation> aSR =
          occ::down_cast<StepShape_ShapeRepresentation>(aSubs2.Value());
        if (aSR.IsNull())
          continue;
        theRC = aSR->ContextOfItems();
        for (Interface_EntityIterator aSubs3 = theGraph.Sharings(aSR);
             aSubs3.More() && thePDS.IsNull();
             aSubs3.Next())
        {
          occ::handle<StepShape_ShapeDefinitionRepresentation> aSDR =
            occ::down_cast<StepShape_ShapeDefinitionRepresentation>(aSubs3.Value());
          if (aSDR.IsNull())
            continue;
          occ::handle<StepRepr_PropertyDefinition> aPropD = aSDR->Definition().PropertyDefinition();
          if (aPropD.IsNull())
            continue;
          thePDS = occ::down_cast<StepRepr_ProductDefinitionShape>(aPropD);
        }
      }
    }
  }
  return true;
}

//=======================================================================
// function : FindPDS
// purpose  : auxiliary: find Product_definition_shape entity for given entity
//=======================================================================
static occ::handle<StepRepr_ProductDefinitionShape> FindPDS(
  const Interface_Graph&                       theGraph,
  const occ::handle<Standard_Transient>&       theEnt,
  occ::handle<StepRepr_RepresentationContext>& theRC)
{
  if (theEnt.IsNull())
    return nullptr;
  occ::handle<StepRepr_ProductDefinitionShape> aPDS;

  // try to find shape_representation in sharings
  for (Interface_EntityIterator anIter = theGraph.Sharings(theEnt); anIter.More() && aPDS.IsNull();
       anIter.Next())
  {
    occ::handle<StepShape_ShapeRepresentation> aSR =
      occ::down_cast<StepShape_ShapeRepresentation>(anIter.Value());
    if (aSR.IsNull())
      continue;
    theRC                           = aSR->ContextOfItems();
    Interface_EntityIterator aSDRIt = theGraph.Sharings(aSR);
    for (aSDRIt.Start(); aSDRIt.More() && aPDS.IsNull(); aSDRIt.Next())
    {
      occ::handle<StepShape_ShapeDefinitionRepresentation> aSDR =
        occ::down_cast<StepShape_ShapeDefinitionRepresentation>(aSDRIt.Value());
      if (aSDR.IsNull())
        continue;
      occ::handle<StepRepr_PropertyDefinition> aPropD = aSDR->Definition().PropertyDefinition();
      if (aPropD.IsNull())
        continue;
      aPDS = occ::down_cast<StepRepr_ProductDefinitionShape>(aPropD);
    }
  }
  if (!aPDS.IsNull())
    return aPDS;

  for (Interface_EntityIterator anIter = theGraph.Sharings(theEnt); anIter.More(); anIter.Next())
  {
    if (anIter.Value()->IsKind(STANDARD_TYPE(StepShape_TopologicalRepresentationItem))
        || anIter.Value()->IsKind(STANDARD_TYPE(StepGeom_GeometricRepresentationItem)))
    {
      aPDS = FindPDS(theGraph, anIter.Value(), theRC);
      if (!aPDS.IsNull())
        return aPDS;
    }
  }
  return aPDS;
}

//=================================================================================================

static StepBasic_Unit GetUnit(const occ::handle<StepRepr_RepresentationContext>& theRC,
                              const bool                                         theIsAngle = false)
{
  StepBasic_Unit                   aUnit;
  occ::handle<StepBasic_NamedUnit> aCurrentUnit;
  if (theIsAngle)
  {
    occ::handle<StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext> aCtx =
      occ::down_cast<StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext>(theRC);
    if (!aCtx.IsNull())
    {
      for (NCollection_HArray1<occ::handle<StepBasic_NamedUnit>>::Iterator aUnitIter(
             aCtx->Units()->Array1());
           aUnitIter.More();
           aUnitIter.Next())
      {
        const occ::handle<StepBasic_NamedUnit>& aCurUnitValue = aUnitIter.Value();
        if (aCurUnitValue->IsKind(STANDARD_TYPE(StepBasic_SiUnitAndPlaneAngleUnit))
            || aCurUnitValue->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndPlaneAngleUnit)))
        {
          aCurrentUnit = aCurUnitValue;
          break;
        }
      }
    }
    if (aCurrentUnit.IsNull())
    {
      occ::handle<StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx> aCtx1 =
        occ::down_cast<StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx>(theRC);
      if (!aCtx1.IsNull())
      {
        for (NCollection_HArray1<occ::handle<StepBasic_NamedUnit>>::Iterator aUnitIter(
               aCtx1->Units()->Array1());
             aUnitIter.More();
             aUnitIter.Next())
        {
          const occ::handle<StepBasic_NamedUnit>& aCurUnitValue = aUnitIter.Value();
          if (aCurUnitValue->IsKind(STANDARD_TYPE(StepBasic_SiUnitAndPlaneAngleUnit))
              || aCurUnitValue->IsKind(
                STANDARD_TYPE(StepBasic_ConversionBasedUnitAndPlaneAngleUnit)))
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
    occ::handle<StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext> aCtx =
      occ::down_cast<StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext>(theRC);
    if (!aCtx.IsNull())
    {
      for (NCollection_HArray1<occ::handle<StepBasic_NamedUnit>>::Iterator aUnitIter(
             aCtx->Units()->Array1());
           aUnitIter.More();
           aUnitIter.Next())
      {
        const occ::handle<StepBasic_NamedUnit>& aCurUnitValue = aUnitIter.Value();
        if (aCurUnitValue->IsKind(STANDARD_TYPE(StepBasic_SiUnitAndLengthUnit))
            || aCurUnitValue->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndLengthUnit)))
        {
          aCurrentUnit = aCurUnitValue;
          break;
        }
      }
    }
    if (aCurrentUnit.IsNull())
    {
      occ::handle<StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx> aCtx1 =
        occ::down_cast<StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx>(theRC);
      if (!aCtx1.IsNull())
      {
        for (NCollection_HArray1<occ::handle<StepBasic_NamedUnit>>::Iterator aUnitIter(
               aCtx1->Units()->Array1());
             aUnitIter.More();
             aUnitIter.Next())
        {
          const occ::handle<StepBasic_NamedUnit>& aCurUnitValue = aUnitIter.Value();
          if (aCurUnitValue->IsKind(STANDARD_TYPE(StepBasic_SiUnitAndLengthUnit))
              || aCurUnitValue->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndLengthUnit)))
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

//=================================================================================================

static occ::handle<StepRepr_ReprItemAndMeasureWithUnit> CreateDimValue(
  const double                                              theValue,
  const StepBasic_Unit&                                     theUnit,
  const occ::handle<TCollection_HAsciiString>&              theName,
  const char* const                                         theMeasureName,
  const bool                                                theIsAngle,
  const bool                                                theIsQualified = false,
  const occ::handle<StepShape_QualifiedRepresentationItem>& theQRI         = nullptr)
{
  occ::handle<StepRepr_RepresentationItem> aReprItem = new StepRepr_RepresentationItem();
  aReprItem->Init(new TCollection_HAsciiString(theName));
  occ::handle<StepBasic_MeasureWithUnit>    aMWU         = new StepBasic_MeasureWithUnit();
  occ::handle<StepBasic_MeasureValueMember> aValueMember = new StepBasic_MeasureValueMember();
  aValueMember->SetName(theMeasureName);
  aValueMember->SetReal(theValue);
  aMWU->Init(aValueMember, theUnit);
  if (theIsQualified)
  {
    if (theIsAngle)
    {
      // Angle & with qualifiers
      occ::handle<StepRepr_ReprItemAndPlaneAngleMeasureWithUnitAndQRI> anItem =
        new StepRepr_ReprItemAndPlaneAngleMeasureWithUnitAndQRI();
      anItem->Init(aMWU, aReprItem, theQRI);
      return anItem;
    }
    else
    {
      // Length & with qualifiers
      occ::handle<StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI> anItem =
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
      occ::handle<StepRepr_ReprItemAndPlaneAngleMeasureWithUnit> anItem =
        new StepRepr_ReprItemAndPlaneAngleMeasureWithUnit();
      anItem->Init(aMWU, aReprItem);
      return anItem;
    }
    else
    {
      // Length & without qualifiers
      occ::handle<StepRepr_ReprItemAndLengthMeasureWithUnit> anItem =
        new StepRepr_ReprItemAndLengthMeasureWithUnit();
      anItem->Init(aMWU, aReprItem);
      return anItem;
    }
  }
}

//=======================================================================
// function : writeShapeAspect
// purpose  : auxiliary (write Shape_Aspect entity for given shape)
//=======================================================================
occ::handle<StepRepr_ShapeAspect> STEPCAFControl_Writer::writeShapeAspect(
  const occ::handle<XSControl_WorkSession>&          theWS,
  const TDF_Label                                    theLabel,
  const TopoDS_Shape&                                theShape,
  occ::handle<StepRepr_RepresentationContext>&       theRC,
  occ::handle<StepAP242_GeometricItemSpecificUsage>& theGISU)
{
  // Get working data
  const occ::handle<Interface_InterfaceModel>& aModel  = theWS->Model();
  const occ::handle<XSControl_TransferWriter>& aTW     = theWS->TransferWriter();
  const occ::handle<Transfer_FinderProcess>&   aFP     = aTW->FinderProcess();
  const occ::handle<Interface_HGraph>          aHGraph = theWS->HGraph();
  if (aHGraph.IsNull())
    return nullptr;
  Interface_Graph aGraph = aHGraph->Graph();

  TopLoc_Location                                       aLoc;
  NCollection_Sequence<occ::handle<Standard_Transient>> aSeqRI;
  FindEntities(aFP, theShape, aLoc, aSeqRI);
  if (aSeqRI.Length() <= 0)
  {
    Message::SendTrace() << "Warning: Cannot find RI for "
                         << theShape.TShape()->DynamicType()->Name() << "\n";
    return nullptr;
  }

  occ::handle<StepRepr_ProductDefinitionShape> aPDS;
  occ::handle<StepRepr_RepresentationContext>  aRC;
  occ::handle<Standard_Transient>              anEnt = aSeqRI.Value(1);
  aPDS                                               = FindPDS(aGraph, anEnt, aRC);
  if (aPDS.IsNull())
    return nullptr;

  theRC = aRC;
  // Shape_Aspect
  occ::handle<TCollection_HAsciiString> aName = new TCollection_HAsciiString();
  occ::handle<TDataStd_Name>            aNameAttr;
  if (theLabel.FindAttribute(TDataStd_Name::GetID(), aNameAttr))
  {
    aName           = new TCollection_HAsciiString(TCollection_AsciiString(aNameAttr->Get()));
    int aFirstSpace = aName->Search(" ");
    if (aFirstSpace != -1)
      aName = aName->SubString(aFirstSpace + 1, aName->Length());
    else
      aName = new TCollection_HAsciiString();
  }
  occ::handle<TCollection_HAsciiString> aDescription = new TCollection_HAsciiString();
  occ::handle<StepRepr_ShapeAspect>     aSA          = new StepRepr_ShapeAspect;
  aSA->Init(aName, aDescription, aPDS, StepData_LTrue);

  // Geometric_Item_Specific_Usage
  occ::handle<StepAP242_GeometricItemSpecificUsage> aGISU =
    new StepAP242_GeometricItemSpecificUsage();
  StepAP242_ItemIdentifiedRepresentationUsageDefinition aDefinition;
  aDefinition.SetValue(aSA);
  occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> anReprItems =
    new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, 1);
  occ::handle<StepRepr_RepresentationItem> anIdentifiedItem =
    occ::down_cast<StepRepr_RepresentationItem>(anEnt);
  anReprItems->SetValue(1, anIdentifiedItem);
  occ::handle<StepShape_ShapeDefinitionRepresentation> aSDR;
  for (Interface_EntityIterator aSharingIter = aGraph.Sharings(aPDS);
       aSharingIter.More() && aSDR.IsNull();
       aSharingIter.Next())
  {
    const occ::handle<Standard_Transient>& anEntity = aSharingIter.Value();
    aSDR = occ::down_cast<StepShape_ShapeDefinitionRepresentation>(anEntity);
  }
  if (aSDR.IsNull())
    return nullptr;

  // Set entities to model
  aGISU->Init(aName, aDescription, aDefinition, aSDR->UsedRepresentation(), anReprItems);
  aModel->AddWithRefs(aSA);
  aModel->AddWithRefs(aGISU);
  theGISU = aGISU;
  return aSA;
}

//=======================================================================
// function : writePresentation
// purpose  : auxiliary (write annotation plane and presentation)
//======================================================================
void STEPCAFControl_Writer::writePresentation(
  const occ::handle<XSControl_WorkSession>&    theWS,
  const TopoDS_Shape&                          thePresentation,
  const occ::handle<TCollection_HAsciiString>& thePrsName,
  const bool                                   theHasSemantic,
  const bool                                   theHasPlane,
  const gp_Ax2&                                theAnnotationPlane,
  const gp_Pnt&                                theTextPosition,
  const occ::handle<Standard_Transient>&       theDimension,
  const StepData_Factors&                      theLocalFactors)
{
  if (thePresentation.IsNull())
    return;
  // Get working data
  const occ::handle<Interface_InterfaceModel>& aModel = theWS->Model();

  // Presentation
  occ::handle<StepVisual_TessellatedGeometricSet> aGeomSet =
    STEPCAFControl_GDTProperty::GetTessellation(thePresentation);
  if (aGeomSet.IsNull())
  {
    return;
  }
  occ::handle<StepVisual_TessellatedAnnotationOccurrence> aTAO =
    new StepVisual_TessellatedAnnotationOccurrence();
  aTAO->Init(new TCollection_HAsciiString(), myGDTPrsCurveStyle, aGeomSet);
  StepVisual_DraughtingCalloutElement aDCElement;
  aDCElement.SetValue(aTAO);
  occ::handle<NCollection_HArray1<StepVisual_DraughtingCalloutElement>> aTAOs =
    new NCollection_HArray1<StepVisual_DraughtingCalloutElement>(1, 1);
  aTAOs->SetValue(1, aDCElement);
  occ::handle<StepVisual_DraughtingCallout> aDCallout = new StepVisual_DraughtingCallout();
  occ::handle<TCollection_HAsciiString>     aPrsName =
    thePrsName.IsNull() ? new TCollection_HAsciiString() : thePrsName;
  aDCallout->Init(aPrsName, aTAOs);
  occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> aDCsForDMIA =
    new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, 1);
  aDCsForDMIA->SetValue(1, aDCallout);
  myGDTAnnotations.Append(aDCallout);
  StepAP242_ItemIdentifiedRepresentationUsageDefinition aDimension;
  aDimension.SetValue(theDimension);
  occ::handle<TCollection_HAsciiString> aDMIAName;
  if (theHasSemantic)
    aDMIAName = new TCollection_HAsciiString("PMI representation to presentation link");
  else
    aDMIAName = new TCollection_HAsciiString();
  occ::handle<StepAP242_DraughtingModelItemAssociation> aDMIA =
    new StepAP242_DraughtingModelItemAssociation();
  aDMIA->Init(aDMIAName,
              new TCollection_HAsciiString(),
              aDimension,
              myGDTPresentationDM,
              aDCsForDMIA);
  aModel->AddWithRefs(aDMIA);

  if (!theHasPlane)
    return;

  // Annotation plane
  // Presentation Style
  occ::handle<StepVisual_NullStyleMember> aNullStyle = new StepVisual_NullStyleMember();
  aNullStyle->SetEnumText(0, ".NULL.");
  StepVisual_PresentationStyleSelect aStyleItem;
  aStyleItem.SetValue(aNullStyle);
  occ::handle<NCollection_HArray1<StepVisual_PresentationStyleSelect>> aStyles =
    new NCollection_HArray1<StepVisual_PresentationStyleSelect>(1, 1);
  aStyles->SetValue(1, aStyleItem);
  occ::handle<StepVisual_PresentationStyleAssignment> aPrsStyle =
    new StepVisual_PresentationStyleAssignment();
  aPrsStyle->Init(aStyles);
  occ::handle<NCollection_HArray1<occ::handle<StepVisual_PresentationStyleAssignment>>> aPrsStyles =
    new NCollection_HArray1<occ::handle<StepVisual_PresentationStyleAssignment>>(1, 1);
  aPrsStyles->SetValue(1, aPrsStyle);
  // Plane
  occ::handle<StepGeom_Plane>                   aPlane = new StepGeom_Plane();
  GeomToStep_MakeAxis2Placement3d               anAxisMaker(theAnnotationPlane, theLocalFactors);
  const occ::handle<StepGeom_Axis2Placement3d>& anAxis = anAxisMaker.Value();
  // Set text position to plane origin
  occ::handle<StepGeom_CartesianPoint>     aTextPos = new StepGeom_CartesianPoint();
  occ::handle<NCollection_HArray1<double>> aCoords  = new NCollection_HArray1<double>(1, 3);
  for (int i = 1; i <= 3; i++)
    aCoords->SetValue(i, theTextPosition.Coord(i));
  aTextPos->Init(new TCollection_HAsciiString(), aCoords);
  anAxis->SetLocation(aTextPos);
  aPlane->Init(new TCollection_HAsciiString(), anAxis);
  // Annotation plane element
  StepVisual_AnnotationPlaneElement aPlaneElement;
  aPlaneElement.SetValue(aDCallout);
  occ::handle<NCollection_HArray1<StepVisual_AnnotationPlaneElement>> aDCsForAnnPln =
    new NCollection_HArray1<StepVisual_AnnotationPlaneElement>(1, 1);
  aDCsForAnnPln->SetValue(1, aPlaneElement);
  // Init AnnotationPlane entity
  occ::handle<StepVisual_AnnotationPlane> anAnnPlane = new StepVisual_AnnotationPlane();
  anAnnPlane->Init(new TCollection_HAsciiString(), aPrsStyles, aPlane, aDCsForAnnPln);
  myGDTAnnotations.Append(anAnnPlane);
  aModel->AddWithRefs(anAnnPlane);
}

//=======================================================================
// function : writeDatumAP242
// purpose  : auxiliary (write Datum entity for given shape or write all
//           necessary entities and link them to already written datum
//           in case of multiple features association)
//=======================================================================
occ::handle<StepDimTol_Datum> STEPCAFControl_Writer::writeDatumAP242(
  const occ::handle<XSControl_WorkSession>& theWS,
  const NCollection_Sequence<TDF_Label>&    theShapeL,
  const TDF_Label&                          theDatumL,
  const bool                                theIsFirstDTarget,
  const occ::handle<StepDimTol_Datum>&      theWrittenDatum,
  const StepData_Factors&                   theLocalFactors)
{
  // Get working data
  const occ::handle<Interface_InterfaceModel>& aModel  = theWS->Model();
  const occ::handle<XSControl_TransferWriter>& aTW     = theWS->TransferWriter();
  const occ::handle<Transfer_FinderProcess>&   aFP     = aTW->FinderProcess();
  const occ::handle<Interface_HGraph>          aHGraph = theWS->HGraph();
  if (aHGraph.IsNull())
    return nullptr;
  Interface_Graph aGraph = aHGraph->Graph();

  occ::handle<StepRepr_ShapeAspect>                       aSA;
  occ::handle<StepRepr_RepresentationContext>             aRC;
  occ::handle<StepRepr_ProductDefinitionShape>            aPDS;
  NCollection_Sequence<occ::handle<StepRepr_ShapeAspect>> aSASeq;
  occ::handle<StepAP242_GeometricItemSpecificUsage>       aGISU;
  int                                                     aSANum = 0, aGISUNum = 0;
  // Link with datum feature
  for (NCollection_Sequence<TDF_Label>::Iterator aLabelIter(theShapeL); aLabelIter.More();
       aLabelIter.Next())
  {
    occ::handle<Standard_Transient>                       anEnt;
    TopLoc_Location                                       aLoc;
    NCollection_Sequence<occ::handle<Standard_Transient>> aSeqRI;

    TopoDS_Shape aShape = XCAFDoc_ShapeTool::GetShape(aLabelIter.Value());
    FindEntities(aFP, aShape, aLoc, aSeqRI);
    if (aSeqRI.Length() <= 0)
    {
      Message::SendTrace() << "Warning: Cannot find RI for "
                           << aShape.TShape()->DynamicType()->Name() << "\n";
      continue;
    }
    anEnt = aSeqRI.Value(1);
    aPDS  = FindPDS(aGraph, anEnt, aRC);
    if (aPDS.IsNull())
      continue;

    occ::handle<StepRepr_ShapeAspect> aCurrentSA =
      writeShapeAspect(theWS, theDatumL, aShape, aRC, aGISU);
    if (aCurrentSA.IsNull())
      continue;
    aSASeq.Append(aCurrentSA);
    aSANum   = aModel->Number(aCurrentSA);
    aGISUNum = aModel->Number(aGISU);
  }
  if (aPDS.IsNull())
  {
    // Workaround for datums without shape
    aPDS                                                        = myGDTCommonPDS;
    Interface_EntityIterator                             aSDRIt = aGraph.Sharings(aPDS);
    occ::handle<StepShape_ShapeDefinitionRepresentation> aSDR;
    for (aSDRIt.Start(); aSDRIt.More() && aSDR.IsNull(); aSDRIt.Next())
      aSDR = occ::down_cast<StepShape_ShapeDefinitionRepresentation>(aSDRIt.Value());
    if (!aSDR.IsNull())
    {
      occ::handle<StepRepr_Representation> aRepr = aSDR->UsedRepresentation();
      if (!aRepr.IsNull())
        aRC = aRepr->ContextOfItems();
    }
  }

  // Find if datum has datum targets and get common datum attributes
  occ::handle<XCAFDoc_Datum> aDatumAttr;
  if (!theDatumL.FindAttribute(XCAFDoc_Datum::GetID(), aDatumAttr))
    return nullptr;
  occ::handle<XCAFDimTolObjects_DatumObject> anObject = aDatumAttr->GetObject();
  if (anObject.IsNull())
    return nullptr;
  bool                                  isSimpleDatum = !anObject->IsDatumTarget();
  occ::handle<TCollection_HAsciiString> anIdentifier  = anObject->GetName();
  occ::handle<TCollection_HAsciiString> aTargetId =
    (anObject->GetDatumTargetNumber() == 0
       ? new TCollection_HAsciiString()
       : new TCollection_HAsciiString(anObject->GetDatumTargetNumber()));

  // If datum type is area, but there is no area in object, write as simple datum
  if (anObject->GetDatumTargetType() == XCAFDimTolObjects_DatumTargetType_Area
      && anObject->GetDatumTarget().IsNull())
    isSimpleDatum = true;

  // Simple datum
  if (isSimpleDatum)
  {
    if (aSASeq.Length() == 0)
    {
      // Create empty datum with name and presentation only
      occ::handle<StepDimTol_DatumFeature> aDF = new StepDimTol_DatumFeature();
      aDF->Init(new TCollection_HAsciiString(),
                new TCollection_HAsciiString(),
                aPDS,
                StepData_LTrue);
      aSA = aDF;
      aModel->AddWithRefs(aDF);
    }
    else if (aSASeq.Length() == 1)
    {
      occ::handle<StepDimTol_DatumFeature> aDF = new StepDimTol_DatumFeature();
      aDF->Init(new TCollection_HAsciiString(),
                new TCollection_HAsciiString(),
                aPDS,
                StepData_LTrue);
      aModel->ReplaceEntity(aSANum, aDF);
      aSA = aDF;
      StepAP242_ItemIdentifiedRepresentationUsageDefinition aDefinition;
      aDefinition.SetValue(aDF);
      aGISU->SetDefinition(aDefinition);
      aModel->ReplaceEntity(aGISUNum, aGISU);
    }
    else if (aSASeq.Length() > 1)
    {
      occ::handle<StepRepr_CompShAspAndDatumFeatAndShAsp> aDF =
        new StepRepr_CompShAspAndDatumFeatAndShAsp();
      aDF->Init(new TCollection_HAsciiString(),
                new TCollection_HAsciiString(),
                aPDS,
                StepData_LTrue);
      for (int i = 1; i <= aSASeq.Length(); i++)
      {
        occ::handle<StepRepr_ShapeAspectRelationship> aSAR = new StepRepr_ShapeAspectRelationship();
        aSAR->Init(new TCollection_HAsciiString(),
                   false,
                   new TCollection_HAsciiString(),
                   aDF,
                   aSASeq.Value(i));
        aModel->AddWithRefs(aSAR);
      }
      aSA = aDF;
      aModel->AddWithRefs(aDF);
    }
  }
  // Datum with datum targets
  else
  {
    XCAFDimTolObjects_DatumTargetType   aDatumType = anObject->GetDatumTargetType();
    occ::handle<StepDimTol_DatumTarget> aDatumTarget;
    // Note: the given way to write such datum type may be incorrect (too little information)
    if (aDatumType == XCAFDimTolObjects_DatumTargetType_Area)
    {
      TopoDS_Shape                                      aDTShape = anObject->GetDatumTarget();
      occ::handle<StepAP242_GeometricItemSpecificUsage> anAreaGISU;
      occ::handle<StepRepr_ShapeAspect>                 anAreaSA =
        writeShapeAspect(theWS, theDatumL, aDTShape, aRC, anAreaGISU);
      aSANum                                  = aModel->Number(anAreaSA);
      aGISUNum                                = aModel->Number(anAreaGISU);
      occ::handle<StepDimTol_DatumTarget> aDT = new StepDimTol_DatumTarget();
      aDT->Init(new TCollection_HAsciiString(),
                new TCollection_HAsciiString("area"),
                anAreaSA->OfShape(),
                StepData_LTrue,
                aTargetId);
      aModel->ReplaceEntity(aSANum, aDT);
      StepAP242_ItemIdentifiedRepresentationUsageDefinition aDefinition;
      aDefinition.SetValue(aDT);
      anAreaGISU->SetDefinition(aDefinition);
      aModel->ReplaceEntity(aGISUNum, anAreaGISU);
    }
    else
    {
      occ::handle<StepDimTol_PlacedDatumTargetFeature> aPDTF =
        new StepDimTol_PlacedDatumTargetFeature();
      aPDTF->Init(new TCollection_HAsciiString(),
                  STEPCAFControl_GDTProperty::GetDatumTargetName(aDatumType),
                  aPDS,
                  StepData_LTrue,
                  aTargetId);
      aModel->AddWithRefs(aPDTF);
      aDatumTarget = aPDTF;
      // Datum targets
      occ::handle<StepRepr_PropertyDefinition> aPD = new StepRepr_PropertyDefinition();
      StepRepr_CharacterizedDefinition         aCDefinition;
      aCDefinition.SetValue(aPDTF);
      aPD->Init(new TCollection_HAsciiString(), false, nullptr, aCDefinition);
      if (anObject->HasDatumTargetParams())
      {
        // write all parameters of datum target
        occ::handle<StepShape_ShapeRepresentationWithParameters> aSRWP =
          new StepShape_ShapeRepresentationWithParameters();
        // Common for all datum targets
        StepBasic_Unit                         aUnit   = GetUnit(aRC);
        gp_Ax2                                 aDTAxis = anObject->GetDatumTargetAxis();
        GeomToStep_MakeAxis2Placement3d        anAxisMaker(aDTAxis, theLocalFactors);
        occ::handle<StepGeom_Axis2Placement3d> anA2P3D = anAxisMaker.Value();
        anA2P3D->SetName(new TCollection_HAsciiString("orientation"));
        occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> anItems;
        // Process each datum target type
        if (aDatumType == XCAFDimTolObjects_DatumTargetType_Point)
        {
          anItems = new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, 1);
        }
        else
        {
          occ::handle<TCollection_HAsciiString> aTargetValueName;
          if (aDatumType == XCAFDimTolObjects_DatumTargetType_Line)
          {
            anItems = new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, 2);
            aTargetValueName = new TCollection_HAsciiString("target length");
          }
          else if (aDatumType == XCAFDimTolObjects_DatumTargetType_Rectangle)
          {
            anItems = new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, 3);
            aTargetValueName = new TCollection_HAsciiString("target length");
            // Additional value
            occ::handle<StepRepr_ReprItemAndMeasureWithUnit> aTargetValue =
              CreateDimValue(anObject->GetDatumTargetWidth(),
                             aUnit,
                             new TCollection_HAsciiString("target width"),
                             "POSITIVE_LENGTH_MEASURE",
                             false);
            anItems->SetValue(2, aTargetValue);
            aModel->AddWithRefs(aTargetValue);
          }
          else if (aDatumType == XCAFDimTolObjects_DatumTargetType_Circle)
          {
            anItems = new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, 2);
            aTargetValueName = new TCollection_HAsciiString("target diameter");
          }
          // Value
          occ::handle<StepRepr_ReprItemAndMeasureWithUnit> aTargetValue =
            CreateDimValue(anObject->GetDatumTargetLength(),
                           aUnit,
                           aTargetValueName,
                           "POSITIVE_LENGTH_MEASURE",
                           false);
          anItems->SetValue(1, aTargetValue);
          aModel->AddWithRefs(aTargetValue);
        }
        anItems->SetValue(anItems->Length(), anA2P3D);
        aSRWP->Init(new TCollection_HAsciiString(), anItems, aRC);
        // Create and write auxiliary entities
        occ::handle<StepShape_ShapeDefinitionRepresentation> aSDR =
          new StepShape_ShapeDefinitionRepresentation();
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
      occ::handle<StepRepr_FeatureForDatumTargetRelationship> aFFDTR =
        new StepRepr_FeatureForDatumTargetRelationship();
      aFFDTR->Init(new TCollection_HAsciiString(), false, nullptr, aSASeq.Value(1), aDatumTarget);
      aModel->AddWithRefs(aFFDTR);
    }
    else if (aSASeq.Length() > 1)
    {
      occ::handle<StepRepr_CompositeShapeAspect> aCompSA = new StepRepr_CompositeShapeAspect();
      aCompSA->Init(new TCollection_HAsciiString(),
                    new TCollection_HAsciiString(),
                    aPDS,
                    aSASeq.Value(1)->ProductDefinitional());
      for (int i = 1; i <= aSASeq.Length(); i++)
      {
        occ::handle<StepRepr_ShapeAspectRelationship> aSAR = new StepRepr_ShapeAspectRelationship();
        aSAR->Init(new TCollection_HAsciiString(),
                   false,
                   new TCollection_HAsciiString(),
                   aCompSA,
                   aSASeq.Value(i));
        aModel->AddWithRefs(aSAR);
      }
      occ::handle<StepRepr_FeatureForDatumTargetRelationship> aFFDTR =
        new StepRepr_FeatureForDatumTargetRelationship();
      aFFDTR->Init(new TCollection_HAsciiString(), false, nullptr, aCompSA, aDatumTarget);
      aModel->AddWithRefs(aFFDTR);
    }
    aSA = aDatumTarget;
  }

  // Datum
  occ::handle<StepDimTol_Datum> aDatum = theWrittenDatum;
  if (theIsFirstDTarget)
  {
    aDatum = new StepDimTol_Datum();
    aDatum->Init(new TCollection_HAsciiString(),
                 new TCollection_HAsciiString(),
                 aPDS,
                 StepData_LFalse,
                 anIdentifier);
    aModel->AddWithRefs(aDatum);
  }

  // Shape_Aspect_Relationship
  if (!aSA.IsNull())
  {
    occ::handle<StepRepr_ShapeAspectRelationship> aSAR = new StepRepr_ShapeAspectRelationship();
    aSAR->Init(new TCollection_HAsciiString(), false, nullptr, aSA, aDatum);
    aModel->AddWithRefs(aSAR);
  }

  // Annotation plane and Presentation
  writePresentation(theWS,
                    anObject->GetPresentation(),
                    anObject->GetPresentationName(),
                    true,
                    anObject->HasPlane(),
                    anObject->GetPlane(),
                    anObject->GetPointTextAttach(),
                    aSA,
                    theLocalFactors);

  return aDatum;
}

//=======================================================================
// function : WriteDimValues
// purpose  : auxiliary (write all data for given dimension: values,
//           qualifiers, modifiers, orientation and tolerance class)
//======================================================================
static void WriteDimValues(const occ::handle<XSControl_WorkSession>&             theWS,
                           const occ::handle<XCAFDimTolObjects_DimensionObject>& theObject,
                           const occ::handle<StepRepr_RepresentationContext>&    theRC,
                           const StepShape_DimensionalCharacteristic&            theDimension,
                           const StepData_Factors&                               theLocalFactors)
{
  // Get working data
  const occ::handle<Interface_InterfaceModel>&           aModel     = theWS->Model();
  NCollection_Sequence<XCAFDimTolObjects_DimensionModif> aModifiers = theObject->GetModifiers();
  const occ::handle<Standard_Transient>&                 aDim       = theDimension.Value();
  bool isAngle = aDim->IsKind(STANDARD_TYPE(StepShape_AngularLocation))
                 || aDim->IsKind(STANDARD_TYPE(StepShape_AngularSize));

  // Unit
  StepBasic_Unit aUnit = GetUnit(theRC, isAngle);
  const char*    aMeasureName;
  if (isAngle)
    aMeasureName = "POSITIVE_PLANE_ANGLE_MEASURE";
  else
    aMeasureName = "POSITIVE_LENGTH_MEASURE";

  // Values
  occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> aValues;
  int aNbItems = 1, aValIt = 1;
  if (theObject->IsDimWithRange())
    aNbItems += 2;
  if (aModifiers.Length() > 0)
    aNbItems++;
  if (theObject->GetType() == XCAFDimTolObjects_DimensionType_Location_Oriented)
    aNbItems++;
  aNbItems += theObject->NbDescriptions();
  aValues = new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, aNbItems);

  // Nominal value
  double aNominal = theObject->GetValue();
  int    aLeftNbDigits, aRightNbDigits;
  theObject->GetNbOfDecimalPlaces(aLeftNbDigits, aRightNbDigits);
  int aNbQualifiers = 0;
  if (theObject->HasQualifier() && !isAngle)
    aNbQualifiers++;
  if (aLeftNbDigits > 0 || aRightNbDigits > 0)
    aNbQualifiers++;
  // With qualifiers
  if (aNbQualifiers > 0)
  {
    occ::handle<StepShape_QualifiedRepresentationItem> aQRI =
      new StepShape_QualifiedRepresentationItem();
    occ::handle<NCollection_HArray1<StepShape_ValueQualifier>> aQualifiers =
      new NCollection_HArray1<StepShape_ValueQualifier>(1, aNbQualifiers);
    // Type qualifier
    if (theObject->HasQualifier() && !isAngle)
    {
      StepShape_ValueQualifier             anItem;
      occ::handle<StepShape_TypeQualifier> aType      = new StepShape_TypeQualifier();
      XCAFDimTolObjects_DimensionQualifier aQualifier = theObject->GetQualifier();
      aType->Init(STEPCAFControl_GDTProperty::GetDimQualifierName(aQualifier));
      aModel->AddWithRefs(aType);
      anItem.SetValue(aType);
      aQualifiers->SetValue(1, anItem);
    }
    // Number of decimal places
    if (aLeftNbDigits > 0 || aRightNbDigits > 0)
    {
      StepShape_ValueQualifier                        anItem;
      occ::handle<StepShape_ValueFormatTypeQualifier> aType =
        new StepShape_ValueFormatTypeQualifier();
      occ::handle<TCollection_HAsciiString> aFormatType = new TCollection_HAsciiString("NR2 ");
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
    occ::handle<StepRepr_ReprItemAndMeasureWithUnit> anItem =
      CreateDimValue(aNominal,
                     aUnit,
                     new TCollection_HAsciiString("nominal value"),
                     aMeasureName,
                     isAngle,
                     true,
                     aQRI);
    aValues->SetValue(aValIt, anItem);
    aValIt++;
  }
  // Without qualifiers
  else
  {
    occ::handle<StepRepr_ReprItemAndMeasureWithUnit> anItem =
      CreateDimValue(aNominal,
                     aUnit,
                     new TCollection_HAsciiString("nominal value"),
                     aMeasureName,
                     isAngle);
    aValues->SetValue(aValIt, anItem);
    aValIt++;
  }

  // Ranges
  if (theObject->IsDimWithRange())
  {
    occ::handle<StepRepr_ReprItemAndMeasureWithUnit> aLowerItem =
      CreateDimValue(theObject->GetLowerBound(),
                     aUnit,
                     new TCollection_HAsciiString("lower limit"),
                     aMeasureName,
                     isAngle);
    occ::handle<StepRepr_ReprItemAndMeasureWithUnit> anUpperItem =
      CreateDimValue(theObject->GetUpperBound(),
                     aUnit,
                     new TCollection_HAsciiString("upper limit"),
                     aMeasureName,
                     isAngle);
    aValues->SetValue(aValIt, aLowerItem);
    aValIt++;
    aValues->SetValue(aValIt, anUpperItem);
    aValIt++;
  }

  // Modifiers
  if (aModifiers.Length() > 0)
  {
    occ::handle<StepRepr_CompoundRepresentationItem> aCompoundRI =
      new StepRepr_CompoundRepresentationItem();
    occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> aModifItems =
      new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, aModifiers.Length());
    for (int i = 1; i <= aModifiers.Length(); i++)
    {
      XCAFDimTolObjects_DimensionModif                    aModif = aModifiers.Value(i);
      occ::handle<StepRepr_DescriptiveRepresentationItem> aModifItem =
        new StepRepr_DescriptiveRepresentationItem();
      aModifItem->Init(new TCollection_HAsciiString(),
                       STEPCAFControl_GDTProperty::GetDimModifierName(aModif));
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
    occ::handle<StepGeom_Axis2Placement3d> anOrientation = new StepGeom_Axis2Placement3d();
    gp_Dir                                 aDir;
    theObject->GetDirection(aDir);
    GeomToStep_MakeCartesianPoint MkPoint(gp_Pnt(0, 0, 0), theLocalFactors.LengthFactor());
    const occ::handle<StepGeom_CartesianPoint>& aLoc    = MkPoint.Value();
    occ::handle<StepGeom_Direction>             anAxis  = new StepGeom_Direction();
    occ::handle<NCollection_HArray1<double>>    aCoords = new NCollection_HArray1<double>(1, 3);
    aCoords->SetValue(1, aDir.X());
    aCoords->SetValue(2, aDir.Y());
    aCoords->SetValue(3, aDir.Z());
    anAxis->Init(new TCollection_HAsciiString(), aCoords);
    anOrientation
      ->Init(new TCollection_HAsciiString("orientation"), aLoc, true, anAxis, false, nullptr);
    aValues->SetValue(aValIt, anOrientation);
    aValIt++;
  }

  // Descriptions
  if (theObject->HasDescriptions())
  {
    for (int i = 0; i < theObject->NbDescriptions(); i++)
    {
      occ::handle<StepRepr_DescriptiveRepresentationItem> aDRI =
        new StepRepr_DescriptiveRepresentationItem();
      aDRI->Init(theObject->GetDescriptionName(i), theObject->GetDescription(i));
      aValues->SetValue(aValIt, aDRI);
      aValIt++;
    }
  }

  for (int i = 1; i <= aValues->Length(); i++)
  {
    aModel->AddWithRefs(aValues->Value(i));
  }

  // Create resulting Shape_Dimension_Representation
  occ::handle<StepShape_ShapeDimensionRepresentation> aSDR =
    new StepShape_ShapeDimensionRepresentation();
  aSDR->Init(new TCollection_HAsciiString(), aValues, theRC);
  aModel->AddWithRefs(aSDR);
  occ::handle<StepShape_DimensionalCharacteristicRepresentation> aDCR =
    new StepShape_DimensionalCharacteristicRepresentation();
  aDCR->Init(theDimension, aSDR);
  aModel->AddWithRefs(aDCR);

  // Plus_Minus_Tolerance
  if (theObject->IsDimWithPlusMinusTolerance())
  {
    occ::handle<TCollection_HAsciiString> aDummyName = new TCollection_HAsciiString(aMeasureName);
    aDummyName             = aDummyName->SubString(9, aDummyName->Length()); // delete "POSITIVE_"
    aMeasureName           = aDummyName->ToCString();
    double aLowerTolValue  = -theObject->GetLowerTolValue(),
           anUpperTolValue = theObject->GetUpperTolValue();
    // Upper
    occ::handle<StepBasic_MeasureWithUnit>    anUpperMWU   = new StepBasic_MeasureWithUnit();
    occ::handle<StepBasic_MeasureValueMember> anUpperValue = new StepBasic_MeasureValueMember();
    anUpperValue->SetName(aMeasureName);
    anUpperValue->SetReal(anUpperTolValue);
    anUpperMWU->Init(anUpperValue, aUnit);
    aModel->AddWithRefs(anUpperMWU);
    // Lower
    occ::handle<StepBasic_MeasureWithUnit>    aLowerMWU   = new StepBasic_MeasureWithUnit();
    occ::handle<StepBasic_MeasureValueMember> aLowerValue = new StepBasic_MeasureValueMember();
    aLowerValue->SetName(aMeasureName);
    aLowerValue->SetReal(aLowerTolValue);
    aLowerMWU->Init(aLowerValue, aUnit);
    aModel->AddWithRefs(aLowerMWU);
    // Tolerance
    occ::handle<StepShape_ToleranceValue> aTolValue = new StepShape_ToleranceValue();
    aTolValue->Init(aLowerMWU, anUpperMWU);
    aModel->AddWithRefs(aTolValue);
    StepShape_ToleranceMethodDefinition aMethod;
    aMethod.SetValue(aTolValue);
    occ::handle<StepShape_PlusMinusTolerance> aPlusMinusTol = new StepShape_PlusMinusTolerance();
    aPlusMinusTol->Init(aMethod, theDimension);
    aModel->AddWithRefs(aPlusMinusTol);
  }
  // Tolerance class
  if (theObject->IsDimWithClassOfTolerance())
  {
    bool                                    isHole;
    XCAFDimTolObjects_DimensionFormVariance aFormVariance;
    XCAFDimTolObjects_DimensionGrade        aGrade;
    if (!theObject->GetClassOfTolerance(isHole, aFormVariance, aGrade))
      return;
    occ::handle<StepShape_LimitsAndFits> aLAF =
      STEPCAFControl_GDTProperty::GetLimitsAndFits(isHole, aFormVariance, aGrade);
    aModel->AddWithRefs(aLAF);
    StepShape_ToleranceMethodDefinition aMethod;
    aMethod.SetValue(aLAF);
    occ::handle<StepShape_PlusMinusTolerance> aPlusMinusTol = new StepShape_PlusMinusTolerance();
    aPlusMinusTol->Init(aMethod, theDimension);
    aModel->AddWithRefs(aPlusMinusTol);
  }
}

//=======================================================================
// function : WriteDerivedGeometry
// purpose  : auxiliary (write connection point for dimensions)
//======================================================================
static void WriteDerivedGeometry(
  const occ::handle<XSControl_WorkSession>&                       theWS,
  const occ::handle<XCAFDimTolObjects_DimensionObject>&           theObject,
  const occ::handle<StepRepr_ConstructiveGeometryRepresentation>& theRepr,
  occ::handle<StepRepr_ShapeAspect>&                              theFirstSA,
  occ::handle<StepRepr_ShapeAspect>&                              theSecondSA,
  NCollection_Vector<occ::handle<StepGeom_CartesianPoint>>&       thePnts,
  const StepData_Factors&                                         theLocalFactors)
{
  const occ::handle<Interface_InterfaceModel>& aModel = theWS->Model();
  // First point
  if (theObject->HasPoint())
  {
    GeomToStep_MakeCartesianPoint        aPointMaker(theObject->GetPoint(),
                                              theLocalFactors.LengthFactor());
    occ::handle<StepGeom_CartesianPoint> aPoint = aPointMaker.Value();
    thePnts.Append(aPoint);
    occ::handle<StepRepr_DerivedShapeAspect> aDSA = new StepRepr_DerivedShapeAspect();
    aDSA->Init(new TCollection_HAsciiString(),
               new TCollection_HAsciiString(),
               theFirstSA->OfShape(),
               StepData_LFalse);
    occ::handle<StepAP242_GeometricItemSpecificUsage> aGISU =
      new StepAP242_GeometricItemSpecificUsage();
    StepAP242_ItemIdentifiedRepresentationUsageDefinition aDefinition;
    aDefinition.SetValue(aDSA);
    occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> anItem =
      new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, 1);
    anItem->SetValue(1, aPoint);
    aGISU->Init(new TCollection_HAsciiString(),
                new TCollection_HAsciiString(),
                aDefinition,
                theRepr,
                anItem);
    occ::handle<StepRepr_ShapeAspectDerivingRelationship> aSADR =
      new StepRepr_ShapeAspectDerivingRelationship();
    aSADR->Init(new TCollection_HAsciiString(),
                false,
                new TCollection_HAsciiString(),
                aDSA,
                theFirstSA);
    theFirstSA = aDSA;
    aModel->AddWithRefs(aGISU);
    aModel->AddWithRefs(aSADR);
  }

  // Second point (for locations)
  if (theObject->HasPoint2())
  {
    GeomToStep_MakeCartesianPoint        aPointMaker(theObject->GetPoint2(),
                                              theLocalFactors.LengthFactor());
    occ::handle<StepGeom_CartesianPoint> aPoint = aPointMaker.Value();
    thePnts.Append(aPoint);
    occ::handle<StepRepr_DerivedShapeAspect> aDSA = new StepRepr_DerivedShapeAspect();
    aDSA->Init(new TCollection_HAsciiString(),
               new TCollection_HAsciiString(),
               theFirstSA->OfShape(),
               StepData_LFalse);
    occ::handle<StepAP242_GeometricItemSpecificUsage> aGISU =
      new StepAP242_GeometricItemSpecificUsage();
    StepAP242_ItemIdentifiedRepresentationUsageDefinition aDefinition;
    aDefinition.SetValue(aDSA);
    occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> anItem =
      new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, 1);
    anItem->SetValue(1, aPoint);
    aGISU->Init(new TCollection_HAsciiString(),
                new TCollection_HAsciiString(),
                aDefinition,
                theRepr,
                anItem);
    occ::handle<StepRepr_ShapeAspectDerivingRelationship> aSADR =
      new StepRepr_ShapeAspectDerivingRelationship();
    aSADR->Init(new TCollection_HAsciiString(),
                false,
                new TCollection_HAsciiString(),
                aDSA,
                theSecondSA);
    theSecondSA = aDSA;
    aModel->AddWithRefs(aGISU);
    aModel->AddWithRefs(aSADR);
  }
}

//=======================================================================
// function : WriteDatumSystem
// purpose  : auxiliary (write Write datum system for given
//           geometric_tolerance)
//======================================================================
static occ::handle<NCollection_HArray1<StepDimTol_DatumSystemOrReference>> WriteDatumSystem(
  const occ::handle<XSControl_WorkSession>&                                            theWS,
  const TDF_Label                                                                      theGeomTolL,
  const NCollection_Sequence<TDF_Label>&                                               theDatumSeq,
  const NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>>& theDatumMap,
  const occ::handle<StepRepr_RepresentationContext>&                                   theRC,
  const StepData_Factors& theLocalFactors)
{
  // Get working data
  const occ::handle<Interface_InterfaceModel>& aModel  = theWS->Model();
  const occ::handle<Interface_HGraph>          aHGraph = theWS->HGraph();
  if (aHGraph.IsNull())
    return nullptr;
  Interface_Graph                    aGraph = aHGraph->Graph();
  occ::handle<XCAFDoc_GeomTolerance> aGTAttr;
  if (!theGeomTolL.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aGTAttr))
    return nullptr;
  occ::handle<XCAFDimTolObjects_GeomToleranceObject> anObject = aGTAttr->GetObject();
  if (anObject.IsNull())
    return nullptr;

  // Unit
  StepBasic_Unit aUnit = GetUnit(theRC);

  NCollection_Sequence<occ::handle<XCAFDimTolObjects_DatumObject>> aDatums;
  int                                                              aMaxDatumNum = 0;

  for (NCollection_Sequence<TDF_Label>::Iterator aDatumIter(theDatumSeq); aDatumIter.More();
       aDatumIter.Next())
  {
    occ::handle<XCAFDoc_Datum> aDatumAttr;
    if (!aDatumIter.Value().FindAttribute(XCAFDoc_Datum::GetID(), aDatumAttr))
      continue;
    occ::handle<XCAFDimTolObjects_DatumObject> aDatumObj = aDatumAttr->GetObject();
    if (aDatumObj.IsNull())
      continue;
    aDatums.Append(aDatumObj);
    aMaxDatumNum = std::max(aMaxDatumNum, aDatumObj->GetPosition());
  }
  if (aMaxDatumNum == 0)
    return nullptr;

  occ::handle<NCollection_HArray1<occ::handle<StepDimTol_DatumReferenceCompartment>>>
    aConstituents =
      new NCollection_HArray1<occ::handle<StepDimTol_DatumReferenceCompartment>>(1, aMaxDatumNum);
  // Auxiliary datum to initialize attributes in Datum_System
  occ::handle<StepDimTol_Datum> aFirstDatum;
  for (int aConstituentsNum = 1; aConstituentsNum <= aMaxDatumNum; aConstituentsNum++)
  {
    // Collect datums with i-th position
    NCollection_Sequence<occ::handle<XCAFDimTolObjects_DatumObject>> aDatumSeqPos;
    for (NCollection_Sequence<occ::handle<XCAFDimTolObjects_DatumObject>>::Iterator aDatumIter(
           aDatums);
         aDatumIter.More();
         aDatumIter.Next())
    {
      if (aDatumIter.Value()->GetPosition() == aConstituentsNum)
      {
        aDatumSeqPos.Append(aDatumIter.Value());
      }
    }
    if (aDatumSeqPos.Length() < 1)
      continue;
    // Initialize Datum_Reference_Compartment
    StepDimTol_DatumOrCommonDatum                     aDatumRef;
    occ::handle<StepDimTol_DatumReferenceCompartment> aCompartment =
      new StepDimTol_DatumReferenceCompartment();
    occ::handle<NCollection_HArray1<StepDimTol_DatumReferenceModifier>> aModifiers;
    if (aDatumSeqPos.Length() == 1)
    {
      // Datum entity
      occ::handle<Standard_Transient> aFDValue;
      if (theDatumMap.Find(aDatumSeqPos.Value(1)->GetName()->String(), aFDValue)
          && !aFDValue.IsNull())
        aFirstDatum = occ::down_cast<StepDimTol_Datum>(aFDValue);
      aDatumRef.SetValue(aFirstDatum);
      // Modifiers
      NCollection_Sequence<XCAFDimTolObjects_DatumSingleModif> aSimpleModifiers =
        aDatumSeqPos.Value(1)->GetModifiers();
      XCAFDimTolObjects_DatumModifWithValue aModifWithVal;
      double                                aValue = 0;
      aDatumSeqPos.Value(1)->GetModifierWithValue(aModifWithVal, aValue);
      aModifiers = STEPCAFControl_GDTProperty::GetDatumRefModifiers(aSimpleModifiers,
                                                                    aModifWithVal,
                                                                    aValue,
                                                                    aUnit);
      // Add Datum_Reference_Modifier_With_Value
      if (!aModifiers.IsNull())
      {
        occ::handle<StepDimTol_DatumReferenceModifierWithValue> aDRMWV =
          aModifiers->Value(aModifiers->Length()).DatumReferenceModifierWithValue();
        if (!aDRMWV.IsNull())
        {
          aModel->AddWithRefs(aDRMWV);
        }
      }
    }
    else
    {
      int aSetInd = 1;
      occ::handle<NCollection_HArray1<occ::handle<StepDimTol_DatumReferenceElement>>>
        aCommonDatumList = new NCollection_HArray1<occ::handle<StepDimTol_DatumReferenceElement>>(
          1,
          aDatumSeqPos.Length());
      for (NCollection_Sequence<occ::handle<XCAFDimTolObjects_DatumObject>>::Iterator aDatumIter(
             aDatumSeqPos);
           aDatumIter.More();
           aDatumIter.Next())
      {
        // Datum entity
        const occ::handle<XCAFDimTolObjects_DatumObject>& aDatumObj = aDatumIter.Value();
        occ::handle<StepDimTol_Datum>                     aDatum;
        occ::handle<Standard_Transient>                   aDValue;
        if (theDatumMap.Find(aDatumObj->GetName()->String(), aDValue))
          aDatum = occ::down_cast<StepDimTol_Datum>(aDValue);
        StepDimTol_DatumOrCommonDatum anElemDatumRef;
        anElemDatumRef.SetValue(aDatum);
        if (aFirstDatum.IsNull())
          aFirstDatum = aDatum;
        // Modifiers
        NCollection_Sequence<XCAFDimTolObjects_DatumSingleModif> aSimpleModifiers =
          aDatumObj->GetModifiers();
        XCAFDimTolObjects_DatumModifWithValue aModifWithVal;
        double                                aValue = 0;
        aDatumObj->GetModifierWithValue(aModifWithVal, aValue);
        occ::handle<NCollection_HArray1<StepDimTol_DatumReferenceModifier>> anElemModifiers =
          STEPCAFControl_GDTProperty::GetDatumRefModifiers(aSimpleModifiers,
                                                           aModifWithVal,
                                                           aValue,
                                                           aUnit);
        // Add Datum_Reference_Modifier_With_Value
        if (!anElemModifiers.IsNull())
        {
          occ::handle<StepDimTol_DatumReferenceModifierWithValue> aDRMWV =
            anElemModifiers->Value(anElemModifiers->Length()).DatumReferenceModifierWithValue();
          if (!aDRMWV.IsNull())
          {
            aModel->AddWithRefs(aDRMWV);
          }
        }
        // Datum_Reference_Element
        occ::handle<StepDimTol_DatumReferenceElement> anElement =
          new StepDimTol_DatumReferenceElement();
        anElement->Init(new TCollection_HAsciiString(),
                        new TCollection_HAsciiString(),
                        aDatum->OfShape(),
                        aDatum->ProductDefinitional(),
                        anElemDatumRef,
                        !anElemModifiers.IsNull(),
                        anElemModifiers);
        aModel->AddWithRefs(anElement);
        aCommonDatumList->SetValue(aSetInd++, anElement);
      }
      aDatumRef.SetValue(aCommonDatumList);
    }
    aCompartment->Init(new TCollection_HAsciiString(),
                       new TCollection_HAsciiString(),
                       aFirstDatum->OfShape(),
                       aFirstDatum->ProductDefinitional(),
                       aDatumRef,
                       !aModifiers.IsNull(),
                       aModifiers);
    aModel->AddWithRefs(aCompartment);
    aConstituents->SetValue(aConstituentsNum, aCompartment);
  }
  // Remove null elements from aConstituents
  int aNbConstituents = 0;
  for (NCollection_HArray1<occ::handle<StepDimTol_DatumReferenceCompartment>>::Iterator
         aConstituentIter(aConstituents->Array1());
       aConstituentIter.More();
       aConstituentIter.Next())
  {
    if (!aConstituentIter.Value().IsNull())
    {
      aNbConstituents++;
    }
  }
  occ::handle<NCollection_HArray1<occ::handle<StepDimTol_DatumReferenceCompartment>>>
    aResConstituents =
      new NCollection_HArray1<occ::handle<StepDimTol_DatumReferenceCompartment>>(1,
                                                                                 aNbConstituents);
  int aConstituentsIt = 0;
  for (NCollection_HArray1<occ::handle<StepDimTol_DatumReferenceCompartment>>::Iterator
         aConstituentIter(aConstituents->Array1());
       aConstituentIter.More();
       aConstituentIter.Next())
  {
    if (!aConstituentIter.Value().IsNull())
    {
      aConstituentsIt++;
      aResConstituents->SetValue(aConstituentsIt, aConstituentIter.Value());
    }
  }

  occ::handle<NCollection_HArray1<StepDimTol_DatumSystemOrReference>> aDatumSystem;
  occ::handle<StepDimTol_DatumSystem> aDS = new StepDimTol_DatumSystem();
  aDS->Init(new TCollection_HAsciiString(),
            new TCollection_HAsciiString(),
            aFirstDatum->OfShape(),
            aFirstDatum->ProductDefinitional(),
            aResConstituents);
  aModel->AddWithRefs(aDS);
  StepDimTol_DatumSystemOrReference anArrayValue;
  anArrayValue.SetValue(aDS);
  aDatumSystem = new NCollection_HArray1<StepDimTol_DatumSystemOrReference>(1, 1);
  aDatumSystem->SetValue(1, anArrayValue);

  // Axis
  if (anObject->HasAxis())
  {
    GeomToStep_MakeAxis2Placement3d        anAxisMaker(anObject->GetAxis(), theLocalFactors);
    occ::handle<StepGeom_Axis2Placement3d> anAxis = anAxisMaker.Value();
    anAxis->SetName(new TCollection_HAsciiString("orientation"));
    occ::handle<StepAP242_GeometricItemSpecificUsage> aGISU =
      new StepAP242_GeometricItemSpecificUsage();
    StepAP242_ItemIdentifiedRepresentationUsageDefinition aDefinition;
    aDefinition.SetValue(aDS);
    occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> anReprItems =
      new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, 1);
    occ::handle<StepRepr_RepresentationItem> anIdentifiedItem = anAxis;
    anReprItems->SetValue(1, anIdentifiedItem);
    occ::handle<StepShape_ShapeDefinitionRepresentation> aSDR;
    for (Interface_EntityIterator aSharingsIter = aGraph.Sharings(aFirstDatum->OfShape());
         aSharingsIter.More() && aSDR.IsNull();
         aSharingsIter.Next())
    {
      aSDR = occ::down_cast<StepShape_ShapeDefinitionRepresentation>(aSharingsIter.Value());
    }
    if (aSDR.IsNull())
      return aDatumSystem;

    aGISU->Init(new TCollection_HAsciiString(),
                new TCollection_HAsciiString(),
                aDefinition,
                aSDR->UsedRepresentation(),
                anReprItems);
    aModel->AddWithRefs(anAxis);
    aModel->AddWithRefs(aGISU);
  }

  return aDatumSystem;
}

//=======================================================================
// function : writeToleranceZone
// purpose  : auxiliary (write tolerace zones)
//=======================================================================
void STEPCAFControl_Writer::writeToleranceZone(
  const occ::handle<XSControl_WorkSession>&                 theWS,
  const occ::handle<XCAFDimTolObjects_GeomToleranceObject>& theObject,
  const occ::handle<StepDimTol_GeometricTolerance>&         theEntity,
  const occ::handle<StepRepr_RepresentationContext>&        theRC)
{
  // Get working data
  const occ::handle<Interface_InterfaceModel>& aModel = theWS->Model();
  if (theEntity.IsNull() || theObject.IsNull())
    return;

  // Return if there is no tolerance zones
  if (theObject->GetTypeOfValue() == XCAFDimTolObjects_GeomToleranceTypeValue_None
      && theObject->GetZoneModifier() != XCAFDimTolObjects_GeomToleranceZoneModif_Runout)
  {
    return;
  }

  // Create Tolerance_Zone
  occ::handle<StepDimTol_ToleranceZoneForm> aForm = new StepDimTol_ToleranceZoneForm();
  aModel->AddWithRefs(aForm);
  aForm->Init(STEPCAFControl_GDTProperty::GetTolValueType(theObject->GetTypeOfValue()));
  occ::handle<NCollection_HArray1<StepDimTol_ToleranceZoneTarget>> aZoneTargetArray =
    new NCollection_HArray1<StepDimTol_ToleranceZoneTarget>(1, 1);
  StepDimTol_ToleranceZoneTarget aTarget;
  aTarget.SetValue(theEntity);
  aZoneTargetArray->SetValue(1, aTarget);
  occ::handle<StepDimTol_ToleranceZone> aZone = new StepDimTol_ToleranceZone();
  aZone->Init(new TCollection_HAsciiString(),
              new TCollection_HAsciiString(),
              theEntity->TolerancedShapeAspect().ShapeAspect()->OfShape(),
              StepData_LFalse,
              aZoneTargetArray,
              aForm);
  aModel->AddWithRefs(aZone);

  // Runout_Tolerance_Zone
  occ::handle<StepBasic_PlaneAngleMeasureWithUnit> aPAMWU =
    new StepBasic_PlaneAngleMeasureWithUnit();
  occ::handle<StepBasic_MeasureValueMember> aValueMember = new StepBasic_MeasureValueMember();
  aValueMember->SetName("PLANE_ANGLE_MEASURE");
  aValueMember->SetReal(theObject->GetValueOfZoneModifier());
  aPAMWU->Init(aValueMember, GetUnit(theRC, true));
  occ::handle<StepDimTol_RunoutZoneOrientation> anOrientation =
    new StepDimTol_RunoutZoneOrientation();
  anOrientation->Init(aPAMWU);
  occ::handle<StepDimTol_RunoutZoneDefinition> aDefinition = new StepDimTol_RunoutZoneDefinition();
  aDefinition->Init(aZone, nullptr, anOrientation);
  aModel->AddWithRefs(aDefinition);
  aModel->AddWithRefs(anOrientation);
  aModel->AddWithRefs(aPAMWU);
}

//=======================================================================
// function : writeGeomTolerance
// purpose  : auxiliary (write Geometric_Tolerance entity for given shapes,
//           label and datum system)
//======================================================================
void STEPCAFControl_Writer::writeGeomTolerance(
  const occ::handle<XSControl_WorkSession>&                                  theWS,
  const NCollection_Sequence<TDF_Label>&                                     theShapeSeqL,
  const TDF_Label&                                                           theGeomTolL,
  const occ::handle<NCollection_HArray1<StepDimTol_DatumSystemOrReference>>& theDatumSystem,
  const occ::handle<StepRepr_RepresentationContext>&                         theRC,
  const StepData_Factors&                                                    theLocalFactors)
{
  // Get working data
  const occ::handle<Interface_InterfaceModel>& aModel = theWS->Model();
  occ::handle<XCAFDoc_GeomTolerance>           aGTAttr;
  if (!theGeomTolL.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aGTAttr))
    return;
  occ::handle<XCAFDimTolObjects_GeomToleranceObject> anObject = aGTAttr->GetObject();
  if (anObject.IsNull())
    return;

  // Value
  occ::handle<StepBasic_LengthMeasureWithUnit> aLMWU        = new StepBasic_LengthMeasureWithUnit();
  StepBasic_Unit                               aUnit        = GetUnit(theRC);
  occ::handle<StepBasic_MeasureValueMember>    aValueMember = new StepBasic_MeasureValueMember();
  aValueMember->SetName("LENGTH_MEASURE");
  aValueMember->SetReal(anObject->GetValue());
  aLMWU->Init(aValueMember, aUnit);
  aModel->AddWithRefs(aLMWU);

  // Geometric_Tolerance target
  occ::handle<StepRepr_ShapeAspect>                 aMainSA;
  occ::handle<StepRepr_RepresentationContext>       dummyRC;
  occ::handle<StepAP242_GeometricItemSpecificUsage> dummyGISU;
  if (theShapeSeqL.Length() == 1)
  {
    TopoDS_Shape aShape = XCAFDoc_ShapeTool::GetShape(theShapeSeqL.Value(1));
    aMainSA             = writeShapeAspect(theWS, theGeomTolL, aShape, dummyRC, dummyGISU);
    aModel->AddWithRefs(aMainSA);
  }
  else
  {
    occ::handle<StepRepr_CompositeShapeAspect> aCSA;
    for (NCollection_Sequence<TDF_Label>::Iterator aShIter(theShapeSeqL); aShIter.More();
         aShIter.Next())
    {
      TopoDS_Shape                      aShape = XCAFDoc_ShapeTool::GetShape(aShIter.Value());
      occ::handle<StepRepr_ShapeAspect> aSA =
        writeShapeAspect(theWS, theGeomTolL, aShape, dummyRC, dummyGISU);
      if (aSA.IsNull())
        continue;
      if (aCSA.IsNull())
      {
        aCSA = new StepRepr_CompositeShapeAspect();
        aCSA->Init(aSA->Name(), aSA->Description(), aSA->OfShape(), aSA->ProductDefinitional());
        aModel->AddWithRefs(aCSA);
      }
      occ::handle<StepRepr_ShapeAspectRelationship> aSAR = new StepRepr_ShapeAspectRelationship();
      aSAR->Init(new TCollection_HAsciiString(), false, nullptr, aCSA, aSA);
      aModel->AddWithRefs(aSAR);
    }
    aMainSA = aCSA;
  }
  StepDimTol_GeometricToleranceTarget aGTTarget;
  aGTTarget.SetValue(aMainSA);

  bool isWithModif = false, isWithDatRef = false, isWithMaxTol = false;
  // Modifiers
  // Simple modifiers
  NCollection_Sequence<XCAFDimTolObjects_GeomToleranceModif> aModifiers = anObject->GetModifiers();
  occ::handle<NCollection_HArray1<StepDimTol_GeometricToleranceModifier>> aModifArray;
  occ::handle<StepBasic_LengthMeasureWithUnit>                            aMaxLMWU;
  int aModifNb = aModifiers.Length();
  if (anObject->GetMaterialRequirementModifier() != XCAFDimTolObjects_GeomToleranceMatReqModif_None)
    aModifNb++;
  for (int i = 1; i <= aModifiers.Length(); i++)
  {
    if (aModifiers.Value(i) == XCAFDimTolObjects_GeomToleranceModif_All_Around
        || aModifiers.Value(i) == XCAFDimTolObjects_GeomToleranceModif_All_Over)
    {
      aModifNb--;
    }
  }
  if (aModifNb > 0)
  {
    isWithModif = true;
    aModifArray = new NCollection_HArray1<StepDimTol_GeometricToleranceModifier>(1, aModifNb);
    int k       = 1;
    for (int i = 1; i <= aModifiers.Length(); i++)
    {
      if (aModifiers.Value(i) == XCAFDimTolObjects_GeomToleranceModif_All_Around
          || aModifiers.Value(i) == XCAFDimTolObjects_GeomToleranceModif_All_Over)
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
    else if (anObject->GetMaterialRequirementModifier()
             == XCAFDimTolObjects_GeomToleranceMatReqModif_M)
    {
      aModifArray->SetValue(aModifNb, StepDimTol_GTMMaximumMaterialRequirement);
    }
    // Modifier with value
    if (anObject->GetMaxValueModifier() != 0)
    {
      isWithMaxTol = true;
      aMaxLMWU     = new StepBasic_LengthMeasureWithUnit();
      occ::handle<StepBasic_MeasureValueMember> aModifierValueMember =
        new StepBasic_MeasureValueMember();
      aModifierValueMember->SetName("LENGTH_MEASURE");
      aModifierValueMember->SetReal(anObject->GetMaxValueModifier());
      aMaxLMWU->Init(aModifierValueMember, aUnit);
      aModel->AddWithRefs(aMaxLMWU);
    }
  }

  // Datum Reference
  isWithDatRef = !theDatumSystem.IsNull();

  // Collect all attributes
  occ::handle<TCollection_HAsciiString> aName        = new TCollection_HAsciiString(),
                                        aDescription = new TCollection_HAsciiString();
  occ::handle<StepDimTol_GeometricToleranceWithDatumReference> aGTWDR =
    new StepDimTol_GeometricToleranceWithDatumReference();
  aGTWDR->SetDatumSystem(theDatumSystem);
  occ::handle<StepDimTol_GeometricToleranceWithModifiers> aGTWM =
    new StepDimTol_GeometricToleranceWithModifiers();
  aGTWM->SetModifiers(aModifArray);
  StepDimTol_GeometricToleranceType aType =
    STEPCAFControl_GDTProperty::GetGeomToleranceType(anObject->GetType());

  // Init and write necessary subtype of Geometric_Tolerance entity
  occ::handle<StepDimTol_GeometricTolerance> aGeomTol;
  if (isWithModif)
  {
    if (isWithMaxTol)
    {
      if (isWithDatRef)
      {
        // Geometric_Tolerance & Geometric_Tolerance_With_Datum_Reference &
        // Geometric_Tolerance_With_Maximum_Tolerance & Geometric_Tolerance_With_Modifiers
        occ::handle<StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol> aResult =
          new StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol();
        aResult->Init(aName, aDescription, aLMWU, aGTTarget, aGTWDR, aGTWM, aMaxLMWU, aType);
        aGeomTol = aResult;
      }
      else
      {
        // Geometric_Tolerance & Geometric_Tolerance_With_Maximum_Tolerance &
        // Geometric_Tolerance_With_Modifiers
        occ::handle<StepDimTol_GeoTolAndGeoTolWthMaxTol> aResult =
          new StepDimTol_GeoTolAndGeoTolWthMaxTol();
        aResult->Init(aName, aDescription, aLMWU, aGTTarget, aGTWM, aMaxLMWU, aType);
        aGeomTol = aResult;
      }
    }
    else
    {
      if (isWithDatRef)
      {
        // Geometric_Tolerance & Geometric_Tolerance_With_Datum_Reference &
        // Geometric_Tolerance_With_Modifiers
        occ::handle<StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod> aResult =
          new StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod();
        aResult->Init(aName, aDescription, aLMWU, aGTTarget, aGTWDR, aGTWM, aType);
        aGeomTol = aResult;
      }
      else
      {
        // Geometric_Tolerance & Geometric_Tolerance_With_Modifiers
        occ::handle<StepDimTol_GeoTolAndGeoTolWthMod> aResult =
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
      occ::handle<StepDimTol_GeoTolAndGeoTolWthDatRef> aResult =
        new StepDimTol_GeoTolAndGeoTolWthDatRef();
      aResult->Init(aName, aDescription, aLMWU, aGTTarget, aGTWDR, aType);
      aGeomTol = aResult;
    }
    else
    {
      // Geometric_Tolerance
      occ::handle<StepDimTol_GeometricTolerance> aResult =
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
  // Annotation plane and Presentation
  writePresentation(theWS,
                    anObject->GetPresentation(),
                    anObject->GetPresentationName(),
                    true,
                    anObject->HasPlane(),
                    anObject->GetPlane(),
                    anObject->GetPointTextAttach(),
                    aGeomTol,
                    theLocalFactors);
}

//=================================================================================================

bool STEPCAFControl_Writer::writeDGTs(const occ::handle<XSControl_WorkSession>& theWS,
                                      const NCollection_Sequence<TDF_Label>&    theLabels) const
{

  if (theLabels.IsEmpty())
    return false;

  // get working data
  const occ::handle<Interface_InterfaceModel>& aModel = theWS->Model();
  const occ::handle<XSControl_TransferWriter>& aTW    = theWS->TransferWriter();
  const occ::handle<Transfer_FinderProcess>&   aFP    = aTW->FinderProcess();

  const occ::handle<Interface_HGraph> aHGraph = theWS->HGraph();
  if (aHGraph.IsNull())
    return false;

  Interface_Graph aGraph = aHGraph->Graph();
  NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>> aDatumMap;

  NCollection_Sequence<TDF_Label> aDGTLabels;
  // Iterate on requested shapes collect Tools
  for (NCollection_Map<TDF_Label>::Iterator aLabelIter(myRootLabels); aLabelIter.More();
       aLabelIter.Next())
  {
    const TDF_Label&                aLabel   = aLabelIter.Value();
    occ::handle<XCAFDoc_DimTolTool> aDGTTool = XCAFDoc_DocumentTool::DimTolTool(aLabel);
    NCollection_Sequence<TDF_Label> aDGTLS;
    aDGTTool->GetDatumLabels(aDGTLS);
    aDGTLabels.Append(aDGTLS);
  }

  if (aDGTLabels.IsEmpty())
    return false;

  for (NCollection_Sequence<TDF_Label>::Iterator aDGTIter(aDGTLabels); aDGTIter.More();
       aDGTIter.Next())
  {
    const TDF_Label&                aDatumL = aDGTIter.Value();
    NCollection_Sequence<TDF_Label> aShapeL;
    NCollection_Sequence<TDF_Label> aNullSeq;
    if (!XCAFDoc_DimTolTool::GetRefShapeLabel(aDatumL, aShapeL, aNullSeq))
      continue;
    // find target shape
    TopoDS_Shape    aShape = XCAFDoc_ShapeTool::GetShape(aShapeL.Value(1));
    TopLoc_Location aLoc;
    NCollection_Sequence<occ::handle<Standard_Transient>> aSeqRI;
    FindEntities(aFP, aShape, aLoc, aSeqRI);
    if (aSeqRI.IsEmpty())
    {
      Message::SendTrace() << "Warning: Cannot find RI for "
                           << aShape.TShape()->DynamicType()->Name() << "\n";
      continue;
    }
    occ::handle<StepRepr_ProductDefinitionShape> aPDS;
    occ::handle<StepRepr_RepresentationContext>  aRC;
    occ::handle<Standard_Transient>              anEnt = aSeqRI.Value(1);
    occ::handle<StepShape_AdvancedFace>          anAF;
    occ::handle<StepShape_EdgeCurve>             anEC;
    FindPDSforDGT(aGraph, anEnt, aPDS, aRC, anAF, anEC);
    if (aPDS.IsNull())
      continue;
    occ::handle<XCAFDoc_Datum> aDatumAttr;
    if (!aDatumL.FindAttribute(XCAFDoc_Datum::GetID(), aDatumAttr))
      continue;
    occ::handle<TCollection_HAsciiString> aName            = aDatumAttr->GetName();
    occ::handle<TCollection_HAsciiString> anIdentification = aDatumAttr->GetIdentification();
    occ::handle<TCollection_HAsciiString> aDescription     = aDatumAttr->GetDescription();
    if (aDescription.IsNull())
    {
      aDescription = new TCollection_HAsciiString();
    }
    occ::handle<StepDimTol_DatumFeature> aDF    = new StepDimTol_DatumFeature;
    occ::handle<StepDimTol_Datum>        aDatum = new StepDimTol_Datum;
    aDF->Init(aName, new TCollection_HAsciiString, aPDS, StepData_LTrue);
    aModel->AddWithRefs(aDF);
    aDatum->Init(aName, new TCollection_HAsciiString, aPDS, StepData_LFalse, anIdentification);
    aModel->AddWithRefs(aDatum);
    occ::handle<StepRepr_ShapeAspectRelationship> aSAR = new StepRepr_ShapeAspectRelationship;
    aSAR->SetName(aName);
    aSAR->SetRelatingShapeAspect(aDF);
    aSAR->SetRelatedShapeAspect(aDatum);
    aModel->AddWithRefs(aSAR);
    // write chain for DatumFeature
    StepRepr_CharacterizedDefinition aCD;
    aCD.SetValue(aDF);
    occ::handle<StepRepr_PropertyDefinition> aPropD = new StepRepr_PropertyDefinition;
    aPropD->Init(aName, true, aDescription, aCD);
    aModel->AddWithRefs(aPropD);
    StepRepr_RepresentedDefinition aRD;
    aRD.SetValue(aPropD);
    occ::handle<StepShape_ShapeRepresentation> aSR = new StepShape_ShapeRepresentation;
    occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> aHARI =
      new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, 1);
    aHARI->SetValue(1, anAF);
    aSR->Init(aName, aHARI, aRC);
    occ::handle<StepShape_ShapeDefinitionRepresentation> aSDR =
      new StepShape_ShapeDefinitionRepresentation;
    aSDR->Init(aRD, aSR);
    aModel->AddWithRefs(aSDR);
    // write chain for Datum
    StepRepr_CharacterizedDefinition aCD1;
    aCD1.SetValue(aDatum);
    occ::handle<StepRepr_PropertyDefinition> aPropD1 = new StepRepr_PropertyDefinition;
    aPropD1->Init(aName, true, aDescription, aCD1);
    aModel->AddWithRefs(aPropD1);
    StepRepr_RepresentedDefinition aRD1;
    aRD1.SetValue(aPropD1);
    occ::handle<StepShape_ShapeRepresentation> aSR1 = new StepShape_ShapeRepresentation;
    occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> aHARI1 =
      new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, 1);
    aHARI1->SetValue(1, anAF->FaceGeometry());
    aSR1->Init(aName, aHARI1, aRC);
    aModel->AddWithRefs(aSR1);
    occ::handle<StepShape_ShapeDefinitionRepresentation> aSDR1 =
      new StepShape_ShapeDefinitionRepresentation;
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
  for (NCollection_Map<TDF_Label>::Iterator aLabelIter(myRootLabels); aLabelIter.More();
       aLabelIter.Next())
  {
    const TDF_Label&                aLabel   = aLabelIter.Value();
    occ::handle<XCAFDoc_DimTolTool> aDGTTool = XCAFDoc_DocumentTool::DimTolTool(aLabel);
    NCollection_Sequence<TDF_Label> aaDGTLS;
    aDGTTool->GetDimTolLabels(aDGTLabels);
    aDGTLabels.Append(aaDGTLS);
  }

  if (aDGTLabels.IsEmpty())
    return false;
  for (NCollection_Sequence<TDF_Label>::Iterator aDGTIter(aDGTLabels); aDGTIter.More();
       aDGTIter.Next())
  {
    const TDF_Label&                aDimTolL = aDGTIter.Value();
    NCollection_Sequence<TDF_Label> aShapeL;
    NCollection_Sequence<TDF_Label> aNullSeq;
    if (!XCAFDoc_DimTolTool::GetRefShapeLabel(aDimTolL, aShapeL, aNullSeq))
      continue;
    // find target shape
    TopoDS_Shape    aShape = XCAFDoc_ShapeTool::GetShape(aShapeL.Value(1));
    TopLoc_Location aLoc;
    NCollection_Sequence<occ::handle<Standard_Transient>> seqRI;
    FindEntities(aFP, aShape, aLoc, seqRI);
    if (seqRI.IsEmpty())
    {
      Message::SendTrace() << "Warning: Cannot find RI for "
                           << aShape.TShape()->DynamicType()->Name() << "\n";
      continue;
    }
    occ::handle<StepRepr_ProductDefinitionShape> aPDS;
    occ::handle<StepRepr_RepresentationContext>  aRC;
    occ::handle<Standard_Transient>              anEnt = seqRI.Value(1);
    occ::handle<StepShape_AdvancedFace>          anAF;
    occ::handle<StepShape_EdgeCurve>             anEC;
    FindPDSforDGT(aGraph, anEnt, aPDS, aRC, anAF, anEC);
    if (aPDS.IsNull())
      continue;

    occ::handle<XCAFDoc_DimTol> aDimTolAttr;
    if (!aDimTolL.FindAttribute(XCAFDoc_DimTol::GetID(), aDimTolAttr))
      continue;
    int                                      aKind        = aDimTolAttr->GetKind();
    occ::handle<NCollection_HArray1<double>> aVal         = aDimTolAttr->GetVal();
    occ::handle<TCollection_HAsciiString>    aName        = aDimTolAttr->GetName();
    occ::handle<TCollection_HAsciiString>    aDescription = aDimTolAttr->GetDescription();

    // common part of writing D&GT entities
    StepRepr_CharacterizedDefinition  aCD;
    occ::handle<StepRepr_ShapeAspect> aSA = new StepRepr_ShapeAspect;
    aSA->Init(aName, new TCollection_HAsciiString, aPDS, StepData_LTrue);
    aModel->AddWithRefs(aSA);
    aCD.SetValue(aSA);
    occ::handle<StepRepr_PropertyDefinition> aPropD = new StepRepr_PropertyDefinition;
    aPropD->Init(aName, true, aDescription, aCD);
    aModel->AddWithRefs(aPropD);
    StepRepr_RepresentedDefinition aRD;
    aRD.SetValue(aPropD);
    occ::handle<StepShape_ShapeRepresentation> aSR = new StepShape_ShapeRepresentation;
    occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> aHARI =
      new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, 1);
    if (aKind < 20)
      aHARI->SetValue(1, anEC);
    else
      aHARI->SetValue(1, anAF);
    aSR->Init(aName, aHARI, aRC);
    occ::handle<StepShape_ShapeDefinitionRepresentation> aSDR =
      new StepShape_ShapeDefinitionRepresentation;
    aSDR->Init(aRD, aSR);
    aModel->AddWithRefs(aSDR);
    // define aUnit for creation LengthMeasureWithUnit (common for all)
    StepBasic_Unit aUnit;
    aUnit = GetUnit(aRC);

    // specific part of writing D&GT entities
    if (aKind < 20)
    { // dimension
      occ::handle<StepShape_DimensionalSize> aDimSize = new StepShape_DimensionalSize;
      aDimSize->Init(aSA, aDescription);
      aModel->AddWithRefs(aDimSize);
      if (aVal->Length() > 1)
      {
        // create MeasureWithUnits
        occ::handle<StepBasic_MeasureValueMember> aMVM1 = new StepBasic_MeasureValueMember;
        aMVM1->SetName("POSITIVE_LENGTH_MEASURE");
        aMVM1->SetReal(aVal->Value(1));
        occ::handle<StepBasic_MeasureWithUnit> aMWU1 = new StepBasic_MeasureWithUnit;
        aMWU1->Init(aMVM1, aUnit);
        occ::handle<StepBasic_MeasureValueMember> aMVM2 = new StepBasic_MeasureValueMember;
        aMVM2->SetName("POSITIVE_LENGTH_MEASURE");
        aMVM2->SetReal(aVal->Value(2));
        occ::handle<StepBasic_MeasureWithUnit> aMWU2 = new StepBasic_MeasureWithUnit;
        aMWU2->Init(aMVM2, aUnit);
        occ::handle<StepRepr_RepresentationItem> aRI1 = new StepRepr_RepresentationItem;
        aRI1->Init(new TCollection_HAsciiString("lower limit"));
        occ::handle<StepRepr_RepresentationItem> aRI2 = new StepRepr_RepresentationItem;
        aRI2->Init(new TCollection_HAsciiString("upper limit"));
        occ::handle<StepRepr_ReprItemAndLengthMeasureWithUnit> aRILMU1 =
          new StepRepr_ReprItemAndLengthMeasureWithUnit;
        aRILMU1->Init(aMWU1, aRI1);
        occ::handle<StepRepr_ReprItemAndLengthMeasureWithUnit> aRILMU2 =
          new StepRepr_ReprItemAndLengthMeasureWithUnit;
        aRILMU2->Init(aMWU2, aRI2);
        aModel->AddWithRefs(aRILMU1);
        aModel->AddWithRefs(aRILMU2);
        occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> aHARIVR =
          new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, 2);
        aHARIVR->SetValue(1, aRILMU1);
        aHARIVR->SetValue(2, aRILMU2);
        occ::handle<StepRepr_ValueRange> aVR = new StepRepr_ValueRange;
        // VR->Init(aName,CID);
        aVR->Init(aName, aHARIVR);
        aModel->AddEntity(aVR);
        occ::handle<StepShape_ShapeDimensionRepresentation> aSDimR =
          new StepShape_ShapeDimensionRepresentation;
        occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> aHARI1 =
          new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, 1);
        aHARI1->SetValue(1, aVR);
        aSDimR->Init(aName, aHARI1, aRC);
        aModel->AddWithRefs(aSDimR);
        occ::handle<StepShape_DimensionalCharacteristicRepresentation> aDimCharR =
          new StepShape_DimensionalCharacteristicRepresentation;
        StepShape_DimensionalCharacteristic aDimChar;
        aDimChar.SetValue(aDimSize);
        aDimCharR->Init(aDimChar, aSDimR);
        aModel->AddEntity(aDimCharR);
      }
    }
    else if (aKind < 50)
    { // tolerance
      if (aKind < 35)
      { // tolerance with datum system
        NCollection_Sequence<TDF_Label> aDatumLabels;
        XCAFDoc_DimTolTool::GetDatumOfTolerLabels(aDimTolL, aDatumLabels);
        int                                                                      aSetDatumInd = 1;
        occ::handle<NCollection_HArray1<occ::handle<StepDimTol_DatumReference>>> aHADR =
          new NCollection_HArray1<occ::handle<StepDimTol_DatumReference>>(1, aDatumLabels.Length());
        for (NCollection_Sequence<TDF_Label>::Iterator aDatumIter(aDatumLabels); aDatumIter.More();
             aDatumIter.Next(), aSetDatumInd++)
        {
          occ::handle<XCAFDoc_Datum> aDatumAttr;
          const TDF_Label&           aDatumL = aDatumIter.Value();
          if (!aDatumL.FindAttribute(XCAFDoc_Datum::GetID(), aDatumAttr))
            continue;
          occ::handle<TCollection_HAsciiString> aNameD            = aDatumAttr->GetName();
          occ::handle<TCollection_HAsciiString> aDescriptionD     = aDatumAttr->GetDescription();
          occ::handle<TCollection_HAsciiString> anIdentificationD = aDatumAttr->GetIdentification();
          TCollection_AsciiString               aStmp(aNameD->ToCString());
          aStmp.AssignCat(aDescriptionD->ToCString());
          aStmp.AssignCat(anIdentificationD->ToCString());
          if (aDatumMap.IsBound(aStmp))
          {
            occ::handle<StepDimTol_Datum> aDatum =
              occ::down_cast<StepDimTol_Datum>(aDatumMap.Find(aStmp));
            occ::handle<StepDimTol_DatumReference> aDR = new StepDimTol_DatumReference;
            aDR->Init(aSetDatumInd, aDatum);
            aModel->AddWithRefs(aDR);
            aHADR->SetValue(aSetDatumInd, aDR);
          }
        }
        // create LengthMeasureWithUnit
        occ::handle<StepBasic_MeasureValueMember> aMVM = new StepBasic_MeasureValueMember;
        aMVM->SetName("LENGTH_MEASURE");
        aMVM->SetReal(aVal->Value(1));
        occ::handle<StepBasic_LengthMeasureWithUnit> aLMWU = new StepBasic_LengthMeasureWithUnit;
        aLMWU->Init(aMVM, aUnit);
        // create tolerance by it's type
        if (aKind < 24)
        {
          occ::handle<StepDimTol_GeometricToleranceWithDatumReference> aGTWDR =
            new StepDimTol_GeometricToleranceWithDatumReference;
          aGTWDR->SetDatumSystem(aHADR);
          occ::handle<StepDimTol_ModifiedGeometricTolerance> aMGT =
            new StepDimTol_ModifiedGeometricTolerance;
          if (aKind == 21)
            aMGT->SetModifier(StepDimTol_MaximumMaterialCondition);
          else if (aKind == 22)
            aMGT->SetModifier(StepDimTol_LeastMaterialCondition);
          else if (aKind == 23)
            aMGT->SetModifier(StepDimTol_RegardlessOfFeatureSize);
          occ::handle<StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol> aGTComplex =
            new StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol;
          aGTComplex->Init(aName, aDescription, aLMWU, aSA, aGTWDR, aMGT);
          aModel->AddWithRefs(aGTComplex);
        }
        else if (aKind == 24)
        {
          occ::handle<StepDimTol_AngularityTolerance> aToler = new StepDimTol_AngularityTolerance;
          aToler->Init(aName, aDescription, aLMWU, aSA, aHADR);
          aModel->AddWithRefs(aToler);
        }
        else if (aKind == 25)
        {
          occ::handle<StepDimTol_CircularRunoutTolerance> aToler =
            new StepDimTol_CircularRunoutTolerance;
          aToler->Init(aName, aDescription, aLMWU, aSA, aHADR);
          aModel->AddWithRefs(aToler);
        }
        else if (aKind == 26)
        {
          occ::handle<StepDimTol_CoaxialityTolerance> aToler = new StepDimTol_CoaxialityTolerance;
          aToler->Init(aName, aDescription, aLMWU, aSA, aHADR);
          aModel->AddWithRefs(aToler);
        }
        else if (aKind == 27)
        {
          occ::handle<StepDimTol_ConcentricityTolerance> aToler =
            new StepDimTol_ConcentricityTolerance;
          aToler->Init(aName, aDescription, aLMWU, aSA, aHADR);
          aModel->AddWithRefs(aToler);
        }
        else if (aKind == 28)
        {
          occ::handle<StepDimTol_ParallelismTolerance> aToler = new StepDimTol_ParallelismTolerance;
          aToler->Init(aName, aDescription, aLMWU, aSA, aHADR);
          aModel->AddWithRefs(aToler);
        }
        else if (aKind == 29)
        {
          occ::handle<StepDimTol_PerpendicularityTolerance> aToler =
            new StepDimTol_PerpendicularityTolerance;
          aToler->Init(aName, aDescription, aLMWU, aSA, aHADR);
          aModel->AddWithRefs(aToler);
        }
        else if (aKind == 30)
        {
          occ::handle<StepDimTol_SymmetryTolerance> aToler = new StepDimTol_SymmetryTolerance;
          aToler->Init(aName, aDescription, aLMWU, aSA, aHADR);
          aModel->AddWithRefs(aToler);
        }
        else if (aKind == 31)
        {
          occ::handle<StepDimTol_TotalRunoutTolerance> aToler = new StepDimTol_TotalRunoutTolerance;
          aToler->Init(aName, aDescription, aLMWU, aSA, aHADR);
          aModel->AddWithRefs(aToler);
        }
      }
    }
  }

  return true;
}

//=================================================================================================

bool STEPCAFControl_Writer::writeDGTsAP242(const occ::handle<XSControl_WorkSession>& theWS,
                                           const NCollection_Sequence<TDF_Label>&    theLabels,
                                           const StepData_Factors& theLocalFactors)
{
  (void)theLabels;
  // Get working data
  const occ::handle<Interface_InterfaceModel>& aModel = theWS->Model();

  const occ::handle<Interface_HGraph> aHGraph = theWS->HGraph();
  if (aHGraph.IsNull())
    return false;

  Interface_Graph aGraph = aHGraph->Graph();

  // Common entities for presentation
  STEPConstruct_Styles           aStyles(theWS);
  occ::handle<StepVisual_Colour> aCurvColor = STEPConstruct_Styles::EncodeColor(Quantity_NOC_WHITE);
  occ::handle<StepRepr_RepresentationItem> anItem = nullptr;
  myGDTPrsCurveStyle->SetValue(
    1,
    aStyles.MakeColorPSA(anItem, aCurvColor, aCurvColor, STEPConstruct_RenderingProperties()));
  for (Interface_EntityIterator aModelIter = aModel->Entities();
       aModelIter.More() && myGDTCommonPDS.IsNull();
       aModelIter.Next())
  {
    myGDTCommonPDS = occ::down_cast<StepRepr_ProductDefinitionShape>(aModelIter.Value());
  }

  NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>> aDatumMap;
  occ::handle<StepRepr_RepresentationContext>                                   aRC;

  NCollection_Sequence<TDF_Label> aDGTLabels;
  // Iterate on requested shapes collect Tools
  for (NCollection_Map<TDF_Label>::Iterator aLabelIter(myRootLabels); aLabelIter.More();
       aLabelIter.Next())
  {
    const TDF_Label&                aLabel   = aLabelIter.Value();
    occ::handle<XCAFDoc_DimTolTool> aDGTTool = XCAFDoc_DocumentTool::DimTolTool(aLabel);
    NCollection_Sequence<TDF_Label> aaDGTLS;
    aDGTTool->GetDatumLabels(aDGTLabels);
    aDGTLabels.Append(aaDGTLS);
  }

  // Find all shapes with datums
  NCollection_Map<TCollection_AsciiString> aNameIdMap;
  for (NCollection_Sequence<TDF_Label>::Iterator aDGTIter(aDGTLabels); aDGTIter.More();
       aDGTIter.Next())
  {
    const TDF_Label&                aDatumL = aDGTIter.Value();
    NCollection_Sequence<TDF_Label> aShapeL, aNullSeq;
    XCAFDoc_DimTolTool::GetRefShapeLabel(aDatumL, aShapeL, aNullSeq);
    occ::handle<XCAFDoc_Datum> aDatumAttr;
    aDatumL.FindAttribute(XCAFDoc_Datum::GetID(), aDatumAttr);
    occ::handle<XCAFDimTolObjects_DatumObject> anObject   = aDatumAttr->GetObject();
    TCollection_AsciiString                    aDatumName = anObject->GetName()->String();
    TCollection_AsciiString                    aDatumTargetId =
      TCollection_AsciiString(anObject->GetDatumTargetNumber());
    if (!aNameIdMap.Add(aDatumName.Cat(aDatumTargetId)))
      continue;
    occ::handle<Standard_Transient> aWrittenDatum;
    bool                            isFirstDT = !aDatumMap.Find(aDatumName, aWrittenDatum);
    occ::handle<StepDimTol_Datum>   aDatum =
      writeDatumAP242(theWS,
                      aShapeL,
                      aDatumL,
                      isFirstDT,
                      occ::down_cast<StepDimTol_Datum>(aWrittenDatum),
                      theLocalFactors);
    // Add created Datum into Map
    aDatumMap.Bind(aDatumName, aDatum);
  }

  // write Dimensions
  aDGTLabels.Clear();
  for (NCollection_Map<TDF_Label>::Iterator aLabelIter(myRootLabels); aLabelIter.More();
       aLabelIter.Next())
  {
    const TDF_Label&                aLabel   = aLabelIter.Value();
    occ::handle<XCAFDoc_DimTolTool> aDGTTool = XCAFDoc_DocumentTool::DimTolTool(aLabel);
    NCollection_Sequence<TDF_Label> aaDGTLS;
    aDGTTool->GetDimensionLabels(aDGTLabels);
    aDGTLabels.Append(aaDGTLS);
  }

  // Auxiliary entities for derived geometry
  occ::handle<StepRepr_ConstructiveGeometryRepresentation> aCGRepr =
    new StepRepr_ConstructiveGeometryRepresentation();
  occ::handle<StepRepr_ConstructiveGeometryRepresentationRelationship> aCGReprRel =
    new StepRepr_ConstructiveGeometryRepresentationRelationship();
  NCollection_Vector<occ::handle<StepGeom_CartesianPoint>> aConnectionPnts;
  occ::handle<StepRepr_RepresentationContext>              dummyRC;
  occ::handle<StepAP242_GeometricItemSpecificUsage>        dummyGISU;
  for (NCollection_Sequence<TDF_Label>::Iterator aDGTIter(aDGTLabels); aDGTIter.More();
       aDGTIter.Next())
  {
    const TDF_Label&                aDimensionL = aDGTIter.Value();
    NCollection_Sequence<TDF_Label> aFirstShapeL, aSecondShapeL;
    occ::handle<XCAFDoc_Dimension>  aDimAttr;
    if (!aDimensionL.FindAttribute(XCAFDoc_Dimension::GetID(), aDimAttr))
      continue;
    occ::handle<XCAFDimTolObjects_DimensionObject> anObject = aDimAttr->GetObject();
    if (anObject.IsNull())
      continue;
    if (anObject->GetType() == XCAFDimTolObjects_DimensionType_CommonLabel)
    {
      occ::handle<StepRepr_ShapeAspect> aSA = new StepRepr_ShapeAspect();
      aSA->Init(new TCollection_HAsciiString(),
                new TCollection_HAsciiString(),
                myGDTCommonPDS,
                StepData_LTrue);
      aModel->AddWithRefs(aSA);
      writePresentation(theWS,
                        anObject->GetPresentation(),
                        anObject->GetPresentationName(),
                        anObject->HasPlane(),
                        false,
                        anObject->GetPlane(),
                        anObject->GetPointTextAttach(),
                        aSA,
                        theLocalFactors);
    }

    if (!XCAFDoc_DimTolTool::GetRefShapeLabel(aDimensionL, aFirstShapeL, aSecondShapeL))
      continue;

    // Write links with shapes
    occ::handle<StepRepr_ShapeAspect> aFirstSA, aSecondSA;
    if (aFirstShapeL.Length() == 1)
    {
      TopoDS_Shape aShape = XCAFDoc_ShapeTool::GetShape(aFirstShapeL.Value(1));
      aFirstSA            = writeShapeAspect(theWS, aDimensionL, aShape, dummyRC, dummyGISU);
      if (aRC.IsNull() && !dummyRC.IsNull())
        aRC = dummyRC;
    }
    else if (aFirstShapeL.Length() > 1)
    {
      occ::handle<StepRepr_CompositeShapeAspect> aCSA;
      for (int shIt = 1; shIt <= aFirstShapeL.Length(); shIt++)
      {
        TopoDS_Shape aShape = XCAFDoc_ShapeTool::GetShape(aFirstShapeL.Value(shIt));
        occ::handle<StepRepr_ShapeAspect> aSA =
          writeShapeAspect(theWS, aDimensionL, aShape, dummyRC, dummyGISU);
        if (aSA.IsNull())
          continue;
        if (aCSA.IsNull())
        {
          aCSA = new StepRepr_CompositeShapeAspect();
          aCSA->Init(aSA->Name(), aSA->Description(), aSA->OfShape(), aSA->ProductDefinitional());
          aModel->AddWithRefs(aCSA);
        }
        occ::handle<StepRepr_ShapeAspectRelationship> aSAR = new StepRepr_ShapeAspectRelationship();
        aSAR->Init(new TCollection_HAsciiString(),
                   false,
                   new TCollection_HAsciiString(),
                   aCSA,
                   aSA);
        aModel->AddWithRefs(aSAR);
        if (aRC.IsNull() && !dummyRC.IsNull())
          aRC = dummyRC;
      }
      aFirstSA = aCSA;
    }
    if (aSecondShapeL.Length() == 1)
    {
      TopoDS_Shape aShape = XCAFDoc_ShapeTool::GetShape(aSecondShapeL.Value(1));
      aSecondSA           = writeShapeAspect(theWS, aDimensionL, aShape, dummyRC, dummyGISU);
      if (aRC.IsNull() && !dummyRC.IsNull())
        aRC = dummyRC;
    }
    else if (aSecondShapeL.Length() > 1)
    {
      occ::handle<StepRepr_CompositeShapeAspect> aCSA;
      for (int shIt = 1; shIt <= aSecondShapeL.Length(); shIt++)
      {
        TopoDS_Shape aShape = XCAFDoc_ShapeTool::GetShape(aSecondShapeL.Value(shIt));
        occ::handle<StepRepr_ShapeAspect> aSA =
          writeShapeAspect(theWS, aDimensionL, aShape, dummyRC, dummyGISU);
        if (aCSA.IsNull() && !aSA.IsNull())
        {
          aCSA = new StepRepr_CompositeShapeAspect();
        }
        aCSA->Init(aSA->Name(), aSA->Description(), aSA->OfShape(), aSA->ProductDefinitional());
        if (!aSA.IsNull())
        {
          occ::handle<StepRepr_ShapeAspectRelationship> aSAR =
            new StepRepr_ShapeAspectRelationship();
          aSAR->Init(new TCollection_HAsciiString(),
                     false,
                     new TCollection_HAsciiString(),
                     aCSA,
                     aSA);
          aModel->AddWithRefs(aSAR);
        }
        if (aRC.IsNull() && !dummyRC.IsNull())
          aRC = dummyRC;
      }
      aSecondSA = aCSA;
    }

    if (anObject->GetType() == XCAFDimTolObjects_DimensionType_DimensionPresentation)
    {
      writePresentation(theWS,
                        anObject->GetPresentation(),
                        anObject->GetPresentationName(),
                        anObject->HasPlane(),
                        false,
                        anObject->GetPlane(),
                        anObject->GetPointTextAttach(),
                        aFirstSA,
                        theLocalFactors);
      continue;
    }

    // Write dimensions
    StepShape_DimensionalCharacteristic aDimension;
    if (anObject->HasPoint() || anObject->HasPoint2())
      WriteDerivedGeometry(theWS,
                           anObject,
                           aCGRepr,
                           aFirstSA,
                           aSecondSA,
                           aConnectionPnts,
                           theLocalFactors);
    XCAFDimTolObjects_DimensionType aDimType = anObject->GetType();
    if (XCAFDimTolObjects_DimensionObject::IsDimensionalLocation(aDimType))
    {
      // Dimensional_Location
      occ::handle<StepShape_DimensionalLocation> aDim = new StepShape_DimensionalLocation();
      aDim->Init(STEPCAFControl_GDTProperty::GetDimTypeName(aDimType),
                 false,
                 nullptr,
                 aFirstSA,
                 aSecondSA);
      aDimension.SetValue(aDim);
    }
    else if (aDimType == XCAFDimTolObjects_DimensionType_Location_Angular)
    {
      // Angular_Location
      occ::handle<StepShape_AngularLocation> aDim     = new StepShape_AngularLocation();
      StepShape_AngleRelator                 aRelator = StepShape_Equal;
      if (anObject->HasQualifier())
      {
        XCAFDimTolObjects_AngularQualifier aQualifier = anObject->GetAngularQualifier();
        switch (aQualifier)
        {
          case XCAFDimTolObjects_AngularQualifier_Small:
            aRelator = StepShape_Small;
            break;
          case XCAFDimTolObjects_AngularQualifier_Large:
            aRelator = StepShape_Large;
            break;
          default:
            aRelator = StepShape_Equal;
        }
      }
      aDim->Init(new TCollection_HAsciiString(), false, nullptr, aFirstSA, aSecondSA, aRelator);
      aDimension.SetValue(aDim);
    }
    else if (aDimType == XCAFDimTolObjects_DimensionType_Location_WithPath)
    {
      // Dimensional_Location_With_Path
      occ::handle<StepShape_DimensionalLocationWithPath> aDim =
        new StepShape_DimensionalLocationWithPath();
      occ::handle<StepRepr_ShapeAspect> aPathSA =
        writeShapeAspect(theWS, aDimensionL, anObject->GetPath(), dummyRC, dummyGISU);
      aDim->Init(new TCollection_HAsciiString(), false, nullptr, aFirstSA, aSecondSA, aPathSA);
      aDimension.SetValue(aDim);
    }
    else if (XCAFDimTolObjects_DimensionObject::IsDimensionalSize(aDimType))
    {
      // Dimensional_Size
      occ::handle<StepShape_DimensionalSize> aDim = new StepShape_DimensionalSize();
      aDim->Init(aFirstSA, STEPCAFControl_GDTProperty::GetDimTypeName(aDimType));
      aDimension.SetValue(aDim);
    }
    else if (aDimType == XCAFDimTolObjects_DimensionType_Size_Angular)
    {
      // Angular_Size
      occ::handle<StepShape_AngularSize> aDim     = new StepShape_AngularSize();
      StepShape_AngleRelator             aRelator = StepShape_Equal;
      if (anObject->HasQualifier())
      {
        XCAFDimTolObjects_AngularQualifier aQualifier = anObject->GetAngularQualifier();
        switch (aQualifier)
        {
          case XCAFDimTolObjects_AngularQualifier_Small:
            aRelator = StepShape_Small;
            break;
          case XCAFDimTolObjects_AngularQualifier_Large:
            aRelator = StepShape_Large;
            break;
          default:
            aRelator = StepShape_Equal;
        }
      }
      aDim->Init(aFirstSA, new TCollection_HAsciiString(), aRelator);
      aDimension.SetValue(aDim);
    }
    else if (aDimType == XCAFDimTolObjects_DimensionType_Size_WithPath)
    {
      // Dimensional_Size_With_Path
      occ::handle<StepShape_DimensionalSizeWithPath> aDim = new StepShape_DimensionalSizeWithPath();
      occ::handle<StepRepr_ShapeAspect>              aPathSA =
        writeShapeAspect(theWS, aDimensionL, anObject->GetPath(), dummyRC, dummyGISU);
      aDim->Init(aFirstSA, new TCollection_HAsciiString(), aPathSA);
      aDimension.SetValue(aDim);
    }

    // Write values
    WriteDimValues(theWS, anObject, aRC, aDimension, theLocalFactors);
    // Annotation plane and Presentation
    writePresentation(theWS,
                      anObject->GetPresentation(),
                      anObject->GetPresentationName(),
                      true,
                      anObject->HasPlane(),
                      anObject->GetPlane(),
                      anObject->GetPointTextAttach(),
                      aDimension.Value(),
                      theLocalFactors);
  }
  // Write Derived geometry
  if (aConnectionPnts.Length() > 0)
  {
    occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> anItems =
      new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1,
                                                                        aConnectionPnts.Length());
    for (int i = 0; i < aConnectionPnts.Length(); i++)
      anItems->SetValue(i + 1, aConnectionPnts(i));
    aCGRepr->Init(new TCollection_HAsciiString(), anItems, dummyRC);
    aCGReprRel->Init(new TCollection_HAsciiString(),
                     new TCollection_HAsciiString(),
                     dummyGISU->UsedRepresentation(),
                     aCGRepr);
    aModel->AddWithRefs(aCGReprRel);
  }

  // write Geometric Tolerances
  aDGTLabels.Clear();
  for (NCollection_Map<TDF_Label>::Iterator aLabelIter(myRootLabels); aLabelIter.More();
       aLabelIter.Next())
  {
    const TDF_Label&                aLabel   = aLabelIter.Value();
    occ::handle<XCAFDoc_DimTolTool> aDGTTool = XCAFDoc_DocumentTool::DimTolTool(aLabel);
    NCollection_Sequence<TDF_Label> aaDGTLS;
    aDGTTool->GetGeomToleranceLabels(aDGTLabels);
    aDGTLabels.Append(aaDGTLS);
  }
  for (NCollection_Sequence<TDF_Label>::Iterator aDGTIter(aDGTLabels); aDGTIter.More();
       aDGTIter.Next())
  {
    const TDF_Label                 aGeomTolL = aDGTIter.Value();
    NCollection_Sequence<TDF_Label> aFirstShapeL, aNullSeqL;
    if (!XCAFDoc_DimTolTool::GetRefShapeLabel(aGeomTolL, aFirstShapeL, aNullSeqL))
      continue;
    NCollection_Sequence<TDF_Label> aDatumSeq;
    XCAFDoc_DimTolTool::GetDatumWithObjectOfTolerLabels(aGeomTolL, aDatumSeq);
    occ::handle<NCollection_HArray1<StepDimTol_DatumSystemOrReference>> aDatumSystem;
    if (aDatumSeq.Length() > 0)
      aDatumSystem = WriteDatumSystem(theWS, aGeomTolL, aDatumSeq, aDatumMap, aRC, theLocalFactors);
    writeGeomTolerance(theWS, aFirstShapeL, aGeomTolL, aDatumSystem, aRC, theLocalFactors);
  }

  // Write Draughting model for Annotation Planes
  if (myGDTAnnotations.Length() == 0)
    return true;

  occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> aItems =
    new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, myGDTAnnotations.Length());
  for (int i = 1; i <= aItems->Length(); i++)
  {
    aItems->SetValue(i, myGDTAnnotations.Value(i - 1));
  }
  myGDTPresentationDM->Init(new TCollection_HAsciiString(), aItems, aRC);
  aModel->AddWithRefs(myGDTPresentationDM);

  return true;
}

//=================================================================================================

static bool FindPDSforRI(const Interface_Graph&                        theGraph,
                         const occ::handle<Standard_Transient>&        theEnt,
                         occ::handle<StepRepr_ProductDefinitionShape>& thePDS,
                         occ::handle<StepRepr_RepresentationContext>&  theRC)
{
  if (theEnt.IsNull() || !theEnt->IsKind(STANDARD_TYPE(StepRepr_RepresentationItem)))
    return false;
  for (Interface_EntityIterator aSharingIter = theGraph.Sharings(theEnt);
       aSharingIter.More() && thePDS.IsNull();
       aSharingIter.Next())
  {
    occ::handle<StepShape_ShapeRepresentation> aSR =
      occ::down_cast<StepShape_ShapeRepresentation>(aSharingIter.Value());
    if (aSR.IsNull())
      continue;
    theRC = aSR->ContextOfItems();
    for (Interface_EntityIterator aSubs1 = theGraph.Sharings(aSR); aSubs1.More() && thePDS.IsNull();
         aSubs1.Next())
    {
      occ::handle<StepShape_ShapeDefinitionRepresentation> aSDR =
        occ::down_cast<StepShape_ShapeDefinitionRepresentation>(aSubs1.Value());
      if (aSDR.IsNull())
        continue;
      occ::handle<StepRepr_PropertyDefinition> aPropD = aSDR->Definition().PropertyDefinition();
      if (aPropD.IsNull())
        continue;
      thePDS = occ::down_cast<StepRepr_ProductDefinitionShape>(aPropD);
    }
  }
  return true;
}

//=================================================================================================

bool STEPCAFControl_Writer::writeMaterials(const occ::handle<XSControl_WorkSession>& theWS,
                                           const NCollection_Sequence<TDF_Label>& theLabels) const
{

  if (theLabels.IsEmpty())
    return false;

  // get working data
  const occ::handle<Interface_InterfaceModel>& aModel = theWS->Model();
  const occ::handle<XSControl_TransferWriter>& aTW    = theWS->TransferWriter();
  const occ::handle<Transfer_FinderProcess>&   aFP    = aTW->FinderProcess();

  const occ::handle<Interface_HGraph> aHGraph = theWS->HGraph();
  if (aHGraph.IsNull())
    return false;

  Interface_Graph aGraph = theWS->HGraph()->Graph();

  NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>> aMapDRI, aMapMRI;
  NCollection_Sequence<TDF_Label>                                               aTopLabels;
  // Iterate on requested shapes collect Tools
  for (NCollection_Map<TDF_Label>::Iterator aLabelIter(myRootLabels); aLabelIter.More();
       aLabelIter.Next())
  {
    const TDF_Label&                aLabel  = aLabelIter.Value();
    occ::handle<XCAFDoc_ShapeTool>  aShTool = XCAFDoc_DocumentTool::ShapeTool(aLabel);
    NCollection_Sequence<TDF_Label> aTopInterLabels;
    aShTool->GetShapes(aTopInterLabels);
    aTopLabels.Append(aTopInterLabels);
  }
  for (NCollection_Sequence<TDF_Label>::Iterator aTopLIter(aTopLabels); aTopLIter.More();
       aTopLIter.Next())
  {
    const TDF_Label&               aShL = aTopLIter.Value();
    occ::handle<TDataStd_TreeNode> aNode;
    if (!aShL.FindAttribute(XCAFDoc::MaterialRefGUID(), aNode) || !aNode->HasFather())
    {
      continue;
    }
    // find PDS for current shape
    TopoDS_Shape    aShape = XCAFDoc_ShapeTool::GetShape(aShL);
    TopLoc_Location aLocation;
    NCollection_Sequence<occ::handle<Standard_Transient>> aSeqRI;
    FindEntities(aFP, aShape, aLocation, aSeqRI);
    if (aSeqRI.Length() <= 0)
      continue;
    occ::handle<StepRepr_ProductDefinitionShape> aPDS;
    occ::handle<StepRepr_RepresentationContext>  aRC;
    occ::handle<Standard_Transient>              anEnt = aSeqRI.Value(1);
    FindPDSforRI(aGraph, anEnt, aPDS, aRC);
    if (aPDS.IsNull())
      continue;
    occ::handle<StepBasic_ProductDefinition> aProdDef = aPDS->Definition().ProductDefinition();
    if (aProdDef.IsNull())
      continue;
    // write material entities
    TDF_Label                             aMatL = aNode->Father()->Label();
    occ::handle<TCollection_HAsciiString> aName;
    occ::handle<TCollection_HAsciiString> aDescription;
    double                                aDensity;
    occ::handle<TCollection_HAsciiString> aDensName;
    occ::handle<TCollection_HAsciiString> aDensValType;
    occ::handle<StepRepr_Representation>  aRepDRI;
    occ::handle<StepRepr_Representation>  aRepMRI;
    if (XCAFDoc_MaterialTool::GetMaterial(aMatL,
                                          aName,
                                          aDescription,
                                          aDensity,
                                          aDensName,
                                          aDensValType))
    {
      if (aName->Length() == 0)
        continue;
      TCollection_AsciiString aKey(aName->ToCString());
      if (aMapDRI.IsBound(aKey))
      {
        aRepDRI = occ::down_cast<StepRepr_Representation>(aMapDRI.Find(aKey));
        if (aMapMRI.IsBound(aKey))
        {
          aRepMRI = occ::down_cast<StepRepr_Representation>(aMapMRI.Find(aKey));
        }
      }
      else
      {
        // write DRI
        occ::handle<StepRepr_DescriptiveRepresentationItem> aDRI =
          new StepRepr_DescriptiveRepresentationItem;
        aDRI->Init(aName, aDescription);
        occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> aHARI =
          new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, 1);
        aHARI->SetValue(1, aDRI);
        aRepDRI = new StepRepr_Representation();
        aRepDRI->Init(new TCollection_HAsciiString("material name"), aHARI, aRC);
        aModel->AddWithRefs(aRepDRI);
        // write MRI
        if (aDensity > 0)
        {
          // mass
          occ::handle<StepBasic_SiUnitAndMassUnit> aSMU = new StepBasic_SiUnitAndMassUnit;
          aSMU->SetName(StepBasic_sunGram);
          occ::handle<StepBasic_DerivedUnitElement> aDUE1 = new StepBasic_DerivedUnitElement;
          aDUE1->Init(aSMU, 3.0);
          // length
          occ::handle<StepBasic_SiUnitAndLengthUnit> aSLU = new StepBasic_SiUnitAndLengthUnit;
          aSLU->Init(true, StepBasic_spCenti, StepBasic_sunMetre);
          occ::handle<StepBasic_DerivedUnitElement> aDUE2 = new StepBasic_DerivedUnitElement;
          aDUE2->Init(aSLU, 2.0);
          // other
          occ::handle<NCollection_HArray1<occ::handle<StepBasic_DerivedUnitElement>>> aHADUE =
            new NCollection_HArray1<occ::handle<StepBasic_DerivedUnitElement>>(1, 2);
          aHADUE->SetValue(1, aDUE1);
          aHADUE->SetValue(2, aDUE2);
          occ::handle<StepBasic_DerivedUnit> aDU = new StepBasic_DerivedUnit;
          aDU->Init(aHADUE);
          aModel->AddWithRefs(aDU);
          StepBasic_Unit aUnit;
          aUnit.SetValue(aDU);
          occ::handle<StepBasic_MeasureValueMember> aMVM = new StepBasic_MeasureValueMember;
          aMVM->SetName(aDensValType->ToCString());
          aMVM->SetReal(aDensity);
          occ::handle<StepRepr_MeasureRepresentationItem> aMRI =
            new StepRepr_MeasureRepresentationItem;
          aMRI->Init(aDensName, aMVM, aUnit);
          aHARI = new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, 1);
          aHARI->SetValue(1, aMRI);
          aRepMRI = new StepRepr_Representation();
          aRepMRI->Init(new TCollection_HAsciiString("density"), aHARI, aRC);
          aModel->AddWithRefs(aRepMRI);
        }
        // WriteNewMaterial(Model,aName,aDescription,aDensity,aDensName,aDensValType,RC,RepDRI,RepMRI);
        aMapDRI.Bind(aKey, aRepDRI);
        if (!aRepMRI.IsNull())
          aMapMRI.Bind(aKey, aRepMRI);
      }
    }

    if (!aRepDRI.IsNull())
    {
      StepRepr_CharacterizedDefinition aCD1;
      aCD1.SetValue(aProdDef);
      occ::handle<StepRepr_PropertyDefinition> aPropD1 = new StepRepr_PropertyDefinition;
      aPropD1->Init(new TCollection_HAsciiString("material property"),
                    true,
                    new TCollection_HAsciiString("material name"),
                    aCD1);
      aModel->AddWithRefs(aPropD1);
      StepRepr_RepresentedDefinition aRD1;
      aRD1.SetValue(aPropD1);
      occ::handle<StepRepr_PropertyDefinitionRepresentation> aPDR1 =
        new StepRepr_PropertyDefinitionRepresentation;
      aPDR1->Init(aRD1, aRepDRI);
      aModel->AddWithRefs(aPDR1);

      if (!aRepMRI.IsNull())
      {
        StepRepr_CharacterizedDefinition aCD2;
        aCD2.SetValue(aProdDef);
        occ::handle<StepRepr_PropertyDefinition> aPropD2 = new StepRepr_PropertyDefinition;
        aPropD2->Init(new TCollection_HAsciiString("material property"),
                      true,
                      new TCollection_HAsciiString("density"),
                      aCD2);
        aModel->AddWithRefs(aPropD2);
        StepRepr_RepresentedDefinition aRD2;
        aRD2.SetValue(aPropD2);
        occ::handle<StepRepr_PropertyDefinitionRepresentation> aPDR2 =
          new StepRepr_PropertyDefinitionRepresentation;
        aPDR2->Init(aRD2, aRepMRI);
        aModel->AddWithRefs(aPDR2);
      }
    }
  }

  return true;
}
