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

#include <gp_XYZ.hxx>
#include <IGESAppli_NodalDisplAndRot.hxx>
#include <IGESAppli_Node.hxx>
#include <IGESBasic_HArray1OfHArray1OfXYZ.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TColgp_HArray1OfXYZ.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_NodalDisplAndRot,IGESData_IGESEntity)

IGESAppli_NodalDisplAndRot::IGESAppli_NodalDisplAndRot ()    {  }


    void  IGESAppli_NodalDisplAndRot::Init
  (const Handle(IGESDimen_HArray1OfGeneralNote)& allNotes,
   const Handle(TColStd_HArray1OfInteger)& allIdentifiers,
   const Handle(IGESAppli_HArray1OfNode)&  allNodes,
   const Handle(IGESBasic_HArray1OfHArray1OfXYZ)& allRotParams,
   const Handle(IGESBasic_HArray1OfHArray1OfXYZ)& allTransParams)
{
  if ( allNodes->Lower() != 1 ||
      (allIdentifiers->Lower()   != 1 ||
       allIdentifiers->Length()  != allNodes->Length()) ||
      (allTransParams->Lower()   != 1 ||
       allTransParams->Length()  != allNodes->Length()) ||
      (allRotParams->Lower()     != 1 ||
       allRotParams->Length()    != allNodes->Length()) )
    throw Standard_DimensionMismatch("IGESAppli_NodalDisplAndRot : Init(Lengths of arrays inconsistent)");

  for (Standard_Integer i= 1; i <= allNodes->Length(); i++)
    {
      Handle(TColgp_HArray1OfXYZ) temp1 = allTransParams->Value(i);
      Handle(TColgp_HArray1OfXYZ) temp2 = allRotParams->Value(i);
      if ((temp1->Lower() != 1 || temp1->Length() != allNotes->Length()) ||
	  (temp2->Lower() != 1 || temp2->Length() != allNotes->Length()) )
	throw Standard_DimensionMismatch("IGESAppli_NodalDisplAndRot: Init(No. of Param per Node != Nbcases)");
    }

  theNotes           = allNotes;
  theNodes           = allNodes;
  theNodeIdentifiers = allIdentifiers;
  theTransParam      = allTransParams;
  theRotParam        = allRotParams;
  InitTypeAndForm(138,0);
}

    Standard_Integer  IGESAppli_NodalDisplAndRot::NbCases () const
{
  return theNotes->Length();
}

    Standard_Integer  IGESAppli_NodalDisplAndRot::NbNodes () const
{
  return theNodes->Length();
}

    Handle(IGESDimen_GeneralNote)  IGESAppli_NodalDisplAndRot::Note
  (const Standard_Integer Index) const
{
  return theNotes->Value(Index);
}

    Standard_Integer  IGESAppli_NodalDisplAndRot::NodeIdentifier
  (const Standard_Integer Index) const
{
  return theNodeIdentifiers->Value(Index);
}

    Handle(IGESAppli_Node)  IGESAppli_NodalDisplAndRot::Node
  (const Standard_Integer Index) const
{
  return theNodes->Value(Index);
}

    gp_XYZ  IGESAppli_NodalDisplAndRot::TranslationParameter
  (const Standard_Integer NodeNum, const Standard_Integer CaseNum) const
{
  return theTransParam->Value(NodeNum)->Value(CaseNum);
}

    gp_XYZ  IGESAppli_NodalDisplAndRot::RotationalParameter
  (const Standard_Integer NodeNum, const Standard_Integer CaseNum) const
{
  return theRotParam->Value(NodeNum)->Value(CaseNum);
}

