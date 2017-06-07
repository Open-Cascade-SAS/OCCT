// Created on: 2016-10-19
// Created by: Irina KRYLOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_View_HeaderFile
#define _XCAFDoc_View_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDF_Attribute.hxx>
class Standard_GUID;
class TDF_Label;
class TDF_Attribute;
class TDF_RelocationTable;
class XCAFView_Object;

// resolve name collisions with WinAPI headers
#ifdef GetObject
  #undef GetObject
#endif

class XCAFDoc_View;
DEFINE_STANDARD_HANDLE(XCAFDoc_View, TDF_Attribute)

//! attribute to store view
class XCAFDoc_View : public TDF_Attribute
{

public:

  Standard_EXPORT XCAFDoc_View();
  
  Standard_EXPORT static const Standard_GUID& GetID();
  
  Standard_EXPORT static Handle(XCAFDoc_View) Set (const TDF_Label& theLabel);
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& With) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& Into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  Standard_EXPORT void SetObject (const Handle(XCAFView_Object)& theViewObject);
  
  Standard_EXPORT Handle(XCAFView_Object) GetObject() const;

  DEFINE_STANDARD_RTTIEXT(XCAFDoc_View, TDF_Attribute)

};

#endif
