// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <BRepPrimAPI_MakeBox.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESGeom_Line.hxx>
#include <IGESControl_Writer.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_FinderProcess.hxx>

#include <gtest/gtest.h>

// Check that 2d curves of shared edges are not lost in BRep mode.
TEST(IGESExportTest, SharedCurvesBRepMode)
{
  TopoDS_Shape aSolid = (TopoDS_Solid)BRepPrimAPI_MakeBox(gp_Pnt(0, 0, 0), 1, 1, 1);

  Standard_Integer   aBRepMode = 1;
  IGESControl_Writer aWriter("MM", aBRepMode);
  aWriter.AddShape(aSolid);

  const Handle(Transfer_FinderProcess)& aFP    = aWriter.TransferProcess();
  const Handle(IGESData_IGESModel)&     aModel = aWriter.Model();

  for (Standard_Integer i = 1; i <= aFP->NbMapped(); i++)
  {
    Handle(Transfer_SimpleBinderOfTransient) aBinder =
      Handle(Transfer_SimpleBinderOfTransient)::DownCast(aFP->MapItem(i));
    if (aBinder.IsNull())
      continue;

    Handle(IGESData_IGESEntity) anEnt = Handle(IGESData_IGESEntity)::DownCast(aBinder->Result());
    if (anEnt.IsNull())
      continue;

    Handle(IGESGeom_Line) aLine = Handle(IGESGeom_Line)::DownCast(anEnt);
    if (aLine.IsNull())
      continue;

    // Check that all the entities are in the model.
    EXPECT_TRUE(aModel->DNum(anEnt) != 0);
  }
}
