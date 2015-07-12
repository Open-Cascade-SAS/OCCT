// Created on: 1993-06-23
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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


#include <Geom_Surface.hxx>
#include <TopOpeBRepDS_Surface.hxx>

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
