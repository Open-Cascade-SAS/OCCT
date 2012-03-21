// Created on: 1993-06-23
// Created by: Jean Yves LEBEY
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


#include <TopOpeBRepDS_Surface.ixx>

//=======================================================================
//function : TopOpeBRepDS_Surface
//purpose  : 
//=======================================================================

TopOpeBRepDS_Surface::TopOpeBRepDS_Surface()
{
}

//=======================================================================
//function : TopOpeBRepDS_Surface
//purpose  : 
//=======================================================================

TopOpeBRepDS_Surface::TopOpeBRepDS_Surface
  (const Handle(Geom_Surface)& P,
   const Standard_Real T) :
       mySurface(P),
       myTolerance(T)
{
}

//
//modified by NIZNHY-PKV Tue Oct 30 09:28:33 2001 f
//=======================================================================
//function : TopOpeBRepDS_Surface::TopOpeBRepDS_Surface
//purpose  : 
//=======================================================================
TopOpeBRepDS_Surface::TopOpeBRepDS_Surface (const TopOpeBRepDS_Surface& Other) 
{
  Assign(Other);
}
//=======================================================================
//function : Assign
//purpose  : 
//=======================================================================
void TopOpeBRepDS_Surface::Assign(const TopOpeBRepDS_Surface& Other)
{
  mySurface=Other.mySurface;
  myTolerance=Other.myTolerance;
  myKeep=Other.myKeep;
}
//modified by NIZNHY-PKV Tue Oct 30 09:28:20 2001 t
//

//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

const Handle(Geom_Surface)&  TopOpeBRepDS_Surface::Surface()const 
{
  return mySurface;
}


//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

Standard_Real  TopOpeBRepDS_Surface::Tolerance()const 
{
  return myTolerance;
}

//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

void TopOpeBRepDS_Surface::Tolerance(const Standard_Real T)
{
  myTolerance = T;
}

//=======================================================================
//function : Keep
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_Surface::Keep() const
{
  return myKeep;
}
//=======================================================================
//function : ChangeKeep
//purpose  : 
//=======================================================================

void TopOpeBRepDS_Surface::ChangeKeep(const Standard_Boolean b)
{
  myKeep = b;
}
