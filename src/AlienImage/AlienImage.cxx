// Created on: 1998-10-21
// Created by: DCB
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <AlienImage.hxx>
#include <OSD_Path.hxx>
#include <OSD_File.hxx>
#include <OSD_Protection.hxx>
#include <OSD_Environment.hxx>

#include <Image_Image.hxx>
#include <AlienImage_AlienUserImage.hxx>
#include <AlienImage_XAlienImage.hxx>
#include <AlienImage_SGIRGBAlienImage.hxx>
#include <AlienImage_EuclidAlienImage.hxx>
#include <AlienImage_SunRFAlienImage.hxx>
#include <AlienImage_GIFAlienImage.hxx>
#include <AlienImage_BMPAlienImage.hxx>

//====================================================================
static OSD_Environment& _CSF_DefaultImageFormat() {
  static OSD_Environment CSF_DefaultImageFormat("CSF_DefaultImageFormat");
  return CSF_DefaultImageFormat;
}
#define CSF_DefaultImageFormat _CSF_DefaultImageFormat()

//====================================================================
Standard_Boolean AlienImage::CreateImage (const TCollection_AsciiString& theFileName,
                                          Handle(Image_Image)& theImage)
{
  OSD_File file = OSD_File (OSD_Path (theFileName));
  return AlienImage::CreateImage (file, theImage);
}

//====================================================================
Standard_Boolean AlienImage::CreateImage (const Standard_CString theFileName,
                                          Handle(Image_Image)& theImage)
{
  OSD_File file = OSD_File (OSD_Path (TCollection_AsciiString (theFileName)));
  return AlienImage::CreateImage (file, theImage);
}

//====================================================================
Standard_Boolean AlienImage::CreateImage (OSD_File& theFile,
                                          Handle(Image_Image)& theImage)
{
  OSD_Protection          theProtection (OSD_R, OSD_R, OSD_R, OSD_R);
  OSD_Path                thePath;
  theFile.Path            (thePath);
  TCollection_AsciiString theExtension  = thePath.Extension();
  theExtension.UpperCase  ();

  Handle(AlienImage_AlienUserImage) theAlienImage;

  theFile.Open (OSD_ReadOnly, theProtection);
  if (theFile.IsOpen()) {
    if (theExtension.IsEqual (".XWD"))
_CreateXWD:
      theAlienImage = new AlienImage_XAlienImage ();
    else if (theExtension.IsEqual (".RGB"))
_CreateRGB:
      theAlienImage = new AlienImage_SGIRGBAlienImage ();
    else if (theExtension.IsEqual (".RS"))
_CreateRS:
      theAlienImage = new AlienImage_SunRFAlienImage ();
    else if (theExtension.IsEqual (".PIX"))
_CreatePIX:
      theAlienImage = new AlienImage_EuclidAlienImage ();
    else if (theExtension.IsEqual (".GIF"))
_CreateGIF:
      theAlienImage = new AlienImage_GIFAlienImage ();
    else if (theExtension.IsEqual (".BMP"))
_CreateBMP:
      theAlienImage = new AlienImage_BMPAlienImage ();

    else if (theExtension.IsEmpty ()) {
      // Trye to read $CSF_DefaultImageFormat environment
      TCollection_AsciiString theDefExt = CSF_DefaultImageFormat.Value();
      theDefExt.Prepend (".");
      thePath.SetExtension (theDefExt);
      theExtension = theDefExt;
      theExtension.UpperCase ();
      if      (theExtension.IsEqual (".XWD")) goto _CreateXWD;
      else if (theExtension.IsEqual (".RGB")) goto _CreateRGB;
      else if (theExtension.IsEqual (".RS"))  goto _CreateRS;
      else if (theExtension.IsEqual (".PIX")) goto _CreatePIX;
      else if (theExtension.IsEqual (".GIF")) goto _CreateGIF;
      else if (theExtension.IsEqual (".BMP")) goto _CreateBMP;
      else                                    return Standard_False;
    } else {
      return Standard_False;
    }

    if (!theAlienImage -> Read (theFile)) {
      theFile.Close ();
      return Standard_False;
    }

    theImage = theAlienImage -> ToImage ();
    theFile.Close ();
    return (!theImage.IsNull());
  }
  return Standard_False;
}

//====================================================================
Standard_Boolean AlienImage::LoadImageFile(const Standard_CString anImageFile,
                                           Handle(Image_Image)& myImage,
                                           Standard_Integer& aWidth,
                                           Standard_Integer& aHeight)
{
  // Try to load new image
  myImage.Nullify ();
  if (AlienImage::CreateImage (anImageFile, myImage)) {
    // New image loaded
//_ExitOK:
    aWidth  = myImage -> Width ();
    aHeight = myImage -> Height ();
    return Standard_True;
  } else {
    // Could not load new image
//_ExitError:
    myImage.Nullify ();
    aWidth = aHeight = 0;
    return Standard_False;
  }
}
 
