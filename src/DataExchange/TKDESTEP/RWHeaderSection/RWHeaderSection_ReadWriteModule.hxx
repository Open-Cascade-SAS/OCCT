// Created on: 1994-06-16
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _RWHeaderSection_ReadWriteModule_HeaderFile
#define _RWHeaderSection_ReadWriteModule_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepData_ReadWriteModule.hxx>
#include <Standard_Integer.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_Sequence.hxx>

#include <string_view>

class TCollection_AsciiString;
class StepData_StepReaderData;
class Interface_Check;
class Standard_Transient;
class StepData_StepWriter;

//! General module to read and write HeaderSection entities
class RWHeaderSection_ReadWriteModule : public StepData_ReadWriteModule
{

public:
  Standard_EXPORT RWHeaderSection_ReadWriteModule();

  //! associates a positive Case Number to each type of HeaderSection entity,
  //! given as a String defined in the EXPRESS form
  Standard_EXPORT int
    CaseStep(const TCollection_AsciiString& atype) const override;

  //! associates a positive Case Number to each type of HeaderSection Complex entity,
  //! given as a String defined in the EXPRESS form
  Standard_EXPORT virtual int CaseStep(
    const NCollection_Sequence<TCollection_AsciiString>& types) const override;

  //! returns True if the Case Number corresponds to a Complex Type
  Standard_EXPORT virtual bool IsComplex(const int CN) const
    override;

  //! returns a StepType (defined in EXPRESS form which belongs to a
  //! Type of Entity, identified by its CaseNumber determined by Protocol
  Standard_EXPORT const std::string_view& StepType(const int CN) const
    override;

  Standard_EXPORT void ReadStep(const int                 CN,
                                const occ::handle<StepData_StepReaderData>& data,
                                const int                 num,
                                occ::handle<Interface_Check>&               ach,
                                const occ::handle<Standard_Transient>&      ent) const override;

  Standard_EXPORT void WriteStep(const int            CN,
                                 StepData_StepWriter&              SW,
                                 const occ::handle<Standard_Transient>& ent) const override;

  DEFINE_STANDARD_RTTIEXT(RWHeaderSection_ReadWriteModule, StepData_ReadWriteModule)

};

#endif // _RWHeaderSection_ReadWriteModule_HeaderFile
