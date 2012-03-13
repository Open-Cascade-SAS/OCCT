#define GER61351		//GG_171199     Enable to set an object RGB color
//						  instead a restricted object NameOfColor.

#include <Prs3d_PointAspect.ixx>

#ifdef GER61351
Prs3d_PointAspect::Prs3d_PointAspect (const Aspect_TypeOfMarker aType,
				  const Quantity_Color &aColor,
				  const Standard_Real aScale) {
  myAspect = new Graphic3d_AspectMarker3d(aType,aColor,aScale);
}
#endif

Prs3d_PointAspect::Prs3d_PointAspect (const Aspect_TypeOfMarker aType,
				  const Quantity_NameOfColor aColor,
				  const Standard_Real aScale) {
  myAspect = new Graphic3d_AspectMarker3d
    (aType,Quantity_Color(aColor),aScale);
}

Prs3d_PointAspect::Prs3d_PointAspect (const Quantity_Color &aColor,
				      const Standard_Real anId,
                                      const Standard_Integer aWidth,
                                      const Standard_Integer aHeight,
				      const Handle(TColStd_HArray1OfByte)& aTexture
                                      ) 
{
  myAspect = new Graphic3d_AspectMarker3d
    (Aspect_TOM_USERDEFINED,aColor,anId,aWidth,aHeight,aTexture);
}

#ifdef GER61351
void Prs3d_PointAspect::SetColor(const Quantity_Color &aColor) {
  myAspect->SetColor(aColor);
}
#endif

void Prs3d_PointAspect::SetColor(const Quantity_NameOfColor aColor) {
  myAspect->SetColor(Quantity_Color(aColor));
}

void Prs3d_PointAspect::SetTypeOfMarker(const Aspect_TypeOfMarker aType){
  myAspect->SetType(aType);
}

void Prs3d_PointAspect::SetScale(const Standard_Real aScale){
  myAspect->SetScale(aScale);
}

Handle (Graphic3d_AspectMarker3d) Prs3d_PointAspect::Aspect () const {
  return myAspect;
}

void Prs3d_PointAspect::GetTextureSize(Standard_Integer& AWidth, Standard_Integer& AHeight)
{
  myAspect->GetTextureSize( AWidth, AHeight);
}

const Handle(TColStd_HArray1OfByte)& Prs3d_PointAspect::GetTexture()
{
  return myAspect->GetTexture();
}
