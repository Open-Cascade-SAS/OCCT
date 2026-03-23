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

#ifndef _BRepGraphAlgo_Validate_HeaderFile
#define _BRepGraphAlgo_Validate_HeaderFile

#include <BRepGraph.hxx>

#include <BRepGraph_NodeId.hxx>
#include <NCollection_Vector.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TCollection_AsciiString.hxx>

//! @brief Structural invariant checker for BRepGraph.
//!
//! Read-only algorithm that verifies the graph's internal consistency:
//! cross-reference bounds, reverse index symmetry, usage-def coherence,
//! geometry reference validity, removed-node isolation, wire connectivity,
//! and parent-chain acyclicity.
//!
//! Distinct from BRepGraphCheck (geometric shape validity). This class
//! checks the graph data structure itself.
class BRepGraphAlgo_Validate
{
public:
  DEFINE_STANDARD_ALLOC

  //! Severity level for reported issues.
  enum class Severity { Warning, Error };

  //! A single structural issue found in the graph.
  struct Issue
  {
    Severity                Sev;
    BRepGraph_NodeId        NodeId;
    TCollection_AsciiString Description;
  };

  //! Aggregated validation result.
  struct Result
  {
    NCollection_Vector<Issue> Issues;

    //! True if no Error-level issues were found.
    bool IsValid() const
    {
      for (int anIdx = 0; anIdx < Issues.Length(); ++anIdx)
      {
        if (Issues.Value(anIdx).Sev == Severity::Error)
          return false;
      }
      return true;
    }

    //! Count issues of a given severity.
    int NbIssues(Severity theSev) const
    {
      int aCount = 0;
      for (int anIdx = 0; anIdx < Issues.Length(); ++anIdx)
      {
        if (Issues.Value(anIdx).Sev == theSev)
          ++aCount;
      }
      return aCount;
    }
  };

  //! Run all structural checks on a built graph.
  //! @param[in] theGraph graph to validate (const, read-only)
  //! @return validation result with all detected issues
  Standard_EXPORT static Result Perform(const BRepGraph& theGraph);

private:
  BRepGraphAlgo_Validate() = delete;
};

#endif // _BRepGraphAlgo_Validate_HeaderFile
