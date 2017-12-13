// Created on: 2008-05-11
// Created by: Vlad ROMASHKO <vladislav.romashko@opencascade.com>
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#ifndef _BinMFunction_ScopeDriver_HeaderFile
#define _BinMFunction_ScopeDriver_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BinMDF_ADriver.hxx>
#include <Standard_Boolean.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <BinObjMgt_SRelocationTable.hxx>
class Message_Messenger;
class TDF_Attribute;
class BinObjMgt_Persistent;


class BinMFunction_ScopeDriver;
DEFINE_STANDARD_HANDLE(BinMFunction_ScopeDriver, BinMDF_ADriver)

//! Scope attribute Driver.
class BinMFunction_ScopeDriver : public BinMDF_ADriver
{

public:

  
  Standard_EXPORT BinMFunction_ScopeDriver(const Handle(Message_Messenger)& theMessageDriver);
  
  Standard_EXPORT virtual Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean Paste (const BinObjMgt_Persistent& Source, const Handle(TDF_Attribute)& Target, BinObjMgt_RRelocationTable& RelocTable) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Paste (const Handle(TDF_Attribute)& Source, BinObjMgt_Persistent& Target, BinObjMgt_SRelocationTable& RelocTable) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(BinMFunction_ScopeDriver,BinMDF_ADriver)

protected:




private:




};







#endif // _BinMFunction_ScopeDriver_HeaderFile
