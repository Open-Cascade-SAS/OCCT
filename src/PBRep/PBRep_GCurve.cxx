#include <PBRep_GCurve.ixx>


//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

PBRep_GCurve::PBRep_GCurve(const PTopLoc_Location& L, 
			   const Standard_Real First, 
			   const Standard_Real Last) :
			   PBRep_CurveRepresentation(L),
			   myFirst(First),
			   myLast(Last)
{
}

//=======================================================================
//function : First
//purpose  : 
//=======================================================================

Standard_Real  PBRep_GCurve::First()const 
{
  return myFirst;
}


//=======================================================================
//function : First
//purpose  : 
//=======================================================================

void  PBRep_GCurve::First(const Standard_Real F)
{
  myFirst = F;
}


//=======================================================================
//function : Last
//purpose  : 
//=======================================================================

Standard_Real  PBRep_GCurve::Last()const 
{
  return myLast;
}


//=======================================================================
//function : Last
//purpose  : 
//=======================================================================

void  PBRep_GCurve::Last(const Standard_Real L)
{
  myLast = L;
}


//=======================================================================
//function : IsGCurve
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_GCurve::IsGCurve() const 
{
  return Standard_True;
}

