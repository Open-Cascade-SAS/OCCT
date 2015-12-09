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

#include <StdLPersistent_PDocStd_Document.hxx>
#include <StdLPersistent_PDF_Data.hxx>
#include <StdLPersistent_PColStd_HArray1.hxx>
#include <StdLPersistent_PCollection_HExtendedString.hxx>
#include <StdLPersistent_PDataStd_Name.hxx>
#include <StdLPersistent_PDataStd_Integer.hxx>

//=======================================================================
//function : BindTypes
//purpose  : Register types
//=======================================================================
void StdLPersistent::BindTypes (StdObjMgt_MapOfInstantiators& theMap)
{
  theMap.Bind <StdLPersistent_PDocStd_Document> ("PDocStd_Document");
  theMap.Bind <StdLPersistent_PDF_Data>         ("PDF_Data");
  theMap.Bind <StdLPersistent_PDataStd_Name>    ("PDataStd_Name");
  theMap.Bind <StdLPersistent_PDataStd_Integer> ("PDataStd_Integer");

  theMap.Bind <StdLPersistent_PColStd_HArray1OfInteger>
    ("PColStd_HArray1OfInteger");

  theMap.Bind <StdLPersistent_PDF_HAttributeArray1>
    ("PDF_HAttributeArray1");

  theMap.Bind <StdLPersistent_PCollection_HExtendedString>
    ("PCollection_HExtendedString");
}
