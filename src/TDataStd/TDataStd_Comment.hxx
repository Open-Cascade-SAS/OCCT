// Created on: 1998-01-15
// Created by: Denis PASCAL
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _TDataStd_Comment_HeaderFile
#define _TDataStd_Comment_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_ExtendedString.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_OStream.hxx>
#include <Standard_Boolean.hxx>
class Standard_GUID;
class TDF_Label;
class TCollection_ExtendedString;
class TDF_Attribute;
class TDF_RelocationTable;


class TDataStd_Comment;
DEFINE_STANDARD_HANDLE(TDataStd_Comment, TDF_Attribute)

//! Comment attribute. may be  associated to any label
//! to store user comment.
class TDataStd_Comment : public TDF_Attribute
{

public:

  
  //! class methods
  //! =============
  //! Returns the GUID for comments.
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Find, or create  a   Comment attribute.  the  Comment
  //! attribute is returned.
  Standard_EXPORT static Handle(TDataStd_Comment) Set (const TDF_Label& label);
  
  //! Finds, or creates a Comment attribute and sets the string.
  //! the Comment attribute is returned.
  //! Comment methods
  //! ============
  Standard_EXPORT static Handle(TDataStd_Comment) Set (const TDF_Label& label, const TCollection_ExtendedString& string);
  
  Standard_EXPORT TDataStd_Comment();
  
  Standard_EXPORT void Set (const TCollection_ExtendedString& S);
  

  //! Returns the comment attribute.
  Standard_EXPORT const TCollection_ExtendedString& Get() const;
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& with) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean AfterRetrieval (const Standard_Boolean forceIt = Standard_False) Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(TDataStd_Comment,TDF_Attribute)

protected:




private:


  TCollection_ExtendedString myString;


};







#endif // _TDataStd_Comment_HeaderFile
