#include <Prs3d_LengthAspect.ixx>

Prs3d_LengthAspect::Prs3d_LengthAspect() {
  myLineAspect = new Prs3d_LineAspect 
                          (Quantity_NOC_LAWNGREEN,Aspect_TOL_SOLID,1.); 
  myArrow1Aspect = new Prs3d_ArrowAspect;
  myArrow2Aspect = new Prs3d_ArrowAspect;
  myTextAspect = new Prs3d_TextAspect;
  myDrawFirstArrow = Standard_True;
  myDrawSecondArrow = Standard_True;
}

Handle (Prs3d_LineAspect) Prs3d_LengthAspect::LineAspect () const {
  return myLineAspect;}

void Prs3d_LengthAspect::SetLineAspect(const Handle(Prs3d_LineAspect)& anAspect) {
  myLineAspect = anAspect;}

Handle(Prs3d_ArrowAspect) Prs3d_LengthAspect::Arrow1Aspect () const {
  return myArrow1Aspect;}

void Prs3d_LengthAspect::SetArrow1Aspect (
				  const Handle(Prs3d_ArrowAspect)& anAspect) {
  myArrow1Aspect = anAspect;
}
Handle(Prs3d_ArrowAspect) Prs3d_LengthAspect::Arrow2Aspect () const {
  return myArrow2Aspect;}

void Prs3d_LengthAspect::SetArrow2Aspect (
				  const Handle(Prs3d_ArrowAspect)& anAspect) {
  myArrow2Aspect = anAspect;
}

Handle(Prs3d_TextAspect) Prs3d_LengthAspect::TextAspect () const {
  return myTextAspect;}

void Prs3d_LengthAspect::SetTextAspect (
				  const Handle(Prs3d_TextAspect)& anAspect) {
  myTextAspect = anAspect;
}
void Prs3d_LengthAspect::SetDrawFirstArrow (const Standard_Boolean draw) {
  myDrawFirstArrow = draw;
}
Standard_Boolean Prs3d_LengthAspect::DrawFirstArrow () const {
  return myDrawFirstArrow;
}

void Prs3d_LengthAspect::SetDrawSecondArrow (const Standard_Boolean draw) {
  myDrawSecondArrow = draw;
}
Standard_Boolean Prs3d_LengthAspect::DrawSecondArrow () const {
  return myDrawSecondArrow;
}

void Prs3d_LengthAspect::Print (Standard_OStream& s) const {
  
  s << "LengthAspect: " << endl;
  s << "                 " ; myLineAspect->Print(s); s << endl;
  s << "   First arrow   " ; myArrow1Aspect->Print(s); s << endl;
  s << "   Second arrow  " ; myArrow2Aspect->Print(s); s << endl;
  s << "                 " ; myTextAspect->Print(s); s << endl;
}
