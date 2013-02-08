// Created on: 2002-03-04
// Created by: Michael KLOKOV
// Copyright (c) 2002-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <BOPAlgo_SectionAttribute.ixx>
//=======================================================================
// function: BOPAlgo_SectionAttribute
// purpose: 
//=======================================================================
BOPAlgo_SectionAttribute::BOPAlgo_SectionAttribute(const Standard_Boolean Aproximation,
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
void BOPAlgo_SectionAttribute::Approximation(const Standard_Boolean theFlag) 
{
  myApproximation = theFlag;
}
//=======================================================================
// function: PCurveOnS1
// purpose: 
//=======================================================================
void BOPAlgo_SectionAttribute::PCurveOnS1(const Standard_Boolean theFlag) 
{
  myPCurve1 = theFlag;
}
//=======================================================================
// function: PCurveOnS2
// purpose: 
//=======================================================================
void BOPAlgo_SectionAttribute::PCurveOnS2(const Standard_Boolean theFlag) 
{
  myPCurve2 = theFlag;
}

