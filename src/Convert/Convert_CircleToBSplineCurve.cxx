//File Convert_CircleToBSplineCurve.cxx
//JCV 16/10/91

#include <Convert_CircleToBSplineCurve.ixx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

#include <Precision.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Trsf2d.hxx>

//Attention :
//Pour eviter de trainer des tableaux persistent dans les champs
//on dimensionne les tableaux au maxi (TheNbKnots et TheNbPoles)
//qui correspondent au cercle complet.  Pour un arc de cercle on a
//evidemment besoin de moins de poles et de noeuds, c'est pourquoi les
//champs nbKnots et nbPoles sont presents et sont mis a jour dans le
//constructeur d'un arc de cercle B-spline pour tenir compte du nombre
//effectif de poles et de noeuds.


// parametrization :
// Reference : Rational B-spline for Curve and Surface Representation
//             Wayne Tiller  CADG September 1983
//
// x(t) = (1 - t^2) / (1 + t^2)
// y(t) =  2 t / (1 + t^2)
//
// then t = Sqrt(2) u /  ((Sqrt(2) - 2) u + 2)
//
// => u = 2 t / (Sqrt(2) + (2 - Sqrt(2)) t)

//=======================================================================
//function : Convert_CircleToBSplineCurve
//purpose  : this constructs a periodic circle 
//=======================================================================

Convert_CircleToBSplineCurve::Convert_CircleToBSplineCurve 
  (const gp_Circ2d& C, const Convert_ParameterisationType Parameterisation)
:Convert_ConicToBSplineCurve(0,0,0){

  Standard_Integer ii ;

  Standard_Real R,
  value ;
  Handle(TColStd_HArray1OfReal) CosNumeratorPtr,
  SinNumeratorPtr ;


  R = C.Radius() ; 
  if (Parameterisation != Convert_TgtThetaOver2 &&
    Parameterisation != Convert_RationalC1) {
    // Dans ce cas BuildCosAndSin ne sait pas gerer la periodicite
    // => on trim sur 0,2*PI
    isperiodic = Standard_False;
    Convert_ConicToBSplineCurve::
      BuildCosAndSin(Parameterisation,
		     0, 2*PI,
		     CosNumeratorPtr,
		     SinNumeratorPtr,
		     weights,
		     degree,
		     knots,
		     mults);
      }   
  else { 
    isperiodic = Standard_True;
    Convert_ConicToBSplineCurve::
      BuildCosAndSin(Parameterisation,
		     CosNumeratorPtr,
		     SinNumeratorPtr,
		     weights,
		     degree,
		     knots,
		     mults);
  }


  nbPoles = CosNumeratorPtr->Length();
  nbKnots = knots->Length();

  poles = 
    new TColgp_HArray1OfPnt2d(1,nbPoles);


  gp_Dir2d Ox = C.XAxis().Direction();
  gp_Dir2d Oy = C.YAxis().Direction();
  gp_Trsf2d Trsf;
  Trsf.SetTransformation( C.XAxis(), gp::OX2d());
  if  ( Ox.X() * Oy.Y() - Ox.Y() * Oy.X() > 0.0e0) {
    value = R ;
  }
  else {
    value = -R ;
   }
  
  // On replace la bspline dans le repere du cercle.
  // et on calcule les poids de la bspline.

  for (ii = 1; ii <= nbPoles ; ii++) {
     poles->ChangeArray1()(ii).SetCoord(1, R * CosNumeratorPtr->Value(ii)) ;
     poles->ChangeArray1()(ii).SetCoord(2, value * SinNumeratorPtr->Value(ii)) ;
     poles->ChangeArray1()(ii).Transform( Trsf); 
   }

}
//=======================================================================
//function : Convert_CircleToBSplineCurve
//purpose  : this constructs a non periodic circle
//=======================================================================

Convert_CircleToBSplineCurve::Convert_CircleToBSplineCurve 
  (const gp_Circ2d& C, 
   const Standard_Real  UFirst,
   const Standard_Real  ULast,
   const Convert_ParameterisationType Parameterisation)
:Convert_ConicToBSplineCurve(0,0,0)
{
  Standard_Real delta = ULast - UFirst ;
  Standard_Real Eps = Precision::PConfusion();

  if ( (delta > (2*PI + Eps)) || (delta <= 0.0e0) ) {
    Standard_DomainError::Raise( "Convert_CircleToBSplineCurve");
  }

  Standard_Integer ii;
  Standard_Real R, value ;
  Handle(TColStd_HArray1OfReal) CosNumeratorPtr,SinNumeratorPtr ;


  R = C.Radius() ;    
  isperiodic = Standard_False;
  Convert_ConicToBSplineCurve::BuildCosAndSin(Parameterisation,
					      UFirst,
					      ULast,
					      CosNumeratorPtr,
					      SinNumeratorPtr,
					      weights,
					      degree,
					      knots,
					      mults) ;



  nbPoles = CosNumeratorPtr->Length();
  nbKnots = knots->Length();

  poles = 
    new TColgp_HArray1OfPnt2d(1,nbPoles)   ;

  gp_Dir2d Ox = C.XAxis().Direction();
  gp_Dir2d Oy = C.YAxis().Direction();
  gp_Trsf2d Trsf;
  Trsf.SetTransformation( C.XAxis(), gp::OX2d());
  if  ( Ox.X() * Oy.Y() - Ox.Y() * Oy.X() > 0.0e0) {
    value = R ;
  }
  else {
    value = -R ;
   }
  
  // On replace la bspline dans le repere du cercle.
  // et on calcule les poids de la bspline.

  for (ii = 1; ii <= nbPoles ; ii++) {
     poles->ChangeArray1()(ii).SetCoord(1, R * CosNumeratorPtr->Value(ii)) ;
     poles->ChangeArray1()(ii).SetCoord(2, value * SinNumeratorPtr->Value(ii)) ;
     poles->ChangeArray1()(ii).Transform( Trsf); 
   }

}

