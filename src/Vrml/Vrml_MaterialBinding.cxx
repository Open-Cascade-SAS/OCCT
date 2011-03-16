#include <Vrml_MaterialBinding.ixx>

 Vrml_MaterialBinding::Vrml_MaterialBinding(const Vrml_MaterialBindingAndNormalBinding aValue)
{
 myValue = aValue;
}

 Vrml_MaterialBinding::Vrml_MaterialBinding()
{
 myValue = Vrml_DEFAULT;
}

void Vrml_MaterialBinding::SetValue(const Vrml_MaterialBindingAndNormalBinding aValue)
{
 myValue = aValue;
}

Vrml_MaterialBindingAndNormalBinding Vrml_MaterialBinding::Value() const 
{
 return myValue;
}

Standard_OStream& Vrml_MaterialBinding::Print(Standard_OStream& anOStream) const 
{
 anOStream  << "MaterialBinding {" << endl;
 switch ( myValue )
   {
   case Vrml_DEFAULT:  break; // anOStream  << "    value" << "\tDEFAULT";
   case Vrml_OVERALL:            anOStream  << "    value" << "\tOVERALL" << endl; break;
   case Vrml_PER_PART:           anOStream  << "    value" << "\tPER_PART" << endl; break;
   case Vrml_PER_PART_INDEXED:   anOStream  << "    value" << "\tPER_PART_INDEXED" << endl; break;
   case Vrml_PER_FACE:           anOStream  << "    value" << "\tPER_FACE" << endl; break;
   case Vrml_PER_FACE_INDEXED:   anOStream  << "    value" << "\tPER_FACE_INDEXED" << endl; break;
   case Vrml_PER_VERTEX:         anOStream  << "    value" << "\tPER_VERTEX" << endl; break;
   case Vrml_PER_VERTEX_INDEXED: anOStream  << "    value" << "\tPER_VERTEX_INDEXED" << endl; break;
   }
 anOStream  << '}' << endl;
 return anOStream;
}

