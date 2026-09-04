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

#include <BRepGraph_UsagePath.hxx>

#include <Standard_HashUtils.hxx>

//=================================================================================================

bool BRepGraph_UsagePath::IsEqual(const BRepGraph_UsagePath& theOther) const
{
  if (Size() != theOther.Size())
  {
    return false;
  }
  for (size_t anIdx = 0; anIdx < Size(); ++anIdx)
  {
    if (!(Value(anIdx) == theOther.Value(anIdx)))
    {
      return false;
    }
  }
  return true;
}

//=================================================================================================

size_t BRepGraph_UsagePath::HashCode() const
{
  const size_t aSize = mySteps.Size();
  if (aSize == 0)
  {
    return opencascade::hash(aSize);
  }

  const Step&  aFirst    = mySteps.First();
  const Step&  aMiddle   = mySteps.Value(aSize / 2);
  const Step&  aLast     = mySteps.Last();
  const size_t aValues[] = {aSize,
                            static_cast<size_t>(aFirst.Node.NodeKind),
                            static_cast<size_t>(aFirst.Node.Index),
                            static_cast<size_t>(aFirst.Ref.RefKind),
                            static_cast<size_t>(aFirst.Ref.Index),
                            static_cast<size_t>(aFirst.StepIndex),
                            static_cast<size_t>(aMiddle.Node.NodeKind),
                            static_cast<size_t>(aMiddle.Node.Index),
                            static_cast<size_t>(aMiddle.Ref.RefKind),
                            static_cast<size_t>(aMiddle.Ref.Index),
                            static_cast<size_t>(aMiddle.StepIndex),
                            static_cast<size_t>(aLast.Node.NodeKind),
                            static_cast<size_t>(aLast.Node.Index),
                            static_cast<size_t>(aLast.Ref.RefKind),
                            static_cast<size_t>(aLast.Ref.Index),
                            static_cast<size_t>(aLast.StepIndex)};
  return opencascade::hashBytes(aValues, sizeof(aValues));
}
