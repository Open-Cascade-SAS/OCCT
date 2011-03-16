// File:	HLRBRep.cxx
// Created:	Thu Aug 27 12:33:14 1992
// Author:	Christophe MARION
//		<cma@sdsun2>

#include <HLRBRep.ixx>
#include <BRepLib_MakeEdge2d.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

//=======================================================================
//function : MakeEdge
//purpose  : 
//=======================================================================

TopoDS_Edge HLRBRep::MakeEdge (const HLRBRep_Curve& ec,
			       const Standard_Real U1,
			       const Standard_Real U2)
{
  TopoDS_Edge Edg;
  //gp_Pnt2d P,P1,P2;
  Standard_Real sta3d = U1;
  Standard_Real end3d = U2;
  Standard_Real sta   = ec.Parameter2d(U1);
  Standard_Real end   = ec.Parameter2d(U2);
  
  if (ec.GetType() == GeomAbs_Line) {
    Edg = BRepLib_MakeEdge2d(ec.Line(),sta,end);
  }
  else if (ec.GetType() == GeomAbs_Circle) {
    Edg = BRepLib_MakeEdge2d(ec.Circle(),sta,end);
  }
  else if (ec.GetType() == GeomAbs_Ellipse) {
    Edg = BRepLib_MakeEdge2d(ec.Ellipse(),sta,end);
  }
  else if (ec.GetType() == GeomAbs_Hyperbola) {
    Edg = BRepLib_MakeEdge2d(ec.Hyperbola(),sta,end);
  }
  else if (ec.GetType() == GeomAbs_Parabola) {
    Edg = BRepLib_MakeEdge2d(ec.Parabola(),sta,end);
  }
  else if (ec.GetType() == GeomAbs_BezierCurve) {
    TColgp_Array1OfPnt2d Poles(1,ec.NbPoles());
    if (ec.IsRational()) {
      TColStd_Array1OfReal Weights(1,ec.NbPoles());
      ec.PolesAndWeights(Poles,Weights);
      Edg = BRepLib_MakeEdge2d(new Geom2d_BezierCurve(Poles,Weights),sta,end);
    }
    else {
      ec.Poles(Poles);
      Edg = BRepLib_MakeEdge2d(new Geom2d_BezierCurve(Poles),sta,end);
    }
  }
  else if (ec.GetType() == GeomAbs_BSplineCurve) {
    TColgp_Array1OfPnt2d    Poles(1,ec.NbPoles());
    TColStd_Array1OfReal    knots(1,ec.NbKnots());
    TColStd_Array1OfInteger mults(1,ec.NbKnots());
    //-- ec.KnotsAndMultiplicities(knots,mults);
    ec.Knots(knots);
    ec.Multiplicities(mults);
    if (ec.IsRational()) {
      TColStd_Array1OfReal Weights(1,ec.NbPoles());
      ec.PolesAndWeights(Poles,Weights);
      Edg = BRepLib_MakeEdge2d
	(new Geom2d_BSplineCurve
	 (Poles,Weights,knots,mults,ec.Degree()),sta,end);
    }
    else {
      ec.Poles(Poles);
      Edg = BRepLib_MakeEdge2d
	(new Geom2d_BSplineCurve(Poles,knots,mults,ec.Degree()),sta,end);
    }
  }
  else {
    Standard_Integer nbPnt = 15;
    TColgp_Array1OfPnt2d    Poles(1,nbPnt);
    TColStd_Array1OfReal    knots(1,nbPnt);
    TColStd_Array1OfInteger mults(1,nbPnt);
    mults.Init(1);
    mults(1    ) = 2;
    mults(nbPnt) = 2;
    Standard_Real step = (end3d-sta3d)/(nbPnt-1);
    
    for (Standard_Integer i = 1; i <= nbPnt; i++) {
      Poles(i) = ec.Value(sta3d);
      knots(i) = sta3d;
      sta3d += step;
    }
    Edg = BRepLib_MakeEdge2d
      (new Geom2d_BSplineCurve(Poles,knots,mults,1),sta,end);
  }
  return Edg;
}

//=======================================================================
//function : PolyHLRAngleAndDeflection
//purpose  : 
//=======================================================================

void 
HLRBRep::PolyHLRAngleAndDeflection (const Standard_Real InAngl,
				    Standard_Real& OutAngl,
				    Standard_Real& OutDefl)
{
  static Standard_Real HAngMin =  1*PI/180;
  static Standard_Real HAngLim =  5*PI/180;
  static Standard_Real HAngMax = 35*PI/180;

  OutAngl = InAngl;
  if (OutAngl < HAngMin) OutAngl = HAngMin;
  if (OutAngl > HAngMax) OutAngl = HAngMax;
  OutAngl = HAngLim + sqrt((OutAngl - HAngMin) * (HAngMax - HAngLim) *
			   (HAngMax - HAngLim) / (HAngMax - HAngMin));
  OutDefl = OutAngl * OutAngl * 0.5;
}

