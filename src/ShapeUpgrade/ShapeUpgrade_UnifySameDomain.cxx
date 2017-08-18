// Created on: 2012-06-09
// Created by: jgv@ROLEX
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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
#include <BRep_CurveRepresentation.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <GC_MakeCircle.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom2dConvert_CompCurveToBSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ElementarySurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_SweptSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <GeomLib_IsPlanarSurface.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <IntPatch_ImpImpIntersection.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_WireOrder.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeExtend_CompositeSurface.hxx>
#include <ShapeFix_ComposeShell.hxx>
#include <ShapeFix_Edge.hxx>
#include <ShapeFix_Face.hxx>
#include <ShapeFix_SequenceOfWireSegment.hxx>
#include <ShapeFix_Shell.hxx>
#include <ShapeFix_Wire.hxx>
#include <ShapeFix_WireSegment.hxx>
#include <ShapeUpgrade_RemoveLocations.hxx>
#include <ShapeUpgrade_UnifySameDomain.hxx>
#include <Standard_Type.hxx>
#include <TColGeom2d_Array1OfBSplineCurve.hxx>
#include <TColGeom2d_HArray1OfBSplineCurve.hxx>
#include <TColGeom2d_SequenceOfBoundedCurve.hxx>
#include <TColGeom_Array1OfBSplineCurve.hxx>
#include <TColGeom_HArray1OfBSplineCurve.hxx>
#include <TColGeom_HArray2OfSurface.hxx>
#include <TColGeom_SequenceOfSurface.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <gp_Circ.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <gp_Vec2d.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeUpgrade_UnifySameDomain,Standard_Transient)

struct SubSequenceOfEdges
{
  TopTools_SequenceOfShape SeqsEdges;
  TopoDS_Edge UnionEdges;
};

static Standard_Boolean IsLikeSeam(const TopoDS_Edge& anEdge,
                                   const TopoDS_Face& aFace,
                                   const Handle(Geom_Surface)& aBaseSurface)
{
  if (!aBaseSurface->IsUPeriodic() && !aBaseSurface->IsVPeriodic())
    return Standard_False;

  BRepAdaptor_Curve2d BAcurve2d(anEdge, aFace);
  gp_Pnt2d FirstPoint, LastPoint;
  gp_Vec2d FirstDir, LastDir;
  BAcurve2d.D1(BAcurve2d.FirstParameter(), FirstPoint, FirstDir);
  BAcurve2d.D1(BAcurve2d.LastParameter(),  LastPoint,  LastDir);
  Standard_Real Length = FirstDir.Magnitude();
  if (Length <= gp::Resolution())
    return Standard_False;
  else
    FirstDir /= Length;
  Length = LastDir.Magnitude();
  if (Length <= gp::Resolution())
    return Standard_False;
  else
    LastDir /= Length;
  
  Standard_Real Tol = 1.e-7;
  if (aBaseSurface->IsUPeriodic() &&
    (Abs(FirstDir.X()) < Tol) &&
    (Abs(LastDir.X()) < Tol))
    return Standard_True;

  if (aBaseSurface->IsVPeriodic() &&
    (Abs(FirstDir.Y()) < Tol) &&
    (Abs(LastDir.Y()) < Tol))
    return Standard_True;

  return Standard_False;
}

static Standard_Boolean CheckSharedEdgeOri(const TopoDS_Face& theF1,
                                           const TopoDS_Face& theF2,
                                           const TopoDS_Edge& theE)
{
  TopAbs_Orientation anEOri = theE.Orientation();
  if (anEOri == TopAbs_EXTERNAL || anEOri == TopAbs_INTERNAL)
    return Standard_False;

  TopExp_Explorer Exp(theF1, TopAbs_EDGE);
  for (;Exp.More();Exp.Next())
  {
    const TopoDS_Shape& aCE = Exp.Current();
    if (aCE.IsSame(theE))
    {
      anEOri = aCE.Orientation(); 
      break;
    }
  }

  for (Exp.Init(theF2, TopAbs_EDGE);Exp.More();Exp.Next())
  {
    const TopoDS_Shape& aCE = Exp.Current();
    if (aCE.IsSame(theE))
    {
      if (aCE.Orientation() == TopAbs::Reverse(anEOri))
        return Standard_True;
      else
        return Standard_False;
    }
  }

  return Standard_False;

}

//=======================================================================
//function : AddOrdinaryEdges
//purpose  : auxilary
//=======================================================================
// adds edges from the shape to the sequence
// seams and equal edges are dropped
// Returns true if one of original edges dropped
static Standard_Boolean AddOrdinaryEdges(TopTools_SequenceOfShape& edges,
                                         const TopoDS_Shape aShape,
                                         Standard_Integer& anIndex)
{
  //map of edges
  TopTools_IndexedMapOfShape aNewEdges;
  //add edges without seams
  for(TopExp_Explorer exp(aShape,TopAbs_EDGE); exp.More(); exp.Next()) {
    TopoDS_Shape edge = exp.Current();
    if(aNewEdges.Contains(edge))
      aNewEdges.RemoveKey(edge);
    else
      aNewEdges.Add(edge);
  }

  Standard_Boolean isDropped = Standard_False;
  //merge edges and drop seams
  Standard_Integer i;
  for (i = 1; i <= edges.Length(); i++) {
    TopoDS_Shape current = edges(i);
    if(aNewEdges.Contains(current)) {

      aNewEdges.RemoveKey(current);
      edges.Remove(i);
      i--;

      if(!isDropped) {
        isDropped = Standard_True;
        anIndex = i;
      }
    }
  }

  //add edges to the sequence
  for (i = 1; i <= aNewEdges.Extent(); i++)
    edges.Append(aNewEdges(i));

  return isDropped;
}

//=======================================================================
//function : getCylinder
//purpose  : auxilary
//=======================================================================
static Standard_Boolean getCylinder(Handle(Geom_Surface)& theInSurface,
                                    gp_Cylinder& theOutCylinder)
{
  Standard_Boolean isCylinder = Standard_False;

  if (theInSurface->IsKind(STANDARD_TYPE(Geom_CylindricalSurface))) {
    Handle(Geom_CylindricalSurface) aGC = Handle(Geom_CylindricalSurface)::DownCast(theInSurface);

    theOutCylinder = aGC->Cylinder();
    isCylinder = Standard_True;
  }
  else if (theInSurface->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution))) {
    Handle(Geom_SurfaceOfRevolution) aRS =
      Handle(Geom_SurfaceOfRevolution)::DownCast(theInSurface);
    Handle(Geom_Curve) aBasis = aRS->BasisCurve();
    if (aBasis->IsKind(STANDARD_TYPE(Geom_Line))) {
      Handle(Geom_Line) aBasisLine = Handle(Geom_Line)::DownCast(aBasis);
      gp_Dir aDir = aRS->Direction();
      gp_Dir aBasisDir = aBasisLine->Position().Direction();
      if (aBasisDir.IsParallel(aDir, Precision::Angular())) {
        // basis line is parallel to the revolution axis: it is a cylinder
        gp_Pnt aLoc = aRS->Location();
        Standard_Real aR = aBasisLine->Lin().Distance(aLoc);
        gp_Ax3 aCylAx (aLoc, aDir);

        theOutCylinder = gp_Cylinder(aCylAx, aR);
        isCylinder = Standard_True;
      }
    }
  }
  else if (theInSurface->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))) {
    Handle(Geom_SurfaceOfLinearExtrusion) aLES =
      Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(theInSurface);
    Handle(Geom_Curve) aBasis = aLES->BasisCurve();
    if (aBasis->IsKind(STANDARD_TYPE(Geom_Circle))) {
      Handle(Geom_Circle) aBasisCircle = Handle(Geom_Circle)::DownCast(aBasis);
      gp_Dir aDir = aLES->Direction();
      gp_Dir aBasisDir = aBasisCircle->Position().Direction();
      if (aBasisDir.IsParallel(aDir, Precision::Angular())) {
        // basis circle is normal to the extrusion axis: it is a cylinder
        gp_Pnt aLoc = aBasisCircle->Location();
        Standard_Real aR = aBasisCircle->Radius();
        gp_Ax3 aCylAx (aLoc, aDir);

        theOutCylinder = gp_Cylinder(aCylAx, aR);
        isCylinder = Standard_True;
      }
    }
  }
  else {
  }

  return isCylinder;
}

//=======================================================================
//function : ClearRts
//purpose  : auxilary
//=======================================================================
static Handle(Geom_Surface) ClearRts(const Handle(Geom_Surface)& aSurface)
{
  if(aSurface->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
    Handle(Geom_RectangularTrimmedSurface) rts =
      Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurface);
    return rts->BasisSurface();
  }
  return aSurface;
}

//=======================================================================
//function : GetNormalToSurface
//purpose  : Gets the normal to surface by the given parameter on edge.
//           Returns True if normal was computed.
//=======================================================================
static Standard_Boolean GetNormalToSurface(const TopoDS_Face& theFace,
                                           const TopoDS_Edge& theEdge,
                                           const Standard_Real theP,
                                           gp_Dir& theNormal)
{
  Standard_Real f, l;
  // get 2d curve to get point in 2d
  const Handle(Geom2d_Curve)& aC2d = BRep_Tool::CurveOnSurface(theEdge, theFace, f, l);
  if (aC2d.IsNull()) {
    return Standard_False;
  }
  //
  // 2d point
  gp_Pnt2d aP2d;
  aC2d->D0(theP, aP2d);
  //
  // get D1
  gp_Vec aDU, aDV;
  gp_Pnt aP3d;
  TopLoc_Location aLoc;
  const Handle(Geom_Surface)& aS = BRep_Tool::Surface(theFace, aLoc);
  aS->D1(aP2d.X(), aP2d.Y(), aP3d, aDU, aDV);
  //
  // compute normal
  gp_Vec aVNormal = aDU.Crossed(aDV);
  if (aVNormal.Magnitude() < Precision::Confusion()) {
    return Standard_False;
  }
  //
  if (theFace.Orientation() == TopAbs_REVERSED) {
    aVNormal.Reverse();
  }
  //
  aVNormal.Transform(aLoc.Transformation());
  theNormal = gp_Dir(aVNormal);
  return Standard_True;
}

//=======================================================================
//function : IsSameDomain
//purpose  : 
//=======================================================================
static Standard_Boolean IsSameDomain(const TopoDS_Face& aFace,
                                     const TopoDS_Face& aCheckedFace,
                                     const Standard_Real theLinTol,
                                     const Standard_Real theAngTol)
{
  //checking the same handles
  TopLoc_Location L1, L2;
  Handle(Geom_Surface) S1, S2;

  S1 = BRep_Tool::Surface(aFace,L1);
  S2 = BRep_Tool::Surface(aCheckedFace,L2);

  if (S1 == S2 && L1 == L2)
    return Standard_True;

  S1 = BRep_Tool::Surface(aFace);
  S2 = BRep_Tool::Surface(aCheckedFace);

  S1 = ClearRts(S1);
  S2 = ClearRts(S2);

  //Handle(Geom_OffsetSurface) aGOFS1, aGOFS2;
  //aGOFS1 = Handle(Geom_OffsetSurface)::DownCast(S1);
  //aGOFS2 = Handle(Geom_OffsetSurface)::DownCast(S2);
  //if (!aGOFS1.IsNull()) S1 = aGOFS1->BasisSurface();
  //if (!aGOFS2.IsNull()) S2 = aGOFS2->BasisSurface();

  // case of two planar surfaces:
  // all kinds of surfaces checked, including b-spline and bezier
  GeomLib_IsPlanarSurface aPlanarityChecker1(S1, theLinTol);
  if (aPlanarityChecker1.IsPlanar()) {
    GeomLib_IsPlanarSurface aPlanarityChecker2(S2, theLinTol);
    if (aPlanarityChecker2.IsPlanar()) {
      gp_Pln aPln1 = aPlanarityChecker1.Plan();
      gp_Pln aPln2 = aPlanarityChecker2.Plan();

      if (aPln1.Position().Direction().IsParallel(aPln2.Position().Direction(), theAngTol) &&
        aPln1.Distance(aPln2) < theLinTol) {
        return Standard_True;
      }
    }
  }

  // case of two elementary surfaces: use OCCT tool
  // elementary surfaces: ConicalSurface, CylindricalSurface,
  //                      Plane, SphericalSurface and ToroidalSurface
  if (S1->IsKind(STANDARD_TYPE(Geom_ElementarySurface)) &&
      S2->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))
  {
    Handle(GeomAdaptor_HSurface) aGA1 = new GeomAdaptor_HSurface(S1);
    Handle(GeomAdaptor_HSurface) aGA2 = new GeomAdaptor_HSurface(S2);

    Handle(BRepTopAdaptor_TopolTool) aTT1 = new BRepTopAdaptor_TopolTool();
    Handle(BRepTopAdaptor_TopolTool) aTT2 = new BRepTopAdaptor_TopolTool();

    try {
      IntPatch_ImpImpIntersection anIIInt(aGA1, aTT1, aGA2, aTT2, theLinTol, theLinTol);
      if (!anIIInt.IsDone() || anIIInt.IsEmpty())
        return Standard_False;

      return anIIInt.TangentFaces();
    }
    catch (Standard_Failure) {
      return Standard_False;
    }
  }

  // case of two cylindrical surfaces, at least one of which is a swept surface
  // swept surfaces: SurfaceOfLinearExtrusion, SurfaceOfRevolution
  if ((S1->IsKind(STANDARD_TYPE(Geom_CylindricalSurface)) ||
       S1->IsKind(STANDARD_TYPE(Geom_SweptSurface))) &&
      (S2->IsKind(STANDARD_TYPE(Geom_CylindricalSurface)) ||
       S2->IsKind(STANDARD_TYPE(Geom_SweptSurface))))
  {
    gp_Cylinder aCyl1, aCyl2;
    if (getCylinder(S1, aCyl1) && getCylinder(S2, aCyl2)) {
      if (fabs(aCyl1.Radius() - aCyl2.Radius()) < theLinTol) {
        gp_Dir aDir1 = aCyl1.Position().Direction();
        gp_Dir aDir2 = aCyl2.Position().Direction();
        if (aDir1.IsParallel(aDir2, Precision::Angular())) {
          gp_Pnt aLoc1 = aCyl1.Location();
          gp_Pnt aLoc2 = aCyl2.Location();
          gp_Vec aVec12 (aLoc1, aLoc2);
          if (aVec12.SquareMagnitude() < theLinTol*theLinTol ||
              aVec12.IsParallel(aDir1, Precision::Angular())) {
            return Standard_True;
          }
        }
      }
    }
  }

  return Standard_False;
}

//=======================================================================
//function : UpdateMapOfShapes
//purpose  :
//=======================================================================
static void UpdateMapOfShapes(TopTools_MapOfShape& theMapOfShapes,
                              Handle(ShapeBuild_ReShape)& theContext)
{
  for (TopTools_MapIteratorOfMapOfShape it(theMapOfShapes); it.More(); it.Next()) {
    const TopoDS_Shape& aShape = it.Value();
    TopoDS_Shape aContextShape = theContext->Apply(aShape);
    if (!aContextShape.IsSame(aShape))
      theMapOfShapes.Add(aContextShape);
  }
}

//=======================================================================
//function : GlueEdgesWithPCurves
//purpose  : Glues the pcurves of the sequence of edges
//           and glues their 3d curves
//=======================================================================
static TopoDS_Edge GlueEdgesWithPCurves(const TopTools_SequenceOfShape& aChain,
                                        const TopoDS_Vertex& FirstVertex,
                                        const TopoDS_Vertex& LastVertex)
{
  Standard_Integer i, j;

  TopoDS_Edge FirstEdge = TopoDS::Edge(aChain(1));
  TColGeom_SequenceOfSurface SurfSeq;
  NCollection_Sequence<TopLoc_Location> LocSeq;
  
  for (int aCurveIndex = 0;; aCurveIndex++)
  {
    Handle(Geom2d_Curve) aCurve;
    Handle(Geom_Surface) aSurface;
    TopLoc_Location aLocation;
    Standard_Real aFirst, aLast;
    BRep_Tool::CurveOnSurface (FirstEdge, aCurve, aSurface, aLocation, aFirst, aLast, aCurveIndex);
    if (aCurve.IsNull())
      break;

    SurfSeq.Append(aSurface);
    LocSeq.Append(aLocation);
  }

  Standard_Real fpar, lpar;
  BRep_Tool::Range(FirstEdge, fpar, lpar);
  TopoDS_Edge PrevEdge = FirstEdge;
  TopoDS_Vertex CV;
  Standard_Real MaxTol = 0.;
  
  TopoDS_Edge ResEdge;
  BRep_Builder BB;

  Standard_Integer nb_curve = aChain.Length();   //number of curves
  TColGeom_Array1OfBSplineCurve tab_c3d(0,nb_curve-1);                    //array of the curves
  TColStd_Array1OfReal tabtolvertex(0,nb_curve-1); //(0,nb_curve-2);  //array of the tolerances
    
  TopoDS_Vertex PrevVertex = FirstVertex;
  for (i = 1; i <= nb_curve; i++)
  {
    TopoDS_Edge anEdge = TopoDS::Edge(aChain(i));
    TopoDS_Vertex VF, VL;
    TopExp::Vertices(anEdge, VF, VL);
    Standard_Boolean ToReverse = (!VF.IsSame(PrevVertex));
    
    Standard_Real Tol1 = BRep_Tool::Tolerance(VF);
    Standard_Real Tol2 = BRep_Tool::Tolerance(VL);
    if (Tol1 > MaxTol)
      MaxTol = Tol1;
    if (Tol2 > MaxTol)
      MaxTol = Tol2;
    
    if (i > 1)
    {
      TopExp::CommonVertex(PrevEdge, anEdge, CV);
      Standard_Real Tol = BRep_Tool::Tolerance(CV);
      tabtolvertex(i-2) = Tol;
    }
    
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, fpar, lpar);
    Handle(Geom_TrimmedCurve) aTrCurve = new Geom_TrimmedCurve(aCurve, fpar, lpar);
    tab_c3d(i-1) = GeomConvert::CurveToBSplineCurve(aTrCurve);
    GeomConvert::C0BSplineToC1BSplineCurve(tab_c3d(i-1), Precision::Confusion());
    if (ToReverse)
      tab_c3d(i-1)->Reverse();
    PrevVertex = (ToReverse)? VF : VL;
    PrevEdge = anEdge;
  }
  Handle(TColGeom_HArray1OfBSplineCurve)  concatcurve;     //array of the concatenated curves
  Handle(TColStd_HArray1OfInteger)        ArrayOfIndices;  //array of the remining Vertex
  GeomConvert::ConcatC1(tab_c3d,
                        tabtolvertex,
                        ArrayOfIndices,
                        concatcurve,
                        Standard_False,
                        Precision::Confusion());   //C1 concatenation
  
  if (concatcurve->Length() > 1)
  {
    GeomConvert_CompCurveToBSplineCurve Concat(concatcurve->Value(concatcurve->Lower()));
    
    for (i = concatcurve->Lower()+1; i <= concatcurve->Upper(); i++)
      Concat.Add( concatcurve->Value(i), MaxTol, Standard_True );
    
    concatcurve->SetValue(concatcurve->Lower(), Concat.BSplineCurve());
  }
  Handle(Geom_BSplineCurve) ResCurve = concatcurve->Value(concatcurve->Lower());
  
  TColGeom2d_SequenceOfBoundedCurve ResPCurves;
  for (j = 1; j <= SurfSeq.Length(); j++)
  {
    TColGeom2d_Array1OfBSplineCurve tab_c2d(0,nb_curve-1); //array of the pcurves
    
    PrevVertex = FirstVertex;
    PrevEdge = FirstEdge;
    for (i = 1; i <= nb_curve; i++)
    {
      TopoDS_Edge anEdge = TopoDS::Edge(aChain(i));
      TopoDS_Vertex VF, VL;
      TopExp::Vertices(anEdge, VF, VL);
      Standard_Boolean ToReverse = (!VF.IsSame(PrevVertex));

      Handle(Geom2d_Curve) aPCurve =
        BRep_Tool::CurveOnSurface(anEdge, SurfSeq(j), LocSeq(j), fpar, lpar);
      if (aPCurve.IsNull())
        continue;
      Handle(Geom2d_TrimmedCurve) aTrPCurve = new Geom2d_TrimmedCurve(aPCurve, fpar, lpar);
      tab_c2d(i-1) = Geom2dConvert::CurveToBSplineCurve(aTrPCurve);
      Geom2dConvert::C0BSplineToC1BSplineCurve(tab_c2d(i-1), Precision::Confusion());
      if (ToReverse)
        tab_c2d(i-1)->Reverse();
      PrevVertex = (ToReverse)? VF : VL;
      PrevEdge = anEdge;
    }
    Handle(TColGeom2d_HArray1OfBSplineCurve)  concatc2d;     //array of the concatenated curves
    Handle(TColStd_HArray1OfInteger)        ArrayOfInd2d;  //array of the remining Vertex
    Geom2dConvert::ConcatC1(tab_c2d,
                            tabtolvertex,
                            ArrayOfInd2d,
                            concatc2d,
                            Standard_False,
                            Precision::Confusion());   //C1 concatenation
    
    if (concatc2d->Length() > 1)
    {
      Geom2dConvert_CompCurveToBSplineCurve Concat2d(concatc2d->Value(concatc2d->Lower()));
      
      for (i = concatc2d->Lower()+1; i <= concatc2d->Upper(); i++)
        Concat2d.Add( concatc2d->Value(i), MaxTol, Standard_True );
      
      concatc2d->SetValue(concatc2d->Lower(), Concat2d.BSplineCurve());
    }
    Handle(Geom2d_BSplineCurve) aResPCurve = concatc2d->Value(concatc2d->Lower());
    ResPCurves.Append(aResPCurve);
  }
  
  ResEdge = BRepLib_MakeEdge(ResCurve,
                             FirstVertex, LastVertex,
                             ResCurve->FirstParameter(), ResCurve->LastParameter());
  BB.SameRange(ResEdge, Standard_False);
  BB.SameParameter(ResEdge, Standard_False);
  for (j = 1; j <= ResPCurves.Length(); j++)
  {
    BB.UpdateEdge(ResEdge, ResPCurves(j), SurfSeq(j), LocSeq(j), MaxTol);
    BB.Range(ResEdge, SurfSeq(j), LocSeq(j), ResPCurves(j)->FirstParameter(), ResPCurves(j)->LastParameter());
  }

  BRepLib::SameParameter(ResEdge, MaxTol, Standard_True);
  
  return ResEdge;
}

//=======================================================================
//function : MergeSubSeq
//purpose  : Merges a sequence of edges into one edge if possible
//=======================================================================

static Standard_Boolean MergeSubSeq(const TopTools_SequenceOfShape& aChain, 
                                    TopoDS_Edge& OutEdge, 
                                    double theAngTol, 
                                    Standard_Boolean ConcatBSplines,
                                    Standard_Boolean isSafeInputMode,
                                    Handle(ShapeBuild_ReShape)& theContext)
{
  ShapeAnalysis_Edge sae;
  BRep_Builder B;
  // union edges in chain
  int j;
  Standard_Real fp1,lp1,fp2,lp2;
  Standard_Boolean IsUnionOfLinesPossible = Standard_True;
  Standard_Boolean IsUnionOfCirclesPossible = Standard_True;
  Handle(Geom_Curve) c3d1, c3d2;
  for(j=1; j<aChain.Length(); j++) 
  {
    TopoDS_Edge edge1 = TopoDS::Edge(aChain.Value(j));
    c3d1 = BRep_Tool::Curve(edge1,fp1,lp1);

    TopoDS_Edge edge2 = TopoDS::Edge(aChain.Value(j+1));
    c3d2 = BRep_Tool::Curve(edge2,fp2,lp2);

    if(c3d1.IsNull() || c3d2.IsNull()) 
      return Standard_False;

    while(c3d1->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
      Handle(Geom_TrimmedCurve) tc =
        Handle(Geom_TrimmedCurve)::DownCast(c3d1);
      c3d1 = tc->BasisCurve();
    }
    while(c3d2->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
      Handle(Geom_TrimmedCurve) tc =
        Handle(Geom_TrimmedCurve)::DownCast(c3d2);
      c3d2 = tc->BasisCurve();
    }
    if( c3d1->IsKind(STANDARD_TYPE(Geom_Line)) && c3d2->IsKind(STANDARD_TYPE(Geom_Line)) ) {
      Handle(Geom_Line) L1 = Handle(Geom_Line)::DownCast(c3d1);
      Handle(Geom_Line) L2 = Handle(Geom_Line)::DownCast(c3d2);
      gp_Dir Dir1 = L1->Position().Direction();
      gp_Dir Dir2 = L2->Position().Direction();
      if(!Dir1.IsParallel(Dir2,theAngTol))  
        IsUnionOfLinesPossible = Standard_False;
    }
    else
      IsUnionOfLinesPossible = Standard_False;
    if( c3d1->IsKind(STANDARD_TYPE(Geom_Circle)) && c3d2->IsKind(STANDARD_TYPE(Geom_Circle)) ) {
      Handle(Geom_Circle) C1 = Handle(Geom_Circle)::DownCast(c3d1);
      Handle(Geom_Circle) C2 = Handle(Geom_Circle)::DownCast(c3d2);
      gp_Pnt P01 = C1->Location();
      gp_Pnt P02 = C2->Location();
      if (P01.Distance(P02) > Precision::Confusion())
        IsUnionOfCirclesPossible = Standard_False;
    }
    else
      IsUnionOfCirclesPossible = Standard_False;
  }
  if (IsUnionOfLinesPossible && IsUnionOfCirclesPossible)
    return Standard_False;

  //union of lines is possible
  if (IsUnionOfLinesPossible)
  {
    TopoDS_Vertex V[2];
    V[0] = sae.FirstVertex(TopoDS::Edge(aChain.First()));
    gp_Pnt PV1 = BRep_Tool::Pnt(V[0]);
    V[1] = sae.LastVertex(TopoDS::Edge(aChain.Last()));
    gp_Pnt PV2 = BRep_Tool::Pnt(V[1]);
    gp_Vec Vec(PV1, PV2);
    if (isSafeInputMode) {
      for (int k = 0; k < 2; k++) {
        if (!theContext->IsRecorded(V[k])) {
          TopoDS_Vertex Vcopy = TopoDS::Vertex(V[k].EmptyCopied());
          theContext->Replace(V[k], Vcopy);
          V[k] = Vcopy;
        }
        else
          V[k] = TopoDS::Vertex(theContext->Apply(V[k]));
      }
    }
    Handle(Geom_Line) L = new Geom_Line(gp_Ax1(PV1,Vec));
    Standard_Real dist = PV1.Distance(PV2);
    Handle(Geom_TrimmedCurve) tc = new Geom_TrimmedCurve(L,0.0,dist);
    TopoDS_Edge E;
    B.MakeEdge (E, tc ,Precision::Confusion());
    B.Add (E,V[0]);  B.Add (E,V[1]);
    B.UpdateVertex(V[0], 0., E, 0.);
    B.UpdateVertex(V[1], dist, E, 0.);
    OutEdge = E;
    return Standard_True;
  }

  if (IsUnionOfCirclesPossible)
  {
    double f,l;
    TopoDS_Edge FE = TopoDS::Edge(aChain.First());
    Handle(Geom_Curve) c3d = BRep_Tool::Curve(FE,f,l);

    while(c3d->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
      Handle(Geom_TrimmedCurve) tc =
        Handle(Geom_TrimmedCurve)::DownCast(c3d);
      c3d = tc->BasisCurve();
    }
    Handle(Geom_Circle) Cir = Handle(Geom_Circle)::DownCast(c3d);

    TopoDS_Vertex V[2];
    V[0] = sae.FirstVertex(FE);
    V[1] = sae.LastVertex(TopoDS::Edge(aChain.Last()));
    TopoDS_Edge E;
    if (V[0].IsSame(V[1])) {
      // closed chain
      BRepAdaptor_Curve adef(FE);
      Handle(Geom_Circle) Cir1;
      double FP, LP;
      if ( FE.Orientation() == TopAbs_FORWARD)
      {
        FP = adef.FirstParameter();
        LP = adef.LastParameter();
      }
      else
      {
        FP = adef.LastParameter();
        LP = adef.FirstParameter();
      }
      if (Abs(FP) < Precision::PConfusion())
      {
        B.MakeEdge (E,Cir, Precision::Confusion());
        B.Add(E,V[0]);
        B.Add(E,V[1]);
        E.Orientation(FE.Orientation());
      }
      else
      {
        GC_MakeCircle MC1 (adef.Value(FP), adef.Value((FP + LP) * 0.5), adef.Value(LP));
        if (MC1.IsDone())
          Cir1 = MC1.Value();
        else
          return Standard_False;
        B.MakeEdge (E, Cir1, Precision::Confusion());
        B.Add(E,V[0]);
        B.Add(E,V[1]);
      }
    }
    else {
      if (isSafeInputMode) {
        for (int k = 0; k < 2; k++) {
          if (!theContext->IsRecorded(V[k])) {
            TopoDS_Vertex Vcopy = TopoDS::Vertex(V[k].EmptyCopied());
            theContext->Replace(V[k], Vcopy);
            V[k] = Vcopy;
          }
          else
            V[k] = TopoDS::Vertex(theContext->Apply(V[k]));
        }
      }
      gp_Pnt PV1 = BRep_Tool::Pnt(V[0]);
      gp_Pnt PV2 = BRep_Tool::Pnt(V[1]);
      TopoDS_Vertex VM = sae.LastVertex(FE);
      gp_Pnt PVM = BRep_Tool::Pnt(VM);
      GC_MakeCircle MC (PV1,PVM,PV2);
      Handle(Geom_Circle) C = MC.Value();
      gp_Pnt P0 = C->Location();
      gp_Dir D1(gp_Vec(P0,PV1));
      gp_Dir D2(gp_Vec(P0,PV2));
      Standard_Real fpar = C->XAxis().Direction().Angle(D1);
      if(fabs(fpar)>Precision::Confusion()) {
        // check orientation
        gp_Dir ND =  C->XAxis().Direction().Crossed(D1);
        if(ND.IsOpposite(C->Axis().Direction(),Precision::Confusion())) {
          fpar = -fpar;
        }
      }
      Standard_Real lpar = C->XAxis().Direction().Angle(D2);
      if(fabs(lpar)>Precision::Confusion()) {
        // check orientation
        gp_Dir ND =  C->XAxis().Direction().Crossed(D2);
        if(ND.IsOpposite(C->Axis().Direction(),Precision::Confusion())) {
          lpar = -lpar;
        }
      }
      if (lpar < fpar) lpar += 2*M_PI;
      Handle(Geom_TrimmedCurve) tc = new Geom_TrimmedCurve(C,fpar,lpar);
      B.MakeEdge (E,tc,Precision::Confusion());
      B.Add(E,V[0]);
      B.Add(E,V[1]);
      B.UpdateVertex(V[0], fpar, E, 0.);
      B.UpdateVertex(V[1], lpar, E, 0.);
    }
    OutEdge = E;
    return Standard_True;
  }
  if (aChain.Length() > 1 && ConcatBSplines) {
    // second step: union edges with various curves
    // skl for bug 0020052 from Mantis: perform such unions
    // only if curves are bspline or bezier

    TopoDS_Vertex VF = sae.FirstVertex(TopoDS::Edge(aChain.First()));
    TopoDS_Vertex VL = sae.LastVertex(TopoDS::Edge(aChain.Last()));
    Standard_Boolean NeedUnion = Standard_True;
    for(j=1; j<=aChain.Length(); j++) {
      TopoDS_Edge edge = TopoDS::Edge(aChain.Value(j));
      TopLoc_Location Loc;
      Handle(Geom_Curve) c3d = BRep_Tool::Curve(edge,Loc,fp1,lp1);
      if(c3d.IsNull()) continue;
      while(c3d->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
        Handle(Geom_TrimmedCurve) tc =
          Handle(Geom_TrimmedCurve)::DownCast(c3d);
        c3d = tc->BasisCurve();
      }
      if( ( c3d->IsKind(STANDARD_TYPE(Geom_BSplineCurve)) ||
            c3d->IsKind(STANDARD_TYPE(Geom_BezierCurve)) ) ) continue;
      NeedUnion = Standard_False;
      break;
    }
    if(NeedUnion) {
#ifdef OCCT_DEBUG
      cout<<"can not make analitical union => make approximation"<<endl;
#endif
      TopoDS_Edge E = GlueEdgesWithPCurves(aChain, VF, VL);
      OutEdge = E;
      return Standard_True;
    }
    else {
#ifdef OCCT_DEBUG
      cout<<"can not make approximation for such types of curves"<<endl;
#endif
      return Standard_False;
    }
  }
  return Standard_False;
}

//=======================================================================
//function : IsMergingPossible
//purpose  : Checks if merging of two edges is possible
//=======================================================================

static Standard_Boolean IsMergingPossible(const TopoDS_Edge& edge1, const TopoDS_Edge& edge2, 
                                          double theAngTol, const TopTools_MapOfShape& AvoidEdgeVrt)
{
  TopoDS_Vertex CV = TopExp::LastVertex(edge1, Standard_True);
  if (CV.IsNull() || AvoidEdgeVrt.Contains(CV))
    return Standard_False;

  BRepAdaptor_Curve ade1(edge1);
  BRepAdaptor_Curve ade2(edge2);

  GeomAbs_CurveType t1 = ade1.GetType();
  GeomAbs_CurveType t2 = ade2.GetType();

  if( t1 == GeomAbs_Circle && t2 == GeomAbs_Circle)
  {
    if (ade1.Circle().Location().Distance(ade2.Circle().Location()) > Precision::Confusion())
      return Standard_False;
  }

  if( ( (t1 != GeomAbs_BezierCurve && t1 != GeomAbs_BSplineCurve) ||
      (t2 != GeomAbs_BezierCurve && t2 != GeomAbs_BSplineCurve)) && t1 != t2)
    return Standard_False;

  gp_Vec Diff1, Diff2;
  gp_Pnt P1, P2;
  if (edge1.Orientation() == TopAbs_FORWARD)
    ade1.D1(ade1.LastParameter(), P1, Diff1);
  else
  {
    ade1.D1(ade1.FirstParameter(), P1, Diff1);
    Diff1 = -Diff1;
  }

  if (edge2.Orientation() == TopAbs_FORWARD)
    ade2.D1(ade2.FirstParameter(), P2, Diff2);
  else
  {
    ade2.D1(ade2.LastParameter(), P2, Diff2);
    Diff2 = -Diff2;
  }

  if (Diff1.Angle(Diff2) > theAngTol)
    return Standard_False;

  return Standard_True;
}

//=======================================================================
//function : GenerateSubSeq
//purpose  : Generates sub-sequences of edges from sequence of edges
//Edges from each subsequences can be merged into the one edge  
//=======================================================================

static void GenerateSubSeq (const TopTools_SequenceOfShape& anInpEdgeSeq,
                            NCollection_Sequence<SubSequenceOfEdges>& SeqOfSubSeqOfEdges,
                            Standard_Boolean IsClosed, double theAngTol, const TopTools_MapOfShape& AvoidEdgeVrt)
{
  Standard_Boolean isOk = Standard_False;
  TopoDS_Edge edge1, edge2;

  SubSequenceOfEdges SubSeq;
  SubSeq.SeqsEdges.Append(TopoDS::Edge(anInpEdgeSeq(1)));
  SeqOfSubSeqOfEdges.Append(SubSeq);

  for (int i = 1; i < anInpEdgeSeq.Length(); i++)
  {
    edge1 = TopoDS::Edge(anInpEdgeSeq(i));
    edge2 = TopoDS::Edge(anInpEdgeSeq(i+1));
    isOk = IsMergingPossible(edge1, edge2, theAngTol, AvoidEdgeVrt);
    if (!isOk)
    {
      SubSequenceOfEdges aSubSeq;
      aSubSeq.SeqsEdges.Append(edge2);
      SeqOfSubSeqOfEdges.Append(aSubSeq);
    }
    else
      SeqOfSubSeqOfEdges.ChangeLast().SeqsEdges.Append(edge2);
  }
  /// check first and last chain segments
  if (IsClosed && SeqOfSubSeqOfEdges.Length() > 1)
  {
    edge1 = TopoDS::Edge(anInpEdgeSeq.Last());
    edge2 = TopoDS::Edge(anInpEdgeSeq.First());
    if (IsMergingPossible(edge1, edge2, theAngTol, AvoidEdgeVrt))
    {
      SeqOfSubSeqOfEdges.ChangeLast().SeqsEdges.Append(SeqOfSubSeqOfEdges.ChangeFirst().SeqsEdges);
      SeqOfSubSeqOfEdges.Remove(1);
    }
  }
}

//=======================================================================
//function : MergeEdges
//purpose  : auxilary
//=======================================================================
static Standard_Boolean MergeEdges(TopTools_SequenceOfShape& SeqEdges,
                                   const Standard_Real theAngTol,
                                   const Standard_Boolean ConcatBSplines,
                                   const Standard_Boolean isSafeInputMode,
                                   Handle(ShapeBuild_ReShape)& theContext,
                                   NCollection_Sequence<SubSequenceOfEdges>& SeqOfSubSeqOfEdges,
                                   const TopTools_MapOfShape& NonMergVrt)
{
  // skip degenerated edges, and forbid merging through them
  TopTools_IndexedDataMapOfShapeListOfShape aMapVE;
  Standard_Integer j;
  TopTools_MapOfShape VerticesToAvoid;
  for (j = 1; j <= SeqEdges.Length(); j++)
  {
    TopoDS_Edge anEdge = TopoDS::Edge(SeqEdges(j));
    if (BRep_Tool::Degenerated(anEdge))
    {
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(anEdge, V1, V2);
      VerticesToAvoid.Add(V1);
      VerticesToAvoid.Add(V2);
      SeqEdges.Remove(j--);
    }
    else
    {
      // fill in the map V-E
      for (TopoDS_Iterator it(anEdge.Oriented(TopAbs_FORWARD)); it.More(); it.Next())
      {
        TopoDS_Shape aV = it.Value();
        if (aV.Orientation() == TopAbs_FORWARD || aV.Orientation() == TopAbs_REVERSED)
        {
          if (!aMapVE.Contains(aV))
            aMapVE.Add(aV, TopTools_ListOfShape());
          aMapVE.ChangeFromKey(aV).Append(anEdge);
        }
      }
    }
  }
  VerticesToAvoid.Unite(NonMergVrt);

  // do loop while there are unused edges
  TopTools_MapOfShape aUsedEdges;
  for (;;)
  {
    TopoDS_Edge edge;
    for(j=1; j <= SeqEdges.Length(); j++)
    {
      edge = TopoDS::Edge(SeqEdges.Value(j));
      if (!aUsedEdges.Contains(edge))
        break;
    }
    if (j > SeqEdges.Length())
      break; // all edges have been used

    // make chain for unite
    TopTools_SequenceOfShape aChain;
    aChain.Append(edge);
    aUsedEdges.Add(edge);
    TopoDS_Vertex V[2];
    TopExp::Vertices(edge, V[0], V[1], Standard_True);

    // connect more edges to the chain in both directions
    for (j = 0; j < 2; j++)
    {
      Standard_Boolean isAdded = Standard_True;
      while (isAdded)
      {
        isAdded = Standard_False;
        if (V[j].IsNull())
          break;
        const TopTools_ListOfShape& aLE = aMapVE.FindFromKey(V[j]);
        for (TopTools_ListIteratorOfListOfShape itL(aLE); itL.More(); itL.Next())
        {
          edge = TopoDS::Edge(itL.Value());
          if (!aUsedEdges.Contains(edge))
          {
            TopoDS_Vertex V2[2];
            TopExp::Vertices(edge, V2[0], V2[1], Standard_True);
            // the neighboring edge must have V[j] reversed and located on the opposite end
            if (V2[1 - j].IsEqual(V[j].Reversed()))
            {
              if (j == 0)
                aChain.Prepend(edge);
              else
                aChain.Append(edge);
              aUsedEdges.Add(edge);
              V[j] = V2[j];
              isAdded = Standard_True;
              break;
            }
          }
        }
      }
    }

    if (aChain.Length() < 2)
      continue;

    Standard_Boolean IsClosed = Standard_False;
    if (V[0].IsSame ( V[1] ))
      IsClosed = Standard_True;

    // split chain by vertices at which merging is not possible
    NCollection_Sequence<SubSequenceOfEdges> aOneSeq;
    GenerateSubSeq(aChain, aOneSeq, IsClosed, theAngTol, VerticesToAvoid);

    // put sub-chains in the result
    SeqOfSubSeqOfEdges.Append(aOneSeq);
  }

  for (int i = 1; i <= SeqOfSubSeqOfEdges.Length(); i++)
  {
    TopoDS_Edge UE;
    if (SeqOfSubSeqOfEdges(i).SeqsEdges.Length() < 2)
      continue;
    if (MergeSubSeq(SeqOfSubSeqOfEdges(i).SeqsEdges, UE, theAngTol, 
                    ConcatBSplines, isSafeInputMode, theContext))
      SeqOfSubSeqOfEdges(i).UnionEdges = UE;
  }
  return Standard_True;
}

//=======================================================================
//function : MergeSeq
//purpose  : Tries to unify the sequence of edges with the set of
//           another edges which lies on the same geometry
//=======================================================================
static Standard_Boolean MergeSeq (TopTools_SequenceOfShape& SeqEdges,
                                  const Standard_Real theAngTol,
                                  const Standard_Boolean ConcatBSplines,
                                  const Standard_Boolean isSafeInputMode,
                                  Handle(ShapeBuild_ReShape)& theContext,
                                  const TopTools_MapOfShape& nonMergVert)
{
  NCollection_Sequence<SubSequenceOfEdges> SeqOfSubsSeqOfEdges;
  if (MergeEdges(SeqEdges, theAngTol, ConcatBSplines, isSafeInputMode,
                 theContext, SeqOfSubsSeqOfEdges, nonMergVert))
  {
    for (Standard_Integer i = 1; i <= SeqOfSubsSeqOfEdges.Length(); i++ )
    {
      if (SeqOfSubsSeqOfEdges(i).UnionEdges.IsNull())
        continue;

      theContext->Merge(SeqOfSubsSeqOfEdges(i).SeqsEdges,
        SeqOfSubsSeqOfEdges(i).UnionEdges);
    }
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : CheckSharedVertices
//purpose  : Checks the sequence of edges on the presence of shared vertex 
//=======================================================================

static void CheckSharedVertices(const TopTools_SequenceOfShape& theSeqEdges, 
                                const TopTools_IndexedDataMapOfShapeListOfShape& theMapEdgesVertex,
                                const TopTools_MapOfShape& theMapKeepShape,
                                TopTools_MapOfShape& theShareVertMap)
{
  ShapeAnalysis_Edge sae;
  TopTools_SequenceOfShape SeqVertexes;
  TopTools_MapOfShape MapVertexes;
  for (Standard_Integer k = 1; k <= theSeqEdges.Length(); k++ )
  {
    TopoDS_Vertex aV1 = sae.FirstVertex(TopoDS::Edge(theSeqEdges(k)));
    TopoDS_Vertex aV2 = sae.LastVertex(TopoDS::Edge(theSeqEdges(k)));
    if (!MapVertexes.Add(aV1))
      SeqVertexes.Append(aV1);
    if (!MapVertexes.Add(aV2))
      SeqVertexes.Append(aV2);
  }

  for (Standard_Integer k = 1; k <= SeqVertexes.Length()/* && !IsSharedVertexPresent*/; k++ )
  {
    const TopTools_ListOfShape& ListEdgesV1 = theMapEdgesVertex.FindFromKey(SeqVertexes(k));
    if (ListEdgesV1.Extent() > 2 || theMapKeepShape.Contains(SeqVertexes(k)))
      theShareVertMap.Add(SeqVertexes(k));
  }
  //return theShareVertMap.IsEmpty() ? false : true;
}

//=======================================================================
//function : ShapeUpgrade_UnifySameDomain
//purpose  : Constructor
//=======================================================================

ShapeUpgrade_UnifySameDomain::ShapeUpgrade_UnifySameDomain()
  : myLinTol(Precision::Confusion()),
    myAngTol(Precision::Angular()),
    myUnifyFaces(Standard_True),
    myUnifyEdges (Standard_True),
    myConcatBSplines (Standard_False),
    myAllowInternal (Standard_False),
    mySafeInputMode(Standard_True),
    myHistory(new BRepTools_History)
{
  myContext = new ShapeBuild_ReShape;
}

//=======================================================================
//function : ShapeUpgrade_UnifySameDomain
//purpose  : Constructor
//=======================================================================

ShapeUpgrade_UnifySameDomain::ShapeUpgrade_UnifySameDomain(const TopoDS_Shape& aShape,
                                                           const Standard_Boolean UnifyEdges,
                                                           const Standard_Boolean UnifyFaces,
                                                           const Standard_Boolean ConcatBSplines)
  : myInitShape (aShape),
    myLinTol(Precision::Confusion()),
    myAngTol(Precision::Angular()),
    myUnifyFaces(UnifyFaces),
    myUnifyEdges (UnifyEdges),
    myConcatBSplines (ConcatBSplines),
    myAllowInternal (Standard_False),
    mySafeInputMode (Standard_True),
    myShape (aShape),
    myHistory(new BRepTools_History)
{
  myContext = new ShapeBuild_ReShape;
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void ShapeUpgrade_UnifySameDomain::Initialize(const TopoDS_Shape& aShape,
                                              const Standard_Boolean UnifyEdges,
                                              const Standard_Boolean UnifyFaces,
                                              const Standard_Boolean ConcatBSplines)
{
  myInitShape = aShape;
  myShape = aShape;
  myUnifyEdges = UnifyEdges;
  myUnifyFaces = UnifyFaces;
  myConcatBSplines = ConcatBSplines;

  myContext->Clear();
  myKeepShapes.Clear();
  myHistory->Clear();
}

//=======================================================================
//function : AllowInternalEdges
//purpose  : 
//=======================================================================

void ShapeUpgrade_UnifySameDomain::AllowInternalEdges (const Standard_Boolean theValue)
{
  myAllowInternal = theValue;
}

//=======================================================================
//function : SetSafeInputMode
//purpose  : 
//=======================================================================

void ShapeUpgrade_UnifySameDomain::SetSafeInputMode(Standard_Boolean theValue)
{
  mySafeInputMode = theValue;
}

//=======================================================================
//function : KeepShape
//purpose  : 
//=======================================================================

void ShapeUpgrade_UnifySameDomain::KeepShape(const TopoDS_Shape& theShape)
{
  if (theShape.ShapeType() == TopAbs_EDGE || theShape.ShapeType() == TopAbs_VERTEX)
    myKeepShapes.Add(theShape);
}

//=======================================================================
//function : KeepShapes
//purpose  : 
//=======================================================================

void ShapeUpgrade_UnifySameDomain::KeepShapes(const TopTools_MapOfShape& theShapes)
{
  for (TopTools_MapIteratorOfMapOfShape it(theShapes); it.More(); it.Next()) {
    if (it.Value().ShapeType() == TopAbs_EDGE || it.Value().ShapeType() == TopAbs_VERTEX)
      myKeepShapes.Add(it.Value());
  }
}

//=======================================================================
//function : putIntWires
//purpose  : Add internal wires that are classified inside the face as a subshape,
//           and remove them from the sequence
//=======================================================================
static void putIntWires(TopoDS_Shape& theFace, TopTools_SequenceOfShape& theWires)
{
  TopoDS_Face& aFace = TopoDS::Face(theFace);
  for (Standard_Integer i=1; i <= theWires.Length(); i++)
  {
    TopoDS_Shape aWire = theWires(i);
    gp_Pnt2d aP2d;
    Standard_Boolean isP2d = Standard_False;
    for (TopoDS_Iterator it(aWire); it.More() && !isP2d; it.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(it.Value());
      Standard_Real aFirst, aLast;
      Handle(Geom2d_Curve) aC2d = BRep_Tool::CurveOnSurface(anEdge, aFace, aFirst, aLast);
      aC2d->D0((aFirst + aLast) * 0.5, aP2d);
      isP2d = Standard_True;
    }
    BRepClass_FaceClassifier aClass(aFace, aP2d, Precision::PConfusion());
    if (aClass.State() == TopAbs_IN)
    {
      BRep_Builder().Add(aFace, aWire);
      theWires.Remove(i);
      i--;
    }
  }
}

//=======================================================================
//function : UnifyFaces
//purpose  : 
//=======================================================================

void ShapeUpgrade_UnifySameDomain::UnifyFaces()
{
  // creating map of edge faces for the whole shape
  TopTools_IndexedDataMapOfShapeListOfShape aGMapEdgeFaces;
  TopExp::MapShapesAndAncestors(myShape, TopAbs_EDGE, TopAbs_FACE, aGMapEdgeFaces);
  
  // unify faces in each shell separately
  TopExp_Explorer exps;
  for (exps.Init(myShape, TopAbs_SHELL); exps.More(); exps.Next())
    IntUnifyFaces(exps.Current(), aGMapEdgeFaces, Standard_False);

  // gather all faces out of shells in one compound and unify them at once
  BRep_Builder aBB;
  TopoDS_Compound aCmp;
  aBB.MakeCompound(aCmp);
  Standard_Integer nbf = 0;
  for (exps.Init(myShape, TopAbs_FACE, TopAbs_SHELL); exps.More(); exps.Next(), nbf++)
    aBB.Add(aCmp, exps.Current());

  if (nbf > 0)
    IntUnifyFaces(aCmp, aGMapEdgeFaces, Standard_True);
  
  myShape = myContext->Apply(myShape);
}

//=======================================================================
//function : IntUnifyFaces
//purpose  : 
//=======================================================================

void ShapeUpgrade_UnifySameDomain::IntUnifyFaces(const TopoDS_Shape& theInpShape,
   TopTools_IndexedDataMapOfShapeListOfShape& theGMapEdgeFaces,
   Standard_Boolean IsCheckSharedEdgeOri)
{
  // creating map of edge faces for the shape
  TopTools_IndexedDataMapOfShapeListOfShape aMapEdgeFaces;
  TopExp::MapShapesAndAncestors(theInpShape, TopAbs_EDGE, TopAbs_FACE, aMapEdgeFaces);

  // map of processed shapes
  TopTools_MapOfShape aProcessed;

  // Check status of the unification
  Standard_Integer NbModif = 0;
  Standard_Boolean hasFailed = Standard_False;

  // processing each face
  TopExp_Explorer exp;
  for (exp.Init(theInpShape, TopAbs_FACE); exp.More(); exp.Next()) {
    const TopoDS_Face& aFaceOriginal = TopoDS::Face(exp.Current());
    TopoDS_Face aFace = TopoDS::Face(aFaceOriginal.Oriented(TopAbs_FORWARD));

    if (aProcessed.Contains(aFace))
      continue;

    // Boundary edges for the new face
    TopTools_SequenceOfShape edges;

    Standard_Integer dummy;
    AddOrdinaryEdges(edges, aFace, dummy);

    // Faces to get unified with the current faces
    TopTools_SequenceOfShape faces;

    // Add the current face for unification
    faces.Append(aFace);

    // surface and location to construct result
    TopLoc_Location aBaseLocation;
    Handle(Geom_Surface) aBaseSurface = BRep_Tool::Surface(aFace,aBaseLocation);
    aBaseSurface = ClearRts(aBaseSurface);

    // find adjacent faces to union
    Standard_Integer i;
    for (i = 1; i <= edges.Length(); i++) {
      TopoDS_Edge edge = TopoDS::Edge(edges(i));
      if (BRep_Tool::Degenerated(edge))
        continue;

      // get connectivity of the edge in the global shape
      const TopTools_ListOfShape& aGList = theGMapEdgeFaces.FindFromKey(edge);
      if (!myAllowInternal && (aGList.Extent() != 2 || myKeepShapes.Contains(edge))) {
        // non manifold case is not processed unless myAllowInternal
        continue;
      }
      //
      // Get the faces connected through the edge in the current shape
      const TopTools_ListOfShape& aList = aMapEdgeFaces.FindFromKey(edge);
      if (aList.Extent() < 2) {
        continue;
      }

      // get normal of the face to compare it with normals of other faces
      gp_Dir aDN1;
      //
      // take intermediate point on edge to compute the normal
      Standard_Real f, l;
      BRep_Tool::Range(edge, f, l);
      Standard_Real aTMid = (f + l) * .5;
      //
      Standard_Boolean bCheckNormals = GetNormalToSurface(aFaceOriginal, edge, aTMid, aDN1);
      //
      // Process the faces
      TopTools_ListIteratorOfListOfShape anIter(aList);
      for (; anIter.More(); anIter.Next()) {
        const TopoDS_Face& aCheckedFaceOriginal = TopoDS::Face(anIter.Value());
        TopoDS_Face anCheckedFace = TopoDS::Face(aCheckedFaceOriginal.Oriented(TopAbs_FORWARD));
        if (anCheckedFace.IsSame(aFace))
          continue;

        if (aProcessed.Contains(anCheckedFace))
          continue;

        if (IsCheckSharedEdgeOri && !CheckSharedEdgeOri(aFace, anCheckedFace, edge) )
          continue;

        if (bCheckNormals) {
          // get normal of checked face using the same parameter on edge
          gp_Dir aDN2;
          if (GetNormalToSurface(aCheckedFaceOriginal, edge, aTMid, aDN2)) {
            // and check if the adjacent faces are having approximately same normals
            Standard_Real anAngle = aDN1.Angle(aDN2);
            if (anAngle > myAngTol) {
              continue;
            }
          }
        }
        //
        if (IsSameDomain(aFace,anCheckedFace, myLinTol, myAngTol)) {

          // hotfix for 27271: prevent merging along periodic direction.
          if (IsLikeSeam(edge, anCheckedFace, aBaseSurface))
            continue;

          if (AddOrdinaryEdges(edges,anCheckedFace,dummy)) {
            // sequence edges is modified
            i = dummy;
          }

          faces.Append(anCheckedFace);
          aProcessed.Add(anCheckedFace);
          break;
        }
      }
    }

    if (faces.Length() > 1) {
      // fill in the connectivity map for selected faces
      TopTools_IndexedDataMapOfShapeListOfShape aMapEF;
      for (i = 1; i <= faces.Length(); i++) {
        TopExp::MapShapesAndAncestors(faces(i), TopAbs_EDGE, TopAbs_FACE, aMapEF);
      }
      if (mySafeInputMode)
        UpdateMapOfShapes(myKeepShapes, myContext);
      // Collect keep edges and multi-connected edges, i.e. edges that are internal to
      // the set of selected faces and have connections to other faces.
      TopTools_ListOfShape aKeepEdges;
      for (i = 1; i <= aMapEF.Extent(); i++) {
        const TopTools_ListOfShape& aLF = aMapEF(i);
        if (aLF.Extent() == 2) {
          const TopoDS_Shape& aE = aMapEF.FindKey(i);
          const TopTools_ListOfShape& aGLF = theGMapEdgeFaces.FindFromKey(aE);
          if (aGLF.Extent() > 2 || myKeepShapes.Contains(aE)) {
            aKeepEdges.Append(aE);
          }
        }
      } 
      if (!aKeepEdges.IsEmpty()) {
        if  (!myAllowInternal) {
          // Remove from the selection the faces which have no other connect edges 
          // and contain multi-connected edges and/or keep edges.
          TopTools_MapOfShape anAvoidFaces;
          TopTools_ListIteratorOfListOfShape it(aKeepEdges);
          for (; it.More(); it.Next()) {
            const TopoDS_Shape& aE = it.Value();
            const TopTools_ListOfShape& aLF = aMapEF.FindFromKey(aE);
            anAvoidFaces.Add(aLF.First());
            anAvoidFaces.Add(aLF.Last());
          }
          for (i = 1; i <= faces.Length(); i++) {
            if (anAvoidFaces.Contains(faces(i))) {
              // update the boundaries of merged area, for that
              // remove from 'edges' the edges of this face and add to 'edges' 
              // the edges of this face that were not present in 'edges' before
              Standard_Boolean hasConnectAnotherFaces = Standard_False;
              TopExp_Explorer ex(faces(i), TopAbs_EDGE);
              for (; ex.More() && !hasConnectAnotherFaces; ex.Next()) {
                TopoDS_Shape aE = ex.Current();
                const TopTools_ListOfShape& aLF = aMapEF.FindFromKey(aE);
                if (aLF.Extent() > 1) {
                  for (it.Init(aLF); it.More() && !hasConnectAnotherFaces; it.Next()) {
                    if (!anAvoidFaces.Contains(it.Value()))
                      hasConnectAnotherFaces = Standard_True;
                  }
                }
              }
              if (!hasConnectAnotherFaces) {
                AddOrdinaryEdges(edges, faces(i), dummy);
                faces.Remove(i);
                i--;
              }
            }
          }
          // check if the faces with keep edges contained in 
          // already updated the boundaries of merged area
          if (!faces.IsEmpty()) {
            TopTools_MapOfShape aMapFaces;
            for (i = 1; i <= faces.Length(); i++) {
              aMapFaces.Add(faces(i));
            }
            for (it.Init(aKeepEdges); it.More(); it.Next()) {
              const TopoDS_Shape& aE = it.Value();
              const TopTools_ListOfShape& aLF = aMapEF.FindFromKey(aE);
              if (aLF.Extent() < 2)
                continue;
              if (aMapFaces.Contains(aLF.First()) && 
                  aMapFaces.Contains(aLF.Last())) {
                for (i = 1; i <= faces.Length(); i++) {
                  if (faces(i).IsEqual(aLF.First()) ||
                      faces(i).IsEqual(aLF.Last())) {
                    AddOrdinaryEdges(edges, faces(i), dummy);
                    faces.Remove(i);
                    i--;
                  }
                }
              }
            }
          }
        }
        else {
          // add multi-connected and keep edges as internal in new face
          TopTools_ListIteratorOfListOfShape it(aKeepEdges);
          for (; it.More(); it.Next()) {
            const TopoDS_Shape& aE = it.Value();
            edges.Append(aE.Oriented(TopAbs_INTERNAL));
          }
        }
      }
    }

    // all faces collected in the sequence. Perform union of faces
    if (faces.Length() > 1) {
      NbModif++;
      TopoDS_Face aResult;
      BRep_Builder B;
      B.MakeFace(aResult,aBaseSurface,aBaseLocation,0);
      Standard_Integer nbWires = 0;

      TopoDS_Face tmpF = TopoDS::Face(myContext->Apply(faces(1).Oriented(TopAbs_FORWARD)));

      // connecting wires
      while (edges.Length()>0) {

        Standard_Boolean isEdge3d = Standard_False;
        nbWires++;
        TopTools_MapOfShape aVertices;
        TopoDS_Wire aWire;
        B.MakeWire(aWire);

        TopoDS_Edge anEdge = TopoDS::Edge(edges(1));
        edges.Remove(1);
        // collect internal edges in separate wires
        Standard_Boolean isInternal = (anEdge.Orientation() == TopAbs_INTERNAL);

        isEdge3d |= !BRep_Tool::Degenerated(anEdge);
        B.Add(aWire,anEdge);
        TopoDS_Vertex V1,V2;
        TopExp::Vertices(anEdge,V1,V2);
        aVertices.Add(V1);
        aVertices.Add(V2);

        Standard_Boolean isNewFound = Standard_False;
        do {
          isNewFound = Standard_False;
          for(Standard_Integer j = 1; j <= edges.Length(); j++) {
            anEdge = TopoDS::Edge(edges(j));
            // check if the current edge orientation corresponds to the first one
            Standard_Boolean isCurrInternal = (anEdge.Orientation() == TopAbs_INTERNAL);
            if (isCurrInternal != isInternal)
              continue;
            TopExp::Vertices(anEdge,V1,V2);
            if(aVertices.Contains(V1) || aVertices.Contains(V2)) {
              isEdge3d |= !BRep_Tool::Degenerated(anEdge);
              aVertices.Add(V1);
              aVertices.Add(V2);
              B.Add(aWire,anEdge);
              edges.Remove(j);
              j--;
              isNewFound = Standard_True;
            }
          }
        } while (isNewFound);

        // sorting any type of edges
        aWire.Closed (BRep_Tool::IsClosed (aWire));
        aWire = TopoDS::Wire(myContext->Apply(aWire));

        Handle(ShapeFix_Wire) sfw = new ShapeFix_Wire(aWire,tmpF,Precision::Confusion());
        if (mySafeInputMode)
          sfw->SetContext(myContext);
        sfw->FixEdgeCurves();
        sfw->FixReorder();
        Standard_Boolean isDegRemoved = Standard_False;
        if(!sfw->StatusReorder ( ShapeExtend_FAIL )) {
          // clear degenerated edges if at least one with 3d curve exist
          if(isEdge3d) {
            Handle(ShapeExtend_WireData) sewd = sfw->WireData();
            for(Standard_Integer j = 1; j<=sewd->NbEdges();j++) {
              TopoDS_Edge E = sewd->Edge(j);
              if(BRep_Tool::Degenerated(E)) {
                sewd->Remove(j);
                isDegRemoved = Standard_True;
                j--;
              }
            }
          }
          sfw->FixShifted();
          if(isDegRemoved)
            sfw->FixDegenerated();
        }
        TopoDS_Wire aWireFixed = sfw->Wire();
        myContext->Replace(aWire,aWireFixed);

        // add resulting wire
        if(isEdge3d) {
          B.Add(aResult,aWireFixed);
        }
        else  {
          // sorting edges
          Handle(ShapeExtend_WireData) sbwd = sfw->WireData();
          Standard_Integer nbEdges = sbwd->NbEdges();
          // sort degenerated edges and create one edge instead of several ones
          ShapeAnalysis_WireOrder sawo(Standard_False, 0);
          ShapeAnalysis_Edge sae;
          Standard_Integer aLastEdge = nbEdges;
          for(Standard_Integer j = 1; j <= nbEdges; j++) {
            Standard_Real f,l;
            //smh protection on NULL pcurve
            Handle(Geom2d_Curve) c2d;
            if(!sae.PCurve(sbwd->Edge(j),tmpF,c2d,f,l)) {
              aLastEdge--;
              continue;
            }
            sawo.Add(c2d->Value(f).XY(),c2d->Value(l).XY());
          }
          if (sawo.NbEdges() == 0)
            continue;
          sawo.Perform();

          // constructind one degenerative edge
          gp_XY aStart, anEnd, tmp;
          Standard_Integer nbFirst = sawo.Ordered(1);
          TopoDS_Edge anOrigE = TopoDS::Edge(sbwd->Edge(nbFirst).Oriented(TopAbs_FORWARD));
          ShapeBuild_Edge sbe;
          TopoDS_Vertex aDummyV;
          TopoDS_Edge E = sbe.CopyReplaceVertices(anOrigE,aDummyV,aDummyV);
          sawo.XY(nbFirst,aStart,tmp);
          sawo.XY(sawo.Ordered(aLastEdge),tmp,anEnd);

          gp_XY aVec = anEnd-aStart;
          Handle(Geom2d_Line) aLine = new Geom2d_Line(aStart,gp_Dir2d(anEnd-aStart));

          B.UpdateEdge(E,aLine,tmpF,0.);
          B.Range(E,tmpF,0.,aVec.Modulus());
          Handle(Geom_Curve) C3d;
          B.UpdateEdge(E,C3d,0.);
          B.Degenerated(E,Standard_True);
          TopoDS_Wire aW;
          B.MakeWire(aW);
          B.Add(aW,E);
          aW.Closed (Standard_True);
          B.Add(aResult,aW);
        }
      }

      // perform substitution of face
      myContext->Replace(myContext->Apply(aFace),aResult);

      ShapeFix_Face sff (aResult);
      //Initializing by tolerances
      sff.SetPrecision(Precision::Confusion());
      sff.SetMinTolerance(Precision::Confusion());
      sff.SetMaxTolerance(1.);
      //Setting modes
      sff.FixOrientationMode() = 0;
      //sff.FixWireMode() = 0;
      sff.SetContext(myContext);
      // Applying the fixes
      sff.Perform();
      if(sff.Status(ShapeExtend_FAIL))
        hasFailed = Standard_True;

      // breaking down to several faces
      TopoDS_Shape theResult = myContext->Apply(aResult);
      for (TopExp_Explorer aFaceExp (theResult,TopAbs_FACE); aFaceExp.More(); aFaceExp.Next()) {
        TopoDS_Face aCurrent = TopoDS::Face(aFaceExp.Current().Oriented(TopAbs_FORWARD));
        Handle(TColGeom_HArray2OfSurface) grid = new TColGeom_HArray2OfSurface ( 1, 1, 1, 1 );
        grid->SetValue ( 1, 1, aBaseSurface );
        Handle(ShapeExtend_CompositeSurface) G = new ShapeExtend_CompositeSurface ( grid );
        ShapeFix_ComposeShell CompShell;
        CompShell.Init ( G, aBaseLocation, aCurrent, ::Precision::Confusion() );//myPrecision
        CompShell.SetContext( myContext );

        TopTools_SequenceOfShape parts, anIntWires;
        ShapeFix_SequenceOfWireSegment wires;
        for(TopExp_Explorer W_Exp(aCurrent,TopAbs_WIRE);W_Exp.More();W_Exp.Next()) {
          const TopoDS_Wire& aWire = TopoDS::Wire(W_Exp.Current());
          // check if the wire is ordinary (contains non-internal edges)
          Standard_Boolean isInternal = Standard_True;
          for (TopoDS_Iterator it(aWire); it.More() && isInternal; it.Next())
            isInternal = (it.Value().Orientation() == TopAbs_INTERNAL);
          if (isInternal)
          {
            // place internal wire separately
            anIntWires.Append(aWire);
          }
          else
          {
            Handle(ShapeExtend_WireData) sbwd =
              new ShapeExtend_WireData (aWire);
            ShapeFix_WireSegment seg ( sbwd, TopAbs_REVERSED );
            wires.Append(seg);
          }
        }

        CompShell.DispatchWires( parts, wires );
        for (Standard_Integer j=1; j <= parts.Length(); j++ ) {
          ShapeFix_Face aFixOrient(TopoDS::Face(parts(j)));
          aFixOrient.SetContext(myContext);
          aFixOrient.FixOrientation();
          // put internal wires to faces
          putIntWires(parts(j), anIntWires);
        }

        TopoDS_Shape CompRes;
        if ( parts.Length() !=1 ) {
          TopoDS_Shell S;
          B.MakeShell ( S );
          for ( i=1; i <= parts.Length(); i++ )
            B.Add ( S, parts(i) );
          S.Closed (BRep_Tool::IsClosed (S));
          CompRes = S;
        }
        else CompRes = parts(1);

        myContext->Replace(aCurrent,CompRes);
      }

      const TopoDS_Shape aResult3 = myContext->Apply(theResult);
      myContext->Merge(faces, aResult3);
    }
  } // end processing each face

  //TopoDS_Shape aResult = Shape;
  if (NbModif > 0 && !hasFailed) {
    TopoDS_Shape aResult = myContext->Apply(theInpShape);

    ShapeFix_Edge sfe;
    if (!myContext.IsNull()) sfe.SetContext(myContext);
    for (exp.Init(aResult,TopAbs_EDGE); exp.More(); exp.Next()) {
      TopoDS_Edge E = TopoDS::Edge(exp.Current());
      sfe.FixVertexTolerance (E);
      // ptv add fix same parameter
      sfe.FixSameParameter(E, Precision::Confusion());
    }

    myContext->Replace(theInpShape, aResult);
  }
}

//=======================================================================
//function : UnifyEdges
//purpose  : 
//=======================================================================
void ShapeUpgrade_UnifySameDomain::UnifyEdges()
{
  TopoDS_Shape aRes = myContext->Apply(myShape);

  TopTools_IndexedMapOfShape ChangedFaces;

  // creating map of edge faces
  TopTools_IndexedDataMapOfShapeListOfShape aMapEdgeFaces;
  TopExp::MapShapesAndAncestors(aRes, TopAbs_EDGE, TopAbs_FACE, aMapEdgeFaces);
  
  // creating map of vertex edges
  TopTools_IndexedDataMapOfShapeListOfShape aMapEdgesVertex;
  TopExp::MapShapesAndUniqueAncestors(aRes, TopAbs_VERTEX, TopAbs_EDGE, aMapEdgesVertex);
  
  TopTools_MapOfShape SharedVert;

  TopTools_IndexedMapOfShape anOldEdges;
  TopExp::MapShapes(myInitShape, TopAbs_EDGE, anOldEdges);

  TopTools_DataMapOfShapeShape NewEdges2OldEdges;
  for (int i = 1; i <= anOldEdges.Extent(); i++)
  {
    const TopoDS_Shape& anOldEdge = anOldEdges(i);
    TopoDS_Shape aNewEdge = myContext->Apply(anOldEdge);
    if (!aNewEdge.IsNull() && !aNewEdge.IsSame(anOldEdge))
      NewEdges2OldEdges.Bind(aNewEdge, anOldEdge);
  }

  if (mySafeInputMode)
    UpdateMapOfShapes(myKeepShapes, myContext);

  TopExp_Explorer exp;
  // processing separate wires
  for (exp.Init(aRes, TopAbs_WIRE, TopAbs_FACE); exp.More(); exp.Next()) 
  {
    TopTools_SequenceOfShape SeqEdges;
    TopExp_Explorer expE(exp.Current(), TopAbs_EDGE);
    for (; expE.More(); expE.Next())
      SeqEdges.Append(expE.Current());
    SharedVert.Clear();
    CheckSharedVertices(SeqEdges, aMapEdgesVertex, myKeepShapes, SharedVert); 
    MergeSeq(SeqEdges, myAngTol, myConcatBSplines, mySafeInputMode, myContext,
      SharedVert);
  }

  // processing each face
  for (exp.Init(aRes, TopAbs_FACE); exp.More(); exp.Next()) {
    TopoDS_Shape aFace = exp.Current().Oriented(TopAbs_FORWARD);
    TopTools_IndexedDataMapOfShapeListOfShape aMapFacesEdges;
    TopTools_SequenceOfShape aNonSharedEdges;
    for (TopExp_Explorer expe(aFace,TopAbs_EDGE); expe.More(); expe.Next()) {
      TopoDS_Edge edge = TopoDS::Edge(expe.Current());
      const TopTools_ListOfShape& aList = aMapEdgeFaces.FindFromKey(edge);
      TopTools_ListIteratorOfListOfShape anIter(aList);
      Standard_Integer NbFacesPerEdge = aList.Extent();
      for ( ; anIter.More(); anIter.Next()) {
        const TopoDS_Shape& aFace1 = anIter.Value();
        if (aFace1.IsSame(aFace) && NbFacesPerEdge != 1)
          continue;
        if (NbFacesPerEdge == 1)
          //store non-shared edges separately 
          aNonSharedEdges.Append(edge);
        else 
        {
          if (aMapFacesEdges.Contains(aFace1))
            aMapFacesEdges.ChangeFromKey(aFace1).Append(edge);
          else 
          {
            TopTools_ListOfShape ListEdges;
            ListEdges.Append(edge);
            aMapFacesEdges.Add(aFace1, ListEdges);
          }
        }
      }
    }
      
    for (Standard_Integer i=1; i<=aMapFacesEdges.Extent(); i++)
    {
      const TopTools_ListOfShape& ListEdges = aMapFacesEdges.FindFromIndex(i);
      TopTools_SequenceOfShape SeqEdges;
      TopTools_ListIteratorOfListOfShape anIter(ListEdges);
      for ( ; anIter.More(); anIter.Next())
        SeqEdges.Append(anIter.Value());
      if (SeqEdges.Length()==1) 
        continue;  

      SharedVert.Clear();
      CheckSharedVertices(SeqEdges, aMapEdgesVertex, myKeepShapes, SharedVert);
      if (MergeSeq(SeqEdges, myAngTol, myConcatBSplines, mySafeInputMode,
        myContext, SharedVert))
      {
        TopoDS_Face tmpF = TopoDS::Face(exp.Current());
        if ( !ChangedFaces.Contains(tmpF) )
          ChangedFaces.Add(tmpF);
        tmpF = TopoDS::Face(aMapFacesEdges.FindKey(i));
        if ( !ChangedFaces.Contains(tmpF) )
          ChangedFaces.Add(tmpF);
      }
    }
    
    if ( aNonSharedEdges.Length() > 1 )
    {
      SharedVert.Clear();
      CheckSharedVertices(aNonSharedEdges, aMapEdgesVertex, myKeepShapes, SharedVert);
      if (MergeSeq(aNonSharedEdges, myAngTol, myConcatBSplines, mySafeInputMode,
        myContext, SharedVert))
      {
        TopoDS_Face tmpF = TopoDS::Face(exp.Current());
        if ( !ChangedFaces.Contains(tmpF) )
          ChangedFaces.Add(tmpF);
      }
    }

  } // end processing each face

  // fix changed faces and replace them in the local context
  Standard_Real aPrec = Precision::Confusion();
  for (Standard_Integer i = 1; i <= ChangedFaces.Extent(); i++) {
    TopoDS_Face aFace = TopoDS::Face(myContext->Apply(ChangedFaces.FindKey(i)));
    if (aFace.IsNull())
      continue;
    Handle(ShapeFix_Face) sff = new ShapeFix_Face(aFace);
    sff->SetContext(myContext);
    sff->SetPrecision(aPrec);
    sff->SetMinTolerance(aPrec);
    sff->SetMaxTolerance(Max(1., aPrec*1000.));
    sff->Perform();
    TopoDS_Shape aNewFace = sff->Face();
    myContext->Replace(aFace,aNewFace);
  }

  if (ChangedFaces.Extent() > 0) {
    // fix changed shell and replace it in the local context
    TopoDS_Shape aRes1 = myContext->Apply(aRes);
    Standard_Boolean isChanged = Standard_False;
    TopExp_Explorer expsh;
    for (expsh.Init(aRes1, TopAbs_SHELL); expsh.More(); expsh.Next()) {
      TopoDS_Shell aShell = TopoDS::Shell(expsh.Current());
      Handle(ShapeFix_Shell) sfsh = new ShapeFix_Shell;
      sfsh->FixFaceOrientation(aShell);
      TopoDS_Shape aNewShell = sfsh->Shell();
      if (!aNewShell.IsSame(aShell)) {
        myContext->Replace(aShell, aNewShell);
        isChanged = Standard_True;
      }
    }
    if (isChanged)
      aRes1 = myContext->Apply(aRes1);
    myContext->Replace(myShape, aRes1);
  }

  myShape = myContext->Apply(myShape);
}

//=======================================================================
//function : UnifyFacesAndEdges
//purpose  : 
//=======================================================================

void ShapeUpgrade_UnifySameDomain::UnifyFacesAndEdges()
{
  UnifyFaces();
  
  /*
  ShapeUpgrade_RemoveLocations RemLoc;
  RemLoc.Remove(myShape);
  myShape = RemLoc.GetResult();
  */

  UnifyEdges();
}

//=======================================================================
//function : Build
//purpose  : builds the resulting shape
//=======================================================================
void ShapeUpgrade_UnifySameDomain::Build() 
{
  if (myUnifyFaces && myUnifyEdges)
    UnifyFacesAndEdges();

  else if (myUnifyEdges)
    UnifyEdges();
  else if (myUnifyFaces)
    UnifyFaces();

  // Fill the history of modifications during the operation
  FillHistory();
}

//=======================================================================
//function : FillHistory
//purpose  : Fill the history of modifications during the operation
//=======================================================================
void ShapeUpgrade_UnifySameDomain::FillHistory()
{
  if (myHistory.IsNull())
    // History is not requested
    return;

  // Only Vertices, Edges and Faces can be modified during unification.
  // Thus, only these kind of shapes should be checked.

  // Get history from the context.
  // It contains all modifications of the operation. Some of these
  // modifications become not relevant and should be filtered.
  Handle(BRepTools_History) aCtxHistory = myContext->History();

  // Explore the history of the context and fill
  // the history of UnifySameDomain algorithm
  Handle(BRepTools_History) aUSDHistory = new BRepTools_History();

  // Map all Vertices, Edges and Faces in the input shape
  TopTools_IndexedMapOfShape aMapInputShape;
  TopExp::MapShapes(myInitShape, TopAbs_VERTEX, aMapInputShape);
  TopExp::MapShapes(myInitShape, TopAbs_EDGE  , aMapInputShape);
  TopExp::MapShapes(myInitShape, TopAbs_FACE  , aMapInputShape);

  // Map all Vertices, Edges and Faces in the result shape
  TopTools_IndexedMapOfShape aMapResultShapes;
  TopExp::MapShapes(myShape, TopAbs_VERTEX, aMapResultShapes);
  TopExp::MapShapes(myShape, TopAbs_EDGE  , aMapResultShapes);
  TopExp::MapShapes(myShape, TopAbs_FACE  , aMapResultShapes);

  // Iterate on all input shapes and get their modifications
  Standard_Integer i, aNb = aMapInputShape.Extent();
  for (i = 1; i <= aNb; ++i)
  {
    const TopoDS_Shape& aS = aMapInputShape(i);

    // Check the shape itself to be present in the result
    if (aMapResultShapes.Contains(aS))
    {
      // The shape is present in the result as is, thus has not been modified
      continue;
    }

    // Check if the shape has been modified during the operation
    const TopTools_ListOfShape& aLSImages = aCtxHistory->Modified(aS);
    if (aLSImages.IsEmpty())
    {
      // The shape has not been modified and not present in the result,
      // thus it has been removed
      aUSDHistory->Remove(aS);
      continue;
    }

    // Check the images of the shape to be present in the result
    Standard_Boolean bRemoved = Standard_True;
    TopTools_ListIteratorOfListOfShape aItLSIm(aLSImages);
    for (; aItLSIm.More(); aItLSIm.Next())
    {
      if (aMapResultShapes.Contains(aItLSIm.Value()))
      {
        // Image is found in the result, thus the shape has been modified
        aUSDHistory->AddModified(aS, aItLSIm.Value());
        bRemoved = Standard_False;
      }
    }

    if (bRemoved)
    {
      // No images are found in the result, thus the shape has been removed
      aUSDHistory->Remove(aS);
    }
  }

  // Merge the history of the operation into global history
  myHistory->Merge(aUSDHistory);
}
