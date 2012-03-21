// Created on: 1991-04-24
// Created by: Arnaud BOUZY
// Copyright (c) 1991-1999 Matra Datavision
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



#include <Draw_Drawable3D.ixx>

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


