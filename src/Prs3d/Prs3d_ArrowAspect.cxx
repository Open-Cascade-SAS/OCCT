#define IMP120100	//GG 01/01/00 Add SetColor() methods

#include <Prs3d_ArrowAspect.ixx>

Prs3d_ArrowAspect::Prs3d_ArrowAspect () 
     : myAngle(PI/180.*10), myLength(1.) {
#ifdef IMP120100
  myArrowAspect = 
	new Graphic3d_AspectLine3d (
		Quantity_Color(Quantity_NOC_WHITE), Aspect_TOL_SOLID, 1.0);
#endif
}


Prs3d_ArrowAspect::Prs3d_ArrowAspect (const Quantity_PlaneAngle anAngle,
				      const Quantity_Length aLength) 
     : myAngle(anAngle), myLength(aLength) {
#ifdef IMP120100
  myArrowAspect = 
	new Graphic3d_AspectLine3d (
		Quantity_Color(Quantity_NOC_WHITE), Aspect_TOL_SOLID, 1.0);
#endif
}

void Prs3d_ArrowAspect::SetAngle ( const Quantity_PlaneAngle anAngle) {
  Prs3d_InvalidAngle_Raise_if ( anAngle <= 0.  ||
                              anAngle >= PI /2. , "");
  myAngle = anAngle;
}
Quantity_PlaneAngle Prs3d_ArrowAspect::Angle () const
{
return myAngle;
}

void Prs3d_ArrowAspect::SetLength ( const Quantity_Length aLength)
{
  myLength = aLength;
}
Quantity_Length Prs3d_ArrowAspect::Length () const
{
return myLength;
}

void Prs3d_ArrowAspect::Print (Standard_OStream& s) const {

  s << "ArrowAspect: Length: " << myLength << " Angle: " << myAngle;

}

#ifdef IMP120100
void Prs3d_ArrowAspect::SetColor(const Quantity_Color &aColor) {
  myArrowAspect->SetColor(aColor);
}

void Prs3d_ArrowAspect::SetColor(const Quantity_NameOfColor aColor) {
  SetColor(Quantity_Color(aColor));
}

Handle(Graphic3d_AspectLine3d) Prs3d_ArrowAspect::Aspect() const {
  return myArrowAspect;
}
#endif
