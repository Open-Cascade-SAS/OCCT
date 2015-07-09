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


#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <XCAFDoc_Datum.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Builder.hxx>


#include <XCAFDimTolObjects_DatumObject.hxx>

enum ChildLab
{
  ChildLab_Name = 1,
  ChildLab_Modifiers,
  ChildLab_ModifierWithValue,
  ChildLab_DatumTarget
};

//=======================================================================
//function : XCAFDoc_Datum
//purpose  : 
//=======================================================================
XCAFDoc_Datum::XCAFDoc_Datum()
{
}


//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_Datum::GetID() 
{
  static Standard_GUID DID("58ed092e-44de-11d8-8776-001083004c77");
  //static Standard_GUID ID("efd212e2-6dfd-11d4-b9c8-0060b0ee281b");
  return DID;
  //return ID;
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(XCAFDoc_Datum) XCAFDoc_Datum::Set(const TDF_Label& theLabel,
                                         const Handle(TCollection_HAsciiString)& theName,
                                         const Handle(TCollection_HAsciiString)& theDescription,
                                         const Handle(TCollection_HAsciiString)& theIdentification) 
{
  Handle(XCAFDoc_Datum) aDatum;
  if (!theLabel.FindAttribute(XCAFDoc_Datum::GetID(), aDatum)) {
    aDatum = new XCAFDoc_Datum();
    theLabel.AddAttribute(aDatum);
  }
  aDatum->Set(theName,theDescription,theIdentification); 
  return aDatum;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(XCAFDoc_Datum) XCAFDoc_Datum::Set(const TDF_Label& theLabel) 
{
  Handle(XCAFDoc_Datum) aDatum;
  if (!theLabel.FindAttribute(XCAFDoc_Datum::GetID(), aDatum)) {
    aDatum = new XCAFDoc_Datum();
    theLabel.AddAttribute(aDatum);
  }
  return aDatum;
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void XCAFDoc_Datum::Set(const Handle(TCollection_HAsciiString)& theName,
                        const Handle(TCollection_HAsciiString)& theDescription,
                        const Handle(TCollection_HAsciiString)& theIdentification) 
{
  Backup();
  myName = theName;
  myDescription = theDescription;
  myIdentification = theIdentification;
}


//=======================================================================
//function : GetName
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) XCAFDoc_Datum::GetName() const
{
  if(myName.IsNull())
    return new TCollection_HAsciiString();
  return myName;
}

//=======================================================================
//function : GetDescriptio7n
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) XCAFDoc_Datum::GetDescription() const
{
  return myDescription;
}


//=======================================================================
//function : GetIdentification
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) XCAFDoc_Datum::GetIdentification() const
{
  return myIdentification;
}

//=======================================================================
//function : SetObject
//purpose  : 
//=======================================================================

void XCAFDoc_Datum::SetObject(const Handle(XCAFDimTolObjects_DatumObject)& theObject)
{
  Backup();
  if (!theObject->GetName().IsNull())
  {
    Handle(TDataStd_AsciiString) anAttName;
    if(!Label().FindChild(ChildLab_Name).FindAttribute(TDataStd_AsciiString::GetID(), anAttName))
    {
      anAttName = new TDataStd_AsciiString();
      Label().FindChild(ChildLab_Name).AddAttribute(anAttName);
    }
    anAttName->Set(theObject->GetName()->String());
  }
  else
  {
    Label().FindChild(ChildLab_Name).ForgetAllAttributes();
  }

  if(theObject->GetModifiers().Length() == 0)
  {
    Label().FindChild(ChildLab_Modifiers).ForgetAllAttributes();
  }
  else
  {
    Handle(TDataStd_IntegerArray) aModifiers;
    if(!Label().FindChild(ChildLab_Modifiers).FindAttribute(TDataStd_IntegerArray::GetID(), aModifiers))
    {
      aModifiers = new TDataStd_IntegerArray();
      Label().FindChild(ChildLab_Modifiers).AddAttribute(aModifiers);
    }
    Handle(TColStd_HArray1OfInteger) anArr = new TColStd_HArray1OfInteger(1,theObject->GetModifiers().Length());
    for(Standard_Integer i = 1; i <= theObject->GetModifiers().Length(); i++)
      anArr->SetValue(i,theObject->GetModifiers().Value(i));
    aModifiers->ChangeArray(anArr);
  }

  XCAFDimTolObjects_DatumModifWithValue aM;
  Standard_Real aV;
  theObject->GetModifierWithValue(aM, aV);
  if(aM != XCAFDimTolObjects_DatumModifWithValue_None)
  {
    Handle(TDataStd_Integer) aModifierWithValueM;
    if(!Label().FindChild(ChildLab_ModifierWithValue).FindAttribute(TDataStd_Integer::GetID(), aModifierWithValueM))
    {
      aModifierWithValueM = new TDataStd_Integer();
      Label().FindChild(ChildLab_ModifierWithValue).AddAttribute(aModifierWithValueM);
    }
    Handle(TDataStd_Real) aModifierWithValueV;
    if(!Label().FindChild(ChildLab_ModifierWithValue).FindAttribute(TDataStd_Real::GetID(), aModifierWithValueV))
    {
      aModifierWithValueV = new TDataStd_Real();
      Label().FindChild(ChildLab_ModifierWithValue).AddAttribute(aModifierWithValueV);
    }
    aModifierWithValueM->Set(aM);
    aModifierWithValueV->Set(aV);
  }
  else
  {
    Label().FindChild(ChildLab_ModifierWithValue).ForgetAllAttributes();
  }

  if(!theObject->GetDatumTarget().IsNull())
  {
    TNaming_Builder tnBuild(Label().FindChild(ChildLab_DatumTarget));
    tnBuild.Generated(theObject->GetDatumTarget());
  }
  else
  {
    Label().FindChild(ChildLab_DatumTarget).ForgetAllAttributes();
  }
}

//=======================================================================
//function : GetObject
//purpose  : 
//=======================================================================

Handle(XCAFDimTolObjects_DatumObject) XCAFDoc_Datum::GetObject() const
{
  Handle(XCAFDimTolObjects_DatumObject) anObj = new XCAFDimTolObjects_DatumObject();

  Handle(TDataStd_AsciiString) anAttName;
  if(Label().FindChild(ChildLab_Name).FindAttribute(TDataStd_AsciiString::GetID(), anAttName))
  {
    Handle(TCollection_HAsciiString) aStr = new TCollection_HAsciiString(anAttName->Get());
    anObj->SetName(aStr);
  }

  Handle(TDataStd_IntegerArray) anArr;
  if(Label().FindChild(ChildLab_Modifiers).FindAttribute(TDataStd_IntegerArray::GetID(), anArr)
     && !anArr->Array().IsNull())
  {
    XCAFDimTolObjects_DatumModifiersSequence aModifiers;
    for(Standard_Integer i = 1; i <= anArr->Length(); i++)
      aModifiers.Append((XCAFDimTolObjects_DatumSingleModif)anArr->Value(i));
    anObj->SetModifiers(aModifiers);
  }

  Handle(TDataStd_Integer) aModifierWithValueM;
  if(Label().FindChild(ChildLab_ModifierWithValue).FindAttribute(TDataStd_Integer::GetID(), aModifierWithValueM))
  {
    Handle(TDataStd_Real) aModifierWithValueV;
    if(Label().FindChild(ChildLab_ModifierWithValue).FindAttribute(TDataStd_Real::GetID(), aModifierWithValueV))
    {
      anObj->SetModifierWithValue((XCAFDimTolObjects_DatumModifWithValue)aModifierWithValueM->Get(),aModifierWithValueV->Get());
    }
  }

  Handle(TNaming_NamedShape) aDatumTarget;
  if(Label().FindChild(ChildLab_DatumTarget).FindAttribute(TNaming_NamedShape::GetID(), aDatumTarget))
  {
    anObj->SetDatumTarget(aDatumTarget->Get());
  }

  return anObj;
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_Datum::ID() const
{
  return GetID();
}


//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void XCAFDoc_Datum::Restore(const Handle(TDF_Attribute)& theWith) 
{
  myName = Handle(XCAFDoc_Datum)::DownCast(theWith)->GetName();
  myDescription = Handle(XCAFDoc_Datum)::DownCast(theWith)->GetDescription();
  myIdentification = Handle(XCAFDoc_Datum)::DownCast(theWith)->GetIdentification();
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) XCAFDoc_Datum::NewEmpty() const
{
  return new XCAFDoc_Datum();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void XCAFDoc_Datum::Paste(const Handle(TDF_Attribute)& theInto,
                          const Handle(TDF_RelocationTable)& /*RT*/) const
{
  Handle(XCAFDoc_Datum)::DownCast(theInto)->Set(myName,myDescription,myIdentification);
}

