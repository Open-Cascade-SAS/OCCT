//--------------------------------------------------------------------
//
//  File Name : IGESSolid_ToroidalSurface.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESSolid_ToroidalSurface.ixx>
#include <gp_GTrsf.hxx>


    IGESSolid_ToroidalSurface::IGESSolid_ToroidalSurface ()    {  }


    void  IGESSolid_ToroidalSurface::Init
  (const Handle(IGESGeom_Point)& aCenter,
   const Handle(IGESGeom_Direction)& anAxis,
   const Standard_Real majRadius, const Standard_Real minRadius,
   const Handle(IGESGeom_Direction)& Refdir)
{
  theCenter      = aCenter;
  theAxis        = anAxis;
  theMajorRadius = majRadius;
  theMinorRadius = minRadius;
  theRefDir      = Refdir;
  InitTypeAndForm(198, (theRefDir.IsNull() ? 0 : 1) );
}

    Handle(IGESGeom_Point)  IGESSolid_ToroidalSurface::Center () const
{
  return theCenter;
}

    gp_Pnt  IGESSolid_ToroidalSurface::TransformedCenter () const
{
  if (!HasTransf()) return theCenter->Value();
  else
    {
      gp_XYZ tmp = theCenter->Value().XYZ();
      Location().Transforms(tmp);
      return gp_Pnt(tmp);
    }
}

    Handle(IGESGeom_Direction)  IGESSolid_ToroidalSurface::Axis () const
{
  return theAxis;
}

    Standard_Real  IGESSolid_ToroidalSurface::MajorRadius () const
{
  return theMajorRadius;
}

    Standard_Real  IGESSolid_ToroidalSurface::MinorRadius () const
{
  return theMinorRadius;
}

    Handle(IGESGeom_Direction)  IGESSolid_ToroidalSurface::ReferenceDir () const
{
  return theRefDir;
}

    Standard_Boolean  IGESSolid_ToroidalSurface::IsParametrised () const
{
  return !(theRefDir.IsNull());
}
