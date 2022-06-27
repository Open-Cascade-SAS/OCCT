// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
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

#ifndef _StepRepr_ProductConcept_HeaderFile
#define _StepRepr_ProductConcept_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class TCollection_HAsciiString;
class StepBasic_ProductConceptContext;


class StepRepr_ProductConcept;
DEFINE_STANDARD_HANDLE(StepRepr_ProductConcept, Standard_Transient)

//! Representation of STEP entity ProductConcept
class StepRepr_ProductConcept : public Standard_Transient
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepRepr_ProductConcept();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aId, const Handle(TCollection_HAsciiString)& aName, const Standard_Boolean hasDescription, const Handle(TCollection_HAsciiString)& aDescription, const Handle(StepBasic_ProductConceptContext)& aMarketContext);
  
  //! Returns field Id
  Standard_EXPORT Handle(TCollection_HAsciiString) Id() const;
  
  //! Set field Id
  Standard_EXPORT void SetId (const Handle(TCollection_HAsciiString)& Id);
  
  //! Returns field Name
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;
  
  //! Set field Name
  Standard_EXPORT void SetName (const Handle(TCollection_HAsciiString)& Name);
  
  //! Returns field Description
  Standard_EXPORT Handle(TCollection_HAsciiString) Description() const;
  
  //! Set field Description
  Standard_EXPORT void SetDescription (const Handle(TCollection_HAsciiString)& Description);
  
  //! Returns True if optional field Description is defined
  Standard_EXPORT Standard_Boolean HasDescription() const;
  
  //! Returns field MarketContext
  Standard_EXPORT Handle(StepBasic_ProductConceptContext) MarketContext() const;
  
  //! Set field MarketContext
  Standard_EXPORT void SetMarketContext (const Handle(StepBasic_ProductConceptContext)& MarketContext);




  DEFINE_STANDARD_RTTIEXT(StepRepr_ProductConcept,Standard_Transient)

protected:




private:


  Handle(TCollection_HAsciiString) theId;
  Handle(TCollection_HAsciiString) theName;
  Handle(TCollection_HAsciiString) theDescription;
  Handle(StepBasic_ProductConceptContext) theMarketContext;
  Standard_Boolean defDescription;


};







#endif // _StepRepr_ProductConcept_HeaderFile
