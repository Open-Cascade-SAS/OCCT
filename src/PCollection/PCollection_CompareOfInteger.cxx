// Copyright: 	Matra-Datavision 1992
//		<mip@sdsun3>
// File:	PCollection_CompareOfInteger.cxx
// Created:	Thu Aug 27 12:06:34 1992
// Author:	Mireille MERCIEN

#include <PCollection_CompareOfInteger.ixx>

// -----------
// Create :
// -----------
PCollection_CompareOfInteger::PCollection_CompareOfInteger()
{
}

// -----------
// IsLower :
// -----------
Standard_Boolean PCollection_CompareOfInteger::IsLower (
       const Standard_Integer &Left,const Standard_Integer &Right) const
{
   return (Left < Right) ;
}

// -----------
// IsGreater :
// -----------
Standard_Boolean PCollection_CompareOfInteger::IsGreater (
       const Standard_Integer &Left,const Standard_Integer &Right) const
{
   return (Left > Right) ;
}						
