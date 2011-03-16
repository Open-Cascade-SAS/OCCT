// File:	PXCAFDoc_Volume.cxx
// Created:	Fri Sep  8 18:01:19 2000
// Author:	data exchange team
//		<det@nordox.nnov.matra-dtv.fr>


#include <PXCAFDoc_Volume.ixx>

//=======================================================================
//function : PXCAFDoc_Volume
//purpose  : 
//=======================================================================

PXCAFDoc_Volume::PXCAFDoc_Volume () { }

//=======================================================================
//function : PXCAFDoc_Volume
//purpose  : 
//=======================================================================

PXCAFDoc_Volume::PXCAFDoc_Volume (const Standard_Real V) : myValue (V) {}
    
//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void PXCAFDoc_Volume::Set (const Standard_Real V) 
{
  myValue = V;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

Standard_Real PXCAFDoc_Volume::Get () const
{
  return myValue;
}
