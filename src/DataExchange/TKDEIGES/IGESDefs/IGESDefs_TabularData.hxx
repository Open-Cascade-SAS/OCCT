// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( SIVA )
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

#ifndef _IGESDefs_TabularData_HeaderFile
#define _IGESDefs_TabularData_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_IGESEntity.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
class IGESBasic_HArray1OfHArray1OfReal;

//! Defines IGES Tabular Data, Type <406> Form <11>,
//! in package IGESDefs
//! This Class is used to provide a Structure to accommodate
//! point form data.
class IGESDefs_TabularData : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESDefs_TabularData();

  //! This method is used to set the fields of the class
  //! TabularData
  //! - nbProps     : Number of property values
  //! - propType    : Property Type
  //! - typesInd    : Type of independent variables
  //! - nbValuesInd : Number of values of independent variables
  //! - valuesInd   : Values of independent variables
  //! - valuesDep   : Values of dependent variables
  //! raises exception if lengths of typeInd and nbValuesInd are not same
  Standard_EXPORT void Init(const int                          nbProps,
                            const int                          propType,
                            const occ::handle<NCollection_HArray1<int>>&         typesInd,
                            const occ::handle<NCollection_HArray1<int>>&         nbValuesInd,
                            const occ::handle<IGESBasic_HArray1OfHArray1OfReal>& valuesInd,
                            const occ::handle<IGESBasic_HArray1OfHArray1OfReal>& valuesDep);

  //! returns the number of property values (recorded)
  Standard_EXPORT int NbPropertyValues() const;

  //! determines the number of property values required
  Standard_EXPORT int ComputedNbPropertyValues() const;

  //! checks, and correct as necessary, the number of property
  //! values. Returns True if corrected, False if already OK
  Standard_EXPORT bool OwnCorrect();

  //! returns the property type
  Standard_EXPORT int PropertyType() const;

  //! returns the number of dependent variables
  Standard_EXPORT int NbDependents() const;

  //! returns the number of independent variables
  Standard_EXPORT int NbIndependents() const;

  //! returns the type of the num'th independent variable
  //! raises exception if num <= 0 or num > NbIndependents()
  Standard_EXPORT int TypeOfIndependents(const int num) const;

  //! returns the number of different values of the num'th indep. variable
  //! raises exception if num <= 0 or num > NbIndependents()
  Standard_EXPORT int NbValues(const int num) const;

  Standard_EXPORT double IndependentValue(const int variablenum,
                                                 const int valuenum) const;

  Standard_EXPORT occ::handle<NCollection_HArray1<double>> DependentValues(const int num) const;

  Standard_EXPORT double DependentValue(const int variablenum,
                                               const int valuenum) const;

  DEFINE_STANDARD_RTTIEXT(IGESDefs_TabularData, IGESData_IGESEntity)

private:
  int                         theNbPropertyValues;
  int                         thePropertyType;
  occ::handle<NCollection_HArray1<int>>         theTypeOfIndependentVariables;
  occ::handle<NCollection_HArray1<int>>         theNbValues;
  occ::handle<IGESBasic_HArray1OfHArray1OfReal> theIndependentValues;
  occ::handle<IGESBasic_HArray1OfHArray1OfReal> theDependentValues;
};

#endif // _IGESDefs_TabularData_HeaderFile
