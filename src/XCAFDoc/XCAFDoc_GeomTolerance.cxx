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
#include <TDF_ChildIterator.hxx>
#include <XCAFDoc.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDataStd_Real.hxx>
#include <XCAFDimTolObjects_GeomToleranceObject.hxx>
#include <TNaming_Tool.hxx>
#include <TNaming_Builder.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDataStd_Name.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_GeomTolerance,TDF_Attribute)

enum ChildLab
{
  ChildLab_Type = 1,
  ChildLab_TypeOfValue,
  ChildLab_Value,
  ChildLab_MatReqModif,
  ChildLab_ZoneModif,
  ChildLab_ValueOfZoneModif,
  ChildLab_Modifiers,
  ChildLab_aMaxValueModif,
  ChildLab_AxisLoc,
  ChildLab_AxisN,
  ChildLab_AxisRef,
  ChildLab_PlaneLoc,
  ChildLab_PlaneN,
  ChildLab_PlaneRef,
  ChildLab_Pnt,
  ChildLab_PntText,
  ChildLab_Presentation
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

  //Label().ForForgetAllAttributes();
  TDF_ChildIterator anIter(Label());
  for(;anIter.More(); anIter.Next())
  {
    anIter.Value().ForgetAllAttributes();
  }

  Handle(TDataStd_Integer) aType = TDataStd_Integer::Set(Label().FindChild(ChildLab_Type), theObject->GetType());

  if(theObject->GetTypeOfValue() != XCAFDimTolObjects_GeomToleranceTypeValue_None)
    Handle(TDataStd_Integer) aTypeOfValue = TDataStd_Integer::Set(Label().FindChild(ChildLab_TypeOfValue), 
                                            theObject->GetTypeOfValue());

  Handle(TDataStd_Real) aValue = TDataStd_Real::Set(Label().FindChild(ChildLab_Value), theObject->GetValue());

  Handle(TDataStd_Integer) aMatReqModif;
  if(theObject->GetMaterialRequirementModifier() != XCAFDimTolObjects_GeomToleranceMatReqModif_None)
    aMatReqModif = TDataStd_Integer::Set(Label().FindChild(ChildLab_MatReqModif), 
                   theObject->GetMaterialRequirementModifier());

  if(theObject->GetZoneModifier() != XCAFDimTolObjects_GeomToleranceZoneModif_None)
    Handle(TDataStd_Integer) aZoneModif = TDataStd_Integer::Set(Label().FindChild(ChildLab_ZoneModif), 
                                          theObject->GetZoneModifier());
  
  if(theObject->GetValueOfZoneModifier() > 0)
    Handle(TDataStd_Real) aValueOfZoneModif = TDataStd_Real::Set(Label().FindChild(ChildLab_ValueOfZoneModif),
                                              theObject->GetValueOfZoneModifier());

  if(theObject->GetModifiers().Length() > 0)
  {
    Handle(TColStd_HArray1OfInteger) anArr = new TColStd_HArray1OfInteger(1,theObject->GetModifiers().Length());
    for(Standard_Integer i = 1; i <= theObject->GetModifiers().Length(); i++)
      anArr->SetValue(i,theObject->GetModifiers().Value(i));
    Handle(TDataStd_IntegerArray) aModifiers = TDataStd_IntegerArray::Set(Label().FindChild(ChildLab_Modifiers), 
                                               1, theObject->GetModifiers().Length());
    if(!aModifiers.IsNull())
      aModifiers->ChangeArray(anArr);
  }

  if(theObject->GetMaxValueModifier() > 0)
    Handle(TDataStd_Real) aMaxValueModif = TDataStd_Real::Set(Label().FindChild(ChildLab_aMaxValueModif),
                                           theObject->GetMaxValueModifier());

  if(theObject->HasAxis())
  {
    gp_Ax2 anAx = theObject->GetAxis();

    Handle(TColStd_HArray1OfReal) aLocArr = new TColStd_HArray1OfReal(1, 3);
    for (Standard_Integer i = 1; i <= 3; i++)
      aLocArr->SetValue(i, anAx.Location().Coord(i));
    Handle(TDataStd_RealArray) aLoc = TDataStd_RealArray::Set(Label().FindChild(ChildLab_AxisLoc), 1, 3);
    if(!aLoc.IsNull())
      aLoc->ChangeArray(aLocArr);

    Handle(TColStd_HArray1OfReal) aNArr = new TColStd_HArray1OfReal(1, 3);
    for (Standard_Integer i = 1; i <= 3; i++)
      aNArr->SetValue(i, anAx.Direction().Coord(i));
    Handle(TDataStd_RealArray) aN = TDataStd_RealArray::Set(Label().FindChild(ChildLab_AxisN), 1, 3);
    if(!aN.IsNull())
      aN->ChangeArray(aNArr);

    Handle(TColStd_HArray1OfReal) aRArr = new TColStd_HArray1OfReal(1, 3);
    for (Standard_Integer i = 1; i <= 3; i++)
      aRArr->SetValue(i, anAx.XDirection().Coord(i));
    Handle(TDataStd_RealArray) aR = TDataStd_RealArray::Set(Label().FindChild(ChildLab_AxisRef), 1, 3);
    if(!aR.IsNull())
      aR->ChangeArray(aRArr);
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
    Handle(TDataStd_RealArray) aR = TDataStd_RealArray::Set(Label().FindChild(ChildLab_PlaneRef), 1, 3);
    if(!aR.IsNull())
      aR->ChangeArray(aRArr);
  }

  if (theObject->HasPoint())
  {
    gp_Pnt aPnt = theObject->GetPoint();

    Handle(TColStd_HArray1OfReal) aLocArr = new TColStd_HArray1OfReal(1, 3);
    for (Standard_Integer i = 1; i <= 3; i++)
      aLocArr->SetValue(i, aPnt.Coord(i));
    Handle(TDataStd_RealArray) aLoc = TDataStd_RealArray::Set(Label().FindChild(ChildLab_Pnt), 1, 3);
    if(!aLoc.IsNull())
      aLoc->ChangeArray(aLocArr);
  }

  if (theObject->HasPointText())
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
  
  Handle(TDataStd_RealArray) aLoc;
  Handle(TDataStd_RealArray) aN;
  Handle(TDataStd_RealArray) aR;
  if(Label().FindChild(ChildLab_AxisLoc).FindAttribute(TDataStd_RealArray::GetID(), aLoc) && aLoc->Length() == 3 &&
    Label().FindChild(ChildLab_AxisN).FindAttribute(TDataStd_RealArray::GetID(), aN) && aN->Length() == 3 &&
    Label().FindChild(ChildLab_AxisRef).FindAttribute(TDataStd_RealArray::GetID(), aR) && aR->Length() == 3 )
  {
    gp_Pnt aL(aLoc->Value(aLoc->Lower()), aLoc->Value(aLoc->Lower()+1), aLoc->Value(aLoc->Lower()+2));
    gp_Dir aD(aN->Value(aN->Lower()), aN->Value(aN->Lower()+1), aN->Value(aN->Lower()+2));
    gp_Dir aDR(aR->Value(aR->Lower()), aR->Value(aR->Lower()+1), aR->Value(aR->Lower()+2));
    gp_Ax2 anAx(aL, aD, aDR);
    anObj->SetAxis(anAx);
  }

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

  Handle(TDataStd_RealArray) aPnt;
  if(Label().FindChild(ChildLab_Pnt).FindAttribute(TDataStd_RealArray::GetID(), aPnt) && aPnt->Length() == 3 )
  {
    gp_Pnt aP(aLoc->Value(aPnt->Lower()), aPnt->Value(aPnt->Lower()+1), aPnt->Value(aPnt->Lower()+2));
    anObj->SetPoint(aP);
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
