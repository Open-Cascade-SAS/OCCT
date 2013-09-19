// Created on: 1995-03-13
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





#include <Select3D_SensitiveEntity.ixx>
#include <Precision.hxx>
#include <SelectBasics_EntityOwner.hxx>
#include <Select3D_Macro.hxx>

//=======================================================================
//function : Select3D_SensitiveEntity
//purpose  : 
//=======================================================================

Select3D_SensitiveEntity::Select3D_SensitiveEntity(const Handle(SelectBasics_EntityOwner)& OwnerId):
SelectBasics_SensitiveEntity(OwnerId)
{}

//=======================================================================
//function : Matches
//purpose  : 
//=======================================================================

Standard_Boolean Select3D_SensitiveEntity::Matches(const Standard_Real,
                                                   const Standard_Real,
                                                   const Standard_Real,
                                                   const Standard_Real,
                                                   const Standard_Real)
{
  return Standard_False;
}

//=======================================================================
//function : Matches
//purpose  : 
//=======================================================================

Standard_Boolean Select3D_SensitiveEntity::Matches(const TColgp_Array1OfPnt2d&,
                                                   const Bnd_Box2d&,
                                                   const Standard_Real)
{
  return Standard_False;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Select3D_SensitiveEntity::Dump(Standard_OStream& S, const Standard_Boolean) const
{
  S<<"\tSensitive Entity 3D"<<endl;
}

//=======================================================================
//function : DumpBox
//purpose  : 
//=======================================================================

void Select3D_SensitiveEntity::DumpBox(Standard_OStream& S,const Bnd_Box2d& b2d) 
{
  if(!b2d.IsVoid())
  {
    Standard_Real xmin,ymin,xmax,ymax;
    b2d.Get(xmin,ymin,xmax,ymax);
    S<<"\t\t\tBox2d: PMIN ["<<xmin<<" , "<<ymin<<"]"<<endl;
    S<<"\t\t\t       PMAX ["<<xmax<<" , "<<ymax<<"]"<<endl;
  }
}

//=======================================================================
//function : ResetLocation
//purpose  : 
//=======================================================================

void Select3D_SensitiveEntity::ResetLocation()
{
}

//=======================================================================
//function : SetLocation
//purpose  : 
//=======================================================================

void Select3D_SensitiveEntity::SetLocation(const TopLoc_Location&)
{
}

//=======================================================================
//function : UpdateLocation
//purpose  : 
//=======================================================================

void Select3D_SensitiveEntity::UpdateLocation(const TopLoc_Location& aLoc)
{
  if(aLoc.IsIdentity() || aLoc == Location()) return;
  if(!HasLocation())
    SetLocation(aLoc);
  else 
  {
    TopLoc_Location compLoc = aLoc * Location();
    SetLocation(compLoc);
  }
}

//=======================================================================
//function : Location
//purpose  : 
//=======================================================================

const TopLoc_Location& Select3D_SensitiveEntity::Location() const 
{
  static TopLoc_Location anIdentity;	
  Handle(SelectBasics_EntityOwner) anOwner = OwnerId();
  return anOwner.IsNull() ? anIdentity : anOwner->Location();
}

//=======================================================================
//function : HasLocation
//purpose  : 
//=======================================================================

Standard_Boolean Select3D_SensitiveEntity::HasLocation() const
{ 
  Handle(SelectBasics_EntityOwner) anOwner = OwnerId();
  return (!anOwner.IsNull() && anOwner->HasLocation());
}

//=======================================================================
//function : Is3D
//purpose  : 
//=======================================================================

Standard_Boolean Select3D_SensitiveEntity::Is3D() const
{return Standard_True;}

//=======================================================================
//function : MaxBoxes
//purpose  : 
//=======================================================================

Standard_Integer Select3D_SensitiveEntity::MaxBoxes() const 
{return 1;}

//=======================================================================
//function : GetConnected
//purpose  : 
//=======================================================================

Handle(Select3D_SensitiveEntity) Select3D_SensitiveEntity::GetConnected(const TopLoc_Location&)  
{
  Handle(Select3D_SensitiveEntity) NiouEnt;
  return NiouEnt;
}
