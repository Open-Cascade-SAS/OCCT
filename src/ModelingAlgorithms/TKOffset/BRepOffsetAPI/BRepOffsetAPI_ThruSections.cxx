// Created on: 1995-07-18
// Created by: Joelle CHAUVET
// Copyright (c) 1995-1999 Matra Datavision
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

// Modified:	Mon Jan 12 10:50:10 1998
//              gestion automatique de l'origine et de l'orientation
//              avec la methode ArrangeWires
// Modified:	Mon Jan 19 10:11:56 1998
//              traitement des cas particuliers cylindre, cone, plan
//              (methodes DetectKPart et CreateKPart)
// Modified:	Mon Feb 23 09:28:46 1998
//              traitement des sections avec nombre d'elements different
//              + quelques ameliorations pour les cas particuliers
//              + cas de la derniere section ponctuelle
// Modified:	Mon Apr  6 15:47:44 1998
//              traitement des cas particuliers deplace dans BRepFill
// Modified:	Thu Apr 30 15:24:17 1998
//              separation sections fermees / sections ouvertes + debug
// Modified:	Fri Jul 10 11:23:35 1998
//              surface de CreateSmoothed par concatenation,approximation
//              et segmentation (PRO13924, CTS21295)
// Modified:	Tue Jul 21 16:48:35 1998
//              pb de ratio (BUC60281)
// Modified:	Thu Jul 23 11:38:36 1998
//              sections bouclantes
// Modified:	Fri Aug 28 10:13:44 1998
//              traitement des sections ponctuelles
//              dans l'historique (cf. loft06 et loft09)
//              et dans le cas des solides
// Modified:	Tue Nov  3 10:06:15 1998
//              utilisation de BRepFill_CompatibleWires

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_FindPlane.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepFill_CompatibleWires.hxx>
#include <BRepFill_Generator.hxx>
#include <BRepGProp.hxx>
#include <BRepLib.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRepTools_ReShape.hxx>
#include <BSplCLib.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Conic.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_ApproxCurve.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <GeomFill_AppSurf.hxx>
#include <GeomFill_Line.hxx>
#include <GeomFill_SectionGenerator.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GProp_GProps.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <math_Gauss.hxx>
#include <math_Matrix.hxx>
#include <math_SVD.hxx>
#include <math_Vector.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NullObject.hxx>
#include <NCollection_Array1.hxx>
#include <TopExp.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_DynamicArray.hxx>
#include <NCollection_List.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <BRepAdaptor_Surface.hxx>

#include <algorithm>
#include <cmath>

//=======================================================================
// function : PreciseUpar
// purpose  : pins the u-parameter of surface close to U-knot
//           to this U-knot
//=======================================================================
static double PreciseUpar(const double anUpar, const occ::handle<Geom_BSplineSurface>& aSurface)
{
  constexpr double Tol = Precision::PConfusion();
  int              i1, i2;

  aSurface->LocateU(anUpar, Tol, i1, i2);
  double U1 = aSurface->UKnot(i1);
  double U2 = aSurface->UKnot(i2);

  double NewU = anUpar;

  NewU = (anUpar - U1 < U2 - anUpar) ? U1 : U2;
  return NewU;
}

//=======================================================================
// function :  PerformPlan
// purpose  : Construct a plane of filling if exists
//=======================================================================

static bool PerformPlan(const TopoDS_Wire& W, const double presPln, TopoDS_Face& theFace)
{
  bool            isDegen = true;
  TopoDS_Iterator iter(W);
  for (; iter.More(); iter.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(iter.Value());
    if (!BRep_Tool::Degenerated(anEdge))
    {
      isDegen = false;
    }
  }
  if (isDegen)
  {
    return true;
  }

  bool Ok = false;
  if (!W.IsNull())
  {
    BRepBuilderAPI_FindPlane Searcher(W, presPln);
    if (Searcher.Found())
    {
      theFace = BRepBuilderAPI_MakeFace(Searcher.Plane(), W);
      Ok      = true;
    }
    else // try to find another surface
    {
      BRepBuilderAPI_MakeFace MF(W);
      if (MF.IsDone())
      {
        theFace = MF.Face();
        Ok      = true;
      }
    }
  }

  return Ok;
}

//=================================================================================================

static bool IsTopologicallyClosedWire(const TopoDS_Wire& theWire)
{
  if (theWire.Closed())
  {
    return true;
  }

  TopoDS_Vertex aFirstVertex, aLastVertex;
  TopExp::Vertices(theWire, aFirstVertex, aLastVertex);
  if (aFirstVertex.IsNull() || aLastVertex.IsNull())
  {
    return false;
  }
  return aFirstVertex.IsSame(aLastVertex);
}

//=================================================================================================

static bool SectionNormal(const TopoDS_Wire& theSection,
                          const TopoDS_Wire& thePreviousSection,
                          const TopoDS_Wire& theNextSection,
                          const double       theTolerance,
                          gp_Vec&            theNormal)
{
  BRepBuilderAPI_FindPlane aPlaneFinder(theSection, theTolerance);
  if (!aPlaneFinder.Found())
  {
    return false;
  }

  gp_Dir       aDirection = aPlaneFinder.Plane()->Pln().Axis().Direction();
  GProp_GProps aSectionProperties;
  GProp_GProps anAdjacentProperties;
  BRepGProp::LinearProperties(theSection, aSectionProperties);

  gp_Vec aProgression;
  if (!theNextSection.IsNull())
  {
    BRepGProp::LinearProperties(theNextSection, anAdjacentProperties);
    aProgression = gp_Vec(aSectionProperties.CentreOfMass(), anAdjacentProperties.CentreOfMass());
  }
  else
  {
    BRepGProp::LinearProperties(thePreviousSection, anAdjacentProperties);
    aProgression = gp_Vec(anAdjacentProperties.CentreOfMass(), aSectionProperties.CentreOfMass());
  }
  if (aProgression.Dot(gp_Vec(aDirection)) < 0.0)
  {
    aDirection.Reverse();
  }
  theNormal = gp_Vec(aDirection);
  return true;
}

//=============================================================================
// function :  IsSameOriented
// purpose  : Checks whether aFace is oriented to the same side as aShell or not
//=============================================================================

static bool IsSameOriented(const TopoDS_Shape& aFace, const TopoDS_Shape& aShell)
{
  TopExp_Explorer    Explo(aFace, TopAbs_EDGE);
  TopoDS_Shape       anEdge = Explo.Current();
  TopAbs_Orientation Or1    = anEdge.Orientation();

  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    EFmap;
  TopExp::MapShapesAndAncestors(aShell, TopAbs_EDGE, TopAbs_FACE, EFmap);

  const TopoDS_Shape& AdjacentFace = EFmap.FindFromKey(anEdge).First();
  TopoDS_Shape        theEdge;
  for (Explo.Init(AdjacentFace, TopAbs_EDGE); Explo.More(); Explo.Next())
  {
    theEdge = Explo.Current();
    if (theEdge.IsSame(anEdge))
    {
      break;
    }
  }

  TopAbs_Orientation Or2 = theEdge.Orientation();
  return Or1 != Or2;
}

//=================================================================================================

static TopoDS_Solid MakeSolid(TopoDS_Shell&      shell,
                              const TopoDS_Wire& wire1,
                              const TopoDS_Wire& wire2,
                              const double       presPln,
                              TopoDS_Face&       face1,
                              TopoDS_Face&       face2)
{
  if (shell.IsNull())
  {
    throw StdFail_NotDone("Thrusections is not build");
  }
  bool         B = shell.Closed();
  BRep_Builder BB;

  if (!B)
  {
    // It is necessary to close the extremities
    B = PerformPlan(wire1, presPln, face1);
    if (B)
    {
      B = PerformPlan(wire2, presPln, face2);
      if (B)
      {
        if (!face1.IsNull() && !IsSameOriented(face1, shell))
        {
          face1.Reverse();
        }
        if (!face2.IsNull() && !IsSameOriented(face2, shell))
        {
          face2.Reverse();
        }

        if (!face1.IsNull())
        {
          BB.Add(shell, face1);
        }
        if (!face2.IsNull())
        {
          BB.Add(shell, face2);
        }

        shell.Closed(true);
      }
    }
  }

  TopoDS_Solid solid;
  BB.MakeSolid(solid);
  BB.Add(solid, shell);

  // verify the orientation the solid
  BRepClass3d_SolidClassifier clas3d(solid);
  clas3d.PerformInfinitePoint(Precision::Confusion());
  if (clas3d.State() == TopAbs_IN)
  {
    BB.MakeSolid(solid);
    TopoDS_Shape aLocalShape = shell.Reversed();
    BB.Add(solid, TopoDS::Shell(aLocalShape));
    //    B.Add(solid, TopoDS::Shell(newShell.Reversed()));
  }

  solid.Closed(true);
  return solid;
}

//=================================================================================================

BRepOffsetAPI_ThruSections::BRepOffsetAPI_ThruSections(const bool   isSolid,
                                                       const bool   ruled,
                                                       const double pres3d)
    : myNbEdgesInSection(0),
      myIsSolid(isSolid),
      myIsRuled(ruled),
      myPres3d(pres3d),
      myDegen1(false),
      myDegen2(false),
      myFirstNormalTolerance(-1.0),
      myLastNormalTolerance(-1.0),
      myFirstSupportAngularTolerance(0.01),
      myLastSupportAngularTolerance(0.01),
      myFirstSupportCurvatureTolerance(0.1),
      myLastSupportCurvatureTolerance(0.1),
      myHasFirstTangent(false),
      myHasLastTangent(false),
      myFirstTangentIsNormal(false),
      myLastTangentIsNormal(false),
      myFirstTangentIsSupport(false),
      myLastTangentIsSupport(false)
{
  myWCheck       = true;
  myMutableInput = true;
  //----------------------------
  myParamType      = Approx_ChordLength;
  myDegMax         = 8;
  myContinuity     = GeomAbs_C2;
  myCritWeights[0] = .4;
  myCritWeights[1] = .2;
  myCritWeights[2] = .4;
  myUseSmoothing   = false;
  myStatus         = BRepFill_ThruSectionErrorStatus_NotDone;
  ClearFirstSectionConstraint();
  ClearLastSectionConstraint();
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::Init(const bool isSolid, const bool ruled, const double pres3d)
{
  myIsSolid      = isSolid;
  myIsRuled      = ruled;
  myPres3d       = pres3d;
  myWCheck       = true;
  myMutableInput = true;
  //----------------------------
  myParamType      = Approx_ChordLength;
  myDegMax         = 6;
  myContinuity     = GeomAbs_C2;
  myCritWeights[0] = .4;
  myCritWeights[1] = .2;
  myCritWeights[2] = .4;
  myUseSmoothing   = false;
  myStatus         = BRepFill_ThruSectionErrorStatus_NotDone;
  ClearFirstSectionConstraint();
  ClearLastSectionConstraint();
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::AddWire(const TopoDS_Wire& wire)
{
  myWires.Append(wire);
  myInputWires.Append(wire);
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::AddVertex(const TopoDS_Vertex& aVertex)
{
  BRep_Builder BB;

  TopoDS_Edge DegEdge;
  BB.MakeEdge(DegEdge);
  BB.Add(DegEdge, aVertex.Oriented(TopAbs_FORWARD));
  BB.Add(DegEdge, aVertex.Oriented(TopAbs_REVERSED));
  BB.Degenerated(DegEdge, true);

  TopoDS_Wire DegWire;
  BB.MakeWire(DegWire);
  BB.Add(DegWire, DegEdge);
  DegWire.Closed(true);

  myWires.Append(DegWire);
  myInputWires.Append(DegWire);
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::CheckCompatibility(const bool check)
{
  myWCheck = check;
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::SetFirstSectionTangent(const gp_Dir& theDirection)
{
  SetFirstSectionTangent(theDirection, GeomAbs_G1);
}

//=================================================================================================

static void CheckBoundaryContinuity(const GeomAbs_Shape theContinuity)
{
  if (theContinuity != GeomAbs_G1 && theContinuity != GeomAbs_G2)
  {
    throw Standard_DomainError("BRepOffsetAPI_ThruSections: boundary continuity must be G1 or G2");
  }
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::SetFirstSectionTangent(const gp_Dir&       theDirection,
                                                        const GeomAbs_Shape theContinuity)
{
  CheckBoundaryContinuity(theContinuity);
  myFirstTangent            = gp_Vec(theDirection);
  myHasFirstTangent         = true;
  myFirstTangentIsNormal    = false;
  myFirstTangentIsSupport   = false;
  myFirstBoundaryContinuity = theContinuity;
  myFirstSupport.Nullify();
  myFirstEdgeSupports.Clear();
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::SetLastSectionTangent(const gp_Dir& theDirection)
{
  SetLastSectionTangent(theDirection, GeomAbs_G1);
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::SetLastSectionTangent(const gp_Dir&       theDirection,
                                                       const GeomAbs_Shape theContinuity)
{
  CheckBoundaryContinuity(theContinuity);
  myLastTangent            = gp_Vec(theDirection);
  myHasLastTangent         = true;
  myLastTangentIsNormal    = false;
  myLastTangentIsSupport   = false;
  myLastBoundaryContinuity = theContinuity;
  myLastSupport.Nullify();
  myLastEdgeSupports.Clear();
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::SetFirstSectionNormal(const double theTolerance)
{
  SetFirstSectionNormal(GeomAbs_G1, theTolerance);
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::SetFirstSectionNormal(const GeomAbs_Shape theContinuity,
                                                       const double        theTolerance)
{
  CheckBoundaryContinuity(theContinuity);
  myFirstNormalTolerance    = theTolerance;
  myHasFirstTangent         = true;
  myFirstTangentIsNormal    = true;
  myFirstTangentIsSupport   = false;
  myFirstBoundaryContinuity = theContinuity;
  myFirstSupport.Nullify();
  myFirstEdgeSupports.Clear();
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::SetLastSectionNormal(const double theTolerance)
{
  SetLastSectionNormal(GeomAbs_G1, theTolerance);
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::SetLastSectionNormal(const GeomAbs_Shape theContinuity,
                                                      const double        theTolerance)
{
  CheckBoundaryContinuity(theContinuity);
  myLastNormalTolerance    = theTolerance;
  myHasLastTangent         = true;
  myLastTangentIsNormal    = true;
  myLastTangentIsSupport   = false;
  myLastBoundaryContinuity = theContinuity;
  myLastSupport.Nullify();
  myLastEdgeSupports.Clear();
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::SetFirstSectionSupport(const TopoDS_Face& theSupport,
                                                        const double       theAngularTolerance)
{
  SetFirstSectionSupport(theSupport, GeomAbs_G1, theAngularTolerance, 0.1);
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::SetFirstSectionSupport(const TopoDS_Face&  theSupport,
                                                        const GeomAbs_Shape theContinuity,
                                                        const double        theAngularTolerance,
                                                        const double        theCurvatureTolerance)
{
  CheckBoundaryContinuity(theContinuity);
  myFirstSupport                   = theSupport;
  myFirstSupportAngularTolerance   = theAngularTolerance;
  myFirstSupportCurvatureTolerance = theCurvatureTolerance;
  myHasFirstTangent                = true;
  myFirstTangentIsNormal           = false;
  myFirstTangentIsSupport          = true;
  myFirstBoundaryContinuity        = theContinuity;
  myFirstEdgeSupports.Clear();
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::SetFirstSectionSupports(const EdgeFaceMap& theSupports,
                                                         const double       theAngularTolerance)
{
  SetFirstSectionSupports(theSupports, GeomAbs_G1, theAngularTolerance, 0.1);
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::SetFirstSectionSupports(const EdgeFaceMap&  theSupports,
                                                         const GeomAbs_Shape theContinuity,
                                                         const double        theAngularTolerance,
                                                         const double        theCurvatureTolerance)
{
  CheckBoundaryContinuity(theContinuity);
  myFirstSupport.Nullify();
  myFirstEdgeSupports              = theSupports;
  myFirstSupportAngularTolerance   = theAngularTolerance;
  myFirstSupportCurvatureTolerance = theCurvatureTolerance;
  myHasFirstTangent                = true;
  myFirstTangentIsNormal           = false;
  myFirstTangentIsSupport          = true;
  myFirstBoundaryContinuity        = theContinuity;
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::SetLastSectionSupport(const TopoDS_Face& theSupport,
                                                       const double       theAngularTolerance)
{
  SetLastSectionSupport(theSupport, GeomAbs_G1, theAngularTolerance, 0.1);
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::SetLastSectionSupport(const TopoDS_Face&  theSupport,
                                                       const GeomAbs_Shape theContinuity,
                                                       const double        theAngularTolerance,
                                                       const double        theCurvatureTolerance)
{
  CheckBoundaryContinuity(theContinuity);
  myLastSupport                   = theSupport;
  myLastSupportAngularTolerance   = theAngularTolerance;
  myLastSupportCurvatureTolerance = theCurvatureTolerance;
  myHasLastTangent                = true;
  myLastTangentIsNormal           = false;
  myLastTangentIsSupport          = true;
  myLastBoundaryContinuity        = theContinuity;
  myLastEdgeSupports.Clear();
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::SetLastSectionSupports(const EdgeFaceMap& theSupports,
                                                        const double       theAngularTolerance)
{
  SetLastSectionSupports(theSupports, GeomAbs_G1, theAngularTolerance, 0.1);
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::SetLastSectionSupports(const EdgeFaceMap&  theSupports,
                                                        const GeomAbs_Shape theContinuity,
                                                        const double        theAngularTolerance,
                                                        const double        theCurvatureTolerance)
{
  CheckBoundaryContinuity(theContinuity);
  myLastSupport.Nullify();
  myLastEdgeSupports              = theSupports;
  myLastSupportAngularTolerance   = theAngularTolerance;
  myLastSupportCurvatureTolerance = theCurvatureTolerance;
  myHasLastTangent                = true;
  myLastTangentIsNormal           = false;
  myLastTangentIsSupport          = true;
  myLastBoundaryContinuity        = theContinuity;
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::ClearFirstSectionConstraint()
{
  myFirstTangent.SetCoord(0.0, 0.0, 0.0);
  myFirstNormalTolerance = -1.0;
  myFirstSupport.Nullify();
  myFirstEdgeSupports.Clear();
  myFirstWorkingEdgeSupports.Clear();
  myFirstSupportAngularTolerance   = 0.01;
  myFirstSupportCurvatureTolerance = 0.1;
  myHasFirstTangent                = false;
  myFirstTangentIsNormal           = false;
  myFirstTangentIsSupport          = false;
  myFirstBoundaryContinuity        = GeomAbs_G1;
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::ClearLastSectionConstraint()
{
  myLastTangent.SetCoord(0.0, 0.0, 0.0);
  myLastNormalTolerance = -1.0;
  myLastSupport.Nullify();
  myLastEdgeSupports.Clear();
  myLastWorkingEdgeSupports.Clear();
  myLastSupportAngularTolerance   = 0.01;
  myLastSupportCurvatureTolerance = 0.1;
  myHasLastTangent                = false;
  myLastTangentIsNormal           = false;
  myLastTangentIsSupport          = false;
  myLastBoundaryContinuity        = GeomAbs_G1;
}

//=================================================================================================

using EdgeNewIndicesMap =
  NCollection_DataMap<TopoDS_Shape, NCollection_List<int>, TopTools_ShapeMapHasher>;

static bool PrepareWorkingEdgeSupports(
  const BRepOffsetAPI_ThruSections::EdgeFaceMap& theInputSupports,
  const TopoDS_Wire&                             theInputWire,
  const TopoDS_Wire&                             theWorkingWire,
  const EdgeNewIndicesMap&                       theEdgeNewIndices,
  BRepOffsetAPI_ThruSections::EdgeFaceMap&       theWorkingSupports)
{
  theWorkingSupports.Clear();
  if (theInputSupports.IsEmpty())
  {
    return true;
  }

  NCollection_DynamicArray<TopoDS_Edge> aWorkingEdges;
  BRep_Builder                          aBuilder;
  for (BRepTools_WireExplorer anExplorer(theWorkingWire); anExplorer.More(); anExplorer.Next())
  {
    aWorkingEdges.Append(TopoDS::Edge(anExplorer.Current()));
  }

  for (BRepTools_WireExplorer anExplorer(theInputWire); anExplorer.More(); anExplorer.Next())
  {
    const TopoDS_Edge&           anInputEdge = TopoDS::Edge(anExplorer.Current());
    const TopoDS_Shape*          aSupport    = theInputSupports.Seek(anInputEdge);
    const NCollection_List<int>* anIndices   = theEdgeNewIndices.Seek(anInputEdge);
    if (aSupport == nullptr || aSupport->ShapeType() != TopAbs_FACE || anIndices == nullptr
        || anIndices->IsEmpty())
    {
      return false;
    }

    for (NCollection_List<int>::Iterator anIndexIterator(*anIndices); anIndexIterator.More();
         anIndexIterator.Next())
    {
      const int anIndex = anIndexIterator.Value();
      if (anIndex < 1 || anIndex > aWorkingEdges.Length())
      {
        return false;
      }
      const TopoDS_Edge& aWorkingEdge = aWorkingEdges[anIndex - 1];
      TopoDS_Vertex      aFirstVertex, aLastVertex;
      TopExp::Vertices(anInputEdge, aFirstVertex, aLastVertex);
      double anInputTolerance = BRep_Tool::Tolerance(anInputEdge);
      if (!aFirstVertex.IsNull())
      {
        anInputTolerance = std::max(anInputTolerance, BRep_Tool::Tolerance(aFirstVertex));
      }
      if (!aLastVertex.IsNull())
      {
        anInputTolerance = std::max(anInputTolerance, BRep_Tool::Tolerance(aLastVertex));
      }
      if (BRep_Tool::Tolerance(aWorkingEdge) < anInputTolerance)
      {
        aBuilder.UpdateEdge(aWorkingEdge, anInputTolerance);
      }
      const TopoDS_Shape* anExistingSupport = theWorkingSupports.Seek(aWorkingEdge);
      if (anExistingSupport != nullptr)
      {
        if (!anExistingSupport->IsSame(*aSupport))
        {
          return false;
        }
      }
      else
      {
        theWorkingSupports.Bind(aWorkingEdge, *aSupport);
      }
    }
  }

  return theWorkingSupports.Extent() == aWorkingEdges.Length();
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::Build(const Message_ProgressRange& /*theRange*/)
{
  myStatus = BRepFill_ThruSectionErrorStatus_Done;
  myBFGenerator.Nullify();
  myDegen1 = false;
  myDegen2 = false;
  myFirstWorkingEdgeSupports.Clear();
  myLastWorkingEdgeSupports.Clear();
  const bool hasBoundaryConstraints = myHasFirstTangent || myHasLastTangent;
  if (hasBoundaryConstraints && myWires.Length() < 2)
  {
    myStatus = BRepFill_ThruSectionErrorStatus_InvalidBoundaryConstraint;
    NotDone();
    return;
  }
  if ((myFirstTangentIsSupport
       && ((myFirstSupport.IsNull() && myFirstEdgeSupports.IsEmpty())
           || !std::isfinite(myFirstSupportAngularTolerance)
           || myFirstSupportAngularTolerance <= 0.0
           || myFirstSupportAngularTolerance >= 0.5 * std::acos(-1.0)
           || (myFirstBoundaryContinuity == GeomAbs_G2
               && (!std::isfinite(myFirstSupportCurvatureTolerance)
                   || myFirstSupportCurvatureTolerance <= 0.0))))
      || (myLastTangentIsSupport
          && ((myLastSupport.IsNull() && myLastEdgeSupports.IsEmpty())
              || !std::isfinite(myLastSupportAngularTolerance)
              || myLastSupportAngularTolerance <= 0.0
              || myLastSupportAngularTolerance >= 0.5 * std::acos(-1.0)
              || (myLastBoundaryContinuity == GeomAbs_G2
                  && (!std::isfinite(myLastSupportCurvatureTolerance)
                      || myLastSupportCurvatureTolerance <= 0.0)))))
  {
    myStatus = BRepFill_ThruSectionErrorStatus_InvalidBoundaryConstraint;
    NotDone();
    return;
  }
  if (hasBoundaryConstraints && (myIsRuled || myUseSmoothing))
  {
    myStatus = BRepFill_ThruSectionErrorStatus_IncompatibleOptions;
    NotDone();
    return;
  }
  // Check set of section for right configuration of punctual sections
  int             i;
  TopExp_Explorer explo;
  for (i = 2; i <= myWires.Length() - 1; i++)
  {
    bool wdeg = true;
    for (explo.Init(myWires(i), TopAbs_EDGE); explo.More(); explo.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(explo.Current());
      wdeg                      = wdeg && (BRep_Tool::Degenerated(anEdge));
    }
    if (wdeg)
    {
      myStatus = BRepFill_ThruSectionErrorStatus_WrongUsage;
      return;
    }
  }
  if (myWires.Length() <= 2)
  {
    bool wdeg = true;
    for (i = 1; i <= myWires.Length(); i++)
    {
      for (explo.Init(myWires(i), TopAbs_EDGE); explo.More(); explo.Next())
      {
        const TopoDS_Edge& anEdge = TopoDS::Edge(explo.Current());
        wdeg                      = wdeg && (BRep_Tool::Degenerated(anEdge));
      }
    }
    if (wdeg)
    {
      myStatus = BRepFill_ThruSectionErrorStatus_WrongUsage;
      return;
    }
  }

  myNbEdgesInSection = 0;

  if (myWCheck)
  {
    // compute origin and orientation on wires to avoid twisted results
    // and update wires to have same number of edges

    // use BRepFill_CompatibleWires
    NCollection_Sequence<TopoDS_Shape> WorkingSections;
    WorkingSections.Clear();
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
      WorkingMap;
    WorkingMap.Clear();

    // Calculate the working sections
    BRepFill_CompatibleWires Georges(myWires);
    Georges.Perform();
    if (Georges.IsDone())
    {
      WorkingSections = Georges.Shape();
      WorkingMap      = Georges.Generated();
      myDegen1        = Georges.IsDegeneratedFirstSection();
      myDegen2        = Georges.IsDegeneratedLastSection();
      // For each sub-edge of each section
      // we save its splits
      int IndFirstSec = 1;
      if (Georges.IsDegeneratedFirstSection())
      {
        IndFirstSec = 2;
      }
      TopoDS_Wire aWorkingSection = TopoDS::Wire(WorkingSections(IndFirstSec));
      myNbEdgesInSection += aWorkingSection.NbChildren();
      for (int ii = 1; ii <= myWires.Length(); ii++)
      {
        TopoDS_Iterator itw(myWires(ii));
        for (; itw.More(); itw.Next())
        {
          const TopoDS_Edge& anEdge = TopoDS::Edge(itw.Value());
          int                aSign  = 1;
          TopoDS_Vertex      Vfirst, Vlast;
          TopExp::Vertices(anEdge, Vfirst, Vlast);
          const NCollection_List<TopoDS_Shape>& aNewEdges = Georges.GeneratedShapes(anEdge);
          NCollection_List<int>                 IList;
          aWorkingSection                                     = TopoDS::Wire(WorkingSections(ii));
          int                                      NbNewEdges = aNewEdges.Extent();
          NCollection_List<TopoDS_Shape>::Iterator itl(aNewEdges);
          for (int kk = 1; itl.More(); itl.Next(), kk++)
          {
            const TopoDS_Edge&     aNewEdge = TopoDS::Edge(itl.Value());
            int                    inde     = 1;
            BRepTools_WireExplorer wexp(aWorkingSection);
            for (; wexp.More(); wexp.Next(), inde++)
            {
              const TopoDS_Shape& aWorkingEdge = wexp.Current();
              if (aWorkingEdge.IsSame(aNewEdge))
              {
                aSign = (aWorkingEdge.Orientation() == TopAbs_FORWARD) ? 1 : -1;
                break;
              }
            }
            IList.Append(inde);
            if (kk == 1 || kk == NbNewEdges)
            {
              // For each sub-vertex of each section
              // we save its index of new edge
              TopoDS_Vertex NewVfirst, NewVlast;
              TopExp::Vertices(aNewEdge, NewVfirst, NewVlast);
              if (NewVfirst.IsSame(Vfirst) && !myVertexIndex.IsBound(Vfirst))
              {
                myVertexIndex.Bind(Vfirst, aSign * inde);
              }
              if (NewVlast.IsSame(Vlast) && !myVertexIndex.IsBound(Vlast))
              {
                myVertexIndex.Bind(Vlast, aSign * (-inde));
              }
            }
          }
          myEdgeNewIndices.Bind(anEdge, IList);
        }
      }
    }
    else
    {
      myStatus = Georges.GetStatus();
      NotDone();
      return;
    }

    myWires = WorkingSections;
  } // if (myWCheck)
  else // no check
  {
    TopoDS_Edge anEdge;
    for (int ii = 1; ii <= myWires.Length(); ii++)
    {
      TopExp_Explorer Explo(myWires(ii), TopAbs_EDGE);
      int             inde = 1;
      for (; Explo.More(); Explo.Next(), inde++)
      {
        anEdge = TopoDS::Edge(Explo.Current());
        NCollection_List<int> IList;
        IList.Append(inde);
        myEdgeNewIndices.Bind(anEdge, IList);
        TopoDS_Vertex V1, V2;
        TopExp::Vertices(anEdge, V1, V2);
        if (!myVertexIndex.IsBound(V1))
        {
          myVertexIndex.Bind(V1, inde);
        }
        if (!myVertexIndex.IsBound(V2))
        {
          myVertexIndex.Bind(V2, -inde);
        }
      }
      inde--;
      if (inde > myNbEdgesInSection)
      {
        myNbEdgesInSection = inde;
      }
      if (inde == 1 && BRep_Tool::Degenerated(anEdge))
      {
        if (ii == 1)
        {
          myDegen1 = true;
        }
        else
        {
          myDegen2 = true;
        }
      }
    }
  }

  if ((myFirstTangentIsSupport && !myFirstEdgeSupports.IsEmpty()
       && (myInputWires.First().ShapeType() != TopAbs_WIRE
           || myWires.First().ShapeType() != TopAbs_WIRE
           || !PrepareWorkingEdgeSupports(myFirstEdgeSupports,
                                          TopoDS::Wire(myInputWires.First()),
                                          TopoDS::Wire(myWires.First()),
                                          myEdgeNewIndices,
                                          myFirstWorkingEdgeSupports)))
      || (myLastTangentIsSupport && !myLastEdgeSupports.IsEmpty()
          && (myInputWires.Last().ShapeType() != TopAbs_WIRE
              || myWires.Last().ShapeType() != TopAbs_WIRE
              || !PrepareWorkingEdgeSupports(myLastEdgeSupports,
                                             TopoDS::Wire(myInputWires.Last()),
                                             TopoDS::Wire(myWires.Last()),
                                             myEdgeNewIndices,
                                             myLastWorkingEdgeSupports))))
  {
    myStatus = BRepFill_ThruSectionErrorStatus_InvalidBoundaryConstraint;
    NotDone();
    return;
  }

  if ((myHasFirstTangent && myDegen1) || (myHasLastTangent && myDegen2)
      || (hasBoundaryConstraints && myWires.Length() > 1
          && myWires(1).IsSame(myWires(myWires.Length()))))
  {
    myStatus = BRepFill_ThruSectionErrorStatus_InvalidBoundaryConstraint;
    NotDone();
    return;
  }

  try
  {
    // Calculate the resulting shape
    if ((myWires.Length() == 2 && !hasBoundaryConstraints) || myIsRuled)
    {
      // create a ruled shell
      CreateRuled();
    }
    else
    {
      // create a smoothed shell
      CreateSmoothed();
    }
  }
  catch (Standard_Failure const&)
  {
    NotDone();
    return;
  }

  if (myStatus != BRepFill_ThruSectionErrorStatus_Done)
  {
    NotDone();
    return;
  }
  // Encode the Regularities
  BRepLib::EncodeRegularity(myShape);
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::CreateRuled()
{
  int nbSects   = myWires.Length();
  myBFGenerator = new BRepFill_Generator();
  myBFGenerator->SetMutableInput(IsMutableInput());
  //  for (int i=1; i<=nbSects; i++) {
  int i;
  for (i = 1; i <= nbSects; i++)
  {
    myBFGenerator->AddWire(TopoDS::Wire(myWires(i)));
  }
  myBFGenerator->Perform();
  BRepFill_ThruSectionErrorStatus aStatus = myBFGenerator->GetStatus();
  if (aStatus != BRepFill_ThruSectionErrorStatus_Done)
  {
    myStatus = aStatus;
    return;
  }
  TopoDS_Shell shell = myBFGenerator->Shell();

  if (myIsSolid)
  {

    // check if the first wire is the same as the last
    bool vClosed = (myWires(1).IsSame(myWires(nbSects)));

    if (vClosed)
    {

      TopoDS_Solid solid;
      BRep_Builder B;
      B.MakeSolid(solid);
      B.Add(solid, shell);

      // verify the orientation of the solid
      BRepClass3d_SolidClassifier clas3d(solid);
      clas3d.PerformInfinitePoint(Precision::Confusion());
      if (clas3d.State() == TopAbs_IN)
      {
        B.MakeSolid(solid);
        TopoDS_Shape aLocalShape = shell.Reversed();
        B.Add(solid, TopoDS::Shell(aLocalShape));
        //	B.Add(solid, TopoDS::Shell(shell.Reversed()));
      }
      myShape = solid;
    }

    else
    {
      // myBFGenerator stores the same 'myWires'
      TopoDS_Wire wire1 = TopoDS::Wire(myBFGenerator->ResultShape(myWires.First()));
      TopoDS_Wire wire2 = TopoDS::Wire(myBFGenerator->ResultShape(myWires.Last()));

      myShape = MakeSolid(shell, wire1, wire2, myPres3d, myFirst, myLast);
    }

    Done();
  }

  else
  {
    myShape = shell;
    Done();
  }

  // history
  BRepTools_WireExplorer anExp1, anExp2;
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    M;
  TopExp::MapShapesAndAncestors(shell, TopAbs_EDGE, TopAbs_FACE, M);
  NCollection_List<TopoDS_Shape>::Iterator it;

  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    MV;
  TopExp::MapShapesAndAncestors(shell, TopAbs_VERTEX, TopAbs_FACE, MV);

  for (i = 1; i <= nbSects - 1; i++)
  {

    const TopoDS_Wire& wire1 = TopoDS::Wire(myWires(i));
    const TopoDS_Wire& wire2 = TopoDS::Wire(myWires(i + 1));

    anExp1.Init(wire1);
    anExp2.Init(wire2);

    bool tantque = anExp1.More() && anExp2.More();

    while (tantque)
    {

      const TopoDS_Shape& edge1  = anExp1.Current();
      const TopoDS_Shape& edge2  = anExp2.Current();
      bool                degen1 = BRep_Tool::Degenerated(anExp1.Current());
      bool                degen2 = BRep_Tool::Degenerated(anExp2.Current());

      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> MapFaces;
      if (degen2)
      {
        TopoDS_Vertex Vdegen =
          TopoDS::Vertex(myBFGenerator->ResultShape(TopExp::FirstVertex(TopoDS::Edge(edge2))));
        for (it.Initialize(MV.FindFromKey(Vdegen)); it.More(); it.Next())
        {
          MapFaces.Add(it.Value());
        }
      }
      else
      {
        for (it.Initialize(M.FindFromKey(myBFGenerator->ResultShape(edge2))); it.More(); it.Next())
        {
          MapFaces.Add(it.Value());
        }
      }

      if (degen1)
      {
        TopoDS_Vertex Vdegen =
          TopoDS::Vertex(myBFGenerator->ResultShape(TopExp::FirstVertex(TopoDS::Edge(edge1))));
        for (it.Initialize(MV.FindFromKey(Vdegen)); it.More(); it.Next())
        {
          const TopoDS_Shape& Face = it.Value();
          if (MapFaces.Contains(Face))
          {
            myEdgeFace.Bind(edge1, Face);
            break;
          }
        }
      }
      else
      {
        for (it.Initialize(M.FindFromKey(myBFGenerator->ResultShape(edge1))); it.More(); it.Next())
        {
          const TopoDS_Shape& Face = it.Value();
          if (MapFaces.Contains(Face))
          {
            myEdgeFace.Bind(edge1, Face);
            break;
          }
        }
      }

      if (!degen1)
      {
        anExp1.Next();
      }
      if (!degen2)
      {
        anExp2.Next();
      }

      tantque = anExp1.More() && anExp2.More();
      if (degen1)
      {
        tantque = anExp2.More();
      }
      if (degen2)
      {
        tantque = anExp1.More();
      }
    }
  }
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::CreateSmoothed()
{
  // initialisation
  int                    nbSects = myWires.Length();
  BRepTools_WireExplorer anExp;

  bool w1Point = true;
  // check if the first wire is punctual
  for (anExp.Init(TopoDS::Wire(myWires(1))); anExp.More(); anExp.Next())
  {
    w1Point = w1Point && (BRep_Tool::Degenerated(anExp.Current()));
  }

  bool w2Point = true;
  // check if the last wire is punctual
  for (anExp.Init(TopoDS::Wire(myWires(nbSects))); anExp.More(); anExp.Next())
  {
    w2Point = w2Point && (BRep_Tool::Degenerated(anExp.Current()));
  }

  bool vClosed = false;
  // check if the first wire is the same as last
  if (myWires(1).IsSame(myWires(myWires.Length())))
  {
    vClosed = true;
  }

  // find the dimension
  int nbEdges = 0;
  if (!w1Point)
  {
    for (anExp.Init(TopoDS::Wire(myWires(1))); anExp.More(); anExp.Next())
    {
      nbEdges++;
    }
  }
  else
  {
    for (anExp.Init(TopoDS::Wire(myWires(2))); anExp.More(); anExp.Next())
    {
      nbEdges++;
    }
  }

  // recover the shapes
  bool                             uClosed = true;
  NCollection_Array1<TopoDS_Shape> shapes(1, nbSects * nbEdges);
  int                              nb                     = 0, i, j;
  const bool                       hasBoundaryConstraints = myHasFirstTangent || myHasLastTangent;

  NCollection_List<TopoDS_Shape>::Iterator anInputWireIterator(myInputWires);
  for (i = 1; i <= nbSects; i++, anInputWireIterator.Next())
  {
    const TopoDS_Wire& wire      = TopoDS::Wire(myWires(i));
    const TopoDS_Wire& inputWire = TopoDS::Wire(anInputWireIterator.Value());
    // Compatibility splitting can duplicate a seam vertex and introduce a
    // small numerical gap. Retain the closure of the corresponding input wire.
    if (hasBoundaryConstraints && !IsTopologicallyClosedWire(wire)
        && !IsTopologicallyClosedWire(inputWire))
    {
      uClosed = false;
    }
    else if (!hasBoundaryConstraints && !wire.Closed())
    {
      // Preserve the legacy closure check for unconstrained lofts.
      TopoDS_Vertex aFirstVertex, aLastVertex;
      TopExp::Vertices(wire, aFirstVertex, aLastVertex);
      if (!aFirstVertex.IsSame(aLastVertex))
      {
        uClosed = false;
      }
    }
    if ((i == 1 && w1Point) || (i == nbSects && w2Point))
    {
      // if the wire is punctual
      anExp.Init(TopoDS::Wire(wire));
      for (j = 1; j <= nbEdges; j++)
      {
        nb++;
        shapes(nb) = anExp.Current();
      }
    }
    else
    {
      // otherwise
      for (anExp.Init(TopoDS::Wire(wire)); anExp.More(); anExp.Next())
      {
        nb++;
        shapes(nb) = anExp.Current();
      }
    }
  }

  // create the new surface
  TopoDS_Shell                     shell;
  TopoDS_Face                      face;
  TopoDS_Wire                      W;
  TopoDS_Edge                      edge, edge1, edge2, edge3, edge4, couture;
  NCollection_Array1<TopoDS_Shape> vcouture(1, nbEdges);

  BRep_Builder B;
  B.MakeShell(shell);

  TopoDS_Wire  newW1, newW2;
  BRep_Builder BW1, BW2;
  BW1.MakeWire(newW1);
  BW2.MakeWire(newW2);

  TopLoc_Location loc;
  TopoDS_Vertex   v1f, v1l, v2f, v2l, v1Seam, v2Seam;

  int                        nbPnts = 21;
  NCollection_Array2<gp_Pnt> points(1, nbPnts, 1, nbSects);

  // concatenate each section to get a total surface that will be segmented
  occ::handle<Geom_BSplineSurface> TS;
  TS = TotalSurf(shapes, nbSects, nbEdges, w1Point, w2Point, vClosed);

  if (TS.IsNull())
  {
    if (myStatus == BRepFill_ThruSectionErrorStatus_Done)
    {
      myStatus = BRepFill_ThruSectionErrorStatus_Failed;
    }
    return;
  }

  NCollection_Array1<gp_Pnt> aUSeamPoles(1, TS->NbVPoles());
  NCollection_Array1<double> aUSeamWeights(1, TS->NbVPoles());
  TopoDS_Shape               firstEdge;
  for (i = 1; i <= nbEdges; i++)
  {

    // segmentation of TS
    occ::handle<Geom_BSplineSurface> surface;
    surface = occ::down_cast<Geom_BSplineSurface>(TS->Copy());
    double Ui1, Ui2, V0, V1;
    Ui1 = i - 1;
    Ui2 = i;
    Ui1 = PreciseUpar(Ui1, surface);
    Ui2 = PreciseUpar(Ui2, surface);
    V0  = surface->VKnot(surface->FirstVKnotIndex());
    V1  = surface->VKnot(surface->LastVKnotIndex());
    surface->Segment(Ui1, Ui2, V0, V1);
    if (hasBoundaryConstraints && uClosed)
    {
      if (surface->NbVPoles() != aUSeamPoles.Length())
      {
        myStatus = BRepFill_ThruSectionErrorStatus_Failed;
        return;
      }
      if (i == 1)
      {
        for (int aVIndex = 1; aVIndex <= surface->NbVPoles(); ++aVIndex)
        {
          aUSeamPoles(aVIndex)   = surface->Pole(1, aVIndex);
          aUSeamWeights(aVIndex) = surface->Weight(1, aVIndex);
        }
      }
      if (i == nbEdges)
      {
        const int aLastUPole = surface->NbUPoles();
        for (int aVIndex = 1; aVIndex <= surface->NbVPoles(); ++aVIndex)
        {
          surface->SetPole(aLastUPole, aVIndex, aUSeamPoles(aVIndex), aUSeamWeights(aVIndex));
        }
      }
    }

    // return vertices
    edge = TopoDS::Edge(shapes(i));
    TopExp::Vertices(edge, v1f, v1l);
    if (edge.Orientation() == TopAbs_REVERSED)
    {
      TopExp::Vertices(edge, v1l, v1f);
    }
    firstEdge = edge;

    edge = TopoDS::Edge(shapes((nbSects - 1) * nbEdges + i));
    TopExp::Vertices(edge, v2f, v2l);
    if (edge.Orientation() == TopAbs_REVERSED)
    {
      TopExp::Vertices(edge, v2l, v2f);
    }
    if (hasBoundaryConstraints && i == 1)
    {
      v1Seam = v1f;
      v2Seam = v2f;
    }
    if (hasBoundaryConstraints && uClosed && i == nbEdges)
    {
      // Use the same topological vertices as the exact first-patch boundary.
      v1l = v1Seam;
      v2l = v2Seam;
    }

    // make the face
    B.MakeFace(face, surface, Precision::Confusion());

    // make the wire
    B.MakeWire(W);

    // make the missing edges
    double f1, f2, l1, l2;
    surface->Bounds(f1, l1, f2, l2);

    // --- edge 1
    if (w1Point)
    {
      // copy the degenerated edge
      TopoDS_Shape aLocalShape = shapes(1).EmptyCopied();
      edge1                    = TopoDS::Edge(aLocalShape);
      //      edge1 =  TopoDS::Edge(shapes(1).EmptyCopied());
      edge1.Orientation(TopAbs_FORWARD);
    }
    else
    {
      B.MakeEdge(edge1, surface->VIso(f2), Precision::Confusion());
    }
    v1f.Orientation(TopAbs_FORWARD);
    B.Add(edge1, v1f);
    v1l.Orientation(TopAbs_REVERSED);
    B.Add(edge1, v1l);
    B.Range(edge1, f1, l1);
    // processing of looping sections
    // store edges of the 1st section
    if (vClosed)
    {
      vcouture(i) = edge1;
    }

    // --- edge 2
    if (vClosed)
    {
      edge2 = TopoDS::Edge(vcouture(i));
    }
    else
    {
      if (w2Point)
      {
        // copy of the degenerated edge
        TopoDS_Shape aLocalShape = shapes(nbSects * nbEdges).EmptyCopied();
        edge2                    = TopoDS::Edge(aLocalShape);
        //	edge2 =  TopoDS::Edge(shapes(nbSects*nbEdges).EmptyCopied());
        edge2.Orientation(TopAbs_FORWARD);
      }
      else
      {
        B.MakeEdge(edge2, surface->VIso(l2), Precision::Confusion());
      }
      v2f.Orientation(TopAbs_FORWARD);
      B.Add(edge2, v2f);
      v2l.Orientation(TopAbs_REVERSED);
      B.Add(edge2, v2l);
      B.Range(edge2, f1, l1);
    }
    edge2.Reverse();

    // --- edge 3
    if (i == 1)
    {
      B.MakeEdge(edge3, surface->UIso(f1), Precision::Confusion());
      v1f.Orientation(TopAbs_FORWARD);
      B.Add(edge3, v1f);
      v2f.Orientation(TopAbs_REVERSED);
      B.Add(edge3, v2f);
      B.Range(edge3, f2, l2);
      if (uClosed)
      {
        couture = edge3;
      }
    }
    else
    {
      edge3 = edge4;
    }
    edge3.Reverse();

    // --- edge 4
    if (uClosed && i == nbEdges)
    {
      edge4 = couture;
    }
    else
    {
      B.MakeEdge(edge4, surface->UIso(l1), Precision::Confusion());
      v1l.Orientation(TopAbs_FORWARD);
      B.Add(edge4, v1l);
      v2l.Orientation(TopAbs_REVERSED);
      B.Add(edge4, v2l);
      B.Range(edge4, f2, l2);
    }

    B.Add(W, edge1);
    B.Add(W, edge4);
    B.Add(W, edge2);
    B.Add(W, edge3);

    // set PCurve
    if (vClosed)
    {
      B.UpdateEdge(edge1,
                   new Geom2d_Line(gp_Pnt2d(0, f2), gp_Dir2d(gp_Dir2d::D::X)),
                   new Geom2d_Line(gp_Pnt2d(0, l2), gp_Dir2d(gp_Dir2d::D::X)),
                   face,
                   Precision::Confusion());
      B.Range(edge1, face, f1, l1);
    }
    else
    {
      B.UpdateEdge(edge1,
                   new Geom2d_Line(gp_Pnt2d(0, f2), gp_Dir2d(gp_Dir2d::D::X)),
                   face,
                   Precision::Confusion());
      B.Range(edge1, face, f1, l1);
      B.UpdateEdge(edge2,
                   new Geom2d_Line(gp_Pnt2d(0, l2), gp_Dir2d(gp_Dir2d::D::X)),
                   face,
                   Precision::Confusion());
      B.Range(edge2, face, f1, l1);
    }

    if (uClosed && nbEdges == 1)
    {
      B.UpdateEdge(edge3,
                   new Geom2d_Line(gp_Pnt2d(l1, 0), gp_Dir2d(gp_Dir2d::D::Y)),
                   new Geom2d_Line(gp_Pnt2d(f1, 0), gp_Dir2d(gp_Dir2d::D::Y)),
                   face,
                   Precision::Confusion());
      B.Range(edge3, face, f2, l2);
    }
    else
    {
      B.UpdateEdge(edge3,
                   new Geom2d_Line(gp_Pnt2d(f1, 0), gp_Dir2d(gp_Dir2d::D::Y)),
                   face,
                   Precision::Confusion());
      B.Range(edge3, face, f2, l2);
      B.UpdateEdge(edge4,
                   new Geom2d_Line(gp_Pnt2d(l1, 0), gp_Dir2d(gp_Dir2d::D::Y)),
                   face,
                   Precision::Confusion());
      B.Range(edge4, face, f2, l2);
    }
    B.Add(face, W);
    B.Add(shell, face);

    // complete newW1 newW2
    TopoDS_Edge edge12 = edge1;
    TopoDS_Edge edge22 = edge2;
    edge12.Reverse();
    edge22.Reverse();
    BW1.Add(newW1, edge12);
    BW2.Add(newW2, edge22);

    // history
    myEdgeFace.Bind(firstEdge, face);
  }

  if (uClosed && w1Point && w2Point)
  {
    shell.Closed(true);
  }

  if (myIsSolid)
  {

    if (vClosed)
    {

      TopoDS_Solid solid;
      B.MakeSolid(solid);
      B.Add(solid, shell);

      // verify the orientation the solid
      BRepClass3d_SolidClassifier clas3d(solid);
      clas3d.PerformInfinitePoint(Precision::Confusion());
      if (clas3d.State() == TopAbs_IN)
      {
        B.MakeSolid(solid);
        TopoDS_Shape aLocalShape = shell.Reversed();
        B.Add(solid, TopoDS::Shell(aLocalShape));
        //	B.Add(solid, TopoDS::Shell(shell.Reversed()));
      }
      myShape = solid;
    }

    else
    {
      myShape = MakeSolid(shell, newW1, newW2, myPres3d, myFirst, myLast);
    }
  }

  else
  {
    myShape = shell;
  }
  Done();

  NCollection_DataMap<TopoDS_Shape, double, TopTools_ShapeMapHasher> aVertexToleranceMap;
  TopExp_Explorer aTopExplorer(myShape, TopAbs_EDGE);
  while (aTopExplorer.More())
  {
    const TopoDS_Edge& aCurEdge = TopoDS::Edge(aTopExplorer.Current());
    B.SameRange(aCurEdge, false);
    B.SameParameter(aCurEdge, false);
    double aTolerance = BRep_Tool::Tolerance(aCurEdge);
    if (myMutableInput)
    {
      BRepLib::SameParameter(aCurEdge, aTolerance);
    }
    else
    {
      // all edges from myShape can be safely updated/changed
      // all vertices from myShape are the part of the original wires
      double aNewTolerance = -1;
      BRepLib::SameParameter(aCurEdge, aTolerance, aNewTolerance, true);
      if (aNewTolerance > 0)
      {
        TopoDS_Vertex aVertex1, aVertex2;
        TopExp::Vertices(aCurEdge, aVertex1, aVertex2);
        if (!aVertex1.IsNull())
        {
          const double* anOldTolerance = aVertexToleranceMap.Seek(aVertex1);
          if (!anOldTolerance || (anOldTolerance && *anOldTolerance < aNewTolerance))
          {
            aVertexToleranceMap.Bind(aVertex1, aNewTolerance);
          }
        }
        if (!aVertex2.IsNull())
        {
          const double* anOldTolerance = aVertexToleranceMap.Seek(aVertex2);
          if (!anOldTolerance || (anOldTolerance && *anOldTolerance < aNewTolerance))
          {
            aVertexToleranceMap.Bind(aVertex2, aNewTolerance);
          }
        }
      }
    }
    aTopExplorer.Next();
  }

  if (!myMutableInput)
  {
    BRepTools_ReShape                                                            aReshaper;
    NCollection_DataMap<TopoDS_Shape, double, TopTools_ShapeMapHasher>::Iterator aMapIterator(
      aVertexToleranceMap);
    for (; aMapIterator.More(); aMapIterator.Next())
    {
      const TopoDS_Vertex& aVertex       = TopoDS::Vertex(aMapIterator.Key());
      double               aNewTolerance = aMapIterator.Value();
      if (BRep_Tool::Tolerance(aVertex) < aNewTolerance)
      {
        TopoDS_Vertex aNnewVertex = TopoDS::Vertex(aVertex.EmptyCopied());
        B.UpdateVertex(aNnewVertex, aNewTolerance);
        aReshaper.Replace(aVertex, aNnewVertex);
      }
    }
    myShape = aReshaper.Apply(myShape);
  }
}

//=======================================================================
// function : EdgeToBSpline
// purpose  : auxiliary -- get curve from edge and convert it to bspline
//           parameterized from 0 to 1
//=======================================================================

// NOTE: this code duplicates the same function in BRepFill_NSections.cxx
static occ::handle<Geom_BSplineCurve> EdgeToBSpline(const TopoDS_Edge& theEdge)
{
  occ::handle<Geom_BSplineCurve> aBSCurve;
  if (BRep_Tool::Degenerated(theEdge))
  {
    // degenerated edge : construction of a point curve
    NCollection_Array1<double> aKnots(1, 2);
    aKnots(1) = 0.;
    aKnots(2) = 1.;

    NCollection_Array1<int> aMults(1, 2);
    aMults(1) = 2;
    aMults(2) = 2;

    NCollection_Array1<gp_Pnt> aPoles(1, 2);
    TopoDS_Vertex              vf, vl;
    TopExp::Vertices(theEdge, vl, vf);
    aPoles(1) = BRep_Tool::Pnt(vf);
    aPoles(2) = BRep_Tool::Pnt(vl);

    aBSCurve = new Geom_BSplineCurve(aPoles, aKnots, aMults, 1);
  }
  else
  {
    // get the curve of the edge
    TopLoc_Location         aLoc;
    double                  aFirst, aLast;
    occ::handle<Geom_Curve> aCurve = BRep_Tool::Curve(theEdge, aLoc, aFirst, aLast);
    if (aCurve.IsNull())
    {
      return nullptr;
    }

    // convert its part used by edge to bspline; note that if edge curve is bspline,
    // conversion made via trimmed curve is still needed -- it will copy it, segment
    // as appropriate, and remove periodicity if it is periodic (deadly for approximator)
    occ::handle<Geom_TrimmedCurve> aTrimCurve = new Geom_TrimmedCurve(aCurve, aFirst, aLast);

    // special treatment of conic curve
    if (aTrimCurve->BasisCurve()->IsKind(STANDARD_TYPE(Geom_Conic)))
    {
      const occ::handle<Geom_Curve>& aCurveTrimmed = aTrimCurve; // to avoid ambiguity
      GeomConvert_ApproxCurve anAppr(aCurveTrimmed, Precision::Confusion(), GeomAbs_C1, 16, 14);
      if (anAppr.HasResult())
      {
        aBSCurve = anAppr.Curve();
      }
    }

    // general case
    if (aBSCurve.IsNull())
    {
      aBSCurve = GeomConvert::CurveToBSplineCurve(aTrimCurve);
    }

    // apply transformation if needed
    if (!aLoc.IsIdentity())
    {
      aBSCurve->Transform(aLoc.Transformation());
    }

    // reparameterize to [0,1]
    NCollection_Array1<double> aKnots(aBSCurve->Knots());
    BSplCLib::Reparametrize(0., 1., aKnots);
    aBSCurve->SetKnots(aKnots);
  }

  // reverse curve if edge is reversed
  if (theEdge.Orientation() == TopAbs_REVERSED)
  {
    aBSCurve->Reverse();
  }

  return aBSCurve;
}

//=================================================================================================

static bool SupportTangentAt(const NCollection_Array1<TopoDS_Shape>&        theShapes,
                             const int                                      theNbEdges,
                             const int                                      theSectionIndex,
                             const occ::handle<Geom_BSplineCurve>&          theSection,
                             const occ::handle<Geom_BSplineCurve>&          theAdjacentSection,
                             const TopoDS_Face&                             theCommonSupport,
                             const BRepOffsetAPI_ThruSections::EdgeFaceMap& theEdgeSupports,
                             const bool                                     theIsFirst,
                             const bool                                     theNeedsCurvature,
                             const double                                   theFraction,
                             gp_Pnt&                                        thePoint,
                             gp_Vec&                                        theDirection,
                             gp_Vec&                                        theAcceleration,
                             gp_Vec&                                        theSectionTangent,
                             gp_Vec&                                        theNormal)
{
  const double aScaledFraction = theFraction * theNbEdges;
  const int    anEdgeIndex     = std::min(static_cast<int>(aScaledFraction) + 1, theNbEdges);
  const double anEdgeFraction =
    anEdgeIndex == theNbEdges && theFraction >= 1.0 ? 1.0 : aScaledFraction - (anEdgeIndex - 1);
  const TopoDS_Edge& anEdge =
    TopoDS::Edge(theShapes((theSectionIndex - 1) * theNbEdges + anEdgeIndex));

  TopoDS_Face aSupport = theCommonSupport;
  if (!theEdgeSupports.IsEmpty())
  {
    const TopoDS_Shape* aMappedSupport = theEdgeSupports.Seek(anEdge);
    if (aMappedSupport == nullptr || aMappedSupport->ShapeType() != TopAbs_FACE)
    {
      return false;
    }
    aSupport = TopoDS::Face(*aMappedSupport);
  }
  if (aSupport.IsNull())
  {
    return false;
  }

  const double aTolerance = std::max(
    {Precision::Confusion(), BRep_Tool::Tolerance(anEdge), BRep_Tool::Tolerance(aSupport)});

  const double aParameter =
    theSection->FirstParameter()
    + theFraction * (theSection->LastParameter() - theSection->FirstParameter());
  theSection->D1(aParameter, thePoint, theSectionTangent);
  const gp_Pnt anAdjacentPoint = theAdjacentSection->Value(aParameter);
  const gp_Vec aProgression =
    theIsFirst ? gp_Vec(thePoint, anAdjacentPoint) : gp_Vec(anAdjacentPoint, thePoint);

  double                    aFirst = 0.0, aLast = 0.0;
  occ::handle<Geom2d_Curve> aPCurve = BRep_Tool::CurveOnSurface(anEdge, aSupport, aFirst, aLast);
  gp_Pnt2d                  aUV;
  if (!aPCurve.IsNull())
  {
    const double aPCurveParameter = anEdge.Orientation() == TopAbs_REVERSED
                                      ? aLast - anEdgeFraction * (aLast - aFirst)
                                      : aFirst + anEdgeFraction * (aLast - aFirst);
    aUV                           = aPCurve->Value(aPCurveParameter);
  }
  else
  {
    // The support may be a copied or otherwise geometrically coincident face
    // without a pcurve for the input edge. Project the section point in that case.
    TopLoc_Location           aLocation;
    occ::handle<Geom_Surface> aSurface = BRep_Tool::Surface(aSupport, aLocation);
    if (aSurface.IsNull())
    {
      return false;
    }
    const gp_Pnt aLocalPoint = thePoint.Transformed(aLocation.Transformation().Inverted());
    double       aUMin = 0.0, aUMax = 0.0, aVMin = 0.0, aVMax = 0.0;
    BRepTools::UVBounds(aSupport, aUMin, aUMax, aVMin, aVMax);
    GeomAPI_ProjectPointOnSurf
      aProjection(aLocalPoint, aSurface, aUMin, aUMax, aVMin, aVMax, Precision::Confusion());
    if (!aProjection.IsDone() || aProjection.NbPoints() < 1
        || aProjection.LowerDistance() > aTolerance)
    {
      return false;
    }
    double aU = 0.0, aV = 0.0;
    aProjection.LowerDistanceParameters(aU, aV);
    aUV.SetCoord(aU, aV);
  }

  BRepClass_FaceClassifier aClassifier(aSupport, aUV, aTolerance);
  if (aClassifier.State() != TopAbs_IN && aClassifier.State() != TopAbs_ON)
  {
    return false;
  }

  gp_Pnt              aSupportPoint;
  gp_Vec              aDU, aDV, aDUU, aDVV, aDUV;
  BRepAdaptor_Surface aSupportAdaptor(aSupport);
  if (theNeedsCurvature)
  {
    aSupportAdaptor.D2(aUV.X(), aUV.Y(), aSupportPoint, aDU, aDV, aDUU, aDVV, aDUV);
  }
  else
  {
    aSupportAdaptor.D1(aUV.X(), aUV.Y(), aSupportPoint, aDU, aDV);
  }
  if (aSupportPoint.Distance(thePoint) > aTolerance)
  {
    return false;
  }
  theNormal = aDU.Crossed(aDV);
  if (theNormal.SquareMagnitude() <= gp::Resolution())
  {
    return false;
  }

  theDirection =
    aProgression - theNormal.Multiplied(aProgression.Dot(theNormal) / theNormal.SquareMagnitude());
  const gp_Vec aTangentInPlane =
    theSectionTangent
    - theNormal.Multiplied(theSectionTangent.Dot(theNormal) / theNormal.SquareMagnitude());
  if (aTangentInPlane.SquareMagnitude() <= gp::Resolution())
  {
    return false;
  }
  theDirection -= aTangentInPlane.Multiplied(theDirection.Dot(aTangentInPlane)
                                             / aTangentInPlane.SquareMagnitude());
  if (theDirection.SquareMagnitude() <= gp::Resolution())
  {
    theDirection = theNormal.Crossed(aTangentInPlane);
    if (theDirection.SquareMagnitude() <= gp::Resolution())
    {
      return false;
    }
    if (theDirection.Dot(aProgression) < 0.0)
    {
      theDirection.Reverse();
    }
  }
  theDirection.Normalize();

  theAcceleration.SetCoord(0.0, 0.0, 0.0);
  if (!theNeedsCurvature)
  {
    return true;
  }

  const double anE          = aDU.SquareMagnitude();
  const double anF          = aDU.Dot(aDV);
  const double aG           = aDV.SquareMagnitude();
  const double aDeterminant = anE * aG - anF * anF;
  if (std::abs(aDeterminant) <= gp::Resolution())
  {
    return false;
  }
  const double aDirectionDU  = theDirection.Dot(aDU);
  const double aDirectionDV  = theDirection.Dot(aDV);
  const double aUProgression = (aG * aDirectionDU - anF * aDirectionDV) / aDeterminant;
  const double aVProgression = (anE * aDirectionDV - anF * aDirectionDU) / aDeterminant;
  theAcceleration            = aDUU.Multiplied(aUProgression * aUProgression)
                    + aDUV.Multiplied(2.0 * aUProgression * aVProgression)
                    + aDVV.Multiplied(aVProgression * aVProgression);
  return true;
}

//=================================================================================================

static bool SupportConstraintAt(const NCollection_Array1<TopoDS_Shape>&        theShapes,
                                const int                                      theNbEdges,
                                const int                                      theSectionIndex,
                                const occ::handle<Geom_BSplineCurve>&          theSection,
                                const occ::handle<Geom_BSplineCurve>&          theAdjacentSection,
                                const TopoDS_Face&                             theCommonSupport,
                                const BRepOffsetAPI_ThruSections::EdgeFaceMap& theEdgeSupports,
                                const bool                                     theIsFirst,
                                const bool                                     theNeedsCurvature,
                                const bool                                     theIsClosed,
                                const double                                   theFraction,
                                gp_Pnt&                                        thePoint,
                                gp_Vec&                                        theDirection,
                                gp_Vec&                                        theAcceleration,
                                gp_Vec&                                        theSectionTangent,
                                gp_Vec&                                        theNormal)
{
  const double aCanonicalFraction = theIsClosed && theFraction >= 1.0 ? 0.0 : theFraction;
  if (!SupportTangentAt(theShapes,
                        theNbEdges,
                        theSectionIndex,
                        theSection,
                        theAdjacentSection,
                        theCommonSupport,
                        theEdgeSupports,
                        theIsFirst,
                        theNeedsCurvature,
                        aCanonicalFraction,
                        thePoint,
                        theDirection,
                        theAcceleration,
                        theSectionTangent,
                        theNormal))
  {
    return false;
  }

  const double aScaledFraction = aCanonicalFraction * theNbEdges;
  const double aNearestVertex  = std::round(aScaledFraction);
  const bool   isAtVertex = std::abs(aScaledFraction - aNearestVertex) <= Precision::PConfusion();
  const bool   isInternalVertex = aNearestVertex > 0.0 && aNearestVertex < theNbEdges;
  if (!isAtVertex || (!theIsClosed && !isInternalVertex))
  {
    return true;
  }

  const double aPreviousFraction = aCanonicalFraction <= Precision::PConfusion()
                                     ? 1.0 - 1.0e-6 / theNbEdges
                                     : aCanonicalFraction - 1.0e-6 / theNbEdges;
  gp_Pnt       aPreviousPoint;
  gp_Vec       aPreviousDirection, aPreviousAcceleration, aPreviousTangent, aPreviousNormal;
  if (!SupportTangentAt(theShapes,
                        theNbEdges,
                        theSectionIndex,
                        theSection,
                        theAdjacentSection,
                        theCommonSupport,
                        theEdgeSupports,
                        theIsFirst,
                        theNeedsCurvature,
                        aPreviousFraction,
                        aPreviousPoint,
                        aPreviousDirection,
                        aPreviousAcceleration,
                        aPreviousTangent,
                        aPreviousNormal))
  {
    return false;
  }

  theDirection += aPreviousDirection;
  if (theDirection.SquareMagnitude() <= gp::Resolution())
  {
    return false;
  }
  theDirection.Normalize();
  theAcceleration += aPreviousAcceleration;
  theAcceleration *= 0.5;
  return true;
}

//=================================================================================================

struct SupportConstraintError
{
  double Angular   = 0.0;
  double Curvature = 0.0;
};

//=================================================================================================

template <typename Evaluator>
static bool ValidateSupportConstraintAdaptively(const int    theNbEdges,
                                                const int    theSamplesPerEdge,
                                                const bool   theSkipEdgeVertices,
                                                const bool   theNeedsCurvature,
                                                const double theAngularTolerance,
                                                const double theCurvatureTolerance,
                                                Evaluator&&  theEvaluator)
{
  constexpr int    THE_MIN_REFINEMENT_DEPTH = 1;
  constexpr int    THE_MAX_REFINEMENT_DEPTH = 5;
  constexpr double THE_REFINEMENT_RATIO     = 0.25;
  constexpr double THE_VARIATION_RATIO      = 0.05;

  const auto normalizedError = [&](const SupportConstraintError& theError) {
    double aRatio = theError.Angular / theAngularTolerance;
    if (theNeedsCurvature)
    {
      aRatio = std::max(aRatio, theError.Curvature / theCurvatureTolerance);
    }
    return aRatio;
  };
  const auto evaluate = [&](const double theFraction, SupportConstraintError& theError) {
    if (!theEvaluator(theFraction, theError) || !std::isfinite(theError.Angular)
        || (theNeedsCurvature && !std::isfinite(theError.Curvature)))
    {
      return false;
    }
    return theError.Angular <= theAngularTolerance
           && (!theNeedsCurvature || theError.Curvature <= theCurvatureTolerance);
  };
  const auto refine = [&](auto&&                        theRefine,
                          const double                  theFirstFraction,
                          const SupportConstraintError& theFirstError,
                          const double                  theLastFraction,
                          const SupportConstraintError& theLastError,
                          const int                     theDepth) -> bool {
    const double           aMiddleFraction = 0.5 * (theFirstFraction + theLastFraction);
    SupportConstraintError aMiddleError;
    if (!evaluate(aMiddleFraction, aMiddleError))
    {
      return false;
    }

    double aVariation =
      std::abs(aMiddleError.Angular - 0.5 * (theFirstError.Angular + theLastError.Angular))
      / theAngularTolerance;
    if (theNeedsCurvature)
    {
      aVariation = std::max(
        aVariation,
        std::abs(aMiddleError.Curvature - 0.5 * (theFirstError.Curvature + theLastError.Curvature))
          / theCurvatureTolerance);
    }
    const double aMaximumError = std::max({normalizedError(theFirstError),
                                           normalizedError(aMiddleError),
                                           normalizedError(theLastError)});
    if (theDepth >= THE_MAX_REFINEMENT_DEPTH
        || (theDepth >= THE_MIN_REFINEMENT_DEPTH && aMaximumError <= THE_REFINEMENT_RATIO
            && aVariation <= THE_VARIATION_RATIO))
    {
      return true;
    }
    return theRefine(theRefine,
                     theFirstFraction,
                     theFirstError,
                     aMiddleFraction,
                     aMiddleError,
                     theDepth + 1)
           && theRefine(theRefine,
                        aMiddleFraction,
                        aMiddleError,
                        theLastFraction,
                        theLastError,
                        theDepth + 1);
  };

  for (int anEdgeIndex = 0; anEdgeIndex < theNbEdges; ++anEdgeIndex)
  {
    const int  aFirstSample   = theSkipEdgeVertices ? 0 : -1;
    const int  aLastSample    = theSamplesPerEdge - 1;
    const auto sampleFraction = [&](const int theSampleIndex) {
      const double anEdgeFraction = theSkipEdgeVertices
                                      ? (theSampleIndex + 0.5) / theSamplesPerEdge
                                      : static_cast<double>(theSampleIndex + 1) / theSamplesPerEdge;
      return (anEdgeIndex + anEdgeFraction) / theNbEdges;
    };

    double                 aPreviousFraction = sampleFraction(aFirstSample);
    SupportConstraintError aPreviousError;
    if (!evaluate(aPreviousFraction, aPreviousError))
    {
      return false;
    }
    for (int aSampleIndex = aFirstSample + 1; aSampleIndex <= aLastSample; ++aSampleIndex)
    {
      const double           aCurrentFraction = sampleFraction(aSampleIndex);
      SupportConstraintError aCurrentError;
      if (!evaluate(aCurrentFraction, aCurrentError)
          || !refine(refine, aPreviousFraction, aPreviousError, aCurrentFraction, aCurrentError, 0))
      {
        return false;
      }
      aPreviousFraction = aCurrentFraction;
      aPreviousError    = aCurrentError;
    }
  }
  return true;
}

//=================================================================================================

static bool BuildSupportConstraintSections(
  const NCollection_Array1<TopoDS_Shape>&        theShapes,
  const int                                      theNbEdges,
  const int                                      theSectionIndex,
  const occ::handle<Geom_BSplineCurve>&          theSection,
  const occ::handle<Geom_BSplineCurve>&          theAdjacentSection,
  const TopoDS_Face&                             theCommonSupport,
  const BRepOffsetAPI_ThruSections::EdgeFaceMap& theEdgeSupports,
  const bool                                     theIsFirst,
  const GeomAbs_Shape                            theContinuity,
  const double                                   theAngularTolerance,
  const double                                   theCurvatureTolerance,
  occ::handle<Geom_BSplineCurve>&                theTangentSection,
  occ::handle<Geom_BSplineCurve>&                theCurvatureSection)
{
  theTangentSection.Nullify();
  theCurvatureSection.Nullify();
  if (theSection.IsNull() || theAdjacentSection.IsNull())
  {
    return false;
  }

  const bool   isClosed        = theSection->IsClosed();
  const double aFirstParameter = theSection->FirstParameter();
  const double aLastParameter  = theSection->LastParameter();
  for (int aSamplesPerEdge = 8; aSamplesPerEdge <= 64; aSamplesPerEdge *= 2)
  {
    // Tangent and curvature loci must use the boundary section denominator. Independent
    // rational representations preserve G1 directions but scale normal curvature by
    // W2 * W0 / (W1 * W1) after their homogeneous finite differences are taken.
    occ::handle<Geom_BSplineCurve> aBasis = occ::down_cast<Geom_BSplineCurve>(theSection->Copy());
    if (aBasis.IsNull() || aBasis->IsPeriodic())
    {
      return false;
    }

    const int aNbIntervals = theNbEdges * aSamplesPerEdge;
    for (int anInterval = 1; anInterval < aNbIntervals; ++anInterval)
    {
      const double aFraction     = static_cast<double>(anInterval) / aNbIntervals;
      const double aParameter    = aFirstParameter + aFraction * (aLastParameter - aFirstParameter);
      const int    aMultiplicity = anInterval % aSamplesPerEdge == 0 ? aBasis->Degree() : 1;
      aBasis->InsertKnot(aParameter, aMultiplicity, Precision::PConfusion(), false);
    }

    const int                         aNbPoles   = aBasis->NbPoles();
    const NCollection_Array1<double>& aFlatKnots = aBasis->KnotSequence();
    NCollection_Array1<double>        aParameters(1, aNbPoles);
    NCollection_Array1<int>           aContactOrders(1, aNbPoles);
    NCollection_Array1<gp_Pnt>        aTangentPoles(1, aNbPoles);
    NCollection_Array1<gp_Pnt>        aCurvaturePoles(1, aNbPoles);
    NCollection_Array1<double>        aTangentWeights(1, aNbPoles);
    NCollection_Array1<double>        aCurvatureWeights(1, aNbPoles);
    BSplCLib::BuildSchoenbergPoints(aBasis->Degree(), aFlatKnots, aParameters);
    aContactOrders.Init(0);

    bool isValid = true;
    try
    {
      for (int aPoleIndex = 1; aPoleIndex <= aNbPoles; ++aPoleIndex)
      {
        const double aFraction =
          (aParameters(aPoleIndex) - aFirstParameter) / (aLastParameter - aFirstParameter);
        gp_Pnt aPoint;
        gp_Vec aDirection, anAcceleration, aSectionTangent, aNormal;
        if (!SupportConstraintAt(theShapes,
                                 theNbEdges,
                                 theSectionIndex,
                                 theSection,
                                 theAdjacentSection,
                                 theCommonSupport,
                                 theEdgeSupports,
                                 theIsFirst,
                                 theContinuity == GeomAbs_G2,
                                 isClosed,
                                 std::clamp(aFraction, 0.0, 1.0),
                                 aPoint,
                                 aDirection,
                                 anAcceleration,
                                 aSectionTangent,
                                 aNormal))
        {
          return false;
        }
        aTangentPoles(aPoleIndex) = aPoint.Translated(aDirection);
        if (theContinuity == GeomAbs_G2)
        {
          aCurvaturePoles(aPoleIndex) =
            aPoint.Translated(aDirection.Multiplied(2.0) + anAcceleration);
        }

        double aWeight = 0.0;
        BSplCLib::D0(aParameters(aPoleIndex),
                     0,
                     aBasis->Degree(),
                     false,
                     aBasis->WeightsArray(),
                     nullptr,
                     aBasis->Knots(),
                     &aBasis->Multiplicities(),
                     aWeight);
        if (aWeight <= gp::Resolution())
        {
          return false;
        }
        aTangentWeights(aPoleIndex)   = aWeight;
        aCurvatureWeights(aPoleIndex) = aWeight;
      }

      int anInversionProblem = 0;
      BSplCLib::Interpolate(aBasis->Degree(),
                            aFlatKnots,
                            aParameters,
                            aContactOrders,
                            aTangentPoles,
                            aTangentWeights,
                            anInversionProblem);
      if (anInversionProblem != 0)
      {
        isValid = false;
      }
      if (isValid && theContinuity == GeomAbs_G2)
      {
        BSplCLib::Interpolate(aBasis->Degree(),
                              aFlatKnots,
                              aParameters,
                              aContactOrders,
                              aCurvaturePoles,
                              aCurvatureWeights,
                              anInversionProblem);
        isValid = anInversionProblem == 0;
      }
    }
    catch (Standard_Failure const&)
    {
      isValid = false;
    }
    if (!isValid)
    {
      continue;
    }

    for (int aPoleIndex = 1; aPoleIndex <= aNbPoles; ++aPoleIndex)
    {
      const double aBasisWeight = aBasis->Weight(aPoleIndex);
      if (aBasisWeight <= gp::Resolution() || aTangentWeights(aPoleIndex) <= gp::Resolution()
          || (theContinuity == GeomAbs_G2 && aCurvatureWeights(aPoleIndex) <= gp::Resolution()))
      {
        isValid = false;
        break;
      }
      aTangentPoles(aPoleIndex).ChangeCoord() *= aTangentWeights(aPoleIndex) / aBasisWeight;
      aTangentWeights(aPoleIndex) = aBasisWeight;
      if (theContinuity == GeomAbs_G2)
      {
        aCurvaturePoles(aPoleIndex).ChangeCoord() *= aCurvatureWeights(aPoleIndex) / aBasisWeight;
        aCurvatureWeights(aPoleIndex) = aBasisWeight;
      }
    }
    if (!isValid)
    {
      continue;
    }
    theTangentSection = new Geom_BSplineCurve(aTangentPoles,
                                              aTangentWeights,
                                              aBasis->Knots(),
                                              aBasis->Multiplicities(),
                                              aBasis->Degree());

    if (theContinuity == GeomAbs_G2)
    {
      theCurvatureSection = new Geom_BSplineCurve(aCurvaturePoles,
                                                  aCurvatureWeights,
                                                  aBasis->Knots(),
                                                  aBasis->Multiplicities(),
                                                  aBasis->Degree());
    }

    isValid = ValidateSupportConstraintAdaptively(
      theNbEdges,
      aSamplesPerEdge * 2,
      !theEdgeSupports.IsEmpty(),
      theContinuity == GeomAbs_G2,
      theAngularTolerance,
      theCurvatureTolerance,
      [&](const double aFraction, SupportConstraintError& theError) {
        gp_Pnt aPoint;
        gp_Vec aDirection, anAcceleration, aSectionTangent, aNormal;
        if (!SupportTangentAt(theShapes,
                              theNbEdges,
                              theSectionIndex,
                              theSection,
                              theAdjacentSection,
                              theCommonSupport,
                              theEdgeSupports,
                              theIsFirst,
                              theContinuity == GeomAbs_G2,
                              aFraction,
                              aPoint,
                              aDirection,
                              anAcceleration,
                              aSectionTangent,
                              aNormal))
        {
          return false;
        }
        const double aParameter = aFirstParameter + aFraction * (aLastParameter - aFirstParameter);
        const gp_Pnt aResultTangentPoint = theTangentSection->Value(aParameter);
        const gp_Vec aResultDirection(aPoint, aResultTangentPoint);
        const gp_Vec aResultNormal = aSectionTangent.Crossed(aResultDirection);
        if (aResultDirection.SquareMagnitude() <= gp::Resolution()
            || aResultNormal.SquareMagnitude() <= gp::Resolution()
            || aNormal.SquareMagnitude() <= gp::Resolution())
        {
          return false;
        }
        theError.Angular = std::acos(std::clamp(
          std::abs(aResultNormal.Dot(aNormal)) / (aResultNormal.Magnitude() * aNormal.Magnitude()),
          0.0,
          1.0));
        if (theContinuity == GeomAbs_G2)
        {
          if (theCurvatureSection.IsNull())
          {
            return false;
          }
          const gp_Pnt aResultCurvaturePoint = theCurvatureSection->Value(aParameter);
          gp_Vec       aResultAcceleration;
          aResultAcceleration.SetXYZ(aResultCurvaturePoint.XYZ() - 2.0 * aResultTangentPoint.XYZ()
                                     + aPoint.XYZ());
          // Once the boundary curve and tangent plane agree, G2 is controlled by
          // normal curvature in the independent transverse tangent direction.
          const double aNormalMagnitude = aNormal.Magnitude();
          const double aTargetCurvature =
            aNormal.Dot(anAcceleration) / (aNormalMagnitude * aDirection.SquareMagnitude());
          const double aResultCurvature = aNormal.Dot(aResultAcceleration)
                                          / (aNormalMagnitude * aResultDirection.SquareMagnitude());
          theError.Curvature = std::abs(aResultCurvature - aTargetCurvature);
        }
        return true;
      });
    if (isValid)
    {
      return true;
    }
  }
  theTangentSection.Nullify();
  theCurvatureSection.Nullify();
  return false;
}

//=================================================================================================

static bool ValidateSupportConstraint(
  const NCollection_Array1<TopoDS_Shape>&        theShapes,
  const int                                      theNbEdges,
  const int                                      theSectionIndex,
  const occ::handle<Geom_BSplineCurve>&          theSection,
  const occ::handle<Geom_BSplineCurve>&          theAdjacentSection,
  const TopoDS_Face&                             theCommonSupport,
  const BRepOffsetAPI_ThruSections::EdgeFaceMap& theEdgeSupports,
  const bool                                     theIsFirst,
  const GeomAbs_Shape                            theContinuity,
  const double                                   theAngularTolerance,
  const double                                   theCurvatureTolerance,
  const occ::handle<Geom_BSplineSurface>&        theSurface)
{
  if (theSurface.IsNull())
  {
    return false;
  }

  double aUFirst = 0.0, aULast = 0.0, aVFirst = 0.0, aVLast = 0.0;
  theSurface->Bounds(aUFirst, aULast, aVFirst, aVLast);
  return ValidateSupportConstraintAdaptively(
    theNbEdges,
    128,
    !theEdgeSupports.IsEmpty(),
    theContinuity == GeomAbs_G2,
    theAngularTolerance,
    theCurvatureTolerance,
    [&](const double aFraction, SupportConstraintError& theError) {
      gp_Pnt aSupportPoint;
      gp_Vec aDirection, anAcceleration, aSectionTangent, aSupportNormal;
      if (!SupportTangentAt(theShapes,
                            theNbEdges,
                            theSectionIndex,
                            theSection,
                            theAdjacentSection,
                            theCommonSupport,
                            theEdgeSupports,
                            theIsFirst,
                            theContinuity == GeomAbs_G2,
                            aFraction,
                            aSupportPoint,
                            aDirection,
                            anAcceleration,
                            aSectionTangent,
                            aSupportNormal))
      {
        return false;
      }

      gp_Pnt aSurfacePoint;
      gp_Vec aDU, aDV, aDUU, aDVV, aDUV;
      theSurface->D2(aUFirst + aFraction * (aULast - aUFirst),
                     theIsFirst ? aVFirst : aVLast,
                     aSurfacePoint,
                     aDU,
                     aDV,
                     aDUU,
                     aDVV,
                     aDUV);
      const gp_Vec aSurfaceNormal = aDU.Crossed(aDV);
      if (aSurfaceNormal.SquareMagnitude() <= gp::Resolution()
          || aSupportNormal.SquareMagnitude() <= gp::Resolution())
      {
        return false;
      }
      theError.Angular =
        std::acos(std::clamp(std::abs(aSurfaceNormal.Dot(aSupportNormal))
                               / (aSurfaceNormal.Magnitude() * aSupportNormal.Magnitude()),
                             0.0,
                             1.0));

      if (theContinuity == GeomAbs_G2)
      {
        if (aDirection.SquareMagnitude() <= gp::Resolution()
            || aDV.SquareMagnitude() <= gp::Resolution())
        {
          return false;
        }
        const double aSupportNormalMagnitude = aSupportNormal.Magnitude();
        const double aTargetCurvature        = aSupportNormal.Dot(anAcceleration)
                                        / (aSupportNormalMagnitude * aDirection.SquareMagnitude());
        const double aResultCurvature =
          aSupportNormal.Dot(aDVV) / (aSupportNormalMagnitude * aDV.SquareMagnitude());
        theError.Curvature = std::abs(aResultCurvature - aTargetCurvature);
      }
      return true;
    });
}

//=================================================================================================

static occ::handle<Geom_BSplineSurface> BuildTwoSectionSurface(
  const GeomFill_SectionGenerator& theSections)
{
  int aNbPoles = 0, aNbKnots = 0, aDegree = 0, aNbPoles2d = 0;
  theSections.GetShape(aNbPoles, aNbKnots, aDegree, aNbPoles2d);

  NCollection_Array1<gp_Pnt> aFirstPoles(1, aNbPoles), aLastPoles(1, aNbPoles);
  NCollection_Array1<gp_Vec> aFirstDerivatives(1, aNbPoles), aLastDerivatives(1, aNbPoles);
  NCollection_Array1<double> aFirstWeights(1, aNbPoles), aLastWeights(1, aNbPoles);
  NCollection_Array1<double> aFirstWeightDerivatives(1, aNbPoles),
    aLastWeightDerivatives(1, aNbPoles);
  NCollection_Array1<gp_Pnt2d> aDummyPoles2d(1, 1);
  NCollection_Array1<gp_Vec2d> aDummyDerivatives2d(1, 1);

  const bool hasFirstDerivative = theSections.Section(1,
                                                      aFirstPoles,
                                                      aFirstDerivatives,
                                                      aDummyPoles2d,
                                                      aDummyDerivatives2d,
                                                      aFirstWeights,
                                                      aFirstWeightDerivatives);
  const bool hasLastDerivative  = theSections.Section(2,
                                                     aLastPoles,
                                                     aLastDerivatives,
                                                     aDummyPoles2d,
                                                     aDummyDerivatives2d,
                                                     aLastWeights,
                                                     aLastWeightDerivatives);

  double aDerivativeScale = 0.0;
  for (int anIndex = 1; anIndex <= aNbPoles; ++anIndex)
  {
    aDerivativeScale += aFirstPoles(anIndex).Distance(aLastPoles(anIndex));
  }
  aDerivativeScale /= aNbPoles;
  if (aDerivativeScale <= Precision::Confusion())
  {
    aDerivativeScale = 1.0;
  }
  for (int anIndex = 1; anIndex <= aNbPoles; ++anIndex)
  {
    if (hasFirstDerivative && aFirstWeightDerivatives(anIndex) < 0.0)
    {
      const double aPositiveWeightScale = 0.95 * 3.0 * (aFirstWeights(anIndex) - gp::Resolution())
                                          / -aFirstWeightDerivatives(anIndex);
      aDerivativeScale = std::min(aDerivativeScale, aPositiveWeightScale);
    }
    if (hasLastDerivative && aLastWeightDerivatives(anIndex) > 0.0)
    {
      const double aPositiveWeightScale =
        0.95 * 3.0 * (aLastWeights(anIndex) - gp::Resolution()) / aLastWeightDerivatives(anIndex);
      aDerivativeScale = std::min(aDerivativeScale, aPositiveWeightScale);
    }
  }
  if (aDerivativeScale <= Precision::Confusion())
  {
    return nullptr;
  }

  NCollection_Array2<gp_Pnt> aSurfacePoles(1, aNbPoles, 1, 4);
  NCollection_Array2<double> aSurfaceWeights(1, aNbPoles, 1, 4);
  for (int anIndex = 1; anIndex <= aNbPoles; ++anIndex)
  {
    const double aFirstWeight      = aFirstWeights(anIndex);
    const double aLastWeight       = aLastWeights(anIndex);
    const gp_XYZ aFirstHomogeneous = aFirstPoles(anIndex).XYZ() * aFirstWeight;
    const gp_XYZ aLastHomogeneous  = aLastPoles(anIndex).XYZ() * aLastWeight;

    gp_XYZ aFirstControlHomogeneous;
    double aFirstControlWeight = 0.0;
    if (hasFirstDerivative)
    {
      const double aCoefficient = aDerivativeScale / 3.0;
      const gp_XYZ aDerivative  = aFirstPoles(anIndex).XYZ() * aFirstWeightDerivatives(anIndex)
                                 + aFirstDerivatives(anIndex).XYZ() * aFirstWeight;
      aFirstControlHomogeneous = aFirstHomogeneous + aDerivative * aCoefficient;
      aFirstControlWeight      = aFirstWeight + aFirstWeightDerivatives(anIndex) * aCoefficient;
    }
    else
    {
      aFirstControlHomogeneous = (2.0 * aFirstHomogeneous + aLastHomogeneous) / 3.0;
      aFirstControlWeight      = (2.0 * aFirstWeight + aLastWeight) / 3.0;
    }

    gp_XYZ aLastControlHomogeneous;
    double aLastControlWeight = 0.0;
    if (hasLastDerivative)
    {
      const double aCoefficient = aDerivativeScale / 3.0;
      const gp_XYZ aDerivative  = aLastPoles(anIndex).XYZ() * aLastWeightDerivatives(anIndex)
                                 + aLastDerivatives(anIndex).XYZ() * aLastWeight;
      aLastControlHomogeneous = aLastHomogeneous - aDerivative * aCoefficient;
      aLastControlWeight      = aLastWeight - aLastWeightDerivatives(anIndex) * aCoefficient;
    }
    else
    {
      aLastControlHomogeneous = (aFirstHomogeneous + 2.0 * aLastHomogeneous) / 3.0;
      aLastControlWeight      = (aFirstWeight + 2.0 * aLastWeight) / 3.0;
    }

    if (aFirstControlWeight <= gp::Resolution() || aLastControlWeight <= gp::Resolution())
    {
      return nullptr;
    }

    aSurfacePoles(anIndex, 1) = aFirstPoles(anIndex);
    aSurfacePoles(anIndex, 2).SetXYZ(aFirstControlHomogeneous / aFirstControlWeight);
    aSurfacePoles(anIndex, 3).SetXYZ(aLastControlHomogeneous / aLastControlWeight);
    aSurfacePoles(anIndex, 4)   = aLastPoles(anIndex);
    aSurfaceWeights(anIndex, 1) = aFirstWeight;
    aSurfaceWeights(anIndex, 2) = aFirstControlWeight;
    aSurfaceWeights(anIndex, 3) = aLastControlWeight;
    aSurfaceWeights(anIndex, 4) = aLastWeight;
  }

  NCollection_Array1<double> aUKnots(1, aNbKnots), aVKnots(1, 2);
  NCollection_Array1<int>    aUMults(1, aNbKnots), aVMults(1, 2);
  theSections.Knots(aUKnots);
  theSections.Mults(aUMults);
  aVKnots(1) = 0.0;
  aVKnots(2) = 1.0;
  aVMults(1) = 4;
  aVMults(2) = 4;

  return new Geom_BSplineSurface(aSurfacePoles,
                                 aSurfaceWeights,
                                 aUKnots,
                                 aVKnots,
                                 aUMults,
                                 aVMults,
                                 aDegree,
                                 3,
                                 theSections.IsPeriodic(),
                                 false);
}

//=================================================================================================

static bool BuildHermiteInterpolationMatrix(const int                         theDegree,
                                            const NCollection_Array1<double>& theFlatKnots,
                                            const NCollection_Array1<double>& theParameters,
                                            const NCollection_Array1<int>&    theContactOrders,
                                            math_Matrix&                      theMatrix)
{
  const int   anOrder = theDegree + 1;
  math_Matrix aBasis(1, anOrder, 1, anOrder, 0.0);
  theMatrix.Init(0.0);
  for (int aRow = theParameters.Lower(); aRow <= theParameters.Upper(); ++aRow)
  {
    const int aContactOrder = theContactOrders(aRow);
    int       aFirstBasis   = 0;
    if (BSplCLib::EvalBsplineBasis(aContactOrder,
                                   anOrder,
                                   theFlatKnots,
                                   theParameters(aRow),
                                   aFirstBasis,
                                   aBasis)
        != 0)
    {
      return false;
    }
    for (int aBasisIndex = 0; aBasisIndex < anOrder; ++aBasisIndex)
    {
      const int aColumn = aFirstBasis + aBasisIndex;
      if (aColumn >= theMatrix.LowerCol() && aColumn <= theMatrix.UpperCol())
      {
        theMatrix(aRow, aColumn) = aBasis(aContactOrder + 1, aBasisIndex + 1);
      }
    }
  }
  return true;
}

//=================================================================================================

static bool HasAcceptableResidual(const math_Matrix& theMatrix,
                                  const math_Vector& theRightHandSide,
                                  const math_Vector& theSolution)
{
  constexpr double THE_RELATIVE_TOLERANCE = 1.0e-10;
  double           aScale                 = 1.0;
  double           aMaxResidual           = 0.0;
  for (int aColumn = theMatrix.LowerCol(); aColumn <= theMatrix.UpperCol(); ++aColumn)
  {
    if (!std::isfinite(theSolution(aColumn)))
    {
      return false;
    }
  }
  for (int aRow = theMatrix.LowerRow(); aRow <= theMatrix.UpperRow(); ++aRow)
  {
    if (!std::isfinite(theRightHandSide(aRow)))
    {
      return false;
    }
    double aValue = 0.0;
    for (int aColumn = theMatrix.LowerCol(); aColumn <= theMatrix.UpperCol(); ++aColumn)
    {
      aValue += theMatrix(aRow, aColumn) * theSolution(aColumn);
    }
    if (!std::isfinite(aValue))
    {
      return false;
    }
    aScale       = std::max(aScale, std::abs(theRightHandSide(aRow)));
    aMaxResidual = std::max(aMaxResidual, std::abs(aValue - theRightHandSide(aRow)));
  }
  return std::isfinite(aMaxResidual) && aMaxResidual <= THE_RELATIVE_TOLERANCE * aScale;
}

//=================================================================================================

static bool SolveHermiteInterpolation(const math_Matrix&          theMatrix,
                                      const math_Gauss&           theGauss,
                                      math_SVD&                   theSVD,
                                      const int                   theDimension,
                                      NCollection_Array1<double>& theCoordinates)
{
  constexpr double THE_SVD_TOLERANCE = 1.0e-12;
  const int        aNbEquations      = theMatrix.RowNumber();
  math_Vector      aRightHandSide(1, aNbEquations);
  math_Vector      aSolution(1, aNbEquations);
  for (int aCoordinate = 0; aCoordinate < theDimension; ++aCoordinate)
  {
    for (int aRow = 1; aRow <= aNbEquations; ++aRow)
    {
      aRightHandSide(aRow) = theCoordinates(theDimension * (aRow - 1) + aCoordinate + 1);
    }

    bool isSolved = false;
    if (theGauss.IsDone())
    {
      theGauss.Solve(aRightHandSide, aSolution);
      isSolved = HasAcceptableResidual(theMatrix, aRightHandSide, aSolution);
    }
    if (!isSolved && theSVD.IsDone())
    {
      theSVD.Solve(aRightHandSide, aSolution, THE_SVD_TOLERANCE);
      isSolved = HasAcceptableResidual(theMatrix, aRightHandSide, aSolution);
    }
    if (!isSolved)
    {
      return false;
    }

    for (int aRow = 1; aRow <= aNbEquations; ++aRow)
    {
      theCoordinates(theDimension * (aRow - 1) + aCoordinate + 1) = aSolution(aRow);
    }
  }
  return true;
}

//=================================================================================================

static occ::handle<Geom_BSplineSurface> BuildCurvatureConstrainedSurface(
  const GeomFill_SectionGenerator& theSections,
  const int                        theNbSections,
  const int                        theMaxDegree,
  const Approx_ParametrizationType theParametrization)
{
  int aNbUPoles = 0, aNbUKnots = 0, aUDegree = 0, aNbPoles2d = 0;
  theSections.GetShape(aNbUPoles, aNbUKnots, aUDegree, aNbPoles2d);

  NCollection_Array2<gp_Pnt>   aSectionPoles(1, theNbSections, 1, aNbUPoles);
  NCollection_Array2<double>   aSectionWeights(1, theNbSections, 1, aNbUPoles);
  NCollection_Array1<gp_Pnt2d> aDummyPoles2d(1, std::max(1, aNbPoles2d));
  for (int aSectionIndex = 1; aSectionIndex <= theNbSections; ++aSectionIndex)
  {
    NCollection_Array1<gp_Pnt> aPoles(aSectionPoles(aSectionIndex, 1), 1, aNbUPoles);
    NCollection_Array1<double> aWeights(aSectionWeights(aSectionIndex, 1), 1, aNbUPoles);
    theSections.Section(aSectionIndex, aPoles, aDummyPoles2d, aWeights);
  }

  NCollection_Array1<gp_Vec> aFirstD1(1, aNbUPoles), aFirstD2(1, aNbUPoles), aLastD1(1, aNbUPoles),
    aLastD2(1, aNbUPoles);
  NCollection_Array1<double> aFirstDW(1, aNbUPoles), aFirstD2W(1, aNbUPoles), aLastDW(1, aNbUPoles),
    aLastD2W(1, aNbUPoles);
  NCollection_Array1<gp_Vec2d> aDummyD1Poles2d(1, std::max(1, aNbPoles2d)),
    aDummyD2Poles2d(1, std::max(1, aNbPoles2d));
  NCollection_Array1<gp_Pnt> aFirstPoles(aSectionPoles(1, 1), 1, aNbUPoles);
  NCollection_Array1<double> aFirstWeights(aSectionWeights(1, 1), 1, aNbUPoles);
  NCollection_Array1<gp_Pnt> aLastPoles(aSectionPoles(theNbSections, 1), 1, aNbUPoles);
  NCollection_Array1<double> aLastWeights(aSectionWeights(theNbSections, 1), 1, aNbUPoles);

  const bool hasFirstCurvature  = theSections.SectionWithCurvature(1,
                                                                  aFirstPoles,
                                                                  aFirstD1,
                                                                  aFirstD2,
                                                                  aDummyPoles2d,
                                                                  aDummyD1Poles2d,
                                                                  aDummyD2Poles2d,
                                                                  aFirstWeights,
                                                                  aFirstDW,
                                                                  aFirstD2W);
  const bool hasLastCurvature   = theSections.SectionWithCurvature(theNbSections,
                                                                 aLastPoles,
                                                                 aLastD1,
                                                                 aLastD2,
                                                                 aDummyPoles2d,
                                                                 aDummyD1Poles2d,
                                                                 aDummyD2Poles2d,
                                                                 aLastWeights,
                                                                 aLastDW,
                                                                 aLastD2W);
  const bool hasFirstDerivative = hasFirstCurvature
                                  || theSections.Section(1,
                                                         aFirstPoles,
                                                         aFirstD1,
                                                         aDummyPoles2d,
                                                         aDummyD1Poles2d,
                                                         aFirstWeights,
                                                         aFirstDW);
  const bool hasLastDerivative = hasLastCurvature
                                 || theSections.Section(theNbSections,
                                                        aLastPoles,
                                                        aLastD1,
                                                        aDummyPoles2d,
                                                        aDummyD1Poles2d,
                                                        aLastWeights,
                                                        aLastDW);

  const int aNbConstraints = theNbSections + (hasFirstDerivative ? 1 : 0)
                             + (hasLastDerivative ? 1 : 0) + (hasFirstCurvature ? 1 : 0)
                             + (hasLastCurvature ? 1 : 0);
  const int aMinimumDegree = std::min(aNbConstraints - 1, 5);
  if (theMaxDegree < aMinimumDegree)
  {
    return nullptr;
  }
  const int aVDegree = std::min({aNbConstraints - 1, theMaxDegree, BSplCLib::MaxDegree()});

  double aDerivativeScale = 0.0;
  for (int aSectionIndex = 1; aSectionIndex < theNbSections; ++aSectionIndex)
  {
    for (int aPoleIndex = 1; aPoleIndex <= aNbUPoles; ++aPoleIndex)
    {
      aDerivativeScale += aSectionPoles(aSectionIndex, aPoleIndex)
                            .Distance(aSectionPoles(aSectionIndex + 1, aPoleIndex));
    }
  }
  aDerivativeScale /= (theNbSections - 1) * aNbUPoles;
  if (aDerivativeScale <= Precision::Confusion())
  {
    return nullptr;
  }

  NCollection_Array1<double> aSectionParameters(1, theNbSections);
  aSectionParameters(1) = 0.0;
  if (theNbSections == 2)
  {
    aSectionParameters(2) = 1.0;
  }
  else if (theParametrization == Approx_ChordLength || theParametrization == Approx_Centripetal)
  {
    for (int aSectionIndex = 2; aSectionIndex <= theNbSections; ++aSectionIndex)
    {
      double aSquaredDistance = 0.0;
      for (int aPoleIndex = 1; aPoleIndex <= aNbUPoles; ++aPoleIndex)
      {
        aSquaredDistance += aSectionPoles(aSectionIndex - 1, aPoleIndex)
                              .SquareDistance(aSectionPoles(aSectionIndex, aPoleIndex));
      }
      const double aDistance = std::sqrt(aSquaredDistance);
      const double anIncrement =
        theParametrization == Approx_ChordLength ? aDistance : std::sqrt(aDistance);
      aSectionParameters(aSectionIndex) = aSectionParameters(aSectionIndex - 1) + anIncrement;
    }
    if (aSectionParameters(theNbSections) <= Precision::Confusion())
    {
      return nullptr;
    }
    const double aScale = (theNbSections - 1.0) / aSectionParameters(theNbSections);
    for (int aSectionIndex = 2; aSectionIndex <= theNbSections; ++aSectionIndex)
    {
      aSectionParameters(aSectionIndex) *= aScale;
    }
  }
  else
  {
    for (int aSectionIndex = 2; aSectionIndex <= theNbSections; ++aSectionIndex)
    {
      aSectionParameters(aSectionIndex) = aSectionIndex - 1.0;
    }
  }

  NCollection_Array1<double> aFlatVKnots(1, aVDegree + aNbConstraints + 1);
  for (int anIndex = 1; anIndex <= aVDegree + 1; ++anIndex)
  {
    aFlatVKnots(anIndex)                                      = aSectionParameters(1);
    aFlatVKnots(aFlatVKnots.Upper() - aVDegree - 1 + anIndex) = aSectionParameters(theNbSections);
  }

  NCollection_Array1<double> aParameters(1, aNbConstraints);
  NCollection_Array1<int>    aContactOrders(1, aNbConstraints);
  int                        aConstraintIndex = 1;
  aParameters(aConstraintIndex)               = aSectionParameters(1);
  aContactOrders(aConstraintIndex++)          = 0;
  if (hasFirstDerivative)
  {
    aParameters(aConstraintIndex)      = aSectionParameters(1);
    aContactOrders(aConstraintIndex++) = 1;
  }
  if (hasFirstCurvature)
  {
    aParameters(aConstraintIndex)      = aSectionParameters(1);
    aContactOrders(aConstraintIndex++) = 2;
  }
  for (int aSectionIndex = 2; aSectionIndex < theNbSections; ++aSectionIndex)
  {
    aParameters(aConstraintIndex)      = aSectionParameters(aSectionIndex);
    aContactOrders(aConstraintIndex++) = 0;
  }
  if (hasLastCurvature)
  {
    aParameters(aConstraintIndex)      = aSectionParameters(theNbSections);
    aContactOrders(aConstraintIndex++) = 2;
  }
  if (hasLastDerivative)
  {
    aParameters(aConstraintIndex)      = aSectionParameters(theNbSections);
    aContactOrders(aConstraintIndex++) = 1;
  }
  aParameters(aConstraintIndex)    = aSectionParameters(theNbSections);
  aContactOrders(aConstraintIndex) = 0;

  const int aNbInteriorKnots = aNbConstraints - aVDegree - 1;
  for (int aKnotIndex = 1; aKnotIndex <= aNbInteriorKnots; ++aKnotIndex)
  {
    double aKnot = 0.0;
    for (int aParameterIndex = aKnotIndex + 1; aParameterIndex <= aKnotIndex + aVDegree;
         ++aParameterIndex)
    {
      aKnot += aParameters(aParameterIndex);
    }
    aFlatVKnots(aVDegree + 1 + aKnotIndex) = aKnot / aVDegree;
  }

  math_Matrix anInterpolationMatrix(1, aNbConstraints, 1, aNbConstraints, 0.0);
  if (!BuildHermiteInterpolationMatrix(aVDegree,
                                       aFlatVKnots,
                                       aParameters,
                                       aContactOrders,
                                       anInterpolationMatrix))
  {
    return nullptr;
  }
  math_Gauss aGauss(anInterpolationMatrix);
  math_SVD   anSVD(anInterpolationMatrix);
  if (!aGauss.IsDone() && !anSVD.IsDone())
  {
    return nullptr;
  }

  NCollection_Array2<gp_Pnt> aSurfacePoles(1, aNbUPoles, 1, aNbConstraints);
  NCollection_Array2<double> aSurfaceWeights(1, aNbUPoles, 1, aNbConstraints);
  NCollection_Array1<double> aCoordinates(1, 4 * aNbConstraints);
  auto setCoordinates = [&](const int theIndex, const gp_XYZ& theXYZ, const double theWeight) {
    const int aCoordinateIndex         = 4 * (theIndex - 1) + 1;
    aCoordinates(aCoordinateIndex)     = theXYZ.X();
    aCoordinates(aCoordinateIndex + 1) = theXYZ.Y();
    aCoordinates(aCoordinateIndex + 2) = theXYZ.Z();
    aCoordinates(aCoordinateIndex + 3) = theWeight;
  };

  for (int aPoleIndex = 1; aPoleIndex <= aNbUPoles; ++aPoleIndex)
  {
    aConstraintIndex           = 1;
    const gp_Pnt& aFirstPole   = aSectionPoles(1, aPoleIndex);
    const double  aFirstWeight = aSectionWeights(1, aPoleIndex);
    setCoordinates(aConstraintIndex++, aFirstPole.XYZ() * aFirstWeight, aFirstWeight);
    if (hasFirstDerivative)
    {
      const gp_XYZ aD1 =
        aFirstPole.XYZ() * aFirstDW(aPoleIndex) + aFirstD1(aPoleIndex).XYZ() * aFirstWeight;
      setCoordinates(aConstraintIndex++,
                     aD1 * aDerivativeScale,
                     aFirstDW(aPoleIndex) * aDerivativeScale);
    }
    if (hasFirstCurvature)
    {
      const gp_XYZ aD2 = aFirstPole.XYZ() * aFirstD2W(aPoleIndex)
                         + aFirstD1(aPoleIndex).XYZ() * (2.0 * aFirstDW(aPoleIndex))
                         + aFirstD2(aPoleIndex).XYZ() * aFirstWeight;
      setCoordinates(aConstraintIndex++,
                     aD2 * (aDerivativeScale * aDerivativeScale),
                     aFirstD2W(aPoleIndex) * aDerivativeScale * aDerivativeScale);
    }
    for (int aSectionIndex = 2; aSectionIndex < theNbSections; ++aSectionIndex)
    {
      const double aWeight = aSectionWeights(aSectionIndex, aPoleIndex);
      setCoordinates(aConstraintIndex++,
                     aSectionPoles(aSectionIndex, aPoleIndex).XYZ() * aWeight,
                     aWeight);
    }
    const gp_Pnt& aLastPole   = aSectionPoles(theNbSections, aPoleIndex);
    const double  aLastWeight = aSectionWeights(theNbSections, aPoleIndex);
    if (hasLastCurvature)
    {
      const gp_XYZ aD2 = aLastPole.XYZ() * aLastD2W(aPoleIndex)
                         + aLastD1(aPoleIndex).XYZ() * (2.0 * aLastDW(aPoleIndex))
                         + aLastD2(aPoleIndex).XYZ() * aLastWeight;
      setCoordinates(aConstraintIndex++,
                     aD2 * (aDerivativeScale * aDerivativeScale),
                     aLastD2W(aPoleIndex) * aDerivativeScale * aDerivativeScale);
    }
    if (hasLastDerivative)
    {
      const gp_XYZ aD1 =
        aLastPole.XYZ() * aLastDW(aPoleIndex) + aLastD1(aPoleIndex).XYZ() * aLastWeight;
      setCoordinates(aConstraintIndex++,
                     aD1 * aDerivativeScale,
                     aLastDW(aPoleIndex) * aDerivativeScale);
    }
    setCoordinates(aConstraintIndex, aLastPole.XYZ() * aLastWeight, aLastWeight);

    if (!SolveHermiteInterpolation(anInterpolationMatrix, aGauss, anSVD, 4, aCoordinates))
    {
      return nullptr;
    }
    for (int aVIndex = 1; aVIndex <= aNbConstraints; ++aVIndex)
    {
      const int    aCoordinateIndex = 4 * (aVIndex - 1) + 1;
      const double aWeight          = aCoordinates(aCoordinateIndex + 3);
      if (aWeight <= gp::Resolution())
      {
        return nullptr;
      }
      aSurfacePoles(aPoleIndex, aVIndex)
        .SetCoord(aCoordinates(aCoordinateIndex) / aWeight,
                  aCoordinates(aCoordinateIndex + 1) / aWeight,
                  aCoordinates(aCoordinateIndex + 2) / aWeight);
      aSurfaceWeights(aPoleIndex, aVIndex) = aWeight;
    }
  }

  NCollection_DynamicArray<double> aDistinctVKnots;
  NCollection_DynamicArray<int>    aDistinctVMults;
  for (const double aKnot : aFlatVKnots)
  {
    if (aDistinctVKnots.IsEmpty() || aKnot != aDistinctVKnots.Last())
    {
      aDistinctVKnots.Append(aKnot);
      aDistinctVMults.Append(1);
    }
    else
    {
      ++aDistinctVMults.ChangeLast();
    }
  }
  NCollection_Array1<double> aUKnots(1, aNbUKnots), aVKnots(1, aDistinctVKnots.Length());
  NCollection_Array1<int>    aUMults(1, aNbUKnots), aVMults(1, aDistinctVMults.Length());
  theSections.Knots(aUKnots);
  theSections.Mults(aUMults);
  for (int aKnotIndex = 1; aKnotIndex <= aVKnots.Upper(); ++aKnotIndex)
  {
    aVKnots(aKnotIndex) = aDistinctVKnots[aKnotIndex - 1];
    aVMults(aKnotIndex) = aDistinctVMults[aKnotIndex - 1];
  }
  return new Geom_BSplineSurface(aSurfacePoles,
                                 aSurfaceWeights,
                                 aUKnots,
                                 aVKnots,
                                 aUMults,
                                 aVMults,
                                 aUDegree,
                                 aVDegree,
                                 theSections.IsPeriodic(),
                                 false);
}

//=================================================================================================

occ::handle<Geom_BSplineSurface> BRepOffsetAPI_ThruSections::TotalSurf(
  const NCollection_Array1<TopoDS_Shape>& shapes,
  const int                               NbSects,
  const int                               NbEdges,
  const bool                              w1Point,
  const bool                              w2Point,
  const bool                              vClosed)
{
  int           i, j, jdeb = 1, jfin = NbSects;
  TopoDS_Vertex vf, vl;

  GeomFill_SectionGenerator                          section;
  occ::handle<Geom_BSplineSurface>                   surface;
  occ::handle<Geom_BSplineCurve>                     BS, BS1;
  occ::handle<Geom_TrimmedCurve>                     curvTrim;
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aSectionCurves(1, NbSects);

  if (w1Point)
  {
    jdeb++;
    TopoDS_Edge edge = TopoDS::Edge(shapes(1));
    TopExp::Vertices(edge, vl, vf);
    NCollection_Array1<gp_Pnt> Extremities(1, 2);
    Extremities(1) = BRep_Tool::Pnt(vf);
    Extremities(2) = BRep_Tool::Pnt(vl);
    NCollection_Array1<double> Bounds(1, 2);
    Bounds(1) = 0.;
    Bounds(2) = 1.;
    NCollection_Array1<int> Mult(1, 2);
    Mult(1)                                = 2;
    Mult(2)                                = 2;
    occ::handle<Geom_BSplineCurve> BSPoint = new Geom_BSplineCurve(Extremities, Bounds, Mult, 1);
    section.AddCurve(BSPoint);
    aSectionCurves(1) = BSPoint;
  }

  if (w2Point)
  {
    jfin--;
  }

  for (j = jdeb; j <= jfin; j++)
  {

    // case of looping sections
    if (j == jfin && vClosed)
    {
      section.AddCurve(BS1);
      aSectionCurves(j) = BS1;
    }

    else
    {
      // read the first edge to initialise CompBS;
      TopoDS_Edge                    aPrevEdge = TopoDS::Edge(shapes((j - 1) * NbEdges + 1));
      occ::handle<Geom_BSplineCurve> curvBS    = EdgeToBSpline(aPrevEdge);
      if (curvBS.IsNull())
      {
        return nullptr;
      }

      // initialization
      GeomConvert_CompCurveToBSplineCurve CompBS(curvBS);

      for (i = 2; i <= NbEdges; i++)
      {
        // read the edge
        TopoDS_Edge aNextEdge = TopoDS::Edge(shapes((j - 1) * NbEdges + i));
        double      aTolV     = Precision::Confusion();
        TopExp::Vertices(aNextEdge, vf, vl);
        aTolV  = std::max(aTolV, BRep_Tool::Tolerance(vf));
        aTolV  = std::max(aTolV, BRep_Tool::Tolerance(vl));
        aTolV  = std::min(aTolV, 1.e-3);
        curvBS = EdgeToBSpline(aNextEdge);
        if (curvBS.IsNull())
        {
          return nullptr;
        }

        // concatenation
        CompBS.Add(curvBS, aTolV, true, false, 1);
      }

      // return the final section
      BS = CompBS.BSplineCurve();
      section.AddCurve(BS);
      aSectionCurves(j) = BS;

      // case of looping sections
      if (j == jdeb && vClosed)
      {
        BS1 = BS;
      }
    }
  }

  if (w2Point)
  {
    TopoDS_Edge edge = TopoDS::Edge(shapes(NbSects * NbEdges));
    TopExp::Vertices(edge, vl, vf);
    NCollection_Array1<gp_Pnt> Extremities(1, 2);
    Extremities(1) = BRep_Tool::Pnt(vf);
    Extremities(2) = BRep_Tool::Pnt(vl);
    NCollection_Array1<double> Bounds(1, 2);
    Bounds(1) = 0.;
    Bounds(2) = 1.;
    NCollection_Array1<int> Mult(1, 2);
    Mult(1)                                = 2;
    Mult(2)                                = 2;
    occ::handle<Geom_BSplineCurve> BSPoint = new Geom_BSplineCurve(Extremities, Bounds, Mult, 1);
    section.AddCurve(BSPoint);
    aSectionCurves(NbSects) = BSPoint;
  }

  if (myHasFirstTangent)
  {
    if (myFirstTangentIsSupport)
    {
      occ::handle<Geom_BSplineCurve> aTangentSection, aCurvatureSection;
      if (!BuildSupportConstraintSections(shapes,
                                          NbEdges,
                                          1,
                                          aSectionCurves(1),
                                          aSectionCurves(2),
                                          myFirstSupport,
                                          myFirstWorkingEdgeSupports,
                                          true,
                                          myFirstBoundaryContinuity,
                                          myFirstSupportAngularTolerance,
                                          myFirstSupportCurvatureTolerance,
                                          aTangentSection,
                                          aCurvatureSection))
      {
        myStatus = BRepFill_ThruSectionErrorStatus_InvalidBoundaryConstraint;
        return nullptr;
      }
      section.SetFirstTangentSection(aTangentSection);
      if (myFirstBoundaryContinuity == GeomAbs_G2)
      {
        section.SetFirstCurvatureSection(aCurvatureSection);
      }
    }
    else
    {
      gp_Vec aTangent = myFirstTangent;
      if (myFirstTangentIsNormal
          && !SectionNormal(TopoDS::Wire(myWires(1)),
                            TopoDS_Wire(),
                            TopoDS::Wire(myWires(2)),
                            myFirstNormalTolerance,
                            aTangent))
      {
        myStatus = BRepFill_ThruSectionErrorStatus_InvalidBoundaryConstraint;
        return nullptr;
      }
      section.SetFirstTangent(aTangent);
      if (myFirstBoundaryContinuity == GeomAbs_G2)
      {
        section.SetFirstCurvature(gp_Vec());
      }
    }
  }

  if (myHasLastTangent)
  {
    if (myLastTangentIsSupport)
    {
      occ::handle<Geom_BSplineCurve> aTangentSection, aCurvatureSection;
      if (!BuildSupportConstraintSections(shapes,
                                          NbEdges,
                                          NbSects,
                                          aSectionCurves(NbSects),
                                          aSectionCurves(NbSects - 1),
                                          myLastSupport,
                                          myLastWorkingEdgeSupports,
                                          false,
                                          myLastBoundaryContinuity,
                                          myLastSupportAngularTolerance,
                                          myLastSupportCurvatureTolerance,
                                          aTangentSection,
                                          aCurvatureSection))
      {
        myStatus = BRepFill_ThruSectionErrorStatus_InvalidBoundaryConstraint;
        return nullptr;
      }
      section.SetLastTangentSection(aTangentSection);
      if (myLastBoundaryContinuity == GeomAbs_G2)
      {
        section.SetLastCurvatureSection(aCurvatureSection);
      }
    }
    else
    {
      gp_Vec aTangent = myLastTangent;
      if (myLastTangentIsNormal
          && !SectionNormal(TopoDS::Wire(myWires(NbSects)),
                            TopoDS::Wire(myWires(NbSects - 1)),
                            TopoDS_Wire(),
                            myLastNormalTolerance,
                            aTangent))
      {
        myStatus = BRepFill_ThruSectionErrorStatus_InvalidBoundaryConstraint;
        return nullptr;
      }
      section.SetLastTangent(aTangent);
      if (myLastBoundaryContinuity == GeomAbs_G2)
      {
        section.SetLastCurvature(gp_Vec());
      }
    }
  }

  section.Perform(Precision::PConfusion());
  const auto validatedSurface = [&](occ::handle<Geom_BSplineSurface> theSurface) {
    const bool isFirstValid = !myFirstTangentIsSupport
                              || ValidateSupportConstraint(shapes,
                                                           NbEdges,
                                                           1,
                                                           aSectionCurves(1),
                                                           aSectionCurves(2),
                                                           myFirstSupport,
                                                           myFirstWorkingEdgeSupports,
                                                           true,
                                                           myFirstBoundaryContinuity,
                                                           myFirstSupportAngularTolerance,
                                                           myFirstSupportCurvatureTolerance,
                                                           theSurface);
    const bool isLastValid = !myLastTangentIsSupport
                             || ValidateSupportConstraint(shapes,
                                                          NbEdges,
                                                          NbSects,
                                                          aSectionCurves(NbSects),
                                                          aSectionCurves(NbSects - 1),
                                                          myLastSupport,
                                                          myLastWorkingEdgeSupports,
                                                          false,
                                                          myLastBoundaryContinuity,
                                                          myLastSupportAngularTolerance,
                                                          myLastSupportCurvatureTolerance,
                                                          theSurface);
    if (!isFirstValid || !isLastValid)
    {
      myStatus = BRepFill_ThruSectionErrorStatus_InvalidBoundaryConstraint;
      theSurface.Nullify();
    }
    return theSurface;
  };

  const bool hasBoundaryCurvature = (myHasFirstTangent && myFirstBoundaryContinuity == GeomAbs_G2)
                                    || (myHasLastTangent && myLastBoundaryContinuity == GeomAbs_G2);
  if (NbSects == 2 && (myHasFirstTangent || myHasLastTangent) && !hasBoundaryCurvature)
  {
    return validatedSurface(BuildTwoSectionSurface(section));
  }
  if (hasBoundaryCurvature)
  {
    return validatedSurface(
      BuildCurvatureConstrainedSurface(section, NbSects, myDegMax, myParamType));
  }
  occ::handle<GeomFill_Line> line = new GeomFill_Line(NbSects);

  int nbIt = 3;
  if (myPres3d <= 1.e-3)
  {
    nbIt = 0;
  }

  int  degmin = 2, degmax = std::max(myDegMax, degmin);
  bool SpApprox = true;

  GeomFill_AppSurf anApprox(degmin, degmax, myPres3d, myPres3d, nbIt);
  anApprox.SetContinuity(myContinuity);

  if (myUseSmoothing)
  {
    anApprox.SetCriteriumWeight(myCritWeights[0], myCritWeights[1], myCritWeights[2]);
    anApprox.PerformSmoothing(line, section);
  }
  else
  {
    anApprox.SetParType(myParamType);
    anApprox.Perform(line, section, SpApprox);
  }

  if (anApprox.IsDone())
  {
    surface = new Geom_BSplineSurface(anApprox.SurfPoles(),
                                      anApprox.SurfWeights(),
                                      anApprox.SurfUKnots(),
                                      anApprox.SurfVKnots(),
                                      anApprox.SurfUMults(),
                                      anApprox.SurfVMults(),
                                      anApprox.UDegree(),
                                      anApprox.VDegree());
  }

  return validatedSurface(surface);
}

//=================================================================================================

const TopoDS_Shape& BRepOffsetAPI_ThruSections::FirstShape() const
{
  return myFirst;
}

//=================================================================================================

const TopoDS_Shape& BRepOffsetAPI_ThruSections::LastShape() const
{
  return myLast;
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& BRepOffsetAPI_ThruSections::Generated(const TopoDS_Shape& S)
{
  myGenerated.Clear();

  NCollection_Sequence<TopoDS_Shape> AllFaces;
  TopExp_Explorer                    Explo(myShape, TopAbs_FACE);
  for (; Explo.More(); Explo.Next())
  {
    AllFaces.Append(Explo.Current());
  }

  if (S.ShapeType() == TopAbs_EDGE)
  {
    if (!myEdgeNewIndices.IsBound(S))
    {
      return myGenerated;
    }

    const NCollection_List<int>& Indices = myEdgeNewIndices(S);
    // Append the faces corresponding to <Indices>
    // These faces "grow" from the first section
    NCollection_List<int>::Iterator itl(Indices);
    for (; itl.More(); itl.Next())
    {
      int IndOfFace = itl.Value();
      if (AllFaces.Length() < IndOfFace)
      {
        continue;
      }
      myGenerated.Append(AllFaces(IndOfFace));
    }

    if (myIsRuled)
    {
      // Append the next faces corresponding to <Indices>
      for (int i = 2; i < myWires.Length(); i++)
      {
        for (itl.Initialize(Indices); itl.More(); itl.Next())
        {
          int IndOfFace = itl.Value();
          IndOfFace += (i - 1) * myNbEdgesInSection;
          if (AllFaces.Length() < IndOfFace)
          {
            continue;
          }
          myGenerated.Append(AllFaces(IndOfFace));
        }
      }
    }
  }
  else if (S.ShapeType() == TopAbs_VERTEX)
  {
    if (!myVertexIndex.IsBound(S))
    {
      return myGenerated;
    }

    NCollection_IndexedDataMap<TopoDS_Shape,
                               NCollection_List<TopoDS_Shape>,
                               TopTools_ShapeMapHasher>
      VEmap;

    bool IsDegen[2] = {false, false};
    if (myDegen1 || myDegen2)
    {
      TopoDS_Shape EndSections[2];
      EndSections[0] = myWires(1);
      EndSections[1] = myWires(myWires.Length());
      for (int i = 0; i < 2; i++)
      {
        if (i == 0 && !myDegen1)
        {
          continue;
        }
        if (i == 1 && !myDegen2)
        {
          continue;
        }

        Explo.Init(EndSections[i], TopAbs_VERTEX);
        const TopoDS_Shape& aVertex = Explo.Current();
        if (S.IsSame(aVertex))
        {
          IsDegen[i] = true;
          break;
        }
      }
    }
    // Only one of <IsDegen> can be True:
    // in case of one vertex for start and end degenerated sections
    // IsDegen[0] is True;
    if (IsDegen[0] || IsDegen[1])
    {
      // For start or end degenerated section
      // we return the whole bunch of longitudinal edges
      TopExp::MapShapesAndAncestors(myShape, TopAbs_VERTEX, TopAbs_EDGE, VEmap);
      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> Emap;
      TopoDS_Shape                                                  aNewShape = S;
      if ((myIsRuled || !myMutableInput) && !myBFGenerator.IsNull())
      {
        aNewShape = myBFGenerator->ResultShape(S);
      }

      const NCollection_List<TopoDS_Shape>&    anEdgeList = VEmap.FindFromKey(aNewShape);
      NCollection_List<TopoDS_Shape>::Iterator aListIterator(anEdgeList);
      for (; aListIterator.More(); aListIterator.Next())
      {
        const TopoDS_Edge& anEdge = TopoDS::Edge(aListIterator.Value());
        if (!BRep_Tool::Degenerated(anEdge))
        {
          TopoDS_Vertex VV[2];
          TopExp::Vertices(anEdge, VV[0], VV[1]);
          // Comprehensive check for possible case of
          // one vertex for start and end degenerated sections:
          // we must take only outgoing or only ingoing edges
          if ((IsDegen[0] && aNewShape.IsSame(VV[0])) || (IsDegen[1] && aNewShape.IsSame(VV[1])))
          {
            Emap.Add(anEdge);
          }
        }
      }
      for (int j = 1; j <= Emap.Extent(); j++)
      {
        TopoDS_Edge anEdge = TopoDS::Edge(Emap(j));
        myGenerated.Append(anEdge);
        if (myIsRuled)
        {
          int i, k;
          for (i = 2, k = myWires.Length() - 1; i < myWires.Length(); i++, k--)
          {
            int           IndOfSec = (IsDegen[0]) ? i : k;
            TopoDS_Vertex aVertex =
              (IsDegen[0]) ? TopExp::LastVertex(anEdge) : TopExp::FirstVertex(anEdge);
            const NCollection_List<TopoDS_Shape>& EElist = VEmap.FindFromKey(aVertex);
            NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> EmapOfSection;
            TopoDS_Shape aWireSection = myWires(IndOfSec);
            if ((myIsRuled || !myMutableInput) && !myBFGenerator.IsNull())
            {
              aWireSection = myBFGenerator->ResultShape(aWireSection);
            }
            TopExp::MapShapes(aWireSection, TopAbs_EDGE, EmapOfSection);
            TopoDS_Edge NextEdge;
            for (aListIterator.Initialize(EElist); aListIterator.More(); aListIterator.Next())
            {
              NextEdge = TopoDS::Edge(aListIterator.Value());
              if (!NextEdge.IsSame(anEdge) && !EmapOfSection.Contains(NextEdge))
              {
                break;
              }
            }
            myGenerated.Append(NextEdge);
            anEdge = NextEdge;
          }
        }
      }
      return myGenerated;
    } // end of if (IsDegen[0] || IsDegen[1])

    int Eindex = myVertexIndex(S);
    int Vindex = (Eindex > 0) ? 0 : 1;
    Eindex     = std::abs(Eindex);

    // Find the first longitudinal edge
    TopoDS_Face FirstFace = TopoDS::Face(AllFaces(Eindex));
    FirstFace.Orientation(TopAbs_FORWARD);
    Explo.Init(FirstFace, TopAbs_EDGE);
    TopoDS_Edge         anEdge;
    BRepAdaptor_Surface BAsurf(FirstFace, false);
    TopoDS_Vertex       FirstVertex;
    TopExp::MapShapesAndAncestors(FirstFace, TopAbs_VERTEX, TopAbs_EDGE, VEmap);
    if (myDegen1 && BAsurf.GetType() == GeomAbs_Plane)
    {
      // There are only 3 edges in the face in this case:
      // we take 1-st or 3-rd edge
      if (Vindex == 0)
      {
        Explo.Next();
        Explo.Next();
      }
      anEdge = TopoDS::Edge(Explo.Current());
    }
    else
    {
      TopoDS_Edge            FirstEdge;
      TopoDS_Vertex          FirstVertexOfFirstEdge;
      const TopoDS_Wire&     FirstSection = TopoDS::Wire(myWires(1));
      BRepTools_WireExplorer aWireExplorer(FirstSection);
      for (int i = 1; aWireExplorer.More(); aWireExplorer.Next(), i++)
      {
        FirstEdge = aWireExplorer.Current();
        if (i == Eindex)
        {
          if ((myIsRuled || !myMutableInput) && !myBFGenerator.IsNull())
          {
            FirstEdge = TopoDS::Edge(myBFGenerator->ResultShape(FirstEdge));
          }
          FirstVertexOfFirstEdge = aWireExplorer.CurrentVertex();
          break;
        }
      }

      TopoDS_Shape FirstEdgeInFace;
      FirstEdgeInFace = Explo.Current();
      TopoDS_Vertex VV[2];
      TopExp::Vertices(FirstEdge, VV[0], VV[1]);
      if (Vindex == 0)
      {
        if (VV[0].IsSame(FirstVertexOfFirstEdge))
        {
          FirstVertex = VV[0];
        }
        else
        {
          FirstVertex = VV[1];
        }
      }
      else // Vindex == 1
      {
        if (VV[0].IsSame(FirstVertexOfFirstEdge))
        {
          FirstVertex = VV[1];
        }
        else
        {
          FirstVertex = VV[0];
        }
      }
      const NCollection_List<TopoDS_Shape>&    Elist = VEmap.FindFromKey(FirstVertex);
      NCollection_List<TopoDS_Shape>::Iterator itl(Elist);
      TopAbs_Orientation anEdgeOr = (Vindex == 0) ? TopAbs_REVERSED : TopAbs_FORWARD;
      for (; itl.More(); itl.Next())
      {
        anEdge = TopoDS::Edge(itl.Value());
        if (!anEdge.IsSame(FirstEdgeInFace) && !BRep_Tool::Degenerated(anEdge)
            && anEdge.Orientation() == anEdgeOr)
        {
          break;
        }
      }
    }
    myGenerated.Append(anEdge);
    if (myIsRuled)
    {
      // Find the chain of longitudinal edges from first to last
      for (int i = 2; i < myWires.Length(); i++)
      {
        FirstVertex                                  = TopExp::LastVertex(anEdge);
        const NCollection_List<TopoDS_Shape>& Elist1 = VEmap.FindFromKey(FirstVertex);
        TopoDS_Edge FirstEdge = (anEdge.IsSame(Elist1.First())) ? TopoDS::Edge(Elist1.Last())
                                                                : TopoDS::Edge(Elist1.First());
        Eindex += myNbEdgesInSection;
        FirstFace = TopoDS::Face(AllFaces(Eindex));
        FirstFace.Orientation(TopAbs_FORWARD);
        VEmap.Clear();
        TopExp::MapShapesAndAncestors(FirstFace, TopAbs_VERTEX, TopAbs_EDGE, VEmap);
        const NCollection_List<TopoDS_Shape>& Elist2 = VEmap.FindFromKey(FirstVertex);
        anEdge = (FirstEdge.IsSame(Elist2.First())) ? TopoDS::Edge(Elist2.Last())
                                                    : TopoDS::Edge(Elist2.First());
        myGenerated.Append(anEdge);
      }
    }
  }

  return myGenerated;
}

//=================================================================================================

TopoDS_Shape BRepOffsetAPI_ThruSections::GeneratedFace(const TopoDS_Shape& edge) const
{
  TopoDS_Shape bid;
  if (myEdgeFace.IsBound(edge))
  {
    return myEdgeFace(edge);
  }
  else
  {
    return bid;
  }
}

//=======================================================================
// function : CriteriumWeight
// purpose  : returns the Weights associated  to the criterium used in
//           the  optimization.
//=======================================================================
//
void BRepOffsetAPI_ThruSections::CriteriumWeight(double& W1, double& W2, double& W3) const
{
  W1 = myCritWeights[0];
  W2 = myCritWeights[1];
  W3 = myCritWeights[2];
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::SetCriteriumWeight(const double W1,
                                                    const double W2,
                                                    const double W3)
{
  if (W1 < 0 || W2 < 0 || W3 < 0)
  {
    myStatus = BRepFill_ThruSectionErrorStatus_Failed;
    return;
  }
  myCritWeights[0] = W1;
  myCritWeights[1] = W2;
  myCritWeights[2] = W3;
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::SetContinuity(const GeomAbs_Shape TheCont)
{
  myContinuity = TheCont;
}

//=================================================================================================

GeomAbs_Shape BRepOffsetAPI_ThruSections::Continuity() const
{
  return myContinuity;
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::SetParType(const Approx_ParametrizationType ParType)
{
  myParamType = ParType;
}

//=================================================================================================

Approx_ParametrizationType BRepOffsetAPI_ThruSections::ParType() const
{
  return myParamType;
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::SetMaxDegree(const int MaxDeg)
{
  myDegMax = MaxDeg;
}

//=================================================================================================

int BRepOffsetAPI_ThruSections::MaxDegree() const
{
  return myDegMax;
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::SetSmoothing(const bool UseVar)
{
  myUseSmoothing = UseVar;
}

//=================================================================================================

bool BRepOffsetAPI_ThruSections::UseSmoothing() const
{
  return myUseSmoothing;
}

//=================================================================================================

void BRepOffsetAPI_ThruSections::SetMutableInput(const bool theIsMutableInput)
{
  myMutableInput = theIsMutableInput;
}

//=================================================================================================

bool BRepOffsetAPI_ThruSections::IsMutableInput() const
{
  return myMutableInput;
}
