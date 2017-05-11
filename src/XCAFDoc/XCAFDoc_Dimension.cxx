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
#include <TDF_ChildIterator.hxx>
#include <XCAFDoc.hxx>
#include <TDataStd_TreeNode.hxx>
#include <Precision.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_ExtStringArray.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_RealArray.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TopoDS.hxx>
#include <XCAFDimTolObjects_DimensionObject.hxx>
#include <TNaming_Tool.hxx>
#include <TDataStd_Name.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_Dimension,TDF_Attribute)

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
  ChildLab_Pnt1,
  ChildLab_Pnt2,
  ChildLab_PlaneLoc,
  ChildLab_PlaneN,
  ChildLab_PlaneRef,
  ChildLab_PntText,
  ChildLab_Presentation,
  ChildLab_Descriptions,
  ChildLab_DescriptionNames
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

  //Label().ForForgetAllAttributes();
  TDF_ChildIterator anIter(Label());
  for(;anIter.More(); anIter.Next())
  {
    anIter.Value().ForgetAllAttributes();
  }
  Handle(TDataStd_Integer) aType = TDataStd_Integer::Set(Label().FindChild(ChildLab_Type), theObject->GetType());

  if(!theObject->GetValues().IsNull())
  {
    Handle(TDataStd_RealArray) aVal = TDataStd_RealArray::Set(Label().FindChild(ChildLab_Value), theObject->GetValues()->Lower(), 
                                      theObject->GetValues()->Lower() + theObject->GetValues()->Length() - 1);
    if(!aVal.IsNull())
      aVal->ChangeArray(theObject->GetValues());
  }

  Handle(TDataStd_Integer) aQualifier = TDataStd_Integer::Set(Label().FindChild(ChildLab_Qualifier), theObject->GetQualifier());
 
  Standard_Boolean aH;
  XCAFDimTolObjects_DimensionFormVariance aF;
  XCAFDimTolObjects_DimensionGrade aG;
  theObject->GetClassOfTolerance(aH,aF,aG);
  Handle(TColStd_HArray1OfInteger) anArrI;
  if(aF != XCAFDimTolObjects_DimensionFormVariance_None)
  {
    anArrI = new TColStd_HArray1OfInteger(1,3);
    anArrI->SetValue(1,aH);
    anArrI->SetValue(2,aF);
    anArrI->SetValue(3,aG);
    Handle(TDataStd_IntegerArray) aClass = TDataStd_IntegerArray::Set(Label().FindChild(ChildLab_Class), 1, 3);
    if(!aClass.IsNull())
      aClass->ChangeArray(anArrI);
  }

  Standard_Integer aL, aR;
  theObject->GetNbOfDecimalPlaces(aL, aR);
  if (aL > 0 || aR > 0)
  {
    anArrI = new TColStd_HArray1OfInteger(1,2);
    anArrI->SetValue(1,aL);
    anArrI->SetValue(2,aR);
    Handle(TDataStd_IntegerArray) aDec = TDataStd_IntegerArray::Set(Label().FindChild(ChildLab_Dec), 1, 2);
    if(!aDec.IsNull())
      aDec->ChangeArray(anArrI);
  }

  if(theObject->GetModifiers().Length() > 0)
  {
    anArrI = new TColStd_HArray1OfInteger(1,theObject->GetModifiers().Length());
    for(Standard_Integer i = 1; i <= theObject->GetModifiers().Length(); i++)
      anArrI->SetValue(i,theObject->GetModifiers().Value(i));
    Handle(TDataStd_IntegerArray) aModifiers = TDataStd_IntegerArray::Set(Label().FindChild(ChildLab_Modifiers), 
                                               1, theObject->GetModifiers().Length());
    if(!aModifiers.IsNull())
      aModifiers->ChangeArray(anArrI);
  }

  if(!theObject->GetPath().IsNull())
  {
    TNaming_Builder tnBuild(Label().FindChild(ChildLab_Path));
    tnBuild.Generated(theObject->GetPath());
  }

  Handle(TColStd_HArray1OfReal) anArrR;
  if(theObject->GetType() == XCAFDimTolObjects_DimensionType_Location_Oriented)
  {
    gp_Dir aD;
    theObject->GetDirection(aD);
    anArrR = new TColStd_HArray1OfReal(1,3);
    anArrR->SetValue(1,aD.X());
    anArrR->SetValue(2,aD.Y());
    anArrR->SetValue(3,aD.Z());
    Handle(TDataStd_RealArray) aDir = TDataStd_RealArray::Set(Label().FindChild(ChildLab_Dir), 1, 3);
    if(!aDir.IsNull())
      aDir->ChangeArray(anArrR);
  }

  if (theObject->HasPoint())
  {
    gp_Pnt aPnt1 = theObject->GetPoint();

    Handle(TColStd_HArray1OfReal) aPntArr = new TColStd_HArray1OfReal(1, 3);
    for (Standard_Integer i = 1; i <= 3; i++)
      aPntArr->SetValue(i, aPnt1.Coord(i));
    Handle(TDataStd_RealArray) aPnt = TDataStd_RealArray::Set(Label().FindChild(ChildLab_Pnt1), 1, 3);
    if(!aPnt.IsNull())
      aPnt->ChangeArray(aPntArr);
  }

  if (theObject->HasPoint2())
  {
    gp_Pnt aPnt2 = theObject->GetPoint2();

    Handle(TColStd_HArray1OfReal) aPntArr = new TColStd_HArray1OfReal(1, 3);
    for (Standard_Integer i = 1; i <= 3; i++)
      aPntArr->SetValue(i, aPnt2.Coord(i));
    Handle(TDataStd_RealArray) aPnt = TDataStd_RealArray::Set(Label().FindChild(ChildLab_Pnt2), 1, 3);
    if(!aPnt.IsNull())
      aPnt->ChangeArray(aPntArr);
  }

  if (theObject->HasPlane())
  {
    gp_Ax2 anAx = theObject->GetPlane();

    Handle(TColStd_HArray1OfReal) aLocArr = new TColStd_HArray1OfReal(1, 3);
    for (Standard_Integer i = 1; i <= 3; i++)
      aLocArr->SetValue(i, anAx.Location().Coord(i));
    Handle(TDataStd_RealArray) aLoc = TDataStd_RealArray::Set(Label().FindChild(ChildLab_PlaneLoc), 1, 3);
    if(!aLoc.IsNull())
      aLoc->ChangeArray(aLocArr);

    Handle(TColStd_HArray1OfReal) aNArr = new TColStd_HArray1OfReal(1, 3);
    for (Standard_Integer i = 1; i <= 3; i++)
      aNArr->SetValue(i, anAx.Direction().Coord(i));
    Handle(TDataStd_RealArray) aN = TDataStd_RealArray::Set(Label().FindChild(ChildLab_PlaneN), 1, 3);
    if(!aN.IsNull())
      aN->ChangeArray(aNArr);

    Handle(TColStd_HArray1OfReal) aRArr = new TColStd_HArray1OfReal(1, 3);
    for (Standard_Integer i = 1; i <= 3; i++)
      aRArr->SetValue(i, anAx.XDirection().Coord(i));
    Handle(TDataStd_RealArray) aRAtt = TDataStd_RealArray::Set(Label().FindChild(ChildLab_PlaneRef), 1, 3);
    if(!aRAtt.IsNull())
      aRAtt->ChangeArray(aRArr);
  }

  if (theObject->HasTextPoint())
  {
    gp_Pnt aPntText = theObject->GetPointTextAttach();

    Handle(TColStd_HArray1OfReal) aLocArr = new TColStd_HArray1OfReal(1, 3);
    for (Standard_Integer i = 1; i <= 3; i++)
      aLocArr->SetValue(i, aPntText.Coord(i));
    Handle(TDataStd_RealArray) aLoc = TDataStd_RealArray::Set(Label().FindChild(ChildLab_PntText), 1, 3);
    if(!aLoc.IsNull())
      aLoc->ChangeArray(aLocArr);
  }

  TopoDS_Shape aPresentation = theObject->GetPresentation();
  if( !aPresentation.IsNull())
  {
    TDF_Label aLPres = Label().FindChild( ChildLab_Presentation);
    TNaming_Builder tnBuild(aLPres);
    tnBuild.Generated(aPresentation);
    Handle(TCollection_HAsciiString) aName =  theObject->GetPresentationName();
    if( !aName.IsNull() )
    {
      TCollection_ExtendedString str ( aName->String() );
      TDataStd_Name::Set ( aLPres, str );
    }
  }

  if (theObject->HasDescriptions())
  {
    Handle(TColStd_HArray1OfExtendedString) aDescrArr = new TColStd_HArray1OfExtendedString(1, theObject->NbDescriptions());
    Handle(TColStd_HArray1OfExtendedString) aDescrNameArr = new TColStd_HArray1OfExtendedString(1, theObject->NbDescriptions());
    for (Standard_Integer i = 0; i < theObject->NbDescriptions(); i++) {
      TCollection_ExtendedString aDescr(theObject->GetDescription(i)->String());
      aDescrArr->SetValue(i + 1, aDescr);
      TCollection_ExtendedString aDescrName(theObject->GetDescriptionName(i)->String());
      aDescrNameArr->SetValue(i + 1, aDescrName);
    }
    Handle(TDataStd_ExtStringArray) aDescriptions = TDataStd_ExtStringArray::Set(Label().FindChild(ChildLab_Descriptions),
                                                    1, theObject->NbDescriptions());
    Handle(TDataStd_ExtStringArray) aDescriptionNames = TDataStd_ExtStringArray::Set(Label().FindChild(ChildLab_DescriptionNames),
                                                        1, theObject->NbDescriptions());
    if(!aDescriptions.IsNull())
      aDescriptions->ChangeArray(aDescrArr);
    if(!aDescriptionNames.IsNull())
      aDescriptionNames->ChangeArray(aDescrNameArr);
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
    anObj->SetClassOfTolerance(aClass->Array()->Value(1) != 0, (XCAFDimTolObjects_DimensionFormVariance)aClass->Array()->Value(2), (XCAFDimTolObjects_DimensionGrade)aClass->Array()->Value(3));
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

  Handle(TDataStd_RealArray) aPnt1;
  if(Label().FindChild(ChildLab_Pnt1).FindAttribute(TDataStd_RealArray::GetID(), aPnt1) && aPnt1->Length() == 3 )
  {
    gp_Pnt aP(aPnt1->Value(aPnt1->Lower()), aPnt1->Value(aPnt1->Lower()+1), aPnt1->Value(aPnt1->Lower()+2));
    anObj->SetPoint(aP);
  }

  Handle(TDataStd_RealArray) aPnt2;
  if(Label().FindChild(ChildLab_Pnt2).FindAttribute(TDataStd_RealArray::GetID(), aPnt2) && aPnt2->Length() == 3 )
  {
    gp_Pnt aP(aPnt2->Value(aPnt2->Lower()), aPnt2->Value(aPnt2->Lower()+1), aPnt2->Value(aPnt2->Lower()+2));
    anObj->SetPoint2(aP);
  }

  Handle(TDataStd_RealArray) aLoc, aN, aR;
  if(Label().FindChild(ChildLab_PlaneLoc).FindAttribute(TDataStd_RealArray::GetID(), aLoc) && aLoc->Length() == 3 &&
    Label().FindChild(ChildLab_PlaneN).FindAttribute(TDataStd_RealArray::GetID(), aN) && aN->Length() == 3 &&
    Label().FindChild(ChildLab_PlaneRef).FindAttribute(TDataStd_RealArray::GetID(), aR) && aR->Length() == 3 )
  {
    gp_Pnt aL(aLoc->Value(aLoc->Lower()), aLoc->Value(aLoc->Lower()+1), aLoc->Value(aLoc->Lower()+2));
    gp_Dir aD(aN->Value(aN->Lower()), aN->Value(aN->Lower()+1), aN->Value(aN->Lower()+2));
    gp_Dir aDR(aR->Value(aR->Lower()), aR->Value(aR->Lower()+1), aR->Value(aR->Lower()+2));
    gp_Ax2 anAx(aL, aD, aDR);
    anObj->SetPlane(anAx);
  }

  Handle(TDataStd_RealArray) aPntText;
  if(Label().FindChild(ChildLab_PntText).FindAttribute(TDataStd_RealArray::GetID(), aPntText) && aPntText->Length() == 3 )
  {
    gp_Pnt aP(aPntText->Value(aPntText->Lower()), aPntText->Value(aPntText->Lower()+1), aPntText->Value(aPntText->Lower()+2));
    anObj->SetPointTextAttach(aP);
  }

  Handle(TNaming_NamedShape) aNS;
  TDF_Label aLPres = Label().FindChild( ChildLab_Presentation);
  if ( aLPres.FindAttribute(TNaming_NamedShape::GetID(), aNS) ) 
  {

    TopoDS_Shape aPresentation = TNaming_Tool::GetShape(aNS);
    if( !aPresentation.IsNull())
    {
     
      Handle(TDataStd_Name) aNameAtrr;
      Handle(TCollection_HAsciiString) aPresentName;
      if (aLPres.FindAttribute(TDataStd_Name::GetID(),aNameAtrr))
      {
        const TCollection_ExtendedString& aName = aNameAtrr->Get();

        if( !aName.IsEmpty())
          aPresentName = new TCollection_HAsciiString(aName);
      }

      anObj->SetPresentation(aPresentation, aPresentName);
    }
  }

  Handle(TDataStd_ExtStringArray) aDescriptions, aDescriptionNames;
  if (Label().FindChild(ChildLab_Descriptions).FindAttribute(TDataStd_ExtStringArray::GetID(), aDescriptions) &&
    Label().FindChild(ChildLab_DescriptionNames).FindAttribute(TDataStd_ExtStringArray::GetID(), aDescriptionNames)) {
    for (Standard_Integer i = 1; i <= aDescriptions->Length(); i++) {
      Handle(TCollection_HAsciiString) aDescription, aDescriptionName;
      aDescription = new TCollection_HAsciiString(TCollection_AsciiString(aDescriptions->Value(i)));
      aDescriptionName = new TCollection_HAsciiString(TCollection_AsciiString(aDescriptionNames->Value(i)));
      anObj->AddDescription(aDescription, aDescriptionName);
    }
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
