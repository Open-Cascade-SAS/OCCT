// Created on: 1992-02-11
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _StepData_StepModel_HeaderFile
#define _StepData_StepModel_HeaderFile

#include <Interface_EntityList.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Resource_FormatType.hxx>
#include <StepData_Factors.hxx>
#include <DESTEP_Parameters.hxx>

class Standard_Transient;
class Interface_EntityIterator;
class Interface_Check;
class TCollection_HAsciiString;

class StepData_StepModel;
DEFINE_STANDARD_HANDLE(StepData_StepModel, Interface_InterfaceModel)

//! Gives access to
//! - entities in a STEP file,
//! - the STEP file header.
class StepData_StepModel : public Interface_InterfaceModel
{

public:
  //! Creates an empty STEP model with an empty header.
  Standard_EXPORT StepData_StepModel();

  //! returns entity given its rank.
  //! Same as InterfaceEntity, but with a shorter name
  Standard_EXPORT Handle(Standard_Transient) Entity(const Standard_Integer num) const;

  //! gets header from another Model (uses Header Protocol)
  Standard_EXPORT void GetFromAnother(const Handle(Interface_InterfaceModel)& other)
    Standard_OVERRIDE;

  //! Returns a New Empty Model, same type as <me>, i.e. StepModel
  Standard_EXPORT Handle(Interface_InterfaceModel) NewEmptyModel() const Standard_OVERRIDE;

  //! returns Header entities under the form of an iterator
  Standard_EXPORT Interface_EntityIterator Header() const;

  //! says if a Header entity has a specified type
  Standard_EXPORT Standard_Boolean HasHeaderEntity(const Handle(Standard_Type)& atype) const;

  //! Returns Header entity with specified type, if there is
  Standard_EXPORT Handle(Standard_Transient) HeaderEntity(const Handle(Standard_Type)& atype) const;

  //! Clears the Header
  Standard_EXPORT void ClearHeader() Standard_OVERRIDE;

  //! Adds an Entity to the Header
  Standard_EXPORT void AddHeaderEntity(const Handle(Standard_Transient)& ent);

  //! Specific Check, checks Header Items with HeaderProtocol
  Standard_EXPORT virtual void VerifyCheck(Handle(Interface_Check)& ach) const Standard_OVERRIDE;

  //! Dumps the Header, with the Header Protocol of StepData.
  //! If the Header Protocol is not defined, for each Header Entity,
  //! prints its Type. Else sends the Header under the form of
  //! HEADER Section of an Ascii Step File
  //! <level> is not used because Header is not so big
  Standard_EXPORT void DumpHeader(Standard_OStream&      S,
                                  const Standard_Integer level = 0) const Standard_OVERRIDE;

  //! erases specific labels, i.e. clears the map (entity-ident)
  Standard_EXPORT void ClearLabels() Standard_OVERRIDE;

  //! Attaches an ident to an entity to produce a label
  //! (does nothing if <ent> is not in <me>)
  Standard_EXPORT void SetIdentLabel(const Handle(Standard_Transient)& ent,
                                     const Standard_Integer            ident);

  //! returns the label ident attached to an entity, 0 if not in me
  Standard_EXPORT Standard_Integer IdentLabel(const Handle(Standard_Transient)& ent) const;

  //! Prints label specific to STEP norm for a given entity, i.e.
  //! if a LabelIdent has been recorded, its value with '#', else
  //! the number in the model with '#' and between ()
  Standard_EXPORT void PrintLabel(const Handle(Standard_Transient)& ent,
                                  Standard_OStream&                 S) const Standard_OVERRIDE;

  //! Returns a string with the label attached to a given entity,
  //! same form as for PrintLabel
  Standard_EXPORT Handle(TCollection_HAsciiString) StringLabel(
    const Handle(Standard_Transient)& ent) const Standard_OVERRIDE;

  //! Return the encoding of STEP file for converting names into UNICODE.
  //! Initialized from "read.step.codepage" variable by constructor, which is Resource_UTF8 by
  //! default.
  Resource_FormatType SourceCodePage() const { return InternalParameters.ReadCodePage; }

  //! Return the encoding of STEP file for converting names into UNICODE.
  void SetSourceCodePage(Resource_FormatType theCode) { InternalParameters.ReadCodePage = theCode; }

  //! Sets local length unit using for transfer process
  Standard_EXPORT void SetLocalLengthUnit(const Standard_Real theUnit);

  //! Returns local length unit using for transfer process (1 by default)
  Standard_EXPORT Standard_Real LocalLengthUnit() const;

  //! Sets length unit using for writing process
  Standard_EXPORT void SetWriteLengthUnit(const Standard_Real theUnit);

  //! Returns length unit using for writing process (1 by default)
  Standard_EXPORT Standard_Real WriteLengthUnit() const;

  //! Returns the unit initialization flag
  //! True - the unit was initialized
  //! False - the unit value was not initialized, the default value is used
  Standard_Boolean IsInitializedUnit() const { return myReadUnitIsInitialized; }

public:
  DESTEP_Parameters InternalParameters;

  DEFINE_STANDARD_RTTIEXT(StepData_StepModel, Interface_InterfaceModel)

private:
  Interface_EntityList             theheader;
  Handle(TColStd_HArray1OfInteger) theidnums;
  mutable Standard_Real            myWriteUnit              = 1.0;
  Standard_Real                    myLocalLengthUnit        = 1.0;
  Standard_Boolean                 myReadUnitIsInitialized  = false;
  mutable Standard_Boolean         myWriteUnitIsInitialized = false;
};

#endif // _StepData_StepModel_HeaderFile
