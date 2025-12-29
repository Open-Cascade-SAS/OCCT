// Created on: 1997-09-08
// Created by: Yves FRICAUD
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

#ifndef _TNaming_Naming_HeaderFile
#define _TNaming_Naming_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TNaming_Name.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <NCollection_Map.hxx>
#include <Standard_OStream.hxx>
#include <TDF_AttributeIndexedMap.hxx>
class Standard_GUID;
class TDF_Label;
class TNaming_NamedShape;
class TopoDS_Shape;
class TDF_RelocationTable;
class TDF_DataSet;
class TDF_IDFilter;

//! This attribute store the topological naming of any
//! selected shape, when this shape is not already
//! attached to a specific label. This class is also used
//! to solve it when the arguments of the topological
//! naming are modified.
class TNaming_Naming : public TDF_Attribute
{

public:
  //! following code from TDesignStd
  //! ==============================
  Standard_EXPORT static const Standard_GUID& GetID();

  Standard_EXPORT static occ::handle<TNaming_Naming> Insert(const TDF_Label& under);

  //! Creates a Naming attribute at label <where> to identify
  //! the shape <Selection>. Geometry is true if we
  //! are only interested by the underlying geometry (e.g.
  //! setting a constraint).
  //! <Context> is used to find neighbours of <S> when required
  //! by the naming. If KeepOrientation is True the Selection
  //! orientation is taken into account.
  //! BNproblem == True points out that Context sub-shapes in
  //! DF have orientation differences with Context shape itself.

  //! instance method
  //! ===============
  Standard_EXPORT static occ::handle<TNaming_NamedShape> Name(const TDF_Label&    where,
                                                              const TopoDS_Shape& Selection,
                                                              const TopoDS_Shape& Context,
                                                              const bool          Geometry = false,
                                                              const bool KeepOrientation   = false,
                                                              const bool BNproblem         = false);

  Standard_EXPORT TNaming_Naming();

  Standard_EXPORT bool IsDefined() const;

  Standard_EXPORT const TNaming_Name& GetName() const;

  Standard_EXPORT TNaming_Name& ChangeName();

  //! regenerate only the Name associated to me
  Standard_EXPORT bool Regenerate(NCollection_Map<TDF_Label>& scope);

  //! Regenerate recursively the whole name with scope. If
  //! scope is empty it means that all the labels of the
  //! framework are valid.
  Standard_EXPORT bool Solve(NCollection_Map<TDF_Label>& scope);

  //! Deferred methods from TDF_Attribute
  //! ===================================
  Standard_EXPORT const Standard_GUID& ID() const override;

  Standard_EXPORT occ::handle<TDF_Attribute> NewEmpty() const override;

  Standard_EXPORT void Restore(const occ::handle<TDF_Attribute>& With) override;

  Standard_EXPORT void Paste(const occ::handle<TDF_Attribute>&       Into,
                             const occ::handle<TDF_RelocationTable>& RT) const override;

  Standard_EXPORT void References(const occ::handle<TDF_DataSet>& aDataSet) const override;

  Standard_EXPORT Standard_OStream& Dump(Standard_OStream& anOS) const override;

  Standard_EXPORT void ExtendedDump(
    Standard_OStream&                                   anOS,
    const TDF_IDFilter&                                 aFilter,
    NCollection_IndexedMap<occ::handle<TDF_Attribute>>& aMap) const override;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

  DEFINE_STANDARD_RTTIEXT(TNaming_Naming, TDF_Attribute)

private:
  TNaming_Name myName;
};

#endif // _TNaming_Naming_HeaderFile
