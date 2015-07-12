// Created on: 2007-05-29
// Created by: Vlad Romashko
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#ifndef _TDataStd_ReferenceList_HeaderFile
#define _TDataStd_ReferenceList_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDF_LabelList.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
class Standard_GUID;
class TDF_Label;
class TDF_Attribute;
class TDF_RelocationTable;
class TDF_DataSet;


class TDataStd_ReferenceList;
DEFINE_STANDARD_HANDLE(TDataStd_ReferenceList, TDF_Attribute)

//! Contains a list of references.
class TDataStd_ReferenceList : public TDF_Attribute
{

public:

  
  //! Static methods
  //! ==============
  //! Returns the ID of the list of references (labels) attribute.
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Finds or creates a list of reference values (labels) attribute.
  Standard_EXPORT static Handle(TDataStd_ReferenceList) Set (const TDF_Label& label);
  
  Standard_EXPORT TDataStd_ReferenceList();
  
  Standard_EXPORT Standard_Boolean IsEmpty() const;
  
  Standard_EXPORT Standard_Integer Extent() const;
  
  Standard_EXPORT void Prepend (const TDF_Label& value);
  
  Standard_EXPORT void Append (const TDF_Label& value);
  
  //! Inserts the <value> before the first meet of <before_value>.
  Standard_EXPORT Standard_Boolean InsertBefore (const TDF_Label& value, const TDF_Label& before_value);
  
  //! Inserts the <value> after the first meet of <after_value>.
  Standard_EXPORT Standard_Boolean InsertAfter (const TDF_Label& value, const TDF_Label& after_value);
  
  //! Removes the first meet of the <value>.
  Standard_EXPORT Standard_Boolean Remove (const TDF_Label& value);
  
  Standard_EXPORT void Clear();
  
  Standard_EXPORT const TDF_Label& First() const;
  
  Standard_EXPORT const TDF_Label& Last() const;
  
  Standard_EXPORT const TDF_LabelList& List() const;
  
  Standard_EXPORT const Standard_GUID& ID() const;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& With);
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& Into, const Handle(TDF_RelocationTable)& RT) const;
  
  Standard_EXPORT virtual void References (const Handle(TDF_DataSet)& DS) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTI(TDataStd_ReferenceList,TDF_Attribute)

protected:




private:


  TDF_LabelList myList;


};







#endif // _TDataStd_ReferenceList_HeaderFile
