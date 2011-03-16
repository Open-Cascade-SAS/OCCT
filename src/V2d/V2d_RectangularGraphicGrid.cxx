#define BUC60639        //GG 15/03/00 Avoid to compute view minmax
//                      including grid origin marker.
//                      // Sets correctly the grid origin marker size.

#define OCC194 //jfa 28/02/2002
//               Fill all the viewer area with rotated rectangular grid

#include <V2d_RectangularGraphicGrid.ixx>
#include <Aspect_Driver.hxx>

#ifdef BUC60639
#define MTHICK 6
#define MINDEX 4
#endif

//=======================================================================
//function : V2d_RectangularGraphicGrid
//purpose  : 
//=======================================================================

V2d_RectangularGraphicGrid::V2d_RectangularGraphicGrid(
      const Handle(Graphic2d_GraphicObject)& aGraphicObject,
      const Standard_Real X,
      const Standard_Real Y,
      const Standard_Real alpha,
      const Standard_Real beta,
      const Standard_Real xo,
      const Standard_Real yo,
      const Standard_Integer aTenthColorIndex) 
:Graphic2d_Primitive(aGraphicObject),StepX((Standard_ShortReal)(X)), StepY((Standard_ShortReal)(Y)),
                     a1((Standard_ShortReal)(alpha)),a2((Standard_ShortReal)(beta)),
                     OX((Standard_ShortReal)(xo)),OY((Standard_ShortReal)(yo)),
                     DrawMode(Aspect_GDM_Lines),myTenthColorIndex(aTenthColorIndex){

}

//=======================================================================
//function : Draw
//purpose  : 
//=======================================================================

void V2d_RectangularGraphicGrid::Draw(const Handle(Graphic2d_Drawer)& aDrawer) {

  Standard_ShortReal XF, YF, SF, XT, YT, STANDARD, ZF;
  aDrawer->Values (XF, YF, SF, XT, YT, STANDARD, ZF);
//
  Standard_Real w,h; aDrawer->Driver()->WorkSpace(w,h);
//  cout << "Work space: " << w << "," << h << endl;
  Standard_ShortReal Coef = Standard_ShortReal( (w > h) ? (w/h) : h/w);
#ifdef OCC194
  if (Abs(a1) > 0.0)
  {
    Standard_ShortReal alpha = a1 - ((Standard_Integer)(a1/1.57))*1.57;
    alpha = alpha - 1.57/2.0;
    Coef = Abs(Coef * 1.41 * Cos(alpha));
  }
#endif

  switch (DrawMode){
  case Aspect_GDM_Points: 
    aDrawer->SetMarkerAttrib (myTenthColorIndex,0,0);
    DrawPoints(aDrawer,XF,YF,SF * Coef);
    break;
  case Aspect_GDM_Lines: 
  default:
    aDrawer->SetLineAttrib (myColorIndex,0,0);
    DrawNetwork(aDrawer,a1,StepX,XF,YF,SF * Coef);
    DrawNetwork(aDrawer,a2,StepY,XF,YF,SF * Coef);
  }
#ifdef BUC60639
  aDrawer->SetMarkerAttrib(myTenthColorIndex,MTHICK,Standard_False);
  aDrawer->MapMarkerFromTo (MINDEX, OX, OY, (Standard_ShortReal)(StepX/2.), (Standard_ShortReal)(StepY/2.), 0.0);
#endif
}

//=======================================================================
//function : Pick
//purpose  : 
//=======================================================================

Standard_Boolean V2d_RectangularGraphicGrid::Pick(const Standard_ShortReal ,
                                                  const Standard_ShortReal ,
                                                  const Standard_ShortReal ,
                                                  const Handle(Graphic2d_Drawer)& )
{
  return Standard_False;
}

//=======================================================================
//function : DrawNetwork
//purpose  : 
//=======================================================================

void V2d_RectangularGraphicGrid::DrawNetwork(const Handle(Graphic2d_Drawer)& aDrawer,
                                             const Standard_ShortReal alpha,
                                             const Standard_ShortReal step,
                                             const Standard_ShortReal XF,
                                             const Standard_ShortReal YF,
                                             const Standard_ShortReal SF) const
{

  Standard_Real thestep = step;
#ifdef OCC194
  Standard_Integer   n = Standard_Integer ( SF * 2 / thestep ) + 1;
#else
  Standard_Integer   n = Standard_Integer ( SF / thestep ) * 2 + 1;
#endif

//+zov Fixing CTS17856
//  cerr << "V2d...Grid::DrawNetwork:  step = " << step << " alpha" << alpha << " n = " << n << endl << flush;  
//-zov


//+zov Fixing CTS17856
//  Standard_ShortReal c = cos(alpha);
//  Standard_ShortReal s = sin(alpha);
  Standard_ShortReal s = Standard_ShortReal(cos(alpha));
  Standard_ShortReal c = Standard_ShortReal(-sin(alpha));
//-zov

  Standard_ShortReal d = Abs(s*(XF-OX) - c*(YF-OY));
  Standard_Integer   n1 = (Standard_Integer )( (d + SF) / thestep);

  Standard_ShortReal decalx,decaly;

  Standard_ShortReal deltax = (Standard_ShortReal) (  thestep * s );
  Standard_ShortReal deltay = (Standard_ShortReal) (- thestep * c );

  Standard_Boolean neg = s*(XF-OX) - c*(YF-OY) < 0 ? Standard_True
                                                    : Standard_False;
  if ( neg ) {
     decalx =   (Standard_ShortReal) (OX -  n1 * thestep * s );
     decaly =   (Standard_ShortReal) (OY +  n1 * thestep * c );
     deltax =   (Standard_ShortReal) (  thestep * s );
     deltay =   (Standard_ShortReal) (- thestep * c );
   }
  else {
     decalx =   (Standard_ShortReal) (OX + n1 * thestep * s );
     decaly =   (Standard_ShortReal) (OY - n1 * thestep * c );
     deltax =   (Standard_ShortReal) (- thestep * s );
     deltay =   (Standard_ShortReal) (  thestep * c );
   }

 Standard_Integer p = Modulus( n1 , 10);
 for (Standard_Integer i = 1; i <= n ; i++) {
    if( p == 0){
      aDrawer->SetLineAttrib(myTenthColorIndex,0,0);
      aDrawer->MapInfiniteLineFromTo(decalx ,decaly ,c,s);
      aDrawer->SetLineAttrib(myColorIndex,0,0);
    }
    else
      if (n < 300)aDrawer->MapInfiniteLineFromTo(decalx ,decaly ,c,s);

    p = p >= 1 ? p-1: 9;
    decalx += deltax;
    decaly += deltay;
  }
}

//=======================================================================
//function : DrawPoints
//purpose  : 
//=======================================================================

void V2d_RectangularGraphicGrid::DrawPoints(const Handle(Graphic2d_Drawer)& aDrawer,
                                            const Standard_ShortReal XF,
                                            const Standard_ShortReal YF,
                                            const Standard_ShortReal SF) const
{
    Standard_ShortReal sx = StepX;
    Standard_ShortReal sy = StepY;
    Standard_Integer n = Standard_Integer(SF/Min(sx,sy)) * 2 + 1;
      

//+zov Fixing CTS17856
//    Standard_ShortReal sin1 = Standard_ShortReal(Sin(a1)); 
//    Standard_ShortReal cos1 = Standard_ShortReal(Cos(a1));
//    Standard_ShortReal sin2 = Standard_ShortReal(Sin(a2)); 
//    Standard_ShortReal cos2 = Standard_ShortReal(Cos(a2));

    Standard_ShortReal cos1 = -Standard_ShortReal(Sin(a1)); 
    Standard_ShortReal sin1 = Standard_ShortReal(Cos(a1));
    Standard_ShortReal cos2 = -Standard_ShortReal(Sin(a2)); 
    Standard_ShortReal sin2 = Standard_ShortReal(Cos(a2));
// well it's somewhat silly to call cos sinus and vice versa but
// it is the simplest way to patch a hole. I mean CTS17856. (zov)
//-zov

    Standard_ShortReal c1 = OX * sin1 - OY * cos1;      
    Standard_ShortReal c2 = OX * sin2 - OY * cos2;
    Standard_ShortReal D1 = sin1 * XF - cos1 * YF - c1;
    Standard_ShortReal D2 = sin2 * XF - cos2 * YF - c2;
    Standard_Integer n1 = Standard_Integer ( Abs(D1)/sx + 0.5);
    Standard_Integer n2 = Standard_Integer ( Abs(D2)/sy + 0.5);
    Standard_ShortReal offset1 = (Standard_ShortReal) (c1 + Standard_ShortReal(n1) * Sign (sx , D1));
    Standard_ShortReal offset2 = (Standard_ShortReal) (c2 + Standard_ShortReal(n2) * Sign (sy , D2));
    Standard_ShortReal Delta = sin1*cos2 - cos1*sin2;
    Standard_ShortReal gridX = ( offset1*cos2 - offset2*cos1) /Delta;
    Standard_ShortReal gridY = ( offset1*sin2 - offset2*sin1) /Delta;
    Standard_ShortReal v1x = sx * cos2;
    Standard_ShortReal v1y = sx * sin2;
    Standard_ShortReal v2x = sy * cos1;
    Standard_ShortReal v2y = sy * sin1;
    
    Standard_ShortReal debx = gridX - n/2 * v1x - n/2 * v2x;
    Standard_ShortReal deby = gridY - n/2 * v1y - n/2 * v2y;
    Standard_ShortReal ax;
    Standard_ShortReal ay;
    for (Standard_Integer i=1; i<=n ; i++) {
      ax = debx;
      ay = deby;
      for (Standard_Integer j=1; j<=n ; j++) {
        aDrawer->MapMarkerFromTo(0,ax,ay,Standard_ShortReal(0.001),Standard_ShortReal(0.001),0.);
        ax += v1x;
        ay += v1y;
      }
      debx += v2x;
      deby += v2y;
    }
} 

//=======================================================================
//function : SetDrawMode
//purpose  : 
//=======================================================================

void V2d_RectangularGraphicGrid::SetDrawMode (const Aspect_GridDrawMode aDrawMode)
{
  DrawMode = aDrawMode;
}

//=======================================================================
//function : Save
//purpose  : 
//=======================================================================

void V2d_RectangularGraphicGrid::Save(Aspect_FStream& /*aFStream*/) const
{
}

