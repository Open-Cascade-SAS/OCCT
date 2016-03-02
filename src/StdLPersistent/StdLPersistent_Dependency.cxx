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

#include <StdLPersistent_Dependency.hxx>


static void ImportName (const Handle(TDataStd_Expression)& theAttribute,
                        const TCollection_ExtendedString&  theName)
  { theAttribute->SetExpression (theName); }

static void ImportName (const Handle(TDataStd_Relation)&  theAttribute,
                        const TCollection_ExtendedString& theName)
  { theAttribute->SetRelation (theName); }


//=======================================================================
//function : Import
//purpose  : Import transient attribuite from the persistent data
//=======================================================================
template <class AttribClass>
void StdLPersistent_Dependency::instance<AttribClass>::Import
  (const Handle(AttribClass)& theAttribute) const
{
  if (myName)
    ImportName (theAttribute, myName->Value()->String());

  if (myVariables)
  {
    StdLPersistent_HArray1OfPersistent::Iterator anIter (*myVariables->Array());
    for (; anIter.More(); anIter.Next())
    {
      const Handle(StdObjMgt_Persistent) aPersistent = anIter.Value();
      if (aPersistent)
        theAttribute->GetVariables().Append (aPersistent->GetAttribute());
    }
  }
}


template class StdLPersistent_Dependency::instance<TDataStd_Expression>;
template class StdLPersistent_Dependency::instance<TDataStd_Relation>;
