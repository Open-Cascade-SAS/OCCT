// Created by: DAUTRY Philippe
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _TDF_ClosureTool_HeaderFile
#define _TDF_ClosureTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TDF_LabelMap.hxx>
#include <TDF_AttributeMap.hxx>
class TDF_DataSet;
class TDF_IDFilter;
class TDF_ClosureMode;
class TDF_Label;


//! This class provides services to build the closure
//! of an information set.
//! This class gives services around the transitive
//! enclosure of a set of information, starting from a
//! list of label.
//! You can set closure options by using IDFilter
//! (to select or exclude specific attribute IDs) and
//! CopyOption objects and by giving to Closure
//! method.
class TDF_ClosureTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Builds the transitive closure of label and
  //! attribute sets into <aDataSet>.
  Standard_EXPORT static void Closure (const Handle(TDF_DataSet)& aDataSet);
  
  //! Builds the transitive closure of label and
  //! attribute sets into <aDataSet>. Uses <aFilter> to
  //! determine if an attribute has to be taken in
  //! account or not. Uses <aMode> for various way of
  //! closing.
  Standard_EXPORT static void Closure (const Handle(TDF_DataSet)& aDataSet, const TDF_IDFilter& aFilter, const TDF_ClosureMode& aMode);
  
  //! Builds the transitive closure of <aLabel>.
  Standard_EXPORT static void Closure (const TDF_Label& aLabel, TDF_LabelMap& aLabMap, TDF_AttributeMap& anAttMap, const TDF_IDFilter& aFilter, const TDF_ClosureMode& aMode);




protected:





private:

  
  //! Adds label attributes and dependences.
  Standard_EXPORT static void LabelAttributes (const TDF_Label& aLabel, TDF_LabelMap& aLabMap, TDF_AttributeMap& anAttMap, const TDF_IDFilter& aFilter, const TDF_ClosureMode& aMode);




};







#endif // _TDF_ClosureTool_HeaderFile
