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

#include <TopOpeBRepDS_Surface.hxx>

#include <Geom_Surface.hxx>

//=======================================================================
//function : TopOpeBRepDS_Surface
//purpose  : 
//=======================================================================
TopOpeBRepDS_Surface::TopOpeBRepDS_Surface()
: myTolerance (0.0),
  myKeep (Standard_False)
{
}

//=======================================================================
//function : TopOpeBRepDS_Surface
//purpose  : 
//=======================================================================

TopOpeBRepDS_Surface::TopOpeBRepDS_Surface (const Handle(Geom_Surface)& theSurface,
                                            const Standard_Real theTolerance)
: mySurface (theSurface),
  myTolerance (theTolerance),
  myKeep (Standard_False)
{
}

//=======================================================================
//function : TopOpeBRepDS_Surface::TopOpeBRepDS_Surface
//purpose  : 
//=======================================================================
TopOpeBRepDS_Surface::TopOpeBRepDS_Surface (const TopOpeBRepDS_Surface& theOther)
: mySurface (theOther.mySurface),
  myTolerance (theOther.myTolerance),
  myKeep (theOther.myKeep)
{
  //
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
