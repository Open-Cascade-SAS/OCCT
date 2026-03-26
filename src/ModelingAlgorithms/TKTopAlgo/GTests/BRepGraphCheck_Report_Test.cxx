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

#include <BRepCheck_Status.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraphCheck_Issue.hxx>
#include <BRepGraphCheck_Report.hxx>

#include <gtest/gtest.h>

TEST(BRepGraphCheck_ReportTest, EmptyReport_IsValid)
{
  BRepGraphCheck_Report aReport;
  EXPECT_TRUE(aReport.IsValid());
  EXPECT_FALSE(aReport.HasIssues());
}

TEST(BRepGraphCheck_ReportTest, AddIssue_MakesInvalid)
{
  BRepGraphCheck_Report aReport;

  BRepGraphCheck_Issue anIssue;
  anIssue.NodeId        = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, 0);
  anIssue.Status        = BRepCheck_No3DCurve;
  anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
  aReport.AddIssue(anIssue);

  EXPECT_TRUE(aReport.HasIssues());
  EXPECT_FALSE(aReport.IsValid());
  EXPECT_EQ(aReport.Issues().Length(), 1);
}

TEST(BRepGraphCheck_ReportTest, WarningOnly_StillValid)
{
  BRepGraphCheck_Report aReport;

  BRepGraphCheck_Issue anIssue;
  anIssue.NodeId        = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, 0);
  anIssue.Status        = BRepCheck_FreeEdge;
  anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Warning;
  aReport.AddIssue(anIssue);

  EXPECT_TRUE(aReport.HasIssues());
  EXPECT_TRUE(aReport.IsValid());
}

TEST(BRepGraphCheck_ReportTest, IssuesForNode_ReturnsCorrectSubset)
{
  BRepGraphCheck_Report aReport;

  const BRepGraph_NodeId aNode0(BRepGraph_NodeId::Kind::Edge, 0);
  const BRepGraph_NodeId aNode1(BRepGraph_NodeId::Kind::Edge, 1);

  BRepGraphCheck_Issue anIssue0;
  anIssue0.NodeId        = aNode0;
  anIssue0.Status        = BRepCheck_No3DCurve;
  anIssue0.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
  aReport.AddIssue(anIssue0);

  BRepGraphCheck_Issue anIssue1;
  anIssue1.NodeId        = aNode1;
  anIssue1.Status        = BRepCheck_InvalidRange;
  anIssue1.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
  aReport.AddIssue(anIssue1);

  BRepGraphCheck_Issue anIssue2;
  anIssue2.NodeId        = aNode0;
  anIssue2.Status        = BRepCheck_InvalidToleranceValue;
  anIssue2.IssueSeverity = BRepGraphCheck_Issue::Severity::Warning;
  aReport.AddIssue(anIssue2);

  const NCollection_Vector<BRepGraphCheck_Issue> aNode0Issues = aReport.IssuesForNode(aNode0);
  EXPECT_EQ(aNode0Issues.Length(), 2);

  const NCollection_Vector<BRepGraphCheck_Issue> aNode1Issues = aReport.IssuesForNode(aNode1);
  EXPECT_EQ(aNode1Issues.Length(), 1);
}

TEST(BRepGraphCheck_ReportTest, Clear_ResetsEverything)
{
  BRepGraphCheck_Report aReport;

  BRepGraphCheck_Issue anIssue;
  anIssue.NodeId        = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, 0);
  anIssue.Status        = BRepCheck_NoSurface;
  anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
  aReport.AddIssue(anIssue);

  EXPECT_TRUE(aReport.HasIssues());
  aReport.Clear();
  EXPECT_FALSE(aReport.HasIssues());
  EXPECT_TRUE(aReport.IsValid());
}
