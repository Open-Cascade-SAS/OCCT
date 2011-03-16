#include <VrmlConverter_PointAspect.ixx>

VrmlConverter_PointAspect::VrmlConverter_PointAspect()
{
 myHasMaterial = Standard_False;
}

VrmlConverter_PointAspect::VrmlConverter_PointAspect (const Handle(Vrml_Material)& aMaterial,
                                                      const Standard_Boolean OnOff)
{
 myMaterial = aMaterial;
 myHasMaterial = OnOff;
}


void VrmlConverter_PointAspect::SetMaterial(const Handle(Vrml_Material)& aMaterial)
{
 myMaterial = aMaterial;
}

Handle(Vrml_Material) VrmlConverter_PointAspect::Material() const 
{
 return myMaterial;
}
void VrmlConverter_PointAspect::SetHasMaterial(const Standard_Boolean OnOff)
{
 myHasMaterial = OnOff;
}

Standard_Boolean VrmlConverter_PointAspect::HasMaterial() const 
{
 return myHasMaterial;
}
