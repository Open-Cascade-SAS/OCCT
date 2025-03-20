// Created on: 1993-01-13
// Created by: CKY / Contract Toubro-Larsen ( Deepak PRABHU )
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IGESDefs_AttributeDef_HeaderFile
#define _IGESDefs_AttributeDef_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfTransient.hxx>
#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;
class IGESDefs_HArray1OfHArray1OfTextDisplayTemplate;
class IGESGraph_TextDisplayTemplate;
class Standard_Transient;

class IGESDefs_AttributeDef;
DEFINE_STANDARD_HANDLE(IGESDefs_AttributeDef, IGESData_IGESEntity)

//! defines IGES Attribute Table Definition Entity,
//! Type <322> Form [0, 1, 2] in package IGESDefs.
//! This is class is used to support the concept of well
//! defined collection of attributes, whether it is a table
//! or a single row of attributes.
class IGESDefs_AttributeDef : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESDefs_AttributeDef();

  Standard_EXPORT void Init(
    const Handle(TCollection_HAsciiString)&                       aName,
    const Standard_Integer                                        aListType,
    const Handle(TColStd_HArray1OfInteger)&                       attrTypes,
    const Handle(TColStd_HArray1OfInteger)&                       attrValueDataTypes,
    const Handle(TColStd_HArray1OfInteger)&                       attrValueCounts,
    const Handle(TColStd_HArray1OfTransient)&                     attrValues,
    const Handle(IGESDefs_HArray1OfHArray1OfTextDisplayTemplate)& attrValuePointers);

  //! Returns True if a Table Name is defined
  Standard_EXPORT Standard_Boolean HasTableName() const;

  //! returns the Attribute Table name, or comment
  //! (default = null, no name : seeHasTableName)
  Standard_EXPORT Handle(TCollection_HAsciiString) TableName() const;

  //! returns the Attribute List Type
  Standard_EXPORT Standard_Integer ListType() const;

  //! returns the Number of Attributes
  Standard_EXPORT Standard_Integer NbAttributes() const;

  //! returns the num'th Attribute Type
  //! raises exception if num <= 0 or num > NbAttributes()
  Standard_EXPORT Standard_Integer AttributeType(const Standard_Integer num) const;

  //! returns the num'th Attribute value data type
  //! raises exception if num <= 0 or num > NbAttributes()
  Standard_EXPORT Standard_Integer AttributeValueDataType(const Standard_Integer num) const;

  //! returns the num'th Attribute value count
  //! raises exception if num <= 0 or num > NbAttributes()
  Standard_EXPORT Standard_Integer AttributeValueCount(const Standard_Integer num) const;

  //! returns false if Values are defined (i.e. for Form = 1 or 2)
  Standard_EXPORT Standard_Boolean HasValues() const;

  //! returns false if TextDisplays are defined (i.e. for Form = 2)
  Standard_EXPORT Standard_Boolean HasTextDisplay() const;

  Standard_EXPORT Handle(IGESGraph_TextDisplayTemplate) AttributeTextDisplay(
    const Standard_Integer AttrNum,
    const Standard_Integer PointerNum) const;

  //! Returns the List of Attributes <AttrNum>, as a Transient.
  //! Its effective Type depends of the Type of Attribute :
  //! HArray1OfInteger for Integer, Logical(0-1),
  //! HArray1OfReal for Real, HArray1OfHSaciiString for String,
  //! HArray1OfIGESEntity for Entity (Pointer)
  //! See methods AttributeAs... for an accurate access
  Standard_EXPORT Handle(Standard_Transient) AttributeList(const Standard_Integer AttrNum) const;

  //! Returns Attribute Value <AttrNum, rank ValueNum> as an Integer
  //! Error if Indices out of Range, or no Value defined, or not an Integer
  Standard_EXPORT Standard_Integer AttributeAsInteger(const Standard_Integer AttrNum,
                                                      const Standard_Integer ValueNum) const;

  //! Returns Attribute Value <AttrNum, rank ValueNum> as a Real
  //! Error if Indices out of Range, or no Value defined, or not a Real
  Standard_EXPORT Standard_Real AttributeAsReal(const Standard_Integer AttrNum,
                                                const Standard_Integer ValueNum) const;

  //! Returns Attribute Value <AttrNum, rank ValueNum> as an Integer
  Standard_EXPORT Handle(TCollection_HAsciiString) AttributeAsString(
    const Standard_Integer AttrNum,
    const Standard_Integer ValueNum) const;

  //! Returns Attribute Value <AttrNum, rank ValueNum> as an Entity
  //! Error if Indices out of Range, or no Value defined, or not a Entity
  Standard_EXPORT Handle(IGESData_IGESEntity) AttributeAsEntity(
    const Standard_Integer AttrNum,
    const Standard_Integer ValueNum) const;

  //! Returns Attribute Value <AttrNum, rank ValueNum> as a Boolean
  //! Error if Indices out of Range, or no Value defined, or not a Logical
  Standard_EXPORT Standard_Boolean AttributeAsLogical(const Standard_Integer AttrNum,
                                                      const Standard_Integer ValueNum) const;

  DEFINE_STANDARD_RTTIEXT(IGESDefs_AttributeDef, IGESData_IGESEntity)

protected:
private:
  Handle(TCollection_HAsciiString)                       theName;
  Standard_Integer                                       theListType;
  Handle(TColStd_HArray1OfInteger)                       theAttrTypes;
  Handle(TColStd_HArray1OfInteger)                       theAttrValueDataTypes;
  Handle(TColStd_HArray1OfInteger)                       theAttrValueCounts;
  Handle(TColStd_HArray1OfTransient)                     theAttrValues;
  Handle(IGESDefs_HArray1OfHArray1OfTextDisplayTemplate) theAttrValuePointers;
};

#endif // _IGESDefs_AttributeDef_HeaderFile
