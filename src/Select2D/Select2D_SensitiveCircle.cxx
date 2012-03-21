// Created on: 1995-01-30
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



#include <Select2D_SensitiveCircle.ixx>
#include <BndLib.hxx>
#include <Bnd_Box2d.hxx>

//=====================================================
// Function : Create
// Purpose  :Constructor
//=====================================================


Select2D_SensitiveCircle::
Select2D_SensitiveCircle(const Handle(SelectBasics_EntityOwner)& OwnerId,
			 const gp_Circ2d& aCirc,
			 const Select2D_TypeOfSelection atype):
Select2D_SensitiveEntity(OwnerId),
myCirc(aCirc),
mytype(atype){}


//=====================================================
// Function : Areas
// Purpose  :
//=====================================================

void Select2D_SensitiveCircle::
Areas (SelectBasics_ListOfBox2d& boxes) 
{
  Bnd_Box2d abox;
  BndLib::Add(myCirc,myCirc.Radius()/100.,abox);
  boxes.Append(abox);
}


//=====================================================
// Function : Matches
// Purpose  :
//=====================================================
Standard_Boolean Select2D_SensitiveCircle::
Matches (const Standard_Real X,
	 const Standard_Real Y,
	 const Standard_Real aTol,
	 Standard_Real& DMin)
{
  Standard_Real TheTol = HasOwnTolerance()? myOwnTolerance : aTol;

  switch(mytype){
  case Select2D_TOS_INTERIOR:
    {
      if(myCirc.Contains(gp_Pnt2d(X,Y),TheTol) )
	{DMin=0.;
	 return Standard_True;}
      else 
	{DMin=myCirc.Distance(gp_Pnt2d(X,Y));}
    }
  case Select2D_TOS_BOUNDARY:
    {
      DMin = myCirc.Distance(gp_Pnt2d(X,Y));
      if(DMin<= TheTol) return Standard_True;
      
    }
  }  
  return Standard_False;
}

Standard_Boolean Select2D_SensitiveCircle::
Matches (const Standard_Real XMin,
	 const Standard_Real YMin,
	 const Standard_Real XMax,
	 const Standard_Real YMax,
	 const Standard_Real aTol)
{
  Standard_Real TheTol = HasOwnTolerance()? myOwnTolerance : aTol;

  Bnd_Box2d abox, BoundBox;
  BoundBox.Update(XMin-TheTol,YMin-TheTol,XMax+TheTol,YMax+TheTol);
  BndLib::Add(myCirc,myCirc.Radius()/100.,abox);

  if(BoundBox.IsOut(abox)) return Standard_False;
 return Standard_True;
}
 


