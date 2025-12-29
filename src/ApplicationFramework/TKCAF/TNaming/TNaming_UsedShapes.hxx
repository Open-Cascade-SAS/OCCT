// Created on: 1997-02-05
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

#ifndef _TNaming_UsedShapes_HeaderFile
#define _TNaming_UsedShapes_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <TNaming_PtrRefShape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_OStream.hxx>
class Standard_GUID;
class TDF_AttributeDelta;
class TDF_DeltaOnAddition;
class TDF_DeltaOnRemoval;
class TDF_RelocationTable;
class TDF_DataSet;

//! Global attribute located under root label to store all
//! the shapes handled by the framework
//! Set of Shapes Used in a Data from TDF
//! Only one instance by Data, it always
//! Stored as Attribute of The Root.
class TNaming_UsedShapes : public TDF_Attribute
{

public:
  Standard_EXPORT void Destroy();

  ~TNaming_UsedShapes() override { Destroy(); }

  NCollection_DataMap<TopoDS_Shape, TNaming_PtrRefShape, TopTools_ShapeMapHasher>& Map();

  //! Returns the ID of the attribute.
  const Standard_GUID& ID() const override;

  //! Returns the ID: 2a96b614-ec8b-11d0-bee7-080009dc3333.
  Standard_EXPORT static const Standard_GUID& GetID();

  //! Copies the attribute contents into a new other
  //! attribute. It is used by Backup().
  Standard_EXPORT occ::handle<TDF_Attribute> BackupCopy() const override;

  //! Restores the contents from <anAttribute> into this
  //! one. It is used when aborting a transaction.
  Standard_EXPORT void Restore(const occ::handle<TDF_Attribute>& anAttribute) override;

  //! Clears the table.
  Standard_EXPORT void BeforeRemoval() override;

  //! Something to do after applying <anAttDelta>.
  Standard_EXPORT bool AfterUndo(const occ::handle<TDF_AttributeDelta>& anAttDelta,
                                 const bool                             forceIt = false) override;

  //! this method returns a null handle (no delta).
  Standard_EXPORT occ::handle<TDF_DeltaOnAddition> DeltaOnAddition() const override;

  //! this method returns a null handle (no delta).
  Standard_EXPORT occ::handle<TDF_DeltaOnRemoval> DeltaOnRemoval() const override;

  //! Returns an new empty attribute from the good end
  //! type. It is used by the copy algorithm.
  Standard_EXPORT occ::handle<TDF_Attribute> NewEmpty() const override;

  //! This method is different from the "Copy" one,
  //! because it is used when copying an attribute from
  //! a source structure into a target structure. This
  //! method pastes the current attribute to the label
  //! corresponding to the insertor. The pasted
  //! attribute may be a brand new one or a new version
  //! of the previous one.
  Standard_EXPORT void Paste(
    const occ::handle<TDF_Attribute>&       intoAttribute,
    const occ::handle<TDF_RelocationTable>& aRelocTationable) const override;

  //! Adds the directly referenced attributes and labels
  //! to <aDataSet>. "Directly" means we have only to
  //! look at the first level of references.
  //!
  //! For this, use only the AddLabel() & AddAttribute()
  //! from DataSet and do not try to modify information
  //! previously stored in <aDataSet>.
  Standard_EXPORT void References(const occ::handle<TDF_DataSet>& aDataSet) const override;

  //! Dumps the attribute on <aStream>.
  Standard_EXPORT Standard_OStream& Dump(Standard_OStream& anOS) const override;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

  friend class TNaming_Builder;

  DEFINE_STANDARD_RTTIEXT(TNaming_UsedShapes, TDF_Attribute)

private:
  Standard_EXPORT TNaming_UsedShapes();

  NCollection_DataMap<TopoDS_Shape, TNaming_PtrRefShape, TopTools_ShapeMapHasher> myMap;
};

#include <TNaming_UsedShapes.lxx>

#endif // _TNaming_UsedShapes_HeaderFile
