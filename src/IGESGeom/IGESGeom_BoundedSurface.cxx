//--------------------------------------------------------------------
//
//  File Name : IGESGeom_BoundedSurface.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGeom_BoundedSurface.ixx>
#include <Standard_DimensionMismatch.hxx>


    IGESGeom_BoundedSurface::IGESGeom_BoundedSurface ()    {  }


    void  IGESGeom_BoundedSurface::Init
  (const Standard_Integer aType,
   const Handle(IGESData_IGESEntity)& aSurface,
   const Handle(IGESGeom_HArray1OfBoundary)& aBoundary)
{
  if (aBoundary->Lower() != 1)
    Standard_DimensionMismatch::Raise("IGESGeom_BoundedSurface : Init");
  theType       = aType;
  theSurface    = aSurface;
  theBoundaries = aBoundary;
  InitTypeAndForm(143,0);
}

    Standard_Integer  IGESGeom_BoundedSurface::RepresentationType () const
{
  return theType;
}

    Handle(IGESData_IGESEntity)  IGESGeom_BoundedSurface::Surface () const
{
  return theSurface;
}

    Standard_Integer  IGESGeom_BoundedSurface::NbBoundaries () const
{
  return theBoundaries->Length();
}

    Handle(IGESGeom_Boundary)  IGESGeom_BoundedSurface::Boundary
  (const Standard_Integer Index) const
{
  return theBoundaries->Value(Index);
}
