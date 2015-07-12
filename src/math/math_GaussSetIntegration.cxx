// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

//#ifndef OCCT_DEBUG
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError

//#endif

#include <math.hxx>
#include <math_FunctionSet.hxx>
#include <math_GaussSetIntegration.hxx>
#include <math_Vector.hxx>
#include <Standard_NotImplemented.hxx>
#include <StdFail_NotDone.hxx>

math_GaussSetIntegration::math_GaussSetIntegration(math_FunctionSet& F, 
                                                    const math_Vector& Lower,
                                                    const math_Vector& Upper,
                                                    const math_IntegerVector& Order)
                          : Val(1, F.NbEquations()) {

   Standard_Integer NbEqua = F.NbEquations() , NbVar = F.NbVariables();
   Standard_Integer i;
   Standard_Boolean IsOk;
   math_Vector FVal1(1, NbEqua), FVal2(1, NbEqua), Tval(1, NbVar); 


// Verification
   Standard_NotImplemented_Raise_if(
            NbVar != 1 || Order.Value(Order.Lower()) > math::GaussPointsMax(),
            "GaussSetIntegration ");

// Initialisations
   Done = Standard_False;

   Standard_Real Xdeb = Lower.Value( Lower.Lower() );
   Standard_Real Xfin = Upper.Value( Upper.Lower() );
   Standard_Integer Ordre = Order.Value(Order.Lower());
   Standard_Real Xm, Xr;
   math_Vector GaussP(1, Ordre), GaussW(1, Ordre);

// Recuperation des points de Gauss dans le fichier GaussPoints.
   math::GaussPoints  (Ordre, GaussP);
   math::GaussWeights (Ordre, GaussW);


// Changement de variable pour la mise a l'echelle [Lower, Upper] :
   Xm = 0.5 * (Xdeb + Xfin);
   Xr = 0.5 * (Xfin - Xdeb);

   Standard_Integer ind = Ordre/2, ind1 = (Ordre+1)/2;
   if(ind1 > ind) { // odder case
       Tval(1) =  Xm; // +  Xr * GaussP(ind1);
       IsOk = F.Value(Tval, Val);
       if (!IsOk) return;
       Val *= GaussW(ind1);
     }
   else {
     Val.Init(0);
   }

   for (i=1; i<= ind; i++) {
       Tval(1) =  Xm +  Xr * GaussP(i);
       IsOk = F.Value(Tval, FVal1);
       if (!IsOk) return;
       Tval(1) =  Xm -  Xr * GaussP(i);
       IsOk = F.Value(Tval, FVal2);
       if (!IsOk) return;
       FVal1 += FVal2;
       FVal1 *=  GaussW(i);
       Val += FVal1;
     }
   Val *= Xr;  

   Done = Standard_True;     
 }

void math_GaussSetIntegration::Dump(Standard_OStream& o) const 
{
  o <<"math_GaussSetIntegration ";
   if (Done) {
     o << " Status = Done \n";
     o << "Integration Value = " << Val<<"\n";
   }
   else {
     o << "Status = not Done \n";
   }
}
