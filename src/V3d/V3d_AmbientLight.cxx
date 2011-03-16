// File         V3d_AmbientLight.cxx
// Created      September 1992
// Modified     30-03-98 : ZOV ; PRO6774 (reconstruction of the class hierarchy and suppressing useless methods)
// Author       GG

//-Copyright    MatraDatavision 1991,1992

#include <V3d.hxx>
#include <V3d_AmbientLight.ixx>
#include <Visual3d_Light.hxx>

V3d_AmbientLight::V3d_AmbientLight(const Handle(V3d_Viewer)& VM, const Quantity_NameOfColor Name):V3d_Light(VM) { 
  
  Quantity_Color C(Name) ;
  MyType = V3d_AMBIENT ;
  MyLight = new Visual3d_Light(C) ;
}

