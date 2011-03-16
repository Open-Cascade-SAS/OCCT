// File:	PXCAFDoc_Color.cxx
// Created:	Wed Aug 16 12:12:01 2000
// Author:	data exchange team
//		<det@strelox.nnov.matra-dtv.fr>


#include <PXCAFDoc_Color.ixx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

PXCAFDoc_Color::PXCAFDoc_Color()
{
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

PXCAFDoc_Color::PXCAFDoc_Color(const Quantity_Color& C)
{
  myColor = C;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

 void PXCAFDoc_Color::Set(const Quantity_Color& C) 
{
  myColor = C;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

 Quantity_Color PXCAFDoc_Color::Get() const
{
  return myColor;
}

