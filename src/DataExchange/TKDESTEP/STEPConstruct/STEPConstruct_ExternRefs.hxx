// Created on: 2000-09-29
// Created by: Andrey BETENEV
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

#ifndef _STEPConstruct_ExternRefs_HeaderFile
#define _STEPConstruct_ExternRefs_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_SequenceOfTransient.hxx>
#include <STEPConstruct_Tool.hxx>
#include <Standard_Integer.hxx>
#include <Standard_CString.hxx>
class StepBasic_ProductRelatedProductCategory;
class StepBasic_DocumentType;
class StepBasic_ProductDefinitionContext;
class StepBasic_ProductContext;
class StepBasic_ApplicationProtocolDefinition;
class XSControl_WorkSession;
class StepBasic_ProductDefinition;
class TCollection_HAsciiString;
class StepAP214_AppliedDocumentReference;
class StepBasic_DocumentFile;

//! Provides a tool for analyzing (reading) and creating (writing)
//! references to external files in STEP
//!
//! It maintains a data structure in the form of sequences
//! of relevant STEP entities (roots), allowing either to create
//! them by convenient API, or load from existing model and
//! investigate
class STEPConstruct_ExternRefs : public STEPConstruct_Tool
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an empty tool
  Standard_EXPORT STEPConstruct_ExternRefs();

  //! Creates a tool and initializes it
  Standard_EXPORT STEPConstruct_ExternRefs(const Handle(XSControl_WorkSession)& WS);

  //! Initializes tool; returns True if succeeded
  Standard_EXPORT Standard_Boolean Init(const Handle(XSControl_WorkSession)& WS);

  //! Clears internal fields (list of defined extern refs)
  Standard_EXPORT void Clear();

  //! Searches current STEP model for external references
  //! and loads them to the internal data structures
  //! NOTE: does not clear data structures before loading
  Standard_EXPORT Standard_Boolean LoadExternRefs();

  //! Returns number of defined extern references
  Standard_EXPORT Standard_Integer NbExternRefs() const;

  //! Returns filename for numth extern reference
  //! Returns Null if FileName is not defined or bad
  Standard_EXPORT Standard_CString FileName(const Standard_Integer num) const;

  //! Returns ProductDefinition to which numth extern reference
  //! is associated.
  //! Returns Null if cannot be detected or if extern reference
  //! is not associated to SDR in a proper way.
  Standard_EXPORT Handle(StepBasic_ProductDefinition) ProdDef(const Standard_Integer num) const;

  //! Returns DocumentFile to which numth extern reference
  //! is associated.
  //! Returns Null if cannot be detected.
  Standard_EXPORT Handle(StepBasic_DocumentFile) DocFile(const Standard_Integer num) const;

  //! Returns format identification string for the extern document
  //! Returns Null handle if format is not defined
  Standard_EXPORT Handle(TCollection_HAsciiString) Format(const Standard_Integer num) const;

  //! Create a new external reference with specified attributes
  //! attached to a given SDR
  //! <format> can be Null string, in that case this information
  //! is not written. Else, it can be "STEP AP214" or "STEP AP203"
  //! Returns index of a new extern ref
  Standard_EXPORT Standard_Integer AddExternRef(const Standard_CString                     filename,
                                                const Handle(StepBasic_ProductDefinition)& PD,
                                                const Standard_CString                     format);

  //! Check (create if it is null) all shared entities for the model
  Standard_EXPORT void checkAP214Shared();

  //! Adds all the currently defined external refs to the model
  //! Returns number of written extern refs
  Standard_EXPORT Standard_Integer WriteExternRefs(const Standard_Integer num) const;

  //! Set the ApplicationProtocolDefinition of the PDM schema
  Standard_EXPORT void SetAP214APD(const Handle(StepBasic_ApplicationProtocolDefinition)& APD);

  //! Returns the ApplicationProtocolDefinition of the PDM schema
  //! NOTE: if not defined then create new APD with new Application Context
  Standard_EXPORT Handle(StepBasic_ApplicationProtocolDefinition) GetAP214APD();

protected:
  //! Create a new additional structure entities and add necessary references
  //! Note: do not refer from ADR to DF directly in AP214 (TRJ11).
  Standard_EXPORT Standard_Boolean
    addAP214ExterRef(const Handle(StepAP214_AppliedDocumentReference)& ADR,
                     const Handle(StepBasic_ProductDefinition)&        PD,
                     const Handle(StepBasic_DocumentFile)&             DF,
                     const Standard_CString                            filename);

private:
  TColStd_SequenceOfTransient                     myAEIAs;
  TColStd_SequenceOfTransient                     myRoles;
  TColStd_SequenceOfTransient                     myFormats;
  TColStd_SequenceOfTransient                     myShapes;
  TColStd_SequenceOfTransient                     myTypes;
  TColStd_SequenceOfInteger                       myIsAP214;
  TColStd_SequenceOfInteger                       myReplaceNum;
  TColStd_SequenceOfTransient                     myDocFiles;
  Handle(StepBasic_ProductRelatedProductCategory) mySharedPRPC;
  Handle(StepBasic_DocumentType)                  mySharedDocType;
  Handle(StepBasic_ProductDefinitionContext)      mySharedPDC;
  Handle(StepBasic_ProductContext)                mySharedPC;
  Handle(StepBasic_ApplicationProtocolDefinition) myAPD;
};

#endif // _STEPConstruct_ExternRefs_HeaderFile
