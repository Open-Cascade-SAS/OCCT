// File:	PBRep_TEdge.cxx
// Created:	Mon Jul 26 10:29:12 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>


#include <PBRep_TEdge.ixx>

static const Standard_Integer ParameterMask       = 1;
static const Standard_Integer RangeMask           = 2;
static const Standard_Integer DegeneratedMask     = 4;

//=======================================================================
//function : PBRep_TEdge
//purpose  : 
//=======================================================================

PBRep_TEdge::PBRep_TEdge() 
{
}

//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

Standard_Real  PBRep_TEdge::Tolerance()const 
{
  return myTolerance;
}


//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

void  PBRep_TEdge::Tolerance(const Standard_Real T)
{
  myTolerance = T;
}


//=======================================================================
//function : SameParameter
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_TEdge::SameParameter()const 
{
  return myFlags & ParameterMask;
}


//=======================================================================
//function : SameParameter
//purpose  : 
//=======================================================================

void  PBRep_TEdge::SameParameter(const Standard_Boolean S)
{
  if (S) myFlags |= ParameterMask;
  else   myFlags &= ~ParameterMask;
}


//=======================================================================
//function : SameRange
//purpose  : 
//=======================================================================

 Standard_Boolean  PBRep_TEdge::SameRange()const 
{
  return myFlags & RangeMask;
}


//=======================================================================
//function : SameRange
//purpose  : 
//=======================================================================

 void  PBRep_TEdge::SameRange(const Standard_Boolean S)
{
  if (S) myFlags |= RangeMask;
  else   myFlags &= ~RangeMask;
}

//=======================================================================
//function : Degenerated
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_TEdge::Degenerated()const 
{
  return myFlags & DegeneratedMask;
}


//=======================================================================
//function : Degenerated
//purpose  : 
//=======================================================================

void  PBRep_TEdge::Degenerated(const Standard_Boolean S)
{
  if (S) myFlags |= DegeneratedMask; 
  else   myFlags &= ~DegeneratedMask;
}


//=======================================================================
//function : Curves
//purpose  : 
//=======================================================================

Handle(PBRep_CurveRepresentation)  PBRep_TEdge::Curves()const 
{
  return myCurves;
}


//=======================================================================
//function : Curves
//purpose  : 
//=======================================================================

void  PBRep_TEdge::Curves(const Handle(PBRep_CurveRepresentation)& C)
{
  myCurves = C;
}


