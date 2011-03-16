// File:	GeomFill_CornerState.cxx
// Created:	Fri Dec  8 16:26:44 1995
// Author:	Laurent BOURESCHE
//		<lbo@phylox>


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


