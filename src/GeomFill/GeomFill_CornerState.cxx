// Created on: 1995-12-08
// Created by: Laurent BOURESCHE
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


#include <GeomFill_CornerState.hxx>

//=======================================================================
//function : GeomFill_CornerState
//purpose  : 
//=======================================================================
GeomFill_CornerState::GeomFill_CornerState() :
 gap(RealLast()),
 isconstrained(0),
 scal(1.),
 coonscnd(1)
{
}


//=======================================================================
//function : Gap
//purpose  : 
//=======================================================================

Standard_Real GeomFill_CornerState::Gap() const 
{
  return gap;
}


//=======================================================================
//function : Gap
//purpose  : 
//=======================================================================

void GeomFill_CornerState::Gap(const Standard_Real G)
{
  gap = G;
}


//=======================================================================
//function : TgtAng
//purpose  : 
//=======================================================================

Standard_Real GeomFill_CornerState::TgtAng() const 
{
  return tgtang;
}


//=======================================================================
//function : TgtAng
//purpose  : 
//=======================================================================

void GeomFill_CornerState::TgtAng(const Standard_Real Ang)
{
  tgtang = Ang;
}


//=======================================================================
//function : HasConstraint
//purpose  : 
//=======================================================================

Standard_Boolean GeomFill_CornerState::HasConstraint() const 
{
  return isconstrained;
}


//=======================================================================
//function : Constraint
//purpose  : 
//=======================================================================

void GeomFill_CornerState::Constraint()
{
  isconstrained = 1;
}


//=======================================================================
//function : NorAng
//purpose  : 
//=======================================================================

Standard_Real GeomFill_CornerState::NorAng() const 
{
  return norang;
}


//=======================================================================
//function : NorAng
//purpose  : 
//=======================================================================

void GeomFill_CornerState::NorAng(const Standard_Real Ang)
{
  norang = Ang;
}


//=======================================================================
//function : IsToKill
//purpose  : 
//=======================================================================

Standard_Boolean GeomFill_CornerState::IsToKill(Standard_Real& Scal) const 
{
  Scal = scal;
  if(!isconstrained) return 0;
  return !coonscnd;
}


//=======================================================================
//function : DoKill
//purpose  : 
//=======================================================================

void GeomFill_CornerState::DoKill(const Standard_Real Scal)
{
  scal = Scal;
  coonscnd = 0;
}


