// Created on: 1997-12-10
// Created by: Denis PASCAL
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _TDataStd_Relation_HeaderFile
#define _TDataStd_Relation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_ExtendedString.hxx>
#include <TDF_AttributeList.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_OStream.hxx>
class Standard_GUID;
class TDF_Label;
class TCollection_ExtendedString;
class TDF_Attribute;
class TDF_RelocationTable;


class TDataStd_Relation;
DEFINE_STANDARD_HANDLE(TDataStd_Relation, TDF_Attribute)

//! Relation attribute.
//! ==================
//!
//! *  Data Structure of  the  Expression is stored in a
//! string and references to variables used by the string
//!
//! Warning:  To be consistent,  each  Variable  referenced by  the
//! relation must have its equivalent in the string
class TDataStd_Relation : public TDF_Attribute
{

public:

  
  //! class methods
  //! =============
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Find, or create, an Relation attribute.
  //! Real methods
  //! ============
  Standard_EXPORT static Handle(TDataStd_Relation) Set (const TDF_Label& label);
  
  Standard_EXPORT TDataStd_Relation();
  
  //! build and return the relation name
  Standard_EXPORT TCollection_ExtendedString Name() const;
  
  Standard_EXPORT void SetRelation (const TCollection_ExtendedString& E);
  
  Standard_EXPORT const TCollection_ExtendedString& GetRelation() const;
  
  Standard_EXPORT TDF_AttributeList& GetVariables();
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& With) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& Into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(TDataStd_Relation,TDF_Attribute)

protected:




private:


  TCollection_ExtendedString myRelation;
  TDF_AttributeList myVariables;


};







#endif // _TDataStd_Relation_HeaderFile
