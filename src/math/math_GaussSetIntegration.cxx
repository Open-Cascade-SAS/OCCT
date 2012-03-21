// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//#ifndef DEB
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError
//#endif

#include <math_GaussSetIntegration.ixx>
#include <math.hxx>
#include <math_Vector.hxx>
#include <math_FunctionSet.hxx>


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
