// Created on: 2004-11-22
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

#include <TObj_Assistant.hxx>

#include <TObj_Common.hxx>
#include <TObj_Model.hxx>
#include <TColStd_SequenceOfTransient.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <TColStd_IndexedMapOfTransient.hxx>

//=======================================================================
//function : getModels
//purpose  : 
//=======================================================================

TColStd_SequenceOfTransient& TObj_Assistant::getModels()
{
  static TColStd_SequenceOfTransient sModels;
  return sModels;
}

//=======================================================================
//function : getTypes
//purpose  : 
//=======================================================================

TColStd_IndexedMapOfTransient& TObj_Assistant::getTypes()
{
  static TColStd_IndexedMapOfTransient sTypes;
  return sTypes;
}

//=======================================================================
//function : getTypes
//purpose  : 
//=======================================================================

Handle(TObj_Model)& TObj_Assistant::getCurrentModel()
{
  static Handle(TObj_Model) sCurrentModel;
  return sCurrentModel;
}

//=======================================================================
//function : getVersion
//purpose  : 
//=======================================================================

Standard_Integer& TObj_Assistant::getVersion()
{
  static Standard_Integer sVersion = 0;
  return sVersion;
}

//=======================================================================
//function : FindModel
//purpose  : 
//=======================================================================

Handle(TObj_Model) TObj_Assistant::FindModel
  (const Standard_CString theName)
{
  TCollection_ExtendedString aName( theName );
  Standard_Integer i = getModels().Length();
  Handle(TObj_Model) aModel;
  for(; i > 0; i --)
  {
    aModel = Handle(TObj_Model)::DownCast(getModels().Value( i ));
    if ( aName == aModel->GetModelName()->String() )
      break;
  }
  if (i == 0)
    aModel.Nullify();
  
  return aModel;
}

//=======================================================================
//function : BindModel
//purpose  : 
//=======================================================================

void TObj_Assistant::BindModel (const Handle(TObj_Model) theModel)
{
  getModels().Append(theModel);
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void TObj_Assistant::ClearModelMap ()
{
  getModels().Clear();
}

//=======================================================================
//function : FindType
//purpose  : 
//=======================================================================

Handle(Standard_Type) TObj_Assistant::FindType
  (const Standard_Integer theTypeIndex)
{
  if(theTypeIndex > 0 && theTypeIndex <= getTypes().Extent())
    return Handle(Standard_Type)::DownCast(getTypes().FindKey(theTypeIndex));

  return 0;
}

//=======================================================================
//function : FindTypeIndex
//purpose  : 
//=======================================================================

Standard_Integer TObj_Assistant::FindTypeIndex
  (const Handle(Standard_Type)& theType)
{
  if(!getTypes().Contains(theType))
    return 0;

  return getTypes().FindIndex(theType);
}

//=======================================================================
//class    : TObj_Assistant_UnknownType
//purpose  : 
//=======================================================================

class TObj_Assistant_UnknownType : public MMgt_TShared
{
 public:
  
  TObj_Assistant_UnknownType() {}
    // Empty constructor
  
  // CASCADE RTTI
  DEFINE_STANDARD_RTTI(TObj_Assistant_UnknownType)
};

// Define handle class for TObj_Assistant_UnknownType
DEFINE_STANDARD_HANDLE(TObj_Assistant_UnknownType,MMgt_TShared)

IMPLEMENT_STANDARD_HANDLE(TObj_Assistant_UnknownType,MMgt_TShared)
IMPLEMENT_STANDARD_RTTIEXT(TObj_Assistant_UnknownType,MMgt_TShared)

//=======================================================================
//function : BindType
//purpose  : 
//=======================================================================

Standard_Integer TObj_Assistant::BindType
  (const Handle(Standard_Type)& theType)
{
  if(theType.IsNull())
  {
    Handle(Standard_Transient) anUnknownType;
    anUnknownType = new TObj_Assistant_UnknownType;
    return getTypes().Add(anUnknownType);
  }

  return getTypes().Add(theType);
}

//=======================================================================
//function : ClearTypeMap
//purpose  : 
//=======================================================================

void TObj_Assistant::ClearTypeMap ()
{
  getTypes().Clear();
}

//=======================================================================
//function : SetCurrentModel
//purpose  : 
//=======================================================================

void TObj_Assistant::SetCurrentModel (const Handle(TObj_Model)& theModel)
{
  getCurrentModel() = theModel;
  getVersion() = -1;
}

//=======================================================================
//function : GetCurrentModel
//purpose  : 
//=======================================================================

Handle(TObj_Model) TObj_Assistant::GetCurrentModel ()
{
  return getCurrentModel();
}

//=======================================================================
//function : UnSetCurrentModel
//purpose  : 
//=======================================================================

void TObj_Assistant::UnSetCurrentModel ()
{
  getCurrentModel().Nullify();
}

//=======================================================================
//function : GetAppVersion
//purpose  : 
//=======================================================================

Standard_Integer TObj_Assistant::GetAppVersion()
{
  Standard_Integer& aVersion = getVersion();
  if (aVersion < 0)
  {
    Handle(TObj_Model)& aModel = getCurrentModel();
    if (!aModel.IsNull())
      aVersion = aModel->GetFormatVersion();
  }
  return aVersion;
}
