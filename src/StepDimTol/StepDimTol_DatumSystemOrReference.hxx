// Created on: 2015-07-21
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _StepDimTol_DatumSystemOrReference_HeaderFile
#define _StepDimTol_DatumSystemOrReference_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>

class Standard_Transient;
class StepDimTol_DatumSystem;
class StepDimTol_DatumReference;

class StepDimTol_DatumSystemOrReference  : public StepData_SelectType
{
public:

  DEFINE_STANDARD_ALLOC
  
  //! Returns a DatumSystemOrReference select type
  Standard_EXPORT StepDimTol_DatumSystemOrReference();
  
  //! Recognizes a DatumSystemOrReference Kind Entity that is :
  //! 1 -> DatumSystem
  //! 2 -> DatumReference
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum (const Handle(Standard_Transient)& ent)  const;
  
  //! returns Value as a DatumSystem (Null if another type)
  Standard_EXPORT Handle(StepDimTol_DatumSystem) DatumSystem()  const;
  
  //! returns Value as a DatumReference (Null if another type)
  Standard_EXPORT Handle(StepDimTol_DatumReference) DatumReference()  const;
};
#endif // _StepDimTol_DatumSystemOrReference_HeaderFile
