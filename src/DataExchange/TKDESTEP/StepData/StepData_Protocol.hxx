// Created on: 1993-02-03
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

#ifndef _StepData_Protocol_HeaderFile
#define _StepData_Protocol_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_DataMap.hxx>
#include <Interface_Protocol.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_Sequence.hxx>
class Interface_InterfaceModel;
class StepData_EDescr;
class StepData_ESDescr;
class StepData_ECDescr;
class StepData_PDescr;

//! Description of Basic Protocol for Step
//! The class Protocol from StepData itself describes a default
//! Protocol, which recognizes only UnknownEntities.
//! Sub-classes will redefine CaseNumber and, if necessary,
//! NbResources and Resources.
class StepData_Protocol : public Interface_Protocol
{

public:
  Standard_EXPORT StepData_Protocol();

  //! Gives the count of Protocols used as Resource (can be zero)
  //! Here, No resource
  Standard_EXPORT int NbResources() const override;

  //! Returns a Resource, given a rank. Here, none
  Standard_EXPORT occ::handle<Interface_Protocol> Resource(const int num) const override;

  //! Returns a unique positive number for any recognized entity
  //! Redefined to work by calling both TypeNumber and, for a
  //! Described Entity (late binding) DescrNumber
  Standard_EXPORT virtual int CaseNumber(const occ::handle<Standard_Transient>& obj) const override;

  //! Returns a Case Number, specific of each recognized Type
  //! Here, only Unknown Entity is recognized
  Standard_EXPORT int TypeNumber(const occ::handle<Standard_Type>& atype) const override;

  //! Returns the Schema Name attached to each class of Protocol
  //! To be redefined by each sub-class
  //! Here, SchemaName returns "(DEFAULT)"
  //! was C++ : return const
  Standard_EXPORT virtual const char* SchemaName(
    const occ::handle<Interface_InterfaceModel>& theModel) const;

  //! Creates an empty Model for Step Norm
  Standard_EXPORT occ::handle<Interface_InterfaceModel> NewModel() const override;

  //! Returns True if <model> is a Model of Step Norm
  Standard_EXPORT bool IsSuitableModel(
    const occ::handle<Interface_InterfaceModel>& model) const override;

  //! Creates a new Unknown Entity for Step (UndefinedEntity)
  Standard_EXPORT occ::handle<Standard_Transient> UnknownEntity() const override;

  //! Returns True if <ent> is an Unknown Entity for the Norm, i.e.
  //! Type UndefinedEntity, status Unknown
  Standard_EXPORT bool IsUnknownEntity(const occ::handle<Standard_Transient>& ent) const override;

  //! Returns a unique positive CaseNumber for types described by
  //! an EDescr (late binding)
  //! Warning : TypeNumber and DescrNumber must give together a unique
  //! positive case number for each distinct case, type or descr
  Standard_EXPORT virtual int DescrNumber(const occ::handle<StepData_EDescr>& adescr) const;

  //! Records an EDescr with its case number
  //! Also records its name for an ESDescr (simple type): an ESDescr
  //! is then used, for case number, or for type name
  Standard_EXPORT void AddDescr(const occ::handle<StepData_EDescr>& adescr, const int CN);

  //! Tells if a Protocol brings at least one ESDescr, i.e. if it
  //! defines at least one entity description by ESDescr mechanism
  Standard_EXPORT bool HasDescr() const;

  //! Returns the description attached to a case number, or null
  Standard_EXPORT occ::handle<StepData_EDescr> Descr(const int num) const;

  //! Returns a description according to its name
  //! <anylevel> True (D) : for <me> and its resources
  //! <anylevel> False : for <me> only
  Standard_EXPORT occ::handle<StepData_EDescr> Descr(const char* name,
                                                     const bool  anylevel = true) const;

  //! Idem as Descr but cast to simple description
  Standard_EXPORT occ::handle<StepData_ESDescr> ESDescr(const char* name,
                                                        const bool  anylevel = true) const;

  //! Returns a complex description according to list of names
  //! <anylevel> True (D) : for <me> and its resources
  //! <anylevel> False : for <me> only
  Standard_EXPORT occ::handle<StepData_ECDescr> ECDescr(
    const NCollection_Sequence<TCollection_AsciiString>& names,
    const bool                                           anylevel = true) const;

  //! Records an PDescr
  Standard_EXPORT void AddPDescr(const occ::handle<StepData_PDescr>& pdescr);

  //! Returns a parameter description according to its name
  //! <anylevel> True (D) : for <me> and its resources
  //! <anylevel> False : for <me> only
  Standard_EXPORT occ::handle<StepData_PDescr> PDescr(const char* name,
                                                      const bool  anylevel = true) const;

  //! Records an ESDescr, intended to build complex descriptions
  Standard_EXPORT void AddBasicDescr(const occ::handle<StepData_ESDescr>& esdescr);

  //! Returns a basic description according to its name
  //! <anylevel> True (D) : for <me> and its resources
  //! <anylevel> False : for <me> only
  Standard_EXPORT occ::handle<StepData_EDescr> BasicDescr(const char* name,
                                                          const bool  anylevel = true) const;

  DEFINE_STANDARD_RTTIEXT(StepData_Protocol, Interface_Protocol)

private:
  NCollection_DataMap<occ::handle<Standard_Transient>, int>                     thedscnum;
  NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>> thedscnam;
  NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>> thepdescr;
  NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>> thedscbas;
};

#endif // _StepData_Protocol_HeaderFile
