// File:        AlienImage_GIFAlienImage.cxx
// Created:     Quebex 20 October 1998
// Author:      DCB
// Copyright:   MatraDatavision 1998

#include <AlienImage_GIFAlienImage.ixx>

//================================================================
AlienImage_GIFAlienImage::AlienImage_GIFAlienImage ()
{
  myData = new AlienImage_GIFAlienData() ;
}

//================================================================
void AlienImage_GIFAlienImage::Clear ()
{
  myData->Clear();
}

//================================================================
void AlienImage_GIFAlienImage::SetName (const TCollection_AsciiString& aName)
{
  myData->SetName (aName);
}

//================================================================
const TCollection_AsciiString& AlienImage_GIFAlienImage::Name () const
{
  return (myData->Name());
}

//================================================================
Standard_Boolean AlienImage_GIFAlienImage::Write (OSD_File& file) const
{
  return myData->Write(file);
}

//================================================================
Standard_Boolean AlienImage_GIFAlienImage::Read (OSD_File& file)
{
  return myData->Read(file);
}

//================================================================
Handle_Image_Image AlienImage_GIFAlienImage::ToImage () const
{
  return myData->ToImage();
}

//================================================================
void AlienImage_GIFAlienImage::FromImage (const Handle_Image_Image& anImage)
{
  myData->FromImage(anImage);
}
 
