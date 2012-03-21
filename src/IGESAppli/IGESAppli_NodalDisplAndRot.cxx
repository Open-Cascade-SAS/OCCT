// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESAppli_NodalDisplAndRot.ixx>
#include <TColgp_HArray1OfXYZ.hxx>


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
    Standard_DimensionMismatch::Raise
  ("IGESAppli_NodalDisplAndRot : Init(Lengths of arrays inconsistent)");

  for (Standard_Integer i= 1; i <= allNodes->Length(); i++)
    {
      Handle(TColgp_HArray1OfXYZ) temp1 = allTransParams->Value(i);
      Handle(TColgp_HArray1OfXYZ) temp2 = allRotParams->Value(i);
      if ((temp1->Lower() != 1 || temp1->Length() != allNotes->Length()) ||
	  (temp2->Lower() != 1 || temp2->Length() != allNotes->Length()) )
	Standard_DimensionMismatch::Raise
	  ("IGESAppli_NodalDisplAndRot: Init(No. of Param per Node != Nbcases)");
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

