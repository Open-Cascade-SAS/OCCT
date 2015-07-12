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

#ifndef _BOPAlgo_SectionAttribute_HeaderFile
#define _BOPAlgo_SectionAttribute_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>


//! Class is a container of three flags used
//! by intersection algorithm
class BOPAlgo_SectionAttribute 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! Initializes me by flags
  Standard_EXPORT BOPAlgo_SectionAttribute(const Standard_Boolean Aproximation = Standard_True, const Standard_Boolean PCurveOnS1 = Standard_True, const Standard_Boolean PCurveOnS2 = Standard_True);
  

  //! Modifier
  Standard_EXPORT void Approximation (const Standard_Boolean theFlag);
  

  //! Modifier
  Standard_EXPORT void PCurveOnS1 (const Standard_Boolean theFlag);
  

  //! Modifier
  Standard_EXPORT void PCurveOnS2 (const Standard_Boolean theFlag);
  

  //! Selector
    Standard_Boolean Approximation() const;
  

  //! Selector
    Standard_Boolean PCurveOnS1() const;
  

  //! Selector
    Standard_Boolean PCurveOnS2() const;




protected:





private:



  Standard_Boolean myApproximation;
  Standard_Boolean myPCurve1;
  Standard_Boolean myPCurve2;


};


#include <BOPAlgo_SectionAttribute.lxx>





#endif // _BOPAlgo_SectionAttribute_HeaderFile
