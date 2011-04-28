// File:	Graphic3d_TextureRoot.cxx
// Created:	Mon Jul 28 17:47:49 1997
// Author:	Pierre CHALAMET
//		<pct@sgi93>
//
// modified:
//    8/09/97 : mise en commentaire des tentatives pour charger
//              autre chose que du RGB. AlienImage buggee ?
//    5/01/99 : Ajout test sur les objets Path et FileName dans LoadTexture().
//		Si le path est null on trappe.   
//   11/06/99 : GG Enable to use GIF and BMP image format
//   10/01/00 : GG IMP Add Path() and Type() methods.
//   10/11/00 : GG Add Image() & LoadTexture() methods.

#define IMP140601	//GG Avoid to change the Trek of the current defined path
//			when the directory string is NULL or empty.

#define xTRACE 1

#include <Graphic3d_TextureRoot.ixx>
#include <Graphic3d_GraphicDevice.hxx>
#include <Graphic3d_GraphicDriver.hxx>

#include <AlienImage_EuclidAlienImage.hxx>
#include <AlienImage_SGIRGBAlienImage.hxx>
#include <AlienImage_XAlienImage.hxx>
#include <AlienImage_GIFAlienImage.hxx>
#include <AlienImage_BMPAlienImage.hxx>
#include <AlienImage_AidaAlienImage.hxx>
#include <AlienImage_SunRFAlienImage.hxx>
#include <AlienImage_AlienImage.hxx>

#include <OSD_Protection.hxx>
#include <OSD_File.hxx>
#include <stdio.h>

Graphic3d_TextureRoot::Graphic3d_TextureRoot(const Handle(Graphic3d_StructureManager)& SM,const Standard_CString Path,const Standard_CString FileName,const Graphic3d_TypeOfTexture Type) : MyPath(FileName),MyType(Type)
{
  // It will be necessary to improve the code below as soon as 3D or 4D textures
  // are implemented. Currently, always allocating an array for two texture bounds...
  MyTexUpperBounds = new TColStd_HArray1OfReal(1,2);

#ifdef IMP140601
  if( Path && (strlen(Path) > 0) )
#endif
    MyPath.SetTrek(TCollection_AsciiString( Path ));

  MyGraphicDriver = Handle(Graphic3d_GraphicDriver)::DownCast(SM->GraphicDevice()->GraphicDriver());

  if (MyGraphicDriver->InquireTextureAvailable())
  {
      // chargement de l'image
//-GG      Handle(AlienImage_AlienImage) MyImage = LoadTexture();
      if (MyImage.IsNull() && FileName && (strlen(FileName) > 0))
	MyImage = LoadTexture();
    
      if (MyImage.IsNull()) {
	MyTexId = -1;
	return;
      }
    
      MyTexId = MyGraphicDriver->CreateTexture(Type, MyImage, FileName, MyTexUpperBounds);
  }
#ifdef TRACE
  printf(" *** Graphic3d_TextureRoot::Create() textId %d\n",MyTexId);
#endif
}

void Graphic3d_TextureRoot::Destroy() const
{
  if (MyTexId >= 0 )  
	MyGraphicDriver->DestroyTexture(MyTexId);
#ifdef TRACE
  printf(" *** Graphic3d_TextureRoot::Destroy() textId %d\n",MyTexId);
#endif
}


Standard_Integer Graphic3d_TextureRoot::TextureId() const
{
  return MyTexId;
}


void Graphic3d_TextureRoot::Update() const
{
  if( MyTexId >= 0 ) 
	MyGraphicDriver->ModifyTexture(MyTexId, MyCInitTexture);
}


Standard_Boolean Graphic3d_TextureRoot::IsDone() const
{
  return (MyTexId >= 0) ? Standard_True : Standard_False;
}

void Graphic3d_TextureRoot::LoadTexture(const Handle(AlienImage_AlienImage)& anImage) {

  if (MyGraphicDriver->InquireTextureAvailable()) {
    if( MyTexId >= 0 )  
	MyGraphicDriver->DestroyTexture(MyTexId);
    MyImage = anImage;
    MyTexId = MyGraphicDriver->CreateTexture(MyType, MyImage, "", MyTexUpperBounds);
    Update(); 
  }
}
 
Handle(AlienImage_AlienImage) Graphic3d_TextureRoot::LoadTexture() const
{
  OSD_Protection Protection( OSD_R, OSD_R, OSD_R, OSD_R ) ; /* Read Only */
  Handle(AlienImage_AlienImage) TheAlienImage = NULL ;

  OSD_File File(MyPath);
  File.Open(OSD_ReadOnly, Protection);

  // open file ok ?
  if ( File.IsOpen() == Standard_False ) {
    TCollection_AsciiString sysname;
    MyPath.SystemName(sysname);
    cout << " *** Can't open texture file '" << sysname << "'" << endl;
    return TheAlienImage;
  }


  ////////////////////////
  // file reading       //
  ////////////////////////


  // image X ?
  ////////////
  Handle(AlienImage_XAlienImage) XAlienImage = new AlienImage_XAlienImage() ;
  
  if (XAlienImage->Read( File )) {
    File.Close();
    return XAlienImage;
  }

  // image GIF ?
  ////////////
  Handle(AlienImage_GIFAlienImage) GIFAlienImage = new AlienImage_GIFAlienImage() ;
  
  if (GIFAlienImage->Read( File )) {
    File.Close();
    return GIFAlienImage;
  }

  // image BMP ?
  ////////////
  Handle(AlienImage_BMPAlienImage) BMPAlienImage = new AlienImage_BMPAlienImage() ;
  
  if (BMPAlienImage->Read( File )) {
    File.Close();
    return BMPAlienImage;
  }
  
  // SunRaster ?
  //////////////
  Handle(AlienImage_SunRFAlienImage) SunRFAlienImage = new AlienImage_SunRFAlienImage() ;

  if (SunRFAlienImage->Read( File )) {
    File.Close();
    return SunRFAlienImage;
  }

  // Aida ?
  /////////
  Handle(AlienImage_AidaAlienImage) AidaAlienImage = new AlienImage_AidaAlienImage() ;
  
  if (AidaAlienImage->Read( File )) {
    File.Close();
    return AidaAlienImage;
  }

  // Euclid ?
  ///////////
  Handle(AlienImage_EuclidAlienImage) EuclidAlienImage = new AlienImage_EuclidAlienImage() ;

  if (EuclidAlienImage->Read( File )) {
    File.Close();
    return EuclidAlienImage;
  }


  // SGIRGB ?
  ///////////
  Handle(AlienImage_SGIRGBAlienImage) SGIRGBAlienImage = new AlienImage_SGIRGBAlienImage() ;

  if (SGIRGBAlienImage->Read( File )) {
    File.Close();
    return SGIRGBAlienImage;
  }


  // raise exception: file type unknown 
  return TheAlienImage;
}

const OSD_Path& Graphic3d_TextureRoot::Path() const {

  return MyPath;
}

Graphic3d_TypeOfTexture Graphic3d_TextureRoot::Type() const {

  return MyType;
}

Handle(AlienImage_AlienImage) Graphic3d_TextureRoot::Image() const {

  return MyImage;
}

Handle(TColStd_HArray1OfReal) Graphic3d_TextureRoot::GetTexUpperBounds() const
{
  return MyTexUpperBounds;
}
