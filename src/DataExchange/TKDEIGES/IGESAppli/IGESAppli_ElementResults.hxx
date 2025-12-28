// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Anand NATRAJAN )
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

#ifndef _IGESAppli_ElementResults_HeaderFile
#define _IGESAppli_ElementResults_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESAppli_FiniteElement.hxx>
#include <IGESData_IGESEntity.hxx>
class IGESDimen_GeneralNote;
class IGESBasic_HArray1OfHArray1OfInteger;
class IGESBasic_HArray1OfHArray1OfReal;
class IGESAppli_FiniteElement;

//! defines ElementResults, Type <148>
//! in package IGESAppli
//! Used to find the results of FEM analysis
class IGESAppli_ElementResults : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESAppli_ElementResults();

  //! This method is used to set the fields of the class
  //! ElementResults
  //! - aNote             : GeneralNote Entity describing analysis
  //! - aSubCase          : Analysis Subcase number
  //! - aTime             : Analysis time value
  //! - nbResults         : Number of result values per FEM
  //! - aResRepFlag       : Results Reporting Flag
  //! - allElementIdents  : FEM element number for elements
  //! - allFiniteElems    : FEM element
  //! - allTopTypes       : Element Topology Types
  //! - nbLayers          : Number of layers per result data location
  //! - allDataLayerFlags : Data Layer Flags
  //! - allnbResDataLocs  : Number of result data report locations
  //! - allResDataLocs    : Result Data Report Locations
  //! - allResults        : List of Result data values of FEM analysis
  Standard_EXPORT void Init(
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
    const occ::handle<IGESBasic_HArray1OfHArray1OfReal>&                          allResults);

  //! Changes the FormNumber (which indicates Type of Result)
  //! Error if not in range [0-34]
  Standard_EXPORT void SetFormNumber(const int form);

  //! returns General Note Entity describing analysis case
  Standard_EXPORT occ::handle<IGESDimen_GeneralNote> Note() const;

  //! returns analysis Subcase number
  Standard_EXPORT int SubCaseNumber() const;

  //! returns analysis time value
  Standard_EXPORT double Time() const;

  //! returns number of result values per FEM
  Standard_EXPORT int NbResultValues() const;

  //! returns Results Reporting Flag
  Standard_EXPORT int ResultReportFlag() const;

  //! returns number of FEM elements
  Standard_EXPORT int NbElements() const;

  //! returns FEM element number for elements
  Standard_EXPORT int ElementIdentifier(const int Index) const;

  //! returns FEM element
  Standard_EXPORT occ::handle<IGESAppli_FiniteElement> Element(const int Index) const;

  //! returns element Topology Types
  Standard_EXPORT int ElementTopologyType(const int Index) const;

  //! returns number of layers per result data location
  Standard_EXPORT int NbLayers(const int Index) const;

  //! returns Data Layer Flags
  Standard_EXPORT int DataLayerFlag(const int Index) const;

  //! returns number of result data report locations
  Standard_EXPORT int NbResultDataLocs(const int Index) const;

  //! returns Result Data Report Locations
  //! UNFINISHED
  Standard_EXPORT int ResultDataLoc(const int NElem, const int NLoc) const;

  //! returns total number of results
  Standard_EXPORT int NbResults(const int Index) const;

  //! returns Result data value for an Element, given its
  //! order between 1 and <NbResults(NElem)> (direct access)
  //! For a more comprehensive access, see below
  Standard_EXPORT double ResultData(const int NElem, const int num) const;

  //! Computes, for a given Element <NElem>, the rank of a
  //! individual Result Data, given <NVal>,<NLay>,<NLoc>
  Standard_EXPORT int ResultRank(const int NElem,
                                 const int NVal,
                                 const int NLay,
                                 const int NLoc) const;

  //! returns Result data values of FEM analysis, according this
  //! definition :
  //! - <NElem> : n0 of the Element to be considered
  //! - <NVal> : n0 of the Value between 1 and NbResultValues
  //! - <NLay> : n0 of the Layer for this Element
  //! - <NLoc> : n0 of the Data Location for this Element
  //! This gives for each Element, the corresponding rank
  //! computed by ResultRank, in which the leftmost subscript
  //! changes most rapidly
  Standard_EXPORT double ResultData(const int NElem,
                                    const int NVal,
                                    const int NLay,
                                    const int NLoc) const;

  //! Returns in once the entire list of data for an Element,
  //! addressed as by ResultRank (See above)
  Standard_EXPORT occ::handle<NCollection_HArray1<double>> ResultList(const int NElem) const;

  DEFINE_STANDARD_RTTIEXT(IGESAppli_ElementResults, IGESData_IGESEntity)

private:
  occ::handle<IGESDimen_GeneralNote>                                     theNote;
  int                                                                    theSubcaseNumber;
  double                                                                 theTime;
  int                                                                    theNbResultValues;
  int                                                                    theResultReportFlag;
  occ::handle<NCollection_HArray1<int>>                                  theElementIdentifiers;
  occ::handle<NCollection_HArray1<occ::handle<IGESAppli_FiniteElement>>> theElements;
  occ::handle<NCollection_HArray1<int>>                                  theElementTopologyTypes;
  occ::handle<NCollection_HArray1<int>>                                  theNbLayers;
  occ::handle<NCollection_HArray1<int>>                                  theDataLayerFlags;
  occ::handle<NCollection_HArray1<int>>                                  theNbResultDataLocs;
  occ::handle<IGESBasic_HArray1OfHArray1OfInteger>                       theResultDataLocs;
  occ::handle<IGESBasic_HArray1OfHArray1OfReal>                          theResultData;
};

#endif // _IGESAppli_ElementResults_HeaderFile
