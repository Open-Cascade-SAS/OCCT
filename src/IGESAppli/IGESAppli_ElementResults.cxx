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

// UNFINISHED & UNSTARTED
#include <IGESAppli_ElementResults.ixx>
#include <Standard_OutOfRange.hxx>


IGESAppli_ElementResults::IGESAppli_ElementResults ()    {  }

    void  IGESAppli_ElementResults::Init
  (const Handle(IGESDimen_GeneralNote)& aNote,
   const Standard_Integer aSubCase, const Standard_Real aTime,
   const Standard_Integer nbResults, const Standard_Integer aResRepFlag,
   const Handle(TColStd_HArray1OfInteger)& allElementIdents,
   const Handle(IGESAppli_HArray1OfFiniteElement)&    allFiniteElems,
   const Handle(TColStd_HArray1OfInteger)&            allTopTypes,
   const Handle(TColStd_HArray1OfInteger)&            nbLayers,
   const Handle(TColStd_HArray1OfInteger)&            allDataLayerFlags,
   const Handle(TColStd_HArray1OfInteger)&            allnbResDataLocs,
   const Handle(IGESBasic_HArray1OfHArray1OfInteger)& allResDataLocs,
   const Handle(IGESBasic_HArray1OfHArray1OfReal)&    allResults) // UNFINISHED
{
  // raises exception if sizes are not consistent, (lower = 1 too)
  // but how to check is not clear ??
  Standard_Integer num = allElementIdents->Length();
  if (allElementIdents->Lower()  != 1 ||
      allFiniteElems->Lower()    != 1 || allFiniteElems->Upper()    != num ||
      allTopTypes->Lower()       != 1 || allTopTypes->Upper()       != num ||
      nbLayers->Lower()          != 1 || nbLayers->Upper()          != num ||
      allDataLayerFlags->Lower() != 1 || allDataLayerFlags->Upper() != num ||
      allnbResDataLocs->Lower()  != 1 || allnbResDataLocs->Upper()  != num ||
      allResDataLocs->Lower()    != 1 || allResDataLocs->Upper()    != num ||
      allResults->Lower()        != 1 || allResults->Upper()        != num )
    Standard_DimensionMismatch::Raise("IGESAppli_ElementsResults : Init");
  for (Standard_Integer i = 1; i <= num; i ++) {
    Standard_Integer nl  = nbLayers->Value(i);
    Standard_Integer nrl = allnbResDataLocs->Value(i);
    if (allResDataLocs->Value(i)->Lower() != 1 || 
	allResDataLocs->Value(i)->Upper() != nrl)
      Standard_DimensionMismatch::Raise("IGESAppli_ElementsResults : DataLoc");
    if (allResults->Value(i)->Lower() != 1 ||
	allResults->Value(i)->Upper() != nl*nrl*nbResults)
      Standard_DimensionMismatch::Raise("IGESAppli_ElementsResults : Results");
  }

  theNote                 = aNote;
  theSubcaseNumber        = aSubCase;
  theTime                 = aTime;
  theNbResultValues       = nbResults;
  theResultReportFlag     = aResRepFlag;
  theElementIdentifiers   = allElementIdents;
  theElements             = allFiniteElems;
  theElementTopologyTypes = allTopTypes;
  theNbLayers             = nbLayers;
  theDataLayerFlags       = allDataLayerFlags;
  theNbResultDataLocs     = allnbResDataLocs;
  theResultDataLocs       = allResDataLocs;
  theResultData           = allResults;
  InitTypeAndForm(148,FormNumber());
// FormNumber -> Type of the Results
}

    void  IGESAppli_ElementResults::SetFormNumber (const Standard_Integer form)
{
  if (form < 0 || form > 34) Standard_OutOfRange::Raise
    ("IGESAppli_ElementResults : SetFormNumber");
  InitTypeAndForm(148,form);
}


    Handle(IGESDimen_GeneralNote)  IGESAppli_ElementResults::Note () const
{
  return theNote;
}

    Standard_Integer  IGESAppli_ElementResults::SubCaseNumber () const
{
  return theSubcaseNumber;
}

    Standard_Real  IGESAppli_ElementResults::Time () const
{
  return theTime;
}

    Standard_Integer  IGESAppli_ElementResults::NbResultValues () const
{
  return theNbResultValues;
}

    Standard_Integer  IGESAppli_ElementResults::ResultReportFlag () const
{
  return theResultReportFlag;
}

    Standard_Integer  IGESAppli_ElementResults::NbElements () const
{
  return theElements->Length();
}

    Standard_Integer  IGESAppli_ElementResults::ElementIdentifier
  (const Standard_Integer Index) const
{
  return theElementIdentifiers->Value(Index);
}

    Handle(IGESAppli_FiniteElement)  IGESAppli_ElementResults::Element
  (const Standard_Integer Index) const
{
  return theElements->Value(Index);
}

    Standard_Integer  IGESAppli_ElementResults::ElementTopologyType
  (const Standard_Integer Index) const
{
  return theElementTopologyTypes->Value(Index);
}

    Standard_Integer  IGESAppli_ElementResults::NbLayers
  (const Standard_Integer Index) const
{
  return theNbLayers->Value(Index);
}

    Standard_Integer  IGESAppli_ElementResults::DataLayerFlag
  (const Standard_Integer Index) const
{
  return theDataLayerFlags->Value(Index);
}

    Standard_Integer  IGESAppli_ElementResults::NbResultDataLocs
  (const Standard_Integer Index) const
{
  return theNbResultDataLocs->Value(Index);
}

//  ?? VERIFIER

    Standard_Integer  IGESAppli_ElementResults::ResultDataLoc
  (const Standard_Integer NElem, const Standard_Integer NLoc) const
{
  return theResultDataLocs->Value(NElem)->Value(NLoc);
}

    Standard_Integer  IGESAppli_ElementResults::NbResults
  (const Standard_Integer Index) const
{
  return theResultData->Value(Index)->Length();
}

    Standard_Real  IGESAppli_ElementResults::ResultData
  (const Standard_Integer NElem, const Standard_Integer num) const
{
  return theResultData->Value(NElem)->Value(num);
}

    Standard_Integer  IGESAppli_ElementResults::ResultRank
  (const Standard_Integer NElem, const Standard_Integer NVal,
   const Standard_Integer NLay, const Standard_Integer NLoc) const
{
  Standard_Integer num = NVal + theNbResultValues *
    (NLay + theNbLayers->Value(NElem) * NLoc);
  return num;
}

    Standard_Real  IGESAppli_ElementResults::ResultData
  (const Standard_Integer NElem, const Standard_Integer NVal,
   const Standard_Integer NLay, const Standard_Integer NLoc) const
{
  return theResultData->Value(NElem)->Value(ResultRank(NElem,NVal,NLay,NLoc));
}

    Handle(TColStd_HArray1OfReal)  IGESAppli_ElementResults::ResultList
  (const Standard_Integer NElem) const
{
  return theResultData->Value(NElem);
}
