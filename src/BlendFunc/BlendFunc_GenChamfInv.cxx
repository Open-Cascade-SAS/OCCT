// Created by: Julia GERASIMOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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


#include <Adaptor2d_Curve2d.hxx>
#include <BlendFunc.hxx>
#include <BlendFunc_ChamfInv.hxx>
#include <math_Matrix.hxx>
#include <Precision.hxx>


//=======================================================================
//function : BlendFunc_GenChamfInv
//purpose  : 
//=======================================================================
BlendFunc_GenChamfInv::BlendFunc_GenChamfInv(const Handle(Adaptor3d_Surface)& S1,
                                             const Handle(Adaptor3d_Surface)& S2,
                                             const Handle(Adaptor3d_Curve)&   C)
: surf1(S1),
  surf2(S2),
  curv(C),
  choix(0),
  first(Standard_False)
{
}

//=======================================================================
//function : NbEquations
//purpose  : 
//=======================================================================

Standard_Integer BlendFunc_GenChamfInv::NbEquations () const
{
  return 4;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BlendFunc_GenChamfInv::Set(const Standard_Boolean OnFirst, const Handle(Adaptor2d_Curve2d)& C)
{
  first = OnFirst;
  csurf = C;
}

//=======================================================================
//function : GetTolerance
//purpose  : 
//=======================================================================

void BlendFunc_GenChamfInv::GetTolerance(math_Vector& Tolerance, const Standard_Real Tol) const
{
  Tolerance(1) = csurf->Resolution(Tol);
  Tolerance(2) = curv->Resolution(Tol);
  if (first) {
    Tolerance(3) = surf2->UResolution(Tol);
    Tolerance(4) = surf2->VResolution(Tol);
  }
  else {
    Tolerance(3) = surf1->UResolution(Tol);
    Tolerance(4) = surf1->VResolution(Tol);
  }
}


//=======================================================================
//function : GetBounds
//purpose  : 
//=======================================================================

void BlendFunc_GenChamfInv::GetBounds(math_Vector& InfBound, math_Vector& SupBound) const
{
  InfBound(1) = csurf->FirstParameter();
  InfBound(2) = curv->FirstParameter();
  SupBound(1) = csurf->LastParameter();
  SupBound(2) = curv->LastParameter();

  if (first) {
    InfBound(3) = surf2->FirstUParameter();
    InfBound(4) = surf2->FirstVParameter();
    SupBound(3) = surf2->LastUParameter();
    SupBound(4) = surf2->LastVParameter();
    if(!Precision::IsInfinite(InfBound(3)) &&
       !Precision::IsInfinite(SupBound(3))) {
      const Standard_Real range = (SupBound(3) - InfBound(3));
      InfBound(3) -= range;
      SupBound(3) += range;
    }
    if(!Precision::IsInfinite(InfBound(4)) &&
       !Precision::IsInfinite(SupBound(4))) {
      const Standard_Real range = (SupBound(4) - InfBound(4));
      InfBound(4) -= range;
      SupBound(4) += range;
    }
  }
  else {
    InfBound(3) = surf1->FirstUParameter();
    InfBound(4) = surf1->FirstVParameter();
    SupBound(3) = surf1->LastUParameter();
    SupBound(4) = surf1->LastVParameter();
    if(!Precision::IsInfinite(InfBound(3)) &&
       !Precision::IsInfinite(SupBound(3))) {
      const Standard_Real range = (SupBound(3) - InfBound(3));
      InfBound(3) -= range;
      SupBound(3) += range;
    }
    if(!Precision::IsInfinite(InfBound(4)) &&
       !Precision::IsInfinite(SupBound(4))) {
      const Standard_Real range = (SupBound(4) - InfBound(4));
      InfBound(4) -= range;
      SupBound(4) += range;
    }
  }    
}

//=======================================================================
//function : Values
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_GenChamfInv::Values(const math_Vector& X, math_Vector& F, math_Matrix& D)
{
  Value(X,F);
  Derivatives(X,D);
  return Standard_True;
}
