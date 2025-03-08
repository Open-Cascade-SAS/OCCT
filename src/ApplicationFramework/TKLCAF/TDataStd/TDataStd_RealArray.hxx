// Created on: 1999-06-16
// Created by: Sergey RUIN
// Copyright (c) 1999 Matra Datavision
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

#ifndef _TDataStd_RealArray_HeaderFile
#define _TDataStd_RealArray_HeaderFile

#include <Standard.hxx>

#include <TColStd_HArray1OfReal.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_OStream.hxx>
#include <Standard_GUID.hxx>

class TDF_Label;
class TDF_RelocationTable;
class TDF_DeltaOnModification;

class TDataStd_RealArray;
DEFINE_STANDARD_HANDLE(TDataStd_RealArray, TDF_Attribute)

//! A framework for an attribute composed of a real number array.
class TDataStd_RealArray : public TDF_Attribute
{
  friend class TDataStd_DeltaOnModificationOfRealArray;
  DEFINE_STANDARD_RTTIEXT(TDataStd_RealArray, TDF_Attribute)
public:
  //! class methods
  //! =============
  //! Returns the GUID for arrays of reals.
  Standard_EXPORT static const Standard_GUID& GetID();

  //! Finds or creates on the <label> a real array attribute with
  //! the specified <lower> and <upper> boundaries.
  //! If <isDelta> == False, DefaultDeltaOnModification is used.
  //! If <isDelta> == True, DeltaOnModification of the current attribute is used.
  //! If attribute is already set, input parameter <isDelta> is refused and the found
  //! attribute returned.
  Standard_EXPORT static Handle(TDataStd_RealArray) Set(
    const TDF_Label&       label,
    const Standard_Integer lower,
    const Standard_Integer upper,
    const Standard_Boolean isDelta = Standard_False);

  //! Finds, or creates, an RealArray attribute with explicit user defined <guid>.
  //! The RealArray attribute  is  returned.
  Standard_EXPORT static Handle(TDataStd_RealArray) Set(
    const TDF_Label&       label,
    const Standard_GUID&   theGuid,
    const Standard_Integer lower,
    const Standard_Integer upper,
    const Standard_Boolean isDelta = Standard_False);

  //! Initialize the inner array with bounds from <lower> to <upper>
  Standard_EXPORT void Init(const Standard_Integer lower, const Standard_Integer upper);

  //! Sets the explicit GUID (user defined) for the attribute.
  Standard_EXPORT void SetID(const Standard_GUID& theGuid) Standard_OVERRIDE;

  //! Sets default GUID for the attribute.
  Standard_EXPORT void SetID() Standard_OVERRIDE;

  //! Sets  the   <Index>th  element  of   the  array to <Value>
  //! OutOfRange exception is raised if <Index> doesn't respect Lower and Upper bounds of the
  //! internal  array.
  Standard_EXPORT void SetValue(const Standard_Integer Index, const Standard_Real Value);

  //! Return the value of  the  <Index>th element of the array
  Standard_EXPORT Standard_Real Value(const Standard_Integer Index) const;

  Standard_Real operator()(const Standard_Integer Index) const { return Value(Index); }

  //! Returns the lower boundary of the array.
  Standard_EXPORT Standard_Integer Lower() const;

  //! Returns the upper boundary of the array.
  Standard_EXPORT Standard_Integer Upper() const;

  //! Returns the number of elements of the array of reals
  //! in terms of the number of elements it contains.
  Standard_EXPORT Standard_Integer Length() const;

  //! Sets the inner array <myValue> of the RealArray attribute
  //! to <newArray>. If value of <newArray> differs from <myValue>,
  //! Backup performed and myValue refers to new instance of HArray1OfReal
  //! that holds <newArray> values
  //! If <isCheckItems> equal True each item of <newArray> will be checked with each
  //! item of <myValue> for coincidence (to avoid backup).
  Standard_EXPORT void ChangeArray(const Handle(TColStd_HArray1OfReal)& newArray,
                                   const Standard_Boolean isCheckItems = Standard_True);

  //! Returns the handle of this array of reals.
  const Handle(TColStd_HArray1OfReal)& Array() const { return myValue; }

  Standard_Boolean GetDelta() const { return myIsDelta; }

  //! for  internal  use  only!
  void SetDelta(const Standard_Boolean isDelta) { myIsDelta = isDelta; }

  Standard_EXPORT TDataStd_RealArray();

  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;

  Standard_EXPORT void Restore(const Handle(TDF_Attribute)& With) Standard_OVERRIDE;

  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  //! Note. Uses inside ChangeArray() method
  Standard_EXPORT void Paste(const Handle(TDF_Attribute)&       Into,
                             const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_OStream& Dump(Standard_OStream& anOS) const Standard_OVERRIDE;

  //! Makes a DeltaOnModification between <me> and
  //! <anOldAttribute>.
  Standard_EXPORT virtual Handle(TDF_DeltaOnModification) DeltaOnModification(
    const Handle(TDF_Attribute)& anOldAttribute) const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

private:
  void RemoveArray() { myValue.Nullify(); }

private:
  Handle(TColStd_HArray1OfReal) myValue;
  Standard_Boolean              myIsDelta;
  Standard_GUID                 myID;
};

#endif // _TDataStd_RealArray_HeaderFile
