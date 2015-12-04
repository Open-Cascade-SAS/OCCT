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

#include <TObj_OcafObjectIterator.hxx>
#include <TObj_Object.hxx>


IMPLEMENT_STANDARD_RTTIEXT(TObj_OcafObjectIterator,TObj_LabelIterator)

//=======================================================================
//function : TObj_OcafObjectIterator
//purpose  :
//=======================================================================

TObj_OcafObjectIterator::TObj_OcafObjectIterator
                         (const TDF_Label&             theLabel,
                          const Handle(Standard_Type)& theType,
                          const Standard_Boolean       theRecursive)
     : TObj_LabelIterator( theLabel, theRecursive ), myType( theType )
{
  MakeStep();
}

//=======================================================================
//function : MakeStep
//purpose  :
//=======================================================================

void TObj_OcafObjectIterator::MakeStep()
{
  for(;myIterator.More() && myNode.IsNull(); )
  {
    TDF_Label L = myIterator.Value();
    Handle(TObj_Object) anObject;
    if(TObj_Object::GetObj(L,anObject))
    {
      if (myType.IsNull() || anObject->IsKind( myType ))
      {
        myObject = anObject;
        myNode = L;
      }
      myIterator.NextBrother();
    }
    else
      myIterator.Next();
  }
}
