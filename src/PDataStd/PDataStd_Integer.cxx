// File:	PDataStd_Integer.cxx
// Created:	Wed Apr  9 13:23:12 1997
// Author:	VAUTHIER Jean-Claude


#include <PDataStd_Integer.ixx>


//=======================================================================
//function : PDataStd_Integer
//purpose  : 
//=======================================================================

PDataStd_Integer::PDataStd_Integer () {}


//=======================================================================
//function : PDataStd_Integer
//purpose  : 
//=======================================================================

PDataStd_Integer::PDataStd_Integer (const Standard_Integer V) : myValue (V) { }



//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void PDataStd_Integer::Set (const Standard_Integer V) 
{
  myValue = V;
}


//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

Standard_Integer PDataStd_Integer::Get () const
{
  return myValue;
}

