// File:	math_CompareOfValueAndWeight.cxx
// Created:	Tue Dec 20 13:30:54 2005
// Author:	Julia GERASIMOVA
//		<jgv@clubox>


#include <math_CompareOfValueAndWeight.ixx>

// -----------
// Create :
// -----------
math_CompareOfValueAndWeight::math_CompareOfValueAndWeight()
{
}

// -----------
// IsLower :
// -----------
Standard_Boolean math_CompareOfValueAndWeight::IsLower(const math_ValueAndWeight& Left,
						       const math_ValueAndWeight& Right) const
{
  return (Left.Value() < Right.Value());
}

// -----------
// IsGreater :
// -----------
Standard_Boolean math_CompareOfValueAndWeight::IsGreater(const math_ValueAndWeight& Left,
							 const math_ValueAndWeight& Right) const
{
  return (Left.Value() > Right.Value());
}

// -----------
// IsEqual :
// -----------
Standard_Boolean math_CompareOfValueAndWeight::IsEqual(const math_ValueAndWeight& Left,
						       const math_ValueAndWeight& Right) const
{
  return (Left.Value() == Right.Value());
}
