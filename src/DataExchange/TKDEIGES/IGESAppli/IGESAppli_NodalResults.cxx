// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESAppli_NodalResults.hxx>
#include <IGESAppli_Node.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_NodalResults, IGESData_IGESEntity)

IGESAppli_NodalResults::IGESAppli_NodalResults() = default;

// Data : Col -> // Nodes.  Row : Data per Node

void IGESAppli_NodalResults::Init(
  const occ::handle<IGESDimen_GeneralNote>&                            aNote,
  const int                                                            aNumber,
  const double                                                         aTime,
  const occ::handle<NCollection_HArray1<int>>&                         allNodeIdentifiers,
  const occ::handle<NCollection_HArray1<occ::handle<IGESAppli_Node>>>& allNodes,
  const occ::handle<NCollection_HArray2<double>>&                      allData)
{
  if (allNodes->Lower() != 1 || allNodeIdentifiers->Lower() != 1
      || allNodes->Length() != allNodeIdentifiers->Length() || allData->LowerCol() != 1
      || allData->LowerRow() != 1 || allNodes->Length() != allData->UpperRow())
    throw Standard_DimensionMismatch("IGESAppli_NodalResults : Init");
  theNote            = aNote;
  theSubCaseNum      = aNumber;
  theTime            = aTime;
  theNodeIdentifiers = allNodeIdentifiers;
  theNodes           = allNodes;
  theData            = allData;
  InitTypeAndForm(146, FormNumber());
  // FormNumber -> Type of the Results
}

void IGESAppli_NodalResults::SetFormNumber(const int form)
{
  if (form < 0 || form > 34)
    throw Standard_OutOfRange("IGESAppli_NodalResults : SetFormNumber");
  InitTypeAndForm(146, form);
}

occ::handle<IGESDimen_GeneralNote> IGESAppli_NodalResults::Note() const
{
  return theNote;
}

occ::handle<IGESAppli_Node> IGESAppli_NodalResults::Node(const int Index) const
{
  return theNodes->Value(Index);
}

int IGESAppli_NodalResults::NbNodes() const
{
  return theNodes->Length();
}

int IGESAppli_NodalResults::SubCaseNumber() const
{
  return theSubCaseNum;
}

double IGESAppli_NodalResults::Time() const
{
  return theTime;
}

int IGESAppli_NodalResults::NbData() const
{
  return theData->RowLength();
}

int IGESAppli_NodalResults::NodeIdentifier(const int Index) const
{
  return theNodeIdentifiers->Value(Index);
}

double IGESAppli_NodalResults::Data(const int NodeNum, const int DataNum) const
{
  return theData->Value(NodeNum, DataNum);
}
