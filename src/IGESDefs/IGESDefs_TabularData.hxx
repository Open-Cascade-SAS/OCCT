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
#include <TColStd_HArray1OfInteger.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <TColStd_HArray1OfReal.hxx>
class IGESBasic_HArray1OfHArray1OfReal;


class IGESDefs_TabularData;
DEFINE_STANDARD_HANDLE(IGESDefs_TabularData, IGESData_IGESEntity)

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
  Standard_EXPORT void Init (const Standard_Integer nbProps, const Standard_Integer propType, const Handle(TColStd_HArray1OfInteger)& typesInd, const Handle(TColStd_HArray1OfInteger)& nbValuesInd, const Handle(IGESBasic_HArray1OfHArray1OfReal)& valuesInd, const Handle(IGESBasic_HArray1OfHArray1OfReal)& valuesDep);
  
  //! returns the number of property values (recorded)
  Standard_EXPORT Standard_Integer NbPropertyValues() const;
  
  //! determines the number of property values required
  Standard_EXPORT Standard_Integer ComputedNbPropertyValues() const;
  
  //! checks, and correct as necessary, the number of property
  //! values. Returns True if corrected, False if already OK
  Standard_EXPORT Standard_Boolean OwnCorrect();
  
  //! returns the property type
  Standard_EXPORT Standard_Integer PropertyType() const;
  
  //! returns the number of dependent variables
  Standard_EXPORT Standard_Integer NbDependents() const;
  
  //! returns the number of independent variables
  Standard_EXPORT Standard_Integer NbIndependents() const;
  
  //! returns the type of the num'th independent variable
  //! raises exception if num <= 0 or num > NbIndependents()
  Standard_EXPORT Standard_Integer TypeOfIndependents (const Standard_Integer num) const;
  
  //! returns the number of different values of the num'th indep. variable
  //! raises exception if num <= 0 or num > NbIndependents()
  Standard_EXPORT Standard_Integer NbValues (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Real IndependentValue (const Standard_Integer variablenum, const Standard_Integer valuenum) const;
  
  Standard_EXPORT Handle(TColStd_HArray1OfReal) DependentValues (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Real DependentValue (const Standard_Integer variablenum, const Standard_Integer valuenum) const;




  DEFINE_STANDARD_RTTIEXT(IGESDefs_TabularData,IGESData_IGESEntity)

protected:




private:


  Standard_Integer theNbPropertyValues;
  Standard_Integer thePropertyType;
  Handle(TColStd_HArray1OfInteger) theTypeOfIndependentVariables;
  Handle(TColStd_HArray1OfInteger) theNbValues;
  Handle(IGESBasic_HArray1OfHArray1OfReal) theIndependentValues;
  Handle(IGESBasic_HArray1OfHArray1OfReal) theDependentValues;


};







#endif // _IGESDefs_TabularData_HeaderFile
