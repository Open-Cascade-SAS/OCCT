// Created on: 2002-12-12
// Created by: data exchange team
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

#ifndef _StepElement_ElementMaterial_HeaderFile
#define _StepElement_ElementMaterial_HeaderFile

#include <Standard.hxx>

#include <StepRepr_HArray1OfMaterialPropertyRepresentation.hxx>
#include <Standard_Transient.hxx>
class TCollection_HAsciiString;


class StepElement_ElementMaterial;
DEFINE_STANDARD_HANDLE(StepElement_ElementMaterial, Standard_Transient)

//! Representation of STEP entity ElementMaterial
class StepElement_ElementMaterial : public Standard_Transient
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepElement_ElementMaterial();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aMaterialId, const Handle(TCollection_HAsciiString)& aDescription, const Handle(StepRepr_HArray1OfMaterialPropertyRepresentation)& aProperties);
  
  //! Returns field MaterialId
  Standard_EXPORT Handle(TCollection_HAsciiString) MaterialId() const;
  
  //! Set field MaterialId
  Standard_EXPORT void SetMaterialId (const Handle(TCollection_HAsciiString)& MaterialId);
  
  //! Returns field Description
  Standard_EXPORT Handle(TCollection_HAsciiString) Description() const;
  
  //! Set field Description
  Standard_EXPORT void SetDescription (const Handle(TCollection_HAsciiString)& Description);
  
  //! Returns field Properties
  Standard_EXPORT Handle(StepRepr_HArray1OfMaterialPropertyRepresentation) Properties() const;
  
  //! Set field Properties
  Standard_EXPORT void SetProperties (const Handle(StepRepr_HArray1OfMaterialPropertyRepresentation)& Properties);




  DEFINE_STANDARD_RTTIEXT(StepElement_ElementMaterial,Standard_Transient)

protected:




private:


  Handle(TCollection_HAsciiString) theMaterialId;
  Handle(TCollection_HAsciiString) theDescription;
  Handle(StepRepr_HArray1OfMaterialPropertyRepresentation) theProperties;


};







#endif // _StepElement_ElementMaterial_HeaderFile
