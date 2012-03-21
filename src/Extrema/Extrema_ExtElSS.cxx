// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <Extrema_ExtElSS.ixx>
#include <Extrema_ExtPElS.hxx>
#include <Standard_NotImplemented.hxx>
#include <StdFail_NotDone.hxx>
#include <StdFail_InfiniteSolutions.hxx>
#include <Precision.hxx>


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
{
  Perform(S1, S2);
}


//void Extrema_ExtElSS::Perform(const gp_Pln& S1, const gp_Sphere& S2)
void Extrema_ExtElSS::Perform(const gp_Pln& , const gp_Sphere& )
{

  myDone = Standard_True;
  myIsPar = Standard_False;
  myNbExt = 0;
  Standard_NotImplemented::Raise();
}

Extrema_ExtElSS::Extrema_ExtElSS(const gp_Sphere& S1, const gp_Sphere& S2)
{
  Perform(S1, S2);
}


//void Extrema_ExtElSS::Perform(const gp_Sphere& S1, const gp_Sphere& S2)
void Extrema_ExtElSS::Perform(const gp_Sphere& , const gp_Sphere& )
{

  myDone = Standard_True;
  myIsPar = Standard_False;
  myNbExt = 0;
  Standard_NotImplemented::Raise();
}


Extrema_ExtElSS::Extrema_ExtElSS(const gp_Sphere& S1, const gp_Cylinder& S2)
{
  Perform(S1, S2);
}


//void Extrema_ExtElSS::Perform(const gp_Sphere& S1, const gp_Cylinder& S2)
void Extrema_ExtElSS::Perform(const gp_Sphere& , const gp_Cylinder& )
{

  myDone = Standard_True;
  myIsPar = Standard_False;
  myNbExt = 0;
  Standard_NotImplemented::Raise();
}

Extrema_ExtElSS::Extrema_ExtElSS(const gp_Sphere& S1, const gp_Cone& S2)
{
  Perform(S1, S2);
}


//void Extrema_ExtElSS::Perform(const gp_Sphere& S1, const gp_Cone& S2)
void Extrema_ExtElSS::Perform(const gp_Sphere& , const gp_Cone& )
{

  myDone = Standard_True;
  myIsPar = Standard_False;
  myNbExt = 0;
  Standard_NotImplemented::Raise();
}


Extrema_ExtElSS::Extrema_ExtElSS(const gp_Sphere& S1, const gp_Torus& S2)
{
  Perform(S1, S2);
}


//void Extrema_ExtElSS::Perform(const gp_Sphere& S1, const gp_Torus& S2)
void Extrema_ExtElSS::Perform(const gp_Sphere& , const gp_Torus& )
{

  myDone = Standard_True;
  myIsPar = Standard_False;
  myNbExt = 0;
  Standard_NotImplemented::Raise();
}


Standard_Boolean Extrema_ExtElSS::IsDone() const
{
  return myDone;
}


Standard_Boolean Extrema_ExtElSS::IsParallel() const
{
  if(!myDone) StdFail_NotDone::Raise();
  return myIsPar;
}


Standard_Integer Extrema_ExtElSS::NbExt() const
{
  if(!myDone) StdFail_NotDone::Raise();
  if (myIsPar) StdFail_InfiniteSolutions::Raise();
  return myNbExt;
}


Standard_Real Extrema_ExtElSS::SquareDistance(const Standard_Integer N) const
{
  if(!myDone) StdFail_NotDone::Raise();
  if (myIsPar && N != 1) StdFail_InfiniteSolutions::Raise();
  return mySqDist->Value(N);
}


void Extrema_ExtElSS::Points(const Standard_Integer N,
			     Extrema_POnSurf&       P1,
			     Extrema_POnSurf&       P2) const
{
  if(!myDone) StdFail_NotDone::Raise();
  if (myIsPar) StdFail_InfiniteSolutions::Raise();
  P1 = myPOnS1->Value(N);
  P2 = myPOnS2->Value(N);
}


