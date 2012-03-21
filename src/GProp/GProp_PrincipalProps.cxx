// Copyright (c) 1995-1999 Matra Datavision
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

#include <GProp_PrincipalProps.ixx>


typedef gp_Vec Vec;
typedef gp_Pnt Pnt;





   GProp_PrincipalProps::GProp_PrincipalProps () {

      i1= i2 = i3 = RealLast();
      r1 = r2 = r3 = RealLast();
      v1 = Vec (1.0, 0.0, 0.0);
      v2 = Vec (0.0, 1.0, 0.0);
      v3 = Vec (0.0, 0.0, 1.0);
      g = Pnt (RealLast(), RealLast(), RealLast());
   }


   GProp_PrincipalProps::GProp_PrincipalProps (
   const Standard_Real Ixx, const Standard_Real Iyy, const Standard_Real Izz, 
   const Standard_Real Rxx, const Standard_Real Ryy, const Standard_Real Rzz,
   const gp_Vec& Vxx, const gp_Vec& Vyy, const gp_Vec& Vzz, const gp_Pnt& G) :
   i1 (Ixx), i2 (Iyy), i3 (Izz), r1 (Rxx), r2 (Ryy), r3 (Rzz),
   v1 (Vxx), v2 (Vyy), v3 (Vzz), g (G) { }


   Standard_Boolean GProp_PrincipalProps::HasSymmetryAxis () const {

//     Standard_Real Eps1 = Abs(Epsilon (i1));
//     Standard_Real Eps2 = Abs(Epsilon (i2));
     const Standard_Real aRelTol = 1.e-10;
     Standard_Real Eps1 = Abs(i1)*aRelTol;
     Standard_Real Eps2 = Abs(i2)*aRelTol;
     return (Abs (i1 - i2) <= Eps1 || Abs (i1 - i3) <= Eps1 ||
             Abs (i2 - i3) <= Eps2);
   }

   Standard_Boolean GProp_PrincipalProps::HasSymmetryAxis (const Standard_Real aTol) const {


     Standard_Real Eps1 = Abs(i1*aTol) + Abs(Epsilon(i1));
     Standard_Real Eps2 = Abs(i2*aTol) + Abs(Epsilon(i2));
     return (Abs (i1 - i2) <= Eps1 || Abs (i1 - i3) <= Eps1 ||
             Abs (i2 - i3) <= Eps2);
   }


   Standard_Boolean GProp_PrincipalProps::HasSymmetryPoint () const {

//     Standard_Real Eps1 = Abs(Epsilon (i1));
     const Standard_Real aRelTol = 1.e-10;
     Standard_Real Eps1 = Abs(i1)*aRelTol;
     return (Abs (i1 - i2) <= Eps1 && Abs (i1 - i3) <= Eps1);
   }

   Standard_Boolean GProp_PrincipalProps::HasSymmetryPoint (const Standard_Real aTol) const {

     Standard_Real Eps1 = Abs(i1*aTol) + Abs(Epsilon(i1));
     return (Abs (i1 - i2) <= Eps1 && Abs (i1 - i3) <= Eps1);
   }


   void GProp_PrincipalProps::Moments (Standard_Real& Ixx, Standard_Real& Iyy, Standard_Real& Izz) const {

      Ixx = i1;
      Iyy = i2;
      Izz = i3;
   }


   const Vec& GProp_PrincipalProps::FirstAxisOfInertia () const {return  v1;}


   const Vec& GProp_PrincipalProps::SecondAxisOfInertia () const {return  v2;}


   const Vec& GProp_PrincipalProps::ThirdAxisOfInertia () const {return v3;}


   void GProp_PrincipalProps::RadiusOfGyration (
   Standard_Real& Rxx, Standard_Real& Ryy, Standard_Real& Rzz) const {
   
     Rxx = r1;
     Ryy = r2;
     Rzz = r3;
   }







