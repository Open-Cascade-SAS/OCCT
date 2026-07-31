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

#ifndef _BRepGraphInc_ParityOrientation_HeaderFile
#define _BRepGraphInc_ParityOrientation_HeaderFile

#include <Standard_ProgramError.hxx>
#include <TopAbs_Orientation.hxx>

namespace BRepGraphInc
{

//! @brief Persisted core-topology orientation stored as forward/reversed parity only.
//!
//! The wrapper keeps storage compact through one bool while remaining implicitly
//! convertible to `TopAbs_Orientation` for existing orientation-facing codepaths.
struct ParityOrientation
{
  //! Stored parity bit: `false` for `TopAbs_FORWARD`, `true` for `TopAbs_REVERSED`.
  bool IsReversed = false;

  //! Constructs the parity wrapper from a core forward/reversed orientation.
  ParityOrientation() = default;

  //! Constructs the parity wrapper from a core forward/reversed orientation.
  ParityOrientation(const TopAbs_Orientation theOrientation)
      : IsReversed(toIsReversed(theOrientation))
  {
  }

  //! Assigns a core forward/reversed orientation.
  ParityOrientation& operator=(const TopAbs_Orientation theOrientation)
  {
    IsReversed = toIsReversed(theOrientation);
    return *this;
  }

  //! Converts stored parity back to `TopAbs_Orientation`.
  operator TopAbs_Orientation() const { return IsReversed ? TopAbs_REVERSED : TopAbs_FORWARD; }

private:
  //! Converts a core forward/reversed orientation to the stored parity bit.
  static bool toIsReversed(const TopAbs_Orientation theOrientation)
  {
    Standard_ProgramError_Raise_if(theOrientation != TopAbs_FORWARD
                                     && theOrientation != TopAbs_REVERSED,
                                   "BRepGraphInc::ParityOrientation stores only FORWARD/REVERSED");
    return theOrientation == TopAbs_REVERSED;
  }
};

} // namespace BRepGraphInc

#endif // _BRepGraphInc_ParityOrientation_HeaderFile
