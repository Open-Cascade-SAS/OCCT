// File:	BRep_TEdge.cxx
// Created:	Tue Aug 25 15:37:58 1992
// Author:	Modelistation
//		<model@phylox>


#include <BRep_TEdge.ixx>
#include <TopAbs.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_ListOfCurveRepresentation.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_CurveOn2Surfaces.hxx>

static const Standard_Integer ParameterMask       = 1;
static const Standard_Integer RangeMask           = 2;
static const Standard_Integer DegeneratedMask     = 4;

//=======================================================================
//function : BRep_TEdge
//purpose  : 
//=======================================================================

BRep_TEdge::BRep_TEdge() :
       TopoDS_TEdge(),
       myTolerance(RealEpsilon()),
       myFlags(0)
{
  SameParameter(Standard_True);
  SameRange(Standard_True);
}

//=======================================================================
//function : SameParameter
//purpose  : 
//=======================================================================

 Standard_Boolean  BRep_TEdge::SameParameter()const 
{
  return myFlags & ParameterMask;
}


//=======================================================================
//function : SameParameter
//purpose  : 
//=======================================================================

 void  BRep_TEdge::SameParameter(const Standard_Boolean S)
{
  if (S) myFlags |= ParameterMask;
  else   myFlags &= ~ParameterMask;
}


//=======================================================================
//function : SameRange
//purpose  : 
//=======================================================================

 Standard_Boolean  BRep_TEdge::SameRange()const 
{
  return myFlags & RangeMask;
}


//=======================================================================
//function : SameRange
//purpose  : 
//=======================================================================

 void  BRep_TEdge::SameRange(const Standard_Boolean S)
{
  if (S) myFlags |= RangeMask;
  else   myFlags &= ~RangeMask;
}


//=======================================================================
//function : Degenerated
//purpose  : 
//=======================================================================

 Standard_Boolean  BRep_TEdge::Degenerated()const 
{
  return myFlags & DegeneratedMask;
}


//=======================================================================
//function : Degenerated
//purpose  : 
//=======================================================================

 void  BRep_TEdge::Degenerated(const Standard_Boolean S)
{
  if (S) myFlags |= DegeneratedMask; 
  else   myFlags &= ~DegeneratedMask;
}

//=======================================================================
//function : EmptyCopy
//purpose  : 
//=======================================================================

Handle(TopoDS_TShape) BRep_TEdge::EmptyCopy() const
{
  Handle(BRep_TEdge) TE = 
    new BRep_TEdge();
  TE->Tolerance(myTolerance);
  // copy the curves representations
  BRep_ListOfCurveRepresentation& l = TE->ChangeCurves();
  BRep_ListIteratorOfListOfCurveRepresentation itr(myCurves);
  
  while (itr.More()) {
    // on ne recopie PAS les polygones
    if ( itr.Value()->IsKind(STANDARD_TYPE(BRep_GCurve)) ||
         itr.Value()->IsKind(STANDARD_TYPE(BRep_CurveOn2Surfaces)) ) {
      l.Append(itr.Value()->Copy());
    }
    itr.Next();
  }

  TE->Degenerated(Degenerated());
  TE->SameParameter(SameParameter());
  TE->SameRange(SameRange());
  
  return TE;
}

