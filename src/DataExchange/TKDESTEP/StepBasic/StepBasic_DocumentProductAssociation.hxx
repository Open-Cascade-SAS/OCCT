// Created on: 2003-01-28
// Created by: data exchange team
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef _StepBasic_DocumentProductAssociation_HeaderFile
#define _StepBasic_DocumentProductAssociation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_ProductOrFormationOrDefinition.hxx>
#include <Standard_Transient.hxx>
class TCollection_HAsciiString;
class StepBasic_Document;

class StepBasic_DocumentProductAssociation;
DEFINE_STANDARD_HANDLE(StepBasic_DocumentProductAssociation, Standard_Transient)

//! Representation of STEP entity DocumentProductAssociation
class StepBasic_DocumentProductAssociation : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT StepBasic_DocumentProductAssociation();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&         aName,
                            const Standard_Boolean                          hasDescription,
                            const Handle(TCollection_HAsciiString)&         aDescription,
                            const Handle(StepBasic_Document)&               aRelatingDocument,
                            const StepBasic_ProductOrFormationOrDefinition& aRelatedProduct);

  //! Returns field Name
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;

  //! Set field Name
  Standard_EXPORT void SetName(const Handle(TCollection_HAsciiString)& Name);

  //! Returns field Description
  Standard_EXPORT Handle(TCollection_HAsciiString) Description() const;

  //! Set field Description
  Standard_EXPORT void SetDescription(const Handle(TCollection_HAsciiString)& Description);

  //! Returns True if optional field Description is defined
  Standard_EXPORT Standard_Boolean HasDescription() const;

  //! Returns field RelatingDocument
  Standard_EXPORT Handle(StepBasic_Document) RelatingDocument() const;

  //! Set field RelatingDocument
  Standard_EXPORT void SetRelatingDocument(const Handle(StepBasic_Document)& RelatingDocument);

  //! Returns field RelatedProduct
  Standard_EXPORT StepBasic_ProductOrFormationOrDefinition RelatedProduct() const;

  //! Set field RelatedProduct
  Standard_EXPORT void SetRelatedProduct(
    const StepBasic_ProductOrFormationOrDefinition& RelatedProduct);

  DEFINE_STANDARD_RTTIEXT(StepBasic_DocumentProductAssociation, Standard_Transient)

protected:
private:
  Handle(TCollection_HAsciiString)         theName;
  Handle(TCollection_HAsciiString)         theDescription;
  Handle(StepBasic_Document)               theRelatingDocument;
  StepBasic_ProductOrFormationOrDefinition theRelatedProduct;
  Standard_Boolean                         defDescription;
};

#endif // _StepBasic_DocumentProductAssociation_HeaderFile
