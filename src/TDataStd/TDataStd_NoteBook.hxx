// Created on: 1997-07-29
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

#ifndef _TDataStd_NoteBook_HeaderFile
#define _TDataStd_NoteBook_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDF_Attribute.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
class TDF_Label;
class Standard_GUID;
class TDataStd_Real;
class TDataStd_Integer;
class TDF_Attribute;
class TDF_RelocationTable;


class TDataStd_NoteBook;
DEFINE_STANDARD_HANDLE(TDataStd_NoteBook, TDF_Attribute)

//! NoteBook Object attribute
class TDataStd_NoteBook : public TDF_Attribute
{

public:

  
  //! class methods
  //! =============
  //! try to retrieve a NoteBook attribute at <current> label
  //! or in  fathers  label of  <current>. Returns True  if
  //! found and set <N>.
  Standard_EXPORT static Standard_Boolean Find (const TDF_Label& current, Handle(TDataStd_NoteBook)& N);
  
  //! Create  an  enpty   NoteBook attribute,  located  at
  //! <label>. Raises if <label> has attribute
  Standard_EXPORT static Handle(TDataStd_NoteBook) New (const TDF_Label& label);
  
  //! NoteBook methods
  //! ===============
  Standard_EXPORT static const Standard_GUID& GetID();
  
  Standard_EXPORT TDataStd_NoteBook();
  
  //! Tool to Create  an  Integer  attribute from  <value>,
  //! Insert it in   a  new son  label   of <me>. The   Real
  //! attribute is returned.
  Standard_EXPORT Handle(TDataStd_Real) Append (const Standard_Real value, const Standard_Boolean isExported = Standard_False);
  
  //! Tool to Create  an Real attribute from <value>, Insert
  //! it  in a new son label  of <me>. The Integer attribute
  //! is returned.
  Standard_EXPORT Handle(TDataStd_Integer) Append (const Standard_Integer value, const Standard_Boolean isExported = Standard_False);
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& with) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(TDataStd_NoteBook,TDF_Attribute)

protected:




private:




};







#endif // _TDataStd_NoteBook_HeaderFile
