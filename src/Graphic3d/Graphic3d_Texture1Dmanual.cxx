// File:	Graphic3d_Texture1Dmanual.cxx
// Created:	Mon Jul 28 17:57:30 1997
// Author:	Pierre CHALAMET
//		<pct@sgi93>


#include <Graphic3d_Texture1Dmanual.ixx>
#include <Graphic3d_TypeOfTextureMode.hxx>

Graphic3d_Texture1Dmanual::Graphic3d_Texture1Dmanual(const Handle(Graphic3d_StructureManager)& SM,const Standard_CString FileName)
: Graphic3d_Texture1D(SM, FileName, Graphic3d_TOT_1D)
{
  MyCInitTexture.doModulate = 0;
  MyCInitTexture.doRepeat = 0;
  MyCInitTexture.Mode = (int)Graphic3d_TOTM_MANUAL;
  MyCInitTexture.doLinear = 0;
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


Graphic3d_Texture1Dmanual::Graphic3d_Texture1Dmanual(const Handle(Graphic3d_StructureManager)& SM,const Graphic3d_NameOfTexture1D NOT)
: Graphic3d_Texture1D(SM, NOT, Graphic3d_TOT_1D)
{
  MyCInitTexture.doModulate = 0;
  MyCInitTexture.doRepeat = 0;
  MyCInitTexture.Mode = (int)Graphic3d_TOTM_MANUAL;
  MyCInitTexture.doLinear = 0;
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
