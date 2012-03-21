// Created on: 1997-07-28
// Created by: Pierre CHALAMET
// Copyright (c) 1997-1999 Matra Datavision
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

// Modified	22-12-97 : FMN ; Ajout Modulate

// Modified :   GG 10/01/2000 IMP 
//              Adds Name(),NumberOfTextures() and TextureName() methods

#include <Graphic3d_Texture1D.ixx>
#include <stdlib.h>

#include <TCollection_AsciiString.hxx>
#include <OSD_Directory.hxx>
#include <OSD_File.hxx>
#include <OSD_Path.hxx>

static const char *NameOfTexture_to_FileName[] = 
{
  "1d_elevation.rgb"
};


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

Graphic3d_Texture1D::Graphic3d_Texture1D(const Handle(Graphic3d_StructureManager)& SM,const Standard_CString FileName,const Graphic3d_TypeOfTexture Type)
: Graphic3d_TextureMap(SM,"", FileName, Type),
  myName(Graphic3d_NOT_1D_UNKNOWN)
{
}


Graphic3d_Texture1D::Graphic3d_Texture1D(const Handle(Graphic3d_StructureManager)& SM,const Graphic3d_NameOfTexture1D NOT, const Graphic3d_TypeOfTexture Type)
: Graphic3d_TextureMap(SM, GetEnvir().ToCString(), NameOfTexture_to_FileName[NOT], Type),
  myName(NOT)
{
}

Graphic3d_NameOfTexture1D Graphic3d_Texture1D::Name() const {

  return myName;
}

Standard_Integer Graphic3d_Texture1D::NumberOfTextures() {

  return sizeof(NameOfTexture_to_FileName)/sizeof(char*);
}

Standard_CString Graphic3d_Texture1D::TextureName(const Standard_Integer aRank)
 {

  if( aRank < 1 || aRank > NumberOfTextures() )
        Standard_OutOfRange::Raise(" BAD index of texture");
  TCollection_AsciiString filename(NameOfTexture_to_FileName[aRank-1]);
  Standard_Integer i = filename.SearchFromEnd(".");

  static TCollection_AsciiString name;
  name = filename.SubString(4,i-1);
  return name.ToCString();
}

