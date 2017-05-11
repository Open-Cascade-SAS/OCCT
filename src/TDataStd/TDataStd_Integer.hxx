// Created on: 1997-02-06
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

#ifndef _TDataStd_Integer_HeaderFile
#define _TDataStd_Integer_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_OStream.hxx>
#include <Standard_GUID.hxx>

class Standard_GUID;
class TDF_Label;
class TDF_Attribute;
class TDF_RelocationTable;


class TDataStd_Integer;
DEFINE_STANDARD_HANDLE(TDataStd_Integer, TDF_Attribute)

//! The basis to define an integer attribute.
class TDataStd_Integer : public TDF_Attribute
{

public:

  
  //! class methods
  //! =============
  //! Returns the GUID for integers.
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Finds, or creates, an Integer attribute and sets <value>
  //! the Integer  attribute is returned.
  Standard_EXPORT static Handle(TDataStd_Integer) Set (const TDF_Label& label, const Standard_Integer value);
  
  //! Finds, or creates, an Integer attribute with explicit user defined <guid> and sets <value>.
  //! The Integer attribute  is  returned. 
  Standard_EXPORT static Handle(TDataStd_Integer) Set (const TDF_Label& label, const Standard_GUID& guid,
	                            const Standard_Integer value);

  //! Integer methods
  //! ===============
  Standard_EXPORT void Set (const Standard_Integer V);
 
  //! Sets the explicit GUID (user defined) for the attribute.
  Standard_EXPORT void SetID (const Standard_GUID& guid) Standard_OVERRIDE;

  //! Sets default GUID for the attribute.
  Standard_EXPORT void SetID() Standard_OVERRIDE;

  //! Returns the integer value contained in the attribute.
  Standard_EXPORT Standard_Integer Get() const;
  
  //! Returns True if there is a reference on the same label
  Standard_EXPORT Standard_Boolean IsCaptured() const;
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& With) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& Into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;
  
  Standard_EXPORT TDataStd_Integer();




  DEFINE_STANDARD_RTTIEXT(TDataStd_Integer,TDF_Attribute)

protected:




private:


  Standard_Integer myValue;
  Standard_GUID myID;

};







#endif // _TDataStd_Integer_HeaderFile
