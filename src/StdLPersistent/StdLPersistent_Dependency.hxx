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


#ifndef _StdLPersistent_Dependency_HeaderFile
#define _StdLPersistent_Dependency_HeaderFile

#include <StdObjMgt_Attribute.hxx>
#include <StdLPersistent_HString.hxx>
#include <StdLPersistent_HArray1.hxx>

#include <TDataStd_Relation.hxx>


class StdLPersistent_Dependency
{
  template <class AttribClass>
  class instance : public StdObjMgt_Attribute<AttribClass>
  {
  public:
    //! Read persistent data from a file.
    inline void Read (StdObjMgt_ReadData& theReadData)
      { theReadData >> myName >> myVariables; }
    //! Write persistent data to a file.
    inline void Write (StdObjMgt_WriteData& theWriteData) const
      { theWriteData << myName << myVariables; }
    //! Gets persistent child objects
    inline void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
    {
      theChildren.Append(myName);
      theChildren.Append(myVariables);
    }
    //! Returns persistent type name
    Standard_CString PName() const;

    //! Import transient attribute from the persistent data.
    void Import (const Handle(AttribClass)& theAttribute) const;

  private:
    Handle(StdLPersistent_HString::Extended)   myName;
    Handle(StdLPersistent_HArray1::Persistent) myVariables;
  };

public:
  typedef instance<TDataStd_Expression> Expression;
  typedef instance<TDataStd_Relation>   Relation;
};

template<>
inline Standard_CString StdLPersistent_Dependency::instance<TDataStd_Expression>::PName() const
  { return "PDataStd_Expression"; }

template<>
inline Standard_CString StdLPersistent_Dependency::instance<TDataStd_Relation>::PName() const
  { return "PDataStd_Relation"; }

#endif
