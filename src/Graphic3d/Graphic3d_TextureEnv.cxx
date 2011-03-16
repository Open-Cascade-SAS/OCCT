// File:	Graphic3d_TextureEnv.cxx
// Created:	Mon Jul 28 17:54:59 1997
// Author:	Pierre CHALAMET
//		<pct@sgi93>

// Modified :   GG 10/01/2000 IMP
//		Adds Name(),NumberOfTextures() and TextureName() methods

#include <Graphic3d_TextureEnv.ixx>
#include <Graphic3d_TypeOfTexture.hxx>
#include <Graphic3d_TypeOfTextureMode.hxx>


#include <TCollection_AsciiString.hxx>
#include <OSD_Directory.hxx>
#include <OSD_File.hxx>
#include <OSD_Path.hxx>

#include <stdlib.h>

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
  "env_clouds.rgb",
  "env_cv.rgb",
  "env_medit.rgb",
  "env_pearl.rgb",
  "env_sky1.rgb",
  "env_sky2.rgb",
  "env_lines.rgb",
  "env_road.rgb"
};


Graphic3d_TextureEnv::Graphic3d_TextureEnv(const Handle(Graphic3d_StructureManager)& SM,const Standard_CString FileName)
: Graphic3d_TextureRoot(SM, "", FileName, Graphic3d_TOT_2D_MIPMAP),
  myName(Graphic3d_NOT_ENV_UNKNOWN)
{
  MyCInitTexture.doModulate = 0;
  MyCInitTexture.doRepeat = 0;
  MyCInitTexture.Mode = (int)Graphic3d_TOTM_SPHERE;
  MyCInitTexture.doLinear = 1;
  MyCInitTexture.sx = 1.0F;
  MyCInitTexture.sy = 1.0F;
  MyCInitTexture.tx = 0.0F;
  MyCInitTexture.ty = 0.0F;
  MyCInitTexture.angle = 0.0F;
  MyCInitTexture.sparams[0] = 1.0F;
  MyCInitTexture.sparams[1] = 0.0F;
  MyCInitTexture.sparams[2] = 0.0F;
  MyCInitTexture.sparams[3] = 0.0F;
  MyCInitTexture.tparams[0] = 0.0F;
  MyCInitTexture.tparams[1] = 1.0F;
  MyCInitTexture.tparams[2] = 0.0F;
  MyCInitTexture.tparams[3] = 0.0F;
  Update();
}


Graphic3d_TextureEnv::Graphic3d_TextureEnv(const Handle(Graphic3d_StructureManager)& SM,const Graphic3d_NameOfTextureEnv NOT)
: Graphic3d_TextureRoot(SM, GetEnvir().ToCString() , NameOfTexture_to_FileName[NOT], Graphic3d_TOT_2D_MIPMAP),
  myName(NOT)
{
  MyCInitTexture.doModulate = 0;
  MyCInitTexture.doRepeat = 0;
  MyCInitTexture.Mode = (int)Graphic3d_TOTM_SPHERE;
  MyCInitTexture.doLinear = 1;
  MyCInitTexture.sx = 1.0F;
  MyCInitTexture.sy = 1.0F;
  MyCInitTexture.tx = 0.0F;
  MyCInitTexture.ty = 0.0F;
  MyCInitTexture.angle = 0.0F;
  MyCInitTexture.sparams[0] = 1.0F;
  MyCInitTexture.sparams[1] = 0.0F;
  MyCInitTexture.sparams[2] = 0.0F;
  MyCInitTexture.sparams[3] = 0.0F;
  MyCInitTexture.tparams[0] = 0.0F;
  MyCInitTexture.tparams[1] = 1.0F;
  MyCInitTexture.tparams[2] = 0.0F;
  MyCInitTexture.tparams[3] = 0.0F;
  Update();
}

Graphic3d_NameOfTextureEnv Graphic3d_TextureEnv::Name() const {

  return myName;
}

Standard_Integer Graphic3d_TextureEnv::NumberOfTextures() {

  return sizeof(NameOfTexture_to_FileName)/sizeof(char*);
}

Standard_CString Graphic3d_TextureEnv::TextureName(const Standard_Integer aRank) {

  if( aRank < 1 || aRank > NumberOfTextures() )
	Standard_OutOfRange::Raise(" BAD index of texture");
  TCollection_AsciiString filename(NameOfTexture_to_FileName[aRank-1]);
  Standard_Integer i = filename.SearchFromEnd(".");

  static TCollection_AsciiString name;
  name = filename.SubString(5,i-1);
  return name.ToCString();
}
