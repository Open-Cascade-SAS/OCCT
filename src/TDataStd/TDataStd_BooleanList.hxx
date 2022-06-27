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

#ifndef _TDataStd_BooleanList_HeaderFile
#define _TDataStd_BooleanList_HeaderFile

#include <Standard.hxx>

#include <TDataStd_ListOfByte.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
#include <Standard_GUID.hxx>

class TDF_Label;
class TDF_RelocationTable;


class TDataStd_BooleanList;
DEFINE_STANDARD_HANDLE(TDataStd_BooleanList, TDF_Attribute)

//! Contains a list of bolleans.
class TDataStd_BooleanList : public TDF_Attribute
{

public:

  
  //! Static methods
  //! ==============
  //! Returns the ID of the list of booleans attribute.
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Finds or creates a list of boolean values attribute.
  Standard_EXPORT static Handle(TDataStd_BooleanList) Set (const TDF_Label& label);

  //! Finds or creates a list of boolean values attribute with explicit user defined <guid>.
  Standard_EXPORT static Handle(TDataStd_BooleanList) Set (const TDF_Label& label, const Standard_GUID& theGuid);
  
  Standard_EXPORT TDataStd_BooleanList();
  
  Standard_EXPORT Standard_Boolean IsEmpty() const;
  
  Standard_EXPORT Standard_Integer Extent() const;
  
  Standard_EXPORT void Prepend (const Standard_Boolean value);
  
  Standard_EXPORT void Append (const Standard_Boolean value);
  
  Standard_EXPORT void Clear();
  
  Standard_EXPORT Standard_Boolean First() const;
  
  Standard_EXPORT Standard_Boolean Last() const;
  
  //! 1 - means TRUE,
  //! 0 - means FALSE.
  Standard_EXPORT const TDataStd_ListOfByte& List() const;
  
  //! Inserts the <value> before the <index> position.
  //! The indices start with 1 .. Extent().
  Standard_EXPORT Standard_Boolean InsertBefore (const Standard_Integer index, const Standard_Boolean before_value);
  
  //! Inserts the <value> after the <index> position.
  //! The indices start with 1 .. Extent().
  Standard_EXPORT Standard_Boolean InsertAfter (const Standard_Integer index, const Standard_Boolean after_value);
  
  //! Removes a value at <index> position.
  Standard_EXPORT Standard_Boolean Remove (const Standard_Integer index);

  //! Sets the explicit GUID (user defined) for the attribute.
  Standard_EXPORT void SetID( const Standard_GUID&  theGuid) Standard_OVERRIDE;

  //! Sets default GUID for the attribute.
  Standard_EXPORT void SetID() Standard_OVERRIDE;

  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& With) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& Into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(TDataStd_BooleanList,TDF_Attribute)

protected:




private:


  TDataStd_ListOfByte myList;
  Standard_GUID myID;

};







#endif // _TDataStd_BooleanList_HeaderFile
