#include <VrmlConverter_IsoAspect.ixx>

VrmlConverter_IsoAspect::VrmlConverter_IsoAspect():VrmlConverter_LineAspect ()
{
 myNumber = 10; 
}

VrmlConverter_IsoAspect::VrmlConverter_IsoAspect (const Handle(Vrml_Material)& aMaterial,
                                                    const Standard_Boolean OnOff,
						    const Standard_Integer aNumber) 
 :VrmlConverter_LineAspect (aMaterial, OnOff)
{
   myNumber = aNumber;
}

void VrmlConverter_IsoAspect::SetNumber (const Standard_Integer aNumber) 
{
  myNumber = aNumber;
}


Standard_Integer VrmlConverter_IsoAspect::Number () const 
{
  return myNumber;
}
