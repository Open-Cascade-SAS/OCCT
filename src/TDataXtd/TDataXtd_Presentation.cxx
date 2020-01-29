// Created on: 2015-04-20
// Created by: Alexander Zaikin
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2015 OPEN CASCADE SAS
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

#include <TDataXtd_Presentation.hxx>

#include <TDF_DefaultDeltaOnRemoval.hxx>
#include <TDF_DefaultDeltaOnModification.hxx>
#include <TDF_DeltaOnAddition.hxx>
#include <TDF_Tool.hxx>
#include <TCollection_ExtendedString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataXtd_Presentation,TDF_Attribute)

//=======================================================================
//function : getColorNameFromOldEnum
//purpose  :
//=======================================================================
Quantity_NameOfColor TDataXtd_Presentation::getColorNameFromOldEnum (Standard_Integer theOld)
{
  Standard_Integer aNew = theOld;
  if (theOld >= 43)  { --aNew; }  // Quantity_NOC_CHARTREUSE1 merged to Quantity_NOC_CHARTREUSE1
  if (theOld >= 124) { --aNew; }  // Quantity_NOC_GOLD1 merged to Quantity_NOC_GOLD
  if (theOld >= 136 && theOld < 223)
  {
    if (theOld == 146) { return Quantity_NOC_GRAY2; }
    if (theOld == 157) { return Quantity_NOC_GRAY3; }
    if (theOld == 168) { return Quantity_NOC_GRAY4; }
    if (theOld == 179) { return Quantity_NOC_GRAY5; }
    if (theOld == 190) { return Quantity_NOC_GRAY6; }
    if (theOld == 201) { return Quantity_NOC_GRAY7; }
    if (theOld == 212) { return Quantity_NOC_GRAY8; }
    if (theOld == 222) { return Quantity_NOC_GRAY9; }

    if      (theOld >  212) { aNew += 1; } // old Quantity_NOC_GRAY8
    else if (theOld >  201) { aNew += 2; } // old Quantity_NOC_GRAY7
    else if (theOld >  190) { aNew += 3; } // old Quantity_NOC_GRAY6
    else if (theOld >  179) { aNew += 4; } // old Quantity_NOC_GRAY5
    else if (theOld >  168) { aNew += 5; } // old Quantity_NOC_GRAY4
    else if (theOld >  157) { aNew += 6; } // old Quantity_NOC_GRAY3
    else if (theOld >  146) { aNew += 7; } // old Quantity_NOC_GRAY2
    else                    { aNew += 8; }
  }

  if (theOld >  228) { aNew+=3; } // moved Quantity_NOC_GRAY96, 97, 98
  if (theOld >= 230) { --aNew; }  // Quantity_NOC_GREEN1 merged to Quantity_NOC_GREEN
  if (theOld == 235) { return Quantity_NOC_GRAY97; }
  if (theOld == 236) { return Quantity_NOC_GRAY98; }
  if (theOld == 237) { return Quantity_NOC_GRAY99; }
  if (theOld >= 238) { aNew-=3; } // moved Quantity_NOC_GRAY96, 97, 98
  if (theOld >= 360) { --aNew; }  // Quantity_NOC_ORANGE1 merged to Quantity_NOC_ORANGE
  if (theOld >= 365) { --aNew; }  // Quantity_NOC_ORANGERED1 merged to Quantity_NOC_ORANGERED
  if (theOld >= 413) { --aNew; }  // Quantity_NOC_RED1 merged to Quantity_NOC_RED
  if (theOld >= 489) { --aNew; }  // Quantity_NOC_TOMATO1 merged to Quantity_NOC_TOMATO
  if (theOld >= 511) { --aNew; }  // Quantity_NOC_YELLOW1 merged to Quantity_NOC_YELLOW
  if (aNew > Quantity_NOC_WHITE) { return Quantity_NOC_WHITE; }
  return (Quantity_NameOfColor )aNew;
}

//=======================================================================
//function : getOldColorNameFromNewEnum
//purpose  :
//=======================================================================
Standard_Integer TDataXtd_Presentation::getOldColorNameFromNewEnum (Quantity_NameOfColor theNew)
{
  Standard_Integer anOld = theNew;
  if (theNew >= Quantity_NOC_CHARTREUSE2)  { ++anOld; }  // Quantity_NOC_CHARTREUSE1 merged to Quantity_NOC_CHARTREUSE1
  if (theNew >= Quantity_NOC_GOLD2)        { ++anOld; }  // Quantity_NOC_GOLD1 merged to Quantity_NOC_GOLD
  if (theNew >= Quantity_NOC_GRAY2 && theNew <= Quantity_NOC_GRAY89)
  {
    if (theNew == Quantity_NOC_GRAY2) { return 146; }
    if (theNew == Quantity_NOC_GRAY3) { return 157; }
    if (theNew == Quantity_NOC_GRAY4) { return 168; }
    if (theNew == Quantity_NOC_GRAY5) { return 179; }
    if (theNew == Quantity_NOC_GRAY6) { return 190; }
    if (theNew == Quantity_NOC_GRAY7) { return 201; }
    if (theNew == Quantity_NOC_GRAY8) { return 212; }
    if (theNew == Quantity_NOC_GRAY9) { return 222; }

    if      (theNew >= Quantity_NOC_GRAY80) { anOld -= 1; }
    else if (theNew >= Quantity_NOC_GRAY70) { anOld -= 2; }
    else if (theNew >= Quantity_NOC_GRAY60) { anOld -= 3; }
    else if (theNew >= Quantity_NOC_GRAY50) { anOld -= 4; }
    else if (theNew >= Quantity_NOC_GRAY40) { anOld -= 5; }
    else if (theNew >= Quantity_NOC_GRAY30) { anOld -= 6; }
    else if (theNew >= Quantity_NOC_GRAY20) { anOld -= 7; }
    else                                    { anOld -= 8; }
  }
  if (theNew == Quantity_NOC_GRAY97) { return 235; }
  if (theNew == Quantity_NOC_GRAY98) { return 236; }
  if (theNew == Quantity_NOC_GRAY99) { return 237; }

  if (theNew >= Quantity_NOC_GREEN)      { anOld -= 3; } // moved Quantity_NOC_GRAY96, 97, 98
  if (theNew >= Quantity_NOC_GREEN2)     { ++anOld; }    // Quantity_NOC_GREEN1 merged to Quantity_NOC_GREEN
  if (theNew >= Quantity_NOC_HONEYDEW)   { anOld += 3; } // moved Quantity_NOC_GRAY96, 97, 98
  if (theNew >= Quantity_NOC_ORANGE2)    { ++anOld; }    // Quantity_NOC_ORANGE1 merged to Quantity_NOC_ORANGE
  if (theNew >= Quantity_NOC_ORANGERED2) { ++anOld; }    // Quantity_NOC_ORANGERED1 merged to Quantity_NOC_ORANGERED
  if (theNew >= Quantity_NOC_RED2)       { ++anOld; }    // Quantity_NOC_RED1 merged to Quantity_NOC_RED
  if (theNew >= Quantity_NOC_TOMATO2)    { ++anOld; }    // Quantity_NOC_TOMATO1 merged to Quantity_NOC_TOMATO
  if (theNew >= Quantity_NOC_YELLOW2)    { ++anOld; }    // Quantity_NOC_YELLOW1 merged to Quantity_NOC_YELLOW
  return anOld;
}

//=======================================================================
//function : TDataXtd_Presentation
//purpose  : Default constructor.
//=======================================================================
TDataXtd_Presentation::TDataXtd_Presentation()
: myDriverGUID           ("00000000-0000-0000-0000-000000000000"),
  myColor                (Quantity_NOC_WHITE),
  myMaterialIndex        (0),
  myMode                 (0),
  myTransparency         (0.0),
  myWidth                (0.0),
  myIsDisplayed          (Standard_False),
  myHasOwnColor          (Standard_False),
  myHasOwnMaterial       (Standard_False),
  myHasOwnTransparency   (Standard_False),
  myHasOwnWidth          (Standard_False),
  myHasOwnMode           (Standard_False),
  myHasOwnSelectionMode  (Standard_False)
{}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle(TDataXtd_Presentation) TDataXtd_Presentation::Set (const TDF_Label& theLabel,
                                                                  const Standard_GUID& theDriverId)
{
  Handle(TDataXtd_Presentation) aPresentation;

  if ( !theLabel.FindAttribute(TDataXtd_Presentation::GetID(), aPresentation) )
  {
    aPresentation = new TDataXtd_Presentation();
    theLabel.AddAttribute(aPresentation);
  }

  aPresentation->SetDriverGUID(theDriverId);
  return aPresentation;
}

//=======================================================================
//function : Unset
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::Unset(const TDF_Label& theLabel)
{
  Handle(TDataXtd_Presentation) aPresentation;
  if (theLabel.FindAttribute(TDataXtd_Presentation::GetID(), aPresentation))
    theLabel.ForgetAttribute(aPresentation);
}


//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataXtd_Presentation::GetID()
{
  static Standard_GUID TDataXtd_PresentationID("04fb4d00-5690-11d1-8940-080009dc3333");
  return TDataXtd_PresentationID;
}


//=======================================================================
//function : ID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataXtd_Presentation::ID() const
{
  return GetID();
}


//=======================================================================
//function :GetDriverGUID
//purpose  : 
//=======================================================================
Standard_GUID TDataXtd_Presentation::GetDriverGUID() const
{
  return myDriverGUID;
}


//=======================================================================
//function :SetDriverGUID
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::SetDriverGUID(const Standard_GUID& theGUID)
{
  if ( myDriverGUID != theGUID )
  {
    Backup();
    myDriverGUID = theGUID;
  }
}


//=======================================================================
//function : IsDisplayed
//purpose  : 
//=======================================================================
Standard_Boolean TDataXtd_Presentation::IsDisplayed() const
{
  return myIsDisplayed;
}


//=======================================================================
//function : HasOwnMaterial
//purpose  : 
//=======================================================================
Standard_Boolean TDataXtd_Presentation::HasOwnMaterial() const
{
  return myHasOwnMaterial;
}


//=======================================================================
//function : HasOwnTransparency
//purpose  : 
//=======================================================================
Standard_Boolean TDataXtd_Presentation::HasOwnTransparency() const
{
  return myHasOwnTransparency;
}


//=======================================================================
//function : HasOwnColor
//purpose  : 
//=======================================================================
Standard_Boolean TDataXtd_Presentation::HasOwnColor() const
{
  return myHasOwnColor;
}


//=======================================================================
//function : HasOwnWidth
//purpose  : 
//=======================================================================
Standard_Boolean TDataXtd_Presentation::HasOwnWidth() const
{
  return myHasOwnWidth;
}


//=======================================================================
//function : HasOwnMode
//purpose  : 
//=======================================================================
Standard_Boolean TDataXtd_Presentation::HasOwnMode() const
{
  return myHasOwnMode;
}


//=======================================================================
//function : HasOwnSelectionMode
//purpose  : 
//=======================================================================
Standard_Boolean TDataXtd_Presentation::HasOwnSelectionMode() const
{
  return myHasOwnSelectionMode;
}


//=======================================================================
//function : SetDisplayed
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::SetDisplayed(const Standard_Boolean theIsDisplayed)
{
  if (myIsDisplayed != theIsDisplayed)
  {
    Backup();
    myIsDisplayed = theIsDisplayed;
  }
}


//=======================================================================
//function : SetMaterialIndex
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::SetMaterialIndex(const Standard_Integer theMaterialIndex)
{
  if (! myHasOwnMaterial || myMaterialIndex != theMaterialIndex)
  {
    Backup();
    myMaterialIndex = theMaterialIndex;
    myHasOwnMaterial = Standard_True;
  }
}


//=======================================================================
//function : SetTransparency
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::SetTransparency(const Standard_Real theValue)
{
  if (! myHasOwnTransparency || myTransparency != theValue)
  {
    Backup();
    myTransparency = theValue;
    myHasOwnTransparency = Standard_True;
  }
}


//=======================================================================
//function : SetColor
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::SetColor(const Quantity_NameOfColor theColor)
{
  if (! myHasOwnColor || myColor != theColor)
  {
    Backup();
    myColor = theColor;
    myHasOwnColor = Standard_True;
  }
}


//=======================================================================
//function : SetWidth
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::SetWidth(const Standard_Real theWidth)
{
  if (! myHasOwnWidth || myWidth != theWidth)
  {
    Backup();
    myWidth = theWidth;
    myHasOwnWidth = Standard_True;
  }
}


//=======================================================================
//function : SetMode
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::SetMode(const Standard_Integer theMode)
{
  if (! myHasOwnMode || myMode != theMode)
  {
    Backup();
    myMode = theMode;
    myHasOwnMode = Standard_True;
  }
}

//=======================================================================
//function : GetNbSelectionModes
//purpose  : Returns the number of selection modes of the attribute.
//         : It starts with 1 .. GetNbSelectionModes().
//=======================================================================
Standard_EXPORT Standard_Integer TDataXtd_Presentation::GetNbSelectionModes() const
{
  return mySelectionModes.Extent();
}

//=======================================================================
//function : SetSelectionMode
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::SetSelectionMode(const Standard_Integer theSelectionMode, const Standard_Boolean theTransaction)
{
  if (!myHasOwnSelectionMode || GetNbSelectionModes() > 1 ||
      (GetNbSelectionModes() > 0 && mySelectionModes.First() != theSelectionMode))
  {
    if (theTransaction)
      Backup();
    mySelectionModes.Clear();
    mySelectionModes.Append(theSelectionMode);
    myHasOwnSelectionMode = Standard_True;
  }
}

//=======================================================================
//function : AddSelectionMode
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::AddSelectionMode(const Standard_Integer theSelectionMode, const Standard_Boolean theTransaction)
{
  if (!myHasOwnSelectionMode || !HasSelectionMode(theSelectionMode))
  {
    if (theTransaction)
      Backup();
    mySelectionModes.Append(theSelectionMode);
    myHasOwnSelectionMode = Standard_True;
  }
}

//=======================================================================
//function : MaterialIndex
//purpose  : 
//=======================================================================
Standard_Integer TDataXtd_Presentation::MaterialIndex() const
{
  return myMaterialIndex;
}


//=======================================================================
//function : Transparency
//purpose  : 
//=======================================================================
Standard_Real TDataXtd_Presentation::Transparency() const
{
  return myTransparency;
}


//=======================================================================
//function : Color
//purpose  : 
//=======================================================================
Quantity_NameOfColor TDataXtd_Presentation::Color() const
{
  return myColor;
}


//=======================================================================
//function : Width
//purpose  : 
//=======================================================================
Standard_Real TDataXtd_Presentation::Width() const
{
   return myWidth;
}


//=======================================================================
//function : Mode
//purpose  : 
//=======================================================================
Standard_Integer TDataXtd_Presentation::Mode() const
{
  return myMode;
}


//=======================================================================
//function : SelectionMode
//purpose  : 
//=======================================================================
Standard_Integer TDataXtd_Presentation::SelectionMode(const Standard_Integer index) const
{
  Standard_Integer aSelectionMode(0);
  TColStd_ListOfInteger::Iterator itr(mySelectionModes);
  for (Standard_Integer i = 1; itr.More() && i <= index; itr.Next(), i++)
  {
    if (i == index)
      aSelectionMode = itr.Value();
  }
  return aSelectionMode;
}


//=======================================================================
//function : UnsetMaterial
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::UnsetMaterial()
{
  if (myHasOwnMaterial)
  {
    Backup();
    myHasOwnMaterial = Standard_False;
  }
}


//=======================================================================
//function : UnsetTransparency
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::UnsetTransparency()
{
  if (myHasOwnTransparency)
  {
    Backup();
    myHasOwnTransparency = Standard_False;
  }
}


//=======================================================================
//function : UnsetColor
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::UnsetColor()
{
  if (myHasOwnColor)
  {
    Backup();
    myHasOwnColor = Standard_False;
  }
}


//=======================================================================
//function : UnsetWidth
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::UnsetWidth()
{
  if (myHasOwnWidth)
  {
    Backup();
    myHasOwnWidth = Standard_False;
  }
}


//=======================================================================
//function : UnsetMode
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::UnsetMode()
{
  if (myHasOwnMode)
  {
    Backup();
    myHasOwnMode = Standard_False;
  }
}


//=======================================================================
//function : UnsetSelectionMode
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::UnsetSelectionMode()
{
  if (myHasOwnSelectionMode)
  {
    Backup();
    myHasOwnSelectionMode = Standard_False;
    mySelectionModes.Clear();
  }
}


//=======================================================================
//function : BackupCopy
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) TDataXtd_Presentation::BackupCopy() const
{
  Handle(TDataXtd_Presentation) aCopy = new TDataXtd_Presentation;

  aCopy->myIsDisplayed   = myIsDisplayed;
  aCopy->myDriverGUID    = myDriverGUID;
  aCopy->mySelectionModes= mySelectionModes;
  aCopy->myTransparency  = myTransparency;
  aCopy->myColor         = myColor;
  aCopy->myMode          = myMode;
  aCopy->myWidth         = myWidth;
  aCopy->myMaterialIndex = myMaterialIndex;

  aCopy->myHasOwnColor         = myHasOwnColor;
  aCopy->myHasOwnMaterial      = myHasOwnMaterial;
  aCopy->myHasOwnWidth         = myHasOwnWidth;
  aCopy->myHasOwnMode          = myHasOwnMode;
  aCopy->myHasOwnTransparency  = myHasOwnTransparency;
  aCopy->myHasOwnSelectionMode = myHasOwnSelectionMode;

  return aCopy;
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) TDataXtd_Presentation::NewEmpty() const
{
  return new TDataXtd_Presentation();
}


//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::Restore(const Handle(TDF_Attribute)& theAttribute)
{
  Handle(TDataXtd_Presentation) aPresentation =
    Handle(TDataXtd_Presentation)::DownCast(theAttribute);

  myHasOwnMaterial = aPresentation->HasOwnMaterial();
  myMaterialIndex = aPresentation->MaterialIndex();

  myHasOwnColor = aPresentation->HasOwnColor();
  myColor = aPresentation->Color();

  myHasOwnWidth = aPresentation->HasOwnWidth();
  myWidth = aPresentation->Width();

  myHasOwnMode = aPresentation->HasOwnMode();
  myMode = aPresentation->Mode();

  myHasOwnSelectionMode = aPresentation->HasOwnSelectionMode();
  mySelectionModes = aPresentation->mySelectionModes;

  myHasOwnTransparency = aPresentation->HasOwnTransparency();
  myTransparency = aPresentation->Transparency();

  myIsDisplayed = aPresentation->IsDisplayed();
  myDriverGUID  = aPresentation->GetDriverGUID();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void TDataXtd_Presentation::Paste(const Handle(TDF_Attribute)& theInto,
                                  const Handle(TDF_RelocationTable)&) const
{
  Handle(TDataXtd_Presentation) anInto =
    Handle(TDataXtd_Presentation)::DownCast(theInto);

  anInto->Backup();

  if (myHasOwnMaterial)
  {
    anInto->myMaterialIndex = myMaterialIndex;
    anInto->myHasOwnMaterial = Standard_True;
  }
  else
  {
    anInto->myHasOwnMaterial = Standard_False;
  }

  if (myHasOwnColor)
  {
    anInto->myColor = myColor;
    anInto->myHasOwnColor = Standard_True;
  }
  else
  {
    anInto->myHasOwnColor = Standard_False;
  }

  if(myHasOwnWidth)
  {
    anInto->myWidth = myWidth;
    anInto->myHasOwnWidth = Standard_True;
  }
  else
  {
    anInto->myHasOwnWidth = Standard_False;
  }

  if (myHasOwnMode)
  {
    anInto->myMode = myMode;
    anInto->myHasOwnMode = Standard_True;
  }
  else
  {
    anInto->myHasOwnMode = Standard_False;
  }

  if (myHasOwnSelectionMode)
  {
    anInto->mySelectionModes = mySelectionModes;
    anInto->myHasOwnSelectionMode = Standard_True;
  }
  else
  {
    anInto->myHasOwnSelectionMode = Standard_False;
  }

  if (myHasOwnTransparency)
  {
    anInto->myTransparency = myTransparency;
    anInto->myHasOwnTransparency = Standard_True;
  }
  else
  {
    anInto->myHasOwnTransparency = Standard_False;
  }

  anInto->myIsDisplayed = myIsDisplayed;
  anInto->myDriverGUID  = myDriverGUID;
}

//=======================================================================
//function : HasSelectionMode
//purpose  : Checks a list of selection modes.
//=======================================================================
Standard_Boolean TDataXtd_Presentation::HasSelectionMode(const Standard_Integer theSelectionMode) const
{
  Standard_Boolean ret(Standard_False);
  TColStd_ListOfInteger::Iterator itr(mySelectionModes);
  for (; itr.More(); itr.Next())
  {
    if (theSelectionMode == itr.Value())
      ret = Standard_True;
  }
  return ret;
}
