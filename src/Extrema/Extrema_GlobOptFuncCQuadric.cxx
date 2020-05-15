// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <Extrema_GlobOptFuncCQuadric.hxx>

#include <gp_Pnt.hxx>
#include <ElSLib.hxx>
#include <ElCLib.hxx>


//=======================================================================
//function : value
//purpose  : 
//=======================================================================
void Extrema_GlobOptFuncCQuadric::value(Standard_Real ct,
                                        Standard_Real &F)
{
  Standard_Real u, v;
  //
  gp_Pnt aCP = myC->Value(ct);
  switch (mySType)
  {
  case GeomAbs_Plane:
    ElSLib::Parameters(myPln, aCP, u, v);
    break;
  case GeomAbs_Cylinder:
    ElSLib::Parameters(myCylinder, aCP, u, v);
    break;
  case GeomAbs_Cone:
    ElSLib::Parameters(myCone, aCP, u, v);
    break;
  case GeomAbs_Sphere:
    ElSLib::Parameters(mySphere, aCP, u, v);
    break;
  case GeomAbs_Torus:
    ElSLib::Parameters(myTorus, aCP, u, v);
    break;
  default:
    F = Precision::Infinite();
    return;
  }
  //
  if (mySType != GeomAbs_Plane)
  {
    if (myUl > 2. * M_PI + Precision::PConfusion())
    {
      u += 2. * M_PI;
    }
  }
  if (mySType == GeomAbs_Torus)
  {
    if (myVl > 2. * M_PI + Precision::PConfusion())
    {
      v += 2. * M_PI;
    }
  }

  F = RealLast();
  if (u >= myUf && u <= myUl && v >= myVf && v <= myVl)
  {
    gp_Pnt aPS = myS->Value(u, v);
    F = Min(F, aCP.SquareDistance(aPS));
  }
  Standard_Integer i;
  for (i = 0; i < 4; ++i)
  {
    F = Min(F, aCP.SquareDistance(myPTrim[i]));
  }
}


//=======================================================================
//function : checkInputData
//purpose  : 
//=======================================================================
Standard_Boolean Extrema_GlobOptFuncCQuadric::checkInputData(const math_Vector   &X,
                                                             Standard_Real       &ct) 
{
  ct = X(X.Lower());

  if (ct < myTf || ct > myTl )
  {
    return Standard_False;
  }
  return Standard_True;
}

//=======================================================================
//function : Extrema_GlobOptFuncCQuadric
//purpose  : Constructor
//=======================================================================
Extrema_GlobOptFuncCQuadric::Extrema_GlobOptFuncCQuadric(const Adaptor3d_Curve   *C,
                                                     const Adaptor3d_Surface *S)
: myC(C)
{
  myTf = myC->FirstParameter();
  myTl = myC->LastParameter();
  Standard_Real anUf = S->FirstUParameter(), anUl = S->LastUParameter();
  Standard_Real aVf = S->FirstVParameter(), aVl = S->LastVParameter();
  LoadQuad(S, anUf, anUl, aVf, aVl);
}
//=======================================================================
//function : Extrema_GlobOptFuncCQuadric
//purpose  : Constructor
//=======================================================================
Extrema_GlobOptFuncCQuadric::Extrema_GlobOptFuncCQuadric(const Adaptor3d_Curve *C)
  : myC(C)
{
  myTf = myC->FirstParameter();
  myTl = myC->LastParameter();
}
//=======================================================================
//function : Extrema_GlobOptFuncCQuadric
//purpose  : Constructor
//=======================================================================
Extrema_GlobOptFuncCQuadric::Extrema_GlobOptFuncCQuadric(const Adaptor3d_Curve *C,
  const Standard_Real theTf, const Standard_Real theTl)
  : myC(C), myTf(theTf), myTl(theTl)
{
}

//=======================================================================
//function : LoadQuad
//purpose  : 
//=======================================================================
void Extrema_GlobOptFuncCQuadric::LoadQuad( const Adaptor3d_Surface *S, 
  const Standard_Real theUf, const Standard_Real theUl,
  const Standard_Real theVf, const Standard_Real theVl)
{
  myS = S;
  myUf = theUf;
  myUl = theUl;
  myVf = theVf;
  myVl = theVl;
  //
  if (myS->IsUPeriodic())
  {
    Standard_Real aTMax = 2. * M_PI + Precision::PConfusion();
    if (myUf > aTMax || myUf < -Precision::PConfusion() ||
      Abs(myUl - myUf) > aTMax)
    {
      ElCLib::AdjustPeriodic(0., 2. * M_PI,
        Min(Abs(myUl - myUf) / 2, Precision::PConfusion()),
        myUf, myUl);
    }
  }
  if (myS->IsVPeriodic())
  {
    Standard_Real aTMax = 2. * M_PI + Precision::PConfusion();
    if (myVf > aTMax || myVf < -Precision::PConfusion() ||
      Abs(myVl - myVf) > aTMax)
    {
      ElCLib::AdjustPeriodic(0., 2. * M_PI,
        Min(Abs(myVl - myVf) / 2, Precision::PConfusion()),
        myVf, myVl);
    }
  }
  myPTrim[0] = myS->Value(myUf, myVf);
  myPTrim[1] = myS->Value(myUl, myVf);
  myPTrim[2] = myS->Value(myUl, myVl);
  myPTrim[3] = myS->Value(myUf, myVl);
  mySType = S->GetType();
  switch (mySType)
  {
  case GeomAbs_Plane:
    myPln = myS->Plane();
    break;
  case GeomAbs_Cylinder:
     myCylinder = myS->Cylinder();
    break;
  case GeomAbs_Cone:
    myCone = myS->Cone();
    break;
  case GeomAbs_Sphere:
    mySphere = myS->Sphere();
    break;
  case GeomAbs_Torus:
    myTorus = myS->Torus();
    break;
  default:
    break;
  }

}

//=======================================================================
//function : NbVariables
//purpose  :
//=======================================================================
Standard_Integer Extrema_GlobOptFuncCQuadric::NbVariables() const
{
  return 1;
}

//=======================================================================
//function : Value
//purpose  :
//=======================================================================
Standard_Boolean Extrema_GlobOptFuncCQuadric::Value(const math_Vector &X,
                                                    Standard_Real     &F)
{
  Standard_Real ct;
  if (!checkInputData(X, ct))
    return Standard_False;

  value(ct, F);
  if (Precision::IsInfinite(F))
  {
    return Standard_False;
  }
  return Standard_True;
}

//=======================================================================
//function : QuadricParameters
//purpose  :
//=======================================================================
void Extrema_GlobOptFuncCQuadric::QuadricParameters(const math_Vector& theCT,
  math_Vector& theUV ) const
{
  Standard_Real u, v;
  //
  //Arrays of extremity points parameters correspond to array of corner
  //points  myPTrim[] 
  Standard_Real uext[4] = { myUf, myUl, myUl, myUf };
  Standard_Real vext[4] = { myVf, myVf, myVl, myVl };
  gp_Pnt aCP = myC->Value(theCT(1));
  switch (mySType)
  {
  case GeomAbs_Plane:
    ElSLib::Parameters(myPln, aCP, u, v);
    break;
  case GeomAbs_Cylinder:
    ElSLib::Parameters(myCylinder, aCP, u, v);
    break;
  case GeomAbs_Cone:
    ElSLib::Parameters(myCone, aCP, u, v);
    break;
  case GeomAbs_Sphere:
    ElSLib::Parameters(mySphere, aCP, u, v);
    break;
  case GeomAbs_Torus:
    ElSLib::Parameters(myTorus, aCP, u, v);
    break;
  default:
    theUV(1) = myUf;
    theUV(2) = myUl;
    return;
  }
  //
  if (mySType != GeomAbs_Plane)
  {
    if (myUl > 2. * M_PI + Precision::PConfusion())
    {
      u += 2. * M_PI;
    }
  }
  if (mySType == GeomAbs_Torus)
  {
    if (myVl > 2. * M_PI + Precision::PConfusion())
    {
      v += 2. * M_PI;
    }
  }

  Standard_Real F = RealLast();
  if (u >= myUf && u <= myUl && v >= myVf && v <= myVl)
  {
    gp_Pnt aPS = myS->Value(u, v);
    F = aCP.SquareDistance(aPS);
  }
  Standard_Integer i;
  for (i = 0; i < 4; ++i)
  {
    Standard_Real Fi = aCP.SquareDistance(myPTrim[i]);
    if (Fi < F)
    {
      F = Fi;
      u = uext[i];
      v = vext[i];
    }
  }
  theUV(1) = u;
  theUV(2) = v;
}

