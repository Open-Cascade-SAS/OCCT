
// Copyright: 	Matra-Datavision 1992
//		<mip@sdsun3>
// File:	TCollection_CompareOfReal.cxx
// Created:	Thu Aug 27 12:06:34 1992
// Author:	Mireille MERCIEN

#include <TCollection_CompareOfReal.ixx>

// -----------
// Create :
// -----------
TCollection_CompareOfReal::TCollection_CompareOfReal()
{
}

// -----------
// IsLower :
// -----------
Standard_Boolean TCollection_CompareOfReal::IsLower (
       const Standard_Real &Left,const Standard_Real &Right) const
{
   return (Left < Right) ;
}

// -----------
// IsGreater :
// -----------
Standard_Boolean TCollection_CompareOfReal::IsGreater (
       const Standard_Real &Left,const Standard_Real &Right) const
{
   return (Left > Right) ;
}						





