// File:	BOPTools_SSIntersectionAttribute.cxx
// Created:	Mon Mar  4 12:05:23 2002
// Author:	Michael KLOKOV
//		<mkk@kurox>


#include <BOPTools_SSIntersectionAttribute.ixx>
//=======================================================================
// function: BOPTools_SSIntersectionAttribute
// purpose: 
//=======================================================================
BOPTools_SSIntersectionAttribute::BOPTools_SSIntersectionAttribute(const Standard_Boolean Aproximation,
								   const Standard_Boolean PCurveOnS1,
								   const Standard_Boolean PCurveOnS2)
{
  myApproximation = Aproximation;
  myPCurve1 = PCurveOnS1;
  myPCurve2 = PCurveOnS2;
}
//=======================================================================
// function: Approximation
// purpose: 
//=======================================================================
void BOPTools_SSIntersectionAttribute::Approximation(const Standard_Boolean theFlag) 
{
  myApproximation = theFlag;
}
//=======================================================================
// function: PCurveOnS1
// purpose: 
//=======================================================================
void BOPTools_SSIntersectionAttribute::PCurveOnS1(const Standard_Boolean theFlag) 
{
  myPCurve1 = theFlag;
}
//=======================================================================
// function: PCurveOnS2
// purpose: 
//=======================================================================
void BOPTools_SSIntersectionAttribute::PCurveOnS2(const Standard_Boolean theFlag) 
{
  myPCurve2 = theFlag;
}

