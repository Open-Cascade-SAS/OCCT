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

#ifndef _StepAP242_ItemIdentifiedRepresentationUsage_HeaderFile
#define _StepAP242_ItemIdentifiedRepresentationUsage_HeaderFile

#include <Standard.hxx>

#include <Standard_Transient.hxx>
#include <StepAP242_ItemIdentifiedRepresentationUsageDefinition.hxx>
#include <Standard_Integer.hxx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>

class TCollection_HAsciiString;
class StepRepr_Representation;
class StepRepr_RepresentationItem;

class StepAP242_ItemIdentifiedRepresentationUsage;
DEFINE_STANDARD_HANDLE(StepAP242_ItemIdentifiedRepresentationUsage, Standard_Transient)
class StepAP242_ItemIdentifiedRepresentationUsage : public Standard_Transient
{

public:
  
  //! Returns a ItemIdentifiedRepresentationUsage
  Standard_EXPORT StepAP242_ItemIdentifiedRepresentationUsage();
  
  //! Init all fields own and inherited
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& theName,
                             const Handle(TCollection_HAsciiString)& theDescription,
                             const StepAP242_ItemIdentifiedRepresentationUsageDefinition& theDefinition,
                             const Handle(StepRepr_Representation)& theUsedRepresentation,
                             const Handle(StepRepr_HArray1OfRepresentationItem)& theIdentifiedItem);
  
  //! Set field Name
  inline void SetName(const Handle(TCollection_HAsciiString)& theName)
  {
    name = theName;
  }
  
  //! Returns field Name
  inline Handle(TCollection_HAsciiString) Name() const
  {
    return name;
  }
  
  //! Set field Description
  inline void SetDescription(const Handle(TCollection_HAsciiString)& theDescription)
  {
    description = theDescription;
  }
  
  //! Returns field Description
  inline Handle(TCollection_HAsciiString) Description() const
  {
    return description;
  }
  
  //! Set field Definition
  inline void SetDefinition(const StepAP242_ItemIdentifiedRepresentationUsageDefinition& theDefinition)
  {
    definition = theDefinition;
  }
  
  //! Returns field Definition
  inline StepAP242_ItemIdentifiedRepresentationUsageDefinition Definition() const
  {
    return definition;
  }
  
  //! Set field UsedRepresentation
  inline void SetUsedRepresentation(const Handle(StepRepr_Representation)& theUsedRepresentation)
  {
    usedRepresentation = theUsedRepresentation;
  }
  
  //! Returns field UsedRepresentation
  inline Handle(StepRepr_Representation) UsedRepresentation() const
  {
    return usedRepresentation;
  }
  
  //! Returns field IdentifiedItem
  inline Handle(StepRepr_HArray1OfRepresentationItem) IdentifiedItem () const
  {  
    return identifiedItem;
  }
  
  //! Returns number of identified items
  inline Standard_Integer  NbIdentifiedItem () const
  {  
    return (identifiedItem.IsNull() ? 0 : identifiedItem->Length());
  }
  
  //! Set fiels IdentifiedItem
  inline void SetIdentifiedItem (const Handle(StepRepr_HArray1OfRepresentationItem)& theIdentifiedItem)
  {  
    identifiedItem = theIdentifiedItem;
  }
  
  //! Returns identified item with given number
  inline Handle(StepRepr_RepresentationItem) IdentifiedItemValue
    (const Standard_Integer num) const
  {  
    return identifiedItem->Value(num);
  }
  
  //! Set identified item with given number
  inline void SetIdentifiedItemValue (const Standard_Integer num, const Handle(StepRepr_RepresentationItem)& theItem)
  {  
    identifiedItem->SetValue (num, theItem);
  }

  DEFINE_STANDARD_RTTIEXT(StepAP242_ItemIdentifiedRepresentationUsage,Standard_Transient)

private: 
  Handle(TCollection_HAsciiString) name;
  Handle(TCollection_HAsciiString) description;
  StepAP242_ItemIdentifiedRepresentationUsageDefinition definition;
  Handle(StepRepr_Representation) usedRepresentation;
  Handle(StepRepr_HArray1OfRepresentationItem) identifiedItem;
};
#endif // _StepAP242_ItemIdentifiedRepresentationUsage_HeaderFile
