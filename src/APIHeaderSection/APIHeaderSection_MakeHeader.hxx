// Created on: 1993-08-12
// Created by: Frederic MAUPAS
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _APIHeaderSection_MakeHeader_HeaderFile
#define _APIHeaderSection_MakeHeader_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
class HeaderSection_FileName;
class HeaderSection_FileSchema;
class HeaderSection_FileDescription;
class StepData_StepModel;
class Interface_Protocol;
class TCollection_HAsciiString;

//! This class allows to consult and prepare/edit  data stored in
//! a Step Model  Header
class APIHeaderSection_MakeHeader
{
public:
  DEFINE_STANDARD_ALLOC

  //! Prepares a new MakeHeader from scratch
  Standard_EXPORT APIHeaderSection_MakeHeader(const Standard_Integer shapetype = 0);

  //! Prepares a MakeHeader from the content of a StepModel
  //! See IsDone to know if the Header is well defined
  Standard_EXPORT APIHeaderSection_MakeHeader(const Handle(StepData_StepModel)& model);

  //! Cancels the former definition and gives a FileName
  //! To be used when a Model has no well defined Header
  Standard_EXPORT void Init(const Standard_CString nameval);

  //! Returns True if all data have been defined (see also
  //! HasFn, HasFs, HasFd)
  Standard_EXPORT Standard_Boolean IsDone() const;

  //! Creates an empty header for a new
  //! STEP model and allows the header fields to be completed.
  Standard_EXPORT void Apply(const Handle(StepData_StepModel)& model) const;

  //! Builds a Header, creates a new StepModel, then applies the
  //! Header to the StepModel
  //! The Schema Name is taken from the Protocol (if it inherits
  //! from StepData, else it is left in blanks)
  Standard_EXPORT Handle(StepData_StepModel) NewModel(
    const Handle(Interface_Protocol)& protocol) const;

  //! Checks whether there is a
  //! file_name entity. Returns True if there is one.
  Standard_EXPORT Standard_Boolean HasFn() const;

  //! Returns the file_name entity.
  //! Returns an empty entity if the file_name entity is not initialized.
  Standard_EXPORT Handle(HeaderSection_FileName) FnValue() const;

  Standard_EXPORT void SetName(const Handle(TCollection_HAsciiString)& aName);

  //! Returns the name attribute for the file_name entity.
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;

  Standard_EXPORT void SetTimeStamp(const Handle(TCollection_HAsciiString)& aTimeStamp);

  //! Returns the value of the time_stamp attribute for the file_name entity.
  Standard_EXPORT Handle(TCollection_HAsciiString) TimeStamp() const;

  Standard_EXPORT void SetAuthor(const Handle(Interface_HArray1OfHAsciiString)& aAuthor);

  Standard_EXPORT void SetAuthorValue(const Standard_Integer                  num,
                                      const Handle(TCollection_HAsciiString)& aAuthor);

  Standard_EXPORT Handle(Interface_HArray1OfHAsciiString) Author() const;

  //! Returns the value of the name attribute for the file_name entity.
  Standard_EXPORT Handle(TCollection_HAsciiString) AuthorValue(const Standard_Integer num) const;

  //! Returns the number of values for the author attribute in the file_name entity.
  Standard_EXPORT Standard_Integer NbAuthor() const;

  Standard_EXPORT void SetOrganization(
    const Handle(Interface_HArray1OfHAsciiString)& aOrganization);

  Standard_EXPORT void SetOrganizationValue(const Standard_Integer                  num,
                                            const Handle(TCollection_HAsciiString)& aOrganization);

  Standard_EXPORT Handle(Interface_HArray1OfHAsciiString) Organization() const;

  //! Returns the value of attribute
  //! organization for the file_name entity.
  Standard_EXPORT Handle(TCollection_HAsciiString) OrganizationValue(
    const Standard_Integer num) const;

  //! Returns the number of values for
  //! the organization attribute in the file_name entity.
  Standard_EXPORT Standard_Integer NbOrganization() const;

  Standard_EXPORT void SetPreprocessorVersion(
    const Handle(TCollection_HAsciiString)& aPreprocessorVersion);

  //! Returns the name of the preprocessor_version for the file_name entity.
  Standard_EXPORT Handle(TCollection_HAsciiString) PreprocessorVersion() const;

  Standard_EXPORT void SetOriginatingSystem(
    const Handle(TCollection_HAsciiString)& aOriginatingSystem);

  Standard_EXPORT Handle(TCollection_HAsciiString) OriginatingSystem() const;

  Standard_EXPORT void SetAuthorisation(const Handle(TCollection_HAsciiString)& aAuthorisation);

  //! Returns the value of the authorization attribute for the file_name entity.
  Standard_EXPORT Handle(TCollection_HAsciiString) Authorisation() const;

  //! Checks whether there is a file_schema entity. Returns True if there is one.
  Standard_EXPORT Standard_Boolean HasFs() const;

  //! Returns the file_schema entity. Returns an empty entity if the file_schema entity is not
  //! initialized.
  Standard_EXPORT Handle(HeaderSection_FileSchema) FsValue() const;

  Standard_EXPORT void SetSchemaIdentifiers(
    const Handle(Interface_HArray1OfHAsciiString)& aSchemaIdentifiers);

  Standard_EXPORT void SetSchemaIdentifiersValue(
    const Standard_Integer                  num,
    const Handle(TCollection_HAsciiString)& aSchemaIdentifier);

  Standard_EXPORT Handle(Interface_HArray1OfHAsciiString) SchemaIdentifiers() const;

  //! Returns the value of the schema_identifier attribute for the file_schema entity.
  Standard_EXPORT Handle(TCollection_HAsciiString) SchemaIdentifiersValue(
    const Standard_Integer num) const;

  //! Returns the number of values for the schema_identifier attribute in the file_schema entity.
  Standard_EXPORT Standard_Integer NbSchemaIdentifiers() const;

  //! Add a subname of schema (if not yet in the list)
  Standard_EXPORT void AddSchemaIdentifier(
    const Handle(TCollection_HAsciiString)& aSchemaIdentifier);

  //! Checks whether there is a file_description entity. Returns True if there is one.
  Standard_EXPORT Standard_Boolean HasFd() const;

  //! Returns the file_description
  //! entity. Returns an empty entity if the file_description entity is not initialized.
  Standard_EXPORT Handle(HeaderSection_FileDescription) FdValue() const;

  Standard_EXPORT void SetDescription(const Handle(Interface_HArray1OfHAsciiString)& aDescription);

  Standard_EXPORT void SetDescriptionValue(const Standard_Integer                  num,
                                           const Handle(TCollection_HAsciiString)& aDescription);

  Standard_EXPORT Handle(Interface_HArray1OfHAsciiString) Description() const;

  //! Returns the value of the
  //! description attribute for the file_description entity.
  Standard_EXPORT Handle(TCollection_HAsciiString) DescriptionValue(
    const Standard_Integer num) const;

  //! Returns the number of values for
  //! the file_description entity in the STEP file header.
  Standard_EXPORT Standard_Integer NbDescription() const;

  Standard_EXPORT void SetImplementationLevel(
    const Handle(TCollection_HAsciiString)& aImplementationLevel);

  //! Returns the value of the
  //! implementation_level attribute for the file_description entity.
  Standard_EXPORT Handle(TCollection_HAsciiString) ImplementationLevel() const;

protected:
private:
  Standard_Boolean                      done;
  Handle(HeaderSection_FileName)        fn;
  Handle(HeaderSection_FileSchema)      fs;
  Handle(HeaderSection_FileDescription) fd;
};

#endif // _APIHeaderSection_MakeHeader_HeaderFile
