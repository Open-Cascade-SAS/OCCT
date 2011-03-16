// File:        TObj_TModel.cxx
// Created:     Thu Nov 23 12:49:09 2004
// Author:      Pavel TELKOV
// Copyright:   Open CASCADE  2007
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
