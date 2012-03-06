// File:      HLRBRep.cxx
// Created:   Thu Aug 27 12:33:14 1992
// Author:    Christophe MARION
// Copyright: OPEN CASCADE 2000

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
  const Standard_Real sta = ec.Parameter2d(U1);
  const Standard_Real end = ec.Parameter2d(U2);

  switch (ec.GetType())
  {
    case GeomAbs_Line:
      Edg = BRepLib_MakeEdge2d(ec.Line(),sta,end);
      break;

    case GeomAbs_Circle:
      Edg = BRepLib_MakeEdge2d(ec.Circle(),sta,end);
      break;

	case GeomAbs_Ellipse:
      Edg = BRepLib_MakeEdge2d(ec.Ellipse(),sta,end);
      break;

	case GeomAbs_Hyperbola:
      Edg = BRepLib_MakeEdge2d(ec.Hyperbola(),sta,end);
      break;

	case GeomAbs_Parabola:
      Edg = BRepLib_MakeEdge2d(ec.Parabola(),sta,end);
      break;

    case GeomAbs_BezierCurve: {
      TColgp_Array1OfPnt2d Poles(1,ec.NbPoles());
      Handle(Geom2d_BezierCurve) ec2d;
      if (ec.IsRational()) {
        TColStd_Array1OfReal Weights(1,ec.NbPoles());
        ec.PolesAndWeights(Poles,Weights);
        ec2d = new Geom2d_BezierCurve(Poles,Weights);
      }
      else {
        ec.Poles(Poles);
        ec2d = new Geom2d_BezierCurve(Poles);
      }
      BRepLib_MakeEdge2d mke2d(ec2d,sta,end);
      if (mke2d.IsDone())
        Edg = mke2d.Edge();
      break;
    }

	case GeomAbs_BSplineCurve: {
      TColgp_Array1OfPnt2d    Poles(1,ec.NbPoles());
      TColStd_Array1OfReal    knots(1,ec.NbKnots());
      TColStd_Array1OfInteger mults(1,ec.NbKnots());
      //-- ec.KnotsAndMultiplicities(knots,mults);
      ec.Knots(knots);
      ec.Multiplicities(mults);
      Handle(Geom2d_BSplineCurve) ec2d;
      if (ec.IsRational()) {
        TColStd_Array1OfReal Weights(1,ec.NbPoles());
        ec.PolesAndWeights(Poles,Weights);
        ec2d = new Geom2d_BSplineCurve(Poles,Weights,knots,mults,ec.Degree());
      }
      else {
        ec.Poles(Poles);
        ec2d = new Geom2d_BSplineCurve(Poles,knots,mults,ec.Degree());
      }
      BRepLib_MakeEdge2d mke2d(ec2d,sta,end);
      if (mke2d.IsDone())
        Edg = mke2d.Edge();
      break;
    }

	default: {
      const Standard_Integer nbPnt = 15;
      TColgp_Array1OfPnt2d    Poles(1,nbPnt);
      TColStd_Array1OfReal    knots(1,nbPnt);
      TColStd_Array1OfInteger mults(1,nbPnt);
      mults.Init(1);
      mults(1    ) = 2;
      mults(nbPnt) = 2;
      const Standard_Real step = (U2-U1)/(nbPnt-1);
      Standard_Real par3d = U1;
      for (Standard_Integer i = 1; i < nbPnt; i++) {
        Poles(i) = ec.Value(par3d);
        knots(i) = par3d;
        par3d += step;
      }
      Poles(nbPnt) = ec.Value(U2);
      knots(nbPnt) = U2;
      
      Handle(Geom2d_BSplineCurve) ec2d = new Geom2d_BSplineCurve(Poles,knots,mults,1);
      BRepLib_MakeEdge2d mke2d(ec2d,sta,end);
      if (mke2d.IsDone())
        Edg = mke2d.Edge();
    }
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
  static Standard_Real HAngMin =  1*M_PI/180;
  static Standard_Real HAngLim =  5*M_PI/180;
  static Standard_Real HAngMax = 35*M_PI/180;

  OutAngl = InAngl;
  if (OutAngl < HAngMin) OutAngl = HAngMin;
  if (OutAngl > HAngMax) OutAngl = HAngMax;
  OutAngl = HAngLim + sqrt((OutAngl - HAngMin) * (HAngMax - HAngLim) *
			   (HAngMax - HAngLim) / (HAngMax - HAngMin));
  OutDefl = OutAngl * OutAngl * 0.5;
}
