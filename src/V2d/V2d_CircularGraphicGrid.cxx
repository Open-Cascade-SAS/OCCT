// Modified	23/02/98 : FMN ; Remplacement PI par Standard_PI
#define PRO10288	//GG 240398/090798 (PRO13334)
//			Permettre un pas de rayon < 2 MM

#define OCC192 // jfa 27/02/2002
//                for right display of rotated grid

#include <V2d_CircularGraphicGrid.ixx>
#include <Aspect_Driver.hxx>

V2d_CircularGraphicGrid::V2d_CircularGraphicGrid(
      const Handle(Graphic2d_GraphicObject)& aGraphicObject,
      const Standard_Real X,
      const Standard_Real Y,
      const Standard_Real alpha,
      const Standard_Real step,
      const Standard_Integer aDivision,
      const Standard_Integer PointsColorIndex)

:Graphic2d_Primitive(aGraphicObject),
OX((Standard_ShortReal)X),
OY((Standard_ShortReal)Y),
angle((Standard_ShortReal)alpha),
Step((Standard_ShortReal)step),
Division(aDivision),
DrawMode(Aspect_GDM_Lines),
myPointsColorIndex(PointsColorIndex)
{

}

void V2d_CircularGraphicGrid::Draw(const Handle(Graphic2d_Drawer)& aDrawer){

  Standard_ShortReal XF, YF, SF, XT, YT, STANDARD, ZF;
  aDrawer->Values (XF, YF, SF, XT, YT, STANDARD, ZF);
  Standard_Real w,h;
  aDrawer->Driver()->WorkSpace(w,h);
  Standard_ShortReal Coef = 
    (Standard_ShortReal) ((w < h) ? w/Sqrt( w*w + h*h) : h/Sqrt( w*w + h*h)) ;


  Standard_Real d = Sqrt ( (OX-XF) * (OX-XF) + (OY-YF) * (OY-YF) );
  Standard_Integer n1,n;
  Standard_Real thestep = Step;
  if(d <= SF/Coef) {
    n1 = 1;
    n = (Standard_Integer )( ((Abs (OX-XF) > Abs (OY-YF) ?  Abs (OX-XF) :Abs (OY-YF)) + SF /Coef)/thestep +1);
  }
  else {
    n1 = (Standard_Integer )( (d - SF/Coef) /thestep);
    n = Standard_Integer ( SF / thestep/Coef ) *2 + 1;
  }
#ifdef PRO10288
  while ( n > 200) {
#else
  while ( n > 100) {
#endif
    thestep = thestep * 10.;
    if(d <= SF/Coef) {
      n1 = 1;
      n = (Standard_Integer )( ((Abs (OX-XF) > Abs (OY-YF) ?  Abs (OX-XF) :Abs (OY-YF)) + SF /Coef)/thestep +1);
    }
    else {
      n1 = (Standard_Integer )( (d - SF/Coef) /thestep);
      n = Standard_Integer ( SF / thestep/Coef ) *2 + 1;
    }
  }
//  cout << "XF=" << XF << " YF =" << YF << " SF=" << SF 
//  << " n1 = " << n1 << " n= " << n << endl << flush;
  Standard_ShortReal xc = Standard_ShortReal(OX);
  Standard_ShortReal yc = Standard_ShortReal(OY);
  Standard_ShortReal ds = Standard_ShortReal(thestep);
  Standard_ShortReal s= Standard_ShortReal(n1)*ds;
  switch (DrawMode){
  case Aspect_GDM_Points: {
    aDrawer->SetMarkerAttrib (myPointsColorIndex,0,0);
    aDrawer->MapMarkerFromTo(0,xc,yc,Standard_ShortReal(0.001),Standard_ShortReal(0.001),0.);
    for (Standard_Integer k=1; k<=n; k++) {
      DrawCircle(aDrawer,xc,yc,s,Standard_True);
      s+=ds;
    }
    break;
  }
  case Aspect_GDM_Lines: 
  default:
    {
    aDrawer->SetLineAttrib (myColorIndex,0,0);
    for (Standard_Integer i=1; i<=n; i++) {
      DrawCircle(aDrawer,xc,yc,s,Standard_False);
      s+=ds;
    }
    Standard_Real a = Standard_PI / Standard_Real(Division);
    for (Standard_Integer j=1; j<= Division; j++) {
     aDrawer->MapInfiniteLineFromTo(OX,OY ,
	 Standard_ShortReal(Cos(angle+Standard_Real(j)*a)),
	 Standard_ShortReal(Sin(angle+Standard_Real(j)*a)));
    }
   }
  }
}

Standard_Boolean V2d_CircularGraphicGrid::Pick(const Standard_ShortReal X,
				       const Standard_ShortReal Y,
				       const Standard_ShortReal aPrecision,
				       const Handle(Graphic2d_Drawer)& aDrawer) {
return Standard_False;
}
void V2d_CircularGraphicGrid::SetDrawMode (const Aspect_GridDrawMode aDrawMode) {
  DrawMode = aDrawMode;
}

void V2d_CircularGraphicGrid::DrawCircle
          (const Handle(Graphic2d_Drawer)& aDrawer,
	   const Standard_ShortReal myX,
	   const Standard_ShortReal myY,
	   const Standard_ShortReal myRadius,
	   const Standard_Boolean DrawPoints) const {

    Standard_ShortReal Def;
    Standard_ShortReal Coeff;
    Aspect_TypeOfDeflection Type;
    aDrawer->DrawPrecision(Def,Coeff,Type);
    Standard_Real val;
    if(myRadius > Def) 
      val = Max( 0.0044 , Min (0.7854 , 2. * ACos(1.-Def/myRadius)));
    else
      val = 0.7854;  // = Standard_PI/4.
    Standard_Integer nbpoints;
    if(DrawPoints) {
      nbpoints = Division *2;
    }
    else {
      nbpoints = Standard_Integer(Abs(2*Standard_PI)/val) +2;
    }
      
    Standard_ShortReal teta = Standard_ShortReal(Abs(2*Standard_PI) /nbpoints);
    Standard_ShortReal x1 = myRadius;
    Standard_ShortReal y1 = 0;
    Standard_ShortReal x2,y2,x3,y3;
    Standard_ShortReal cosin = Standard_ShortReal(Cos(teta));
    x2 =  Standard_ShortReal(myRadius * Cos(teta));
    y2 =  Standard_ShortReal(myRadius * Sin(teta));

#ifdef OCC192
    Standard_ShortReal x_alpha, y_alpha;
    Standard_ShortReal cos_alpha = Cos(angle), sin_alpha = Sin(angle);
#endif
    if(DrawPoints) {
#ifdef OCC192
      x_alpha = x1*cos_alpha - y1*sin_alpha; // rotate on <angle> radians
      y_alpha = x1*sin_alpha + y1*cos_alpha; // 
      aDrawer->MapMarkerFromTo(0,x_alpha+myX,y_alpha+myY,Standard_ShortReal(0.001),Standard_ShortReal(0.001),0.);
#else
      aDrawer->MapMarkerFromTo(0,x1+myX,y1+myY,Standard_ShortReal(0.001),Standard_ShortReal(0.001),0.);
      aDrawer->MapMarkerFromTo(0,x2+myX,y2+myY,Standard_ShortReal(0.001),Standard_ShortReal(0.001),0.);
#endif
      }
    else {
      aDrawer->MapSegmentFromTo(x1+myX,y1+myY,x2+myX,y2+myY);
    }
    for (Standard_Integer i = 3; i <= nbpoints+1; i++) {
      x3 = 2*x2*cosin - x1;
      y3 = 2*y2*cosin - y1;
      if(DrawPoints) {
#ifdef OCC192
      x_alpha = x2*cos_alpha - y2*sin_alpha;
      y_alpha = x2*sin_alpha + y2*cos_alpha;
      aDrawer->MapMarkerFromTo(0,x_alpha+myX,y_alpha+myY,Standard_ShortReal(0.001),Standard_ShortReal(0.001),0.);
#else
      aDrawer->MapMarkerFromTo(0,x2+myX,y2+myY,Standard_ShortReal(0.001),Standard_ShortReal(0.001),0.);
#endif
      }
      else {
	aDrawer->MapSegmentFromTo(x2+myX,y2+myY,x3+myX,y3+myY);
      }
      x1 = x2; y1 = y2; x2 = x3; y2 = y3;
    }
  }

void V2d_CircularGraphicGrid::Save(Aspect_FStream& aFStream) const
{
}

