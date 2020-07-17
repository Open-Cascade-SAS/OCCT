// Copyright (c) 2015 OPEN CASCADE SAS
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


#ifndef _ShapePersistent_HSequence_HeaderFile
#define _ShapePersistent_HSequence_HeaderFile

#include <Standard_NotImplemented.hxx>

#include <StdObjMgt_Persistent.hxx>
#include <StdObject_gp_Vectors.hxx>

#include <TColgp_HSequenceOfXYZ.hxx>
#include <TColgp_HSequenceOfPnt.hxx>
#include <TColgp_HSequenceOfDir.hxx>
#include <TColgp_HSequenceOfVec.hxx>


class ShapePersistent_HSequence
{
  template <class SequenceClass>
  class node : public StdObjMgt_Persistent
  {
  public:
    typedef typename SequenceClass::value_type ItemType;

  public:
    //! Read persistent data from a file.
    Standard_EXPORT virtual void Read (StdObjMgt_ReadData& theReadData);

    //! Write persistent data to a file.
    Standard_EXPORT virtual void Write (StdObjMgt_WriteData& theWriteData) const;

    //! Gets persistent objects
    virtual void PChildren (SequenceOfPersistent& theChildren) const
    {
      theChildren.Append(this->myPreviuos);
      theChildren.Append(this->myNext);
    }

    //! Returns persistent type name
    virtual Standard_CString PName() const
    {
      Standard_NotImplemented::Raise("ShapePersistent_HSequence::node::PName - not implemented");
      return "";
    }

    const Handle(node)& Previuos() const  { return myPreviuos; } 
    const Handle(node)& Next()     const  { return myNext; } 
    const ItemType&     Item()     const  { return myItem; }

  private:
    Handle(node) myPreviuos;
    Handle(node) myNext;
    ItemType     myItem;
  };

  template <class SequenceClass>
  class instance : public StdObjMgt_Persistent
  {
  public:
    typedef node<SequenceClass> Node;

  public:
    //! Empty constructor.
    instance()
    : mySize(0)
    {
    }

    //! Read persistent data from a file.
    Standard_EXPORT virtual void Read (StdObjMgt_ReadData& theReadData);

    //! Write persistent data to a file.
    Standard_EXPORT virtual void Write (StdObjMgt_WriteData& theWriteData) const;

    //! Gets persistent objects
    virtual void PChildren(SequenceOfPersistent& theChildren) const
    {
      theChildren.Append(this->myFirst);
      theChildren.Append(this->myLast);
    }

    //! Returns persistent type name
    virtual Standard_CString PName() const
    {
      Standard_NotImplemented::Raise("ShapePersistent_HSequence::instance::PName - not implemented");
      return "";
    }

    //! Import transient object from the persistent data.
    Standard_EXPORT Handle(SequenceClass) Import() const;

  private:
    Handle(Node)     myFirst;
    Handle(Node)     myLast;
    Standard_Integer mySize;
  };

public:
  typedef instance<TColgp_HSequenceOfXYZ> XYZ;
  typedef instance<TColgp_HSequenceOfPnt> Pnt;
  typedef instance<TColgp_HSequenceOfDir> Dir;
  typedef instance<TColgp_HSequenceOfVec> Vec;
};

//=======================================================================
// XYZ 
//=======================================================================
template<>
Standard_CString ShapePersistent_HSequence::instance<TColgp_HSequenceOfXYZ>
  ::PName() const;

template<>
Standard_CString ShapePersistent_HSequence::node<TColgp_HSequenceOfXYZ>
  ::PName() const;

//=======================================================================
// Pnt 
//=======================================================================
template<>
Standard_CString ShapePersistent_HSequence::instance<TColgp_HSequenceOfPnt>
  ::PName() const;

template<>
Standard_CString ShapePersistent_HSequence::node<TColgp_HSequenceOfPnt>
  ::PName() const;

//=======================================================================
// Dir 
//=======================================================================
template<>
Standard_CString ShapePersistent_HSequence::instance<TColgp_HSequenceOfDir>
  ::PName() const;

template<>
Standard_CString ShapePersistent_HSequence::node<TColgp_HSequenceOfDir>
  ::PName() const;

//=======================================================================
// Vec 
//=======================================================================
template<>
Standard_CString ShapePersistent_HSequence::instance<TColgp_HSequenceOfVec>
  ::PName() const;

template<>
Standard_CString ShapePersistent_HSequence::node<TColgp_HSequenceOfVec>
  ::PName() const;

#endif
