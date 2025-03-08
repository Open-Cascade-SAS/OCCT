// Created on: 2003-10-08
// Created by: Alexander SOLOVYOV
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef MeshVS_TwoColors_HeaderFile
#define MeshVS_TwoColors_HeaderFile

#include <Quantity_Color.hxx>

struct MeshVS_TwoColors
{
  unsigned int r1 : 8;
  unsigned int g1 : 8;
  unsigned int b1 : 8;
  unsigned int r2 : 8;
  unsigned int g2 : 8;
  unsigned int b2 : 8;

  bool operator==(const MeshVS_TwoColors& TwoColors) const
  {
    return (((r1 * 256 + g1) * 256 + b1)
              == ((TwoColors.r1 * 256 + TwoColors.g1) * 256 + TwoColors.b1)
            && ((r2 * 256 + g2) * 256 + b2)
                 == ((TwoColors.r2 * 256 + TwoColors.g2) * 256 + TwoColors.b2));
  }
};

namespace std
{
template <>
struct hash<MeshVS_TwoColors>
{
  size_t operator()(const MeshVS_TwoColors& TwoColors) const noexcept
  {
    unsigned int aHash = 0;
    meshprsHashByte(aHash, TwoColors.r1);
    meshprsHashByte(aHash, TwoColors.g1);
    meshprsHashByte(aHash, TwoColors.b1);
    meshprsHashByte(aHash, TwoColors.r2);
    meshprsHashByte(aHash, TwoColors.g2);
    meshprsHashByte(aHash, TwoColors.b2);
    return std::hash<unsigned int>{}(aHash);
  }

private:
  void meshprsHashByte(unsigned int& theHash, const unsigned int theValues) const noexcept
  {
    theHash += (theValues);
    theHash += (theHash << 10);
    theHash ^= (theHash >> 6);
  }
};
} // namespace std

Standard_EXPORT MeshVS_TwoColors BindTwoColors(const Quantity_Color&, const Quantity_Color&);
Standard_EXPORT Quantity_Color   ExtractColor(MeshVS_TwoColors&, const Standard_Integer);
Standard_EXPORT void             ExtractColors(MeshVS_TwoColors&, Quantity_Color&, Quantity_Color&);

#endif
