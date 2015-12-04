// Created on: 2004-11-23
// Created by: Pavel TELKOV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#include <TObj_Partition.hxx>

#include <TObj_Model.hxx>
#include <TObj_TNameContainer.hxx>

#include <TDataStd_Name.hxx>


IMPLEMENT_STANDARD_RTTIEXT(TObj_Partition,TObj_Object)
IMPLEMENT_TOBJOCAF_PERSISTENCE(TObj_Partition)

//=======================================================================
//function : TObj_Partition
//purpose  :
//=======================================================================

TObj_Partition::TObj_Partition (const TDF_Label& theLabel)
     : TObj_Object( theLabel )
{
}

//=======================================================================
//function : Create
//purpose  :
//=======================================================================

Handle(TObj_Partition) TObj_Partition::Create
                           (const TDF_Label& theLabel)
{
  Handle(TObj_Partition) aPartition =
    new TObj_Partition(theLabel);
  aPartition->SetLastIndex(0);
  return aPartition;
}

//=======================================================================
//function : NewLabel
//purpose  :
//=======================================================================

TDF_Label TObj_Partition::NewLabel() const
{
  TDF_Label aLabel;
  TDF_TagSource aTag;
  aLabel = aTag.NewChild(GetChildLabel());
  return aLabel;
}

//=======================================================================
//function : SetNamePrefix
//purpose  :
//=======================================================================

void TObj_Partition::SetNamePrefix
                        (const Handle(TCollection_HExtendedString)& thePrefix)
{ myPrefix = thePrefix; }

//=======================================================================
//function : NewName
//purpose  :
//=======================================================================

Handle(TCollection_HExtendedString) TObj_Partition::GetNewName
( const Standard_Boolean theIsToChangeCount )
{
  if ( myPrefix.IsNull() ) return 0;

  Standard_Integer aRank = GetLastIndex()+1;
  Standard_Integer saveRank = aRank;
  Handle(TCollection_HExtendedString) aName;
  do
  {
    aName = new TCollection_HExtendedString(myPrefix->String()+aRank++);
  } while( GetModel()->IsRegisteredName( aName, GetDictionary() ) );

  // the last index is increased taking into account only names that are
  // actually set; the name requested by the current operation can be
  // dropped later and this will not cause index to be increased
  if ( theIsToChangeCount && --aRank > saveRank )
    SetLastIndex ( aRank );
  return aName;
}

//=======================================================================
//function : GetPartition
//purpose  :
//=======================================================================

Handle(TObj_Partition) TObj_Partition::GetPartition
                        (const Handle(TObj_Object)& theObject)
{
  Handle(TObj_Partition) aPartition;
  if(!theObject.IsNull())
  {
    TDF_Label aLabel = theObject->GetLabel().Father();

    // find partition which contains the object
    while(aPartition.IsNull() && !aLabel.IsNull())
    {
      Handle(TObj_Object) anObject;
      if(TObj_Object::GetObj(aLabel,anObject,Standard_True))
        aPartition = Handle(TObj_Partition)::DownCast(anObject);

      if(aPartition.IsNull())
        aLabel = aLabel.Father();
    }
  }
  return aPartition;
}

//=======================================================================
//function : GetLastIndex
//purpose  :
//=======================================================================

Standard_Integer TObj_Partition::GetLastIndex() const
{
  return getInteger(DataTag_LastIndex);
}

//=======================================================================
//function : SetLastIndex
//purpose  :
//=======================================================================

void TObj_Partition::SetLastIndex(const Standard_Integer theIndex)
{
  setInteger(theIndex,DataTag_LastIndex);
}

//=======================================================================
//function : copyData
//purpose  : protected
//=======================================================================

Standard_Boolean TObj_Partition::copyData
                (const Handle(TObj_Object)& theTargetObject)
{
  Standard_Boolean IsDone;
  Handle(TObj_Partition) aTargetPartition =
    Handle(TObj_Partition)::DownCast(theTargetObject);
  IsDone = aTargetPartition.IsNull() ? Standard_False : Standard_True;
  if(IsDone) 
  {
    IsDone = TObj_Object::copyData(theTargetObject);
    if ( IsDone ) 
    {
      aTargetPartition->myPrefix = myPrefix;
    }
  }
  return IsDone;
}

//=======================================================================
//function : SetName
//purpose  : do not register a name in the dictionary
//=======================================================================

Standard_Boolean TObj_Partition::SetName(const Handle(TCollection_HExtendedString)& theName) const
{
  Handle(TCollection_HExtendedString) anOldName = GetName();
  if( !anOldName.IsNull() && theName->String().IsEqual(anOldName->String()) )
    return Standard_True;

  TDataStd_Name::Set(GetLabel(),theName->String());
  return Standard_True;
}

//=======================================================================
//function : AfterRetrieval
//purpose  : do not register a name in the dictionary
//=======================================================================

void TObj_Partition::AfterRetrieval()
{
}
