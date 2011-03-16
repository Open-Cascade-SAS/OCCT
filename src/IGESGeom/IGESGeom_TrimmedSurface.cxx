//--------------------------------------------------------------------
//
//  File Name : IGESGeom_TrimmedSurface.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
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
