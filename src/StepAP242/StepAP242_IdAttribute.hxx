// Created on: 2015-07-10
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

#ifndef _StepAP242_IdAttribute_HeaderFile
#define _StepAP242_IdAttribute_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepAP242_IdAttributeSelect.hxx>
#include <Standard_Transient.hxx>
class TCollection_HAsciiString;
class StepAP242_IdAttributeSelect;

class StepAP242_IdAttribute;
DEFINE_STANDARD_HANDLE(StepAP242_IdAttribute, Standard_Transient)
class StepAP242_IdAttribute : public Standard_Transient
{

public:
  
  //! Returns a IdAttribute
  Standard_EXPORT StepAP242_IdAttribute();
  
  //! Init all field own and inherited
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& theAttributeValue,
                             const StepAP242_IdAttributeSelect& theIdentifiedItem);
  
  // Set field AttributeValue
  inline void SetAttributeValue(const Handle(TCollection_HAsciiString)& theAttributeValue)
  {
    attributeValue = theAttributeValue;
  }
  
  //! Returns field AttributeValue
  inline Handle(TCollection_HAsciiString) AttributeValue() const
  {
    return attributeValue;
  }
  
  //! Set field IdentifiedItem
  inline void SetIdentifiedItem(const StepAP242_IdAttributeSelect& theIdentifiedItem)
  {
    identifiedItem = theIdentifiedItem;
  }
  
  //! Returns IdentifiedItem
  inline StepAP242_IdAttributeSelect IdentifiedItem() const
  {
    return identifiedItem;
  }

  DEFINE_STANDARD_RTTIEXT(StepAP242_IdAttribute,Standard_Transient)

private: 
  Handle(TCollection_HAsciiString) attributeValue;
  StepAP242_IdAttributeSelect identifiedItem;
};
#endif // _StepAP242_IdAttribute_HeaderFile
