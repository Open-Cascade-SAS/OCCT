// File:	math_ValueAndWeight.cxx
// Created:	Tue Dec 20 18:16:21 2005
// Author:	Julia GERASIMOVA
//		<jgv@clubox>


#include <math_ValueAndWeight.ixx>

math_ValueAndWeight::math_ValueAndWeight()
{
}

math_ValueAndWeight::math_ValueAndWeight(const Standard_Real Value,
					 const Standard_Real Weight)
     : myValue(Value),
       myWeight(Weight)
{
}

Standard_Real math_ValueAndWeight::Value() const
{
  return myValue;
}

Standard_Real math_ValueAndWeight::Weight() const
{
  return myWeight;
}
