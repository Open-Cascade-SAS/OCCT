// Created on: 1999-06-25
// Created by: Sergey RUIN
// Copyright (c) 1999 Matra Datavision
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

#ifndef _TDataStd_Directory_HeaderFile
#define _TDataStd_Directory_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDF_Attribute.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_OStream.hxx>
class TDF_Label;
class Standard_GUID;
class TDF_Attribute;
class TDF_RelocationTable;
class TDF_DataSet;


class TDataStd_Directory;
DEFINE_STANDARD_HANDLE(TDataStd_Directory, TDF_Attribute)

//! Associates a directory in the data framework with
//! a TDataStd_TagSource attribute.
//! You can create a new directory label and add
//! sub-directory or object labels to it,
class TDataStd_Directory : public TDF_Attribute
{

public:

  
  //! class methods
  //! =============
  //! Searches for a directory attribute on the label
  //! current, or on one of the father labels of current.
  //! If a directory attribute is found, true is returned,
  //! and the attribute found is set as D.
  Standard_EXPORT static Standard_Boolean Find (const TDF_Label& current, Handle(TDataStd_Directory)& D);
  
  //! Creates  an  enpty   Directory attribute,  located  at
  //! <label>. Raises if <label> has attribute
  Standard_EXPORT static Handle(TDataStd_Directory) New (const TDF_Label& label);
  
  //! Creates a new sub-label and sets the
  //! sub-directory dir on that label.
  Standard_EXPORT static Handle(TDataStd_Directory) AddDirectory (const Handle(TDataStd_Directory)& dir);
  
  //! Makes new label and returns it to insert
  //! other object attributes (sketch,part...etc...)
  Standard_EXPORT static TDF_Label MakeObjectLabel (const Handle(TDataStd_Directory)& dir);
  
  //! Directory methods
  //! ===============
  Standard_EXPORT static const Standard_GUID& GetID();
  
  Standard_EXPORT TDataStd_Directory();
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& with) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void References (const Handle(TDF_DataSet)& DS) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(TDataStd_Directory,TDF_Attribute)

protected:




private:




};







#endif // _TDataStd_Directory_HeaderFile
