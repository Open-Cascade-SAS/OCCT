// Created on: 1995-01-26
// Created by: Mister rmi
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


#include <Select2D_SensitivePoint.ixx>
#include <Bnd_Box2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Lin2d.hxx>

//==================================
//function : Constructor
//purpose  : 
//==================================

Select2D_SensitivePoint::
Select2D_SensitivePoint(const Handle(SelectBasics_EntityOwner)& OwnerId,
			const gp_Pnt2d& Location,
			const Standard_Real InitSensitivity):
Select2D_SensitiveEntity(OwnerId),
mylocation(Location),
mysensitivity(InitSensitivity)
{}


//==================================
//function : Areas
//purpose  : 
//==================================

void Select2D_SensitivePoint::
Areas (SelectBasics_ListOfBox2d& boxes)
{ Bnd_Box2d abox;
 abox.Set(mylocation);
 abox.Enlarge(mysensitivity);
 boxes.Append(abox);
}
 
//==================================
//function : Matches
//purpose  : 
//==================================


Standard_Boolean Select2D_SensitivePoint::
Matches (const Standard_Real X,
	 const Standard_Real Y,
	 const Standard_Real aTol,
	 Standard_Real& MinDist)
{
  Standard_Real TheTol = HasOwnTolerance()? myOwnTolerance : aTol;
  MinDist = mylocation.Distance(gp_Pnt2d(X,Y));
  if(MinDist<=TheTol+mysensitivity) return Standard_True;
  return Standard_False;
}

Standard_Boolean Select2D_SensitivePoint::
Matches (const Standard_Real XMin,
	 const Standard_Real YMin,
	 const Standard_Real XMax,
	 const Standard_Real YMax,
	 const Standard_Real aTol)
{//distance point-Line....
  Standard_Real MinDist = gp_Lin2d(gp_Pnt2d(XMin,YMin),
		     gp_Vec2d(gp_Pnt2d(XMin,YMin),
			      gp_Pnt2d(XMax,YMax))
		     ).Distance(mylocation);
  
  return (MinDist<=aTol+mysensitivity);
}




