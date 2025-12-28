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

#ifndef _TDF_DataSet_HeaderFile
#define _TDF_DataSet_HeaderFile

#include <Standard.hxx>

#include <TDF_Label.hxx>
#include <NCollection_List.hxx>
#include <TDF_Label.hxx>
#include <NCollection_Map.hxx>
#include <Standard_Handle.hxx>
#include <TDF_Attribute.hxx>
#include <NCollection_Map.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_OStream.hxx>
class TDF_Label;
class TDF_Attribute;

//! This class is a set of TDF information like labels and attributes.
class TDF_DataSet : public Standard_Transient
{

public:
  //! Creates an empty DataSet object.
  Standard_EXPORT TDF_DataSet();

  //! Clears all information.
  Standard_EXPORT void Clear();

  //! Returns true if there is at least one label or one
  //! attribute.
  bool IsEmpty() const;

  //! Adds <aLabel> in the current data set.
  void AddLabel(const TDF_Label& aLabel);

  //! Returns true if the label <alabel> is in the data set.
  bool ContainsLabel(const TDF_Label& aLabel) const;

  //! Returns the map of labels in this data set.
  //! This map can be used directly, or updated.
  NCollection_Map<TDF_Label>& Labels();

  //! Adds <anAttribute> into the current data set.
  void AddAttribute(const occ::handle<TDF_Attribute>& anAttribute);

  //! Returns true if <anAttribute> is in the data set.
  bool ContainsAttribute(const occ::handle<TDF_Attribute>& anAttribute) const;

  //! Returns the map of attributes in the current data set.
  //! This map can be used directly, or updated.
  NCollection_Map<occ::handle<TDF_Attribute>>& Attributes();

  //! Adds a root label to <myRootLabels>.
  void AddRoot(const TDF_Label& aLabel);

  //! Returns <myRootLabels> to be used or updated.
  NCollection_List<TDF_Label>& Roots();

  //! Dumps the minimum information about <me> on
  //! <aStream>.
  Standard_EXPORT Standard_OStream& Dump(Standard_OStream& anOS) const;

  Standard_OStream& operator<<(Standard_OStream& anOS) const { return Dump(anOS); }

  DEFINE_STANDARD_RTTIEXT(TDF_DataSet, Standard_Transient)

private:
  NCollection_List<TDF_Label>    myRootLabels;
  NCollection_Map<TDF_Label>     myLabelMap;
  NCollection_Map<occ::handle<TDF_Attribute>> myAttributeMap;
};

#include <TDF_DataSet.lxx>

#endif // _TDF_DataSet_HeaderFile
