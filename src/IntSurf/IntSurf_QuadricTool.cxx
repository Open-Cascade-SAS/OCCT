#include <IntSurf_QuadricTool.ixx>

#include <gp_Sphere.hxx>
#include <gp_Cylinder.hxx>

Standard_Real IntSurf_QuadricTool::Tolerance (const IntSurf_Quadric& Q) {
  switch (Q.TypeQuadric()) {
  case GeomAbs_Sphere:
    return 2.e-6*Q.Sphere().Radius();
  case GeomAbs_Cylinder:
    return 2.e-6*Q.Cylinder().Radius();
  default: 
    break;
  }
  return(1e-6);
}

