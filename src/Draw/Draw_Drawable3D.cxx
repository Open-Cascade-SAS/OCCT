// Created on: 1991-04-24
// Created by: Arnaud BOUZY
// Copyright (c) 1991-1999 Matra Datavision
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


#include <Draw_Display.hxx>
#include <Draw_Drawable3D.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Draw_Drawable3D,Standard_Transient)

//=======================================================================
//function : Draw_Drawable3D
//purpose  : 
//=======================================================================
Draw_Drawable3D::Draw_Drawable3D() :
  isVisible(Standard_False),
  isProtected(Standard_False),
  myName(NULL)
{
}

//=======================================================================
//function : PickReject
//purpose  : 
//=======================================================================

Standard_Boolean Draw_Drawable3D::PickReject(const Standard_Real X,
					     const Standard_Real Y,
					     const Standard_Real Prec) const
{
  return ((X+Prec < myXmin) || (X-Prec > myXmax) ||
	  (Y+Prec < myYmin) || (Y-Prec > myYmax));
}
 

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Draw_Drawable3D)  Draw_Drawable3D::Copy() const
{
  return this;
}


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void  Draw_Drawable3D::Dump(Standard_OStream& S) const
{
  S << myXmin << " " << myXmax << "\n";
  S << myYmin << " " << myYmax << "\n";
}


//=======================================================================
//function : Whatis
//purpose  : 
//=======================================================================

void  Draw_Drawable3D::Whatis(Draw_Interpretor& S) const
{
  S << "drawable 3d";
}

//=======================================================================
//function : Is3D
//purpose  : 
//=======================================================================

Standard_Boolean Draw_Drawable3D::Is3D() const
{
  return Standard_True;
}

//=======================================================================
//function : SetBounds
//purpose  : 
//=======================================================================

void  Draw_Drawable3D::SetBounds(const Standard_Real xmin, 
				 const Standard_Real xmax, 
				 const Standard_Real ymin, 
				 const Standard_Real ymax)
{
  myXmin = xmin;
  myXmax = xmax;
  myYmin = ymin;
  myYmax = ymax;
}


//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================

void  Draw_Drawable3D::Bounds(Standard_Real& xmin, 
			      Standard_Real& xmax, 
			      Standard_Real& ymin, 
			      Standard_Real& ymax) const
{
  xmin = myXmin;
  xmax = myXmax;
  ymin = myYmin;
  ymax = myYmax;
}


