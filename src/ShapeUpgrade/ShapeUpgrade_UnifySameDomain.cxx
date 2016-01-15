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

IMPLEMENT_STANDARD_RTTIEXT(ShapeUpgrade_UnifySameDomain,MMgt_TShared)

struct SubSequenceOfEdges
{
  TopTools_SequenceOfShape SeqsEdges;
  TopoDS_Edge UnionEdges;
};


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
    {
      //aNewEdges.Remove(edge);
      TopoDS_Shape LastEdge = aNewEdges(aNewEdges.Extent());
      aNewEdges.RemoveLast();
      if (aNewEdges.FindIndex(edge) != 0)
        aNewEdges.Substitute(aNewEdges.FindIndex(edge), LastEdge);
      /////////////////////////
    }
    else
      aNewEdges.Add(edge);
  }

  Standard_Boolean isDropped = Standard_False;
  //merge edges and drop seams
  Standard_Integer i;
  for (i = 1; i <= edges.Length(); i++) {
    TopoDS_Shape current = edges(i);
    if(aNewEdges.Contains(current)) {

      //aNewEdges.Remove(current);
      TopoDS_Shape LastEdge = aNewEdges(aNewEdges.Extent());
      aNewEdges.RemoveLast();
      if (aNewEdges.FindIndex(current) != 0)
        aNewEdges.Substitute(aNewEdges.FindIndex(current), LastEdge);
      /////////////////////////
      edges.Remove(i);
      i--;

      if(!isDropped) {
        isDropped = Standard_True;
        anIndex = i;
      }
    }
  }

  //add edges to the sequemce
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
//function : IsSameDomain
//purpose  : 
//=======================================================================
static Standard_Boolean IsSameDomain(const TopoDS_Face& aFace,
                                     const TopoDS_Face& aCheckedFace)
{
  //checking the same handles
  TopLoc_Location L1, L2;
  Handle(Geom_Surface) S1, S2;

  S1 = BRep_Tool::Surface(aFace,L1);
  S2 = BRep_Tool::Surface(aCheckedFace,L2);

  if (S1 == S2 && L1 == L2)
    return Standard_True;

  // planar and cylindrical cases (IMP 20052)
  Standard_Real aPrec = Precision::Confusion();

  S1 = BRep_Tool::Surface(aFace);
  S2 = BRep_Tool::Surface(aCheckedFace);

  S1 = ClearRts(S1);
  S2 = ClearRts(S2);

  //Handle(Geom_OffsetSurface) aGOFS1, aGOFS2;
  //aGOFS1 = Handle(Geom_OffsetSurface)::DownCast(S1);
  //aGOFS2 = Handle(Geom_OffsetSurface)::DownCast(S2);
  //if (!aGOFS1.IsNull()) S1 = aGOFS1->BasisSurface();
  //if (!aGOFS2.IsNull()) S2 = aGOFS2->BasisSurface();

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
      IntPatch_ImpImpIntersection anIIInt (aGA1, aTT1, aGA2, aTT2, aPrec, aPrec);
      if (!anIIInt.IsDone() || anIIInt.IsEmpty())
        return Standard_False;

      return anIIInt.TangentFaces();
    }
    catch (Standard_Failure) {
      return Standard_False;
    }
  }

  // case of two planar surfaces:
  // all kinds of surfaces checked, including b-spline and bezier
  GeomLib_IsPlanarSurface aPlanarityChecker1 (S1, aPrec);
  if (aPlanarityChecker1.IsPlanar()) {
    GeomLib_IsPlanarSurface aPlanarityChecker2 (S2, aPrec);
    if (aPlanarityChecker2.IsPlanar()) {
      gp_Pln aPln1 = aPlanarityChecker1.Plan();
      gp_Pln aPln2 = aPlanarityChecker2.Plan();

      if (aPln1.Position().Direction().IsParallel(aPln2.Position().Direction(),Precision::Angular()) &&
          aPln1.Distance(aPln2) < aPrec) {
        return Standard_True;
      }
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
      if (fabs(aCyl1.Radius() - aCyl2.Radius()) < aPrec) {
        gp_Dir aDir1 = aCyl1.Position().Direction();
        gp_Dir aDir2 = aCyl2.Position().Direction();
        if (aDir1.IsParallel(aDir2, Precision::Angular())) {
          gp_Pnt aLoc1 = aCyl1.Location();
          gp_Pnt aLoc2 = aCyl2.Location();
          gp_Vec aVec12 (aLoc1, aLoc2);
          if (aVec12.SquareMagnitude() < aPrec*aPrec ||
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
//function : MovePCurves
//purpose  :
//=======================================================================
static void MovePCurves(TopoDS_Face& aTarget,
                        const TopoDS_Face& aSource)
{
  BRep_Builder B;
  for(TopExp_Explorer wexp(aSource,TopAbs_WIRE);wexp.More();wexp.Next()) {
    Handle(ShapeFix_Wire) sfw = new ShapeFix_Wire(TopoDS::Wire(wexp.Current()),
                                                  aTarget, Precision::Confusion());
    sfw->FixReorder();
    Standard_Boolean isReoredFailed = sfw->StatusReorder ( ShapeExtend_FAIL );
    sfw->FixEdgeCurves();
    if(isReoredFailed)
      continue;

    sfw->FixShifted();
    sfw->FixDegenerated();

    // remove degenerated edges from not degenerated points
    ShapeAnalysis_Edge sae;
    Handle(ShapeExtend_WireData) sewd = sfw->WireData();
    for(Standard_Integer i = 1; i<=sewd->NbEdges();i++) {
      TopoDS_Edge E = sewd->Edge(i);
      if(BRep_Tool::Degenerated(E)&&!sae.HasPCurve(E,aTarget)) {
        sewd->Remove(i);
        i--;
      }
    }

    TopoDS_Wire ResWire = sfw->Wire();
    B.Add(aTarget,ResWire);
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
  //TColGeom2d_SequenceOfCurve PCurveSeq;
  TColGeom_SequenceOfSurface SurfSeq;
  //TopTools_SequenceOfShape LocSeq;
  
  BRep_ListIteratorOfListOfCurveRepresentation itr( (Handle(BRep_TEdge)::DownCast(FirstEdge.TShape()))->Curves() );
  for (; itr.More(); itr.Next())
  {
    Handle(BRep_CurveRepresentation) CurveRep = itr.Value();
    if (CurveRep->IsCurveOnSurface())
    {
      //PCurveSeq.Append(CurveRep->PCurve());
      SurfSeq.Append(CurveRep->Surface());
      /*
      TopoDS_Shape aLocShape;
      aLocShape.Location(CurveRep->Location());
      LocSeq.Append(aLocShape);
      */
    }
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
  TopLoc_Location aLoc;
  for (j = 1; j <= SurfSeq.Length(); j++)
  {
    TColGeom2d_Array1OfBSplineCurve tab_c2d(0,nb_curve-1); //array of the pcurves
    
    PrevVertex = FirstVertex;
    PrevEdge = FirstEdge;
    //TopLoc_Location theLoc = LocSeq(j).Location();
    for (i = 1; i <= nb_curve; i++)
    {
      TopoDS_Edge anEdge = TopoDS::Edge(aChain(i));
      TopoDS_Vertex VF, VL;
      TopExp::Vertices(anEdge, VF, VL);
      Standard_Boolean ToReverse = (!VF.IsSame(PrevVertex));

      /*
      Handle(Geom2d_Curve) aPCurve =
        BRep_Tool::CurveOnSurface(anEdge, SurfSeq(j), anEdge.Location()*theLoc, fpar, lpar);
      */
      Handle(Geom2d_Curve) aPCurve =
        BRep_Tool::CurveOnSurface(anEdge, SurfSeq(j), aLoc, fpar, lpar);
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
    BB.UpdateEdge(ResEdge, ResPCurves(j), SurfSeq(j), aLoc, MaxTol);
    BB.Range(ResEdge, SurfSeq(j), aLoc, ResPCurves(j)->FirstParameter(), ResPCurves(j)->LastParameter());
  }

  BRepLib::SameParameter(ResEdge, MaxTol, Standard_True);
  
  return ResEdge;
}

//=======================================================================
//function : MergeSubSeq
//purpose  : Merges a sequence of edges into one edge if possible
//=======================================================================

static Standard_Boolean MergeSubSeq(const TopTools_SequenceOfShape& aChain, TopoDS_Edge& OutEdge, double Tol, Standard_Boolean ConcatBSplines) 
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
      if(!Dir1.IsParallel(Dir2,Tol))  
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
    TopoDS_Vertex V1 = sae.FirstVertex(TopoDS::Edge(aChain.First()));
    gp_Pnt PV1 = BRep_Tool::Pnt(V1);
    TopoDS_Vertex V2 = sae.LastVertex(TopoDS::Edge(aChain.Last()));
    gp_Pnt PV2 = BRep_Tool::Pnt(V2);
    gp_Vec Vec(PV1, PV2);
    Handle(Geom_Line) L = new Geom_Line(gp_Ax1(PV1,Vec));
    Standard_Real dist = PV1.Distance(PV2);
    Handle(Geom_TrimmedCurve) tc = new Geom_TrimmedCurve(L,0.0,dist);
    TopoDS_Edge E;
    B.MakeEdge (E, tc ,Precision::Confusion());
    B.Add (E,V1);  B.Add (E,V2);
    B.UpdateVertex(V1, 0., E, 0.);
    B.UpdateVertex(V2, dist, E, 0.);
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

    TopoDS_Vertex V1 = sae.FirstVertex(FE);
    TopoDS_Vertex V2 = sae.LastVertex(TopoDS::Edge(aChain.Last()));
    TopoDS_Edge E;
    if (V1.IsSame(V2)) {
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
        B.Add(E,V1);
        B.Add(E,V2);
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
        B.Add(E,V1);
        B.Add(E,V2);
      }
    }
    else {
      gp_Pnt PV1 = BRep_Tool::Pnt(V1);
      gp_Pnt PV2 = BRep_Tool::Pnt(V2);
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
      B.Add(E,V1);
      B.Add(E,V2);
      B.UpdateVertex(V1, fpar, E, 0.);
      B.UpdateVertex(V2, lpar, E, 0.);
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
                                          double Tol, const TopTools_MapOfShape& DegEdgeVrt)
{
  TopoDS_Vertex CV = TopExp::LastVertex(edge1, Standard_True);
  if (CV.IsNull() || DegEdgeVrt.Contains(CV))
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

  if (Diff1.Angle(Diff2) > Tol)
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
                            Standard_Boolean IsClosed, double Tol, const TopTools_MapOfShape& DegEdgeVrt )
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
    isOk = IsMergingPossible(edge1, edge2, Tol, DegEdgeVrt);
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
    if (IsMergingPossible(edge1, edge2, Tol, DegEdgeVrt))
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
static Standard_Boolean MergeEdges(const TopTools_SequenceOfShape& SeqEdges,
                                   const Standard_Real Tol,
                                   const Standard_Boolean ConcatBSplines,
                                   NCollection_Sequence<SubSequenceOfEdges>& SeqOfSubSeqOfEdges,
                                   const TopTools_MapOfShape& NonMergVrt )
{
  // make chain for union
  //BRep_Builder B;
  ShapeAnalysis_Edge sae;
  TopoDS_Edge FirstE = TopoDS::Edge(SeqEdges.Value(1));
  TopoDS_Edge LastE = FirstE;
  TopoDS_Vertex VF = sae.FirstVertex(FirstE);
  TopoDS_Vertex VL = sae.LastVertex(LastE);
  TopTools_SequenceOfShape aChain;
  aChain.Append(FirstE);
  TColStd_MapOfInteger IndUsedEdges;
  IndUsedEdges.Add(1);
  Standard_Integer j;
  TopTools_MapOfShape VerticesToAvoid;
  TopTools_SequenceOfShape SeqEdges1;
  for (j = 1; j <= SeqEdges.Length(); j++)
  {
    TopoDS_Edge anEdge = TopoDS::Edge(SeqEdges(j));
    if (BRep_Tool::Degenerated(anEdge))
    {
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(anEdge, V1, V2);
      VerticesToAvoid.Add(V1);
      VerticesToAvoid.Add(V2);
      continue;
    }
    SeqEdges1.Append(anEdge);
  }

  for(j=2; j<=SeqEdges1.Length(); j++) {
    for(Standard_Integer k=2; k<=SeqEdges1.Length(); k++) {
      if(IndUsedEdges.Contains(k)) continue;
      TopoDS_Edge edge = TopoDS::Edge(SeqEdges1.Value(k));
      TopoDS_Vertex VF2 = sae.FirstVertex(edge);
      TopoDS_Vertex VL2 = sae.LastVertex(edge);
      if(VF2.IsSame(VL)) {
        aChain.Append(edge);
        LastE = edge;
        VL = sae.LastVertex(LastE);
        IndUsedEdges.Add(k);
      }
      else if(VL2.IsSame(VF)) {
        aChain.Prepend(edge);
        FirstE = edge;
        VF = sae.FirstVertex(FirstE);
        IndUsedEdges.Add(k);
      }
    }
  }

  Standard_Boolean IsClosed = Standard_False;
  if (VF.IsSame ( VL ))
    IsClosed = Standard_True;

  VerticesToAvoid.Unite(NonMergVrt);
  GenerateSubSeq(aChain, SeqOfSubSeqOfEdges, IsClosed, Tol, VerticesToAvoid);

  for (int i = 1; i <= SeqOfSubSeqOfEdges.Length(); i++)
  {
    TopoDS_Edge UE;
    if (SeqOfSubSeqOfEdges(i).SeqsEdges.Length() < 2)
      continue;
    if (MergeSubSeq(SeqOfSubSeqOfEdges(i).SeqsEdges, UE, Tol, ConcatBSplines))
      SeqOfSubSeqOfEdges(i).UnionEdges = UE;
  }
  return Standard_True;
}

//=======================================================================
//function : MergeSeq
//purpose  : Tries to unify the sequence of edges with the set of another edges 
//which lies on the same geometry
//=======================================================================

static Standard_Boolean MergeSeq (const TopTools_SequenceOfShape& SeqEdges,
                                  const Standard_Real Tol,
                                  const Standard_Boolean ConcatBSplines,
                                  Handle(ShapeBuild_ReShape)& theContext,
                                  TopTools_DataMapOfShapeShape& theOldShapes,
                                  const TopTools_MapOfShape& nonMergVert,
                                  const TopTools_DataMapOfShapeShape& NewEdges2OldEdges)
{ 
  NCollection_Sequence<SubSequenceOfEdges> SeqOfSubsSeqOfEdges;
  int k = 1;
  if ( MergeEdges(SeqEdges, Tol, ConcatBSplines, SeqOfSubsSeqOfEdges, nonMergVert) )
  {
    for (Standard_Integer i = 1; i <= SeqOfSubsSeqOfEdges.Length(); i++ )
    {
      if (SeqOfSubsSeqOfEdges(i).UnionEdges.IsNull())
        continue;
      theContext->Replace(SeqOfSubsSeqOfEdges(i).SeqsEdges(1), SeqOfSubsSeqOfEdges(i).UnionEdges);
      k++;
      for (Standard_Integer j = 2; j <= SeqOfSubsSeqOfEdges(i).SeqsEdges.Length(); j++)
      {
        TopoDS_Shape OldEdge = NewEdges2OldEdges(SeqOfSubsSeqOfEdges(i).SeqsEdges(j));
        theOldShapes.Bind(OldEdge, SeqOfSubsSeqOfEdges(i).UnionEdges);
        theContext->Remove(SeqOfSubsSeqOfEdges(i).SeqsEdges(j));
      }
    }
    return Standard_True;
  }
  else
    return Standard_False;
}

//=======================================================================
//function : CheckSharedVertices
//purpose  : Checks the sequence of edges on the presence of shared vertex 
//=======================================================================

static void CheckSharedVertices(const TopTools_SequenceOfShape& theSeqEdges, 
                                const TopTools_IndexedDataMapOfShapeListOfShape& theMapEdgesVertex,
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
    TopTools_MapOfShape aMapOfEdges;
    TopTools_ListIteratorOfListOfShape iter(ListEdgesV1);
    for (; iter.More(); iter.Next())
      aMapOfEdges.Add(iter.Value());
    if (aMapOfEdges.Extent() > 2)
      theShareVertMap.Add(SeqVertexes(k));
  }
  //return theShareVertMap.IsEmpty() ? false : true;
}

//=======================================================================
//function : ShapeUpgrade_UnifySameDomain
//purpose  : Constructor
//=======================================================================

ShapeUpgrade_UnifySameDomain::ShapeUpgrade_UnifySameDomain()
{
  myUnifyEdges = Standard_True;
  myUnifyFaces = Standard_True;
  myConcatBSplines = Standard_False;

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
{
  myInitShape = aShape;
  myShape = aShape;
  myUnifyEdges = UnifyEdges;
  myUnifyFaces = UnifyFaces;
  myConcatBSplines = ConcatBSplines;

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
  myOldShapes.Clear();
  //myGenerated.Clear();
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
  // creating map of edge faces
  TopTools_IndexedDataMapOfShapeListOfShape aMapEdgeFaces;
  TopExp::MapShapesAndAncestors(myShape, TopAbs_EDGE, TopAbs_FACE, aMapEdgeFaces);
  // processing each shell
  TopExp_Explorer exps;
  for (exps.Init(myShape, TopAbs_SHELL); exps.More(); exps.Next()) {
    TopoDS_Shell aShell = TopoDS::Shell(exps.Current());

    // map of processed shapes
    TopTools_MapOfShape aProcessed;

    Standard_Integer NbModif = 0;
    Standard_Boolean hasFailed = Standard_False;
    Standard_Real tol = Precision::Confusion();

    // count faces
    Standard_Integer nbf = 0;
    TopExp_Explorer exp;
    TopTools_MapOfShape mapF;
    for (exp.Init(aShell, TopAbs_FACE); exp.More(); exp.Next()) {
      if (mapF.Add(exp.Current()))
        nbf++;
    }

    // processing each face
    mapF.Clear();
    for (exp.Init(aShell, TopAbs_FACE); exp.More(); exp.Next()) {
      TopoDS_Face aFace = TopoDS::Face(exp.Current().Oriented(TopAbs_FORWARD));

      if (aProcessed.Contains(aFace))
        continue;

      Standard_Integer dummy;
      TopTools_SequenceOfShape edges;
      AddOrdinaryEdges(edges,aFace,dummy);

      TopTools_SequenceOfShape faces;
      faces.Append(aFace);

      //surface and location to construct result
      TopLoc_Location aBaseLocation;
      Handle(Geom_Surface) aBaseSurface = BRep_Tool::Surface(aFace,aBaseLocation);
      aBaseSurface = ClearRts(aBaseSurface);

      // find adjacent faces to union
      Standard_Integer i;
      for (i = 1; i <= edges.Length(); i++) {
        TopoDS_Edge edge = TopoDS::Edge(edges(i));
        if (BRep_Tool::Degenerated(edge))
          continue;

        const TopTools_ListOfShape& aList = aMapEdgeFaces.FindFromKey(edge);
        TopTools_ListIteratorOfListOfShape anIter(aList);
        for (; anIter.More(); anIter.Next()) {
          TopoDS_Face anCheckedFace = TopoDS::Face(anIter.Value().Oriented(TopAbs_FORWARD));
          if (anCheckedFace.IsSame(aFace))
            continue;

          if (aProcessed.Contains(anCheckedFace))
            continue;

          if (IsSameDomain(aFace,anCheckedFace)) {

            if (aList.Extent() != 2) {
              // non mainfold case is not processed
              continue;
            }

            // replacing pcurves
            TopoDS_Face aMockUpFace;
            BRep_Builder B;
            B.MakeFace(aMockUpFace,aBaseSurface,aBaseLocation,0.);
            MovePCurves(aMockUpFace,anCheckedFace);

            if (AddOrdinaryEdges(edges,aMockUpFace,dummy)) {
              // sequence edges is modified
              i = dummy;
            }

            faces.Append(anCheckedFace);
            aProcessed.Add(anCheckedFace);
            break;
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
          //aContext->Replace(aWire,aWireFixed);
          myContext->Replace(aWire,aWireFixed);
          //for history
          /*
          if (!myOldNewMap.IsBound(aWire))
          {
            TopTools_ListOfShape EmptyList;
            myOldNewMap.Bind(aWire, EmptyList);
          }
          myOldNewMap(aWire).Clear();
          myOldNewMap(aWire).Append(aWireFixed);
          */
          /////////////
          
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
        //aContext->Replace(aContext->Apply(aFace),aResult);
        myContext->Replace(myContext->Apply(aFace),aResult);
        //for history
        /*
        if (!myOldNewMap.IsBound(aFace))
        {
          TopTools_ListOfShape EmptyList;
          myOldNewMap.Bind(aFace, EmptyList);
        }
        myOldNewMap(aFace).Clear();
        myOldNewMap(aFace).Append(aResult);
        */
        /////////////

        ShapeFix_Face sff (aResult);
        //Intializing by tolerances
        sff.SetPrecision(Precision::Confusion());
        sff.SetMinTolerance(tol);
        sff.SetMaxTolerance(1.);
        //Setting modes
        sff.FixOrientationMode() = 0;
        //sff.FixWireMode() = 0;
        //sff.SetContext(aContext);
        sff.SetContext(myContext);
        // Applying the fixes
        sff.Perform();
        if(sff.Status(ShapeExtend_FAIL))
        hasFailed = Standard_True;

        // breaking down to several faces
        //TopoDS_Shape theResult = aContext->Apply(aResult);
        TopoDS_Shape theResult = myContext->Apply(aResult);
        for (TopExp_Explorer aFaceExp (theResult,TopAbs_FACE); aFaceExp.More(); aFaceExp.Next()) {
          TopoDS_Face aCurrent = TopoDS::Face(aFaceExp.Current().Oriented(TopAbs_FORWARD));
          Handle(TColGeom_HArray2OfSurface) grid = new TColGeom_HArray2OfSurface ( 1, 1, 1, 1 );
          grid->SetValue ( 1, 1, aBaseSurface );
          Handle(ShapeExtend_CompositeSurface) G = new ShapeExtend_CompositeSurface ( grid );
          ShapeFix_ComposeShell CompShell;
          CompShell.Init ( G, aBaseLocation, aCurrent, ::Precision::Confusion() );//myPrecision
          //CompShell.SetContext( aContext );
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

          CompShell.DispatchWires ( parts,wires );
          for (Standard_Integer j=1; j <= parts.Length(); j++ ) {
            ShapeFix_Face aFixOrient(TopoDS::Face(parts(j)));
            //aFixOrient.SetContext(aContext);
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

          //aContext->Replace(aCurrent,CompRes);
          myContext->Replace(aCurrent,CompRes);
          //for history
          /*
          if (!myOldNewMap.IsBound(aCurrent))
          {
            TopTools_ListOfShape EmptyList;
            myOldNewMap.Bind(aCurrent, EmptyList);
          }
          myOldNewMap(aCurrent).Clear();
          myOldNewMap(aCurrent).Append(CompRes);
          */
          /////////////
        }

        // remove the remaining faces
        for(i = 2; i <= faces.Length(); i++)
        { 
          myOldShapes.Bind(faces(i), theResult);
          myContext->Remove(faces(i));
        }
      }
    } // end processing each face

    //TopoDS_Shape aResult = Shape;
    if (NbModif > 0 && !hasFailed) {
      //TopoDS_Shape aResult = aContext->Apply(aShell);
      TopoDS_Shape aResult = myContext->Apply(aShell);

      ShapeFix_Edge sfe;
      if (!myContext.IsNull()) sfe.SetContext(myContext);
      for (exp.Init(aResult,TopAbs_EDGE); exp.More(); exp.Next()) {
        TopoDS_Edge E = TopoDS::Edge(exp.Current());
        sfe.FixVertexTolerance (E);
        // ptv add fix same parameter
        sfe.FixSameParameter(E, Precision::Confusion());
      }

      myContext->Replace(aShell, aResult);
      //for history
      /*
      if (!myOldNewMap.IsBound(aShell))
      {
        TopTools_ListOfShape EmptyList;
        myOldNewMap.Bind(aShell, EmptyList);
      }
      myOldNewMap(aShell).Clear();
      myOldNewMap(aShell).Append(aResult);
      */
      /////////////
    }
    //else
    {
      for (exp.Init(aShell, TopAbs_FACE); exp.More(); exp.Next()) {
        TopoDS_Face aFace = TopoDS::Face(exp.Current().Oriented(TopAbs_FORWARD));
        Handle(ShapeFix_Wire) sfw = new ShapeFix_Wire;
        sfw->SetContext(myContext);
        sfw->SetPrecision(Precision::Confusion());
        sfw->SetMinTolerance(Precision::Confusion());
        sfw->SetMaxTolerance(1.);
        sfw->SetFace(aFace);
        for (TopoDS_Iterator iter (aFace,Standard_False); iter.More(); iter.Next()) {
          TopoDS_Wire wire = TopoDS::Wire(iter.Value());
          sfw->Load(wire);
          sfw->FixReorder();
          sfw->FixShifted();
        }
      }
    }
  } // end processing each shell

  myShape = myContext->Apply(myShape);
}

//=======================================================================
//function : UnifyEdges
//purpose  : 
//=======================================================================
void ShapeUpgrade_UnifySameDomain::UnifyEdges()
{
  Standard_Real Tol = Precision::Confusion();
  
  //Handle(ShapeBuild_ReShape) myContext = new ShapeBuild_ReShape;
  Standard_Real myTolerance = Precision::Confusion();
  TopoDS_Shape aResult = myContext->Apply(myShape);

  TopTools_IndexedMapOfShape ChangedFaces;

  // creating map of edge faces
  TopTools_IndexedDataMapOfShapeListOfShape aMapEdgeFaces;
  TopExp::MapShapesAndAncestors(myShape, TopAbs_EDGE, TopAbs_FACE, aMapEdgeFaces);
  
  // creating map of vertex edges
  TopTools_IndexedDataMapOfShapeListOfShape aMapEdgesVertex;
  TopExp::MapShapesAndAncestors(myShape, TopAbs_VERTEX, TopAbs_EDGE, aMapEdgesVertex);

  //Handle(ShapeBuild_ReShape) aContext = new ShapeBuild_ReShape;
  TopoDS_Shape aRes = myShape;
  //aRes = aContext->Apply(aSolid);
  aRes = myContext->Apply(myShape);
  
  TopTools_MapOfShape SharedVert;

  
  TopTools_IndexedMapOfShape anOldEdges;
  TopExp::MapShapes(myInitShape, TopAbs_EDGE, anOldEdges);

  TopTools_DataMapOfShapeShape NewEdges2OldEdges;
  for (int i = 1; i <= anOldEdges.Extent(); i++)
  {
    TopoDS_Shape NewEdge = myContext->Apply(anOldEdges(i));
    if (!NewEdge.IsNull())
      NewEdges2OldEdges.Bind(NewEdge, anOldEdges(i));
  }

  TopExp_Explorer exp;
  // processing separate wires
  for (exp.Init(aRes, TopAbs_WIRE, TopAbs_FACE); exp.More(); exp.Next()) 
  {
    TopTools_SequenceOfShape SeqEdges;
    TopExp_Explorer expE(exp.Current(), TopAbs_EDGE);
    for (; expE.More(); expE.Next())
      SeqEdges.Append(expE.Current());
    SharedVert.Clear();
    CheckSharedVertices(SeqEdges, aMapEdgesVertex, SharedVert); 
    MergeSeq(SeqEdges, Tol, myConcatBSplines, myContext, myOldShapes, SharedVert, NewEdges2OldEdges);
  }

  TopTools_DataMapOfShapeShape oldFaces2NewFaces;
  for (exp.Init(myShape, TopAbs_FACE); exp.More(); exp.Next()) 
  {
    const TopoDS_Face& f = TopoDS::Face(exp.Current());
    TopoDS_Face NewF = TopoDS::Face(myContext->Apply(f));
    if (!NewF.IsNull())
      oldFaces2NewFaces.Bind(f, NewF);
  }

  // processing each face
  for (exp.Init(aRes, TopAbs_FACE); exp.More(); exp.Next()) {
    //TopoDS_Face aFace = TopoDS::Face(aContext->Apply(exp.Current().Oriented(TopAbs_FORWARD)));
    TopoDS_Face aFace = TopoDS::Face(myContext->Apply(exp.Current().Oriented(TopAbs_FORWARD)));
    TopTools_IndexedDataMapOfShapeListOfShape aMapFacesEdges;
    TopTools_SequenceOfShape aNonSharedEdges;
    for (TopExp_Explorer expe(aFace,TopAbs_EDGE); expe.More(); expe.Next()) {
      TopoDS_Edge edge = TopoDS::Edge(expe.Current());
      if (!aMapEdgeFaces.Contains(edge)) continue;
      const TopTools_ListOfShape& aList = aMapEdgeFaces.FindFromKey(edge);
      TopTools_ListIteratorOfListOfShape anIter(aList);
      Standard_Integer NbFacesPerEdge = aList.Extent();
      for ( ; anIter.More(); anIter.Next()) {
        TopoDS_Face face = TopoDS::Face(anIter.Value());
        TopoDS_Face face1 = TopoDS::Face(oldFaces2NewFaces(anIter.Value()));
        if (face1.IsSame(aFace) && NbFacesPerEdge != 1)
          continue;
        if (NbFacesPerEdge == 1)
          //store non-shared edges separately 
          aNonSharedEdges.Append(edge);
        else 
        {
          if (aMapFacesEdges.Contains(face))
            aMapFacesEdges.ChangeFromKey(face).Append(edge);
          else 
          {
            TopTools_ListOfShape ListEdges;
            ListEdges.Append(edge);
            aMapFacesEdges.Add(face,ListEdges);
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
      CheckSharedVertices(SeqEdges, aMapEdgesVertex, SharedVert);
      //if (!SharedVert.IsEmpty()) 
      //  continue;
      if ( MergeSeq(SeqEdges,Tol,myConcatBSplines, myContext, myOldShapes, SharedVert, NewEdges2OldEdges) )
      {
        //for history
        /*
        for (j = 1; j <= SeqEdges.Length(); j++)
        {
          if (!myOldNewMap.IsBound(SeqEdges(j)))
          {
            TopTools_ListOfShape EmptyList;
            myOldNewMap.Bind(SeqEdges(j), EmptyList);
          }
          myOldNewMap(SeqEdges(j)).Clear();
          myOldNewMap(SeqEdges(j)).Append(E);
        }
        */
        /////////////
        
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
      CheckSharedVertices(aNonSharedEdges, aMapEdgesVertex, SharedVert); 
      if ( MergeSeq(aNonSharedEdges, Tol, myConcatBSplines, myContext, myOldShapes, SharedVert, NewEdges2OldEdges) )
      {
        TopoDS_Face tmpF = TopoDS::Face(exp.Current());
        if ( !ChangedFaces.Contains(tmpF) )
          ChangedFaces.Add(tmpF);
      }
    }

  } // end processing each face

  // fix changed faces and replace them in the local context
  for (Standard_Integer i=1; i<=ChangedFaces.Extent(); i++) {
    //TopoDS_Face aFace = TopoDS::Face(aContext->Apply(ChangedFaces.FindKey(i)));
    TopoDS_Face aFace = TopoDS::Face(myContext->Apply(ChangedFaces.FindKey(i)));
    if (aFace.IsNull())
      continue;
    Handle(ShapeFix_Face) sff = new ShapeFix_Face(aFace);
    sff->SetContext(myContext);
    sff->SetPrecision(myTolerance);
    sff->SetMinTolerance(myTolerance);
    sff->SetMaxTolerance(Max(1.,myTolerance*1000.));
    sff->Perform();
    TopoDS_Shape aNewFace = sff->Face();
    //aContext->Replace(aFace,aNewFace);
    myContext->Replace(aFace,aNewFace);
    //for history
    /*
    if (!myOldNewMap.IsBound(aFace))
    {
      TopTools_ListOfShape EmptyList;
      myOldNewMap.Bind(aFace, EmptyList);
    }
    myOldNewMap(aFace).Clear();
    myOldNewMap(aFace).Append(aNewFace);
    */
    /////////////
  }

  if (ChangedFaces.Extent() > 0) {
    // fix changed shell and replace it in the local context
    //TopoDS_Shape aRes1 = aContext->Apply(aRes);
    TopoDS_Shape aRes1 = myContext->Apply(aRes);
    TopExp_Explorer expsh;
    for (expsh.Init(aRes1, TopAbs_SHELL); expsh.More(); expsh.Next()) {
      TopoDS_Shell aShell = TopoDS::Shell(expsh.Current());
      Handle(ShapeFix_Shell) sfsh = new ShapeFix_Shell;
      sfsh->FixFaceOrientation(aShell);
      TopoDS_Shape aNewShell = sfsh->Shell();
      //aContext->Replace(aShell,aNewShell);
      myContext->Replace(aShell,aNewShell);
      //for history
      /*
      if (!myOldNewMap.IsBound(aShell))
      {
        TopTools_ListOfShape EmptyList;
        myOldNewMap.Bind(aShell, EmptyList);
      }
      myOldNewMap(aShell).Clear();
      myOldNewMap(aShell).Append(aNewShell);
      */
      /////////////
    }
    //TopoDS_Shape aRes2 = aContext->Apply(aRes1);
    TopoDS_Shape aRes2 = myContext->Apply(aRes1);
    myContext->Replace(myShape,aRes2);
    //for history
    /*
    if (!myOldNewMap.IsBound(aSolid))
    {
      TopTools_ListOfShape EmptyList;
      myOldNewMap.Bind(aSolid, EmptyList);
    }
    myOldNewMap(aSolid).Clear();
    myOldNewMap(aSolid).Append(aRes2);
    */
    /////////////
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
//======================================================================
void ShapeUpgrade_UnifySameDomain::Build() 
{
  if (myUnifyFaces && myUnifyEdges)
    UnifyFacesAndEdges();

  else if (myUnifyEdges)
    UnifyEdges();
  else if (myUnifyFaces)
    UnifyFaces();

  //Done();
}

//=======================================================================
//function : Shape
//purpose  : give the resulting shape
//=======================================================================
const TopoDS_Shape& ShapeUpgrade_UnifySameDomain::Shape() const
{
  return myShape;
}

//=======================================================================
//function : Generated
//purpose  : returns the new shape from the old one
//=======================================================================
TopoDS_Shape ShapeUpgrade_UnifySameDomain::Generated(const TopoDS_Shape& aShape) const
{
  TopoDS_Shape aNewShape = myContext->Apply(aShape);

  if (aNewShape.IsNull())
    aNewShape = myContext->Apply(myOldShapes(aShape));
  
  return aNewShape;
}
