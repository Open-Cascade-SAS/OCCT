// Created on: 2000-01-19
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _XSAlgo_HeaderFile
#define _XSAlgo_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class XSAlgo_AlgoContainer;



class XSAlgo 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Provides initerface to the algorithms from Shape Healing
  //! and others for XSTEP processors.
  //! Creates and initializes default AlgoContainer.
  Standard_EXPORT static void Init();
  
  //! Sets default AlgoContainer
  Standard_EXPORT static void SetAlgoContainer (const Handle(XSAlgo_AlgoContainer)& aContainer);
  
  //! Returns default AlgoContainer
  Standard_EXPORT static Handle(XSAlgo_AlgoContainer) AlgoContainer();

};

#endif // _XSAlgo_HeaderFile
