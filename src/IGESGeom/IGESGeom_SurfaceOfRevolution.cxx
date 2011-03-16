//--------------------------------------------------------------------
//
//  File Name : IGESGeom_SurfaceOfRevolution.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGeom_SurfaceOfRevolution.ixx>


    IGESGeom_SurfaceOfRevolution::IGESGeom_SurfaceOfRevolution ()    {  }


    void IGESGeom_SurfaceOfRevolution::Init
  (const Handle(IGESGeom_Line)& anAxis,
   const Handle(IGESData_IGESEntity)& aGeneratrix,
   const Standard_Real aStartAngle, const Standard_Real anEndAngle)
{
  theLine       = anAxis;
  theGeneratrix = aGeneratrix;
  theStartAngle = aStartAngle;
  theEndAngle   = anEndAngle;
  InitTypeAndForm(120,0);
}

    Handle(IGESGeom_Line) IGESGeom_SurfaceOfRevolution::AxisOfRevolution () const
{
  return theLine;
}

    Handle(IGESData_IGESEntity) IGESGeom_SurfaceOfRevolution::Generatrix () const
{
  return theGeneratrix;
}

    Standard_Real IGESGeom_SurfaceOfRevolution::StartAngle () const
{
  return theStartAngle;
}

    Standard_Real IGESGeom_SurfaceOfRevolution::EndAngle () const
{
  return theEndAngle;
}
