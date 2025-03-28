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

#ifndef _MergeSTEPEntities_LineHasher_HeaderFile
#define _MergeSTEPEntities_LineHasher_HeaderFile

#include <MergeSTEPEntities_CartesianPointHasher.hxx>
#include <MergeSTEPEntities_VectorHasher.hxx>

#include <Standard_HashUtils.hxx>
#include <StepGeom_Line.hxx>
#include <TCollection_HAsciiString.hxx>

//! OCCT-style hasher for StepGeom_Line entities.
struct MergeSTEPEntities_LineHasher
{
  // Hashes the Line by its name and Line ratios.
  std::size_t operator()(const Handle(StepGeom_Line)& theLine) const noexcept
  {
    const size_t aHashes[2]{MergeSTEPEntities_CartesianPointHasher{}(theLine->Pnt()),
                            MergeSTEPEntities_VectorHasher{}(theLine->Dir())};

    const size_t aCombinedHash = opencascade::hashBytes(aHashes, sizeof(aHashes));
    if (theLine->Name().IsNull())
    {
      // If the name is not present, return the hash.
      return aCombinedHash;
    }
    // Add the name to the hash if it is present.
    const size_t aCombinedHashWithName[2]{
      aCombinedHash,
      std::hash<TCollection_AsciiString>{}(theLine->Name()->String())};
    return opencascade::hashBytes(aCombinedHashWithName, sizeof(aCombinedHashWithName));
  }

  // Compares two Lines by their names and Line ratios.
  bool operator()(const Handle(StepGeom_Line)& theLine1,
                  const Handle(StepGeom_Line)& theLine2) const noexcept

  {
    // Compare names.
    if (theLine1->Name().IsNull() != theLine2->Name().IsNull())
    {
      return false;
    }
    if (!theLine1->Name()->IsSameString(theLine2->Name()))
    {
      return false;
    }

    // Compare points.
    if (!MergeSTEPEntities_CartesianPointHasher{}(theLine1->Pnt(), theLine2->Pnt()))
    {
      return false;
    }

    // Compare directions.
    if (!MergeSTEPEntities_VectorHasher{}(theLine1->Dir(), theLine2->Dir()))
    {
      return false;
    }

    return true;
  }
};

#endif // _MergeSTEPEntities_LineHasher_HeaderFile
