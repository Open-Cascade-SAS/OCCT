
// Copyright: 	Matra-Datavision 1992
//		<mip@sdsun3>
// File:	TCollection_CompareOfInteger.cxx
// Created:	Thu Aug 27 12:06:34 1992
// Author:	Mireille MERCIEN

#include <TCollection_CompareOfInteger.ixx>

// -----------
// Create :
// -----------
TCollection_CompareOfInteger::TCollection_CompareOfInteger()
{
}

// -----------
// IsLower :
// -----------
Standard_Boolean TCollection_CompareOfInteger::IsLower (
       const Standard_Integer &Left,const Standard_Integer &Right) const
{
   return (Left < Right) ;
}

// -----------
// IsGreater :
// -----------
Standard_Boolean TCollection_CompareOfInteger::IsGreater (
       const Standard_Integer &Left,const Standard_Integer &Right) const
{
   return (Left > Right) ;
}						
