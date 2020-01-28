// Created on: 2007-05-29
// Created by: Vlad Romashko
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#ifndef _TDataStd_BooleanArray_HeaderFile
#define _TDataStd_BooleanArray_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_HArray1OfByte.hxx>
#include <Standard_Integer.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_OStream.hxx>
#include <Standard_GUID.hxx>
class Standard_GUID;
class TDF_Label;
class TDF_Attribute;
class TDF_RelocationTable;


class TDataStd_BooleanArray;
DEFINE_STANDARD_HANDLE(TDataStd_BooleanArray, TDF_Attribute)

//! An array of boolean values.
class TDataStd_BooleanArray : public TDF_Attribute
{

public:

  
  //! Static methods
  //! ==============
  //! Returns an ID for array.
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Finds or creates an attribute with internal boolean array.
  Standard_EXPORT static Handle(TDataStd_BooleanArray) Set (const TDF_Label& label, const Standard_Integer lower, const Standard_Integer upper);

  //! Finds or creates an attribute with the array using explicit user defined <guid>.
  Standard_EXPORT static Handle(TDataStd_BooleanArray) Set (const TDF_Label& label, const Standard_GUID&   theGuid, 
                                                            const Standard_Integer lower, const Standard_Integer upper);

  //! Initialize the inner array with bounds from <lower> to <upper>
  Standard_EXPORT void Init (const Standard_Integer lower, const Standard_Integer upper);
  
  //! Sets the <Index>th element of the array to <Value>
  //! OutOfRange exception is raised if <Index> doesn't respect Lower and Upper bounds of the internal  array.
  Standard_EXPORT void SetValue (const Standard_Integer index, const Standard_Boolean value);

  //! Sets the explicit GUID (user defined) for the attribute.
  Standard_EXPORT void SetID( const Standard_GUID&  theGuid) Standard_OVERRIDE;

  //! Sets default GUID for the attribute.
  Standard_EXPORT void SetID() Standard_OVERRIDE;

  //! Return the value of the <Index>th element of the array.
  Standard_EXPORT Standard_Boolean Value (const Standard_Integer Index) const;

  Standard_Boolean operator () (const Standard_Integer Index) const
  {
    return Value(Index);
  }
  
  //! Returns the lower boundary of the array.
  Standard_EXPORT Standard_Integer Lower() const;
  
  //! Returns the upper boundary of the array.
  Standard_EXPORT Standard_Integer Upper() const;
  
  //! Returns the number of elements in the array.
  Standard_EXPORT Standard_Integer Length() const;
  
  Standard_EXPORT const Handle(TColStd_HArray1OfByte)& InternalArray() const;
  
  Standard_EXPORT void SetInternalArray (const Handle(TColStd_HArray1OfByte)& values);
  
  Standard_EXPORT TDataStd_BooleanArray();
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& with) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& OS) const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(TDataStd_BooleanArray,TDF_Attribute)

protected:




private:


  Handle(TColStd_HArray1OfByte) myValues;
  Standard_Integer myLower;
  Standard_Integer myUpper;
  Standard_GUID myID;

};







#endif // _TDataStd_BooleanArray_HeaderFile
