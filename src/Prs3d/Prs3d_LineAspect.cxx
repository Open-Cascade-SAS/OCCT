#define GER61351		//GG_171199     Enable to set an object RGB color
//						  instead a restricted object NameOfColor.

#include <Prs3d_LineAspect.ixx>

#ifdef GER61351
Prs3d_LineAspect::Prs3d_LineAspect (const Quantity_Color &aColor,
				const Aspect_TypeOfLine aType,
				const Standard_Real aWidth) {
  myAspect = new Graphic3d_AspectLine3d(aColor,aType,aWidth);
}
#endif

Prs3d_LineAspect::Prs3d_LineAspect (const Quantity_NameOfColor aColor,
				const Aspect_TypeOfLine aType,
				const Standard_Real aWidth) {

  myAspect = new Graphic3d_AspectLine3d
    (Quantity_Color(aColor),aType,aWidth);
}

#ifdef GER61351
void Prs3d_LineAspect::SetColor(const Quantity_Color &aColor) {
 myAspect->SetColor(aColor);
}
#endif

void Prs3d_LineAspect::SetColor(const Quantity_NameOfColor aColor) {
  myAspect->SetColor(Quantity_Color(aColor));
}

void Prs3d_LineAspect::SetTypeOfLine(const Aspect_TypeOfLine aType){
  myAspect->SetType(aType);
}

void Prs3d_LineAspect::SetWidth(const Standard_Real aWidth){
  myAspect->SetWidth(aWidth);
}

Handle (Graphic3d_AspectLine3d) Prs3d_LineAspect::Aspect () const {
  return myAspect;
}
