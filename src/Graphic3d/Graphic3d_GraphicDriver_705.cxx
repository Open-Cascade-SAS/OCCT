// File		Graphic3d_GraphicDriver_705.cxx
// Created	Mardi 28 janvier 1997
// Author	CAL
// Modified
//              27/07/97 ; PCT : ajout coordonnee texture
//      02.15.100 : JR : Clutter

//-Copyright	MatraDatavision 1997

//-Version	

//-Design	Declaration des variables specifiques aux Drivers

//-Warning	Un driver encapsule les Pex et OpenGl drivers

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_GraphicDriver.jxx>

#include <Aspect_DriverDefinitionError.hxx>

//-Aliases

//-Global data definitions

//-Methods, in order

void Graphic3d_GraphicDriver::Polygon (const Graphic3d_CGroup& , const Graphic3d_Array1OfVertex& , const Graphic3d_TypeOfPolygon , const Standard_Boolean ) {
}

void Graphic3d_GraphicDriver::Polygon (const Graphic3d_CGroup& , const Graphic3d_Array1OfVertex& , const Graphic3d_Vector& , const Graphic3d_TypeOfPolygon , const Standard_Boolean ) {
}

void Graphic3d_GraphicDriver::Polygon (const Graphic3d_CGroup& , const Graphic3d_Array1OfVertexN& , const Graphic3d_TypeOfPolygon , const Standard_Boolean ) {
}

void Graphic3d_GraphicDriver::Polygon (const Graphic3d_CGroup& , const Graphic3d_Array1OfVertexN& , const Graphic3d_Vector& , const Graphic3d_TypeOfPolygon , const Standard_Boolean ) {
}


void Graphic3d_GraphicDriver::Polygon(const Graphic3d_CGroup& ,const Graphic3d_Array1OfVertexNT& ,const Graphic3d_TypeOfPolygon ,const Standard_Boolean ) 
{
}


void Graphic3d_GraphicDriver::PolygonHoles (const Graphic3d_CGroup& , const TColStd_Array1OfInteger& , const Graphic3d_Array1OfVertex& , const Standard_Boolean ) {
}

void Graphic3d_GraphicDriver::PolygonHoles (const Graphic3d_CGroup& , const TColStd_Array1OfInteger& , const Graphic3d_Array1OfVertex& , const Graphic3d_Vector& , const Standard_Boolean ) {
}

void Graphic3d_GraphicDriver::PolygonHoles (const Graphic3d_CGroup& , const TColStd_Array1OfInteger& , const Graphic3d_Array1OfVertexN& , const Standard_Boolean ) {
}

void Graphic3d_GraphicDriver::PolygonHoles (const Graphic3d_CGroup& , const TColStd_Array1OfInteger& , const Graphic3d_Array1OfVertexN& , const Graphic3d_Vector& , const Standard_Boolean ) {
}
