// Created on: 1993-07-06
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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



#include <BRep_CurveOnClosedSurface.ixx>
#include <Precision.hxx>



//=======================================================================
//function : BRep_CurveOnClosedSurface
//purpose  : 
//=======================================================================

BRep_CurveOnClosedSurface::BRep_CurveOnClosedSurface
  (const Handle(Geom2d_Curve)& PC1, 
   const Handle(Geom2d_Curve)& PC2,
   const Handle(Geom_Surface)& S, 
   const TopLoc_Location& L, 
   const GeomAbs_Shape C) :
  BRep_CurveOnSurface(PC1,S,L),
  myPCurve2(PC2),
  myContinuity(C)
{
}


//=======================================================================
//function : IsCurveOnClosedSurface
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_CurveOnClosedSurface::IsCurveOnClosedSurface()const 
{
  return Standard_True;
}

//=======================================================================
//function : IsRegularity
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_CurveOnClosedSurface::IsRegularity()const 
{
  return Standard_True;
}


//=======================================================================
//function : IsRegularity
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_CurveOnClosedSurface::IsRegularity
  (const Handle(Geom_Surface)& S1, 
   const Handle(Geom_Surface)& S2, 
   const TopLoc_Location& L1, 
   const TopLoc_Location& L2)const 
{
  return ((Surface()  == S1) &&
	  (Surface()  == S2) &&
	  (Location() == L1) &&
	  (Location() == L2));
}



//=======================================================================
//function : PCurve2
//purpose  : 
//=======================================================================

const Handle(Geom2d_Curve)&  BRep_CurveOnClosedSurface::PCurve2()const 
{
  return myPCurve2;
}


//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

const GeomAbs_Shape&  BRep_CurveOnClosedSurface::Continuity()const 
{
  return myContinuity;
}

//=======================================================================
//function : Surface2
//purpose  : 
//=======================================================================

const Handle(Geom_Surface)&  BRep_CurveOnClosedSurface::Surface2()const 
{
  return Surface();
}


//=======================================================================
//function : Location2
//purpose  : 
//=======================================================================

const TopLoc_Location&  BRep_CurveOnClosedSurface::Location2()const 
{
  return Location();
}

//=======================================================================
//function : PCurve2
//purpose  : 
//=======================================================================

void  BRep_CurveOnClosedSurface::PCurve2(const Handle(Geom2d_Curve)& C)
{
  myPCurve2 = C;
}


//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

void BRep_CurveOnClosedSurface::Continuity(const GeomAbs_Shape C)
{
  myContinuity = C;
}


//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(BRep_CurveRepresentation) BRep_CurveOnClosedSurface::Copy() const
{
  Handle(BRep_CurveOnClosedSurface) C =
    new BRep_CurveOnClosedSurface(PCurve(),PCurve2(),
				  Surface(),Location(),myContinuity);

  C->SetRange(First(), Last());
  C->SetUVPoints(myUV1,myUV2);
  C->SetUVPoints2(myUV21,myUV22);

  return C;
}


//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

void  BRep_CurveOnClosedSurface::Update()
{
  if (!Precision::IsNegativeInfinite(First()))
    myPCurve2->D0(First(),myUV21);
  if (!Precision::IsPositiveInfinite(Last()))
    myPCurve2->D0(Last(),myUV22);
  BRep_CurveOnSurface::Update();
}


