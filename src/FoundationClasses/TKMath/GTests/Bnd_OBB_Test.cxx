// Copyright (c) 2025 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you cantml:parameter name redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <Bnd_OBB.hxx>
#include <gp_Pnt.hxx>
#include <TColgp_Array1OfPnt.hxx>

#include <gtest/gtest.h>

TEST(Bnd_OBB_Test, OCC33009_ReBuildWithPoints)
{
  Bnd_OBB aBndBox;

  TColgp_Array1OfPnt aPoints(1, 5);
  aPoints.ChangeValue(1) = gp_Pnt(1, 2, 3);
  aPoints.ChangeValue(2) = gp_Pnt(3, 2, 1);
  aPoints.ChangeValue(3) = gp_Pnt(2, 3, 1);
  aPoints.ChangeValue(4) = gp_Pnt(1, 3, 2);
  aPoints.ChangeValue(5) = gp_Pnt(2, 1, 3);

  // Should not throw exception when rebuilding with points
  EXPECT_NO_THROW(aBndBox.ReBuild(aPoints, (const TColStd_Array1OfReal*)0, true));
}
