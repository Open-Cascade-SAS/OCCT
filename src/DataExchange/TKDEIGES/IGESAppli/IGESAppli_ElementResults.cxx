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

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_ElementResults, IGESData_IGESEntity)

IGESAppli_ElementResults::IGESAppli_ElementResults() = default;

void IGESAppli_ElementResults::Init(
  const occ::handle<IGESDimen_GeneralNote>&                                     aNote,
  const int                                                                     aSubCase,
  const double                                                                  aTime,
  const int                                                                     nbResults,
  const int                                                                     aResRepFlag,
  const occ::handle<NCollection_HArray1<int>>&                                  allElementIdents,
  const occ::handle<NCollection_HArray1<occ::handle<IGESAppli_FiniteElement>>>& allFiniteElems,
  const occ::handle<NCollection_HArray1<int>>&                                  allTopTypes,
  const occ::handle<NCollection_HArray1<int>>&                                  nbLayers,
  const occ::handle<NCollection_HArray1<int>>&                                  allDataLayerFlags,
  const occ::handle<NCollection_HArray1<int>>&                                  allnbResDataLocs,
  const occ::handle<IGESBasic_HArray1OfHArray1OfInteger>&                       allResDataLocs,
  const occ::handle<IGESBasic_HArray1OfHArray1OfReal>& allResults) // UNFINISHED
{
  // raises exception if sizes are not consistent, (lower = 1 too)
  // but how to check is not clear ??
  int num = allElementIdents->Length();
  if (allElementIdents->Lower() != 1 || allFiniteElems->Lower() != 1
      || allFiniteElems->Upper() != num || allTopTypes->Lower() != 1 || allTopTypes->Upper() != num
      || nbLayers->Lower() != 1 || nbLayers->Upper() != num || allDataLayerFlags->Lower() != 1
      || allDataLayerFlags->Upper() != num || allnbResDataLocs->Lower() != 1
      || allnbResDataLocs->Upper() != num || allResDataLocs->Lower() != 1
      || allResDataLocs->Upper() != num || allResults->Lower() != 1 || allResults->Upper() != num)
    throw Standard_DimensionMismatch("IGESAppli_ElementsResults : Init");
  for (int i = 1; i <= num; i++)
  {
    int nl  = nbLayers->Value(i);
    int nrl = allnbResDataLocs->Value(i);
    if (allResDataLocs->Value(i)->Lower() != 1 || allResDataLocs->Value(i)->Upper() != nrl)
      throw Standard_DimensionMismatch("IGESAppli_ElementsResults : DataLoc");
    if (allResults->Value(i)->Lower() != 1 || allResults->Value(i)->Upper() != nl * nrl * nbResults)
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
  InitTypeAndForm(148, FormNumber());
  // FormNumber -> Type of the Results
}

void IGESAppli_ElementResults::SetFormNumber(const int form)
{
  if (form < 0 || form > 34)
    throw Standard_OutOfRange("IGESAppli_ElementResults : SetFormNumber");
  InitTypeAndForm(148, form);
}

occ::handle<IGESDimen_GeneralNote> IGESAppli_ElementResults::Note() const
{
  return theNote;
}

int IGESAppli_ElementResults::SubCaseNumber() const
{
  return theSubcaseNumber;
}

double IGESAppli_ElementResults::Time() const
{
  return theTime;
}

int IGESAppli_ElementResults::NbResultValues() const
{
  return theNbResultValues;
}

int IGESAppli_ElementResults::ResultReportFlag() const
{
  return theResultReportFlag;
}

int IGESAppli_ElementResults::NbElements() const
{
  return theElements->Length();
}

int IGESAppli_ElementResults::ElementIdentifier(const int Index) const
{
  return theElementIdentifiers->Value(Index);
}

occ::handle<IGESAppli_FiniteElement> IGESAppli_ElementResults::Element(const int Index) const
{
  return theElements->Value(Index);
}

int IGESAppli_ElementResults::ElementTopologyType(const int Index) const
{
  return theElementTopologyTypes->Value(Index);
}

int IGESAppli_ElementResults::NbLayers(const int Index) const
{
  return theNbLayers->Value(Index);
}

int IGESAppli_ElementResults::DataLayerFlag(const int Index) const
{
  return theDataLayerFlags->Value(Index);
}

int IGESAppli_ElementResults::NbResultDataLocs(const int Index) const
{
  return theNbResultDataLocs->Value(Index);
}

//  ?? VERIFIER

int IGESAppli_ElementResults::ResultDataLoc(const int NElem, const int NLoc) const
{
  return theResultDataLocs->Value(NElem)->Value(NLoc);
}

int IGESAppli_ElementResults::NbResults(const int Index) const
{
  return theResultData->Value(Index)->Length();
}

double IGESAppli_ElementResults::ResultData(const int NElem, const int num) const
{
  return theResultData->Value(NElem)->Value(num);
}

int IGESAppli_ElementResults::ResultRank(const int NElem,
                                         const int NVal,
                                         const int NLay,
                                         const int NLoc) const
{
  int num = NVal + theNbResultValues * (NLay + theNbLayers->Value(NElem) * NLoc);
  return num;
}

double IGESAppli_ElementResults::ResultData(const int NElem,
                                            const int NVal,
                                            const int NLay,
                                            const int NLoc) const
{
  return theResultData->Value(NElem)->Value(ResultRank(NElem, NVal, NLay, NLoc));
}

occ::handle<NCollection_HArray1<double>> IGESAppli_ElementResults::ResultList(const int NElem) const
{
  return theResultData->Value(NElem);
}
