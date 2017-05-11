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
#include <TDF_ChildIterator.hxx>
#include <TDF_RelocationTable.hxx>
#include <XCAFDoc_Datum.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Tool.hxx>
#include <TDataStd_Name.hxx>

#include <XCAFDimTolObjects_DatumObject.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_Datum,TDF_Attribute)

enum ChildLab
{
  ChildLab_Name = 1,
  ChildLab_Position,
  ChildLab_Modifiers,
  ChildLab_ModifierWithValue,
  ChildLab_IsDTarget,
  ChildLab_DTargetType,
  ChildLab_AxisLoc,
  ChildLab_AxisN,
  ChildLab_AxisRef,
  ChildLab_DTargetLength,
  ChildLab_DTargetWidth,
  ChildLab_DTargetNumber,
  ChildLab_DatumTarget,
  ChildLab_PlaneLoc,
  ChildLab_PlaneN,
  ChildLab_PlaneRef,
  ChildLab_Pnt,
  ChildLab_PntText,
  ChildLab_Presentation
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
  TDF_ChildIterator anIter(Label());
  for(;anIter.More(); anIter.Next())
  {
    anIter.Value().ForgetAllAttributes();
  }
  if (!theObject->GetName().IsNull() && !theObject->GetName()->IsEmpty())
    Handle(TDataStd_AsciiString) anAttName = TDataStd_AsciiString::Set(Label().FindChild(ChildLab_Name),
                                             theObject->GetName()->String());

  Handle(TDataStd_Integer) aPosition = TDataStd_Integer::Set(Label().FindChild(ChildLab_Position),
                                       theObject->GetPosition());

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

  XCAFDimTolObjects_DatumModifWithValue aM;
  Standard_Real aV;
  theObject->GetModifierWithValue(aM, aV);
  if(aM != XCAFDimTolObjects_DatumModifWithValue_None)
  {
    Handle(TDataStd_Integer) aModifierWithValueM = 
      TDataStd_Integer::Set(Label().FindChild(ChildLab_ModifierWithValue), aM);

    Handle(TDataStd_Real) aModifierWithValueV = 
      TDataStd_Real::Set(Label().FindChild(ChildLab_ModifierWithValue), aV);
  }

  Handle(TDataStd_Integer) aIsTarget = TDataStd_Integer::Set(Label().FindChild(ChildLab_IsDTarget), 
                                       theObject->IsDatumTarget());

  if(theObject->IsDatumTarget())
  {
    Handle(TDataStd_Integer) aType = TDataStd_Integer::Set(Label().FindChild(ChildLab_DTargetType), 
                                     theObject->GetDatumTargetType());

    if(theObject->GetDatumTargetType() == XCAFDimTolObjects_DatumTargetType_Area)
    {
      if(!theObject->GetDatumTarget().IsNull())
      {
        TNaming_Builder tnBuild(Label().FindChild(ChildLab_DatumTarget));
        tnBuild.Generated(theObject->GetDatumTarget());
      }
    }
    else if (theObject->HasDatumTargetParams())
    {
      gp_Ax2 anAx = theObject->GetDatumTargetAxis();

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

      if(theObject->GetDatumTargetType() != XCAFDimTolObjects_DatumTargetType_Point)
      {
        Handle(TDataStd_Real) aLen = TDataStd_Real::Set(Label().FindChild(ChildLab_DTargetLength),
                                     theObject->GetDatumTargetLength());
        if(theObject->GetDatumTargetType() == XCAFDimTolObjects_DatumTargetType_Rectangle)
          Handle(TDataStd_Real) aWidth = TDataStd_Real::Set(Label().FindChild(ChildLab_DTargetWidth),
                                         theObject->GetDatumTargetWidth());
      }
    }
    Handle(TDataStd_Integer) aNum = TDataStd_Integer::Set(Label().FindChild(ChildLab_DTargetNumber), 
                                    theObject->GetDatumTargetNumber());
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

  Handle(TDataStd_Integer) aPosition;
  if(Label().FindChild(ChildLab_Position).FindAttribute(TDataStd_Integer::GetID(), aPosition))
  {
    anObj->SetPosition(aPosition->Get());
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

  Handle(TDataStd_RealArray) aPnt;
  if(Label().FindChild(ChildLab_Pnt).FindAttribute(TDataStd_RealArray::GetID(), aPnt) && aPnt->Length() == 3 )
  {
    gp_Pnt aP(aLoc->Value(aPnt->Lower()), aPnt->Value(aPnt->Lower()+1), aPnt->Value(aPnt->Lower()+2));
    anObj->SetPoint(aP);
  }

  Handle(TDataStd_RealArray) aPntText;
  if(Label().FindChild(ChildLab_PntText).FindAttribute(TDataStd_RealArray::GetID(), aPntText) && aPntText->Length() == 3 )
  {
    gp_Pnt aP (aPntText->Value(aPntText->Lower()), aPntText->Value(aPntText->Lower()+1), aPntText->Value(aPntText->Lower()+2));                          
    anObj->SetPointTextAttach(aP);
  }

  Handle(TDataStd_Integer) aIsDTarget;
  if(Label().FindChild(ChildLab_IsDTarget).FindAttribute(TDataStd_Integer::GetID(), aIsDTarget))
  {
    anObj->IsDatumTarget((aIsDTarget->Get() != 0));
  }
  else
  {
    return anObj;
  }

  if (aIsDTarget->Get() != 0)
  {
    Handle(TDataStd_Integer) aDTargetType;
    if(Label().FindChild(ChildLab_DTargetType).FindAttribute(TDataStd_Integer::GetID(), aDTargetType))
    {
      anObj->SetDatumTargetType((XCAFDimTolObjects_DatumTargetType)aDTargetType->Get());
      if(anObj->GetDatumTargetType() == XCAFDimTolObjects_DatumTargetType_Area)
      {
        Handle(TNaming_NamedShape) aDatumTarget;
        if(Label().FindChild(ChildLab_DatumTarget).FindAttribute(TNaming_NamedShape::GetID(), aDatumTarget))
        {
          anObj->SetDatumTarget(aDatumTarget->Get());
        }
      }
      else
      {
        if(Label().FindChild(ChildLab_AxisLoc).FindAttribute(TDataStd_RealArray::GetID(), aLoc) && aLoc->Length() == 3 &&
          Label().FindChild(ChildLab_AxisN).FindAttribute(TDataStd_RealArray::GetID(), aN) && aN->Length() == 3 &&
          Label().FindChild(ChildLab_AxisRef).FindAttribute(TDataStd_RealArray::GetID(), aR) && aR->Length() == 3 )
        {
          gp_Pnt aL(aLoc->Value(aLoc->Lower()), aLoc->Value(aLoc->Lower()+1), aLoc->Value(aLoc->Lower()+2));
          gp_Dir aD(aN->Value(aN->Lower()), aN->Value(aN->Lower()+1), aN->Value(aN->Lower()+2));
          gp_Dir aDR(aR->Value(aR->Lower()), aR->Value(aR->Lower()+1), aR->Value(aR->Lower()+2));
          gp_Ax2 anAx(aL, aD, aDR);
          anObj->SetDatumTargetAxis(anAx);
        }

        if(anObj->GetDatumTargetType() != XCAFDimTolObjects_DatumTargetType_Point)
        {
          Handle(TDataStd_Real) aLen;
          if(Label().FindChild(ChildLab_DTargetLength).FindAttribute(TDataStd_Real::GetID(), aLen))
          {
            anObj->SetDatumTargetLength(aLen->Get());
          }
          if(anObj->GetDatumTargetType() == XCAFDimTolObjects_DatumTargetType_Rectangle)
          {
            Handle(TDataStd_Real) aWidth;
            if(Label().FindChild(ChildLab_DTargetWidth).FindAttribute(TDataStd_Real::GetID(), aWidth))
            {
              anObj->SetDatumTargetWidth(aWidth->Get());
            }
          }
        }
      }
    }
    Handle(TDataStd_Integer) aNum;
    if(Label().FindChild(ChildLab_DTargetNumber).FindAttribute(TDataStd_Integer::GetID(), aNum))
    {
      anObj->SetDatumTargetNumber(aNum->Get());
    }
    else 
    {
      anObj->SetDatumTargetNumber(0);
    }
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

