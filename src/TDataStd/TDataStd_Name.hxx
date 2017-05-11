// Created on: 1997-07-31
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

#ifndef _TDataStd_Name_HeaderFile
#define _TDataStd_Name_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_ExtendedString.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_OStream.hxx>
#include <Standard_GUID.hxx>

class Standard_DomainError;
class TDF_Label;
class TCollection_ExtendedString;
class TDF_Attribute;
class TDF_RelocationTable;


class TDataStd_Name;
DEFINE_STANDARD_HANDLE(TDataStd_Name, TDF_Attribute)

//! Used to define a name attribute containing a string which specifies the name.
class TDataStd_Name : public TDF_Attribute
{

public:

  
  //! class methods working on the name itself
  //! ========================================
  //! Returns the GUID for name attributes.
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Creates (if does not exist) and sets the name in the name attribute.
  //! from any label <L>  search in father  labels (L is not
  //! concerned) the first name attribute.if found set it in
  //! <father>.
  //! class methods working on the name tree
  //! ======================================
  //! Search in the  whole TDF_Data the Name attribute which
  //! fit with <fullPath>. Returns True if found.
  //! Search  under <currentLabel>  a  label which fit with
  //! <name>. Returns True if  found. Shortcut which  avoids
  //! building a ListOfExtendedStrin.
  //! Search in the whole TDF_Data the label which fit with name
  //! Returns True if found.
  //! tools methods to translate path <-> pathlist
  //! ===========================================
  //! move to draw For Draw test we may provide this tool method which convert a path in a
  //! sequence of string to call after the FindLabel methods.
  //! Example: if it's given "Assembly:Part_1:Sketch_5" it will return in <pathlist>
  //! the list of 3 strings: "Assembly","Part_1","Sketch_5".
  //! move to draw from <pathlist> build the string path
  //! Name methods
  //! ============
  Standard_EXPORT static Handle(TDataStd_Name) Set (const TDF_Label& label, const TCollection_ExtendedString& string);

  //! Finds, or creates, a Name attribute with explicit user defined <guid> and sets <string>.
  //! The Name attribute  is  returned. 
  Standard_EXPORT static Handle(TDataStd_Name) Set (const TDF_Label& label, const Standard_GUID& guid,
	                            const TCollection_ExtendedString& string);
  Standard_EXPORT TDataStd_Name();
  
  //! Sets <S> as name. Raises if <S> is not a valid name.
  Standard_EXPORT void Set (const TCollection_ExtendedString& S);
  
  //! Sets the explicit user defined GUID  to the attribute.
  Standard_EXPORT void SetID (const Standard_GUID& guid) Standard_OVERRIDE;

  //! Sets default GUID for the attribute.
  Standard_EXPORT void SetID() Standard_OVERRIDE;

  //! Returns the name contained in this name attribute.
  Standard_EXPORT const TCollection_ExtendedString& Get() const;
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& with) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(TDataStd_Name,TDF_Attribute)

protected:




private:


  TCollection_ExtendedString myString;
  Standard_GUID myID;

};







#endif // _TDataStd_Name_HeaderFile
