// Created on: 2004-11-23
// Created by: Pavel TELKOV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#include <TObj_Object.hxx>
#include <TObj_ReferenceIterator.hxx>
#include <TObj_TReference.hxx>


IMPLEMENT_STANDARD_RTTIEXT(TObj_ReferenceIterator,TObj_LabelIterator)

//=======================================================================
//function : TObj_ObjectIterator
//purpose  :
//=======================================================================

TObj_ReferenceIterator::TObj_ReferenceIterator
                         (const TDF_Label&             theLabel,
                          const Handle(Standard_Type)& theType,
                          const Standard_Boolean       theRecursive)
  : TObj_LabelIterator (theLabel, theRecursive), myType (theType)
{
  MakeStep();
}

//=======================================================================
//function : MakeStep
//purpose  :
//=======================================================================

void TObj_ReferenceIterator::MakeStep()
{
  for(;myIterator.More() && myNode.IsNull(); myIterator.Next())
  {
    TDF_Label L = myIterator.Value();

    Handle(TObj_TReference) A;
    if ( L.FindAttribute(TObj_TReference::GetID(), A) )
    {
      myObject = A->Get();
      if (! myType.IsNull() && !myObject.IsNull() && !myObject->IsKind( myType ))
        continue;

      myNode = L;
    }
  }
}
