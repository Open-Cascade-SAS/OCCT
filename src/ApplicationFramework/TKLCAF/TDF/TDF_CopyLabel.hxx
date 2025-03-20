// Created on: 1999-06-24
// Created by: Sergey ZARITCHNY
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

#ifndef _TDF_CopyLabel_HeaderFile
#define _TDF_CopyLabel_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TDF_Label.hxx>
#include <TDF_IDFilter.hxx>
#include <TDF_AttributeMap.hxx>
class TDF_RelocationTable;
class TDF_DataSet;

//! This class gives copy of  source  label  hierarchy
class TDF_CopyLabel
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty  constructor
  Standard_EXPORT TDF_CopyLabel();

  //! CopyTool
  Standard_EXPORT TDF_CopyLabel(const TDF_Label& aSource, const TDF_Label& aTarget);

  //! Loads  src  and  tgt  labels
  Standard_EXPORT void Load(const TDF_Label& aSource, const TDF_Label& aTarget);

  //! Sets  filter
  Standard_EXPORT void UseFilter(const TDF_IDFilter& aFilter);

  //! Check  external  references and  if  exist  fills  the  aExternals  Map
  Standard_EXPORT static Standard_Boolean ExternalReferences(const TDF_Label&    Lab,
                                                             TDF_AttributeMap&   aExternals,
                                                             const TDF_IDFilter& aFilter);

  //! Check  external  references and  if  exist  fills  the  aExternals  Map
  Standard_EXPORT static void ExternalReferences(const TDF_Label&     aRefLab,
                                                 const TDF_Label&     Lab,
                                                 TDF_AttributeMap&    aExternals,
                                                 const TDF_IDFilter&  aFilter,
                                                 Handle(TDF_DataSet)& aDataSet);

  //! performs  algorithm  of  selfcontained  copy
  Standard_EXPORT void Perform();

  Standard_Boolean IsDone() const;

  //! returns  relocation  table
  Standard_EXPORT const Handle(TDF_RelocationTable)& RelocationTable() const;

protected:
private:
  Handle(TDF_RelocationTable) myRT;
  TDF_Label                   mySL;
  TDF_Label                   myTL;
  TDF_IDFilter                myFilter;
  TDF_AttributeMap            myMapOfExt;
  Standard_Boolean            myIsDone;
};

#include <TDF_CopyLabel.lxx>

#endif // _TDF_CopyLabel_HeaderFile
