// Created on: 2000-09-08
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_Volume_HeaderFile
#define _XCAFDoc_Volume_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_OStream.hxx>
class Standard_GUID;
class TDF_Label;
class TDF_Attribute;
class TDF_RelocationTable;


class XCAFDoc_Volume;
DEFINE_STANDARD_HANDLE(XCAFDoc_Volume, TDF_Attribute)

//! attribute to store volume
class XCAFDoc_Volume : public TDF_Attribute
{

public:

  
  //! class methods
  //! =============
  Standard_EXPORT XCAFDoc_Volume();
  
  Standard_EXPORT static const Standard_GUID& GetID();
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  //! Sets a value of volume
  Standard_EXPORT void Set (const Standard_Real vol);
  
  //! Find, or create, an Volume attribute and set its value
  Standard_EXPORT static Handle(XCAFDoc_Volume) Set (const TDF_Label& label, const Standard_Real vol);
  
  Standard_EXPORT Standard_Real Get() const;
  
  //! Returns volume as argument
  //! returns false if no such attribute at the <label>
  Standard_EXPORT static Standard_Boolean Get (const TDF_Label& label, Standard_Real& vol);
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& With) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& Into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(XCAFDoc_Volume,TDF_Attribute)

protected:




private:


  Standard_Real myValue;


};







#endif // _XCAFDoc_Volume_HeaderFile
