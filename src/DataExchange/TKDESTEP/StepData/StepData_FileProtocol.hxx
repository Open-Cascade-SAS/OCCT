// Created on: 1993-07-23
// Created by: Christian CAILLET
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

#ifndef _StepData_FileProtocol_HeaderFile
#define _StepData_FileProtocol_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <StepData_Protocol.hxx>
#include <Standard_Integer.hxx>
class Interface_Protocol;
class Interface_Graph;
class Interface_Check;

//! A FileProtocol is defined as the addition of several already
//! existing Protocols. It corresponds to the definition of a
//! SchemaName with several Names, each one being attached to a
//! specific Protocol. Thus, a File defined with a compound Schema
//! is processed as any other one, once built the equivalent
//! compound Protocol, a FileProtocol
class StepData_FileProtocol : public StepData_Protocol
{

public:
  //! Creates an empty FileProtocol
  Standard_EXPORT StepData_FileProtocol();

  //! Adds a Protocol to the definition list of the FileProtocol
  //! But ensures that each class of Protocol is present only once
  //! in this list
  Standard_EXPORT void Add(const occ::handle<StepData_Protocol>& protocol);

  //! Gives the count of Protocols used as Resource (can be zero)
  //! i.e. the count of Protocol recorded by calling the method Add
  Standard_EXPORT virtual int NbResources() const override;

  //! Returns a Resource, given a rank. Here, rank of calling Add
  Standard_EXPORT virtual occ::handle<Interface_Protocol> Resource(const int num) const
    override;

  //! Returns a Case Number, specific of each recognized Type
  //! Here, NO Type at all is recognized properly : all Types are
  //! recognized by the resources
  Standard_EXPORT virtual int TypeNumber(const occ::handle<Standard_Type>& atype) const
    override;

  //! Calls GlobalCheck for each of its recorded resources
  Standard_EXPORT virtual bool GlobalCheck(const Interface_Graph&   G,
                                                       occ::handle<Interface_Check>& ach) const
    override;

  //! Returns the Schema Name attached to each class of Protocol
  //! To be redefined by each sub-class
  //! Here, SchemaName returns "" (empty String)
  //! was C++ : return const
  Standard_EXPORT virtual const char* SchemaName(
    const occ::handle<Interface_InterfaceModel>& theModel) const override;

  DEFINE_STANDARD_RTTIEXT(StepData_FileProtocol, StepData_Protocol)

private:
  NCollection_Sequence<occ::handle<Standard_Transient>> thecomps;
};

#endif // _StepData_FileProtocol_HeaderFile
