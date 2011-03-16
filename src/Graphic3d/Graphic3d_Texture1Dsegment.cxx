// File:	Graphic3d_Texture1Dsegment.cxx
// Created:	Mon Jul 28 17:58:46 1997
// Author:	Pierre CHALAMET
//		<pct@sgi93>

#include <Graphic3d_Texture1Dsegment.ixx>
#include <Graphic3d_TypeOfTextureMode.hxx>


Graphic3d_Texture1Dsegment::Graphic3d_Texture1Dsegment(const Handle(Graphic3d_StructureManager)& SM,const Standard_CString FileName)
: Graphic3d_Texture1D(SM, FileName, Graphic3d_TOT_1D)
{
  MyCInitTexture.doModulate = 0;
  MyCInitTexture.doRepeat = 1;
  MyCInitTexture.Mode = (int)Graphic3d_TOTM_OBJECT;
  MyCInitTexture.doLinear = 0;
  MyCInitTexture.sx = 1.0F;
  MyCInitTexture.sy = 1.0F;
  MyCInitTexture.tx = 0.0F;
  MyCInitTexture.ty = 0.0F;
  MyCInitTexture.angle = 0.0F;
  MyCInitTexture.sparams[0] = 0.0F;
  MyCInitTexture.sparams[1] = 0.0F;
  MyCInitTexture.sparams[2] = 1.0F;
  MyCInitTexture.sparams[3] = 0.0F;
  MyCInitTexture.tparams[0] = 0.0F;
  MyCInitTexture.tparams[1] = 0.0F;
  MyCInitTexture.tparams[2] = 0.0F;
  MyCInitTexture.tparams[3] = 0.0F;
  Update();
}


Graphic3d_Texture1Dsegment::Graphic3d_Texture1Dsegment(const Handle(Graphic3d_StructureManager)& SM,const Graphic3d_NameOfTexture1D NOT)
: Graphic3d_Texture1D(SM, NOT, Graphic3d_TOT_1D)
{
  MyCInitTexture.doModulate = 0;
  MyCInitTexture.doRepeat = 1;
  MyCInitTexture.Mode = (int)Graphic3d_TOTM_OBJECT;
  MyCInitTexture.doLinear = 0;
  MyCInitTexture.sx = 1.0F;
  MyCInitTexture.sy = 1.0F;
  MyCInitTexture.tx = 0.0F;
  MyCInitTexture.ty = 0.0F;
  MyCInitTexture.angle = 0.0F;
  MyCInitTexture.sparams[0] = 0.0F;
  MyCInitTexture.sparams[1] = 0.0F;
  MyCInitTexture.sparams[2] = 1.0F;
  MyCInitTexture.sparams[3] = 0.0F;
  MyCInitTexture.tparams[0] = 0.0F;
  MyCInitTexture.tparams[1] = 0.0F;
  MyCInitTexture.tparams[2] = 0.0F;
  MyCInitTexture.tparams[3] = 0.0F;
  Update();
}


void Graphic3d_Texture1Dsegment::SetSegment(const Standard_ShortReal X1,const Standard_ShortReal Y1,const Standard_ShortReal Z1,const Standard_ShortReal X2,const Standard_ShortReal Y2,const Standard_ShortReal Z2) 
{
  Standard_ShortReal sq_norme;

  MyX1 = X1;
  MyY1 = Y1;
  MyZ1 = Z1;
  MyX2 = X2;
  MyY2 = Y2;
  MyZ2 = Z2;

  MyCInitTexture.sparams[0] = X2-X1;
  MyCInitTexture.sparams[1] = Y2-Y1;
  MyCInitTexture.sparams[2] = Z2-Z1;
  sq_norme = MyCInitTexture.sparams[0]*MyCInitTexture.sparams[0]
           + MyCInitTexture.sparams[1]*MyCInitTexture.sparams[1]
           + MyCInitTexture.sparams[2]*MyCInitTexture.sparams[2];

  MyCInitTexture.sparams[0] /= sq_norme;
  MyCInitTexture.sparams[1] /= sq_norme;
  MyCInitTexture.sparams[2] /= sq_norme;

  MyCInitTexture.sparams[3] = - MyCInitTexture.sparams[0]*X1
                              - MyCInitTexture.sparams[1]*Y1
			      - MyCInitTexture.sparams[2]*Z1;
  
  Update();
}



void Graphic3d_Texture1Dsegment::Segment(Standard_ShortReal& X1,Standard_ShortReal& Y1,Standard_ShortReal& Z1,Standard_ShortReal& X2,Standard_ShortReal& Y2,Standard_ShortReal& Z2) const
{
  X1 = MyX1;
  Y1 = MyY1;
  Z1 = MyZ1;
  X2 = MyX2;
  Y2 = MyY2;
  Z2 = MyZ2;
}
