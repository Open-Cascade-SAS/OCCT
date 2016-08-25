// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

//:k8 abv 6 Jan 99: unique names for PRODUCTs
//:k9 abv 6 Jan 99: TR10: eliminating duplicated APPLICATION_CONTEXT entities
//abv,gka 05.04.99: S4136: change parameters and update units
// PTV    22.08.2002 OCC609 transfer solo vertices into step file.
// PTV    16.09.2002 OCC725 transfer compound of vertices into one geometrical curve set.

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools_Modifier.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <GeomToStep_MakeAxis2Placement3d.hxx>
#include <gp_Ax2.hxx>
#include <Interface_Macros.hxx>
#include <Interface_MSG.hxx>
#include <Interface_Static.hxx>
#include <MoniTool_DataMapOfShapeTransient.hxx>
#include <OSD_Timer.hxx>
#include <ShapeAnalysis_ShapeTolerance.hxx>
#include <ShapeExtend_Explorer.hxx>
#include <ShapeProcess_ShapeContext.hxx>
#include <Standard_Type.hxx>
#include <StepBasic_ApplicationProtocolDefinition.hxx>
#include <StepBasic_HArray1OfProduct.hxx>
#include <STEPConstruct_AP203Context.hxx>
#include <STEPConstruct_Assembly.hxx>
#include <STEPConstruct_ContextTool.hxx>
#include <STEPConstruct_Part.hxx>
#include <STEPConstruct_UnitContext.hxx>
#include <STEPControl_ActorWrite.hxx>
#include <STEPControl_StepModelType.hxx>
#include <StepData_StepModel.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx.hxx>
#include <StepGeom_Point.hxx>
#include <StepRepr_GlobalUnitAssignedContext.hxx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <StepRepr_PropertyDefinition.hxx>
#include <StepRepr_ShapeRepresentationRelationship.hxx>
#include <StepShape_AdvancedBrepShapeRepresentation.hxx>
#include <StepShape_BrepWithVoids.hxx>
#include <StepShape_FacetedBrep.hxx>
#include <StepShape_FacetedBrepAndBrepWithVoids.hxx>
#include <StepShape_FacetedBrepShapeRepresentation.hxx>
#include <StepShape_GeometricallyBoundedWireframeShapeRepresentation.hxx>
#include <StepShape_GeometricCurveSet.hxx>
#include <StepShape_GeometricSetSelect.hxx>
#include <StepShape_HArray1OfGeometricSetSelect.hxx>
#include <StepShape_ManifoldSolidBrep.hxx>
#include <StepShape_ManifoldSurfaceShapeRepresentation.hxx>
#include <StepShape_NonManifoldSurfaceShapeRepresentation.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <StepShape_ShellBasedSurfaceModel.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <StepShape_VertexPoint.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDSToStep.hxx>
#include <TopoDSToStep_Builder.hxx>
#include <TopoDSToStep_FacetedTool.hxx>
#include <TopoDSToStep_MakeBrepWithVoids.hxx>
#include <TopoDSToStep_MakeFacetedBrep.hxx>
#include <TopoDSToStep_MakeFacetedBrepAndBrepWithVoids.hxx>
#include <TopoDSToStep_MakeGeometricCurveSet.hxx>
#include <TopoDSToStep_MakeManifoldSolidBrep.hxx>
#include <TopoDSToStep_MakeShellBasedSurfaceModel.hxx>
#include <TopoDSToStep_MakeStepVertex.hxx>
#include <TopoDSToStep_Tool.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_Finder.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_SequenceOfBinder.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>
#include <TransferBRep_ShapeMapper.hxx>
#include <UnitsMethods.hxx>
#include <XSAlgo.hxx>
#include <XSAlgo_AlgoContainer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(STEPControl_ActorWrite,Transfer_ActorOfFinderProcess)

//  Transfer
//  TransferShape
//:d6
// Non-manifold topology processing (ssv; 10.11.2010)
// ============================================================================
// Function: DumpWhatIs   
// Purpose: Use it in debug mode to dump your shapes
// ============================================================================
#ifdef OCCT_DEBUG
static void DumpWhatIs(const TopoDS_Shape& S) {

  TopTools_MapOfShape aMapOfShape;
  aMapOfShape.Add(S);
  TopTools_ListOfShape aListOfShape;
  aListOfShape.Append(S);
  TopTools_ListIteratorOfListOfShape itL(aListOfShape);
  Standard_Integer nbSolids = 0,
                   nbShells = 0,
                   nbOpenShells = 0,
                   nbFaces = 0, 
                   nbWires = 0, 
                   nbEdges = 0, 
                   nbVertexes = 0;

  for( ; itL.More(); itL.Next() ) {
    TopoDS_Iterator it( itL.Value() );
    for ( ; it.More(); it.Next() ) {
      TopoDS_Shape aSubShape = it.Value();
      if ( !aMapOfShape.Add(aSubShape) )
        continue;
      aListOfShape.Append(aSubShape);
      if (aSubShape.ShapeType() == TopAbs_SOLID)
        nbSolids++;
      if (aSubShape.ShapeType() == TopAbs_SHELL) {
        if ( !aSubShape.Closed() )
          nbOpenShells++;
        nbShells++;
      }
      if (aSubShape.ShapeType() == TopAbs_FACE)
        nbFaces++;
      if (aSubShape.ShapeType() == TopAbs_WIRE)
        nbWires++;
      if (aSubShape.ShapeType() == TopAbs_EDGE)
        nbEdges++;
      if (aSubShape.ShapeType() == TopAbs_VERTEX)
        nbVertexes++;
    }
  }

  cout << "//What is?// NB SOLIDS: " << nbSolids << endl;
  cout << "//What is?// NB SHELLS: " << nbShells << endl;
  cout << "//What is?//    OPEN SHELLS: " << nbOpenShells << endl;
  cout << "//What is?//    CLOSED SHELLS: " << nbShells - nbOpenShells << endl;
  cout << "//What is?// NB FACES: " << nbFaces << endl;
  cout << "//What is?// NB WIRES: " << nbWires << endl;
  cout << "//What is?// NB EDGES: " << nbEdges << endl;
  cout << "//What is?// NB VERTEXES: " << nbVertexes << endl;
}
#endif

//=======================================================================
// Function : IsManifoldShape
// Purpose  : Used to define whether the passed shape has manifold
//            topology or not
//=======================================================================

static Standard_Boolean IsManifoldShape(const TopoDS_Shape& theShape) {

   Standard_Boolean aResult = Standard_True;

  // Do not check nested Compounds
  TopoDS_Compound aDirectShapes;
  BRep_Builder aBrepBuilder;
  aBrepBuilder.MakeCompound(aDirectShapes);

  TopoDS_Iterator anIt(theShape);
  for ( ; anIt.More(); anIt.Next() ) {
    TopoDS_Shape aDirectChild = anIt.Value();
    if (aDirectChild.ShapeType() != TopAbs_COMPOUND)
      aBrepBuilder.Add(aDirectShapes, aDirectChild);
  }  

  #ifdef OCCT_DEBUG
  DumpWhatIs(aDirectShapes);
  #endif

  TopTools_IndexedDataMapOfShapeListOfShape aMapEdgeFaces;
  TopExp::MapShapesAndAncestors(aDirectShapes, TopAbs_EDGE, TopAbs_FACE, aMapEdgeFaces);

  Standard_Integer aNbEdges = aMapEdgeFaces.Extent();
  #ifdef OCCT_DEBUG
  cout << "Checking whether the topology passed is manifold..." << endl;
  #endif

  // Check topology
  for (Standard_Integer i = 1; i <= aNbEdges; i++) {
    TopoDS_Edge aCurrentEdge = TopoDS::Edge( aMapEdgeFaces.FindKey(i) );
    if ( !BRep_Tool::Degenerated(aCurrentEdge) ) { 
      Standard_Integer aNbAncestors = aMapEdgeFaces.FindFromIndex(i).Extent();
      if (aNbAncestors > 2) {
        aResult = Standard_False;
        break;
      }
    }
  }

  #ifdef OCCT_DEBUG
  cout << "Check result: "
       << (aResult ? "TRUE" : "FALSE") << endl;
  #endif

  return aResult;
}
  
//=======================================================================
//function : STEPControl_ActorWrite
//purpose  : 
//=======================================================================

STEPControl_ActorWrite::STEPControl_ActorWrite ()
: mygroup (0) , mytoler (-1.)
{  
  SetMode(STEPControl_ShellBasedSurfaceModel);  
}

//=======================================================================
//method: getNMSSRForGroup
//purpose: allows to get NMSSR (NON_MANIFOLD_SURFACE_SHAPE_REPRESENTATION)
//         STEP's entity for the group of shells (!) passed
//=======================================================================

Handle(StepShape_NonManifoldSurfaceShapeRepresentation) STEPControl_ActorWrite::getNMSSRForGroup(const Handle(TopTools_HSequenceOfShape)& shapeGroup,
                                                                                                 const Handle(Transfer_FinderProcess)& FP, 
                                                                                                 Standard_Boolean& isNMSSRCreated) const
{
  Handle(StepShape_NonManifoldSurfaceShapeRepresentation) aResult;

  if ( !shapeGroup.IsNull() ) {
    for (Standard_Integer i = 1; i <= shapeGroup->Length(); i++) {
      TopoDS_Shape aCurrentShape = shapeGroup->Value(i);
      Handle(TransferBRep_ShapeMapper) mapper = TransferBRep::ShapeMapper(FP, aCurrentShape);
      if ( FP->FindTypedTransient(mapper, STANDARD_TYPE(StepShape_NonManifoldSurfaceShapeRepresentation), aResult) )
        break;
    }
  }

  if ( aResult.IsNull() ) {
    #ifdef OCCT_DEBUG
    cout << "\nNew NMSSR created" << endl;
    #endif
    aResult = new StepShape_NonManifoldSurfaceShapeRepresentation;
    isNMSSRCreated = Standard_True;
  } else {
    #ifdef OCCT_DEBUG
    cout << "\nExisting NMSSR is used" << endl;
    #endif
    isNMSSRCreated = Standard_False;
  }

  return aResult;
}

//=======================================================================
//function : mergeInfoForNM
//purpose  : bind already written shared faces to STEP entity for non-manifold
//=======================================================================
void STEPControl_ActorWrite::mergeInfoForNM(const Handle(Transfer_FinderProcess)& theFP,
                                            const Handle(Standard_Transient) &theInfo) const
{
  Handle(ShapeProcess_ShapeContext) aContext = Handle(ShapeProcess_ShapeContext)::DownCast ( theInfo );
  if ( aContext.IsNull() ) return;

  const TopTools_DataMapOfShapeShape &aMap = aContext->Map();
  TopTools_DataMapIteratorOfDataMapOfShapeShape aShapeShapeIt(aMap);

  for ( ; aShapeShapeIt.More(); aShapeShapeIt.Next() ) {
    TopoDS_Shape anOrig = aShapeShapeIt.Key(), aRes = aShapeShapeIt.Value();
    if (anOrig.ShapeType() != TopAbs_FACE)
      continue;

    Handle(TransferBRep_ShapeMapper) anOrigMapper= TransferBRep::ShapeMapper ( theFP, anOrig);
    Handle(Transfer_Binder) anOrigBinder = theFP->Find ( anOrigMapper );
    if (anOrigBinder.IsNull())
      continue;

    Handle(TransferBRep_ShapeMapper) aResMapper = TransferBRep::ShapeMapper ( theFP, aRes );
    theFP->Bind(aResMapper, anOrigBinder);
  }
}


//=======================================================================
//function : SetMode
//purpose  : 
//=======================================================================

void STEPControl_ActorWrite::SetMode (const STEPControl_StepModelType M)
{
  switch (M) {
  case STEPControl_AsIs : ModeTrans() = 0; break;
  case STEPControl_ManifoldSolidBrep : ModeTrans() = 3; break;
  case STEPControl_BrepWithVoids :     ModeTrans() = 5; break;
  case STEPControl_FacetedBrep :       ModeTrans() = 1; break;
  case STEPControl_FacetedBrepAndBrepWithVoids : ModeTrans() = 6; break;
  case STEPControl_ShellBasedSurfaceModel :      ModeTrans() = 2;
  case STEPControl_GeometricCurveSet :           ModeTrans() = 4;
  case STEPControl_Hybrid : ModeTrans() = 0; break;  // PAS IMPLEMENTE !!
    default: break;
  }
}

//=======================================================================
//function : Mode
//purpose  : 
//=======================================================================

STEPControl_StepModelType STEPControl_ActorWrite::Mode () const
{
  switch (themodetrans) {
  case 0 : return STEPControl_AsIs;
  case 1 : return STEPControl_FacetedBrep;
  case 2 : return STEPControl_ShellBasedSurfaceModel;
  case 3 : return STEPControl_ManifoldSolidBrep;
  case 4 : return STEPControl_GeometricCurveSet;
  case 5 : return STEPControl_BrepWithVoids;
  case 6 : return STEPControl_FacetedBrepAndBrepWithVoids;
    default : break;
  }
  return STEPControl_AsIs;
}

//=======================================================================
//function : SetGroupMode
//purpose  : 
//=======================================================================

void STEPControl_ActorWrite::SetGroupMode (const Standard_Integer mode)
{  
  if (mode >= 0) mygroup = mode;  
}

//=======================================================================
//function : GroupMode
//purpose  : 
//=======================================================================

Standard_Integer STEPControl_ActorWrite::GroupMode () const
{  
  return mygroup;  
}

//=======================================================================
//function : SetTolerance
//purpose  : 
//=======================================================================

void STEPControl_ActorWrite::SetTolerance (const Standard_Real Tol)
{  
  mytoler = Tol;  
}

//=======================================================================
//function : Recognize
//  ATTENTION, Recognize doit s aligner sur ce que Transfer sait faire
//purpose  : 
//=======================================================================

Standard_Boolean  STEPControl_ActorWrite::Recognize (const Handle(Transfer_Finder)& start)
{
  STEPControl_StepModelType mymode = Mode();
  Handle(TransferBRep_ShapeMapper) mapper = Handle(TransferBRep_ShapeMapper)::DownCast(start);
  if (mapper.IsNull()) return Standard_False;
  if (mymode == STEPControl_AsIs) return Standard_True;

  Standard_Boolean yasolid = Standard_False, yashell = Standard_False,
                   yaface  = Standard_False;

  TopoDS_Shape theShape, aShape;
//  theShape = TopoDSToStep::DirectFaces(mapper->Value());
  theShape = mapper->Value();  // pour une reconnaissance c est bien assez

  if (theShape.ShapeType() == TopAbs_COMPOUND) {
    
    TopExp_Explorer SolidExp, ShellExp, FaceExp;
    
    for (SolidExp.Init(theShape, TopAbs_SOLID);
	 SolidExp.More();SolidExp.Next()) yasolid = Standard_True;
    for (ShellExp.Init(theShape, TopAbs_SHELL, TopAbs_SOLID);
	 ShellExp.More();ShellExp.Next()) yashell = Standard_True;
    for (FaceExp.Init(theShape, TopAbs_FACE, TopAbs_SHELL);
	 FaceExp.More();FaceExp.Next())   yaface  = Standard_True;
  }
  else if (theShape.ShapeType() == TopAbs_SOLID) yasolid = Standard_True;
  else if (theShape.ShapeType() == TopAbs_SHELL) yashell = Standard_True;
  else if (theShape.ShapeType() == TopAbs_FACE)  yaface  = Standard_True;
  else if (mymode != STEPControl_GeometricCurveSet) return Standard_False;
//  pour wireframe ?

//  Faceted : il est OBLIGATOIRE d avoir des surfaces support Plane et des
//   courbes 3D Line (pcurves ignorees)

  if (mymode == STEPControl_FacetedBrep || mymode == STEPControl_FacetedBrepAndBrepWithVoids) {
    for (TopExp_Explorer ffac(theShape,TopAbs_FACE); ffac.More(); ffac.Next()) {
      const TopoDS_Face& F = TopoDS::Face (ffac.Current());
      TopLoc_Location locbid;
      Handle(Geom_Surface) surf = BRep_Tool::Surface (F,locbid);
      if (surf.IsNull() || !surf->IsKind(STANDARD_TYPE(Geom_Plane)) ) return Standard_False;
    }
    for (TopExp_Explorer fedg(theShape,TopAbs_EDGE); fedg.More(); fedg.Next()) {
      const TopoDS_Edge& E = TopoDS::Edge (fedg.Current());
      TopLoc_Location locbid;  Standard_Real first,last;
      Handle(Geom_Curve) curv = BRep_Tool::Curve (E,locbid,first,last);
      if (curv.IsNull() || !curv->IsKind(STANDARD_TYPE(Geom_Line)) ) return Standard_False;
    }
  }

  switch (mymode) {
    case STEPControl_ManifoldSolidBrep: return (yasolid || yashell);
    case STEPControl_BrepWithVoids:
    case STEPControl_FacetedBrep:
    case STEPControl_FacetedBrepAndBrepWithVoids:       return yasolid;
    case STEPControl_ShellBasedSurfaceModel:
					return (yasolid || yashell || yaface);
    case STEPControl_GeometricCurveSet:  return Standard_True;  // tout OK
    default : break;
  }
  return Standard_False;
}


//  ########    MAKE PRODUCT DATA + CONTEXT    ########

//=======================================================================
//function : Transfer
//purpose  : 
//=======================================================================

Handle(Transfer_Binder) STEPControl_ActorWrite::Transfer (const Handle(Transfer_Finder)& start,
                                                          const Handle(Transfer_FinderProcess)& FP)
{
  XSAlgo::AlgoContainer()->PrepareForTransfer();
    
  Handle(TransferBRep_ShapeMapper) mapper = Handle(TransferBRep_ShapeMapper)::DownCast(start);

  if (mapper.IsNull()) return NullResult();
  TopoDS_Shape shape = mapper->Value();

  // init context
  Handle(StepData_StepModel) model = Handle(StepData_StepModel)::DownCast ( FP->Model() );
  if ( ! model.IsNull() ) myContext.SetModel ( model ); //: abv 04.11.00: take APD from model
  myContext.AddAPD ( Standard_False ); // update APD
  myContext.SetLevel ( 1 ); // set assembly level to 1 (to ensure)
  
  //:S4136: init UnitsMethods to reset angle unit factors (see TopoDSToStep)
  Standard_Real lFactor = UnitsMethods::GetLengthFactorValue ( Interface_Static::IVal ( "write.step.unit" ) );
  lFactor /= UnitsMethods::GetCasCadeLengthUnit();
  Standard_Integer anglemode = Interface_Static::IVal("step.angleunit.mode");
  UnitsMethods::InitializeFactors ( lFactor, ( anglemode <= 1 ? 1. : M_PI/180. ), 1. );

  // create SDR
  STEPConstruct_Part SDRTool;
  SDRTool.MakeSDR ( 0, myContext.GetProductName(), myContext.GetAPD()->Application() );
  Handle(StepShape_ShapeDefinitionRepresentation) sdr = SDRTool.SDRValue();
  // transfer shape

  Handle(Transfer_Binder) resbind = TransferShape (mapper,sdr,FP);

//  Handle(StepShape_ShapeRepresentation) resultat;
//  FP->GetTypedTransient (resbind,STANDARD_TYPE(StepShape_ShapeRepresentation),resultat);
//  sdr->SetUsedRepresentation (resultat);

  // create binder with all root entities produced from shape
  Handle(TColStd_HSequenceOfTransient) roots = myContext.GetRootsForPart ( SDRTool );
  Handle(Transfer_Binder) resprod = TransientResult ( myContext.GetAPD() );
  for ( Standard_Integer i=1; i <= roots->Length(); i++ ) 
    resprod->AddResult ( TransientResult ( roots->Value(i) ) );
  resprod->AddResult(resbind);

  // bind and exit
  //FP->Bind (mapper,resprod);
  myContext.NextIndex();
  return resprod;
}

//==========================================

static Standard_Real UsedTolerance (const Standard_Real mytoler, 
				    const TopoDS_Shape& theShape)
{

  //    COMPUTING 3D TOLERANCE
  //    Either from Session, or Computed (Least,Average, or Greatest)
  //    Then given to TopoDSToStep_Tool

  Standard_Real Tol = mytoler;
  Standard_Integer tolmod = Interface_Static::IVal("write.precision.mode");
  if (Tol <= 0 && tolmod == 2) Tol =
    Interface_Static::RVal("write.precision.val");
  if (Tol <= 0) {
    ShapeAnalysis_ShapeTolerance stu;
    Tol = stu.Tolerance (theShape,tolmod);
    //  Par defaut, on prend une tolerance moyenne, on elimine les aberrations
    Tol = Interface_MSG::Intervalled (Tol * 1.5);  // arrondi a 1 2 5 ...
  }
  if (Tol == 0) Tol = 1.e-07;  // minimum ...

  return Tol;
}

//=======================================================================
//function : IsAssembly
//purpose  : 
//=======================================================================
// if GroupMode is >1 downgrades all compounds having single subshape to that 
// subshape

Standard_Boolean STEPControl_ActorWrite::IsAssembly (TopoDS_Shape &S) const
{
  if ( ! GroupMode() || S.ShapeType() != TopAbs_COMPOUND ) return Standard_False;
  // PTV 16.09.2002  OCC725 for storing compound of vertices
  if (Interface_Static::IVal("write.step.vertex.mode") == 0) {//bug 23950
    if (S.ShapeType() == TopAbs_COMPOUND ) {
      Standard_Boolean IsOnlyVertices = Standard_True;
      TopoDS_Iterator anItr( S );
      for ( ; anItr.More(); anItr.Next() ) {
        if ( anItr.Value().ShapeType() != TopAbs_VERTEX ) {
          IsOnlyVertices = Standard_False;
          break;
        }
      }
      if ( IsOnlyVertices )
        return Standard_False;
    }
  }
  if ( GroupMode() ==1 ) return Standard_True;
  TopoDS_Iterator it ( S );
  if ( ! it.More() ) return Standard_False;
  TopoDS_Shape shape = it.Value();
  it.Next();
  if ( it.More() ) return Standard_True;
  S = shape;
  return IsAssembly ( S );
}

//=======================================================================
//function : TransferShape
//purpose  : 
//=======================================================================

/*
static void UpdateMap (const TopoDS_Shape &shape, 
		       BRepTools_Modifier &M1, 
		       BRepTools_Modifier &M2, 
		       const Handle(Transfer_FinderProcess) &FinderProcess)
{
  TopoDS_Shape S = M1.ModifiedShape ( shape );
  S = M2.ModifiedShape ( S );
  if ( S == shape ) return;

  Handle(TransferBRep_ShapeMapper) mapper = TransferBRep::ShapeMapper ( FinderProcess, S );
  Handle(Transfer_Binder) binder = FinderProcess->Find ( mapper );
  if ( ! binder.IsNull() ) {
    mapper = TransferBRep::ShapeMapper ( FinderProcess, shape );
    FinderProcess->Bind ( mapper, binder );
  }
  
  for ( TopoDS_Iterator it(shape); it.More(); it.Next() ) 
    UpdateMap ( it.Value(), M1, M2, FinderProcess );
}
*/

// PTV 16.09.2002 added for transfering vertices.
static Standard_Boolean transferVertex (const Handle(Transfer_FinderProcess)& FP,
                                        Handle(StepShape_HArray1OfGeometricSetSelect)& aGSS,
                                        const TopoDS_Shape& aShVrtx,
                                        const Standard_Integer theNum)
{
  Standard_Boolean IsDone = Standard_False;
  MoniTool_DataMapOfShapeTransient aMap;
  TopoDSToStep_Tool    aTool(aMap, Standard_True);
  TopoDSToStep_MakeStepVertex aMkVrtx ( TopoDS::Vertex(aShVrtx), aTool, FP );
  
  if (!aMkVrtx.IsDone())
    return IsDone;
  
  Handle(StepShape_VertexPoint) aVP = 
    Handle(StepShape_VertexPoint)::DownCast(aTool.Find(aShVrtx));
  if (aVP.IsNull())
    return IsDone;
    
  StepShape_GeometricSetSelect select;
  select.SetValue(aVP->VertexGeometry());
  // add current result
  aGSS->SetValue( theNum, select );
  IsDone = Standard_True;
  return IsDone;
}

     
Handle(Transfer_Binder) STEPControl_ActorWrite::TransferShape (const Handle(Transfer_Finder)& start,
                                                               const Handle(StepShape_ShapeDefinitionRepresentation)& SDR0,
                                                               const Handle(Transfer_FinderProcess)& FP,
                                                               const Handle(TopTools_HSequenceOfShape)& shapeGroup,
                                                               const Standard_Boolean isManifold)
{
  STEPControl_StepModelType mymode = Mode();
  Handle(TransferBRep_ShapeMapper) mapper = Handle(TransferBRep_ShapeMapper)::DownCast(start);
  Handle(Transfer_Binder) binder;

  // Indicates whether to use an exising NMSSR to write items to (ss; 13.11.2010)
  Standard_Boolean useExistingNMSSR = Standard_False;

  if (mapper.IsNull()) return binder;
  TopoDS_Shape theShape = mapper->Value();

  if (theShape.IsNull()) return binder;

  // INDIVIDUAL SHAPE ALREADY TRANSFERRED : RETURN IT !
  binder = FP->Find(start);
  if (!binder.IsNull()) {  if (!binder->HasResult()) binder.Nullify();  }
  if (!binder.IsNull()) { 
    //:abv 20.05.02: writing box & face from it (shared) in one compound 
    // as assembly - while face already translated, it should be 
    // re-translated to break sharing
#ifdef OCCT_DEBUG
    cout << "Warning: STEPControl_ActorWrite::TransferShape(): shape already translated" << endl;
#endif
//    return binder;
  }

  // MODE ASSEMBLY : if Compound, (sub-)assembly
  if ( IsAssembly(theShape) )
    return TransferCompound(start, SDR0, FP);

  // [BEGIN] Separate manifold topology from non-manifold in group mode 0 (ssv; 18.11.2010)
  Standard_Boolean isNMMode = Interface_Static::IVal("write.step.nonmanifold") != 0;
  Handle(Transfer_Binder) aNMBinder;
  if (isNMMode && !GroupMode() && theShape.ShapeType() == TopAbs_COMPOUND) {
    TopoDS_Compound aNMCompound;
    TopoDS_Compound aManifoldCompound;
    BRep_Builder brepBuilder;

    // Create empty Compounds
    brepBuilder.MakeCompound(aManifoldCompound);
    brepBuilder.MakeCompound(aNMCompound);

    // Indicates whether there is only non-manifold topology detected 
    // (there is no manifold topology found in the Compound passed)
    Standard_Boolean isOnlyNonManifold = Standard_False;
    
    // Find a Compound containing non-manifold topology.
    // NOTE: only one such Compound must exist in the entire Compound passed
    if ( !IsManifoldShape(theShape) ) {
      aNMCompound = TopoDS::Compound(theShape);
      isOnlyNonManifold = Standard_True;
    }
    else {
      TopTools_ListOfShape aListOfShapes;
      TopTools_ListOfShape aListOfManifoldShapes;
      aListOfShapes.Append(theShape);

      TopTools_ListIteratorOfListOfShape itL(aListOfShapes);
      for ( ; itL.More(); itL.Next() ) {
        TopoDS_Shape aParentShape = itL.Value();
        TopoDS_Iterator it(aParentShape);
        for ( ; it.More(); it.Next() ) {
          TopoDS_Shape aSubShape = it.Value();
          if (aSubShape.ShapeType() == TopAbs_COMPOUND && !IsManifoldShape(aSubShape) )
            aNMCompound = TopoDS::Compound(aSubShape);
          else if (aSubShape.ShapeType() == TopAbs_COMPOUND)
            aListOfShapes.Append(aSubShape);
          else 
            aListOfManifoldShapes.Append(aSubShape);
        }
      }
            
      // Group manifold topology together.
      // NOTE: there is no sense that initial Compound structure was lost as
      //       group mode is set to 0 (no Assemblies are mapped)
      for ( itL.Initialize(aListOfManifoldShapes); itL.More(); itL.Next() ) {
        TopoDS_Shape aCurrentManiShape = itL.Value();
        brepBuilder.Add(aManifoldCompound, aCurrentManiShape);
      }

    }

    // Process only manifold topology in the current TransferShape invocation.
    // Invoke TransferShape for non-manifold topology separately (see below)
    theShape = aManifoldCompound;
    
    // List of items to transfer
    Handle(TopTools_HSequenceOfShape) RepItemSeq = new TopTools_HSequenceOfShape();
    // Non-manifold group to pass into TransferShape with each shape from RepItemSeq
    Handle(TopTools_HSequenceOfShape) NonManifoldGroup = new TopTools_HSequenceOfShape();

    // Transfer Solids to closed Shells. Prepare RepItemSeq & NonManifoldGroup
    for ( TopoDS_Iterator iter(aNMCompound); iter.More(); iter.Next() ) {
      TopoDS_Shape aSubShape = iter.Value();
      if (aSubShape.ShapeType() == TopAbs_SOLID) {
        for ( TopoDS_Iterator aSubIter(aSubShape); aSubIter.More(); aSubIter.Next() ) {
          TopoDS_Shell aSubShell = TopoDS::Shell( aSubIter.Value() );
          aSubShell.Closed(Standard_True);
          RepItemSeq->Append(aSubShell);
          NonManifoldGroup->Append(aSubShell);
        }
      }
      else if (!isManifold && (aSubShape.ShapeType() == TopAbs_SHELL) ) {
        RepItemSeq->Append(aSubShape);
        NonManifoldGroup->Append(aSubShape);
      } 
      else
        RepItemSeq->Append( iter.Value() );
    }

    Standard_Integer aNMItemsNb = RepItemSeq->Length();
     
    // In case of pure manifold topology do nothing; theShape is processed as usual (see below)
    if (aNMItemsNb > 0) {

      // Prepare SDR for non-manifold group. This SDR will be linked to NMSSR by means
      // of TransferShape invocation. SDR is not created if there is no any manifold
      // topology in the passed Compound. If topology is pure non-manifold, SDR0 (passed)
      // is used
      Handle(StepShape_ShapeDefinitionRepresentation) sdr;
      if (isOnlyNonManifold)
        sdr = SDR0;
      else {
        STEPConstruct_Part SDRTool;
        SDRTool.MakeSDR( 0, myContext.GetProductName(), myContext.GetAPD()->Application() );
        sdr = SDRTool.SDRValue();
      }

      aNMBinder = TransientResult(sdr);
    
      // Complete SDR with shape representations.
      // NOTE: aNMBinder is connected now with this SDR. It will be added to the resulting
      //       binder in the end of this invocation of TransferShape
      for (Standard_Integer i = 1; i <= aNMItemsNb; i++) {
        Handle(TransferBRep_ShapeMapper) aMapper = TransferBRep::ShapeMapper( FP, RepItemSeq->Value(i) );
        TransferShape(aMapper, sdr, FP, NonManifoldGroup, Standard_False);
      }

      // Nothing else needed for pure non-manifold topology, return
      if (isOnlyNonManifold)
        return aNMBinder;
   
    }

  }
  // [END] Separate manifold topology from non-manifold in group mode 0 (ssv; 18.11.2010)

  // create a list of items to translate
  Handle(TopTools_HSequenceOfShape) RepItemSeq = new TopTools_HSequenceOfShape();
  
  Standard_Boolean isSeparateVertices = 
    Interface_Static::IVal("write.step.vertex.mode") == 0;//bug 23950
  // PTV 16.09.2002 OCC725 separate shape from solo vertices.
  Standard_Boolean isOnlyVertices = Standard_False;
  if (theShape.ShapeType() == TopAbs_COMPOUND) {
    Standard_Integer countVrtx = 0;
    Standard_Integer countSh = 0;
    TopoDS_Compound aNewShape, aCompOfVrtx;
    BRep_Builder aB;
    aB.MakeCompound(aNewShape);
    aB.MakeCompound(aCompOfVrtx);
    TopoDS_Iterator anCompIt(theShape);
    if (isSeparateVertices) {
      for (; anCompIt.More(); anCompIt.Next()) {
        TopoDS_Shape aCurSh = anCompIt.Value();
        if (aCurSh.ShapeType() != TopAbs_VERTEX) {
          aB.Add(aNewShape, aCurSh);
          countSh++;
        }
        else {
          aB.Add(aCompOfVrtx, aCurSh);
          countVrtx++;
        }
      }
      // replace the shapes
      if (countSh)
        theShape = aNewShape;
      if (countVrtx)
        RepItemSeq->Append(aCompOfVrtx);
      if (countSh == 0) 
        isOnlyVertices = Standard_True;
    }
  } 
  
  if (theShape.ShapeType() == TopAbs_COMPOUND) {
    TopExp_Explorer SolidExp, ShellExp, FaceExp;
    if (mymode != STEPControl_GeometricCurveSet) {
      for (SolidExp.Init(theShape, TopAbs_SOLID);
           SolidExp.More();SolidExp.Next()) {
        RepItemSeq->Append(TopoDS::Solid(SolidExp.Current()));
      }
      for (ShellExp.Init(theShape, TopAbs_SHELL, TopAbs_SOLID);
           ShellExp.More();ShellExp.Next()) {
        RepItemSeq->Append(TopoDS::Shell(ShellExp.Current()));
      }
      
      for (FaceExp.Init(theShape, TopAbs_FACE, TopAbs_SHELL);
           FaceExp.More();FaceExp.Next()) {
        RepItemSeq->Append(TopoDS::Face(FaceExp.Current()));
      }
    }
    else {
      if (!isOnlyVertices) 
        RepItemSeq->Append(theShape); //:j1
    }
    if(mymode == STEPControl_AsIs) {
      TopExp_Explorer WireExp, EdgeExp;
      for (WireExp.Init(theShape, TopAbs_WIRE, TopAbs_FACE);
           WireExp.More();WireExp.Next()) 
        RepItemSeq->Append(TopoDS::Wire(WireExp.Current()));
      for (EdgeExp.Init(theShape, TopAbs_EDGE, TopAbs_WIRE);
           EdgeExp.More();EdgeExp.Next()) 
        RepItemSeq->Append(TopoDS::Edge(EdgeExp.Current()));
    }
    
  }
  else if (theShape.ShapeType() == TopAbs_SOLID) {
    RepItemSeq->Append(TopoDS::Solid(theShape));
  }
  else if (theShape.ShapeType() == TopAbs_SHELL) {
    RepItemSeq->Append(TopoDS::Shell(theShape));
  }
  else if (theShape.ShapeType() == TopAbs_FACE) {
    RepItemSeq->Append(TopoDS::Face(theShape));
  }
  else if (theShape.ShapeType() == TopAbs_COMPSOLID) {
    FP->AddWarning(start,"NonManifold COMPSOLID was translated like a set of SOLIDs");
    if ( GroupMode() > 0)
      return TransferCompound(start, SDR0, FP);
    else {
      TopExp_Explorer SolidExp;
      for (SolidExp.Init(theShape, TopAbs_SOLID);
           SolidExp.More();SolidExp.Next()) {
        RepItemSeq->Append(TopoDS::Solid(SolidExp.Current()));
      }
    }
  }

  else if (mymode != STEPControl_GeometricCurveSet && mymode != STEPControl_AsIs) {
    FP->AddFail(start,"The Shape is not a SOLID, nor a SHELL, nor a FACE");
    return binder;
  }
  else RepItemSeq->Append (theShape);

  //    COMPUTING 3D TOLERANCE
  //    Either from Session, or Computed (Least,Average, or Greatest)
  //    Then given to TopoDSToStep_Tool
  Standard_Real Tol = UsedTolerance (mytoler,theShape);
  
  // Create a STEP-Entity for each TopoDS_Shape  
  // according to the current StepModelMode
  
  Standard_Integer nbs = RepItemSeq->Length();
  Handle(TColStd_HSequenceOfTransient) ItemSeq = 
    new TColStd_HSequenceOfTransient();

//ptv 10.11.00: allow to write empty Compound:  if (GroupMode() >0)
  ItemSeq->Append (myContext.GetDefaultAxis());
  STEPControl_StepModelType trmode = mymode;
  for (Standard_Integer i = 1; i <= nbs; i++) {
    TopoDS_Shape xShape = RepItemSeq->Value(i);
 
    if(mymode == STEPControl_AsIs) {
      switch(xShape.ShapeType()) {
        case TopAbs_SOLID :  trmode = STEPControl_ManifoldSolidBrep;break;
        case TopAbs_SHELL :  trmode = STEPControl_ShellBasedSurfaceModel; break;
        case TopAbs_FACE : trmode = STEPControl_ShellBasedSurfaceModel;break;
        default : trmode =STEPControl_GeometricCurveSet; break;
      }
    }
    //:abv 24Jan99 CAX-IF TRJ3: expanded Shape Processing
//    TopoDS_Shape aShape = xShape;
    // eliminate conical surfaces with negative semiangles
//    Handle(TopoDSToStep_ConicalSurfModif) CSM = new TopoDSToStep_ConicalSurfModif();
//    BRepTools_Modifier CSMT(aShape,CSM);
//    if ( CSMT.IsDone() ) aShape = CSMT.ModifiedShape ( aShape );
//    // eliminate indirect elementary surfaces
//    Handle(TopoDSToStep_DirectModification) DM = new TopoDSToStep_DirectModification();
//    BRepTools_Modifier DMT(aShape,DM);
//    if ( DMT.IsDone() ) aShape = DMT.ModifiedShape ( aShape );
////    aShape = TopoDSToStep::DirectFaces(xShape);
    Handle(Standard_Transient) info;
    Standard_Real maxTol = Interface_Static::RVal("read.maxprecision.val");

    TopoDS_Shape aShape;
    aShape = XSAlgo::AlgoContainer()->ProcessShape(xShape, Tol, maxTol, 
                                                  "write.step.resource.name", 
                                                  "write.step.sequence", info,
                                                  FP->GetProgress() );
    if (!isManifold) {
      mergeInfoForNM(FP, info);
    }

    // create a STEP entity corresponding to shape
    Handle(StepGeom_GeometricRepresentationItem) item;
    switch (trmode)
      {
      case STEPControl_ManifoldSolidBrep:
	{
	  if (aShape.ShapeType() == TopAbs_SOLID) {
	    TopoDS_Solid aSolid = TopoDS::Solid(aShape);

	    //:d6 abv 13 Mar 98: if solid has more than 1 shell, 
	    // try to treat it as solid with voids
	    Standard_Integer nbShells = 0;
	    for ( TopoDS_Iterator It ( aSolid ); It.More(); It.Next() ) 
              if (It.Value().ShapeType() == TopAbs_SHELL) nbShells++;
	    if ( nbShells >1 ) {
	      TopoDSToStep_MakeBrepWithVoids MkBRepWithVoids(aSolid,FP);
	      MkBRepWithVoids.Tolerance() = Tol;
	      if (MkBRepWithVoids.IsDone()) {
		item = MkBRepWithVoids.Value();
	      }
	      else nbShells = 1; //smth went wrong; let it will be just Manifold
	    }
	    if ( nbShells ==1 ) {

	      TopoDSToStep_MakeManifoldSolidBrep MkManifoldSolidBrep(aSolid,FP);
	      MkManifoldSolidBrep.Tolerance() = Tol;
	      if (MkManifoldSolidBrep.IsDone()) {
		item = MkManifoldSolidBrep.Value();
	      }
	    }
	  }
	  else if (aShape.ShapeType() == TopAbs_SHELL) {
	    TopoDS_Shell aShell = TopoDS::Shell(aShape);
	    TopoDSToStep_MakeManifoldSolidBrep MkManifoldSolidBrep(aShell,FP);
	    MkManifoldSolidBrep.Tolerance() = Tol;
	    if (MkManifoldSolidBrep.IsDone()) {
	      item = MkManifoldSolidBrep.Value();
	    }
	  }
	  break;
	}
      case STEPControl_BrepWithVoids:
	{
	  if (aShape.ShapeType() == TopAbs_SOLID) {
	    TopoDS_Solid aSolid = TopoDS::Solid(aShape);
	    TopoDSToStep_MakeBrepWithVoids MkBRepWithVoids(aSolid,FP);
	    MkBRepWithVoids.Tolerance() = Tol;
	    if (MkBRepWithVoids.IsDone()) {
	      item = MkBRepWithVoids.Value();
	    }
	  }
	  break;
	}
      case STEPControl_FacetedBrep:
	{
	  TopoDSToStep_FacetedError facErr = TopoDSToStep_FacetedTool::CheckTopoDSShape(aShape);
	  if (facErr != TopoDSToStep_FacetedDone) {
	    FP->AddFail(start,"Error in Faceted Shape from TopoDS");
	    if (facErr == TopoDSToStep_SurfaceNotPlane) {
	      FP->AddFail(start,"-- The TopoDS_Face is not plane");
	    }
	    else if (facErr == TopoDSToStep_PCurveNotLinear) {
	      FP->AddFail(start,"-- The Face contains non linear PCurves");
	    }
	    return binder;
	  }
	  if (aShape.ShapeType() == TopAbs_SOLID) {
	    TopoDS_Solid aSolid = TopoDS::Solid(aShape);
	    TopoDSToStep_MakeFacetedBrep MkFacetedBrep(aSolid,FP);
	    MkFacetedBrep.Tolerance() = Tol;
	    if (MkFacetedBrep.IsDone()) {
	      item = MkFacetedBrep.Value();
	    }
	  }
	  break;
	}
      case STEPControl_FacetedBrepAndBrepWithVoids:
	{
	  TopoDSToStep_FacetedError facErr = TopoDSToStep_FacetedTool::CheckTopoDSShape(aShape);
	  if (facErr != TopoDSToStep_FacetedDone) {
	    FP->AddFail(start,"Error in Faceted Shape from TopoDS");
	    if (facErr == TopoDSToStep_SurfaceNotPlane) {
	      FP->AddFail(start,"-- The TopoDS_Face is not plane");
	    }
	    else if (facErr == TopoDSToStep_PCurveNotLinear) {
	      FP->AddFail(start,"-- The Face contains non linear PCurves");
	    }
	    return binder;
	  }
	  if (aShape.ShapeType() == TopAbs_SOLID) {
	    TopoDS_Solid aSolid = TopoDS::Solid(aShape);
	    TopoDSToStep_MakeFacetedBrepAndBrepWithVoids 
	      MkFacetedBrepAndBrepWithVoids(aSolid,FP);
	    MkFacetedBrepAndBrepWithVoids.Tolerance() = Tol;
	    if (MkFacetedBrepAndBrepWithVoids.IsDone()) {
	      item = MkFacetedBrepAndBrepWithVoids.Value();
	    }
	  }
	  break;
	}
      case STEPControl_ShellBasedSurfaceModel:
	{
	  if (aShape.ShapeType() == TopAbs_SOLID) {
	    TopoDS_Solid aSolid = TopoDS::Solid(aShape);
	    TopoDSToStep_MakeShellBasedSurfaceModel
	      MkShellBasedSurfaceModel(aSolid, FP);
	    MkShellBasedSurfaceModel.Tolerance() = Tol;
	    if (MkShellBasedSurfaceModel.IsDone()) {
	      item = MkShellBasedSurfaceModel.Value();
	    }
	  }
	  else if (aShape.ShapeType() == TopAbs_SHELL) {
            TopoDS_Shell aShell = TopoDS::Shell(aShape);
            // Non-manifold topology is stored via NMSSR containing series of SBSM (ssv; 13.11.2010)
            TopoDSToStep_MakeShellBasedSurfaceModel MkShellBasedSurfaceModel(aShell, FP);
            MkShellBasedSurfaceModel.Tolerance() = Tol;
            if (MkShellBasedSurfaceModel.IsDone()) {
              item = MkShellBasedSurfaceModel.Value();
            }
	  }
	  else if (aShape.ShapeType() == TopAbs_FACE) {
	    TopoDS_Face aFace = TopoDS::Face(aShape);
	    TopoDSToStep_MakeShellBasedSurfaceModel
	      MkShellBasedSurfaceModel(aFace, FP);
	    MkShellBasedSurfaceModel.Tolerance() = Tol;
	    if (MkShellBasedSurfaceModel.IsDone()) {
	      item = MkShellBasedSurfaceModel.Value();
	    }
	  }
	  break;
	}
      case STEPControl_GeometricCurveSet:
	{
	  TopoDSToStep_MakeGeometricCurveSet MkGeometricCurveSet(aShape,FP);
	  MkGeometricCurveSet.Tolerance() = Tol;
	  if (MkGeometricCurveSet.IsDone()) {
	    item = MkGeometricCurveSet.Value();
	  }
          // PTV 22.08.2002 OCC609 ------------------------- begin --------------------
          // modified by PTV 16.09.2002 OCC725
          else if (aShape.ShapeType() == TopAbs_COMPOUND || 
                   aShape.ShapeType() == TopAbs_VERTEX) {
            // it is compund with solo vertices.
            Standard_Integer aNbVrtx = 0;
            Standard_Integer curNb = 0;
            TopExp_Explorer anExp (aShape, TopAbs_VERTEX);
            for ( ; anExp.More(); anExp.Next() ) {
              if ( anExp.Current().ShapeType() != TopAbs_VERTEX )
                continue;
              aNbVrtx++;
            }
            if ( aNbVrtx ) {
              // create new geometric curve set for all vertices
              Handle(StepShape_HArray1OfGeometricSetSelect) aGSS =
                new StepShape_HArray1OfGeometricSetSelect(1,aNbVrtx);
              Handle(TCollection_HAsciiString) empty = new TCollection_HAsciiString("");
              Handle(StepShape_GeometricCurveSet) aGCSet =
                new StepShape_GeometricCurveSet;
              aGCSet->SetName(empty);
              // iterates on compound with vertices and trances each vertex
              for ( anExp.ReInit() ; anExp.More(); anExp.Next() ) {
                TopoDS_Shape aVertex = anExp.Current();
                if ( aVertex.ShapeType() != TopAbs_VERTEX )
                  continue;
                curNb++;
                transferVertex (FP, aGSS, aVertex, curNb);
              } // end of iteration on compound with vertices.
              aGCSet->SetElements(aGSS);
              item = aGCSet;
            } // end of check that number of vertices is not null
          }
          // PTV 22.08.2002 OCC609-------------------------  end  --------------------
	  break;
	}
      default: break;
      }
    if ( item.IsNull() ) continue;

    // add resulting item to the FP
    ItemSeq->Append(item);
    Handle(TransferBRep_ShapeMapper) submapper;
    if ( xShape.IsSame ( mapper->Value() ) ) 
      submapper = Handle(TransferBRep_ShapeMapper)::DownCast ( start );
    if ( submapper.IsNull() ) submapper = TransferBRep::ShapeMapper (FP,xShape);
    Handle(Transfer_Binder) subbind = FP->Find ( submapper );
    if ( subbind.IsNull() ) {
      subbind = TransientResult ( item );
      FP->Bind ( submapper, subbind );
    }
    else subbind->AddResult ( TransientResult ( item ) );

    //:abv 24Jan99 CAX-IF TRJ3: Update FinderProcess map to take into account shape processing
//    UpdateMap ( xShape, CSMT, DMT, FP );
    XSAlgo::AlgoContainer()->MergeTransferInfo(FP, info);
  }
  
  // - Make Shape Representation 
  Standard_Integer nCc1 = ItemSeq->Length();
  if (nCc1 < 1) {
    FP->AddFail(start,"The Shape has not the appropriate type");
    return binder;
  }
  Handle(StepShape_ShapeRepresentation) shapeRep;
  if ( theShape.ShapeType() == TopAbs_SHAPE ) { // for external references
    shapeRep = new StepShape_ShapeRepresentation;
  }
  else {
    switch (mymode) {
    case STEPControl_ManifoldSolidBrep:
      shapeRep = new StepShape_AdvancedBrepShapeRepresentation;
      break;
    case STEPControl_FacetedBrep:
      shapeRep = new StepShape_FacetedBrepShapeRepresentation;
      break;
    // NOTE: STEPControl_AsIs mode is normally used to transfer non-manifold topology.
    //       However, as ShellBasedSurfaceModel is used in non-manifold processing
    //       internally, STEPControl_ShellBasedSurfaceModel is also adjusted to
    //       be able to work with non-manifold cases
    case STEPControl_ShellBasedSurfaceModel:
      if (isManifold)
        shapeRep = new StepShape_ManifoldSurfaceShapeRepresentation;
      else {
        Standard_Boolean isNewNMSSRCreated;
        shapeRep = this->getNMSSRForGroup(shapeGroup, FP, isNewNMSSRCreated);
        useExistingNMSSR = !isNewNMSSRCreated;
      }
      break;
    case STEPControl_GeometricCurveSet:
      shapeRep = new StepShape_GeometricallyBoundedWireframeShapeRepresentation;
      break;
    case STEPControl_AsIs :
    {
      if(nbs == 1) {
        if(trmode == STEPControl_ManifoldSolidBrep)
          shapeRep = new StepShape_AdvancedBrepShapeRepresentation;
        else if(trmode == STEPControl_ShellBasedSurfaceModel)
          // Process non-manifold topology separately (ssv; 13.11.2010)
          if (isManifold)
            shapeRep = new StepShape_ManifoldSurfaceShapeRepresentation;
          else {
            Standard_Boolean isNewNMSSRCreated;
            shapeRep = this->getNMSSRForGroup(shapeGroup, FP, isNewNMSSRCreated);
            useExistingNMSSR = !isNewNMSSRCreated;
          }
        else if(trmode == STEPControl_GeometricCurveSet)
          shapeRep = new StepShape_GeometricallyBoundedWireframeShapeRepresentation;
        else if(trmode ==STEPControl_FacetedBrep)
          shapeRep = new StepShape_FacetedBrepShapeRepresentation; 
      }
      else shapeRep = new StepShape_ShapeRepresentation;
    }
      break;
    default: break;
    }
  }
  if(shapeRep.IsNull()) {
    Handle(Transfer_Binder) resb;
    return resb;
  }
    
  Handle(StepRepr_HArray1OfRepresentationItem) items =
    new StepRepr_HArray1OfRepresentationItem(1,nCc1);

  for (Standard_Integer rep = 1; rep <= nCc1; rep++) {
    Handle(StepRepr_RepresentationItem) repit = 
      GetCasted(StepRepr_RepresentationItem, ItemSeq->Value(rep));
    items->SetValue(rep,repit);
  }
  Standard_Integer ap = Interface_Static::IVal("write.step.schema");
  Transfer_SequenceOfBinder aSeqBindRelation;
  if(ap == 3 && nbs > 1) {
    Standard_Integer j = 1;
    if(items->Value(j)->IsKind(STANDARD_TYPE(StepGeom_Axis2Placement3d))) {
      Handle(StepRepr_HArray1OfRepresentationItem) axis =
        new StepRepr_HArray1OfRepresentationItem(1,1);
      axis->SetValue(1,items->Value(j++));
      shapeRep->SetItems(axis);
    }
    for (; j <= items->Length(); j++) {
      
      Handle(StepShape_ShapeRepresentation) ShapeRepr1;
      if(items->Value(j)->IsKind(STANDARD_TYPE(StepShape_ManifoldSolidBrep)))
         ShapeRepr1 = new StepShape_AdvancedBrepShapeRepresentation;
      else if(items->Value(j)->IsKind(STANDARD_TYPE(StepShape_ShellBasedSurfaceModel))) 
        ShapeRepr1 = new StepShape_ManifoldSurfaceShapeRepresentation;
      else if(items->Value(j)->IsKind(STANDARD_TYPE(StepShape_GeometricCurveSet)))
        ShapeRepr1 = new StepShape_GeometricallyBoundedWireframeShapeRepresentation;
      else if (items->Value(j)->IsKind(STANDARD_TYPE(StepShape_FacetedBrep)))
        ShapeRepr1 = new StepShape_FacetedBrepShapeRepresentation;
      else ShapeRepr1 = new StepShape_ShapeRepresentation;
      
      Handle(StepRepr_HArray1OfRepresentationItem) repr1 = new StepRepr_HArray1OfRepresentationItem(1,2);
      repr1->SetValue(1,myContext.GetDefaultAxis());
      repr1->SetValue(2,items->Value(j));
      ShapeRepr1->SetItems(repr1);
      STEPConstruct_UnitContext mk1;
      mk1.Init(Tol);
      ShapeRepr1->SetContextOfItems(mk1.Value());  // la tolerance, voir au debut
      ShapeRepr1->SetName (new TCollection_HAsciiString(""));
      
      Handle(StepRepr_ShapeRepresentationRelationship) aShapeRel = new StepRepr_ShapeRepresentationRelationship;
      Handle(TCollection_HAsciiString) aName = new TCollection_HAsciiString("");
      Handle(TCollection_HAsciiString) aDescr = new TCollection_HAsciiString("");
      aShapeRel->SetName(aName);
      aShapeRel->SetDescription(aDescr);
      aShapeRel->SetRep2(shapeRep);
      aShapeRel->SetRep1(ShapeRepr1);
      
      aSeqBindRelation.Append(TransientResult (aShapeRel));
    }
  }
  else {
    if (!useExistingNMSSR)
      shapeRep->SetItems(items);
    else {
      // Add new representation item to the NMSSR's existing collection (ssv; 13.11.2010)
      Handle(StepRepr_HArray1OfRepresentationItem) oldItems = shapeRep->Items();
      Handle(StepRepr_HArray1OfRepresentationItem) newItems = 
        new StepRepr_HArray1OfRepresentationItem(1, oldItems->Length() + 1);
      Standard_Integer el = 1;
      for (Standard_Integer i = 1; i <= oldItems->Length(); i++)
        newItems->SetValue( el++, oldItems->Value(i) );
      newItems->SetValue( el, items->Value( items->Length() ) );
      shapeRep->SetItems(newItems);
    }
  }

  // init representation
  STEPConstruct_UnitContext mk;
  mk.Init(Tol);
  shapeRep->SetContextOfItems(mk.Value());  // la tolerance, voir au debut
  shapeRep->SetName (new TCollection_HAsciiString(""));

  // Create SDR (only once for non-manifold group)
  if (!useExistingNMSSR) {
    SDR0->SetUsedRepresentation (shapeRep);
    // create binder for SR and attach to it binder for RepItem (if exists)
    Handle(Transfer_Binder) resbind = TransientResult(shapeRep);
    binder = FP->Find(start);
    if ( ! binder.IsNull() ) {
      resbind->AddResult ( binder );
      FP->Rebind(start,resbind);
      //binder->AddResult ( resbind );
      //resbind = binder;
    }
    for(Standard_Integer k = 1; k <= aSeqBindRelation.Length(); k++)
      resbind->AddResult(aSeqBindRelation.Value(k));

    // Add SDR for non-manifold topology in group mode 0 (ssv; 18.11.2010)
    if ( !aNMBinder.IsNull() )
      resbind->AddResult(aNMBinder);

    return resbind;
  } else return FP->Find(start);

}

//=======================================================================
//function : TransferCompound
//    ####    TRANSFER COMPOUND AS (SUB-)ASSEMBLY
//purpose  : 
//=======================================================================

Handle(Transfer_Binder) STEPControl_ActorWrite::TransferCompound (const Handle(Transfer_Finder)& start,
                                                                  const Handle(StepShape_ShapeDefinitionRepresentation)& SDR0,
                                                                  const Handle(Transfer_FinderProcess)& FP)
{
  Handle(TransferBRep_ShapeMapper) mapper = Handle(TransferBRep_ShapeMapper)::DownCast(start);
  Handle(Transfer_Binder) binder;
  if (mapper.IsNull()) return binder;
  TopoDS_Shape theShape = mapper->Value();

  // Inspect non-manifold topology case (ssv; 10.11.2010)
  Standard_Boolean isNMMode = Interface_Static::IVal("write.step.nonmanifold") != 0;
  Standard_Boolean isManifold;
  if (isNMMode)
    isManifold = IsManifoldShape(theShape);
  else
    isManifold = Standard_True;

  // get a sequence of components (subshapes)
  Handle(TopTools_HSequenceOfShape) RepItemSeq = new TopTools_HSequenceOfShape();
  // Prepare a collection for non-manifold group of shapes
  Handle(TopTools_HSequenceOfShape) NonManifoldGroup = new TopTools_HSequenceOfShape();
  Standard_Boolean isSeparateVertices = 
    (Interface_Static::IVal("write.step.vertex.mode") == 0);//bug 23950
  // PTV OCC725 17.09.2002 -- begin --
  Standard_Integer nbFreeVrtx = 0;
  TopoDS_Compound aCompOfVrtx;
  BRep_Builder aB;
  aB.MakeCompound(aCompOfVrtx);
 
  #ifdef OCCT_DEBUG
  if (!isManifold)
    cout << "Exploding Solids to Shells if any..." << endl;
  #endif

  for (TopoDS_Iterator iter(theShape); iter.More(); iter.Next()) {
    TopoDS_Shape aSubShape = iter.Value();
    if (aSubShape.ShapeType() != TopAbs_VERTEX || !isSeparateVertices) {

      // Store non-manifold topology as shells (ssv; 10.11.2010)
      if (!isManifold && aSubShape.ShapeType() == TopAbs_SOLID) {
        for ( TopoDS_Iterator aSubIter(aSubShape); aSubIter.More(); aSubIter.Next() ) {
          TopoDS_Shell aSubShell = TopoDS::Shell( aSubIter.Value() );
          aSubShell.Closed(Standard_True);
          RepItemSeq->Append(aSubShell);
          NonManifoldGroup->Append(aSubShell);
        }
      } 
      else if (!isManifold && (aSubShape.ShapeType() == TopAbs_SHELL) ) {
        RepItemSeq->Append(aSubShape);
        NonManifoldGroup->Append(aSubShape);
      }
      else
        RepItemSeq->Append(aSubShape);

      continue;
    }
    aB.Add(aCompOfVrtx, iter.Value());
    nbFreeVrtx++;
  }
  if (nbFreeVrtx)
    RepItemSeq->Append (aCompOfVrtx);

  // PTV OCC725 17.09.2002 -- end --
  
  // Constitution : liste d axes, le premier est l origine, les suivants : 1
  // par sous-item
  Handle(StepShape_ShapeRepresentation) shapeRep =
    Handle(StepShape_ShapeRepresentation)::DownCast(SDR0->UsedRepresentation());
  if ( shapeRep.IsNull() ) {
    shapeRep = new StepShape_ShapeRepresentation;
    SDR0->SetUsedRepresentation(shapeRep);  // to be used by MakeItem
  }
  binder = TransientResult(SDR0); // set SDR as first item in order to be found first (but not SDR of subshape!)
  binder->AddResult ( TransientResult(shapeRep) );

  // translate components
  Standard_Integer i, nbs = RepItemSeq->Length();
  Handle(TColStd_HSequenceOfTransient) ItemSeq = new TColStd_HSequenceOfTransient();
  ItemSeq->Append (myContext.GetDefaultAxis());
  myContext.NextLevel();
  for ( i = 1; i <= nbs; i ++) {
    Handle(TransferBRep_ShapeMapper) subs = TransferBRep::ShapeMapper (FP,RepItemSeq->Value(i));
    Handle(StepGeom_Axis2Placement3d) AX1;
    
    Handle(Transfer_Binder) bnd = TransferSubShape(subs, SDR0, AX1, FP, NonManifoldGroup, isManifold);

    if (!AX1.IsNull()) ItemSeq->Append (AX1);
    // copy binders so as to have all roots in upper binder, but do not conflict
    while ( !bnd.IsNull() ) {
      Handle(Transfer_SimpleBinderOfTransient) bx = 
        Handle(Transfer_SimpleBinderOfTransient)::DownCast(bnd);
      if ( !bx.IsNull() ) {
        // Single SDR is created for a non-manifold group (ssv: 12.11.2010)
        if (!isManifold && i > 1)
          break;
        else
          binder->AddResult( TransientResult( bx->Result() ) );
      }
      bnd = bnd->NextResult();
    }
  }
  myContext.PrevLevel();

  Standard_Integer nsub = ItemSeq->Length();
  Handle(StepRepr_HArray1OfRepresentationItem) items =
    new StepRepr_HArray1OfRepresentationItem(1,nsub);

  // initialize representation
  for (Standard_Integer rep = 1; rep <= nsub; rep++)
    items->SetValue(rep,GetCasted(StepRepr_RepresentationItem, ItemSeq->Value(rep)));
  shapeRep->SetItems(items);
  Standard_Real Tol = UsedTolerance (mytoler,theShape);
  STEPConstruct_UnitContext mk;
  mk.Init(Tol);
  shapeRep->SetContextOfItems(mk.Value());  // la tolerance, voir au debut
  shapeRep->SetName (new TCollection_HAsciiString(""));

  // set it to SDR
//  SDR0->SetUsedRepresentation (shapeRep);

  return binder;
}

//=======================================================================
//function : TransferSubShape
//purpose  : 
//=======================================================================

Handle(Transfer_Binder)  STEPControl_ActorWrite::TransferSubShape (const Handle(Transfer_Finder)& start,
                                                                   const Handle(StepShape_ShapeDefinitionRepresentation)& SDR0,
                                                                   Handle(StepGeom_Axis2Placement3d)& AX1,
                                                                   const Handle(Transfer_FinderProcess)& FP,
                                                                   const Handle(TopTools_HSequenceOfShape)& shapeGroup,
                                                                   const Standard_Boolean isManifold)
{
  Handle(TransferBRep_ShapeMapper) mapper = Handle(TransferBRep_ShapeMapper)::DownCast(start);
  if (mapper.IsNull()) return NullResult();
  TopoDS_Shape shape = mapper->Value();

  //   SHAPE EN POSITION VENANT D UN ASSEMBLAGE
  //   Il faut alors distinguer la transformation de la shape meme
  //   laquelle est consideree a l origine, puis transferee
  //   A part, un item decrivant une occurence en position est cree
  //   SINON, la shape est prise et transferee telle quelle
  TopoDS_Shape sh0 = shape;
  gp_Trsf aLoc;
  if ( GroupMode() >0) {
    TopLoc_Location shloc = shape.Location();
    aLoc = shloc.Transformation();
    TopLoc_Location shident;
    sh0.Location (shident);
    mapper = TransferBRep::ShapeMapper(FP,sh0);
    mapper->SameAttributes (start);
  }

  Handle(Transfer_Binder) resbind = FP->Find(mapper);
  Handle(StepShape_ShapeDefinitionRepresentation) sdr;
//  Handle(StepShape_ShapeRepresentation) resultat;
  STEPConstruct_Part SDRTool;  

  // Already SDR and SR available : take them as are
  Standard_Boolean iasdr = FP->GetTypedTransient
    (resbind,STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation),sdr);
  if ( iasdr ) SDRTool.ReadSDR ( sdr ); 
  else { 
    SDRTool.MakeSDR ( 0, myContext.GetProductName(), myContext.GetAPD()->Application() );
    sdr = SDRTool.SDRValue();
  }
//  resultat = GetCasted(StepShape_ShapeRepresentation,sdr->UsedRepresentation());

  // if shape itself not yet translated, do it now
  //:abv 20.05.02: see comment in TransferShape(): added "! iasdr ||"
  Handle(Transfer_Binder) resprod = TransientResult(sdr);  //KA - OCC7141(skl 10.11.2004)
  if ( ! iasdr || resbind.IsNull() ) {
    resbind = TransferShape(mapper, sdr, FP, shapeGroup, isManifold);
    Handle(Transfer_Binder) oldbind = FP->Find ( mapper );
    if ( ! oldbind.IsNull() && !resbind.IsNull()) resbind->AddResult ( oldbind );
    FP->Bind (mapper,resbind);
    resprod=resbind; //KA - OCC7141(skl 10.11.2004)
  }
  if (resprod.IsNull())
    return resprod;

  // A new resbind may have been produced
//  DeclareAndCast(Transfer_SimpleBinderOfTransient,restrans,resbind);
//  if (restrans.IsNull()) return resbind;
//  FP->GetTypedTransient (resbind,STANDARD_TYPE(StepShape_ShapeRepresentation),resultat);
//  sdr->SetUsedRepresentation(resultat);  // to be used by MakeItem

  // make location for assembly placement
  GeomToStep_MakeAxis2Placement3d mkax (aLoc);
  Handle(StepGeom_Axis2Placement3d) AxLoc = mkax.Value();
  AX1 = AxLoc;

  // create assembly structures (CDSR, NAUO etc.)
  STEPConstruct_Assembly mkitem;
  mkitem.Init (sdr,SDR0,myContext.GetDefaultAxis(),AxLoc);
  mkitem.MakeRelationship ();
  Handle(TColStd_HSequenceOfTransient) roots = myContext.GetRootsForAssemblyLink ( mkitem );

  // add roots corresponding to assembly and product structures to binder
  //Handle(Transfer_Binder) resprod = resbind; //KA - OCC7141(skl 10.11.2004)
  //KA: we need only the current subshape in resprod, since the binder is copied
  //    in Transfershape which calls Transfersubshape   [ OCC7141(skl 10.11.2004) ]
  if ( ! iasdr ) {
    resprod->AddResult ( TransientResult ( SDRTool.SDRValue() ) );
    resbind->AddResult ( TransientResult ( SDRTool.SDRValue() ) ); //KA - OCC7141(skl 10.11.2004)
    roots->Append ( myContext.GetRootsForPart ( SDRTool ) );
  }
  for ( Standard_Integer i=1; i <= roots->Length(); i++ ) {
    resprod->AddResult ( TransientResult ( roots->Value(i) ) );
    resbind->AddResult ( TransientResult ( roots->Value(i) ) );  //KA - OCC7141(skl 10.11.2004)
  }
  myContext.NextIndex();

  //FP->Bind (mapper,resprod); //KA - OCC7141(skl 10.11.2004)

  // abv 16.10.00: bind CDSR (et al) to located shape in order to be able to track instances
  if ( mapper != start ) {
    Handle(Transfer_Binder) bnd = FP->Find ( start );
    for ( Standard_Integer j=1; j <= roots->Length(); j++ ) 
      if ( bnd.IsNull() ) bnd = TransientResult ( roots->Value(j) );
      else bnd->AddResult ( TransientResult ( roots->Value(j) ) );
    FP->Bind ( start, bnd );
  }
  
  return resprod;
}
