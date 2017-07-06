// Created on: 1997-02-27
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

#ifndef _Storage_CallBack_HeaderFile
#define _Storage_CallBack_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class Standard_Persistent;
class Storage_Schema;
class Storage_BaseDriver;


class Storage_CallBack;
DEFINE_STANDARD_HANDLE(Storage_CallBack, Standard_Transient)


class Storage_CallBack : public Standard_Transient
{

public:

  
  Standard_EXPORT virtual Handle(Standard_Persistent) New() const = 0;
  
  Standard_EXPORT virtual void Add (const Handle(Standard_Persistent)& aPers, const Handle(Storage_Schema)& aSchema) const = 0;
  
  Standard_EXPORT virtual void Write (const Handle(Standard_Persistent)& aPers, Storage_BaseDriver& aDriver, const Handle(Storage_Schema)& aSchema) const = 0;
  
  Standard_EXPORT virtual void Read (const Handle(Standard_Persistent)& aPers, Storage_BaseDriver& aDriver, const Handle(Storage_Schema)& aSchema) const = 0;




  DEFINE_STANDARD_RTTIEXT(Storage_CallBack,Standard_Transient)

protected:




private:




};







#endif // _Storage_CallBack_HeaderFile
