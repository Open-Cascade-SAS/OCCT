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

#include <TDataStd_Expression.hxx>
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

    //! Import transient attribuite from the persistent data.
    void Import (const Handle(AttribClass)& theAttribute) const;

  private:
    Handle(StdLPersistent_HString::Extended)   myName;
    Handle(StdLPersistent_HArray1::Persistent) myVariables;
  };

public:
  typedef instance<TDataStd_Expression> Expression;
  typedef instance<TDataStd_Relation>   Relation;
};

#endif
