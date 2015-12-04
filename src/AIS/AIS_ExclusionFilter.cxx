// Created on: 1997-11-28
// Created by: Robert COUBLANC
// Copyright (c) 1997-1999 Matra Datavision
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


#include <AIS_ExclusionFilter.hxx>
#include <AIS_InteractiveObject.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Standard_Type.hxx>
#include <TColStd_DataMapIteratorOfDataMapOfIntegerListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_ExclusionFilter,SelectMgr_Filter)

//=======================================================================
//function : AIS_ExclusionFilter
//purpose  : Constructors
//=======================================================================
AIS_ExclusionFilter::AIS_ExclusionFilter(const Standard_Boolean ExclusionFlagOn):
myIsExclusionFlagOn(ExclusionFlagOn)
{
}

AIS_ExclusionFilter::AIS_ExclusionFilter(const AIS_KindOfInteractive TypeToExclude,
					 const Standard_Boolean ExclusionFlagOn):
myIsExclusionFlagOn(ExclusionFlagOn)
{
  TColStd_ListOfInteger L;
  myStoredTypes.Bind((Standard_Integer)TypeToExclude,L);
}

AIS_ExclusionFilter::AIS_ExclusionFilter(const AIS_KindOfInteractive TypeToExclude,
					 const Standard_Integer SignatureInType,
					 const Standard_Boolean ExclusionFlagOn):
myIsExclusionFlagOn(ExclusionFlagOn)
{
  TColStd_ListOfInteger L;
  L.Append(SignatureInType);
  myStoredTypes.Bind((Standard_Integer)TypeToExclude,L);
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
Standard_Boolean AIS_ExclusionFilter::Add(const AIS_KindOfInteractive TypeToExclude) 
{
  if(IsStored(TypeToExclude)) 
    return Standard_False;
  TColStd_ListOfInteger L;
  myStoredTypes.Bind((Standard_Integer)TypeToExclude,L);
  return Standard_True;
}

Standard_Boolean AIS_ExclusionFilter::Add(const AIS_KindOfInteractive TypeToExclude,
					  const Standard_Integer SignatureInType) 
{
  if(!IsStored(TypeToExclude)){
    TColStd_ListOfInteger L;
    L.Append(SignatureInType);
    myStoredTypes.Bind((Standard_Integer)TypeToExclude,L);
    return Standard_True;
  }

  myStoredTypes((Standard_Integer)TypeToExclude).Append(SignatureInType);
  return Standard_True;
}

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

Standard_Boolean AIS_ExclusionFilter::Remove(const AIS_KindOfInteractive TypeToExclude) 
{
  if(!IsStored(TypeToExclude)) return Standard_False;
  myStoredTypes((Standard_Integer)TypeToExclude).Clear();
  myStoredTypes.UnBind((Standard_Integer)TypeToExclude);
  return Standard_True;
}

Standard_Boolean AIS_ExclusionFilter::Remove(const AIS_KindOfInteractive TypeToExclude,
					     const Standard_Integer SignatureInType) 
{
  if(!IsStored(TypeToExclude)) return Standard_False;
  TColStd_ListOfInteger& LL = myStoredTypes.ChangeFind((Standard_Integer)TypeToExclude);
  for(TColStd_ListIteratorOfListOfInteger it(LL);it.More();it.Next()){
    if(it.Value()==SignatureInType){
      LL.Remove(it);
      return Standard_True;
    }
  }
  return Standard_False;
}


//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void  AIS_ExclusionFilter::Clear()
{
  TColStd_DataMapIteratorOfDataMapOfIntegerListOfInteger Mit(myStoredTypes);
  for(;Mit.More();Mit.Next())
    myStoredTypes.ChangeFind(Mit.Key()).Clear();
  myStoredTypes.Clear();
}

//=======================================================================
//function : IsStored
//purpose  : 
//=======================================================================

Standard_Boolean AIS_ExclusionFilter::IsStored(const AIS_KindOfInteractive aType) const
{
  return myStoredTypes.IsBound(Standard_Integer(aType));
}

//=======================================================================
//function : IsSignatureIn
//purpose  : 
//=======================================================================
Standard_Boolean AIS_ExclusionFilter::IsSignatureIn(const AIS_KindOfInteractive aType,
						    const Standard_Integer SignatureInType) const
{
  if(!myStoredTypes.IsBound(aType)) return Standard_False;
  for(TColStd_ListIteratorOfListOfInteger Lit(myStoredTypes((Standard_Integer)aType));
      Lit.More();
      Lit.Next()){
    if(Lit.Value()==SignatureInType)
      return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : ListOfStoredTypes
//purpose  : 
//=======================================================================

void AIS_ExclusionFilter::ListOfStoredTypes(TColStd_ListOfInteger& TheList) const
{
  TheList.Clear();
  TColStd_DataMapIteratorOfDataMapOfIntegerListOfInteger MIT(myStoredTypes);
  for(;MIT.More();MIT.Next())
    TheList.Append(MIT.Key());
}

//=======================================================================
//function : ListOfSignature
//purpose  : 
//=======================================================================

void AIS_ExclusionFilter::ListOfSignature(const AIS_KindOfInteractive aType,TColStd_ListOfInteger& TheStoredList) const
{
  TheStoredList.Clear();
  if(IsStored(aType))
    for(TColStd_ListIteratorOfListOfInteger it(myStoredTypes(aType));it.More();it.Next())
      TheStoredList.Append(it.Value());
}

//=======================================================================
//function : IsOk
//purpose  : 
//=======================================================================

Standard_Boolean AIS_ExclusionFilter::IsOk(const Handle(SelectMgr_EntityOwner)& EO) const
{
  if(myStoredTypes.IsEmpty())
    return myIsExclusionFlagOn;

  if(EO.IsNull()) 
    return Standard_False;
  Handle(AIS_InteractiveObject) IO = Handle(AIS_InteractiveObject)::DownCast(EO->Selectable());
  if(IO.IsNull()) 
    return Standard_False;

  // type of AIS is not in the map...
  if(!myStoredTypes.IsBound(IO->Type()))
    return myIsExclusionFlagOn ;
  // type of AIS is not in the map and there is no signature indicated
  if(myStoredTypes(IO->Type()).IsEmpty())
    return !myIsExclusionFlagOn ;
  // one or several signatures are indicated...
  if(IsSignatureIn(IO->Type(),IO->Signature()))
    return !myIsExclusionFlagOn;
  
  return myIsExclusionFlagOn;
}







