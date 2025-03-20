// Created on: 1997-02-28
// Created by: Christophe LEYNADIER
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

#ifndef _Storage_TypedCallBack_HeaderFile
#define _Storage_TypedCallBack_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_AsciiString.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
class Storage_CallBack;

class Storage_TypedCallBack;
DEFINE_STANDARD_HANDLE(Storage_TypedCallBack, Standard_Transient)

class Storage_TypedCallBack : public Standard_Transient
{

public:
  Standard_EXPORT Storage_TypedCallBack();

  Standard_EXPORT Storage_TypedCallBack(const TCollection_AsciiString&  aTypeName,
                                        const Handle(Storage_CallBack)& aCallBack);

  Standard_EXPORT void SetType(const TCollection_AsciiString& aType);

  Standard_EXPORT TCollection_AsciiString Type() const;

  Standard_EXPORT void SetCallBack(const Handle(Storage_CallBack)& aCallBack);

  Standard_EXPORT Handle(Storage_CallBack) CallBack() const;

  Standard_EXPORT void SetIndex(const Standard_Integer anIndex);

  Standard_EXPORT Standard_Integer Index() const;

  DEFINE_STANDARD_RTTIEXT(Storage_TypedCallBack, Standard_Transient)

protected:
private:
  TCollection_AsciiString  myType;
  Handle(Storage_CallBack) myCallBack;
  Standard_Integer         myIndex;
};

#endif // _Storage_TypedCallBack_HeaderFile
