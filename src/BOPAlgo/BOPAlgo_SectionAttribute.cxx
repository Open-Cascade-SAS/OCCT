// Created on: 2002-03-04
// Created by: Michael KLOKOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <BOPAlgo_SectionAttribute.hxx>

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

