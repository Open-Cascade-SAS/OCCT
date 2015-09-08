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
#include <Interface_EntityIterator.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Static.hxx>
#include <NCollection_DataMap.hxx>
#include <OSD_Path.hxx>
#include <Quantity_Color.hxx>
#include <StepAP214_AppliedExternalIdentificationAssignment.hxx>
#include <StepBasic_ConversionBasedUnitAndLengthUnit.hxx>
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
#include <StepBasic_Unit.hxx>
#include <STEPCAFControl_Controller.hxx>
#include <STEPCAFControl_DataMapIteratorOfDataMapOfShapePD.hxx>
#include <STEPCAFControl_DataMapOfPDExternFile.hxx>
#include <STEPCAFControl_DataMapOfSDRExternFile.hxx>
#include <STEPCAFControl_DataMapOfShapePD.hxx>
#include <STEPCAFControl_DictionaryOfExternFile.hxx>
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
#include <StepDimTol_AngularityTolerance.hxx>
#include <StepDimTol_CircularRunoutTolerance.hxx>
#include <StepDimTol_CoaxialityTolerance.hxx>
#include <StepDimTol_ConcentricityTolerance.hxx>
#include <StepDimTol_CylindricityTolerance.hxx>
#include <StepDimTol_Datum.hxx>
#include <StepDimTol_DatumFeature.hxx>
#include <StepDimTol_DatumReference.hxx>
#include <StepDimTol_FlatnessTolerance.hxx>
#include <StepDimTol_GeometricTolerance.hxx>
#include <StepDimTol_GeometricToleranceWithDatumReference.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol.hxx>
#include <StepDimTol_HArray1OfDatumReference.hxx>
#include <StepDimTol_LineProfileTolerance.hxx>
#include <StepDimTol_ModifiedGeometricTolerance.hxx>
#include <StepDimTol_ParallelismTolerance.hxx>
#include <StepDimTol_PerpendicularityTolerance.hxx>
#include <StepDimTol_PositionTolerance.hxx>
#include <StepDimTol_RoundnessTolerance.hxx>
#include <StepDimTol_StraightnessTolerance.hxx>
#include <StepDimTol_SurfaceProfileTolerance.hxx>
#include <StepDimTol_SymmetryTolerance.hxx>
#include <StepDimTol_TotalRunoutTolerance.hxx>
#include <StepRepr_AssemblyComponentUsage.hxx>
#include <StepRepr_CharacterizedDefinition.hxx>
#include <StepRepr_DescriptiveRepresentationItem.hxx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <StepRepr_MeasureRepresentationItem.hxx>
#include <StepRepr_NextAssemblyUsageOccurrence.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_PropertyDefinition.hxx>
#include <StepRepr_PropertyDefinitionRepresentation.hxx>
#include <StepRepr_Representation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_RepresentationRelationship.hxx>
#include <StepRepr_RepresentedDefinition.hxx>
#include <StepRepr_ReprItemAndLengthMeasureWithUnit.hxx>
#include <StepRepr_SequenceOfRepresentationItem.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepRepr_ShapeAspectRelationship.hxx>
#include <StepRepr_ShapeRepresentationRelationship.hxx>
#include <StepRepr_SpecifiedHigherUsageOccurrence.hxx>
#include <StepRepr_ValueRange.hxx>
#include <StepShape_AdvancedFace.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_ConnectedFaceSet.hxx>
#include <StepShape_ContextDependentShapeRepresentation.hxx>
#include <StepShape_DimensionalCharacteristicRepresentation.hxx>
#include <StepShape_DimensionalSize.hxx>
#include <StepShape_EdgeCurve.hxx>
#include <StepShape_EdgeLoop.hxx>
#include <StepShape_GeometricSet.hxx>
#include <StepShape_HArray1OfFace.hxx>
#include <StepShape_HArray1OfFaceBound.hxx>
#include <StepShape_HArray1OfOrientedEdge.hxx>
#include <StepShape_HArray1OfShell.hxx>
#include <StepShape_Loop.hxx>
#include <StepShape_ManifoldSolidBrep.hxx>
#include <StepShape_OpenShell.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <StepShape_ShapeDimensionRepresentation.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <StepShape_ShellBasedSurfaceModel.hxx>
#include <StepShape_SolidModel.hxx>
#include <StepShape_Vertex.hxx>
#include <StepVisual_Invisibility.hxx>
#include <StepVisual_LayeredItem.hxx>
#include <StepVisual_PresentationLayerAssignment.hxx>
#include <StepVisual_PresentationStyleByContext.hxx>
#include <StepVisual_StyleContextSelect.hxx>
#include <StepVisual_StyledItem.hxx>
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
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_DataMapOfShapeLabel.hxx>
#include <XCAFDoc_DimTolTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <XCAFDoc_MaterialTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_Volume.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>

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
//function : IsEqual
//purpose  : global function to check equality of topological shapes
//=======================================================================

inline Standard_Boolean IsEqual(const TopoDS_Shape& theShape1,
                                const TopoDS_Shape& theShape2) 
{
  return theShape1.IsEqual(theShape2);
}

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
       myMatMode  ( Standard_True )
{
  STEPCAFControl_Controller::Init();
  myFiles = new STEPCAFControl_DictionaryOfExternFile;
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
       myMatMode  ( Standard_True )
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
  myFiles = new STEPCAFControl_DictionaryOfExternFile;
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

const Handle(STEPCAFControl_DictionaryOfExternFile) &STEPCAFControl_Reader::ExternFiles () const
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
  if ( myFiles.IsNull() || ! myFiles->HasItem ( name ) ) 
    return Standard_False;
  ef = myFiles->Item ( name );
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
  Handle(Transfer_TransientProcess) TP = reader.WS()->TransferReader()->TransientProcess();
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

  // Expand resulting CAF structure for sub-shapes (optionally with their
  // names) if requested
  ExpandSubShapes(STool, map, ShapePDMap);

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
  if ( myFiles->HasItem ( file, Standard_True ) ) {
    return myFiles->Item ( file );
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
  myFiles->SetItem ( file, EF );
  
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
    TopoDS_Shape S = STEPConstruct::FindShape ( Styles.TransientProcess(), style->Item() );
    Standard_Boolean isSkipSHUOstyle = Standard_False;
    // take shape with real location.
    while ( IsComponent ) {
      // take SR of NAUO
      Handle(StepShape_ShapeRepresentation) aSR;
      findStyledSR( style, aSR );
      // search for SR along model
      if (aSR.IsNull())
        break;
//       Handle(Interface_InterfaceModel) Model = WS->Model();
      Handle(XSControl_TransferReader) TR = WS->TransferReader();
      Handle(Transfer_TransientProcess) TP = TR->TransientProcess();
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
//         Handle(Transfer_Binder) binder = TP->Find(NAUO);
//         if ( binder.IsNull() || ! binder->HasResult() )
//           continue;
//         aSh = TransferBRep::ShapeResult ( TP, binder );
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
  Handle(Interface_InterfaceModel) Model = WS->Model();
  Handle(XSControl_TransferReader) TR = WS->TransferReader();
  Handle(Transfer_TransientProcess) TP = TR->TransientProcess();
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
            PDR->Description()->Length() >0 ) name = PDR->Description();
        else if ( !PDR->Name().IsNull() && PDR->Name()->Length() >0 ) name = PDR->Name();
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
  Handle(Interface_InterfaceModel) Model = WS->Model();
  Handle(XSControl_TransferReader) TR = WS->TransferReader();
  Handle(Transfer_TransientProcess) TP = TR->TransientProcess();
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
  Handle(Interface_InterfaceModel) Model = WS->Model();
  Handle(XSControl_TransferReader) TR = WS->TransferReader();
  Handle(Transfer_TransientProcess) TP = TR->TransientProcess();
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
  
  Handle(StepRepr_NextAssemblyUsageOccurrence) NUNAUO =
    Handle(StepRepr_NextAssemblyUsageOccurrence)::DownCast(subSHUO->NextUsage());
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
  Handle(StepRepr_NextAssemblyUsageOccurrence) NUNAUO =
    Handle(StepRepr_NextAssemblyUsageOccurrence)::DownCast(SHUO->NextUsage());
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
static Standard_Boolean GetLengthConversionFactor(Handle(StepBasic_NamedUnit)& NU,
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
//function : ReadDatums
//purpose  : auxilary
//=======================================================================
static Standard_Boolean ReadDatums(const Handle(XCAFDoc_ShapeTool) &STool,
                                   const Handle(XCAFDoc_DimTolTool) &DGTTool,
                                   const Interface_Graph &graph,
                                   Handle(Transfer_TransientProcess) &TP,
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
//function : ReadGDTs
//purpose  : 
//=======================================================================

Standard_Boolean STEPCAFControl_Reader::ReadGDTs(const Handle(XSControl_WorkSession) &WS,
                                                 Handle(TDocStd_Document)& Doc) const
{
  Handle(Interface_InterfaceModel) Model = WS->Model();
  Handle(XCAFDoc_ShapeTool) STool = XCAFDoc_DocumentTool::ShapeTool( Doc->Main() );
  Handle(XSControl_TransferReader) TR = WS->TransferReader();
  Handle(Transfer_TransientProcess) TP = TR->TransientProcess();
  Handle(XCAFDoc_DimTolTool) DGTTool = XCAFDoc_DocumentTool::DimTolTool( Doc->Main() );
  if ( DGTTool.IsNull() ) return Standard_False;
  
  Standard_Integer nb = Model->NbEntities();
  const Interface_Graph& graph = TP->Graph();
  for(Standard_Integer i=1; i<=nb; i++) {
    Handle(Standard_Transient) ent = Model->Value(i);
    if(ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspect))) {
      Handle(StepRepr_ShapeAspect) SA = Handle(StepRepr_ShapeAspect)::DownCast(ent);
      // find RepresentationItem for current ShapeAspect
      Handle(StepRepr_RepresentationItem) RI;
      Handle(StepRepr_PropertyDefinition) PropD;
      Interface_EntityIterator subs3 = graph.Sharings(SA);
      for(subs3.Start(); subs3.More() && PropD.IsNull(); subs3.Next()) {
        PropD = Handle(StepRepr_PropertyDefinition)::DownCast(subs3.Value());
      }
      if(PropD.IsNull()) continue;
      Interface_EntityIterator subs4 = graph.Sharings(PropD);
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
      // read DGT entities:
      subs3 = graph.Sharings(SA);
      for(subs3.Start(); subs3.More(); subs3.Next()) {
        if(subs3.Value()->IsKind(STANDARD_TYPE(StepShape_DimensionalSize))) {
          // read dimensions
          Handle(StepShape_EdgeCurve) EC = Handle(StepShape_EdgeCurve)::DownCast(RI);
          if(EC.IsNull()) continue;
          Handle(TCollection_HAsciiString) aName;
          Handle(StepShape_DimensionalSize) DimSize = 
            Handle(StepShape_DimensionalSize)::DownCast(subs3.Value());
          Standard_Real dim1=-1.,dim2=-1.;
          subs4 = graph.Sharings(DimSize);
          for(subs4.Start(); subs4.More(); subs4.Next()) {
            Handle(StepShape_DimensionalCharacteristicRepresentation) DimCharR = 
              Handle(StepShape_DimensionalCharacteristicRepresentation)::DownCast(subs4.Value());
            if(!DimCharR.IsNull()) {
              Handle(StepShape_ShapeDimensionRepresentation) SDimR = DimCharR->Representation();
              if(!SDimR.IsNull() && SDimR->NbItems()>0) {
                Handle(StepRepr_RepresentationItem) aRI = SDimR->ItemsValue(1);
                Handle(StepRepr_ValueRange) VR = Handle(StepRepr_ValueRange)::DownCast(aRI);
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
                    Handle(StepRepr_RepresentationItem) RI1 =
                      Handle(StepRepr_RepresentationItem)::DownCast(HARI->Value(1));
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
                    Handle(StepRepr_RepresentationItem) RI2 =
                      Handle(StepRepr_RepresentationItem)::DownCast(HARI->Value(2));
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
          Standard_Integer index = TP->MapIndex(EC);
          TopoDS_Shape aSh;
          if(index >0) {
            Handle(Transfer_Binder) binder = TP->MapItem(index);
            aSh = TransferBRep::ShapeResult(binder);
          }
          if(aSh.IsNull()) continue; 
          TDF_Label shL;
          if( !STool->Search(aSh, shL, Standard_True, Standard_True, Standard_True) ) continue;
          Handle(TColStd_HArray1OfReal) arr = new TColStd_HArray1OfReal(1,2);
          arr->SetValue(1,dim1);
          arr->SetValue(2,dim2);
          DGTTool->SetDimTol(shL,1,arr,aName,DimSize->Name());
        }
        // read tolerances and datums
        else if(subs3.Value()->IsKind(STANDARD_TYPE(StepDimTol_GeometricTolerance))) {
          Handle(StepDimTol_GeometricTolerance) GT =
            Handle(StepDimTol_GeometricTolerance)::DownCast(subs3.Value());
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
          Standard_Integer index = TP->MapIndex(AF);
          TopoDS_Shape aSh;
          if(index >0) {
            Handle(Transfer_Binder) binder = TP->MapItem(index);
            aSh = TransferBRep::ShapeResult(binder);
          }
          if(aSh.IsNull()) continue; 
          TDF_Label shL;
          if( !STool->Search(aSh, shL, Standard_True, Standard_True, Standard_True) ) continue;
          // read specific data for tolerance
          if(GT->IsKind(STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol))) {
            Handle(StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol) GTComplex =
              Handle(StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol)::DownCast(subs3.Value());
            Standard_Integer kind=20;
            Handle(StepDimTol_ModifiedGeometricTolerance) MGT = 
              GTComplex->GetModifiedGeometricTolerance();
            if(!MGT.IsNull()) {
              kind = kind + MGT->Modifier()+1;
            }
            TDF_Label TolerL = DGTTool->SetDimTol(shL,kind,arr,aName,aDescription);
            // translate datums connected with this tolerance
            Handle(StepDimTol_GeometricToleranceWithDatumReference) GTWDR =
              GTComplex->GetGeometricToleranceWithDatumReference();
            if(!GTWDR.IsNull()) {
              ReadDatums(STool,DGTTool,graph,TP,TolerL,GTWDR);
            }
          }
          else if(GT->IsKind(STANDARD_TYPE(StepDimTol_GeometricToleranceWithDatumReference))) {
            Handle(StepDimTol_GeometricToleranceWithDatumReference) GTWDR =
              Handle(StepDimTol_GeometricToleranceWithDatumReference)::DownCast(subs3.Value());
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
            TDF_Label TolerL = DGTTool->SetDimTol(shL,kind,arr,aName,aDescription);
            ReadDatums(STool,DGTTool,graph,TP,TolerL,GTWDR);
          }
          else if(GT->IsKind(STANDARD_TYPE(StepDimTol_ModifiedGeometricTolerance))) {
            Handle(StepDimTol_ModifiedGeometricTolerance) MGT =
              Handle(StepDimTol_ModifiedGeometricTolerance)::DownCast(subs3.Value());
            Standard_Integer kind = 35 + MGT->Modifier();
            DGTTool->SetDimTol(shL,kind,arr,aName,aDescription);
          }
          else if(GT->IsKind(STANDARD_TYPE(StepDimTol_CylindricityTolerance))) {
            DGTTool->SetDimTol(shL,38,arr,aName,aDescription);
          }
          else if(GT->IsKind(STANDARD_TYPE(StepDimTol_FlatnessTolerance))) {
            DGTTool->SetDimTol(shL,39,arr,aName,aDescription);
          }
          else if(GT->IsKind(STANDARD_TYPE(StepDimTol_LineProfileTolerance))) {
            DGTTool->SetDimTol(shL,40,arr,aName,aDescription);
          }
          else if(GT->IsKind(STANDARD_TYPE(StepDimTol_PositionTolerance))) {
            DGTTool->SetDimTol(shL,41,arr,aName,aDescription);
          }
          else if(GT->IsKind(STANDARD_TYPE(StepDimTol_RoundnessTolerance))) {
            DGTTool->SetDimTol(shL,42,arr,aName,aDescription);
          }
          else if(GT->IsKind(STANDARD_TYPE(StepDimTol_StraightnessTolerance))) {
            DGTTool->SetDimTol(shL,43,arr,aName,aDescription);
          }
          else if(GT->IsKind(STANDARD_TYPE(StepDimTol_SurfaceProfileTolerance))) {
            DGTTool->SetDimTol(shL,44,arr,aName,aDescription);
          }
        }
      }
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
  Handle(Interface_InterfaceModel) Model = WS->Model();
  Handle(XCAFDoc_ShapeTool) STool = XCAFDoc_DocumentTool::ShapeTool( Doc->Main() );
  Handle(XSControl_TransferReader) TR = WS->TransferReader();
  Handle(Transfer_TransientProcess) TP = TR->TransientProcess();
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
