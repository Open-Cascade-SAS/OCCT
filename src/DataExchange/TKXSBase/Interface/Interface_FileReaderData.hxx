// Created on: 1992-02-10
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

#ifndef _Interface_FileReaderData_HeaderFile
#define _Interface_FileReaderData_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfTransient.hxx>
#include <Standard_Transient.hxx>
#include <Standard_CString.hxx>
#include <Interface_ParamType.hxx>
class Interface_ParamSet;
class TCollection_AsciiString;
class Interface_FileParameter;
class Interface_ParamList;

class Interface_FileReaderData;
DEFINE_STANDARD_HANDLE(Interface_FileReaderData, Standard_Transient)

//! This class defines services which permit to access Data issued
//! from a File, in a form which does not depend of physical
//! format : thus, each Record has an attached ParamList (to be
//! managed) and resulting Entity.
//!
//! Each Interface defines its own FileReaderData : on one hand by
//! defining deferred methods given here, on the other hand by
//! describing literal data and their accesses, with the help of
//! basic classes such as String, Array1OfString, etc...
//!
//! FileReaderData is used by a FileReaderTool, which is also
//! specific of each Norm, to read an InterfaceModel of the Norm
//! FileReaderData inherits TShared to be accessed by Handle :
//! this allows FileReaderTool to define more easily the specific
//! methods, and improves memory management.
class Interface_FileReaderData : public Standard_Transient
{

public:
  //! Returns the count of registered records
  //! That is, value given for Initialization (can be redefined)
  Standard_EXPORT virtual Standard_Integer NbRecords() const;

  //! Returns the count of entities. Depending of each norm, records
  //! can be Entities or SubParts (SubList in STEP, SubGroup in SET
  //! ...). NbEntities counts only Entities, not Subs
  //! Used for memory reservation in InterfaceModel
  //! Default implementation uses FindNextRecord
  //! Can be redefined into a more performant way
  Standard_EXPORT virtual Standard_Integer NbEntities() const;

  //! Determines the record number defining an Entity following a
  //! given record number. Specific to each sub-class of
  //! FileReaderData. Returning zero means no record found
  Standard_EXPORT virtual Standard_Integer FindNextRecord(const Standard_Integer num) const = 0;

  //! attaches an empty ParamList to a Record
  Standard_EXPORT void InitParams(const Standard_Integer num);

  //! Adds a parameter to record no "num" and fills its fields
  //! (EntityNumber is optional)
  //! Warning : <aval> is assumed to be memory-managed elsewhere : it is NOT
  //! copied. This gives a best speed : strings remain stored in
  //! pages of characters
  Standard_EXPORT void AddParam(const Standard_Integer    num,
                                const Standard_CString    aval,
                                const Interface_ParamType atype,
                                const Standard_Integer    nument = 0);

  //! Same as above, but gets a AsciiString from TCollection
  //! Remark that the content of the AsciiString is locally copied
  //! (because its content is most often lost after using)
  Standard_EXPORT void AddParam(const Standard_Integer         num,
                                const TCollection_AsciiString& aval,
                                const Interface_ParamType      atype,
                                const Standard_Integer         nument = 0);

  //! Same as above, but gets a complete FileParameter
  //! Warning : Content of <FP> is NOT copied : its original address and space
  //! in memory are assumed to be managed elsewhere (see ParamSet)
  Standard_EXPORT void AddParam(const Standard_Integer num, const Interface_FileParameter& FP);

  //! Sets a new value for a parameter of a record, given by :
  //! num : record number; nump : parameter number in the record
  Standard_EXPORT void SetParam(const Standard_Integer         num,
                                const Standard_Integer         nump,
                                const Interface_FileParameter& FP);

  //! Returns count of parameters attached to record "num"
  //! If <num> = 0, returns the total recorded count of parameters
  Standard_EXPORT Standard_Integer NbParams(const Standard_Integer num) const;

  //! Returns the complete ParamList of a record (read only)
  //! num = 0 to return the whole param list for the file
  Standard_EXPORT Handle(Interface_ParamList) Params(const Standard_Integer num) const;

  //! Returns parameter "nump" of record "num", as a complete
  //! FileParameter
  Standard_EXPORT const Interface_FileParameter& Param(const Standard_Integer num,
                                                       const Standard_Integer nump) const;

  //! Same as above, but in order to be modified on place
  Standard_EXPORT Interface_FileParameter& ChangeParam(const Standard_Integer num,
                                                       const Standard_Integer nump);

  //! Returns type of parameter "nump" of record "num"
  //! Returns literal value of parameter "nump" of record "num"
  //! was C++ : return const &
  Standard_EXPORT Interface_ParamType ParamType(const Standard_Integer num,
                                                const Standard_Integer nump) const;

  //! Same as above, but as a CString
  //! was C++ : return const
  Standard_EXPORT Standard_CString ParamCValue(const Standard_Integer num,
                                               const Standard_Integer nump) const;

  //! Returns True if parameter "nump" of record "num" is defined
  //! (it is not if its type is ParamVoid)
  Standard_EXPORT Standard_Boolean IsParamDefined(const Standard_Integer num,
                                                  const Standard_Integer nump) const;

  //! Returns record number of an entity referenced by a parameter
  //! of type Ident; 0 if no EntityNumber has been determined
  //! Note that it is used to reference Entities but also Sublists
  //! (sublists are not objects, but internal descriptions)
  Standard_EXPORT Standard_Integer ParamNumber(const Standard_Integer num,
                                               const Standard_Integer nump) const;

  //! Returns the StepEntity referenced by a parameter
  //! Error if none
  Standard_EXPORT const Handle(Standard_Transient)& ParamEntity(const Standard_Integer num,
                                                                const Standard_Integer nump) const;

  //! Returns the absolute rank of the beginning of a record
  //! (its list is from ParamFirstRank+1 to ParamFirstRank+NbParams)
  Standard_EXPORT Standard_Integer ParamFirstRank(const Standard_Integer num) const;

  //! Returns the entity bound to a record, set by SetEntities
  Standard_EXPORT const Handle(Standard_Transient)& BoundEntity(const Standard_Integer num) const;

  //! Binds an entity to a record
  Standard_EXPORT void BindEntity(const Standard_Integer            num,
                                  const Handle(Standard_Transient)& ent);

  //! Sets the status "Error Load" on, to overside check fails
  //! <val> True  : declares unloaded
  //! <val> False : declares loaded
  //! If not called before loading (see FileReaderTool), check fails
  //! give the status
  //! IsErrorLoad says if SetErrorLoad has been called by user
  //! ResetErrorLoad resets it (called by FileReaderTool)
  //! This allows to specify that the currently loaded entity
  //! remains unloaded (because of syntactic fail)
  Standard_EXPORT void SetErrorLoad(const Standard_Boolean val);

  //! Returns True if the status "Error Load" has been set (to True
  //! or False)
  Standard_EXPORT Standard_Boolean IsErrorLoad() const;

  //! Returns the former value of status "Error Load" then resets it
  //! Used to read the status then ensure it is reset
  Standard_EXPORT Standard_Boolean ResetErrorLoad();

  //! Destructor (waiting for memory management)
  Standard_EXPORT void Destroy();

  ~Interface_FileReaderData() { Destroy(); }

  //! Same spec.s as standard <atof> but 5 times faster
  Standard_EXPORT static Standard_Real Fastof(const Standard_CString str);

  DEFINE_STANDARD_RTTIEXT(Interface_FileReaderData, Standard_Transient)

protected:
  //! Initializes arrays of Entities and of ParamLists attached
  //! to registered records
  //! <nbr> must be the maximum number of records to get (no way to
  //! extend it at run-time) : count entities and sub-entities ...
  //! <npar> is the total count of parameters (if it is not exact,
  //! it will be extended as necessary)
  //!
  //! Hence, to each record can be bound an Entity and a list of
  //! Parameters. Each kind of FileReaderData can add other data, by
  //! having them in parallel (other arrays with same sizes)
  //! Else, it must manage binding between items and their data
  Standard_EXPORT Interface_FileReaderData(const Standard_Integer nbr, const Standard_Integer npar);

  //! Returns a parameter given its absolute rank in the file
  //! in order to be consulted or modified in specilaized actions
  Standard_EXPORT Interface_FileParameter& ChangeParameter(const Standard_Integer numpar);

  //! For a given absolute rank of parameter, determines the
  //! record to which its belongs, and the parameter number for it
  Standard_EXPORT void ParamPosition(const Standard_Integer numpar,
                                     Standard_Integer&      num,
                                     Standard_Integer&      nump) const;

private:
  Standard_Integer           thenum0;
  Standard_Integer           therrload;
  Handle(Interface_ParamSet) theparams;
  TColStd_Array1OfInteger    thenumpar;
  TColStd_Array1OfTransient  theents;
};

#endif // _Interface_FileReaderData_HeaderFile
