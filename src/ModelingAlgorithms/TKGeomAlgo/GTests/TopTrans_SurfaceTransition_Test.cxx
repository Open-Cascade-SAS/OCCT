// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <TopTrans_SurfaceTransition.hxx>

#include <Precision.hxx>
#include <TopAbs_State.hxx>
#include <TopAbs_Orientation.hxx>
#include <gp_Dir.hxx>

#include <gtest/gtest.h>

TEST(TopTrans_SurfaceTransition_Test, UndefinedStateStaysInstanceLocal)
{
  TopTrans_SurfaceTransition aReferenceTransition;
  aReferenceTransition.Reset(gp_Dir(1.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  aReferenceTransition.Compare(Precision::Angular(),
                               gp_Dir(0.0, 0.0, 1.0),
                               TopAbs_FORWARD,
                               TopAbs_FORWARD);

  const TopAbs_State aBefore = aReferenceTransition.StateBefore();
  const TopAbs_State anAfter = aReferenceTransition.StateAfter();
  ASSERT_NE(aBefore, TopAbs_UNKNOWN);
  ASSERT_NE(anAfter, TopAbs_UNKNOWN);

  TopTrans_SurfaceTransition anInvalidTransition;
  anInvalidTransition.Reset(gp_Dir(1.0, 0.0, 0.0),
                            gp_Dir(0.0, 0.0, 1.0),
                            gp_Dir(1.0, 0.0, 0.0),
                            gp_Dir(1.0, 0.0, 0.0),
                            1.0,
                            1.0);

  EXPECT_EQ(anInvalidTransition.StateBefore(), TopAbs_UNKNOWN);
  EXPECT_EQ(anInvalidTransition.StateAfter(), TopAbs_UNKNOWN);
  EXPECT_EQ(aReferenceTransition.StateBefore(), aBefore);
  EXPECT_EQ(aReferenceTransition.StateAfter(), anAfter);
}

TEST(TopTrans_SurfaceTransition_Test, CompareUsesProvidedTolerance)
{
  TopTrans_SurfaceTransition aTransition;
  aTransition.Reset(gp_Dir(1.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  aTransition.Compare(1.0,
                      gp_Dir(0.0, 0.0, 1.0),
                      gp_Dir(0.6, 0.8, 0.0),
                      gp_Dir(0.6, -0.8, 0.0),
                      1.0,
                      0.5,
                      TopAbs_FORWARD,
                      TopAbs_FORWARD);

  EXPECT_NE(aTransition.StateBefore(), TopAbs_UNKNOWN);
  EXPECT_NE(aTransition.StateAfter(), TopAbs_UNKNOWN);
}
