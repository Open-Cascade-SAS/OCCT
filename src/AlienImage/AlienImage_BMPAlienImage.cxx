// File:        AlienImage_BMPAlienImage.cxx
// Created:     Quebex 20 October 1998
// Author:      DCB
// Copyright:   MatraDatavision 1998

#include <AlienImage_BMPAlienImage.ixx>

//================================================================
AlienImage_BMPAlienImage::AlienImage_BMPAlienImage ()
{
  myData = new AlienImage_BMPAlienData() ;
}

//================================================================
void AlienImage_BMPAlienImage::Clear ()
{
  myData->Clear();
}

//================================================================
void AlienImage_BMPAlienImage::SetName (const TCollection_AsciiString& aName)
{
  myData->SetName (aName);
}

//================================================================
const TCollection_AsciiString& AlienImage_BMPAlienImage::Name () const
{
  return (myData->Name());
}

//================================================================
Standard_Boolean AlienImage_BMPAlienImage::Write (OSD_File& file) const
{
  return myData->Write(file);
}

//================================================================
Standard_Boolean AlienImage_BMPAlienImage::Read (OSD_File& file)
{
  return myData->Read(file);
}

//================================================================
Handle_Image_Image AlienImage_BMPAlienImage::ToImage () const
{
  return myData->ToImage();
}

//================================================================
void AlienImage_BMPAlienImage::FromImage (const Handle_Image_Image& anImage)
{
  myData->FromImage(anImage);
}
 
