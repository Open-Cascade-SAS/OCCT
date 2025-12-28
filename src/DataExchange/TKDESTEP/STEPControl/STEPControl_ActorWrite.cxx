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

//: k8 abv 6 Jan 99: unique names for PRODUCTs
//: k9 abv 6 Jan 99: TR10: eliminating duplicated APPLICATION_CONTEXT entities
// abv,gka 05.04.99: S4136: change parameters and update units
//  PTV    22.08.2002 OCC609 transfer solo vertices into step file.
//  PTV    16.09.2002 OCC725 transfer compound of vertices into one geometrical curve set.

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TEdge.hxx>
#include <BRepTools_Modifier.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <GeomToStep_MakeAxis2Placement3d.hxx>
#include <GeomToStep_MakeCartesianTransformationOperator.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_MSG.hxx>
#include <Message_ProgressScope.hxx>
#include <ShapeAnalysis_ShapeTolerance.hxx>
#include <ShapeProcess_ShapeContext.hxx>
#include <Standard_Type.hxx>
#include <StepBasic_ApplicationProtocolDefinition.hxx>
#include <StepBasic_Product.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <STEPConstruct_Assembly.hxx>
#include <STEPConstruct_Part.hxx>
#include <STEPConstruct_UnitContext.hxx>
#include <STEPControl_ActorWrite.hxx>
#include <STEPControl_StepModelType.hxx>
#include <StepData_StepModel.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_CartesianTransformationOperator3d.hxx>
#include <StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx.hxx>
#include <StepGeom_Point.hxx>
#include <StepRepr_ShapeRepresentationRelationship.hxx>
#include <StepShape_AdvancedBrepShapeRepresentation.hxx>
#include <StepShape_BrepWithVoids.hxx>
#include <StepShape_FacetedBrep.hxx>
#include <StepShape_FacetedBrepAndBrepWithVoids.hxx>
#include <StepShape_FacetedBrepShapeRepresentation.hxx>
#include <StepShape_GeometricallyBoundedWireframeShapeRepresentation.hxx>
#include <StepShape_GeometricCurveSet.hxx>
#include <StepShape_GeometricSetSelect.hxx>
#include <StepShape_ManifoldSolidBrep.hxx>
#include <StepShape_ManifoldSurfaceShapeRepresentation.hxx>
#include <StepShape_NonManifoldSurfaceShapeRepresentation.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <StepShape_ShellBasedSurfaceModel.hxx>
#include <StepShape_VertexPoint.hxx>
#include <StepVisual_TessellatedItem.hxx>
#include <StepVisual_TessellatedShapeRepresentation.hxx>
#include <StepVisual_TessellatedSolid.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDSToStep_FacetedTool.hxx>
#include <TopoDSToStep_MakeBrepWithVoids.hxx>
#include <TopoDSToStep_MakeFacetedBrep.hxx>
#include <TopoDSToStep_MakeFacetedBrepAndBrepWithVoids.hxx>
#include <TopoDSToStep_MakeGeometricCurveSet.hxx>
#include <TopoDSToStep_MakeManifoldSolidBrep.hxx>
#include <TopoDSToStep_MakeShellBasedSurfaceModel.hxx>
#include <TopoDSToStep_MakeStepVertex.hxx>
#include <TopoDSToStep_Tool.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_Finder.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <TransferBRep.hxx>
#include <TransferBRep_ShapeMapper.hxx>
#include <UnitsMethods.hxx>
#include <XSAlgo.hxx>
#include <XSAlgo_ShapeProcessor.hxx>

IMPLEMENT_STANDARD_RTTIEXT(STEPControl_ActorWrite, Transfer_ActorOfFinderProcess)

//  Transfer
//  TransferShape
//: d6
// Non-manifold topology processing (ssv; 10.11.2010)
// ============================================================================
// Function: DumpWhatIs
// Purpose: Use it in debug mode to dump your shapes
// ============================================================================
#ifdef OCCT_DEBUG
static void DumpWhatIs(const TopoDS_Shape& S)
{

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMapOfShape;
  aMapOfShape.Add(S);
  NCollection_List<TopoDS_Shape> aListOfShape;
  aListOfShape.Append(S);
  NCollection_List<TopoDS_Shape>::Iterator itL(aListOfShape);
  int nbSolids = 0, nbShells = 0, nbOpenShells = 0, nbFaces = 0, nbWires = 0, nbEdges = 0,
      nbVertexes = 0;

  for (; itL.More(); itL.Next())
  {
    TopoDS_Iterator it(itL.Value());
    for (; it.More(); it.Next())
    {
      TopoDS_Shape aSubShape = it.Value();
      if (!aMapOfShape.Add(aSubShape))
        continue;
      aListOfShape.Append(aSubShape);
      if (aSubShape.ShapeType() == TopAbs_SOLID)
        nbSolids++;
      if (aSubShape.ShapeType() == TopAbs_SHELL)
      {
        if (!aSubShape.Closed())
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

  std::cout << "//What is?// NB SOLIDS: " << nbSolids << std::endl;
  std::cout << "//What is?// NB SHELLS: " << nbShells << std::endl;
  std::cout << "//What is?//    OPEN SHELLS: " << nbOpenShells << std::endl;
  std::cout << "//What is?//    CLOSED SHELLS: " << nbShells - nbOpenShells << std::endl;
  std::cout << "//What is?// NB FACES: " << nbFaces << std::endl;
  std::cout << "//What is?// NB WIRES: " << nbWires << std::endl;
  std::cout << "//What is?// NB EDGES: " << nbEdges << std::endl;
  std::cout << "//What is?// NB VERTEXES: " << nbVertexes << std::endl;
}
#endif

static bool hasGeometry(const TopoDS_Shape& theShape)
{
  TopAbs_ShapeEnum aType = theShape.ShapeType();

  if (aType == TopAbs_VERTEX)
  {
    return true;
  }
  else if (aType == TopAbs_EDGE)
  {
    occ::handle<BRep_TEdge> TE = occ::down_cast<BRep_TEdge>(theShape.TShape());
    NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itrc(TE->Curves());

    while (itrc.More())
    {
      const occ::handle<BRep_CurveRepresentation>& CR = itrc.Value();
      bool aHasGeometry = (CR->IsCurve3D() && !CR->Curve3D().IsNull()) || CR->IsCurveOnSurface()
                          || CR->IsRegularity() || (CR->IsPolygon3D() && !CR->Polygon3D().IsNull())
                          || CR->IsPolygonOnTriangulation() || CR->IsPolygonOnSurface();
      if (!aHasGeometry)
        return false;
      itrc.Next();
    }
    return true;
  }
  else if (aType == TopAbs_FACE)
  {
    occ::handle<BRep_TFace> TF = occ::down_cast<BRep_TFace>(theShape.TShape());
    if (!TF->Surface().IsNull())
    {
      return true;
    }
  }
  else
  {
    TopoDS_Iterator anIt(theShape, false, false);
    for (; anIt.More(); anIt.Next())
    {
      const TopoDS_Shape& aShape       = anIt.Value();
      bool                aHasGeometry = hasGeometry(aShape);
      if (!aHasGeometry)
        return false;
    }
    return true;
  }

  return false;
}

//=======================================================================
// Function : IsManifoldShape
// Purpose  : Used to define whether the passed shape has manifold
//            topology or not
//=======================================================================

static bool IsManifoldShape(const TopoDS_Shape& theShape)
{

  bool aResult = true;

  // Do not check nested Compounds
  TopoDS_Compound aDirectShapes;
  BRep_Builder    aBrepBuilder;
  aBrepBuilder.MakeCompound(aDirectShapes);

  TopoDS_Iterator anIt(theShape);
  for (; anIt.More(); anIt.Next())
  {
    const TopoDS_Shape& aDirectChild = anIt.Value();
    if (aDirectChild.ShapeType() != TopAbs_COMPOUND)
      aBrepBuilder.Add(aDirectShapes, aDirectChild);
  }

#ifdef OCCT_DEBUG
  DumpWhatIs(aDirectShapes);
#endif

  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aMapEdgeFaces;
  TopExp::MapShapesAndAncestors(aDirectShapes, TopAbs_EDGE, TopAbs_FACE, aMapEdgeFaces);

  int aNbEdges = aMapEdgeFaces.Extent();
#ifdef OCCT_DEBUG
  std::cout << "Checking whether the topology passed is manifold..." << std::endl;
#endif

  // Check topology
  for (int i = 1; i <= aNbEdges; i++)
  {
    TopoDS_Edge aCurrentEdge = TopoDS::Edge(aMapEdgeFaces.FindKey(i));
    if (!BRep_Tool::Degenerated(aCurrentEdge))
    {
      int aNbAncestors = aMapEdgeFaces.FindFromIndex(i).Extent();
      if (aNbAncestors > 2)
      {
        aResult = false;
        break;
      }
    }
  }

#ifdef OCCT_DEBUG
  std::cout << "Check result: " << (aResult ? "TRUE" : "FALSE") << std::endl;
#endif

  return aResult;
}

//=================================================================================================

STEPControl_ActorWrite::STEPControl_ActorWrite()
    : mygroup(0),
      mytoler(-1.)
{
  SetMode(STEPControl_ShellBasedSurfaceModel);
}

//=======================================================================
// method: getNMSSRForGroup
// purpose: allows to get NMSSR (NON_MANIFOLD_SURFACE_SHAPE_REPRESENTATION)
//         STEP's entity for the group of shells (!) passed
//=======================================================================

occ::handle<StepShape_NonManifoldSurfaceShapeRepresentation> STEPControl_ActorWrite::
  getNMSSRForGroup(const occ::handle<NCollection_HSequence<TopoDS_Shape>>& shapeGroup,
                   const occ::handle<Transfer_FinderProcess>&              FP,
                   bool&                                                   isNMSSRCreated) const
{
  occ::handle<StepShape_NonManifoldSurfaceShapeRepresentation> aResult;

  if (!shapeGroup.IsNull())
  {
    for (int i = 1; i <= shapeGroup->Length(); i++)
    {
      TopoDS_Shape                          aCurrentShape = shapeGroup->Value(i);
      occ::handle<TransferBRep_ShapeMapper> mapper = TransferBRep::ShapeMapper(FP, aCurrentShape);
      if (FP->FindTypedTransient(mapper,
                                 STANDARD_TYPE(StepShape_NonManifoldSurfaceShapeRepresentation),
                                 aResult))
        break;
    }
  }

  if (aResult.IsNull())
  {
#ifdef OCCT_DEBUG
    std::cout << "\nNew NMSSR created" << std::endl;
#endif
    aResult        = new StepShape_NonManifoldSurfaceShapeRepresentation;
    isNMSSRCreated = true;
  }
  else
  {
#ifdef OCCT_DEBUG
    std::cout << "\nExisting NMSSR is used" << std::endl;
#endif
    isNMSSRCreated = false;
  }

  return aResult;
}

//=======================================================================
// function : mergeInfoForNM
// purpose  : bind already written shared faces to STEP entity for non-manifold
//=======================================================================
void STEPControl_ActorWrite::mergeInfoForNM(const occ::handle<Transfer_FinderProcess>& theFP,
                                            const occ::handle<Standard_Transient>& theInfo) const
{
  occ::handle<ShapeProcess_ShapeContext> aContext =
    occ::down_cast<ShapeProcess_ShapeContext>(theInfo);
  if (aContext.IsNull())
    return;

  const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& aMap =
    aContext->Map();
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator aShapeShapeIt(
    aMap);

  for (; aShapeShapeIt.More(); aShapeShapeIt.Next())
  {
    TopoDS_Shape anOrig = aShapeShapeIt.Key(), aRes = aShapeShapeIt.Value();
    if (anOrig.ShapeType() != TopAbs_FACE)
      continue;

    occ::handle<TransferBRep_ShapeMapper> anOrigMapper = TransferBRep::ShapeMapper(theFP, anOrig);
    occ::handle<Transfer_Binder>          anOrigBinder = theFP->Find(anOrigMapper);
    if (anOrigBinder.IsNull())
      continue;

    occ::handle<TransferBRep_ShapeMapper> aResMapper = TransferBRep::ShapeMapper(theFP, aRes);
    theFP->Bind(aResMapper, anOrigBinder);
  }
}

//=================================================================================================

bool STEPControl_ActorWrite::separateShapeToSoloVertex(
  const TopoDS_Shape&                 theShape,
  NCollection_Sequence<TopoDS_Shape>& theVertices)
{
  if (theShape.IsNull())
  {
    return false;
  }
  switch (theShape.ShapeType())
  {
    case TopAbs_COMPOUND: {
      for (TopoDS_Iterator anIter(theShape); anIter.More(); anIter.Next())
      {
        if (!separateShapeToSoloVertex(anIter.Value(), theVertices))
        {
          return false;
        }
      }
      break;
    }
    case TopAbs_VERTEX: {
      theVertices.Append(theShape);
      break;
    }
    default: {
      theVertices.Clear();
      return false;
    }
  }
  return true;
}

//=================================================================================================

void STEPControl_ActorWrite::SetMode(const STEPControl_StepModelType M)
{
  switch (M)
  {
    case STEPControl_AsIs:
      ModeTrans() = 0;
      break;
    case STEPControl_ManifoldSolidBrep:
      ModeTrans() = 3;
      break;
    case STEPControl_BrepWithVoids:
      ModeTrans() = 5;
      break;
    case STEPControl_FacetedBrep:
      ModeTrans() = 1;
      break;
    case STEPControl_FacetedBrepAndBrepWithVoids:
      ModeTrans() = 6;
      break;
    case STEPControl_ShellBasedSurfaceModel:
      ModeTrans() = 2;
      break;
    case STEPControl_GeometricCurveSet:
      ModeTrans() = 4;
      break;
    case STEPControl_Hybrid:
      ModeTrans() = 0;
      break; // PAS IMPLEMENTE !!
    default:
      break;
  }
}

//=================================================================================================

STEPControl_StepModelType STEPControl_ActorWrite::Mode() const
{
  switch (themodetrans)
  {
    case 0:
      return STEPControl_AsIs;
    case 1:
      return STEPControl_FacetedBrep;
    case 2:
      return STEPControl_ShellBasedSurfaceModel;
    case 3:
      return STEPControl_ManifoldSolidBrep;
    case 4:
      return STEPControl_GeometricCurveSet;
    case 5:
      return STEPControl_BrepWithVoids;
    case 6:
      return STEPControl_FacetedBrepAndBrepWithVoids;
    default:
      break;
  }
  return STEPControl_AsIs;
}

//=================================================================================================

void STEPControl_ActorWrite::SetGroupMode(const int mode)
{
  if (mode >= 0)
    mygroup = mode;
}

//=================================================================================================

int STEPControl_ActorWrite::GroupMode() const
{
  return mygroup;
}

//=================================================================================================

void STEPControl_ActorWrite::SetTolerance(const double Tol)
{
  mytoler = Tol;
}

//=======================================================================
// function : Recognize
//  ATTENTION, Recognize doit s aligner sur ce que Transfer sait faire
// purpose  :
//=======================================================================

bool STEPControl_ActorWrite::Recognize(const occ::handle<Transfer_Finder>& start)
{
  STEPControl_StepModelType             mymode = Mode();
  occ::handle<TransferBRep_ShapeMapper> mapper = occ::down_cast<TransferBRep_ShapeMapper>(start);
  if (mapper.IsNull())
    return false;
  if (mymode == STEPControl_AsIs)
    return true;

  bool yasolid = false, yashell = false, yaface = false;

  TopoDS_Shape theShape, aShape;
  //  theShape = TopoDSToStep::DirectFaces(mapper->Value());
  theShape = mapper->Value(); // pour une reconnaissance c est bien assez

  if (theShape.ShapeType() == TopAbs_COMPOUND)
  {

    TopExp_Explorer SolidExp, ShellExp, FaceExp;

    for (SolidExp.Init(theShape, TopAbs_SOLID); SolidExp.More(); SolidExp.Next())
      yasolid = true;
    for (ShellExp.Init(theShape, TopAbs_SHELL, TopAbs_SOLID); ShellExp.More(); ShellExp.Next())
      yashell = true;
    for (FaceExp.Init(theShape, TopAbs_FACE, TopAbs_SHELL); FaceExp.More(); FaceExp.Next())
      yaface = true;
  }
  else if (theShape.ShapeType() == TopAbs_SOLID)
    yasolid = true;
  else if (theShape.ShapeType() == TopAbs_SHELL)
    yashell = true;
  else if (theShape.ShapeType() == TopAbs_FACE)
    yaface = true;
  else if (mymode != STEPControl_GeometricCurveSet)
    return false;
  //  pour wireframe ?

  //  Faceted : il est OBLIGATOIRE d avoir des surfaces support Plane et des
  //   courbes 3D Line (pcurves ignorees)

  if (mymode == STEPControl_FacetedBrep || mymode == STEPControl_FacetedBrepAndBrepWithVoids)
  {
    for (TopExp_Explorer ffac(theShape, TopAbs_FACE); ffac.More(); ffac.Next())
    {
      const TopoDS_Face&        F = TopoDS::Face(ffac.Current());
      TopLoc_Location           locbid;
      occ::handle<Geom_Surface> surf = BRep_Tool::Surface(F, locbid);
      if (surf.IsNull() || !surf->IsKind(STANDARD_TYPE(Geom_Plane)))
        return false;
    }
    for (TopExp_Explorer fedg(theShape, TopAbs_EDGE); fedg.More(); fedg.Next())
    {
      const TopoDS_Edge&      E = TopoDS::Edge(fedg.Current());
      TopLoc_Location         locbid;
      double                  first, last;
      occ::handle<Geom_Curve> curv = BRep_Tool::Curve(E, locbid, first, last);
      if (curv.IsNull() || !curv->IsKind(STANDARD_TYPE(Geom_Line)))
        return false;
    }
  }

  switch (mymode)
  {
    case STEPControl_ManifoldSolidBrep:
      return (yasolid || yashell);
    case STEPControl_BrepWithVoids:
    case STEPControl_FacetedBrep:
    case STEPControl_FacetedBrepAndBrepWithVoids:
      return yasolid;
    case STEPControl_ShellBasedSurfaceModel:
      return (yasolid || yashell || yaface);
    case STEPControl_GeometricCurveSet:
      return true; // tout OK
    default:
      break;
  }
  return false;
}

//  ########    MAKE PRODUCT DATA + CONTEXT    ########

//=================================================================================================

occ::handle<Transfer_Binder> STEPControl_ActorWrite::Transfer(
  const occ::handle<Transfer_Finder>&        start,
  const occ::handle<Transfer_FinderProcess>& FP,
  const Message_ProgressRange&               theProgress)
{
  occ::handle<TransferBRep_ShapeMapper> mapper = occ::down_cast<TransferBRep_ShapeMapper>(start);

  if (mapper.IsNull())
    return NullResult();
  TopoDS_Shape shape = mapper->Value();

  // init context
  occ::handle<StepData_StepModel> model = occ::down_cast<StepData_StepModel>(FP->Model());
  if (!model.IsNull())
    myContext.SetModel(model); //: abv 04.11.00: take APD from model
  myContext.AddAPD(false);     // update APD
  myContext.SetLevel(1);       // set assembly level to 1 (to ensure)
  if (!model->IsInitializedUnit())
  {
    XSAlgo_ShapeProcessor::PrepareForTransfer(); // update unit info
    model->SetLocalLengthUnit(UnitsMethods::GetCasCadeLengthUnit());
  }
  double aLFactor = model->WriteLengthUnit();
  aLFactor /= model->LocalLengthUnit();
  const int        anglemode = model->InternalParameters.AngleUnit;
  StepData_Factors aLocalFactors;
  aLocalFactors.InitializeFactors(aLFactor, (anglemode <= 1 ? 1. : M_PI / 180.), 1.);
  // create SDR
  STEPConstruct_Part SDRTool;
  SDRTool.MakeSDR(0, myContext.GetProductName(), myContext.GetAPD()->Application(), model);
  occ::handle<StepShape_ShapeDefinitionRepresentation> sdr = SDRTool.SDRValue();
  // transfer shape

  occ::handle<Transfer_Binder> resbind =
    TransferShape(mapper, sdr, FP, aLocalFactors, 0L, true, theProgress);

  //  occ::handle<StepShape_ShapeRepresentation> resultat;
  //  FP->GetTypedTransient (resbind,STANDARD_TYPE(StepShape_ShapeRepresentation),resultat);
  //  sdr->SetUsedRepresentation (resultat);

  // create binder with all root entities produced from shape
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> roots =
    myContext.GetRootsForPart(SDRTool);
  occ::handle<Transfer_Binder> resprod = TransientResult(myContext.GetAPD());
  for (int i = 1; i <= roots->Length(); i++)
    resprod->AddResult(TransientResult(roots->Value(i)));
  resprod->AddResult(resbind);

  // bind and exit
  // FP->Bind (mapper,resprod);
  myContext.NextIndex();
  return resprod;
}

//==========================================

static double UsedTolerance(occ::handle<StepData_StepModel>& theStepModel,
                            const double                     mytoler,
                            const TopoDS_Shape&              theShape)
{

  //    COMPUTING 3D TOLERANCE
  //    Either from Session, or Computed (Least,Average, or Greatest)
  //    Then given to TopoDSToStep_Tool
  double Tol    = mytoler;
  int    tolmod = theStepModel->InternalParameters.WritePrecisionMode;
  if (Tol <= 0 && tolmod == 2)
    Tol = theStepModel->InternalParameters.WritePrecisionVal;
  if (Tol <= 0)
  {
    ShapeAnalysis_ShapeTolerance stu;
    Tol = stu.Tolerance(theShape, tolmod);
    //  Par defaut, on prend une tolerance moyenne, on elimine les aberrations
    Tol = Interface_MSG::Intervalled(Tol * 1.5); // arrondi a 1 2 5 ...
  }
  if (Tol == 0)
    Tol = 1.e-07; // minimum ...

  return Tol;
}

//=================================================================================================

// if GroupMode is >1 downgrades all compounds having single subshape to that
// subshape

bool STEPControl_ActorWrite::IsAssembly(const occ::handle<StepData_StepModel>& theModel,
                                        TopoDS_Shape&                          S) const
{
  if (!GroupMode() || S.ShapeType() != TopAbs_COMPOUND)
    return false;
  // PTV 16.09.2002  OCC725 for storing compound of vertices
  if (theModel->InternalParameters.WriteVertexMode == 0)
  { // bug 23950
    if (S.ShapeType() == TopAbs_COMPOUND)
    {
      bool            IsOnlyVertices = true;
      TopoDS_Iterator anItr(S);
      for (; anItr.More(); anItr.Next())
      {
        if (anItr.Value().ShapeType() != TopAbs_VERTEX)
        {
          IsOnlyVertices = false;
          break;
        }
      }
      if (IsOnlyVertices)
        return false;
    }
  }
  if (GroupMode() == 1)
    return true;
  TopoDS_Iterator it(S);
  if (!it.More())
    return false;
  TopoDS_Shape shape = it.Value();
  it.Next();
  if (it.More())
    return true;
  S = shape;
  return IsAssembly(theModel, S);
}

// PTV 16.09.2002 added for transferring vertices.
static bool transferVertex(const occ::handle<Transfer_FinderProcess>&                      FP,
                           occ::handle<NCollection_HArray1<StepShape_GeometricSetSelect>>& aGSS,
                           const TopoDS_Shape&                                             aShVrtx,
                           const int                                                       theNum,
                           const StepData_Factors& theLocalFactors)
{
  bool IsDone = false;
  NCollection_DataMap<TopoDS_Shape, occ::handle<Standard_Transient>, TopTools_ShapeMapHasher> aMap;
  occ::handle<StepData_StepModel> aStepModel = occ::down_cast<StepData_StepModel>(FP->Model());
  TopoDSToStep_Tool           aTool(aMap, true, aStepModel->InternalParameters.WriteSurfaceCurMode);
  TopoDSToStep_MakeStepVertex aMkVrtx(TopoDS::Vertex(aShVrtx), aTool, FP, theLocalFactors);

  if (!aMkVrtx.IsDone())
    return IsDone;

  occ::handle<StepShape_VertexPoint> aVP =
    occ::down_cast<StepShape_VertexPoint>(aTool.Find(aShVrtx));
  if (aVP.IsNull())
    return IsDone;

  StepShape_GeometricSetSelect select;
  select.SetValue(aVP->VertexGeometry());
  // add current result
  aGSS->SetValue(theNum, select);
  IsDone = true;
  return IsDone;
}

occ::handle<Transfer_Binder> STEPControl_ActorWrite::TransferShape(
  const occ::handle<Transfer_Finder>&                         start,
  const occ::handle<StepShape_ShapeDefinitionRepresentation>& SDR0,
  const occ::handle<Transfer_FinderProcess>&                  FP,
  const StepData_Factors&                                     theLocalFactors,
  const occ::handle<NCollection_HSequence<TopoDS_Shape>>&     shapeGroup,
  const bool                                                  isManifold,
  const Message_ProgressRange&                                theProgress)
{
  STEPControl_StepModelType             mymode = Mode();
  occ::handle<TransferBRep_ShapeMapper> mapper = occ::down_cast<TransferBRep_ShapeMapper>(start);
  occ::handle<Transfer_Binder>          binder;
  occ::handle<StepData_StepModel> aStepModel = occ::down_cast<StepData_StepModel>(FP->Model());

  // Indicates whether to use an existing NMSSR to write items to (ss; 13.11.2010)
  bool useExistingNMSSR = false;

  if (mapper.IsNull())
    return binder;
  TopoDS_Shape theShape = mapper->Value();

  if (theShape.IsNull())
    return binder;

  // INDIVIDUAL SHAPE ALREADY TRANSFERRED : RETURN IT !
  binder = FP->Find(start);
  if (!binder.IsNull())
  {
    if (!binder->HasResult())
      binder.Nullify();
  }
  if (!binder.IsNull())
  {
    //: abv 20.05.02: writing box & face from it (shared) in one compound
    // as assembly - while face already translated, it should be
    // re-translated to break sharing
#ifdef OCCT_DEBUG
    std::cout << "Warning: STEPControl_ActorWrite::TransferShape(): shape already translated"
              << std::endl;
#endif
    //    return binder;
  }

  // MODE ASSEMBLY : if Compound, (sub-)assembly
  if (IsAssembly(aStepModel, theShape))
    return TransferCompound(start, SDR0, FP, theLocalFactors, theProgress);

  Message_ProgressScope aPSRoot(theProgress, NULL, 2);

  // [BEGIN] Separate manifold topology from non-manifold in group mode 0 (ssv; 18.11.2010)
  bool                         isNMMode = aStepModel->InternalParameters.WriteNonmanifold != 0;
  occ::handle<Transfer_Binder> aNMBinder;
  if (isNMMode && !GroupMode() && theShape.ShapeType() == TopAbs_COMPOUND)
  {
    TopoDS_Compound aNMCompound;
    TopoDS_Compound aManifoldCompound;
    BRep_Builder    brepBuilder;

    // Create empty Compounds
    brepBuilder.MakeCompound(aManifoldCompound);
    brepBuilder.MakeCompound(aNMCompound);

    // Indicates whether there is only non-manifold topology detected
    // (there is no manifold topology found in the Compound passed)
    bool isOnlyNonManifold = false;

    // Find a Compound containing non-manifold topology.
    // NOTE: only one such Compound must exist in the entire Compound passed
    if (!IsManifoldShape(theShape))
    {
      aNMCompound       = TopoDS::Compound(theShape);
      isOnlyNonManifold = true;
    }
    else
    {
      NCollection_List<TopoDS_Shape> aListOfShapes;
      NCollection_List<TopoDS_Shape> aListOfManifoldShapes;
      aListOfShapes.Append(theShape);

      NCollection_List<TopoDS_Shape>::Iterator itL(aListOfShapes);
      for (; itL.More(); itL.Next())
      {
        TopoDS_Shape    aParentShape = itL.Value();
        TopoDS_Iterator it(aParentShape);
        for (; it.More(); it.Next())
        {
          TopoDS_Shape aSubShape = it.Value();
          if (aSubShape.ShapeType() == TopAbs_COMPOUND && !IsManifoldShape(aSubShape))
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
      for (itL.Initialize(aListOfManifoldShapes); itL.More(); itL.Next())
      {
        TopoDS_Shape aCurrentManiShape = itL.Value();
        brepBuilder.Add(aManifoldCompound, aCurrentManiShape);
      }
    }

    // Process only manifold topology in the current TransferShape invocation.
    // Invoke TransferShape for non-manifold topology separately (see below)
    theShape = aManifoldCompound;

    // List of items to transfer
    occ::handle<NCollection_HSequence<TopoDS_Shape>> RepItemSeq =
      new NCollection_HSequence<TopoDS_Shape>();
    // Non-manifold group to pass into TransferShape with each shape from RepItemSeq
    occ::handle<NCollection_HSequence<TopoDS_Shape>> NonManifoldGroup =
      new NCollection_HSequence<TopoDS_Shape>();

    // Transfer Solids to closed Shells. Prepare RepItemSeq & NonManifoldGroup
    for (TopoDS_Iterator iter(aNMCompound); iter.More(); iter.Next())
    {
      const TopoDS_Shape& aSubShape = iter.Value();
      if (aSubShape.ShapeType() == TopAbs_SOLID)
      {
        for (TopoDS_Iterator aSubIter(aSubShape); aSubIter.More(); aSubIter.Next())
        {
          TopoDS_Shell aSubShell = TopoDS::Shell(aSubIter.Value());
          aSubShell.Closed(true);
          RepItemSeq->Append(aSubShell);
          NonManifoldGroup->Append(aSubShell);
        }
      }
      else if (!isManifold && (aSubShape.ShapeType() == TopAbs_SHELL))
      {
        RepItemSeq->Append(aSubShape);
        NonManifoldGroup->Append(aSubShape);
      }
      else
        RepItemSeq->Append(iter.Value());
    }

    int aNMItemsNb = RepItemSeq->Length();

    // In case of pure manifold topology do nothing; theShape is processed as usual (see below)
    if (aNMItemsNb > 0)
    {

      // Prepare SDR for non-manifold group. This SDR will be linked to NMSSR by means
      // of TransferShape invocation. SDR is not created if there is no any manifold
      // topology in the passed Compound. If topology is pure non-manifold, SDR0 (passed)
      // is used
      occ::handle<StepShape_ShapeDefinitionRepresentation> sdr;
      if (isOnlyNonManifold)
        sdr = SDR0;
      else
      {
        STEPConstruct_Part SDRTool;
        SDRTool.MakeSDR(0,
                        myContext.GetProductName(),
                        myContext.GetAPD()->Application(),
                        aStepModel);
        sdr = SDRTool.SDRValue();
      }

      aNMBinder = TransientResult(sdr);

      // Complete SDR with shape representations.
      // NOTE: aNMBinder is connected now with this SDR. It will be added to the resulting
      //       binder in the end of this invocation of TransferShape
      Message_ProgressScope aPS(aPSRoot.Next(), NULL, aNMItemsNb);
      for (int i = 1; i <= aNMItemsNb && aPS.More(); i++)
      {
        occ::handle<TransferBRep_ShapeMapper> aMapper =
          TransferBRep::ShapeMapper(FP, RepItemSeq->Value(i));
        TransferShape(aMapper, sdr, FP, theLocalFactors, NonManifoldGroup, false, aPS.Next());
      }

      // Nothing else needed for pure non-manifold topology, return
      if (isOnlyNonManifold)
        return aNMBinder;
    }
  }
  // [END] Separate manifold topology from non-manifold in group mode 0 (ssv; 18.11.2010)

  if (aPSRoot.UserBreak())
    return occ::handle<Transfer_Binder>();

  // create a list of items to translate
  occ::handle<NCollection_HSequence<TopoDS_Shape>> RepItemSeq =
    new NCollection_HSequence<TopoDS_Shape>();

  bool isSeparateVertices = aStepModel->InternalParameters.WriteVertexMode == 0; // bug 23950
  // PTV 16.09.2002 OCC725 separate shape from solo vertices.
  bool isOnlyVertices = false;
  if (theShape.ShapeType() == TopAbs_COMPOUND && isSeparateVertices)
  {
    TopoDS_Compound aNewShape, aCompOfVrtx;
    BRep_Builder    aBuilder;
    aBuilder.MakeCompound(aNewShape);
    aBuilder.MakeCompound(aCompOfVrtx);
    NCollection_Sequence<TopoDS_Shape> aVertices;
    isOnlyVertices = separateShapeToSoloVertex(theShape, aVertices);
    if (!isOnlyVertices)
    {
      for (TopoDS_Iterator anCompIt(theShape); anCompIt.More(); anCompIt.Next())
      {
        const TopoDS_Shape&                aCurSh = anCompIt.Value();
        NCollection_Sequence<TopoDS_Shape> aVerticesOfSubSh;
        if (separateShapeToSoloVertex(aCurSh, aVerticesOfSubSh))
        {
          aVertices.Append(aVerticesOfSubSh);
        }
        else
        {
          aBuilder.Add(aNewShape, aCurSh);
        }
      }
      theShape = aNewShape;
    }
    for (NCollection_HSequence<TopoDS_Shape>::Iterator anIterV(aVertices); anIterV.More();
         anIterV.Next())
    {
      aBuilder.Add(aCompOfVrtx, anIterV.Value());
    }
    if (!aVertices.IsEmpty())
    {
      RepItemSeq->Append(aCompOfVrtx);
    }
  }

  if (theShape.ShapeType() == TopAbs_COMPOUND)
  {
    TopExp_Explorer SolidExp, ShellExp, FaceExp;
    if (mymode != STEPControl_GeometricCurveSet)
    {
      for (SolidExp.Init(theShape, TopAbs_SOLID); SolidExp.More(); SolidExp.Next())
      {
        RepItemSeq->Append(TopoDS::Solid(SolidExp.Current()));
      }
      for (ShellExp.Init(theShape, TopAbs_SHELL, TopAbs_SOLID); ShellExp.More(); ShellExp.Next())
      {
        RepItemSeq->Append(TopoDS::Shell(ShellExp.Current()));
      }

      for (FaceExp.Init(theShape, TopAbs_FACE, TopAbs_SHELL); FaceExp.More(); FaceExp.Next())
      {
        RepItemSeq->Append(TopoDS::Face(FaceExp.Current()));
      }
    }
    else
    {
      if (!isOnlyVertices)
        RepItemSeq->Append(theShape); //: j1
    }
    if (mymode == STEPControl_AsIs)
    {
      TopExp_Explorer WireExp, EdgeExp;
      for (WireExp.Init(theShape, TopAbs_WIRE, TopAbs_FACE); WireExp.More(); WireExp.Next())
        RepItemSeq->Append(TopoDS::Wire(WireExp.Current()));
      for (EdgeExp.Init(theShape, TopAbs_EDGE, TopAbs_WIRE); EdgeExp.More(); EdgeExp.Next())
        RepItemSeq->Append(TopoDS::Edge(EdgeExp.Current()));
    }
  }
  else if (theShape.ShapeType() == TopAbs_SOLID)
  {
    RepItemSeq->Append(TopoDS::Solid(theShape));
  }
  else if (theShape.ShapeType() == TopAbs_SHELL)
  {
    RepItemSeq->Append(TopoDS::Shell(theShape));
  }
  else if (theShape.ShapeType() == TopAbs_FACE)
  {
    RepItemSeq->Append(TopoDS::Face(theShape));
  }
  else if (theShape.ShapeType() == TopAbs_COMPSOLID)
  {
    FP->AddWarning(start, "NonManifold COMPSOLID was translated like a set of SOLIDs");
    if (GroupMode() > 0)
      return TransferCompound(start, SDR0, FP, theLocalFactors, aPSRoot.Next());
    else
    {
      TopExp_Explorer SolidExp;
      for (SolidExp.Init(theShape, TopAbs_SOLID); SolidExp.More(); SolidExp.Next())
      {
        RepItemSeq->Append(TopoDS::Solid(SolidExp.Current()));
      }
    }
  }

  else if (mymode != STEPControl_GeometricCurveSet && mymode != STEPControl_AsIs)
  {
    FP->AddFail(start, "The Shape is not a SOLID, nor a SHELL, nor a FACE");
    return binder;
  }
  else
    RepItemSeq->Append(theShape);

  //    COMPUTING 3D TOLERANCE
  //    Either from Session, or Computed (Least,Average, or Greatest)
  //    Then given to TopoDSToStep_Tool
  double Tol = UsedTolerance(aStepModel, mytoler, theShape);

  // Create a STEP-Entity for each TopoDS_Shape
  // according to the current StepModelMode

  int                                                                 nbs = RepItemSeq->Length();
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> ItemSeq =
    new NCollection_HSequence<occ::handle<Standard_Transient>>();

  // ptv 10.11.00: allow to write empty Compound:  if (GroupMode() >0)
  ItemSeq->Append(myContext.GetDefaultAxis());
  STEPControl_StepModelType trmode = mymode;
  Message_ProgressScope     aPS(aPSRoot.Next(), NULL, nbs);
  for (int i = 1; i <= nbs && aPS.More(); i++)
  {
    TopoDS_Shape xShape = RepItemSeq->Value(i);

    if (mymode == STEPControl_AsIs)
    {
      switch (xShape.ShapeType())
      {
        case TopAbs_SOLID:
          trmode = STEPControl_ManifoldSolidBrep;
          break;
        case TopAbs_SHELL:
          trmode = STEPControl_ShellBasedSurfaceModel;
          break;
        case TopAbs_FACE:
          trmode = STEPControl_ShellBasedSurfaceModel;
          break;
        default:
          trmode = STEPControl_GeometricCurveSet;
          break;
      }
    }

    Message_ProgressScope aPS1(aPS.Next(), NULL, 2);

    TopoDS_Shape aShape = xShape;

    XSAlgo_ShapeProcessor::ParameterMap aParameters = GetShapeFixParameters();
    XSAlgo_ShapeProcessor::SetParameter("FixShape.Tolerance3d", Tol, true, aParameters);
    XSAlgo_ShapeProcessor::SetParameter("FixShape.MaxTolerance3d",
                                        aStepModel->InternalParameters.ReadMaxPrecisionVal,
                                        true,
                                        aParameters);
    XSAlgo_ShapeProcessor aShapeProcessor(aParameters);

    if (hasGeometry(aShape))
    {
      aShape = aShapeProcessor.ProcessShape(xShape, GetShapeProcessFlags().first, aPS1.Next());

      if (aPS1.UserBreak())
      {
        return occ::handle<Transfer_Binder>();
      }
    }

    if (!isManifold)
    {
      mergeInfoForNM(FP, aShapeProcessor.GetContext());
    }

    // create a STEP entity corresponding to shape
    occ::handle<StepGeom_GeometricRepresentationItem> item, itemTess;
    switch (trmode)
    {
      case STEPControl_ManifoldSolidBrep: {
        if (aShape.ShapeType() == TopAbs_SOLID)
        {
          TopoDS_Solid aSolid = TopoDS::Solid(aShape);

          //: d6 abv 13 Mar 98: if solid has more than 1 shell,
          // try to treat it as solid with voids
          int nbShells = 0;
          for (TopoDS_Iterator It(aSolid); It.More(); It.Next())
            if (It.Value().ShapeType() == TopAbs_SHELL)
              nbShells++;
          if (nbShells > 1)
          {
            TopoDSToStep_MakeBrepWithVoids MkBRepWithVoids(aSolid,
                                                           FP,
                                                           theLocalFactors,
                                                           aPS1.Next());
            MkBRepWithVoids.Tolerance() = Tol;
            if (MkBRepWithVoids.IsDone())
            {
              item     = MkBRepWithVoids.Value();
              itemTess = MkBRepWithVoids.TessellatedValue();
            }
            else
              nbShells = 1; // smth went wrong; let it will be just Manifold
          }
          if (nbShells == 1)
          {
            TopoDSToStep_MakeManifoldSolidBrep MkManifoldSolidBrep(aSolid,
                                                                   FP,
                                                                   theLocalFactors,
                                                                   aPS1.Next());
            MkManifoldSolidBrep.Tolerance() = Tol;
            if (MkManifoldSolidBrep.IsDone())
            {
              item     = MkManifoldSolidBrep.Value();
              itemTess = MkManifoldSolidBrep.TessellatedValue();
            }
          }
        }
        else if (aShape.ShapeType() == TopAbs_SHELL)
        {
          TopoDS_Shell                       aShell = TopoDS::Shell(aShape);
          TopoDSToStep_MakeManifoldSolidBrep MkManifoldSolidBrep(aShell,
                                                                 FP,
                                                                 theLocalFactors,
                                                                 aPS1.Next());
          MkManifoldSolidBrep.Tolerance() = Tol;
          if (MkManifoldSolidBrep.IsDone())
          {
            item     = MkManifoldSolidBrep.Value();
            itemTess = MkManifoldSolidBrep.TessellatedValue();
          }
        }
        break;
      }
      case STEPControl_BrepWithVoids: {
        if (aShape.ShapeType() == TopAbs_SOLID)
        {
          TopoDS_Solid                   aSolid = TopoDS::Solid(aShape);
          TopoDSToStep_MakeBrepWithVoids MkBRepWithVoids(aSolid, FP, theLocalFactors, aPS1.Next());
          MkBRepWithVoids.Tolerance() = Tol;
          if (MkBRepWithVoids.IsDone())
          {
            item     = MkBRepWithVoids.Value();
            itemTess = MkBRepWithVoids.TessellatedValue();
          }
        }
        break;
      }
      case STEPControl_FacetedBrep: {
        TopoDSToStep_FacetedError facErr = TopoDSToStep_FacetedTool::CheckTopoDSShape(aShape);
        if (facErr != TopoDSToStep_FacetedDone)
        {
          FP->AddFail(start, "Error in Faceted Shape from TopoDS");
          if (facErr == TopoDSToStep_SurfaceNotPlane)
          {
            FP->AddFail(start, "-- The TopoDS_Face is not plane");
          }
          else if (facErr == TopoDSToStep_PCurveNotLinear)
          {
            FP->AddFail(start, "-- The Face contains non linear PCurves");
          }
          return binder;
        }
        if (aShape.ShapeType() == TopAbs_SOLID)
        {
          TopoDS_Solid                 aSolid = TopoDS::Solid(aShape);
          TopoDSToStep_MakeFacetedBrep MkFacetedBrep(aSolid, FP, theLocalFactors, aPS1.Next());
          MkFacetedBrep.Tolerance() = Tol;
          if (MkFacetedBrep.IsDone())
          {
            item     = MkFacetedBrep.Value();
            itemTess = MkFacetedBrep.TessellatedValue();
          }
        }
        break;
      }
      case STEPControl_FacetedBrepAndBrepWithVoids: {
        TopoDSToStep_FacetedError facErr = TopoDSToStep_FacetedTool::CheckTopoDSShape(aShape);
        if (facErr != TopoDSToStep_FacetedDone)
        {
          FP->AddFail(start, "Error in Faceted Shape from TopoDS");
          if (facErr == TopoDSToStep_SurfaceNotPlane)
          {
            FP->AddFail(start, "-- The TopoDS_Face is not plane");
          }
          else if (facErr == TopoDSToStep_PCurveNotLinear)
          {
            FP->AddFail(start, "-- The Face contains non linear PCurves");
          }
          return binder;
        }
        if (aShape.ShapeType() == TopAbs_SOLID)
        {
          TopoDS_Solid                                 aSolid = TopoDS::Solid(aShape);
          TopoDSToStep_MakeFacetedBrepAndBrepWithVoids MkFacetedBrepAndBrepWithVoids(
            aSolid,
            FP,
            theLocalFactors,
            aPS1.Next());
          MkFacetedBrepAndBrepWithVoids.Tolerance() = Tol;
          if (MkFacetedBrepAndBrepWithVoids.IsDone())
          {
            item     = MkFacetedBrepAndBrepWithVoids.Value();
            itemTess = MkFacetedBrepAndBrepWithVoids.TessellatedValue();
          }
        }
        break;
      }
      case STEPControl_ShellBasedSurfaceModel: {
        if (aShape.ShapeType() == TopAbs_SOLID)
        {
          TopoDS_Solid                            aSolid = TopoDS::Solid(aShape);
          TopoDSToStep_MakeShellBasedSurfaceModel MkShellBasedSurfaceModel(aSolid,
                                                                           FP,
                                                                           theLocalFactors,
                                                                           aPS1.Next());
          MkShellBasedSurfaceModel.Tolerance() = Tol;
          if (MkShellBasedSurfaceModel.IsDone())
          {
            item     = MkShellBasedSurfaceModel.Value();
            itemTess = MkShellBasedSurfaceModel.TessellatedValue();
          }
        }
        else if (aShape.ShapeType() == TopAbs_SHELL)
        {
          TopoDS_Shell aShell = TopoDS::Shell(aShape);
          // Non-manifold topology is stored via NMSSR containing series of SBSM (ssv; 13.11.2010)
          TopoDSToStep_MakeShellBasedSurfaceModel MkShellBasedSurfaceModel(aShell,
                                                                           FP,
                                                                           theLocalFactors,
                                                                           aPS1.Next());
          MkShellBasedSurfaceModel.Tolerance() = Tol;
          if (MkShellBasedSurfaceModel.IsDone())
          {
            item     = MkShellBasedSurfaceModel.Value();
            itemTess = MkShellBasedSurfaceModel.TessellatedValue();
          }
        }
        else if (aShape.ShapeType() == TopAbs_FACE)
        {
          TopoDS_Face                             aFace = TopoDS::Face(aShape);
          TopoDSToStep_MakeShellBasedSurfaceModel MkShellBasedSurfaceModel(aFace,
                                                                           FP,
                                                                           theLocalFactors,
                                                                           aPS1.Next());
          MkShellBasedSurfaceModel.Tolerance() = Tol;
          if (MkShellBasedSurfaceModel.IsDone())
          {
            item     = MkShellBasedSurfaceModel.Value();
            itemTess = MkShellBasedSurfaceModel.TessellatedValue();
          }
        }
        break;
      }
      case STEPControl_GeometricCurveSet: {
        TopoDSToStep_MakeGeometricCurveSet MkGeometricCurveSet(aShape, FP, theLocalFactors);
        MkGeometricCurveSet.Tolerance() = Tol;
        if (MkGeometricCurveSet.IsDone())
        {
          item = MkGeometricCurveSet.Value();
        }
        // PTV 22.08.2002 OCC609 ------------------------- begin --------------------
        // modified by PTV 16.09.2002 OCC725
        else if (aShape.ShapeType() == TopAbs_COMPOUND || aShape.ShapeType() == TopAbs_VERTEX)
        {
          // it is compound with solo vertices.
          int             aNbVrtx = 0;
          int             curNb   = 0;
          TopExp_Explorer anExp(aShape, TopAbs_VERTEX);
          for (; anExp.More(); anExp.Next())
          {
            if (anExp.Current().ShapeType() != TopAbs_VERTEX)
              continue;
            aNbVrtx++;
          }
          if (aNbVrtx)
          {
            // create new geometric curve set for all vertices
            occ::handle<NCollection_HArray1<StepShape_GeometricSetSelect>> aGSS =
              new NCollection_HArray1<StepShape_GeometricSetSelect>(1, aNbVrtx);
            occ::handle<TCollection_HAsciiString>    empty  = new TCollection_HAsciiString("");
            occ::handle<StepShape_GeometricCurveSet> aGCSet = new StepShape_GeometricCurveSet;
            aGCSet->SetName(empty);
            // iterates on compound with vertices and traces each vertex
            for (anExp.ReInit(); anExp.More(); anExp.Next())
            {
              const TopoDS_Shape& aVertex = anExp.Current();
              if (aVertex.ShapeType() != TopAbs_VERTEX)
                continue;
              curNb++;
              transferVertex(FP, aGSS, aVertex, curNb, theLocalFactors);
            } // end of iteration on compound with vertices.
            aGCSet->SetElements(aGSS);
            item = aGCSet;
          } // end of check that number of vertices is not null
        }
        // PTV 22.08.2002 OCC609-------------------------  end  --------------------
        break;
      }
      default:
        break;
    }
    if (item.IsNull() && itemTess.IsNull())
      continue;

    // add resulting item to the FP
    if (!item.IsNull())
    {
      ItemSeq->Append(item);
      occ::handle<TransferBRep_ShapeMapper> submapper;
      if (xShape.IsSame(mapper->Value()))
        submapper = occ::down_cast<TransferBRep_ShapeMapper>(start);
      if (submapper.IsNull())
        submapper = TransferBRep::ShapeMapper(FP, xShape);
      occ::handle<Transfer_Binder> subbind = FP->Find(submapper);
      if (subbind.IsNull())
      {
        subbind = TransientResult(item);
        FP->Bind(submapper, subbind);
      }
      else
        subbind->AddResult(TransientResult(item));
    }
    if (!itemTess.IsNull())
    {
      ItemSeq->Append(itemTess);
      occ::handle<TransferBRep_ShapeMapper> submapper;
      if (xShape.IsSame(mapper->Value()))
        submapper = occ::down_cast<TransferBRep_ShapeMapper>(start);
      if (submapper.IsNull())
        submapper = TransferBRep::ShapeMapper(FP, xShape);
      occ::handle<Transfer_Binder> subbind = FP->Find(submapper);
      if (subbind.IsNull())
      {
        subbind = TransientResult(itemTess);
        FP->Bind(submapper, subbind);
      }
      else
        subbind->AddResult(TransientResult(itemTess));
    }

    // If shape is not processed, this function does nothing. Intentionally.
    aShapeProcessor.MergeTransferInfo(FP);
  }

  // - Make Shape Representation
  int nCc1 = ItemSeq->Length();
  if (nCc1 < 1)
  {
    FP->AddFail(start, "The Shape has not the appropriate type");
    return binder;
  }
  occ::handle<StepShape_ShapeRepresentation> shapeRep;
  if (theShape.ShapeType() == TopAbs_SHAPE)
  { // for external references
    shapeRep = new StepShape_ShapeRepresentation;
  }
  else
  {
    switch (mymode)
    {
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
        else
        {
          bool isNewNMSSRCreated;
          shapeRep         = this->getNMSSRForGroup(shapeGroup, FP, isNewNMSSRCreated);
          useExistingNMSSR = !isNewNMSSRCreated;
        }
        break;
      case STEPControl_GeometricCurveSet:
        shapeRep = new StepShape_GeometricallyBoundedWireframeShapeRepresentation;
        break;
      case STEPControl_AsIs: {
        if (nbs == 1)
        {
          if (trmode == STEPControl_ManifoldSolidBrep)
            shapeRep = new StepShape_AdvancedBrepShapeRepresentation;
          else if (trmode == STEPControl_ShellBasedSurfaceModel)
            // Process non-manifold topology separately (ssv; 13.11.2010)
            if (isManifold)
              shapeRep = new StepShape_ManifoldSurfaceShapeRepresentation;
            else
            {
              bool isNewNMSSRCreated;
              shapeRep         = this->getNMSSRForGroup(shapeGroup, FP, isNewNMSSRCreated);
              useExistingNMSSR = !isNewNMSSRCreated;
            }
          else if (trmode == STEPControl_GeometricCurveSet)
            shapeRep = new StepShape_GeometricallyBoundedWireframeShapeRepresentation;
          else if (trmode == STEPControl_FacetedBrep)
            shapeRep = new StepShape_FacetedBrepShapeRepresentation;
        }
        else
          shapeRep = new StepShape_ShapeRepresentation;
      }
      break;
      default:
        break;
    }
  }
  if (shapeRep.IsNull())
  {
    occ::handle<Transfer_Binder> resb;
    return resb;
  }

  occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> items =
    new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, nCc1);

  for (int rep = 1; rep <= nCc1; rep++)
  {
    occ::handle<StepRepr_RepresentationItem> repit =
      GetCasted(StepRepr_RepresentationItem, ItemSeq->Value(rep));
    items->SetValue(rep, repit);
  }
  int ap = aStepModel->InternalParameters.WriteSchema;
  NCollection_Sequence<occ::handle<Transfer_Binder>> aSeqBindRelation;
  if (ap == 3 && nbs > 1)
  {
    int j = 1;
    if (items->Value(j)->IsKind(STANDARD_TYPE(StepGeom_Axis2Placement3d)))
    {
      occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> axis =
        new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, 1);
      axis->SetValue(1, items->Value(j++));
      shapeRep->SetItems(axis);
    }
    for (; j <= items->Length(); j++)
    {

      occ::handle<StepShape_ShapeRepresentation> ShapeRepr1;
      if (items->Value(j)->IsKind(STANDARD_TYPE(StepShape_ManifoldSolidBrep)))
        ShapeRepr1 = new StepShape_AdvancedBrepShapeRepresentation;
      else if (items->Value(j)->IsKind(STANDARD_TYPE(StepShape_ShellBasedSurfaceModel)))
        ShapeRepr1 = new StepShape_ManifoldSurfaceShapeRepresentation;
      else if (items->Value(j)->IsKind(STANDARD_TYPE(StepShape_GeometricCurveSet)))
        ShapeRepr1 = new StepShape_GeometricallyBoundedWireframeShapeRepresentation;
      else if (items->Value(j)->IsKind(STANDARD_TYPE(StepShape_FacetedBrep)))
        ShapeRepr1 = new StepShape_FacetedBrepShapeRepresentation;
      else if (items->Value(j)->IsKind(STANDARD_TYPE(StepVisual_TessellatedItem)))
        ShapeRepr1 = new StepVisual_TessellatedShapeRepresentation;
      else
        ShapeRepr1 = new StepShape_ShapeRepresentation;

      occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> repr1 =
        new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, 2);
      repr1->SetValue(1, myContext.GetDefaultAxis());
      repr1->SetValue(2, items->Value(j));
      ShapeRepr1->SetItems(repr1);
      STEPConstruct_UnitContext mk1;
      mk1.Init(Tol, aStepModel, theLocalFactors);
      ShapeRepr1->SetContextOfItems(mk1.Value()); // la tolerance, voir au debut
      ShapeRepr1->SetName(new TCollection_HAsciiString(""));

      occ::handle<StepRepr_ShapeRepresentationRelationship> aShapeRel =
        new StepRepr_ShapeRepresentationRelationship;
      occ::handle<TCollection_HAsciiString> aName  = new TCollection_HAsciiString("");
      occ::handle<TCollection_HAsciiString> aDescr = new TCollection_HAsciiString("");
      aShapeRel->SetName(aName);
      aShapeRel->SetDescription(aDescr);
      aShapeRel->SetRep2(shapeRep);
      aShapeRel->SetRep1(ShapeRepr1);

      aSeqBindRelation.Append(TransientResult(aShapeRel));
    }
  }
  else
  {
    int nC = 0;
    for (int i = 1; i <= items->Length(); i++)
    {
      if (!items->Value(i)->IsKind(STANDARD_TYPE(StepVisual_TessellatedItem)))
        continue;
      ++nC;
    }
    if (nC > 0)
    {
      occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> itemsTess =
        new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, nC);
      int i = 1;
      for (int j = 1; j <= items->Length(); j++)
      {
        if (!items->Value(j)->IsKind(STANDARD_TYPE(StepVisual_TessellatedItem)))
          continue;
        itemsTess->SetValue(i++, items->Value(j));
      }

      occ::handle<StepShape_ShapeRepresentation> shapeTessRepr =
        new StepVisual_TessellatedShapeRepresentation;
      shapeTessRepr->SetItems(itemsTess);
      STEPConstruct_UnitContext mk1;
      mk1.Init(Tol, aStepModel, theLocalFactors);
      shapeTessRepr->SetContextOfItems(mk1.Value());
      shapeTessRepr->SetName(new TCollection_HAsciiString(""));

      aSeqBindRelation.Append(TransientResult(shapeTessRepr));
    }
    if (!useExistingNMSSR)
      shapeRep->SetItems(items);
    else
    {
      // Add new representation item to the NMSSR's existing collection (ssv; 13.11.2010)
      occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> oldItems =
        shapeRep->Items();
      occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> newItems =
        new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1,
                                                                          oldItems->Length() + 1);
      int el = 1;
      for (int i = 1; i <= oldItems->Length(); i++)
        newItems->SetValue(el++, oldItems->Value(i));
      newItems->SetValue(el, items->Value(items->Length()));
      shapeRep->SetItems(newItems);
    }
  }

  // init representation
  STEPConstruct_UnitContext mk;
  mk.Init(Tol, aStepModel, theLocalFactors);
  shapeRep->SetContextOfItems(mk.Value()); // la tolerance, voir au debut
  shapeRep->SetName(new TCollection_HAsciiString(""));

  // Create SDR (only once for non-manifold group)
  if (!useExistingNMSSR)
  {
    SDR0->SetUsedRepresentation(shapeRep);
    // create binder for SR and attach to it binder for RepItem (if exists)
    occ::handle<Transfer_Binder> resbind = TransientResult(shapeRep);
    binder                               = FP->Find(start);
    if (!binder.IsNull())
    {
      resbind->AddResult(binder);
      FP->Rebind(start, resbind);
      // binder->AddResult ( resbind );
      // resbind = binder;
    }
    for (int k = 1; k <= aSeqBindRelation.Length(); k++)
      resbind->AddResult(aSeqBindRelation.Value(k));

    // Add SDR for non-manifold topology in group mode 0 (ssv; 18.11.2010)
    if (!aNMBinder.IsNull())
      resbind->AddResult(aNMBinder);

    return resbind;
  }
  else
    return FP->Find(start);
}

//=======================================================================
// function : TransferCompound
//    ####    TRANSFER COMPOUND AS (SUB-)ASSEMBLY
// purpose  :
//=======================================================================

occ::handle<Transfer_Binder> STEPControl_ActorWrite::TransferCompound(
  const occ::handle<Transfer_Finder>&                         start,
  const occ::handle<StepShape_ShapeDefinitionRepresentation>& SDR0,
  const occ::handle<Transfer_FinderProcess>&                  FP,
  const StepData_Factors&                                     theLocalFactors,
  const Message_ProgressRange&                                theProgress)
{
  occ::handle<TransferBRep_ShapeMapper> mapper = occ::down_cast<TransferBRep_ShapeMapper>(start);
  occ::handle<Transfer_Binder>          binder;
  if (mapper.IsNull())
    return binder;
  TopoDS_Shape theShape = mapper->Value();

  occ::handle<StepData_StepModel> aStepModel = occ::down_cast<StepData_StepModel>(FP->Model());

  // Inspect non-manifold topology case (ssv; 10.11.2010)
  bool isNMMode = aStepModel->InternalParameters.WriteNonmanifold != 0;
  bool isManifold;
  if (isNMMode)
    isManifold = IsManifoldShape(theShape);
  else
    isManifold = true;

  // get a sequence of components (subshapes)
  occ::handle<NCollection_HSequence<TopoDS_Shape>> RepItemSeq =
    new NCollection_HSequence<TopoDS_Shape>();
  // Prepare a collection for non-manifold group of shapes
  occ::handle<NCollection_HSequence<TopoDS_Shape>> NonManifoldGroup =
    new NCollection_HSequence<TopoDS_Shape>();
  bool isSeparateVertices = (aStepModel->InternalParameters.WriteVertexMode == 0); // bug 23950
  // PTV OCC725 17.09.2002 -- begin --
  int             nbFreeVrtx = 0;
  TopoDS_Compound aCompOfVrtx;
  BRep_Builder    aB;
  aB.MakeCompound(aCompOfVrtx);

#ifdef OCCT_DEBUG
  if (!isManifold)
    std::cout << "Exploding Solids to Shells if any..." << std::endl;
#endif

  for (TopoDS_Iterator iter(theShape); iter.More(); iter.Next())
  {
    const TopoDS_Shape& aSubShape = iter.Value();
    if (aSubShape.ShapeType() != TopAbs_VERTEX || !isSeparateVertices)
    {

      // Store non-manifold topology as shells (ssv; 10.11.2010)
      if (!isManifold && aSubShape.ShapeType() == TopAbs_SOLID)
      {
        for (TopoDS_Iterator aSubIter(aSubShape); aSubIter.More(); aSubIter.Next())
        {
          TopoDS_Shell aSubShell = TopoDS::Shell(aSubIter.Value());
          aSubShell.Closed(true);
          RepItemSeq->Append(aSubShell);
          NonManifoldGroup->Append(aSubShell);
        }
      }
      else if (!isManifold
               && (aSubShape.ShapeType() == TopAbs_SHELL || aSubShape.ShapeType() == TopAbs_FACE))
      {
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
    RepItemSeq->Append(aCompOfVrtx);

  // PTV OCC725 17.09.2002 -- end --

  // Constitution : liste d axes, le premier est l origine, les suivants : 1
  // par sous-item
  occ::handle<StepShape_ShapeRepresentation> shapeRep =
    occ::down_cast<StepShape_ShapeRepresentation>(SDR0->UsedRepresentation());
  if (shapeRep.IsNull())
  {
    shapeRep = new StepShape_ShapeRepresentation;
    SDR0->SetUsedRepresentation(shapeRep); // to be used by MakeItem
  }
  // clang-format off
  binder = TransientResult(SDR0); // set SDR as first item in order to be found first (but not SDR of subshape!)
  // clang-format on
  binder->AddResult(TransientResult(shapeRep));

  // translate components
  int                                                                 i, nbs = RepItemSeq->Length();
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> ItemSeq =
    new NCollection_HSequence<occ::handle<Standard_Transient>>();
  ItemSeq->Append(myContext.GetDefaultAxis());
  myContext.NextLevel();
  Message_ProgressScope aPS(theProgress, NULL, nbs);
  for (i = 1; i <= nbs && aPS.More(); i++)
  {
    occ::handle<TransferBRep_ShapeMapper> subs =
      TransferBRep::ShapeMapper(FP, RepItemSeq->Value(i));
    occ::handle<StepGeom_GeometricRepresentationItem> AX1;

    occ::handle<Transfer_Binder> bnd = TransferSubShape(subs,
                                                        SDR0,
                                                        AX1,
                                                        FP,
                                                        theLocalFactors,
                                                        NonManifoldGroup,
                                                        isManifold,
                                                        aPS.Next());

    if (!AX1.IsNull())
      ItemSeq->Append(AX1);
    // copy binders so as to have all roots in upper binder, but do not conflict
    while (!bnd.IsNull())
    {
      occ::handle<Transfer_SimpleBinderOfTransient> bx =
        occ::down_cast<Transfer_SimpleBinderOfTransient>(bnd);
      if (!bx.IsNull())
      {
        binder->AddResult(TransientResult(bx->Result()));
      }
      bnd = bnd->NextResult();
    }
  }
  myContext.PrevLevel();

  int nsub = ItemSeq->Length();
  occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> items =
    new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, nsub);

  // initialize representation
  for (int rep = 1; rep <= nsub; rep++)
    items->SetValue(rep, GetCasted(StepRepr_RepresentationItem, ItemSeq->Value(rep)));
  shapeRep->SetItems(items);
  double                    Tol = UsedTolerance(aStepModel, mytoler, theShape);
  STEPConstruct_UnitContext mk;
  mk.Init(Tol, aStepModel, theLocalFactors);
  shapeRep->SetContextOfItems(mk.Value()); // la tolerance, voir au debut
  shapeRep->SetName(new TCollection_HAsciiString(""));

  // set it to SDR
  //  SDR0->SetUsedRepresentation (shapeRep);

  return binder;
}

//=================================================================================================

occ::handle<Transfer_Binder> STEPControl_ActorWrite::TransferSubShape(
  const occ::handle<Transfer_Finder>&                         start,
  const occ::handle<StepShape_ShapeDefinitionRepresentation>& SDR0,
  occ::handle<StepGeom_GeometricRepresentationItem>&          AX1,
  const occ::handle<Transfer_FinderProcess>&                  FP,
  const StepData_Factors&                                     theLocalFactors,
  const occ::handle<NCollection_HSequence<TopoDS_Shape>>&     shapeGroup,
  const bool                                                  isManifold,
  const Message_ProgressRange&                                theProgress)
{
  occ::handle<TransferBRep_ShapeMapper> mapper = occ::down_cast<TransferBRep_ShapeMapper>(start);
  if (mapper.IsNull())
    return NullResult();
  TopoDS_Shape shape = mapper->Value();

  //   SHAPE EN POSITION VENANT D UN ASSEMBLAGE
  //   Il faut alors distinguer la transformation de la shape meme
  //   laquelle est consideree a l origine, puis transferee
  //   A part, un item decrivant une occurence en position est cree
  //   SINON, la shape est prise et transferee telle quelle
  TopoDS_Shape sh0 = shape;
  gp_Trsf      aLoc;
  bool         isShapeLocated = false;
  if (GroupMode() > 0)
  {
    const TopLoc_Location& shloc = shape.Location();
    isShapeLocated               = !shloc.IsIdentity();
    aLoc                         = shloc.Transformation();
    TopLoc_Location shident;
    sh0.Location(shident);
    mapper = TransferBRep::ShapeMapper(FP, sh0);
    mapper->SameAttributes(start);
  }

  occ::handle<Transfer_Binder>                         resbind = FP->Find(mapper);
  occ::handle<StepShape_ShapeDefinitionRepresentation> sdr;
  //  occ::handle<StepShape_ShapeRepresentation> resultat;
  STEPConstruct_Part              SDRTool;
  occ::handle<StepData_StepModel> aStepModel = occ::down_cast<StepData_StepModel>(FP->Model());

  // Already SDR and SR available : take them as are
  bool iasdr =
    FP->GetTypedTransient(resbind, STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation), sdr);
  if (iasdr)
    SDRTool.ReadSDR(sdr);
  else
  {
    SDRTool.MakeSDR(0, myContext.GetProductName(), myContext.GetAPD()->Application(), aStepModel);
    sdr = SDRTool.SDRValue();
  }
  //  resultat = GetCasted(StepShape_ShapeRepresentation,sdr->UsedRepresentation());

  // if shape itself not yet translated, do it now
  //: abv 20.05.02: see comment in TransferShape(): added "! iasdr ||"
  occ::handle<Transfer_Binder> resprod = TransientResult(sdr); // KA - OCC7141(skl 10.11.2004)
  bool                         isJustTransferred = false;
  if (!iasdr || resbind.IsNull())
  {
    occ::handle<Transfer_Binder> resbind1 =
      TransferShape(mapper, sdr, FP, theLocalFactors, shapeGroup, isManifold, theProgress);
    if (resbind1.IsNull() || sdr->UsedRepresentation().IsNull())
      return occ::handle<Transfer_Binder>();
    resbind                              = resbind1;
    occ::handle<Transfer_Binder> oldbind = FP->Find(mapper);
    if (!oldbind.IsNull() && !resbind.IsNull())
      resbind->AddResult(oldbind);
    isJustTransferred = true;
  }

  // A new resbind may have been produced
  //  DeclareAndCast(Transfer_SimpleBinderOfTransient,restrans,resbind);
  //  if (restrans.IsNull()) return resbind;
  //  FP->GetTypedTransient (resbind,STANDARD_TYPE(StepShape_ShapeRepresentation),resultat);
  //  sdr->SetUsedRepresentation(resultat);  // to be used by MakeItem

  // create assembly structures (CDSR, NAUO etc.)
  STEPConstruct_Assembly mkitem;

  // make location for assembly placement
  if (std::abs(aLoc.ScaleFactor() - 1.0) > Precision::Confusion())
  {
    if (aStepModel->InternalParameters.WriteScalingTrsf)
      FP->AddWarning(
        start,
        "The shape has a scaling factor, exported as cartesian_transformation_operator");
    else
      FP->AddWarning(start, "The shape has a scaling factor, skipped");
  }
  if (std::abs(aLoc.ScaleFactor() - 1.0) < Precision::Confusion()
      || !aStepModel->InternalParameters.WriteScalingTrsf)
  {
    // create a new axis2placement3d
    GeomToStep_MakeAxis2Placement3d mkax(aLoc, theLocalFactors);
    mkitem.Init(sdr, SDR0, myContext.GetDefaultAxis(), mkax.Value());
    AX1 = mkax.Value();
  }
  else
  {
    // create a new cartesian transformation
    GeomToStep_MakeCartesianTransformationOperator aMaker(aLoc, theLocalFactors);
    if (!aMaker.IsDone())
    {
      FP->AddFail(start, "The transformation relation creation failed");
    }
    mkitem.Init(sdr, SDR0, aMaker.Value());
    AX1 = aMaker.Value();
  }

  mkitem.MakeRelationship();
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> roots =
    myContext.GetRootsForAssemblyLink(mkitem);

  // add roots corresponding to assembly and product structures to binder
  // occ::handle<Transfer_Binder> resprod = resbind; //KA - OCC7141(skl 10.11.2004)
  // KA: we need only the current subshape in resprod, since the binder is copied
  //    in Transfershape which calls Transfersubshape   [ OCC7141(skl 10.11.2004) ]
  if (!iasdr)
  {
    resprod->AddResult(TransientResult(sdr));
    if (resprod != resbind)
      resbind->AddResult(TransientResult(sdr)); // KA - OCC7141(skl 10.11.2004)
    roots->Append(myContext.GetRootsForPart(SDRTool));
  }
  for (int i = 1; i <= roots->Length(); i++)
  {
    resprod->AddResult(TransientResult(roots->Value(i)));
    if (resprod != resbind)
      resbind->AddResult(TransientResult(roots->Value(i))); // KA - OCC7141(skl 10.11.2004)
  }
  if (isJustTransferred)
  {
    // We make CDSR of the current shape preceding CDSR of any subshapes,
    // therefore add resbind at the end.
    resprod->AddResult(resbind);
    FP->Bind(mapper, resprod);
  }

  myContext.NextIndex();

  // abv 16.10.00: bind CDSR (et al) to located shape in order to be able to track instances
  if (mapper != start && isShapeLocated)
  {
    occ::handle<Transfer_Binder> bnd = FP->Find(start);
    for (int j = 1; j <= roots->Length(); j++)
      if (bnd.IsNull())
        bnd = TransientResult(roots->Value(j));
      else
        bnd->AddResult(TransientResult(roots->Value(j)));
    FP->Bind(start, bnd);
  }

  return resprod;
}
