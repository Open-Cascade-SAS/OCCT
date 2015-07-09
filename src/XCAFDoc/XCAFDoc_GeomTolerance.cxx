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

#include <XCAFDoc_GeomTolerance.hxx>

#include <TDF_RelocationTable.hxx>
#include <XCAFDoc.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_Real.hxx>
#include <XCAFDimTolObjects_GeomToleranceObject.hxx>

enum ChildLab
{
  ChildLab_Type = 1,
  ChildLab_TypeOfValue,
  ChildLab_Value,
  ChildLab_MatReqModif,
  ChildLab_ZoneModif,
  ChildLab_ValueOfZoneModif,
  ChildLab_Modifiers,
  ChildLab_aMaxValueModif
};

//=======================================================================
//function : XCAFDoc_GeomTolerance
//purpose  : 
//=======================================================================

XCAFDoc_GeomTolerance::XCAFDoc_GeomTolerance()
{
}


//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_GeomTolerance::GetID() 
{
  static Standard_GUID DGTID ("58ed092f-44de-11d8-8776-001083004c77");
  //static Standard_GUID ID("efd212e9-6dfd-11d4-b9c8-0060b0ee281b");
  return DGTID; 
  //return ID;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

Handle(XCAFDoc_GeomTolerance) XCAFDoc_GeomTolerance::Set (const TDF_Label& theLabel)
{
  Handle(XCAFDoc_GeomTolerance) A;
  if (!theLabel.FindAttribute(XCAFDoc_GeomTolerance::GetID(), A)) {
    A = new XCAFDoc_GeomTolerance();
    theLabel.AddAttribute(A);
  }
  return A;
}

//=======================================================================
//function : SetObject
//purpose  : 
//=======================================================================

void XCAFDoc_GeomTolerance::SetObject (const Handle(XCAFDimTolObjects_GeomToleranceObject)& theObject) 
{
  Backup();

  Handle(TDataStd_Integer) aType;
  if(!Label().FindChild(ChildLab_Type).FindAttribute(TDataStd_Integer::GetID(), aType))
  {
    aType = new TDataStd_Integer();
    Label().FindChild(ChildLab_Type).AddAttribute(aType);
  }
  aType->Set(theObject->GetType());

  Handle(TDataStd_Integer) aTypeOfValue;
  if(!Label().FindChild(ChildLab_TypeOfValue).FindAttribute(TDataStd_Integer::GetID(), aTypeOfValue))
  {
    aTypeOfValue = new TDataStd_Integer();
    Label().FindChild(ChildLab_TypeOfValue).AddAttribute(aTypeOfValue);
  }
  aTypeOfValue->Set(theObject->GetTypeOfValue());

  Handle(TDataStd_Real) aValue;
  if(!Label().FindChild(ChildLab_Value).FindAttribute(TDataStd_Real::GetID(), aValue))
  {
    aValue = new TDataStd_Real();
    Label().FindChild(ChildLab_Value).AddAttribute(aValue);
  }
  aValue->Set(theObject->GetValue());

  Handle(TDataStd_Integer) aMatReqModif;
  if(!Label().FindChild(ChildLab_MatReqModif).FindAttribute(TDataStd_Integer::GetID(), aMatReqModif))
  {
    aMatReqModif = new TDataStd_Integer();
    Label().FindChild(ChildLab_MatReqModif).AddAttribute(aMatReqModif);
  }
  aMatReqModif->Set(theObject->GetMaterialRequirementModifier());

  Handle(TDataStd_Integer) aZoneModif;
  if(!Label().FindChild(ChildLab_ZoneModif).FindAttribute(TDataStd_Integer::GetID(), aZoneModif))
  {
    aZoneModif = new TDataStd_Integer();
    Label().FindChild(ChildLab_ZoneModif).AddAttribute(aZoneModif);
  }
  aZoneModif->Set(theObject->GetZoneModifier());

  Handle(TDataStd_Real) aValueOfZoneModif;
  if(!Label().FindChild(ChildLab_ValueOfZoneModif).FindAttribute(TDataStd_Real::GetID(), aValueOfZoneModif))
  {
    aValueOfZoneModif = new TDataStd_Real();
    Label().FindChild(ChildLab_ValueOfZoneModif).AddAttribute(aValueOfZoneModif);
  }
  aValueOfZoneModif->Set(theObject->GetValueOfZoneModifier());

  if(theObject->GetModifiers().Length() == 0)
  {
    Label().FindChild(ChildLab_Modifiers).ForgetAllAttributes();
  }
  else
  {
    Handle(TDataStd_IntegerArray) aModifiers;
    if(!Label().FindChild(ChildLab_Modifiers).FindAttribute(TDataStd_IntegerArray::GetID(), aModifiers)
      || theObject->GetModifiers().Length() == 0)
    {
      aModifiers = new TDataStd_IntegerArray();
      Label().FindChild(ChildLab_Modifiers).AddAttribute(aModifiers);
    }
    Handle(TColStd_HArray1OfInteger) anArr = new TColStd_HArray1OfInteger(1,theObject->GetModifiers().Length());
    for(Standard_Integer i = 1; i <= theObject->GetModifiers().Length(); i++)
      anArr->SetValue(i,theObject->GetModifiers().Value(i));
    aModifiers->ChangeArray(anArr);
  }

  Handle(TDataStd_Real) aMaxValueModif;
  if(!Label().FindChild(ChildLab_aMaxValueModif).FindAttribute(TDataStd_Real::GetID(), aMaxValueModif))
  {
    aMaxValueModif = new TDataStd_Real();
    Label().FindChild(ChildLab_aMaxValueModif).AddAttribute(aMaxValueModif);
  }
  aMaxValueModif->Set(theObject->GetMaxValueModifier());
}

//=======================================================================
//function :GetObject
//purpose  : 
//=======================================================================

Handle(XCAFDimTolObjects_GeomToleranceObject) XCAFDoc_GeomTolerance::GetObject()  const
{
  Handle(XCAFDimTolObjects_GeomToleranceObject) anObj = new XCAFDimTolObjects_GeomToleranceObject();

  Handle(TDataStd_Integer) aType;
  if(Label().FindChild(ChildLab_Type).FindAttribute(TDataStd_Integer::GetID(), aType))
  {
    anObj->SetType((XCAFDimTolObjects_GeomToleranceType)aType->Get());
  }

  Handle(TDataStd_Integer) aTypeOfValue;
  if(Label().FindChild(ChildLab_TypeOfValue).FindAttribute(TDataStd_Integer::GetID(), aTypeOfValue))
  {
    anObj->SetTypeOfValue((XCAFDimTolObjects_GeomToleranceTypeValue)aTypeOfValue->Get());
  }

  Handle(TDataStd_Real) aValue;
  if(Label().FindChild(ChildLab_Value).FindAttribute(TDataStd_Real::GetID(), aValue))
  {
    anObj->SetValue(aValue->Get());
  }

  Handle(TDataStd_Integer) aMatReqModif;
  if(Label().FindChild(ChildLab_MatReqModif).FindAttribute(TDataStd_Integer::GetID(), aMatReqModif))
  {
    anObj->SetMaterialRequirementModifier((XCAFDimTolObjects_GeomToleranceMatReqModif)aMatReqModif->Get());
  }

  Handle(TDataStd_Integer) aZoneModif;
  if(Label().FindChild(ChildLab_ZoneModif).FindAttribute(TDataStd_Integer::GetID(), aZoneModif))
  {
    anObj->SetZoneModifier((XCAFDimTolObjects_GeomToleranceZoneModif)aZoneModif->Get());
  }

  Handle(TDataStd_Real) aValueOfZoneModif;
  if(Label().FindChild(ChildLab_ValueOfZoneModif).FindAttribute(TDataStd_Real::GetID(), aValueOfZoneModif))
  {
    anObj->SetValueOfZoneModifier(aValueOfZoneModif->Get());
  }

  Handle(TDataStd_IntegerArray) anArr;
  if(Label().FindChild(ChildLab_Modifiers).FindAttribute(TDataStd_IntegerArray::GetID(), anArr)
     && !anArr->Array().IsNull())
  {
    XCAFDimTolObjects_GeomToleranceModifiersSequence aModifiers;
    for(Standard_Integer i = 1; i <= anArr->Length(); i++)
      aModifiers.Append((XCAFDimTolObjects_GeomToleranceModif)anArr->Value(i));
    anObj->SetModifiers(aModifiers);
  }

  Handle(TDataStd_Real) aMaxValueModif;
  if(Label().FindChild(ChildLab_aMaxValueModif).FindAttribute(TDataStd_Real::GetID(), aMaxValueModif))
  {
    anObj->SetMaxValueModifier(aMaxValueModif->Get());
  }

  return anObj;
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_GeomTolerance::ID() const
{
  return GetID();
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void XCAFDoc_GeomTolerance::Restore(const Handle(TDF_Attribute)& /*With*/) 
{
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) XCAFDoc_GeomTolerance::NewEmpty() const
{
  return new XCAFDoc_GeomTolerance();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void XCAFDoc_GeomTolerance::Paste(const Handle(TDF_Attribute)& /*Into*/,
                           const Handle(TDF_RelocationTable)& /*RT*/) const
{
}
