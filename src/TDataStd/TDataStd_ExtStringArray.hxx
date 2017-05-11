// Created on: 2002-01-16
// Created by: Michael PONIKAROV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _TDataStd_ExtStringArray_HeaderFile
#define _TDataStd_ExtStringArray_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_HArray1OfExtendedString.hxx>
#include <Standard_Boolean.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
#include <Standard_GUID.hxx>

class TDataStd_DeltaOnModificationOfExtStringArray;
class Standard_GUID;
class TDF_Label;
class TCollection_ExtendedString;
class TDF_Attribute;
class TDF_RelocationTable;
class TDF_DeltaOnModification;


class TDataStd_ExtStringArray;
DEFINE_STANDARD_HANDLE(TDataStd_ExtStringArray, TDF_Attribute)

//! ExtStringArray Attribute. Handles an array of UNICODE strings (represented by the TCollection_ExtendedString class).
class TDataStd_ExtStringArray : public TDF_Attribute
{
  friend class TDataStd_DeltaOnModificationOfExtStringArray;
  DEFINE_STANDARD_RTTIEXT(TDataStd_ExtStringArray, TDF_Attribute)
public:

  //! class methods
  //! =============
  //! Returns the GUID for the attribute.
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Finds, or creates, an ExtStringArray attribute with <lower>
  //! and <upper> bounds on the specified label.
  //! If <isDelta> == False, DefaultDeltaOnModification is used.
  //! If <isDelta> == True, DeltaOnModification of the current attribute is used.
  //! If attribute is already set, all input parameters are refused and the found
  //! attribute is returned.
  Standard_EXPORT static Handle(TDataStd_ExtStringArray) Set (const TDF_Label& label, const Standard_Integer lower, const Standard_Integer upper, const Standard_Boolean isDelta = Standard_False);

  //! Finds, or creates, an ExtStringArray attribute with explicit user defined <guid>.
  //! The ExtStringArray attribute  is  returned.
  Standard_EXPORT static Handle(TDataStd_ExtStringArray) Set (const TDF_Label& label,  const Standard_GUID&   theGuid,
                                                              const Standard_Integer lower, const Standard_Integer upper,
                                                              const Standard_Boolean isDelta = Standard_False);


  //! Initializes the inner array with bounds from <lower> to <upper>
  Standard_EXPORT void Init (const Standard_Integer lower, const Standard_Integer upper);
  
  //! Sets  the   <Index>th  element  of   the  array to <Value>
  //! OutOfRange exception is raised if <Index> doesn't respect Lower and Upper bounds of the internal  array.
  Standard_EXPORT void SetValue (const Standard_Integer Index, const TCollection_ExtendedString& Value);

  //! Sets the explicit GUID (user defined) for the attribute.
  Standard_EXPORT void SetID( const Standard_GUID&  theGuid) Standard_OVERRIDE;

  //! Sets default GUID for the attribute.
  Standard_EXPORT void SetID() Standard_OVERRIDE;

  //! Returns the value of  the  <Index>th element of the array
  Standard_EXPORT const TCollection_ExtendedString& Value (const Standard_Integer Index) const;

  const TCollection_ExtendedString& operator () (const Standard_Integer Index) const
  {
    return Value(Index);
  }
  
  //! Return the lower bound.
  Standard_EXPORT Standard_Integer Lower() const;
  
  //! Return the upper bound
  Standard_EXPORT Standard_Integer Upper() const;
  
  //! Return the number of elements of <me>.
  Standard_EXPORT Standard_Integer Length() const;
  
  //! Sets the inner array <myValue> of the ExtStringArray attribute to <newArray>.
  //! If value of <newArray> differs from <myValue>, Backup performed and myValue
  //! refers to new instance of HArray1OfExtendedString that holds <newArray> values
  //! If <isCheckItems> equal True each item of <newArray> will be checked with each
  //! item of <myValue> for coincidence (to avoid backup).
  Standard_EXPORT void ChangeArray (const Handle(TColStd_HArray1OfExtendedString)& newArray, const Standard_Boolean isCheckItems = Standard_True);

  //! Return the inner array of the ExtStringArray attribute
  const Handle(TColStd_HArray1OfExtendedString)& Array() const { return myValue; }

  Standard_Boolean GetDelta() const { return myIsDelta; }

  //! for  internal  use  only!
  void SetDelta (const Standard_Boolean isDelta) { myIsDelta = isDelta; }

  Standard_EXPORT TDataStd_ExtStringArray();
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& With) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& Into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;
  
  //! Makes a DeltaOnModification between <me> and
  //! <anOldAttribute>.
  Standard_EXPORT virtual Handle(TDF_DeltaOnModification) DeltaOnModification (const Handle(TDF_Attribute)& anOldAttribute) const Standard_OVERRIDE;

private:

  void RemoveArray() { myValue.Nullify(); }

private:

  Handle(TColStd_HArray1OfExtendedString) myValue;
  Standard_Boolean myIsDelta;
  Standard_GUID myID;

};

#endif // _TDataStd_ExtStringArray_HeaderFile
