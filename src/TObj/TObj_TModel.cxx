// Created on: 2004-11-23
// Created by: Pavel TELKOV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#include <TObj_TModel.hxx>

#include <Standard_GUID.hxx>
#include <TDF_RelocationTable.hxx>


IMPLEMENT_STANDARD_RTTIEXT(TObj_TModel,TDF_Attribute)

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& TObj_TModel::GetID() 
{
  static Standard_GUID GModelID ("bbdab6a6-dca9-11d4-ba37-0060b0ee18ea");
  return GModelID; 
}

//=======================================================================
//function : TObj_TModel
//purpose  : 
//=======================================================================

TObj_TModel::TObj_TModel()
{
}


//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TObj_TModel::ID() const
{
  return GetID();
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TObj_TModel::NewEmpty() const
{
  return new TObj_TModel;
}

//=======================================================================
//function : Model
//purpose  : 
//=======================================================================

Handle(TObj_Model) TObj_TModel::Model() const
{
  return myModel;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void TObj_TModel::Set(const Handle(TObj_Model)& theModel)
{
  Backup();
  myModel = theModel;
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TObj_TModel::Restore(const Handle(TDF_Attribute)& theWith) 
{
  Handle(TObj_TModel) R = Handle(TObj_TModel)::DownCast (theWith);
  myModel = R->Model();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TObj_TModel::Paste (const Handle(TDF_Attribute)& theInto ,
                             const Handle(TDF_RelocationTable)& /* RT */) const
{
  Handle(TObj_TModel) R = Handle(TObj_TModel)::DownCast (theInto);
  R->Set(myModel);
}
