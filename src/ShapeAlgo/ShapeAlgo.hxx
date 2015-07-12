// Created on: 2000-02-07
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

#ifndef _ShapeAlgo_HeaderFile
#define _ShapeAlgo_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class ShapeAlgo_AlgoContainer;
class ShapeAlgo_ToolContainer;
class ShapeAlgo_AlgoContainer;



class ShapeAlgo 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Provides initerface to the algorithms from Shape Healing.
  //! Creates and initializes default AlgoContainer.
  Standard_EXPORT static void Init();
  
  //! Sets default AlgoContainer
  Standard_EXPORT static void SetAlgoContainer (const Handle(ShapeAlgo_AlgoContainer)& aContainer);
  
  //! Returns default AlgoContainer
  Standard_EXPORT static Handle(ShapeAlgo_AlgoContainer) AlgoContainer();




protected:





private:




friend class ShapeAlgo_ToolContainer;
friend class ShapeAlgo_AlgoContainer;

};







#endif // _ShapeAlgo_HeaderFile
