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

#include <gtest/gtest.h>

#include <atomic>
#include <sstream>

#include <BRepPrimAPI_MakeBox.hxx>
#include <DESTEP_Parameters.hxx>
#include <OSD_Parallel.hxx>
#include <STEPCAFControl_Controller.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <STEPCAFControl_Writer.hxx>
#include <STEPControl_StepModelType.hxx>
#include <STEPControl_Writer.hxx>

// Test OCC33657 (case 1): STEPCAFControl_Reader and STEPCAFControl_Writer constructors
// can be created and destroyed in parallel without crashing.
TEST(STEPCAFControl_ControllerTest, OCC33657_ParallelConstructors)
{
  STEPCAFControl_Controller::Init();
  EXPECT_NO_FATAL_FAILURE(OSD_Parallel::For(0, 1000, [](int) {
    STEPCAFControl_Reader aReader;
    aReader.SetColorMode(true);
    STEPCAFControl_Writer aWriter;
    aWriter.SetDimTolMode(true);
  }));
}

// Test OCC33657 (case 3): STEPControl_Writer can write in parallel to in-memory buffers.
// Each thread creates its own shape and writer to avoid shared-state issues.
TEST(STEPCAFControl_ControllerTest, OCC33657_ParallelWritersToBuffer)
{
  STEPCAFControl_Controller::Init();
  std::atomic<bool> allOk{true};
  EXPECT_NO_FATAL_FAILURE(OSD_Parallel::For(0, 100, [&](int) {
    const TopoDS_Shape aShape = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();
    STEPControl_Writer  aWriter;
    aWriter.Transfer(aShape, STEPControl_StepModelType::STEPControl_AsIs, DESTEP_Parameters{});
    std::ostringstream aStream;
    aWriter.WriteStream(aStream);
    if (aStream.str().empty())
      allOk = false;
  }));
  EXPECT_TRUE(allOk);
}
