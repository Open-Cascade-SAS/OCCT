// Created on: 1997-03-03
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

#ifndef _Storage_DefaultCallBack_HeaderFile
#define _Storage_DefaultCallBack_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Storage_CallBack.hxx>
class Standard_Persistent;
class Storage_Schema;
class Storage_BaseDriver;
class Storage_DefaultCallBack;

DEFINE_STANDARD_HANDLE(Storage_DefaultCallBack, Storage_CallBack)

class Storage_DefaultCallBack : public Storage_CallBack
{
public:
  Standard_EXPORT Storage_DefaultCallBack();

  Standard_EXPORT Handle(Standard_Persistent) New() const Standard_OVERRIDE;

  Standard_EXPORT void Add(const Handle(Standard_Persistent)& thePers,
                           const Handle(Storage_Schema)&      theSchema) const Standard_OVERRIDE;

  Standard_EXPORT void Write(const Handle(Standard_Persistent)& thePers,
                             const Handle(Storage_BaseDriver)&  theDriver,
                             const Handle(Storage_Schema)&      theSchema) const Standard_OVERRIDE;

  Standard_EXPORT void Read(const Handle(Standard_Persistent)& thePers,
                            const Handle(Storage_BaseDriver)&  theDriver,
                            const Handle(Storage_Schema)&      theSchema) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Storage_DefaultCallBack, Storage_CallBack)
};

#endif // _Storage_DefaultCallBack_HeaderFile
