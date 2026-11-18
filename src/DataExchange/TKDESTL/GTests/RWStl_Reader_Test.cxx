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

#include <Poly_Triangulation.hxx>
#include <RWStl.hxx>

#include <gtest/gtest.h>

#include <sstream>

TEST(RWStl_ReaderTest, RejectsTruncatedAsciiKeywords)
{
  for (const char* aContent : {"solid s\nf", "solid s\nfacet normal 0 0 1\no"})
  {
    std::istringstream anInput(aContent);
    EXPECT_TRUE(RWStl::ReadAsciiStream(anInput).IsNull());
  }
}

TEST(RWStl_ReaderTest, RejectsIncompleteFacet)
{
  std::istringstream anInput("solid s\n"
                             "facet normal 0 0 0\n"
                             "outer loop\n"
                             "vertex 0 0 0\n"
                             "endloop\n"
                             "endfacet\n"
                             "endsolid\n");
  EXPECT_TRUE(RWStl::ReadAsciiStream(anInput).IsNull());
}

TEST(RWStl_ReaderTest, RejectsMalformedFacetClosures)
{
  for (const char* aContent : {"solid s\n"
                               "facet normal 0 0 1\n"
                               "outer loop\n"
                               "vertex 0 0 0\n"
                               "vertex 1 0 0\n"
                               "vertex 0 1 0\n"
                               "not_endloop\n"
                               "endfacet\n"
                               "endsolid\n",
                               "solid s\n"
                               "facet normal 0 0 1\n"
                               "outer loop\n"
                               "vertex 0 0 0\n"
                               "vertex 1 0 0\n"
                               "vertex 0 1 0\n"
                               "endloop\n"
                               "not_endfacet\n"
                               "endsolid\n",
                               "solid s\n"
                               "facet normal 0 0 1\n"
                               "outer loop\n"
                               "vertex 0 0 0\n"
                               "vertex 1 0 0\n"
                               "vertex 0 1 0\n",
                               "solid s\n"
                               "facet normal 0 0 1\n"
                               "outer loop\n"
                               "vertex 0 0 0\n"
                               "vertex 1 0 0\n"
                               "vertex 0 1 0\n"
                               "endloop\n"})
  {
    std::istringstream anInput(aContent);
    EXPECT_TRUE(RWStl::ReadAsciiStream(anInput).IsNull());
  }
}

TEST(RWStl_ReaderTest, ReadsCompleteAsciiKeywords)
{
  std::istringstream                    anInput("solid triangle\n"
                                                "facet normal 0 0 1\n"
                                                "outer loop\n"
                                                "vertex 0 0 0\n"
                                                "vertex 1 0 0\n"
                                                "vertex 0 1 0\n"
                                                "endloop\n"
                                                "endfacet\n"
                                                "endsolid triangle\n");
  const occ::handle<Poly_Triangulation> aTriangulation = RWStl::ReadAsciiStream(anInput);
  ASSERT_FALSE(aTriangulation.IsNull());
  EXPECT_EQ(aTriangulation->NbTriangles(), 1);
}
