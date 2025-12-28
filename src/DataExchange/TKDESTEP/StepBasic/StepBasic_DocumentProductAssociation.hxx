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

//! Representation of STEP entity DocumentProductAssociation
class StepBasic_DocumentProductAssociation : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT StepBasic_DocumentProductAssociation();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>&         aName,
                            const bool                          hasDescription,
                            const occ::handle<TCollection_HAsciiString>&         aDescription,
                            const occ::handle<StepBasic_Document>&               aRelatingDocument,
                            const StepBasic_ProductOrFormationOrDefinition& aRelatedProduct);

  //! Returns field Name
  Standard_EXPORT occ::handle<TCollection_HAsciiString> Name() const;

  //! Set field Name
  Standard_EXPORT void SetName(const occ::handle<TCollection_HAsciiString>& Name);

  //! Returns field Description
  Standard_EXPORT occ::handle<TCollection_HAsciiString> Description() const;

  //! Set field Description
  Standard_EXPORT void SetDescription(const occ::handle<TCollection_HAsciiString>& Description);

  //! Returns True if optional field Description is defined
  Standard_EXPORT bool HasDescription() const;

  //! Returns field RelatingDocument
  Standard_EXPORT occ::handle<StepBasic_Document> RelatingDocument() const;

  //! Set field RelatingDocument
  Standard_EXPORT void SetRelatingDocument(const occ::handle<StepBasic_Document>& RelatingDocument);

  //! Returns field RelatedProduct
  Standard_EXPORT StepBasic_ProductOrFormationOrDefinition RelatedProduct() const;

  //! Set field RelatedProduct
  Standard_EXPORT void SetRelatedProduct(
    const StepBasic_ProductOrFormationOrDefinition& RelatedProduct);

  DEFINE_STANDARD_RTTIEXT(StepBasic_DocumentProductAssociation, Standard_Transient)

private:
  occ::handle<TCollection_HAsciiString>         theName;
  occ::handle<TCollection_HAsciiString>         theDescription;
  occ::handle<StepBasic_Document>               theRelatingDocument;
  StepBasic_ProductOrFormationOrDefinition theRelatedProduct;
  bool                         defDescription;
};

#endif // _StepBasic_DocumentProductAssociation_HeaderFile
