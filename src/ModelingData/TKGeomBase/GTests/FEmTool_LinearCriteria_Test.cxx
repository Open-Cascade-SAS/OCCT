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

#include <FEmTool_LinearFlexion.hxx>
#include <FEmTool_LinearJerk.hxx>
#include <FEmTool_LinearTension.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_ConstructionError.hxx>

#include <gtest/gtest.h>

TEST(FEmTool_LinearCriteria, SupportEachConstraintOrder)
{
  EXPECT_NO_THROW({ FEmTool_LinearFlexion aCriterion(4, GeomAbs_C0); });
  EXPECT_NO_THROW({ FEmTool_LinearFlexion aCriterion(4, GeomAbs_C1); });
  EXPECT_NO_THROW({ FEmTool_LinearFlexion aCriterion(4, GeomAbs_C2); });

  EXPECT_NO_THROW({ FEmTool_LinearJerk aCriterion(4, GeomAbs_C0); });
  EXPECT_NO_THROW({ FEmTool_LinearJerk aCriterion(4, GeomAbs_C1); });
  EXPECT_NO_THROW({ FEmTool_LinearJerk aCriterion(4, GeomAbs_C2); });

  EXPECT_NO_THROW({ FEmTool_LinearTension aCriterion(4, GeomAbs_C0); });
  EXPECT_NO_THROW({ FEmTool_LinearTension aCriterion(4, GeomAbs_C1); });
  EXPECT_NO_THROW({ FEmTool_LinearTension aCriterion(4, GeomAbs_C2); });
}

TEST(FEmTool_LinearCriteria, RejectUnsupportedDegreeAfterCacheInitialization)
{
  EXPECT_NO_THROW({ FEmTool_LinearFlexion aCriterion(14, GeomAbs_C0); });
  EXPECT_THROW({ FEmTool_LinearFlexion aCriterion(15, GeomAbs_C0); }, Standard_ConstructionError);

  EXPECT_NO_THROW({ FEmTool_LinearJerk aCriterion(14, GeomAbs_C0); });
  EXPECT_THROW({ FEmTool_LinearJerk aCriterion(15, GeomAbs_C0); }, Standard_ConstructionError);

  EXPECT_NO_THROW({ FEmTool_LinearTension aCriterion(14, GeomAbs_C0); });
  EXPECT_THROW({ FEmTool_LinearTension aCriterion(15, GeomAbs_C0); }, Standard_ConstructionError);
}
