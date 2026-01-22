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

#include <STEPCAFControl_Reader.hxx>

#include <BRep_Builder.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Plane.hxx>
#include <Interface_EntityIterator.hxx>
#include <StepData_StepModel.hxx>
#include <HeaderSection_FileSchema.hxx>
#include <Message_ProgressScope.hxx>
#include <NCollection_DataMap.hxx>
#include <OSD_Path.hxx>
#include <Quantity_ColorRGBA.hxx>
#include <StepBasic_ConversionBasedUnitAndLengthUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndPlaneAngleUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndMassUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndTimeUnit.hxx>
#include <StepBasic_DerivedUnit.hxx>
#include <StepBasic_DerivedUnitElement.hxx>
#include <StepBasic_DocumentFile.hxx>
#include <StepBasic_GeneralProperty.hxx>
#include <StepBasic_GeneralPropertyAssociation.hxx>
#include <StepBasic_MeasureValueMember.hxx>
#include <StepBasic_NamedUnit.hxx>
#include <StepBasic_Product.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepBasic_ProductDefinitionWithAssociatedDocuments.hxx>
#include <StepBasic_SiUnitAndLengthUnit.hxx>
#include <StepBasic_SiUnitAndMassUnit.hxx>
#include <StepBasic_SiUnitAndThermodynamicTemperatureUnit.hxx>
#include <StepBasic_SiUnitAndTimeUnit.hxx>
#include <STEPCAFControl_Controller.hxx>
#include <STEPCAFControl_ExternFile.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <STEPConstruct.hxx>
#include <STEPConstruct_ExternRefs.hxx>
#include <STEPConstruct_Styles.hxx>
#include <STEPConstruct_Tool.hxx>
#include <STEPConstruct_UnitContext.hxx>
#include <STEPConstruct_ValidationProps.hxx>
#include <STEPControl_ActorRead.hxx>
#include <STEPControl_Reader.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_Direction.hxx>
#include <StepData_Factors.hxx>
#include <StepDimTol_AngularityTolerance.hxx>
#include <StepDimTol_CircularRunoutTolerance.hxx>
#include <StepDimTol_CoaxialityTolerance.hxx>
#include <StepDimTol_ConcentricityTolerance.hxx>
#include <StepDimTol_CylindricityTolerance.hxx>
#include <StepDimTol_Datum.hxx>
#include <StepDimTol_DatumFeature.hxx>
#include <StepDimTol_DatumReference.hxx>
#include <StepDimTol_DatumReferenceElement.hxx>
#include <StepDimTol_DatumSystem.hxx>
#include <StepDimTol_FlatnessTolerance.hxx>
#include <StepDimTol_GeometricTolerance.hxx>
#include <StepDimTol_GeometricToleranceWithDatumReference.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthMaxTol.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepDimTol_LineProfileTolerance.hxx>
#include <StepDimTol_ModifiedGeometricTolerance.hxx>
#include <StepDimTol_ParallelismTolerance.hxx>
#include <StepDimTol_PerpendicularityTolerance.hxx>
#include <StepDimTol_PositionTolerance.hxx>
#include <StepDimTol_ProjectedZoneDefinition.hxx>
#include <StepDimTol_RoundnessTolerance.hxx>
#include <StepDimTol_RunoutZoneDefinition.hxx>
#include <StepDimTol_StraightnessTolerance.hxx>
#include <StepDimTol_SurfaceProfileTolerance.hxx>
#include <StepDimTol_SymmetryTolerance.hxx>
#include <StepDimTol_ToleranceZone.hxx>
#include <StepDimTol_ToleranceZoneForm.hxx>
#include <StepDimTol_TotalRunoutTolerance.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthMod.hxx>
#include <StepDimTol_GeometricToleranceWithMaximumTolerance.hxx>
#include <StepGeom_Plane.hxx>
#include <StepDimTol_PlacedDatumTargetFeature.hxx>
#include <StepRepr_DerivedShapeAspect.hxx>
#include <StepRepr_DescriptiveRepresentationItem.hxx>
#include <StepRepr_MappedItem.hxx>
#include <StepRepr_MeasureRepresentationItem.hxx>
#include <StepRepr_NextAssemblyUsageOccurrence.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_PropertyDefinition.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_RepresentationMap.hxx>
#include <StepRepr_ReprItemAndLengthMeasureWithUnit.hxx>
#include <StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI.hxx>
#include <StepRepr_ReprItemAndPlaneAngleMeasureWithUnit.hxx>
#include <StepRepr_ReprItemAndPlaneAngleMeasureWithUnitAndQRI.hxx>
#include <NCollection_Sequence.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepRepr_ShapeAspectDerivingRelationship.hxx>
#include <StepRepr_AllAroundShapeAspect.hxx>
#include <StepRepr_ShapeAspectRelationship.hxx>
#include <StepRepr_ShapeRepresentationRelationship.hxx>
#include <StepRepr_SpecifiedHigherUsageOccurrence.hxx>
#include <StepRepr_ValueRange.hxx>
#include <StepRepr_FeatureForDatumTargetRelationship.hxx>
#include <StepShape_AdvancedFace.hxx>
#include <StepShape_AdvancedBrepShapeRepresentation.hxx>
#include <StepShape_AngularSize.hxx>
#include <StepShape_AngularLocation.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_ConnectedFaceSet.hxx>
#include <StepRepr_CompShAspAndDatumFeatAndShAsp.hxx>
#include <StepShape_DimensionalCharacteristicRepresentation.hxx>
#include <StepShape_DimensionalSizeWithPath.hxx>
#include <StepShape_DimensionalLocationWithPath.hxx>
#include <StepShape_ShapeRepresentationWithParameters.hxx>
#include <StepShape_DimensionalSize.hxx>
#include <StepShape_DimensionalLocation.hxx>
#include <StepShape_EdgeCurve.hxx>
#include <StepShape_EdgeLoop.hxx>
#include <StepShape_GeometricCurveSet.hxx>
#include <StepShape_GeometricSet.hxx>
#include <StepShape_Face.hxx>
#include <StepShape_FaceBound.hxx>
#include <StepShape_OrientedEdge.hxx>
#include <StepShape_Loop.hxx>
#include <StepShape_ManifoldSolidBrep.hxx>
#include <StepShape_PlusMinusTolerance.hxx>
#include <StepShape_QualifiedRepresentationItem.hxx>
#include <StepShape_OpenShell.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <StepShape_ShapeDimensionRepresentation.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <StepShape_ShellBasedSurfaceModel.hxx>
#include <StepShape_SolidModel.hxx>
#include <StepShape_ToleranceMethodDefinition.hxx>
#include <StepShape_ToleranceValue.hxx>
#include <StepShape_ValueFormatTypeQualifier.hxx>
#include <StepShape_Vertex.hxx>
#include <StepToGeom.hxx>
#include <StepVisual_AnnotationPlane.hxx>
#include <StepVisual_CameraModelD3MultiClipping.hxx>
#include <StepVisual_CameraModelD3MultiClippingIntersection.hxx>
#include <StepVisual_CameraModelD3MultiClippingUnion.hxx>
#include <StepVisual_CameraModelD3MultiClippingInterectionSelect.hxx>
#include <StepVisual_CameraModelD3MultiClippingUnionSelect.hxx>
#include <StepVisual_DraughtingCallout.hxx>
#include <StepVisual_DraughtingCalloutElement.hxx>
#include <StepVisual_DraughtingModel.hxx>
#include <StepVisual_Invisibility.hxx>
#include <StepVisual_PlanarBox.hxx>
#include <StepVisual_PresentationLayerAssignment.hxx>
#include <StepVisual_PresentationStyleByContext.hxx>
#include <StepVisual_RepositionedTessellatedGeometricSet.hxx>
#include <StepVisual_StyleContextSelect.hxx>
#include <StepVisual_StyledItem.hxx>
#include <StepVisual_ViewVolume.hxx>
#include <StepShape_TypeQualifier.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_HSequence.hxx>
#include <NCollection_Map.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Document.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>
#include <UnitsMethods.hxx>
#include <XCAFDoc_Area.hxx>
#include <XCAFDoc_Centroid.hxx>
#include <XCAFDoc_ClippingPlaneTool.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_DimTolTool.hxx>
#include <XCAFDoc_Dimension.hxx>
#include <XCAFDoc_Datum.hxx>
#include <XCAFDoc_GeomTolerance.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <XCAFDoc_MaterialTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_View.hxx>
#include <XCAFDoc_ViewTool.hxx>
#include <XCAFDoc_Volume.hxx>
#include <XCAFDimTolObjects_DimensionObject.hxx>
#include <XCAFDimTolObjects_GeomToleranceObject.hxx>
#include <XCAFDimTolObjects_DatumObject.hxx>
#include <XCAFView_Object.hxx>
#include <XSAlgo.hxx>
#include <XSAlgo_ShapeProcessor.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>
#include <StepAP242_DraughtingModelItemAssociation.hxx>
#include <StepAP242_GeometricItemSpecificUsage.hxx>
#include <StepAP242_IdAttribute.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <STEPCAFControl_GDTProperty.hxx>
#include <StepVisual_TessellatedAnnotationOccurrence.hxx>
#include <StepVisual_TessellatedGeometricSet.hxx>
#include <StepVisual_TessellatedCurveSet.hxx>
#include <StepVisual_ComplexTriangulatedSurfaceSet.hxx>
#include <StepVisual_CoordinatesList.hxx>
#include <NCollection_Vector.hxx>
#include <StepVisual_OverRidingStyledItem.hxx>
#include <StepVisual_ContextDependentOverRidingStyledItem.hxx>
#include <StepRepr_ShapeRepresentationRelationshipWithTransformation.hxx>
#include <StepRepr_BooleanRepresentationItem.hxx>
#include <StepRepr_IntegerRepresentationItem.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepRepr_PropertyDefinitionRelationship.hxx>
#include <StepRepr_RealRepresentationItem.hxx>
#include <StepRepr_ValueRepresentationItem.hxx>

#include <gp_XYZ.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>

namespace
{
// Returns a MeasureWithUnit from the given Standard_Transient object.
// If the object is a StepRepr_ReprItemAndMeasureWithUnit, it retrieves
// the MeasureWithUnit from it. If it is a StepBasic_MeasureWithUnit,
// it returns it directly. If the object is neither, it returns a null handle.
// @param theMeasure The Standard_Transient object to check.
// @return Handle to StepBasic_MeasureWithUnit if found, otherwise null handle.
static occ::handle<StepBasic_MeasureWithUnit> GetMeasureWithUnit(
  const occ::handle<Standard_Transient>& theMeasure)
{
  if (theMeasure.IsNull())
  {
    return nullptr;
  }

  occ::handle<StepBasic_MeasureWithUnit> aMeasureWithUnit;
  if (theMeasure->IsKind(STANDARD_TYPE(StepBasic_MeasureWithUnit)))
  {
    aMeasureWithUnit = occ::down_cast<StepBasic_MeasureWithUnit>(theMeasure);
  }
  else if (theMeasure->IsKind(STANDARD_TYPE(StepRepr_ReprItemAndMeasureWithUnit)))
  {
    occ::handle<StepRepr_ReprItemAndMeasureWithUnit> aReprMeasureItem =
      occ::down_cast<StepRepr_ReprItemAndMeasureWithUnit>(theMeasure);
    aMeasureWithUnit = aReprMeasureItem->GetMeasureWithUnit();
  }
  return aMeasureWithUnit;
}
} // namespace

//=================================================================================================

STEPCAFControl_Reader::STEPCAFControl_Reader()
    : myColorMode(true),
      myNameMode(true),
      myLayerMode(true),
      myPropsMode(true),
      myMetaMode(true),
      myProductMetaMode(false),
      mySHUOMode(false),
      myGDTMode(true),
      myMatMode(true),
      myViewMode(true)
{
  STEPCAFControl_Controller::Init();
  if (!myReader.WS().IsNull())
  {
    Init(myReader.WS());
  }
}

//=================================================================================================

STEPCAFControl_Reader::STEPCAFControl_Reader(const occ::handle<XSControl_WorkSession>& WS,
                                             const bool                                scratch)
    : myColorMode(true),
      myNameMode(true),
      myLayerMode(true),
      myPropsMode(true),
      myMetaMode(true),
      myProductMetaMode(false),
      mySHUOMode(false),
      myGDTMode(true),
      myMatMode(true),
      myViewMode(true)
{
  STEPCAFControl_Controller::Init();
  Init(WS, scratch);
}

//=================================================================================================

STEPCAFControl_Reader::~STEPCAFControl_Reader() = default;

//=================================================================================================

void STEPCAFControl_Reader::Init(const occ::handle<XSControl_WorkSession>& WS, const bool scratch)
{
  // necessary only in Writer, to set good actor:  WS->SelectNorm ( "STEP" );
  myReader.SetWS(WS, scratch);
  myFiles.Clear();
  myMap.Clear();
}

//=================================================================================================

TCollection_ExtendedString STEPCAFControl_Reader::convertName(
  const TCollection_AsciiString& theName) const
{
  // If source code page is not a NoConversion
  // the string is treated as having UTF-8 coding,
  // else each character is copied to ExtCharacter.
  return TCollection_ExtendedString(theName,
                                    myReader.StepModel()->SourceCodePage()
                                      != Resource_FormatType_NoConversion);
}

//=================================================================================================

IFSelect_ReturnStatus STEPCAFControl_Reader::ReadFile(const char* const theFileName)
{
  return myReader.ReadFile(theFileName);
}

//=================================================================================================

IFSelect_ReturnStatus STEPCAFControl_Reader::ReadFile(const char* const        theFileName,
                                                      const DESTEP_Parameters& theParams)
{
  return myReader.ReadFile(theFileName, theParams);
}

//=================================================================================================

IFSelect_ReturnStatus STEPCAFControl_Reader::ReadStream(const char* const theName,
                                                        std::istream&     theIStream)
{
  return myReader.ReadStream(theName, theIStream);
}

//=================================================================================================

int STEPCAFControl_Reader::NbRootsForTransfer()
{
  return myReader.NbRootsForTransfer();
}

//=================================================================================================

bool STEPCAFControl_Reader::TransferOneRoot(const int                            num,
                                            const occ::handle<TDocStd_Document>& doc,
                                            const Message_ProgressRange&         theProgress)
{
  NCollection_Sequence<TDF_Label> Lseq;
  return Transfer(myReader, num, doc, Lseq, false, theProgress);
}

//=================================================================================================

bool STEPCAFControl_Reader::Transfer(const occ::handle<TDocStd_Document>& doc,
                                     const Message_ProgressRange&         theProgress)
{
  NCollection_Sequence<TDF_Label> Lseq;
  return Transfer(myReader, 0, doc, Lseq, false, theProgress);
}

//=================================================================================================

bool STEPCAFControl_Reader::Perform(const char* const                    filename,
                                    const occ::handle<TDocStd_Document>& doc,
                                    const Message_ProgressRange&         theProgress)
{
  if (ReadFile(filename) != IFSelect_RetDone)
  {
    return false;
  }
  return Transfer(doc, theProgress);
}

//=================================================================================================

bool STEPCAFControl_Reader::Perform(const char* const                    filename,
                                    const occ::handle<TDocStd_Document>& doc,
                                    const DESTEP_Parameters&             theParams,
                                    const Message_ProgressRange&         theProgress)
{
  if (ReadFile(filename, theParams) != IFSelect_RetDone)
  {
    return false;
  }
  return Transfer(doc, theProgress);
}

//=================================================================================================

bool STEPCAFControl_Reader::Perform(const TCollection_AsciiString&       filename,
                                    const occ::handle<TDocStd_Document>& doc,
                                    const Message_ProgressRange&         theProgress)
{
  if (ReadFile(filename.ToCString()) != IFSelect_RetDone)
  {
    return false;
  }
  return Transfer(doc, theProgress);
}

//=================================================================================================

bool STEPCAFControl_Reader::Perform(const TCollection_AsciiString&       filename,
                                    const occ::handle<TDocStd_Document>& doc,
                                    const DESTEP_Parameters&             theParams,
                                    const Message_ProgressRange&         theProgress)
{
  if (ReadFile(filename.ToCString(), theParams) != IFSelect_RetDone)
  {
    return false;
  }
  return Transfer(doc, theProgress);
}

//=================================================================================================

const NCollection_DataMap<TCollection_AsciiString, occ::handle<STEPCAFControl_ExternFile>>&
  STEPCAFControl_Reader::ExternFiles() const
{
  return myFiles;
}

//=================================================================================================

bool STEPCAFControl_Reader::ExternFile(const char* const                       name,
                                       occ::handle<STEPCAFControl_ExternFile>& ef) const
{
  ef.Nullify();
  if (myFiles.IsEmpty() || !myFiles.IsBound(name))
    return false;
  ef = myFiles.Find(name);
  return true;
}

//=================================================================================================

STEPControl_Reader& STEPCAFControl_Reader::ChangeReader()
{
  return myReader;
}

//=================================================================================================

const STEPControl_Reader& STEPCAFControl_Reader::Reader() const
{
  return myReader;
}

//=======================================================================
// function : FillShapesMap
// purpose  : auxiliary: fill a map by all compounds and their components
//=======================================================================

static void FillShapesMap(const TopoDS_Shape&                                     S,
                          NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& map)
{
  TopoDS_Shape    S0 = S;
  TopLoc_Location loc;
  S0.Location(loc);
  map.Add(S0);
  if (S.ShapeType() != TopAbs_COMPOUND)
    return;
  for (TopoDS_Iterator it(S); it.More(); it.Next())
    FillShapesMap(it.Value(), map);
}

//=================================================================================================

void STEPCAFControl_Reader::prepareUnits(const occ::handle<StepData_StepModel>& theModel,
                                         const occ::handle<TDocStd_Document>&   theDoc,
                                         StepData_Factors& theLocalFactors) const
{
  double aScaleFactorMM = 1.;
  if (!XCAFDoc_DocumentTool::GetLengthUnit(theDoc,
                                           aScaleFactorMM,
                                           UnitsMethods_LengthUnit_Millimeter))
  {
    XSAlgo_ShapeProcessor::PrepareForTransfer(); // update unit info
    aScaleFactorMM = UnitsMethods::GetCasCadeLengthUnit();
    // Sets length unit to the document
    XCAFDoc_DocumentTool::SetLengthUnit(theDoc, aScaleFactorMM, UnitsMethods_LengthUnit_Millimeter);
  }
  theLocalFactors.SetCascadeUnit(aScaleFactorMM);
  theModel->SetLocalLengthUnit(aScaleFactorMM);
}

//=================================================================================================

bool STEPCAFControl_Reader::Transfer(STEPControl_Reader&                  reader,
                                     const int                            nroot,
                                     const occ::handle<TDocStd_Document>& doc,
                                     NCollection_Sequence<TDF_Label>&     Lseq,
                                     const bool                           asOne,
                                     const Message_ProgressRange&         theProgress)
{
  reader.ClearShapes();
  occ::handle<StepData_StepModel> aModel = occ::down_cast<StepData_StepModel>(reader.Model());
  StepData_Factors                aLocalFactors;
  prepareUnits(aModel, doc, aLocalFactors);
  int i;

  // Read all shapes
  int num = reader.NbRootsForTransfer();
  if (num <= 0)
    return false;

  Message_ProgressScope aPSRoot(theProgress, nullptr, 2);

  if (nroot)
  {
    if (nroot > num)
      return false;
    reader.TransferOneRoot(nroot, aPSRoot.Next());
  }
  else
  {
    Message_ProgressScope aPS(aPSRoot.Next(), nullptr, num);
    for (i = 1; i <= num && aPS.More(); i++)
      reader.TransferOneRoot(i, aPS.Next());
  }
  if (aPSRoot.UserBreak())
    return false;

  num = reader.NbShapes();
  if (num <= 0)
    return false;

  // Fill a map of (top-level) shapes resulting from that transfer
  // Only these shapes will be considered further
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> ShapesMap, NewShapesMap;
  for (i = 1; i <= num; i++)
    FillShapesMap(reader.Shape(i), ShapesMap);

  // Collect information on shapes originating from SDRs
  // this will be used to distinguish compounds representing assemblies
  // from the ones representing hybrid models and shape sets
  NCollection_DataMap<TopoDS_Shape,
                      occ::handle<StepBasic_ProductDefinition>,
                      TopTools_ShapeMapHasher>
    ShapePDMap;
  NCollection_DataMap<occ::handle<StepBasic_ProductDefinition>,
                      occ::handle<STEPCAFControl_ExternFile>>
                                                PDFileMap;
  const occ::handle<Transfer_TransientProcess>& TP =
    reader.WS()->TransferReader()->TransientProcess();
  int nb = aModel->NbEntities();

  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> SeqPDS =
    new NCollection_HSequence<occ::handle<Standard_Transient>>;

  for (i = 1; i <= nb; i++)
  {
    occ::handle<Standard_Transient> enti = aModel->Value(i);
    if (enti->IsKind(STANDARD_TYPE(StepRepr_ProductDefinitionShape)))
    {
      // sequence for acceleration ReadMaterials
      SeqPDS->Append(enti);
    }
    if (enti->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition)))
    {
      occ::handle<StepBasic_ProductDefinition> PD =
        occ::down_cast<StepBasic_ProductDefinition>(enti);
      int index = TP->MapIndex(PD);
      if (index > 0)
      {
        occ::handle<Transfer_Binder> binder = TP->MapItem(index);
        TopoDS_Shape                 S      = TransferBRep::ShapeResult(binder);
        if (!S.IsNull() && ShapesMap.Contains(S))
        {
          NewShapesMap.Add(S);
          ShapePDMap.Bind(S, PD);
          occ::handle<STEPCAFControl_ExternFile> EF;
          PDFileMap.Bind(PD, EF);
        }
      }
    }
    if (enti->IsKind(STANDARD_TYPE(StepShape_ShapeRepresentation)))
    {
      int index = TP->MapIndex(enti);
      if (index > 0)
      {
        occ::handle<Transfer_Binder> binder = TP->MapItem(index);
        TopoDS_Shape                 S      = TransferBRep::ShapeResult(binder);
        if (!S.IsNull() && ShapesMap.Contains(S))
          NewShapesMap.Add(S);
      }
    }
  }

  // get file name and directory name of the main file
  OSD_Path                mainfile(reader.WS()->LoadedFile());
  TCollection_AsciiString aMainName;
  aMainName = mainfile.Name() + mainfile.Extension();
  mainfile.SetName("");
  mainfile.SetExtension("");
  TCollection_AsciiString dpath;
  mainfile.SystemName(dpath);

  // Load external references (only for relevant SDRs)
  // and fill map SDR -> extern file
  STEPConstruct_ExternRefs ExtRefs(reader.WS());
  ExtRefs.LoadExternRefs();
  Message_ProgressScope aPSE(aPSRoot.Next(), nullptr, ExtRefs.NbExternRefs());
  for (i = 1; i <= ExtRefs.NbExternRefs() && aPSE.More(); i++)
  {
    Message_ProgressRange aRange = aPSE.Next();
    // check extern ref format
    occ::handle<TCollection_HAsciiString> format = ExtRefs.Format(i);
    if (!format.IsNull())
    {
      static occ::handle<TCollection_HAsciiString> ap203 =
        new TCollection_HAsciiString("STEP AP203");
      static occ::handle<TCollection_HAsciiString> ap214 =
        new TCollection_HAsciiString("STEP AP214");
      if (!format->IsSameString(ap203, false) && !format->IsSameString(ap214, false))
      {
#ifdef OCCT_DEBUG
        std::cout << "Warning: STEPCAFControl_Reader::Transfer: Extern document is neither STEP "
                     "AP203 nor AP214"
                  << std::endl;
#else
        continue;
#endif
      }
    }
#ifdef OCCT_DEBUG
    else
      std::cout << "Warning: STEPCAFControl_Reader::Transfer: Extern document format not defined"
                << std::endl;
#endif

    // get and check filename of the current extern ref
    const char* const filename = ExtRefs.FileName(i);

#ifdef OCCT_DEBUG
    std::cout << "filename=" << filename << std::endl;
#endif

    if (!filename || !filename[0])
    {
#ifdef OCCT_DEBUG
      std::cout << "Warning: STEPCAFControl_Reader::Transfer: Extern reference file name is empty"
                << std::endl;
#endif
      continue; // not a valid extern ref
    }

    // compute true path to the extern file
    TCollection_AsciiString fullname = OSD_Path::AbsolutePath(dpath, filename);
    if (fullname.Length() <= 0)
      fullname = filename;

    // check for not the same file
    TCollection_AsciiString aMainFullName = OSD_Path::AbsolutePath(dpath, aMainName);
    if (TCollection_AsciiString::IsSameString(aMainFullName, fullname, false))
    {
      TP->AddWarning(ExtRefs.DocFile(i), "External reference file is the same main file");
      continue; // not a valid extern ref
    }

    /*
        char fullname[1024];
        char *mainfile = reader.WS()->LoadedFile();
        if ( ! mainfile ) mainfile = "";
        int slash = 0;
        for ( int k=0; mainfile[k]; k++ )
          if ( mainfile[k] == '/' ) slash = k;
        strncpy ( fullname, mainfile, slash );
        Sprintf ( &fullname[slash], "%s%s", ( mainfile[0] ? "/" : "" ), filename );
    */

    // get and check PD associated with the current extern ref
    occ::handle<StepBasic_ProductDefinition> PD = ExtRefs.ProdDef(i);
    if (PD.IsNull())
      continue; // not a valid extern ref
    if (!PDFileMap.IsBound(PD))
      continue; // this PD is not concerned by current transfer

    // read extern file (or use existing data) and record its data
    occ::handle<STEPCAFControl_ExternFile> EF =
      ReadExternFile(filename, fullname.ToCString(), doc, aRange);
    PDFileMap.Bind(PD, EF);
  }

  // and insert them to the document
  occ::handle<XCAFDoc_ShapeTool> STool = XCAFDoc_DocumentTool::ShapeTool(doc->Main());
  if (STool.IsNull())
    return false;
  if (asOne)
    Lseq.Append(AddShape(reader.OneShape(), STool, NewShapesMap, ShapePDMap, PDFileMap));
  else
  {
    for (i = 1; i <= num; i++)
    {
      Lseq.Append(AddShape(reader.Shape(i), STool, NewShapesMap, ShapePDMap, PDFileMap));
    }
  }

  // read colors
  if (GetColorMode())
    ReadColors(reader.WS(), doc, aLocalFactors);

  // read names
  if (GetNameMode())
    ReadNames(reader.WS(), doc, PDFileMap);

  // read validation props
  if (GetPropsMode())
    ReadValProps(reader.WS(), doc, PDFileMap, aLocalFactors);

  // read layers
  if (GetLayerMode())
    ReadLayers(reader.WS(), doc);

  // read SHUO entities from STEP model
  if (GetSHUOMode())
    ReadSHUOs(reader.WS(), doc, PDFileMap);

  // read GDT entities from STEP model
  if (GetGDTMode())
    ReadGDTs(reader.WS(), doc, aLocalFactors);

  // read Material entities from STEP model
  if (GetMatMode())
    ReadMaterials(reader.WS(), doc, SeqPDS, aLocalFactors);

  // read View entities from STEP model
  if (GetViewMode())
    ReadViews(reader.WS(), doc, aLocalFactors);

  // read metadata
  if (GetMetaMode())
  {
    ReadMetadata(reader.WS(), doc, aLocalFactors);
  }

  // read product metadata
  if (GetProductMetaMode())
  {
    ReadProductMetadata(reader.WS(), doc);
  }

  // Expand resulting CAF structure for sub-shapes (optionally with their
  // names) if requested
  ExpandSubShapes(STool, ShapePDMap);

  // Update assembly compounds
  STool->UpdateAssemblies();
  return true;
}

//=================================================================================================

TDF_Label STEPCAFControl_Reader::AddShape(
  const TopoDS_Shape&                                                S,
  const occ::handle<XCAFDoc_ShapeTool>&                              STool,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&      NewShapesMap,
  const NCollection_DataMap<TopoDS_Shape,
                            occ::handle<StepBasic_ProductDefinition>,
                            TopTools_ShapeMapHasher>&                ShapePDMap,
  const NCollection_DataMap<occ::handle<StepBasic_ProductDefinition>,
                            occ::handle<STEPCAFControl_ExternFile>>& PDFileMap)
{
  // if shape has already been mapped, just return corresponding label
  if (myMap.IsBound(S))
  {
    return myMap.Find(S);
  }

  // if shape is located, create instance
  if (!S.Location().IsIdentity())
  {
    TopoDS_Shape    S0 = S;
    TopLoc_Location loc;
    S0.Location(loc);
    AddShape(S0, STool, NewShapesMap, ShapePDMap, PDFileMap);
    TDF_Label L = STool->AddShape(S, false); // should create reference
    myMap.Bind(S, L);
    return L;
  }

  // if shape is not compound, simple add it
  if (S.ShapeType() != TopAbs_COMPOUND)
  {
    TDF_Label L = STool->AddShape(S, false);
    myMap.Bind(S, L);
    return L;
  }

  // for compounds, compute number of subshapes and check whether this is assembly
  bool            isAssembly   = false;
  int             nbComponents = 0;
  TopoDS_Iterator it;
  for (it.Initialize(S); it.More() && !isAssembly; it.Next(), nbComponents++)
  {
    TopoDS_Shape    Sub0 = it.Value();
    TopLoc_Location loc;
    Sub0.Location(loc);
    if (NewShapesMap.Contains(Sub0))
      isAssembly = true;
  }

  //  if(nbComponents>0) isAssembly = true;

  // check whether it has associated external ref
  NCollection_Sequence<occ::handle<TCollection_HAsciiString>> SHAS;
  if (ShapePDMap.IsBound(S) && PDFileMap.IsBound(ShapePDMap.Find(S)))
  {
    const occ::handle<STEPCAFControl_ExternFile>& EF = PDFileMap.Find(ShapePDMap.Find(S));
    if (!EF.IsNull())
    {
      // (store information on extern refs in the document)
      SHAS.Append(EF->GetName());
      // if yes, just return corresponding label
      if (!EF->GetLabel().IsNull())
      {
        // but if components >0, ignore extern ref!
        if (nbComponents <= 0)
        {
          myMap.Bind(S, EF->GetLabel());
          STool->SetExternRefs(EF->GetLabel(), SHAS);
          return EF->GetLabel();
        }
      }
#ifdef OCCT_DEBUG
      if (!EF->GetLabel().IsNull())
        std::cout << "Warning: STEPCAFControl_Reader::AddShape: Non-empty shape with external ref; "
                     "ref is ignored"
                  << std::endl;
      else if (nbComponents <= 0)
        std::cout
          << "Warning: STEPCAFControl_Reader::AddShape: Result of reading extern ref is Null"
          << std::endl;
#endif
    }
  }

  // add compound either as a whole,
  if (!isAssembly)
  {
    TDF_Label L = STool->AddShape(S, false);
    if (SHAS.Length() > 0)
      STool->SetExternRefs(L, SHAS);
    myMap.Bind(S, L);
    return L;
  }

  // or as assembly, component-by-component
  TDF_Label L  = STool->NewShape();
  nbComponents = 0;
  for (it.Initialize(S); it.More(); it.Next(), nbComponents++)
  {
    TopoDS_Shape    Sub0 = it.Value();
    TopLoc_Location loc;
    Sub0.Location(loc);
    TDF_Label subL = AddShape(Sub0, STool, NewShapesMap, ShapePDMap, PDFileMap);
    if (!subL.IsNull())
    {
      TDF_Label instL = STool->AddComponent(L, subL, it.Value().Location());
      myMap.TryBound(it.Value(), instL);
    }
  }
  if (SHAS.Length() > 0)
    STool->SetExternRefs(L, SHAS);
  myMap.Bind(S, L);
  // STool->SetShape ( L, S ); // it is necessary for assemblies OCC1747 // commemted by skl for
  // OCC2941

  return L;
}

//=================================================================================================

occ::handle<STEPCAFControl_ExternFile> STEPCAFControl_Reader::ReadExternFile(
  const char* const                    file,
  const char* const                    fullname,
  const occ::handle<TDocStd_Document>& doc,
  const Message_ProgressRange&         theProgress)
{
  // if the file is already read, associate it with SDR
  if (myFiles.IsBound(file))
  {
    return myFiles.ChangeFind(file);
  }

#ifdef OCCT_DEBUG
  std::cout << "Reading extern file: " << fullname << std::endl;
#endif

  // create new WorkSession and Reader
  occ::handle<XSControl_WorkSession> newWS = new XSControl_WorkSession;
  newWS->SelectNorm("STEP");
  STEPControl_Reader sr(newWS, false);

  // start to fill the resulting ExternFile structure
  occ::handle<STEPCAFControl_ExternFile> EF = new STEPCAFControl_ExternFile;
  EF->SetWS(newWS);
  EF->SetName(new TCollection_HAsciiString(file));

  // read file
  EF->SetLoadStatus(sr.ReadFile(fullname));

  // transfer in single-result mode
  if (EF->GetLoadStatus() == IFSelect_RetDone)
  {
    NCollection_Sequence<TDF_Label> labels;
    EF->SetTransferStatus(Transfer(sr, 0, doc, labels, false, theProgress));
    if (labels.Length() > 0)
      EF->SetLabel(labels.Value(1));
  }

  // add read file to dictionary
  myFiles.Bind(file, EF);

  return EF;
}

//=================================================================================================

static void findStyledSR(const occ::handle<StepVisual_StyledItem>&   style,
                         occ::handle<StepShape_ShapeRepresentation>& aSR)
{
  // search Shape Representation for component styled item
  for (int j = 1; j <= style->NbStyles(); j++)
  {
    occ::handle<StepVisual_PresentationStyleByContext> PSA =
      occ::down_cast<StepVisual_PresentationStyleByContext>(style->StylesValue(j));
    if (PSA.IsNull())
      continue;
    StepVisual_StyleContextSelect              aStyleCntxSlct = PSA->StyleContext();
    occ::handle<StepShape_ShapeRepresentation> aCurrentSR =
      occ::down_cast<StepShape_ShapeRepresentation>(aStyleCntxSlct.Representation());
    if (aCurrentSR.IsNull())
      continue;
    aSR = aCurrentSR;
    break;
  }
}

//=======================================================================
// function : propagateColorToParts
// purpose  : auxiliary, propagate color styles from assemblies to parts
//=======================================================================

static void propagateColorToParts(const occ::handle<XCAFDoc_ShapeTool>& theSTool,
                                  const occ::handle<XCAFDoc_ColorTool>& theCTool,
                                  const TDF_Label&                      theRoot)
{
  // collect components to propagate
  NCollection_Sequence<TDF_Label> aComponents;
  if (theRoot.IsEqual(theSTool->Label()))
    theSTool->GetFreeShapes(aComponents);
  else
    theSTool->GetComponents(theRoot, aComponents);

  // iterate each component
  for (NCollection_Sequence<TDF_Label>::Iterator anIt(aComponents); anIt.More(); anIt.Next())
  {
    // get original label
    TDF_Label anOriginalL = anIt.Value();
    theSTool->GetReferredShape(anOriginalL, anOriginalL);

    // propagate to components without own colors
    TDF_Label aColorL, aDummyColorL;
    for (int aType = 1; aType <= 3; aType++)
    {
      if (theCTool->GetColor(theRoot, (XCAFDoc_ColorType)aType, aColorL)
          && !theCTool->GetColor(anOriginalL, (XCAFDoc_ColorType)aType, aDummyColorL))
        theCTool->SetColor(anOriginalL, aColorL, (XCAFDoc_ColorType)aType);
    }
    if (!theCTool->IsVisible(theRoot))
      theCTool->SetVisibility(anOriginalL, false);

    // propagate to next level children
    if (theSTool->IsAssembly(anOriginalL))
      propagateColorToParts(theSTool, theCTool, anOriginalL);
  }
}

//=======================================================================
// function : SetAssemblyComponentStyle
// purpose  : auxiliary: set override style for assembly components
//=======================================================================

static void SetAssemblyComponentStyle(
  const occ::handle<Transfer_TransientProcess>&                       theTP,
  const occ::handle<XCAFDoc_ColorTool>&                               theCTool,
  const STEPConstruct_Styles&                                         theStyles,
  const occ::handle<StepVisual_ContextDependentOverRidingStyledItem>& theStyle,
  const StepData_Factors&                                             theLocalFactors)
{
  if (theStyle.IsNull())
    return;

  occ::handle<StepVisual_Colour>    aSurfCol, aBoundCol, aCurveCol;
  STEPConstruct_RenderingProperties aRenderProps;
  // check if it is component style
  bool anIsComponent = false;
  if (!theStyles.GetColors(theStyle, aSurfCol, aBoundCol, aCurveCol, aRenderProps, anIsComponent))
    return;

  const Interface_Graph& aGraph = theTP->Graph();
  TopLoc_Location        aLoc; // init;
  // find shape
  TopoDS_Shape                               aShape;
  occ::handle<Transfer_Binder>               aBinder;
  occ::handle<StepShape_ShapeRepresentation> aRepr =
    occ::down_cast<StepShape_ShapeRepresentation>(theStyle->ItemAP242().Value());
  if (aRepr.IsNull())
    return;
  occ::handle<StepRepr_ShapeRepresentationRelationship> aSRR;
  Interface_EntityIterator                              aSubs = theTP->Graph().Sharings(aRepr);
  for (aSubs.Start(); aSubs.More(); aSubs.Next())
  {
    const occ::handle<Standard_Transient>& aSubsVal = aSubs.Value();
    if (aSubsVal->IsKind(STANDARD_TYPE(StepRepr_ShapeRepresentationRelationship)))
    {
      // NB: C cast is used instead of DownCast() to improve performance on some cases.
      // This saves ~10% of elapsed time on "testgrid perf de bug29* -parallel 0".
      aSRR = (StepRepr_ShapeRepresentationRelationship*)(aSubsVal.get());
    }
  }

  aBinder = theTP->Find(aSRR);
  if (!aBinder.IsNull())
  {
    aShape = TransferBRep::ShapeResult(aBinder);
  }
  if (aShape.IsNull())
    return;

  // get transformation
  aSubs = aGraph.Shareds(theStyle);
  aSubs.Start();
  for (; aSubs.More(); aSubs.Next())
  {
    occ::handle<StepRepr_ShapeRepresentationRelationshipWithTransformation> aRelation =
      occ::down_cast<StepRepr_ShapeRepresentationRelationshipWithTransformation>(aSubs.Value());
    if (aRelation.IsNull())
      continue;

    auto aTransf = aRelation->TransformationOperator();
    if (auto anItemTransf = aTransf.ItemDefinedTransformation())
    {
      occ::handle<StepGeom_Axis2Placement3d> anAxp1 =
        occ::down_cast<StepGeom_Axis2Placement3d>(anItemTransf->TransformItem1());
      occ::handle<StepGeom_Axis2Placement3d> anAxp2 =
        occ::down_cast<StepGeom_Axis2Placement3d>(anItemTransf->TransformItem2());

      if (!anAxp1.IsNull() && !anAxp2.IsNull())
      {
        occ::handle<Geom_Axis2Placement> anOrig =
          StepToGeom::MakeAxis2Placement(anAxp1, theLocalFactors);
        occ::handle<Geom_Axis2Placement> aTarg =
          StepToGeom::MakeAxis2Placement(anAxp2, theLocalFactors);
        gp_Ax3 anAx3Orig(anOrig->Ax2());
        gp_Ax3 anAx3Targ(aTarg->Ax2());

        gp_Trsf aTr1;
        aTr1.SetTransformation(anAx3Targ, anAx3Orig);
        TopLoc_Location aLoc1(aTr1);
        aLoc = aLoc.Multiplied(aLoc1);
      }
    }
  }

  aShape.Location(aLoc, false);

  if (!aSurfCol.IsNull() || !aBoundCol.IsNull() || !aCurveCol.IsNull() || aRenderProps.IsDefined())
  {
    Quantity_Color     aSCol, aBCol, aCCol, aRCol;
    Quantity_ColorRGBA aFullSCol;
    if (!aSurfCol.IsNull())
    {
      STEPConstruct_Styles::DecodeColor(aSurfCol, aSCol);
      aFullSCol = Quantity_ColorRGBA(aSCol);
    }
    if (!aBoundCol.IsNull())
      STEPConstruct_Styles::DecodeColor(aBoundCol, aBCol);
    if (!aCurveCol.IsNull())
      STEPConstruct_Styles::DecodeColor(aCurveCol, aCCol);
    if (aRenderProps.IsDefined())
    {
      aFullSCol = aRenderProps.GetRGBAColor();
    }

    if (!aSurfCol.IsNull() || aRenderProps.IsDefined())
      theCTool->SetInstanceColor(aShape, XCAFDoc_ColorSurf, aFullSCol);
    if (!aBoundCol.IsNull())
      theCTool->SetInstanceColor(aShape, XCAFDoc_ColorCurv, aBCol);
    if (!aCurveCol.IsNull())
      theCTool->SetInstanceColor(aShape, XCAFDoc_ColorCurv, aCCol);
  }
}

//=======================================================================
// function : SetStyle
// purpose  : auxiliary: set style for parts and instances
//=======================================================================

static void SetStyle(
  const occ::handle<XSControl_WorkSession>&                                    theWS,
  const NCollection_DataMap<TopoDS_Shape, TDF_Label, TopTools_ShapeMapHasher>& theMap,
  const occ::handle<XCAFDoc_ColorTool>&                                        theCTool,
  const occ::handle<XCAFDoc_ShapeTool>&                                        theSTool,
  const STEPConstruct_Styles&                                                  theStyles,
  const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>&   theHSeqOfInvisStyle,
  const occ::handle<StepVisual_StyledItem>&                                    theStyle,
  const StepData_Factors&                                                      theLocalFactors)
{
  if (theStyle.IsNull())
    return;

  const occ::handle<Transfer_TransientProcess>& aTP = theWS->TransferReader()->TransientProcess();
  if (occ::handle<StepVisual_OverRidingStyledItem> anOverridingStyle =
        occ::down_cast<StepVisual_OverRidingStyledItem>(theStyle))
  {
    SetStyle(theWS,
             theMap,
             theCTool,
             theSTool,
             theStyles,
             theHSeqOfInvisStyle,
             anOverridingStyle->OverRiddenStyle(),
             theLocalFactors);
    if (occ::handle<StepVisual_ContextDependentOverRidingStyledItem> anAssemblyComponentStyle =
          occ::down_cast<StepVisual_ContextDependentOverRidingStyledItem>(theStyle))
    {
      SetAssemblyComponentStyle(aTP,
                                theCTool,
                                theStyles,
                                anAssemblyComponentStyle,
                                theLocalFactors);
      return;
    }
  }

  bool anIsVisible = true;
  // check the visibility of styled item.
  for (int si = 1; si <= theHSeqOfInvisStyle->Length(); si++)
  {
    if (theStyle != theHSeqOfInvisStyle->Value(si))
      continue;
    // found that current style is invisible.
    anIsVisible = false;
    break;
  }

  occ::handle<StepVisual_Colour> aSurfCol, aBoundCol, aCurveCol;
  // check if it is component style
  bool                              anIsComponent = false;
  STEPConstruct_RenderingProperties aRenderProps;
  if (!theStyles.GetColors(theStyle, aSurfCol, aBoundCol, aCurveCol, aRenderProps, anIsComponent)
      && anIsVisible)
    return;

  // collect styled items
  NCollection_Vector<StepVisual_StyledItemTarget> anItems;
  if (!theStyle->ItemAP242().IsNull())
  {
    anItems.Append(theStyle->ItemAP242());
  }

  for (int itemIt = 0; itemIt < anItems.Length(); itemIt++)
  {
    int          anIndex = aTP->MapIndex(anItems.Value(itemIt).Value());
    TopoDS_Shape aS;
    if (anIndex > 0)
    {
      occ::handle<Transfer_Binder> aBinder = aTP->MapItem(anIndex);
      aS                                   = TransferBRep::ShapeResult(aBinder);
    }
    bool isSkipSHUOstyle = false;
    // take shape with real location.
    while (anIsComponent)
    {
      // take SR of NAUO
      occ::handle<StepShape_ShapeRepresentation> aSR;
      findStyledSR(theStyle, aSR);
      // search for SR along model
      if (aSR.IsNull())
        break;
      Interface_EntityIterator aSubs = theWS->HGraph()->Graph().Sharings(aSR);
      occ::handle<StepShape_ShapeDefinitionRepresentation> aSDR;
      for (aSubs.Start(); aSubs.More(); aSubs.Next())
      {
        aSDR = occ::down_cast<StepShape_ShapeDefinitionRepresentation>(aSubs.Value());
        if (aSDR.IsNull())
          continue;
        StepRepr_RepresentedDefinition               aPDSselect = aSDR->Definition();
        occ::handle<StepRepr_ProductDefinitionShape> PDS =
          occ::down_cast<StepRepr_ProductDefinitionShape>(aPDSselect.PropertyDefinition());
        if (PDS.IsNull())
          continue;
        StepRepr_CharacterizedDefinition aCharDef = PDS->Definition();

        occ::handle<StepRepr_AssemblyComponentUsage> ACU =
          occ::down_cast<StepRepr_AssemblyComponentUsage>(aCharDef.ProductDefinitionRelationship());
        if (ACU.IsNull())
          continue;
        // PTV 10.02.2003 skip styled item that refer to SHUO
        if (ACU->IsKind(STANDARD_TYPE(StepRepr_SpecifiedHigherUsageOccurrence)))
        {
          isSkipSHUOstyle = true;
          break;
        }
        occ::handle<StepRepr_NextAssemblyUsageOccurrence> NAUO =
          occ::down_cast<StepRepr_NextAssemblyUsageOccurrence>(ACU);
        if (NAUO.IsNull())
          continue;

        TopoDS_Shape aSh;
        // PTV 10.02.2003 to find component of assembly CORRECTLY
        STEPConstruct_Tool aTool(theWS);
        TDF_Label          aShLab =
          STEPCAFControl_Reader::FindInstance(NAUO, theCTool->ShapeTool(), aTool, theMap);
        aSh = theCTool->ShapeTool()->GetShape(aShLab);
        if (!aSh.IsNull())
        {
          aS = aSh;
          break;
        }
      }
      break;
    }
    if (isSkipSHUOstyle)
      continue; // skip styled item which refer to SHUO

    if (aS.IsNull())
      continue;

    if (!aSurfCol.IsNull() || !aBoundCol.IsNull() || !aCurveCol.IsNull() || aRenderProps.IsDefined()
        || !anIsVisible)
    {
      TDF_Label aL;
      bool      isFound = theSTool->SearchUsingMap(aS, aL, false, true);
      if (!aSurfCol.IsNull() || !aBoundCol.IsNull() || !aCurveCol.IsNull()
          || aRenderProps.IsDefined())
      {
        Quantity_Color     aSCol, aBCol, aCCol, aRCol;
        Quantity_ColorRGBA aFullSCol;
        if (!aSurfCol.IsNull())
        {
          STEPConstruct_Styles::DecodeColor(aSurfCol, aSCol);
          aFullSCol = Quantity_ColorRGBA(aSCol);
        }
        if (!aBoundCol.IsNull())
          STEPConstruct_Styles::DecodeColor(aBoundCol, aBCol);
        if (!aCurveCol.IsNull())
          STEPConstruct_Styles::DecodeColor(aCurveCol, aCCol);
        if (aRenderProps.IsDefined())
        {
          aFullSCol = aRenderProps.GetRGBAColor();
        }
        if (isFound)
        {
          if (!aSurfCol.IsNull() || aRenderProps.IsDefined())
            theCTool->SetColor(aL, aFullSCol, XCAFDoc_ColorSurf);
          if (!aBoundCol.IsNull())
            theCTool->SetColor(aL, aBCol, XCAFDoc_ColorCurv);
          if (!aCurveCol.IsNull())
            theCTool->SetColor(aL, aCCol, XCAFDoc_ColorCurv);
        }
        else
        {
          for (TopoDS_Iterator it(aS); it.More(); it.Next())
          {
            TDF_Label aL1;
            if (theSTool->SearchUsingMap(it.Value(), aL1, false, true))
            {
              if (!aSurfCol.IsNull() || aRenderProps.IsDefined())
                theCTool->SetColor(aL1, aFullSCol, XCAFDoc_ColorSurf);
              if (!aBoundCol.IsNull())
                theCTool->SetColor(aL1, aBCol, XCAFDoc_ColorCurv);
              if (!aCurveCol.IsNull())
                theCTool->SetColor(aL1, aCCol, XCAFDoc_ColorCurv);
            }
          }
        }
      }
      if (!anIsVisible)
      {
        // sets the invisibility for shape.
        if (isFound)
          theCTool->SetVisibility(aL, false);
      }
    }
  }
}

//=======================================================================
// function : IsOverriden
// purpose  : auxiliary: check that style is overridden
//=======================================================================

static bool IsOverriden(const Interface_Graph&                    theGraph,
                        const occ::handle<StepVisual_StyledItem>& theStyle,
                        bool                                      theIsRoot)
{
  Interface_EntityIterator aSubs = theGraph.Sharings(theStyle);
  aSubs.Start();
  for (; aSubs.More(); aSubs.Next())
  {
    occ::handle<StepVisual_OverRidingStyledItem> anOverRidingStyle =
      occ::down_cast<StepVisual_OverRidingStyledItem>(aSubs.Value());
    if (!anOverRidingStyle.IsNull())
    {
      if (!theIsRoot)
      {
        return true;
      }
      // for root style returns true only if it is overridden by other root style
      const occ::handle<Standard_Transient> anItem = anOverRidingStyle->ItemAP242().Value();
      if (!anItem.IsNull() && anItem->IsKind(STANDARD_TYPE(StepShape_ShapeRepresentation)))
      {
        return true;
      }
    }
  }
  return false;
}

//=================================================================================================

bool STEPCAFControl_Reader::ReadColors(const occ::handle<XSControl_WorkSession>& WS,
                                       const occ::handle<TDocStd_Document>&      Doc,
                                       const StepData_Factors& theLocalFactors) const
{
  STEPConstruct_Styles Styles(WS);
  if (!Styles.LoadStyles())
  {
#ifdef OCCT_DEBUG
    std::cout << "Warning: no styles are found in the model" << std::endl;
#endif
    return false;
  }
  // searching for invisible items in the model
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> aHSeqOfInvisStyle =
    new NCollection_HSequence<occ::handle<Standard_Transient>>;
  Styles.LoadInvisStyles(aHSeqOfInvisStyle);

  occ::handle<XCAFDoc_ColorTool> CTool = XCAFDoc_DocumentTool::ColorTool(Doc->Main());
  if (CTool.IsNull())
    return false;
  occ::handle<XCAFDoc_ShapeTool> STool = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  if (STool.IsNull())
    return false;

  const Interface_Graph& aGraph = Styles.Graph();

  // parse and search for color attributes
  int nb = Styles.NbRootStyles();
  // apply root styles earlier, as they can be overridden
  // function IsOverriden for root style returns true only if it is overridden by other root style
  bool anIsRootStyle = true;
  for (int i = 1; i <= nb; i++)
  {
    occ::handle<StepVisual_StyledItem> Style = Styles.RootStyle(i);
    // check that style is overridden by other root style
    if (!IsOverriden(aGraph, Style, anIsRootStyle))
    {
      SetStyle(WS, myMap, CTool, STool, Styles, aHSeqOfInvisStyle, Style, theLocalFactors);
    }
  }

  nb = Styles.NbStyles();
  // apply leaf styles, they can override root styles
  anIsRootStyle = false;
  for (int i = 1; i <= nb; i++)
  {
    occ::handle<StepVisual_StyledItem> Style = Styles.Style(i);
    // check that style is overridden
    if (!IsOverriden(aGraph, Style, anIsRootStyle))
    {
      SetStyle(WS, myMap, CTool, STool, Styles, aHSeqOfInvisStyle, Style, theLocalFactors);
    }
  }

  CTool->ReverseChainsOfTreeNodes();

  // some colors can be attached to assemblies, propagate them to components
  propagateColorToParts(STool, CTool, STool->Label());
  return true;
}

//=================================================================================================

static TDF_Label GetLabelFromPD(
  const occ::handle<StepBasic_ProductDefinition>&                              PD,
  const occ::handle<XCAFDoc_ShapeTool>&                                        STool,
  const occ::handle<Transfer_TransientProcess>&                                TP,
  const NCollection_DataMap<occ::handle<StepBasic_ProductDefinition>,
                            occ::handle<STEPCAFControl_ExternFile>>&           PDFileMap,
  const NCollection_DataMap<TopoDS_Shape, TDF_Label, TopTools_ShapeMapHasher>& ShapeLabelMap)
{
  TDF_Label L;
  if (PDFileMap.IsBound(PD))
  {
    const occ::handle<STEPCAFControl_ExternFile>& EF = PDFileMap.Find(PD);
    if (!EF.IsNull())
    {
      L = EF->GetLabel();
      if (!L.IsNull())
        return L;
    }
  }

  TopoDS_Shape                 S;
  occ::handle<Transfer_Binder> binder = TP->Find(PD);
  if (binder.IsNull() || !binder->HasResult())
    return L;
  S = TransferBRep::ShapeResult(TP, binder);
  if (S.IsNull())
    return L;

  if (ShapeLabelMap.IsBound(S))
    L = ShapeLabelMap.Find(S);
  if (L.IsNull())
    STool->Search(S, L, true, true, false);
  return L;
}

//=================================================================================================

TDF_Label STEPCAFControl_Reader::FindInstance(
  const occ::handle<StepRepr_NextAssemblyUsageOccurrence>&                     NAUO,
  const occ::handle<XCAFDoc_ShapeTool>&                                        STool,
  const STEPConstruct_Tool&                                                    Tool,
  const NCollection_DataMap<TopoDS_Shape, TDF_Label, TopTools_ShapeMapHasher>& ShapeLabelMap)
{
  TDF_Label L;

  // get shape resulting from CDSR (in fact, only location is interesting)
  const occ::handle<Transfer_TransientProcess>& TP     = Tool.TransientProcess();
  occ::handle<Transfer_Binder>                  binder = TP->Find(NAUO);
  if (binder.IsNull() || !binder->HasResult())
  {
#ifdef OCCT_DEBUG
    std::cout << "Error: STEPCAFControl_Reader::FindInstance: NAUO is not mapped to shape"
              << std::endl;
#endif
    return L;
  }

  TopoDS_Shape S = TransferBRep::ShapeResult(TP, binder);
  if (S.IsNull())
  {
#ifdef OCCT_DEBUG
    std::cout << "Error: STEPCAFControl_Reader::FindInstance: NAUO is not mapped to shape"
              << std::endl;
#endif
    return L;
  }

  if (ShapeLabelMap.IsBound(S))
    L = ShapeLabelMap(S);
  else
    STool->Search(S, L, true, true, false);

  return L;
}

//=================================================================================================

bool STEPCAFControl_Reader::ReadNames(
  const occ::handle<XSControl_WorkSession>&                          WS,
  const occ::handle<TDocStd_Document>&                               Doc,
  const NCollection_DataMap<occ::handle<StepBasic_ProductDefinition>,
                            occ::handle<STEPCAFControl_ExternFile>>& PDFileMap) const
{
  // get starting data
  const occ::handle<Interface_InterfaceModel>&  Model = WS->Model();
  const occ::handle<XSControl_TransferReader>&  TR    = WS->TransferReader();
  const occ::handle<Transfer_TransientProcess>& TP    = TR->TransientProcess();
  occ::handle<XCAFDoc_ShapeTool> STool = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  if (STool.IsNull())
    return false;
  STEPConstruct_Tool Tool(WS);

  // iterate on model to find all SDRs and CDSRs
  int                        nb    = Model->NbEntities();
  occ::handle<Standard_Type> tNAUO = STANDARD_TYPE(StepRepr_NextAssemblyUsageOccurrence);
  occ::handle<Standard_Type> tPD   = STANDARD_TYPE(StepBasic_ProductDefinition);
  occ::handle<Standard_Type> tPDWAD =
    STANDARD_TYPE(StepBasic_ProductDefinitionWithAssociatedDocuments);
  occ::handle<TCollection_HAsciiString> name;
  TDF_Label                             L;
  for (int i = 1; i <= nb; i++)
  {
    occ::handle<Standard_Transient> enti = Model->Value(i);

    // get description of NAUO
    if (enti->DynamicType() == tNAUO)
    {
      L.Nullify();
      occ::handle<StepRepr_NextAssemblyUsageOccurrence> NAUO =
        occ::down_cast<StepRepr_NextAssemblyUsageOccurrence>(enti);
      if (NAUO.IsNull())
        continue;
      Interface_EntityIterator subs = WS->Graph().Sharings(NAUO);
      for (subs.Start(); subs.More(); subs.Next())
      {
        occ::handle<StepRepr_ProductDefinitionShape> PDS =
          occ::down_cast<StepRepr_ProductDefinitionShape>(subs.Value());
        if (PDS.IsNull())
          continue;
        occ::handle<StepBasic_ProductDefinitionRelationship> PDR =
          PDS->Definition().ProductDefinitionRelationship();
        if (PDR.IsNull())
          continue;
        if (PDR->HasDescription() && PDR->Description()->UsefullLength() > 0)
          name = PDR->Description();
        else if (!PDR->Name().IsNull() && PDR->Name()->UsefullLength() > 0)
          name = PDR->Name();
        else if (!PDR->Id().IsNull())
          name = PDR->Id();
        else
          name = new TCollection_HAsciiString;
      }
      // find proper label
      L = FindInstance(NAUO, STool, Tool, myMap);
      if (L.IsNull())
        continue;

      TCollection_ExtendedString str = convertName(name->String());
      TDataStd_Name::Set(L, str);
    }

    // for PD get name of associated product
    if (enti->DynamicType() == tPD || enti->DynamicType() == tPDWAD)
    {
      L.Nullify();
      occ::handle<StepBasic_ProductDefinition> PD =
        occ::down_cast<StepBasic_ProductDefinition>(enti);
      if (PD.IsNull())
        continue;
      occ::handle<StepBasic_Product> Prod =
        (!PD->Formation().IsNull() ? PD->Formation()->OfProduct() : nullptr);
      if (Prod.IsNull())
        name = new TCollection_HAsciiString;
      else if (!Prod->Name().IsNull() && Prod->Name()->UsefullLength() > 0)
        name = Prod->Name();
      else if (!Prod->Id().IsNull())
        name = Prod->Id();
      else
        name = new TCollection_HAsciiString;
      L = GetLabelFromPD(PD, STool, TP, PDFileMap, myMap);
      if (L.IsNull())
        continue;
      TCollection_ExtendedString str = convertName(name->String());
      TDataStd_Name::Set(L, str);
    }
    // set a name to the document
    // TCollection_ExtendedString str = convertName (name->String());
    // TDataStd_Name::Set ( L, str );
  }

  return true;
}

//=================================================================================================

static TDF_Label GetLabelFromPD(
  const occ::handle<StepBasic_ProductDefinition>&                              PD,
  const occ::handle<XCAFDoc_ShapeTool>&                                        STool,
  const STEPConstruct_ValidationProps&                                         Props,
  const NCollection_DataMap<occ::handle<StepBasic_ProductDefinition>,
                            occ::handle<STEPCAFControl_ExternFile>>&           PDFileMap,
  const NCollection_DataMap<TopoDS_Shape, TDF_Label, TopTools_ShapeMapHasher>& ShapeLabelMap)
{
  TDF_Label L;
  if (PDFileMap.IsBound(PD))
  {
    const occ::handle<STEPCAFControl_ExternFile>& EF = PDFileMap.Find(PD);
    if (!EF.IsNull())
    {
      L = EF->GetLabel();
      if (!L.IsNull())
        return L;
    }
  }
  TopoDS_Shape S = Props.GetPropShape(PD);
  if (S.IsNull())
    return L;
  if (ShapeLabelMap.IsBound(S))
    L = ShapeLabelMap.Find(S);
  if (L.IsNull())
    STool->Search(S, L, true, true, false);
  return L;
}

//=================================================================================================

bool STEPCAFControl_Reader::ReadValProps(
  const occ::handle<XSControl_WorkSession>&                          WS,
  const occ::handle<TDocStd_Document>&                               Doc,
  const NCollection_DataMap<occ::handle<StepBasic_ProductDefinition>,
                            occ::handle<STEPCAFControl_ExternFile>>& PDFileMap,
  const StepData_Factors&                                            theLocalFactors) const
{
  // get starting data
  const occ::handle<XSControl_TransferReader>&  TR = WS->TransferReader();
  const occ::handle<Transfer_TransientProcess>& TP = TR->TransientProcess();
  occ::handle<XCAFDoc_ShapeTool> STool             = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  if (STool.IsNull())
    return false;

  // load props from the STEP model
  NCollection_Sequence<occ::handle<Standard_Transient>> props;
  STEPConstruct_ValidationProps                         Props(WS);
  if (!Props.LoadProps(props))
  {
#ifdef OCCT_DEBUG
    std::cout << "Warning: no validation props found in the model" << std::endl;
#endif
    return false;
  }

  // interpret props one by one
  for (int i = 1; i <= props.Length(); i++)
  {
    occ::handle<StepRepr_PropertyDefinitionRepresentation> PDR =
      occ::down_cast<StepRepr_PropertyDefinitionRepresentation>(props.Value(i));
    if (PDR.IsNull())
      continue;

    TDF_Label L;

    occ::handle<StepRepr_PropertyDefinition> PD   = PDR->Definition().PropertyDefinition();
    Interface_EntityIterator                 subs = Props.Graph().Shareds(PD);
    for (subs.Start(); L.IsNull() && subs.More(); subs.Next())
    {
      if (subs.Value()->IsKind(STANDARD_TYPE(StepRepr_ProductDefinitionShape)))
      {
        occ::handle<StepRepr_ProductDefinitionShape> PDS =
          occ::down_cast<StepRepr_ProductDefinitionShape>(subs.Value());
        if (PDS.IsNull())
          continue;
        // find corresponding NAUO
        occ::handle<StepRepr_NextAssemblyUsageOccurrence> NAUO;
        Interface_EntityIterator                          subs1 = Props.Graph().Shareds(PDS);
        for (subs1.Start(); NAUO.IsNull() && subs1.More(); subs1.Next())
        {
          if (subs1.Value()->IsKind(STANDARD_TYPE(StepRepr_NextAssemblyUsageOccurrence)))
            NAUO = occ::down_cast<StepRepr_NextAssemblyUsageOccurrence>(subs1.Value());
        }
        if (!NAUO.IsNull())
        {
          L = FindInstance(NAUO, STool, WS, myMap);
          if (L.IsNull())
            continue;
        }
        else
        {
          // find corresponding ProductDefinition:
          occ::handle<StepBasic_ProductDefinition> ProdDef;
          Interface_EntityIterator                 subsPDS = Props.Graph().Shareds(PDS);
          for (subsPDS.Start(); ProdDef.IsNull() && subsPDS.More(); subsPDS.Next())
          {
            if (subsPDS.Value()->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition)))
              ProdDef = occ::down_cast<StepBasic_ProductDefinition>(subsPDS.Value());
          }
          if (ProdDef.IsNull())
            continue;
          L = GetLabelFromPD(ProdDef, STool, Props, PDFileMap, myMap);
        }
      }

      if (subs.Value()->IsKind(STANDARD_TYPE(StepRepr_ShapeAspect)))
      {
        occ::handle<StepRepr_ShapeAspect> SA = occ::down_cast<StepRepr_ShapeAspect>(subs.Value());
        if (SA.IsNull())
          continue;
        // find ShapeRepresentation
        occ::handle<StepShape_ShapeRepresentation> SR;
        Interface_EntityIterator                   subs1 = Props.Graph().Sharings(SA);
        for (subs1.Start(); subs1.More() && SR.IsNull(); subs1.Next())
        {
          occ::handle<StepRepr_PropertyDefinition> PropD1 =
            occ::down_cast<StepRepr_PropertyDefinition>(subs1.Value());
          if (PropD1.IsNull())
            continue;
          Interface_EntityIterator subs2 = Props.Graph().Sharings(PropD1);
          for (subs2.Start(); subs2.More() && SR.IsNull(); subs2.Next())
          {
            occ::handle<StepShape_ShapeDefinitionRepresentation> SDR =
              occ::down_cast<StepShape_ShapeDefinitionRepresentation>(subs2.Value());
            if (SDR.IsNull())
              continue;
            SR = occ::down_cast<StepShape_ShapeRepresentation>(SDR->UsedRepresentation());
          }
        }
        if (SR.IsNull())
          continue;
        occ::handle<Transfer_Binder> binder;
        for (int ir = 1; ir <= SR->NbItems() && binder.IsNull(); ir++)
        {
          if (SR->ItemsValue(ir)->IsKind(STANDARD_TYPE(StepShape_SolidModel)))
          {
            occ::handle<StepShape_SolidModel> SM =
              occ::down_cast<StepShape_SolidModel>(SR->ItemsValue(ir));
            binder = TP->Find(SM);
          }
          else if (SR->ItemsValue(ir)->IsKind(STANDARD_TYPE(StepShape_ShellBasedSurfaceModel)))
          {
            occ::handle<StepShape_ShellBasedSurfaceModel> SBSM =
              occ::down_cast<StepShape_ShellBasedSurfaceModel>(SR->ItemsValue(ir));
            binder = TP->Find(SBSM);
          }
          else if (SR->ItemsValue(ir)->IsKind(STANDARD_TYPE(StepShape_GeometricSet)))
          {
            occ::handle<StepShape_GeometricSet> GS =
              occ::down_cast<StepShape_GeometricSet>(SR->ItemsValue(ir));
            binder = TP->Find(GS);
          }
        }
        if (binder.IsNull() || !binder->HasResult())
          continue;
        TopoDS_Shape S;
        S = TransferBRep::ShapeResult(TP, binder);
        if (S.IsNull())
          continue;
        if (myMap.IsBound(S))
          L = myMap.Find(S);
        if (L.IsNull())
          STool->Search(S, L, true, true, true);
      }
    }

    if (L.IsNull())
      continue;

    // decode validation properties
    occ::handle<StepRepr_Representation> rep = PDR->UsedRepresentation();
    for (int j = 1; j <= rep->NbItems(); j++)
    {
      occ::handle<StepRepr_RepresentationItem> ent = rep->ItemsValue(j);
      bool                                     isArea;
      double                                   val;
      gp_Pnt                                   pos;
      if (Props.GetPropReal(ent, val, isArea, theLocalFactors))
      {
        if (isArea)
          XCAFDoc_Area::Set(L, val);
        else
          XCAFDoc_Volume::Set(L, val);
      }
      else if (Props.GetPropPnt(ent, rep->ContextOfItems(), pos, theLocalFactors))
      {
        XCAFDoc_Centroid::Set(L, pos);
      }
    }
  }
  return true;
}

//=================================================================================================

bool STEPCAFControl_Reader::ReadLayers(const occ::handle<XSControl_WorkSession>& WS,
                                       const occ::handle<TDocStd_Document>&      Doc) const
{
  const occ::handle<Interface_InterfaceModel>&  Model = WS->Model();
  const occ::handle<XSControl_TransferReader>&  TR    = WS->TransferReader();
  const occ::handle<Transfer_TransientProcess>& TP    = TR->TransientProcess();
  occ::handle<XCAFDoc_ShapeTool> STool = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  if (STool.IsNull())
    return false;
  occ::handle<XCAFDoc_LayerTool> LTool = XCAFDoc_DocumentTool::LayerTool(Doc->Main());
  if (LTool.IsNull())
    return false;

  occ::handle<Standard_Type> tSVPLA = STANDARD_TYPE(StepVisual_PresentationLayerAssignment);
  int                        nb     = Model->NbEntities();
  occ::handle<TCollection_HAsciiString> name;

  for (int i = 1; i <= nb; i++)
  {
    occ::handle<Standard_Transient> enti = Model->Value(i);
    if (!enti->IsKind(tSVPLA))
      continue;
    occ::handle<StepVisual_PresentationLayerAssignment> SVPLA =
      occ::down_cast<StepVisual_PresentationLayerAssignment>(enti);
    if (SVPLA->AssignedItems().IsNull())
      continue;

    occ::handle<TCollection_HAsciiString> descr = SVPLA->Description();
    occ::handle<TCollection_HAsciiString> hName = SVPLA->Name();
    TCollection_ExtendedString            aLayerName(hName->String());
    TDF_Label                             aLayerLabel;

    // check invisibility
    bool isVisible = true;
    ;
    Interface_EntityIterator subs = WS->Graph().Sharings(SVPLA);
    for (subs.Start(); subs.More() && isVisible; subs.Next())
    {
      if (!subs.Value()->IsKind(STANDARD_TYPE(StepVisual_Invisibility)))
        continue;
#ifdef OCCT_DEBUG
      std::cout << "\tLayer \"" << aLayerName << "\" is invisible" << std::endl;
#endif
      isVisible = false;
    }

    // find a target shape and its label in the document
    for (int j = 1; j <= SVPLA->NbAssignedItems(); j++)
    {
      StepVisual_LayeredItem       LI     = SVPLA->AssignedItemsValue(j);
      occ::handle<Transfer_Binder> binder = TP->Find(LI.Value());
      if (binder.IsNull() || !binder->HasResult())
        continue;

      TopoDS_Shape S = TransferBRep::ShapeResult(TP, binder);
      if (S.IsNull())
        continue;

      TDF_Label shL;
      if (!STool->Search(S, shL, true, true, true))
        continue;
      if (aLayerLabel.IsNull())
        aLayerLabel = LTool->AddLayer(aLayerName, isVisible);
      LTool->SetLayer(shL, aLayerLabel);
    }

    if (!aLayerLabel.IsNull())
      LTool->SetVisibility(aLayerLabel, isVisible);
  }
  return true;
}

//=================================================================================================

static bool findNextSHUOlevel(
  const occ::handle<XSControl_WorkSession>&                                    WS,
  const occ::handle<StepRepr_SpecifiedHigherUsageOccurrence>&                  SHUO,
  const occ::handle<XCAFDoc_ShapeTool>&                                        STool,
  const NCollection_DataMap<occ::handle<StepBasic_ProductDefinition>,
                            occ::handle<STEPCAFControl_ExternFile>>&           PDFileMap,
  const NCollection_DataMap<TopoDS_Shape, TDF_Label, TopTools_ShapeMapHasher>& ShapeLabelMap,
  NCollection_Sequence<TDF_Label>&                                             aLabels)
{
  Interface_EntityIterator                             subs = WS->HGraph()->Graph().Sharings(SHUO);
  occ::handle<StepRepr_SpecifiedHigherUsageOccurrence> subSHUO;
  for (subs.Start(); subs.More(); subs.Next())
  {
    if (subs.Value()->IsKind(STANDARD_TYPE(StepRepr_SpecifiedHigherUsageOccurrence)))
    {
      subSHUO = occ::down_cast<StepRepr_SpecifiedHigherUsageOccurrence>(subs.Value());
      break;
    }
  }
  if (subSHUO.IsNull())
    return false;

  occ::handle<StepRepr_NextAssemblyUsageOccurrence> NUNAUO = subSHUO->NextUsage();
  if (NUNAUO.IsNull())
    return false;
  //   occ::handle<Interface_InterfaceModel> Model = WS->Model();
  //   occ::handle<XSControl_TransferReader> TR = WS->TransferReader();
  //   occ::handle<Transfer_TransientProcess> TP = TR->TransientProcess();
  //   occ::handle<Transfer_Binder> binder = TP->Find(NUNAUO);
  //   if ( binder.IsNull() || ! binder->HasResult() )
  //     return false;
  //   TopoDS_Shape NUSh = TransferBRep::ShapeResult ( TP, binder );
  // get label of NAUO next level
  TDF_Label          NULab;
  STEPConstruct_Tool Tool(WS);
  NULab = STEPCAFControl_Reader::FindInstance(NUNAUO, STool, Tool, ShapeLabelMap);
  //   STool->Search(NUSh, NUlab);
  if (NULab.IsNull())
    return false;
  aLabels.Append(NULab);
  // and check by recurse.
  findNextSHUOlevel(WS, subSHUO, STool, PDFileMap, ShapeLabelMap, aLabels);
  return true;
}

//=================================================================================================

static TDF_Label setSHUOintoDoc(
  const occ::handle<XSControl_WorkSession>&                                    WS,
  const occ::handle<StepRepr_SpecifiedHigherUsageOccurrence>&                  SHUO,
  const occ::handle<XCAFDoc_ShapeTool>&                                        STool,
  const NCollection_DataMap<occ::handle<StepBasic_ProductDefinition>,
                            occ::handle<STEPCAFControl_ExternFile>>&           PDFileMap,
  const NCollection_DataMap<TopoDS_Shape, TDF_Label, TopTools_ShapeMapHasher>& ShapeLabelMap)
{
  TDF_Label aMainLabel;
  // get upper usage NAUO from SHUO.
  occ::handle<StepRepr_NextAssemblyUsageOccurrence> UUNAUO =
    occ::down_cast<StepRepr_NextAssemblyUsageOccurrence>(SHUO->UpperUsage());
  occ::handle<StepRepr_NextAssemblyUsageOccurrence> NUNAUO = SHUO->NextUsage();
  if (UUNAUO.IsNull() || NUNAUO.IsNull())
  {
#ifdef OCCT_DEBUG
    std::cout << "Warning: " << __FILE__
              << ": Upper_usage or Next_usage of styled SHUO is null. Skip it" << std::endl;
#endif
    return aMainLabel;
  }
  //   occ::handle<Interface_InterfaceModel> Model = WS->Model();
  //   occ::handle<XSControl_TransferReader> TR = WS->TransferReader();
  //   occ::handle<Transfer_TransientProcess> TP = TR->TransientProcess();
  //   TopoDS_Shape UUSh, NUSh;
  //   occ::handle<Transfer_Binder> binder = TP->Find(UUNAUO);
  //   if ( binder.IsNull() || ! binder->HasResult() )
  //     return aMainLabel;
  //   UUSh = TransferBRep::ShapeResult ( TP, binder );
  //   binder = TP->Find(NUNAUO);
  //   if ( binder.IsNull() || ! binder->HasResult() )
  //     return aMainLabel;
  //   NUSh = TransferBRep::ShapeResult ( TP, binder );

  // get first labels for first SHUO attribute
  TDF_Label          UULab, NULab;
  STEPConstruct_Tool Tool(WS);
  UULab = STEPCAFControl_Reader::FindInstance(UUNAUO, STool, Tool, ShapeLabelMap);
  NULab = STEPCAFControl_Reader::FindInstance(NUNAUO, STool, Tool, ShapeLabelMap);

  //   STool->Search(UUSh, UULab);
  //   STool->Search(NUSh, NULab);
  if (UULab.IsNull() || NULab.IsNull())
    return aMainLabel;
  // create sequence fo labels to set SHUO structure into the document
  NCollection_Sequence<TDF_Label> ShuoLabels;
  ShuoLabels.Append(UULab);
  ShuoLabels.Append(NULab);
  // add all other labels of sub SHUO entities
  findNextSHUOlevel(WS, SHUO, STool, PDFileMap, ShapeLabelMap, ShuoLabels);
  // last accord for SHUO
  occ::handle<XCAFDoc_GraphNode> anSHUOAttr;
  if (STool->SetSHUO(ShuoLabels, anSHUOAttr))
    aMainLabel = anSHUOAttr->Label();

  return aMainLabel;
}

//=================================================================================================

bool STEPCAFControl_Reader::ReadSHUOs(
  const occ::handle<XSControl_WorkSession>&                          WS,
  const occ::handle<TDocStd_Document>&                               Doc,
  const NCollection_DataMap<occ::handle<StepBasic_ProductDefinition>,
                            occ::handle<STEPCAFControl_ExternFile>>& PDFileMap) const
{
  // the big part code duplication from ReadColors.
  // It is possible to share this code functionality, just to decide how ???
  occ::handle<XCAFDoc_ColorTool> CTool = XCAFDoc_DocumentTool::ColorTool(Doc->Main());
  occ::handle<XCAFDoc_ShapeTool> STool = CTool->ShapeTool();

  STEPConstruct_Styles Styles(WS);
  if (!Styles.LoadStyles())
  {
#ifdef OCCT_DEBUG
    std::cout << "Warning: no styles are found in the model" << std::endl;
#endif
    return false;
  }
  // searching for invisible items in the model
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> aHSeqOfInvisStyle =
    new NCollection_HSequence<occ::handle<Standard_Transient>>;
  Styles.LoadInvisStyles(aHSeqOfInvisStyle);
  // parse and search for color attributes
  int nb = Styles.NbStyles();
  for (int i = 1; i <= nb; i++)
  {
    occ::handle<StepVisual_StyledItem> style = Styles.Style(i);
    if (style.IsNull())
      continue;

    bool IsVisible = true;
    // check the visibility of styled item.
    for (int si = 1; si <= aHSeqOfInvisStyle->Length(); si++)
    {
      if (style != aHSeqOfInvisStyle->Value(si))
        continue;
      // found that current style is invisible.
#ifdef OCCT_DEBUG
      std::cout << "Warning: item No " << i << "(" << style->Item()->DynamicType()->Name()
                << ") is invisible" << std::endl;
#endif
      IsVisible = false;
      break;
    }

    occ::handle<StepVisual_Colour> SurfCol, BoundCol, CurveCol;
    // check if it is component style
    bool                              IsComponent = false;
    STEPConstruct_RenderingProperties aRenderProps;
    if (!Styles.GetColors(style, SurfCol, BoundCol, CurveCol, aRenderProps, IsComponent)
        && IsVisible)
      continue;
    if (!IsComponent)
      continue;
    occ::handle<StepShape_ShapeRepresentation> aSR;
    findStyledSR(style, aSR);
    // search for SR along model
    if (aSR.IsNull())
      continue;
    Interface_EntityIterator                             subs = WS->HGraph()->Graph().Sharings(aSR);
    occ::handle<StepShape_ShapeDefinitionRepresentation> aSDR;
    for (subs.Start(); subs.More(); subs.Next())
    {
      aSDR = occ::down_cast<StepShape_ShapeDefinitionRepresentation>(subs.Value());
      if (aSDR.IsNull())
        continue;
      StepRepr_RepresentedDefinition               aPDSselect = aSDR->Definition();
      occ::handle<StepRepr_ProductDefinitionShape> PDS =
        occ::down_cast<StepRepr_ProductDefinitionShape>(aPDSselect.PropertyDefinition());
      if (PDS.IsNull())
        continue;
      StepRepr_CharacterizedDefinition                     aCharDef = PDS->Definition();
      occ::handle<StepRepr_SpecifiedHigherUsageOccurrence> SHUO =
        occ::down_cast<StepRepr_SpecifiedHigherUsageOccurrence>(
          aCharDef.ProductDefinitionRelationship());
      if (SHUO.IsNull())
        continue;

      // set the SHUO structure to the document
      TDF_Label aLabelForStyle = setSHUOintoDoc(WS, SHUO, STool, PDFileMap, myMap);
      if (aLabelForStyle.IsNull())
      {
#ifdef OCCT_DEBUG
        std::cout << "Warning: " << __FILE__ << ": coudnot create SHUO structure in the document"
                  << std::endl;
#endif
        continue;
      }
      // now set the style to the SHUO main label.
      if (!SurfCol.IsNull() || aRenderProps.IsDefined())
      {
        Quantity_Color     col;
        Quantity_ColorRGBA colRGBA;
        if (!SurfCol.IsNull())
        {
          STEPConstruct_Styles::DecodeColor(SurfCol, col);
          colRGBA = Quantity_ColorRGBA(col);
        }
        if (aRenderProps.IsDefined())
        {
          colRGBA = aRenderProps.GetRGBAColor();
        }
        CTool->SetColor(aLabelForStyle, colRGBA, XCAFDoc_ColorSurf);
      }
      if (!BoundCol.IsNull())
      {
        Quantity_Color col;
        STEPConstruct_Styles::DecodeColor(BoundCol, col);
        CTool->SetColor(aLabelForStyle, col, XCAFDoc_ColorCurv);
      }
      if (!CurveCol.IsNull())
      {
        Quantity_Color col;
        STEPConstruct_Styles::DecodeColor(CurveCol, col);
        CTool->SetColor(aLabelForStyle, col, XCAFDoc_ColorCurv);
      }
      if (!IsVisible)
        // sets the invisibility for shape.
        CTool->SetVisibility(aLabelForStyle, false);

    } // end search SHUO by SDR
  } // end iterates on styles

  return true;
}

//=================================================================================================

static bool GetMassConversionFactor(const occ::handle<StepBasic_NamedUnit>& NU, double& afact)
{
  afact = 1.;
  if (!NU->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndMassUnit)))
    return false;
  occ::handle<StepBasic_ConversionBasedUnitAndMassUnit> CBUMU =
    occ::down_cast<StepBasic_ConversionBasedUnitAndMassUnit>(NU);
  occ::handle<StepBasic_MeasureWithUnit> MWUCBU = GetMeasureWithUnit(CBUMU->ConversionFactor());
  afact                                         = MWUCBU->ValueComponent();
  StepBasic_Unit anUnit2                        = MWUCBU->UnitComponent();
  if (anUnit2.CaseNum(anUnit2.Value()) == 1)
  {
    occ::handle<StepBasic_NamedUnit> NU2 = anUnit2.NamedUnit();
    if (NU2->IsKind(STANDARD_TYPE(StepBasic_SiUnit)))
    {
      occ::handle<StepBasic_SiUnit> SU = occ::down_cast<StepBasic_SiUnit>(NU2);
      if (SU->Name() == StepBasic_sunGram)
      {
        if (SU->HasPrefix())
          afact *= STEPConstruct_UnitContext::ConvertSiPrefix(SU->Prefix());
      }
    }
  }
  return true;
}

// ============================================================================
// Method  : createMesh
// Purpose : creates a Poly_Triangulation from ComplexTriangulatedSurfaceSet
// ============================================================================

occ::handle<Poly_Triangulation> createMesh(
  const occ::handle<StepVisual_ComplexTriangulatedSurfaceSet>& theTriangulatedSufaceSet,
  const double                                                 theFact)
{
  occ::handle<StepVisual_CoordinatesList>  aCoords = theTriangulatedSufaceSet->Coordinates();
  occ::handle<NCollection_HArray1<gp_XYZ>> aNodes  = aCoords->Points();
  occ::handle<NCollection_HArray1<occ::handle<Standard_Transient>>> aTriaStrips =
    theTriangulatedSufaceSet->TriangleStrips();
  occ::handle<NCollection_HArray1<occ::handle<Standard_Transient>>> aTriaFans =
    theTriangulatedSufaceSet->TriangleFans();
  const bool aHasUVNodes = false;
  const int  aNbNormals  = theTriangulatedSufaceSet->NbNormals();
  // Number of pairs (Point, Normal). It is possible for one point to have multiple normals. This is
  // useful when the underlying surface is not C1 continuous.
  const int  aNbPairs    = aNbNormals > 1 ? theTriangulatedSufaceSet->Pnmax() : aNodes->Length();
  const bool aHasNormals = aNbNormals > 0;

  // Counting number of triangles in the triangle strips list.
  // A triangle strip is a list of 3 or more points defining a set of connected triangles.
  int aNbTriaStrips = 0;
  for (int i = 1; i <= theTriangulatedSufaceSet->NbTriangleStrips(); ++i)
  {
    occ::handle<NCollection_HArray1<int>> aTriangleStrip =
      occ::down_cast<NCollection_HArray1<int>>(aTriaStrips->Value(i));
    aNbTriaStrips += aTriangleStrip->Length() - 2;
  }

  // Counting number of triangles in the triangle fans list.
  // A triangle fan is a set of 3 or more points defining a set of connected triangles sharing a
  // common vertex.
  int aNbTriaFans = 0;
  for (int i = 1; i <= theTriangulatedSufaceSet->NbTriangleFans(); ++i)
  {
    occ::handle<NCollection_HArray1<int>> aTriangleFan =
      occ::down_cast<NCollection_HArray1<int>>(aTriaFans->Value(i));
    aNbTriaFans += aTriangleFan->Length() - 2;
  }

  occ::handle<Poly_Triangulation> aMesh =
    new Poly_Triangulation(aNbPairs, aNbTriaStrips + aNbTriaFans, aHasUVNodes, aHasNormals);

  for (int j = 1; j <= aNbPairs; ++j)
  {
    const gp_XYZ& aPoint =
      aNodes->Value((aNbNormals > 1 && theTriangulatedSufaceSet->NbPnindex() > 0)
                      ? theTriangulatedSufaceSet->PnindexValue(j)
                      : j);
    aMesh->SetNode(j, theFact * aPoint);
  }

  // Creating triangles from triangle strips. Processing is split in two parts to
  // path through nodes in the same direction.
  int k = 1;
  for (int i = 1; i <= theTriangulatedSufaceSet->NbTriangleStrips(); ++i)
  {
    occ::handle<NCollection_HArray1<int>> aTriangleStrip =
      occ::down_cast<NCollection_HArray1<int>>(aTriaStrips->Value(i));
    for (int j = 3; j <= aTriangleStrip->Length(); j += 2)
    {
      if (aTriangleStrip->Value(j) != aTriangleStrip->Value(j - 2)
          && aTriangleStrip->Value(j) != aTriangleStrip->Value(j - 1))
      {
        aMesh->SetTriangle(k++,
                           Poly_Triangle(aTriangleStrip->Value(j - 2),
                                         aTriangleStrip->Value(j),
                                         aTriangleStrip->Value(j - 1)));
      }
    }
    for (int j = 4; j <= aTriangleStrip->Length(); j += 2)
    {
      if (aTriangleStrip->Value(j) != aTriangleStrip->Value(j - 2)
          && aTriangleStrip->Value(j) != aTriangleStrip->Value(j - 1))
      {
        aMesh->SetTriangle(k++,
                           Poly_Triangle(aTriangleStrip->Value(j - 2),
                                         aTriangleStrip->Value(j - 1),
                                         aTriangleStrip->Value(j)));
      }
    }
  }

  // Creating triangles from triangle strips.
  for (int i = 1; i <= theTriangulatedSufaceSet->NbTriangleFans(); ++i)
  {
    occ::handle<NCollection_HArray1<int>> aTriangleFan =
      occ::down_cast<NCollection_HArray1<int>>(aTriaFans->Value(i));
    for (int j = 3; j <= aTriangleFan->Length(); ++j)
    {
      aMesh->SetTriangle(
        k++,
        Poly_Triangle(aTriangleFan->Value(1), aTriangleFan->Value(j - 1), aTriangleFan->Value(j)));
    }
  }

  if (aHasNormals)
  {
    occ::handle<NCollection_HArray2<double>> aNormals = theTriangulatedSufaceSet->Normals();
    gp_XYZ                                   aNorm;
    if (theTriangulatedSufaceSet->NbNormals() == 1)
    {
      aNorm.SetX(aNormals->Value(1, 1));
      aNorm.SetY(aNormals->Value(1, 2));
      aNorm.SetZ(aNormals->Value(1, 3));
      for (int i = 1; i <= aNbPairs; ++i)
      {
        aMesh->SetNormal(i, aNorm);
      }
    }
    else
    {
      for (int i = 1; i <= aNbPairs; ++i)
      {
        aNorm.SetX(aNormals->Value(i, 1));
        aNorm.SetY(aNormals->Value(i, 2));
        aNorm.SetZ(aNormals->Value(i, 3));
        aMesh->SetNormal(i, aNorm);
      }
    }
  }

  return aMesh;
}

//=======================================================================
// function : readPMIPresentation
// purpose  : read polyline or tessellated presentation for
// (Annotation_Curve_Occurrence or Draughting_Callout)
//=======================================================================
bool readPMIPresentation(const occ::handle<Standard_Transient>&       thePresentEntity,
                         const occ::handle<XSControl_TransferReader>& theTR,
                         const double                                 theFact,
                         TopoDS_Shape&                                thePresentation,
                         occ::handle<TCollection_HAsciiString>&       thePresentName,
                         Bnd_Box&                                     theBox,
                         const StepData_Factors&                      theLocalFactors)
{
  if (thePresentEntity.IsNull())
  {
    return false;
  }
  occ::handle<Transfer_TransientProcess>                 aTP = theTR->TransientProcess();
  occ::handle<StepVisual_StyledItem>                     anAO;
  NCollection_Vector<occ::handle<StepVisual_StyledItem>> anAnnotations;
  if (thePresentEntity->IsKind(STANDARD_TYPE(StepVisual_AnnotationOccurrence))
      || thePresentEntity->IsKind(STANDARD_TYPE(StepVisual_TessellatedAnnotationOccurrence)))
  {
    anAO = occ::down_cast<StepVisual_StyledItem>(thePresentEntity);
    if (!anAO.IsNull())
    {
      thePresentName = anAO->Name();
      anAnnotations.Append(anAO);
    }
  }
  else if (thePresentEntity->IsKind(STANDARD_TYPE(StepVisual_DraughtingCallout)))
  {
    occ::handle<StepVisual_DraughtingCallout> aDCallout =
      occ::down_cast<StepVisual_DraughtingCallout>(thePresentEntity);
    thePresentName = aDCallout->Name();
    for (int i = 1; i <= aDCallout->NbContents() && anAO.IsNull(); i++)
    {
      anAO = occ::down_cast<StepVisual_AnnotationOccurrence>(aDCallout->ContentsValue(i).Value());
      if (!anAO.IsNull())
      {
        anAnnotations.Append(anAO);
        continue;
      }
      occ::handle<StepVisual_TessellatedAnnotationOccurrence> aTesselation =
        aDCallout->ContentsValue(i).TessellatedAnnotationOccurrence();
      if (!aTesselation.IsNull())
        anAnnotations.Append(aTesselation);
    }
  }

  if (!anAnnotations.Length())
  {
    return false;
  }

  BRep_Builder    aB;
  TopoDS_Compound aResAnnotation;
  aB.MakeCompound(aResAnnotation);

  Bnd_Box aBox;
  int     aNbShapes = 0;
  for (int i = 0; i < anAnnotations.Length(); i++)
  {
    occ::handle<StepVisual_StyledItem> anItem = anAnnotations(i);
    anAO = occ::down_cast<StepVisual_AnnotationOccurrence>(anItem);
    TopoDS_Shape anAnnotationShape;
    if (!anAO.IsNull())
    {
      occ::handle<StepRepr_RepresentationItem> aCurveItem = anAO->Item();
      anAnnotationShape = STEPConstruct::FindShape(aTP, aCurveItem);
      if (anAnnotationShape.IsNull())
      {
        occ::handle<Transfer_Binder> aBinder = theTR->Actor()->Transfer(aCurveItem, aTP);
        if (!aBinder.IsNull() && aBinder->HasResult())
        {
          anAnnotationShape = TransferBRep::ShapeResult(aTP, aBinder);
        }
      }
    }
    // case of tessellated entities
    else
    {
      occ::handle<StepRepr_RepresentationItem> aRepresentationItem = anItem->Item();
      if (aRepresentationItem.IsNull())
        continue;
      occ::handle<StepVisual_TessellatedGeometricSet> aTessSet =
        occ::down_cast<StepVisual_TessellatedGeometricSet>(aRepresentationItem);
      if (aTessSet.IsNull())
        continue;
      gp_Trsf aTransf;
      if (aTessSet->IsKind(STANDARD_TYPE(StepVisual_RepositionedTessellatedGeometricSet)))
      {
        occ::handle<StepVisual_RepositionedTessellatedGeometricSet> aRTGS =
          occ::down_cast<StepVisual_RepositionedTessellatedGeometricSet>(aTessSet);
        occ::handle<Geom_Axis2Placement> aLocation =
          StepToGeom::MakeAxis2Placement(aRTGS->Location(), theLocalFactors);
        if (!aLocation.IsNull())
        {
          const gp_Ax3 anAx3Orig = gp::XOY();
          const gp_Ax3 anAx3Targ(aLocation->Ax2());
          if (anAx3Targ.Location().SquareDistance(anAx3Orig.Location())
                >= Precision::SquareConfusion()
              || !anAx3Targ.Direction().IsEqual(anAx3Orig.Direction(), Precision::Angular())
              || !anAx3Targ.XDirection().IsEqual(anAx3Orig.XDirection(), Precision::Angular())
              || !anAx3Targ.YDirection().IsEqual(anAx3Orig.YDirection(), Precision::Angular()))
          {
            aTransf.SetTransformation(anAx3Targ, anAx3Orig);
          }
        }
      }
      NCollection_Handle<NCollection_Array1<occ::handle<StepVisual_TessellatedItem>>> aListItems =
        aTessSet->Items();
      int             aNbItems = aListItems.IsNull() ? 0 : aListItems->Length();
      TopoDS_Compound aComp;
      aB.MakeCompound(aComp);
      for (int j = 1; j <= aNbItems; j++)
      {
        occ::handle<StepVisual_TessellatedItem> aTessItem = aListItems->Value(j);
        if (aTessItem.IsNull())
        {
          continue;
        }
        if (aTessItem->IsKind(STANDARD_TYPE(StepVisual_TessellatedCurveSet)))
        {
          occ::handle<StepVisual_TessellatedCurveSet> aTessCurve =
            occ::down_cast<StepVisual_TessellatedCurveSet>(aTessItem);
          occ::handle<StepVisual_CoordinatesList> aCoordList = aTessCurve->CoordList();
          if (aCoordList.IsNull())
          {
            continue;
          }

          occ::handle<NCollection_HArray1<gp_XYZ>> aPoints = aCoordList->Points();
          if (aPoints.IsNull() || aPoints->Length() == 0)
          {
            continue;
          }

          NCollection_Handle<NCollection_Vector<occ::handle<NCollection_HSequence<int>>>> aCurves =
            aTessCurve->Curves();
          int aNbCurves = (aCurves.IsNull() ? 0 : aCurves->Length());
          for (int k = 0; k < aNbCurves; k++)
          {
            occ::handle<NCollection_HSequence<int>> anIndexes = aCurves->Value(k);
            TopoDS_Wire                             aCurW;
            aB.MakeWire(aCurW);
            for (int n = 1; n < anIndexes->Length(); n++)
            {
              int anIndex    = anIndexes->Value(n);
              int aNextIndex = anIndexes->Value(n + 1);
              if (anIndex > aPoints->Length() || aNextIndex > aPoints->Length())
              {
                continue;
              }
              gp_Pnt                  aP1(aPoints->Value(anIndex) * theFact);
              gp_Pnt                  aP2(aPoints->Value(aNextIndex) * theFact);
              BRepBuilderAPI_MakeEdge aMaker(aP1, aP2);
              if (aMaker.IsDone())
              {
                TopoDS_Edge aCurE = aMaker.Edge();
                aB.Add(aCurW, aCurE);
              }
            }
            aB.Add(aComp, aCurW);
          }
        }
        else if (aTessItem->IsKind(STANDARD_TYPE(StepVisual_ComplexTriangulatedSurfaceSet)))
        {
          occ::handle<StepVisual_ComplexTriangulatedSurfaceSet> aTessSurfSet =
            occ::down_cast<StepVisual_ComplexTriangulatedSurfaceSet>(aTessItem);
          occ::handle<Poly_Triangulation> aSurfSetMesh = createMesh(aTessSurfSet, theFact);
          TopoDS_Face                     aFace;
          aB.MakeFace(aFace, aSurfSetMesh);
          aB.Add(aComp, aFace);
        }
      }
      if (!aComp.IsNull())
      {
        anAnnotationShape = aComp.Moved(aTransf);
      }
    }
    if (!anAnnotationShape.IsNull())
    {
      aNbShapes++;
      aB.Add(aResAnnotation, anAnnotationShape);
      if (i == anAnnotations.Length() - 1)
        BRepBndLib::AddClose(anAnnotationShape, aBox);
    }
  }

  thePresentation = aResAnnotation;
  theBox          = aBox;
  return (aNbShapes > 0);
}

//=================================================================================================

bool readAnnotationPlane(const occ::handle<StepVisual_AnnotationPlane>& theAnnotationPlane,
                         gp_Ax2&                                        thePlane,
                         const StepData_Factors&                        theLocalFactors)
{
  if (theAnnotationPlane.IsNull())
    return false;
  occ::handle<StepRepr_RepresentationItem> aPlaneItem = theAnnotationPlane->Item();
  if (aPlaneItem.IsNull())
    return false;
  occ::handle<StepGeom_Axis2Placement3d> aA2P3D;
  // retrieve axes from AnnotationPlane
  if (aPlaneItem->IsKind(STANDARD_TYPE(StepGeom_Plane)))
  {
    occ::handle<StepGeom_Plane> aPlane = occ::down_cast<StepGeom_Plane>(aPlaneItem);
    aA2P3D                             = aPlane->Position();
  }
  else if (aPlaneItem->IsKind(STANDARD_TYPE(StepVisual_PlanarBox)))
  {
    occ::handle<StepVisual_PlanarBox> aBox = occ::down_cast<StepVisual_PlanarBox>(aPlaneItem);
    aA2P3D                                 = aBox->Placement().Axis2Placement3d();
  }
  if (aA2P3D.IsNull())
    return false;

  occ::handle<Geom_Axis2Placement> anAxis = StepToGeom::MakeAxis2Placement(aA2P3D, theLocalFactors);
  thePlane                                = anAxis->Ax2();
  return true;
}

//=======================================================================
// function : readAnnotation
// purpose  : read annotation plane and position for given GDT
// (Dimension, Geometric_Tolerance, Datum_Feature or Placed_Datum_Target_Feature)
//=======================================================================
void readAnnotation(const occ::handle<XSControl_TransferReader>& theTR,
                    const occ::handle<Standard_Transient>&       theGDT,
                    const occ::handle<Standard_Transient>&       theDimObject,
                    const StepData_Factors&                      theLocalFactors)
{
  if (theGDT.IsNull() || theDimObject.IsNull())
    return;
  occ::handle<TCollection_HAsciiString>  aPresentName;
  TopoDS_Compound                        aResAnnotation;
  occ::handle<Transfer_TransientProcess> aTP    = theTR->TransientProcess();
  const Interface_Graph&                 aGraph = aTP->Graph();
  // find the proper DraughtingModelItemAssociation
  Interface_EntityIterator                              subs = aGraph.Sharings(theGDT);
  occ::handle<StepAP242_DraughtingModelItemAssociation> aDMIA;
  for (subs.Start(); subs.More() && aDMIA.IsNull(); subs.Next())
  {
    if (!subs.Value()->IsKind(STANDARD_TYPE(StepAP242_DraughtingModelItemAssociation)))
      continue;
    aDMIA = occ::down_cast<StepAP242_DraughtingModelItemAssociation>(subs.Value());
    occ::handle<TCollection_HAsciiString> aName = aDMIA->Name();
    aName->LowerCase();
    if (!aName->Search(new TCollection_HAsciiString("pmi representation to presentation link")))
    {
      aDMIA = nullptr;
    }
  }
  if (aDMIA.IsNull() || aDMIA->NbIdentifiedItem() == 0)
    return;

  // calculate units
  occ::handle<StepVisual_DraughtingModel> aDModel =
    occ::down_cast<StepVisual_DraughtingModel>(aDMIA->UsedRepresentation());
  XSAlgo_ShapeProcessor::PrepareForTransfer();
  STEPControl_ActorRead anActor(aTP->Model());
  StepData_Factors      aLocalFactors = theLocalFactors;
  anActor.PrepareUnits(aDModel, aTP, aLocalFactors);
  double aFact = aLocalFactors.LengthFactor();

  // retrieve AnnotationPlane
  occ::handle<StepRepr_RepresentationItem> aDMIAE = aDMIA->IdentifiedItemValue(1);
  if (aDMIAE.IsNull())
    return;
  gp_Ax2 aPlaneAxes;
  subs = aGraph.Sharings(aDMIAE);
  occ::handle<StepVisual_AnnotationPlane> anAnPlane;
  for (subs.Start(); subs.More() && anAnPlane.IsNull(); subs.Next())
  {
    anAnPlane = occ::down_cast<StepVisual_AnnotationPlane>(subs.Value());
  }
  bool isHasPlane = readAnnotationPlane(anAnPlane, aPlaneAxes, aLocalFactors);

  // set plane axes to XCAF
  if (isHasPlane)
  {
    if (theDimObject->IsKind(STANDARD_TYPE(XCAFDimTolObjects_DimensionObject)))
    {
      occ::handle<XCAFDimTolObjects_DimensionObject> anObj =
        occ::down_cast<XCAFDimTolObjects_DimensionObject>(theDimObject);
      occ::handle<NCollection_HArray1<gp_Pnt>> aPnts = new NCollection_HArray1<gp_Pnt>(1, 1);
      anObj->SetPlane(aPlaneAxes);
    }
    else if (theDimObject->IsKind(STANDARD_TYPE(XCAFDimTolObjects_DatumObject)))
    {
      occ::handle<XCAFDimTolObjects_DatumObject> anObj =
        occ::down_cast<XCAFDimTolObjects_DatumObject>(theDimObject);
      anObj->SetPlane(aPlaneAxes);
    }
    else if (theDimObject->IsKind(STANDARD_TYPE(XCAFDimTolObjects_GeomToleranceObject)))
    {
      occ::handle<XCAFDimTolObjects_GeomToleranceObject> anObj =
        occ::down_cast<XCAFDimTolObjects_GeomToleranceObject>(theDimObject);
      anObj->SetPlane(aPlaneAxes);
    }
  }

  // Retrieve presentation
  Bnd_Box aBox;
  if (!readPMIPresentation(aDMIAE, theTR, aFact, aResAnnotation, aPresentName, aBox, aLocalFactors))
    return;
  gp_Pnt aPtext(0., 0., 0.);
  // if Annotation plane location inside bounding box set it to text position
  // else set the center of bounding box to text position 0027372
  if (!aBox.IsVoid())
  {
    double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
    if (isHasPlane && !aBox.IsOut(aPlaneAxes.Location()))
    {
      aPtext = aPlaneAxes.Location();
    }
    else
    {
      aPtext = gp_Pnt((aXmin + aXmax) * 0.5, (aYmin + aYmax) * 0.5, (aZmin + aZmax) * 0.5);
    }
  }
  else
  {
    aPtext = aPlaneAxes.Location();
  }

  // set point to XCAF
  if (theDimObject->IsKind(STANDARD_TYPE(XCAFDimTolObjects_DimensionObject)))
  {
    occ::handle<XCAFDimTolObjects_DimensionObject> anObj =
      occ::down_cast<XCAFDimTolObjects_DimensionObject>(theDimObject);
    anObj->SetPointTextAttach(aPtext);
    anObj->SetPresentation(aResAnnotation, aPresentName);
  }
  else if (theDimObject->IsKind(STANDARD_TYPE(XCAFDimTolObjects_DatumObject)))
  {
    occ::handle<XCAFDimTolObjects_DatumObject> anObj =
      occ::down_cast<XCAFDimTolObjects_DatumObject>(theDimObject);
    anObj->SetPointTextAttach(aPtext);
    anObj->SetPresentation(aResAnnotation, aPresentName);
  }
  else if (theDimObject->IsKind(STANDARD_TYPE(XCAFDimTolObjects_GeomToleranceObject)))
  {
    occ::handle<XCAFDimTolObjects_GeomToleranceObject> anObj =
      occ::down_cast<XCAFDimTolObjects_GeomToleranceObject>(theDimObject);
    anObj->SetPointTextAttach(aPtext);
    anObj->SetPresentation(aResAnnotation, aPresentName);
  }
  return;
}

//=======================================================================
// function : retrieveConnectionPointFromGISU
// purpose  : auxiliary method for reading connection points
//=======================================================================
static bool retrieveConnectionPointFromGISU(
  const occ::handle<StepAP242_GeometricItemSpecificUsage>& theGISU,
  const double                                             theFact,
  gp_Ax2&                                                  theConnectionCS,
  bool&                                                    theIsPoint,
  occ::handle<TCollection_HAsciiString>&                   theConnName)
{
  if (theGISU.IsNull() || theGISU->NbIdentifiedItem() == 0)
    return false;

  occ::handle<StepGeom_CartesianPoint> aPoint =
    occ::down_cast<StepGeom_CartesianPoint>(theGISU->IdentifiedItem()->Value(1));
  if (!aPoint.IsNull())
  {
    theIsPoint  = true;
    theConnName = aPoint->Name();
    gp_Pnt aLoc(aPoint->CoordinatesValue(1) * theFact,
                aPoint->CoordinatesValue(2) * theFact,
                aPoint->CoordinatesValue(3) * theFact);
    theConnectionCS.SetLocation(aLoc);
    return true;
  }
  else
  {
    // try Axis2Placement3d.location instead of CartesianPoint
    occ::handle<StepGeom_Axis2Placement3d> anA2P3D =
      occ::down_cast<StepGeom_Axis2Placement3d>(theGISU->IdentifiedItem()->Value(1));
    if (anA2P3D.IsNull())
      return false;
    theIsPoint  = false;
    theConnName = anA2P3D->Name();
    occ::handle<Geom_Axis2Placement> anAxis =
      StepToGeom::MakeAxis2Placement(anA2P3D, StepData_Factors());
    theConnectionCS = anAxis->Ax2();
    return true;
  }
  return false;
}

//=======================================================================
// function : readConnectionPoints
// purpose  : read connection points for given dimension
//=======================================================================
void readConnectionPoints(const occ::handle<XSControl_TransferReader>&          theTR,
                          const occ::handle<Standard_Transient>&                theGDT,
                          const occ::handle<XCAFDimTolObjects_DimensionObject>& theDimObject,
                          const StepData_Factors&                               theLocalFactors)
{
  if (theGDT.IsNull() || theDimObject.IsNull())
    return;
  occ::handle<Transfer_TransientProcess> aTP    = theTR->TransientProcess();
  const Interface_Graph&                 aGraph = aTP->Graph();

  double aFact = 1.;

  occ::handle<StepShape_ShapeDimensionRepresentation> aSDR = nullptr;
  for (Interface_EntityIterator anIt = aGraph.Sharings(theGDT); aSDR.IsNull() && anIt.More();
       anIt.Next())
  {
    const occ::handle<Standard_Transient>&                         anEnt = anIt.Value();
    occ::handle<StepShape_DimensionalCharacteristicRepresentation> aDCR =
      occ::down_cast<StepShape_DimensionalCharacteristicRepresentation>(anEnt);
    if (!aDCR.IsNull())
      aSDR = !aDCR.IsNull() ? aDCR->Representation()
                            : occ::down_cast<StepShape_ShapeDimensionRepresentation>(anEnt);
  }
  if (!aSDR.IsNull())
  {
    XSAlgo_ShapeProcessor::PrepareForTransfer();
    STEPControl_ActorRead anActor(theTR->Model());
    StepData_Factors      aLocalFactors = theLocalFactors;
    anActor.PrepareUnits(aSDR, aTP, aLocalFactors);
    aFact = aLocalFactors.LengthFactor();
  }

  if (theGDT->IsKind(STANDARD_TYPE(StepShape_DimensionalSize)))
  {
    // retrieve derived geometry
    occ::handle<StepShape_DimensionalSize> aDim = occ::down_cast<StepShape_DimensionalSize>(theGDT);
    occ::handle<StepRepr_DerivedShapeAspect> aDSA =
      occ::down_cast<StepRepr_DerivedShapeAspect>(aDim->AppliesTo());
    if (aDSA.IsNull())
      return;
    occ::handle<StepAP242_GeometricItemSpecificUsage> aGISU = nullptr;
    for (Interface_EntityIterator anIt = aGraph.Sharings(aDSA); aGISU.IsNull() && anIt.More();
         anIt.Next())
    {
      aGISU = occ::down_cast<StepAP242_GeometricItemSpecificUsage>(anIt.Value());
    }
    gp_Ax2                                aCS;
    bool                                  aIsPoint = false;
    occ::handle<TCollection_HAsciiString> aConnName;
    if (retrieveConnectionPointFromGISU(aGISU, aFact, aCS, aIsPoint, aConnName))
    {
      if (aIsPoint)
        theDimObject->SetPoint(aCS.Location());
      else
        theDimObject->SetConnectionAxis(aCS);
      theDimObject->SetConnectionName(aConnName);
    }
  }
  else if (theGDT->IsKind(STANDARD_TYPE(StepShape_DimensionalLocation)))
  {
    // retrieve derived geometry
    occ::handle<StepShape_DimensionalLocation> aDim =
      occ::down_cast<StepShape_DimensionalLocation>(theGDT);
    occ::handle<StepRepr_DerivedShapeAspect> aDSA1 =
      occ::down_cast<StepRepr_DerivedShapeAspect>(aDim->RelatingShapeAspect());
    occ::handle<StepRepr_DerivedShapeAspect> aDSA2 =
      occ::down_cast<StepRepr_DerivedShapeAspect>(aDim->RelatedShapeAspect());
    if (aDSA1.IsNull() && aDSA2.IsNull())
      return;
    occ::handle<StepAP242_GeometricItemSpecificUsage> aGISU1 = nullptr;
    occ::handle<StepAP242_GeometricItemSpecificUsage> aGISU2 = nullptr;
    if (!aDSA1.IsNull())
    {
      for (Interface_EntityIterator anIt = aGraph.Sharings(aDSA1); aGISU1.IsNull() && anIt.More();
           anIt.Next())
      {
        aGISU1 = occ::down_cast<StepAP242_GeometricItemSpecificUsage>(anIt.Value());
      }
    }
    if (!aDSA2.IsNull())
    {
      for (Interface_EntityIterator anIt = aGraph.Sharings(aDSA2); aGISU2.IsNull() && anIt.More();
           anIt.Next())
      {
        aGISU2 = occ::down_cast<StepAP242_GeometricItemSpecificUsage>(anIt.Value());
      }
    }
    // first point
    gp_Ax2                                aCS;
    bool                                  aIsPoint = false;
    occ::handle<TCollection_HAsciiString> aConnName;
    if (retrieveConnectionPointFromGISU(aGISU1, aFact, aCS, aIsPoint, aConnName))
    {
      if (aIsPoint)
        theDimObject->SetPoint(aCS.Location());
      else
        theDimObject->SetConnectionAxis(aCS);
      theDimObject->SetConnectionName(aConnName);
    }
    // second point
    if (retrieveConnectionPointFromGISU(aGISU2, aFact, aCS, aIsPoint, aConnName))
    {
      if (aIsPoint)
        theDimObject->SetPoint2(aCS.Location());
      else
        theDimObject->SetConnectionAxis2(aCS);
      theDimObject->SetConnectionName2(aConnName);
    }
  }
}

//=================================================================================================

static bool ReadDatums(const occ::handle<XCAFDoc_ShapeTool>&                               STool,
                       const occ::handle<XCAFDoc_DimTolTool>&                              DGTTool,
                       const Interface_Graph&                                              graph,
                       const occ::handle<Transfer_TransientProcess>&                       TP,
                       const TDF_Label                                                     TolerL,
                       const occ::handle<StepDimTol_GeometricToleranceWithDatumReference>& GTWDR)
{
  if (GTWDR.IsNull())
    return false;
  occ::handle<NCollection_HArray1<StepDimTol_DatumSystemOrReference>> aHADSOR =
    GTWDR->DatumSystemAP242();
  if (aHADSOR.IsNull())
  {
    return false;
  }
  NCollection_List<occ::handle<StepDimTol_Datum>> aDatumList;
  for (int idr = 1; idr <= aHADSOR->Length(); idr++)
  {
    const StepDimTol_DatumSystemOrReference aDSOR = aHADSOR->Value(idr);
    if (aDSOR.IsNull())
      continue;
    occ::handle<StepDimTol_DatumSystem>    aDS = aDSOR.DatumSystem();
    occ::handle<StepDimTol_DatumReference> aDR = aDSOR.DatumReference();
    occ::handle<StepDimTol_Datum>          aDatum;
    if (!aDS.IsNull())
    {
      auto aDatumConList = aDS->Constituents();
      for (int anInd = 1; anInd <= aDatumConList->Length(); anInd++)
      {
        occ::handle<StepDimTol_DatumReferenceCompartment> aDatRefC = aDatumConList->Value(anInd);
        aDatumList.Append(aDatRefC->Base().Datum());
      }
    }
    else if (!aDR.IsNull())
    {
      aDatumList.Append(aDR->ReferencedDatum());
    }
  }
  for (NCollection_List<occ::handle<StepDimTol_Datum>>::Iterator anIt(aDatumList); anIt.More();
       anIt.Next())
  {
    occ::handle<StepDimTol_Datum> aDatum = anIt.Value();
    if (aDatum.IsNull())
      continue;
    Interface_EntityIterator subs4 = graph.Sharings(aDatum);
    for (subs4.Start(); subs4.More(); subs4.Next())
    {
      occ::handle<StepRepr_ShapeAspectRelationship> SAR =
        occ::down_cast<StepRepr_ShapeAspectRelationship>(subs4.Value());
      if (SAR.IsNull())
        continue;
      occ::handle<StepDimTol_DatumFeature> DF =
        occ::down_cast<StepDimTol_DatumFeature>(SAR->RelatingShapeAspect());
      if (DF.IsNull())
        continue;
      Interface_EntityIterator                 subs5 = graph.Sharings(DF);
      occ::handle<StepRepr_PropertyDefinition> PropDef;
      for (subs5.Start(); subs5.More() && PropDef.IsNull(); subs5.Next())
      {
        PropDef = occ::down_cast<StepRepr_PropertyDefinition>(subs5.Value());
      }
      if (PropDef.IsNull())
        continue;
      occ::handle<StepShape_AdvancedFace> AF;
      subs5 = graph.Sharings(PropDef);
      for (subs5.Start(); subs5.More(); subs5.Next())
      {
        occ::handle<StepShape_ShapeDefinitionRepresentation> SDR =
          occ::down_cast<StepShape_ShapeDefinitionRepresentation>(subs5.Value());
        if (!SDR.IsNull())
        {
          occ::handle<StepRepr_Representation> Repr = SDR->UsedRepresentation();
          if (!Repr.IsNull() && Repr->NbItems() > 0)
          {
            occ::handle<StepRepr_RepresentationItem> RI = Repr->ItemsValue(1);
            AF = occ::down_cast<StepShape_AdvancedFace>(RI);
          }
        }
      }
      if (AF.IsNull())
        return false;
      int          index = TP->MapIndex(AF);
      TopoDS_Shape aSh;
      if (index > 0)
      {
        occ::handle<Transfer_Binder> binder = TP->MapItem(index);
        aSh                                 = TransferBRep::ShapeResult(binder);
      }
      if (aSh.IsNull())
        continue;
      TDF_Label shL;
      if (!STool->Search(aSh, shL, true, true, true))
        continue;
      DGTTool->SetDatum(shL,
                        TolerL,
                        PropDef->Name(),
                        PropDef->Description(),
                        aDatum->Identification());
    }
  }
  return true;
}

//=======================================================================
// function : FindShapeIndexForDGT
// purpose  : auxiliary find shape index in map og imported shapes
//=======================================================================
static int FindShapeIndexForDGT(const occ::handle<Standard_Transient>&    theEnt,
                                const occ::handle<XSControl_WorkSession>& theWS)
{
  const occ::handle<Transfer_TransientProcess>& aTP = theWS->TransferReader()->TransientProcess();
  // try to find index of given entity
  int anIndex = aTP->MapIndex(theEnt);
  if (anIndex > 0 || theEnt.IsNull())
    return anIndex;
  // if theEnt is a geometry item try to find its topological item
  const Interface_Graph&   aGraph = aTP->Graph();
  Interface_EntityIterator anIter = aGraph.Sharings(theEnt);
  for (anIter.Start(); anIter.More(); anIter.Next())
  {
    if (anIter.Value()->IsKind(STANDARD_TYPE(StepShape_TopologicalRepresentationItem)))
    {
      anIndex = aTP->MapIndex(anIter.Value());
      if (anIndex > 0)
        return anIndex;
    }
  }
  return 0;
}

//=================================================================================================

static void collectShapeAspect(const occ::handle<StepRepr_ShapeAspect>&                 theSA,
                               const occ::handle<XSControl_WorkSession>&                theWS,
                               NCollection_Sequence<occ::handle<StepRepr_ShapeAspect>>& theSAs)
{
  if (theSA.IsNull())
    return;
  occ::handle<XSControl_TransferReader>  aTR    = theWS->TransferReader();
  occ::handle<Transfer_TransientProcess> aTP    = aTR->TransientProcess();
  const Interface_Graph&                 aGraph = aTP->Graph();
  // Retrieve Shape_Aspect, connected to Representation_Item from Derived_Shape_Aspect
  if (theSA->IsKind(STANDARD_TYPE(StepRepr_DerivedShapeAspect)))
  {
    Interface_EntityIterator                              anIter = aGraph.Sharings(theSA);
    occ::handle<StepRepr_ShapeAspectDerivingRelationship> aSADR  = nullptr;
    for (; aSADR.IsNull() && anIter.More(); anIter.Next())
    {
      aSADR = occ::down_cast<StepRepr_ShapeAspectDerivingRelationship>(anIter.Value());
    }
    if (!aSADR.IsNull())
      collectShapeAspect(aSADR->RelatedShapeAspect(), theWS, theSAs);
  }
  else if (theSA->IsKind(STANDARD_TYPE(StepDimTol_DatumFeature))
           || theSA->IsKind(STANDARD_TYPE(StepDimTol_DatumTarget)))
  {
    theSAs.Append(theSA);
    return;
  }
  else
  {
    // Find all children Shape_Aspect
    bool                     isSimple = true;
    Interface_EntityIterator anIter   = aGraph.Sharings(theSA);
    for (; anIter.More(); anIter.Next())
    {
      if (anIter.Value()->IsKind(STANDARD_TYPE(StepRepr_ShapeAspectRelationship))
          && !anIter.Value()->IsKind(STANDARD_TYPE(StepShape_DimensionalLocation)))
      {
        occ::handle<StepRepr_ShapeAspectRelationship> aSAR =
          occ::down_cast<StepRepr_ShapeAspectRelationship>(anIter.Value());
        if (aSAR->RelatingShapeAspect() == theSA && !aSAR->RelatedShapeAspect().IsNull()
            && !aSAR->RelatedShapeAspect()->IsKind(STANDARD_TYPE(StepDimTol_Datum)))
        {
          collectShapeAspect(aSAR->RelatedShapeAspect(), theWS, theSAs);
          isSimple = false;
        }
      }
    }
    // If not Composite_Shape_Aspect (or subtype) append to sequence.
    if (isSimple)
      theSAs.Append(theSA);
  }
}

//=================================================================================================

static TDF_Label getShapeLabel(const occ::handle<StepRepr_RepresentationItem>& theItem,
                               const occ::handle<XSControl_WorkSession>&       theWS,
                               const occ::handle<XCAFDoc_ShapeTool>&           theShapeTool)
{
  TDF_Label                                     aShapeL;
  const occ::handle<Transfer_TransientProcess>& aTP   = theWS->TransferReader()->TransientProcess();
  int                                           index = FindShapeIndexForDGT(theItem, theWS);
  TopoDS_Shape                                  aShape;
  if (index > 0)
  {
    occ::handle<Transfer_Binder> aBinder = aTP->MapItem(index);
    aShape                               = TransferBRep::ShapeResult(aBinder);
  }
  if (aShape.IsNull())
    return aShapeL;
  theShapeTool->Search(aShape, aShapeL, true, true, true);
  return aShapeL;
}

//=================================================================================================

bool STEPCAFControl_Reader::setDatumToXCAF(
  const occ::handle<StepDimTol_Datum>&                            theDat,
  const TDF_Label                                                 theGDTL,
  const int                                                       thePositionCounter,
  const NCollection_Sequence<XCAFDimTolObjects_DatumSingleModif>& theXCAFModifiers,
  const XCAFDimTolObjects_DatumModifWithValue                     theXCAFModifWithVal,
  const double                                                    theModifValue,
  const occ::handle<TDocStd_Document>&                            theDoc,
  const occ::handle<XSControl_WorkSession>&                       theWS,
  const StepData_Factors&                                         theLocalFactors)
{
  occ::handle<XCAFDoc_ShapeTool>  aSTool   = XCAFDoc_DocumentTool::ShapeTool(theDoc->Main());
  occ::handle<XCAFDoc_DimTolTool> aDGTTool = XCAFDoc_DocumentTool::DimTolTool(theDoc->Main());
  const occ::handle<XSControl_TransferReader>&  aTR    = theWS->TransferReader();
  const occ::handle<Transfer_TransientProcess>& aTP    = aTR->TransientProcess();
  const Interface_Graph&                        aGraph = aTP->Graph();
  occ::handle<XCAFDoc_Datum>                    aDat;
  NCollection_Sequence<TDF_Label>               aShapeLabels;
  occ::handle<XCAFDimTolObjects_DatumObject>    aDatObj = new XCAFDimTolObjects_DatumObject();

  // Collect all links to shapes
  NCollection_Sequence<occ::handle<StepRepr_ShapeAspect>> aSAs;
  Interface_EntityIterator                                anIterD = aGraph.Sharings(theDat);
  for (anIterD.Start(); anIterD.More(); anIterD.Next())
  {
    occ::handle<StepRepr_ShapeAspectRelationship> aSAR =
      occ::down_cast<StepRepr_ShapeAspectRelationship>(anIterD.Value());
    if (aSAR.IsNull() || aSAR->RelatingShapeAspect().IsNull())
      continue;
    collectShapeAspect(aSAR->RelatingShapeAspect(), theWS, aSAs);
    occ::handle<StepDimTol_DatumFeature> aDF =
      occ::down_cast<StepDimTol_DatumFeature>(aSAR->RelatingShapeAspect());
    if (!aSAR->RelatingShapeAspect()->IsKind(STANDARD_TYPE(StepDimTol_DatumTarget)))
      readAnnotation(aTR, aSAR->RelatingShapeAspect(), aDatObj, theLocalFactors);
  }

  // Collect shape labels
  for (int i = 1; i <= aSAs.Length(); i++)
  {
    occ::handle<StepRepr_ShapeAspect> aSA = aSAs.Value(i);
    if (aSA.IsNull())
      continue;
    // Skip datum targets
    if (aSA->IsKind(STANDARD_TYPE(StepDimTol_DatumTarget)))
      continue;

    // Process all connected GISU
    Interface_EntityIterator anIter = aGraph.Sharings(aSA);
    for (; anIter.More(); anIter.Next())
    {
      occ::handle<StepAP242_GeometricItemSpecificUsage> aGISU =
        occ::down_cast<StepAP242_GeometricItemSpecificUsage>(anIter.Value());
      if (aGISU.IsNull())
        continue;
      for (int j = 1; j <= aGISU->NbIdentifiedItem(); j++)
      {
        TDF_Label aShapeL = getShapeLabel(aGISU->IdentifiedItemValue(j), theWS, aSTool);
        if (!aShapeL.IsNull())
          aShapeLabels.Append(aShapeL);
      }
    }
  }

  // Process datum targets and create objects for them
  bool isExistDatumTarget = false;
  for (int i = 1; i <= aSAs.Length(); i++)
  {
    occ::handle<StepDimTol_PlacedDatumTargetFeature> aDT =
      occ::down_cast<StepDimTol_PlacedDatumTargetFeature>(aSAs.Value(i));
    if (aDT.IsNull())
      continue;
    occ::handle<XCAFDimTolObjects_DatumObject> aDatTargetObj = new XCAFDimTolObjects_DatumObject();
    XCAFDimTolObjects_DatumTargetType          aType;
    if (!STEPCAFControl_GDTProperty::GetDatumTargetType(aDT->Description(), aType))
    {
      aTP->AddWarning(aDT, "Unknown datum target type");
      continue;
    }
    aDatTargetObj->SetDatumTargetType(aType);
    bool isValidDT = false;

    // Feature for datum target
    NCollection_Sequence<TDF_Label>                         aDTShapeLabels;
    Interface_EntityIterator                                aDTIter = aGraph.Sharings(aDT);
    occ::handle<StepRepr_FeatureForDatumTargetRelationship> aRelationship;
    for (; aDTIter.More() && aRelationship.IsNull(); aDTIter.Next())
    {
      aRelationship = occ::down_cast<StepRepr_FeatureForDatumTargetRelationship>(aDTIter.Value());
    }
    if (!aRelationship.IsNull())
    {
      occ::handle<StepRepr_ShapeAspect> aSA     = aRelationship->RelatingShapeAspect();
      Interface_EntityIterator          aSAIter = aGraph.Sharings(aSA);
      for (; aSAIter.More(); aSAIter.Next())
      {
        occ::handle<StepAP242_GeometricItemSpecificUsage> aGISU =
          occ::down_cast<StepAP242_GeometricItemSpecificUsage>(aSAIter.Value());
        if (aGISU.IsNull())
          continue;
        for (int j = 1; j <= aGISU->NbIdentifiedItem(); j++)
        {
          TDF_Label aShapeL = getShapeLabel(aGISU->IdentifiedItemValue(j), theWS, aSTool);
          if (!aShapeL.IsNull())
          {
            aDTShapeLabels.Append(aShapeL);
            isValidDT = true;
          }
        }
      }
    }

    if (aType != XCAFDimTolObjects_DatumTargetType_Area && !isValidDT)
    {
      // Try another way of feature connection
      for (aDTIter.Start(); aDTIter.More(); aDTIter.Next())
      {
        occ::handle<StepAP242_GeometricItemSpecificUsage> aGISU =
          occ::down_cast<StepAP242_GeometricItemSpecificUsage>(aDTIter.Value());
        if (aGISU.IsNull())
          continue;
        for (int j = 1; j <= aGISU->NbIdentifiedItem(); j++)
        {
          TDF_Label aShapeL = getShapeLabel(aGISU->IdentifiedItemValue(j), theWS, aSTool);
          if (!aShapeL.IsNull())
          {
            aDTShapeLabels.Append(aShapeL);
            isValidDT = true;
          }
        }
      }
    }

    if (aType == XCAFDimTolObjects_DatumTargetType_Area)
    {
      // Area datum target
      if (aRelationship.IsNull())
        continue;
      occ::handle<StepRepr_ShapeAspect>                 aSA = aRelationship->RelatingShapeAspect();
      Interface_EntityIterator                          aSAIter = aGraph.Sharings(aSA);
      occ::handle<StepAP242_GeometricItemSpecificUsage> aGISU;
      for (; aSAIter.More() && aGISU.IsNull(); aSAIter.Next())
      {
        aGISU = occ::down_cast<StepAP242_GeometricItemSpecificUsage>(aSAIter.Value());
      }
      occ::handle<StepRepr_RepresentationItem> anItem;
      if (!aGISU.IsNull() && aGISU->NbIdentifiedItem() > 0)
        anItem = aGISU->IdentifiedItemValue(1);
      if (anItem.IsNull())
        continue;
      int anItemIndex = FindShapeIndexForDGT(anItem, theWS);
      if (anItemIndex > 0)
      {
        occ::handle<Transfer_Binder> aBinder     = aTP->MapItem(anItemIndex);
        TopoDS_Shape                 anItemShape = TransferBRep::ShapeResult(aBinder);
        aDatTargetObj->SetDatumTarget(anItemShape);
        isValidDT = true;
      }
    }
    else
    {
      // Point/line/rectangle/circle datum targets
      Interface_EntityIterator                 anIter = aGraph.Sharings(aDT);
      occ::handle<StepRepr_PropertyDefinition> aPD;
      for (; anIter.More() && aPD.IsNull(); anIter.Next())
      {
        aPD = occ::down_cast<StepRepr_PropertyDefinition>(anIter.Value());
      }
      if (!aPD.IsNull())
      {
        anIter = aGraph.Sharings(aPD);
        occ::handle<StepShape_ShapeDefinitionRepresentation> aSDR;
        for (; anIter.More() && aSDR.IsNull(); anIter.Next())
        {
          aSDR = occ::down_cast<StepShape_ShapeDefinitionRepresentation>(anIter.Value());
        }
        if (!aSDR.IsNull())
        {
          occ::handle<StepShape_ShapeRepresentationWithParameters> aSRWP =
            occ::down_cast<StepShape_ShapeRepresentationWithParameters>(aSDR->UsedRepresentation());
          if (!aSRWP.IsNull())
          {
            isValidDT = true;
            // Collect parameters of datum target
            for (int j = aSRWP->Items()->Lower(); j <= aSRWP->Items()->Upper(); j++)
            {
              if (aSRWP->ItemsValue(j).IsNull())
                continue;
              if (aSRWP->ItemsValue(j)->IsKind(STANDARD_TYPE(StepGeom_Axis2Placement3d)))
              {
                occ::handle<StepGeom_Axis2Placement3d> anAx =
                  occ::down_cast<StepGeom_Axis2Placement3d>(aSRWP->ItemsValue(j));
                XSAlgo_ShapeProcessor::PrepareForTransfer();
                STEPControl_ActorRead anActor(aTP->Model());
                StepData_Factors      aLocalFactors = theLocalFactors;
                anActor.PrepareUnits(aSRWP, aTP, aLocalFactors);
                occ::handle<Geom_Axis2Placement> anAxis =
                  StepToGeom::MakeAxis2Placement(anAx, aLocalFactors);
                aDatTargetObj->SetDatumTargetAxis(anAxis->Ax2());
              }
              else if (aSRWP->ItemsValue(j)->IsKind(
                         STANDARD_TYPE(StepRepr_ReprItemAndLengthMeasureWithUnit)))
              {
                occ::handle<StepRepr_ReprItemAndLengthMeasureWithUnit> aM =
                  occ::down_cast<StepRepr_ReprItemAndLengthMeasureWithUnit>(aSRWP->ItemsValue(j));
                double         aVal   = aM->GetMeasureWithUnit()->ValueComponent();
                StepBasic_Unit anUnit = aM->GetMeasureWithUnit()->UnitComponent();
                if (anUnit.IsNull())
                  continue;
                occ::handle<StepBasic_NamedUnit> aNU = anUnit.NamedUnit();
                if (aNU.IsNull())
                  continue;
                STEPConstruct_UnitContext anUnitCtx;
                anUnitCtx.ComputeFactors(aNU, theLocalFactors);
                aVal = aVal * anUnitCtx.LengthFactor();
                if (aM->Name()->String().IsEqual("target length")
                    || aM->Name()->String().IsEqual("target diameter"))
                  aDatTargetObj->SetDatumTargetLength(aVal);
                else
                  aDatTargetObj->SetDatumTargetWidth(aVal);
              }
            }
          }
        }
      }
    }

    // Create datum target object
    if (isValidDT)
    {
      TDF_Label aDatL =
        aDGTTool->AddDatum(theDat->Name(), theDat->Description(), theDat->Identification());
      myGDTMap.Bind(aDT, aDatL);
      aDGTTool->Lock(aDatL);
      aDat = XCAFDoc_Datum::Set(aDatL);
      aDGTTool->SetDatum(aDTShapeLabels, aDatL);
      aDatTargetObj->SetName(theDat->Identification());
      aDatTargetObj->SetPosition(thePositionCounter);
      if (!theXCAFModifiers.IsEmpty())
        aDatTargetObj->SetModifiers(theXCAFModifiers);
      if (theXCAFModifWithVal != XCAFDimTolObjects_DatumModifWithValue_None)
        aDatTargetObj->SetModifierWithValue(theXCAFModifWithVal, theModifValue);
      aDGTTool->SetDatumToGeomTol(aDatL, theGDTL);
      aDatTargetObj->IsDatumTarget(true);
      aDatTargetObj->SetDatumTargetNumber(aDT->TargetId()->IntegerValue());
      readAnnotation(aTR, aDT, aDatTargetObj, theLocalFactors);
      aDat->SetObject(aDatTargetObj);
      isExistDatumTarget = true;
    }
  }

  if (aShapeLabels.Length() > 0 || !isExistDatumTarget)
  {
    // Create object for datum
    TDF_Label aDatL =
      aDGTTool->AddDatum(theDat->Name(), theDat->Description(), theDat->Identification());
    myGDTMap.Bind(theDat, aDatL);
    // bind datum label with all reference datum_feature entities
    for (int i = 1; i <= aSAs.Length(); i++)
    {
      occ::handle<StepRepr_ShapeAspect> aSA = aSAs.Value(i);
      if (aSA.IsNull() || aSA->IsKind(STANDARD_TYPE(StepDimTol_DatumTarget)))
        continue;
      myGDTMap.Bind(aSA, aDatL);
    }
    aDGTTool->Lock(aDatL);
    aDat = XCAFDoc_Datum::Set(aDatL);
    aDGTTool->SetDatum(aShapeLabels, aDatL);
    aDatObj->SetName(theDat->Identification());
    aDatObj->SetPosition(thePositionCounter);
    if (!theXCAFModifiers.IsEmpty())
      aDatObj->SetModifiers(theXCAFModifiers);
    if (theXCAFModifWithVal != XCAFDimTolObjects_DatumModifWithValue_None)
      aDatObj->SetModifierWithValue(theXCAFModifWithVal, theModifValue);
    aDGTTool->SetDatumToGeomTol(aDatL, theGDTL);
    if (aDatObj->GetPresentation().IsNull())
    {
      // Try find annotation connected to datum entity (not right case, according recommended
      // practices)
      readAnnotation(aTR, theDat, aDatObj, theLocalFactors);
    }
    aDat->SetObject(aDatObj);
  }

  return true;
}

//=================================================================================================

bool STEPCAFControl_Reader::readDatumsAP242(const occ::handle<Standard_Transient>&    theEnt,
                                            const TDF_Label                           theGDTL,
                                            const occ::handle<TDocStd_Document>&      theDoc,
                                            const occ::handle<XSControl_WorkSession>& theWS,
                                            const StepData_Factors& theLocalFactors)
{
  const occ::handle<XSControl_TransferReader>&  aTR    = theWS->TransferReader();
  const occ::handle<Transfer_TransientProcess>& aTP    = aTR->TransientProcess();
  const Interface_Graph&                        aGraph = aTP->Graph();

  Interface_EntityIterator anIter = aGraph.Shareds(theEnt);
  for (anIter.Start(); anIter.More(); anIter.Next())
  {
    const occ::handle<Standard_Transient>& anAtr = anIter.Value();
    if (anAtr->IsKind(STANDARD_TYPE(StepDimTol_DatumSystem)))
    {
      int                                 aPositionCounter = 0; // position on frame
      occ::handle<StepDimTol_DatumSystem> aDS      = occ::down_cast<StepDimTol_DatumSystem>(anAtr);
      Interface_EntityIterator            anIterDS = aGraph.Sharings(aDS);
      for (anIterDS.Start(); anIterDS.More(); anIterDS.Next())
      {
        const occ::handle<Standard_Transient>& anAtrDS = anIterDS.Value();
        if (anAtrDS->IsKind(STANDARD_TYPE(StepAP242_GeometricItemSpecificUsage)))
        {
          // get axis
          occ::handle<StepAP242_GeometricItemSpecificUsage> aAxGISUI =
            occ::down_cast<StepAP242_GeometricItemSpecificUsage>(anAtrDS);
          if (aAxGISUI->IdentifiedItemValue(1)->IsKind(STANDARD_TYPE(StepGeom_Axis2Placement3d)))
          {
            occ::handle<StepGeom_Axis2Placement3d> anAx =
              occ::down_cast<StepGeom_Axis2Placement3d>(aAxGISUI->IdentifiedItemValue(1));
            occ::handle<XCAFDoc_GeomTolerance> aTol;
            if (theGDTL.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aTol))
            {
              occ::handle<XCAFDimTolObjects_GeomToleranceObject> anObj = aTol->GetObject();
              const occ::handle<Geom_Axis2Placement>             aGeomAx2 =
                StepToGeom::MakeAxis2Placement(anAx, theLocalFactors);
              if (!aGeomAx2.IsNull())
              {
                anObj->SetAxis(aGeomAx2->Ax2());
                aTol->SetObject(anObj);
              }
            }
          }
        }
      }
      if (aDS->NbConstituents() > 0)
      {
        // get datum feature and datum target from datum system
        occ::handle<NCollection_HArray1<occ::handle<StepDimTol_DatumReferenceCompartment>>> aDRCA =
          aDS->Constituents();
        if (!aDRCA.IsNull())
        {
          for (int i = aDRCA->Lower(); i <= aDRCA->Upper(); i++)
          {
            occ::handle<StepDimTol_DatumReferenceCompartment> aDRC = aDRCA->Value(i);
            // gete modifiers
            occ::handle<NCollection_HArray1<StepDimTol_DatumReferenceModifier>> aModif =
              aDRC->Modifiers();
            NCollection_Sequence<XCAFDimTolObjects_DatumSingleModif> aXCAFModifiers;
            XCAFDimTolObjects_DatumModifWithValue                    aXCAFModifWithVal =
              XCAFDimTolObjects_DatumModifWithValue_None;
            double aModifValue = 0;
            if (!aModif.IsNull())
            {
              for (int m = aModif->Lower(); m <= aModif->Upper(); m++)
              {
                if (aModif->Value(m).CaseNumber() == 2)
                  aXCAFModifiers.Append((XCAFDimTolObjects_DatumSingleModif)aModif->Value(m)
                                          .SimpleDatumReferenceModifierMember()
                                          ->Value());
                else if (aModif->Value(m).CaseNumber() == 1)
                {
                  aXCAFModifWithVal =
                    (XCAFDimTolObjects_DatumModifWithValue)(aModif->Value(m)
                                                              .DatumReferenceModifierWithValue()
                                                              ->ModifierType()
                                                            + 1);
                  double aVal = aModif->Value(m)
                                  .DatumReferenceModifierWithValue()
                                  ->ModifierValue()
                                  ->ValueComponent();
                  StepBasic_Unit anUnit = aModif->Value(m)
                                            .DatumReferenceModifierWithValue()
                                            ->ModifierValue()
                                            ->UnitComponent();
                  if (anUnit.IsNull())
                    continue;
                  if (!(anUnit.CaseNum(anUnit.Value()) == 1))
                    continue;
                  occ::handle<StepBasic_NamedUnit> NU = anUnit.NamedUnit();
                  STEPConstruct_UnitContext        anUnitCtx;
                  anUnitCtx.ComputeFactors(NU, theLocalFactors);
                  aModifValue = aVal * anUnitCtx.LengthFactor();
                }
              }
            }
            aPositionCounter++;
            Interface_EntityIterator anIterDRC = aGraph.Shareds(aDRC);
            for (anIterDRC.Start(); anIterDRC.More(); anIterDRC.Next())
            {

              if (anIterDRC.Value()->IsKind(STANDARD_TYPE(StepDimTol_Datum)))
              {
                occ::handle<StepDimTol_Datum> aD =
                  occ::down_cast<StepDimTol_Datum>(anIterDRC.Value());
                setDatumToXCAF(aD,
                               theGDTL,
                               aPositionCounter,
                               aXCAFModifiers,
                               aXCAFModifWithVal,
                               aModifValue,
                               theDoc,
                               theWS,
                               theLocalFactors);
              }
              else if (anIterDRC.Value()->IsKind(STANDARD_TYPE(StepDimTol_DatumReferenceElement)))
              {
                occ::handle<StepDimTol_DatumReferenceElement> aDRE =
                  occ::down_cast<StepDimTol_DatumReferenceElement>(anIterDRC.Value());
                // get modifiers from group of datums
                occ::handle<NCollection_HArray1<StepDimTol_DatumReferenceModifier>> aModifE =
                  aDRE->Modifiers();
                if (!aModifE.IsNull())
                {
                  for (int k = aModifE->Lower(); k <= aModifE->Upper(); k++)
                  {
                    if (aModifE->Value(k).CaseNumber() == 2)
                      aXCAFModifiers.Append((XCAFDimTolObjects_DatumSingleModif)aModifE->Value(k)
                                              .SimpleDatumReferenceModifierMember()
                                              ->Value());
                    else if (aModifE->Value(k).CaseNumber() == 1)
                    {
                      aXCAFModifWithVal =
                        (XCAFDimTolObjects_DatumModifWithValue)(aModifE->Value(k)
                                                                  .DatumReferenceModifierWithValue()
                                                                  ->ModifierType()
                                                                + 1);
                      double aVal = aModifE->Value(k)
                                      .DatumReferenceModifierWithValue()
                                      ->ModifierValue()
                                      ->ValueComponent();
                      StepBasic_Unit anUnit = aModifE->Value(k)
                                                .DatumReferenceModifierWithValue()
                                                ->ModifierValue()
                                                ->UnitComponent();
                      if (anUnit.IsNull())
                        continue;
                      if (!(anUnit.CaseNum(anUnit.Value()) == 1))
                        continue;
                      occ::handle<StepBasic_NamedUnit> NU = anUnit.NamedUnit();
                      STEPConstruct_UnitContext        anUnitCtx;
                      anUnitCtx.ComputeFactors(NU, theLocalFactors);
                      aModifValue = aVal * anUnitCtx.LengthFactor();
                    }
                  }
                }
                Interface_EntityIterator anIterDRE = aGraph.Shareds(aDRE);
                for (anIterDRE.Start(); anIterDRE.More(); anIterDRE.Next())
                {
                  if (anIterDRE.Value()->IsKind(STANDARD_TYPE(StepDimTol_Datum)))
                  {
                    occ::handle<StepDimTol_Datum> aD =
                      occ::down_cast<StepDimTol_Datum>(anIterDRE.Value());
                    setDatumToXCAF(aD,
                                   theGDTL,
                                   aPositionCounter,
                                   aXCAFModifiers,
                                   aXCAFModifWithVal,
                                   aModifValue,
                                   theDoc,
                                   theWS,
                                   theLocalFactors);
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return true;
}

//=================================================================================================

TDF_Label STEPCAFControl_Reader::createGDTObjectInXCAF(
  const occ::handle<Standard_Transient>&    theEnt,
  const occ::handle<TDocStd_Document>&      theDoc,
  const occ::handle<XSControl_WorkSession>& theWS,
  const StepData_Factors&                   theLocalFactors)
{
  TDF_Label aGDTL;
  if (!theEnt->IsKind(STANDARD_TYPE(StepShape_DimensionalSize))
      && !theEnt->IsKind(STANDARD_TYPE(StepShape_DimensionalLocation))
      && !theEnt->IsKind(STANDARD_TYPE(StepDimTol_GeometricTolerance)))
  {
    return aGDTL;
  }

  occ::handle<TCollection_HAsciiString> aSemanticName;

  // protection against invalid input
  if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_GeometricTolerance)))
  {
    occ::handle<StepDimTol_GeometricTolerance> aGeomTol =
      occ::down_cast<StepDimTol_GeometricTolerance>(theEnt);
    if (aGeomTol->TolerancedShapeAspect().IsNull())
      return aGDTL;
    aSemanticName = aGeomTol->Name();
  }
  if (theEnt->IsKind(STANDARD_TYPE(StepShape_DimensionalSize)))
  {
    occ::handle<StepShape_DimensionalSize> aDim = occ::down_cast<StepShape_DimensionalSize>(theEnt);
    if (aDim->AppliesTo().IsNull())
      return aGDTL;
    aSemanticName = aDim->Name();
  }
  if (theEnt->IsKind(STANDARD_TYPE(StepShape_DimensionalLocation)))
  {
    occ::handle<StepShape_DimensionalLocation> aDim =
      occ::down_cast<StepShape_DimensionalLocation>(theEnt);
    if (aDim->RelatedShapeAspect().IsNull() || aDim->RelatingShapeAspect().IsNull())
      return aGDTL;
    aSemanticName = aDim->Name();
  }

  occ::handle<XCAFDoc_ShapeTool>  aSTool   = XCAFDoc_DocumentTool::ShapeTool(theDoc->Main());
  occ::handle<XCAFDoc_DimTolTool> aDGTTool = XCAFDoc_DocumentTool::DimTolTool(theDoc->Main());
  const occ::handle<XSControl_TransferReader>&  aTR         = theWS->TransferReader();
  const occ::handle<Transfer_TransientProcess>& aTP         = aTR->TransientProcess();
  const Interface_Graph&                        aGraph      = aTP->Graph();
  bool                                          isAllAround = false;
  bool                                          isAllOver   = false;

  // find RepresentationItem for current Ent
  NCollection_Sequence<occ::handle<Standard_Transient>> aSeqRI1, aSeqRI2;

  // Collect all Shape_Aspect entities
  Interface_EntityIterator anIter = aGraph.Shareds(theEnt);
  for (anIter.Start(); anIter.More(); anIter.Next())
  {
    const occ::handle<Standard_Transient>&                  anAtr = anIter.Value();
    NCollection_Sequence<occ::handle<StepRepr_ShapeAspect>> aSAs;
    if (anAtr->IsKind(STANDARD_TYPE(StepRepr_ProductDefinitionShape)))
    {
      // if associating tolerances with part (All-Over)
      Interface_EntityIterator anIterSDR = aGraph.Sharings(anAtr);
      for (anIterSDR.Start(); anIterSDR.More(); anIterSDR.Next())
      {
        const occ::handle<Standard_Transient>& anAtrSDR = anIterSDR.Value();
        if (anAtrSDR->IsKind(STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation)))
        {
          isAllOver                           = true;
          Interface_EntityIterator anIterABSR = aGraph.Shareds(anAtrSDR);
          for (anIterABSR.Start(); anIterABSR.More(); anIterABSR.Next())
          {
            const occ::handle<Standard_Transient>& anAtrABSR = anIterABSR.Value();
            if (anAtrABSR->IsKind(STANDARD_TYPE(StepShape_AdvancedBrepShapeRepresentation)))
            {
              aSeqRI1.Append(anAtrABSR);
            }
          }
        }
      }
    }
    else if (anAtr->IsKind(STANDARD_TYPE(StepShape_DimensionalLocation))
             || anAtr->IsKind(STANDARD_TYPE(StepShape_DimensionalSize)))
    {
      // if tolerance attached to dimension
      Interface_EntityIterator anIterDim = aGraph.Shareds(anAtr);
      for (anIterDim.Start(); anIterDim.More(); anIterDim.Next())
      {
        occ::handle<StepRepr_ShapeAspect> aSA =
          occ::down_cast<StepRepr_ShapeAspect>(anIterDim.Value());
        if (!aSA.IsNull())
        {
          collectShapeAspect(aSA, theWS, aSAs);
        }
      }
    }
    else if (anAtr->IsKind(STANDARD_TYPE(StepRepr_ShapeAspect)))
    {
      if (anAtr->IsKind(STANDARD_TYPE(StepRepr_AllAroundShapeAspect)))
      {
        // if applied AllAround Modifier
        isAllAround = true;
      }
      // dimensions and default tolerances
      occ::handle<StepRepr_ShapeAspect> aSA = occ::down_cast<StepRepr_ShapeAspect>(anAtr);
      if (!aSA.IsNull())
      {
        collectShapeAspect(aSA, theWS, aSAs);
      }
    }

    // Collect all representation items
    if (!aSAs.IsEmpty())
    {
      // get representation items
      NCollection_Sequence<occ::handle<Standard_Transient>> aSeqRI;
      for (int i = NCollection_Sequence<opencascade::handle<StepRepr_ShapeAspect>>::Lower();
           i <= aSAs.Upper();
           i++)
      {
        Interface_EntityIterator                          anIterSA = aGraph.Sharings(aSAs.Value(i));
        occ::handle<StepAP242_GeometricItemSpecificUsage> aGISU;
        occ::handle<StepRepr_PropertyDefinition>          PropD;
        for (anIterSA.Start(); anIterSA.More() && aGISU.IsNull() && PropD.IsNull(); anIterSA.Next())
        {
          aGISU = occ::down_cast<StepAP242_GeometricItemSpecificUsage>(anIterSA.Value());
          PropD = occ::down_cast<StepRepr_PropertyDefinition>(anIterSA.Value());
        }
        if (!PropD.IsNull()) // for old version
        {
          occ::handle<StepRepr_RepresentationItem> RI;
          Interface_EntityIterator                 subs4 = aGraph.Sharings(PropD);
          for (subs4.Start(); subs4.More(); subs4.Next())
          {
            occ::handle<StepShape_ShapeDefinitionRepresentation> SDR =
              occ::down_cast<StepShape_ShapeDefinitionRepresentation>(subs4.Value());
            if (!SDR.IsNull())
            {
              occ::handle<StepRepr_Representation> Repr = SDR->UsedRepresentation();
              if (!Repr.IsNull() && Repr->NbItems() > 0)
              {
                RI = Repr->ItemsValue(1);
              }
            }
          }
          if (RI.IsNull())
            continue;

          if (theEnt->IsKind(STANDARD_TYPE(StepShape_DimensionalSize)))
          {
            // read dimensions
            occ::handle<StepShape_EdgeCurve> EC = occ::down_cast<StepShape_EdgeCurve>(RI);
            if (EC.IsNull())
              continue;
            occ::handle<TCollection_HAsciiString>  aName;
            occ::handle<StepShape_DimensionalSize> DimSize =
              occ::down_cast<StepShape_DimensionalSize>(theEnt);
            double dim1 = -1., dim2 = -1.;
            subs4 = aGraph.Sharings(DimSize);
            for (subs4.Start(); subs4.More(); subs4.Next())
            {
              occ::handle<StepShape_DimensionalCharacteristicRepresentation> DimCharR =
                occ::down_cast<StepShape_DimensionalCharacteristicRepresentation>(subs4.Value());
              if (!DimCharR.IsNull())
              {
                occ::handle<StepShape_ShapeDimensionRepresentation> SDimR =
                  DimCharR->Representation();
                if (!SDimR.IsNull() && SDimR->NbItems() > 0)
                {
                  occ::handle<StepRepr_RepresentationItem> anItem = SDimR->ItemsValue(1);
                  occ::handle<StepRepr_ValueRange> VR = occ::down_cast<StepRepr_ValueRange>(anItem);
                  if (!VR.IsNull())
                  {
                    aName = VR->Name();
                    // StepRepr_CompoundItemDefinition CID = VR->ItemElement();
                    // if(CID.IsNull()) continue;
                    // occ::handle<StepRepr_CompoundItemDefinitionMember> CIDM =
                    //   occ::down_cast<StepRepr_CompoundItemDefinitionMember>(CID.Value());
                    // if(CIDM.IsNull()) continue;
                    // if(CIDM->ArrTransient().IsNull()) continue;
                    // occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>>
                    // HARI; if(CID.CaseMem(CIDM)==1)
                    //   HARI = CID.ListRepresentationItem();
                    // if(CID.CaseMem(CIDM)==2)
                    //   HARI = CID.SetRepresentationItem();
                    occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>>
                      HARI = VR->ItemElement();
                    if (HARI.IsNull())
                      continue;
                    if (HARI->Length() > 0)
                    {
                      occ::handle<StepRepr_RepresentationItem> RI1 = HARI->Value(1);
                      if (RI1.IsNull())
                        continue;
                      if (RI1->IsKind(STANDARD_TYPE(StepRepr_ReprItemAndLengthMeasureWithUnit)))
                      {
                        occ::handle<StepRepr_ReprItemAndLengthMeasureWithUnit> RILMWU =
                          occ::down_cast<StepRepr_ReprItemAndLengthMeasureWithUnit>(RI1);
                        dim1                  = RILMWU->GetMeasureWithUnit()->ValueComponent();
                        StepBasic_Unit anUnit = RILMWU->GetMeasureWithUnit()->UnitComponent();
                        if (anUnit.IsNull())
                          continue;
                        if (!(anUnit.CaseNum(anUnit.Value()) == 1))
                          continue;
                        occ::handle<StepBasic_NamedUnit> NU = anUnit.NamedUnit();
                        STEPConstruct_UnitContext        anUnitCtx;
                        anUnitCtx.ComputeFactors(NU, theLocalFactors);
                        dim1 = dim1 * anUnitCtx.LengthFactor();
                      }
                    }
                    if (HARI->Length() > 1)
                    {
                      occ::handle<StepRepr_RepresentationItem> RI2 = HARI->Value(2);
                      if (RI2.IsNull())
                        continue;
                      if (RI2->IsKind(STANDARD_TYPE(StepRepr_ReprItemAndLengthMeasureWithUnit)))
                      {
                        occ::handle<StepRepr_ReprItemAndLengthMeasureWithUnit> RILMWU =
                          occ::down_cast<StepRepr_ReprItemAndLengthMeasureWithUnit>(RI2);
                        dim2                  = RILMWU->GetMeasureWithUnit()->ValueComponent();
                        StepBasic_Unit anUnit = RILMWU->GetMeasureWithUnit()->UnitComponent();
                        if (anUnit.IsNull())
                          continue;
                        if (!(anUnit.CaseNum(anUnit.Value()) == 1))
                          continue;
                        occ::handle<StepBasic_NamedUnit> NU = anUnit.NamedUnit();
                        STEPConstruct_UnitContext        anUnitCtx;
                        anUnitCtx.ComputeFactors(NU, theLocalFactors);
                        dim2 = dim2 * anUnitCtx.LengthFactor();
                      }
                    }
                  }
                }
              }
            }
            if (dim1 < 0)
              continue;
            if (dim2 < 0)
              dim2 = dim1;
            // std::cout<<"DimensionalSize: dim1="<<dim1<<"  dim2="<<dim2<<std::endl;
            //  now we know edge_curve and value range therefore
            //  we can create corresponding D&GT labels
            int          index = aTP->MapIndex(EC);
            TopoDS_Shape aSh;
            if (index > 0)
            {
              occ::handle<Transfer_Binder> binder = aTP->MapItem(index);
              aSh                                 = TransferBRep::ShapeResult(binder);
            }
            if (aSh.IsNull())
              continue;
            TDF_Label shL;
            if (!aSTool->Search(aSh, shL, true, true, true))
              continue;
            occ::handle<NCollection_HArray1<double>> arr = new NCollection_HArray1<double>(1, 2);
            arr->SetValue(1, dim1);
            arr->SetValue(2, dim2);
            aDGTTool->SetDimTol(shL, 1, arr, aName, DimSize->Name());
          }
          // read tolerances and datums
          else if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_GeometricTolerance)))
          {
            occ::handle<StepDimTol_GeometricTolerance> GT =
              occ::down_cast<StepDimTol_GeometricTolerance>(theEnt);
            // read common data for tolerance
            occ::handle<StepBasic_MeasureWithUnit> dim3 = GetMeasureWithUnit(GT->Magnitude());
            if (dim3.IsNull())
              continue;
            double         dim    = dim3->ValueComponent();
            StepBasic_Unit anUnit = dim3->UnitComponent();
            if (anUnit.IsNull())
              continue;
            if (!(anUnit.CaseNum(anUnit.Value()) == 1))
              continue;
            occ::handle<StepBasic_NamedUnit> NU = anUnit.NamedUnit();
            STEPConstruct_UnitContext        anUnitCtx;
            anUnitCtx.ComputeFactors(NU, theLocalFactors);
            dim = dim * anUnitCtx.LengthFactor();
            // std::cout<<"GeometricTolerance: Magnitude = "<<dim<<std::endl;
            occ::handle<NCollection_HArray1<double>> arr = new NCollection_HArray1<double>(1, 1);
            arr->SetValue(1, dim);
            occ::handle<TCollection_HAsciiString> aName        = GT->Name();
            occ::handle<TCollection_HAsciiString> aDescription = GT->Description();
            occ::handle<StepShape_AdvancedFace>   AF = occ::down_cast<StepShape_AdvancedFace>(RI);
            if (AF.IsNull())
              continue;
            int          index = aTP->MapIndex(AF);
            TopoDS_Shape aSh;
            if (index > 0)
            {
              occ::handle<Transfer_Binder> binder = aTP->MapItem(index);
              aSh                                 = TransferBRep::ShapeResult(binder);
            }
            if (aSh.IsNull())
              continue;
            TDF_Label shL;
            if (!aSTool->Search(aSh, shL, true, true, true))
              continue;
            // read specific data for tolerance
            if (GT->IsKind(STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol)))
            {
              occ::handle<StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol> GTComplex =
                occ::down_cast<StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol>(theEnt);
              int                                                kind = 20;
              occ::handle<StepDimTol_ModifiedGeometricTolerance> MGT =
                GTComplex->GetModifiedGeometricTolerance();
              if (!MGT.IsNull())
              {
                kind = kind + MGT->Modifier() + 1;
              }
              TDF_Label TolerL = aDGTTool->SetDimTol(shL, kind, arr, aName, aDescription);
              // translate datums connected with this tolerance
              occ::handle<StepDimTol_GeometricToleranceWithDatumReference> GTWDR =
                GTComplex->GetGeometricToleranceWithDatumReference();
              if (!GTWDR.IsNull())
              {
                ReadDatums(aSTool, aDGTTool, aGraph, aTP, TolerL, GTWDR);
              }
            }
            else if (GT->IsKind(STANDARD_TYPE(StepDimTol_GeometricToleranceWithDatumReference)))
            {
              occ::handle<StepDimTol_GeometricToleranceWithDatumReference> GTWDR =
                occ::down_cast<StepDimTol_GeometricToleranceWithDatumReference>(theEnt);
              if (GTWDR.IsNull())
                continue;
              int kind = 0;
              if (GTWDR->IsKind(STANDARD_TYPE(StepDimTol_AngularityTolerance)))
                kind = 24;
              else if (GTWDR->IsKind(STANDARD_TYPE(StepDimTol_CircularRunoutTolerance)))
                kind = 25;
              else if (GTWDR->IsKind(STANDARD_TYPE(StepDimTol_CoaxialityTolerance)))
                kind = 26;
              else if (GTWDR->IsKind(STANDARD_TYPE(StepDimTol_ConcentricityTolerance)))
                kind = 27;
              else if (GTWDR->IsKind(STANDARD_TYPE(StepDimTol_ParallelismTolerance)))
                kind = 28;
              else if (GTWDR->IsKind(STANDARD_TYPE(StepDimTol_PerpendicularityTolerance)))
                kind = 29;
              else if (GTWDR->IsKind(STANDARD_TYPE(StepDimTol_SymmetryTolerance)))
                kind = 30;
              else if (GTWDR->IsKind(STANDARD_TYPE(StepDimTol_TotalRunoutTolerance)))
                kind = 31;
              // std::cout<<"GTWDR: kind="<<kind<<std::endl;
              TDF_Label TolerL = aDGTTool->SetDimTol(shL, kind, arr, aName, aDescription);
              ReadDatums(aSTool, aDGTTool, aGraph, aTP, TolerL, GTWDR);
            }
            else if (GT->IsKind(STANDARD_TYPE(StepDimTol_ModifiedGeometricTolerance)))
            {
              occ::handle<StepDimTol_ModifiedGeometricTolerance> MGT =
                occ::down_cast<StepDimTol_ModifiedGeometricTolerance>(theEnt);
              int kind = 35 + MGT->Modifier();
              aDGTTool->SetDimTol(shL, kind, arr, aName, aDescription);
            }
            else if (GT->IsKind(STANDARD_TYPE(StepDimTol_CylindricityTolerance)))
            {
              aDGTTool->SetDimTol(shL, 38, arr, aName, aDescription);
            }
            else if (GT->IsKind(STANDARD_TYPE(StepDimTol_FlatnessTolerance)))
            {
              aDGTTool->SetDimTol(shL, 39, arr, aName, aDescription);
            }
            else if (GT->IsKind(STANDARD_TYPE(StepDimTol_LineProfileTolerance)))
            {
              aDGTTool->SetDimTol(shL, 40, arr, aName, aDescription);
            }
            else if (GT->IsKind(STANDARD_TYPE(StepDimTol_PositionTolerance)))
            {
              aDGTTool->SetDimTol(shL, 41, arr, aName, aDescription);
            }
            else if (GT->IsKind(STANDARD_TYPE(StepDimTol_RoundnessTolerance)))
            {
              aDGTTool->SetDimTol(shL, 42, arr, aName, aDescription);
            }
            else if (GT->IsKind(STANDARD_TYPE(StepDimTol_StraightnessTolerance)))
            {
              aDGTTool->SetDimTol(shL, 43, arr, aName, aDescription);
            }
            else if (GT->IsKind(STANDARD_TYPE(StepDimTol_SurfaceProfileTolerance)))
            {
              aDGTTool->SetDimTol(shL, 44, arr, aName, aDescription);
            }
          }
        }
        else
        {
          if (aGISU.IsNull())
            continue;
          int j = 1;
          for (; j <= aGISU->NbIdentifiedItem(); j++)
          {
            aSeqRI.Append(aGISU->IdentifiedItemValue(j));
          }
        }
      }
      if (!aSeqRI.IsEmpty())
      {
        if (aSeqRI1.IsEmpty())
          aSeqRI1 = aSeqRI;
        else
          aSeqRI2 = aSeqRI;
      }
    }
  }
  if (aSeqRI1.IsEmpty())
    return aGDTL;

  NCollection_Sequence<TDF_Label> aShLS1, aShLS2;

  // Collect shapes
  for (int i = NCollection_Sequence<opencascade::handle<Standard_Transient>>::Lower();
       i <= aSeqRI1.Upper();
       i++)
  {
    int          anIndex = FindShapeIndexForDGT(aSeqRI1.Value(i), theWS);
    TopoDS_Shape aSh;
    if (anIndex > 0)
    {
      occ::handle<Transfer_Binder> aBinder = aTP->MapItem(anIndex);
      aSh                                  = TransferBRep::ShapeResult(aBinder);
    }
    if (!aSh.IsNull())
    {
      TDF_Label aShL;
      aSTool->Search(aSh, aShL, true, true, true);
      if (aShL.IsNull() && aSh.ShapeType() == TopAbs_WIRE)
      {
        TopExp_Explorer ex(aSh, TopAbs_EDGE, TopAbs_SHAPE);
        while (ex.More())
        {
          TDF_Label edgeL;
          aSTool->Search(ex.Current(), edgeL, true, true, true);
          if (!edgeL.IsNull())
            aShLS1.Append(edgeL);
          ex.Next();
        }
      }
      if (!aShL.IsNull())
        aShLS1.Append(aShL);
    }
  }
  if (!aSeqRI2.IsEmpty())
  {
    // for dimensional location
    for (int i = NCollection_Sequence<opencascade::handle<Standard_Transient>>::Lower();
         i <= aSeqRI2.Upper();
         i++)
    {
      int          anIndex = FindShapeIndexForDGT(aSeqRI2.Value(i), theWS);
      TopoDS_Shape aSh;
      if (anIndex > 0)
      {
        occ::handle<Transfer_Binder> aBinder = aTP->MapItem(anIndex);
        aSh                                  = TransferBRep::ShapeResult(aBinder);
      }
      if (!aSh.IsNull())
      {
        TDF_Label aShL;
        aSTool->Search(aSh, aShL, true, true, true);
        if (aShL.IsNull() && aSh.ShapeType() == TopAbs_WIRE)
        {
          TopExp_Explorer ex(aSh, TopAbs_EDGE, TopAbs_SHAPE);
          while (ex.More())
          {
            TDF_Label edgeL;
            aSTool->Search(ex.Current(), edgeL, true, true, true);
            if (!edgeL.IsNull())
              aShLS2.Append(edgeL);
            ex.Next();
          }
        }
        if (!aShL.IsNull())
          aShLS2.Append(aShL);
      }
    }
  }

  if (!aShLS1.IsEmpty())
  {
    // add to XCAF
    if (!theEnt->IsKind(STANDARD_TYPE(StepDimTol_GeometricTolerance)))
    {
      aGDTL = aDGTTool->AddDimension();
      myGDTMap.Bind(theEnt, aGDTL);
      aDGTTool->Lock(aGDTL);
      occ::handle<XCAFDoc_Dimension> aDim = XCAFDoc_Dimension::Set(aGDTL);
      TCollection_AsciiString        aStr("DGT:Dimensional_");
      if (theEnt->IsKind(STANDARD_TYPE(StepShape_DimensionalSize)))
      {
        aStr.AssignCat("Size");
      }
      else if (theEnt->IsKind(STANDARD_TYPE(StepShape_DimensionalLocation)))
      {
        aStr.AssignCat("Location");
      }
      TDataStd_Name::Set(aGDTL, aStr);

      if (!aShLS2.IsEmpty())
      {
        aDGTTool->SetDimension(aShLS1, aShLS2, aGDTL);
      }
      else
      {
        NCollection_Sequence<TDF_Label> aEmptySeq;
        aDGTTool->SetDimension(aShLS1, aEmptySeq, aGDTL);
      }
    }
    else
    {
      aGDTL = aDGTTool->AddGeomTolerance();
      myGDTMap.Bind(theEnt, aGDTL);
      aDGTTool->Lock(aGDTL);
      occ::handle<XCAFDoc_GeomTolerance> aGTol = XCAFDoc_GeomTolerance::Set(aGDTL);
      TCollection_AsciiString            aStr("DGT:GeomTolerance");
      TDataStd_Name::Set(aGDTL, aStr);
      aDGTTool->SetGeomTolerance(aShLS1, aGDTL);
      occ::handle<XCAFDimTolObjects_GeomToleranceObject> anObj = aGTol->GetObject();
      if (isAllAround)
        anObj->AddModifier(XCAFDimTolObjects_GeomToleranceModif_All_Around);
      else if (isAllOver)
        anObj->AddModifier(XCAFDimTolObjects_GeomToleranceModif_All_Over);
      aGTol->SetObject(anObj);
    }

    if (aSemanticName)
    {
      TCollection_ExtendedString str(aSemanticName->String());
      TDataStd_Name::Set(aGDTL, str);
    }

    readDatumsAP242(theEnt, aGDTL, theDoc, theWS, theLocalFactors);
  }
  return aGDTL;
}

//=================================================================================================

void convertAngleValue(const STEPConstruct_UnitContext& anUnitCtx, double& aVal)
{
  // convert radian to deg
  double aFact = anUnitCtx.PlaneAngleFactor() * 180 / M_PI;
  // in order to avoid inaccuracy of calculation perform conversion
  // only if aFact not equal 1 with some precision
  if (fabs(1. - aFact) > Precision::Confusion())
  {
    aVal = aVal * aFact;
  }
}

//=================================================================================================

static void setDimObjectToXCAF(const occ::handle<Standard_Transient>&    theEnt,
                               const TDF_Label&                          aDimL,
                               const occ::handle<TDocStd_Document>&      theDoc,
                               const occ::handle<XSControl_WorkSession>& theWS,
                               const StepData_Factors&                   theLocalFactors)
{
  occ::handle<XCAFDoc_ShapeTool>  aSTool   = XCAFDoc_DocumentTool::ShapeTool(theDoc->Main());
  occ::handle<XCAFDoc_DimTolTool> aDGTTool = XCAFDoc_DocumentTool::DimTolTool(theDoc->Main());
  const occ::handle<XSControl_TransferReader>&   aTR    = theWS->TransferReader();
  const occ::handle<Transfer_TransientProcess>&  aTP    = aTR->TransientProcess();
  const Interface_Graph&                         aGraph = aTP->Graph();
  occ::handle<XCAFDimTolObjects_DimensionObject> aDimObj;
  if (!theEnt->IsKind(STANDARD_TYPE(StepShape_DimensionalSize))
      && !theEnt->IsKind(STANDARD_TYPE(StepShape_DimensionalLocation)))
    return;

  occ::handle<StepShape_DimensionalSize> aDimSize =
    occ::down_cast<StepShape_DimensionalSize>(theEnt);
  occ::handle<StepShape_DimensionalLocation> aDimLocation =
    occ::down_cast<StepShape_DimensionalLocation>(theEnt);

  aDimObj                                                = new XCAFDimTolObjects_DimensionObject();
  double                                           aDim1 = -1., aDim2 = -1., aDim3 = -1.;
  bool                                             isPlusMinusTolerance = false;
  occ::handle<StepShape_TypeQualifier>             aTQ;
  occ::handle<StepShape_ValueFormatTypeQualifier>  aVFTQ;
  occ::handle<StepShape_ToleranceValue>            aTV;
  occ::handle<StepShape_LimitsAndFits>             aLAF;
  occ::handle<StepRepr_CompoundRepresentationItem> aCRI;
  occ::handle<StepGeom_Axis2Placement3d>           anAP;

  Interface_EntityIterator anIterDim;
  if (!aDimSize.IsNull())
  {
    anIterDim = aGraph.Sharings(aDimSize);
  }
  else
  {
    anIterDim = aGraph.Sharings(aDimLocation);
  }
  for (anIterDim.Start(); anIterDim.More(); anIterDim.Next())
  {
    occ::handle<StepShape_DimensionalCharacteristicRepresentation> aDCR =
      occ::down_cast<StepShape_DimensionalCharacteristicRepresentation>(anIterDim.Value());
    occ::handle<StepShape_PlusMinusTolerance> aPMT =
      occ::down_cast<StepShape_PlusMinusTolerance>(anIterDim.Value());
    if (!aDCR.IsNull())
    {
      occ::handle<StepShape_ShapeDimensionRepresentation> aSDR = aDCR->Representation();
      if (!aSDR.IsNull())
      {
        occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> aHARI =
          aSDR->Items();

        if (!aHARI.IsNull())
        {
          for (int nr = aHARI->Lower(); nr <= aHARI->Upper(); nr++)
          {
            occ::handle<StepRepr_RepresentationItem> aDRI = aHARI->Value(nr);
            if (aDRI.IsNull())
              continue;

            if (aDRI->IsKind(STANDARD_TYPE(StepRepr_ReprItemAndMeasureWithUnit)))
            {
              // simple value / range
              occ::handle<StepRepr_ReprItemAndMeasureWithUnit> aMWU =
                occ::down_cast<StepRepr_ReprItemAndMeasureWithUnit>(aDRI);
              double         aVal   = aMWU->GetMeasureWithUnit()->ValueComponent();
              StepBasic_Unit anUnit = aMWU->GetMeasureWithUnit()->UnitComponent();
              if (anUnit.IsNull())
                continue;
              if (!(anUnit.CaseNum(anUnit.Value()) == 1))
                continue;
              occ::handle<StepBasic_NamedUnit> NU = anUnit.NamedUnit();
              STEPConstruct_UnitContext        anUnitCtx;
              anUnitCtx.ComputeFactors(NU, theLocalFactors);
              if (aMWU->IsKind(STANDARD_TYPE(StepRepr_ReprItemAndLengthMeasureWithUnit)))
              {
                aVal = aVal * anUnitCtx.LengthFactor();
              }
              else if (aMWU->IsKind(STANDARD_TYPE(StepRepr_ReprItemAndPlaneAngleMeasureWithUnit)))
              {
                convertAngleValue(anUnitCtx, aVal);
              }
              occ::handle<TCollection_HAsciiString> aName = aMWU->Name();
              if (aName->Search("upper") > 0) // upper limit
                aDim2 = aVal;
              else // lower limit or simple nominal value
                aDim1 = aVal;
            }
            else if (aDRI->IsKind(STANDARD_TYPE(StepRepr_ReprItemAndMeasureWithUnitAndQRI)))
            {
              // value with qualifier (minimum/maximum/average)
              occ::handle<StepRepr_ReprItemAndMeasureWithUnitAndQRI> aMWU =
                occ::down_cast<StepRepr_ReprItemAndMeasureWithUnitAndQRI>(aDRI);
              double         aVal   = aMWU->GetMeasureWithUnit()->ValueComponent();
              StepBasic_Unit anUnit = aMWU->GetMeasureWithUnit()->UnitComponent();
              if (anUnit.IsNull())
                continue;
              if (!(anUnit.CaseNum(anUnit.Value()) == 1))
                continue;
              occ::handle<StepBasic_NamedUnit> NU = anUnit.NamedUnit();
              STEPConstruct_UnitContext        anUnitCtx;
              anUnitCtx.ComputeFactors(NU, theLocalFactors);
              if (aMWU->IsKind(STANDARD_TYPE(StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI)))
              {
                aVal = aVal * anUnitCtx.LengthFactor();
              }
              else if (aMWU->IsKind(
                         STANDARD_TYPE(StepRepr_ReprItemAndPlaneAngleMeasureWithUnitAndQRI)))
              {
                convertAngleValue(anUnitCtx, aVal);
              }
              occ::handle<StepShape_QualifiedRepresentationItem> aQRI =
                aMWU->GetQualifiedRepresentationItem();
              if (aQRI->Qualifiers()->Length() == 0)
              {
                aDim1 = aVal;
                continue;
              }
              occ::handle<StepShape_TypeQualifier> aValueType =
                aQRI->Qualifiers()->Value(1).TypeQualifier();
              if (aValueType->Name()->String().IsEqual("minimum"))
                aDim2 = aVal;
              else if (aValueType->Name()->String().IsEqual("maximum"))
                aDim3 = aVal;
              else
                aDim1 = aVal;
            }
            else if (aDRI->IsKind(STANDARD_TYPE(StepShape_QualifiedRepresentationItem)))
            {
              // get qualifier
              occ::handle<StepShape_QualifiedRepresentationItem> aQRI =
                occ::down_cast<StepShape_QualifiedRepresentationItem>(aDRI);
              for (int l = 1; l <= aQRI->NbQualifiers(); l++)
              {
                aTQ   = aQRI->Qualifiers()->Value(l).TypeQualifier();
                aVFTQ = aQRI->Qualifiers()->Value(l).ValueFormatTypeQualifier();
              }
            }
            else if (aDRI->IsKind(STANDARD_TYPE(StepRepr_DescriptiveRepresentationItem)))
            {
              occ::handle<StepRepr_DescriptiveRepresentationItem> aDescription =
                occ::down_cast<StepRepr_DescriptiveRepresentationItem>(aDRI);
              aDimObj->AddDescription(aDescription->Description(), aDescription->Name());
            }
            else if (aDRI->IsKind(STANDARD_TYPE(StepRepr_CompoundRepresentationItem)))
            {
              aCRI = occ::down_cast<StepRepr_CompoundRepresentationItem>(aDRI);
            }
            else if (aDRI->IsKind(STANDARD_TYPE(StepGeom_Axis2Placement3d)))
            {
              anAP = occ::down_cast<StepGeom_Axis2Placement3d>(aDRI);
            }
          }
        }
      }
    }
    else if (!aPMT.IsNull())
    {
      isPlusMinusTolerance                     = true;
      StepShape_ToleranceMethodDefinition aTMD = aPMT->Range();
      if (aPMT.IsNull())
        continue;
      if (aTMD.CaseNumber() == 1)
      //! 1 -> ToleranceValue from StepShape
      //! 2 -> LimitsAndFits from StepShape
      {
        // plus minus tolerance
        aTV = aTMD.ToleranceValue();
        if (aTV.IsNull())
          continue;

        occ::handle<Standard_Transient> aUpperBound = aTV->UpperBound();
        if (aUpperBound.IsNull())
          continue;
        occ::handle<StepBasic_MeasureWithUnit> aMWU = GetMeasureWithUnit(aUpperBound);
        if (aMWU.IsNull())
          continue;
        double         aVal   = aMWU->ValueComponent();
        StepBasic_Unit anUnit = aMWU->UnitComponent();
        if (anUnit.IsNull())
          continue;
        if (!(anUnit.CaseNum(anUnit.Value()) == 1))
          continue;
        occ::handle<StepBasic_NamedUnit> NU = anUnit.NamedUnit();
        STEPConstruct_UnitContext        anUnitCtxUpperBound;
        anUnitCtxUpperBound.ComputeFactors(NU, theLocalFactors);
        if (aMWU->IsKind(STANDARD_TYPE(StepBasic_PlaneAngleMeasureWithUnit))
            || aMWU->IsKind(STANDARD_TYPE(StepRepr_ReprItemAndPlaneAngleMeasureWithUnitAndQRI)))
        {
          convertAngleValue(anUnitCtxUpperBound, aVal);
        }
        else if ((aMWU->IsKind(STANDARD_TYPE(StepBasic_MeasureWithUnit))
                  && anUnitCtxUpperBound.LengthFactor() > 0.)
                 || aMWU->IsKind(STANDARD_TYPE(StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI)))
        {
          aVal = aVal * anUnitCtxUpperBound.LengthFactor();
        }
        aDim3 = aVal;

        occ::handle<Standard_Transient> aLowerBound = aTV->LowerBound();
        if (aLowerBound.IsNull())
          continue;

        aMWU = GetMeasureWithUnit(aLowerBound);
        if (aMWU.IsNull())
          continue;

        aVal   = aMWU->ValueComponent();
        anUnit = aMWU->UnitComponent();
        if (anUnit.IsNull())
          continue;
        if (!(anUnit.CaseNum(anUnit.Value()) == 1))
          continue;
        NU = anUnit.NamedUnit();
        STEPConstruct_UnitContext anUnitCtxLowerBound;
        anUnitCtxLowerBound.ComputeFactors(NU, theLocalFactors);
        if (aMWU->IsKind(STANDARD_TYPE(StepBasic_PlaneAngleMeasureWithUnit))
            || aMWU->IsKind(STANDARD_TYPE(StepRepr_ReprItemAndPlaneAngleMeasureWithUnitAndQRI)))
        {
          convertAngleValue(anUnitCtxLowerBound, aVal);
        }
        else if ((aMWU->IsKind(STANDARD_TYPE(StepBasic_MeasureWithUnit))
                  && anUnitCtxLowerBound.LengthFactor() > 0.)
                 || aMWU->IsKind(STANDARD_TYPE(StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI)))
        {
          aVal = aVal * anUnitCtxLowerBound.LengthFactor();
        }
        aDim2 = std::abs(aVal);
      }
      else
      {
        // class of tolerance
        aLAF = aTMD.LimitsAndFits();
      }
    }
  }

  if (aDim1 < 0)
    return;

  if (aDim2 < 0)
    aDimObj->SetValue(aDim1);
  else if (aDim3 < 0)
  {
    occ::handle<NCollection_HArray1<double>> anArr = new NCollection_HArray1<double>(1, 2);
    anArr->SetValue(1, aDim1);
    anArr->SetValue(2, aDim2);
    aDimObj->SetValues(anArr);
  }
  else
  {
    occ::handle<NCollection_HArray1<double>> anArr = new NCollection_HArray1<double>(1, 3);
    if (!isPlusMinusTolerance)
    {
      aDim2 = aDim1 - aDim2;
      aDim3 = aDim3 - aDim1;
    }
    anArr->SetValue(1, aDim1);
    anArr->SetValue(2, aDim2);
    anArr->SetValue(3, aDim3);
    aDimObj->SetValues(anArr);
  }
  if (!aTQ.IsNull())
  {
    XCAFDimTolObjects_DimensionQualifier aQ;
    if (STEPCAFControl_GDTProperty::GetDimQualifierType(aTQ->Name(), aQ))
    {
      aDimObj->SetQualifier(aQ);
    }
  }

  if (!aVFTQ.IsNull())
  {
    // A typical value would be 'NR2 2.2'
    TCollection_HAsciiString aFormat = aVFTQ->FormatType();
    int                      i       = aFormat.Location(1, ' ', 1, aFormat.Length());
    aFormat.SubString(i + 1, i + 1)->IntegerValue();
    aDimObj->SetNbOfDecimalPlaces(aFormat.SubString(i + 1, i + 1)->IntegerValue(),
                                  aFormat.SubString(i + 3, i + 3)->IntegerValue());
  }

  if (!aLAF.IsNull())
  {
    // get class of tolerance
    bool                                    aHolle = false;
    XCAFDimTolObjects_DimensionFormVariance aFV    = XCAFDimTolObjects_DimensionFormVariance_None;
    XCAFDimTolObjects_DimensionGrade        aG     = XCAFDimTolObjects_DimensionGrade_IT01;
    STEPCAFControl_GDTProperty::GetDimClassOfTolerance(aLAF, aHolle, aFV, aG);
    aDimObj->SetClassOfTolerance(aHolle, aFV, aG);
  }

  if (!aCRI.IsNull() && !aCRI->ItemElement().IsNull() && aCRI->ItemElement()->Length() > 0)
  {
    // get modifiers
    NCollection_Sequence<XCAFDimTolObjects_DimensionModif> aModifiers;
    STEPCAFControl_GDTProperty::GetDimModifiers(aCRI, aModifiers);
    if (aModifiers.Length() > 0)
      aDimObj->SetModifiers(aModifiers);
  }

  occ::handle<TCollection_HAsciiString> aName;
  if (!aDimSize.IsNull())
  {
    aName = aDimSize->Name();
  }
  else
  {
    aName = aDimLocation->Name();
  }
  XCAFDimTolObjects_DimensionType aType = XCAFDimTolObjects_DimensionType_Location_None;
  if (!STEPCAFControl_GDTProperty::GetDimType(aName, aType))
  {
    if (!aDimSize.IsNull())
    {
      occ::handle<StepShape_AngularSize> anAngSize =
        occ::down_cast<StepShape_AngularSize>(aDimSize);
      if (!anAngSize.IsNull())
      {
        // get qualifier for angular value
        aType = XCAFDimTolObjects_DimensionType_Size_Angular;
        if (anAngSize->AngleSelection() == StepShape_Equal)
          aDimObj->SetAngularQualifier(XCAFDimTolObjects_AngularQualifier_Equal);
        else if (anAngSize->AngleSelection() == StepShape_Large)
          aDimObj->SetAngularQualifier(XCAFDimTolObjects_AngularQualifier_Large);
        else if (anAngSize->AngleSelection() == StepShape_Small)
          aDimObj->SetAngularQualifier(XCAFDimTolObjects_AngularQualifier_Small);
      }
    }
    else
    {
      occ::handle<StepShape_AngularLocation> anAngLoc =
        occ::down_cast<StepShape_AngularLocation>(aDimLocation);
      if (!anAngLoc.IsNull())
      {
        // get qualifier for angular value
        aType = XCAFDimTolObjects_DimensionType_Location_Angular;
        if (anAngLoc->AngleSelection() == StepShape_Equal)
          aDimObj->SetAngularQualifier(XCAFDimTolObjects_AngularQualifier_Equal);
        else if (anAngLoc->AngleSelection() == StepShape_Large)
          aDimObj->SetAngularQualifier(XCAFDimTolObjects_AngularQualifier_Large);
        else if (anAngLoc->AngleSelection() == StepShape_Small)
          aDimObj->SetAngularQualifier(XCAFDimTolObjects_AngularQualifier_Small);
      }
    }
    if (aType == XCAFDimTolObjects_DimensionType_Location_None)
    {
      occ::handle<StepRepr_ShapeAspect> aPSA;
      if (!aDimSize.IsNull())
      {
        occ::handle<StepShape_DimensionalSizeWithPath> aDimSizeWithPath =
          occ::down_cast<StepShape_DimensionalSizeWithPath>(aDimSize);
        if (!aDimSizeWithPath.IsNull())
        {
          aType = XCAFDimTolObjects_DimensionType_Size_WithPath;
          aPSA  = aDimSizeWithPath->Path();
        }
      }
      else
      {
        occ::handle<StepShape_DimensionalLocationWithPath> aDimLocWithPath =
          occ::down_cast<StepShape_DimensionalLocationWithPath>(aDimLocation);
        if (!aDimLocWithPath.IsNull())
        {
          aType = XCAFDimTolObjects_DimensionType_Location_WithPath;
          aPSA  = aDimLocWithPath->Path();
        }
      }

      if (!aPSA.IsNull())
      {
        // for DimensionalLocationWithPath
        occ::handle<StepGeom_GeometricRepresentationItem> aGRI;
        occ::handle<StepAP242_GeometricItemSpecificUsage> aPGISU;
        Interface_EntityIterator                          anIterDSWP = aGraph.Sharings(aPSA);
        for (anIterDSWP.Start(); anIterDSWP.More() && aPGISU.IsNull(); anIterDSWP.Next())
        {
          aPGISU = occ::down_cast<StepAP242_GeometricItemSpecificUsage>(anIterDSWP.Value());
        }
        if (aPGISU.IsNull())
          return;
        if (aPGISU->NbIdentifiedItem() > 0)
        {
          aGRI =
            occ::down_cast<StepGeom_GeometricRepresentationItem>(aPGISU->IdentifiedItemValue(1));
        }
        if (aGRI.IsNull())
          return;
        occ::handle<StepRepr_RepresentationItem> aPRI;
        Interface_EntityIterator                 anIterGRI = aGraph.Sharings(aGRI);
        for (anIterGRI.Start(); anIterGRI.More() && aPRI.IsNull(); anIterGRI.Next())
        {
          aPRI = occ::down_cast<StepRepr_RepresentationItem>(anIterGRI.Value());
        }
        int         anIndex = FindShapeIndexForDGT(aPRI, theWS);
        TopoDS_Edge aSh;
        if (anIndex > 0)
        {
          occ::handle<Transfer_Binder> aBinder = aTP->MapItem(anIndex);
          aSh                                  = TopoDS::Edge(TransferBRep::ShapeResult(aBinder));
        }
        if (aSh.IsNull())
          return;
        aDimObj->SetPath(aSh);
      }
      else if (!anAP.IsNull() && !anAP->RefDirection().IsNull() && !anAP->Name().IsNull()
               && !anAP->Axis().IsNull() && anAP->Name()->String().IsEqual("orientation"))
      {
        // for Oriented Dimensional Location
        const occ::handle<StepGeom_Direction>& aRefDirection = anAP->RefDirection();
        const std::array<double, 3>&           aDirArr       = aRefDirection->DirectionRatios();
        if (aRefDirection->NbDirectionRatios() >= 3)
        {
          aDimObj->SetDirection({aDirArr[0], aDirArr[1], aDirArr[2]});
        }
        else if (aRefDirection->NbDirectionRatios() == 2)
        {
          aDimObj->SetDirection({aDirArr[0], aDirArr[1], 0.});
        }
      }
    }
  }
  aDimObj->SetType(aType);

  if (!aDimObj.IsNull())
  {

    occ::handle<XCAFDoc_Dimension> aDim;

    if (aDimL.FindAttribute(XCAFDoc_Dimension::GetID(), aDim))
    {
      readAnnotation(aTR, theEnt, aDimObj, theLocalFactors);
      readConnectionPoints(aTR, theEnt, aDimObj, theLocalFactors);
      aDim->SetObject(aDimObj);
    }
  }
}

//=================================================================================================

static bool getTolType(const occ::handle<Standard_Transient>& theEnt,
                       XCAFDimTolObjects_GeomToleranceType&   theType)
{
  if (theEnt.IsNull() || !theEnt->IsKind(STANDARD_TYPE(StepDimTol_GeometricTolerance)))
    return false;
  theType = XCAFDimTolObjects_GeomToleranceType_None;
  if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthDatRef)))
  {
    occ::handle<StepDimTol_GeoTolAndGeoTolWthDatRef> anE =
      occ::down_cast<StepDimTol_GeoTolAndGeoTolWthDatRef>(theEnt);
    theType = STEPCAFControl_GDTProperty::GetGeomToleranceType(anE->GetToleranceType());
  }
  else if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol)))
  {
    occ::handle<StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol> anE =
      occ::down_cast<StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol>(theEnt);
    theType = STEPCAFControl_GDTProperty::GetGeomToleranceType(anE->GetToleranceType());
  }
  else if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod)))
  {
    occ::handle<StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod> anE =
      occ::down_cast<StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod>(theEnt);
    theType = STEPCAFControl_GDTProperty::GetGeomToleranceType(anE->GetToleranceType());
  }
  else if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthMaxTol)))
  {
    occ::handle<StepDimTol_GeoTolAndGeoTolWthMaxTol> anE =
      occ::down_cast<StepDimTol_GeoTolAndGeoTolWthMaxTol>(theEnt);
    theType = STEPCAFControl_GDTProperty::GetGeomToleranceType(anE->GetToleranceType());
  }
  else if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthMod)))
  {
    occ::handle<StepDimTol_GeoTolAndGeoTolWthMod> anE =
      occ::down_cast<StepDimTol_GeoTolAndGeoTolWthMod>(theEnt);
    theType = STEPCAFControl_GDTProperty::GetGeomToleranceType(anE->GetToleranceType());
  }
  else if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol)))
  {
    occ::handle<StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol> anE =
      occ::down_cast<StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol>(theEnt);
    theType = STEPCAFControl_GDTProperty::GetGeomToleranceType(anE->GetToleranceType());
  }
  else if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_AngularityTolerance)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_Angularity;
  }
  else if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_CircularRunoutTolerance)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_CircularRunout;
  }
  else if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_RoundnessTolerance)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_CircularityOrRoundness;
  }
  else if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_CoaxialityTolerance)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_Coaxiality;
  }
  else if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_ConcentricityTolerance)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_Concentricity;
  }
  else if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_CylindricityTolerance)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_Cylindricity;
  }
  else if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_FlatnessTolerance)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_Flatness;
  }
  else if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_ParallelismTolerance)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_Parallelism;
  }
  else if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_PerpendicularityTolerance)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_Perpendicularity;
  }
  else if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_PositionTolerance))
           || theEnt->IsKind(
             STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_Position;
  }
  else if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_LineProfileTolerance)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_ProfileOfLine;
  }
  else if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_SurfaceProfileTolerance)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_ProfileOfSurface;
  }
  else if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_StraightnessTolerance)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_Straightness;
  }
  else if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_SymmetryTolerance)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_Symmetry;
  }
  else if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_TotalRunoutTolerance)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_TotalRunout;
  }
  return true;
}

//=================================================================================================

static void setGeomTolObjectToXCAF(const occ::handle<Standard_Transient>&    theEnt,
                                   const TDF_Label&                          theTolL,
                                   const occ::handle<TDocStd_Document>&      theDoc,
                                   const occ::handle<XSControl_WorkSession>& theWS,
                                   const StepData_Factors&                   theLocalFactors)
{
  occ::handle<XCAFDoc_ShapeTool>  aSTool   = XCAFDoc_DocumentTool::ShapeTool(theDoc->Main());
  occ::handle<XCAFDoc_DimTolTool> aDGTTool = XCAFDoc_DocumentTool::DimTolTool(theDoc->Main());
  const occ::handle<XSControl_TransferReader>&  aTR    = theWS->TransferReader();
  const occ::handle<Transfer_TransientProcess>& aTP    = aTR->TransientProcess();
  const Interface_Graph&                        aGraph = aTP->Graph();
  occ::handle<XCAFDoc_GeomTolerance>            aGTol;
  if (!theTolL.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aGTol))
  {
    return;
  }
  occ::handle<XCAFDimTolObjects_GeomToleranceObject> aTolObj = aGTol->GetObject();
  occ::handle<StepDimTol_GeometricTolerance>         aTolEnt =
    occ::down_cast<StepDimTol_GeometricTolerance>(theEnt);

  XCAFDimTolObjects_GeomToleranceType aType = XCAFDimTolObjects_GeomToleranceType_None;
  getTolType(theEnt, aType);
  aTolObj->SetType(aType);
  if (!aTolEnt->Magnitude().IsNull())
  {
    // get value
    if (occ::handle<StepBasic_MeasureWithUnit> aMWU = GetMeasureWithUnit(aTolEnt->Magnitude()))
    {
      double         aVal   = aMWU->ValueComponent();
      StepBasic_Unit anUnit = aMWU->UnitComponent();
      if (anUnit.IsNull())
        return;
      if (!(anUnit.CaseNum(anUnit.Value()) == 1))
        return;
      occ::handle<StepBasic_NamedUnit> NU = anUnit.NamedUnit();
      STEPConstruct_UnitContext        anUnitCtx;
      anUnitCtx.ComputeFactors(NU, theLocalFactors);
      aVal = aVal * anUnitCtx.LengthFactor();
      aTolObj->SetValue(aVal);
    }
  }
  // get modifiers
  XCAFDimTolObjects_GeomToleranceTypeValue aTypeV = XCAFDimTolObjects_GeomToleranceTypeValue_None;
  Interface_EntityIterator                 anIter = aGraph.Sharings(aTolEnt);
  for (anIter.Start(); anIter.More(); anIter.Next())
  {
    if (anIter.Value()->IsKind(STANDARD_TYPE(StepDimTol_ToleranceZone)))
    {
      occ::handle<StepDimTol_ToleranceZoneForm> aForm =
        occ::down_cast<StepDimTol_ToleranceZone>(anIter.Value())->Form();
      STEPCAFControl_GDTProperty::GetTolValueType(aForm->Name(), aTypeV);
      Interface_EntityIterator anIt = aGraph.Sharings(anIter.Value());
      for (anIt.Start(); anIt.More(); anIt.Next())
      {
        if (anIt.Value()->IsKind(STANDARD_TYPE(StepDimTol_ProjectedZoneDefinition)))
        {
          occ::handle<StepDimTol_ProjectedZoneDefinition> aPZone =
            occ::down_cast<StepDimTol_ProjectedZoneDefinition>(anIt.Value());
          if (!aPZone->ProjectionLength().IsNull())
          {
            double         aVal   = aPZone->ProjectionLength()->ValueComponent();
            StepBasic_Unit anUnit = aPZone->ProjectionLength()->UnitComponent();
            if (anUnit.IsNull())
              return;
            if (!(anUnit.CaseNum(anUnit.Value()) == 1))
              return;
            occ::handle<StepBasic_NamedUnit> NU = anUnit.NamedUnit();
            STEPConstruct_UnitContext        anUnitCtx;
            anUnitCtx.ComputeFactors(NU, theLocalFactors);
            aVal = aVal * anUnitCtx.LengthFactor();
            aTolObj->SetValueOfZoneModifier(aVal);
            aTolObj->SetZoneModifier(XCAFDimTolObjects_GeomToleranceZoneModif_Projected);
          }
        }
        else if (anIt.Value()->IsKind(STANDARD_TYPE(StepDimTol_RunoutZoneDefinition)))
        {
          occ::handle<StepDimTol_RunoutZoneDefinition> aRZone =
            occ::down_cast<StepDimTol_RunoutZoneDefinition>(anIt.Value());
          if (!aRZone->Orientation().IsNull())
          {
            double         aVal   = aRZone->Orientation()->Angle()->ValueComponent();
            StepBasic_Unit anUnit = aRZone->Orientation()->Angle()->UnitComponent();
            if (anUnit.IsNull())
              continue;
            if (!(anUnit.CaseNum(anUnit.Value()) == 1))
              continue;
            occ::handle<StepBasic_NamedUnit> NU = anUnit.NamedUnit();
            STEPConstruct_UnitContext        anUnitCtx;
            anUnitCtx.ComputeFactors(NU, theLocalFactors);
            convertAngleValue(anUnitCtx, aVal);
            aTolObj->SetValueOfZoneModifier(aVal);
            aTolObj->SetZoneModifier(XCAFDimTolObjects_GeomToleranceZoneModif_Runout);
          }
        }
      }
      aTolObj->SetTypeOfValue(aTypeV);
    }
  }
  occ::handle<NCollection_HArray1<StepDimTol_GeometricToleranceModifier>> aModifiers;
  if (aTolEnt->IsKind(STANDARD_TYPE(StepDimTol_GeometricToleranceWithModifiers)))
  {
    aModifiers = occ::down_cast<StepDimTol_GeometricToleranceWithModifiers>(aTolEnt)->Modifiers();
  }
  else if (aTolEnt->IsKind(STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod)))
  {
    aModifiers = occ::down_cast<StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod>(aTolEnt)
                   ->GetGeometricToleranceWithModifiers()
                   ->Modifiers();
  }
  else if (aTolEnt->IsKind(STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthMod)))
  {
    aModifiers = occ::down_cast<StepDimTol_GeoTolAndGeoTolWthMod>(aTolEnt)
                   ->GetGeometricToleranceWithModifiers()
                   ->Modifiers();
  }
  else if (aTolEnt->IsKind(STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthMaxTol)))
  {
    aModifiers = occ::down_cast<StepDimTol_GeoTolAndGeoTolWthMaxTol>(aTolEnt)
                   ->GetGeometricToleranceWithModifiers()
                   ->Modifiers();
  }
  else if (aTolEnt->IsKind(STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol)))
  {
    aModifiers = occ::down_cast<StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol>(aTolEnt)
                   ->GetGeometricToleranceWithModifiers()
                   ->Modifiers();
  }
  if (!aModifiers.IsNull())
  {
    for (int i = aModifiers->Lower(); i <= aModifiers->Upper(); i++)
    {
      if (aModifiers->Value(i) == StepDimTol_GTMLeastMaterialRequirement)
        aTolObj->SetMaterialRequirementModifier(XCAFDimTolObjects_GeomToleranceMatReqModif_L);
      else if (aModifiers->Value(i) == StepDimTol_GTMMaximumMaterialRequirement)
        aTolObj->SetMaterialRequirementModifier(XCAFDimTolObjects_GeomToleranceMatReqModif_M);
      else
        aTolObj->AddModifier((XCAFDimTolObjects_GeomToleranceModif)aModifiers->Value(i));
    }
  }
  double         aVal = 0;
  StepBasic_Unit anUnit;
  if (aTolEnt->IsKind(STANDARD_TYPE(StepDimTol_GeometricToleranceWithMaximumTolerance)))
  {
    occ::handle<StepDimTol_GeometricToleranceWithMaximumTolerance> aMax =
      occ::down_cast<StepDimTol_GeometricToleranceWithMaximumTolerance>(aTolEnt);
    aVal   = aMax->MaximumUpperTolerance()->ValueComponent();
    anUnit = aMax->MaximumUpperTolerance()->UnitComponent();
  }
  else if (aTolEnt->IsKind(STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthMaxTol)))
  {
    occ::handle<StepDimTol_GeoTolAndGeoTolWthMaxTol> aMax =
      occ::down_cast<StepDimTol_GeoTolAndGeoTolWthMaxTol>(aTolEnt);
    aVal   = aMax->GetMaxTolerance()->ValueComponent();
    anUnit = aMax->GetMaxTolerance()->UnitComponent();
  }
  else if (aTolEnt->IsKind(STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol)))
  {
    occ::handle<StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol> aMax =
      occ::down_cast<StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol>(aTolEnt);
    aVal   = aMax->GetMaxTolerance()->ValueComponent();
    anUnit = aMax->GetMaxTolerance()->UnitComponent();
  }
  if (!anUnit.IsNull() && (anUnit.CaseNum(anUnit.Value()) == 1))
  {
    occ::handle<StepBasic_NamedUnit> NU = anUnit.NamedUnit();
    STEPConstruct_UnitContext        anUnitCtx;
    anUnitCtx.ComputeFactors(NU, theLocalFactors);
    convertAngleValue(anUnitCtx, aVal);
    aTolObj->SetMaxValueModifier(aVal);
  }

  readAnnotation(aTR, theEnt, aTolObj, theLocalFactors);
  aGTol->SetObject(aTolObj);
}

//=================================================================================================

bool STEPCAFControl_Reader::ReadGDTs(const occ::handle<XSControl_WorkSession>& theWS,
                                     const occ::handle<TDocStd_Document>&      theDoc,
                                     const StepData_Factors&                   theLocalFactors)
{
  const occ::handle<Interface_InterfaceModel>& aModel = theWS->Model();
  const Interface_Graph&                       aGraph = theWS->Graph();
  const occ::handle<XSControl_TransferReader>& aTR    = theWS->TransferReader();
  occ::handle<StepData_StepModel>              aSM    = occ::down_cast<StepData_StepModel>(aModel);
  Interface_EntityIterator                     anI    = aSM->Header();
  occ::handle<HeaderSection_FileSchema>        aH;
  for (anI.Start(); anI.More() && aH.IsNull(); anI.Next())
    aH = occ::down_cast<HeaderSection_FileSchema>(anI.Value());
  occ::handle<XCAFDoc_ShapeTool>  aSTool   = XCAFDoc_DocumentTool::ShapeTool(theDoc->Main());
  occ::handle<XCAFDoc_DimTolTool> aDGTTool = XCAFDoc_DocumentTool::DimTolTool(theDoc->Main());
  if (aDGTTool.IsNull())
    return false;

  int nb = aModel->NbEntities();
  for (int i = 1; i <= nb; i++)
  {
    occ::handle<Standard_Transient> anEnt = aModel->Value(i);
    if (anEnt->IsKind(STANDARD_TYPE(StepShape_DimensionalSize))
        || anEnt->IsKind(STANDARD_TYPE(StepShape_DimensionalLocation))
        || anEnt->IsKind(STANDARD_TYPE(StepDimTol_GeometricTolerance)))
    {
      TDF_Label aGDTL = createGDTObjectInXCAF(anEnt, theDoc, theWS, theLocalFactors);
      if (!aGDTL.IsNull())
      {
        if (anEnt->IsKind(STANDARD_TYPE(StepDimTol_GeometricTolerance)))
        {
          setGeomTolObjectToXCAF(anEnt, aGDTL, theDoc, theWS, theLocalFactors);
        }
        else
        {
          setDimObjectToXCAF(anEnt, aGDTL, theDoc, theWS, theLocalFactors);
        }
      }
    }
    else if (anEnt->IsKind(STANDARD_TYPE(StepVisual_DraughtingCallout))
             || anEnt->IsKind(STANDARD_TYPE(StepVisual_AnnotationOccurrence))
             || anEnt->IsKind(STANDARD_TYPE(StepVisual_TessellatedAnnotationOccurrence)))
    {
      // Protection against import presentation twice
      occ::handle<StepVisual_DraughtingCallout> aDC;
      for (Interface_EntityIterator anIter = aGraph.Sharings(anEnt); anIter.More() && aDC.IsNull();
           anIter.Next())
      {
        aDC = occ::down_cast<StepVisual_DraughtingCallout>(anIter.Value());
      }
      if (!aDC.IsNull())
        continue;
      // Read presentations for PMIs without semantic data.
      occ::handle<StepAP242_DraughtingModelItemAssociation> aDMIA;
      NCollection_Sequence<TDF_Label>                       aShapesL;
      for (Interface_EntityIterator anIter = aGraph.Sharings(anEnt);
           anIter.More() && aDMIA.IsNull();
           anIter.Next())
      {
        aDMIA = occ::down_cast<StepAP242_DraughtingModelItemAssociation>(anIter.Value());
      }
      if (!aDMIA.IsNull())
      {
        // Check entity, skip all, attached to GDTs
        occ::handle<StepRepr_ShapeAspect> aDefinition = aDMIA->Definition().ShapeAspect();
        if (!aDefinition.IsNull())
        {
          bool isConnectedToGDT = false;
          // Skip if definition is a datum
          if (aDefinition->IsKind(STANDARD_TYPE(StepDimTol_Datum))
              || aDefinition->IsKind(STANDARD_TYPE(StepDimTol_DatumTarget))
              || aDefinition->IsKind(STANDARD_TYPE(StepDimTol_DatumFeature))
              || aDefinition->IsKind(STANDARD_TYPE(StepRepr_CompShAspAndDatumFeatAndShAsp)))
          {
            isConnectedToGDT = true;
          }
          // Skip if any GDT is applied to definition
          for (Interface_EntityIterator anIter = aGraph.Sharings(aDefinition);
               anIter.More() && !isConnectedToGDT;
               anIter.Next())
          {
            if (anIter.Value()->IsKind(STANDARD_TYPE(StepShape_DimensionalSize))
                || anIter.Value()->IsKind(STANDARD_TYPE(StepShape_DimensionalLocation))
                || anIter.Value()->IsKind(STANDARD_TYPE(StepDimTol_GeometricTolerance)))
            {
              isConnectedToGDT = true;
              continue;
            }
            occ::handle<StepRepr_ShapeAspectRelationship> aSAR =
              occ::down_cast<StepRepr_ShapeAspectRelationship>(anIter.Value());
            if (!aSAR.IsNull())
            {
              occ::handle<StepRepr_ShapeAspect> aSA = aSAR->RelatedShapeAspect();
              if (!aSA.IsNull())
              {
                if (aSA->IsKind(STANDARD_TYPE(StepDimTol_Datum))
                    || aSA->IsKind(STANDARD_TYPE(StepDimTol_DatumTarget))
                    || aSA->IsKind(STANDARD_TYPE(StepDimTol_DatumFeature))
                    || aSA->IsKind(STANDARD_TYPE(StepRepr_CompShAspAndDatumFeatAndShAsp)))
                {
                  isConnectedToGDT = true;
                }
                for (Interface_EntityIterator aDimIter = aGraph.Sharings(aSA);
                     aDimIter.More() && !isConnectedToGDT;
                     aDimIter.Next())
                {
                  if (aDimIter.Value()->IsKind(STANDARD_TYPE(StepShape_DimensionalSize))
                      || aDimIter.Value()->IsKind(STANDARD_TYPE(StepShape_DimensionalLocation))
                      || aDimIter.Value()->IsKind(STANDARD_TYPE(StepDimTol_GeometricTolerance)))
                  {
                    isConnectedToGDT = true;
                    continue;
                  }
                }
              }
            }
          }
          if (isConnectedToGDT)
            continue;
        }
        else if (aDMIA->Definition().PropertyDefinition().IsNull())
          continue;

        // Get shapes
        NCollection_Sequence<occ::handle<StepRepr_ShapeAspect>> aSAs;
        collectShapeAspect(aDefinition, theWS, aSAs);
        for (int aSAIt = 1; aSAIt <= aSAs.Length(); aSAIt++)
        {
          occ::handle<StepAP242_GeometricItemSpecificUsage> aGISU;
          for (Interface_EntityIterator anIter = aGraph.Sharings(aSAs.Value(aSAIt));
               anIter.More() && aGISU.IsNull();
               anIter.Next())
            aGISU = occ::down_cast<StepAP242_GeometricItemSpecificUsage>(anIter.Value());
          if (aGISU.IsNull())
            continue;
          for (int anItemIt = 1; anItemIt <= aGISU->NbIdentifiedItem(); anItemIt++)
          {
            TDF_Label aLabel = getShapeLabel(aGISU->IdentifiedItemValue(anItemIt),
                                             theWS,
                                             XCAFDoc_DocumentTool::ShapeTool(theDoc->Main()));
            if (!aLabel.IsNull())
              aShapesL.Append(aLabel);
          }
        }
      }
      bool isCommonLabel = (aShapesL.Length() == 0);

      // Calculate unit
      double           aFact         = 1.0;
      StepData_Factors aLocalFactors = theLocalFactors;
      if (!aDMIA.IsNull())
      {
        XSAlgo_ShapeProcessor::PrepareForTransfer();
        STEPControl_ActorRead                  anActor(aModel);
        occ::handle<Transfer_TransientProcess> aTP = aTR->TransientProcess();
        anActor.PrepareUnits(aDMIA->UsedRepresentation(), aTP, aLocalFactors);
        aFact = aLocalFactors.LengthFactor();
      }

      // Presentation
      TopoDS_Shape                          aPresentation;
      occ::handle<TCollection_HAsciiString> aPresentName;
      Bnd_Box                               aBox;
      if (!readPMIPresentation(anEnt, aTR, aFact, aPresentation, aPresentName, aBox, aLocalFactors))
        continue;
      // Annotation plane
      occ::handle<StepVisual_AnnotationPlane> anAnPlane;
      for (Interface_EntityIterator anIter = aGraph.Sharings(anEnt);
           anIter.More() && anAnPlane.IsNull();
           anIter.Next())
        anAnPlane = occ::down_cast<StepVisual_AnnotationPlane>(anIter.Value());

      // Set object to XCAF
      TDF_Label aGDTL = aDGTTool->AddDimension();
      myGDTMap.Bind(anEnt, aGDTL);
      aDGTTool->Lock(aGDTL);
      occ::handle<XCAFDimTolObjects_DimensionObject> aDimObj =
        new XCAFDimTolObjects_DimensionObject();
      occ::handle<XCAFDoc_Dimension> aDim = XCAFDoc_Dimension::Set(aGDTL);
      TCollection_AsciiString        aStr("DGT:");
      if (isCommonLabel)
      {
        aStr.AssignCat("Common_label");
        aDimObj->SetType(XCAFDimTolObjects_DimensionType_CommonLabel);
      }
      else
      {
        aStr.AssignCat("Dimension");
        aDimObj->SetType(XCAFDimTolObjects_DimensionType_DimensionPresentation);
      }
      TDataStd_Name::Set(aGDTL, aStr);
      NCollection_Sequence<TDF_Label> anEmptySeq2;
      aDGTTool->SetDimension(aShapesL, anEmptySeq2, aGDTL);
      gp_Ax2 aPlaneAxes;
      if (!anAnPlane.IsNull())
      {
        if (readAnnotationPlane(anAnPlane, aPlaneAxes, aLocalFactors))
          aDimObj->SetPlane(aPlaneAxes);
      }
      aDimObj->SetPresentation(aPresentation, aPresentName);
      aDim->SetObject(aDimObj);
    }
  }
  return true;
}

//=================================================================================================

static occ::handle<StepShape_SolidModel> FindSolidForPDS(
  const occ::handle<StepRepr_ProductDefinitionShape>& PDS,
  const Interface_Graph&                              graph)
{
  occ::handle<StepShape_SolidModel>          SM;
  Interface_EntityIterator                   subs = graph.Sharings(PDS);
  occ::handle<StepShape_ShapeRepresentation> SR;
  for (subs.Start(); subs.More() && SM.IsNull(); subs.Next())
  {
    occ::handle<StepShape_ShapeDefinitionRepresentation> SDR =
      occ::down_cast<StepShape_ShapeDefinitionRepresentation>(subs.Value());
    if (SDR.IsNull())
      continue;
    SR = occ::down_cast<StepShape_ShapeRepresentation>(SDR->UsedRepresentation());
    if (SR.IsNull())
      continue;
    for (int i = 1; i <= SR->NbItems() && SM.IsNull(); i++)
    {
      SM = occ::down_cast<StepShape_SolidModel>(SR->ItemsValue(i));
    }
    if (SM.IsNull())
    {
      Interface_EntityIterator subs1 = graph.Sharings(SR);
      for (subs1.Start(); subs1.More() && SM.IsNull(); subs1.Next())
      {
        occ::handle<StepRepr_RepresentationRelationship> RR =
          occ::down_cast<StepRepr_RepresentationRelationship>(subs1.Value());
        if (RR.IsNull())
          continue;
        occ::handle<StepShape_ShapeRepresentation> SR2;
        if (RR->Rep1() == SR)
          SR2 = occ::down_cast<StepShape_ShapeRepresentation>(RR->Rep2());
        else
          SR2 = occ::down_cast<StepShape_ShapeRepresentation>(RR->Rep1());
        if (SR2.IsNull())
          continue;
        for (int i2 = 1; i2 <= SR2->NbItems() && SM.IsNull(); i2++)
        {
          SM = occ::down_cast<StepShape_SolidModel>(SR2->ItemsValue(i2));
        }
      }
    }
  }
  return SM;
}

//=================================================================================================

bool STEPCAFControl_Reader::ReadMaterials(
  const occ::handle<XSControl_WorkSession>&                                  WS,
  const occ::handle<TDocStd_Document>&                                       Doc,
  const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& SeqPDS,
  const StepData_Factors&                                                    theLocalFactors) const
{
  const occ::handle<XSControl_TransferReader>&  TR = WS->TransferReader();
  const occ::handle<Transfer_TransientProcess>& TP = TR->TransientProcess();
  occ::handle<XCAFDoc_ShapeTool>    STool          = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  occ::handle<XCAFDoc_MaterialTool> MatTool = XCAFDoc_DocumentTool::MaterialTool(Doc->Main());
  if (MatTool.IsNull())
    return false;

  const Interface_Graph& graph = TP->Graph();
  for (int i = 1; i <= SeqPDS->Length(); i++)
  {
    occ::handle<StepRepr_ProductDefinitionShape> PDS =
      occ::down_cast<StepRepr_ProductDefinitionShape>(SeqPDS->Value(i));
    if (PDS.IsNull())
      continue;
    occ::handle<StepBasic_ProductDefinition> aProdDef = PDS->Definition().ProductDefinition();
    if (aProdDef.IsNull())
      continue;
    occ::handle<TCollection_HAsciiString> aName        = new TCollection_HAsciiString("");
    occ::handle<TCollection_HAsciiString> aDescription = new TCollection_HAsciiString("");
    occ::handle<TCollection_HAsciiString> aDensName    = new TCollection_HAsciiString("");
    occ::handle<TCollection_HAsciiString> aDensValType = new TCollection_HAsciiString("");
    double                                aDensity     = 0;
    Interface_EntityIterator              subs         = graph.Sharings(aProdDef);
    for (subs.Start(); subs.More(); subs.Next())
    {
      occ::handle<StepRepr_PropertyDefinition> PropD =
        occ::down_cast<StepRepr_PropertyDefinition>(subs.Value());
      if (PropD.IsNull())
        continue;
      Interface_EntityIterator subs1 = graph.Sharings(PropD);
      for (subs1.Start(); subs1.More(); subs1.Next())
      {
        occ::handle<StepRepr_PropertyDefinitionRepresentation> PDR =
          occ::down_cast<StepRepr_PropertyDefinitionRepresentation>(subs1.Value());
        if (PDR.IsNull())
          continue;
        occ::handle<StepRepr_Representation> Repr = PDR->UsedRepresentation();
        if (Repr.IsNull())
          continue;
        int ir;
        for (ir = 1; ir <= Repr->NbItems(); ir++)
        {
          occ::handle<StepRepr_RepresentationItem> RI = Repr->ItemsValue(ir);
          if (RI.IsNull())
            continue;
          if (RI->IsKind(STANDARD_TYPE(StepRepr_DescriptiveRepresentationItem)))
          {
            // find name and description for material
            occ::handle<StepRepr_DescriptiveRepresentationItem> DRI =
              occ::down_cast<StepRepr_DescriptiveRepresentationItem>(RI);
            aName = DRI->Name();

            aDescription = DRI->Description();
            if (aName.IsNull())
              aName = aDescription;
          }
          if (RI->IsKind(STANDARD_TYPE(StepRepr_MeasureRepresentationItem)))
          {
            // try to find density for material
            occ::handle<StepRepr_MeasureRepresentationItem> MRI =
              occ::down_cast<StepRepr_MeasureRepresentationItem>(RI);
            aDensity  = MRI->Measure()->ValueComponent();
            aDensName = MRI->Name();
            aDensValType =
              new TCollection_HAsciiString(MRI->Measure()->ValueComponentMember()->Name());
            StepBasic_Unit aUnit = MRI->Measure()->UnitComponent();
            if (!aUnit.IsNull())
            {
              occ::handle<StepBasic_DerivedUnit> DU = aUnit.DerivedUnit();
              if (DU.IsNull())
                continue;
              for (int idu = 1; idu <= DU->NbElements(); idu++)
              {
                occ::handle<StepBasic_DerivedUnitElement> DUE = DU->ElementsValue(idu);
                occ::handle<StepBasic_NamedUnit>          NU  = DUE->Unit();
                if (NU.IsNull())
                  continue;
                if (NU->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndLengthUnit))
                    || NU->IsKind(STANDARD_TYPE(StepBasic_SiUnitAndLengthUnit)))
                {
                  STEPConstruct_UnitContext anUnitCtx;
                  anUnitCtx.ComputeFactors(NU, theLocalFactors);
                  aDensity = aDensity
                             / (anUnitCtx.LengthFactor() * anUnitCtx.LengthFactor()
                                * anUnitCtx.LengthFactor());
                  // transfer length value for Density from millimeter to santimeter
                  // in order to result density has dimension gram/(sm*sm*sm)
                  const double aCascadeUnit = theLocalFactors.CascadeUnit();
                  aDensity = aDensity * 1000. / (aCascadeUnit * aCascadeUnit * aCascadeUnit);
                }
                if (NU->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndMassUnit)))
                {
                  double afact = 1.;
                  if (GetMassConversionFactor(NU, afact))
                  {
                    aDensity = aDensity * afact;
                  }
                }
              }
            }
          }
        }
      }
    }

    if (aName.IsNull() || aName->Length() == 0)
      continue;
    // find shape label amd create Material link
    TopoDS_Shape                      aSh;
    occ::handle<StepShape_SolidModel> SM = FindSolidForPDS(PDS, graph);
    if (!SM.IsNull())
    {
      int index = TP->MapIndex(SM);
      if (index > 0)
      {
        occ::handle<Transfer_Binder> binder = TP->MapItem(index);
        if (!binder.IsNull())
          aSh = TransferBRep::ShapeResult(binder);
      }
    }
    if (aSh.IsNull())
      continue;
    TDF_Label shL;
    if (!STool->Search(aSh, shL, true, true, true))
      continue;
    MatTool->SetMaterial(shL, aName, aDescription, aDensity, aDensName, aDensValType);
  }

  return true;
}

//=======================================================================
// function : collectViewShapes
// purpose  : collect all labels of representations in given representation
//=======================================================================

void collectViewShapes(const occ::handle<XSControl_WorkSession>&   theWS,
                       const occ::handle<TDocStd_Document>&        theDoc,
                       const occ::handle<StepRepr_Representation>& theRepr,
                       NCollection_Sequence<TDF_Label>&            theShapes)
{
  occ::handle<XSControl_TransferReader>  aTR     = theWS->TransferReader();
  occ::handle<Transfer_TransientProcess> aTP     = aTR->TransientProcess();
  const Interface_Graph&                 aGraph  = aTP->Graph();
  occ::handle<XCAFDoc_ShapeTool>         aSTool  = XCAFDoc_DocumentTool::ShapeTool(theDoc->Main());
  int                                    anIndex = aTP->MapIndex(theRepr);
  TopoDS_Shape                           aSh;
  if (anIndex > 0)
  {
    occ::handle<Transfer_Binder> aBinder = aTP->MapItem(anIndex);
    aSh                                  = TransferBRep::ShapeResult(aBinder);
  }
  if (!aSh.IsNull())
  {
    TDF_Label aShL;
    aSTool->FindShape(aSh, aShL);
    if (!aShL.IsNull())
      theShapes.Append(aShL);
  }
  Interface_EntityIterator anIter = aGraph.Sharings(theRepr);
  for (; anIter.More(); anIter.Next())
  {
    if (!anIter.Value()->IsKind(STANDARD_TYPE(StepRepr_RepresentationRelationship)))
      continue;
    occ::handle<StepRepr_RepresentationRelationship> aReprRelationship =
      occ::down_cast<StepRepr_RepresentationRelationship>(anIter.Value());
    if (!aReprRelationship->Rep1().IsNull() && aReprRelationship->Rep1() != theRepr)
      collectViewShapes(theWS, theDoc, aReprRelationship->Rep1(), theShapes);
  }
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> buildClippingPlanes(
  const occ::handle<StepGeom_GeometricRepresentationItem>& theClippingCameraModel,
  NCollection_Sequence<TDF_Label>&                         theClippingPlanes,
  const occ::handle<XCAFDoc_ClippingPlaneTool>&            theTool,
  const StepData_Factors&                                  theLocalFactors)
{
  occ::handle<TCollection_HAsciiString> anExpression = new TCollection_HAsciiString();
  NCollection_Sequence<occ::handle<StepGeom_GeometricRepresentationItem>> aPlanes;
  occ::handle<TCollection_HAsciiString> anOperation = new TCollection_HAsciiString("*");

  // Store operands
  if (theClippingCameraModel->IsKind(STANDARD_TYPE(StepVisual_CameraModelD3MultiClipping)))
  {
    occ::handle<StepVisual_CameraModelD3MultiClipping> aCameraModel =
      occ::down_cast<StepVisual_CameraModelD3MultiClipping>(theClippingCameraModel);

    if (aCameraModel->ShapeClipping().IsNull())
      return anExpression;

    // Root of clipping planes tree
    if (aCameraModel->ShapeClipping()->Length() == 1)
    {
      occ::handle<StepVisual_CameraModelD3MultiClippingUnion> aCameraModelUnion =
        aCameraModel->ShapeClipping()->Value(1).CameraModelD3MultiClippingUnion();
      if (!aCameraModelUnion.IsNull())
        return buildClippingPlanes(aCameraModelUnion, theClippingPlanes, theTool, theLocalFactors);
    }
    for (int i = 1; i <= aCameraModel->ShapeClipping()->Length(); i++)
    {
      aPlanes.Append(occ::down_cast<StepGeom_GeometricRepresentationItem>(
        aCameraModel->ShapeClipping()->Value(i).Value()));
    }
  }
  else if (theClippingCameraModel->IsKind(
             STANDARD_TYPE(StepVisual_CameraModelD3MultiClippingUnion)))
  {
    occ::handle<StepVisual_CameraModelD3MultiClippingUnion> aCameraModel =
      occ::down_cast<StepVisual_CameraModelD3MultiClippingUnion>(theClippingCameraModel);
    anOperation = new TCollection_HAsciiString("+");
    for (int i = 1; i <= aCameraModel->ShapeClipping()->Length(); i++)
    {
      aPlanes.Append(occ::down_cast<StepGeom_GeometricRepresentationItem>(
        aCameraModel->ShapeClipping()->Value(i).Value()));
    }
  }
  else if (theClippingCameraModel->IsKind(
             STANDARD_TYPE(StepVisual_CameraModelD3MultiClippingIntersection)))
  {
    occ::handle<StepVisual_CameraModelD3MultiClippingIntersection> aCameraModel =
      occ::down_cast<StepVisual_CameraModelD3MultiClippingIntersection>(theClippingCameraModel);
    for (int i = 1; i <= aCameraModel->ShapeClipping()->Length(); i++)
    {
      aPlanes.Append(occ::down_cast<StepGeom_GeometricRepresentationItem>(
        aCameraModel->ShapeClipping()->Value(i).Value()));
    }
  }
  // Build expression
  anExpression->AssignCat("(");
  for (int i = 1; i <= aPlanes.Length(); i++)
  {
    occ::handle<StepGeom_Plane> aPlaneEnt = occ::down_cast<StepGeom_Plane>(aPlanes.Value(i));
    if (!aPlaneEnt.IsNull())
    {
      occ::handle<Geom_Plane> aPlane = StepToGeom::MakePlane(aPlaneEnt, theLocalFactors);
      if (!aPlane.IsNull())
      {
        TDF_Label aPlaneL = theTool->AddClippingPlane(aPlane->Pln(), aPlaneEnt->Name());
        theClippingPlanes.Append(aPlaneL);
        TCollection_AsciiString anEntry;
        TDF_Tool::Entry(aPlaneL, anEntry);
        anExpression->AssignCat(new TCollection_HAsciiString(anEntry));
      }
    }
    else
    {
      anExpression->AssignCat(
        buildClippingPlanes(aPlanes.Value(i), theClippingPlanes, theTool, theLocalFactors));
    }
    anExpression->AssignCat(anOperation);
  }
  // Insert brace instead of operation after last operand.
  anExpression->SetValue(anExpression->Length(), ')');
  return anExpression;
}

//=================================================================================================

bool STEPCAFControl_Reader::ReadViews(const occ::handle<XSControl_WorkSession>& theWS,
                                      const occ::handle<TDocStd_Document>&      theDoc,
                                      const StepData_Factors& theLocalFactors) const
{
  const occ::handle<Interface_InterfaceModel>& aModel = theWS->Model();
  occ::handle<XCAFDoc_ShapeTool>  aSTool    = XCAFDoc_DocumentTool::ShapeTool(theDoc->Main());
  occ::handle<XCAFDoc_DimTolTool> aDGTTool  = XCAFDoc_DocumentTool::DimTolTool(theDoc->Main());
  occ::handle<XCAFDoc_ViewTool>   aViewTool = XCAFDoc_DocumentTool::ViewTool(theDoc->Main());
  if (aDGTTool.IsNull())
    return false;

  int nb = aModel->NbEntities();
  for (int i = 1; i <= nb; i++)
  {
    occ::handle<Standard_Transient> anEnt = aModel->Value(i);
    if (!anEnt->IsKind(STANDARD_TYPE(StepVisual_CameraModelD3)))
      continue;
    occ::handle<XCAFView_Object> anObj = new XCAFView_Object();
    // Import attributes of view
    occ::handle<StepVisual_CameraModelD3> aCameraModel =
      occ::down_cast<StepVisual_CameraModelD3>(anEnt);

    const occ::handle<XSControl_TransferReader>& aTR    = theWS->TransferReader();
    occ::handle<Transfer_TransientProcess>       aTP    = aTR->TransientProcess();
    const Interface_Graph&                       aGraph = aTP->Graph();
    // find the proper DraughtingModel
    Interface_EntityIterator                subs = aGraph.Sharings(aCameraModel);
    occ::handle<StepVisual_DraughtingModel> aDModel;
    for (subs.Start(); subs.More() && aDModel.IsNull(); subs.Next())
    {
      if (!subs.Value()->IsKind(STANDARD_TYPE(StepVisual_DraughtingModel)))
      {
        continue;
      }
      aDModel = occ::down_cast<StepVisual_DraughtingModel>(subs.Value());
    }
    StepData_Factors aLocalFactors = theLocalFactors;
    if (!aDModel.IsNull())
    {
      XSAlgo_ShapeProcessor::PrepareForTransfer();
      STEPControl_ActorRead anActor(aTP->Model());
      anActor.PrepareUnits(aDModel, aTP, aLocalFactors);
    }

    anObj->SetName(aCameraModel->Name());
    occ::handle<Geom_Axis2Placement> anAxis =
      StepToGeom::MakeAxis2Placement(aCameraModel->ViewReferenceSystem(), aLocalFactors);
    anObj->SetViewDirection(anAxis->Direction());
    anObj->SetUpDirection(anAxis->Direction() ^ anAxis->XDirection());
    occ::handle<StepVisual_ViewVolume> aViewVolume = aCameraModel->PerspectiveOfVolume();
    XCAFView_ProjectionType            aType       = XCAFView_ProjectionType_NoCamera;
    if (aViewVolume->ProjectionType() == StepVisual_copCentral)
      aType = XCAFView_ProjectionType_Central;
    else if (aViewVolume->ProjectionType() == StepVisual_copParallel)
      aType = XCAFView_ProjectionType_Parallel;
    anObj->SetType(aType);
    occ::handle<Geom_CartesianPoint> aPoint =
      StepToGeom::MakeCartesianPoint(aViewVolume->ProjectionPoint(), aLocalFactors);
    anObj->SetProjectionPoint(aPoint->Pnt());
    anObj->SetZoomFactor(aViewVolume->ViewPlaneDistance());
    anObj->SetWindowHorizontalSize(aViewVolume->ViewWindow()->SizeInX());
    anObj->SetWindowVerticalSize(aViewVolume->ViewWindow()->SizeInY());
    if (aViewVolume->FrontPlaneClipping())
      anObj->SetFrontPlaneDistance(aViewVolume->FrontPlaneDistance());
    if (aViewVolume->BackPlaneClipping())
      anObj->SetBackPlaneDistance(aViewVolume->BackPlaneDistance());
    anObj->SetViewVolumeSidesClipping(aViewVolume->ViewVolumeSidesClipping());
    // Clipping plane
    occ::handle<StepVisual_CameraModelD3MultiClipping> aClippingCameraModel =
      occ::down_cast<StepVisual_CameraModelD3MultiClipping>(aCameraModel);
    NCollection_Sequence<TDF_Label> aClippingPlanes;
    if (!aClippingCameraModel.IsNull())
    {
      occ::handle<TCollection_HAsciiString>  aClippingExpression;
      occ::handle<XCAFDoc_ClippingPlaneTool> aClippingPlaneTool =
        XCAFDoc_DocumentTool::ClippingPlaneTool(theDoc->Main());
      aClippingExpression = buildClippingPlanes(aClippingCameraModel,
                                                aClippingPlanes,
                                                aClippingPlaneTool,
                                                aLocalFactors);
      anObj->SetClippingExpression(aClippingExpression);
    }

    // Collect shapes and GDTs
    if (aDModel.IsNull())
      return false;
    NCollection_Sequence<TDF_Label> aShapes, aGDTs;
    Interface_EntityIterator        anIter = aGraph.Shareds(aDModel);
    for (; anIter.More(); anIter.Next())
    {
      if (anIter.Value()->IsKind(STANDARD_TYPE(StepRepr_MappedItem)))
      {
        occ::handle<StepRepr_MappedItem> anItem =
          occ::down_cast<StepRepr_MappedItem>(anIter.Value());
        if (occ::handle<StepRepr_Representation> aRepr =
              anItem->MappingSource()->MappedRepresentation())
        {
          collectViewShapes(theWS, theDoc, aRepr, aShapes);
        }
      }
      else if (anIter.Value()->IsKind(STANDARD_TYPE(StepVisual_AnnotationOccurrence))
               || anIter.Value()->IsKind(STANDARD_TYPE(StepVisual_DraughtingCallout)))
      {
        Interface_EntityIterator aDMIAIter = aGraph.Sharings(anIter.Value());
        for (; aDMIAIter.More(); aDMIAIter.Next())
        {
          if (!aDMIAIter.Value()->IsKind(STANDARD_TYPE(StepAP242_DraughtingModelItemAssociation)))
            continue;
          occ::handle<StepAP242_DraughtingModelItemAssociation> aDMIA =
            occ::down_cast<StepAP242_DraughtingModelItemAssociation>(aDMIAIter.Value());
          TDF_Label aGDTL;
          bool      isFind = myGDTMap.Find(aDMIA->Definition().Value(), aGDTL);
          if (!isFind)
          {
            isFind = myGDTMap.Find(anIter.Value(), aGDTL);
          }
          if (isFind)
            aGDTs.Append(aGDTL);
        }
      }
      else if (anIter.Value()->IsKind(STANDARD_TYPE(StepVisual_AnnotationPlane)))
      {
        occ::handle<StepVisual_AnnotationPlane> aPlane =
          occ::down_cast<StepVisual_AnnotationPlane>(anIter.Value());
        for (int j = 1; j <= aPlane->NbElements(); j++)
        {
          Interface_EntityIterator aDMIAIter = aGraph.Sharings(anIter.Value());
          for (; aDMIAIter.More(); aDMIAIter.Next())
          {
            if (!aDMIAIter.Value()->IsKind(STANDARD_TYPE(StepAP242_DraughtingModelItemAssociation)))
              continue;
            occ::handle<StepAP242_DraughtingModelItemAssociation> aDMIA =
              occ::down_cast<StepAP242_DraughtingModelItemAssociation>(aDMIAIter.Value());
            TDF_Label aGDTL;
            bool      isFind = myGDTMap.Find(aDMIA->Definition().Value(), aGDTL);
            if (isFind)
              aGDTs.Append(aGDTL);
          }
        }
      }
    }
    TDF_Label                 aViewL = aViewTool->AddView();
    occ::handle<XCAFDoc_View> aView  = XCAFDoc_View::Set(aViewL);
    aView->SetObject(anObj);
    aViewTool->SetView(aShapes, aGDTs, aClippingPlanes, aViewL);
    aViewTool->Lock(aViewL);
  }
  return true;
}

//=================================================================================================

TDF_Label STEPCAFControl_Reader::SettleShapeData(
  const occ::handle<StepRepr_RepresentationItem>& theItem,
  const TDF_Label&                                theLab,
  const occ::handle<XCAFDoc_ShapeTool>&           theShapeTool,
  const occ::handle<Transfer_TransientProcess>&   TP) const
{
  TDF_Label aResult = theLab;
  if (theItem.IsNull())
    return aResult;

  occ::handle<TCollection_HAsciiString> hName = theItem->Name();
  if (hName.IsNull() || hName->IsEmpty())
    return aResult;

  occ::handle<Transfer_Binder> aBinder = TP->Find(theItem);
  if (aBinder.IsNull())
    return aResult;

  TopoDS_Shape aShape = TransferBRep::ShapeResult(aBinder);
  if (aShape.IsNull())
    return aResult;

  // Allocate sub-Label
  aResult = theShapeTool->AddSubShape(theLab, aShape);
  if (aResult.IsNull())
    return aResult;

  TCollection_AsciiString aName = hName->String();
  TDataStd_Name::Set(aResult, aName);
  theShapeTool->SetShape(aResult, aShape);

  return aResult;
}

//=======================================================================
// function : collectRepresentationItems
// purpose  : recursive collection of representation items for given representation
//           with all representations, related to it.
//=======================================================================

void collectRepresentationItems(
  const Interface_Graph&                                          theGraph,
  const occ::handle<StepShape_ShapeRepresentation>&               theRepresentation,
  NCollection_Sequence<occ::handle<StepRepr_RepresentationItem>>& theItems)
{
  for (NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>::Iterator anIter(
         theRepresentation->Items()->Array1());
       anIter.More();
       anIter.Next())
  {
    const occ::handle<StepRepr_RepresentationItem>& anReprItem = anIter.Value();
    if (anReprItem.IsNull())
    {
      continue;
    }
    theItems.Append(anReprItem);
  }

  Interface_EntityIterator entIt =
    theGraph.TypedSharings(theRepresentation, STANDARD_TYPE(StepRepr_RepresentationRelationship));
  for (entIt.Start(); entIt.More(); entIt.Next())
  {
    occ::handle<StepRepr_RepresentationRelationship> aRelationship =
      occ::down_cast<StepRepr_RepresentationRelationship>(entIt.Value());
    if (aRelationship->Rep1() == theRepresentation)
    {
      occ::handle<StepShape_ShapeRepresentation> aRepr =
        occ::down_cast<StepShape_ShapeRepresentation>(aRelationship->Rep2());
      if (!aRepr.IsNull())
        collectRepresentationItems(theGraph, aRepr, theItems);
    }
  }
}

//=================================================================================================

void STEPCAFControl_Reader::ExpandSubShapes(
  const occ::handle<XCAFDoc_ShapeTool>&               ShapeTool,
  const NCollection_DataMap<TopoDS_Shape,
                            occ::handle<StepBasic_ProductDefinition>,
                            TopTools_ShapeMapHasher>& ShapePDMap) const
{
  const occ::handle<Transfer_TransientProcess>& TP =
    Reader().WS()->TransferReader()->TransientProcess();
  NCollection_DataMap<TopoDS_Shape, occ::handle<TCollection_HAsciiString>> ShapeNameMap;
  NCollection_Map<occ::handle<Standard_Transient>>                         aRepItems;
  occ::handle<StepData_StepModel> aStepModel = occ::down_cast<StepData_StepModel>(TP->Model());
  if (!aStepModel->InternalParameters.ReadSubshapeNames)
    return;

  const Interface_Graph& Graph = Reader().WS()->Graph();

  for (NCollection_DataMap<TopoDS_Shape,
                           occ::handle<StepBasic_ProductDefinition>,
                           TopTools_ShapeMapHasher>::Iterator it(ShapePDMap);
       it.More();
       it.Next())
  {
    const TopoDS_Shape&                             aRootShape = it.Key();
    const occ::handle<StepBasic_ProductDefinition>& aPDef      = it.Value();
    if (aPDef.IsNull())
      continue;

    // Find SDR by Product
    occ::handle<StepShape_ShapeDefinitionRepresentation> aSDR;
    Interface_EntityIterator                             entIt =
      Graph.TypedSharings(aPDef, STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation));
    for (entIt.Start(); entIt.More(); entIt.Next())
    {
      const occ::handle<Standard_Transient>& aReferer = entIt.Value();
      aSDR = occ::down_cast<StepShape_ShapeDefinitionRepresentation>(aReferer);
      if (!aSDR.IsNull())
        break;
    }

    if (aSDR.IsNull())
      continue;

    // Access shape representation
    occ::handle<StepShape_ShapeRepresentation> aShapeRepr =
      occ::down_cast<StepShape_ShapeRepresentation>(aSDR->UsedRepresentation());

    if (aShapeRepr.IsNull())
      continue;

    // Access representation items
    NCollection_Sequence<occ::handle<StepRepr_RepresentationItem>> aReprItems;
    collectRepresentationItems(Graph, aShapeRepr, aReprItems);

    if (aReprItems.Length() == 0)
      continue;

    const TDF_Label* pRootLab = myMap.Seek(aRootShape);
    if (!pRootLab)
      continue;

    TDF_Label aRootLab = *pRootLab;
    // Do not add subshapes to assembly,
    // they will be processed with corresponding Shape_Product_Definition of necessary part.
    if (ShapeTool->IsAssembly(aRootLab))
      continue;

    NCollection_Sequence<occ::handle<StepRepr_RepresentationItem>> aMSBSeq;
    NCollection_Sequence<occ::handle<StepRepr_RepresentationItem>> aSBSMSeq;

    // Iterate over the top level representation items collecting the
    // topological containers to expand
    for (int i = 1; i <= aReprItems.Length(); ++i)
    {
      const occ::handle<StepRepr_RepresentationItem>& aTRepr = aReprItems.Value(i);
      if (aTRepr->IsKind(STANDARD_TYPE(StepShape_ManifoldSolidBrep)))
        aMSBSeq.Append(aTRepr);
      else if (aTRepr->IsKind(STANDARD_TYPE(StepShape_ShellBasedSurfaceModel)))
        aSBSMSeq.Append(aTRepr);
    }

    // Insert intermediate OCAF Labels for SOLIDs in case there are more
    // than one Manifold Solid BRep in the Shape Representation
    bool doInsertSolidLab = (aMSBSeq.Length() > 1);

    // Expand Manifold Solid BReps
    for (int i = 1; i <= aMSBSeq.Length(); ++i)
    {
      // Put additional Label for SOLID
      if (doInsertSolidLab)
        SettleShapeData(aMSBSeq.Value(i), aRootLab, ShapeTool, TP);

      ExpandManifoldSolidBrep(aRootLab, aMSBSeq.Value(i), TP, ShapeTool);
    }

    // Expand Shell-Based Surface Models
    for (int i = 1; i <= aSBSMSeq.Length(); ++i)
      ExpandSBSM(aRootLab, aSBSMSeq.Value(i), TP, ShapeTool);
  }
}

//=================================================================================================

void STEPCAFControl_Reader::ExpandManifoldSolidBrep(
  TDF_Label&                                      ShapeLab,
  const occ::handle<StepRepr_RepresentationItem>& Repr,
  const occ::handle<Transfer_TransientProcess>&   TP,
  const occ::handle<XCAFDoc_ShapeTool>&           ShapeTool) const
{
  // Access outer shell
  occ::handle<StepShape_ManifoldSolidBrep> aMSB = occ::down_cast<StepShape_ManifoldSolidBrep>(Repr);
  occ::handle<StepShape_ConnectedFaceSet>  aShell = aMSB->Outer();

  // Expand shell contents to CAF tree
  ExpandShell(aShell, ShapeLab, TP, ShapeTool);
}

//=================================================================================================

void STEPCAFControl_Reader::ExpandSBSM(TDF_Label&                                      ShapeLab,
                                       const occ::handle<StepRepr_RepresentationItem>& Repr,
                                       const occ::handle<Transfer_TransientProcess>&   TP,
                                       const occ::handle<XCAFDoc_ShapeTool>& ShapeTool) const
{
  occ::handle<StepShape_ShellBasedSurfaceModel> aSBSM =
    occ::down_cast<StepShape_ShellBasedSurfaceModel>(Repr);

  // Access boundary shells
  occ::handle<NCollection_HArray1<StepShape_Shell>> aShells = aSBSM->SbsmBoundary();
  for (int s = aShells->Lower(); s <= aShells->Upper(); ++s)
  {
    const StepShape_Shell&                  aShell = aShells->Value(s);
    occ::handle<StepShape_ConnectedFaceSet> aCFS;
    occ::handle<StepShape_OpenShell>        anOpenShell  = aShell.OpenShell();
    occ::handle<StepShape_ClosedShell>      aClosedShell = aShell.ClosedShell();

    if (!anOpenShell.IsNull())
      aCFS = anOpenShell;
    else
      aCFS = aClosedShell;

    ExpandShell(aCFS, ShapeLab, TP, ShapeTool);
  }
}

//=================================================================================================

void STEPCAFControl_Reader::ExpandShell(const occ::handle<StepShape_ConnectedFaceSet>& Shell,
                                        TDF_Label&                                     RootLab,
                                        const occ::handle<Transfer_TransientProcess>&  TP,
                                        const occ::handle<XCAFDoc_ShapeTool>& ShapeTool) const
{
  // Record CAF data
  SettleShapeData(Shell, RootLab, ShapeTool, TP);

  // Access faces
  occ::handle<NCollection_HArray1<occ::handle<StepShape_Face>>> aFaces = Shell->CfsFaces();
  for (int f = aFaces->Lower(); f <= aFaces->Upper(); ++f)
  {
    const occ::handle<StepShape_Face>& aFace = aFaces->Value(f);
    if (aFace.IsNull())
      continue;

    // Record CAF data
    SettleShapeData(aFace, RootLab, ShapeTool, TP);

    // Access face bounds
    occ::handle<NCollection_HArray1<occ::handle<StepShape_FaceBound>>> aWires = aFace->Bounds();
    if (aWires.IsNull())
      continue;
    for (int w = aWires->Lower(); w <= aWires->Upper(); ++w)
    {
      const occ::handle<StepShape_Loop>& aWire = aWires->Value(w)->Bound();

      // Record CAF data
      SettleShapeData(aWire, RootLab, ShapeTool, TP);

      // Access wire edges
      // Currently only EDGE LOOPs are considered (!)
      if (!aWire->IsInstance(STANDARD_TYPE(StepShape_EdgeLoop)))
        continue;

      // Access edges
      occ::handle<StepShape_EdgeLoop> anEdgeLoop = occ::down_cast<StepShape_EdgeLoop>(aWire);
      occ::handle<NCollection_HArray1<occ::handle<StepShape_OrientedEdge>>> anEdges =
        anEdgeLoop->EdgeList();
      for (int e = anEdges->Lower(); e <= anEdges->Upper(); ++e)
      {
        occ::handle<StepShape_OrientedEdge> anOrientedEdge = anEdges->Value(e);
        if (anOrientedEdge.IsNull())
          continue;
        occ::handle<StepShape_Edge> anEdge = anOrientedEdge->EdgeElement();
        if (anEdge.IsNull())
          continue;

        // Record CAF data
        SettleShapeData(anEdge, RootLab, ShapeTool, TP);

        // Access vertices
        occ::handle<StepShape_Vertex> aV1 = anEdge->EdgeStart();
        occ::handle<StepShape_Vertex> aV2 = anEdge->EdgeEnd();

        // Record CAF data
        SettleShapeData(aV1, RootLab, ShapeTool, TP);
        SettleShapeData(aV2, RootLab, ShapeTool, TP);
      }
    }
  }
}

//=================================================================================================

void STEPCAFControl_Reader::SetColorMode(const bool colormode)
{
  myColorMode = colormode;
}

//=================================================================================================

bool STEPCAFControl_Reader::GetColorMode() const
{
  return myColorMode;
}

//=================================================================================================

void STEPCAFControl_Reader::SetNameMode(const bool namemode)
{
  myNameMode = namemode;
}

//=================================================================================================

bool STEPCAFControl_Reader::GetNameMode() const
{
  return myNameMode;
}

//=================================================================================================

void STEPCAFControl_Reader::SetLayerMode(const bool layermode)
{
  myLayerMode = layermode;
}

//=================================================================================================

bool STEPCAFControl_Reader::GetLayerMode() const
{
  return myLayerMode;
}

//=================================================================================================

void STEPCAFControl_Reader::SetPropsMode(const bool propsmode)
{
  myPropsMode = propsmode;
}

//=================================================================================================

bool STEPCAFControl_Reader::GetPropsMode() const
{
  return myPropsMode;
}

//=================================================================================================

void STEPCAFControl_Reader::SetMetaMode(const bool theMetaMode)
{
  myMetaMode = theMetaMode;
}

//=================================================================================================

bool STEPCAFControl_Reader::GetMetaMode() const
{
  return myMetaMode;
}

//=================================================================================================

void STEPCAFControl_Reader::SetProductMetaMode(const bool theProductMetaMode)
{
  myProductMetaMode = theProductMetaMode;
}

//=================================================================================================

bool STEPCAFControl_Reader::GetProductMetaMode() const
{
  return myProductMetaMode;
}

//=================================================================================================

void STEPCAFControl_Reader::SetSHUOMode(const bool mode)
{
  mySHUOMode = mode;
}

//=================================================================================================

bool STEPCAFControl_Reader::GetSHUOMode() const
{
  return mySHUOMode;
}

//=================================================================================================

void STEPCAFControl_Reader::SetGDTMode(const bool gdtmode)
{
  myGDTMode = gdtmode;
}

//=================================================================================================

bool STEPCAFControl_Reader::GetGDTMode() const
{
  return myGDTMode;
}

//=================================================================================================

void STEPCAFControl_Reader::SetMatMode(const bool matmode)
{
  myMatMode = matmode;
}

//=================================================================================================

bool STEPCAFControl_Reader::GetMatMode() const
{
  return myMatMode;
}

//=================================================================================================

void STEPCAFControl_Reader::SetViewMode(const bool viewmode)
{
  myViewMode = viewmode;
}

//=================================================================================================

bool STEPCAFControl_Reader::GetViewMode() const
{
  return myViewMode;
}

//=============================================================================

void STEPCAFControl_Reader::SetShapeFixParameters(
  const XSAlgo_ShapeProcessor::ParameterMap& theParameters)
{
  myReader.SetShapeFixParameters(theParameters);
}

//=============================================================================

void STEPCAFControl_Reader::SetShapeFixParameters(
  XSAlgo_ShapeProcessor::ParameterMap&& theParameters)
{
  myReader.SetShapeFixParameters(std::move(theParameters));
}

//=============================================================================

void STEPCAFControl_Reader::SetShapeFixParameters(
  const DE_ShapeFixParameters&               theParameters,
  const XSAlgo_ShapeProcessor::ParameterMap& theAdditionalParameters)
{
  myReader.SetShapeFixParameters(theParameters, theAdditionalParameters);
}

//=============================================================================

const XSAlgo_ShapeProcessor::ParameterMap& STEPCAFControl_Reader::GetShapeFixParameters() const
{
  return myReader.GetShapeFixParameters();
}

//=============================================================================

void STEPCAFControl_Reader::SetShapeProcessFlags(const ShapeProcess::OperationsFlags& theFlags)
{
  return myReader.SetShapeProcessFlags(theFlags);
}

//=============================================================================

const XSAlgo_ShapeProcessor::ProcessingFlags& STEPCAFControl_Reader::GetShapeProcessFlags() const
{
  return myReader.GetShapeProcessFlags();
}

//=================================================================================================

TDF_Label STEPCAFControl_Reader::getShapeLabelFromProductDefinition(
  const occ::handle<Transfer_TransientProcess>&   theTransferProcess,
  const occ::handle<StepBasic_ProductDefinition>& theProductDefinition) const
{
  TDF_Label aShapeLabel;

  occ::handle<Transfer_Binder> aBinder = theTransferProcess->Find(theProductDefinition);
  if (aBinder.IsNull())
  {
    return aShapeLabel;
  }

  const TopoDS_Shape aShape = TransferBRep::ShapeResult(theTransferProcess, aBinder);
  if (aShape.IsNull())
  {
    return aShapeLabel;
  }

  myMap.Find(aShape, aShapeLabel);
  return aShapeLabel;
}

//=================================================================================================

occ::handle<StepBasic_Product> STEPCAFControl_Reader::getProductFromProductDefinition(
  const occ::handle<StepBasic_ProductDefinition>& theProductDefinition) const
{
  occ::handle<StepBasic_ProductDefinitionFormation> aFormation = theProductDefinition->Formation();
  if (aFormation.IsNull())
  {
    return nullptr;
  }

  return aFormation->OfProduct();
}

//=================================================================================================

std::vector<occ::handle<StepRepr_PropertyDefinition>> STEPCAFControl_Reader::
  collectPropertyDefinitions(const occ::handle<XSControl_WorkSession>& theWorkSession,
                             const occ::handle<Standard_Transient>&    theGeneralProperty) const
{
  std::vector<occ::handle<StepRepr_PropertyDefinition>> aResult;

  occ::handle<StepBasic_GeneralProperty> aGeneralProp =
    occ::down_cast<StepBasic_GeneralProperty>(theGeneralProperty);
  if (aGeneralProp.IsNull())
  {
    return aResult;
  }

  Interface_EntityIterator aSharingListOfGP = theWorkSession->Graph().Sharings(aGeneralProp);
  for (aSharingListOfGP.Start(); aSharingListOfGP.More(); aSharingListOfGP.Next())
  {
    occ::handle<StepBasic_GeneralPropertyAssociation> aPropAssociation =
      occ::down_cast<StepBasic_GeneralPropertyAssociation>(aSharingListOfGP.Value());
    if (aPropAssociation.IsNull())
    {
      continue;
    }

    aResult.emplace_back(aPropAssociation->PropertyDefinition());
  }

  return aResult;
}

//=================================================================================================

std::vector<TDF_Label> STEPCAFControl_Reader::collectShapeLabels(
  const occ::handle<XSControl_WorkSession>&       theWorkSession,
  const occ::handle<Transfer_TransientProcess>&   theTransferProcess,
  const occ::handle<StepRepr_PropertyDefinition>& theSource) const
{
  std::vector<TDF_Label> aResult;

  NCollection_List<occ::handle<Transfer_Binder>> aBinders;
  collectBinders(theWorkSession, theTransferProcess, theSource, aBinders);
  if (aBinders.IsEmpty())
  {
    return aResult;
  }

  NCollection_List<occ::handle<Transfer_Binder>>::Iterator aBindIt(aBinders);
  for (; aBindIt.More(); aBindIt.Next())
  {
    const TopoDS_Shape aShape = TransferBRep::ShapeResult(theTransferProcess, aBindIt.Value());
    if (aShape.IsNull())
    {
      continue;
    }

    TDF_Label aShapeResultLabel;
    if (myMap.Find(aShape, aShapeResultLabel))
    {
      aResult.emplace_back(aShapeResultLabel);
    }
  }

  return aResult;
}

//=================================================================================================

std::vector<occ::handle<StepRepr_PropertyDefinition>> STEPCAFControl_Reader::
  collectRelatedPropertyDefinitions(
    const occ::handle<XSControl_WorkSession>&       theWorkSession,
    const occ::handle<StepRepr_PropertyDefinition>& theProperty) const
{
  std::vector<occ::handle<StepRepr_PropertyDefinition>> aGroupedProperties;
  aGroupedProperties.emplace_back(theProperty);

  Interface_EntityIterator aSharingsListOfPD = theWorkSession->Graph().Sharings(theProperty);
  for (aSharingsListOfPD.Start(); aSharingsListOfPD.More(); aSharingsListOfPD.Next())
  {
    occ::handle<StepRepr_PropertyDefinitionRelationship> aRel =
      occ::down_cast<StepRepr_PropertyDefinitionRelationship>(aSharingsListOfPD.Value());
    if (aRel.IsNull())
    {
      continue;
    }

    occ::handle<StepRepr_PropertyDefinition> aGroupedProp = aRel->RelatedPropertyDefinition();
    if (!aGroupedProp.IsNull())
    {
      aGroupedProperties.emplace_back(aGroupedProp);
    }
  }

  return aGroupedProperties;
}

//=================================================================================================

occ::handle<TDataStd_NamedData> STEPCAFControl_Reader::getNamedData(const TDF_Label& theLabel) const
{
  occ::handle<TDataStd_NamedData> anAttribute;
  if (!theLabel.FindAttribute(TDataStd_NamedData::GetID(), anAttribute))
  {
    anAttribute = new TDataStd_NamedData;
    theLabel.AddAttribute(anAttribute);
  }
  return anAttribute;
}

//=================================================================================================

void STEPCAFControl_Reader::collectBinders(
  const occ::handle<XSControl_WorkSession>&       theWorkSession,
  const occ::handle<Transfer_TransientProcess>&   theTransientProcess,
  const occ::handle<StepRepr_PropertyDefinition>& theSource,
  NCollection_List<occ::handle<Transfer_Binder>>& theBinders) const
{
  Interface_EntityIterator aSharedListOfPD = theWorkSession->Graph().Shareds(theSource);
  for (aSharedListOfPD.Start(); aSharedListOfPD.More(); aSharedListOfPD.Next())
  {
    // Acquire binder from StepBasic_ProductDefinition.
    occ::handle<StepBasic_ProductDefinition> aProductDefinition =
      occ::down_cast<StepBasic_ProductDefinition>(aSharedListOfPD.Value());
    if (!aProductDefinition.IsNull())
    {
      const occ::handle<Transfer_Binder> aBinder = theTransientProcess->Find(aProductDefinition);
      if (!aBinder.IsNull())
      {
        theBinders.Append(aBinder);
      }
      continue;
    }

    // Acquire binder from StepRepr_ProductDefinitionShape.
    occ::handle<StepRepr_ProductDefinitionShape> aProductDefinitionShape =
      occ::down_cast<StepRepr_ProductDefinitionShape>(aSharedListOfPD.Value());
    if (!aProductDefinitionShape.IsNull())
    {
      occ::handle<StepBasic_ProductDefinition> aProductDef =
        aProductDefinitionShape->Definition().ProductDefinition();
      const occ::handle<Transfer_Binder> aBinder = theTransientProcess->Find(aProductDef);
      if (!aBinder.IsNull())
      {
        theBinders.Append(aBinder);
      }
      continue;
    }

    // Acquire binder from StepRepr_NextAssemblyUsageOccurrence.
    occ::handle<StepRepr_NextAssemblyUsageOccurrence> aNextAssembUsOcc =
      occ::down_cast<StepRepr_NextAssemblyUsageOccurrence>(aSharedListOfPD.Value());
    if (!aNextAssembUsOcc.IsNull())
    {
      const occ::handle<Transfer_Binder> aBinder = theTransientProcess->Find(aNextAssembUsOcc);
      if (!aBinder.IsNull())
      {
        theBinders.Append(aBinder);
      }
      continue;
    }

    occ::handle<StepRepr_ShapeAspect> aShapeAspect =
      occ::down_cast<StepRepr_ShapeAspect>(aSharedListOfPD.Value());
    if (!aShapeAspect.IsNull())
    {
      Interface_EntityIterator aSharedListOfSA = theWorkSession->Graph().Sharings(aShapeAspect);
      for (aSharedListOfSA.Start(); aSharedListOfSA.More(); aSharedListOfSA.Next())
      {
        occ::handle<StepAP242_DraughtingModelItemAssociation> aDMIA =
          occ::down_cast<StepAP242_DraughtingModelItemAssociation>(aSharedListOfSA.Value());
        if (!aDMIA.IsNull())
        {
          break;
        }

        occ::handle<StepAP242_ItemIdentifiedRepresentationUsage> anItemIdentUsage =
          occ::down_cast<StepAP242_ItemIdentifiedRepresentationUsage>(aSharedListOfSA.Value());
        if (!anItemIdentUsage.IsNull())
        {
          for (int anIndex = 1; anIndex <= anItemIdentUsage->NbIdentifiedItem(); ++anIndex)
          {
            occ::handle<StepRepr_RepresentationItem> aReprItem =
              anItemIdentUsage->IdentifiedItemValue(anIndex);
            if (aReprItem.IsNull())
            {
              continue;
            }

            const occ::handle<Transfer_Binder> aBinder = theTransientProcess->Find(aReprItem);
            if (!aBinder.IsNull())
            {
              theBinders.Append(aBinder);
            }
          }
          continue;
        }

        occ::handle<StepRepr_PropertyDefinition> aPropDef =
          occ::down_cast<StepRepr_PropertyDefinition>(aSharedListOfSA.Value());
        if (!aPropDef.IsNull() && aPropDef != theSource)
        {
          Interface_EntityIterator aSharingListOfPD = theWorkSession->Graph().Sharings(aPropDef);
          for (aSharingListOfPD.Start(); aSharingListOfPD.More(); aSharingListOfPD.Next())
          {
            occ::handle<StepShape_ShapeDefinitionRepresentation> aShDef =
              occ::down_cast<StepShape_ShapeDefinitionRepresentation>(aSharingListOfPD.Value());
            if (aShDef.IsNull())
            {
              continue;
            }

            findReprItems(theWorkSession, aShDef, theBinders);
          }
          continue;
        }

        occ::handle<StepShape_ShapeDefinitionRepresentation> aShapeDefRepr =
          occ::down_cast<StepShape_ShapeDefinitionRepresentation>(aSharedListOfSA.Value());
        if (!aShapeDefRepr.IsNull())
        {
          findReprItems(theWorkSession, aShapeDefRepr, theBinders);
        }
      }
    }
  }
}

//=================================================================================================

bool STEPCAFControl_Reader::ReadMetadata(const occ::handle<XSControl_WorkSession>& theWS,
                                         const occ::handle<TDocStd_Document>&      theDoc,
                                         const StepData_Factors& theLocalFactors) const
{
  if (XCAFDoc_DocumentTool::ShapeTool(theDoc->Main()).IsNull())
  {
    return false;
  }

  const occ::handle<Interface_InterfaceModel>&  aModel = theWS->Model();
  const occ::handle<Transfer_TransientProcess>& aTransientProcess =
    theWS->TransferReader()->TransientProcess();

  for (int anEntityInd = 1; anEntityInd <= aModel->NbEntities(); ++anEntityInd)
  {
    for (const auto& aPropertyDefinition :
         collectPropertyDefinitions(theWS, aModel->Value(anEntityInd)))
    {
      // Collect all the related PropertyDefinitions.
      const std::vector<occ::handle<StepRepr_PropertyDefinition>> aGroupedProperties =
        collectRelatedPropertyDefinitions(theWS, aPropertyDefinition);

      // Collect all the shapes labels.
      const std::vector<TDF_Label> aLabels =
        collectShapeLabels(theWS, aTransientProcess, aPropertyDefinition);

      // Fill all attributes for each shape label.
      for (const auto& aLabel : aLabels)
      {
        occ::handle<TDataStd_NamedData> anAttribute = getNamedData(aLabel);

        for (const auto& aPropDefinition : aGroupedProperties)
        {
          fillAttributes(theWS, aPropDefinition, theLocalFactors, anAttribute);
        }
      }
    }
  }

  return true;
}

//=================================================================================================

bool STEPCAFControl_Reader::ReadProductMetadata(const occ::handle<XSControl_WorkSession>& theWS,
                                                const occ::handle<TDocStd_Document>& theDoc) const
{
  if (XCAFDoc_DocumentTool::ShapeTool(theDoc->Main()).IsNull())
  {
    return false;
  }

  const occ::handle<Interface_InterfaceModel>&  aModel = theWS->Model();
  const occ::handle<Transfer_TransientProcess>& aTransientProcess =
    theWS->TransferReader()->TransientProcess();

  for (int anEntityInd = 1; anEntityInd <= aModel->NbEntities(); ++anEntityInd)
  {
    // Processing only StepBasic_ProductDefinition entities.
    const occ::handle<StepBasic_ProductDefinition> aProdDefinition =
      occ::down_cast<StepBasic_ProductDefinition>(aModel->Value(anEntityInd));
    if (aProdDefinition.IsNull())
    {
      continue;
    }

    // Prepare required data for processing: ShapeLabel and Product entity.
    const TDF_Label aShapeLabel =
      getShapeLabelFromProductDefinition(aTransientProcess, aProdDefinition);
    if (aShapeLabel.IsNull())
    {
      continue;
    }
    const occ::handle<StepBasic_Product> aProduct =
      getProductFromProductDefinition(aProdDefinition);
    if (aProduct.IsNull())
    {
      continue;
    }

    // Find or create NamedData attribute for ShapeLabel.
    occ::handle<TDataStd_NamedData> anAttribute = getNamedData(aShapeLabel);
    // Fill NamedData attribute with product metadata.
    if (aProduct->Id() && !aProduct->Id()->String().IsEmpty())
    {
      anAttribute->SetString("ProductID", aProduct->Id()->String());
    }
    if (aProduct->Name() && !aProduct->Name()->String().IsEmpty())
    {
      anAttribute->SetString("ProductName", aProduct->Name()->String());
    }
    if (aProduct->Description() && !aProduct->Description()->String().IsEmpty())
    {
      anAttribute->SetString("Description", aProduct->Description()->String());
    }
    if (aProdDefinition->Description() && !aProdDefinition->Description()->String().IsEmpty())
    {
      anAttribute->SetString("ProductDefinition", aProdDefinition->Description()->String());
    }
  }

  return true;
}

//=================================================================================================

bool STEPCAFControl_Reader::findReprItems(
  const occ::handle<XSControl_WorkSession>&                   theWS,
  const occ::handle<StepShape_ShapeDefinitionRepresentation>& theShDefRepr,
  NCollection_List<occ::handle<Transfer_Binder>>&             theBinders) const
{
  occ::handle<StepRepr_Representation> aRepr = theShDefRepr->UsedRepresentation();
  if (aRepr.IsNull())
  {
    return false;
  }

  occ::handle<Transfer_Binder>                  aBinder;
  const occ::handle<Transfer_TransientProcess>& aTP = theWS->TransferReader()->TransientProcess();
  occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> aReprItems =
    aRepr->Items();
  if (!aReprItems->IsEmpty())
  {
    for (int anInd = aReprItems->Lower(); anInd <= aReprItems->Upper(); anInd++)
    {
      occ::handle<StepRepr_RepresentationItem> aReprItem = aReprItems->Value(anInd);
      aBinder                                            = aTP->Find(aReprItem);
      if (!aBinder.IsNull())
      {
        theBinders.Append(aBinder);
      }
    }
  }

  return true;
}

//=================================================================================================

bool STEPCAFControl_Reader::fillAttributes(
  const occ::handle<XSControl_WorkSession>&       theWS,
  const occ::handle<StepRepr_PropertyDefinition>& thePropDef,
  const StepData_Factors&                         theLocalFactors,
  occ::handle<TDataStd_NamedData>&                theAttr) const
{
  // Skip if property definition or name is null
  if (thePropDef.IsNull() || thePropDef->Name().IsNull())
  {
    return false;
  }

  const TCollection_AsciiString& aPropName = thePropDef->Name()->String();

  Interface_EntityIterator aSharingListOfPD = theWS->Graph().Sharings(thePropDef);
  for (aSharingListOfPD.Start(); aSharingListOfPD.More(); aSharingListOfPD.Next())
  {
    occ::handle<StepRepr_PropertyDefinitionRepresentation> aPropDefRepr =
      occ::down_cast<StepRepr_PropertyDefinitionRepresentation>(aSharingListOfPD.Value());
    if (aPropDefRepr.IsNull())
      continue;

    occ::handle<StepRepr_Representation> aUsedRepr = aPropDefRepr->UsedRepresentation();
    if (aUsedRepr.IsNull())
      continue;

    occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> aReprItems =
      aUsedRepr->Items();
    if (aReprItems.IsNull())
      continue;

    for (int anIndex = aReprItems->Lower(); anIndex <= aReprItems->Upper(); anIndex++)
    {
      occ::handle<StepRepr_RepresentationItem> anItem = aReprItems->Value(anIndex);
      if (anItem.IsNull())
        continue;

      if (anItem->IsKind(STANDARD_TYPE(StepRepr_DescriptiveRepresentationItem)))
      {
        occ::handle<StepRepr_DescriptiveRepresentationItem> aDescrItem =
          occ::down_cast<StepRepr_DescriptiveRepresentationItem>(anItem);
        occ::handle<TCollection_HAsciiString> aDescription = aDescrItem->Description();
        if (!aDescription.IsNull())
        {
          theAttr->SetString(aPropName, aDescription->ToCString());
        }
        continue;
      }

      if (anItem->IsKind(STANDARD_TYPE(StepRepr_MeasureRepresentationItem)))
      {
        occ::handle<StepRepr_MeasureRepresentationItem> aMeasureItem =
          occ::down_cast<StepRepr_MeasureRepresentationItem>(anItem);
        if (aMeasureItem->Measure().IsNull()
            || aMeasureItem->Measure()->ValueComponentMember().IsNull())
        {
          continue;
        }

        double                  aValue   = aMeasureItem->Measure()->ValueComponent();
        TCollection_AsciiString aValType = aMeasureItem->Measure()->ValueComponentMember()->Name();
        StepBasic_Unit          anUnit   = aMeasureItem->Measure()->UnitComponent();

        if (!anUnit.IsNull())
        {
          if (anUnit.Type() == STANDARD_TYPE(StepBasic_DerivedUnit))
          {
            occ::handle<StepBasic_DerivedUnit> aDUnit = anUnit.DerivedUnit();
            if (!aDUnit.IsNull())
            {
              double aParam = 1.;
              for (int anInd = 1; anInd <= aDUnit->NbElements(); ++anInd)
              {
                occ::handle<StepBasic_DerivedUnitElement> aDUElem = aDUnit->ElementsValue(anInd);
                if (aDUElem.IsNull())
                  continue;

                double                           anExp  = aDUElem->Exponent();
                occ::handle<StepBasic_NamedUnit> aNUnit = aDUElem->Unit();

                if (!aNUnit.IsNull())
                {
                  if (aNUnit->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndLengthUnit))
                      || aNUnit->IsKind(STANDARD_TYPE(StepBasic_SiUnitAndLengthUnit)))
                  {
                    STEPConstruct_UnitContext anUnitCtx;
                    anUnitCtx.ComputeFactors(aNUnit, theLocalFactors);
                    if (aValType == "VOLUME_MEASURE")
                    {
                      aParam = pow(theLocalFactors.LengthFactor(), anExp);
                    }
                    else
                    {
                      aParam = aParam / pow(theLocalFactors.LengthFactor(), anExp);
                    }
                  }
                  else if (aNUnit->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndMassUnit)))
                  {
                    double aFact = 1.;
                    if (GetMassConversionFactor(aNUnit, aFact))
                    {
                      aParam = aParam * aFact;
                    }
                  }
                  else
                  {
                    STEPConstruct_UnitContext anUnitCtx;
                    anUnitCtx.ComputeFactors(aNUnit, theLocalFactors);
                    if (anUnitCtx.AreaDone())
                      aParam = anUnitCtx.AreaFactor();
                    if (anUnitCtx.LengthDone())
                    {
                      double aLengthFactor = anUnitCtx.LengthFactor();
                      aParam *= pow(aLengthFactor, anExp);
                    }
                  }
                }
              }
              aValue = aValue * aParam;
            }
          }
          else
          {
            occ::handle<StepBasic_NamedUnit> aNUnit = anUnit.NamedUnit();
            if (!aNUnit.IsNull())
            {
              double aParam = 1.;
              if (aNUnit->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndMassUnit)))
              {
                double aFact = 1.;
                if (GetMassConversionFactor(aNUnit, aFact))
                {
                  aValue *= aFact;
                }
              }
              else
              {
                STEPConstruct_UnitContext anUnitCtx;
                anUnitCtx.ComputeFactors(aNUnit, theLocalFactors);
                if (anUnitCtx.AreaDone())
                  aParam = anUnitCtx.AreaFactor();
                if (anUnitCtx.VolumeDone())
                  aParam = anUnitCtx.VolumeFactor();
                if (anUnitCtx.LengthDone())
                  aParam = anUnitCtx.LengthFactor();

                aValue *= aParam;
              }
            }
          }
        }
        theAttr->SetReal(aPropName, aValue);
        continue;
      }

      if (anItem->IsKind(STANDARD_TYPE(StepRepr_ValueRepresentationItem)))
      {
        occ::handle<StepRepr_ValueRepresentationItem> aValueItem =
          occ::down_cast<StepRepr_ValueRepresentationItem>(anItem);
        occ::handle<StepBasic_MeasureValueMember> aMeasureValueMem =
          aValueItem->ValueComponentMember();

        if (!aMeasureValueMem.IsNull())
        {
          Interface_ParamType aParamType = aMeasureValueMem->ParamType();
          if (aParamType == Interface_ParamInteger)
          {
            theAttr->SetInteger(aPropName, aMeasureValueMem->Integer());
          }
          else if (aParamType == Interface_ParamReal)
          {
            theAttr->SetReal(aPropName, aMeasureValueMem->Real());
          }
        }
        continue;
      }

      if (anItem->IsKind(STANDARD_TYPE(StepRepr_IntegerRepresentationItem)))
      {
        occ::handle<StepRepr_IntegerRepresentationItem> anIntegerItem =
          occ::down_cast<StepRepr_IntegerRepresentationItem>(anItem);
        theAttr->SetInteger(aPropName, anIntegerItem->Value());
        continue;
      }

      if (anItem->IsKind(STANDARD_TYPE(StepRepr_RealRepresentationItem)))
      {
        occ::handle<StepRepr_RealRepresentationItem> aRealItem =
          occ::down_cast<StepRepr_RealRepresentationItem>(anItem);
        theAttr->SetReal(aPropName, aRealItem->Value());
        continue;
      }

      if (anItem->IsKind(STANDARD_TYPE(StepRepr_BooleanRepresentationItem)))
      {
        occ::handle<StepRepr_BooleanRepresentationItem> aBoolItem =
          occ::down_cast<StepRepr_BooleanRepresentationItem>(anItem);
        theAttr->SetInteger(aPropName, aBoolItem->Value());
        continue;
      }
    }
  }
  return true;
}
