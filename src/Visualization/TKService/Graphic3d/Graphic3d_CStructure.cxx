// Copyright (c) 1995-1999 Matra Datavision
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

#include <Graphic3d_CStructure.hxx>

#include <Graphic3d_StructureManager.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Standard_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_CStructure, Standard_Transient)

//=================================================================================================

Graphic3d_CStructure::Graphic3d_CStructure(const Handle(Graphic3d_StructureManager)& theManager)
    : myGraphicDriver(theManager->GraphicDriver()),
      myId(-1),
      myZLayer(Graphic3d_ZLayerId_Default),
      myPriority(Graphic3d_DisplayPriority_Normal),
      myPreviousPriority(Graphic3d_DisplayPriority_Normal),
      myIsCulled(Standard_True),
      myBndBoxClipCheck(Standard_True),
      myHasGroupTrsf(Standard_False),
      //
      IsInfinite(0),
      stick(0),
      highlight(0),
      visible(1),
      HLRValidation(0),
      IsForHighlight(Standard_False),
      IsMutable(Standard_False),
      Is2dText(Standard_False)
{
  myId = myGraphicDriver->NewIdentification();
}

//=================================================================================================

void Graphic3d_CStructure::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  for (Graphic3d_SequenceOfGroup::Iterator anIterator(myGroups); anIterator.More();
       anIterator.Next())
  {
    const Handle(Graphic3d_Group)& aGroup = anIterator.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, aGroup.get())
  }

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myId)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myZLayer)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPriority)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPreviousPriority)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, IsInfinite)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, stick)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, highlight)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, visible)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, HLRValidation)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, IsForHighlight)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, IsMutable)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, Is2dText)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myBndBox)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myTrsf.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myTrsfPers.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myClipPlanes.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myHighlightStyle.get())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsCulled)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myBndBoxClipCheck)
}
