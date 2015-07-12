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

#ifndef OCCT_DEBUG
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#endif


#include <gp.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <IntAna_Int3Pln.hxx>
#include <math_Gauss.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <Standard_DomainError.hxx>
#include <StdFail_NotDone.hxx>

IntAna_Int3Pln::IntAna_Int3Pln () : done(Standard_False) {}


IntAna_Int3Pln::IntAna_Int3Pln (const gp_Pln& P1, const gp_Pln& P2,
				const gp_Pln& P3) {

  Perform(P1,P2,P3);
}

void IntAna_Int3Pln::Perform (const gp_Pln& P1, const gp_Pln& P2,
			      const gp_Pln& P3) {

  done=Standard_False;
  math_Matrix M(1,3,1,3);
  math_Vector V(1,3);
  
  P1.Coefficients(M(1,1),M(1,2),M(1,3),V(1));
  P2.Coefficients(M(2,1),M(2,2),M(2,3),V(2));
  P3.Coefficients(M(3,1),M(3,2),M(3,3),V(3));
  
  math_Gauss Resol(M,gp::Resolution());
  
  if (!Resol.IsDone()) {
    empt=Standard_True;
  }
  else {
    empt=Standard_False;
    V=-V;
    Resol.Solve(V);
    pnt.SetCoord(V(1),V(2),V(3));
  }
  done=Standard_True;
}



