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

#include <Standard_HashUtils.hxx>

#include <gtest/gtest.h>

#include <array>

TEST(Standard_HashUtilsTest, FNVUnsignedBytesUseCanonicalValues)
{
  const std::array<uint8_t, 4> aBytes = {0x00u, 0x7fu, 0x80u, 0xffu};
  EXPECT_EQ(opencascade::FNVHash::FNVHash1A(aBytes.data(),
                                            static_cast<int>(aBytes.size()),
                                            opencascade::FNVHash::optimalSeed<uint32_t>()),
            0x84657fcdu);
  EXPECT_EQ(opencascade::FNVHash::FNVHash64A(aBytes.data(),
                                             static_cast<int>(aBytes.size()),
                                             opencascade::FNVHash::optimalSeed<uint64_t>()),
            0x29bdf07e674617edull);
}

TEST(Standard_HashUtilsTest, FNVUnsignedBytesCanBeUpdatedIncrementally)
{
  const std::array<uint8_t, 4> aBytes = {0x00u, 0x7fu, 0x80u, 0xffu};
  const uint64_t               aComplete =
    opencascade::FNVHash::FNVHash64A(aBytes.data(),
                                     static_cast<int>(aBytes.size()),
                                     opencascade::FNVHash::optimalSeed<uint64_t>());
  const uint64_t aFirst =
    opencascade::FNVHash::FNVHash64A(aBytes.data(),
                                     2,
                                     opencascade::FNVHash::optimalSeed<uint64_t>());
  const uint64_t aIncremental = opencascade::FNVHash::FNVHash64A(aBytes.data() + 2, 2, aFirst);
  EXPECT_EQ(aIncremental, aComplete);
}
