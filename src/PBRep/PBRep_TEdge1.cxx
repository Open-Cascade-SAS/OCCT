// File:	PBRep_TEdge1.cxx
// Created:	Mon Jul 26 10:29:12 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>


#include <PBRep_TEdge1.ixx>

// Enum terms are better than statics who must be intialized.
enum {
  PBRep_ParameterMask       = 1,
  PBRep_RangeMask           = 2,
  PBRep_DegeneratedMask     = 4
};

//=======================================================================
//function : PBRep_TEdge1
//purpose  : 
//=======================================================================

PBRep_TEdge1::PBRep_TEdge1() 
{
}

//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

Standard_Real  PBRep_TEdge1::Tolerance()const 
{
  return myTolerance;
}


//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

void  PBRep_TEdge1::Tolerance(const Standard_Real T)
{
  myTolerance = T;
}


//=======================================================================
//function : SameParameter
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_TEdge1::SameParameter()const 
{
  return myFlags & PBRep_ParameterMask;
}


//=======================================================================
//function : SameParameter
//purpose  : 
//=======================================================================

void  PBRep_TEdge1::SameParameter(const Standard_Boolean S)
{
  if (S) myFlags |= PBRep_ParameterMask;
  else   myFlags &= ~PBRep_ParameterMask;
}


//=======================================================================
//function : SameRange
//purpose  : 
//=======================================================================

 Standard_Boolean  PBRep_TEdge1::SameRange()const 
{
  return myFlags & PBRep_RangeMask;
}


//=======================================================================
//function : SameRange
//purpose  : 
//=======================================================================

 void  PBRep_TEdge1::SameRange(const Standard_Boolean S)
{
  if (S) myFlags |= PBRep_RangeMask;
  else   myFlags &= ~PBRep_RangeMask;
}

//=======================================================================
//function : Degenerated
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_TEdge1::Degenerated()const 
{
  return myFlags & PBRep_DegeneratedMask;
}


//=======================================================================
//function : Degenerated
//purpose  : 
//=======================================================================

void  PBRep_TEdge1::Degenerated(const Standard_Boolean S)
{
  if (S) myFlags |= PBRep_DegeneratedMask; 
  else   myFlags &= ~PBRep_DegeneratedMask;
}


//=======================================================================
//function : Curves
//purpose  : 
//=======================================================================

Handle(PBRep_CurveRepresentation)  PBRep_TEdge1::Curves()const 
{
  return myCurves;
}


//=======================================================================
//function : Curves
//purpose  : 
//=======================================================================

void  PBRep_TEdge1::Curves(const Handle(PBRep_CurveRepresentation)& C)
{
  myCurves = C;
}


