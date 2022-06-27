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


class TNaming_NamedShape;
DEFINE_STANDARD_HANDLE(TNaming_NamedShape, TDF_Attribute)

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
  
  Standard_EXPORT Standard_Boolean IsEmpty() const;
  
  //! Returns the shapes contained in <NS>. Returns a null
  //! shape if IsEmpty.
  Standard_EXPORT TopoDS_Shape Get() const;
  
  //! Returns the Evolution of the attribute.
    TNaming_Evolution Evolution() const;
  
  //! Returns the Version of the attribute.
    Standard_Integer Version() const;
  
  //! Set the Version of the attribute.
    void SetVersion (const Standard_Integer version);
  
  Standard_EXPORT void Clear();
~TNaming_NamedShape()
{
  Clear();
}
  
  //! Returns the ID of the attribute.
    const Standard_GUID& ID() const Standard_OVERRIDE;
  
  //! Copies  the attribute  contents into  a  new other
  //! attribute. It is used by Backup().
  Standard_EXPORT virtual Handle(TDF_Attribute) BackupCopy() const Standard_OVERRIDE;
  
  //! Restores the contents from <anAttribute> into this
  //! one. It is used when aborting a transaction.
  Standard_EXPORT virtual void Restore (const Handle(TDF_Attribute)& anAttribute) Standard_OVERRIDE;
  
  //! Makes a DeltaOnModification between <me> and
  //! <anOldAttribute.
  Standard_EXPORT virtual Handle(TDF_DeltaOnModification) DeltaOnModification (const Handle(TDF_Attribute)& anOldAttribute) const Standard_OVERRIDE;
  
  //! Applies a DeltaOnModification to <me>.
  Standard_EXPORT virtual void DeltaOnModification (const Handle(TDF_DeltaOnModification)& aDelta) Standard_OVERRIDE;
  
  //! Makes a DeltaOnRemoval on <me> because <me> has
  //! disappeared from the DS.
  Standard_EXPORT virtual Handle(TDF_DeltaOnRemoval) DeltaOnRemoval() const Standard_OVERRIDE;
  
  //! Returns an new empty attribute from the good end
  //! type. It is used by the copy algorithm.
  Standard_EXPORT virtual Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  //! This method is different from the "Copy" one,
  //! because it is used when copying an attribute from
  //! a source structure into a target structure. This
  //! method pastes the current attribute to the label
  //! corresponding to the insertor. The pasted
  //! attribute may be a brand new one or a new version
  //! of the previous one.
  Standard_EXPORT virtual void Paste (const Handle(TDF_Attribute)& intoAttribute, const Handle(TDF_RelocationTable)& aRelocTationable) const Standard_OVERRIDE;
  
  //! Adds the directly referenced attributes and labels
  //! to <aDataSet>. "Directly" means we have only to
  //! look at the first level of references.
  Standard_EXPORT virtual void References (const Handle(TDF_DataSet)& aDataSet) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void BeforeRemoval() Standard_OVERRIDE;
  
  //! Something to do before applying <anAttDelta>
  Standard_EXPORT virtual Standard_Boolean BeforeUndo (const Handle(TDF_AttributeDelta)& anAttDelta, const Standard_Boolean forceIt = Standard_False) Standard_OVERRIDE;
  
  //! Something to do after applying <anAttDelta>.
  Standard_EXPORT virtual Standard_Boolean AfterUndo (const Handle(TDF_AttributeDelta)& anAttDelta, const Standard_Boolean forceIt = Standard_False) Standard_OVERRIDE;
  
  //! Dumps the attribute on <aStream>.
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;
  
  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

friend class TNaming_Builder;
friend class TNaming_Iterator;
friend class TNaming_NewShapeIterator;
friend class TNaming_OldShapeIterator;


  DEFINE_STANDARD_RTTIEXT(TNaming_NamedShape,TDF_Attribute)

protected:




private:

  
  //! Adds an evolution
  Standard_EXPORT void Add (TNaming_PtrNode& Evolution);

  TNaming_PtrNode myNode;
  TNaming_Evolution myEvolution;
  Standard_Integer myVersion;


};


#include <TNaming_NamedShape.lxx>





#endif // _TNaming_NamedShape_HeaderFile
