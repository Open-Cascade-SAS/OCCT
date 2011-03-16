// File:	PDataStd_Real.cxx
// Created:	Wed Apr  9 13:24:14 1997
// Author:	VAUTHIER Jean-Claude

#include <PDataStd_Real.ixx>


//=======================================================================
//function : PDataStd_Real
//purpose  : 
//=======================================================================

PDataStd_Real::PDataStd_Real () { }



//=======================================================================
//function : PDataStd_Real
//purpose  : 
//=======================================================================

PDataStd_Real::PDataStd_Real (const Standard_Real V, 
			      const Standard_Integer D) : myValue (V), myDimension(D) { }


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void PDataStd_Real::Set (const Standard_Real V) 
{
  myValue = V;
}



//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

Standard_Real PDataStd_Real::Get () const
{
  return myValue;
}

//=======================================================================
//function : SetDimension
//purpose  : 
//=======================================================================

void PDataStd_Real::SetDimension (const Standard_Integer D)
{
  myDimension = D;
}



//=======================================================================
//function : GetDimension
//purpose  : 
//=======================================================================

Standard_Integer PDataStd_Real::GetDimension () const
{
  return myDimension;
}


