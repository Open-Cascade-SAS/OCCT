// Copyright: 	Matra-Datavision 1991
// File:	Draw_Drawable3D.cxx
// Created:	Wed Apr 24 15:44:04 1991
// Author:	Arnaud BOUZY
//		<adn>


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


