#include <VrmlConverter_ShadingAspect.ixx>
#include <Vrml_ShapeHints.hxx>
#include <Vrml_Material.hxx>

 VrmlConverter_ShadingAspect::VrmlConverter_ShadingAspect()
{
 Handle(Vrml_Material) m = new Vrml_Material;
 Vrml_ShapeHints sh;
 myFrontMaterial = m;
 myShapeHints = sh;
 myHasNormals = Standard_False;
 myHasMaterial = Standard_False;
}
void VrmlConverter_ShadingAspect::SetFrontMaterial(const Handle(Vrml_Material)& aMaterial)
{
 myFrontMaterial = aMaterial;
}

Handle(Vrml_Material) VrmlConverter_ShadingAspect::FrontMaterial() const 
{
 return myFrontMaterial;
}

void VrmlConverter_ShadingAspect::SetShapeHints(const Vrml_ShapeHints& aShapeHints)
{
 myShapeHints = aShapeHints;
}

Vrml_ShapeHints VrmlConverter_ShadingAspect::ShapeHints() const 
{
 return  myShapeHints;
}

void VrmlConverter_ShadingAspect::SetHasNormals(const Standard_Boolean OnOff)
{
 myHasNormals = OnOff; 
}

Standard_Boolean VrmlConverter_ShadingAspect::HasNormals() const 
{
 return myHasNormals;
}

void VrmlConverter_ShadingAspect::SetHasMaterial(const Standard_Boolean OnOff)
{
 myHasMaterial = OnOff;

}

Standard_Boolean VrmlConverter_ShadingAspect::HasMaterial() const 
{
 return myHasMaterial;
}
