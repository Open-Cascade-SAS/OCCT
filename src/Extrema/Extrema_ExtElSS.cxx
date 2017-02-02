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


#include <Extrema_ExtElSS.hxx>
#include <Extrema_ExtPElS.hxx>
#include <Extrema_POnSurf.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pln.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <Precision.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_InfiniteSolutions.hxx>
#include <StdFail_NotDone.hxx>

Extrema_ExtElSS::Extrema_ExtElSS()
{
  myDone = Standard_False;
  myIsPar = Standard_False;
}


Extrema_ExtElSS::Extrema_ExtElSS(const gp_Pln& S1, const gp_Pln& S2)
{
  Perform(S1, S2);
}


void Extrema_ExtElSS::Perform(const gp_Pln& S1, const gp_Pln& S2)
{

  myDone = Standard_True;
  myIsPar = Standard_False;
  myNbExt = 0;

  if ((S1.Axis().Direction()).IsParallel(S2.Axis().Direction(),
					 Precision::Angular())) {
    myIsPar = Standard_True;
    myNbExt = 1;
    mySqDist = new TColStd_HArray1OfReal(1, 1);
    mySqDist->SetValue(1, S1.SquareDistance(S2));
  }
}


Extrema_ExtElSS::Extrema_ExtElSS(const gp_Pln& S1, const gp_Sphere& S2)
{  Perform(S1, S2);}


//void Extrema_ExtElSS::Perform(const gp_Pln& S1, const gp_Sphere& S2)
void Extrema_ExtElSS::Perform(const gp_Pln& , const gp_Sphere& )
{

  myDone = Standard_True;
  myIsPar = Standard_False;
  myNbExt = 0;
  throw Standard_NotImplemented();
}

Extrema_ExtElSS::Extrema_ExtElSS(const gp_Sphere& S1, const gp_Sphere& S2)
{  Perform(S1, S2);}


//void Extrema_ExtElSS::Perform(const gp_Sphere& S1, const gp_Sphere& S2)
void Extrema_ExtElSS::Perform(const gp_Sphere& , const gp_Sphere& )
{
  myDone = Standard_True;
  myIsPar = Standard_False;
  myNbExt = 0;
  throw Standard_NotImplemented();
}


Extrema_ExtElSS::Extrema_ExtElSS(const gp_Sphere& S1, const gp_Cylinder& S2)
{  Perform(S1, S2);}


//void Extrema_ExtElSS::Perform(const gp_Sphere& S1, const gp_Cylinder& S2)
void Extrema_ExtElSS::Perform(const gp_Sphere& , const gp_Cylinder& )
{

  myDone = Standard_True;
  myIsPar = Standard_False;
  myNbExt = 0;
  throw Standard_NotImplemented();
}

Extrema_ExtElSS::Extrema_ExtElSS(const gp_Sphere& S1, const gp_Cone& S2)
{  Perform(S1, S2);}


//void Extrema_ExtElSS::Perform(const gp_Sphere& S1, const gp_Cone& S2)
void Extrema_ExtElSS::Perform(const gp_Sphere& , const gp_Cone& )
{

  myDone = Standard_True;
  myIsPar = Standard_False;
  myNbExt = 0;
  throw Standard_NotImplemented();
}


Extrema_ExtElSS::Extrema_ExtElSS(const gp_Sphere& S1, const gp_Torus& S2)
{  Perform(S1, S2);}


//void Extrema_ExtElSS::Perform(const gp_Sphere& S1, const gp_Torus& S2)
void Extrema_ExtElSS::Perform(const gp_Sphere& , const gp_Torus& )
{

  myDone = Standard_True;
  myIsPar = Standard_False;
  myNbExt = 0;
  throw Standard_NotImplemented();
}


Standard_Boolean Extrema_ExtElSS::IsDone() const
{
  return myDone;
}


Standard_Boolean Extrema_ExtElSS::IsParallel() const
{
  if(!myDone) throw StdFail_NotDone();
  return myIsPar;
}


Standard_Integer Extrema_ExtElSS::NbExt() const
{
  if(!myDone) throw StdFail_NotDone();
  if (myIsPar) throw StdFail_InfiniteSolutions();
  return myNbExt;
}


Standard_Real Extrema_ExtElSS::SquareDistance(const Standard_Integer N) const
{
  if(!myDone) throw StdFail_NotDone();
  if (myIsPar && N != 1) throw StdFail_InfiniteSolutions();
  return mySqDist->Value(N);
}


void Extrema_ExtElSS::Points(const Standard_Integer N,
			     Extrema_POnSurf&       P1,
			     Extrema_POnSurf&       P2) const
{
  if(!myDone) throw StdFail_NotDone();
  if (myIsPar) throw StdFail_InfiniteSolutions();
  P1 = myPOnS1->Value(N);
  P2 = myPOnS2->Value(N);
}


