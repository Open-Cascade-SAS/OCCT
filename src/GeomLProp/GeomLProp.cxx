// Created on: 1994-02-23
// Created by: Laurent BOURESCHE
// Copyright (c) 1994-1999 Matra Datavision
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


#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomLProp.hxx>
#include <GeomLProp_CLProps.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>

Standard_Integer GeomAbsToInteger(const GeomAbs_Shape  gcont) 
{
  Standard_Integer cont=0 ;
  switch (gcont) {
  case GeomAbs_C0 :
      cont = 0 ;
      break ;
  case GeomAbs_G1 :
      cont = 1 ;
      break ;
  case GeomAbs_C1 :
      cont = 2 ;
      break ;
  case GeomAbs_G2 :
      cont = 3 ;
      break ;
  case GeomAbs_C2 :
      cont = 4 ;
      break ;
  case GeomAbs_C3 :
      cont = 5 ;
      break ;
  case GeomAbs_CN :
      cont = 6 ;
      break ; 
  } 
 return cont ;
}
//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape GeomLProp::Continuity(const Handle(Geom_Curve)& C1, 
				    const Handle(Geom_Curve)& C2, 
				    const Standard_Real u1, 
				    const Standard_Real u2,
				    const Standard_Boolean r1, 
				    const Standard_Boolean r2,
				    const Standard_Real tl,
				    const Standard_Real ta)
{
  GeomAbs_Shape cont = GeomAbs_C0;
  Standard_Integer index1,
    index2 ;
  Standard_Real  tolerance ;
  Standard_Boolean fini = Standard_False;
  gp_Vec d1,d2;
  gp_Dir dir1,dir2;
  Standard_Integer cont1, cont2 ;
  GeomAbs_Shape gcont1 = C1->Continuity(), gcont2 = C2->Continuity();
  cont1 = GeomAbsToInteger(gcont1) ;
  cont2 = GeomAbsToInteger(gcont2) ;
      
  Handle(Geom_Curve) aCurve1 = C1 ;
  Handle(Geom_Curve) aCurve2 = C2 ;
  if (C1->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))){
    Handle(Geom_TrimmedCurve) aTrimmed =
      Handle(Geom_TrimmedCurve) ::DownCast(aCurve1) ;
    aCurve1 = aTrimmed->BasisCurve() ;
  }
  if (C2->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))){
    Handle(Geom_TrimmedCurve) aTrimmed =
      Handle(Geom_TrimmedCurve) ::DownCast(aCurve2) ;
    aCurve2 = aTrimmed->BasisCurve() ;
  }
  if (aCurve1->IsKind(STANDARD_TYPE(Geom_BSplineCurve))){
    Handle(Geom_BSplineCurve) BSplineCurve =
      Handle(Geom_BSplineCurve)::DownCast(aCurve1) ;
    BSplineCurve->Resolution(tl,
			 tolerance) ;
    BSplineCurve->LocateU(
		   u1,
		   tolerance,
		   index1,
		   index2) ;
    
    if (index1 > 1 && index2 < BSplineCurve->NbKnots() && index1 == index2) {
       cont1 = BSplineCurve->Degree() - BSplineCurve->Multiplicity(index1) ;
    }
    else {
      cont1 = 5 ;
    }
  }
  if (aCurve2->IsKind(STANDARD_TYPE(Geom_BSplineCurve))){
    Handle(Geom_BSplineCurve) BSplineCurve =
      Handle(Geom_BSplineCurve)::DownCast(aCurve2) ;
    BSplineCurve->Resolution(tl,
			 tolerance) ;
    BSplineCurve->LocateU(
		   u2,
		   tolerance,
		   index1,
		   index2) ;
    
    if (index1 > 1 && index2 < BSplineCurve->NbKnots() && index1 == index2) {
       cont2 = BSplineCurve->Degree() - BSplineCurve->Multiplicity(index1) ;
    }
    else {
      cont2 = 5 ;
    }
  }    
  Standard_Integer n1 = 0, n2 = 0;
  if (cont1 >= 5) n1 = 3;
  else if(cont1 == 4) n1 = 2;
  else if(cont1 == 2) n1 = 1;
  if (cont2 >= 5) n2 = 3;
  else if(cont2 == 4) n2 = 2;
  else if(cont2 == 2) n2 = 1;
  GeomLProp_CLProps clp1(C1,u1,n1,tl);
  GeomLProp_CLProps clp2(C2,u2,n2,tl);
  if(!(clp1.Value().IsEqual(clp2.Value(),tl))) {
    throw Standard_Failure("Courbes non jointives");
  }
  Standard_Integer min = Min(n1,n2);
  if ( min >= 1 ) {
    d1 = clp1.D1();
    d2 = clp2.D1();
    if(r1) d1.Reverse();
    if(r2) d2.Reverse();
    if(d1.IsEqual(d2,tl,ta)) { 
      cont = GeomAbs_C1; 
    }
    else if(clp1.IsTangentDefined() && clp2.IsTangentDefined()){
      clp1.Tangent(dir1);
      clp2.Tangent(dir2);
      if(r1) dir1.Reverse();
      if(r2) dir2.Reverse();
      if(dir1.IsEqual(dir2,ta)){ 
	cont = GeomAbs_G1; 
      }
      fini = Standard_True;
    }
    else {fini = Standard_True; }
  }
  if ( min >= 2 && !fini ) {
    d1 = clp1.D2();
    d2 = clp2.D2();
    if(d1.IsEqual(d2,tl,ta)){
      cont = GeomAbs_C2;
    }
  }
  return cont;
}


//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape GeomLProp::Continuity(const Handle(Geom_Curve)& C1, 
				    const Handle(Geom_Curve)& C2, 
				    const Standard_Real u1, 
				    const Standard_Real u2,
				    const Standard_Boolean r1, 
				    const Standard_Boolean r2)
{
  return Continuity(C1,C2,u1,u2,r1,r2,
		    Precision::Confusion(),Precision::Angular());
}
