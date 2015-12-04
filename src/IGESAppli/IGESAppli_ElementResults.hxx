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
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <IGESAppli_HArray1OfFiniteElement.hxx>
#include <IGESData_IGESEntity.hxx>
#include <TColStd_HArray1OfReal.hxx>
class IGESDimen_GeneralNote;
class IGESBasic_HArray1OfHArray1OfInteger;
class IGESBasic_HArray1OfHArray1OfReal;
class Standard_DimensionMismatch;
class Standard_OutOfRange;
class IGESAppli_FiniteElement;


class IGESAppli_ElementResults;
DEFINE_STANDARD_HANDLE(IGESAppli_ElementResults, IGESData_IGESEntity)

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
  Standard_EXPORT void Init (const Handle(IGESDimen_GeneralNote)& aNote, const Standard_Integer aSubCase, const Standard_Real aTime, const Standard_Integer nbResults, const Standard_Integer aResRepFlag, const Handle(TColStd_HArray1OfInteger)& allElementIdents, const Handle(IGESAppli_HArray1OfFiniteElement)& allFiniteElems, const Handle(TColStd_HArray1OfInteger)& allTopTypes, const Handle(TColStd_HArray1OfInteger)& nbLayers, const Handle(TColStd_HArray1OfInteger)& allDataLayerFlags, const Handle(TColStd_HArray1OfInteger)& allnbResDataLocs, const Handle(IGESBasic_HArray1OfHArray1OfInteger)& allResDataLocs, const Handle(IGESBasic_HArray1OfHArray1OfReal)& allResults);
  
  //! Changes the FormNumber (which indicates Type of Result)
  //! Error if not in range [0-34]
  Standard_EXPORT void SetFormNumber (const Standard_Integer form);
  
  //! returns General Note Entity describing analysis case
  Standard_EXPORT Handle(IGESDimen_GeneralNote) Note() const;
  
  //! returns analysis Subcase number
  Standard_EXPORT Standard_Integer SubCaseNumber() const;
  
  //! returns analysis time value
  Standard_EXPORT Standard_Real Time() const;
  
  //! returns number of result values per FEM
  Standard_EXPORT Standard_Integer NbResultValues() const;
  
  //! returns Results Reporting Flag
  Standard_EXPORT Standard_Integer ResultReportFlag() const;
  
  //! returns number of FEM elements
  Standard_EXPORT Standard_Integer NbElements() const;
  
  //! returns FEM element number for elements
  Standard_EXPORT Standard_Integer ElementIdentifier (const Standard_Integer Index) const;
  
  //! returns FEM element
  Standard_EXPORT Handle(IGESAppli_FiniteElement) Element (const Standard_Integer Index) const;
  
  //! returns element Topology Types
  Standard_EXPORT Standard_Integer ElementTopologyType (const Standard_Integer Index) const;
  
  //! returns number of layers per result data location
  Standard_EXPORT Standard_Integer NbLayers (const Standard_Integer Index) const;
  
  //! returns Data Layer Flags
  Standard_EXPORT Standard_Integer DataLayerFlag (const Standard_Integer Index) const;
  
  //! returns number of result data report locations
  Standard_EXPORT Standard_Integer NbResultDataLocs (const Standard_Integer Index) const;
  
  //! returns Result Data Report Locations
  //! UNFINISHED
  Standard_EXPORT Standard_Integer ResultDataLoc (const Standard_Integer NElem, const Standard_Integer NLoc) const;
  
  //! returns total number of results
  Standard_EXPORT Standard_Integer NbResults (const Standard_Integer Index) const;
  
  //! returns Result data value for an Element, given its
  //! order between 1 and <NbResults(NElem)> (direct access)
  //! For a more comprehensive access, see below
  Standard_EXPORT Standard_Real ResultData (const Standard_Integer NElem, const Standard_Integer num) const;
  
  //! Computes, for a given Element <NElem>, the rank of a
  //! individual Result Data, given <NVal>,<NLay>,<NLoc>
  Standard_EXPORT Standard_Integer ResultRank (const Standard_Integer NElem, const Standard_Integer NVal, const Standard_Integer NLay, const Standard_Integer NLoc) const;
  
  //! returns Result data values of FEM analysis, according this
  //! definition :
  //! - <NElem> : n0 of the Element to be considered
  //! - <NVal> : n0 of the Value between 1 and NbResultValues
  //! - <NLay> : n0 of the Layer for this Element
  //! - <NLoc> : n0 of the Data Location for this Element
  //! This gives for each Element, the corresponding rank
  //! computed by ResultRank, in which the leftmost subscript
  //! changes most rapidly
  Standard_EXPORT Standard_Real ResultData (const Standard_Integer NElem, const Standard_Integer NVal, const Standard_Integer NLay, const Standard_Integer NLoc) const;
  
  //! Returns in once the entire list of data for an Element,
  //! addressed as by ResultRank (See above)
  Standard_EXPORT Handle(TColStd_HArray1OfReal) ResultList (const Standard_Integer NElem) const;




  DEFINE_STANDARD_RTTIEXT(IGESAppli_ElementResults,IGESData_IGESEntity)

protected:




private:


  Handle(IGESDimen_GeneralNote) theNote;
  Standard_Integer theSubcaseNumber;
  Standard_Real theTime;
  Standard_Integer theNbResultValues;
  Standard_Integer theResultReportFlag;
  Handle(TColStd_HArray1OfInteger) theElementIdentifiers;
  Handle(IGESAppli_HArray1OfFiniteElement) theElements;
  Handle(TColStd_HArray1OfInteger) theElementTopologyTypes;
  Handle(TColStd_HArray1OfInteger) theNbLayers;
  Handle(TColStd_HArray1OfInteger) theDataLayerFlags;
  Handle(TColStd_HArray1OfInteger) theNbResultDataLocs;
  Handle(IGESBasic_HArray1OfHArray1OfInteger) theResultDataLocs;
  Handle(IGESBasic_HArray1OfHArray1OfReal) theResultData;


};







#endif // _IGESAppli_ElementResults_HeaderFile
