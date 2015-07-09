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

#include <XCAFDoc_Dimension.hxx>

#include <TDF_RelocationTable.hxx>
#include <XCAFDoc.hxx>
#include <TDataStd_TreeNode.hxx>
#include <Precision.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_RealArray.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TopoDS.hxx>
#include <XCAFDimTolObjects_DimensionObject.hxx>

enum ChildLab
{
  ChildLab_Type = 1,
  ChildLab_Value,
  ChildLab_Qualifier,
  ChildLab_Class,
  ChildLab_Dec,
  ChildLab_Modifiers,
  ChildLab_Path,
  ChildLab_Dir,
  ChildLab_Pnts
};

//=======================================================================
//function : XCAFDoc_Dimension
//purpose  : 
//=======================================================================

XCAFDoc_Dimension::XCAFDoc_Dimension()
{
}


//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_Dimension::GetID() 
{
  static Standard_GUID DGTID ("58ed092c-44de-11d8-8776-001083004c77");
  //static Standard_GUID ID("efd212e9-6dfd-11d4-b9c8-0060b0ee281b");
  return DGTID; 
  //return ID;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle(XCAFDoc_Dimension) XCAFDoc_Dimension::Set(const TDF_Label& theLabel) 
{
  Handle(XCAFDoc_Dimension) A;
  if (!theLabel.FindAttribute(XCAFDoc_Dimension::GetID(), A)) {
    A = new XCAFDoc_Dimension();
    theLabel.AddAttribute(A);
  }
  return A;
}

//=======================================================================
//function : SetObject
//purpose  : 
//=======================================================================
void XCAFDoc_Dimension::SetObject (const Handle(XCAFDimTolObjects_DimensionObject)& theObject)
{
  Backup();

  Handle(TDataStd_Integer) aType;
  if(!Label().FindChild(ChildLab_Type).FindAttribute(TDataStd_Integer::GetID(), aType))
  {
    aType = new TDataStd_Integer();
    Label().FindChild(ChildLab_Type).AddAttribute(aType);
  }
  aType->Set(theObject->GetType());

  if(theObject->GetValues().IsNull())
  {
    Label().FindChild(ChildLab_Value).ForgetAllAttributes();
  }
  else
  {
    Handle(TDataStd_RealArray) aVal;
    if(!Label().FindChild(ChildLab_Value).FindAttribute(TDataStd_RealArray::GetID(), aVal))
    {
      aVal = new TDataStd_RealArray();
      Label().FindChild(ChildLab_Value).AddAttribute(aVal);
    }
    aVal->ChangeArray(theObject->GetValues());
  }

  Handle(TDataStd_Integer) aQualifier;
  if(!Label().FindChild(ChildLab_Qualifier).FindAttribute(TDataStd_Integer::GetID(), aQualifier))
  {
    aQualifier = new TDataStd_Integer();
    Label().FindChild(ChildLab_Qualifier).AddAttribute(aQualifier);
  }
  aQualifier->Set(theObject->GetQualifier());
 
  Standard_Boolean aH;
  XCAFDimTolObjects_DimensionFormVariance aF;
  XCAFDimTolObjects_DimensionGrade aG;
  theObject->GetClassOfTolerance(aH,aF,aG);
  Handle(TDataStd_IntegerArray) aClass;
  if(!Label().FindChild(ChildLab_Class).FindAttribute(TDataStd_IntegerArray::GetID(), aClass))
  {
    aClass = new TDataStd_IntegerArray();
    Label().FindChild(ChildLab_Class).AddAttribute(aClass);
  }
  Handle(TColStd_HArray1OfInteger) anArrI = new TColStd_HArray1OfInteger(1,3);
  anArrI->SetValue(1,aH);
  anArrI->SetValue(2,aF);
  anArrI->SetValue(3,aG);
  aClass->ChangeArray(anArrI);

  Standard_Integer aL, aR;
  theObject->GetNbOfDecimalPlaces(aL, aR);
  Handle(TDataStd_IntegerArray) aDec;
  if(!Label().FindChild(ChildLab_Dec).FindAttribute(TDataStd_IntegerArray::GetID(), aDec))
  {
    aDec = new TDataStd_IntegerArray();
    Label().FindChild(ChildLab_Dec).AddAttribute(aDec);
  }
  anArrI = new TColStd_HArray1OfInteger(1,2);
  anArrI->SetValue(1,aL);
  anArrI->SetValue(2,aR);
  aDec->ChangeArray(anArrI);
 
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
    anArrI = new TColStd_HArray1OfInteger(1,theObject->GetModifiers().Length());
    for(Standard_Integer i = 1; i <= theObject->GetModifiers().Length(); i++)
      anArrI->SetValue(i,theObject->GetModifiers().Value(i));
    aModifiers->ChangeArray(anArrI);
  }

  if(!theObject->GetPath().IsNull())
  {
  TNaming_Builder tnBuild(Label().FindChild(ChildLab_Path));
  tnBuild.Generated(theObject->GetPath());
  }
  else
  {
    Label().FindChild(ChildLab_Path).ForgetAllAttributes();
  }

  Handle(TDataStd_RealArray) aDir;
  if(!Label().FindChild(ChildLab_Dir).FindAttribute(TDataStd_RealArray::GetID(), aDir))
  {
    aDir = new TDataStd_RealArray();
    Label().FindChild(ChildLab_Dir).AddAttribute(aDir);
  }
  gp_Dir aD;
  theObject->GetDirection(aD);
  Handle(TColStd_HArray1OfReal) anArrR = new TColStd_HArray1OfReal(1,3);
  anArrR->SetValue(1,aD.X());
  anArrR->SetValue(2,aD.Y());
  anArrR->SetValue(3,aD.Z());
  aDir->ChangeArray(anArrR);

  Handle(TDataStd_RealArray) aPnts;
  if(!Label().FindChild(ChildLab_Pnts).FindAttribute(TDataStd_RealArray::GetID(), aPnts))
  {
    aPnts = new TDataStd_RealArray();
    Label().FindChild(ChildLab_Pnts).AddAttribute(aPnts);
  }
  Handle(TColgp_HArray1OfPnt) aP = theObject->GetPoints();
  if(!aP.IsNull() && aP->Length() > 0)
  {
    anArrR = new TColStd_HArray1OfReal(1,6);
    anArrR->SetValue(1,aP->Value(1).X());
    anArrR->SetValue(2,aP->Value(1).Y());
    anArrR->SetValue(3,aP->Value(1).Z());
    anArrR->SetValue(4,aP->Value(2).X());
    anArrR->SetValue(5,aP->Value(2).Y());
    anArrR->SetValue(6,aP->Value(2).Z());
    aPnts->ChangeArray(anArrR);
  }
}

//=======================================================================
//function : GetObject
//purpose  : 
//=======================================================================
Handle(XCAFDimTolObjects_DimensionObject) XCAFDoc_Dimension::GetObject()  const
{
  Handle(XCAFDimTolObjects_DimensionObject) anObj = new XCAFDimTolObjects_DimensionObject();

  Handle(TDataStd_Integer) aType;
  if(Label().FindChild(ChildLab_Type).FindAttribute(TDataStd_Integer::GetID(), aType))
  {
    anObj->SetType((XCAFDimTolObjects_DimensionType)aType->Get());
  }

  Handle(TDataStd_RealArray) aVal;
  if(Label().FindChild(ChildLab_Value).FindAttribute(TDataStd_RealArray::GetID(), aVal) 
     && !aVal->Array().IsNull())
  {
    anObj->SetValues(aVal->Array());
  }

  Handle(TDataStd_Integer) aQualifier;
  if(Label().FindChild(ChildLab_Qualifier).FindAttribute(TDataStd_Integer::GetID(), aQualifier))
  {
    anObj->SetQualifier((XCAFDimTolObjects_DimensionQualifier)aQualifier->Get());
  }
 
  Handle(TDataStd_IntegerArray) aClass;
  if(Label().FindChild(ChildLab_Class).FindAttribute(TDataStd_IntegerArray::GetID(), aClass) 
     && !aClass->Array().IsNull() && aClass->Array()->Length() > 0)
  {
    anObj->SetClassOfTolerance(aClass->Array()->Value(1), (XCAFDimTolObjects_DimensionFormVariance)aClass->Array()->Value(2), (XCAFDimTolObjects_DimensionGrade)aClass->Array()->Value(3));
  }

  Handle(TDataStd_IntegerArray) aDec;
  if(Label().FindChild(ChildLab_Dec).FindAttribute(TDataStd_IntegerArray::GetID(), aDec)
     && !aDec->Array().IsNull() && aDec->Array()->Length() > 0)
  {
      anObj->SetNbOfDecimalPlaces(aDec->Array()->Value(1), aDec->Array()->Value(2));
  }
 
  Handle(TDataStd_IntegerArray) aModifiers;
  if(Label().FindChild(ChildLab_Modifiers).FindAttribute(TDataStd_IntegerArray::GetID(), aModifiers) 
     && !aModifiers->Array().IsNull())
  {
    XCAFDimTolObjects_DimensionModifiersSequence aM;
    for(Standard_Integer i = 1; i <= aModifiers->Array()->Length(); i++)
      aM.Append((XCAFDimTolObjects_DimensionModif)aModifiers->Array()->Value(i));
    anObj->SetModifiers(aM);
  }

  Handle(TNaming_NamedShape) aShape;
  if(Label().FindChild(ChildLab_Path).FindAttribute(TNaming_NamedShape::GetID(), aShape)
    && !aShape.IsNull())
  {
    anObj->SetPath(TopoDS::Edge(aShape->Get()));
  }


  Handle(TDataStd_RealArray) aDir;
  if(Label().FindChild(ChildLab_Dir).FindAttribute(TDataStd_RealArray::GetID(), aDir)
     && !aDir->Array().IsNull() && aDir->Array()->Length() > 0)
  {
    gp_Dir aD(aDir->Array()->Value(1), aDir->Array()->Value(2), aDir->Array()->Value(3));
    anObj->SetDirection(aD);
  }

  Handle(TDataStd_RealArray) aPnts;
  if(Label().FindChild(ChildLab_Pnts).FindAttribute(TDataStd_RealArray::GetID(), aPnts) 
     && !aPnts->Array().IsNull() && aPnts->Array()->Length() > 0)
  {
    Handle(TColgp_HArray1OfPnt) aP = new TColgp_HArray1OfPnt(1,2);
    aP->SetValue(1, gp_Pnt(aPnts->Array()->Value(1), aPnts->Array()->Value(2), aPnts->Array()->Value(3)));
    aP->SetValue(2, gp_Pnt(aPnts->Array()->Value(4), aPnts->Array()->Value(5), aPnts->Array()->Value(6)));
    anObj->SetPoints(aP);
  }

  return anObj;
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_Dimension::ID() const
{
  return GetID();
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void XCAFDoc_Dimension::Restore(const Handle(TDF_Attribute)& /*With*/) 
{
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) XCAFDoc_Dimension::NewEmpty() const
{
  return new XCAFDoc_Dimension();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void XCAFDoc_Dimension::Paste(const Handle(TDF_Attribute)& /*Into*/,
                           const Handle(TDF_RelocationTable)& /*RT*/) const
{
}
