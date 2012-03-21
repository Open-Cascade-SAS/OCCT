// Created on: 2004-11-23
// Created by: Pavel TELKOV
// Copyright (c) 2004-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// The original implementation Copyright: (C) RINA S.p.A

#include <TObj_TModel.hxx>

#include <Standard_GUID.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_HANDLE(TObj_TModel,TDF_Attribute)
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
