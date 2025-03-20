// Created on: 1992-02-04
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

#ifndef _Interface_UndefinedContent_HeaderFile
#define _Interface_UndefinedContent_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <Interface_EntityList.hxx>
#include <Standard_Transient.hxx>
#include <Interface_ParamType.hxx>
class TCollection_HAsciiString;
class Interface_CopyTool;

class Interface_UndefinedContent;
DEFINE_STANDARD_HANDLE(Interface_UndefinedContent, Standard_Transient)

//! Defines resources for an "Undefined Entity" : such an Entity
//! is used to describe an Entity which complies with the Norm,
//! but of an Unknown Type : hence it is kept under a literal
//! form (avoiding to loose data). UndefinedContent offers a way
//! to store a list of Parameters, as literals or references to
//! other Entities
//!
//! Each Interface must provide one "UndefinedEntity", which must
//! have same basic description as all its types of entities :
//! the best way would be double inheritance : on the Entity Root
//! of the Norm and on an general "UndefinedEntity"
//!
//! While it is not possible to do so, the UndefinedEntity of each
//! Interface can define its own UndefinedEntity by INCLUDING
//! (in a field) this UndefinedContent
//!
//! Hence, for that UndefinedEntity, define a Constructor which
//! creates this UndefinedContent, plus access methods to it
//! (or to its data, calling methods defined here).
//!
//! Finally, the Protocols of each norm have to Create and
//! Recognize Unknown Entities of this norm
class Interface_UndefinedContent : public Standard_Transient
{

public:
  //! Defines an empty UndefinedContent
  Standard_EXPORT Interface_UndefinedContent();

  //! Gives count of recorded parameters
  Standard_EXPORT Standard_Integer NbParams() const;

  //! Gives count of Literal Parameters
  Standard_EXPORT Standard_Integer NbLiterals() const;

  //! Returns data of a Parameter : its type, and the entity if it
  //! designates en entity ("ent") or its literal value else ("str")
  //! Returned value (Boolean) : True if it is an Entity, False else
  Standard_EXPORT Standard_Boolean ParamData(const Standard_Integer            num,
                                             Interface_ParamType&              ptype,
                                             Handle(Standard_Transient)&       ent,
                                             Handle(TCollection_HAsciiString)& val) const;

  //! Returns the ParamType of a Param, given its rank
  //! Error if num is not between 1 and NbParams
  Standard_EXPORT Interface_ParamType ParamType(const Standard_Integer num) const;

  //! Returns True if a Parameter is recorded as an entity
  //! Error if num is not between 1 and NbParams
  Standard_EXPORT Standard_Boolean IsParamEntity(const Standard_Integer num) const;

  //! Returns Entity corresponding to a Param, given its rank
  Standard_EXPORT Handle(Standard_Transient) ParamEntity(const Standard_Integer num) const;

  //! Returns literal value of a Parameter, given its rank
  Standard_EXPORT Handle(TCollection_HAsciiString) ParamValue(const Standard_Integer num) const;

  //! Manages reservation for parameters (internal use)
  //! (nb : total count of parameters, nblit : count of literals)
  Standard_EXPORT void Reservate(const Standard_Integer nb, const Standard_Integer nblit);

  //! Adds a literal Parameter to the list
  Standard_EXPORT void AddLiteral(const Interface_ParamType               ptype,
                                  const Handle(TCollection_HAsciiString)& val);

  //! Adds a Parameter which references an Entity
  Standard_EXPORT void AddEntity(const Interface_ParamType         ptype,
                                 const Handle(Standard_Transient)& ent);

  //! Removes a Parameter given its rank
  Standard_EXPORT void RemoveParam(const Standard_Integer num);

  //! Sets a new value for the Parameter <num>, to a literal value
  //! (if it referenced formerly an Entity, this Entity is removed)
  Standard_EXPORT void SetLiteral(const Standard_Integer                  num,
                                  const Interface_ParamType               ptype,
                                  const Handle(TCollection_HAsciiString)& val);

  //! Sets a new value for the Parameter <num>, to reference an
  //! Entity. To simply change the Entity, see the variant below
  Standard_EXPORT void SetEntity(const Standard_Integer            num,
                                 const Interface_ParamType         ptype,
                                 const Handle(Standard_Transient)& ent);

  //! Changes the Entity referenced by the Parameter <num>
  //! (with same ParamType)
  Standard_EXPORT void SetEntity(const Standard_Integer num, const Handle(Standard_Transient)& ent);

  //! Returns globally the list of param entities. Note that it can
  //! be used as shared entity list for the UndefinedEntity
  Standard_EXPORT Interface_EntityList EntityList() const;

  //! Copies contents of undefined entities; deigned to be called by
  //! GetFromAnother method from Undefined entity of each Interface
  //! (the basic operation is the same regardless the norm)
  Standard_EXPORT void GetFromAnother(const Handle(Interface_UndefinedContent)& other,
                                      Interface_CopyTool&                       TC);

  DEFINE_STANDARD_RTTIEXT(Interface_UndefinedContent, Standard_Transient)

protected:
private:
  Standard_Integer                        thenbparams;
  Standard_Integer                        thenbstr;
  Handle(TColStd_HArray1OfInteger)        theparams;
  Handle(Interface_HArray1OfHAsciiString) thevalues;
  Interface_EntityList                    theentities;
};

#endif // _Interface_UndefinedContent_HeaderFile
