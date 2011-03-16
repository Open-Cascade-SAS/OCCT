// File:	Geom2dConvert_BSplineCurveToBezierCurve.cxx
// Created:	Tue Mar 12 15:51:42 1996
// Author:	Bruno DUMORTIER
//		<dub@fuegox>


#include <Geom2dConvert_BSplineCurveToBezierCurve.ixx>

#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>


//=======================================================================
//function : Geom2dConvert_BSplineCurveToBezierCurve
//purpose  : 
//=======================================================================

Geom2dConvert_BSplineCurveToBezierCurve::Geom2dConvert_BSplineCurveToBezierCurve (const Handle(Geom2d_BSplineCurve)& BasisCurve)
{
  myCurve = Handle(Geom2d_BSplineCurve)::DownCast(BasisCurve->Copy());
  // periodic curve can't be converted correctly by two main reasons:
  // last pole (equal to first one) is missing;
  // poles recomputation using default boor scheme is fails.
  if(myCurve->IsPeriodic()) myCurve->SetNotPeriodic();
  Standard_Real Uf = myCurve->FirstParameter();
  Standard_Real Ul = myCurve->LastParameter();
  myCurve->Segment(Uf,Ul);
  myCurve->IncreaseMultiplicity(myCurve->FirstUKnotIndex(),
				myCurve->LastUKnotIndex(),
				myCurve->Degree());
}


//=======================================================================
//function : Geom2dConvert_BSplineCurveToBezierCurve
//purpose  : 
//=======================================================================

Geom2dConvert_BSplineCurveToBezierCurve::Geom2dConvert_BSplineCurveToBezierCurve
(const Handle(Geom2d_BSplineCurve)& BasisCurve, 
 const Standard_Real U1, 
 const Standard_Real U2,
 const Standard_Real ParametricTolerance)
{
 if (U2 - U1 < ParametricTolerance)
      Standard_DomainError::Raise("GeomConvert_BSplineCurveToBezierSurface");

  Standard_Real Uf = U1, Ul = U2; 
  Standard_Real PTol = ParametricTolerance/2 ;

  Standard_Integer I1, I2;
  myCurve = Handle(Geom2d_BSplineCurve)::DownCast(BasisCurve->Copy());
  if(myCurve->IsPeriodic()) myCurve->SetNotPeriodic();

  myCurve->LocateU(U1, PTol, I1, I2);
  if (I1==I2) { // On est sur le noeud
    if ( myCurve->Knot(I1) > U1) Uf = myCurve->Knot(I1);
  }

  myCurve->LocateU(U2, PTol, I1, I2);
  if (I1==I2) { // On est sur le noeud
    if ( myCurve->Knot(I1) < U2) Ul = myCurve->Knot(I1);
  }


  myCurve->Segment(Uf, Ul);
  myCurve->IncreaseMultiplicity(myCurve->FirstUKnotIndex(),
				myCurve->LastUKnotIndex(),
				myCurve->Degree());
}


//=======================================================================
//function : Arc
//purpose  : 
//=======================================================================

Handle(Geom2d_BezierCurve) Geom2dConvert_BSplineCurveToBezierCurve::Arc
(const Standard_Integer Index)
{
  if ( Index < 1 || Index > myCurve->NbKnots()-1) {
    Standard_OutOfRange::Raise("Geom2dConvert_BSplineCurveToBezierCurve");
  }
  Standard_Integer Deg = myCurve->Degree();

  TColgp_Array1OfPnt2d Poles(1,Deg+1);

  Handle(Geom2d_BezierCurve) C;
  if ( myCurve->IsRational()) {
    TColStd_Array1OfReal Weights(1,Deg+1);
    for ( Standard_Integer i = 1; i <= Deg+1; i++) {
      Poles(i)   = myCurve->Pole(i+Deg*(Index-1));
      Weights(i) = myCurve->Weight(i+Deg*(Index-1));
    }
    C = new Geom2d_BezierCurve(Poles,Weights);
  }
  else {
    for ( Standard_Integer i = 1; i <= Deg+1; i++) {
      Poles(i)   = myCurve->Pole(i+Deg*(Index-1));
    }
    C = new Geom2d_BezierCurve(Poles);
  }
  return C;
}


//=======================================================================
//function : Arcs
//purpose  : 
//=======================================================================

void Geom2dConvert_BSplineCurveToBezierCurve::Arcs
(TColGeom2d_Array1OfBezierCurve& Curves)
{
  Standard_Integer n = NbArcs();
  for ( Standard_Integer i = 1; i <= n; i++) {
    Curves(i) = Arc(i);
  } 
}

//=======================================================================
//function : Knots
//purpose  : 
//=======================================================================

void Geom2dConvert_BSplineCurveToBezierCurve::Knots
     (TColStd_Array1OfReal& TKnots) const
{
 Standard_Integer ii, kk;
  for (ii = 1, kk = TKnots.Lower();
       ii <=myCurve->NbKnots(); ii++, kk++)
    TKnots(kk) = myCurve->Knot(ii);
}

//=======================================================================
//function : NbArcs
//purpose  : 
//=======================================================================

Standard_Integer Geom2dConvert_BSplineCurveToBezierCurve::NbArcs() const 
{
  return (myCurve->NbKnots()-1);
}
