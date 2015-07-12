// Copyright (c) 1995-1999 Matra Datavision
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


#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <GProp_PrincipalProps.hxx>
#include <GProp_UndefinedAxis.hxx>

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







