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

#include <Interface_DataMapOfTransientInteger.hxx>
#include <Interface_Protocol.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
class Interface_InterfaceModel;
class StepData_EDescr;
class StepData_ESDescr;
class StepData_ECDescr;
class StepData_PDescr;


class StepData_Protocol;
DEFINE_STANDARD_HANDLE(StepData_Protocol, Interface_Protocol)

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
  Standard_EXPORT Standard_Integer NbResources() const Standard_OVERRIDE;
  
  //! Returns a Resource, given a rank. Here, none
  Standard_EXPORT Handle(Interface_Protocol) Resource (const Standard_Integer num) const Standard_OVERRIDE;
  
  //! Returns a unique positive number for any recognized entity
  //! Redefined to work by calling both TypeNumber and, for a
  //! Described Entity (late binding) DescrNumber
  Standard_EXPORT virtual Standard_Integer CaseNumber (const Handle(Standard_Transient)& obj) const Standard_OVERRIDE;
  
  //! Returns a Case Number, specific of each recognized Type
  //! Here, only Unknown Entity is recognized
  Standard_EXPORT Standard_Integer TypeNumber (const Handle(Standard_Type)& atype) const Standard_OVERRIDE;
  
  //! Returns the Schema Name attached to each class of Protocol
  //! To be redefined by each sub-class
  //! Here, SchemaName returns "(DEFAULT)"
  //! was C++ : return const
  Standard_EXPORT virtual Standard_CString SchemaName() const;
  
  //! Creates an empty Model for Step Norm
  Standard_EXPORT Handle(Interface_InterfaceModel) NewModel() const Standard_OVERRIDE;
  
  //! Returns True if <model> is a Model of Step Norm
  Standard_EXPORT Standard_Boolean IsSuitableModel (const Handle(Interface_InterfaceModel)& model) const Standard_OVERRIDE;
  
  //! Creates a new Unknown Entity for Step (UndefinedEntity)
  Standard_EXPORT Handle(Standard_Transient) UnknownEntity() const Standard_OVERRIDE;
  
  //! Returns True if <ent> is an Unknown Entity for the Norm, i.e.
  //! Type UndefinedEntity, status Unknown
  Standard_EXPORT Standard_Boolean IsUnknownEntity (const Handle(Standard_Transient)& ent) const Standard_OVERRIDE;
  
  //! Returns a unique positive CaseNumber for types described by
  //! an EDescr (late binding)
  //! Warning : TypeNumber and DescrNumber must give together a unique
  //! positive case number for each distinct case, type or descr
  Standard_EXPORT virtual Standard_Integer DescrNumber (const Handle(StepData_EDescr)& adescr) const;
  
  //! Records an EDescr with its case number
  //! Also records its name for an ESDescr (simple type): an ESDescr
  //! is then used, for case number, or for type name
  Standard_EXPORT void AddDescr (const Handle(StepData_EDescr)& adescr, const Standard_Integer CN);
  
  //! Tells if a Protocol brings at least one ESDescr, i.e. if it
  //! defines at least one entity description by ESDescr mechanism
  Standard_EXPORT Standard_Boolean HasDescr() const;
  
  //! Returns the description attached to a case number, or null
  Standard_EXPORT Handle(StepData_EDescr) Descr (const Standard_Integer num) const;
  
  //! Returns a description according to its name
  //! <anylevel> True (D) : for <me> and its resources
  //! <anylevel> False : for <me> only
  Standard_EXPORT Handle(StepData_EDescr) Descr (const Standard_CString name, const Standard_Boolean anylevel = Standard_True) const;
  
  //! Idem as Descr but cast to simple description
  Standard_EXPORT Handle(StepData_ESDescr) ESDescr (const Standard_CString name, const Standard_Boolean anylevel = Standard_True) const;
  
  //! Returns a complex description according to list of names
  //! <anylevel> True (D) : for <me> and its resources
  //! <anylevel> False : for <me> only
  Standard_EXPORT Handle(StepData_ECDescr) ECDescr (const TColStd_SequenceOfAsciiString& names, const Standard_Boolean anylevel = Standard_True) const;
  
  //! Records an PDescr
  Standard_EXPORT void AddPDescr (const Handle(StepData_PDescr)& pdescr);
  
  //! Returns a parameter description according to its name
  //! <anylevel> True (D) : for <me> and its resources
  //! <anylevel> False : for <me> only
  Standard_EXPORT Handle(StepData_PDescr) PDescr (const Standard_CString name, const Standard_Boolean anylevel = Standard_True) const;
  
  //! Records an ESDescr, intended to build complex descriptions
  Standard_EXPORT void AddBasicDescr (const Handle(StepData_ESDescr)& esdescr);
  
  //! Returns a basic description according to its name
  //! <anylevel> True (D) : for <me> and its resources
  //! <anylevel> False : for <me> only
  Standard_EXPORT Handle(StepData_EDescr) BasicDescr (const Standard_CString name, const Standard_Boolean anylevel = Standard_True) const;




  DEFINE_STANDARD_RTTIEXT(StepData_Protocol,Interface_Protocol)

protected:




private:


  Interface_DataMapOfTransientInteger thedscnum;
  NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)> thedscnam;
  NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)> thepdescr;
  NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)> thedscbas;


};







#endif // _StepData_Protocol_HeaderFile
