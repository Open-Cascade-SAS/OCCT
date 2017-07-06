// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _StepRepr_ShapeAspect_HeaderFile
#define _StepRepr_ShapeAspect_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepData_Logical.hxx>
#include <Standard_Transient.hxx>
class TCollection_HAsciiString;
class StepRepr_ProductDefinitionShape;


class StepRepr_ShapeAspect;
DEFINE_STANDARD_HANDLE(StepRepr_ShapeAspect, Standard_Transient)


class StepRepr_ShapeAspect : public Standard_Transient
{

public:

  
  //! Returns a ShapeAspect
  Standard_EXPORT StepRepr_ShapeAspect();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(TCollection_HAsciiString)& aDescription, const Handle(StepRepr_ProductDefinitionShape)& aOfShape, const StepData_Logical aProductDefinitional);
  
  Standard_EXPORT void SetName (const Handle(TCollection_HAsciiString)& aName);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;
  
  Standard_EXPORT void SetDescription (const Handle(TCollection_HAsciiString)& aDescription);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Description() const;
  
  Standard_EXPORT void SetOfShape (const Handle(StepRepr_ProductDefinitionShape)& aOfShape);
  
  Standard_EXPORT Handle(StepRepr_ProductDefinitionShape) OfShape() const;
  
  Standard_EXPORT void SetProductDefinitional (const StepData_Logical aProductDefinitional);
  
  Standard_EXPORT StepData_Logical ProductDefinitional() const;




  DEFINE_STANDARD_RTTIEXT(StepRepr_ShapeAspect,Standard_Transient)

protected:




private:


  Handle(TCollection_HAsciiString) name;
  Handle(TCollection_HAsciiString) description;
  Handle(StepRepr_ProductDefinitionShape) ofShape;
  StepData_Logical productDefinitional;


};







#endif // _StepRepr_ShapeAspect_HeaderFile
