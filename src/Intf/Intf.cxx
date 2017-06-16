// Created on: 1993-01-20
// Created by: Didier PIFFAULT
// Copyright (c) 1993-1999 Matra Datavision
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
#include <gp_XYZ.hxx>
#include <Intf.hxx>

//=======================================================================
//function : PlaneEquation
//purpose  : 
//=======================================================================
void Intf::PlaneEquation (const gp_Pnt&  P1,
			  const gp_Pnt&  P2,
			  const gp_Pnt&  P3,
			  gp_XYZ&        NormalVector,
			  Standard_Real& PolarDistance)
{
  gp_XYZ v1=P2.XYZ()-P1.XYZ();
  gp_XYZ v2=P3.XYZ()-P2.XYZ();
  gp_XYZ v3=P1.XYZ()-P3.XYZ();
  NormalVector= (v1^v2)+(v2^v3)+(v3^v1);
  Standard_Real aNormLen = NormalVector.Modulus();
  if (aNormLen < gp::Resolution()) {
    PolarDistance = 0.;
  }
  else {
    NormalVector.Divide(aNormLen);
    PolarDistance = NormalVector * P1.XYZ();
  }
}


//=======================================================================
//function : Contain
//purpose  : 
//=======================================================================

Standard_Boolean Intf::Contain (const gp_Pnt&  P1,
				const gp_Pnt&  P2,
				const gp_Pnt&  P3,
				const gp_Pnt& ThePnt)
{
  gp_XYZ v1=(P2.XYZ()-P1.XYZ())^(ThePnt.XYZ()-P1.XYZ());
  gp_XYZ v2=(P3.XYZ()-P2.XYZ())^(ThePnt.XYZ()-P2.XYZ());
  gp_XYZ v3=(P1.XYZ()-P3.XYZ())^(ThePnt.XYZ()-P3.XYZ());
  if (v1*v2 >= 0. && v2*v3 >= 0. && v3*v1>=0.) return Standard_True;
  else                                         return Standard_False;
}
