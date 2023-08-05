// Created on: 1997-02-04
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

#ifndef _TDF_Label_HeaderFile
#define _TDF_Label_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TDF_LabelNodePtr.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
#include <TDF_AttributeIndexedMap.hxx>
class TDF_Attribute;
class TDF_Data;
class Standard_GUID;
class TDF_IDFilter;


//! This class provides basic operations  to define
//! a label in a data structure.
//! A label is a feature in the feature hierarchy. A
//! label is always connected to a Data from TDF.
//! To a label is attached attributes containing the
//! software components information.
//!
//! Label information:
//!
//! It is possible to know the tag, the father, the
//! depth in the tree of the label, if the label is
//! root, null or equal to another label.
//!
//! Comfort methods:
//! Some methods useful on a label.
//!
//! Attributes:
//!
//! It is possible to get an attribute in accordance
//! to an ID, or the yougest previous version of a
//! current attribute.
class TDF_Label 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs an empty label object.
    TDF_Label();
  
  //! Nullifies the label.
    void Nullify();
  
  //! Returns the Data owning <me>.
    Handle(TDF_Data) Data() const;
  
  //! Returns the tag of the label.
  //! This is the integer assigned randomly to a label
  //! in a data framework. This integer is used to
  //! identify this label in an entry.
    Standard_Integer Tag() const;
  
  //! Returns the label father. This label may be null
  //! if the label is root.
    const TDF_Label Father() const;
  
  //! Returns True if the <aLabel> is null, i.e. it has
  //! not been included in the data framework.
    Standard_Boolean IsNull() const;
  
  //! Sets or unsets <me> and all its descendants as
  //! imported label, according to <aStatus>.
  Standard_EXPORT void Imported (const Standard_Boolean aStatus) const;
  
  //! Returns True if the <aLabel> is imported.
    Standard_Boolean IsImported() const;
  
  //! Returns True if the <aLabel> is equal to me (same
  //! LabelNode*).
    Standard_Boolean IsEqual (const TDF_Label& aLabel) const;
  Standard_Boolean operator == (const TDF_Label& aLabel) const
{
  return IsEqual(aLabel);
}
  
    Standard_Boolean IsDifferent (const TDF_Label& aLabel) const;
  Standard_Boolean operator != (const TDF_Label& aLabel) const
{
  return IsDifferent(aLabel);
}
  
    Standard_Boolean IsRoot() const;
  
  //! Returns true if <me> owns an attribute with <anID> as ID.
  Standard_EXPORT Standard_Boolean IsAttribute (const Standard_GUID& anID) const;
  
  //! Adds an Attribute  to the current label. Raises if
  //! there is already one.
  Standard_EXPORT void AddAttribute (const Handle(TDF_Attribute)& anAttribute, const Standard_Boolean append = Standard_True) const;
  
  //! Forgets an  Attribute   from the  current  label,
  //! setting its   forgotten status true and  its valid
  //! status false. Raises if   the attribute is not in
  //! the structure.
  Standard_EXPORT void ForgetAttribute (const Handle(TDF_Attribute)& anAttribute) const;
  
  //! Forgets the  Attribute of  GUID <aguid> from   the
  //! current label   . If the   attribute doesn't exist
  //! returns False. Otherwise returns True.
  Standard_EXPORT Standard_Boolean ForgetAttribute (const Standard_GUID& aguid) const;
  
  //! Forgets all the attributes. Does it on also on the
  //! sub-labels if <clearChildren> is set to true. Of
  //! course, this method is compatible with Transaction
  //! & Delta mechanisms.
  Standard_EXPORT void ForgetAllAttributes (const Standard_Boolean clearChildren = Standard_True) const;
  
  //! Undo Forget action, setting its forgotten status
  //! false and its valid status true. Raises if the
  //! attribute is not in the structure.
  Standard_EXPORT void ResumeAttribute (const Handle(TDF_Attribute)& anAttribute) const;
  
  //! Finds an attribute of the current label, according
  //! to <anID>.
  //! If anAttribute is not a valid one, false is returned.
  //!
  //! The method returns True if found, False otherwise.
  //!
  //! A removed attribute cannot be found.
  Standard_EXPORT Standard_Boolean FindAttribute (const Standard_GUID& anID, Handle(TDF_Attribute)& anAttribute) const;
  
  //! Safe variant of FindAttribute() for arbitrary type of argument
  template <class T> 
  Standard_Boolean FindAttribute (const Standard_GUID& theID, Handle(T)& theAttr) const
  { 
    Handle(TDF_Attribute) anAttr;
    return FindAttribute (theID, anAttr) && ! (theAttr = Handle(T)::DownCast(anAttr)).IsNull();
  }

  //! Finds an attribute of the current label, according
  //! to <anID> and <aTransaction>. This attribute
  //! has/had to be a valid one for the given
  //! transaction index . So, this attribute is not
  //! necessary a valid one.
  //!
  //! The method returns True if found, False otherwise.
  //!
  //! A removed attribute cannot be found nor a backuped
  //! attribute of a removed one.
  Standard_EXPORT Standard_Boolean FindAttribute (const Standard_GUID& anID, const Standard_Integer aTransaction, Handle(TDF_Attribute)& anAttribute) const;
  
  //! Returns true if <me> or a DESCENDANT of <me> owns
  //! attributes not yet available in transaction 0. It
  //! means at least one of their attributes is new,
  //! modified or deleted.
    Standard_Boolean MayBeModified() const;
  
  //! Returns true if <me> owns attributes not yet
  //! available in transaction 0. It means at least one
  //! attribute is new, modified or deleted.
    Standard_Boolean AttributesModified() const;
  
  //! Returns true if this label has at least one attribute.
  Standard_EXPORT Standard_Boolean HasAttribute() const;
  
  //! Returns the number of attributes.
  Standard_EXPORT Standard_Integer NbAttributes() const;
  
  //! Returns the depth of the label in the data framework.
  //! This corresponds to the number of fathers which
  //! this label has, and is used in determining
  //! whether a label is root, null or equivalent to another label.
  //! Exceptions:
  //! Standard_NullObject if this label is null. This is
  //! because a null object can have no depth.
  Standard_EXPORT Standard_Integer Depth() const;
  
  //! Returns True if <me> is a descendant of
  //! <aLabel>. Attention: every label is its own
  //! descendant.
  Standard_EXPORT Standard_Boolean IsDescendant (const TDF_Label& aLabel) const;
  
  //! Returns the root label Root of the data structure.
  //! This has a depth of 0.
  //! Exceptions:
  //! Standard_NullObject if this label is null. This is
  //! because a null object can have no depth.
  Standard_EXPORT const TDF_Label Root() const;
  
  //! Returns true if this label has at least one child.
    Standard_Boolean HasChild() const;
  
  //! Returns the number of children.
  Standard_EXPORT Standard_Integer NbChildren() const;
  
  //! Finds a child label having <aTag> as tag. Creates
  //! The tag aTag identifies the label which will be the parent.
  //! If create is true and no child label is found, a new one is created.
  //! Example:
  //! //creating a label with tag 10 at Root
  //! TDF_Label lab1 = aDF->Root().FindChild(10);
  //! //creating labels 7 and 2 on label 10
  //! TDF_Label lab2 = lab1.FindChild(7);
  //! TDF_Label lab3 = lab1.FindChild(2);
  Standard_EXPORT TDF_Label FindChild (const Standard_Integer aTag, const Standard_Boolean create = Standard_True) const;
  
  //! Create  a new child   label of me  using autoamtic
  //! delivery tags provided by TagSource.
    TDF_Label NewChild() const;
  
  //! Returns the current transaction index.
  Standard_EXPORT Standard_Integer Transaction() const;
  
  //! Returns true if node address of <me> is lower than
  //! <otherLabel> one. Used to quickly sort labels (not
  //! on entry criterion).
  //!
  //! -C++: inline
  Standard_EXPORT Standard_Boolean HasLowerNode (const TDF_Label& otherLabel) const;
  
  //! Returns true if node address of <me> is greater
  //! than <otherLabel> one. Used to quickly sort labels
  //! (not on entry criterion).
  //!
  //! -C++: inline
  Standard_EXPORT Standard_Boolean HasGreaterNode (const TDF_Label& otherLabel) const;
  
  //! Dumps the minimum information about <me> on
  //! <aStream>.
  Standard_EXPORT Standard_OStream& Dump (Standard_OStream& anOS) const;
Standard_OStream& operator<< (Standard_OStream& anOS) const
{
  return Dump(anOS);
}
  
  //! Dumps the label on <aStream> and its attributes
  //! rank in <aMap> if their IDs are kept by <IDFilter>.
  Standard_EXPORT void ExtendedDump (Standard_OStream& anOS, const TDF_IDFilter& aFilter, TDF_AttributeIndexedMap& aMap) const;
  
  //! Dumps the label entry.
  Standard_EXPORT void EntryDump (Standard_OStream& anOS) const;


friend class TDF_ChildIterator;
friend class TDF_Attribute;
friend class TDF_AttributeIterator;
friend class TDF_Data;

template<class T>
friend struct std::hash;

protected:





private:

  
  //! Reserved to the friends.
    TDF_Label(const TDF_LabelNodePtr& aNode);
  
  //! Adds an Attribute to <toNode>. Raises if there is
  //! already one.
  Standard_EXPORT void AddToNode (const TDF_LabelNodePtr& toNode, const Handle(TDF_Attribute)& anAttribute, const Standard_Boolean append) const;
  
  //! Forgets an Attribute from <fromNode>.  Raises if
  //! the attribute is not in the structure.
  Standard_EXPORT void ForgetFromNode (const TDF_LabelNodePtr& fromNode, const Handle(TDF_Attribute)& anAttribute) const;
  
  //! Resumes a forgotten Attribute to <toNode>.  Raises
  //! if the attribute is not in the structure.
  Standard_EXPORT void ResumeToNode (const TDF_LabelNodePtr& fromNode, const Handle(TDF_Attribute)& anAttribute) const;
  
  Standard_EXPORT TDF_LabelNodePtr FindOrAddChild (const Standard_Integer aTag, const Standard_Boolean create) const;
  
  Standard_EXPORT void InternalDump (Standard_OStream& anOS, const TDF_IDFilter& aFilter, TDF_AttributeIndexedMap& aMap, const Standard_Boolean extended) const;


  TDF_LabelNodePtr myLabelNode;


};


#include <TDF_Label.lxx>





#endif // _TDF_Label_HeaderFile
