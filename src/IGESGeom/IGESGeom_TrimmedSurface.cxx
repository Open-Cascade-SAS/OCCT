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

#include <IGESGeom_TrimmedSurface.ixx>
#include <Standard_DimensionMismatch.hxx>


IGESGeom_TrimmedSurface::IGESGeom_TrimmedSurface ()    {  }


    void IGESGeom_TrimmedSurface::Init
  (const Handle(IGESData_IGESEntity)& aSurface,
   const Standard_Integer aFlag,
   const Handle(IGESGeom_CurveOnSurface)& anOuter,
   const Handle(IGESGeom_HArray1OfCurveOnSurface)& anInner)
{
  if (!anInner.IsNull())
    if (anInner->Lower() != 1)
      Standard_DimensionMismatch::Raise("IGESGeom_TrimmedSurface : Init");

  theSurface     = aSurface;
  theFlag        = aFlag;
  theOuterCurve  = anOuter;
  theInnerCurves = anInner;
  InitTypeAndForm(144,0);
}

    Handle(IGESData_IGESEntity) IGESGeom_TrimmedSurface::Surface () const
{
  return theSurface;
}

    Standard_Boolean IGESGeom_TrimmedSurface::HasOuterContour () const
{
  return (! theOuterCurve.IsNull());
}

    Handle(IGESGeom_CurveOnSurface) IGESGeom_TrimmedSurface::OuterContour () const
{
  return theOuterCurve;
}

    Standard_Integer IGESGeom_TrimmedSurface::NbInnerContours () const
{
  return (theInnerCurves.IsNull() ? 0 : theInnerCurves->Length());
}

    Standard_Integer IGESGeom_TrimmedSurface::OuterBoundaryType () const
{
  return theFlag;
}

    Handle(IGESGeom_CurveOnSurface) IGESGeom_TrimmedSurface::InnerContour
  (const Standard_Integer anIndex) const
{
  return (theInnerCurves->Value(anIndex));
  // Exception OutOfRange will be raises if anIndex <= 0 or
  //                                        anIndex > NbInnerCounters()
}
