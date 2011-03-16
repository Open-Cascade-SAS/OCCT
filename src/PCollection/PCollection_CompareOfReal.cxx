// Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1992
//		<mip@sdsun3>
// File:	PCollection_CompareOfReal.cxx
// Created:	Thu Aug 27 12:06:34 1992
// Author:	Mireille MERCIEN

#include <PCollection_CompareOfReal.ixx>

// -----------
// Create :
// -----------
PCollection_CompareOfReal::PCollection_CompareOfReal()
{
}

// -----------
// IsLower :
// -----------
Standard_Boolean PCollection_CompareOfReal::IsLower (
       const Standard_Real &Left,const Standard_Real &Right) const
{
   return (Left < Right) ;
}

// -----------
// IsGreater :
// -----------
Standard_Boolean PCollection_CompareOfReal::IsGreater (
       const Standard_Real &Left,const Standard_Real &Right) const
{
   return (Left > Right) ;
}						





