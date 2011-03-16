// File:	PXCAFDoc_Area.cxx
// Created:	Fri Sep  8 18:01:19 2000
// Author:	data exchange team
//		<det@nordox.nnov.matra-dtv.fr>


#include <PXCAFDoc_Area.ixx>

//=======================================================================
//function : PXCAFDoc_Area
//purpose  : 
//=======================================================================

PXCAFDoc_Area::PXCAFDoc_Area () { }

//=======================================================================
//function : PXCAFDoc_Area
//purpose  : 
//=======================================================================

PXCAFDoc_Area::PXCAFDoc_Area (const Standard_Real V) : myValue (V) {}
    
//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void PXCAFDoc_Area::Set (const Standard_Real V) 
{
  myValue = V;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

Standard_Real PXCAFDoc_Area::Get () const
{
  return myValue;
}
