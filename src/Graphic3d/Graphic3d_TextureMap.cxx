// File:	Graphic3d_TextureMap.cxx
// Created:	Mon Jul 28 17:49:46 1997
// Author:	Pierre CHALAMET
//		<fmn@sgi93>
// Modified : 	GG 09/11/00 smooth,modulate,repeat texture attributes
//		must be available both for textures 1D & 2D.


#include <Graphic3d_TextureMap.ixx>

Graphic3d_TextureMap::Graphic3d_TextureMap(const Handle(Graphic3d_StructureManager)& SM,const Standard_CString Path,const Standard_CString FileName,const Graphic3d_TypeOfTexture Type)
: Graphic3d_TextureRoot(SM, Path, FileName, Type)
{
}

void Graphic3d_TextureMap::EnableSmooth() {
  MyCInitTexture.doLinear = 1;
  Update();
}

void Graphic3d_TextureMap::DisableSmooth() {
  MyCInitTexture.doLinear = 0;
  Update();
}

Standard_Boolean Graphic3d_TextureMap::IsSmoothed() const {
  return (MyCInitTexture.doLinear != 0);
}

void Graphic3d_TextureMap::EnableModulate() {
  MyCInitTexture.doModulate = 1;
  Update();
}

void Graphic3d_TextureMap::DisableModulate() {
  MyCInitTexture.doModulate = 0;
  Update();
}

Standard_Boolean Graphic3d_TextureMap::IsModulate() const {
  return (MyCInitTexture.doModulate != 0);
}

void Graphic3d_TextureMap::EnableRepeat() {
  MyCInitTexture.doRepeat = 1;
  Update();
}

void Graphic3d_TextureMap::DisableRepeat() {
  MyCInitTexture.doRepeat = 0;
  Update();
}

Standard_Boolean Graphic3d_TextureMap::IsRepeat() const {
  return (MyCInitTexture.doRepeat != 0);
}

