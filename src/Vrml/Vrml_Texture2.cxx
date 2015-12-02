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


#include <TCollection_AsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <Vrml_SFImage.hxx>
#include <Vrml_Texture2.hxx>

Vrml_Texture2::Vrml_Texture2()
{
  myFilename = "";

  myImage = new Vrml_SFImage;

  myImage->SetWidth(0);
  myImage->SetHeight(0);
  myImage->SetNumber(Vrml_NULL);

  myWrapS = Vrml_REPEAT;
  myWrapT = Vrml_REPEAT; 
}

Vrml_Texture2::Vrml_Texture2(const TCollection_AsciiString& aFilename,
			     const Handle(Vrml_SFImage)& aImage,
			     const Vrml_Texture2Wrap aWrapS,
			     const Vrml_Texture2Wrap aWrapT)
{
  myFilename = aFilename;
  myImage = aImage;
  myWrapS = aWrapS;
  myWrapT = aWrapT;
}

 void Vrml_Texture2::SetFilename(const TCollection_AsciiString& aFilename) 
{
  myFilename = aFilename;
}

 TCollection_AsciiString Vrml_Texture2::Filename() const
{
  return myFilename;
}

 void Vrml_Texture2::SetImage(const Handle(Vrml_SFImage)& aImage) 
{
  myImage = aImage;
}

 Handle(Vrml_SFImage) Vrml_Texture2::Image() const
{
  return  myImage;
}

 void Vrml_Texture2::SetWrapS(const Vrml_Texture2Wrap aWrapS) 
{
  myWrapS = aWrapS;
}

 Vrml_Texture2Wrap Vrml_Texture2::WrapS() const
{
  return myWrapS;
}

 void Vrml_Texture2::SetWrapT(const Vrml_Texture2Wrap aWrapT) 
{
  myWrapT = aWrapT;
}

 Vrml_Texture2Wrap Vrml_Texture2::WrapT() const
{
  return myWrapT;
}

 Standard_OStream& Vrml_Texture2::Print(Standard_OStream& anOStream) const
{
 Standard_Integer i;
 anOStream  << "Texture2 {\n";

 if ( !(myFilename.IsEqual("") ) )
   {
    anOStream  << "    filename\t";
    anOStream << '"' << myFilename << '"' << "\n";
   }

 if ( myImage->Width() != 0 || myImage->Height() != 0 || myImage->Number() != Vrml_NULL ) 
   {
    anOStream  << "    image\t";
    anOStream <<  myImage->Width() << " " << myImage->Height() << " ";

    switch ( myImage->Number() )
      {
      case Vrml_NULL:  anOStream  << "0"; break;
      case Vrml_ONE:   anOStream  << "1"; break;
      case Vrml_TWO:   anOStream  << "2"; break; 
      case Vrml_THREE: anOStream  << "3"; break;  
      case Vrml_FOUR:  anOStream  << "4"; break; 
      }

    if ( myImage->ArrayFlag() == Standard_True )
      { 
	for ( i = myImage->Array()->Lower(); i <= myImage->Array()->Upper(); i++ )
	  {
//	    anOStream << " " << hex(myImage->Array()->Value(i),0);
	    anOStream << " " << myImage->Array()->Value(i);
	  }
      }
    anOStream  <<  "\n";
  }

  switch ( myWrapS )
    {
     case Vrml_REPEAT: break; // anOStream  << "    wrapS\tREPEAT ";
     case Vrml_CLAMP: anOStream  << "    wrapS\tCLAMP\n"; break;
    }

  switch ( myWrapT )
    {
     case Vrml_REPEAT: break; // anOStream  << "    wrapT\tREPEAT ";
     case Vrml_CLAMP: anOStream  << "    wrapT\tCLAMP\n"; break;
    }

 anOStream  << "}\n";
 return anOStream;
}
