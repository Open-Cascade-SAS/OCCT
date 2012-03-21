// Created on: 1995-01-31
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


#include <Select2D_SensitiveBox.ixx>
#include <gp_Vec2d.hxx>

//=============================================================
//   Function : Constructor
//   Purpose  : 
//=============================================================    


Select2D_SensitiveBox::
Select2D_SensitiveBox (const Handle(SelectBasics_EntityOwner)& OwnerId,
			    const gp_Pnt2d& Center,
			    const Standard_Real Height,
			    const Standard_Real Width,
			    const Select2D_TypeOfSelection Type):
Select2D_SensitiveEntity(OwnerId),
mytype(Type)
{
  mybox.Set(Center.Translated(gp_Vec2d(Width/2.,Height/2.)));
  mybox.Add(Center.Translated(gp_Vec2d(-Width/2.,-Height/2.)));
}


Select2D_SensitiveBox::
Select2D_SensitiveBox (const Handle(SelectBasics_EntityOwner)& OwnerId,
		       const Standard_Real XMin,
		       const Standard_Real YMin,
		       const Standard_Real XMax,
		       const Standard_Real YMax,
		       const Select2D_TypeOfSelection Type):
Select2D_SensitiveEntity(OwnerId),
mytype(Type)
{
 mybox.Update(XMin,YMin,XMax,YMax);
}
//=============================================================
//   Function : Areas
//   Purpose  : 
//=============================================================    


void Select2D_SensitiveBox::Areas (SelectBasics_ListOfBox2d& boxes) 
{
  boxes.Append(mybox);
}

//=============================================================
//   Function : Matches
//   Purpose  : 
//=============================================================    


Standard_Boolean Select2D_SensitiveBox::
Matches (const Standard_Real X,
	 const Standard_Real Y,
	 const Standard_Real aTol,
	 Standard_Real& DMin)
{
  Standard_Real TheTol = HasOwnTolerance()? myOwnTolerance : aTol;

  Bnd_Box2d TmpBox;
  TmpBox.Add(mybox);
  TmpBox.Enlarge(TheTol);
  DMin=0.;

  gp_Pnt2d PickPoint(X,Y);
  //Standard_Boolean Status = Standard_False;

  switch(mytype) {
  case Select2D_TOS_INTERIOR:
    {
      if(!TmpBox.IsOut(PickPoint)) return Standard_True;
    }
  case Select2D_TOS_BOUNDARY:
    {
      TmpBox.Enlarge(TheTol);
      if(mybox.IsOut(PickPoint)&&
	 !TmpBox.IsOut(PickPoint)) return Standard_True;
    }
  }
  return Standard_False;
}


Standard_Boolean Select2D_SensitiveBox::
Matches (const Standard_Real XMin,
	 const Standard_Real YMin,
	 const Standard_Real XMax,
	 const Standard_Real YMax,
	 const Standard_Real aTol)
{
  Standard_Real TheTol = HasOwnTolerance()? myOwnTolerance : aTol;

  Bnd_Box2d BoundBox;
  BoundBox.Update(XMin-TheTol,YMin-TheTol,XMax+TheTol,YMax+TheTol);
  if(BoundBox.IsOut(mybox)) return Standard_False;
  return Standard_True;
}
