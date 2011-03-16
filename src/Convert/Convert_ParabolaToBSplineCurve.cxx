//File Convert_ParabolaToBSplineCurve.cxx
//JCV 16/10/91


#include <Convert_ParabolaToBSplineCurve.ixx>

#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Trsf2d.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

static Standard_Integer TheDegree  = 2;
static Standard_Integer MaxNbKnots = 2;
static Standard_Integer MaxNbPoles = 3;

//=======================================================================
//function : Convert_ParabolaToBSplineCurve
//purpose  : 
//=======================================================================

Convert_ParabolaToBSplineCurve::Convert_ParabolaToBSplineCurve 
  (const gp_Parab2d&   Prb,
   const Standard_Real U1 ,
   const Standard_Real U2  )
: Convert_ConicToBSplineCurve (MaxNbPoles, MaxNbKnots, TheDegree) 
{
  Standard_DomainError_Raise_if( Abs(U2 - U1) < Epsilon(0.),
				"Convert_ParabolaToBSplineCurve");

  Standard_Real UF = Min (U1, U2);
  Standard_Real UL = Max( U1, U2);
  
  Standard_Real p = Prb.Parameter();

  nbPoles = 3;
  nbKnots = 2;
  isperiodic = Standard_False;
  knots->ChangeArray1()(1) = UF;  mults->ChangeArray1()(1) = 3;
  knots->ChangeArray1()(2) = UL;  mults->ChangeArray1()(2) = 3;

 weights->ChangeArray1()(1) = 1.;
 weights->ChangeArray1()(2) = 1.;
 weights->ChangeArray1()(3) = 1.;

  gp_Dir2d Ox = Prb.Axis().XDirection();
  gp_Dir2d Oy = Prb.Axis().YDirection();
  Standard_Real S = ( Ox.X() * Oy.Y() - Ox.Y() * Oy.X() > 0.) ?  1 : -1;


  // poles exprimes dans le repere de reference
  poles->ChangeArray1()(1) = 
    gp_Pnt2d( ( UF * UF) / ( 2. * p), S *   UF            );
  poles->ChangeArray1()(2) = 
    gp_Pnt2d( ( UF * UL) / ( 2. * p), S * ( UF + UL) / 2. );
  poles->ChangeArray1()(3) = 
    gp_Pnt2d( ( UL * UL) / ( 2. * p), S *   UL            );

  // on replace la bspline dans le repere de la parabole.
  gp_Trsf2d Trsf;
  Trsf.SetTransformation( Prb.Axis().XAxis(), gp::OX2d());
  poles->ChangeArray1()(1).Transform( Trsf);
  poles->ChangeArray1()(2).Transform( Trsf);
  poles->ChangeArray1()(3).Transform( Trsf);
}


