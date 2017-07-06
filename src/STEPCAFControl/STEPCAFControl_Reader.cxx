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


#include <BRep_Builder.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Plane.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_InterfaceModel.hxx>
#include <StepData_StepModel.hxx>
#include <HeaderSection_FileSchema.hxx>
#include <Interface_Static.hxx>
#include <NCollection_DataMap.hxx>
#include <OSD_Path.hxx>
#include <Quantity_Color.hxx>
#include <StepAP214_AppliedExternalIdentificationAssignment.hxx>
#include <StepBasic_ConversionBasedUnitAndLengthUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndPlaneAngleUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndMassUnit.hxx>
#include <StepBasic_DerivedUnit.hxx>
#include <StepBasic_DerivedUnitElement.hxx>
#include <StepBasic_MeasureValueMember.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepBasic_NamedUnit.hxx>
#include <StepBasic_Product.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepBasic_ProductDefinitionRelationship.hxx>
#include <StepBasic_SiUnit.hxx>
#include <StepBasic_SiUnitAndLengthUnit.hxx>
#include <StepBasic_Unit.hxx>
#include <STEPCAFControl_Controller.hxx>
#include <STEPCAFControl_DataMapIteratorOfDataMapOfShapePD.hxx>
#include <STEPCAFControl_DataMapOfPDExternFile.hxx>
#include <STEPCAFControl_DataMapOfSDRExternFile.hxx>
#include <STEPCAFControl_DataMapOfShapePD.hxx>
#include <STEPCAFControl_ExternFile.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <STEPConstruct.hxx>
#include <STEPConstruct_Assembly.hxx>
#include <STEPConstruct_ExternRefs.hxx>
#include <STEPConstruct_Styles.hxx>
#include <STEPConstruct_Tool.hxx>
#include <STEPConstruct_UnitContext.hxx>
#include <STEPConstruct_ValidationProps.hxx>
#include <STEPControl_Reader.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_Direction.hxx>
#include <StepDimTol_AngularityTolerance.hxx>
#include <StepDimTol_CircularRunoutTolerance.hxx>
#include <StepDimTol_CoaxialityTolerance.hxx>
#include <StepDimTol_ConcentricityTolerance.hxx>
#include <StepDimTol_CylindricityTolerance.hxx>
#include <StepDimTol_Datum.hxx>
#include <StepDimTol_DatumFeature.hxx>
#include <StepDimTol_DatumReference.hxx>
#include <StepDimTol_DatumReferenceElement.hxx>
#include <StepDimTol_DatumReferenceCompartment.hxx>
#include <StepDimTol_DatumSystem.hxx>
#include <StepDimTol_FlatnessTolerance.hxx>
#include <StepDimTol_GeometricTolerance.hxx>
#include <StepDimTol_GeometricToleranceWithDatumReference.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthDatRef.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthMaxTol.hxx>
#include <StepDimTol_HArray1OfDatumReference.hxx>
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
#include <StepDimTol_GeometricToleranceWithModifiers.hxx>
#include <StepDimTol_HArray1OfGeometricToleranceModifier.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthMod.hxx>
#include <StepDimTol_GeometricToleranceWithMaximumTolerance.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext.hxx>
#include <StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx.hxx>
#include <StepGeom_Plane.hxx>
#include <StepGeom_Polyline.hxx>
#include <StepDimTol_PlacedDatumTargetFeature.hxx>
#include <StepRepr_AssemblyComponentUsage.hxx>
#include <StepRepr_CharacterizedDefinition.hxx>
#include <StepRepr_CompoundRepresentationItem.hxx>
#include <StepRepr_DerivedShapeAspect.hxx>
#include <StepRepr_DescriptiveRepresentationItem.hxx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <StepRepr_MappedItem.hxx>
#include <StepRepr_MeasureRepresentationItem.hxx>
#include <StepRepr_NextAssemblyUsageOccurrence.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_PropertyDefinition.hxx>
#include <StepRepr_PropertyDefinitionRepresentation.hxx>
#include <StepRepr_Representation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <StepRepr_RepresentationMap.hxx>
#include <StepRepr_RepresentationRelationship.hxx>
#include <StepRepr_RepresentedDefinition.hxx>
#include <StepRepr_ReprItemAndLengthMeasureWithUnit.hxx>
#include <StepRepr_ReprItemAndPlaneAngleMeasureWithUnit.hxx>
#include <StepRepr_SequenceOfRepresentationItem.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepRepr_ShapeAspectDerivingRelationship.hxx>
#include <StepRepr_CompositeShapeAspect.hxx>
#include <StepRepr_AllAroundShapeAspect.hxx>
#include <StepRepr_CompShAspAndDatumFeatAndShAsp.hxx>
#include <StepRepr_ShapeAspectRelationship.hxx>
#include <StepRepr_ShapeRepresentationRelationship.hxx>
#include <StepRepr_SpecifiedHigherUsageOccurrence.hxx>
#include <StepRepr_ValueRange.hxx>
#include <StepRepr_FeatureForDatumTargetRelationship.hxx>
#include <StepShape_AdvancedFace.hxx>
#include <StepShape_AdvancedBrepShapeRepresentation.hxx>
#include <StepShape_AngleRelator.hxx>
#include <StepShape_AngularSize.hxx>
#include <StepShape_AngularLocation.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_ConnectedFaceSet.hxx>
#include <StepShape_ContextDependentShapeRepresentation.hxx>
#include <StepRepr_CompGroupShAspAndCompShAspAndDatumFeatAndShAsp.hxx>
#include <StepRepr_CompShAspAndDatumFeatAndShAsp.hxx>
#include <StepShape_DimensionalCharacteristicRepresentation.hxx>
#include <StepShape_DimensionalSizeWithPath.hxx>
#include <StepShape_DimensionalLocationWithPath.hxx>
#include <StepShape_ShapeDimensionRepresentationItem.hxx>
#include <StepShape_ShapeRepresentationWithParameters.hxx>
#include <StepShape_HArray1OfShapeDimensionRepresentationItem.hxx>
#include <StepShape_DimensionalSize.hxx>
#include <StepShape_DimensionalLocation.hxx>
#include <StepShape_EdgeCurve.hxx>
#include <StepShape_EdgeLoop.hxx>
#include <StepShape_GeometricCurveSet.hxx>
#include <StepShape_GeometricSet.hxx>
#include <StepShape_HArray1OfFace.hxx>
#include <StepShape_HArray1OfFaceBound.hxx>
#include <StepShape_HArray1OfOrientedEdge.hxx>
#include <StepShape_HArray1OfShell.hxx>
#include <StepShape_LimitsAndFits.hxx>
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
#include <StepVisual_AnnotationCurveOccurrence.hxx>
#include <StepVisual_AnnotationFillArea.hxx>
#include <StepVisual_AnnotationPlane.hxx>
#include <StepVisual_CameraModelD3.hxx>
#include <StepVisual_CameraModelD3MultiClipping.hxx>
#include <StepVisual_CameraModelD3MultiClippingIntersection.hxx>
#include <StepVisual_CameraModelD3MultiClippingUnion.hxx>
#include <StepVisual_CoordinatesList.hxx>
#include <StepVisual_HArray1OfCameraModelD3MultiClippingInterectionSelect.hxx>
#include <StepVisual_HArray1OfCameraModelD3MultiClippingUnionSelect.hxx>
#include <StepVisual_DraughtingCallout.hxx>
#include <StepVisual_DraughtingCalloutElement.hxx>
#include <StepVisual_DraughtingModel.hxx>
#include <StepVisual_Invisibility.hxx>
#include <StepVisual_LayeredItem.hxx>
#include <StepVisual_PlanarBox.hxx>
#include <StepVisual_PresentationLayerAssignment.hxx>
#include <StepVisual_PresentationStyleByContext.hxx>
#include <StepVisual_StyleContextSelect.hxx>
#include <StepVisual_StyledItem.hxx>
#include <StepVisual_ViewVolume.hxx>
#include <StepShape_TypeQualifier.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfTransient.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TColStd_IndexedDataMapOfTransientTransient.hxx>
#include <TColStd_MapIteratorOfMapOfTransient.hxx>
#include <TColStd_MapOfTransient.hxx>
#include <TColStd_SequenceOfHAsciiString.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Document.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>
#include <XCAFDoc.hxx>
#include <XCAFDoc_Area.hxx>
#include <XCAFDoc_Centroid.hxx>
#include <XCAFDoc_ClippingPlaneTool.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_DataMapOfShapeLabel.hxx>
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
#include <XCAFDimTolObjects_DimensionModifiersSequence.hxx>
#include <XCAFDimTolObjects_GeomToleranceType.hxx>
#include <XCAFDimTolObjects_DimensionObject.hxx>
#include <XCAFDimTolObjects_GeomToleranceObject.hxx>
#include <XCAFDimTolObjects_DatumObject.hxx>
#include <XCAFView_Object.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>
#include <StepAP242_DraughtingModelItemAssociation.hxx>
#include <StepAP242_GeometricItemSpecificUsage.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <STEPCAFControl_GDTProperty.hxx>
#include <StepVisual_TessellatedAnnotationOccurrence.hxx>
#include <StepVisual_TessellatedAnnotationOccurrence.hxx>
#include <StepVisual_TessellatedItem.hxx>
#include <StepVisual_TessellatedGeometricSet.hxx>
#include <StepVisual_TessellatedCurveSet.hxx>
#include <StepVisual_CoordinatesList.hxx>
#include <NCollection_Vector.hxx>

#include <TColgp_HArray1OfXYZ.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepTools.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>

// skl 21.08.2003 for reading G&DT
//#include <StepRepr_CompoundItemDefinition.hxx>
//#include <StepRepr_CompoundItemDefinitionMember.hxx>
//#include <StepBasic_ConversionBasedUnit.hxx>
//#include <TDataStd_Real.hxx>
//#include <TDataStd_Constraint.hxx>
//#include <TDataStd_ConstraintEnum.hxx>
//#include <TNaming_Tool.hxx>
//#include <AIS_InteractiveObject.hxx>
//#include <TPrsStd_ConstraintTools.hxx>
//#include <AIS_DiameterDimension.hxx>
//#include <TPrsStd_Position.hxx>
//#include <TPrsStd_AISPresentation.hxx>
//#include <TNaming_Builder.hxx>
#ifdef OCCT_DEBUG
//! Converts address of the passed shape (TShape) to string.
//! \param theShape [in] Shape to dump.
//! \return corresponding string.
TCollection_AsciiString AddrToString(const TopoDS_Shape& theShape)
{
  std::string anAddrStr;
  std::ostringstream ost;
  ost << theShape.TShape().get();
  anAddrStr = ost.str();

  TCollection_AsciiString aStr =
    TCollection_AsciiString("[").Cat( anAddrStr.c_str() ).Cat("]");

  return aStr;
}
#endif

//=======================================================================
//function : AllocateSubLabel
//purpose  :
//=======================================================================

static TDF_Label AllocateSubLabel(TDF_Label& theRoot)
{
  return TDF_TagSource::NewChild(theRoot);
}

//=======================================================================
//function : STEPCAFControl_Reader
//purpose  : 
//=======================================================================

STEPCAFControl_Reader::STEPCAFControl_Reader ():
       myColorMode( Standard_True ),
       myNameMode ( Standard_True ),
       myLayerMode( Standard_True ),
       myPropsMode( Standard_True ),
	   mySHUOMode ( Standard_False ),
       myGDTMode  ( Standard_True ),
       myMatMode(Standard_True),
       myViewMode(Standard_True)
{
  STEPCAFControl_Controller::Init();
}


//=======================================================================
//function : STEPCAFControl_Reader
//purpose  : 
//=======================================================================

STEPCAFControl_Reader::STEPCAFControl_Reader (const Handle(XSControl_WorkSession)& WS,
                                              const Standard_Boolean scratch) :
       myColorMode( Standard_True ),
       myNameMode ( Standard_True ),
       myLayerMode( Standard_True ),
       myPropsMode( Standard_True ),
	   mySHUOMode ( Standard_False ),
       myGDTMode  ( Standard_True ),
       myMatMode(Standard_True),
       myViewMode(Standard_True)
{
  STEPCAFControl_Controller::Init();
  Init ( WS, scratch );
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void STEPCAFControl_Reader::Init (const Handle(XSControl_WorkSession)& WS,
				  const Standard_Boolean scratch)
{
// necessary only in Writer, to set good actor:  WS->SelectNorm ( "STEP" );
  myReader.SetWS (WS,scratch);
  myFiles.Clear();
}


//=======================================================================
//function : ReadFile
//purpose  : 
//=======================================================================

IFSelect_ReturnStatus STEPCAFControl_Reader::ReadFile (const Standard_CString filename)
{
  return myReader.ReadFile ( filename );
}


//=======================================================================
//function : NbRootsForTransfer
//purpose  : 
//=======================================================================

Standard_Integer STEPCAFControl_Reader::NbRootsForTransfer () 
{
  return myReader.NbRootsForTransfer();
}


//=======================================================================
//function : TransferOneRoot
//purpose  : 
//=======================================================================

Standard_Boolean STEPCAFControl_Reader::TransferOneRoot (const Standard_Integer num,
						         Handle(TDocStd_Document) &doc) 
{
  TDF_LabelSequence Lseq;
  return Transfer ( myReader, num, doc, Lseq );
}


//=======================================================================
//function : Transfer
//purpose  : 
//=======================================================================

Standard_Boolean STEPCAFControl_Reader::Transfer (Handle(TDocStd_Document) &doc)
{
  TDF_LabelSequence Lseq;
  return Transfer ( myReader, 0, doc, Lseq );
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

Standard_Boolean STEPCAFControl_Reader::Perform (const Standard_CString filename,
						 Handle(TDocStd_Document) &doc)
{
  if ( ReadFile ( filename ) != IFSelect_RetDone ) return Standard_False;
  return Transfer ( doc );
}
  

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

Standard_Boolean STEPCAFControl_Reader::Perform (const TCollection_AsciiString &filename,
						 Handle(TDocStd_Document) &doc)
{
  if ( ReadFile ( filename.ToCString() ) != IFSelect_RetDone ) return Standard_False;
  return Transfer ( doc );
}
  

//=======================================================================
//function : ExternFiles
//purpose  : 
//=======================================================================

const   NCollection_DataMap<TCollection_AsciiString, Handle(STEPCAFControl_ExternFile)>& STEPCAFControl_Reader::ExternFiles () const
{
  return myFiles;
}
	

//=======================================================================
//function : ExternFile
//purpose  : 
//=======================================================================

Standard_Boolean STEPCAFControl_Reader::ExternFile (const Standard_CString name,
						    Handle(STEPCAFControl_ExternFile) &ef) const
{
  ef.Nullify();
  if ( myFiles.IsEmpty() || !myFiles.IsBound ( name ) ) 
    return Standard_False;
  ef = myFiles.Find ( name );
  return Standard_True;
}


//=======================================================================
//function : Reader
//purpose  : 
//=======================================================================

STEPControl_Reader &STEPCAFControl_Reader::ChangeReader () 
{
  return myReader;
}
  

//=======================================================================
//function : Reader
//purpose  : 
//=======================================================================

const STEPControl_Reader &STEPCAFControl_Reader::Reader () const
{
  return myReader;
}
  

//=======================================================================
//function : FillShapesMap
//purpose  : auxiliary: fill a map by all compounds and their components
//=======================================================================

static void FillShapesMap (const TopoDS_Shape &S, TopTools_MapOfShape &map)
{
  TopoDS_Shape S0 = S;
  TopLoc_Location loc;
  S0.Location ( loc );
  map.Add ( S0 );
  if ( S.ShapeType() != TopAbs_COMPOUND ) return;
  for ( TopoDS_Iterator it(S); it.More(); it.Next() ) 
    FillShapesMap ( it.Value(), map );
}


//=======================================================================
//function : Transfer
//purpose  : basic working method
//=======================================================================

Standard_Boolean STEPCAFControl_Reader::Transfer (STEPControl_Reader &reader,
						  const Standard_Integer nroot,
						  Handle(TDocStd_Document) &doc,
						  TDF_LabelSequence &Lseq,
						  const Standard_Boolean asOne)
{
  reader.ClearShapes();
  Standard_Integer i;
  
  // Read all shapes
  Standard_Integer num = reader.NbRootsForTransfer();
  if ( num <=0 ) return Standard_False;
  if ( nroot ) {
    if ( nroot > num ) return Standard_False;
    reader.TransferOneRoot ( nroot );
  }
  else {
    for ( i=1; i <= num; i++ ) reader.TransferOneRoot ( i );
  }
  num = reader.NbShapes();
  if ( num <=0 ) return Standard_False;

  // Fill a map of (top-level) shapes resulting from that transfer
  // Only these shapes will be considered further
  TopTools_MapOfShape ShapesMap, NewShapesMap;
  for ( i=1; i <= num; i++ ) FillShapesMap ( reader.Shape(i), ShapesMap );
  
  // Collect information on shapes originating from SDRs
  // this will be used to distinguish compounds representing assemblies
  // from the ones representing hybrid models and shape sets
  STEPCAFControl_DataMapOfShapePD ShapePDMap;
  STEPCAFControl_DataMapOfPDExternFile PDFileMap;
  Handle(Interface_InterfaceModel) Model = reader.Model();
  const Handle(Transfer_TransientProcess) &TP = reader.WS()->TransferReader()->TransientProcess();
  Standard_Integer nb = Model->NbEntities();

  Handle(TColStd_HSequenceOfTransient) SeqPDS = new TColStd_HSequenceOfTransient;

  for (i = 1; i <= nb; i ++) {
    Handle(Standard_Transient) enti = Model->Value(i);
    if(enti->IsKind(STANDARD_TYPE(StepRepr_ProductDefinitionShape))) {
      // sequence for acceleration ReadMaterials
      SeqPDS->Append(enti);
    }
    if ( enti->IsKind ( STANDARD_TYPE(StepBasic_ProductDefinition ) ) ) {
      Handle(StepBasic_ProductDefinition) PD = 
        Handle(StepBasic_ProductDefinition)::DownCast(enti);
      Standard_Integer index = TP->MapIndex(PD);
      if ( index >0 ) {
        Handle(Transfer_Binder) binder = TP->MapItem (index);
        TopoDS_Shape S = TransferBRep::ShapeResult(binder);
        if ( ! S.IsNull() && ShapesMap.Contains(S) ) {
          NewShapesMap.Add(S);
          ShapePDMap.Bind ( S, PD ); 
          Handle(STEPCAFControl_ExternFile) EF;
          PDFileMap.Bind ( PD, EF );
        }
      }
    }
    if ( enti->IsKind ( STANDARD_TYPE(StepShape_ShapeRepresentation) ) ) {
      Standard_Integer index = TP->MapIndex(enti);
      if ( index >0 ) {
        Handle(Transfer_Binder) binder = TP->MapItem (index);
        TopoDS_Shape S = TransferBRep::ShapeResult(binder);
        if ( ! S.IsNull() && ShapesMap.Contains(S) )
          NewShapesMap.Add(S);
      }
    }
  }

  // get directory name of the main file
  OSD_Path mainfile ( reader.WS()->LoadedFile() );
  mainfile.SetName ( "" );
  mainfile.SetExtension ( "" );
  TCollection_AsciiString dpath;
  mainfile.SystemName ( dpath );

  // Load external references (only for relevant SDRs)
  // and fill map SDR -> extern file
  STEPConstruct_ExternRefs ExtRefs ( reader.WS() );
  ExtRefs.LoadExternRefs();
  for ( i=1; i <= ExtRefs.NbExternRefs(); i++ ) {
    // check extern ref format
    Handle(TCollection_HAsciiString) format = ExtRefs.Format(i);
    if ( ! format.IsNull() ) {
      static Handle(TCollection_HAsciiString) ap203 = new TCollection_HAsciiString ( "STEP AP203" );
      static Handle(TCollection_HAsciiString) ap214 = new TCollection_HAsciiString ( "STEP AP214" );
      if ( ! format->IsSameString ( ap203, Standard_False ) && 
	   ! format->IsSameString ( ap214, Standard_False ) ) {
#ifdef OCCT_DEBUG
	cout << "Warning: STEPCAFControl_Reader::Transfer: Extern document is neither STEP AP203 nor AP214" << endl;
#else
	continue;
#endif
      }
    }
#ifdef OCCT_DEBUG
    else cout << "Warning: STEPCAFControl_Reader::Transfer: Extern document format not defined" << endl;
#endif
    
    // get and check filename of the current extern ref
    const Standard_CString filename = ExtRefs.FileName(i);

#ifdef OCCT_DEBUG
    cout<<"filename="<<filename<<endl;
#endif

    if ( ! filename || ! filename[0] ) {
#ifdef OCCT_DEBUG
      cout << "Warning: STEPCAFControl_Reader::Transfer: Extern reference file name is empty" << endl;
#endif
      continue; // not a valid extern ref
    }

    // compute true path to the extern file
    TCollection_AsciiString fullname = OSD_Path::AbsolutePath ( dpath, filename );
    if ( fullname.Length() <= 0 ) fullname = filename;

/*    
    char fullname[1024];
    char *mainfile = reader.WS()->LoadedFile();
    if ( ! mainfile ) mainfile = "";
    Standard_Integer slash = 0;
    for ( Standard_Integer k=0; mainfile[k]; k++ )
      if ( mainfile[k] == '/' ) slash = k;
    strncpy ( fullname, mainfile, slash );
    sprintf ( &fullname[slash], "%s%s", ( mainfile[0] ? "/" : "" ), filename );
*/
    
    // get and check PD associated with the current extern ref
    Handle(StepBasic_ProductDefinition) PD = ExtRefs.ProdDef(i);
    if ( PD.IsNull() ) continue; // not a valid extern ref
    if ( ! PDFileMap.IsBound ( PD ) ) continue; // this PD is not concerned by current transfer
    
    // read extern file (or use existing data) and record its data
    Handle(STEPCAFControl_ExternFile) EF = 
      ReadExternFile ( filename, fullname.ToCString(), doc );
    PDFileMap.Bind ( PD, EF );
  }
  
  // and insert them to the document
  Handle(XCAFDoc_ShapeTool) STool = XCAFDoc_DocumentTool::ShapeTool( doc->Main() );
  if ( STool.IsNull() ) return Standard_False;
  XCAFDoc_DataMapOfShapeLabel map;
  if ( asOne )
    Lseq.Append ( AddShape ( reader.OneShape(), STool, NewShapesMap, ShapePDMap, PDFileMap, map ) );
  else {
    for ( i=1; i <= num; i++ ) {
      Lseq.Append ( AddShape ( reader.Shape(i), STool, NewShapesMap, ShapePDMap, PDFileMap, map ) );
    }
  }
  
  // read colors
  if ( GetColorMode() )
    ReadColors ( reader.WS(), doc, PDFileMap, map );
  
  // read names
  if ( GetNameMode() )
    ReadNames ( reader.WS(), doc, PDFileMap, map );

  // read validation props
  if ( GetPropsMode() )
    ReadValProps ( reader.WS(), doc, PDFileMap, map );

  // read layers
  if ( GetLayerMode() )
    ReadLayers ( reader.WS(), doc );
  
  // read SHUO entities from STEP model
  if ( GetSHUOMode() )
    ReadSHUOs ( reader.WS(), doc, PDFileMap, map );

  // read GDT entities from STEP model
  if(GetGDTMode())
    ReadGDTs(reader.WS(),doc);

  // read Material entities from STEP model
  if(GetMatMode())
    ReadMaterials(reader.WS(),doc,SeqPDS);

  // read View entities from STEP model
  if (GetViewMode())
    ReadViews(reader.WS(), doc);

  // Expand resulting CAF structure for sub-shapes (optionally with their
  // names) if requested
  ExpandSubShapes(STool, map, ShapePDMap);

  // Update assembly compounds
  STool->UpdateAssemblies();

  return Standard_True;
}

//=======================================================================
//function : AddShape
//purpose  : 
//=======================================================================

TDF_Label STEPCAFControl_Reader::AddShape (const TopoDS_Shape &S, 
					   const Handle(XCAFDoc_ShapeTool) &STool,
                                           const TopTools_MapOfShape &NewShapesMap,
					   const STEPCAFControl_DataMapOfShapePD &ShapePDMap,
					   const STEPCAFControl_DataMapOfPDExternFile &PDFileMap,
					   XCAFDoc_DataMapOfShapeLabel &ShapeLabelMap) const
{
  // if shape has already been mapped, just return corresponding label
  if ( ShapeLabelMap.IsBound ( S ) ) {
    return ShapeLabelMap.Find ( S );
  }
  
  // if shape is located, create instance
  if ( ! S.Location().IsIdentity() ) {
    TopoDS_Shape S0 = S;
    TopLoc_Location loc;
    S0.Location ( loc );
    AddShape ( S0, STool, NewShapesMap, ShapePDMap, PDFileMap, ShapeLabelMap );
    TDF_Label L = STool->AddShape ( S, Standard_False ); // should create reference
    ShapeLabelMap.Bind ( S, L );
    return L;
  }
  
  // if shape is not compound, simple add it
  if ( S.ShapeType() != TopAbs_COMPOUND ) {
    TDF_Label L = STool->AddShape ( S, Standard_False );
    ShapeLabelMap.Bind ( S, L );
    return L;
  }
  
  // for compounds, compute number of subshapes and check whether this is assembly
  Standard_Boolean isAssembly = Standard_False;
  Standard_Integer nbComponents = 0;
  TopoDS_Iterator it;
  for ( it.Initialize(S); it.More(); it.Next(), nbComponents++ ) {
    TopoDS_Shape Sub0 = it.Value();
    TopLoc_Location loc;
    Sub0.Location ( loc );
    if ( NewShapesMap.Contains ( Sub0 ) ) isAssembly = Standard_True;
  }

//  if(nbComponents>0) isAssembly = Standard_True;
  
  // check whether it has associated external ref
  TColStd_SequenceOfHAsciiString SHAS;
  if ( ShapePDMap.IsBound ( S ) && PDFileMap.IsBound ( ShapePDMap.Find(S) ) ) {
    Handle(STEPCAFControl_ExternFile) EF = PDFileMap.Find ( ShapePDMap.Find(S) );
    if ( ! EF.IsNull() ) {
      // (store information on extern refs in the document)
      SHAS.Append(EF->GetName());
      // if yes, just return corresponding label
      if ( ! EF->GetLabel().IsNull() ) {
	// but if components >0, ignore extern ref!
	if ( nbComponents <=0 ) {
	  ShapeLabelMap.Bind ( S, EF->GetLabel() );
          STool->SetExternRefs(EF->GetLabel(),SHAS);
	  return EF->GetLabel();
	}
      }
#ifdef OCCT_DEBUG
      if ( ! EF->GetLabel().IsNull() )
        cout << "Warning: STEPCAFControl_Reader::AddShape: Non-empty shape with external ref; ref is ignored" << endl;
      else if ( nbComponents <=0 ) 
	cout << "Warning: STEPCAFControl_Reader::AddShape: Result of reading extern ref is Null" << endl;
#endif
    }
  }
  
  // add compound either as a whole,
  if ( ! isAssembly ) {
    TDF_Label L = STool->AddShape ( S, Standard_False );
    if ( SHAS.Length() >0 ) STool->SetExternRefs(L,SHAS);
    ShapeLabelMap.Bind ( S, L );
    return L;
  }
  
  // or as assembly, component-by-component
  TDF_Label L = STool->NewShape();
  for ( it.Initialize(S); it.More(); it.Next(), nbComponents++ ) {
    TopoDS_Shape Sub0 = it.Value();
    TopLoc_Location loc;
    Sub0.Location ( loc );
    TDF_Label subL = AddShape ( Sub0, STool, NewShapesMap, ShapePDMap, PDFileMap, ShapeLabelMap );
    if ( ! subL.IsNull() ) {
      STool->AddComponent ( L, subL, it.Value().Location() );
    }
  }
  if ( SHAS.Length() >0 ) STool->SetExternRefs(L,SHAS);
  ShapeLabelMap.Bind ( S, L );
  //STool->SetShape ( L, S ); // it is necessary for assemblies OCC1747 // commemted by skl for OCC2941

  return L;
}

//=======================================================================
//function : ReadExternFile
//purpose  : 
//=======================================================================

Handle(STEPCAFControl_ExternFile) STEPCAFControl_Reader::ReadExternFile (const Standard_CString file, 
									 const Standard_CString fullname, 
									 Handle(TDocStd_Document)& doc) 
{
  // if the file is already read, associate it with SDR
  if ( myFiles.IsBound ( file ) ) {
    return myFiles.ChangeFind ( file );
  }

#ifdef OCCT_DEBUG
  cout << "Reading extern file: " << fullname << endl;
#endif
 
  // create new WorkSession and Reader
  Handle(XSControl_WorkSession) newWS = new XSControl_WorkSession;
  newWS->SelectNorm ( "STEP" );
  STEPControl_Reader sr ( newWS, Standard_False );
  
  // start to fill the resulting ExternFile structure
  Handle(STEPCAFControl_ExternFile) EF = new STEPCAFControl_ExternFile;
  EF->SetWS ( newWS );
  EF->SetName ( new TCollection_HAsciiString ( file ) );
  
  // read file
  EF->SetLoadStatus ( sr.ReadFile ( fullname ) );
  
  // transfer in single-result mode
  if ( EF->GetLoadStatus() == IFSelect_RetDone ) {
    TDF_LabelSequence labels;
    EF->SetTransferStatus ( Transfer ( sr, 0, doc, labels, Standard_True ) );
    if ( labels.Length() >0 ) EF->SetLabel ( labels.Value(1) );
  }
  
  // add read file to dictionary
  myFiles.Bind( file, EF );
  
  return EF;
}


//=======================================================================
//function : SetColorToSubshape
//purpose  : auxilary
//=======================================================================
static void SetColorToSubshape(const Handle(XCAFDoc_ColorTool) & CTool,
			       const TopoDS_Shape & S,
			       const Quantity_Color& col,
			       const XCAFDoc_ColorType type)
{
  for (TopoDS_Iterator it(S); it.More(); it.Next())
    if (! CTool->SetColor( it.Value(), col, type)) break;
}


//=======================================================================
//function : findStyledSR
//purpose  : auxilary
//=======================================================================
static void findStyledSR (const Handle(StepVisual_StyledItem) &style,
                          Handle(StepShape_ShapeRepresentation)& aSR)
{
  // search Shape Represenatation for component styled item
  for ( Standard_Integer j=1; j <= style->NbStyles(); j++ ) {
    Handle(StepVisual_PresentationStyleByContext) PSA = 
      Handle(StepVisual_PresentationStyleByContext)::DownCast(style->StylesValue ( j ));
    if ( PSA.IsNull() )
      continue;
    StepVisual_StyleContextSelect aStyleCntxSlct = PSA->StyleContext();
    Handle(StepShape_ShapeRepresentation) aCurrentSR = 
      Handle(StepShape_ShapeRepresentation)::DownCast(aStyleCntxSlct.Representation());
    if ( aCurrentSR.IsNull() )
      continue;
    aSR = aCurrentSR;
      break;
  }
}


//=======================================================================
//function : ReadColors
//purpose  : 
//=======================================================================

Standard_Boolean STEPCAFControl_Reader::ReadColors (const Handle(XSControl_WorkSession) &WS,
						    Handle(TDocStd_Document)& Doc,
                                                    const STEPCAFControl_DataMapOfPDExternFile &PDFileMap,
                                                    const XCAFDoc_DataMapOfShapeLabel &ShapeLabelMap) const
{
  STEPConstruct_Styles Styles ( WS );
  if ( ! Styles.LoadStyles() ) {
#ifdef OCCT_DEBUG
    cout << "Warning: no styles are found in the model" << endl;
#endif
    return Standard_False;
  }
  // searching for invisible items in the model
  Handle(TColStd_HSequenceOfTransient) aHSeqOfInvisStyle = new TColStd_HSequenceOfTransient;
  Styles.LoadInvisStyles( aHSeqOfInvisStyle );
  
  Handle(XCAFDoc_ColorTool) CTool = XCAFDoc_DocumentTool::ColorTool( Doc->Main() );
  if ( CTool.IsNull() ) return Standard_False;

  // parse and search for color attributes
  Standard_Integer nb = Styles.NbStyles();
  for ( Standard_Integer i=1; i <= nb; i++ ) {
    Handle(StepVisual_StyledItem) style = Styles.Style ( i );
    if ( style.IsNull() ) continue;
    
    Standard_Boolean IsVisible = Standard_True;
    // check the visibility of styled item.
    for (Standard_Integer si = 1; si <= aHSeqOfInvisStyle->Length(); si++ ) {
      if ( style != aHSeqOfInvisStyle->Value( si ) )
        continue;
      // found that current style is invisible.
      IsVisible = Standard_False;
      break;
    }

    Handle(StepVisual_Colour) SurfCol, BoundCol, CurveCol;
    // check if it is component style
    Standard_Boolean IsComponent = Standard_False;
    if ( ! Styles.GetColors ( style, SurfCol, BoundCol, CurveCol, IsComponent ) && IsVisible )
      continue;
    
    // find shape
    NCollection_Vector<Handle(Standard_Transient)> anItems;
    if (!style->Item().IsNull()) {
      anItems.Append(style->Item());
    }
    else if (!style->ItemAP242().Representation().IsNull()){
      //special case for AP242: item can be Reprsentation
      Handle(StepRepr_Representation) aRepr = style->ItemAP242().Representation();
      for (Standard_Integer j = 1; j <= aRepr->Items()->Length(); j++)
        anItems.Append(aRepr->Items()->Value(j));
    }
    for (Standard_Integer itemIt = 0; itemIt < anItems.Length(); itemIt++) {
      TopoDS_Shape S = STEPConstruct::FindShape ( Styles.TransientProcess(),
        Handle(StepRepr_RepresentationItem)::DownCast(anItems.Value(itemIt)) );
      Standard_Boolean isSkipSHUOstyle = Standard_False;
      // take shape with real location.
      while ( IsComponent ) {
        // take SR of NAUO
        Handle(StepShape_ShapeRepresentation) aSR;
        findStyledSR( style, aSR );
        // search for SR along model
        if (aSR.IsNull())
          break;
        Interface_EntityIterator subs = WS->HGraph()->Graph().Sharings( aSR );
        Handle(StepShape_ShapeDefinitionRepresentation) aSDR;
        for (subs.Start(); subs.More(); subs.Next()) {
          aSDR = Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(subs.Value());
          if ( aSDR.IsNull() )
            continue;
          StepRepr_RepresentedDefinition aPDSselect = aSDR->Definition();
          Handle(StepRepr_ProductDefinitionShape) PDS = 
            Handle(StepRepr_ProductDefinitionShape)::DownCast(aPDSselect.PropertyDefinition());
          if ( PDS.IsNull() )
            continue;
          StepRepr_CharacterizedDefinition aCharDef = PDS->Definition();
          
          Handle(StepRepr_AssemblyComponentUsage) ACU = 
            Handle(StepRepr_AssemblyComponentUsage)::DownCast(aCharDef.ProductDefinitionRelationship());
          if (ACU.IsNull())
            continue;
          // PTV 10.02.2003 skip styled item that refer to SHUO
          if (ACU->IsKind(STANDARD_TYPE(StepRepr_SpecifiedHigherUsageOccurrence))) {
            isSkipSHUOstyle = Standard_True;
            break;
          }
          Handle(StepRepr_NextAssemblyUsageOccurrence) NAUO =
            Handle(StepRepr_NextAssemblyUsageOccurrence)::DownCast(ACU);
          if ( NAUO.IsNull() )
            continue;
          
          TopoDS_Shape aSh;
          // PTV 10.02.2003 to find component of assembly CORRECTLY
          STEPConstruct_Tool Tool( WS );
          TDF_Label aShLab = FindInstance ( NAUO, CTool->ShapeTool(), Tool, PDFileMap, ShapeLabelMap );
          aSh = CTool->ShapeTool()->GetShape(aShLab);
          if (!aSh.IsNull()) {
            S = aSh;
            break;
          }
        }
        break;
      }
      if (isSkipSHUOstyle)
        continue; // skip styled item which refer to SHUO
      
      if ( S.IsNull() )
        continue;
      
      if ( ! SurfCol.IsNull() ) {
        Quantity_Color col;
        Styles.DecodeColor ( SurfCol, col );
        if ( ! CTool->SetColor ( S, col, XCAFDoc_ColorSurf ))
          SetColorToSubshape( CTool, S, col, XCAFDoc_ColorSurf );
      }
      if ( ! BoundCol.IsNull() ) {
        Quantity_Color col;
        Styles.DecodeColor ( BoundCol, col );
        if ( ! CTool->SetColor ( S, col, XCAFDoc_ColorCurv ))
          SetColorToSubshape(  CTool, S, col, XCAFDoc_ColorCurv );
      }
      if ( ! CurveCol.IsNull() ) {
        Quantity_Color col;
        Styles.DecodeColor ( CurveCol, col );
        if ( ! CTool->SetColor ( S, col, XCAFDoc_ColorCurv ))
          SetColorToSubshape(  CTool, S, col, XCAFDoc_ColorCurv );
      }
      if ( !IsVisible ) {
        // sets the invisibility for shape.
        TDF_Label aInvL;
        if ( CTool->ShapeTool()->Search( S, aInvL ) )
          CTool->SetVisibility( aInvL, Standard_False );
      }
    }
  }
  CTool->ReverseChainsOfTreeNodes();
  return Standard_True;
}

//=======================================================================
//function : GetLabelFromPD
//purpose  : 
//=======================================================================

static TDF_Label GetLabelFromPD (const Handle(StepBasic_ProductDefinition) &PD,
				 const Handle(XCAFDoc_ShapeTool) &STool,
				 const Handle(Transfer_TransientProcess) &TP,
				 const STEPCAFControl_DataMapOfPDExternFile &PDFileMap,
				 const XCAFDoc_DataMapOfShapeLabel &ShapeLabelMap)
{
  TDF_Label L;
  if ( PDFileMap.IsBound ( PD ) ) {
    Handle(STEPCAFControl_ExternFile) EF = PDFileMap.Find ( PD );
    if ( ! EF.IsNull() ) {
      L = EF->GetLabel();
      if ( ! L.IsNull() ) return L;
    }
  }

  TopoDS_Shape S;
  Handle(Transfer_Binder) binder = TP->Find(PD);
  if ( binder.IsNull() || ! binder->HasResult() ) return L;
  S = TransferBRep::ShapeResult ( TP, binder );
  if ( S.IsNull() ) return L;

  if ( S.IsNull() ) return L;
  if ( ShapeLabelMap.IsBound ( S ) )
    L = ShapeLabelMap.Find ( S );
  if ( L.IsNull() )
    STool->Search ( S, L, Standard_True, Standard_True, Standard_False );
  return L;
}

//=======================================================================
//function : FindInstance
//purpose  : 
//=======================================================================

TDF_Label STEPCAFControl_Reader::FindInstance (const Handle(StepRepr_NextAssemblyUsageOccurrence) &NAUO,
					       const Handle(XCAFDoc_ShapeTool) &STool,
					       const STEPConstruct_Tool &Tool,
					       const STEPCAFControl_DataMapOfPDExternFile &PDFileMap,
					       const XCAFDoc_DataMapOfShapeLabel &ShapeLabelMap)
{
  TDF_Label L;
  
  // get shape resulting from CDSR (in fact, only location is interesting)
  Handle(Transfer_TransientProcess) TP = Tool.TransientProcess();
  Handle(Transfer_Binder) binder = TP->Find(NAUO);
  if ( binder.IsNull() || ! binder->HasResult() ) {
#ifdef OCCT_DEBUG
    cout << "Error: STEPCAFControl_Reader::FindInstance: NAUO is not mapped to shape" << endl;
#endif
    return L;
  }
  
  TopoDS_Shape S = TransferBRep::ShapeResult ( TP, binder );
  if ( S.IsNull() ) {
#ifdef OCCT_DEBUG
    cout << "Error: STEPCAFControl_Reader::FindInstance: NAUO is not mapped to shape" << endl;
#endif
    return L;
  }

  // find component`s original label
  Handle(StepBasic_ProductDefinition) PD = NAUO->RelatedProductDefinition();
  if ( PD.IsNull() ) return L;
  TDF_Label Lref = GetLabelFromPD ( PD, STool, TP, PDFileMap, ShapeLabelMap );
  if ( Lref.IsNull() ) return L;
  
  // find main shape (assembly) label
  PD.Nullify();
  PD = NAUO->RelatingProductDefinition();
  if ( PD.IsNull() ) return L;
  TDF_Label L0 = GetLabelFromPD ( PD, STool, TP, PDFileMap, ShapeLabelMap );
  if ( L0.IsNull() ) return L;
  
  // if CDSR and NAUO are reversed, swap labels
  Handle(StepShape_ContextDependentShapeRepresentation) CDSR;
  Interface_EntityIterator subs1 = Tool.Graph().Sharings(NAUO);
  for (subs1.Start(); subs1.More(); subs1.Next()) {
    Handle(StepRepr_ProductDefinitionShape) PDS = 
      Handle(StepRepr_ProductDefinitionShape)::DownCast(subs1.Value());
    if(PDS.IsNull()) continue;
    Interface_EntityIterator subs2 = Tool.Graph().Sharings(PDS);
    for (subs2.Start(); subs2.More(); subs2.Next()) {
      Handle(StepShape_ContextDependentShapeRepresentation) CDSRtmp = 
        Handle(StepShape_ContextDependentShapeRepresentation)::DownCast(subs2.Value());
      if (CDSRtmp.IsNull()) continue;
      CDSR = CDSRtmp;
    }
  }
  if (CDSR.IsNull()) return L;
//  if ( STEPConstruct_Assembly::CheckSRRReversesNAUO ( Tool.Model(), CDSR ) ) {
//    TDF_Label Lsw = L0; L0 = Lref; Lref = Lsw;
//  }
  
  // iterate on components to find proper one
  TDF_LabelSequence seq;
  XCAFDoc_ShapeTool::GetComponents ( L0, seq );
  for ( Standard_Integer k=1; L.IsNull() && k <= seq.Length(); k++ ) {
    TDF_Label Lcomp = seq(k), Lref2;
    if ( XCAFDoc_ShapeTool::GetReferredShape ( Lcomp, Lref2 ) && 
	Lref2 == Lref &&
	S.Location() == XCAFDoc_ShapeTool::GetLocation ( Lcomp ) ) 
      L = Lcomp;
  }
  
  return L;
}

//=======================================================================
//function : ReadNames
//purpose  : 
//=======================================================================

Standard_Boolean STEPCAFControl_Reader::ReadNames (const Handle(XSControl_WorkSession) &WS,
                                                   Handle(TDocStd_Document)& Doc,
                                                   const STEPCAFControl_DataMapOfPDExternFile &PDFileMap,
                                                   const XCAFDoc_DataMapOfShapeLabel &ShapeLabelMap) const
{
  // get starting data
  const Handle(Interface_InterfaceModel) &Model = WS->Model();
  const Handle(XSControl_TransferReader) &TR = WS->TransferReader();
  const Handle(Transfer_TransientProcess) &TP = TR->TransientProcess();
  Handle(XCAFDoc_ShapeTool) STool = XCAFDoc_DocumentTool::ShapeTool( Doc->Main() );
  if ( STool.IsNull() ) return Standard_False;
  STEPConstruct_Tool Tool ( WS );

  // iterate on model to find all SDRs and CDSRs
  Standard_Integer nb = Model->NbEntities();
  Handle(Standard_Type) tNAUO = STANDARD_TYPE(StepRepr_NextAssemblyUsageOccurrence);
  Handle(Standard_Type) tPD  = STANDARD_TYPE(StepBasic_ProductDefinition);
  Handle(TCollection_HAsciiString) name;
  TDF_Label L;
  for (Standard_Integer i = 1; i <= nb; i++) {
    Handle(Standard_Transient) enti = Model->Value(i);

    // get description of NAUO
    if ( enti->DynamicType() == tNAUO ) {
      L.Nullify();
      Handle(StepRepr_NextAssemblyUsageOccurrence) NAUO = 
        Handle(StepRepr_NextAssemblyUsageOccurrence)::DownCast(enti);
      if(NAUO.IsNull()) continue;
      Interface_EntityIterator subs = WS->Graph().Sharings(NAUO);
      for (subs.Start(); subs.More(); subs.Next()) {
        Handle(StepRepr_ProductDefinitionShape) PDS = 
          Handle(StepRepr_ProductDefinitionShape)::DownCast(subs.Value());
        if(PDS.IsNull()) continue;
        Handle(StepBasic_ProductDefinitionRelationship) PDR = PDS->Definition().ProductDefinitionRelationship();
        if ( PDR.IsNull() ) continue;
        if ( PDR->HasDescription() && 
            PDR->Description()->UsefullLength() >0 ) name = PDR->Description();
        else if ( !PDR->Name().IsNull() && PDR->Name()->UsefullLength() >0 ) name = PDR->Name();
        else if ( !PDR->Id().IsNull()) name = PDR->Id();
        else name = new TCollection_HAsciiString;
      }
      // find proper label
      L = FindInstance ( NAUO, STool, Tool, PDFileMap, ShapeLabelMap );
      if ( L.IsNull() ) continue;
      TCollection_ExtendedString str ( name->String() );
      TDataStd_Name::Set ( L, str );
    }

    // for PD get name of associated product
    if ( enti->DynamicType() == tPD ) {
      L.Nullify();
      Handle(StepBasic_ProductDefinition) PD = 
        Handle(StepBasic_ProductDefinition)::DownCast(enti);
      if(PD.IsNull()) continue;
      Handle(StepBasic_Product) Prod = (!PD->Formation().IsNull() ? PD->Formation()->OfProduct() : NULL);
      if (Prod.IsNull())
        name = new TCollection_HAsciiString;
      else if (!Prod->Name().IsNull() && Prod->Name()->UsefullLength() > 0) 
        name = Prod->Name();
      else if (!Prod->Id().IsNull()) 
        name = Prod->Id();
      else 
        name = new TCollection_HAsciiString;
      L = GetLabelFromPD ( PD, STool, TP, PDFileMap, ShapeLabelMap );
      if ( L.IsNull() ) continue;
      TCollection_ExtendedString str ( name->String() );
      TDataStd_Name::Set ( L, str );
    }
    // set a name to the document
    //TCollection_ExtendedString str ( name->String() );
    //TDataStd_Name::Set ( L, str );
  }

  return Standard_True;
}

//=======================================================================
//function : GetLabelFromPD
//purpose  : 
//=======================================================================

static TDF_Label GetLabelFromPD (const Handle(StepBasic_ProductDefinition) &PD,
                                 const Handle(XCAFDoc_ShapeTool) &STool,
                                 const STEPConstruct_ValidationProps &Props,
                                 const STEPCAFControl_DataMapOfPDExternFile &PDFileMap,
                                 const XCAFDoc_DataMapOfShapeLabel &ShapeLabelMap)
{
  TDF_Label L;
  if ( PDFileMap.IsBound ( PD ) ) {
    Handle(STEPCAFControl_ExternFile) EF = PDFileMap.Find ( PD );
    if ( ! EF.IsNull() ) {
      L = EF->GetLabel();
      if ( ! L.IsNull() ) return L;
    }
  }
  TopoDS_Shape S = Props.GetPropShape ( PD );
  if ( S.IsNull() ) return L;
  if ( ShapeLabelMap.IsBound ( S ) )
    L = ShapeLabelMap.Find ( S );
  if ( L.IsNull() )
    STool->Search ( S, L, Standard_True, Standard_True, Standard_False );
  return L;
}

//=======================================================================
//function : ReadValProps
//purpose  : 
//=======================================================================

Standard_Boolean STEPCAFControl_Reader::ReadValProps (const Handle(XSControl_WorkSession) &WS,
						      Handle(TDocStd_Document)& Doc,
						      const STEPCAFControl_DataMapOfPDExternFile &PDFileMap,
						      const XCAFDoc_DataMapOfShapeLabel &ShapeLabelMap) const
{
  // get starting data
  const Handle(XSControl_TransferReader) &TR = WS->TransferReader();
  const Handle(Transfer_TransientProcess) &TP = TR->TransientProcess();
  Handle(XCAFDoc_ShapeTool) STool = XCAFDoc_DocumentTool::ShapeTool( Doc->Main() );
  if ( STool.IsNull() ) return Standard_False;

  // load props from the STEP model
  TColStd_SequenceOfTransient props;
  STEPConstruct_ValidationProps Props ( WS );
  if ( ! Props.LoadProps ( props ) ) {
#ifdef OCCT_DEBUG
    cout << "Warning: no validation props found in the model" << endl;
#endif
    return Standard_False;
  }

  // interpret props one by one
  for (Standard_Integer i = 1; i <= props.Length(); i ++) {
    Handle(StepRepr_PropertyDefinitionRepresentation) PDR = 
      Handle(StepRepr_PropertyDefinitionRepresentation)::DownCast ( props.Value(i) );
    if ( PDR.IsNull() ) continue;

    TDF_Label L;

    Handle(StepRepr_PropertyDefinition) PD = PDR->Definition().PropertyDefinition();
    Interface_EntityIterator subs = Props.Graph().Shareds(PD);
    for (subs.Start(); L.IsNull() && subs.More(); subs.Next()) {
      if ( subs.Value()->IsKind(STANDARD_TYPE(StepRepr_ProductDefinitionShape)) ) {
        Handle(StepRepr_ProductDefinitionShape) PDS = Handle(StepRepr_ProductDefinitionShape)::DownCast(subs.Value());
        if(PDS.IsNull()) continue;
        // find corresponding NAUO
        Handle(StepRepr_NextAssemblyUsageOccurrence) NAUO;
        Interface_EntityIterator subs1 = Props.Graph().Shareds(PDS);
        for (subs1.Start(); NAUO.IsNull() && subs1.More(); subs1.Next()) {
          if ( subs1.Value()->IsKind(STANDARD_TYPE(StepRepr_NextAssemblyUsageOccurrence)) ) 
            NAUO = Handle(StepRepr_NextAssemblyUsageOccurrence)::DownCast(subs1.Value());
        }
        if ( !NAUO.IsNull() ) {
          L = FindInstance ( NAUO, STool, WS, PDFileMap, ShapeLabelMap );
          if ( L.IsNull() ) continue;
        }
        else {
          // find corresponding ProductDefinition:
          Handle(StepBasic_ProductDefinition) ProdDef;
          Interface_EntityIterator subsPDS = Props.Graph().Shareds(PDS);
          for (subsPDS.Start(); ProdDef.IsNull() && subsPDS.More(); subsPDS.Next()) {
            if ( subsPDS.Value()->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition)) ) 
              ProdDef = Handle(StepBasic_ProductDefinition)::DownCast(subsPDS.Value());
          }
          if ( ProdDef.IsNull() ) continue;
          L = GetLabelFromPD ( ProdDef, STool, Props, PDFileMap, ShapeLabelMap );
        }
      }

      if ( subs.Value()->IsKind(STANDARD_TYPE(StepRepr_ShapeAspect)) ) {
        Handle(StepRepr_ShapeAspect) SA = Handle(StepRepr_ShapeAspect)::DownCast(subs.Value());
        if(SA.IsNull()) continue;
        // find ShapeRepresentation
        Handle(StepShape_ShapeRepresentation) SR;
        Interface_EntityIterator subs1 = Props.Graph().Sharings(SA);
        for(subs1.Start(); subs1.More() && SR.IsNull(); subs1.Next()) {
          Handle(StepRepr_PropertyDefinition) PropD1 = 
            Handle(StepRepr_PropertyDefinition)::DownCast(subs1.Value());
          if(PropD1.IsNull()) continue;
          Interface_EntityIterator subs2 = Props.Graph().Sharings(PropD1);
          for(subs2.Start(); subs2.More() && SR.IsNull(); subs2.Next()) {
            Handle(StepShape_ShapeDefinitionRepresentation) SDR =
              Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(subs2.Value());
            if(SDR.IsNull()) continue;
            SR = Handle(StepShape_ShapeRepresentation)::DownCast(SDR->UsedRepresentation());
          }
        }
        if(SR.IsNull()) continue;
        Handle(Transfer_Binder) binder;
        for(Standard_Integer ir=1; ir<=SR->NbItems() && binder.IsNull(); ir++) {
          if(SR->ItemsValue(ir)->IsKind(STANDARD_TYPE(StepShape_SolidModel))) {
            Handle(StepShape_SolidModel) SM = 
              Handle(StepShape_SolidModel)::DownCast(SR->ItemsValue(ir));
            binder = TP->Find(SM);
          }
          else if(SR->ItemsValue(ir)->IsKind(STANDARD_TYPE(StepShape_ShellBasedSurfaceModel))) {
            Handle(StepShape_ShellBasedSurfaceModel) SBSM =
              Handle(StepShape_ShellBasedSurfaceModel)::DownCast(SR->ItemsValue(ir));
            binder = TP->Find(SBSM);
          }
          else if(SR->ItemsValue(ir)->IsKind(STANDARD_TYPE(StepShape_GeometricSet))) {
            Handle(StepShape_GeometricSet) GS =
              Handle(StepShape_GeometricSet)::DownCast(SR->ItemsValue(ir));
            binder = TP->Find(GS);
          }
        }
        if ( binder.IsNull() || ! binder->HasResult() ) continue;
        TopoDS_Shape S;
        S = TransferBRep::ShapeResult ( TP, binder );
        if(S.IsNull()) continue;
        if ( ShapeLabelMap.IsBound ( S ) )
          L = ShapeLabelMap.Find ( S );
        if ( L.IsNull() )
          STool->Search ( S, L, Standard_True, Standard_True, Standard_True );
      }
    }

    if(L.IsNull()) continue;
      
    // decode validation properties
    Handle(StepRepr_Representation) rep = PDR->UsedRepresentation();
    for ( Standard_Integer j=1; j <= rep->NbItems(); j++ ) {
      Handle(StepRepr_RepresentationItem) ent = rep->ItemsValue(j);
      Standard_Boolean isArea;
      Standard_Real val;
      gp_Pnt pos;
      if ( Props.GetPropReal ( ent, val, isArea ) ) {
	if ( isArea ) XCAFDoc_Area::Set ( L, val );
	else XCAFDoc_Volume::Set ( L, val );
      }
      else if ( Props.GetPropPnt ( ent, rep->ContextOfItems(), pos ) ) {
	XCAFDoc_Centroid::Set ( L, pos );
      }
    }
  }
  return Standard_True;
}

//=======================================================================
//function : ReadLayers
//purpose  : 
//=======================================================================

Standard_Boolean STEPCAFControl_Reader::ReadLayers (const Handle(XSControl_WorkSession) &WS,
						    Handle(TDocStd_Document)& Doc) const
{
  const Handle(Interface_InterfaceModel) &Model = WS->Model();
  const Handle(XSControl_TransferReader) &TR = WS->TransferReader();
  const Handle(Transfer_TransientProcess) &TP = TR->TransientProcess();
  Handle(XCAFDoc_ShapeTool) STool = XCAFDoc_DocumentTool::ShapeTool( Doc->Main() );
  if ( STool.IsNull() ) return Standard_False;
  Handle(XCAFDoc_LayerTool) LTool = XCAFDoc_DocumentTool::LayerTool( Doc->Main() );
  if ( LTool.IsNull() ) return Standard_False;
  
  Handle(Standard_Type) tSVPLA = STANDARD_TYPE(StepVisual_PresentationLayerAssignment);
  Standard_Integer nb = Model->NbEntities();
  Handle(TCollection_HAsciiString) name;
  
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Handle(Standard_Transient) enti = Model->Value(i);
    if ( ! enti->IsKind ( tSVPLA ) ) continue;
    Handle(StepVisual_PresentationLayerAssignment) SVPLA = 
      Handle(StepVisual_PresentationLayerAssignment)::DownCast(enti);
    if (SVPLA->AssignedItems().IsNull())
      continue;
    
    Handle(TCollection_HAsciiString) descr = SVPLA->Description();
    Handle(TCollection_HAsciiString) hName = SVPLA->Name();
    TCollection_ExtendedString aLayerName ( hName->String() );
     
    // find a target shape and its label in the document
    for (Standard_Integer j = 1; j <= SVPLA->NbAssignedItems(); j++ ) {
      StepVisual_LayeredItem LI = SVPLA->AssignedItemsValue(j);
      Handle(Transfer_Binder) binder = TP->Find( LI.Value() );
      if ( binder.IsNull() || ! binder->HasResult() ) continue;
      
      TopoDS_Shape S = TransferBRep::ShapeResult ( TP, binder );
      if ( S.IsNull() ) continue;
	
      TDF_Label shL;
      if ( ! STool->Search ( S, shL, Standard_True, Standard_True, Standard_True ) ) continue;
      LTool->SetLayer ( shL, aLayerName );
    }
    
    // check invisibility
    Interface_EntityIterator subs = WS->Graph().Sharings(SVPLA);
    for (subs.Start(); subs.More(); subs.Next()) {
      if ( ! subs.Value()->IsKind(STANDARD_TYPE(StepVisual_Invisibility)) ) continue;
#ifdef OCCT_DEBUG
      cout<< "\tLayer \"" << aLayerName << "\" is invisible"<<endl;
#endif
      //TDF_Label InvLayerLab = LTool->FindLayer(aLayerName);
      TDF_Label InvLayerLab = LTool->AddLayer(aLayerName); //skl for OCC3926
      Handle(TDataStd_UAttribute) aUAttr;
      aUAttr->Set( InvLayerLab, XCAFDoc::InvisibleGUID() );
    }
  }
  return Standard_True;
}

//=======================================================================
//function : ReadSHUOs
//purpose  : 
//=======================================================================

static Standard_Boolean findNextSHUOlevel (const Handle(XSControl_WorkSession) &WS,
                                           const Handle(StepRepr_SpecifiedHigherUsageOccurrence)& SHUO,
                                           const Handle(XCAFDoc_ShapeTool)& STool,
                                           const STEPCAFControl_DataMapOfPDExternFile &PDFileMap,
                                           const XCAFDoc_DataMapOfShapeLabel &ShapeLabelMap,
                                           TDF_LabelSequence& aLabels)
{
  Interface_EntityIterator subs = WS->HGraph()->Graph().Sharings(SHUO);
  Handle(StepRepr_SpecifiedHigherUsageOccurrence) subSHUO;
  for (subs.Start(); subs.More(); subs.Next()) {
    if (subs.Value()->IsKind(STANDARD_TYPE(StepRepr_SpecifiedHigherUsageOccurrence))) {
      subSHUO = Handle(StepRepr_SpecifiedHigherUsageOccurrence)::DownCast(subs.Value());
      break;
    }
  }
  if (subSHUO.IsNull())
    return Standard_False;
  
  Handle(StepRepr_NextAssemblyUsageOccurrence) NUNAUO = subSHUO->NextUsage();
  if (NUNAUO.IsNull())
    return Standard_False;
//   Handle(Interface_InterfaceModel) Model = WS->Model();
//   Handle(XSControl_TransferReader) TR = WS->TransferReader();
//   Handle(Transfer_TransientProcess) TP = TR->TransientProcess();
//   Handle(Transfer_Binder) binder = TP->Find(NUNAUO);
//   if ( binder.IsNull() || ! binder->HasResult() )
//     return Standard_False;
//   TopoDS_Shape NUSh = TransferBRep::ShapeResult ( TP, binder );
  // get label of NAUO next level
  TDF_Label NULab;
  STEPConstruct_Tool Tool( WS );
  NULab = STEPCAFControl_Reader::FindInstance ( NUNAUO, STool, Tool, PDFileMap, ShapeLabelMap ); 
//   STool->Search(NUSh, NUlab);
  if (NULab.IsNull())
    return Standard_False;
  aLabels.Append( NULab );
  // and check by recurse.
  findNextSHUOlevel( WS, subSHUO, STool, PDFileMap, ShapeLabelMap, aLabels );
  return Standard_True;
}


//=======================================================================
//function : setSHUOintoDoc
//purpose  : auxilary
//=======================================================================
static TDF_Label setSHUOintoDoc (const Handle(XSControl_WorkSession) &WS,
                                 const Handle(StepRepr_SpecifiedHigherUsageOccurrence)& SHUO,
                                 const Handle(XCAFDoc_ShapeTool)& STool,
                                 const STEPCAFControl_DataMapOfPDExternFile &PDFileMap,
                                 const XCAFDoc_DataMapOfShapeLabel &ShapeLabelMap)
{
  TDF_Label aMainLabel;
  // get upper usage NAUO from SHUO.
  Handle(StepRepr_NextAssemblyUsageOccurrence) UUNAUO =
    Handle(StepRepr_NextAssemblyUsageOccurrence)::DownCast(SHUO->UpperUsage());
  Handle(StepRepr_NextAssemblyUsageOccurrence) NUNAUO = SHUO->NextUsage();
  if ( UUNAUO.IsNull() || NUNAUO.IsNull() ) {
#ifdef OCCT_DEBUG
    cout << "Warning: " << __FILE__ <<": Upper_usage or Next_usage of styled SHUO is null. Skip it" << endl;
#endif
    return aMainLabel;
  }
//   Handle(Interface_InterfaceModel) Model = WS->Model();
//   Handle(XSControl_TransferReader) TR = WS->TransferReader();
//   Handle(Transfer_TransientProcess) TP = TR->TransientProcess();
//   TopoDS_Shape UUSh, NUSh;
//   Handle(Transfer_Binder) binder = TP->Find(UUNAUO);
//   if ( binder.IsNull() || ! binder->HasResult() )
//     return aMainLabel;
//   UUSh = TransferBRep::ShapeResult ( TP, binder );
//   binder = TP->Find(NUNAUO);
//   if ( binder.IsNull() || ! binder->HasResult() )
//     return aMainLabel;
//   NUSh = TransferBRep::ShapeResult ( TP, binder );

  // get first labels for first SHUO attribute
  TDF_Label UULab, NULab;
  STEPConstruct_Tool Tool( WS );
  UULab = STEPCAFControl_Reader::FindInstance ( UUNAUO, STool, Tool, PDFileMap, ShapeLabelMap ); 
  NULab = STEPCAFControl_Reader::FindInstance ( NUNAUO, STool, Tool, PDFileMap, ShapeLabelMap ); 
  
//   STool->Search(UUSh, UULab);
//   STool->Search(NUSh, NULab);
  if (UULab.IsNull() || NULab.IsNull()) return aMainLabel;
  //create sequence fo labels to set SHUO structure into the document
  TDF_LabelSequence ShuoLabels;
  ShuoLabels.Append( UULab );
  ShuoLabels.Append( NULab );
  // add all other labels of sub SHUO entities
  findNextSHUOlevel( WS, SHUO, STool, PDFileMap, ShapeLabelMap, ShuoLabels );
  // last accord for SHUO
  Handle(XCAFDoc_GraphNode) anSHUOAttr;
  if ( STool->SetSHUO( ShuoLabels, anSHUOAttr ) )
    aMainLabel = anSHUOAttr->Label();
  
  return aMainLabel;
}


//=======================================================================
//function : ReadSHUOs
//purpose  : 
//=======================================================================

Standard_Boolean STEPCAFControl_Reader::ReadSHUOs (const Handle(XSControl_WorkSession) &WS,
                                                   Handle(TDocStd_Document)& Doc,
                                                   const STEPCAFControl_DataMapOfPDExternFile &PDFileMap,
                                                   const XCAFDoc_DataMapOfShapeLabel &ShapeLabelMap) const
{
  // the big part code duplication from ReadColors.
  // It is possible to share this code functionality, just to decide how ???
  Handle(XCAFDoc_ColorTool) CTool = XCAFDoc_DocumentTool::ColorTool( Doc->Main() );
  Handle(XCAFDoc_ShapeTool) STool = CTool->ShapeTool();
  
  STEPConstruct_Styles Styles ( WS );
  if ( ! Styles.LoadStyles() ) {
#ifdef OCCT_DEBUG
    cout << "Warning: no styles are found in the model" << endl;
#endif
    return Standard_False;
  }
  // searching for invisible items in the model
  Handle(TColStd_HSequenceOfTransient) aHSeqOfInvisStyle = new TColStd_HSequenceOfTransient;
  Styles.LoadInvisStyles( aHSeqOfInvisStyle );
  // parse and search for color attributes
  Standard_Integer nb = Styles.NbStyles();
  for ( Standard_Integer i=1; i <= nb; i++ ) {
    Handle(StepVisual_StyledItem) style = Styles.Style ( i );
    if ( style.IsNull() ) continue;
    
    Standard_Boolean IsVisible = Standard_True;
    // check the visibility of styled item.
    for (Standard_Integer si = 1; si <= aHSeqOfInvisStyle->Length(); si++ ) {
      if ( style != aHSeqOfInvisStyle->Value( si ) )
        continue;
      // found that current style is invisible.
#ifdef OCCT_DEBUG
      cout << "Warning: item No " << i << "(" << style->Item()->DynamicType()->Name() << ") is invisible" << endl;
#endif
      IsVisible = Standard_False;
      break;
    }

    Handle(StepVisual_Colour) SurfCol, BoundCol, CurveCol;
    // check if it is component style
    Standard_Boolean IsComponent = Standard_False;
    if ( ! Styles.GetColors ( style, SurfCol, BoundCol, CurveCol, IsComponent ) && IsVisible )
      continue;
    if (!IsComponent)
      continue;
    Handle(StepShape_ShapeRepresentation) aSR;
    findStyledSR( style, aSR );
    // search for SR along model
    if ( aSR.IsNull() )
      continue;
    Interface_EntityIterator subs = WS->HGraph()->Graph().Sharings( aSR );
    Handle(StepShape_ShapeDefinitionRepresentation) aSDR;
    for (subs.Start(); subs.More(); subs.Next()) {
      aSDR = Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(subs.Value());
      if ( aSDR.IsNull() )
        continue;
      StepRepr_RepresentedDefinition aPDSselect = aSDR->Definition();
      Handle(StepRepr_ProductDefinitionShape) PDS = 
        Handle(StepRepr_ProductDefinitionShape)::DownCast(aPDSselect.PropertyDefinition());
      if ( PDS.IsNull() )
        continue;
      StepRepr_CharacterizedDefinition aCharDef = PDS->Definition();
      Handle(StepRepr_SpecifiedHigherUsageOccurrence) SHUO =
        Handle(StepRepr_SpecifiedHigherUsageOccurrence)::DownCast(aCharDef.ProductDefinitionRelationship());
      if ( SHUO.IsNull() )
        continue;
      
      // set the SHUO structure to the document
      TDF_Label aLabelForStyle = setSHUOintoDoc( WS, SHUO, STool, PDFileMap, ShapeLabelMap );
      if ( aLabelForStyle.IsNull() ) {
#ifdef OCCT_DEBUG
        cout << "Warning: " << __FILE__ <<": coudnot create SHUO structure in the document" << endl;
#endif
        continue;
      }
      // now set the style to the SHUO main label.
      if ( ! SurfCol.IsNull() ) {
        Quantity_Color col;
        Styles.DecodeColor ( SurfCol, col );
        CTool->SetColor ( aLabelForStyle, col, XCAFDoc_ColorSurf );
      }
      if ( ! BoundCol.IsNull() ) {
        Quantity_Color col;
        Styles.DecodeColor ( BoundCol, col );
        CTool->SetColor ( aLabelForStyle, col, XCAFDoc_ColorCurv );
      }
      if ( ! CurveCol.IsNull() ) {
        Quantity_Color col;
        Styles.DecodeColor ( CurveCol, col );
        CTool->SetColor ( aLabelForStyle, col, XCAFDoc_ColorCurv );
      }
      if ( !IsVisible )
        // sets the invisibility for shape.
        CTool->SetVisibility( aLabelForStyle, Standard_False );
      
    } // end search SHUO by SDR
  } // end iterates on styles
      
  return Standard_True;
}


//=======================================================================
//function : GetLengthConversionFactor
//purpose  : 
//=======================================================================
static Standard_Boolean GetLengthConversionFactor(const Handle(StepBasic_NamedUnit)& NU,
                                                  Standard_Real& afact)
{
  afact=1.;
  if( !NU->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndLengthUnit)) ) return Standard_False;
  Handle(StepBasic_ConversionBasedUnitAndLengthUnit) CBULU =
    Handle(StepBasic_ConversionBasedUnitAndLengthUnit)::DownCast(NU);
  Handle(StepBasic_MeasureWithUnit) MWUCBU = CBULU->ConversionFactor();
  afact = MWUCBU->ValueComponent();
  StepBasic_Unit anUnit2 = MWUCBU->UnitComponent();
  if(anUnit2.CaseNum(anUnit2.Value())==1) {
    Handle(StepBasic_NamedUnit) NU2 = anUnit2.NamedUnit();
    if(NU2->IsKind(STANDARD_TYPE(StepBasic_SiUnit))) {
      Handle(StepBasic_SiUnit) SU = Handle(StepBasic_SiUnit)::DownCast(NU2);
      if(SU->Name()==StepBasic_sunMetre) {
        if(SU->HasPrefix()) 
          afact *= STEPConstruct_UnitContext::ConvertSiPrefix (SU->Prefix());
        // convert m to mm
        afact *= 1000.;
      }
    }
  }
  return Standard_True;
}

//=======================================================================
//function : GetLengthConversionFactorFromContext
//purpose  : 
//=======================================================================
static Standard_Boolean GetLengthConversionFactorFromContext(const Handle(StepRepr_RepresentationContext)& theRC,
                                                             Standard_Real& theFact)
{
  theFact = 1;
  if (theRC.IsNull())
    return Standard_False;
  Handle(StepBasic_ConversionBasedUnitAndLengthUnit) aSiLU;
  Handle(StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext) aCtx =
    Handle(StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext)::DownCast(theRC);
  if (!aCtx.IsNull()) {
    for (Standard_Integer j = 1; j <= aCtx->NbUnits(); j++) {
      if (aCtx->UnitsValue(j)->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndLengthUnit))) {
        aSiLU = Handle(StepBasic_ConversionBasedUnitAndLengthUnit)::DownCast(aCtx->UnitsValue(j));
        break;
      }
    }
  }
  if (aSiLU.IsNull()) {
    Handle(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx) aCtx1 =
      Handle(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx)::DownCast(theRC);
    if (!aCtx1.IsNull()) {
      for (Standard_Integer j = 1; j <= aCtx1->NbUnits(); j++) {
        if (aCtx1->UnitsValue(j)->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndLengthUnit))) {
          aSiLU = Handle(StepBasic_ConversionBasedUnitAndLengthUnit)::DownCast(aCtx1->UnitsValue(j));
          break;
        }
      }
    }
  }
  if (aSiLU.IsNull())
    return Standard_False;
  return GetLengthConversionFactor(aSiLU, theFact);
  
}

//=======================================================================
//function : GetAngleConversionFactor
//purpose  : 
//=======================================================================
static Standard_Boolean GetAngleConversionFactor(Handle(StepBasic_NamedUnit)& NU,
                                                  Standard_Real& afact)
{
  afact=1.;
  if( !NU->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndPlaneAngleUnit)) ) return Standard_False;
  Handle(StepBasic_ConversionBasedUnitAndPlaneAngleUnit) CBULU =
    Handle(StepBasic_ConversionBasedUnitAndPlaneAngleUnit)::DownCast(NU);
  Handle(StepBasic_MeasureWithUnit) MWUCBU = CBULU->ConversionFactor();
  afact = MWUCBU->ValueComponent();
  StepBasic_Unit anUnit2 = MWUCBU->UnitComponent();
  if(anUnit2.CaseNum(anUnit2.Value())==1) {
    Handle(StepBasic_NamedUnit) NU2 = anUnit2.NamedUnit();
    if(NU2->IsKind(STANDARD_TYPE(StepBasic_SiUnit))) {
      Handle(StepBasic_SiUnit) SU = Handle(StepBasic_SiUnit)::DownCast(NU2);
      if(SU->Name()==StepBasic_sunRadian) {
        if(SU->HasPrefix()) 
          afact *= STEPConstruct_UnitContext::ConvertSiPrefix (SU->Prefix());
        // convert radian to deg
        afact *= 180/M_PI;
      }
    }
  }
  return Standard_True;
}

//=======================================================================
//function : GetMassConversionFactor
//purpose  : 
//=======================================================================
static Standard_Boolean GetMassConversionFactor(Handle(StepBasic_NamedUnit)& NU,
                                                Standard_Real& afact)
{
  afact=1.;
  if( !NU->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndMassUnit)) ) return Standard_False;
  Handle(StepBasic_ConversionBasedUnitAndMassUnit) CBUMU =
    Handle(StepBasic_ConversionBasedUnitAndMassUnit)::DownCast(NU);
  Handle(StepBasic_MeasureWithUnit) MWUCBU = CBUMU->ConversionFactor();
  afact = MWUCBU->ValueComponent();
  StepBasic_Unit anUnit2 = MWUCBU->UnitComponent();
  if(anUnit2.CaseNum(anUnit2.Value())==1) {
    Handle(StepBasic_NamedUnit) NU2 = anUnit2.NamedUnit();
    if(NU2->IsKind(STANDARD_TYPE(StepBasic_SiUnit))) {
      Handle(StepBasic_SiUnit) SU = Handle(StepBasic_SiUnit)::DownCast(NU2);
      if(SU->Name()==StepBasic_sunGram) {
        if(SU->HasPrefix())
          afact *= STEPConstruct_UnitContext::ConvertSiPrefix (SU->Prefix());
      }
    }
  }
  return Standard_True;
}

//=======================================================================
//function : readPMIPresentation
//purpose  : read polyline or tessellated presentation for 
// (Annotation_Curve_Occurrence or Draughting_Callout)
//=======================================================================
Standard_Boolean readPMIPresentation(const Handle(Standard_Transient)& thePresentEntity,
                                     const Handle(XSControl_TransferReader)& theTR,
                                     const Standard_Real theFact,
                                     TopoDS_Shape& thePresentation, 
                                     Handle(TCollection_HAsciiString)& thePresentName, 
                                     Bnd_Box& theBox)
{
  if (thePresentEntity.IsNull())
    return Standard_False;
  Handle(Transfer_TransientProcess) aTP = theTR->TransientProcess();
  Handle(StepVisual_AnnotationOccurrence) anAO;
  NCollection_Vector<Handle(StepVisual_StyledItem)> anAnnotations;
  if (thePresentEntity->IsKind(STANDARD_TYPE(StepVisual_AnnotationOccurrence)))
  {
    anAO = Handle(StepVisual_AnnotationOccurrence)::DownCast(thePresentEntity);
    if (!anAO.IsNull()) {
      thePresentName = anAO->Name();
      anAnnotations.Append(anAO);
    }
  }
  else if (thePresentEntity->IsKind(STANDARD_TYPE(StepVisual_DraughtingCallout)))
  {
    Handle(StepVisual_DraughtingCallout) aDCallout =
      Handle(StepVisual_DraughtingCallout)::DownCast(thePresentEntity);
    thePresentName = aDCallout->Name();
    for (Standard_Integer i = 1; i <= aDCallout->NbContents() && anAO.IsNull(); i++) {
      anAO = Handle(StepVisual_AnnotationOccurrence)::DownCast(aDCallout->ContentsValue(i).Value());
      if (!anAO.IsNull())
      {
        anAnnotations.Append(anAO);
        continue;
      }
      Handle(StepVisual_TessellatedAnnotationOccurrence) aTesselation =
        aDCallout->ContentsValue(i).TessellatedAnnotationOccurrence();
      if (!aTesselation.IsNull())
        anAnnotations.Append(aTesselation);
    }
  }

  if (!anAnnotations.Length())
    return Standard_False;


  BRep_Builder aB;
  TopoDS_Compound aResAnnotation;
  aB.MakeCompound(aResAnnotation);

  Standard_Integer i = 0;
  Bnd_Box aBox;
  Standard_Integer nbShapes = 0;
  for (; i < anAnnotations.Length(); i++)
  {
    Handle(StepVisual_StyledItem) anItem = anAnnotations(i);
    anAO = Handle(StepVisual_AnnotationOccurrence)::DownCast(anItem);
    TopoDS_Shape anAnnotationShape;
    if (!anAO.IsNull())
    {
      Handle(StepRepr_RepresentationItem) aCurveItem = anAO->Item();
      anAnnotationShape = STEPConstruct::FindShape(aTP, aCurveItem);
      if (anAnnotationShape.IsNull())
      {
        Handle(Transfer_Binder) binder = theTR->Actor()->Transfer(aCurveItem, aTP);
        if (!binder.IsNull() && binder->HasResult()) {
          anAnnotationShape = TransferBRep::ShapeResult(aTP, binder);
        }
      }
    }
    //case of tessellated entities
    else
    {
      Handle(StepRepr_RepresentationItem) aTessItem = anItem->Item();
      if (aTessItem.IsNull())
        continue;
      Handle(StepVisual_TessellatedGeometricSet) aTessSet = Handle(StepVisual_TessellatedGeometricSet)::DownCast(aTessItem);
      if (aTessSet.IsNull())
        continue;
      NCollection_Handle<StepVisual_Array1OfTessellatedItem> aListItems = aTessSet->Items();
      Standard_Integer nb = aListItems.IsNull() ? 0 : aListItems->Length();
      Handle(StepVisual_TessellatedCurveSet) aTessCurve;
      for (Standard_Integer n = 1; n <= nb && aTessCurve.IsNull(); n++)
      {
        aTessCurve = Handle(StepVisual_TessellatedCurveSet)::DownCast(aListItems->Value(n));
      }
      if (aTessCurve.IsNull())
        continue;
      Handle(StepVisual_CoordinatesList) aCoordList = aTessCurve->CoordList();
      if (aCoordList.IsNull())
        continue;
      Handle(TColgp_HArray1OfXYZ)  aPoints = aCoordList->Points();

      if (aPoints.IsNull() || aPoints->Length() == 0)
        continue;
      NCollection_Handle<StepVisual_VectorOfHSequenceOfInteger> aCurves = aTessCurve->Curves();
      Standard_Integer aNbC = (aCurves.IsNull() ? 0 : aCurves->Length());
      TopoDS_Compound aComp;
      aB.MakeCompound(aComp);

      Standard_Integer k = 0;
      for (; k < aNbC; k++)
      {
        Handle(TColStd_HSequenceOfInteger) anIndexes = aCurves->Value(k);
        TopoDS_Wire aCurW;
        aB.MakeWire(aCurW);

        for (Standard_Integer n = 1; n < anIndexes->Length(); n++)
        {
          Standard_Integer ind = anIndexes->Value(n);
          Standard_Integer indnext = anIndexes->Value(n + 1);
          if (ind > aPoints->Length() || indnext > aPoints->Length())
            continue;
          gp_Pnt aP1(aPoints->Value(ind) * theFact);
          gp_Pnt aP2(aPoints->Value(indnext) * theFact);
          BRepBuilderAPI_MakeEdge aMaker(aP1, aP2);
          if (aMaker.IsDone())
          {
            TopoDS_Edge aCurE = aMaker.Edge();
            aB.Add(aCurW, aCurE);
          }
        }
        aB.Add(aComp, aCurW);
      }
      anAnnotationShape = aComp;
    }
    if (!anAnnotationShape.IsNull())
    {
      nbShapes++;
      aB.Add(aResAnnotation, anAnnotationShape);
      if (i == anAnnotations.Length() - 1)
        BRepBndLib::AddClose(anAnnotationShape, aBox);
    }
  }

  thePresentation = aResAnnotation;
  theBox = aBox;
  return (nbShapes > 0);
}

//=======================================================================
//function : readAnnotationPlane
//purpose  : read annotation plane
//=======================================================================
Standard_Boolean readAnnotationPlane(const Handle(StepVisual_AnnotationPlane) theAnnotationPlane,
                                     const Standard_Real theFact,
                                     gp_Ax2& thePlane)
{
  if (theAnnotationPlane.IsNull())
    return Standard_False;
  Handle(StepRepr_RepresentationItem) aPlaneItem = theAnnotationPlane->Item();
  if (aPlaneItem.IsNull())
    return Standard_False;
  Handle(StepGeom_Axis2Placement3d) aA2P3D;
  //retrieve axes from AnnotationPlane
  if (aPlaneItem->IsKind(STANDARD_TYPE(StepGeom_Plane))) {
    Handle(StepGeom_Plane) aPlane = Handle(StepGeom_Plane)::DownCast(aPlaneItem);
    aA2P3D = aPlane->Position();
  }
  else if (aPlaneItem->IsKind(STANDARD_TYPE(StepVisual_PlanarBox))) {
    Handle(StepVisual_PlanarBox) aBox = Handle(StepVisual_PlanarBox)::DownCast(aPlaneItem);
    aA2P3D = aBox->Placement().Axis2Placement3d();
  }
  if (aA2P3D.IsNull())
    return Standard_False;

  gp_Ax2 aPlaneAxes;
  Handle(Geom_Axis2Placement) anAxis = StepToGeom::MakeAxis2Placement(aA2P3D);
  aPlaneAxes = anAxis->Ax2();
  gp_XYZ aLocPos = aPlaneAxes.Location().XYZ();
  aLocPos *= theFact;
  aPlaneAxes.SetLocation(aLocPos);
  thePlane = aPlaneAxes;
  return Standard_True;
}

//=======================================================================
//function : readAnnotation
//purpose  : read annotation plane and position for given GDT
// (Dimension, Geometric_Tolerance, Datum_Feature or Placed_Datum_Target_Feature)
//=======================================================================
void readAnnotation(const Handle(XSControl_TransferReader)& theTR, 
  const Handle(Standard_Transient)& theGDT,
  const Handle(Standard_Transient)& theDimObject)
{
  if (theGDT.IsNull() || theDimObject.IsNull())
    return;
  Handle(TCollection_HAsciiString) aPresentName;
  TopoDS_Compound aResAnnotation;
  Handle(Transfer_TransientProcess) aTP = theTR->TransientProcess();
  const Interface_Graph& aGraph = aTP->Graph();
  // find the proper DraughtingModelItemAssociation
  Interface_EntityIterator subs = aGraph.Sharings(theGDT);
  Handle(StepAP242_DraughtingModelItemAssociation) aDMIA;
  for (subs.Start(); subs.More() && aDMIA.IsNull(); subs.Next()) {
    if (!subs.Value()->IsKind(STANDARD_TYPE(StepAP242_DraughtingModelItemAssociation)))
      continue;
    aDMIA = Handle(StepAP242_DraughtingModelItemAssociation)::DownCast(subs.Value());
    Handle(TCollection_HAsciiString) aName = aDMIA->Name();
    aName->LowerCase();
    if (!aName->Search(new TCollection_HAsciiString("pmi representation to presentation link"))) {
      aDMIA = NULL;
    }
  }
  if (aDMIA.IsNull() || aDMIA->NbIdentifiedItem() == 0)
    return;

  // calculate units
  Handle(StepVisual_DraughtingModel) aDModel = 
    Handle(StepVisual_DraughtingModel)::DownCast(aDMIA->UsedRepresentation());
  Standard_Real aFact = 1;
  if (!aDModel.IsNull())
    GetLengthConversionFactorFromContext(aDModel->ContextOfItems(), aFact);

  // retrieve AnnotationPlane
  Handle(StepRepr_RepresentationItem) aDMIAE = aDMIA->IdentifiedItemValue(1);
  if (aDMIAE.IsNull())
    return;
  gp_Ax2 aPlaneAxes;
  subs = aGraph.Sharings(aDMIAE);
  Handle(StepVisual_AnnotationPlane) anAnPlane;
  for (subs.Start(); subs.More() && anAnPlane.IsNull(); subs.Next()) {
    anAnPlane = Handle(StepVisual_AnnotationPlane)::DownCast(subs.Value());
  }
  Standard_Boolean isHasPlane = readAnnotationPlane(anAnPlane, aFact, aPlaneAxes);

  // set plane axes to XCAF
  if (isHasPlane) {
    if (theDimObject->IsKind(STANDARD_TYPE(XCAFDimTolObjects_DimensionObject))) {
      Handle(XCAFDimTolObjects_DimensionObject) anObj = 
        Handle(XCAFDimTolObjects_DimensionObject)::DownCast(theDimObject);
      Handle(TColgp_HArray1OfPnt) aPnts = new TColgp_HArray1OfPnt(1, 1);
      anObj->SetPlane(aPlaneAxes);
    }
    else if (theDimObject->IsKind(STANDARD_TYPE(XCAFDimTolObjects_DatumObject))) {
      Handle(XCAFDimTolObjects_DatumObject) anObj =
        Handle(XCAFDimTolObjects_DatumObject)::DownCast(theDimObject);
      anObj->SetPlane(aPlaneAxes);
    }
    else if (theDimObject->IsKind(STANDARD_TYPE(XCAFDimTolObjects_GeomToleranceObject))) {
      Handle(XCAFDimTolObjects_GeomToleranceObject) anObj =
        Handle(XCAFDimTolObjects_GeomToleranceObject)::DownCast(theDimObject);
      anObj->SetPlane(aPlaneAxes);
    }
  }

  // Retrieve presentation
  Bnd_Box aBox;
  if (!readPMIPresentation(aDMIAE, theTR, aFact, aResAnnotation, aPresentName, aBox))
    return;
  gp_Pnt aPtext(0., 0., 0.);
  // if Annotation plane location inside bounding box set it to text position
  // else set the center of bounding box to text position
  if (!aBox.IsVoid())
  {
    Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
    if (isHasPlane && !aBox.IsOut(aPlaneAxes.Location())) {
      aPtext = aPlaneAxes.Location();
    }
    else {
      aPtext = gp_Pnt((aXmin + aXmax) * 0.5, (aYmin + aYmax) * 0.5, (aZmin + aZmax) * 0.5);
    }
  }
  else {
    aPtext = aPlaneAxes.Location();
  }

  // set point to XCAF
  if (theDimObject->IsKind(STANDARD_TYPE(XCAFDimTolObjects_DimensionObject))) {
    Handle(XCAFDimTolObjects_DimensionObject) anObj = 
      Handle(XCAFDimTolObjects_DimensionObject)::DownCast(theDimObject);
    anObj->SetPointTextAttach(aPtext);
    anObj->SetPresentation(aResAnnotation, aPresentName);
  }
  else if (theDimObject->IsKind(STANDARD_TYPE(XCAFDimTolObjects_DatumObject))) {
    Handle(XCAFDimTolObjects_DatumObject) anObj =
      Handle(XCAFDimTolObjects_DatumObject)::DownCast(theDimObject);
      anObj->SetPointTextAttach(aPtext);
      anObj->SetPresentation(aResAnnotation, aPresentName);
  }
  else if (theDimObject->IsKind(STANDARD_TYPE(XCAFDimTolObjects_GeomToleranceObject))) {
    Handle(XCAFDimTolObjects_GeomToleranceObject) anObj =
      Handle(XCAFDimTolObjects_GeomToleranceObject)::DownCast(theDimObject);
    anObj->SetPointTextAttach(aPtext);
    anObj->SetPresentation(aResAnnotation, aPresentName);
  }
  return;
}

//=======================================================================
//function : readConnectionPoints
//purpose  : read connection points for given dimension
//=======================================================================
void readConnectionPoints(const Handle(XSControl_TransferReader)& theTR, 
  const Handle(Standard_Transient) theGDT,
  const Handle(XCAFDimTolObjects_DimensionObject)& theDimObject)
{
  if (theGDT.IsNull() || theDimObject.IsNull())
    return;
  Handle(Transfer_TransientProcess) aTP = theTR->TransientProcess();
  const Interface_Graph& aGraph = aTP->Graph();

  //calculate units
  Standard_Real aFact = 1;
  Handle(StepShape_ShapeDimensionRepresentation) aSDR = NULL;
  for (Interface_EntityIterator anIt = aGraph.Sharings(theGDT); aSDR.IsNull() && anIt.More(); anIt.Next()) {
    aSDR = Handle(StepShape_ShapeDimensionRepresentation)::DownCast(anIt.Value());
  }
  if (!aSDR.IsNull())
    GetLengthConversionFactorFromContext(aSDR->ContextOfItems(), aFact);
  
  if (theGDT->IsKind(STANDARD_TYPE(StepShape_DimensionalSize))) {
    // retrieve derived geometry
    Handle(StepShape_DimensionalSize) aDim = Handle(StepShape_DimensionalSize)::DownCast(theGDT);
    Handle(StepRepr_DerivedShapeAspect) aDSA = Handle(StepRepr_DerivedShapeAspect)::DownCast(aDim->AppliesTo());
    if (aDSA.IsNull())
      return;
    Handle(StepAP242_GeometricItemSpecificUsage) aGISU = NULL;
    for (Interface_EntityIterator anIt = aGraph.Sharings(aDSA); aGISU.IsNull() && anIt.More(); anIt.Next()) {
      aGISU = Handle(StepAP242_GeometricItemSpecificUsage)::DownCast(anIt.Value());
    }
    if (aGISU.IsNull() || aGISU->NbIdentifiedItem() == 0)
      return;
    Handle(StepGeom_CartesianPoint) aPoint = Handle(StepGeom_CartesianPoint)::DownCast(aGISU->IdentifiedItem()->Value(1));
    if (aPoint.IsNull()) {
      // try Axis2Placement3d.location instead of CartesianPoint
      Handle(StepGeom_Axis2Placement3d) anA2P3D =
        Handle(StepGeom_Axis2Placement3d)::DownCast(aGISU->IdentifiedItem()->Value(1));
      if (anA2P3D.IsNull())
        return;
      aPoint = anA2P3D->Location();
    }
  
    // set connection point to object
    gp_Pnt aPnt(aPoint->CoordinatesValue(1) * aFact, aPoint->CoordinatesValue(2) * aFact, aPoint->CoordinatesValue(3) * aFact);
    theDimObject->SetPoint(aPnt);
  }
  else if (theGDT->IsKind(STANDARD_TYPE(StepShape_DimensionalLocation))) {
    // retrieve derived geometry
    Handle(StepShape_DimensionalLocation) aDim = Handle(StepShape_DimensionalLocation)::DownCast(theGDT);
    Handle(StepRepr_DerivedShapeAspect) aDSA1 = Handle(StepRepr_DerivedShapeAspect)::DownCast(aDim->RelatingShapeAspect());
    Handle(StepRepr_DerivedShapeAspect) aDSA2 = Handle(StepRepr_DerivedShapeAspect)::DownCast(aDim->RelatedShapeAspect());
    if (aDSA1.IsNull() && aDSA2.IsNull())
      return;
    Handle(StepAP242_GeometricItemSpecificUsage) aGISU1 = NULL;
    Handle(StepAP242_GeometricItemSpecificUsage) aGISU2 = NULL;
    if (!aDSA1.IsNull()) {
      for (Interface_EntityIterator anIt = aGraph.Sharings(aDSA1); aGISU1.IsNull() && anIt.More(); anIt.Next()) {
        aGISU1 = Handle(StepAP242_GeometricItemSpecificUsage)::DownCast(anIt.Value());
      }
    }
    if (!aDSA2.IsNull()) {
      for (Interface_EntityIterator anIt = aGraph.Sharings(aDSA2); aGISU2.IsNull() && anIt.More(); anIt.Next()) {
        aGISU2 = Handle(StepAP242_GeometricItemSpecificUsage)::DownCast(anIt.Value());
      }
    }
    // first point
    if (!aGISU1.IsNull() && aGISU1->NbIdentifiedItem() > 0) {
      Handle(StepGeom_CartesianPoint) aPoint = Handle(StepGeom_CartesianPoint)::DownCast(aGISU1->IdentifiedItem()->Value(1));
      if (aPoint.IsNull()) {
        // try Axis2Placement3d.location instead of CartesianPoint
        Handle(StepGeom_Axis2Placement3d) anA2P3D =
          Handle(StepGeom_Axis2Placement3d)::DownCast(aGISU1->IdentifiedItem()->Value(1));
        if (!anA2P3D.IsNull())
          aPoint = anA2P3D->Location();
      }
      if (!aPoint.IsNull()) {
        // set connection point to object
        gp_Pnt aPnt(aPoint->CoordinatesValue(1) * aFact, aPoint->CoordinatesValue(2) * aFact, aPoint->CoordinatesValue(3) * aFact);
        theDimObject->SetPoint(aPnt);
      }
    }
    // second point
    if (!aGISU2.IsNull() && aGISU2->NbIdentifiedItem() > 0) {
      Handle(StepGeom_CartesianPoint) aPoint = Handle(StepGeom_CartesianPoint)::DownCast(aGISU2->IdentifiedItem()->Value(1));
      if (aPoint.IsNull()) {
        // try Axis2Placement3d.location instead of CartesianPoint
        Handle(StepGeom_Axis2Placement3d) anA2P3D =
          Handle(StepGeom_Axis2Placement3d)::DownCast(aGISU2->IdentifiedItem()->Value(1));
        if (!anA2P3D.IsNull())
          aPoint = anA2P3D->Location();
      }
      if (!aPoint.IsNull()) {
        // set connection point to object
        gp_Pnt aPnt(aPoint->CoordinatesValue(1) * aFact, aPoint->CoordinatesValue(2) * aFact, aPoint->CoordinatesValue(3) * aFact);
        theDimObject->SetPoint2(aPnt);
      }
    }
  }
}

//=======================================================================
//function : ReadDatums
//purpose  : auxilary
//=======================================================================
static Standard_Boolean ReadDatums(const Handle(XCAFDoc_ShapeTool) &STool,
                                   const Handle(XCAFDoc_DimTolTool) &DGTTool,
                                   const Interface_Graph &graph,
                                   const Handle(Transfer_TransientProcess) &TP,
                                   const TDF_Label TolerL,
                                   const Handle(StepDimTol_GeometricToleranceWithDatumReference) GTWDR)
{
  if(GTWDR.IsNull()) return Standard_False;
  Handle(StepDimTol_HArray1OfDatumReference) HADR = GTWDR->DatumSystem();
  if(HADR.IsNull()) return Standard_False;
  for(Standard_Integer idr=1; idr<=HADR->Length(); idr++) {
    Handle(StepDimTol_DatumReference) DR = HADR->Value(idr);
    Handle(StepDimTol_Datum) aDatum = DR->ReferencedDatum();
    if(aDatum.IsNull()) continue;
    Interface_EntityIterator subs4 = graph.Sharings(aDatum);
    for(subs4.Start(); subs4.More(); subs4.Next()) {
      Handle(StepRepr_ShapeAspectRelationship) SAR = 
        Handle(StepRepr_ShapeAspectRelationship)::DownCast(subs4.Value());
      if(SAR.IsNull()) continue;
      Handle(StepDimTol_DatumFeature) DF = 
        Handle(StepDimTol_DatumFeature)::DownCast(SAR->RelatingShapeAspect());
      if(DF.IsNull()) continue;
      Interface_EntityIterator subs5 = graph.Sharings(DF);
      Handle(StepRepr_PropertyDefinition) PropDef;
      for(subs5.Start(); subs5.More() && PropDef.IsNull(); subs5.Next()) {
        PropDef = Handle(StepRepr_PropertyDefinition)::DownCast(subs5.Value());
      }
      if(PropDef.IsNull()) continue;
      Handle(StepShape_AdvancedFace) AF;
      subs5 = graph.Sharings(PropDef);
      for(subs5.Start(); subs5.More(); subs5.Next()) {
        Handle(StepShape_ShapeDefinitionRepresentation) SDR = 
          Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(subs5.Value());
        if(!SDR.IsNull()) {
          Handle(StepRepr_Representation) Repr = SDR->UsedRepresentation();
          if( !Repr.IsNull() && Repr->NbItems()>0 ) {
            Handle(StepRepr_RepresentationItem) RI = Repr->ItemsValue(1);
            AF = Handle(StepShape_AdvancedFace)::DownCast(RI);
          }
        }
      }
      if(AF.IsNull()) return Standard_False;
      Standard_Integer index = TP->MapIndex(AF);
      TopoDS_Shape aSh;
      if(index >0) {
        Handle(Transfer_Binder) binder = TP->MapItem(index);
        aSh = TransferBRep::ShapeResult(binder);
      }
      if(aSh.IsNull()) continue; 
      TDF_Label shL;
      if( !STool->Search(aSh, shL, Standard_True, Standard_True, Standard_True) ) continue;
      DGTTool->SetDatum(shL,TolerL,PropDef->Name(),PropDef->Description(),aDatum->Identification());
    }
  }
  return Standard_True;
}

//=======================================================================
//function : FindShapeIndexForDGT
//purpose  : auxiliary find shape index in map og imported shapes
//=======================================================================
static Standard_Integer FindShapeIndexForDGT(const Handle(Standard_Transient)& theEnt,
  const Handle(XSControl_WorkSession)& theWS)
{
  const Handle(Transfer_TransientProcess) &aTP = theWS->TransferReader()->TransientProcess();
  // try to find index of given entity
  Standard_Integer anIndex = aTP->MapIndex(theEnt);
  if (anIndex > 0)
    return anIndex;
  // if theEnt is a geometry item try to find its topological item
  const Interface_Graph& aGraph = aTP->Graph();
  Interface_EntityIterator anIter = aGraph.Sharings(theEnt);
  for (anIter.Start(); anIter.More(); anIter.Next()) {
    if (anIter.Value()->IsKind(STANDARD_TYPE(StepShape_TopologicalRepresentationItem)))
    {
      anIndex = aTP->MapIndex(anIter.Value());
      if (anIndex > 0)
        return anIndex;
    }
  }
  return 0;
}

//=======================================================================
//function : collectShapeAspect
//purpose  : 
//=======================================================================
static void collectShapeAspect(const Handle(StepRepr_ShapeAspect)& theSA,
                               const Handle(XSControl_WorkSession)& theWS,
                               NCollection_Sequence<Handle(StepRepr_ShapeAspect)>& theSAs)
{
  if (theSA.IsNull())
    return;
  Handle(XSControl_TransferReader) aTR = theWS->TransferReader();
  Handle(Transfer_TransientProcess) aTP = aTR->TransientProcess();
  const Interface_Graph& aGraph = aTP->Graph();
  // Retrieve Shape_Aspect, connected to Representation_Item from Derived_Shape_Aspect
  if (theSA->IsKind(STANDARD_TYPE(StepRepr_DerivedShapeAspect))) {
    Interface_EntityIterator anIter = aGraph.Sharings(theSA);
    Handle(StepRepr_ShapeAspectDerivingRelationship) aSADR = NULL;
    for (; aSADR.IsNull() && anIter.More(); anIter.Next()) {
      aSADR = Handle(StepRepr_ShapeAspectDerivingRelationship)::DownCast(anIter.Value());
    }
    if (!aSADR.IsNull())
      collectShapeAspect(aSADR->RelatedShapeAspect(), theWS, theSAs);
  }
  else if (theSA->IsKind(STANDARD_TYPE(StepDimTol_DatumFeature)) ||
    theSA->IsKind(STANDARD_TYPE(StepDimTol_DatumTarget))) {
    theSAs.Append(theSA);
    return;
  }
  else {
    // Find all children Shape_Aspect
    Standard_Boolean isSimple = Standard_True;
    Interface_EntityIterator anIter = aGraph.Sharings(theSA);
    for (; anIter.More(); anIter.Next()) {
      if (anIter.Value()->IsKind(STANDARD_TYPE(StepRepr_ShapeAspectRelationship)) &&
          !anIter.Value()->IsKind(STANDARD_TYPE(StepShape_DimensionalLocation))) {
        Handle(StepRepr_ShapeAspectRelationship) aSAR =
          Handle(StepRepr_ShapeAspectRelationship)::DownCast(anIter.Value());
        if (aSAR->RelatingShapeAspect() == theSA && !aSAR->RelatedShapeAspect().IsNull()
            && !aSAR->RelatedShapeAspect()->IsKind(STANDARD_TYPE(StepDimTol_Datum))) {
          collectShapeAspect(aSAR->RelatedShapeAspect(), theWS, theSAs);
          isSimple = Standard_False;
        }
      }
    }
    // If not Composite_Shape_Aspect (or subtype) append to sequence.
    if (isSimple)
      theSAs.Append(theSA);
  }
}

//=======================================================================
//function : getShapeLabel
//purpose  : 
//=======================================================================

static TDF_Label getShapeLabel(const Handle(StepRepr_RepresentationItem)& theItem,
                               const Handle(XSControl_WorkSession)& theWS,
                               const Handle(XCAFDoc_ShapeTool)& theShapeTool)
{
  TDF_Label aShapeL;
  const Handle(Transfer_TransientProcess) &aTP = theWS->TransferReader()->TransientProcess();
  Standard_Integer index = FindShapeIndexForDGT(theItem, theWS);
  TopoDS_Shape aShape;
  if (index > 0) {
    Handle(Transfer_Binder) aBinder = aTP->MapItem(index);
    aShape = TransferBRep::ShapeResult(aBinder);
  }
  if (aShape.IsNull())
    return aShapeL;
  theShapeTool->Search(aShape, aShapeL, Standard_True, Standard_True, Standard_True);
  return aShapeL;
}

//=======================================================================
//function : setDatumToXCAF
//purpose  : 
//=======================================================================

Standard_Boolean STEPCAFControl_Reader::setDatumToXCAF(const Handle(StepDimTol_Datum)& theDat,
                                                       const TDF_Label theGDTL,
                                                       const Standard_Integer thePositionCounter,
                                                       const XCAFDimTolObjects_DatumModifiersSequence& theXCAFModifiers,
                                                       const XCAFDimTolObjects_DatumModifWithValue theXCAFModifWithVal,
                                                       const Standard_Real theModifValue,
                                                       const Handle(TDocStd_Document)& theDoc,
                                                       const Handle(XSControl_WorkSession)& theWS)
{
  Handle(XCAFDoc_ShapeTool) aSTool = XCAFDoc_DocumentTool::ShapeTool(theDoc->Main());
  Handle(XCAFDoc_DimTolTool) aDGTTool = XCAFDoc_DocumentTool::DimTolTool(theDoc->Main());
  const Handle(XSControl_TransferReader) &aTR = theWS->TransferReader();
  const Handle(Transfer_TransientProcess) &aTP = aTR->TransientProcess();
  const Interface_Graph& aGraph = aTP->Graph();
  Handle(XCAFDoc_Datum) aDat;
  TDF_LabelSequence aShapeLabels;
  Handle(XCAFDimTolObjects_DatumObject) aDatObj = new XCAFDimTolObjects_DatumObject();

  // Collect all links to shapes
  NCollection_Sequence<Handle(StepRepr_ShapeAspect)> aSAs;
  Interface_EntityIterator anIterD = aGraph.Sharings(theDat);
  for (anIterD.Start(); anIterD.More(); anIterD.Next()) {
    Handle(StepRepr_ShapeAspectRelationship) aSAR = Handle(StepRepr_ShapeAspectRelationship)::DownCast(anIterD.Value());
    if (aSAR.IsNull() || aSAR->RelatingShapeAspect().IsNull())
      continue;
    collectShapeAspect(aSAR->RelatingShapeAspect(), theWS, aSAs);
    Handle(StepDimTol_DatumFeature) aDF = Handle(StepDimTol_DatumFeature)::DownCast(aSAR->RelatingShapeAspect());
    if (!aSAR->RelatingShapeAspect()->IsKind(STANDARD_TYPE(StepDimTol_DatumTarget)))
      readAnnotation(aTR, aSAR->RelatingShapeAspect(), aDatObj);
  }

  // Collect shape labels
  for (Standard_Integer i = 1; i <= aSAs.Length(); i++) {
    Handle(StepRepr_ShapeAspect) aSA = aSAs.Value(i);
    if (aSA.IsNull())
      continue;
    // Skip datum targets
    if (aSA->IsKind(STANDARD_TYPE(StepDimTol_DatumTarget)))
      continue;

    // Process all connected GISU
    Interface_EntityIterator anIter = aGraph.Sharings(aSA);
    for (; anIter.More(); anIter.Next()) {
      Handle(StepAP242_GeometricItemSpecificUsage) aGISU = Handle(StepAP242_GeometricItemSpecificUsage)::DownCast(anIter.Value());
      if (aGISU.IsNull())
        continue;
      for (Standard_Integer j = 1; j <= aGISU->NbIdentifiedItem(); j++) {
        TDF_Label aShapeL = getShapeLabel(aGISU->IdentifiedItemValue(j), theWS, aSTool);
        if (!aShapeL.IsNull())
          aShapeLabels.Append(aShapeL);
      }
    }
  }

  // Process datum targets and create objects for them
  Standard_Boolean isExistDatumTarget = Standard_False;
  for (Standard_Integer i = 1; i <= aSAs.Length(); i++) {
    Handle(StepDimTol_PlacedDatumTargetFeature) aDT = Handle(StepDimTol_PlacedDatumTargetFeature)::DownCast(aSAs.Value(i));
    if (aDT.IsNull())
      continue;
    Handle(XCAFDimTolObjects_DatumObject) aDatTargetObj = new XCAFDimTolObjects_DatumObject();
    XCAFDimTolObjects_DatumTargetType aType;
    if (!STEPCAFControl_GDTProperty::GetDatumTargetType(aDT->Description(), aType))
      continue;
    aDatTargetObj->SetDatumTargetType(aType);
    Standard_Boolean isValidDT = Standard_False;

    // Feature for datum target
    TDF_LabelSequence aDTShapeLabels;
    Interface_EntityIterator aDTIter = aGraph.Sharings(aDT);
    Handle(StepRepr_FeatureForDatumTargetRelationship) aRelationship;
    for (; aDTIter.More() && aRelationship.IsNull(); aDTIter.Next()) {
      aRelationship = Handle(StepRepr_FeatureForDatumTargetRelationship)::DownCast(aDTIter.Value());
    }
    if (!aRelationship.IsNull()) {
      Handle(StepRepr_ShapeAspect) aSA = aRelationship->RelatingShapeAspect();
      Interface_EntityIterator aSAIter = aGraph.Sharings(aSA);
      for (; aSAIter.More(); aSAIter.Next()) {
        Handle(StepAP242_GeometricItemSpecificUsage) aGISU = Handle(StepAP242_GeometricItemSpecificUsage)::DownCast(aSAIter.Value());
        if (aGISU.IsNull())
          continue;
        for (Standard_Integer j = 1; j <= aGISU->NbIdentifiedItem(); j++) {
          TDF_Label aShapeL = getShapeLabel(aGISU->IdentifiedItemValue(j), theWS, aSTool);
          if (!aShapeL.IsNull()) {
            aDTShapeLabels.Append(aShapeL);
            isValidDT = Standard_True;
          }
        }
      }
    }

    if (aType != XCAFDimTolObjects_DatumTargetType_Area && !isValidDT) {
      // Try another way of feature connection
      for (aDTIter.Start(); aDTIter.More(); aDTIter.Next()) {
        Handle(StepAP242_GeometricItemSpecificUsage) aGISU = Handle(StepAP242_GeometricItemSpecificUsage)::DownCast(aDTIter.Value());
        if (aGISU.IsNull())
          continue;
        for (Standard_Integer j = 1; j <= aGISU->NbIdentifiedItem(); j++) {
          TDF_Label aShapeL = getShapeLabel(aGISU->IdentifiedItemValue(j), theWS, aSTool);
          if (!aShapeL.IsNull()) {
            aDTShapeLabels.Append(aShapeL);
            isValidDT = Standard_True;
          }
        }
      }
    }

    if (aType == XCAFDimTolObjects_DatumTargetType_Area) {
      // Area datum target
      Interface_EntityIterator anIterDTF = aGraph.Shareds(aDT);
      Handle(StepAP242_GeometricItemSpecificUsage) aGISU;
      for (; anIterDTF.More() && aGISU.IsNull(); anIterDTF.Next()) {
        aGISU = Handle(StepAP242_GeometricItemSpecificUsage)::DownCast(anIterDTF.Value());
      }
      Handle(StepRepr_RepresentationItem) anItem;
      if (aGISU->NbIdentifiedItem() > 0)
        anItem = aGISU->IdentifiedItemValue(1);
      if (anItem.IsNull())
        continue;
      Standard_Integer anItemIndex = FindShapeIndexForDGT(anItem, theWS);
      if (anItemIndex > 0) {
        Handle(Transfer_Binder) aBinder = aTP->MapItem(anItemIndex);
        TopoDS_Shape anItemShape = TransferBRep::ShapeResult(aBinder);
        aDatTargetObj->SetDatumTarget(anItemShape);
        isValidDT = Standard_True;
      }
    }
    else {
      // Point/line/rectangle/circle datum targets 
      Interface_EntityIterator anIter = aGraph.Sharings(aDT);
      Handle(StepRepr_PropertyDefinition) aPD;
      for (; anIter.More() && aPD.IsNull(); anIter.Next()) {
        aPD = Handle(StepRepr_PropertyDefinition)::DownCast(anIter.Value());
      }
      if (!aPD.IsNull()) {
        anIter = aGraph.Sharings(aPD);
        Handle(StepShape_ShapeDefinitionRepresentation) aSDR;
        for (; anIter.More() && aSDR.IsNull(); anIter.Next()) {
          aSDR = Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(anIter.Value());
        }
        if (!aSDR.IsNull()) {
          Handle(StepShape_ShapeRepresentationWithParameters) aSRWP
            = Handle(StepShape_ShapeRepresentationWithParameters)::DownCast(aSDR->UsedRepresentation());
          if (!aSRWP.IsNull()) {
            isValidDT = Standard_True;
            // Collect parameters of datum target
            for (Standard_Integer j = aSRWP->Items()->Lower(); j <= aSRWP->Items()->Upper(); j++)
            {
              if (aSRWP->ItemsValue(j).IsNull())
                continue;
              if (aSRWP->ItemsValue(j)->IsKind(STANDARD_TYPE(StepGeom_Axis2Placement3d)))
              {
                Handle(StepGeom_Axis2Placement3d) anAx
                  = Handle(StepGeom_Axis2Placement3d)::DownCast(aSRWP->ItemsValue(j));
                Handle(Geom_Axis2Placement) anAxis = StepToGeom::MakeAxis2Placement(anAx);
                aDatTargetObj->SetDatumTargetAxis(anAxis->Ax2());
              }
              else if (aSRWP->ItemsValue(j)->IsKind(STANDARD_TYPE(StepRepr_ReprItemAndLengthMeasureWithUnit)))
              {
                Handle(StepRepr_ReprItemAndLengthMeasureWithUnit) aM =
                  Handle(StepRepr_ReprItemAndLengthMeasureWithUnit)::DownCast(aSRWP->ItemsValue(j));
                Standard_Real aVal = aM->GetMeasureWithUnit()->ValueComponent();
                StepBasic_Unit anUnit = aM->GetMeasureWithUnit()->UnitComponent();
                Standard_Real aFact = 1.;
                if (anUnit.IsNull())
                  continue;
                Handle(StepBasic_NamedUnit) aNU = anUnit.NamedUnit();
                if (aNU.IsNull())
                  continue;
                if (GetLengthConversionFactor(aNU, aFact))
                  aVal = aVal * aFact;
                if (aM->Name()->String().IsEqual("target length") ||
                  aM->Name()->String().IsEqual("target diameter"))
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
    if (isValidDT) {
      TDF_Label aDatL = aDGTTool->AddDatum();
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
      aDatTargetObj->IsDatumTarget(Standard_True);
      aDatTargetObj->SetDatumTargetNumber(aDT->TargetId()->IntegerValue());
      readAnnotation(aTR, aDT, aDatTargetObj);
      aDat->SetObject(aDatTargetObj);
      isExistDatumTarget = Standard_True;
    }
  }

  if (aShapeLabels.Length() > 0 || !isExistDatumTarget) {
    // Create object for datum
    TDF_Label aDatL = aDGTTool->AddDatum();
    myGDTMap.Bind(theDat, aDatL);
    // bind datum label with all reference datum_feature entities
    for (Standard_Integer i = 1; i <= aSAs.Length(); i++) {
      Handle(StepRepr_ShapeAspect) aSA = aSAs.Value(i);
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
    if (aDatObj->GetPresentation().IsNull()) {
      // Try find annotation connected to datum entity (not right case, according recommended practices)
      readAnnotation(aTR, theDat, aDatObj);
    }
    aDat->SetObject(aDatObj);
  }

  return Standard_True;
}


//=======================================================================
//function : ReadDatums
//purpose  : auxilary
//=======================================================================
Standard_Boolean STEPCAFControl_Reader::readDatumsAP242(const Handle(Standard_Transient)& theEnt,
                                                        const TDF_Label theGDTL,
                                                        const Handle(TDocStd_Document)& theDoc,
                                                        const Handle(XSControl_WorkSession)& theWS)
{
  const Handle(XSControl_TransferReader) &aTR = theWS->TransferReader();
  const Handle(Transfer_TransientProcess) &aTP = aTR->TransientProcess();
  const Interface_Graph& aGraph = aTP->Graph();

  Interface_EntityIterator anIter = aGraph.Shareds(theEnt);
  for(anIter.Start(); anIter.More(); anIter.Next()) {
    Handle(Standard_Transient) anAtr = anIter.Value();
    if(anAtr->IsKind(STANDARD_TYPE(StepDimTol_DatumSystem)))
    {
      Standard_Integer aPositionCounter = 0;//position on frame 
      Handle(StepDimTol_DatumSystem) aDS = Handle(StepDimTol_DatumSystem)::DownCast(anAtr);
      Interface_EntityIterator anIterDS = aGraph.Sharings(aDS);
      for(anIterDS.Start(); anIterDS.More(); anIterDS.Next()) {
        Handle(Standard_Transient) anAtrDS = anIterDS.Value();
        if(anAtrDS->IsKind(STANDARD_TYPE(StepAP242_GeometricItemSpecificUsage)))
        {
          //get axis
          Handle(StepAP242_GeometricItemSpecificUsage)aAxGISUI
            = Handle(StepAP242_GeometricItemSpecificUsage)::DownCast(anAtrDS);
          if (aAxGISUI->IdentifiedItemValue(1)->IsKind(STANDARD_TYPE(StepGeom_Axis2Placement3d)))
          {
            Handle(StepGeom_Axis2Placement3d) anAx
              = Handle(StepGeom_Axis2Placement3d)::DownCast(aAxGISUI->IdentifiedItemValue(1));
            Handle(XCAFDoc_GeomTolerance) aTol;
            if(theGDTL.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aTol))
            {
              Handle(XCAFDimTolObjects_GeomToleranceObject) anObj = aTol->GetObject();
              Handle(TColStd_HArray1OfReal) aDirArr = anAx->Axis()->DirectionRatios();
              Handle(TColStd_HArray1OfReal) aDirRArr = anAx->RefDirection()->DirectionRatios();
              Handle(TColStd_HArray1OfReal) aLocArr = anAx->Location()->Coordinates();
              gp_Dir aDir;
              gp_Dir aDirR;
              gp_Pnt aPnt;
              if(!aDirArr.IsNull() && aDirArr->Length() > 2 &&
                !aDirRArr.IsNull() && aDirRArr->Length() > 2 && 
                !aLocArr.IsNull() && aLocArr->Length() > 2)
              {
                aDir.SetCoord(aDirArr->Lower(), aDirArr->Lower()+1, aDirArr->Lower()+2);
                aDirR.SetCoord(aDirRArr->Lower(), aDirRArr->Lower()+1, aDirRArr->Lower()+2);
                aPnt.SetCoord(aLocArr->Lower(), aLocArr->Lower()+1, aLocArr->Lower()+2);
                gp_Ax2 anA(aPnt, aDir, aDirR);
                anObj->SetAxis(anA);
                aTol->SetObject(anObj);
              }
            }
          }
        }
      }
      if (aDS->NbConstituents() > 0)
      {
        //get datum feature and datum target from datum system
        Handle(StepDimTol_HArray1OfDatumReferenceCompartment) aDRCA = aDS->Constituents();
        if(!aDRCA.IsNull())
        {
          for(Standard_Integer i = aDRCA->Lower(); i <= aDRCA->Upper(); i++)
          {
            Handle(StepDimTol_DatumReferenceCompartment) aDRC = aDRCA->Value(i);
            //gete modifiers
            Handle(StepDimTol_HArray1OfDatumReferenceModifier) aModif = aDRC->Modifiers();
            XCAFDimTolObjects_DatumModifiersSequence aXCAFModifiers;
            XCAFDimTolObjects_DatumModifWithValue aXCAFModifWithVal = XCAFDimTolObjects_DatumModifWithValue_None;
            Standard_Real aModifValue = 0;
            if(!aModif.IsNull())
            {
              for(Standard_Integer m = aModif->Lower(); m <= aModif->Upper(); m++)
              {
                if(aModif->Value(m).CaseNumber() == 2)
                  aXCAFModifiers.Append(
                  (XCAFDimTolObjects_DatumSingleModif)aModif->Value(m).
                  SimpleDatumReferenceModifierMember()->Value());
                else if (aModif->Value(m).CaseNumber() == 1)
                {
                  aXCAFModifWithVal = (XCAFDimTolObjects_DatumModifWithValue)(aModif->Value(m).DatumReferenceModifierWithValue()->ModifierType() + 1);
                  Standard_Real aVal = aModif->Value(m).DatumReferenceModifierWithValue()->ModifierValue()->ValueComponent();
                  StepBasic_Unit anUnit = aModif->Value(m).DatumReferenceModifierWithValue()->ModifierValue()->UnitComponent();
                  Standard_Real aFact=1.;
                  if(anUnit.IsNull()) continue;
                  if( !(anUnit.CaseNum(anUnit.Value())==1) ) continue;
                  Handle(StepBasic_NamedUnit) NU = anUnit.NamedUnit();
                  if(GetLengthConversionFactor(NU,aFact)) aVal=aVal*aFact;
                  aModifValue = aVal;
                }
              }
            }
            aPositionCounter++;
            Interface_EntityIterator anIterDRC = aGraph.Shareds(aDRC);
            for(anIterDRC.Start(); anIterDRC.More(); anIterDRC.Next()) {
              
              if(anIterDRC.Value()->IsKind(STANDARD_TYPE(StepDimTol_Datum)))
              {
                Handle(StepDimTol_Datum) aD = Handle(StepDimTol_Datum)::DownCast(anIterDRC.Value());
                setDatumToXCAF(aD, theGDTL, aPositionCounter, aXCAFModifiers, aXCAFModifWithVal, aModifValue,theDoc, theWS);
              }
              else if(anIterDRC.Value()->IsKind(STANDARD_TYPE(StepDimTol_DatumReferenceElement)))
              {
                Handle(StepDimTol_DatumReferenceElement) aDRE 
                  = Handle(StepDimTol_DatumReferenceElement)::DownCast(anIterDRC.Value());
                //get modifiers from group of datums
                Handle(StepDimTol_HArray1OfDatumReferenceModifier) aModifE = aDRE->Modifiers();
                if(!aModifE.IsNull())
                {
                  for(Standard_Integer k = aModifE->Lower(); k <= aModifE->Upper(); k++)
                  {
                    if(aModifE->Value(k).CaseNumber() == 2)
                      aXCAFModifiers.Append(
                      (XCAFDimTolObjects_DatumSingleModif)aModifE->Value(k).
                      SimpleDatumReferenceModifierMember()->Value());
                    else if (aModifE->Value(k).CaseNumber() == 1)
                    {
                      aXCAFModifWithVal = (XCAFDimTolObjects_DatumModifWithValue)(aModifE->Value(k).DatumReferenceModifierWithValue()->ModifierType() + 1);
                      Standard_Real aVal = aModifE->Value(k).DatumReferenceModifierWithValue()->ModifierValue()->ValueComponent();
                      StepBasic_Unit anUnit = aModifE->Value(k).DatumReferenceModifierWithValue()->ModifierValue()->UnitComponent();
                      Standard_Real aFact=1.;
                      if(anUnit.IsNull()) continue;
                      if( !(anUnit.CaseNum(anUnit.Value())==1) ) continue;
                      Handle(StepBasic_NamedUnit) NU = anUnit.NamedUnit();
                      if(GetLengthConversionFactor(NU,aFact)) aVal=aVal*aFact;
                      aModifValue = aVal;
                    }
                  }
                }
                Interface_EntityIterator anIterDRE = aGraph.Shareds(aDRE);
                for(anIterDRE.Start(); anIterDRE.More(); anIterDRE.Next()) {
                  if(anIterDRE.Value()->IsKind(STANDARD_TYPE(StepDimTol_Datum)))
                  {
                    Handle(StepDimTol_Datum) aD = Handle(StepDimTol_Datum)::DownCast(anIterDRE.Value());
                    setDatumToXCAF(aD, theGDTL, aPositionCounter, aXCAFModifiers, aXCAFModifWithVal, aModifValue,theDoc, theWS);
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return Standard_True;
}

//=======================================================================
//function : createGeomTolObjectInXCAF
//purpose  : 
//=======================================================================
TDF_Label STEPCAFControl_Reader::createGDTObjectInXCAF(const Handle(Standard_Transient)& theEnt,
                                                       const Handle(TDocStd_Document)& theDoc,
                                                       const Handle(XSControl_WorkSession)& theWS)
{
  TDF_Label aGDTL;
  if(!theEnt->IsKind(STANDARD_TYPE(StepShape_DimensionalSize)) &&
     !theEnt->IsKind(STANDARD_TYPE(StepShape_DimensionalLocation)) && 
     !theEnt->IsKind(STANDARD_TYPE(StepDimTol_GeometricTolerance)))
  {
    return aGDTL;
  }
  // protection against invalid input
  if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_GeometricTolerance))) {
    Handle(StepDimTol_GeometricTolerance) aGeomTol = Handle(StepDimTol_GeometricTolerance)::DownCast(theEnt);
    if (aGeomTol->TolerancedShapeAspect().IsNull())
      return aGDTL;
  }
  if (theEnt->IsKind(STANDARD_TYPE(StepShape_DimensionalSize))) {
    Handle(StepShape_DimensionalSize) aDim = Handle(StepShape_DimensionalSize)::DownCast(theEnt);
    if (aDim->AppliesTo().IsNull())
      return aGDTL;
  }
  if (theEnt->IsKind(STANDARD_TYPE(StepShape_DimensionalLocation))) {
    Handle(StepShape_DimensionalLocation) aDim = Handle(StepShape_DimensionalLocation)::DownCast(theEnt);
    if (aDim->RelatedShapeAspect().IsNull() || aDim->RelatingShapeAspect().IsNull())
      return aGDTL;
  }

  Handle(XCAFDoc_ShapeTool) aSTool = XCAFDoc_DocumentTool::ShapeTool( theDoc->Main() );
  Handle(XCAFDoc_DimTolTool) aDGTTool = XCAFDoc_DocumentTool::DimTolTool( theDoc->Main() );
  const Handle(XSControl_TransferReader) &aTR = theWS->TransferReader();
  const Handle(Transfer_TransientProcess) &aTP = aTR->TransientProcess();
  const Interface_Graph& aGraph = aTP->Graph();
  Standard_Boolean isAllAround = Standard_False;
  Standard_Boolean isAllOver = Standard_False;
  
  // find RepresentationItem for current Ent
  NCollection_Sequence<Handle(Standard_Transient)> aSeqRI1, aSeqRI2;

  // Collect all Shape_Aspect entities
  Interface_EntityIterator anIter = aGraph.Shareds(theEnt);
  for(anIter.Start(); anIter.More(); anIter.Next()) {
    Handle(Standard_Transient) anAtr = anIter.Value();
    NCollection_Sequence<Handle(StepRepr_ShapeAspect)> aSAs;
    if(anAtr->IsKind(STANDARD_TYPE(StepRepr_ProductDefinitionShape)))
    {
      //if associating tolerances with part (All-Over)
      Interface_EntityIterator anIterSDR = aGraph.Sharings(anAtr);
      for(anIterSDR.Start(); anIterSDR.More(); anIterSDR.Next()) 
      {
        Handle(Standard_Transient) anAtrSDR = anIterSDR.Value();
        if(anAtrSDR->IsKind(STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation)))
        {
          isAllOver = Standard_True;
          Interface_EntityIterator anIterABSR = aGraph.Shareds(anAtrSDR);
          for(anIterABSR.Start(); anIterABSR.More(); anIterABSR.Next()) 
          {
            Handle(Standard_Transient) anAtrABSR = anIterABSR.Value();
            if(anAtrABSR->IsKind(STANDARD_TYPE(StepShape_AdvancedBrepShapeRepresentation)))
            {
              aSeqRI1.Append(anAtrABSR);
            }
          }
        }
      }
    }
    else if(anAtr->IsKind(STANDARD_TYPE(StepShape_DimensionalLocation)) || 
      anAtr->IsKind(STANDARD_TYPE(StepShape_DimensionalSize)))
    {
      //if tolerance attached to dimension
      Interface_EntityIterator anIterDim = aGraph.Shareds(anAtr);
      for(anIterDim.Start(); anIterDim.More(); anIterDim.Next()) 
      {
        Handle(StepRepr_ShapeAspect) aSA = Handle(StepRepr_ShapeAspect)::DownCast(anIterDim.Value());
        if(!aSA.IsNull()) {
          collectShapeAspect(aSA, theWS, aSAs);
        }
      }
    }
    else if(anAtr->IsKind(STANDARD_TYPE(StepRepr_ShapeAspect)))
    {
      if(anAtr->IsKind(STANDARD_TYPE(StepRepr_AllAroundShapeAspect)))
      {
        // if applyed AllAround Modifier
        isAllAround = Standard_True;
      }
      // dimensions and default tolerances
      Handle(StepRepr_ShapeAspect) aSA = Handle(StepRepr_ShapeAspect)::DownCast(anAtr);
      if(!aSA.IsNull()) {
        collectShapeAspect(aSA, theWS, aSAs);
      }
    }

    // Collect all representation items
    if(!aSAs.IsEmpty())
    {
      //get representation items
      NCollection_Sequence<Handle(Standard_Transient)> aSeqRI;
      for(Standard_Integer i = aSAs.Lower(); i <= aSAs.Upper(); i++)
      {
        Interface_EntityIterator anIterSA = aGraph.Sharings(aSAs.Value(i));
        Handle(StepAP242_GeometricItemSpecificUsage) aGISU;
        Handle(StepRepr_PropertyDefinition) PropD;
        for(anIterSA.Start(); anIterSA.More() && aGISU.IsNull() && PropD.IsNull(); anIterSA.Next()) {
          aGISU = Handle(StepAP242_GeometricItemSpecificUsage)::DownCast(anIterSA.Value());
          PropD = Handle(StepRepr_PropertyDefinition)::DownCast(anIterSA.Value());
        }
        if(!PropD.IsNull())//for old version
        {
          Handle(StepRepr_RepresentationItem) RI;
          Interface_EntityIterator subs4 = aGraph.Sharings(PropD);
          for(subs4.Start(); subs4.More(); subs4.Next()) {
            Handle(StepShape_ShapeDefinitionRepresentation) SDR = 
              Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(subs4.Value());
            if(!SDR.IsNull()) {
              Handle(StepRepr_Representation) Repr = SDR->UsedRepresentation();
              if( !Repr.IsNull() && Repr->NbItems()>0 ) {
                RI = Repr->ItemsValue(1);
              }
            }
          }
          if(RI.IsNull()) continue;

          if(theEnt->IsKind(STANDARD_TYPE(StepShape_DimensionalSize))) {
            // read dimensions
            Handle(StepShape_EdgeCurve) EC = Handle(StepShape_EdgeCurve)::DownCast(RI);
            if(EC.IsNull()) continue;
            Handle(TCollection_HAsciiString) aName;
            Handle(StepShape_DimensionalSize) DimSize = 
              Handle(StepShape_DimensionalSize)::DownCast(theEnt);
            Standard_Real dim1=-1.,dim2=-1.;
            subs4 = aGraph.Sharings(DimSize);
            for(subs4.Start(); subs4.More(); subs4.Next()) {
              Handle(StepShape_DimensionalCharacteristicRepresentation) DimCharR = 
                Handle(StepShape_DimensionalCharacteristicRepresentation)::DownCast(subs4.Value());
              if(!DimCharR.IsNull()) {
                Handle(StepShape_ShapeDimensionRepresentation) SDimR = DimCharR->Representation();
                if(!SDimR.IsNull() && SDimR->NbItems()>0) {
                  Handle(StepRepr_RepresentationItem) anItem = SDimR->ItemsValue(1);
                  Handle(StepRepr_ValueRange) VR = Handle(StepRepr_ValueRange)::DownCast(anItem);
                  if(!VR.IsNull()) {
                    aName = VR->Name();
                    //StepRepr_CompoundItemDefinition CID = VR->ItemElement();
                    //if(CID.IsNull()) continue;
                    //Handle(StepRepr_CompoundItemDefinitionMember) CIDM = 
                    //  Handle(StepRepr_CompoundItemDefinitionMember)::DownCast(CID.Value());
                    //if(CIDM.IsNull()) continue;
                    //if(CIDM->ArrTransient().IsNull()) continue;
                    //Handle(StepRepr_HArray1OfRepresentationItem) HARI;
                    //if(CID.CaseMem(CIDM)==1)
                    //  HARI = CID.ListRepresentationItem();
                    //if(CID.CaseMem(CIDM)==2)
                    //  HARI = CID.SetRepresentationItem();
                    Handle(StepRepr_HArray1OfRepresentationItem) HARI = VR->ItemElement();
                    if(HARI.IsNull()) continue;
                    if(HARI->Length()>0) {
                      Handle(StepRepr_RepresentationItem) RI1 = HARI->Value(1);
                      if(RI1.IsNull()) continue;
                      if(RI1->IsKind(STANDARD_TYPE(StepRepr_ReprItemAndLengthMeasureWithUnit))) {
                        Handle(StepRepr_ReprItemAndLengthMeasureWithUnit) RILMWU =
                          Handle(StepRepr_ReprItemAndLengthMeasureWithUnit)::DownCast(RI1);
                        dim1 = RILMWU->GetMeasureWithUnit()->ValueComponent();
                        StepBasic_Unit anUnit = RILMWU->GetMeasureWithUnit()->UnitComponent();
                        Standard_Real afact=1.;
                        if(anUnit.IsNull()) continue;
                        if( !(anUnit.CaseNum(anUnit.Value())==1) ) continue;
                        Handle(StepBasic_NamedUnit) NU = anUnit.NamedUnit();
                        if(GetLengthConversionFactor(NU,afact)) dim1=dim1*afact;
                      }
                    }
                    if(HARI->Length()>1) {
                      Handle(StepRepr_RepresentationItem) RI2 = HARI->Value(2);
                      if(RI2.IsNull()) continue;
                      if(RI2->IsKind(STANDARD_TYPE(StepRepr_ReprItemAndLengthMeasureWithUnit))) {
                        Handle(StepRepr_ReprItemAndLengthMeasureWithUnit) RILMWU =
                          Handle(StepRepr_ReprItemAndLengthMeasureWithUnit)::DownCast(RI2);
                        dim2 = RILMWU->GetMeasureWithUnit()->ValueComponent();
                        StepBasic_Unit anUnit = RILMWU->GetMeasureWithUnit()->UnitComponent();
                        Standard_Real afact=1.;
                        if(anUnit.IsNull()) continue;
                        if( !(anUnit.CaseNum(anUnit.Value())==1) ) continue;
                        Handle(StepBasic_NamedUnit) NU = anUnit.NamedUnit();
                        if(GetLengthConversionFactor(NU,afact)) dim2 = dim2*afact;
                      }
                    }
                  }
                }
              }
            }
            if(dim1<0) continue;
            if(dim2<0) dim2=dim1;
            //cout<<"DimensionalSize: dim1="<<dim1<<"  dim2="<<dim2<<endl;
            // now we know edge_curve and value range therefore
            // we can create corresponding D&GT labels
            Standard_Integer index = aTP->MapIndex(EC);
            TopoDS_Shape aSh;
            if(index >0) {
              Handle(Transfer_Binder) binder = aTP->MapItem(index);
              aSh = TransferBRep::ShapeResult(binder);
            }
            if(aSh.IsNull()) continue; 
            TDF_Label shL;
            if( !aSTool->Search(aSh, shL, Standard_True, Standard_True, Standard_True) ) continue;
            Handle(TColStd_HArray1OfReal) arr = new TColStd_HArray1OfReal(1,2);
            arr->SetValue(1,dim1);
            arr->SetValue(2,dim2);
            aDGTTool->SetDimTol(shL,1,arr,aName,DimSize->Name());
          }
          // read tolerances and datums
          else if(theEnt->IsKind(STANDARD_TYPE(StepDimTol_GeometricTolerance))) {
            Handle(StepDimTol_GeometricTolerance) GT =
              Handle(StepDimTol_GeometricTolerance)::DownCast(theEnt);
            // read common data for tolerance
            //Standard_Real dim = GT->Magnitude()->ValueComponent();
            Handle (StepBasic_MeasureWithUnit) dim3 = GT->Magnitude();
            if(dim3.IsNull()) continue;
            Standard_Real dim = dim3->ValueComponent();
            StepBasic_Unit anUnit = GT->Magnitude()->UnitComponent();
            Standard_Real afact=1.;
            if(anUnit.IsNull()) continue;
            if( !(anUnit.CaseNum(anUnit.Value())==1) ) continue;
            Handle(StepBasic_NamedUnit) NU = anUnit.NamedUnit();
            if(GetLengthConversionFactor(NU,afact)) dim = dim*afact;
            //cout<<"GeometricTolerance: Magnitude = "<<dim<<endl;
            Handle(TColStd_HArray1OfReal) arr = new TColStd_HArray1OfReal(1,1);
            arr->SetValue(1,dim);
            Handle(TCollection_HAsciiString) aName = GT->Name();
            Handle(TCollection_HAsciiString) aDescription = GT->Description();
            Handle(StepShape_AdvancedFace) AF = Handle(StepShape_AdvancedFace)::DownCast(RI);
            if(AF.IsNull()) continue;
            Standard_Integer index = aTP->MapIndex(AF);
            TopoDS_Shape aSh;
            if(index >0) {
              Handle(Transfer_Binder) binder = aTP->MapItem(index);
              aSh = TransferBRep::ShapeResult(binder);
            }
            if(aSh.IsNull()) continue; 
            TDF_Label shL;
            if( !aSTool->Search(aSh, shL, Standard_True, Standard_True, Standard_True) ) continue;
            // read specific data for tolerance
            if(GT->IsKind(STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol))) {
              Handle(StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol) GTComplex =
                Handle(StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol)::DownCast(theEnt);
              Standard_Integer kind=20;
              Handle(StepDimTol_ModifiedGeometricTolerance) MGT = 
                GTComplex->GetModifiedGeometricTolerance();
              if(!MGT.IsNull()) {
                kind = kind + MGT->Modifier()+1;
              }
              TDF_Label TolerL = aDGTTool->SetDimTol(shL,kind,arr,aName,aDescription);
              // translate datums connected with this tolerance
              Handle(StepDimTol_GeometricToleranceWithDatumReference) GTWDR =
                GTComplex->GetGeometricToleranceWithDatumReference();
              if(!GTWDR.IsNull()) {
                ReadDatums(aSTool,aDGTTool,aGraph,aTP,TolerL,GTWDR);
              }
            }
            else if(GT->IsKind(STANDARD_TYPE(StepDimTol_GeometricToleranceWithDatumReference))) {
              Handle(StepDimTol_GeometricToleranceWithDatumReference) GTWDR =
                Handle(StepDimTol_GeometricToleranceWithDatumReference)::DownCast(theEnt);
              if(GTWDR.IsNull()) continue;
              Standard_Integer kind = 0;
              if     (GTWDR->IsKind(STANDARD_TYPE(StepDimTol_AngularityTolerance)))       kind = 24;
              else if(GTWDR->IsKind(STANDARD_TYPE(StepDimTol_CircularRunoutTolerance)))   kind = 25;
              else if(GTWDR->IsKind(STANDARD_TYPE(StepDimTol_CoaxialityTolerance)))       kind = 26;
              else if(GTWDR->IsKind(STANDARD_TYPE(StepDimTol_ConcentricityTolerance)))    kind = 27;
              else if(GTWDR->IsKind(STANDARD_TYPE(StepDimTol_ParallelismTolerance)))      kind = 28;
              else if(GTWDR->IsKind(STANDARD_TYPE(StepDimTol_PerpendicularityTolerance))) kind = 29;
              else if(GTWDR->IsKind(STANDARD_TYPE(StepDimTol_SymmetryTolerance)))         kind = 30;
              else if(GTWDR->IsKind(STANDARD_TYPE(StepDimTol_TotalRunoutTolerance)))      kind = 31;
              //cout<<"GTWDR: kind="<<kind<<endl;
              TDF_Label TolerL = aDGTTool->SetDimTol(shL,kind,arr,aName,aDescription);
              ReadDatums(aSTool,aDGTTool,aGraph,aTP,TolerL,GTWDR);
            }
            else if(GT->IsKind(STANDARD_TYPE(StepDimTol_ModifiedGeometricTolerance))) {
              Handle(StepDimTol_ModifiedGeometricTolerance) MGT =
                Handle(StepDimTol_ModifiedGeometricTolerance)::DownCast(theEnt);
              Standard_Integer kind = 35 + MGT->Modifier();
              aDGTTool->SetDimTol(shL,kind,arr,aName,aDescription);
            }
            else if(GT->IsKind(STANDARD_TYPE(StepDimTol_CylindricityTolerance))) {
              aDGTTool->SetDimTol(shL,38,arr,aName,aDescription);
            }
            else if(GT->IsKind(STANDARD_TYPE(StepDimTol_FlatnessTolerance))) {
              aDGTTool->SetDimTol(shL,39,arr,aName,aDescription);
            }
            else if(GT->IsKind(STANDARD_TYPE(StepDimTol_LineProfileTolerance))) {
              aDGTTool->SetDimTol(shL,40,arr,aName,aDescription);
            }
            else if(GT->IsKind(STANDARD_TYPE(StepDimTol_PositionTolerance))) {
              aDGTTool->SetDimTol(shL,41,arr,aName,aDescription);
            }
            else if(GT->IsKind(STANDARD_TYPE(StepDimTol_RoundnessTolerance))) {
              aDGTTool->SetDimTol(shL,42,arr,aName,aDescription);
            }
            else if(GT->IsKind(STANDARD_TYPE(StepDimTol_StraightnessTolerance))) {
              aDGTTool->SetDimTol(shL,43,arr,aName,aDescription);
            }
            else if(GT->IsKind(STANDARD_TYPE(StepDimTol_SurfaceProfileTolerance))) {
              aDGTTool->SetDimTol(shL,44,arr,aName,aDescription);
            }
          }
        }
        else
        {
          if(aGISU.IsNull()) continue;
          Standard_Integer j =1;
          for ( ; j <= aGISU->NbIdentifiedItem(); j++) {
            aSeqRI.Append(aGISU->IdentifiedItemValue(j));
          }
        }
      }
      if(!aSeqRI.IsEmpty())
      {
        if(aSeqRI1.IsEmpty())
          aSeqRI1 = aSeqRI;
        else 
          aSeqRI2 = aSeqRI;
      }
    }
  }
  if(aSeqRI1.IsEmpty()) 
    return aGDTL;

  TDF_LabelSequence aShLS1, aShLS2;

  // Collect shapes
  for(Standard_Integer i = aSeqRI1.Lower(); i <= aSeqRI1.Upper() ;i++)
  {
    Standard_Integer anIndex = FindShapeIndexForDGT(aSeqRI1.Value(i), theWS);
    TopoDS_Shape aSh;
    if(anIndex >0) {
      Handle(Transfer_Binder) aBinder = aTP->MapItem(anIndex);
      aSh = TransferBRep::ShapeResult(aBinder);
    }
    if(!aSh.IsNull())
    {
      TDF_Label aShL;
      aSTool->Search(aSh, aShL, Standard_True, Standard_True, Standard_True);
      aShLS1.Append(aShL);
    }
  }
  if (!aSeqRI2.IsEmpty())
  {
    //for dimensional location
    for(Standard_Integer i = aSeqRI2.Lower(); i <= aSeqRI2.Upper() ;i++)
    {
      Standard_Integer anIndex = FindShapeIndexForDGT(aSeqRI2.Value(i), theWS);
      TopoDS_Shape aSh;
      if(anIndex >0) {
        Handle(Transfer_Binder) aBinder = aTP->MapItem(anIndex);
        aSh = TransferBRep::ShapeResult(aBinder);
      }
      if(!aSh.IsNull())
      {
        TDF_Label aShL;
        aSTool->Search(aSh, aShL, Standard_True, Standard_True, Standard_True);
        aShLS2.Append(aShL);
      }
    }
  }

  if(!aShLS1.IsEmpty())
  {
    // add to XCAF
    if(!theEnt->IsKind(STANDARD_TYPE(StepDimTol_GeometricTolerance)))
    {
      aGDTL = aDGTTool->AddDimension();
      myGDTMap.Bind(theEnt, aGDTL);
      aDGTTool->Lock(aGDTL);
      Handle(XCAFDoc_Dimension) aDim = XCAFDoc_Dimension::Set(aGDTL);
      TCollection_AsciiString aStr("DGT:Dimensional_");
      if(theEnt->IsKind(STANDARD_TYPE(StepShape_DimensionalSize)))
      {
        aStr.AssignCat("Size");
      }
      else if(theEnt->IsKind(STANDARD_TYPE(StepShape_DimensionalLocation)))
      {
        aStr.AssignCat("Location");
      }
      TDataStd_Name::Set(aGDTL, aStr);

      if(!aShLS2.IsEmpty())
      {
        aDGTTool->SetDimension(aShLS1, aShLS2, aGDTL);
      }
      else
      {
        TDF_LabelSequence aEmptySeq;
        aDGTTool->SetDimension(aShLS1, aEmptySeq, aGDTL);
      }
    }
    else
    {
      aGDTL = aDGTTool->AddGeomTolerance();
      myGDTMap.Bind(theEnt, aGDTL);
      aDGTTool->Lock(aGDTL);
      Handle(XCAFDoc_GeomTolerance) aGTol = XCAFDoc_GeomTolerance::Set(aGDTL);
      TCollection_AsciiString aStr("DGT:GeomTolerance");
      TDataStd_Name::Set(aGDTL, aStr);
      aDGTTool->SetGeomTolerance(aShLS1, aGDTL);
      Handle(XCAFDimTolObjects_GeomToleranceObject) anObj = aGTol->GetObject();
      if(isAllAround)
        anObj->AddModifier(XCAFDimTolObjects_GeomToleranceModif_All_Around);
      else if(isAllOver)
        anObj->AddModifier(XCAFDimTolObjects_GeomToleranceModif_All_Over);
      aGTol->SetObject(anObj);
    }
    readDatumsAP242(theEnt, aGDTL, theDoc, theWS);
  }
  return aGDTL;
}

//=======================================================================
//function : setDimObjectToXCAF
//purpose  : 
//=======================================================================
static void setDimObjectToXCAF(const Handle(Standard_Transient)& theEnt,
                                           const TDF_Label& aDimL,
                                           const Handle(TDocStd_Document)& theDoc,
                                           const Handle(XSControl_WorkSession)& theWS)
{
  Handle(XCAFDoc_ShapeTool) aSTool = XCAFDoc_DocumentTool::ShapeTool( theDoc->Main() );
  Handle(XCAFDoc_DimTolTool) aDGTTool = XCAFDoc_DocumentTool::DimTolTool( theDoc->Main() );
  const Handle(XSControl_TransferReader) &aTR = theWS->TransferReader();
  const Handle(Transfer_TransientProcess) &aTP = aTR->TransientProcess();
  const Interface_Graph& aGraph = aTP->Graph();
  Handle(XCAFDimTolObjects_DimensionObject) aDimObj;
  if(!theEnt->IsKind(STANDARD_TYPE(StepShape_DimensionalSize)) &&
    !theEnt->IsKind(STANDARD_TYPE(StepShape_DimensionalLocation))) 
    return;

  Handle(StepShape_DimensionalSize) aDimSize = 
    Handle(StepShape_DimensionalSize)::DownCast(theEnt);
  Handle(StepShape_DimensionalLocation) aDimLocation = 
    Handle(StepShape_DimensionalLocation)::DownCast(theEnt);

  aDimObj = new XCAFDimTolObjects_DimensionObject();
  Standard_Real aDim1=-1.,aDim2=-1.,aDim3=-1.;
  Handle(StepShape_TypeQualifier) aTQ;
  Handle(StepShape_ValueFormatTypeQualifier) aVFTQ;
  Handle(StepShape_ToleranceValue) aTV;
  Handle(StepShape_LimitsAndFits) aLAF;
  Handle(StepRepr_CompoundRepresentationItem) aCRI;
  Handle(StepGeom_Axis2Placement3d) anAP;

  Interface_EntityIterator anIterDim;
  if(!aDimSize.IsNull())
  {
    anIterDim = aGraph.Sharings(aDimSize);
  }
  else
  {
    anIterDim = aGraph.Sharings(aDimLocation);
  }
  for(anIterDim.Start(); anIterDim.More(); anIterDim.Next()) {
    Handle(StepShape_DimensionalCharacteristicRepresentation) aDCR = 
      Handle(StepShape_DimensionalCharacteristicRepresentation)::DownCast(anIterDim.Value());
    Handle(StepShape_PlusMinusTolerance) aPMT = 
      Handle(StepShape_PlusMinusTolerance)::DownCast(anIterDim.Value());
    if(!aDCR.IsNull()) {
      Handle(StepShape_ShapeDimensionRepresentation) aSDR = aDCR->Representation();
      if(!aSDR.IsNull()) {
        Handle(StepRepr_HArray1OfRepresentationItem) aHARI = aSDR->Items();

        if(!aHARI.IsNull())
        {
          for(Standard_Integer nr = aHARI->Lower(); nr <= aHARI->Upper(); nr++)
          {
            Handle(StepRepr_RepresentationItem) aDRI = aHARI->Value(nr);
            if(aDRI.IsNull()) continue;

            if(aDRI->IsKind(STANDARD_TYPE(StepRepr_ReprItemAndLengthMeasureWithUnit))) {
              //get values
              Handle(StepRepr_ReprItemAndLengthMeasureWithUnit) aRILMWU =
                Handle(StepRepr_ReprItemAndLengthMeasureWithUnit)::DownCast(aDRI);
              Standard_Real aVal = aRILMWU->GetMeasureWithUnit()->ValueComponent();
              StepBasic_Unit anUnit = aRILMWU->GetMeasureWithUnit()->UnitComponent();
              Standard_Real aFact=1.;
              if(anUnit.IsNull()) continue;
              if( !(anUnit.CaseNum(anUnit.Value())==1) ) continue;
              Handle(StepBasic_NamedUnit) NU = anUnit.NamedUnit();
              if(GetLengthConversionFactor(NU,aFact)) aVal=aVal*aFact;
              if(aDim2 > 0)
              {
                if(aDim1 > aDim2)
                {
                  Standard_Real aTmp = aDim1;
                  aDim1 = aDim2;
                  aDim2 = aTmp;
                }
                if(aVal < aDim1)
                  aDim1 = aVal;
                else if(aVal > aDim2)
                  aDim2 = aVal;
              }
              else if(aDim1 > 0)
              {
                if (aVal > aDim1)
                  aDim2 = aVal;
                else
                {
                  aDim2 = aDim1;
                  aDim1 = aVal;
                }
              }
              else
                aDim1 = aVal;
            }
            else if(aDRI->IsKind(STANDARD_TYPE(StepRepr_ReprItemAndPlaneAngleMeasureWithUnit))) {
              //get values
              Handle(StepRepr_ReprItemAndPlaneAngleMeasureWithUnit) aRIPAMWU =
                Handle(StepRepr_ReprItemAndPlaneAngleMeasureWithUnit)::DownCast(aDRI);
              Standard_Real aVal = aRIPAMWU->GetMeasureWithUnit()->ValueComponent();
              StepBasic_Unit anUnit = aRIPAMWU->GetMeasureWithUnit()->UnitComponent();
              Standard_Real aFact=1.;
              if(anUnit.IsNull()) continue;
              if( !(anUnit.CaseNum(anUnit.Value())==1) ) continue;
              Handle(StepBasic_NamedUnit) NU = anUnit.NamedUnit();
              if(GetAngleConversionFactor(NU,aFact)) aVal=aVal*aFact;
              if(aDim2 > 0)
              {
                if(aDim1 > aDim2)
                {
                  Standard_Real aTmp = aDim1;
                  aDim1 = aDim2;
                  aDim2 = aTmp;
                }
                if(aVal < aDim1)
                  aDim1 = aVal;
                else if(aVal > aDim2)
                  aDim2 = aVal;
              }
              else if(aDim1 > 0)
              {
                if (aVal > aDim1)
                  aDim2 = aVal;
                else
                {
                  aDim2 = aDim1;
                  aDim1 = aVal;
                }
              }
              else
                aDim1 = aVal;
            }
            else if(aDRI->IsKind(STANDARD_TYPE(StepShape_QualifiedRepresentationItem))) {
              //get qualifier
              Handle(StepShape_QualifiedRepresentationItem) aQRI =
                Handle(StepShape_QualifiedRepresentationItem)::DownCast(aDRI);
              for(Standard_Integer l = 1; l <= aQRI->NbQualifiers(); l++)
              {
                aTQ = aQRI->Qualifiers()->Value(l).TypeQualifier();
                aVFTQ = aQRI->Qualifiers()->Value(l).ValueFormatTypeQualifier();
              }
            }
            else if (aDRI->IsKind(STANDARD_TYPE(StepRepr_DescriptiveRepresentationItem))) {
              Handle(StepRepr_DescriptiveRepresentationItem) aDescription =
                Handle(StepRepr_DescriptiveRepresentationItem)::DownCast(aDRI);
              aDimObj->AddDescription(aDescription->Description(), aDescription->Name());
            }
            else if(aDRI->IsKind(STANDARD_TYPE(StepRepr_CompoundRepresentationItem))) {
              aCRI = Handle(StepRepr_CompoundRepresentationItem)::DownCast(aDRI);
            }
            else if(aDRI->IsKind(STANDARD_TYPE(StepGeom_Axis2Placement3d)))
            {
              anAP = Handle(StepGeom_Axis2Placement3d)::DownCast(aDRI);
            }
          }
        }
      }
    }
    else if (!aPMT.IsNull())
    {
      StepShape_ToleranceMethodDefinition aTMD = aPMT->Range();
      if(aPMT.IsNull()) continue;
      if(aTMD.CaseNumber() == 1)
        //! 1 -> ToleranceValue from StepShape
        //! 2 -> LimitsAndFits from StepShape
      {
        //plus minus tolerance 
        aTV = aTMD.ToleranceValue();
        if (aTV.IsNull()) continue;

        Standard_Real aVal = aTV->UpperBound()->ValueComponent();
        StepBasic_Unit anUnit = aTV->UpperBound()->UnitComponent();
        Standard_Real aFact=1.;
        if(anUnit.IsNull()) continue;
        if( !(anUnit.CaseNum(anUnit.Value())==1) ) continue;
        Handle(StepBasic_NamedUnit) NU = anUnit.NamedUnit();
        if(GetLengthConversionFactor(NU,aFact)) aVal=aVal*aFact;
        aDim3 = aVal;

        aVal = aTV->LowerBound()->ValueComponent();
        anUnit = aTV->LowerBound()->UnitComponent();
        aFact=1.;
        if(anUnit.IsNull()) continue;
        if( !(anUnit.CaseNum(anUnit.Value())==1) ) continue;
        NU = anUnit.NamedUnit();
        if(GetLengthConversionFactor(NU,aFact)) aVal=aVal*aFact;
        aDim2 = Abs(aVal);
      }
      else
      {
        // class of tolerance
        aLAF = aTMD.LimitsAndFits();
      }
    }
  }

  if(aDim1<0) return;

  if(aDim2 < 0)
    aDimObj->SetValue(aDim1);
  else if (aDim3 < 0)
  {
    Handle(TColStd_HArray1OfReal) anArr = new TColStd_HArray1OfReal(1,2);
    anArr->SetValue(1,aDim1);
    anArr->SetValue(2,aDim2);
    aDimObj->SetValues(anArr);
  }
  else
  {
    Handle(TColStd_HArray1OfReal) anArr = new TColStd_HArray1OfReal(1,3);
    anArr->SetValue(1,aDim1);
    anArr->SetValue(2,aDim2);
    anArr->SetValue(3,aDim3);
    aDimObj->SetValues(anArr);
  }
  if(!aTQ.IsNull()) 
  {
    XCAFDimTolObjects_DimensionQualifier aQ;
    if (STEPCAFControl_GDTProperty::GetDimQualifierType(aTQ->Name(), aQ))
    {
        aDimObj->SetQualifier(aQ);
    }
  }

  if (!aVFTQ.IsNull())
  {
    //A typical value would be 'NR2 2.2'
    TCollection_HAsciiString aFormat = aVFTQ->FormatType();
    Standard_Integer i = aFormat.Location(1,' ',1,aFormat.Length());
    aFormat.SubString(i+1,i+1)->IntegerValue();
    aDimObj->SetNbOfDecimalPlaces(aFormat.SubString(i+1,i+1)->IntegerValue(),
      aFormat.SubString(i+3,i+3)->IntegerValue());
  }

  if (!aLAF.IsNull())
  { 
    //get class of tolerance
    Standard_Boolean aHolle = Standard_False;
    XCAFDimTolObjects_DimensionFormVariance aFV = XCAFDimTolObjects_DimensionFormVariance_None;
    XCAFDimTolObjects_DimensionGrade aG = XCAFDimTolObjects_DimensionGrade_IT01;
    STEPCAFControl_GDTProperty::GetDimClassOfTolerance(aLAF, aHolle, aFV, aG);
    aDimObj->SetClassOfTolerance(aHolle, aFV, aG);
  }

  if (!aCRI.IsNull() && aCRI->ItemElement()->Length() > 0)
  {
    //get modifiers
    XCAFDimTolObjects_DimensionModifiersSequence aModifiers;
    STEPCAFControl_GDTProperty::GetDimModifiers(aCRI, aModifiers);
    if(aModifiers.Length() > 0)
      aDimObj->SetModifiers(aModifiers);
  }

  Handle(TCollection_HAsciiString) aName;
  if(!aDimSize.IsNull())
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
    if(!aDimSize.IsNull())
    {
      Handle(StepShape_AngularSize) anAngSize= 
        Handle(StepShape_AngularSize)::DownCast(aDimSize);
      if(!anAngSize.IsNull())
      {
        //get qualifier for angular value
        aType = XCAFDimTolObjects_DimensionType_Size_Angular;
        if(anAngSize->AngleSelection() == StepShape_Equal)
          aDimObj->SetQualifier(XCAFDimTolObjects_DimensionQualifier_Avg);
        else if (anAngSize->AngleSelection() == StepShape_Large)
          aDimObj->SetQualifier(XCAFDimTolObjects_DimensionQualifier_Max);
        else if (anAngSize->AngleSelection() == StepShape_Small)
          aDimObj->SetQualifier(XCAFDimTolObjects_DimensionQualifier_Min);
      }
    }
    else
    {
      Handle(StepShape_AngularLocation) anAngLoc= 
        Handle(StepShape_AngularLocation)::DownCast(aDimLocation);
      if(!anAngLoc.IsNull())
      {
        //get qualifier for angular value
        aType = XCAFDimTolObjects_DimensionType_Location_Angular;
        if(anAngLoc->AngleSelection() == StepShape_Equal)
          aDimObj->SetQualifier(XCAFDimTolObjects_DimensionQualifier_Avg);
        else if (anAngLoc->AngleSelection() == StepShape_Large)
          aDimObj->SetQualifier(XCAFDimTolObjects_DimensionQualifier_Max);
        else if (anAngLoc->AngleSelection() == StepShape_Small)
          aDimObj->SetQualifier(XCAFDimTolObjects_DimensionQualifier_Min);
      }
    }
    if(aType == XCAFDimTolObjects_DimensionType_Location_None)
    {
      Handle(StepRepr_ShapeAspect) aPSA;
      if(!aDimSize.IsNull())
      {
        Handle(StepShape_DimensionalSizeWithPath) aDimSizeWithPath= 
          Handle(StepShape_DimensionalSizeWithPath)::DownCast(aDimSize);
        if (!aDimSizeWithPath.IsNull())
        {
          aType = XCAFDimTolObjects_DimensionType_Size_WithPath;
          aPSA = aDimSizeWithPath->Path();
        }
      }
      else
      {
        Handle(StepShape_DimensionalLocationWithPath) aDimLocWithPath= 
          Handle(StepShape_DimensionalLocationWithPath)::DownCast(aDimLocation);
        if (!aDimLocWithPath.IsNull())
        {
          aType = XCAFDimTolObjects_DimensionType_Location_WithPath;
          aPSA = aDimLocWithPath->Path();
        }
      }

      if(!aPSA.IsNull())
      {
        //for DimensionalLocationWithPath
        Handle(StepGeom_GeometricRepresentationItem) aGRI;
        Handle(StepAP242_GeometricItemSpecificUsage) aPGISU;
        Interface_EntityIterator anIterDSWP = aGraph.Sharings(aPSA);
        for(anIterDSWP.Start(); anIterDSWP.More() && aPGISU.IsNull(); anIterDSWP.Next()) {
          aPGISU = Handle(StepAP242_GeometricItemSpecificUsage)::DownCast(anIterDSWP.Value());
        }
        if(aPGISU.IsNull()) return;
        if(aPGISU->NbIdentifiedItem() > 0) {
          aGRI = Handle(StepGeom_GeometricRepresentationItem)::DownCast(aPGISU->IdentifiedItemValue(1));
        }
        if(aGRI.IsNull()) return;
        Handle(StepRepr_RepresentationItem) aPRI;
        Interface_EntityIterator anIterGRI = aGraph.Sharings(aGRI);
        for(anIterGRI.Start(); anIterGRI.More() && aPGISU.IsNull(); anIterGRI.Next()) {
          aPRI = Handle(StepRepr_RepresentationItem)::DownCast(anIterGRI.Value());
        }
        Standard_Integer anIndex = FindShapeIndexForDGT(aPRI, theWS);
        TopoDS_Edge aSh;
        if(anIndex >0) {
          Handle(Transfer_Binder) aBinder = aTP->MapItem(anIndex);
          aSh = TopoDS::Edge(TransferBRep::ShapeResult(aBinder));
        }
        if(aSh.IsNull()) return; 
        aDimObj->SetPath(aSh);
      }
      else if(!anAP.IsNull())
      {
        if(anAP->Name()->String().IsEqual("orientation") && !anAP->Axis().IsNull())
        {
          //for Oriented Dimensional Location
          Handle(TColStd_HArray1OfReal) aDirArr = anAP->RefDirection()->DirectionRatios();
          gp_Dir aDir;
          if(!aDirArr.IsNull() && aDirArr->Length() > 2)
          {
            aDir.SetCoord(aDirArr->Lower(), aDirArr->Lower()+1, aDirArr->Lower()+2);
            aDimObj->SetDirection(aDir);
          }
          else if(aDirArr->Length() > 1)
          {
            aDir.SetCoord(aDirArr->Lower(), aDirArr->Lower()+1, 0);
            aDimObj->SetDirection(aDir);
          }
        }
      }
    }
  }
  aDimObj->SetType(aType);

  
  if(!aDimObj.IsNull())
  {

    Handle(XCAFDoc_Dimension) aDim;

    if(aDimL.FindAttribute(XCAFDoc_Dimension::GetID(),aDim))
    {
      readAnnotation(aTR, theEnt, aDimObj);
      readConnectionPoints(aTR, theEnt, aDimObj);
      aDim->SetObject(aDimObj);
    }
  }
}

//=======================================================================
//function : getTolType
//purpose  : 
//=======================================================================
static Standard_Boolean getTolType(const Handle(Standard_Transient)& theEnt,
                       XCAFDimTolObjects_GeomToleranceType& theType)
{
  if(theEnt.IsNull() || !theEnt->IsKind(STANDARD_TYPE(StepDimTol_GeometricTolerance))) 
    return Standard_False;
  theType = XCAFDimTolObjects_GeomToleranceType_None;
  if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthDatRef)))
  {
    Handle(StepDimTol_GeoTolAndGeoTolWthDatRef) anE = Handle(StepDimTol_GeoTolAndGeoTolWthDatRef)::DownCast(theEnt);
    theType = STEPCAFControl_GDTProperty::GetGeomToleranceType(anE->GetToleranceType());
  }
  else if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol)))
  {
    Handle(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol) anE = 
      Handle(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol)::DownCast(theEnt);
    theType = STEPCAFControl_GDTProperty::GetGeomToleranceType(anE->GetToleranceType());
  }
  else if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod)))
  {
    Handle(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod) anE = 
      Handle(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod)::DownCast(theEnt);
    theType = STEPCAFControl_GDTProperty::GetGeomToleranceType(anE->GetToleranceType());
  }
  else if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthMaxTol)))
  {
    Handle(StepDimTol_GeoTolAndGeoTolWthMaxTol) anE = 
      Handle(StepDimTol_GeoTolAndGeoTolWthMaxTol)::DownCast(theEnt);
    theType = STEPCAFControl_GDTProperty::GetGeomToleranceType(anE->GetToleranceType());
  }
  else if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthMod)))
  {
    Handle(StepDimTol_GeoTolAndGeoTolWthMod) anE = 
      Handle(StepDimTol_GeoTolAndGeoTolWthMod)::DownCast(theEnt);
    theType = STEPCAFControl_GDTProperty::GetGeomToleranceType(anE->GetToleranceType());
  }
  else if (theEnt->IsKind(STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol)))
  {
    Handle(StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol) anE = 
      Handle(StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol)::DownCast(theEnt);
    theType = STEPCAFControl_GDTProperty::GetGeomToleranceType(anE->GetToleranceType());
  }
  else if(theEnt->IsKind(STANDARD_TYPE(StepDimTol_AngularityTolerance)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_Angularity;
  }
  else if(theEnt->IsKind(STANDARD_TYPE(StepDimTol_CircularRunoutTolerance)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_CircularRunout;
  }
  else if(theEnt->IsKind(STANDARD_TYPE(StepDimTol_RoundnessTolerance)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_CircularityOrRoundness;
  }
  else if(theEnt->IsKind(STANDARD_TYPE(StepDimTol_CoaxialityTolerance)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_Coaxiality;
  }
  else if(theEnt->IsKind(STANDARD_TYPE(StepDimTol_ConcentricityTolerance)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_Concentricity;
  }
  else if(theEnt->IsKind(STANDARD_TYPE(StepDimTol_CylindricityTolerance)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_Cylindricity;
  }
  else if(theEnt->IsKind(STANDARD_TYPE(StepDimTol_FlatnessTolerance)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_Flatness;
  }
  else if(theEnt->IsKind(STANDARD_TYPE(StepDimTol_ParallelismTolerance)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_Parallelism;
  }
  else if(theEnt->IsKind(STANDARD_TYPE(StepDimTol_PerpendicularityTolerance)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_Perpendicularity;
  }
  else if(theEnt->IsKind(STANDARD_TYPE(StepDimTol_PositionTolerance)) || 
     theEnt->IsKind(STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_Position;
  }
  else if(theEnt->IsKind(STANDARD_TYPE(StepDimTol_LineProfileTolerance)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_ProfileOfLine;
  }
  else if(theEnt->IsKind(STANDARD_TYPE(StepDimTol_SurfaceProfileTolerance)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_ProfileOfSurface;
  }
  else if(theEnt->IsKind(STANDARD_TYPE(StepDimTol_StraightnessTolerance)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_Straightness;
  }
  else if(theEnt->IsKind(STANDARD_TYPE(StepDimTol_SymmetryTolerance)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_Symmetry;
  }
  else if(theEnt->IsKind(STANDARD_TYPE(StepDimTol_TotalRunoutTolerance)))
  {
    theType = XCAFDimTolObjects_GeomToleranceType_TotalRunout;
  }
  return Standard_True;
}
//=======================================================================
//function : setGeomTolObjectToXCAF
//purpose  : 
//=======================================================================
static void setGeomTolObjectToXCAF(const Handle(Standard_Transient)& theEnt,
                                           const TDF_Label& theTolL,
                                           const Handle(TDocStd_Document)& theDoc,
                                           const Handle(XSControl_WorkSession)& theWS)
{
  Handle(XCAFDoc_ShapeTool) aSTool = XCAFDoc_DocumentTool::ShapeTool( theDoc->Main() );
  Handle(XCAFDoc_DimTolTool) aDGTTool = XCAFDoc_DocumentTool::DimTolTool( theDoc->Main() );
  const Handle(XSControl_TransferReader) &aTR = theWS->TransferReader();
  const Handle(Transfer_TransientProcess) &aTP = aTR->TransientProcess();
  const Interface_Graph& aGraph = aTP->Graph();
  Handle(XCAFDoc_GeomTolerance) aGTol;
  if(!theTolL.FindAttribute(XCAFDoc_GeomTolerance::GetID(), aGTol))
  {
    return;
  }
  Handle(XCAFDimTolObjects_GeomToleranceObject) aTolObj = aGTol->GetObject();
  Handle(StepDimTol_GeometricTolerance) aTolEnt = Handle(StepDimTol_GeometricTolerance)::DownCast(theEnt);

  XCAFDimTolObjects_GeomToleranceType aType = XCAFDimTolObjects_GeomToleranceType_None;
  getTolType(theEnt, aType);
  aTolObj->SetType(aType);
  if(!aTolEnt->Magnitude().IsNull()){
    //get value
    Standard_Real aVal = aTolEnt->Magnitude()->ValueComponent();
    StepBasic_Unit anUnit = aTolEnt->Magnitude()->UnitComponent();
    Standard_Real aFact=1.;
    if(anUnit.IsNull()) return;
    if( !(anUnit.CaseNum(anUnit.Value())==1) ) return;
    Handle(StepBasic_NamedUnit) NU = anUnit.NamedUnit();
    if(GetLengthConversionFactor(NU,aFact))
      aVal=aVal*aFact;
    aTolObj->SetValue(aVal);
  }
  //get modifiers
  XCAFDimTolObjects_GeomToleranceTypeValue aTypeV = XCAFDimTolObjects_GeomToleranceTypeValue_None;
  Interface_EntityIterator anIter = aGraph.Sharings(aTolEnt);
  for(anIter.Start(); anIter.More(); anIter.Next()) {
    if(anIter.Value()->IsKind(STANDARD_TYPE(StepDimTol_ToleranceZone))){
      Handle(StepDimTol_ToleranceZoneForm) aForm 
                            = Handle(StepDimTol_ToleranceZone)::DownCast(anIter.Value())->Form();
      STEPCAFControl_GDTProperty::GetTolValueType(aForm->Name(), aTypeV);
      Interface_EntityIterator anIt = aGraph.Sharings(anIter.Value());
      for(anIt.Start(); anIt.More(); anIt.Next()) {
        if(anIt.Value()->IsKind(STANDARD_TYPE(StepDimTol_ProjectedZoneDefinition))){
          Handle(StepDimTol_ProjectedZoneDefinition) aPZone
                              = Handle(StepDimTol_ProjectedZoneDefinition)::DownCast(anIt.Value());
          if(!aPZone->ProjectionLength().IsNull())
          {
            Standard_Real aVal = aPZone->ProjectionLength()->ValueComponent();
            StepBasic_Unit anUnit = aPZone->ProjectionLength()->UnitComponent();
            Standard_Real aFact=1.;
            if(anUnit.IsNull()) return;
            if( !(anUnit.CaseNum(anUnit.Value())==1) ) return;
            Handle(StepBasic_NamedUnit) NU = anUnit.NamedUnit();
            if(GetLengthConversionFactor(NU,aFact))
              aVal=aVal*aFact;
            aTolObj->SetValueOfZoneModifier(aVal);
            aTolObj->SetZoneModifier(XCAFDimTolObjects_GeomToleranceZoneModif_Projected);
          }
        }
        else if (anIt.Value()->IsKind(STANDARD_TYPE(StepDimTol_RunoutZoneDefinition)))
        {
          Handle(StepDimTol_RunoutZoneDefinition) aRZone
                                = Handle(StepDimTol_RunoutZoneDefinition)::DownCast(anIt.Value());
          if(!aRZone->Orientation().IsNull())
          {
            Standard_Real aVal = aRZone->Orientation()->Angle()->ValueComponent();
            StepBasic_Unit anUnit = aRZone->Orientation()->Angle()->UnitComponent();
            Standard_Real aFact=1.;
            if(anUnit.IsNull()) continue;
            if( !(anUnit.CaseNum(anUnit.Value())==1) ) continue;
            Handle(StepBasic_NamedUnit) NU = anUnit.NamedUnit();
            if(GetAngleConversionFactor(NU,aFact)) aVal=aVal*aFact;
            aTolObj->SetValueOfZoneModifier(aVal);
            aTolObj->SetZoneModifier(XCAFDimTolObjects_GeomToleranceZoneModif_Runout);
          }
        }
      }
      aTolObj->SetTypeOfValue(aTypeV);
    }
  }
  Handle(StepDimTol_HArray1OfGeometricToleranceModifier) aModifiers;
  if (aTolEnt->IsKind(STANDARD_TYPE(StepDimTol_GeometricToleranceWithModifiers)))
  {
    aModifiers = Handle(StepDimTol_GeometricToleranceWithModifiers)::DownCast(aTolEnt)->Modifiers();
  }
  else if (aTolEnt->IsKind(STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod)))
  {
    aModifiers = Handle(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod)
                            ::DownCast(aTolEnt)->GetGeometricToleranceWithModifiers()->Modifiers();
  }
  else if (aTolEnt->IsKind(STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthMod)))
  {
    aModifiers = Handle(StepDimTol_GeoTolAndGeoTolWthMod)
                             ::DownCast(aTolEnt)->GetGeometricToleranceWithModifiers()->Modifiers();
  }
  else if (aTolEnt->IsKind(STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthMaxTol)))
  {
    aModifiers = Handle(StepDimTol_GeoTolAndGeoTolWthMaxTol)
                             ::DownCast(aTolEnt)->GetGeometricToleranceWithModifiers()->Modifiers();
  }
  else if (aTolEnt->IsKind(STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol)))
  {
    aModifiers = Handle(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol)
                             ::DownCast(aTolEnt)->GetGeometricToleranceWithModifiers()->Modifiers();
  }
  if(!aModifiers.IsNull())
  {
    for(Standard_Integer i = aModifiers->Lower(); i <= aModifiers->Upper(); i++)
    {
      if(aModifiers->Value(i) == StepDimTol_GTMLeastMaterialRequirement)
        aTolObj->SetMaterialRequirementModifier(XCAFDimTolObjects_GeomToleranceMatReqModif_L);
      else if (aModifiers->Value(i) == StepDimTol_GTMMaximumMaterialRequirement)
        aTolObj->SetMaterialRequirementModifier(XCAFDimTolObjects_GeomToleranceMatReqModif_M);
      else
        aTolObj->AddModifier((XCAFDimTolObjects_GeomToleranceModif)aModifiers->Value(i));
    }
  }
  Standard_Real aVal = 0;
  StepBasic_Unit anUnit;
  if (aTolEnt->IsKind(STANDARD_TYPE(StepDimTol_GeometricToleranceWithMaximumTolerance)))
  {
    Handle(StepDimTol_GeometricToleranceWithMaximumTolerance) aMax = Handle(StepDimTol_GeometricToleranceWithMaximumTolerance)::DownCast(aTolEnt);
    aVal = aMax->MaximumUpperTolerance()->ValueComponent();
    anUnit = aMax->MaximumUpperTolerance()->UnitComponent();
  }
  else if (aTolEnt->IsKind(STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthMaxTol)))
  {
    Handle(StepDimTol_GeoTolAndGeoTolWthMaxTol) aMax = 
      Handle(StepDimTol_GeoTolAndGeoTolWthMaxTol)::DownCast(aTolEnt);
    aVal = aMax->GetMaxTolerance()->ValueComponent();
    anUnit = aMax->GetMaxTolerance()->UnitComponent();
  }
  else if (aTolEnt->IsKind(STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol)))
  {
    Handle(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol) aMax = 
      Handle(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol)::DownCast(aTolEnt);
    aVal = aMax->GetMaxTolerance()->ValueComponent();
    anUnit = aMax->GetMaxTolerance()->UnitComponent();
  }
  if (!anUnit.IsNull() && (anUnit.CaseNum(anUnit.Value()) == 1))
  {
    Standard_Real aFact=1.;
    Handle(StepBasic_NamedUnit) NU = anUnit.NamedUnit();
    if(GetAngleConversionFactor(NU,aFact)) aVal=aVal*aFact;
    aTolObj->SetMaxValueModifier(aVal);
  }
  
  readAnnotation(aTR, theEnt, aTolObj);
  aGTol->SetObject(aTolObj);
}

//=======================================================================
//function : ReadGDTs
//purpose  : 
//=======================================================================

Standard_Boolean STEPCAFControl_Reader::ReadGDTs(const Handle(XSControl_WorkSession)& theWS,
                                                 Handle(TDocStd_Document)& theDoc)
{
  const Handle(Interface_InterfaceModel) &aModel = theWS->Model();
  const Interface_Graph& aGraph = theWS->Graph();
  const Handle(XSControl_TransferReader) &aTR = theWS->TransferReader();
  Handle(StepData_StepModel) aSM = Handle(StepData_StepModel)::DownCast(aModel);
  Interface_EntityIterator anI = aSM->Header();
  Handle(HeaderSection_FileSchema) aH;
  for(anI.Start(); anI.More() && aH.IsNull();anI.Next())
    aH = Handle(HeaderSection_FileSchema)::DownCast(anI.Value());
  Handle(XCAFDoc_ShapeTool) aSTool = XCAFDoc_DocumentTool::ShapeTool( theDoc->Main() );
  Handle(XCAFDoc_DimTolTool) aDGTTool = XCAFDoc_DocumentTool::DimTolTool( theDoc->Main() );
  if ( aDGTTool.IsNull() ) return Standard_False;

  Standard_Integer nb = aModel->NbEntities();
  for(Standard_Integer i=1; i<=nb; i++) {
    Handle(Standard_Transient) anEnt = aModel->Value(i);
    if(anEnt->IsKind(STANDARD_TYPE(StepShape_DimensionalSize)) || 
       anEnt->IsKind(STANDARD_TYPE(StepShape_DimensionalLocation)) ||
       anEnt->IsKind(STANDARD_TYPE(StepDimTol_GeometricTolerance))) {
      TDF_Label aGDTL = createGDTObjectInXCAF(anEnt,theDoc,theWS);
      if(!aGDTL.IsNull()){
        if(anEnt->IsKind(STANDARD_TYPE(StepDimTol_GeometricTolerance))){
          setGeomTolObjectToXCAF(anEnt, aGDTL,theDoc,theWS);
        }
        else {
          setDimObjectToXCAF(anEnt, aGDTL,theDoc,theWS);
        }
      }
    }
    else if (anEnt->IsKind(STANDARD_TYPE(StepVisual_DraughtingCallout)) ||
      anEnt->IsKind(STANDARD_TYPE(StepVisual_AnnotationOccurrence)))
    {
      // Protection against import presentation twice
      Handle(StepVisual_DraughtingCallout) aDC;
      for (Interface_EntityIterator anIter = aGraph.Sharings(anEnt); anIter.More() && aDC.IsNull(); anIter.Next()) {
        aDC = Handle(StepVisual_DraughtingCallout)::DownCast(anIter.Value());
      }
      if (!aDC.IsNull())
        continue;
      // Read presentations for PMIs without semantic data.
      Handle(StepAP242_DraughtingModelItemAssociation) aDMIA;
      TDF_LabelSequence aShapesL;
      for (Interface_EntityIterator anIter = aGraph.Sharings(anEnt); anIter.More() && aDMIA.IsNull(); anIter.Next()) {
        aDMIA = Handle(StepAP242_DraughtingModelItemAssociation)::DownCast(anIter.Value());
      }
      if (!aDMIA.IsNull()) {
        // Check entity, skip all, attached to GDTs
        Handle(StepRepr_ShapeAspect) aDefinition = aDMIA->Definition().ShapeAspect();
        if (!aDefinition.IsNull()) {
          Standard_Boolean isConnectedToGDT = Standard_False;
          // Skip if definition is a datum
          if (aDefinition->IsKind(STANDARD_TYPE(StepDimTol_Datum)) ||
            aDefinition->IsKind(STANDARD_TYPE(StepDimTol_DatumTarget)) ||
            aDefinition->IsKind(STANDARD_TYPE(StepDimTol_DatumFeature)) ||
            aDefinition->IsKind(STANDARD_TYPE(StepRepr_CompShAspAndDatumFeatAndShAsp))) {
            isConnectedToGDT = Standard_True;
          }
          // Skip if any GDT is applied to definition
          for (Interface_EntityIterator anIter = aGraph.Sharings(aDefinition); anIter.More() && !isConnectedToGDT; anIter.Next()) {
            if (anIter.Value()->IsKind(STANDARD_TYPE(StepShape_DimensionalSize)) ||
              anIter.Value()->IsKind(STANDARD_TYPE(StepShape_DimensionalLocation)) ||
              anIter.Value()->IsKind(STANDARD_TYPE(StepDimTol_GeometricTolerance))) {
              isConnectedToGDT = Standard_True;
              continue;
            }
            Handle(StepRepr_ShapeAspectRelationship) aSAR = Handle(StepRepr_ShapeAspectRelationship)::DownCast(anIter.Value());
            if (!aSAR.IsNull()) {
              Handle(StepRepr_ShapeAspect) aSA = aSAR->RelatedShapeAspect();
              if (!aSA.IsNull()) {
                if (aSA->IsKind(STANDARD_TYPE(StepDimTol_Datum)) ||
                  aSA->IsKind(STANDARD_TYPE(StepDimTol_DatumTarget)) ||
                  aSA->IsKind(STANDARD_TYPE(StepDimTol_DatumFeature)) ||
                  aSA->IsKind(STANDARD_TYPE(StepRepr_CompShAspAndDatumFeatAndShAsp))) {
                  isConnectedToGDT = Standard_True;
                }
                for (Interface_EntityIterator aDimIter = aGraph.Sharings(aSA); aDimIter.More() && !isConnectedToGDT; aDimIter.Next()) {
                  if (aDimIter.Value()->IsKind(STANDARD_TYPE(StepShape_DimensionalSize)) ||
                    aDimIter.Value()->IsKind(STANDARD_TYPE(StepShape_DimensionalLocation)) ||
                    aDimIter.Value()->IsKind(STANDARD_TYPE(StepDimTol_GeometricTolerance))) {
                    isConnectedToGDT = Standard_True;
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
        NCollection_Sequence<Handle(StepRepr_ShapeAspect)> aSAs;
        collectShapeAspect(aDefinition, theWS, aSAs);
        for (Standard_Integer aSAIt = 1; aSAIt <= aSAs.Length(); aSAIt++) {
          Handle(StepAP242_GeometricItemSpecificUsage) aGISU;
          for (Interface_EntityIterator anIter = aGraph.Sharings(aSAs.Value(aSAIt)); anIter.More() && aGISU.IsNull(); anIter.Next())
            aGISU = Handle(StepAP242_GeometricItemSpecificUsage)::DownCast(anIter.Value());
          if (aGISU.IsNull())
            continue;
          for (Standard_Integer anItemIt = 1; anItemIt <= aGISU->NbIdentifiedItem(); anItemIt++) {
            TDF_Label aLabel = getShapeLabel(aGISU->IdentifiedItemValue(anItemIt), theWS, XCAFDoc_DocumentTool::ShapeTool(theDoc->Main()));
            if (!aLabel.IsNull())
              aShapesL.Append(aLabel);
          }
        }
      }
      Standard_Boolean isCommonLabel = (aShapesL.Length() == 0);

      // Calculate unit
      Standard_Real aFact = 1.0;
      if (!aDMIA.IsNull() && !aDMIA->UsedRepresentation().IsNull())
        GetLengthConversionFactorFromContext(aDMIA->UsedRepresentation()->ContextOfItems(), aFact);

      // Presentation
      TopoDS_Shape aPresentation;
      Handle(TCollection_HAsciiString) aPresentName;
      Bnd_Box aBox;
      if (!readPMIPresentation(anEnt, aTR, aFact, aPresentation, aPresentName, aBox))
        continue;
      // Annotation plane
      Handle(StepVisual_AnnotationPlane) anAnPlane;
      for (Interface_EntityIterator anIter = aGraph.Sharings(anEnt); anIter.More() && anAnPlane.IsNull(); anIter.Next())
        anAnPlane = Handle(StepVisual_AnnotationPlane)::DownCast(anIter.Value());
      
      // Set object to XCAF
      TDF_Label aGDTL = aDGTTool->AddDimension();
      myGDTMap.Bind(anEnt, aGDTL);
      aDGTTool->Lock(aGDTL);
      Handle(XCAFDimTolObjects_DimensionObject) aDimObj = new XCAFDimTolObjects_DimensionObject();
      Handle(XCAFDoc_Dimension) aDim = XCAFDoc_Dimension::Set(aGDTL);
      TCollection_AsciiString aStr("DGT:");
      if (isCommonLabel) {
        aStr.AssignCat("Common_label");
        aDimObj->SetType(XCAFDimTolObjects_DimensionType_CommonLabel);
      }
      else {
        aStr.AssignCat("Dimension");
        aDimObj->SetType(XCAFDimTolObjects_DimensionType_DimensionPresentation);
      }
      TDataStd_Name::Set(aGDTL, aStr);
      TDF_LabelSequence anEmptySeq2;
      aDGTTool->SetDimension(aShapesL, anEmptySeq2, aGDTL);
      gp_Ax2 aPlaneAxes;
      if (!anAnPlane.IsNull()) {
        if (readAnnotationPlane(anAnPlane, aFact, aPlaneAxes))
          aDimObj->SetPlane(aPlaneAxes);
      }
      aDimObj->SetPresentation(aPresentation, aPresentName);
      aDim->SetObject(aDimObj);
    }
  }
  return Standard_True;
}


//=======================================================================
//function : FindSolidForPDS
//purpose  : auxilary
//=======================================================================

static Handle(StepShape_SolidModel) FindSolidForPDS(const Handle(StepRepr_ProductDefinitionShape) &PDS,
                                                    const Interface_Graph &graph)
{
  Handle(StepShape_SolidModel) SM;
  Interface_EntityIterator subs = graph.Sharings(PDS);
  Handle(StepShape_ShapeRepresentation) SR;
  for(subs.Start(); subs.More() && SM.IsNull(); subs.Next()) {
    Handle(StepShape_ShapeDefinitionRepresentation) SDR =
      Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(subs.Value());
    if(SDR.IsNull()) continue;
    SR = Handle(StepShape_ShapeRepresentation)::DownCast(SDR->UsedRepresentation());
    if(SR.IsNull()) continue;
    for(Standard_Integer i=1; i<=SR->NbItems() && SM.IsNull(); i++) {
      SM = Handle(StepShape_SolidModel)::DownCast(SR->ItemsValue(i));
    }
    if(SM.IsNull()) {
      Interface_EntityIterator subs1 = graph.Sharings(SR);
      for(subs1.Start(); subs1.More() && SM.IsNull(); subs1.Next()) {
        Handle(StepRepr_RepresentationRelationship) RR =
          Handle(StepRepr_RepresentationRelationship)::DownCast(subs1.Value());
        if(RR.IsNull()) continue;
        Handle(StepShape_ShapeRepresentation) SR2;
        if(RR->Rep1()==SR) SR2 = Handle(StepShape_ShapeRepresentation)::DownCast(RR->Rep2());
        else SR2 = Handle(StepShape_ShapeRepresentation)::DownCast(RR->Rep1());
        if(SR2.IsNull()) continue;
        for(Standard_Integer i2=1; i2<=SR2->NbItems() && SM.IsNull(); i2++) {
          SM = Handle(StepShape_SolidModel)::DownCast(SR2->ItemsValue(i2));
        }
      }
    }
  }
  return SM;
}


//=======================================================================
//function : ReadMaterials
//purpose  : 
//=======================================================================

Standard_Boolean STEPCAFControl_Reader::ReadMaterials(const Handle(XSControl_WorkSession) &WS,
                                                      Handle(TDocStd_Document)& Doc,
                                                      const Handle(TColStd_HSequenceOfTransient) &SeqPDS) const
{
  const Handle(XSControl_TransferReader) &TR = WS->TransferReader();
  const Handle(Transfer_TransientProcess) &TP = TR->TransientProcess();
  Handle(XCAFDoc_ShapeTool) STool = XCAFDoc_DocumentTool::ShapeTool( Doc->Main() );
  Handle(XCAFDoc_MaterialTool) MatTool = XCAFDoc_DocumentTool::MaterialTool( Doc->Main() );
  if(MatTool.IsNull()) return Standard_False;
  
  const Interface_Graph& graph = TP->Graph();
  for(Standard_Integer i=1; i<=SeqPDS->Length(); i++) {
    Handle(StepRepr_ProductDefinitionShape) PDS =
      Handle(StepRepr_ProductDefinitionShape)::DownCast(SeqPDS->Value(i));
    if(PDS.IsNull()) 
      continue;
    Handle(StepBasic_ProductDefinition) aProdDef = PDS->Definition().ProductDefinition();
    if(aProdDef.IsNull())
      continue;
    Handle(TCollection_HAsciiString) aName = new TCollection_HAsciiString("");
    Handle(TCollection_HAsciiString) aDescription = new TCollection_HAsciiString("");
    Handle(TCollection_HAsciiString) aDensName = new TCollection_HAsciiString("");
    Handle(TCollection_HAsciiString) aDensValType = new TCollection_HAsciiString("");
    Standard_Real aDensity=0;
    Interface_EntityIterator subs = graph.Sharings( aProdDef);
    for(subs.Start(); subs.More(); subs.Next()) {
      Handle(StepRepr_PropertyDefinition) PropD =
        Handle(StepRepr_PropertyDefinition)::DownCast(subs.Value());
      if(PropD.IsNull()) continue;
      Interface_EntityIterator subs1 = graph.Sharings(PropD);
      for(subs1.Start(); subs1.More(); subs1.Next()) {
        Handle(StepRepr_PropertyDefinitionRepresentation) PDR =
          Handle(StepRepr_PropertyDefinitionRepresentation)::DownCast(subs1.Value());
        if(PDR.IsNull()) continue;
        Handle(StepRepr_Representation) Repr = PDR->UsedRepresentation();
        if(Repr.IsNull()) continue;
        Standard_Integer ir;
        for(ir=1; ir<=Repr->NbItems(); ir++) {
          Handle(StepRepr_RepresentationItem) RI = Repr->ItemsValue(ir);
          if(RI.IsNull()) continue;
          if(RI->IsKind(STANDARD_TYPE(StepRepr_DescriptiveRepresentationItem))) {
            // find name and description for material
            Handle(StepRepr_DescriptiveRepresentationItem) DRI =
              Handle(StepRepr_DescriptiveRepresentationItem)::DownCast(RI);
            aName = DRI->Name();
            
            aDescription = DRI->Description();
            if(aName.IsNull())
              aName = aDescription;
          }
          if(RI->IsKind(STANDARD_TYPE(StepRepr_MeasureRepresentationItem))) {
            // try to find density for material
            Handle(StepRepr_MeasureRepresentationItem) MRI =
              Handle(StepRepr_MeasureRepresentationItem)::DownCast(RI);
            aDensity = MRI->Measure()->ValueComponent();
            aDensName = MRI->Name();
            aDensValType = new TCollection_HAsciiString(MRI->Measure()->ValueComponentMember()->Name());
            StepBasic_Unit aUnit = MRI->Measure()->UnitComponent();
            if(!aUnit.IsNull()) {
              Handle(StepBasic_DerivedUnit) DU = aUnit.DerivedUnit();
              if(DU.IsNull()) continue;
              for(Standard_Integer idu=1; idu<=DU->NbElements(); idu++) {
                Handle(StepBasic_DerivedUnitElement) DUE = DU->ElementsValue(idu);
                Handle(StepBasic_NamedUnit) NU = DUE->Unit();
                Standard_Real afact=1.;
                if(NU->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndLengthUnit))) {
                  if(GetLengthConversionFactor(NU,afact)) aDensity = aDensity/(afact*afact*afact);
                  // transfer length value for Density from millimeter to santimeter
                  // in order to result density has dimension gram/(sm*sm*sm)
                  aDensity = aDensity*1000.;
                }
                if(NU->IsKind(STANDARD_TYPE(StepBasic_ConversionBasedUnitAndMassUnit))) {
                  if(GetMassConversionFactor(NU,afact)) aDensity=aDensity*afact;
                }
              }
            }
          }
        }
      }
    }
   
    if( aName.IsNull() || aName->Length()==0 ) 
      continue;
    // find shape label amd create Material link
    TopoDS_Shape aSh;
    Handle(StepShape_SolidModel) SM = FindSolidForPDS(PDS,graph);
    if(!SM.IsNull()) {
      Standard_Integer index = TP->MapIndex(SM);
      if(index >0) {
        Handle(Transfer_Binder) binder = TP->MapItem(index);
        if(!binder.IsNull())
          aSh = TransferBRep::ShapeResult(binder);
      }
    }
    if(aSh.IsNull()) continue; 
    TDF_Label shL;
    if( !STool->Search(aSh, shL, Standard_True, Standard_True, Standard_True) ) continue;
    MatTool->SetMaterial(shL,aName,aDescription,aDensity,aDensName,aDensValType);
  }

  return Standard_True;
}

//=======================================================================
//function : collectViewShapes
//purpose  : collect all labels of representations in given representation
//=======================================================================

void collectViewShapes(const Handle(XSControl_WorkSession)& theWS,
  const Handle(TDocStd_Document)& theDoc,
  const Handle(StepRepr_Representation) theRepr,
  TDF_LabelSequence& theShapes)
{
  Handle(XSControl_TransferReader) aTR = theWS->TransferReader();
  Handle(Transfer_TransientProcess) aTP = aTR->TransientProcess();
  const Interface_Graph& aGraph = aTP->Graph();
  Handle(XCAFDoc_ShapeTool) aSTool = XCAFDoc_DocumentTool::ShapeTool(theDoc->Main());
  Standard_Integer anIndex = aTP->MapIndex(theRepr);
  TopoDS_Shape aSh;
  if (anIndex > 0) {
    Handle(Transfer_Binder) aBinder = aTP->MapItem(anIndex);
    aSh = TransferBRep::ShapeResult(aBinder);
  }
  if (!aSh.IsNull()) {
    TDF_Label aShL;
    aSTool->FindShape(aSh, aShL);
    if (!aShL.IsNull())
      theShapes.Append(aShL);
  }
  Interface_EntityIterator anIter = aGraph.Sharings(theRepr);
  for (; anIter.More(); anIter.Next()) {
    if (!anIter.Value()->IsKind(STANDARD_TYPE(StepRepr_RepresentationRelationship)))
      continue;
    Handle(StepRepr_RepresentationRelationship) aReprRelationship = Handle(StepRepr_RepresentationRelationship)::DownCast(anIter.Value());
    if (aReprRelationship->Rep1() != theRepr)
      collectViewShapes(theWS, theDoc, aReprRelationship->Rep1(), theShapes);
  }
}

//=======================================================================
//function : buildClippingPlanes
//purpose  :
//=======================================================================
Handle(TCollection_HAsciiString) buildClippingPlanes(const Handle(StepGeom_GeometricRepresentationItem)& theClippingCameraModel,
  TDF_LabelSequence& theClippingPlanes,
  const Handle(XCAFDoc_ClippingPlaneTool) theTool)
{
  Handle(TCollection_HAsciiString) anExpression = new TCollection_HAsciiString();
  NCollection_Sequence<Handle(StepGeom_GeometricRepresentationItem)> aPlanes;
  Handle(TCollection_HAsciiString) anOperation = new TCollection_HAsciiString("*");

  // Store operands
  if (theClippingCameraModel->IsKind(STANDARD_TYPE(StepVisual_CameraModelD3MultiClipping))) {
    Handle(StepVisual_CameraModelD3MultiClipping) aCameraModel =
      Handle(StepVisual_CameraModelD3MultiClipping)::DownCast(theClippingCameraModel);
    // Root of clipping planes tree
    if (aCameraModel->ShapeClipping()->Length() == 1) {
      Handle(StepVisual_CameraModelD3MultiClippingUnion) aCameraModelUnion =
        aCameraModel->ShapeClipping()->Value(1).CameraModelD3MultiClippingUnion();
      if (!aCameraModelUnion.IsNull())
        return buildClippingPlanes(aCameraModelUnion, theClippingPlanes, theTool);
    }
    for (Standard_Integer i = 1; i <= aCameraModel->ShapeClipping()->Length(); i++) {
      aPlanes.Append(Handle(StepGeom_GeometricRepresentationItem)::DownCast(aCameraModel->ShapeClipping()->Value(i).Value()));
    }
  }
  else if (theClippingCameraModel->IsKind(STANDARD_TYPE(StepVisual_CameraModelD3MultiClippingUnion))) {
    Handle(StepVisual_CameraModelD3MultiClippingUnion) aCameraModel =
      Handle(StepVisual_CameraModelD3MultiClippingUnion)::DownCast(theClippingCameraModel);
    anOperation = new TCollection_HAsciiString("+");
    for (Standard_Integer i = 1; i <= aCameraModel->ShapeClipping()->Length(); i++) {
      aPlanes.Append(Handle(StepGeom_GeometricRepresentationItem)::DownCast(aCameraModel->ShapeClipping()->Value(i).Value()));
    }
  }
  else if (theClippingCameraModel->IsKind(STANDARD_TYPE(StepVisual_CameraModelD3MultiClippingIntersection))) {
    Handle(StepVisual_CameraModelD3MultiClippingIntersection) aCameraModel =
      Handle(StepVisual_CameraModelD3MultiClippingIntersection)::DownCast(theClippingCameraModel);
    for (Standard_Integer i = 1; i <= aCameraModel->ShapeClipping()->Length(); i++) {
      aPlanes.Append(Handle(StepGeom_GeometricRepresentationItem)::DownCast(aCameraModel->ShapeClipping()->Value(i).Value()));
    }
  }
  // Build expression
  anExpression->AssignCat("(");
  for (Standard_Integer i = 1; i <= aPlanes.Length(); i++) {
    Handle(StepGeom_Plane) aPlaneEnt = Handle(StepGeom_Plane)::DownCast(aPlanes.Value(i));
    if (!aPlaneEnt.IsNull()) {
      Handle(Geom_Plane) aPlane = StepToGeom::MakePlane(aPlaneEnt);
      if (!aPlane.IsNull()) {
        TDF_Label aPlaneL = theTool->AddClippingPlane(aPlane->Pln(), aPlaneEnt->Name());
        theClippingPlanes.Append(aPlaneL);
        TCollection_AsciiString anEntry;
        TDF_Tool::Entry(aPlaneL, anEntry);
        anExpression->AssignCat(new TCollection_HAsciiString(anEntry));
      }
    }
    else {
      anExpression->AssignCat(buildClippingPlanes(aPlanes.Value(i), theClippingPlanes, theTool));
    }
    anExpression->AssignCat(anOperation);
  }
  // Insert brace instead of operation after last operand.
  anExpression->SetValue(anExpression->Length(), ')');
  return anExpression;
}


//=======================================================================
//function : ReadViews
//purpose  :
//=======================================================================
Standard_Boolean STEPCAFControl_Reader::ReadViews(const Handle(XSControl_WorkSession)& theWS, Handle(TDocStd_Document)& theDoc) const
{
  const Handle(Interface_InterfaceModel) &aModel = theWS->Model();
  Handle(XCAFDoc_ShapeTool) aSTool = XCAFDoc_DocumentTool::ShapeTool(theDoc->Main());
  Handle(XCAFDoc_DimTolTool) aDGTTool = XCAFDoc_DocumentTool::DimTolTool(theDoc->Main());
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(theDoc->Main());
  if (aDGTTool.IsNull()) return Standard_False;

  Standard_Integer nb = aModel->NbEntities();
  for (Standard_Integer i = 1; i <= nb; i++) {
    Handle(Standard_Transient) anEnt = aModel->Value(i);
    if (!anEnt->IsKind(STANDARD_TYPE(StepVisual_CameraModelD3)))
      continue;
    Handle(XCAFView_Object) anObj = new XCAFView_Object();
    // Import attributes of view
    Handle(StepVisual_CameraModelD3) aCameraModel = Handle(StepVisual_CameraModelD3)::DownCast(anEnt);
    anObj->SetName(aCameraModel->Name());
    Handle(Geom_Axis2Placement) anAxis = StepToGeom::MakeAxis2Placement(aCameraModel->ViewReferenceSystem());
    anObj->SetViewDirection(anAxis->Direction());
    anObj->SetUpDirection(anAxis->Direction() ^ anAxis->XDirection());
    Handle(StepVisual_ViewVolume) aViewVolume = aCameraModel->PerspectiveOfVolume();
    XCAFView_ProjectionType aType = XCAFView_ProjectionType_NoCamera;
    if (aViewVolume->ProjectionType() == StepVisual_copCentral)
      aType = XCAFView_ProjectionType_Central;
    else if (aViewVolume->ProjectionType() == StepVisual_copParallel)
      aType = XCAFView_ProjectionType_Parallel;
    anObj->SetType(aType);
    Handle(Geom_CartesianPoint) aPoint = StepToGeom::MakeCartesianPoint(aViewVolume->ProjectionPoint());
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
    Handle(StepVisual_CameraModelD3MultiClipping) aClippingCameraModel =
      Handle(StepVisual_CameraModelD3MultiClipping)::DownCast(aCameraModel);
    TDF_LabelSequence aClippingPlanes;
    if (!aClippingCameraModel.IsNull()) {
      Handle(TCollection_HAsciiString) aClippingExpression;
      Handle(XCAFDoc_ClippingPlaneTool) aClippingPlaneTool = XCAFDoc_DocumentTool::ClippingPlaneTool(theDoc->Main());
      aClippingExpression = buildClippingPlanes(aClippingCameraModel, aClippingPlanes, aClippingPlaneTool);
      anObj->SetClippingExpression(aClippingExpression);
    }
    // Collect shapes and GDTs
    TDF_LabelSequence aShapes, aGDTs;
    Handle(XSControl_TransferReader) aTR = theWS->TransferReader();
    Handle(Transfer_TransientProcess) aTP = aTR->TransientProcess();
    const Interface_Graph& aGraph = aTP->Graph();
    Handle(StepVisual_DraughtingModel) aDModel;
    Interface_EntityIterator anIter = aGraph.Sharings(aCameraModel);
    for (; anIter.More() && aDModel.IsNull(); anIter.Next()) {
      aDModel = Handle(StepVisual_DraughtingModel)::DownCast(anIter.Value());
    }
    if (aDModel.IsNull())
      return Standard_False;

    anIter = aGraph.Shareds(aDModel);
    for (; anIter.More(); anIter.Next()) {
      if (anIter.Value()->IsKind(STANDARD_TYPE(StepRepr_MappedItem))) {
        Handle(StepRepr_MappedItem) anItem = Handle(StepRepr_MappedItem)::DownCast(anIter.Value());
        Handle(StepRepr_Representation) aRepr = anItem->MappingSource()->MappedRepresentation();
        collectViewShapes(theWS, theDoc, aRepr, aShapes);
      }
      else if (anIter.Value()->IsKind(STANDARD_TYPE(StepVisual_AnnotationOccurrence)) ||
        anIter.Value()->IsKind(STANDARD_TYPE(StepVisual_DraughtingCallout))) {
        Interface_EntityIterator aDMIAIter = aGraph.Sharings(anIter.Value());
        for (; aDMIAIter.More(); aDMIAIter.Next()) {
          if (!aDMIAIter.Value()->IsKind(STANDARD_TYPE(StepAP242_DraughtingModelItemAssociation)))
            continue;
          Handle(StepAP242_DraughtingModelItemAssociation) aDMIA =
            Handle(StepAP242_DraughtingModelItemAssociation)::DownCast(aDMIAIter.Value());
          TDF_Label aGDTL;
          Standard_Boolean isFind = myGDTMap.Find(aDMIA->Definition().Value(), aGDTL);
          if (!isFind) {
            isFind = myGDTMap.Find(anIter.Value(), aGDTL);
          }
          if (isFind)
            aGDTs.Append(aGDTL);
        }
      }
      else if (anIter.Value()->IsKind(STANDARD_TYPE(StepVisual_AnnotationPlane))) {
        Handle(StepVisual_AnnotationPlane) aPlane = Handle(StepVisual_AnnotationPlane)::DownCast(anIter.Value());
        for (Standard_Integer j = 1; j <= aPlane->NbElements(); j++) {
          Interface_EntityIterator aDMIAIter = aGraph.Sharings(anIter.Value());
          for (; aDMIAIter.More(); aDMIAIter.Next()) {
            if (!aDMIAIter.Value()->IsKind(STANDARD_TYPE(StepAP242_DraughtingModelItemAssociation)))
              continue;
            Handle(StepAP242_DraughtingModelItemAssociation) aDMIA =
              Handle(StepAP242_DraughtingModelItemAssociation)::DownCast(aDMIAIter.Value());
            TDF_Label aGDTL;
            Standard_Boolean isFind = myGDTMap.Find(aDMIA->Definition().Value(), aGDTL);
            if (isFind)
              aGDTs.Append(aGDTL);
          }
        }
      }
    }
    TDF_Label aViewL = aViewTool->AddView();
    Handle(XCAFDoc_View) aView = XCAFDoc_View::Set(aViewL);
    aView->SetObject(anObj);
    aViewTool->SetView(aShapes, aGDTs, aClippingPlanes, aViewL);
    aViewTool->Lock(aViewL);
  }
  return Standard_True;
}

//=======================================================================
//function : SettleShapeData
//purpose  :
//=======================================================================

TDF_Label STEPCAFControl_Reader::SettleShapeData(const Handle(StepRepr_RepresentationItem)& theItem,
                                                 TDF_Label& theLab,
                                                 const Handle(XCAFDoc_ShapeTool)& theShapeTool,
                                                 const Handle(Transfer_TransientProcess)& TP) const
{
  TDF_Label aResult = theLab;

  Handle(TCollection_HAsciiString) hName = theItem->Name();
  if ( hName.IsNull() || hName->IsEmpty() )
    return aResult;

  Handle(Transfer_Binder) aBinder = TP->Find(theItem);
  if ( aBinder.IsNull() )
    return aResult;

  TopoDS_Shape aShape = TransferBRep::ShapeResult(aBinder);
  if ( aShape.IsNull() )
    return aResult;

  // Allocate sub-Label
  aResult = AllocateSubLabel(theLab);

  TCollection_AsciiString aName = hName->String();
  TDataStd_Name::Set(aResult, aName);
  theShapeTool->SetShape(aResult, aShape);

  return aResult;
}

//=======================================================================
//function : ExpandSubShapes
//purpose  :
//=======================================================================

void STEPCAFControl_Reader::ExpandSubShapes(const Handle(XCAFDoc_ShapeTool)& ShapeTool,
                                            const XCAFDoc_DataMapOfShapeLabel& ShapeLabelMap,
                                            const STEPCAFControl_DataMapOfShapePD& ShapePDMap) const
{
  const Handle(Transfer_TransientProcess)& TP = Reader().WS()->TransferReader()->TransientProcess();
  NCollection_DataMap<TopoDS_Shape, Handle(TCollection_HAsciiString)> ShapeNameMap;
  TColStd_MapOfTransient aRepItems;

  // Read translation control variables
  Standard_Boolean doReadSNames = (Interface_Static::IVal("read.stepcaf.subshapes.name") > 0);

  if ( !doReadSNames )
    return;

  const Interface_Graph& Graph = Reader().WS()->Graph();

  for ( STEPCAFControl_DataMapIteratorOfDataMapOfShapePD it(ShapePDMap); it.More(); it.Next() )
  {
    const TopoDS_Shape& aRootShape = it.Key();
    const Handle(StepBasic_ProductDefinition)& aPDef = it.Value();
    if ( aPDef.IsNull() )
      continue;

    // Find SDR by Product
    Handle(StepShape_ShapeDefinitionRepresentation) aSDR;
    Interface_EntityIterator entIt = Graph.TypedSharings( aPDef, STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation) );
    for ( entIt.Start(); entIt.More(); entIt.Next() )
    {
      const Handle(Standard_Transient)& aReferer = entIt.Value();
      aSDR = Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(aReferer);
      if ( !aSDR.IsNull() )
        break;
    }

    if ( aSDR.IsNull() )
      continue;

    // Access shape representation
    Handle(StepShape_ShapeRepresentation)
      aShapeRepr = Handle(StepShape_ShapeRepresentation)::DownCast( aSDR->UsedRepresentation() );

    if ( aShapeRepr.IsNull() )
      continue;

    // Access representation items
    Handle(StepRepr_HArray1OfRepresentationItem) aReprItems = aShapeRepr->Items();

    if ( aReprItems.IsNull() )
      continue;

    if ( !ShapeLabelMap.IsBound(aRootShape) )
      continue;

    TDF_Label aRootLab = ShapeLabelMap.Find(aRootShape);

    StepRepr_SequenceOfRepresentationItem aMSBSeq;
    StepRepr_SequenceOfRepresentationItem aSBSMSeq;

    // Iterate over the top level representation items collecting the
    // topological containers to expand
    for ( Standard_Integer i = aReprItems->Lower(); i <= aReprItems->Upper(); ++i )
    {
      Handle(StepRepr_RepresentationItem) aTRepr = aReprItems->Value(i);
      if ( aTRepr->IsKind( STANDARD_TYPE(StepShape_ManifoldSolidBrep) ) )
        aMSBSeq.Append(aTRepr);
      else if ( aTRepr->IsKind( STANDARD_TYPE(StepShape_ShellBasedSurfaceModel) ) )
        aSBSMSeq.Append(aTRepr);
    }

    // Insert intermediate OCAF Labels for SOLIDs in case there are more
    // than one Manifold Solid BRep in the Shape Representation
    Standard_Boolean doInsertSolidLab = (aMSBSeq.Length() > 1);

    // Expand Manifold Solid BReps
    for ( Standard_Integer i = 1; i <= aMSBSeq.Length(); ++i )
    {
      const Handle(StepRepr_RepresentationItem)& aManiRepr = aMSBSeq.Value(i);

      // Put additional Label for SOLID
      TDF_Label aManiLab;
      if ( doInsertSolidLab )
        aManiLab = SettleShapeData(aManiRepr, aRootLab, ShapeTool, TP);
      else
        aManiLab = aRootLab;

      ExpandManifoldSolidBrep(aManiLab, aMSBSeq.Value(i), TP, ShapeTool);
    }

    // Expand Shell-Based Surface Models
    for ( Standard_Integer i = 1; i <= aSBSMSeq.Length(); ++i )
      ExpandSBSM(aRootLab, aSBSMSeq.Value(i), TP, ShapeTool);
  }
}

//=======================================================================
//function : ExpandManifoldSolidBrep
//purpose  :
//=======================================================================

void STEPCAFControl_Reader::ExpandManifoldSolidBrep(TDF_Label& ShapeLab,
                                                    const Handle(StepRepr_RepresentationItem)& Repr,
                                                    const Handle(Transfer_TransientProcess)& TP,
                                                    const Handle(XCAFDoc_ShapeTool)& ShapeTool) const
{
  // Access outer shell
  Handle(StepShape_ManifoldSolidBrep) aMSB = Handle(StepShape_ManifoldSolidBrep)::DownCast(Repr);
  Handle(StepShape_ConnectedFaceSet) aShell = aMSB->Outer();

  // Expand shell contents to CAF tree
  ExpandShell(aShell, ShapeLab, TP, ShapeTool);
}

//=======================================================================
//function : ExpandSBSM
//purpose  :
//=======================================================================

void STEPCAFControl_Reader::ExpandSBSM(TDF_Label& ShapeLab,
                                       const Handle(StepRepr_RepresentationItem)& Repr,
                                       const Handle(Transfer_TransientProcess)& TP,
                                       const Handle(XCAFDoc_ShapeTool)& ShapeTool) const
{
  Handle(StepShape_ShellBasedSurfaceModel) aSBSM = Handle(StepShape_ShellBasedSurfaceModel)::DownCast(Repr);

  // Access boundary shells
  Handle(StepShape_HArray1OfShell) aShells = aSBSM->SbsmBoundary();
  for ( Standard_Integer s = aShells->Lower(); s <= aShells->Upper(); ++s )
  {
    const StepShape_Shell& aShell = aShells->Value(s);
    Handle(StepShape_ConnectedFaceSet) aCFS;
    Handle(StepShape_OpenShell) anOpenShell = aShell.OpenShell();
    Handle(StepShape_ClosedShell) aClosedShell = aShell.ClosedShell();

    if ( !anOpenShell.IsNull() )
      aCFS = anOpenShell;
    else
      aCFS = aClosedShell;

    ExpandShell(aCFS, ShapeLab, TP, ShapeTool);
  }
}

//=======================================================================
//function : ExpandShell
//purpose  :
//=======================================================================

void STEPCAFControl_Reader::ExpandShell(const Handle(StepShape_ConnectedFaceSet)& Shell,
                                        TDF_Label& RootLab,
                                        const Handle(Transfer_TransientProcess)& TP,
                                        const Handle(XCAFDoc_ShapeTool)& ShapeTool) const
{
  // Record CAF data
  TDF_Label aShellLab = SettleShapeData(Shell, RootLab, ShapeTool, TP);

  // Access faces
  Handle(StepShape_HArray1OfFace) aFaces = Shell->CfsFaces();
  for ( Standard_Integer f = aFaces->Lower(); f <= aFaces->Upper(); ++f )
  {
    const Handle(StepShape_Face)& aFace = aFaces->Value(f);

    // Record CAF data
    TDF_Label aFaceLab = SettleShapeData(aFace, aShellLab, ShapeTool, TP);

    // Access face bounds
    Handle(StepShape_HArray1OfFaceBound) aWires = aFace->Bounds();
    for ( Standard_Integer w = aWires->Lower(); w <= aWires->Upper(); ++w )
    {
      const Handle(StepShape_Loop)& aWire = aWires->Value(w)->Bound();

      // Record CAF data
      TDF_Label aWireLab = SettleShapeData(aWire, aFaceLab, ShapeTool, TP);

      // Access wire edges
      // Currently only EDGE LOOPs are considered (!)
      if ( !aWire->IsInstance( STANDARD_TYPE(StepShape_EdgeLoop) ) )
        continue;

      // Access edges
      Handle(StepShape_EdgeLoop) anEdgeLoop = Handle(StepShape_EdgeLoop)::DownCast(aWire);
      Handle(StepShape_HArray1OfOrientedEdge) anEdges = anEdgeLoop->EdgeList();
      for ( Standard_Integer e = anEdges->Lower(); e <= anEdges->Upper(); ++e )
      {
        Handle(StepShape_Edge) anEdge = anEdges->Value(e)->EdgeElement();

        // Record CAF data
        TDF_Label anEdgeLab = SettleShapeData(anEdge, aWireLab, ShapeTool, TP);

        // Access vertices
        Handle(StepShape_Vertex) aV1 = anEdge->EdgeStart();
        Handle(StepShape_Vertex) aV2 = anEdge->EdgeEnd();

        // Record CAF data
        SettleShapeData(aV1, anEdgeLab, ShapeTool, TP);
        SettleShapeData(aV2, anEdgeLab, ShapeTool, TP);
      }
    }
  }
}

//=======================================================================
//function : SetColorMode
//purpose  : 
//=======================================================================

void STEPCAFControl_Reader::SetColorMode (const Standard_Boolean colormode)
{
  myColorMode = colormode;
}

//=======================================================================
//function : GetColorMode
//purpose  : 
//=======================================================================

Standard_Boolean STEPCAFControl_Reader::GetColorMode () const
{
  return myColorMode;
}

//=======================================================================
//function : SetNameMode
//purpose  : 
//=======================================================================

void STEPCAFControl_Reader::SetNameMode (const Standard_Boolean namemode)
{
  myNameMode = namemode;
}

//=======================================================================
//function : GetNameMode
//purpose  : 
//=======================================================================

Standard_Boolean STEPCAFControl_Reader::GetNameMode () const
{
  return myNameMode;
}

//=======================================================================
//function : SetLayerMode
//purpose  : 
//=======================================================================

void STEPCAFControl_Reader::SetLayerMode (const Standard_Boolean layermode)
{
  myLayerMode = layermode;
}

//=======================================================================
//function : GetLayerMode
//purpose  : 
//=======================================================================

Standard_Boolean STEPCAFControl_Reader::GetLayerMode () const
{
  return myLayerMode;
}

//=======================================================================
//function : SetPropsMode
//purpose  : 
//=======================================================================

void STEPCAFControl_Reader::SetPropsMode (const Standard_Boolean propsmode)
{
  myPropsMode = propsmode;
}

//=======================================================================
//function : GetPropsMode
//purpose  : 
//=======================================================================

Standard_Boolean STEPCAFControl_Reader::GetPropsMode () const
{
  return myPropsMode;
}

//=======================================================================
//function : SetSHUOMode
//purpose  : 
//=======================================================================

void STEPCAFControl_Reader::SetSHUOMode (const Standard_Boolean mode)
{
  mySHUOMode = mode;
}

//=======================================================================
//function : GetSHUOMode
//purpose  : 
//=======================================================================

Standard_Boolean STEPCAFControl_Reader::GetSHUOMode () const
{
  return mySHUOMode;
}

//=======================================================================
//function : SetGDTMode
//purpose  : 
//=======================================================================

void STEPCAFControl_Reader::SetGDTMode (const Standard_Boolean gdtmode)
{
  myGDTMode = gdtmode;
}

//=======================================================================
//function : GetGDTMode
//purpose  : 
//=======================================================================

Standard_Boolean STEPCAFControl_Reader::GetGDTMode () const
{
  return myGDTMode;
}


//=======================================================================
//function : SetMatMode
//purpose  : 
//=======================================================================

void STEPCAFControl_Reader::SetMatMode (const Standard_Boolean matmode)
{
  myMatMode = matmode;
}

//=======================================================================
//function : GetMatMode
//purpose  : 
//=======================================================================

Standard_Boolean STEPCAFControl_Reader::GetMatMode () const
{
  return myMatMode;
}

//=======================================================================
//function : SetViewMode
//purpose  : 
//=======================================================================

void STEPCAFControl_Reader::SetViewMode(const Standard_Boolean viewmode)
{
  myViewMode = viewmode;
}

//=======================================================================
//function : GetViewMode
//purpose  : 
//=======================================================================

Standard_Boolean STEPCAFControl_Reader::GetViewMode() const
{
  return myViewMode;
}
