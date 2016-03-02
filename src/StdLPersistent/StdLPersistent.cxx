// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <StdLPersistent.hxx>
#include <StdObjMgt_MapOfInstantiators.hxx>

#include <StdLPersistent_Document.hxx>
#include <StdLPersistent_Data.hxx>
#include <StdLPersistent_HString.hxx>
#include <StdLPersistent_HArray1.hxx>
#include <StdLPersistent_HArray2.hxx>
#include <StdLPersistent_Void.hxx>
#include <StdLPersistent_Real.hxx>
#include <StdLPersistent_Value.hxx>
#include <StdLPersistent_Collection.hxx>
#include <StdLPersistent_Dependency.hxx>
#include <StdLPersistent_Variable.hxx>
#include <StdLPersistent_XLink.hxx>
#include <StdLPersistent_Function.hxx>
#include <StdLPersistent_TreeNode.hxx>
#include <StdLPersistent_NamedData.hxx>


//=======================================================================
//function : BindTypes
//purpose  : Register types
//=======================================================================
void StdLPersistent::BindTypes (StdObjMgt_MapOfInstantiators& theMap)
{
  // Non-attribute data
  theMap.Bind <StdLPersistent_Document> ("PDocStd_Document");
  theMap.Bind <StdLPersistent_Data>     ("PDF_Data");

  theMap.Bind <StdLPersistent_HString::Ascii>      ("PCollection_HAsciiString");
  theMap.Bind <StdLPersistent_HString::Extended>   ("PCollection_HExtendedString");

  theMap.Bind <StdLPersistent_HArray1::Integer>    ("PColStd_HArray1OfInteger");
  theMap.Bind <StdLPersistent_HArray1::Real>       ("PColStd_HArray1OfReal");
  theMap.Bind <StdLPersistent_HArray1::Persistent> ("PColStd_HArray1OfExtendedString");
  theMap.Bind <StdLPersistent_HArray1::Persistent> ("PDF_HAttributeArray1");
  theMap.Bind <StdLPersistent_HArray1::Persistent> ("PDataStd_HArray1OfHAsciiString");
  theMap.Bind <StdLPersistent_HArray1::Persistent> ("PDataStd_HArray1OfHArray1OfInteger");
  theMap.Bind <StdLPersistent_HArray1::Persistent> ("PDataStd_HArray1OfHArray1OfReal");
  theMap.Bind <StdLPersistent_HArray1::Byte>       ("PDataStd_HArray1OfByte");

  theMap.Bind <StdLPersistent_HArray2::Integer>    ("PColStd_HArray2OfInteger");

  // Attributes
  theMap.Bind <StdLPersistent_Void::Directory>     ("PDataStd_Directory");
  theMap.Bind <StdLPersistent_Void::Tick>          ("PDataStd_Tick");
  theMap.Bind <StdLPersistent_Void::NoteBook>      ("PDataStd_NoteBook");

  theMap.Bind <StdLPersistent_Value::Integer>      ("PDataStd_Integer");
  theMap.Bind <StdLPersistent_Value::TagSource>    ("PDF_TagSource");
  theMap.Bind <StdLPersistent_Value::Reference>    ("PDF_Reference");
  theMap.Bind <StdLPersistent_Value::UAttribute>   ("PDataStd_UAttribute");

  theMap.Bind <StdLPersistent_Value::Name>         ("PDataStd_Name");
  theMap.Bind <StdLPersistent_Value::Comment>      ("PDataStd_Comment");
  theMap.Bind <StdLPersistent_Value::AsciiString>  ("PDataStd_AsciiString");

  theMap.Bind <StdLPersistent_Collection::IntegerArray>     ("PDataStd_IntegerArray");
  theMap.Bind <StdLPersistent_Collection::RealArray>        ("PDataStd_RealArray");
  theMap.Bind <StdLPersistent_Collection::ByteArray>        ("PDataStd_ByteArray");
  theMap.Bind <StdLPersistent_Collection::ExtStringArray>   ("PDataStd_ExtStringArray");
  theMap.Bind <StdLPersistent_Collection::BooleanArray>     ("PDataStd_BooleanArray");
  theMap.Bind <StdLPersistent_Collection::ReferenceArray>   ("PDataStd_ReferenceArray");

  theMap.Bind <StdLPersistent_Collection::IntegerArray_1>   ("PDataStd_IntegerArray_1");
  theMap.Bind <StdLPersistent_Collection::RealArray_1>      ("PDataStd_RealArray_1");
  theMap.Bind <StdLPersistent_Collection::ByteArray_1>      ("PDataStd_ByteArray_1");
  theMap.Bind <StdLPersistent_Collection::ExtStringArray_1> ("PDataStd_ExtStringArray_1");

  theMap.Bind <StdLPersistent_Collection::IntegerList>      ("PDataStd_IntegerList");
  theMap.Bind <StdLPersistent_Collection::RealList>         ("PDataStd_RealList");
  theMap.Bind <StdLPersistent_Collection::BooleanList>      ("PDataStd_BooleanList");
  theMap.Bind <StdLPersistent_Collection::ExtStringList>    ("PDataStd_ExtStringList");
  theMap.Bind <StdLPersistent_Collection::ReferenceList>    ("PDataStd_ReferenceList");

  theMap.Bind <StdLPersistent_Collection::IntPackedMap>     ("PDataStd_IntPackedMap");
  theMap.Bind <StdLPersistent_Collection::IntPackedMap_1>   ("PDataStd_IntPackedMap_1");

  theMap.Bind <StdLPersistent_Real>                   ("PDataStd_Real");
  theMap.Bind <StdLPersistent_Dependency::Expression> ("PDataStd_Expression");
  theMap.Bind <StdLPersistent_Dependency::Relation>   ("PDataStd_Relation");
  theMap.Bind <StdLPersistent_Variable>               ("PDataStd_Variable");
  theMap.Bind <StdLPersistent_XLink>                  ("PDocStd_XLink");
  theMap.Bind <StdLPersistent_Function>               ("PFunction_Function");
  theMap.Bind <StdLPersistent_TreeNode>               ("PDataStd_TreeNode");
  theMap.Bind <StdLPersistent_NamedData>              ("PDataStd_NamedData");
}
