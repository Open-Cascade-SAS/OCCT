// File:      ShapeUpgrade_UnifySameDomain.cxx
// Created:   09.06.12 14:35:07
// Author:    jgv@ROLEX
// Copyright: Open CASCADE 2012

#include <ShapeUpgrade_UnifySameDomain.ixx>
#include <ShapeUpgrade_UnifySameDomain.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Line.hxx>
#include <gp_Dir.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <gp_Cylinder.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <gp_Lin.hxx>
#include <Geom_Circle.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <TopoDS_Face.hxx>
#include <BRep_Tool.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <IntPatch_ImpImpIntersection.hxx>
#include <GeomLib_IsPlanarSurface.hxx>
#include <BRep_Builder.hxx>
#include <ShapeFix_Wire.hxx>
#include <TopoDS.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <TopoDS_Edge.hxx>
#include <TColGeom_SequenceOfSurface.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <TColGeom_Array1OfBSplineCurve.hxx>
#include <TColGeom_HArray1OfBSplineCurve.hxx>
#include <TColGeom2d_Array1OfBSplineCurve.hxx>
#include <TColGeom2d_HArray1OfBSplineCurve.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TopExp.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <GeomConvert.hxx>
#include <Geom2dConvert.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <Geom2dConvert_CompCurveToBSplineCurve.hxx>
#include <TColGeom2d_SequenceOfBoundedCurve.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <BRepLib.hxx>
#include <GC_MakeCircle.hxx>
#include <Geom_BezierCurve.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <ShapeAnalysis_WireOrder.hxx>
#include <ShapeBuild_Edge.hxx>
#include <Geom2d_Line.hxx>
#include <ShapeFix_Face.hxx>
#include <TColGeom_HArray2OfSurface.hxx>
#include <ShapeExtend_CompositeSurface.hxx>
#include <ShapeFix_ComposeShell.hxx>
#include <ShapeFix_SequenceOfWireSegment.hxx>
#include <ShapeFix_WireSegment.hxx>
#include <ShapeFix_Edge.hxx>
#include <ShapeFix_Shell.hxx>
#include <ShapeUpgrade_RemoveLocations.hxx>


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
//function : MergeEdges
//purpose  : auxilary
//=======================================================================
static Standard_Boolean MergeEdges(const TopTools_SequenceOfShape& SeqEdges,
                                   const TopoDS_Face& aFace,
                                   const Standard_Real Tol,
                                   const Standard_Boolean ConcatBSplines,
                                   TopoDS_Edge& anEdge)
{
  // make chain for union
  BRep_Builder B;
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
  for (j = 1; j <= SeqEdges.Length(); j++)
  {
    TopoDS_Edge anEdge = TopoDS::Edge(SeqEdges(j));
    if (BRep_Tool::Degenerated(anEdge))
      return Standard_False;
  }
  
  for(j=2; j<=SeqEdges.Length(); j++) {
    for(Standard_Integer k=2; k<=SeqEdges.Length(); k++) {
      if(IndUsedEdges.Contains(k)) continue;
      TopoDS_Edge edge = TopoDS::Edge(SeqEdges.Value(k));
      TopoDS_Vertex VF2 = sae.FirstVertex(edge);
      TopoDS_Vertex VL2 = sae.LastVertex(edge);
      if(sae.FirstVertex(edge).IsSame(VL)) {
        aChain.Append(edge);
        LastE = edge;
        VL = sae.LastVertex(LastE);
        IndUsedEdges.Add(k);
      }
      else if(sae.LastVertex(edge).IsSame(VF)) {
        aChain.Prepend(edge);
        FirstE = edge;
        VF = sae.FirstVertex(FirstE);
        IndUsedEdges.Add(k);
      }
    }
  }
  if(aChain.Length()<SeqEdges.Length()) {
#ifdef DEB
    cout<<"can not create correct chain..."<<endl;
#endif
    return Standard_False;
  }
  // union edges in chain
  // first step: union lines and circles
  TopLoc_Location Loc;
  Standard_Real fp1,lp1,fp2,lp2;
  for(j=1; j<aChain.Length(); j++) {
    TopoDS_Edge edge1 = TopoDS::Edge(aChain.Value(j));
    Handle(Geom_Curve) c3d1 = BRep_Tool::Curve(edge1,Loc,fp1,lp1);
    if(c3d1.IsNull()) break;
    while(c3d1->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
      Handle(Geom_TrimmedCurve) tc =
        Handle(Geom_TrimmedCurve)::DownCast(c3d1);
      c3d1 = tc->BasisCurve();
    }
    TopoDS_Edge edge2 = TopoDS::Edge(aChain.Value(j+1));
    Handle(Geom_Curve) c3d2 = BRep_Tool::Curve(edge2,Loc,fp2,lp2);
    if(c3d2.IsNull()) break;
    while(c3d2->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
      Handle(Geom_TrimmedCurve) tc =
        Handle(Geom_TrimmedCurve)::DownCast(c3d2);
      c3d2 = tc->BasisCurve();
    }
    if( c3d1->IsKind(STANDARD_TYPE(Geom_Line)) && c3d2->IsKind(STANDARD_TYPE(Geom_Line)) ) {
      // union lines
      Handle(Geom_Line) L1 = Handle(Geom_Line)::DownCast(c3d1);
      Handle(Geom_Line) L2 = Handle(Geom_Line)::DownCast(c3d2);
      gp_Dir Dir1 = L1->Position().Direction();
      gp_Dir Dir2 = L2->Position().Direction();
      //if(!Dir1.IsEqual(Dir2,Precision::Angular())) { 
      //if(!Dir1.IsParallel(Dir2,Precision::Angular())) { 
      if(!Dir1.IsParallel(Dir2,Tol)) { 
        continue;
      }
      // can union lines => create new edge
      TopoDS_Vertex V1 = sae.FirstVertex(edge1);
      gp_Pnt PV1 = BRep_Tool::Pnt(V1);
      TopoDS_Vertex V2 = sae.LastVertex(edge2);
      gp_Pnt PV2 = BRep_Tool::Pnt(V2);
      gp_Vec Vec(PV1,PV2);
      Handle(Geom_Line) L = new Geom_Line(gp_Ax1(PV1,Vec));
      Standard_Real dist = PV1.Distance(PV2);
      Handle(Geom_TrimmedCurve) tc = new Geom_TrimmedCurve(L,0.0,dist);
      TopoDS_Edge E;
      B.MakeEdge (E,tc,Precision::Confusion());
      B.Add (E,V1);  B.Add (E,V2);
      B.UpdateVertex(V1, 0., E, 0.);
      B.UpdateVertex(V2, dist, E, 0.);
      //ShapeFix_Edge sfe;
      //sfe.FixAddPCurve(E,aFace,Standard_False);
      //sfe.FixSameParameter(E);
      aChain.Remove(j);
      aChain.SetValue(j,E);
      j--;
    }
    if( c3d1->IsKind(STANDARD_TYPE(Geom_Circle)) && c3d2->IsKind(STANDARD_TYPE(Geom_Circle)) ) {
      // union circles
      Handle(Geom_Circle) C1 = Handle(Geom_Circle)::DownCast(c3d1);
      Handle(Geom_Circle) C2 = Handle(Geom_Circle)::DownCast(c3d2);
      gp_Pnt P01 = C1->Location();
      gp_Pnt P02 = C2->Location();
      if (P01.Distance(P02) > Precision::Confusion()) continue;
      // can union circles => create new edge
      TopoDS_Vertex V1 = sae.FirstVertex(edge1);
      gp_Pnt PV1 = BRep_Tool::Pnt(V1);
      TopoDS_Vertex V2 = sae.LastVertex(edge2);
      gp_Pnt PV2 = BRep_Tool::Pnt(V2);
      TopoDS_Vertex VM = sae.LastVertex(edge1);
      gp_Pnt PVM = BRep_Tool::Pnt(VM);
      GC_MakeCircle MC (PV1,PVM,PV2);
      Handle(Geom_Circle) C = MC.Value();
      TopoDS_Edge E;
      if (!MC.IsDone() || C.IsNull()) {
        // jfa for Mantis issue 0020228
        if (PV1.Distance(PV2) > Precision::Confusion()) continue;
        // closed chain
        C = C1;
        B.MakeEdge (E,C,Precision::Confusion());
        B.Add(E,V1);
        B.Add(E,V2);
      }
      else {
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
      aChain.Remove(j);
      aChain.SetValue(j,E);
      j--;
    }
  }
  if (j < aChain.Length()) {
#ifdef DEB
    cout<<"null curve3d in edge..."<<endl;
#endif
    return Standard_False;
  }
  if (aChain.Length() > 1 && ConcatBSplines) {
    // second step: union edges with various curves
    // skl for bug 0020052 from Mantis: perform such unions
    // only if curves are bspline or bezier
    bool NeedUnion = true;
    for(j=1; j<=aChain.Length(); j++) {
      TopoDS_Edge edge = TopoDS::Edge(aChain.Value(j));
      Handle(Geom_Curve) c3d = BRep_Tool::Curve(edge,Loc,fp1,lp1);
      if(c3d.IsNull()) continue;
      while(c3d->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
        Handle(Geom_TrimmedCurve) tc =
          Handle(Geom_TrimmedCurve)::DownCast(c3d);
        c3d = tc->BasisCurve();
      }
      if( ( c3d->IsKind(STANDARD_TYPE(Geom_BSplineCurve)) ||
            c3d->IsKind(STANDARD_TYPE(Geom_BezierCurve)) ) ) continue;
      NeedUnion = false;
      break;
    }
    if(NeedUnion) {
#ifdef DEB
      cout<<"can not make analitical union => make approximation"<<endl;
#endif
      TopoDS_Edge E = GlueEdgesWithPCurves(aChain, VF, VL);
      /*
      TopoDS_Wire W;
      B.MakeWire(W);
      for(j=1; j<=aChain.Length(); j++) {
        TopoDS_Edge edge = TopoDS::Edge(aChain.Value(j));
        B.Add(W,edge);
      }
      Handle(BRepAdaptor_HCompCurve) Adapt = new BRepAdaptor_HCompCurve(W);
      Approx_Curve3d Conv(Adapt,Tol,GeomAbs_C1,9,1000);
      Handle(Geom_BSplineCurve) bc = Conv.Curve();
      TopoDS_Edge E;
      B.MakeEdge (E,bc,Precision::Confusion());
      B.Add (E,VF);
      B.Add (E,VL);
      */
      aChain.SetValue(1,E);
    }
    else {
#ifdef DEB
      cout<<"can not make approximation for such types of curves"<<endl;
#endif
      return Standard_False;
    }
  }

  anEdge = TopoDS::Edge(aChain.Value(1));
  return Standard_True;
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
  //myOldNewMap.Clear();
  //myGenerated.Clear();
}

//=======================================================================
//function : UnifyFaces
//purpose  : 
//=======================================================================

void ShapeUpgrade_UnifySameDomain::UnifyFaces()
{
  //Handle(ShapeBuild_ReShape) myContext = new ShapeBuild_ReShape;
  TopoDS_Shape aResShape = myContext->Apply(myShape);

  // processing each shell
  TopExp_Explorer exps;
  for (exps.Init(myShape, TopAbs_SHELL); exps.More(); exps.Next()) {
    TopoDS_Shell aShell = TopoDS::Shell(exps.Current());

    // creating map of edge faces
    TopTools_IndexedDataMapOfShapeListOfShape aMapEdgeFaces;
    TopExp::MapShapesAndAncestors(aShell, TopAbs_EDGE, TopAbs_FACE, aMapEdgeFaces);

    // map of processed shapes
    TopTools_MapOfShape aProcessed;

    //Handle(ShapeBuild_ReShape) aContext = new ShapeBuild_ReShape;

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

        // connecting wires
        while (edges.Length()>0) {

          Standard_Boolean isEdge3d = Standard_False;
          nbWires++;
          TopTools_MapOfShape aVertices;
          TopoDS_Wire aWire;
          B.MakeWire(aWire);

          TopoDS_Edge anEdge = TopoDS::Edge(edges(1));
          edges.Remove(1);

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
          //aWire = TopoDS::Wire(aContext->Apply(aWire));
          aWire = TopoDS::Wire(myContext->Apply(aWire));

          //TopoDS_Face tmpF = TopoDS::Face(aContext->Apply(faces(1).Oriented(TopAbs_FORWARD)));
          TopoDS_Face tmpF = TopoDS::Face(myContext->Apply(faces(1).Oriented(TopAbs_FORWARD)));
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

          TopTools_SequenceOfShape parts;
          ShapeFix_SequenceOfWireSegment wires;
          for(TopExp_Explorer W_Exp(aCurrent,TopAbs_WIRE);W_Exp.More();W_Exp.Next()) {
            Handle(ShapeExtend_WireData) sbwd =
              new ShapeExtend_WireData ( TopoDS::Wire(W_Exp.Current() ));
            ShapeFix_WireSegment seg ( sbwd, TopAbs_REVERSED );
            wires.Append(seg);
          }

          CompShell.DispatchWires ( parts,wires );
          for (Standard_Integer j=1; j <= parts.Length(); j++ ) {
            ShapeFix_Face aFixOrient(TopoDS::Face(parts(j)));
            //aFixOrient.SetContext(aContext);
            aFixOrient.SetContext(myContext);
            aFixOrient.FixOrientation();
          }

          TopoDS_Shape CompRes;
          if ( faces.Length() !=1 ) {
            TopoDS_Shell S;
            B.MakeShell ( S );
            for ( i=1; i <= parts.Length(); i++ )
              B.Add ( S, parts(i) );
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
          //aContext->Remove(faces(i));
          myContext->Remove(faces(i));
      }
    } // end processing each face

    //TopoDS_Shape aResult = Shape;
    if (NbModif > 0 && !hasFailed) {
      //TopoDS_Shape aResult = aContext->Apply(aShell);
      TopoDS_Shape aResult = myContext->Apply(aShell);

      ShapeFix_Edge sfe;
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

  // processing each solid
  TopAbs_ShapeEnum aType = TopAbs_SOLID;
  TopExp_Explorer exps (myShape, aType);
  if (!exps.More()) {
    aType = TopAbs_SHELL;
    exps.Init(myShape, aType);
  }
  for (; exps.More(); exps.Next()) {
    //TopoDS_Solid aSolid = TopoDS::Solid(exps.Current());
    TopoDS_Shape aSolid = exps.Current();

    TopTools_IndexedMapOfShape ChangedFaces;

    // creating map of edge faces
    TopTools_IndexedDataMapOfShapeListOfShape aMapEdgeFaces;
    TopExp::MapShapesAndAncestors(aSolid, TopAbs_EDGE, TopAbs_FACE, aMapEdgeFaces);

    //Handle(ShapeBuild_ReShape) aContext = new ShapeBuild_ReShape;
    TopoDS_Shape aRes = aSolid;
    //aRes = aContext->Apply(aSolid);
    aRes = myContext->Apply(aSolid);

    // processing each face
    TopExp_Explorer exp;
    for (exp.Init(aRes, TopAbs_FACE); exp.More(); exp.Next()) {
      //TopoDS_Face aFace = TopoDS::Face(aContext->Apply(exp.Current().Oriented(TopAbs_FORWARD)));
      TopoDS_Face aFace = TopoDS::Face(myContext->Apply(exp.Current().Oriented(TopAbs_FORWARD)));
      TopTools_IndexedDataMapOfShapeListOfShape aMapFacesEdges;

      for (TopExp_Explorer expe(aFace,TopAbs_EDGE); expe.More(); expe.Next()) {
        TopoDS_Edge edge = TopoDS::Edge(expe.Current());
        if (!aMapEdgeFaces.Contains(edge)) continue;
        const TopTools_ListOfShape& aList = aMapEdgeFaces.FindFromKey(edge);
        TopTools_ListIteratorOfListOfShape anIter(aList);
        for ( ; anIter.More(); anIter.Next()) {
          TopoDS_Face face = TopoDS::Face(anIter.Value());
          //TopoDS_Face face1 = TopoDS::Face(aContext->Apply(anIter.Value()));
          TopoDS_Face face1 = TopoDS::Face(myContext->Apply(anIter.Value()));
          if (face1.IsSame(aFace)) continue;
          if (aMapFacesEdges.Contains(face)) {
            aMapFacesEdges.ChangeFromKey(face).Append(edge);
          }
          else {
            TopTools_ListOfShape ListEdges;
            ListEdges.Append(edge);
            aMapFacesEdges.Add(face,ListEdges);
          }
        }
      }

      for (Standard_Integer i=1; i<=aMapFacesEdges.Extent(); i++) {
        const TopTools_ListOfShape& ListEdges = aMapFacesEdges.FindFromIndex(i);
        TopTools_SequenceOfShape SeqEdges;
        TopTools_ListIteratorOfListOfShape anIter(ListEdges);
        for ( ; anIter.More(); anIter.Next()) {
          SeqEdges.Append(anIter.Value());
        }
        if (SeqEdges.Length()==1) continue;
        TopoDS_Edge E;
        if ( MergeEdges(SeqEdges,aFace,Tol,myConcatBSplines,E) ) {
          // now we have only one edge - aChain.Value(1)
          // we have to replace old ListEdges with this new edge
          //aContext->Replace(SeqEdges(1),E);
          myContext->Replace(SeqEdges(1),E);
          Standard_Integer j;
          for (j = 2; j <= SeqEdges.Length(); j++) {
            //aContext->Remove(SeqEdges(j));
            myContext->Remove(SeqEdges(j));
            //myOldNewMap.Bind(SeqEdges(j), E);
          }
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

    } // end processing each face

    // fix changed faces and replace them in the local context
    for (Standard_Integer i=1; i<=ChangedFaces.Extent(); i++) {
      //TopoDS_Face aFace = TopoDS::Face(aContext->Apply(ChangedFaces.FindKey(i)));
      TopoDS_Face aFace = TopoDS::Face(myContext->Apply(ChangedFaces.FindKey(i)));
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
      // put new solid into global context
      myContext->Replace(aSolid,aRes2);
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

  } // end processing each solid

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
  /*
  if (aNewShape.IsNull())
    aNewShape = myOldNewMap(aShape);
  */
  
  return aNewShape;
}
