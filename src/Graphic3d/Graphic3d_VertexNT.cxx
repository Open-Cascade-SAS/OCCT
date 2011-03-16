
// File		Graphic3d_VertexNT.cxx
// Created	27/08/97
// Author	PCT
// Modified:	03-02-98 : FMN ; Add Flag Normal

//-Copyright	MatraDatavision 1997

//-Version	

//-Design	Declaration des variables specifiques aux points

//-Warning	Un point est defini par ses coordonnees et sa normale et des coordonnees texture

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_VertexNT.ixx>

//-Aliases

//-Global data definitions

//	-- la normale au point
//      MyTX, MyTY      :       Standard_Real;

//-Constructors

//-Destructors

//-Methods, in order

Graphic3d_VertexNT::Graphic3d_VertexNT ():
Graphic3d_VertexN (0.0, 0.0, 0.0, 0.0, 0.0, 1.0),
MyTX(0.0),
MyTY(0.0) {
}

Graphic3d_VertexNT::Graphic3d_VertexNT (const Standard_Real AX, const Standard_Real AY, const Standard_Real AZ, const Standard_Real ANX, const Standard_Real ANY, const Standard_Real ANZ,const Standard_Real ATX,const Standard_Real ATY,const Standard_Boolean FlagNormalise):
Graphic3d_VertexN (0.0, 0.0, 0.0, 0.0, 0.0, 1.0,FlagNormalise),
MyTX(ATX),
MyTY(ATY) {
}

Graphic3d_VertexNT::Graphic3d_VertexNT (const Graphic3d_Vertex& APoint, const Graphic3d_Vector& AVector,const Standard_Real ATX,const Standard_Real ATY,const Standard_Boolean FlagNormalise):
Graphic3d_VertexN (APoint, AVector,FlagNormalise),
MyTX(ATX),
MyTY(ATY) {
}

void Graphic3d_VertexNT::SetTextureCoordinate(const Standard_Real ATX,const Standard_Real ATY) 
{
  MyTX = ATX;
  MyTY = ATY;
}

void Graphic3d_VertexNT::TextureCoordinate(Standard_Real& ATX,Standard_Real& ATY) const
{
  ATX = MyTX;
  ATY = MyTY;
}
