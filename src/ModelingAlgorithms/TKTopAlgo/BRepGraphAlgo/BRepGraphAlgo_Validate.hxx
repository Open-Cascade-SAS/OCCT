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
//! cross-reference bounds, reverse index symmetry, incidence ref consistency,
//! geometry reference validity, removed-node isolation, and wire connectivity.
//!
//! Distinct from BRepGraphCheck (geometric shape validity). This class
//! checks the graph data structure itself.
//!
//! ### Validation Mode Check Matrix
//!
//! | Check                          | Lightweight | Audit |
//! |--------------------------------|:-----------:|:-----:|
//! | Active entity count boundary   |     YES     |  YES  |
//! | Cross-reference bounds         |      -      |  YES  |
//! | Reverse-index consistency      |      -      |  YES  |
//! | Face-count cache consistency   |      -      |  YES  |
//! | Incidence ref consistency      |      -      |  YES  |
//! | Geometry representation refs   |      -      |  YES  |
//! | Removed-node isolation         |      -      |  YES  |
//! | Wire edge connectivity         |      -      |  YES  |
//! | Entity ID positional integrity |      -      |  YES  |
//! | UID round-trip integrity       |      -      |  YES  |
class BRepGraphAlgo_Validate
{
public:
  DEFINE_STANDARD_ALLOC

  //! Severity level for reported issues.
  enum class Severity
  {
    Warning,
    Error
  };

  //! Validation mode controlling check depth/performance trade-off.
  enum class Mode
  {
    //! Fast boundary-oriented checks for frequent validation points.
    Lightweight,
    //! Full structural audit (superset of Lightweight).
    Audit,
    //! Backward-compatible alias.
    DeepAudit = Audit
  };

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
    [[nodiscard]] bool IsValid() const
    {
      for (int anIdx = 0; anIdx < Issues.Length(); ++anIdx)
      {
        if (Issues.Value(anIdx).Sev == Severity::Error)
          return false;
      }
      return true;
    }

    //! Count issues of a given severity.
    [[nodiscard]] int NbIssues(const Severity theSev) const
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

  //! Validation options.
  struct Options
  {
    //! Default mode for regular validation calls.
    Mode ValidationMode = Mode::Lightweight;

    //! Build options for lightweight validation.
    static Options Lightweight()
    {
      Options anOptions;
      anOptions.ValidationMode = Mode::Lightweight;
      return anOptions;
    }

    //! Build options for deep-audit validation.
    static Options DeepAudit() { return Audit(); }

    //! Build options for full-audit validation.
    static Options Audit()
    {
      Options anOptions;
      anOptions.ValidationMode = Mode::Audit;
      return anOptions;
    }
  };

  //! Run default lightweight structural checks on a built graph.
  //! Uses Mode::Lightweight; for full structural audit use Perform(theGraph, Mode::Audit).
  //! @param[in] theGraph graph to validate (const, read-only)
  //! @return validation result with all detected issues
  [[nodiscard]] Standard_EXPORT static Result Perform(const BRepGraph& theGraph);

  //! Run structural checks on a built graph with explicit mode.
  //! @param[in] theGraph graph to validate (const, read-only)
  //! @param[in] theMode validation mode
  //! @return validation result with all detected issues
  [[nodiscard]] Standard_EXPORT static Result Perform(const BRepGraph& theGraph,
                                                      const Mode       theMode);

  //! Run structural checks on a built graph with explicit options.
  //! @param[in] theGraph graph to validate (const, read-only)
  //! @param[in] theOptions validation profile/options
  //! @return validation result with all detected issues
  [[nodiscard]] Standard_EXPORT static Result Perform(const BRepGraph& theGraph,
                                                      const Options&   theOptions);

private:
  BRepGraphAlgo_Validate() = delete;
};

#endif // _BRepGraphAlgo_Validate_HeaderFile
