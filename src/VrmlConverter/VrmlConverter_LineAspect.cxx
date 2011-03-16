#include <VrmlConverter_LineAspect.ixx>

 VrmlConverter_LineAspect::VrmlConverter_LineAspect()
{
 myHasMaterial = Standard_False;
}

VrmlConverter_LineAspect::VrmlConverter_LineAspect (const Handle(Vrml_Material)& aMaterial,
                                                    const Standard_Boolean OnOff)
{
 myMaterial = aMaterial;
 myHasMaterial = OnOff;
}


void VrmlConverter_LineAspect::SetMaterial(const Handle(Vrml_Material)& aMaterial)
{
 myMaterial = aMaterial;
}

Handle(Vrml_Material) VrmlConverter_LineAspect::Material() const 
{
 return myMaterial;
}
void VrmlConverter_LineAspect::SetHasMaterial(const Standard_Boolean OnOff)
{
 myHasMaterial = OnOff;
}

Standard_Boolean VrmlConverter_LineAspect::HasMaterial() const 
{
 return myHasMaterial;
}

