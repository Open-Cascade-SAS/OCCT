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

#include <TNaming_DataMapOfShapePtrRefShape.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_OStream.hxx>
class TNaming_Builder;
class Standard_GUID;
class TDF_Attribute;
class TDF_AttributeDelta;
class TDF_DeltaOnAddition;
class TDF_DeltaOnRemoval;
class TDF_RelocationTable;
class TDF_DataSet;


class TNaming_UsedShapes;
DEFINE_STANDARD_HANDLE(TNaming_UsedShapes, TDF_Attribute)

//! Global attribute located under root label to store all
//! the shapes handled by the framework
//! Set of Shapes Used in a Data from TDF
//! Only one instance by Data, it always
//! Stored as Attribute of The Root.
class TNaming_UsedShapes : public TDF_Attribute
{

public:

  
  Standard_EXPORT void Destroy();
~TNaming_UsedShapes()
{
  Destroy();
}
  
    TNaming_DataMapOfShapePtrRefShape& Map();
  
  //! Returns the ID of the attribute.
    const Standard_GUID& ID() const Standard_OVERRIDE;
  
  //! Returns the ID: 2a96b614-ec8b-11d0-bee7-080009dc3333.
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Copies  the attribute  contents into  a  new other
  //! attribute. It is used by Backup().
  Standard_EXPORT virtual Handle(TDF_Attribute) BackupCopy() const Standard_OVERRIDE;
  
  //! Restores the contents from <anAttribute> into this
  //! one. It is used when aborting a transaction.
  Standard_EXPORT virtual void Restore (const Handle(TDF_Attribute)& anAttribute) Standard_OVERRIDE;
  
  //! Clears the table.
  Standard_EXPORT virtual void BeforeRemoval() Standard_OVERRIDE;
  
  //! Something to do after applying <anAttDelta>.
  Standard_EXPORT virtual Standard_Boolean AfterUndo (const Handle(TDF_AttributeDelta)& anAttDelta, const Standard_Boolean forceIt = Standard_False) Standard_OVERRIDE;
  
  //! this method returns a null handle (no delta).
  Standard_EXPORT virtual Handle(TDF_DeltaOnAddition) DeltaOnAddition() const Standard_OVERRIDE;
  
  //! this method returns a null handle (no delta).
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
  //!
  //! For this, use only the AddLabel() & AddAttribute()
  //! from DataSet and do not try to modify information
  //! previously stored in <aDataSet>.
  Standard_EXPORT virtual void References (const Handle(TDF_DataSet)& aDataSet) const Standard_OVERRIDE;
  
  //! Dumps the attribute on <aStream>.
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;
  
  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

friend class TNaming_Builder;


  DEFINE_STANDARD_RTTIEXT(TNaming_UsedShapes,TDF_Attribute)

protected:




private:

  
  Standard_EXPORT TNaming_UsedShapes();

  TNaming_DataMapOfShapePtrRefShape myMap;


};


#include <TNaming_UsedShapes.lxx>





#endif // _TNaming_UsedShapes_HeaderFile
