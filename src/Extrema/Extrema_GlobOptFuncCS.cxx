// Created on: 2014-06-23
// Created by: Alexander Malyshev
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

#include <Extrema_GlobOptFuncCS.hxx>

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <math_Vector.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OutOfRange.hxx>

//!F(cu, su, sv)=(C^{(x)}(cu)-S^{(x)}(su,sv))^{2}+
//               (C^{(y)}(cu)-S^{(y)}(su,sv))^{2}+
//               (C^{(z)}(cu)-S^{(z)}(su,sv))^{2}


//=======================================================================
//function : value
//purpose  : 
//=======================================================================
void Extrema_GlobOptFuncCS::value(Standard_Real cu,
                                  Standard_Real su,
                                  Standard_Real sv,
                                  Standard_Real &F)
{
  F = myC->Value(cu).SquareDistance(myS->Value(su, sv));
}

//=======================================================================
//function : gradient
//purpose  : 
//=======================================================================
void Extrema_GlobOptFuncCS::gradient(Standard_Real cu,
                                     Standard_Real su,
                                     Standard_Real sv,
                                     math_Vector &G)
{
  gp_Pnt CD0, SD0;
  gp_Vec CD1, SD1U, SD1V;

  myC->D1(cu, CD0, CD1);
  myS->D1(su, sv, SD0, SD1U, SD1V);

  G(1) = + (CD0.X() - SD0.X()) * CD1.X()
         + (CD0.Y() - SD0.Y()) * CD1.Y()
         + (CD0.Z() - SD0.Z()) * CD1.Z();
  G(2) = - (CD0.X() - SD0.X()) * SD1U.X()
         - (CD0.Y() - SD0.Y()) * SD1U.Y()
         - (CD0.Z() - SD0.Z()) * SD1U.Z();
  G(3) = - (CD0.X() - SD0.X()) * SD1V.X()
         - (CD0.Y() - SD0.Y()) * SD1V.Y()
         - (CD0.Z() - SD0.Z()) * SD1V.Z();
}

//=======================================================================
//function : hessian
//purpose  : 
//=======================================================================
void Extrema_GlobOptFuncCS::hessian(Standard_Real cu,
                                    Standard_Real su,
                                    Standard_Real sv,
                                    math_Matrix &H)
{
  gp_Pnt CD0, SD0;
  gp_Vec CD1, SD1U, SD1V, CD2, SD2UU, SD2UV, SD2VV;

  myC->D2(cu, CD0, CD1, CD2);
  myS->D2(su, sv, SD0, SD1U, SD1V, SD2UU, SD2VV, SD2UV);

  H(1,1) = + CD1.X() * CD1.X()
           + CD1.Y() * CD1.Y()
           + CD1.Z() * CD1.Z()
           + (CD0.X() - SD0.X()) * CD2.X()
           + (CD0.Y() - SD0.Y()) * CD2.Y()
           + (CD0.Z() - SD0.Z()) * CD2.Z();

  H(1,2) = - CD1.X() * SD1U.X()
           - CD1.Y() * SD1U.Y()
           - CD1.Z() * SD1U.Z();

  H(1,3) = - CD1.X() * SD1V.X()
           - CD1.Y() * SD1V.Y()
           - CD1.Z() * SD1V.Z();

  H(2,1) = H(1,2);

  H(2,2) = + SD1U.X() * SD1U.X()
           + SD1U.Y() * SD1U.Y()
           + SD1U.Z() * SD1U.Z()
           - (CD0.X() - SD0.X()) * SD2UU.X()
           - (CD0.Y() - SD0.Y()) * SD2UU.Y()
           - (CD0.Z() - SD0.Z()) * SD2UU.Z();

  H(2,3) = + SD1U.X() * SD1V.X()
           + SD1U.Y() * SD1V.Y()
           + SD1U.Z() * SD1V.Z()
           - (CD0.X() - SD0.X()) * SD2UV.X()
           - (CD0.Y() - SD0.Y()) * SD2UV.Y()
           - (CD0.Z() - SD0.Z()) * SD2UV.Z();

  H(3,1) = H(1,3);

  H(3,2) = H(2,3);

  H(3,3) = + SD1V.X() * SD1V.X()
           + SD1V.Y() * SD1V.Y()
           + SD1V.Z() * SD1V.Z()
           - (CD0.X() - SD0.X()) * SD2VV.X()
           - (CD0.Y() - SD0.Y()) * SD2VV.Y()
           - (CD0.Z() - SD0.Z()) * SD2VV.Z();
}

//=======================================================================
//function : checkInputData
//purpose  : 
//=======================================================================
Standard_Boolean Extrema_GlobOptFuncCS::checkInputData(const math_Vector   &X,
                                                       Standard_Real       &cu,
                                                       Standard_Real       &su,
                                                       Standard_Real       &sv)
{
  Standard_Integer aStartIndex = X.Lower();
  cu = X(aStartIndex);
  su = X(aStartIndex + 1);
  sv = X(aStartIndex + 2);

  if (cu < myC->FirstParameter()  ||
      cu > myC->LastParameter()   ||
      su < myS->FirstUParameter() ||
      su > myS->LastUParameter()  ||
      sv < myS->FirstVParameter() ||
      sv > myS->LastVParameter())
  {
    return Standard_False;
  }
  return Standard_True;
}

//=======================================================================
//function : Extrema_GlobOptFuncCS
//purpose  : Constructor
//=======================================================================
Extrema_GlobOptFuncCS::Extrema_GlobOptFuncCS(const Adaptor3d_Curve   *C,
                                             const Adaptor3d_Surface *S)
: myC(C),
  myS(S)
{
}

//=======================================================================
//function : NbVariables
//purpose  :
//=======================================================================
Standard_Integer Extrema_GlobOptFuncCS::NbVariables() const
{
  return 3;
}

//=======================================================================
//function : Value
//purpose  :
//=======================================================================
Standard_Boolean Extrema_GlobOptFuncCS::Value(const math_Vector &X,
                                              Standard_Real     &F)
{
  Standard_Real cu, su, sv;
  if (!checkInputData(X, cu, su, sv))
    return Standard_False;

  value(cu, su, sv, F);
  return Standard_True;
}

//=======================================================================
//function : Gradient
//purpose  :
//=======================================================================
Standard_Boolean Extrema_GlobOptFuncCS::Gradient(const math_Vector &X,
                                                 math_Vector       &G)
{
  Standard_Real cu, su, sv;
  if (!checkInputData(X, cu, su, sv))
    return Standard_False;

  gradient(cu, su, sv, G);
  return Standard_True;
}

//=======================================================================
//function : Values
//purpose  :
//=======================================================================
Standard_Boolean Extrema_GlobOptFuncCS::Values(const math_Vector &X,
                                               Standard_Real     &F,
                                               math_Vector       &G)
{
  Standard_Real cu, su, sv;
  if (!checkInputData(X, cu, su, sv))
    return Standard_False;

  value(cu, su, sv, F);
  gradient(cu, su, sv, G);
  return Standard_True;
}

//=======================================================================
//function : Values
//purpose  :
//=======================================================================
Standard_Boolean Extrema_GlobOptFuncCS::Values(const math_Vector &X,
                                               Standard_Real     &F,
                                               math_Vector       &G,
                                               math_Matrix       &H)
{
  Standard_Real cu, su, sv;
  if (!checkInputData(X, cu, su, sv))
    return Standard_False;

  value(cu, su, sv, F);
  gradient(cu, su, sv, G);
  hessian(cu, su, sv, H);
  return Standard_True;
}