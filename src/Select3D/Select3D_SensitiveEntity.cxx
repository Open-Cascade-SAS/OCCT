// Created on: 1995-03-13
// Created by: Robert COUBLANC
// Copyright (c) 1995-1999 Matra Datavision
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

const TopLoc_Location Select3D_SensitiveEntity::Location() const 
{
  TopLoc_Location anIdentity;	
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
