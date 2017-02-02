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
#include <TColStd_HArray1OfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_NodalResults,IGESData_IGESEntity)

IGESAppli_NodalResults::IGESAppli_NodalResults ()    {  }


// Data : Col -> // Nodes.  Row : Data per Node

    void  IGESAppli_NodalResults::Init
  (const Handle(IGESDimen_GeneralNote)&    aNote,
   const Standard_Integer aNumber, const Standard_Real aTime,
   const Handle(TColStd_HArray1OfInteger)& allNodeIdentifiers,
   const Handle(IGESAppli_HArray1OfNode)&  allNodes,
   const Handle(TColStd_HArray2OfReal)&    allData)
{
  if (allNodes->Lower()   != 1 || allNodeIdentifiers->Lower() != 1 ||
      allNodes->Length()  != allNodeIdentifiers->Length() ||
      allData->LowerCol() != 1 || allData->LowerRow() != 1 ||
      allNodes->Length()  != allData->UpperRow() )
    throw Standard_DimensionMismatch("IGESAppli_NodalResults : Init");
  theNote            = aNote;
  theSubCaseNum      = aNumber;
  theTime            = aTime;
  theNodeIdentifiers = allNodeIdentifiers;
  theNodes           = allNodes;
  theData            = allData;
  InitTypeAndForm(146,FormNumber());
// FormNumber -> Type of the Results
}

    void  IGESAppli_NodalResults::SetFormNumber (const Standard_Integer form)
{
  if (form < 0 || form > 34) throw Standard_OutOfRange("IGESAppli_NodalResults : SetFormNumber");
  InitTypeAndForm(146,form);
}


    Handle(IGESDimen_GeneralNote)  IGESAppli_NodalResults::Note () const
{
  return theNote;
}

    Handle(IGESAppli_Node)  IGESAppli_NodalResults::Node
  (const Standard_Integer Index) const
{
  return theNodes->Value(Index);
}

    Standard_Integer  IGESAppli_NodalResults::NbNodes () const
{
  return theNodes->Length();
}

    Standard_Integer  IGESAppli_NodalResults::SubCaseNumber () const
{
  return theSubCaseNum;
}

    Standard_Real  IGESAppli_NodalResults::Time () const
{
  return theTime;
}

    Standard_Integer  IGESAppli_NodalResults::NbData () const
{
  return theData->RowLength();
}

    Standard_Integer  IGESAppli_NodalResults::NodeIdentifier
  (const Standard_Integer Index) const
{
  return theNodeIdentifiers->Value(Index);
}

    Standard_Real  IGESAppli_NodalResults::Data
  (const Standard_Integer NodeNum, const Standard_Integer DataNum) const
{
  return theData->Value(NodeNum,DataNum);
}
