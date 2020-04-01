// Created:	Tue Nov  2 13:14:31 1999
// Author:	Andrey BETENEV
// Copyright (c) 1999-2020 OPEN CASCADE SAS
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

#include <Express_Item.hxx>

#include <Message.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Express_Item, Standard_Transient)

Standard_Integer Express_Item::myIndex = -1;

//=======================================================================
// function : Express_Item
// purpose  :
//=======================================================================

Express_Item::Express_Item (const Standard_CString theName)
{
  myName = new TCollection_HAsciiString (theName);
  myGenMode = GM_Undefined;
  myShortName = new TCollection_HAsciiString;
  myCategory = new TCollection_HAsciiString;
  myhasCheck = Standard_False;
  myhasFillShared = Standard_False;
  myLoopFlag = Standard_False;
}

//=======================================================================
// function : Express_Item
// purpose  :
//=======================================================================

Express_Item::Express_Item (const Handle(TCollection_HAsciiString)& theName)
{
  myName = theName;
  myGenMode = GM_Undefined;
  myShortName = new TCollection_HAsciiString;
  myCategory = new TCollection_HAsciiString;
  myhasCheck = Standard_False;
  myhasFillShared = Standard_False;
  myLoopFlag = Standard_False;
}

//=======================================================================
// function : Name
// purpose  :
//=======================================================================

const TCollection_AsciiString& Express_Item::Name() const
{
  return myName->String();
}

//=======================================================================
// function : HName
// purpose  :
//=======================================================================

Handle(TCollection_HAsciiString) Express_Item::HName() const
{
  return myName;
}

//=======================================================================
// function : CPPName
// purpose  :
//=======================================================================

const TCollection_AsciiString Express_Item::CPPName() const
{
  TCollection_AsciiString aName = GetPackageName();
  aName += "_";
  aName += Name();
  return aName;
}

//=======================================================================
// function : SetPackageName
// purpose  :
//=======================================================================

void Express_Item::SetPackageName (const TCollection_AsciiString& thePack)
{
  myPack = new TCollection_HAsciiString(thePack);
}

//=======================================================================
// function : GetPackageName
// purpose  :
//=======================================================================

const TCollection_AsciiString& Express_Item::GetPackageName() const
{
  if (myPack.IsNull())
  {
    Message::SendWarning() << "Warning: item " << Name() << " still has no package assigned, used " << GetUnknownPackageName();
    return GetUnknownPackageName();
  }
  return myPack->String();
}

//=======================================================================
// function : IsPackageNameSet
// purpose  :
//=======================================================================

Standard_Boolean Express_Item::IsPackageNameSet() const
{
  return !myPack.IsNull();
}

//=======================================================================
// function : GetUnknownPackageName
// purpose  :
//=======================================================================

TCollection_AsciiString& Express_Item::GetUnknownPackageName()
{
  static TCollection_AsciiString aUnknownPackageName = "StepStep";

  return aUnknownPackageName;
}

//=======================================================================
// function : GetGenMode
// purpose  :
//=======================================================================

Express_Item::GenMode Express_Item::GetGenMode() const
{
  return myGenMode;
}

//=======================================================================
// function : SetGenMode
// purpose  :
//=======================================================================

void Express_Item::SetGenMode (const Express_Item::GenMode theGenMode)
{
  myGenMode = theGenMode;
}

//=======================================================================
// function : ResetLoopFlag
// purpose  :
//=======================================================================

void Express_Item::ResetLoopFlag()
{
  myLoopFlag = Standard_False;
}

//=======================================================================
// function : Generate
// purpose  :
//=======================================================================

Standard_Boolean Express_Item::Generate()
{
  // skip items without "generate" mark
  GenMode aMode = GetGenMode();
  if (!(aMode == GM_GenByUser || aMode == GM_GenByAlgo))
  {
    return Standard_False;
  }
  // manage indent for cout in order to mark structure of calls
  static Standard_Integer aShift = -1;
  aShift++;
  for (Standard_Integer i = 0; i < aShift; i++)
  {
    std::cout << "  ";
  }
  // sets the mode to generated before "GenerateClass" function to avoid looping
  SetGenMode (GM_Generated);
  Standard_Boolean aRes = GenerateClass();
  aShift--;

  return aRes;
}

//=======================================================================
// function : Use
// purpose  :
//=======================================================================

Standard_Boolean Express_Item::Use()
{
  // if Item is not mentioned by the user but is used, then it is necessary to generate
  if (GetGenMode() != GM_Undefined)
  {
    return Standard_False;
  }
  SetGenMode (GM_GenByAlgo);

  return Generate();
}

//=======================================================================
// function : Use2
// purpose  :
//=======================================================================

void Express_Item::Use2 (const TCollection_AsciiString& theRefName, const TCollection_AsciiString& theRefPack)
{
  if (myLoopFlag)
  {
    return;
  }
  else
  {
    myLoopFlag = Standard_True;
  }
  // issue a warning message if item does not have package assigned
  if (!IsPackageNameSet())
  {
    Message::SendWarning() << "Warning: item " << Name() << " has no package assigned but used by " << theRefName << ", setting " << theRefPack;
    SetPackageName (theRefPack);
  }

  PropagateUse();
}

//=======================================================================
// function : SetCategory
// purpose  :
//=======================================================================

void Express_Item::SetCategory (const Handle(TCollection_HAsciiString)& theCateg)
{
  myCategory = theCateg;
}

//=======================================================================
// function : Cartegory
// purpose  :
//=======================================================================

const TCollection_AsciiString& Express_Item::Category() const
{
  return myCategory->String();
}

//=======================================================================
// function : SetShortName
// purpose  :
//=======================================================================

void Express_Item::SetShortName (const Handle(TCollection_HAsciiString)& theShName)
{
  myShortName = theShName;
}

//=======================================================================
// function : ShortName
// purpose  :
//=======================================================================

Handle(TCollection_HAsciiString) Express_Item::ShortName() const
{
  return myShortName;
}
//=======================================================================
// function : SetCheckFlag
// purpose  :
//=======================================================================

void Express_Item::SetCheckFlag (const Standard_Boolean theCheckFlag)
{
  myhasCheck = theCheckFlag;
}

//=======================================================================
// function : CheckFlag
// purpose  :
//=======================================================================

Standard_Boolean Express_Item::CheckFlag() const
{
  return myhasCheck;
}

//=======================================================================
// function : SetFillSharedFlag
// purpose  :
//=======================================================================

void Express_Item::SetFillSharedFlag (const Standard_Boolean theFillSharedFlag)
{
  myhasFillShared = theFillSharedFlag;
}

//=======================================================================
// function : FillSharedFlag
// purpose  :
//=======================================================================

Standard_Boolean Express_Item::FillSharedFlag() const
{
  return myhasFillShared;
}

//=======================================================================
// function : SetIndex
// purpose  :
//=======================================================================

void Express_Item::SetIndex (const Standard_Integer theIndex)
{
  myIndex = theIndex;
}

//=======================================================================
// function : Index
// purpose  :
//=======================================================================

Standard_Integer Express_Item::Index()
{
  return myIndex;
}
