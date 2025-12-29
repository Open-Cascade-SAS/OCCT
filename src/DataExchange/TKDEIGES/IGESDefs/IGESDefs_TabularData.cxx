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
// UNFINISHED
// The last field (theDependentValues) not resolved. Queried to mdtv

#include <IGESBasic_HArray1OfHArray1OfReal.hxx>
#include <IGESDefs_TabularData.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDefs_TabularData, IGESData_IGESEntity)

IGESDefs_TabularData::IGESDefs_TabularData() = default;

void IGESDefs_TabularData::Init(const int nbProps,
                                const int propType,
                                /*     const int nbDeps, */
                                /*     const int nbIndeps, */
                                const occ::handle<NCollection_HArray1<int>>&         typesInd,
                                const occ::handle<NCollection_HArray1<int>>&         nbValuesInd,
                                const occ::handle<IGESBasic_HArray1OfHArray1OfReal>& valuesInd,
                                const occ::handle<IGESBasic_HArray1OfHArray1OfReal>& valuesDep)
{
  int num = typesInd->Length();
  if (typesInd->Lower() != 1 || nbValuesInd->Lower() != 1 || nbValuesInd->Length() != num
      || valuesInd->Lower() != 1 || valuesInd->Length() != num || valuesDep->Lower() != 1)
    throw Standard_DimensionMismatch("IGESDefs_TabularData : Init");
  theNbPropertyValues = nbProps;
  thePropertyType     = propType;
  /*     theNbDependents = nbDeps; */
  /*     theNbIndependents = nbIndeps; */
  theTypeOfIndependentVariables = typesInd;
  theNbValues                   = nbValuesInd;
  theIndependentValues          = valuesInd;
  theDependentValues            = valuesDep;
  InitTypeAndForm(406, 11);
}

int IGESDefs_TabularData::NbPropertyValues() const
{
  return theNbPropertyValues;
}

int IGESDefs_TabularData::ComputedNbPropertyValues() const
{
  return theNbPropertyValues; // not smart ... to improve
}

bool IGESDefs_TabularData::OwnCorrect()
{
  int newnb = ComputedNbPropertyValues();
  if (newnb == theNbPropertyValues)
    return false;
  theNbPropertyValues = newnb;
  return true;
}

int IGESDefs_TabularData::PropertyType() const
{
  return thePropertyType;
}

int IGESDefs_TabularData::NbDependents() const
{
  return theDependentValues->Length();
}

int IGESDefs_TabularData::NbIndependents() const
{
  return theTypeOfIndependentVariables->Length();
}

int IGESDefs_TabularData::TypeOfIndependents(const int num) const
{
  return theTypeOfIndependentVariables->Value(num);
}

int IGESDefs_TabularData::NbValues(const int num) const
{
  return theNbValues->Value(num);
}

double IGESDefs_TabularData::IndependentValue(const int variablenum, const int valuenum) const
{
  return (theIndependentValues->Value(variablenum))->Value(valuenum);
}

occ::handle<NCollection_HArray1<double>> IGESDefs_TabularData::DependentValues(const int num) const
{
  return theDependentValues->Value(num);
}

// UNFINISHED
// Array limits not sure.
double IGESDefs_TabularData::DependentValue(const int /*variablenum*/, const int /*valuenum*/) const
{
  double val = 0.;
#if 0
  int sum = 0;
  for (int i = 1; i < variablenum; i++)
    {
      sum += theNbValues->Value(i);
    }
  sum += valuenum;
  val = theDependentValues->Value(sum);
#endif
  return val;
}
