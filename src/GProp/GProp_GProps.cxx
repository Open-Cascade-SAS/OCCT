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


#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Mat.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <GProp.hxx>
#include <GProp_GProps.hxx>
#include <GProp_PrincipalProps.hxx>
#include <math_Jacobi.hxx>
#include <Standard_DomainError.hxx>

GProp_GProps::GProp_GProps () : g (gp::Origin()) , loc (gp::Origin()), dim (0.0) 
{ 
  inertia = gp_Mat(0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
}


GProp_GProps::GProp_GProps (const gp_Pnt& SystemLocation) : 
       g (gp::Origin()), loc (SystemLocation), dim (0.0)
{ 
  inertia = gp_Mat(0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
}



void GProp_GProps::Add (const GProp_GProps& Item, const Standard_Real Density) {
  if (Density <= gp::Resolution())  throw Standard_DomainError();
  if (loc.Distance (Item.loc) <= gp::Resolution ()) {
    gp_XYZ GXYZ = (Item.g.XYZ()).Multiplied (Item.dim * Density);
    g.SetXYZ (g.XYZ().Multiplied (dim));
    GXYZ.Add (g.XYZ());
    dim = dim + Item.dim * Density;
    if (Abs(dim) >= 1.e-20 ) {
      GXYZ.Divide (dim);
      g.SetXYZ (GXYZ);
    }
    else {
      g.SetCoord(0., 0., 0.);
    }
    inertia = inertia + Item.inertia * Density;
  }else{
    gp_XYZ Itemloc = loc.XYZ() - Item.loc.XYZ();
    gp_XYZ Itemg   = Item.loc.XYZ() + Item.g.XYZ();
    gp_XYZ GXYZ    = Item.g.XYZ() - Itemloc;
    GXYZ    = GXYZ.Multiplied (Item.dim * Density);
    g.SetXYZ (g.XYZ().Multiplied (dim));
    GXYZ.Add (g.XYZ());
    dim = dim + Item.dim * Density;
    if (Abs(dim) >= 1.e-20 ) {
      GXYZ.Divide (dim);
      g.SetXYZ (GXYZ);
    }
    else {
      g.SetCoord(0., 0., 0.);
    }
    //We have to compute the inertia of the Item at the location point
    //of the system using the Huyghens theorem
    gp_Mat HMat;
    gp_Mat ItemInertia = Item.inertia;
    if (Item.g.XYZ().Modulus() > gp::Resolution()) {
      //Computes the inertia of Item at its dim centre
      GProp::HOperator (Itemg, Item.loc, Item.dim, HMat);
      ItemInertia = ItemInertia - HMat;
    }
    //Computes the inertia of Item at the location point of the system
    GProp::HOperator (Itemg, loc, Item.dim, HMat);
    ItemInertia = ItemInertia + HMat;
    inertia = inertia + ItemInertia * Density;
  }
}

Standard_Real GProp_GProps::Mass () const { return dim;  }

gp_Pnt GProp_GProps::CentreOfMass () const 
{ 
  return gp_Pnt(loc.XYZ() + g.XYZ()); 
}

gp_Mat GProp_GProps::MatrixOfInertia () const { 
  gp_Mat HMat;
  GProp::HOperator (g,gp::Origin(),dim, HMat);
  return inertia - HMat;
}



void GProp_GProps::StaticMoments (Standard_Real& Ix, 
				  Standard_Real& Iy, 
				  Standard_Real& Iz) const {

  gp_XYZ G = loc.XYZ() + g.XYZ();
  Ix = G.X() * dim;
  Iy = G.Y() * dim;
  Iz = G.Z() * dim;
}




Standard_Real GProp_GProps::MomentOfInertia (const gp_Ax1& A) const {
  // Moment of inertia / axis A
  // 1] computes the math_Matrix of inertia / A.location()
  // 2] applies this math_Matrix to A.Direction()
  // 3] then computes the scalar product between this vector and 
  //    A.Direction()

      
  if (loc.Distance (A.Location()) <= gp::Resolution()) {
    return (A.Direction().XYZ()).Dot (
           (A.Direction().XYZ()).Multiplied (inertia));
  }
  else {
    gp_Mat HMat;
    gp_Mat AxisInertia = MatrixOfInertia();
    GProp::HOperator (gp_Pnt (loc.XYZ() + g.XYZ()), A.Location(), dim, HMat);
    AxisInertia = AxisInertia + HMat;
    return (A.Direction().XYZ()).Dot (
           (A.Direction().XYZ()).Multiplied (AxisInertia));
  }
}



Standard_Real GProp_GProps::RadiusOfGyration (const gp_Ax1& A) const {

  return Sqrt (MomentOfInertia (A) / dim);
}



GProp_PrincipalProps GProp_GProps::PrincipalProperties () const {
  
  math_Matrix DiagMat (1, 3, 1, 3);
  Standard_Integer i, j;
  gp_Mat AxisInertia = MatrixOfInertia();
  for (j = 1; j <= 3; j++) {
    for (i = 1; i <= 3; i++) {
       DiagMat (i, j) = AxisInertia.Value (i, j);
    }
  }
  math_Jacobi J (DiagMat);
  Standard_Real Ixx = J.Value (1);
  Standard_Real Iyy = J.Value (2);
  Standard_Real Izz = J.Value (3);
  DiagMat  = J.Vectors ();      
  gp_Vec Vxx (DiagMat (1,1), DiagMat (2, 1), DiagMat (3, 1));
  gp_Vec Vyy (DiagMat (1,2), DiagMat (2, 2), DiagMat (3, 2));
  gp_Vec Vzz (DiagMat (1,3), DiagMat (2, 3), DiagMat (3, 3));
  //
  // protection contre dim == 0.0e0 au cas ou on aurait rentre qu'un point
  //
  Standard_Real Rxx = 0.0e0 ;
  Standard_Real Ryy = 0.0e0 ;
  Standard_Real Rzz = 0.0e0 ;  
  if (0.0e0 != dim) {
    Rxx = Sqrt (Abs(Ixx / dim));
    Ryy = Sqrt (Abs(Iyy / dim));
    Rzz = Sqrt (Abs(Izz / dim));
  }
  return GProp_PrincipalProps (Ixx, Iyy, Izz, Rxx, Ryy, Rzz, Vxx, Vyy, Vzz,
                             gp_Pnt(g.XYZ() + loc.XYZ()));
}


















