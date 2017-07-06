// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Standard_Type.hxx>
#include <Vrml_Material.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Vrml_Material,Standard_Transient)

Vrml_Material::Vrml_Material(const Handle(Quantity_HArray1OfColor)& aAmbientColor, 
			      const Handle(Quantity_HArray1OfColor)& aDiffuseColor, 
			      const Handle(Quantity_HArray1OfColor)& aSpecularColor, 
			      const Handle(Quantity_HArray1OfColor)& aEmissiveColor, 
			      const Handle(TColStd_HArray1OfReal)& aShininess, 
			      const Handle(TColStd_HArray1OfReal)& aTransparency)
{
 myAmbientColor = aAmbientColor;
 myDiffuseColor = aDiffuseColor;
 mySpecularColor = aSpecularColor;
 myEmissiveColor = aEmissiveColor;

 Standard_Integer i;
 for ( i = aShininess->Lower(); i <= aShininess->Upper(); i++ )
   {
     if (aShininess->Value(i) < 0. || aShininess->Value(i) > 1.)
       {
	 throw Standard_Failure("The value of aShininess is out of range (0 - 1)");
       }
   }
 myShininess     = aShininess;

 for ( i = aTransparency->Lower(); i <= aTransparency->Upper(); i++ )
   {
     if (aTransparency->Value(i) < 0. || aTransparency->Value(i) > 1.)
       {
	 throw Standard_Failure("The value of aTransparency is out of range (0 - 1)");
       }
   }
 myTransparency  = aTransparency;
}

 Vrml_Material::Vrml_Material()
{
 Quantity_Color tmpcolor;
// myAmbientColor  = new Quantity_HArray1OfColor(1,1);
    tmpcolor.SetValues(0.2, 0.2, 0.2, Quantity_TOC_RGB);
 myAmbientColor  = new Quantity_HArray1OfColor(1,1,tmpcolor);
//    myAmbientColor->SetValue(1, tmpcolor);
// myDiffuseColor  = new Quantity_HArray1OfColor(1,1);
    tmpcolor.SetValues(0.8, 0.8, 0.8, Quantity_TOC_RGB);
 myDiffuseColor  = new Quantity_HArray1OfColor(1,1,tmpcolor);
//    myDiffuseColor->SetValue(1, tmpcolor);
 mySpecularColor = new Quantity_HArray1OfColor(1,1);
    tmpcolor.SetValues(0, 0, 0, Quantity_TOC_RGB);
    mySpecularColor->SetValue(1, tmpcolor);
 myEmissiveColor = new Quantity_HArray1OfColor(1,1);
    tmpcolor.SetValues(0, 0, 0, Quantity_TOC_RGB);
    myEmissiveColor->SetValue(1, tmpcolor);

 myShininess     = new TColStd_HArray1OfReal (1,1,0.2);
 myTransparency  = new TColStd_HArray1OfReal (1,1,0);
}

void Vrml_Material::SetAmbientColor(const Handle(Quantity_HArray1OfColor)& aAmbientColor)
{
 myAmbientColor = aAmbientColor;
}

Handle(Quantity_HArray1OfColor) Vrml_Material::AmbientColor() const 
{
 return myAmbientColor;
}

void Vrml_Material::SetDiffuseColor(const Handle(Quantity_HArray1OfColor)& aDiffuseColor)
{
 myDiffuseColor = aDiffuseColor;
}

Handle(Quantity_HArray1OfColor) Vrml_Material::DiffuseColor() const 
{
 return myDiffuseColor;
}

void Vrml_Material::SetSpecularColor(const Handle(Quantity_HArray1OfColor)& aSpecularColor)
{
 mySpecularColor = aSpecularColor;
}

Handle(Quantity_HArray1OfColor) Vrml_Material::SpecularColor() const 
{
 return mySpecularColor;
}

void Vrml_Material::SetEmissiveColor(const Handle(Quantity_HArray1OfColor)& aEmissiveColor)
{
 myEmissiveColor = aEmissiveColor;
}

Handle(Quantity_HArray1OfColor) Vrml_Material::EmissiveColor() const 
{
 return myEmissiveColor;
}

void Vrml_Material::SetShininess(const Handle(TColStd_HArray1OfReal)& aShininess)
{
 Standard_Integer i;
 for ( i = aShininess->Lower(); i <= aShininess->Upper(); i++ )
   {
     if (aShininess->Value(i) < 0. || aShininess->Value(i) > 1.)
       {
	 throw Standard_Failure("The value of aShininess is out of range (0 - 1)");
       }
   }
 myShininess = aShininess;
}

Handle(TColStd_HArray1OfReal) Vrml_Material::Shininess() const 
{
 return myShininess;
}

void Vrml_Material::SetTransparency(const Handle(TColStd_HArray1OfReal)& aTransparency)
{
 Standard_Integer i;
 for ( i = aTransparency->Lower(); i <= aTransparency->Upper(); i++ )
   {
     if (aTransparency->Value(i) < 0. || aTransparency->Value(i) > 1.)
       {
	 throw Standard_Failure("The value of aTransparency is out of range (0 - 1)");
       }
   }
 myTransparency = aTransparency;
}

Handle(TColStd_HArray1OfReal) Vrml_Material::Transparency() const 
{
 return myTransparency;
}

Standard_OStream& Vrml_Material::Print(Standard_OStream& anOStream) const 
{
 Standard_Integer i;
 anOStream  << "Material {\n";

 if ( myAmbientColor->Length() != 1 ||
     Abs(myAmbientColor->Value(myAmbientColor->Lower()).Red() - 0.2)    > 0.0001 ||
     Abs(myAmbientColor->Value(myAmbientColor->Lower()).Green() - 0.2)  > 0.0001 || 
     Abs(myAmbientColor->Value(myAmbientColor->Lower()).Blue() - 0.2)   > 0.0001 )
  {
  anOStream  << "    ambientColor [\n\t";
  for ( i = myAmbientColor->Lower(); i <= myAmbientColor->Upper(); i++ )
    {
     anOStream << myAmbientColor->Value(i).Red() << ' ' << myAmbientColor->Value(i).Green() << ' ' << myAmbientColor->Value(i).Blue();
     if ( i < myAmbientColor->Length() )
	anOStream  << ",\n\t"; // ,,,,,,,,,,
    }
   anOStream  << " ]\n";
  }

 if ( myDiffuseColor->Length() != 1 || 
     Abs(myDiffuseColor->Value(myDiffuseColor->Lower()).Red() - 0.8)   > 0.0001  || 
     Abs(myDiffuseColor->Value(myDiffuseColor->Lower()).Green() - 0.8) > 0.0001  || 
     Abs(myDiffuseColor->Value(myDiffuseColor->Lower()).Blue() - 0.8)  > 0.0001 )
  {
  anOStream  << "    diffuseColor [\n\t";
  for ( i = myDiffuseColor->Lower(); i <= myDiffuseColor->Upper(); i++ )
    {
     anOStream << myDiffuseColor->Value(i).Red() << ' ' << myDiffuseColor->Value(i).Green() << ' ' << myDiffuseColor->Value(i).Blue();
     if ( i < myDiffuseColor->Length() )
	anOStream  << ",\n\t";
    }
   anOStream  << " ]\n";
  }

 if ( mySpecularColor->Length() != 1 || 
     Abs(mySpecularColor->Value(mySpecularColor->Lower()).Red() - 0)   > 0.0001 || 
     Abs(mySpecularColor->Value(mySpecularColor->Lower()).Green() - 0) > 0.0001 || 
     Abs(mySpecularColor->Value(mySpecularColor->Lower()).Blue() - 0)  > 0.0001 )
  {
   anOStream  << "    specularColor [\n\t";
   for ( i = mySpecularColor->Lower(); i <= mySpecularColor->Upper(); i++ )
     {
     anOStream << mySpecularColor->Value(i).Red() << ' ' << mySpecularColor->Value(i).Green() << ' ' << mySpecularColor->Value(i).Blue();
     if ( i < mySpecularColor->Length() )
	anOStream  << ",\n\t";
     }
   anOStream  << " ]\n";
  }

 if ( myEmissiveColor->Length() != 1 || 
     Abs(myEmissiveColor->Value(myEmissiveColor->Lower()).Red() - 0)   > 0.0001 || 
     Abs(myEmissiveColor->Value(myEmissiveColor->Lower()).Green() - 0) > 0.0001 || 
     Abs(myEmissiveColor->Value(myEmissiveColor->Lower()).Blue() - 0)  > 0.0001 )
  {
  anOStream  << "    emissiveColor [\n\t";
  for ( i = myEmissiveColor->Lower(); i <= myEmissiveColor->Upper(); i++ )
    {
     anOStream << myEmissiveColor->Value(i).Red() << ' ' << myEmissiveColor->Value(i).Green() << ' ' << myEmissiveColor->Value(i).Blue();
     if ( i < myEmissiveColor->Length() )
	anOStream  << ",\n\t";
    }
   anOStream  << " ]\n";
  }

 if ( myShininess->Length() != 1 ||  Abs(myShininess->Value(myShininess->Lower()) - 0.2) > 0.0001 )
   {
    anOStream  << "    shininess\t\t[ ";
    for ( i = myShininess->Lower(); i <= myShininess->Upper(); i++ )
      {
       anOStream << myShininess->Value(i);
       if ( i < myShininess->Length() )
          anOStream  << ", ";
      }
    anOStream  << " ]\n";
  }

 if ( myTransparency->Length() != 1 ||  Abs(myTransparency->Value(myTransparency->Lower()) - 0) > 0.0001 )
  {
   anOStream  << "    transparency\t[ ";
   for ( i = myTransparency->Lower(); i <= myTransparency->Upper(); i++ )
     {
      anOStream << myTransparency->Value(i);
      if ( i < myTransparency->Length() )
         anOStream  << ", ";
     }
    anOStream  << " ]\n";
  } 
  anOStream  << "}\n";
  
 return anOStream;
}

