
// File		Graphic3d_AspectMarker3d.cxx
// Created	Fevrier 1992
// Author	NW,JPB,CAL

//-Copyright	MatraDatavision 1991,1992

//-Version	

//-Design	Declaration des variables specifiques au contexte
//		de trace des markers 3d

// Rappels	Un contexte de trace de markers 3d herite du contexte
//		defini par :
//		- la couleur
//		- le type de marker
//		- l'echelle

//-Warning	

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_AspectMarker3d.ixx>
#include <TColStd_Array1OfByte.hxx>

//-Aliases

//-Global data definitions

//-Constructors

//-Destructors

//-Methods, in order

Graphic3d_AspectMarker3d::Graphic3d_AspectMarker3d ()
     :Aspect_AspectMarker(), MyTextureWidth(0), MyTextureHeight(0)
{  
}

Graphic3d_AspectMarker3d::Graphic3d_AspectMarker3d (const Aspect_TypeOfMarker AType, const Quantity_Color& AColor, const Standard_Real AScaleOrId ):
Aspect_AspectMarker( AColor, AType, AScaleOrId ), MyTextureWidth(0), MyTextureHeight(0)
{ 
}


Graphic3d_AspectMarker3d::Graphic3d_AspectMarker3d (const Aspect_TypeOfMarker AType, 
						    const Quantity_Color& AColor, 
						    const Standard_Real AScaleOrId, 
						    const Standard_Integer AWidth,
						    const Standard_Integer AHeight,
						    const Handle(TColStd_HArray1OfByte)& ATexture ):
Aspect_AspectMarker( AColor, AType, AScaleOrId ), MyTexture( ATexture ), MyTextureWidth(AWidth), MyTextureHeight(AHeight)
{
  /*for( Standard_Integer aIndex = ATexture.Lower(); aIndex <= ATexture.Upper(); aIndex++ )
    {
      MyTexture.SetValue( aIndex, ATexture.Value( aIndex ) );
    }
  */
  
}

void Graphic3d_AspectMarker3d::GetTextureSize(Standard_Integer& AWidth, Standard_Integer& AHeight)
{
  AWidth = MyTextureWidth;
  AHeight = MyTextureHeight;
}

const Handle(TColStd_HArray1OfByte)& Graphic3d_AspectMarker3d::GetTexture()
{
  return MyTexture;
}

void Graphic3d_AspectMarker3d::SetTexture (const Standard_Integer AWidth,
					   const Standard_Integer AHeight,
                                           const Handle(TColStd_HArray1OfByte)& ATexture )
{
   MyTextureWidth = AWidth;
   MyTextureHeight = AHeight;
     
   MyTexture = ATexture;
}
