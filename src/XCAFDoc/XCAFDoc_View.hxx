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

#include <TDataStd_GenericEmpty.hxx>
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
DEFINE_STANDARD_HANDLE(XCAFDoc_View, TDataStd_GenericEmpty)

//! Attribute to store view
class XCAFDoc_View : public TDataStd_GenericEmpty
{

public:

  Standard_EXPORT XCAFDoc_View();
  
  Standard_EXPORT static const Standard_GUID& GetID();
  
  Standard_EXPORT static Handle(XCAFDoc_View) Set (const TDF_Label& theLabel);
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  //! Updates parent's label and its sub-labels with data taken from theViewObject.
  //! Old data associated with the label will be lost.
  Standard_EXPORT void SetObject(const Handle(XCAFView_Object)& theViewObject);
  
  //! Returns view object data taken from the paren's label and its sub-labels.
  Standard_EXPORT Handle(XCAFView_Object) GetObject() const;
  

  DEFINE_DERIVED_ATTRIBUTE(XCAFDoc_View, TDataStd_GenericEmpty)

};

#endif
