//--------------------------------------------------------------------
//
//  File Name : IGESGeom_CurveOnSurface.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGeom_CurveOnSurface.ixx>


    IGESGeom_CurveOnSurface::IGESGeom_CurveOnSurface ()    {  }


    void IGESGeom_CurveOnSurface::Init
  (const Standard_Integer aMode,
   const Handle(IGESData_IGESEntity)& aSurface,
   const Handle(IGESData_IGESEntity)& aCurveUV,
   const Handle(IGESData_IGESEntity)& aCurve3D,
   const Standard_Integer aPreference)
{
  theCreationMode   = aMode;
  theSurface        = aSurface;
  theCurveUV        = aCurveUV;
  theCurve3D        = aCurve3D;
  thePreferenceMode = aPreference;
  InitTypeAndForm(142,0);
}

    Standard_Integer IGESGeom_CurveOnSurface::CreationMode () const
{
  return theCreationMode;
}

    Handle(IGESData_IGESEntity) IGESGeom_CurveOnSurface::Surface () const
{
  return theSurface;
}

    Handle(IGESData_IGESEntity) IGESGeom_CurveOnSurface::CurveUV () const
{
  return theCurveUV;
}

    Handle(IGESData_IGESEntity) IGESGeom_CurveOnSurface::Curve3D () const
{
  return theCurve3D;
}

    Standard_Integer IGESGeom_CurveOnSurface::PreferenceMode () const
{
  return thePreferenceMode;
}
