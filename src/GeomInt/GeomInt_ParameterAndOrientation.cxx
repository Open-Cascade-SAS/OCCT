// File:	GeomInt_ParameterAndOrientation.cxx
// Created:	Wed Feb  8 09:37:12 1995
// Author:	Jacques GOUSSARD
//		<jag@topsn2>


#include <GeomInt_ParameterAndOrientation.ixx>


//=======================================================================
//function : GeomInt_ParameterAndOrientation
//purpose  : 
//=======================================================================
  GeomInt_ParameterAndOrientation::GeomInt_ParameterAndOrientation() :
  prm(0.0),or1(TopAbs_FORWARD),or2(TopAbs_FORWARD)
{}



//=======================================================================
//function : GeomInt_ParameterAndOrientation
//purpose  : 
//=======================================================================
  GeomInt_ParameterAndOrientation::GeomInt_ParameterAndOrientation
   (const Standard_Real P,
    const TopAbs_Orientation Or1,
    const TopAbs_Orientation Or2) : prm(P), or1(Or1), or2(Or2)
{}



//=======================================================================
//function : SetOrientation1
//purpose  : 
//=======================================================================
  void GeomInt_ParameterAndOrientation::SetOrientation1
   (const TopAbs_Orientation Or1)
{
  or1 = Or1;
}


//=======================================================================
//function : SetOrientation2
//purpose  : 
//=======================================================================
  void GeomInt_ParameterAndOrientation::SetOrientation2
   (const TopAbs_Orientation Or2)
{
  or2 = Or2;
}


//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================
  Standard_Real GeomInt_ParameterAndOrientation::Parameter () const
{
  return prm;
}


//=======================================================================
//function : Orientation1
//purpose  : 
//=======================================================================
  TopAbs_Orientation GeomInt_ParameterAndOrientation::Orientation1 () const
{
  return or1;
}


//=======================================================================
//function : Orientation2
//purpose  : 
//=======================================================================
  TopAbs_Orientation GeomInt_ParameterAndOrientation::Orientation2 () const
{
  return or2;
}


