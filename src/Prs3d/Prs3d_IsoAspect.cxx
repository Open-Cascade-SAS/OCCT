#define GER61351		//GG_171199     Enable to set an object RGB color
//						  instead a restricted object NameOfColor.

#include <Prs3d_IsoAspect.ixx>

#ifdef GER61351
Prs3d_IsoAspect::Prs3d_IsoAspect(const Quantity_Color &aColor,
			     const Aspect_TypeOfLine aType,
			     const Standard_Real aWidth,
			     const Standard_Integer aNumber) 
 :Prs3d_LineAspect (aColor,aType,aWidth) {
   myNumber = aNumber;
}
#endif

Prs3d_IsoAspect::Prs3d_IsoAspect(const Quantity_NameOfColor aColor,
			     const Aspect_TypeOfLine aType,
			     const Standard_Real aWidth,
			     const Standard_Integer aNumber) 
 :Prs3d_LineAspect (aColor,aType,aWidth) {
   myNumber = aNumber;
}
void Prs3d_IsoAspect::SetNumber (const Standard_Integer aNumber) {
  myNumber = aNumber;
}

Standard_Integer Prs3d_IsoAspect::Number () const {return myNumber;}

void Prs3d_IsoAspect::Print(Standard_OStream& s) const {

  s << "IsoAspect: " << myNumber << " ";
  Prs3d_LineAspect::Print(s);

}
