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
void Prs3d_PointAspect::Print (Standard_OStream& s) const {

  Quantity_Color C;
  Aspect_TypeOfMarker T;
  Standard_Real S;
  myAspect->Values(C,T,S);
  switch (T) {
  case Aspect_TOM_POINT:
    s << "PointAspect: " << Quantity_Color::StringName(C.Name()) << " POINT " << S;
    break;
  case Aspect_TOM_PLUS:
    s << "PointAspect: " << Quantity_Color::StringName(C.Name()) << " PLUS " << S;
    break;
  case Aspect_TOM_STAR:
    s << "PointAspect: " << Quantity_Color::StringName(C.Name()) << " STAR " << S;
    break;
  case Aspect_TOM_O:
    s << "PointAspect: " << Quantity_Color::StringName(C.Name()) << " O " << S;
    break;
  case Aspect_TOM_X:
    s << "PointAspect: " << Quantity_Color::StringName(C.Name()) << " X " << S;
    break;
  case Aspect_TOM_O_POINT:
    s << "PointAspect: " << Quantity_Color::StringName(C.Name()) << " O_POINT " << S;
    break;
  case Aspect_TOM_O_PLUS:
    s << "PointAspect: " << Quantity_Color::StringName(C.Name()) << " O_PLUS " << S;
    break;
  case Aspect_TOM_O_STAR:
    s << "PointAspect: " << Quantity_Color::StringName(C.Name()) << " O_STAR " << S;
    break;
  case Aspect_TOM_O_X:
    s << "PointAspect: " << Quantity_Color::StringName(C.Name()) << " O_X " << S;
    break;
  case Aspect_TOM_BALL:
    s << "PointAspect: " << Quantity_Color::StringName(C.Name()) << " BALL " << S;
    break;
  case Aspect_TOM_RING1:
    s << "PointAspect: " << Quantity_Color::StringName(C.Name()) << " RING1 " << S;
    break;
  case Aspect_TOM_RING2:
    s << "PointAspect: " << Quantity_Color::StringName(C.Name()) << " RING2 " << S;
    break;
  case Aspect_TOM_RING3:
    s << "PointAspect: " << Quantity_Color::StringName(C.Name()) << " RING3 " << S;
    break;
  case Aspect_TOM_USERDEFINED:
    s << "PointAspect: " << Quantity_Color::StringName(C.Name()) << " USERDEFINED " << S;
    break;
  default:
    break;
  }
}
void Prs3d_PointAspect::GetTextureSize(Standard_Integer& AWidth, Standard_Integer& AHeight)
{
  myAspect->GetTextureSize( AWidth, AHeight);
}

const Handle(TColStd_HArray1OfByte)& Prs3d_PointAspect::GetTexture()
{
  return myAspect->GetTexture();
}
