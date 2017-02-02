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
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TColStd_HArray1OfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDefs_TabularData,IGESData_IGESEntity)

IGESDefs_TabularData::IGESDefs_TabularData ()    {  }


    void IGESDefs_TabularData::Init
  (const Standard_Integer nbProps,
   const Standard_Integer propType,
/*     const Standard_Integer nbDeps, */
/*     const Standard_Integer nbIndeps, */
   const Handle(TColStd_HArray1OfInteger)& typesInd,
   const Handle(TColStd_HArray1OfInteger)& nbValuesInd,
   const Handle(IGESBasic_HArray1OfHArray1OfReal)& valuesInd,
   const Handle(IGESBasic_HArray1OfHArray1OfReal)& valuesDep)
{
  Standard_Integer num = typesInd->Length();
  if (typesInd->Lower() != 1 ||
      nbValuesInd->Lower() != 1 || nbValuesInd->Length() != num ||
      valuesInd->Lower()   != 1 || valuesInd->Length()   != num ||
      valuesDep->Lower()   != 1 )
    throw Standard_DimensionMismatch("IGESDefs_TabularData : Init");
  theNbPropertyValues = nbProps;
  thePropertyType               = propType;
/*     theNbDependents = nbDeps; */
/*     theNbIndependents = nbIndeps; */
  theTypeOfIndependentVariables = typesInd;
  theNbValues                   = nbValuesInd;
  theIndependentValues          = valuesInd;
  theDependentValues            = valuesDep;
  InitTypeAndForm(406,11);
}

    Standard_Integer IGESDefs_TabularData::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Standard_Integer IGESDefs_TabularData::ComputedNbPropertyValues () const
{
  return theNbPropertyValues;  // pas malin ... a ameliorer
}

    Standard_Boolean IGESDefs_TabularData::OwnCorrect ()
{
  Standard_Integer newnb = ComputedNbPropertyValues();
  if (newnb == theNbPropertyValues) return Standard_False;
  theNbPropertyValues = newnb;
  return Standard_True;
}


    Standard_Integer IGESDefs_TabularData::PropertyType () const
{
  return thePropertyType;
}

    Standard_Integer IGESDefs_TabularData::NbDependents () const
{
  return theDependentValues->Length();
}

    Standard_Integer IGESDefs_TabularData::NbIndependents () const
{
  return theTypeOfIndependentVariables->Length();
}

    Standard_Integer IGESDefs_TabularData::TypeOfIndependents
  (const Standard_Integer num) const
{
  return theTypeOfIndependentVariables->Value(num);
}

    Standard_Integer IGESDefs_TabularData::NbValues (const Standard_Integer num) const
{
  return theNbValues->Value(num);
}

    Standard_Real IGESDefs_TabularData::IndependentValue
  (const Standard_Integer variablenum, const Standard_Integer valuenum) const
{
  return (theIndependentValues->Value(variablenum))->Value(valuenum);
}

    Handle(TColStd_HArray1OfReal)  IGESDefs_TabularData::DependentValues
  (const Standard_Integer num) const
{
  return theDependentValues->Value(num);
}

// UNFINISHED
// Array limits not sure.
Standard_Real IGESDefs_TabularData::DependentValue (const Standard_Integer /*variablenum*/, 
                                                    const Standard_Integer /*valuenum*/) const
{
  Standard_Real val = 0.;
#if 0
  Standard_Integer sum = 0;
  for (Standard_Integer i = 1; i < variablenum; i++)
    {
      sum += theNbValues->Value(i);
    }
  sum += valuenum;
  val = theDependentValues->Value(sum);
#endif
  return val;
}
