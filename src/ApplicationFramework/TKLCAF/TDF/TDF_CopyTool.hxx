// Created by: DAUTRY Philippe
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

#ifndef _TDF_CopyTool_HeaderFile
#define _TDF_CopyTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TDF_LabelDataMap.hxx>
#include <TDF_AttributeDataMap.hxx>
#include <TDF_LabelMap.hxx>
#include <TDF_AttributeMap.hxx>
class TDF_DataSet;
class TDF_RelocationTable;
class TDF_IDFilter;
class TDF_Label;

//! This class provides services to build, copy or
//! paste a set of information.
//!
//! Copy methods:
//! -------------
//!
//! * Copy(aSourceDataSet, aTargetLabel,
//! aRelocationTable) copies a source DataSet under
//! its target place (see below: IMPORTANT NOTICE 1).
//!
//! * Copy(aSourceDataSet, anTargetLabel,
//! aRelocationTable, aFilter) does the same job as
//! the previous method. But <aFilter> gives a list of
//! IDs for which a target attribute prevails over a
//! source one. In this special case, the source
//! attribute will be copied only if there will be no
//! target attribute.
//!
//! IMPORTANT NOTICE : Label pre-binding
//! ------------------
//!
//! For it is possible to copy root labels in another
//! place in the same Data or in a different one with
//! other tags, it is necessary to inform the Copy
//! algorithm about the target place. To do so:
//!
//! * first get or create new target root labels;
//!
//! * then bind them with the source root labels using
//! the relocation table method:
//! SetRelocation(aSourceLabel, aTargetLabel);
//!
//! * finally call Copy(...) with the relocation table
//! previously set. In this way, this method will take
//! these relocations in account.
class TDF_CopyTool
{
public:
  DEFINE_STANDARD_ALLOC

  //! Copy   <aSourceDataSet> with using  and  updating
  //! <aRelocationTable>. This  method ignores   target
  //! attributes privilege over source ones.
  Standard_EXPORT static void Copy(const Handle(TDF_DataSet)&         aSourceDataSet,
                                   const Handle(TDF_RelocationTable)& aRelocationTable);

  //! Copy  <aSourceDataSet>  using      and updating
  //! <aRelocationTable>. Use <aPrivilegeFilter> to give
  //! a list  of   IDs for which  the target  attribute
  //! prevails over the source one.
  Standard_EXPORT static void Copy(const Handle(TDF_DataSet)&         aSourceDataSet,
                                   const Handle(TDF_RelocationTable)& aRelocationTable,
                                   const TDF_IDFilter&                aPrivilegeFilter);

  //! Copy    <aSourceDataSet>   using and    updating
  //! <aRelocationTable>. Use <aPrivilegeFilter> to give
  //! a  list of IDs   for which  the target  attribute
  //! prevails    over    the    source     one.   If
  //! <setSelfContained>   is   set  to   true,  every
  //! TDF_Reference will  be replaced  by the referenced
  //! structure according to <aRefFilter>.
  //!
  //! NB: <aRefFilter> is used only if
  //! <setSelfContained> is true.
  //! Internal root label copy recursive method.
  Standard_EXPORT static void Copy(const Handle(TDF_DataSet)&         aSourceDataSet,
                                   const Handle(TDF_RelocationTable)& aRelocationTable,
                                   const TDF_IDFilter&                aPrivilegeFilter,
                                   const TDF_IDFilter&                aRefFilter,
                                   const Standard_Boolean             setSelfContained);

protected:
private:
  //! Internal root label copy recursive method.
  Standard_EXPORT static void CopyLabels(const TDF_Label&        aSLabel,
                                         TDF_Label&              aTargetLabel,
                                         TDF_LabelDataMap&       aLabMap,
                                         TDF_AttributeDataMap&   aAttMap,
                                         const TDF_LabelMap&     aSrcLabelMap,
                                         const TDF_AttributeMap& aSrcAttributeMap);

  //! Internal attribute copy method.
  Standard_EXPORT static void CopyAttributes(const TDF_Label&        aSLabel,
                                             TDF_Label&              aTargetLabel,
                                             TDF_AttributeDataMap&   aAttMap,
                                             const TDF_AttributeMap& aSrcAttributeMap);
};

#endif // _TDF_CopyTool_HeaderFile
