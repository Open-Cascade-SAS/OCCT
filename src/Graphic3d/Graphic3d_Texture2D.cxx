// File:	Graphic3d_Texture2D.cxx
// Created:	Mon Jul 28 17:53:14 1997
// Author:	Pierre CHALAMET
//		<pct@sgi93>

// Modified :   GG 10/01/2000 IMP
//              Adds Name(),NumberOfTextures() and TextureName() methods



#include <Graphic3d_Texture2D.ixx>
#include <stdlib.h>


#include <TCollection_AsciiString.hxx>
#include <OSD_Directory.hxx>
#include <OSD_File.hxx>
#include <OSD_Path.hxx>

static TCollection_AsciiString GetEnvir ( ) {

  static Standard_Boolean IsDefined=Standard_False ;
  static TCollection_AsciiString VarName;
  if ( !IsDefined ) {
    char *envir, *casroot ;
    envir = getenv("CSF_MDTVTexturesDirectory") ;
    
    Standard_Boolean HasDefinition = Standard_False ;
    if ( !envir ) { 
      casroot  = getenv("CASROOT");
      if ( casroot ) {
	VarName = TCollection_AsciiString  (casroot);
	VarName += "/src/Textures" ;
	HasDefinition = Standard_True ;
      }
    } else {
      VarName = TCollection_AsciiString  (envir);
      HasDefinition = Standard_True ;
    }
    if ( HasDefinition ) {
      OSD_Path aPath ( VarName );
      OSD_Directory aDir(aPath);
      if ( aDir.Exists () ) {
	TCollection_AsciiString aTexture = VarName + "/2d_MatraDatavision.rgb" ;
	OSD_File TextureFile ( aTexture );
	if ( !TextureFile.Exists() ) {
	  cout << " CSF_MDTVTexturesDirectory or CASROOT not correctly setted " << endl;
	  cout << " not all files are found in : "<<VarName.ToCString() << endl;
	  Standard_Failure::Raise ( "CSF_MDTVTexturesDirectory or CASROOT not correctly setted " );
	}
      } else {
	cout << " CSF_MDTVTexturesDirectory or CASROOT not correctly setted " << endl;
	cout << " Directory : "<< VarName.ToCString() << " not exist " << endl;
	Standard_Failure::Raise ( "CSF_MDTVTexturesDirectory or CASROOT not correctly setted " );
      }      
      return VarName ;
    } else {
      cout << " CSF_MDTVTexturesDirectory and CASROOT not setted " << endl;
      cout << " one of these variable are mandatory to use this fonctionnality" << endl;
      Standard_Failure::Raise ( "CSF_MDTVTexturesDirectory and CASROOT not setted " );
    }   
    IsDefined = Standard_True ; 
  } 
  return VarName ;
}



static const char *NameOfTexture_to_FileName[] =
{
  "2d_MatraDatavision.rgb",
  "2d_alienskin.rgb",
  "2d_blue_rock.rgb",
  "2d_bluewhite_paper.rgb",
  "2d_brushed.rgb",
  "2d_bubbles.rgb",
  "2d_bumps.rgb",
  "2d_cast.rgb",
  "2d_chipbd.rgb",
  "2d_clouds.rgb",
  "2d_flesh.rgb",
  "2d_floor.rgb",
  "2d_galvnisd.rgb",
  "2d_grass.rgb",
  "2d_aluminum.rgb",
  "2d_rock.rgb",
  "2d_knurl.rgb",
  "2d_maple.rgb",
  "2d_marble.rgb",
  "2d_mottled.rgb",
  "2d_rain.rgb"
};


Graphic3d_Texture2D::Graphic3d_Texture2D(const Handle(Graphic3d_StructureManager)& SM,const Standard_CString FileName,const Graphic3d_TypeOfTexture Type)
: Graphic3d_TextureMap(SM, "", FileName, Type),
  myName(Graphic3d_NOT_2D_UNKNOWN)
{
}


Graphic3d_Texture2D::Graphic3d_Texture2D(const Handle(Graphic3d_StructureManager)& SM,const Graphic3d_NameOfTexture2D NOT, const Graphic3d_TypeOfTexture Type)
: Graphic3d_TextureMap(SM,  GetEnvir().ToCString() , NameOfTexture_to_FileName[NOT], Type),
  myName(NOT)
{
}

Standard_Integer Graphic3d_Texture2D::NumberOfTextures() {

  return sizeof(NameOfTexture_to_FileName)/sizeof(char*);
}

Graphic3d_NameOfTexture2D Graphic3d_Texture2D::Name() const {

  return myName;
}

Standard_CString Graphic3d_Texture2D::TextureName(const Standard_Integer aRank)
 {

  if( aRank < 1 || aRank > NumberOfTextures() )
        Standard_OutOfRange::Raise(" BAD index of texture");
  TCollection_AsciiString filename(NameOfTexture_to_FileName[aRank-1]);
  Standard_Integer i = filename.SearchFromEnd(".");

  static TCollection_AsciiString name;

  name = filename.SubString(4,i-1);
  return name.ToCString();
}
