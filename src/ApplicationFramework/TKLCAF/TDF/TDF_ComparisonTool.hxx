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

#ifndef _TDF_ComparisonTool_HeaderFile
#define _TDF_ComparisonTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
class TDF_DataSet;
class TDF_IDFilter;
class TDF_RelocationTable;
class TDF_Label;

//! This class provides services to compare sets of
//! information. The use of this tool can works after
//! a copy, acted by a CopyTool.
//!
//! * Compare(...) compares two DataSet and returns the result.
//!
//! * SourceUnbound(...) builds the difference between
//! a relocation dictionary and a source set of information.
//!
//! * TargetUnbound(...) does the same between a
//! relocation dictionary and a target set of information.
//!
//! * Cut(aDataSet, anLabel) removes a set of attributes.
//!
//! * IsSelfContained(...) returns true if all the
//! labels of the attributes of the given DataSet are
//! descendant of the given label.
class TDF_ComparisonTool
{
public:
  DEFINE_STANDARD_ALLOC

  //! Compares <aSourceDataSet> with <aTargetDataSet>,
  //! updating <aRelocationTable> with labels and
  //! attributes found in both sets.
  Standard_EXPORT static void Compare(const Handle(TDF_DataSet)&         aSourceDataSet,
                                      const Handle(TDF_DataSet)&         aTargetDataSet,
                                      const TDF_IDFilter&                aFilter,
                                      const Handle(TDF_RelocationTable)& aRelocationTable);

  //! Finds from <aRefDataSet> all the keys not bound
  //! into <aRelocationTable> and put them into
  //! <aDiffDataSet>. Returns True if the difference
  //! contains at least one key. (A key is a source
  //! object).
  //!
  //! <anOption> may take the following values:
  //! 1 : labels treatment only;
  //! 2 : attributes treatment only (default value);
  //! 3 : both labels & attributes treatment.
  Standard_EXPORT static Standard_Boolean SourceUnbound(
    const Handle(TDF_DataSet)&         aRefDataSet,
    const Handle(TDF_RelocationTable)& aRelocationTable,
    const TDF_IDFilter&                aFilter,
    const Handle(TDF_DataSet)&         aDiffDataSet,
    const Standard_Integer             anOption = 2);

  //! Subtracts from <aRefDataSet> all the items bound
  //! into <aRelocationTable>. The result is put into
  //! <aDiffDataSet>. Returns True if the difference
  //! contains at least one item. (An item is a target
  //! object).
  //!
  //! <anOption> may take the following values:
  //! 1 : labels treatment only;
  //! 2 : attributes treatment  only(default value);
  //! 3 : both labels & attributes treatment.
  Standard_EXPORT static Standard_Boolean TargetUnbound(
    const Handle(TDF_DataSet)&         aRefDataSet,
    const Handle(TDF_RelocationTable)& aRelocationTable,
    const TDF_IDFilter&                aFilter,
    const Handle(TDF_DataSet)&         aDiffDataSet,
    const Standard_Integer             anOption = 2);

  //! Removes attributes from <aDataSet>.
  Standard_EXPORT static void Cut(const Handle(TDF_DataSet)& aDataSet);

  //! Returns true if all the labels of <aDataSet> are
  //! descendant of <aLabel>.
  Standard_EXPORT static Standard_Boolean IsSelfContained(const TDF_Label&           aLabel,
                                                          const Handle(TDF_DataSet)& aDataSet);

protected:
private:
  //! Internal comparison method used by Compare(...).
  Standard_EXPORT static void Compare(const TDF_Label&                   aSrcLabel,
                                      const TDF_Label&                   aTrgLabel,
                                      const Handle(TDF_DataSet)&         aSourceDataSet,
                                      const Handle(TDF_DataSet)&         aTargetDataSet,
                                      const TDF_IDFilter&                aFilter,
                                      const Handle(TDF_RelocationTable)& aRelocationTable);

  //! Internal function used by SourceUnbound() and
  //! TargetUnbound().
  Standard_EXPORT static Standard_Boolean Unbound(
    const Handle(TDF_DataSet)&         aRefDataSet,
    const Handle(TDF_RelocationTable)& aRelocationTable,
    const TDF_IDFilter&                aFilter,
    const Handle(TDF_DataSet)&         aDiffDataSet,
    const Standard_Integer             anOption,
    const Standard_Boolean             theSource);
};

#endif // _TDF_ComparisonTool_HeaderFile
