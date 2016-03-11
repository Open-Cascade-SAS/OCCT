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

#include <StdPersistent_DataXtd_PatternStd.hxx>

#include <TNaming_NamedShape.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_Integer.hxx>


//=======================================================================
//function : Import
//purpose  : Import transient attribuite from the persistent data
//=======================================================================
void StdPersistent_DataXtd_PatternStd::Import
  (const Handle(TDataXtd_PatternStd)& theAttribute) const
{
  theAttribute->Signature     (mySignature);
  theAttribute->Axis1Reversed (myAxis1Reversed);
  theAttribute->Axis2Reversed (myAxis2Reversed);

  if (mySignature < 5)
  {
    theAttribute->Axis1 (
      Handle(TNaming_NamedShape)::DownCast (myAxis1->GetAttribute()));

    theAttribute->Value1 (
      Handle(TDataStd_Real)::DownCast (myValue1->GetAttribute()));

    theAttribute->NbInstances1 (
      Handle(TDataStd_Integer)::DownCast (myNb1->GetAttribute()));

    if (mySignature > 2)
    {
      theAttribute->Axis2 (
        Handle(TNaming_NamedShape)::DownCast (myAxis2->GetAttribute()));

      theAttribute->Value2 (
        Handle(TDataStd_Real)::DownCast (myValue2->GetAttribute()));

      theAttribute->NbInstances2 (
        Handle(TDataStd_Integer)::DownCast (myNb2->GetAttribute()));
    }
  }
  else
  {
    theAttribute->Mirror (
      Handle(TNaming_NamedShape)::DownCast (myMirror->GetAttribute()));
  }
}
