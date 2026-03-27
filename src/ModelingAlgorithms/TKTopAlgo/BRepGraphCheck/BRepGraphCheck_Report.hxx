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

#ifndef _BRepGraphCheck_Report_HeaderFile
#define _BRepGraphCheck_Report_HeaderFile

#include <BRepGraphCheck_Issue.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Vector.hxx>
#include <Standard_DefineAlloc.hxx>

#include <mutex>

//! Thread-safe collection of validation issues with per-node index.
//!
//! Issues are appended via AddIssue() / AddIssues() and automatically
//! indexed by NodeId for efficient per-node queries.  All public methods
//! are guarded by a mutex for safe concurrent use from parallel checkers.
class BRepGraphCheck_Report
{
public:
  DEFINE_STANDARD_ALLOC

  //! Add a single issue to the report.
  //! @param[in] theIssue diagnostic record to append
  Standard_EXPORT void AddIssue(const BRepGraphCheck_Issue& theIssue);

  //! Merge a batch of issues into the report (bulk insert).
  //! @param[in] theIssues vector of diagnostic records
  Standard_EXPORT void AddIssues(const NCollection_Vector<BRepGraphCheck_Issue>& theIssues);

  //! All collected issues.
  Standard_EXPORT const NCollection_Vector<BRepGraphCheck_Issue>& Issues() const;

  //! True if any issues were recorded.
  Standard_EXPORT bool HasIssues() const;

  //! True if any issues reference the given node.
  //! @param[in] theNode graph node to query
  Standard_EXPORT bool HasIssuesForNode(const BRepGraph_NodeId theNode) const;

  //! Return all issues referencing a specific node.
  //! @param[in] theNode graph node to query
  //! @return copy of issues for thread safety
  Standard_EXPORT NCollection_Vector<BRepGraphCheck_Issue> IssuesForNode(
    const BRepGraph_NodeId theNode) const;

  //! True if no Error or Fatal issues exist (warnings are acceptable).
  Standard_EXPORT bool IsValid() const;

  //! Remove all issues and clear the index.
  Standard_EXPORT void Clear();

private:
  NCollection_Vector<BRepGraphCheck_Issue>                       myIssues;
  NCollection_DataMap<BRepGraph_NodeId, NCollection_Vector<int>> myNodeIndex;
  mutable std::mutex                                             myMutex;
};

#endif // _BRepGraphCheck_Report_HeaderFile
