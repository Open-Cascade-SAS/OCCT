// Created on: 1993-02-02
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

#ifndef _Interface_Protocol_HeaderFile
#define _Interface_Protocol_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class Interface_Graph;
class Interface_Check;
class Interface_InterfaceModel;


class Interface_Protocol;
DEFINE_STANDARD_HANDLE(Interface_Protocol, Standard_Transient)

//! General description of Interface Protocols. A Protocol defines
//! a set of Entity types. This class provides also the notion of
//! Active Protocol, as a working context, defined once then
//! exploited by various Tools and Libraries.
//!
//! It also gives control of type definitions. By default, types
//! are provided by CDL, but specific implementations, or topics
//! like multi-typing, may involve another way
class Interface_Protocol : public Standard_Transient
{

public:

  
  //! Returns the Active Protocol, if defined (else, returns a
  //! Null Handle, which means "no defined active protocol")
  Standard_EXPORT static Handle(Interface_Protocol) Active();
  
  //! Sets a given Protocol to be the Active one (for the users of
  //! Active, see just above). Applies to every sub-type of Protocol
  Standard_EXPORT static void SetActive (const Handle(Interface_Protocol)& aprotocol);
  
  //! Erases the Active Protocol (hence it becomes undefined)
  Standard_EXPORT static void ClearActive();
  
  //! Returns count of Protocol used as Resources (level one)
  Standard_EXPORT virtual Standard_Integer NbResources() const = 0;
  
  //! Returns a Resource, given its rank (between 1 and NbResources)
  Standard_EXPORT virtual Handle(Interface_Protocol) Resource (const Standard_Integer num) const = 0;
  
  //! Returns a unique positive CaseNumber for each Recognized
  //! Object. By default, recognition is based on Type(1)
  //! By default, calls the following one which is deferred.
  Standard_EXPORT virtual Standard_Integer CaseNumber (const Handle(Standard_Transient)& obj) const;
  
  //! Returns True if type of <obj> is that defined from CDL
  //! This is the default but it may change according implementation
  Standard_EXPORT virtual Standard_Boolean IsDynamicType (const Handle(Standard_Transient)& obj) const;
  
  //! Returns the count of DISTINCT types under which an entity may
  //! be processed. Each one is candidate to be recognized by
  //! TypeNumber, <obj> is then processed according it
  //! By default, returns 1 (the DynamicType)
  Standard_EXPORT virtual Standard_Integer NbTypes (const Handle(Standard_Transient)& obj) const;
  
  //! Returns a type under which <obj> can be recognized and
  //! processed, according its rank in its definition list (see
  //! NbTypes).
  //! By default, returns DynamicType
  Standard_EXPORT Handle(Standard_Type) Type (const Handle(Standard_Transient)& obj, const Standard_Integer nt = 1) const;
  
  //! Returns a unique positive CaseNumber for each Recognized Type,
  //! Returns Zero for "<type> not recognized"
  Standard_EXPORT virtual Standard_Integer TypeNumber (const Handle(Standard_Type)& atype) const = 0;
  
  //! Evaluates a Global Check for a model (with its Graph)
  //! Returns True when done, False if data in model do not apply
  //!
  //! Very specific of each norm, i.e. of each protocol : the
  //! uppest level Protocol assumes it, it can call GlobalCheck of
  //! its resources only if it is necessary
  //!
  //! Default does nothing, can be redefined
  Standard_EXPORT virtual Standard_Boolean GlobalCheck (const Interface_Graph& G, Handle(Interface_Check)& ach) const;
  
  //! Creates an empty Model of the considered Norm
  Standard_EXPORT virtual Handle(Interface_InterfaceModel) NewModel() const = 0;
  
  //! Returns True if <model> is a Model of the considered Norm
  Standard_EXPORT virtual Standard_Boolean IsSuitableModel (const Handle(Interface_InterfaceModel)& model) const = 0;
  
  //! Creates a new Unknown Entity for the considered Norm
  Standard_EXPORT virtual Handle(Standard_Transient) UnknownEntity() const = 0;
  
  //! Returns True if <ent> is an Unknown Entity for the Norm, i.e.
  //! same Type as them created by method UnknownEntity
  //! (for an Entity out of the Norm, answer can be unpredicable)
  Standard_EXPORT virtual Standard_Boolean IsUnknownEntity (const Handle(Standard_Transient)& ent) const = 0;




  DEFINE_STANDARD_RTTIEXT(Interface_Protocol,Standard_Transient)

protected:




private:




};







#endif // _Interface_Protocol_HeaderFile
