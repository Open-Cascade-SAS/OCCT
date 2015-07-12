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


#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_Conic.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

void IntAna2d_AnaIntersection::Perform (const gp_Lin2d& L1,
					const gp_Lin2d& L2) {

 
  done = Standard_False;

  Standard_Real A1,B1,C1;
  Standard_Real A2,B2,C2;
  L1.Coefficients(A1,B1,C1);
  L2.Coefficients(A2,B2,C2);

  Standard_Real al1,be1,ga1;
  Standard_Real al2,be2,ga2;
  
  Standard_Real Det =Max (Abs(A1),Max(Abs(A2),Max(Abs(B1),Abs(B2))));

  if (Abs(A1)==Det) {
    al1=A1;
    be1=B1;
    ga1=C1;
    al2=A2;
    be2=B2;
    ga2=C2;
  }
  else if (Abs(B1)==Det) {
    al1=B1;
    be1=A1;
    ga1=C1;
    al2=B2;
    be2=A2;
    ga2=C2;
  }
  else if (Abs(A2)==Det) {
    al1=A2;
    be1=B2;
    ga1=C2;
    al2=A1;
    be2=B1;
    ga2=C1;
  }
  else {
    al1=B2;
    be1=A2;
    ga1=C2;
    al2=B1;
    be2=A1;
    ga2=C1;
  }

  Standard_Real rap=al2/al1;
  Standard_Real denom=be2-rap*be1;

  if (Abs(denom)<=RealEpsilon()) {                // Directions confondues
    para=Standard_True;
    nbp=0;
    if (Abs(ga2-rap*ga1)<=RealEpsilon()) {          // Droites confondues
      iden=Standard_True;
      empt=Standard_False;
    }
    else {                                       // Droites paralleles
      iden=Standard_False;
      empt=Standard_True;
    }
  }
  else {
    para=Standard_False;
    iden=Standard_False;
    empt=Standard_False;
    nbp=1;
    Standard_Real XS = (be1*ga2/al1-be2*ga1/al1)/denom;
    Standard_Real YS = (rap*ga1-ga2)/denom;

    if (((Abs(A1)!=Det)&&(Abs(B1)==Det))||
	((Abs(A1)!=Det)&&(Abs(B1)!=Det)&&(Abs(A2)!=Det))) {
      Standard_Real temp=XS;
      XS=YS;
      YS=temp;
    }

    Standard_Real La,Mu;
    if (Abs(A1)>=Abs(B1)) {
      La=(YS-L1.Location().Y())/A1;
    }
    else {
      La=(L1.Location().X()-XS)/B1;
    }
    if (Abs(A2)>=Abs(B2)) {
      Mu=(YS-L2.Location().Y())/A2;
    }
    else {
      Mu=(L2.Location().X()-XS)/B2;
    }
    lpnt[0].SetValue(XS,YS,La,Mu);
  }
  done=Standard_True;
}


