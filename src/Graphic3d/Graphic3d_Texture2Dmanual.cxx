// File:	Graphic3d_Texture2Dmanual.cxx
// Created:	Mon Jul 28 18:00:10 1997
// Author:	Pierre CHALAMET
//		<pct@sgi93>


#include <Graphic3d_Texture2Dmanual.ixx>
#include <Graphic3d_TypeOfTextureMode.hxx>


Graphic3d_Texture2Dmanual::Graphic3d_Texture2Dmanual(const Handle(Graphic3d_StructureManager)& SM,const Standard_CString FileName)
: Graphic3d_Texture2D(SM, FileName, Graphic3d_TOT_2D_MIPMAP)
{
  MyCInitTexture.doModulate = 1;
  MyCInitTexture.doRepeat = 1;
  MyCInitTexture.Mode = (int)Graphic3d_TOTM_MANUAL;
  MyCInitTexture.doLinear = 1;
  MyCInitTexture.sx = 1.0F;
  MyCInitTexture.sy = 1.0F;
  MyCInitTexture.tx = 0.0F;
  MyCInitTexture.ty = 0.0F;
  MyCInitTexture.angle = 0.0F;
  MyCInitTexture.sparams[0] = 0.0F;
  MyCInitTexture.sparams[1] = 0.0F;
  MyCInitTexture.sparams[2] = 0.0F;
  MyCInitTexture.sparams[3] = 0.0F;
  MyCInitTexture.tparams[0] = 0.0F;
  MyCInitTexture.tparams[1] = 0.0F;
  MyCInitTexture.tparams[2] = 0.0F;
  MyCInitTexture.tparams[3] = 0.0F;
  Update();
}


Graphic3d_Texture2Dmanual::Graphic3d_Texture2Dmanual(const Handle(Graphic3d_StructureManager)& SM,const Graphic3d_NameOfTexture2D NOT)
: Graphic3d_Texture2D(SM, NOT, Graphic3d_TOT_2D_MIPMAP)
{
  MyCInitTexture.doModulate = 1;
  MyCInitTexture.doRepeat = 1;
  MyCInitTexture.Mode = (int)Graphic3d_TOTM_MANUAL;
  MyCInitTexture.doLinear = 1;
  MyCInitTexture.sx = 1.0F;
  MyCInitTexture.sy = 1.0F;
  MyCInitTexture.tx = 0.0F;
  MyCInitTexture.ty = 0.0F;
  MyCInitTexture.angle = 0.0F;
  MyCInitTexture.sparams[0] = 0.0F;
  MyCInitTexture.sparams[1] = 0.0F;
  MyCInitTexture.sparams[2] = 0.0F;
  MyCInitTexture.sparams[3] = 0.0F;
  MyCInitTexture.tparams[0] = 0.0F;
  MyCInitTexture.tparams[1] = 0.0F;
  MyCInitTexture.tparams[2] = 0.0F;
  MyCInitTexture.tparams[3] = 0.0F;
  Update();
}

