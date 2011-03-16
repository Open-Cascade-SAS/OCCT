#include <Prs3d_AngleAspect.ixx>

Prs3d_AngleAspect::Prs3d_AngleAspect() {
  myLineAspect = new Prs3d_LineAspect 
                          (Quantity_NOC_LAWNGREEN,Aspect_TOL_SOLID,1.); 
  myArrowAspect = new Prs3d_ArrowAspect;
  myTextAspect = new Prs3d_TextAspect;
}

Handle (Prs3d_LineAspect) Prs3d_AngleAspect::LineAspect () const {
  return myLineAspect;}

void Prs3d_AngleAspect::SetLineAspect(const Handle(Prs3d_LineAspect)& anAspect) {
  myLineAspect = anAspect;}

Handle(Prs3d_ArrowAspect) Prs3d_AngleAspect::ArrowAspect () const {
  return myArrowAspect;}

void Prs3d_AngleAspect::SetArrowAspect (
				  const Handle(Prs3d_ArrowAspect)& anAspect) {
  myArrowAspect = anAspect;
}

Handle(Prs3d_TextAspect) Prs3d_AngleAspect::TextAspect () const {
  return myTextAspect;}

void Prs3d_AngleAspect::SetTextAspect (
				  const Handle(Prs3d_TextAspect)& anAspect) {
  myTextAspect = anAspect;
}

void Prs3d_AngleAspect::Print (Standard_OStream& s) const {
  
  s << "AngleAspect: " << endl;
  s << "                 " ; myLineAspect->Print(s); s << endl;
  s << "         arrow   " ; myArrowAspect->Print(s); s << endl;
  s << "                 " ; myTextAspect->Print(s); s << endl;
}

