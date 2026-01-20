// Created on: 1993-11-19
// Created by: Yves FRICAUD
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _MAT2d_BiInt_HeaderFile
#define _MAT2d_BiInt_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_HashUtils.hxx>

//! BiInt is a set of two integers.
class MAT2d_BiInt
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT MAT2d_BiInt(const int I1, const int I2);

  Standard_EXPORT int FirstIndex() const;

  Standard_EXPORT int SecondIndex() const;

  Standard_EXPORT void FirstIndex(const int I1);

  Standard_EXPORT void SecondIndex(const int I2);

  Standard_EXPORT bool IsEqual(const MAT2d_BiInt& B) const;

  bool operator==(const MAT2d_BiInt& B) const { return IsEqual(B); }

private:
  int i1;
  int i2;
};

namespace std
{
template <>
struct hash<MAT2d_BiInt>
{
  size_t operator()(const MAT2d_BiInt& theBiInt) const noexcept
  {
    // Combine two int values into a single hash value.
    int aCombination[2]{theBiInt.FirstIndex(), theBiInt.SecondIndex()};
    return opencascade::hashBytes(aCombination, sizeof(aCombination));
  }
};
} // namespace std

#endif // _MAT2d_BiInt_HeaderFile
