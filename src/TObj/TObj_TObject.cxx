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

#include <TObj_TObject.hxx>

#include <Standard_GUID.hxx>
#include <TDF_AttributeDelta.hxx>
#include <TDF_ChildIterator.hxx>

IMPLEMENT_STANDARD_HANDLE(TObj_TObject,TDF_Attribute)
IMPLEMENT_STANDARD_RTTIEXT(TObj_TObject,TDF_Attribute)

//=======================================================================
//function : TObj_TObject
//purpose  : 
//=======================================================================

TObj_TObject::TObj_TObject()
{
}

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& TObj_TObject::GetID()
{
  static Standard_GUID GInterfaceID ("bbdab6a7-dca9-11d4-ba37-0060b0ee18ea");
  return GInterfaceID;
}
    
//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TObj_TObject::ID() const
{
  return GetID();
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void TObj_TObject::Set(const Handle(TObj_Object)& theElem)
{
  Backup();
  myElem = theElem;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(TObj_TObject) TObj_TObject::Set(const TDF_Label& theLabel,
                                               const Handle(TObj_Object)& theElem)
{
  Handle(TObj_TObject) A;
  if (!theLabel.FindAttribute(TObj_TObject::GetID(), A)) 
  {
    A = new TObj_TObject;
    theLabel.AddAttribute(A);
  }
  A->Set(theElem);
  return A;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

Handle(TObj_Object) TObj_TObject::Get() const
{
  return myElem;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TObj_TObject::NewEmpty () const
{  
  return new TObj_TObject();
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TObj_TObject::Restore(const Handle(TDF_Attribute)& theWith) 
{
  Handle(TObj_TObject) R = Handle(TObj_TObject)::DownCast (theWith);
  myElem = R->Get();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TObj_TObject::Paste (const Handle(TDF_Attribute)& theInto,
                               const Handle(TDF_RelocationTable)& /* RT */) const
{ 
  Handle(TObj_TObject) R = Handle(TObj_TObject)::DownCast (theInto);
  R->Set(myElem);
}

//=======================================================================
//function : BeforeForget
//purpose  : Tell TObj_Object to die,
//           i.e. (myElem->IsAlive() == false) after that
//=======================================================================

void TObj_TObject::BeforeForget()
{
  if (!myElem.IsNull()) 
  {
    // attempt to delete all data from sublabels of object to remove dependences
    TDF_Label aObjLabel = myElem->myLabel;
    if (!aObjLabel.IsNull())
    {
      TDF_ChildIterator aLI(aObjLabel);
      TDF_Label aSubLabel;
      for(; aLI.More(); aLI.Next())
      {
        aSubLabel = aLI.Value();
        if (!aSubLabel.IsNull())
          aSubLabel.ForgetAllAttributes(Standard_True);
      }
    }
    // remove back references before document die
    myElem->RemoveBackReferences(TObj_Forced);
    TDF_Label aNullLabel;
    myElem->myLabel = aNullLabel;
  }
}

//=======================================================================
//function : AfterUndo
//purpose  : Tell TObj_Object to rise from the dead,
//           i.e. (myElem->IsAlive() == true) after that
//=======================================================================

Standard_Boolean TObj_TObject::AfterUndo
  (const Handle(TDF_AttributeDelta)& anAttDelta,
   const Standard_Boolean /*forceIt*/)
{
  if (!myElem.IsNull()) 
  {
    TDF_Label aLabel = anAttDelta->Label();
    Handle(TDF_Attribute) anAttr;
    Handle(TObj_TObject) aTObject;
    Handle(TDF_Attribute) me;
    me = this;
    if(!aLabel.IsNull() && aLabel.FindAttribute(GetID(), anAttr))
      aTObject = Handle(TObj_TObject)::DownCast(anAttr);

    if(!aTObject.IsNull() && aTObject->Get() == myElem)
      myElem->myLabel = aLabel;
    else 
    {
      TDF_Label aNullLabel;
      myElem->myLabel = aNullLabel;
    }
  }
  return Standard_True;
}
