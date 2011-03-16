#include <Prs3d_DatumAspect.ixx>

Prs3d_DatumAspect::Prs3d_DatumAspect () {

  myFirstAxisAspect = new Prs3d_LineAspect
      (Quantity_NOC_PEACHPUFF,Aspect_TOL_SOLID,1.);
  mySecondAxisAspect = new Prs3d_LineAspect
      (Quantity_NOC_PEACHPUFF,Aspect_TOL_SOLID,1.);
  myThirdAxisAspect = new Prs3d_LineAspect
      (Quantity_NOC_PEACHPUFF,Aspect_TOL_SOLID,1.);
  myDrawFirstAndSecondAxis = Standard_True;
  myDrawThirdAxis = Standard_True;
  myFirstAxisLength = 10.;
  mySecondAxisLength = 10.;
  myThirdAxisLength = 10.;
}

Handle(Prs3d_LineAspect) Prs3d_DatumAspect::FirstAxisAspect() const {

  return myFirstAxisAspect;
}

Handle(Prs3d_LineAspect) Prs3d_DatumAspect::SecondAxisAspect()  const {

  return mySecondAxisAspect;
}

Handle(Prs3d_LineAspect) Prs3d_DatumAspect::ThirdAxisAspect()  const {

  return myThirdAxisAspect;

}
Standard_Boolean Prs3d_DatumAspect::DrawFirstAndSecondAxis () const {

  return myDrawFirstAndSecondAxis;

}

void Prs3d_DatumAspect::SetDrawFirstAndSecondAxis (const Standard_Boolean draw)
{
  myDrawFirstAndSecondAxis = draw;
}

Standard_Boolean Prs3d_DatumAspect::DrawThirdAxis () const {

  return myDrawThirdAxis;

}

void Prs3d_DatumAspect::SetDrawThirdAxis (const Standard_Boolean draw)
{
  myDrawThirdAxis = draw;
}

void Prs3d_DatumAspect::SetAxisLength (const Quantity_Length L1,
				       const Quantity_Length L2,
				       const Quantity_Length L3) {

  myFirstAxisLength = L1;
  mySecondAxisLength = L2;
  myThirdAxisLength = L3;
}


Quantity_Length Prs3d_DatumAspect::FirstAxisLength () const {

  return myFirstAxisLength;

}

Quantity_Length Prs3d_DatumAspect::SecondAxisLength () const {

  return mySecondAxisLength;

}

Quantity_Length Prs3d_DatumAspect::ThirdAxisLength () const {

  return myThirdAxisLength;

}

void Prs3d_DatumAspect::Print (Standard_OStream& s) const {

  s << "DatumAspect: " << endl;
  s << "        FirstAxis:  length "  << myFirstAxisLength  << "  ";
  myFirstAxisAspect->Print(s);
  s << endl;
  s << "        SecondAxis: length "  << mySecondAxisLength << "  ";
  mySecondAxisAspect->Print(s);
  s << endl;
  s << "        ThirdAxis:  length "  << myThirdAxisLength  << "  ";
  myThirdAxisAspect->Print(s);
  s << endl;
  if (myDrawFirstAndSecondAxis)
    s << "      Draws first and second axis";
  else
    s << "      Does not draw first and second axis";
  
  if (myDrawThirdAxis)
    s << "      Draws third axis";
  else
    s << "      Does not draw third axis";
}
