// Created on: 1995-12-08
// Created by: Laurent BOURESCHE
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



#include <GeomFill_CornerState.ixx>
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


