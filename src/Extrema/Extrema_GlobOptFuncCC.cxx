// Created on: 2014-01-20
// Created by: Alexaner Malyshev
// Copyright (c) 2014-2014 OPEN CASCADE SAS
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
// commercial license or contractual agreement

#include <Extrema_GlobOptFuncCC.hxx>

#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <math_Vector.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OutOfRange.hxx>

static Standard_Integer _NbVariables()
{
  return 2;
}

// 3d _Value
static Standard_Boolean _Value(const Adaptor3d_Curve& C1,
                               const Adaptor3d_Curve& C2,
                               const math_Vector& X,
                               Standard_Real& F)
{
  Standard_Real u = X(1);
  Standard_Real v = X(2);

  if (u < C1.FirstParameter() ||
      u > C1.LastParameter()  ||
      v < C2.FirstParameter() ||
      v > C2.LastParameter())
  {
    return Standard_False;
  }

  F = C2.Value(v).Distance(C1.Value(u));
  return Standard_True;
}

// 2d _Value
static Standard_Boolean _Value(const Adaptor2d_Curve2d& C1,
                               const Adaptor2d_Curve2d& C2,
                               const math_Vector& X,
                               Standard_Real& F)
{
  Standard_Real u = X(1);
  Standard_Real v = X(2);

  if (u < C1.FirstParameter() ||
      u > C1.LastParameter()  ||
      v < C2.FirstParameter() ||
      v > C2.LastParameter())
  {
    return Standard_False;
  }

  F = C2.Value(v).Distance(C1.Value(u));
  return Standard_True;
}

//! F = (x2(v) - x1(u))^2 + (y2(v) - y1(u))^2 + (z2(v) - z1(u))^2

// 3d _Gradient
static Standard_Boolean _Gradient(const Adaptor3d_Curve& C1,
                                  const Adaptor3d_Curve& C2,
                                  const math_Vector& X,
                                  math_Vector& G)
{
  gp_Pnt C1D0, C2D0;
  gp_Vec C1D1, C2D1;

  if(X(1) < C1.FirstParameter() ||
     X(1) > C1.LastParameter()  ||
     X(2) < C2.FirstParameter() ||
     X(2) > C2.LastParameter())
  {
    return Standard_False;
  }

  C1.D1(X(1), C1D0, C1D1);
  C2.D1(X(2), C2D0, C2D1);

  G(1) = - (C2D0.X() - C1D0.X()) * C1D1.X() 
         - (C2D0.Y() - C1D0.Y()) * C1D1.Y() 
         - (C2D0.Z() - C1D0.Z()) * C1D1.Z();
  G(2) =   (C2D0.X() - C1D0.X()) * C2D1.X() 
         + (C2D0.Y() - C1D0.Y()) * C2D1.Y() 
         + (C2D0.Z() - C1D0.Z()) * C2D1.Z();
  return Standard_True;
}

// 2d _Graient
static Standard_Boolean _Gradient(const Adaptor2d_Curve2d& C1,
                                  const Adaptor2d_Curve2d& C2,
                                  const math_Vector& X,
                                  math_Vector& G)
{
  gp_Pnt2d C1D0, C2D0;
  gp_Vec2d C1D1, C2D1;

  if(X(1) < C1.FirstParameter() ||
     X(1) > C1.LastParameter()  ||
     X(2) < C2.FirstParameter() ||
     X(2) > C2.LastParameter())
  {
    return Standard_False;
  }

  C1.D1(X(1), C1D0, C1D1);
  C2.D1(X(2), C2D0, C2D1);

  G(1) = - (C2D0.X() - C1D0.X()) * C1D1.X() 
         - (C2D0.Y() - C1D0.Y()) * C1D1.Y();
  G(2) =   (C2D0.X() - C1D0.X()) * C2D1.X() 
         + (C2D0.Y() - C1D0.Y()) * C2D1.Y();
  return Standard_True;
}

// 3d _Hessian
static Standard_Boolean _Hessian (const Adaptor3d_Curve& C1,
                                  const Adaptor3d_Curve& C2,
                                  const math_Vector& X,
                                  math_Matrix & H)
{
  gp_Pnt C1D0, C2D0;
  gp_Vec C1D1, C2D1;
  gp_Vec C1D2, C2D2;

  if(X(1) < C1.FirstParameter() ||
     X(1) > C1.LastParameter()  ||
     X(2) < C2.FirstParameter() ||
     X(2) > C2.LastParameter())
  {
    return Standard_False;
  }

  C1.D2(X(1), C1D0, C1D1, C1D2);
  C2.D2(X(2), C2D0, C2D1, C2D2);

  H(1, 1) =   C1D1.X() * C1D1.X() 
            + C1D1.Y() * C1D1.Y() 
            + C1D1.Z() * C1D1.Z() 
            - (C2D0.X() - C1D0.X()) * C1D2.X() 
            - (C2D0.Y() - C1D0.Y()) * C1D2.Y() 
            - (C2D0.Z() - C1D0.Z()) * C1D2.Z();

  H(1, 2) = - C2D1.X() * C1D1.X()
            - C2D1.Y() * C1D1.Y()
            - C2D1.Z() * C1D1.Z();

  H(2,1) = H(1,2);

  H(2,2) =   C2D1.X() * C2D1.X() 
           + C2D1.Y() * C2D1.Y() 
           + C2D1.Z() * C2D1.Z() 
           + (C2D0.X() - C1D0.X()) * C2D2.X() 
           + (C2D0.Y() - C1D0.Y()) * C2D2.Y() 
           + (C2D0.Z() - C1D0.Z()) * C2D2.Z();
  return Standard_True;
}

// 2d _Hessian
static Standard_Boolean _Hessian (const Adaptor2d_Curve2d& C1,
                                  const Adaptor2d_Curve2d& C2,
                                  const math_Vector& X,
                                  math_Matrix & H)
{
  gp_Pnt2d C1D0, C2D0;
  gp_Vec2d C1D1, C2D1;
  gp_Vec2d C1D2, C2D2;

  if(X(1) < C1.FirstParameter() ||
     X(1) > C1.LastParameter()  ||
     X(2) < C2.FirstParameter() ||
     X(2) > C2.LastParameter())
  {
    return Standard_False;
  }

  C1.D2(X(1), C1D0, C1D1, C1D2);
  C2.D2(X(2), C2D0, C2D1, C2D2);

  H(1, 1) =   C1D1.X() * C1D1.X() 
            + C1D1.Y() * C1D1.Y() 
            - (C2D0.X() - C1D0.X()) * C1D2.X() 
            - (C2D0.Y() - C1D0.Y()) * C1D2.Y();

  H(1, 2) = - C2D1.X() * C1D1.X()
            - C2D1.Y() * C1D1.Y();

  H(2,1) = H(1,2);

  H(2,2) =   C2D1.X() * C2D1.X() 
           + C2D1.Y() * C2D1.Y() 
           + (C2D0.X() - C1D0.X()) * C2D2.X() 
           + (C2D0.Y() - C1D0.Y()) * C2D2.Y();
  return Standard_True;
}

// C0

//=======================================================================
//function : Extrema_GlobOptFuncCCC0
//purpose  : Constructor
//=======================================================================
Extrema_GlobOptFuncCCC0::Extrema_GlobOptFuncCCC0(const Adaptor3d_Curve& C1,
                                                 const Adaptor3d_Curve& C2)
: myC1_3d(&C1),
  myC2_3d(&C2)
{
  myType = 1;
}

//=======================================================================
//function : Extrema_GlobOptFuncCCC0
//purpose  : Constructor
//=======================================================================
Extrema_GlobOptFuncCCC0::Extrema_GlobOptFuncCCC0(const Adaptor2d_Curve2d& C1,
                                                 const Adaptor2d_Curve2d& C2)
: myC1_2d(&C1),
  myC2_2d(&C2)
{
  myType = 2;
}


//=======================================================================
//function : NbVariables
//purpose  :
//=======================================================================
Standard_Integer Extrema_GlobOptFuncCCC0::NbVariables() const
{
  return _NbVariables();
}

//=======================================================================
//function : Value
//purpose  :
//=======================================================================
Standard_Boolean Extrema_GlobOptFuncCCC0::Value(const math_Vector& X,Standard_Real& F)
{
  if (myType == 1)
    return _Value(*myC1_3d, *myC2_3d, X, F);
  else
    return _Value(*myC1_2d, *myC2_2d, X, F);
}

// C1

//=======================================================================
//function : Extrema_GlobOptFuncCCC1
//purpose  : Constructor
//=======================================================================
Extrema_GlobOptFuncCCC1::Extrema_GlobOptFuncCCC1(const Adaptor3d_Curve& C1,
                                                 const Adaptor3d_Curve& C2)
: myC1_3d(&C1),
  myC2_3d(&C2)
{
  myType = 1;
}

//=======================================================================
//function : Extrema_GlobOptFuncCCC1
//purpose  : Constructor
//=======================================================================
Extrema_GlobOptFuncCCC1::Extrema_GlobOptFuncCCC1(const Adaptor2d_Curve2d& C1,
                                                 const Adaptor2d_Curve2d& C2)
: myC1_2d(&C1),
  myC2_2d(&C2)
{
  myType = 2;
}

//=======================================================================
//function : NbVariables
//purpose  :
//=======================================================================
Standard_Integer Extrema_GlobOptFuncCCC1::NbVariables() const
{
  return _NbVariables();
}

//=======================================================================
//function : Value
//purpose  :
//=======================================================================
Standard_Boolean Extrema_GlobOptFuncCCC1::Value(const math_Vector& X,Standard_Real& F)
{
  if (myType == 1)
    return _Value(*myC1_3d, *myC2_3d, X, F);
  else
    return _Value(*myC1_2d, *myC2_2d, X, F);
}

//=======================================================================
//function : Gradient
//purpose  :
//=======================================================================
Standard_Boolean Extrema_GlobOptFuncCCC1::Gradient(const math_Vector& X,math_Vector& G)
{
  if (myType == 1)
    return _Gradient(*myC1_3d, *myC2_3d, X, G);
  else
    return _Gradient(*myC1_2d, *myC2_2d, X, G);
}

//=======================================================================
//function : Values
//purpose  :
//=======================================================================
Standard_Boolean Extrema_GlobOptFuncCCC1::Values(const math_Vector& X,Standard_Real& F,math_Vector& G)
{
  return (Value(X, F) && Gradient(X, G));
}

// C2

//=======================================================================
//function : Extrema_GlobOptFuncCCC2
//purpose  : Constructor
//=======================================================================
Extrema_GlobOptFuncCCC2::Extrema_GlobOptFuncCCC2(const Adaptor3d_Curve& C1,
                                                 const Adaptor3d_Curve& C2)
: myC1_3d(&C1),
  myC2_3d(&C2)
{
  myType = 1;
}

//=======================================================================
//function : Extrema_GlobOptFuncCCC2
//purpose  : Constructor
//=======================================================================
Extrema_GlobOptFuncCCC2::Extrema_GlobOptFuncCCC2(const Adaptor2d_Curve2d& C1,
                                                 const Adaptor2d_Curve2d& C2)
: myC1_2d(&C1),
  myC2_2d(&C2)
{
  myType = 2;
}

//=======================================================================
//function : NbVariables
//purpose  :
//=======================================================================
Standard_Integer Extrema_GlobOptFuncCCC2::NbVariables() const
{
  return _NbVariables();
}

//=======================================================================
//function : Value
//purpose  :
//=======================================================================
Standard_Boolean Extrema_GlobOptFuncCCC2::Value(const math_Vector& X,Standard_Real& F)
{
  if (myType == 1)
    return _Value(*myC1_3d, *myC2_3d, X, F);
  else
    return _Value(*myC1_2d, *myC2_2d, X, F);
}

//=======================================================================
//function : Gradient
//purpose  :
//=======================================================================
Standard_Boolean Extrema_GlobOptFuncCCC2::Gradient(const math_Vector& X,math_Vector& G)
{
  if (myType == 1)
    return _Gradient(*myC1_3d, *myC2_3d, X, G);
  else
    return _Gradient(*myC1_2d, *myC2_2d, X, G);
}

//=======================================================================
//function : Values
//purpose  :
//=======================================================================
Standard_Boolean Extrema_GlobOptFuncCCC2::Values(const math_Vector& X,Standard_Real& F,math_Vector& G)
{
  return  (Value(X, F) && Gradient(X, G));
}

//=======================================================================
//function : Values
//purpose  :
//=======================================================================
Standard_Boolean Extrema_GlobOptFuncCCC2::Values(const math_Vector& X,Standard_Real& F,math_Vector& G,math_Matrix& H)
{
  Standard_Boolean isHessianComputed = Standard_False;
  if (myType == 1)
    isHessianComputed = _Hessian(*myC1_3d, *myC2_3d, X, H);
  else
    isHessianComputed = _Hessian(*myC1_2d, *myC2_2d, X, H);


  return (Value(X, F) && Gradient(X, G) && isHessianComputed);
}
