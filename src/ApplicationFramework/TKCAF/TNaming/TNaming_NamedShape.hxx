// Created on: 1997-02-04
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

#ifndef _TNaming_NamedShape_HeaderFile
#define _TNaming_NamedShape_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TNaming_PtrNode.hxx>
#include <TNaming_Evolution.hxx>
#include <Standard_Integer.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_OStream.hxx>
class Standard_GUID;
class TopoDS_Shape;
class TDF_DeltaOnModification;
class TDF_DeltaOnRemoval;
class TDF_RelocationTable;
class TDF_DataSet;
class TDF_AttributeDelta;

//! The basis to define an attribute for the storage of
//! topology and naming data.
//! This attribute contains two parts:
//! -   The type of evolution, a term of the
//! enumeration TNaming_Evolution
//! -   A list of pairs of shapes called the "old"
//! shape and the "new" shape. The meaning
//! depends on the type of evolution.
class TNaming_NamedShape : public TDF_Attribute
{

public:
  //! class method
  //! ============
  //! Returns the GUID for named shapes.
  Standard_EXPORT static const Standard_GUID& GetID();

  Standard_EXPORT TNaming_NamedShape();

  Standard_EXPORT bool IsEmpty() const;

  //! Returns the shapes contained in <NS>. Returns a null
  //! shape if IsEmpty.
  Standard_EXPORT TopoDS_Shape Get() const;

  //! Returns the Evolution of the attribute.
  TNaming_Evolution Evolution() const;

  //! Returns the Version of the attribute.
  int Version() const;

  //! Set the Version of the attribute.
  void SetVersion(const int version);

  Standard_EXPORT void Clear();

  ~TNaming_NamedShape() { Clear(); }

  //! Returns the ID of the attribute.
  const Standard_GUID& ID() const override;

  //! Copies the attribute contents into a new other
  //! attribute. It is used by Backup().
  Standard_EXPORT virtual occ::handle<TDF_Attribute> BackupCopy() const override;

  //! Restores the contents from <anAttribute> into this
  //! one. It is used when aborting a transaction.
  Standard_EXPORT virtual void Restore(const occ::handle<TDF_Attribute>& anAttribute) override;

  //! Makes a DeltaOnModification between <me> and
  //! <anOldAttribute.
  Standard_EXPORT virtual occ::handle<TDF_DeltaOnModification> DeltaOnModification(
    const occ::handle<TDF_Attribute>& anOldAttribute) const override;

  //! Applies a DeltaOnModification to <me>.
  Standard_EXPORT virtual void DeltaOnModification(const occ::handle<TDF_DeltaOnModification>& aDelta)
    override;

  //! Makes a DeltaOnRemoval on <me> because <me> has
  //! disappeared from the DS.
  Standard_EXPORT virtual occ::handle<TDF_DeltaOnRemoval> DeltaOnRemoval() const override;

  //! Returns an new empty attribute from the good end
  //! type. It is used by the copy algorithm.
  Standard_EXPORT virtual occ::handle<TDF_Attribute> NewEmpty() const override;

  //! This method is different from the "Copy" one,
  //! because it is used when copying an attribute from
  //! a source structure into a target structure. This
  //! method pastes the current attribute to the label
  //! corresponding to the insertor. The pasted
  //! attribute may be a brand new one or a new version
  //! of the previous one.
  Standard_EXPORT virtual void Paste(const occ::handle<TDF_Attribute>&       intoAttribute,
                                     const occ::handle<TDF_RelocationTable>& aRelocTationable) const
    override;

  //! Adds the directly referenced attributes and labels
  //! to <aDataSet>. "Directly" means we have only to
  //! look at the first level of references.
  Standard_EXPORT virtual void References(const occ::handle<TDF_DataSet>& aDataSet) const
    override;

  Standard_EXPORT virtual void BeforeRemoval() override;

  //! Something to do before applying <anAttDelta>
  Standard_EXPORT virtual bool BeforeUndo(
    const occ::handle<TDF_AttributeDelta>& anAttDelta,
    const bool            forceIt = false) override;

  //! Something to do after applying <anAttDelta>.
  Standard_EXPORT virtual bool AfterUndo(
    const occ::handle<TDF_AttributeDelta>& anAttDelta,
    const bool            forceIt = false) override;

  //! Dumps the attribute on <aStream>.
  Standard_EXPORT virtual Standard_OStream& Dump(Standard_OStream& anOS) const override;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        int  theDepth = -1) const override;

  friend class TNaming_Builder;
  friend class TNaming_Iterator;
  friend class TNaming_NewShapeIterator;
  friend class TNaming_OldShapeIterator;

  DEFINE_STANDARD_RTTIEXT(TNaming_NamedShape, TDF_Attribute)

private:
  //! Adds an evolution
  Standard_EXPORT void Add(TNaming_PtrNode& Evolution);

  TNaming_PtrNode   myNode;
  TNaming_Evolution myEvolution;
  int  myVersion;
};

#include <TNaming_NamedShape.lxx>

#endif // _TNaming_NamedShape_HeaderFile
