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

#ifndef _BRepGraph_RevisionStatus_HeaderFile
#define _BRepGraph_RevisionStatus_HeaderFile

#include <NCollection_LinearVector.hxx>
#include <Standard_GUID.hxx>
#include <TCollection_AsciiString.hxx>

#include <cstdint>

//! Status and diagnostics for BRepGraph revision operations.
class BRepGraph_RevisionStatus
{
public:
  //! Result status for a revision operation.
  enum class Code
  {
    Ok,
    InvalidInput,
    CopyFailed,
    HashFailed,
    ValidationFailed,
    InvalidBase,
    AlreadyFinished,
    UnsupportedComponent
  };

  //! Severity of a diagnostic.
  enum class DiagnosticSeverity
  {
    Warning,
    Error
  };

  //! Structured diagnostic emitted by a revision operation.
  struct Diagnostic
  {
    DiagnosticSeverity      Severity = DiagnosticSeverity::Error;
    TCollection_AsciiString Code;
    TCollection_AsciiString Message;
    Standard_GUID           ComponentGUID;
    uint32_t                SchemaVersion = 0;
  };

  using Diagnostics = NCollection_LinearVector<Diagnostic>;

  //! Return true for a successful status.
  [[nodiscard]] static bool IsSuccess(const Code theStatus) noexcept
  {
    return theStatus == Code::Ok;
  }
};

#endif // _BRepGraph_RevisionStatus_HeaderFile
