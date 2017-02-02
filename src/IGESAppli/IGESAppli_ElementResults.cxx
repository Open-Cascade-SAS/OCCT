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
// UNFINISHED & UNSTARTED

#include <IGESAppli_ElementResults.hxx>
#include <IGESAppli_FiniteElement.hxx>
#include <IGESBasic_HArray1OfHArray1OfInteger.hxx>
#include <IGESBasic_HArray1OfHArray1OfReal.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_ElementResults,IGESData_IGESEntity)

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
    throw Standard_DimensionMismatch("IGESAppli_ElementsResults : Init");
  for (Standard_Integer i = 1; i <= num; i ++) {
    Standard_Integer nl  = nbLayers->Value(i);
    Standard_Integer nrl = allnbResDataLocs->Value(i);
    if (allResDataLocs->Value(i)->Lower() != 1 || 
	allResDataLocs->Value(i)->Upper() != nrl)
      throw Standard_DimensionMismatch("IGESAppli_ElementsResults : DataLoc");
    if (allResults->Value(i)->Lower() != 1 ||
	allResults->Value(i)->Upper() != nl*nrl*nbResults)
      throw Standard_DimensionMismatch("IGESAppli_ElementsResults : Results");
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
  if (form < 0 || form > 34) throw Standard_OutOfRange("IGESAppli_ElementResults : SetFormNumber");
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
