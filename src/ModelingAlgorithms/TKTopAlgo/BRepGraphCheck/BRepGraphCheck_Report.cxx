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

#include <BRepGraphCheck_Report.hxx>

//=================================================================================================

void BRepGraphCheck_Report::AddIssue(const BRepGraphCheck_Issue& theIssue)
{
  std::lock_guard<std::mutex> aLock(myMutex);
  const int anIdx = myIssues.Length();
  myIssues.Append(theIssue);

  if (theIssue.NodeId.IsValid())
  {
    if (!myNodeIndex.IsBound(theIssue.NodeId))
    {
      myNodeIndex.Bind(theIssue.NodeId, NCollection_Vector<int>());
    }
    myNodeIndex.ChangeFind(theIssue.NodeId).Append(anIdx);
  }
}

//=================================================================================================

void BRepGraphCheck_Report::AddIssues(const NCollection_Vector<BRepGraphCheck_Issue>& theIssues)
{
  if (theIssues.IsEmpty())
    return;

  std::lock_guard<std::mutex> aLock(myMutex);
  for (int anIssueIter = 0; anIssueIter < theIssues.Length(); ++anIssueIter)
  {
    const BRepGraphCheck_Issue& anIssue = theIssues.Value(anIssueIter);
    const int anIdx = myIssues.Length();
    myIssues.Append(anIssue);

    if (anIssue.NodeId.IsValid())
    {
      if (!myNodeIndex.IsBound(anIssue.NodeId))
      {
        myNodeIndex.Bind(anIssue.NodeId, NCollection_Vector<int>());
      }
      myNodeIndex.ChangeFind(anIssue.NodeId).Append(anIdx);
    }
  }
}

//=================================================================================================

const NCollection_Vector<BRepGraphCheck_Issue>& BRepGraphCheck_Report::Issues() const
{
  return myIssues;
}

//=================================================================================================

bool BRepGraphCheck_Report::HasIssues() const
{
  return !myIssues.IsEmpty();
}

//=================================================================================================

bool BRepGraphCheck_Report::HasIssuesForNode(BRepGraph_NodeId theNode) const
{
  std::lock_guard<std::mutex> aLock(myMutex);
  return myNodeIndex.IsBound(theNode);
}

//=================================================================================================

NCollection_Vector<BRepGraphCheck_Issue> BRepGraphCheck_Report::IssuesForNode(
  BRepGraph_NodeId theNode) const
{
  NCollection_Vector<BRepGraphCheck_Issue> aResult;
  std::lock_guard<std::mutex>              aLock(myMutex);
  if (!myNodeIndex.IsBound(theNode))
    return aResult;

  const NCollection_Vector<int>& anIndices = myNodeIndex.Find(theNode);
  for (int anIdxIter = 0; anIdxIter < anIndices.Length(); ++anIdxIter)
  {
    aResult.Append(myIssues.Value(anIndices.Value(anIdxIter)));
  }
  return aResult;
}

//=================================================================================================

bool BRepGraphCheck_Report::IsValid() const
{
  std::lock_guard<std::mutex> aLock(myMutex);
  for (int anIssueIter = 0; anIssueIter < myIssues.Length(); ++anIssueIter)
  {
    const BRepGraphCheck_Issue::Severity aSev = myIssues.Value(anIssueIter).IssueSeverity;
    if (aSev == BRepGraphCheck_Issue::Severity::Error
        || aSev == BRepGraphCheck_Issue::Severity::Fatal)
    {
      return false;
    }
  }
  return true;
}

//=================================================================================================

void BRepGraphCheck_Report::Clear()
{
  std::lock_guard<std::mutex> aLock(myMutex);
  myIssues.Clear();
  myNodeIndex.Clear();
}
