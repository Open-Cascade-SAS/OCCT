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

#include <IGESGeom_Boundary.ixx>


IGESGeom_Boundary::IGESGeom_Boundary ()    {  }


    void  IGESGeom_Boundary::Init
  (const Standard_Integer aType,
   const Standard_Integer aPreference,
   const Handle(IGESData_IGESEntity)& aSurface,
   const Handle(IGESData_HArray1OfIGESEntity)& allModelCurves,
   const Handle(TColStd_HArray1OfInteger)& allSenses,
   const Handle(IGESBasic_HArray1OfHArray1OfIGESEntity)& allParameterCurves)
{
  Standard_Integer num1 = allSenses->Length();
  if ( allSenses->Lower()          != 1 ||
      (allModelCurves->Lower()     != 1 || allModelCurves->Length() != num1) ||
      (allParameterCurves->Lower() != 1 || allParameterCurves->Length() != num1))
    Standard_DimensionMismatch::Raise("IGESGeom_Boundary: Init");

  theType            = aType;
  thePreference      = aPreference;
  theSurface         = aSurface;
  theModelCurves     = allModelCurves;
  theSenses          = allSenses;
  theParameterCurves = allParameterCurves;
  InitTypeAndForm(141,0);
}


    Standard_Integer  IGESGeom_Boundary::BoundaryType () const
{
  return theType;
}

    Standard_Integer  IGESGeom_Boundary::PreferenceType () const
{
  return thePreference;
}

    Handle(IGESData_IGESEntity)  IGESGeom_Boundary::Surface () const
{
  return theSurface;
}

    Standard_Integer  IGESGeom_Boundary::NbModelSpaceCurves () const
{
  return theModelCurves->Length();
}

    Standard_Integer  IGESGeom_Boundary::Sense (const Standard_Integer Index) const
{
  return theSenses->Value(Index);
}

    Handle(IGESData_IGESEntity)  IGESGeom_Boundary::ModelSpaceCurve
  (const Standard_Integer Index) const
{
  return theModelCurves->Value(Index);
}

    Standard_Integer  IGESGeom_Boundary::NbParameterCurves
  (const Standard_Integer Index) const
{
  if (theParameterCurves->Value(Index).IsNull()) return 0;
  return  theParameterCurves->Value(Index)->Length();
}

    Handle(IGESData_HArray1OfIGESEntity)  IGESGeom_Boundary::ParameterCurves
  (const Standard_Integer Index) const
{
  return theParameterCurves->Value(Index);
}

    Handle(IGESData_IGESEntity)  IGESGeom_Boundary::ParameterCurve
  (const Standard_Integer Index, const Standard_Integer Num) const
{
  return theParameterCurves->Value(Index)->Value(Num);
}
