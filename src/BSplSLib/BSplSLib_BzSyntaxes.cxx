// File:	BSplSLib_BzSyntaxes.cxx
// Created:	Fri Oct 27 12:49:43 1995
// Author:	Laurent BOURESCHE
//		<lbo@phylox>

//  pmn  16-10-96 : Correction de PolesCoefficient (PRO5782)
//                  ColLength et RowLength avaient encore frappes !!
//                  

#define No_Standard_RangeError
#define No_Standard_OutOfRange

#include <BSplSLib.hxx>
#include <BSplCLib.hxx>
#include <TColStd_Array1OfReal.hxx>

//=======================================================================
//function : PolesCoefficients
//purpose  : 
//=======================================================================

void BSplSLib::PolesCoefficients (const TColgp_Array2OfPnt& Poles, 
				  const TColStd_Array2OfReal& Weights, 
				  TColgp_Array2OfPnt& CachePoles, 
				  TColStd_Array2OfReal& CacheWeights)
{
  Standard_Integer i;
  Standard_Integer uclas = Poles.ColLength(); 
  Standard_Integer vclas = Poles.RowLength(); 
  TColStd_Array1OfReal biduflatknots(1,uclas << 1);
  TColStd_Array1OfReal bidvflatknots(1,vclas << 1);

  for(i = 1; i <= uclas; i++) {
    biduflatknots(i        ) = 0.;
    biduflatknots(i + uclas) = 1.;
  }

  for(i = 1; i <= vclas; i++) {
    bidvflatknots(i        ) = 0.;
    bidvflatknots(i + vclas) = 1.;
  }
  if ( uclas > vclas) {
    BSplSLib::BuildCache(0.,0.,
			 1.,1.,0,0,
			 uclas - 1,vclas - 1,0,0,
			 biduflatknots,bidvflatknots,
			 Poles,Weights,
			 CachePoles,CacheWeights);
  }
  else {
    // BuilCache exige que les resultats soient formates en [MaxCoeff,MinCoeff]
    TColgp_Array2OfPnt   CPoles  (1,vclas, 1, uclas);
    TColStd_Array2OfReal CWeights(1,vclas, 1, uclas);
    Standard_Integer ii, jj;
    BSplSLib::BuildCache(0.,0.,
			 1.,1.,0,0,
			 uclas - 1,vclas - 1,0,0,
			 biduflatknots,bidvflatknots,
			 Poles,Weights,
			 CPoles,CWeights);
    if (&Weights == NULL) {
      
      for (ii = 1; ii <= uclas; ii++) {
	
	for (jj = 1; jj <= vclas; jj++) {
	  CachePoles(ii, jj) = CPoles(jj, ii);
	}
      }
    }
    else {
      
      for (ii = 1; ii <= uclas; ii++) {
	
	for (jj = 1; jj <= vclas; jj++) {
	  CachePoles  (ii, jj) = CPoles  (jj, ii);
	  CacheWeights(ii, jj) = CWeights(jj, ii);
	}
      }
    }
  }
}

