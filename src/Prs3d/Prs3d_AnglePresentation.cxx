#include <Prs3d_AnglePresentation.ixx>
#include <gp_Lin.hxx>
#include <gp_Dir.hxx>
#include <ElCLib.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_AngleAspect.hxx>
#include <TCollection_AsciiString.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Prs3d_Text.hxx>

void Prs3d_AnglePresentation::Draw (
			       const Handle(Prs3d_Presentation)& aPresentation,
			       const Handle(Prs3d_Drawer)& aDrawer,
			       const TCollection_ExtendedString& aText,
			       const gp_Pnt& AttachmentPoint1,
			       const gp_Pnt& AttachmentPoint2,
			       const gp_Pnt& AttachmentPoint3,
			       const gp_Pnt& OffsetPoint) {


 Handle(Prs3d_AngleAspect) AA = aDrawer->AngleAspect();
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(AA->LineAspect()->Aspect());
  gp_Vec V1 (AttachmentPoint1,AttachmentPoint2);
  gp_Vec V2 (AttachmentPoint1,AttachmentPoint3);
  Standard_Real alpha = V1.Angle(V2);
  Standard_Real ray = AttachmentPoint1.Distance(OffsetPoint);
 

  gp_Dir I (V1);
  gp_Dir K =  I.Crossed(gp_Dir(V2));
  gp_Dir J = K.Crossed(I);
//
  Standard_Real xc,yc,zc;
  AttachmentPoint1.Coord(xc,yc,zc);
  Graphic3d_Array1OfVertex V(1,2);
  Standard_Real x1,y1,z1,x2,y2,z2;
  AttachmentPoint2.Coord(x1,y1,z1);
  AttachmentPoint3.Coord(x2,y2,z2);
  Standard_Integer nbp = Max (4 , Standard_Integer (50. * alpha / PI));
  Standard_Real dteta = alpha/(nbp-1);
  Standard_Real x,y,z;
  gp_Vec u;
  for (Standard_Integer i = 1; i<=nbp; i++) {
    u =  (gp_Vec(I) * Cos ( (i-1) * dteta) 
       +  gp_Vec(J) * Sin ( (i-1) * dteta)) * ray ;
    u.Coord(x,y,z);
    if(i == 1) {
       V(1).SetCoord( xc + x, yc + y, zc + z); 
       V(2).SetCoord( x1,y1,z1);
       Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);}
    else {
       V(2).SetCoord( xc + x, yc + y, zc + z);
       Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
       if( i == nbp ) {
         Standard_Real a1,b1,c1,a2,b2,c2;
         V(1).Coord(a1,b1,c1); gp_Pnt pt1(a1,b1,c1);
         V(2).Coord(a2,b2,c2); gp_Pnt pt2(a2,b2,c2);
         gp_Dir dir( gp_Vec(pt1 , pt2) );
	 Prs3d_Arrow::Draw(aPresentation,pt2,dir,AA->ArrowAspect()->Angle(),
                                                 AA->ArrowAspect()->Length());  
       }
       V(1)=V(2);
     }
  }
    V(2).SetCoord(x2,y2,z2);
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
    u =  (gp_Vec(I) * Cos ( alpha/2.) 
              + gp_Vec(J) * Sin ( alpha/2.)) * ray ;
    u.Coord(x,y,z);
  Prs3d_Text::Draw(aPresentation,AA->TextAspect(),aText,
     gp_Pnt(xc+x,yc+y,zc+z));
}


