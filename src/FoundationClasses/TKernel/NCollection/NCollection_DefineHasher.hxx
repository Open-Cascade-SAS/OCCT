// Copyright (c) 2023 OPEN CASCADE SAS
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

#ifndef NCollection_DefineHasher_HeaderFile
#define NCollection_DefineHasher_HeaderFile

#define DEFINE_HASHER(HasherName, TheKeyType, HashFunctor, EqualFunctor)                           \
  struct HasherName : protected HashFunctor, EqualFunctor                                          \
  {                                                                                                \
    size_t operator()(const TheKeyType& theKey) const noexcept                                     \
    {                                                                                              \
      return HashFunctor::operator()(theKey);                                                      \
    }                                                                                              \
                                                                                                   \
    bool operator()(const TheKeyType& theK1, const TheKeyType& theK2) const noexcept               \
    {                                                                                              \
      return EqualFunctor::operator()(theK1, theK2);                                               \
    }                                                                                              \
  };

#endif
