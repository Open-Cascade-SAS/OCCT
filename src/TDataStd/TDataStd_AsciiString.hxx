// Created on: 2007-07-31
// Created by: Sergey ZARITCHNY
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

#ifndef _TDataStd_AsciiString_HeaderFile
#define _TDataStd_AsciiString_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_AsciiString.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_OStream.hxx>
#include <Standard_GUID.hxx>

class TDF_Label;
class TCollection_AsciiString;
class TDF_Attribute;
class TDF_RelocationTable;


class TDataStd_AsciiString;
DEFINE_STANDARD_HANDLE(TDataStd_AsciiString, TDF_Attribute)

//! Used to define an AsciiString attribute containing a TCollection_AsciiString
class TDataStd_AsciiString : public TDF_Attribute
{

public:

  
  //! class methods
  //! =============
  //! Returns the GUID of the attribute.
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Finds, or creates an AsciiString attribute and sets the string.
  //! the AsciiString attribute is returned.
  //! AsciiString methods
  //! ===================
  Standard_EXPORT static Handle(TDataStd_AsciiString) Set (const TDF_Label& label, const TCollection_AsciiString& string);
 
  //! Finds, or creates, an AsciiString attribute with explicit user defined <guid> and sets <string>.
  //! The Name attribute  is  returned. 
  Standard_EXPORT static Handle(TDataStd_AsciiString) Set (const TDF_Label& label, const Standard_GUID& guid,
	                            const TCollection_AsciiString& string);

  Standard_EXPORT TDataStd_AsciiString();
  
  Standard_EXPORT void Set (const TCollection_AsciiString& S);
  
  //! Sets the explicit user defined GUID  to the attribute.
  Standard_EXPORT void SetID (const Standard_GUID& guid) Standard_OVERRIDE;

  //! Sets default GUID for the attribute.
  Standard_EXPORT void SetID() Standard_OVERRIDE;

  Standard_EXPORT const TCollection_AsciiString& Get() const;
  
  Standard_EXPORT Standard_Boolean IsEmpty() const;
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& with) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(TDataStd_AsciiString,TDF_Attribute)

protected:




private:


  TCollection_AsciiString myString;
  Standard_GUID myID;

};







#endif // _TDataStd_AsciiString_HeaderFile
