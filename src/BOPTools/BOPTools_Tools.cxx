// File:	BOPTools_Tools.cxx
// Created:	Thu Nov 16 11:42:23 2000
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOPTools_Tools.ixx>

#include <Precision.hxx>

#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedDataMapOfShapeShape.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Iterator.hxx>

#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <gp_Pnt2d.hxx>

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRep_Tool.hxx>

#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>

#include <GeomAdaptor_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomAbs_CurveType.hxx>

#include <BooleanOperations_OnceExplorer.hxx>

#include <IntTools_ShrunkRange.hxx>
#include <IntTools_Tools.hxx>
#include <IntTools_Range.hxx>


static  
  void CopySrc(const TopoDS_Shape& E, 
	       TopTools_IndexedDataMapOfShapeShape& aMapSS, 
	       TopoDS_Shape& anEdge);

//=======================================================================
// function: IsBlockInOnFace
// purpose: 
//=======================================================================
  Standard_Boolean BOPTools_Tools::IsBlockInOnFace (const BOPTools_PaveBlock& aPB1,
						    const TopoDS_Face& aF,
						    IntTools_Context& aContext)
{
  Standard_Boolean bFlag;
  Standard_Real f1, l1, ULD, VLD;
  gp_Pnt2d aP2D;
  gp_Pnt aP11, aP12;
  //
  // Edge's data 
  const IntTools_ShrunkRange& aSR1=aPB1.ShrunkRange();
  const TopoDS_Edge& aE1=aSR1.Edge();
 
  const IntTools_Range& aShrR= aSR1.ShrunkRange();
  aShrR.Range(f1, l1);
  //
  Standard_Real dt=0.0075,  k;//dt=0.001,  k;
  k=dt*(l1-f1);
  f1=f1+k;
  l1=l1-k;
  //
  // Treatment P11
  BOPTools_Tools::PointOnEdge(aE1, f1, aP11);
  //
  GeomAPI_ProjectPointOnSurf& aProjector=aContext.ProjPS(aF);
  aProjector.Perform(aP11);
  //
  bFlag=aProjector.IsDone();
  if (!bFlag) {
    return bFlag;
  }
  
  aProjector.LowerDistanceParameters(ULD, VLD);
  aP2D.SetCoord(ULD, VLD);
  //
  bFlag=aContext.IsPointInOnFace (aF, aP2D);
  //
  if (!bFlag) {
    return bFlag;
  }
  //
  // Treatment P12
  BOPTools_Tools::PointOnEdge(aE1, l1, aP12);
  //
  aProjector.Perform(aP12);
  //
  bFlag=aProjector.IsDone();
  if (!bFlag) {
    return bFlag;
  }
  
  aProjector.LowerDistanceParameters(ULD, VLD);
  aP2D.SetCoord(ULD, VLD);
  //
  bFlag=aContext.IsPointInOnFace (aF, aP2D);
  //
  if (!bFlag) {
    return bFlag;
  }
  //
  // Treatment intemediate
  Standard_Real m1, aTolF, aTolE, aTol, aDist;
  m1=IntTools_Tools::IntermediatePoint(f1, l1);
  BOPTools_Tools::PointOnEdge(aE1, m1, aP12);
  //
  aProjector.Perform(aP12);
  //
  bFlag=aProjector.IsDone();
  if (!bFlag) {
    return bFlag;
  }
  //
  aTolE=BRep_Tool::Tolerance(aE1);
  aTolF=BRep_Tool::Tolerance(aF);
  aTol=aTolE+aTolF;
  aDist=aProjector.LowerDistance();
  if (aDist > aTol){
   return Standard_False;
  }

  aProjector.LowerDistanceParameters(ULD, VLD);
  aP2D.SetCoord(ULD, VLD);
  //
  bFlag=aContext.IsPointInOnFace (aF, aP2D);
  //
  if (!bFlag) {
    return bFlag;
  }
  return bFlag;
}

//=======================================================================
// function: IsBlocksCoinside
// purpose: 
//=======================================================================
  Standard_Boolean BOPTools_Tools::IsBlocksCoinside(const BOPTools_PaveBlock& aPB1,
						    const BOPTools_PaveBlock& aPB2)
{
  //
  Standard_Real f1, l1, aTol1;
  const IntTools_ShrunkRange aSR1=aPB1.ShrunkRange();
  const TopoDS_Edge& aE1=aSR1.Edge();
  aTol1=BRep_Tool::Tolerance(aE1);
  aPB1.Parameters(f1, l1);
  gp_Pnt aP11, aP12;
  BOPTools_Tools::PointOnEdge(aE1, f1, aP11);
  BOPTools_Tools::PointOnEdge(aE1, l1, aP12);
  //
  Standard_Real f2, l2, aTol2;
  const IntTools_ShrunkRange aSR2=aPB2.ShrunkRange();
  const TopoDS_Edge& aE2=aSR2.Edge();
  aTol2=BRep_Tool::Tolerance(aE2);
  aPB2.Parameters(f2, l2);
  gp_Pnt aP21, aP22;
  BOPTools_Tools::PointOnEdge(aE2, f2, aP21);
  BOPTools_Tools::PointOnEdge(aE2, l2, aP22);
  ////
  Standard_Real aTolSum, d1121, d1122, d1222, d1221;

  aTolSum=1.05*(aTol1+aTol2);
  
  d1121=aP11.Distance(aP21);
  if (d1121<aTolSum) {
    d1222=aP12.Distance(aP22);
    if (d1222<aTolSum) {
      return Standard_True;
    }
  }
  //
  d1122=aP11.Distance(aP22);
  if (d1122<aTolSum) {
    d1221=aP12.Distance(aP21);
    if (d1221<aTolSum) {
      return Standard_True;
    }
  }
  return Standard_False;
}
//
//=======================================================================
// function: UpdateVertex
// purpose: 
//=======================================================================
  void BOPTools_Tools::UpdateVertex (const TopoDS_Vertex& aVF,
				     const TopoDS_Vertex& aNewVertex)
{
  Standard_Real aTolVF, aTolNewVertex, aDist, aDTol=1.e-12, aNewTol;
  //
  gp_Pnt aPVF=BRep_Tool::Pnt(aVF);
  gp_Pnt aPNewVertex=BRep_Tool::Pnt(aNewVertex);
  aTolVF=BRep_Tool::Tolerance(aVF);
  aTolNewVertex=BRep_Tool::Tolerance(aNewVertex);

  aDist=aPVF.Distance(aPNewVertex);
  aNewTol=aDist+aTolNewVertex;

  if (aNewTol>aTolVF) {
    BRep_Builder BB;
    BB.UpdateVertex (aVF, aNewTol+aDTol);
  }
}
//
//=======================================================================
// function: UpdateVertex
// purpose: 
//=======================================================================
  void BOPTools_Tools::UpdateVertex (const TopoDS_Edge& aE,
				     const Standard_Real  aT,
				     const TopoDS_Vertex& aV)
{
  Standard_Real aTolV, aDist, aDTol=1.e-12, aFirst, aLast;
  gp_Pnt  aPc; 

  gp_Pnt aPv=BRep_Tool::Pnt(aV);
  aTolV=BRep_Tool::Tolerance(aV);

  Handle(Geom_Curve) aC3D=BRep_Tool::Curve(aE, aFirst, aLast);
  aC3D->D0(aT, aPc);
  aDist=aPv.Distance(aPc);
  if (aDist>aTolV) {
    BRep_Builder BB;
    BB.UpdateVertex (aV, aDist+aDTol);
  }
}
//
//=======================================================================
// function: UpdateVertex
// purpose: 
//=======================================================================
  void BOPTools_Tools::UpdateVertex (const IntTools_Curve& aC,
				     const Standard_Real  aT,
				     const TopoDS_Vertex& aV)
{
  Standard_Real aTolV, aDist, aDTol=1.e-12;
  gp_Pnt  aPc; 

  gp_Pnt aPv=BRep_Tool::Pnt(aV);
  aTolV=BRep_Tool::Tolerance(aV);

  Handle(Geom_Curve) aC3D=aC.Curve();
  aC3D->D0(aT, aPc);
  aDist=aPv.Distance(aPc);
  if (aDist>aTolV) {
    BRep_Builder BB;
    BB.UpdateVertex (aV, aDist+aDTol);
  }
}
//=======================================================================
// function: MakeSectEdge
// purpose: 
//=======================================================================
  void BOPTools_Tools::MakeSectEdge(const IntTools_Curve& aIC,
				     const TopoDS_Vertex& aV1,
				     const Standard_Real  aP1,
				     const TopoDS_Vertex& aV2,
				     const Standard_Real  aP2,
				     TopoDS_Edge& aNewEdge)
{
  Handle(Geom_Curve) aC=aIC.Curve ();
  
  BRepBuilderAPI_MakeEdge aMakeEdge(aC, aV1, aV2, aP1, aP2);
  
  const TopoDS_Edge& aE=TopoDS::Edge(aMakeEdge.Shape());
  //
  // Range must be as it was !
  BRep_Builder aBB;
  aBB.Range (aE, aP1, aP2);
  //
  aNewEdge=aE;
  
}

//=======================================================================
// function: MakeSplitEdge
// purpose: 
//=======================================================================
  void BOPTools_Tools::MakeSplitEdge(const TopoDS_Edge&   aE,
				     const TopoDS_Vertex& aV1,
				     const Standard_Real  aP1,
				     const TopoDS_Vertex& aV2,
				     const Standard_Real  aP2,
				     TopoDS_Edge& aNewEdge)
{
  Standard_Real f, l, aTol;
  Handle(Geom_Curve) aC=BRep_Tool::Curve (aE, f, l);
  aTol=BRep_Tool::Tolerance(aE);
  //
  // MakeEdge is used for chechking all input data only 
  BRepBuilderAPI_MakeEdge aMakeEdge(aC, aV1, aV2, aP1, aP2);
  //ZZ const TopoDS_Edge& E1=TopoDS::Edge(aMakeEdge.Shape());
  TopoDS_Edge E=aE;
  E.EmptyCopy();

  BRep_Builder BB;
  BB.Add  (E, aV1);
  BB.Add  (E, aV2);
  BB.Range(E, aP1, aP2);
  BB.UpdateEdge(E, aTol);
  aNewEdge=E;
}

//=======================================================================
// function: MakeNewVertex
// purpose: 
//=======================================================================
  void BOPTools_Tools::MakeNewVertex(const TopoDS_Vertex& aV1,
				     const TopoDS_Vertex& aV2,
				     TopoDS_Vertex& aNewVertex)
{
  gp_Pnt aPnt1=BRep_Tool::Pnt(aV1);
  Standard_Real aTol1=BRep_Tool::Tolerance(aV1);
	
  gp_Pnt aPnt2=BRep_Tool::Pnt(aV2);
  Standard_Real aTol2=BRep_Tool::Tolerance(aV2);

  Standard_Real aMaxTol, aDist;
	
  aDist=aPnt1.Distance(aPnt2);
  aMaxTol=(aTol1>aTol2)? aTol1 : aTol2;
  aMaxTol=aMaxTol+0.5*aDist;
	
  const gp_XYZ& aXYZ1=aPnt1.XYZ();
  const gp_XYZ& aXYZ2=aPnt2.XYZ();
  gp_XYZ aNewXYZ=0.5*(aXYZ1+aXYZ2);
	
  gp_Pnt aNewPnt(aNewXYZ);
  BRep_Builder aBB;
  aBB.MakeVertex (aNewVertex, aNewPnt, aMaxTol);
}
 //=======================================================================
// function: MakeNewVertex
// purpose: 
//=======================================================================
  void BOPTools_Tools::MakeNewVertex(const gp_Pnt& aP,
				     const Standard_Real aTol,
				     TopoDS_Vertex& aNewVertex)
{
  BRep_Builder aBB;
  aBB.MakeVertex (aNewVertex, aP, aTol);
}

//=======================================================================
// function: MakeNewVertex
// purpose: 
//=======================================================================
  void BOPTools_Tools::MakeNewVertex(const TopoDS_Edge& aE1,
				     const Standard_Real aParm1,
				     const TopoDS_Edge& aE2,
				     const Standard_Real aParm2,
				     TopoDS_Vertex& aNewVertex)
{
  Standard_Real aTol1, aTol2, aMaxTol, aDist; 
  gp_Pnt aPnt1, aPnt2;

  PointOnEdge (aE1, aParm1, aPnt1);
  PointOnEdge (aE2, aParm2, aPnt2);

  aTol1=BRep_Tool::Tolerance(aE1);
  aTol2=BRep_Tool::Tolerance(aE2);
  
  aDist=aPnt1.Distance(aPnt2);
  aMaxTol=(aTol1>aTol2)? aTol1 : aTol2;
  aMaxTol=aMaxTol+0.5*aDist;

  const gp_XYZ& aXYZ1=aPnt1.XYZ();
  const gp_XYZ& aXYZ2=aPnt2.XYZ();
  gp_XYZ aNewXYZ=0.5*(aXYZ1+aXYZ2);
	
  gp_Pnt aNewPnt(aNewXYZ);
  BRep_Builder aBB;
  aBB.MakeVertex (aNewVertex, aNewPnt, aMaxTol);
}
//=======================================================================
// function: MakeNewVertex
// purpose: 
//=======================================================================
  void BOPTools_Tools::MakeNewVertex(const TopoDS_Edge& aE1,
				     const Standard_Real aParm1,
				     const TopoDS_Face& aF1,
				     TopoDS_Vertex& aNewVertex)
{
  Standard_Real aTol1, aTol2, aMaxTol, delta=1.e-12; 
  gp_Pnt aPnt;

  PointOnEdge (aE1, aParm1, aPnt);

  aTol1=BRep_Tool::Tolerance(aE1);
  aTol2=BRep_Tool::Tolerance(aF1);
  //
  //aMaxTol=(aTol1>aTol2)? aTol1 : aTol2;
  aMaxTol=aTol1+aTol2+delta;
  //
  BRep_Builder aBB;
  aBB.MakeVertex (aNewVertex, aPnt, aMaxTol);
}

//=======================================================================
// function: PointOnEdge
// purpose: 
//=======================================================================
  void BOPTools_Tools::PointOnEdge(const TopoDS_Edge& aE,
				   const Standard_Real aParm,
				   gp_Pnt& aPnt)
{
  Standard_Real f, l;
  Handle(Geom_Curve) C1=BRep_Tool::Curve(aE, f, l);
  C1->D0(aParm, aPnt);
}

//=======================================================================
//function : CopySource
//purpose  : 
//=======================================================================
  void BOPTools_Tools::CopySource(const TopoDS_Shape& aSS, 
				  TopoDS_Shape& aSD)
{
  TopTools_IndexedDataMapOfShapeShape aMapSS;
  CopySrc (aSS, aMapSS, aSD);
}

//=======================================================================
//function : MapShapes
//purpose  : 
//=======================================================================
  void BOPTools_Tools::MapShapes(const TopoDS_Shape& aS,
				 TopTools_IndexedMapOfShape& aM)
{
  aM.Add(aS);
  TopoDS_Iterator anIt;
  anIt.Initialize(aS);
  for (; anIt.More(); anIt.Next()) {
    const TopoDS_Shape& aSx=anIt.Value();
    BOPTools_Tools::MapShapes(aSx, aM);
  }
}

//=======================================================================
//function : CorrectRange
//purpose  : 
//=======================================================================
  void BOPTools_Tools::CorrectRange(const TopoDS_Edge& aE1,
				    const TopoDS_Edge& aE2,
				    const IntTools_Range& aSR,
				    IntTools_Range& aNewSR)
{
  Standard_Integer i;
  Standard_Real aRes, aTolE1, aTolE2, aTF, aTL, dT;
  BRepAdaptor_Curve aBC;
  GeomAbs_CurveType aCT;
  gp_Pnt aP;
  gp_Vec aDer;
  //
  aNewSR=aSR;
  //
  //modified by NIZNHY-PKV Tue Feb 10 08:47:03 2009f
  aBC.Initialize(aE1);
  aCT=aBC.GetType();
  if (aCT==GeomAbs_Line) {
    return;
  }
  //modified by NIZNHY-PKV Tue Feb 10 08:47:06 2009t
  //
  dT=Precision::PConfusion();
  aTF=aSR.First();
  aTL=aSR.Last();
  //
  //modified by NIZNHY-PKV Tue Feb 10 08:47:39 2009f
  /*
  aBC.Initialize(aE1);
  aCT=aBC.GetType();
  */
  //modified by NIZNHY-PKV Tue Feb 10 08:47:43 2009t
  //
  aTolE1=BRep_Tool::Tolerance(aE1);
  aTolE2=BRep_Tool::Tolerance(aE2);
  //
  for(i=0; i<2; ++i) {
    aRes = 2.*(aTolE1 + aTolE2);
    //
    if (aCT==GeomAbs_BezierCurve ||
	aCT==GeomAbs_BSplineCurve||
	aCT==GeomAbs_OtherCurve) {
      
      if(!i){
	aBC.D1 (aTF, aP, aDer);
      }
      else {
	aBC.D1 (aTL, aP, aDer);
      }
      //
      Standard_Real aMgn = aDer.Magnitude();
      
      if(aMgn  > 1.e-12) {
	aRes = aRes/aMgn ;
      }
      else {
	aRes = aBC.Resolution(aRes);
      }
    } // if (aCT==GeomAbs_BezierCurve||...
    else {
      aRes = aBC.Resolution(aRes);
    }
    //
    if(!i) {
      aNewSR.SetFirst (aTF+aRes);
    }
    else {
      aNewSR.SetLast (aTL-aRes);
    }
    //
    if ((aNewSR.Last()-aNewSR.First()) < dT) {
      aNewSR=aSR;
    }
    //aNewSR=((aNewSR.Last()-aNewSR.First()) < dT) ? aSR : aNewSR;
  }
}
				    
//=======================================================================
//function : CorrectRange
//purpose  : 
//=======================================================================
  void BOPTools_Tools::CorrectRange(const TopoDS_Edge& aE,
				    const TopoDS_Face& aF,
				    const IntTools_Range& aSR,
				    IntTools_Range& aNewSR)
{
  Standard_Integer i;
  Standard_Real aRes, aTolF, aTF, aTL, dT;
  BRepAdaptor_Curve aBC;
  GeomAbs_CurveType aCT;
  gp_Pnt aP;
  gp_Vec aDer;
  //
  aNewSR=aSR;
  //
  dT=Precision::PConfusion();
  aTF=aSR.First();
  aTL=aSR.Last();
  //
  aBC.Initialize(aE);
  aCT=aBC.GetType();
  //
  aTolF=BRep_Tool::Tolerance(aF);
  //
  for(i=0; i<2; ++i) {
    aRes =aTolF;

    if (aCT==GeomAbs_BezierCurve ||
	aCT==GeomAbs_BSplineCurve||
	aCT==GeomAbs_OtherCurve) {
      
      if(!i){
	aBC.D1 (aTF, aP, aDer);
      }
      else {
	aBC.D1 (aTL, aP, aDer);
      }
      //
      Standard_Real aMgn = aDer.Magnitude();
      
      if(aMgn  > 1.e-12) {
	aRes = aRes/aMgn ;
      }
      else {
	aRes = aBC.Resolution(aRes);
      }
    } // if (aCT==GeomAbs_BezierCurve||...
    else {
      aRes = aBC.Resolution(aRes);
    }
    //
    if(!i) {
      aNewSR.SetFirst (aTF+aRes);
    }
    else {
      aNewSR.SetLast (aTL-aRes);
    }
    //
    if ((aNewSR.Last()-aNewSR.First()) < dT) {
      aNewSR=aSR;
    }
  }
}

//=======================================================================
//function : CopySrc
//purpose  : 
//=======================================================================
void CopySrc(const TopoDS_Shape& E, 
	     TopTools_IndexedDataMapOfShapeShape& aMapSS, 
	     TopoDS_Shape& anEdge)
{
  BRep_Builder BB;
  TopAbs_ShapeEnum aTT;
  Standard_Integer aR;
  aTT=E.ShapeType();

  if (aMapSS.Contains(E)) {
    anEdge=aMapSS.ChangeFromKey(E);
    if (aTT==TopAbs_EDGE)
      return;
  }
  else {
    anEdge=E.EmptyCopied();
    aMapSS.Add(E, anEdge);
  }
  
  aR=(Standard_Integer)aTT+1;

  if (aR>TopAbs_VERTEX) {
    return;
  }

  Standard_Boolean free = anEdge.Free();
  anEdge.Free(Standard_True);
 
 aTT=(TopAbs_ShapeEnum) aR;
  TopExp_Explorer anExpVertices(E, aTT);
  for (; anExpVertices.More();  anExpVertices.Next()) {
    const TopoDS_Shape& V=anExpVertices.Current();
    TopoDS_Shape aVertex;

    CopySrc (V, aMapSS, aVertex);  

    aVertex.Orientation(V.Orientation());
    BB.Add(anEdge, aVertex);
  }

  anEdge.Free(free);
}
