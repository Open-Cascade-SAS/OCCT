// Created on: 1995-05-23
// Created by: Robert COUBLANC
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




#include <Select2D_SensitiveArc.ixx>
#include <Bnd_Box2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
//#include <.hxx>
//#include <.hxx>
//#include <.hxx>


//==================================================
// Function: Constructor 
// Purpose :
//==================================================

Select2D_SensitiveArc::
Select2D_SensitiveArc(const Handle(SelectBasics_EntityOwner)& OwnerId,
		      const gp_Ax2d& OriginAxis,
		      const Standard_Real Angle,
		      const Standard_Real Radius,
		      const Standard_Integer NbPoints):
Select2D_SensitiveEntity(OwnerId),
myradius(Radius),
myax2d( OriginAxis),
myangle(Angle),
mynbpt(NbPoints)

{
  if(myangle<=Precision::Confusion()) myangle=2*M_PI;
}


//=====================================================
// Function : Areas
// Purpose  :
//=====================================================

void Select2D_SensitiveArc::
Areas (SelectBasics_ListOfBox2d& boxes) 
{
  Bnd_Box2d abox;
  Standard_Real deteta =myangle/mynbpt;


  gp_Pnt2d P1 = myax2d.Location().Translated(gp_Vec2d(myax2d.Direction())*myradius);
  
  abox.Set(P1);
  for(Standard_Integer i=1;i<=mynbpt;i++){
    abox.Add(P1.Rotated(myax2d.Location(),deteta*i));}
  boxes.Append(abox);
}


//=====================================================
// Function : Matches
// Purpose  :
//=====================================================
Standard_Boolean Select2D_SensitiveArc::
Matches (const Standard_Real X,
	 const Standard_Real Y,
	 const Standard_Real aTol,
	 Standard_Real& DMin)
{
  Standard_Real TheTol = HasOwnTolerance()? myOwnTolerance : aTol;

  gp_Pnt2d Pick(X,Y);
  Standard_Real Angle = (gp_Dir2d(gp_Vec2d(myax2d.Location(),Pick))).Angle(myax2d.Direction());
  if(Angle<0 && Angle>myangle) return Standard_False;
  else
    {
      DMin =myax2d.Location().Distance(Pick);
      if (DMin>myradius-TheTol&&DMin<=myradius+TheTol) return Standard_True;
    }
  return Standard_False;
}

Standard_Boolean Select2D_SensitiveArc::
Matches (const Standard_Real XMin,
	 const Standard_Real YMin,
	 const Standard_Real XMax,
	 const Standard_Real YMax,
	 const Standard_Real aTol)
{
  Standard_Real TheTol = HasOwnTolerance()? myOwnTolerance : aTol;
  if(mynbpt<1) return Standard_True; //to avoid problems...

  Standard_Real deteta = myangle/mynbpt;
  Bnd_Box2d BoundBox;
  BoundBox.Update(XMin-TheTol,YMin-TheTol,XMax+TheTol,YMax+TheTol);
  gp_Pnt2d ExtPt = myax2d.Location().Translated(gp_Vec2d(myax2d.Direction())*myradius);

  if(BoundBox.IsOut(ExtPt)) return Standard_False;

  for (Standard_Integer I=1;I<=mynbpt;I++)
    {
      if(BoundBox.IsOut(ExtPt.Rotated(myax2d.Location(),deteta*I))) return Standard_False;
    }
  return Standard_True;
}




