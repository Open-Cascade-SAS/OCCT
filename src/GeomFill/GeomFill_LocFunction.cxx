// Created on: 1998-02-02
// Created by: Philippe MANGIN
// Copyright (c) 1998-1999 Matra Datavision
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


#include <GeomFill_LocationLaw.hxx>
#include <GeomFill_LocFunction.hxx>
#include <gp_Mat.hxx>
#include <gp_Vec.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfVec2d.hxx>

GeomFill_LocFunction::GeomFill_LocFunction(const Handle(GeomFill_LocationLaw)& Law) 
                                          :V(1,4), DV(1,4), D2V(1,4)

{
  myLaw = Law;
}

 Standard_Boolean GeomFill_LocFunction::D0(const Standard_Real Param,
//					   const Standard_Real First,
					   const Standard_Real ,
//					   const Standard_Real Last) 
					   const Standard_Real ) 
{
  gp_Mat aM;
  Standard_Boolean B;
  B = myLaw->D0(Param, aM, V.ChangeValue(1));
  V(2).SetXYZ(aM.Column(1));
  V(3).SetXYZ(aM.Column(2));
  V(4).SetXYZ(aM.Column(3));
  return B;
}

 Standard_Boolean GeomFill_LocFunction::D1(const Standard_Real Param,
//					   const Standard_Real First,
					   const Standard_Real ,
//					   const Standard_Real Last) 
					   const Standard_Real ) 
{
  TColgp_Array1OfPnt2d T1(1,1);
  TColgp_Array1OfVec2d T2(1,1);
  gp_Mat aM, aDM;
  Standard_Boolean B;
  B = myLaw->D1(Param, aM, V.ChangeValue(1),
		aDM, DV.ChangeValue(1),
		T1, T2);

  V(2).SetXYZ(aM.Column(1));
  V(3).SetXYZ(aM.Column(2));
  V(4).SetXYZ(aM.Column(3));

  DV(2).SetXYZ(aDM.Column(1));
  DV(3).SetXYZ(aDM.Column(2));
  DV(4).SetXYZ(aDM.Column(3));
  return B;
}

 Standard_Boolean GeomFill_LocFunction::D2(const Standard_Real Param,
//					   const Standard_Real First,
					   const Standard_Real ,
//					   const Standard_Real Last) 
					   const Standard_Real ) 
{
  TColgp_Array1OfPnt2d T1(1,1);
  TColgp_Array1OfVec2d T2(1,1), T3(1,1);
  gp_Mat aM, aDM, aD2M;
  Standard_Boolean B;
  B = myLaw->D2(Param, aM, V.ChangeValue(1),
		aDM, DV.ChangeValue(1),
		aD2M, D2V.ChangeValue(1),
		T1, T2, T3);
  V(2).SetXYZ(aM.Column(1));
  V(3).SetXYZ(aM.Column(2));
  V(4).SetXYZ(aM.Column(3));

  DV(2).SetXYZ(aDM.Column(1));
  DV(3).SetXYZ(aDM.Column(2));
  DV(4).SetXYZ(aDM.Column(3));


  D2V(2).SetXYZ(aD2M.Column(1));
  D2V(3).SetXYZ(aD2M.Column(2));
  D2V(4).SetXYZ(aD2M.Column(3));

  return B;  
}

 void GeomFill_LocFunction::DN(const Standard_Real Param,
					   const Standard_Real First,
					   const Standard_Real Last,
					   const Standard_Integer Order,
					   Standard_Real& Result,
					   Standard_Integer& Ier) 
{
  Standard_Boolean B;
  Standard_Real * AddrResult =  &Result;
  const Standard_Real * LocalResult=NULL;
    
  Ier = 0;
  switch (Order) {
  case 0:
    {
      B = D0(Param, First, Last);
      LocalResult = (Standard_Real*)(&V(1));
      break;
    }
  case 1:
    {
      B = D1(Param, First, Last);
      LocalResult = (Standard_Real*)(&DV(1));
      break;
    }
  case 2:
    {
      B = D2(Param, First, Last);
      LocalResult = (Standard_Real*)(&D2V(1));
      break;
    }
    default :
      {
	B = Standard_False;
      }
  }
  if (!B) {
    Ier = Order+1;
  }
  for (Standard_Integer ii=0; ii<=11; ii++) {
    AddrResult[ii] =  LocalResult[ii]; 
  }
}
