// File         V3d.cxx
// Created      September 1992
// Author       GG
// Modified     23/02/98 : FMN ; Remplacement PI par Standard_PI
//              02.15.100 : JR : Clutter

//-Copyright    MatraDatavision 1991,1992

//-Version

//-Design       

//-Warning     

//-References

//-Language     C++ 2.1

//-Declarations

// for the class
#include <V3d.ixx>
#include <V3d_View.hxx>

#include <Quantity_NameOfColor.hxx>
#include <Aspect_Grid.hxx>
#include <Aspect_Window.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Visual3d_HSetOfView.hxx>
#include <Visual3d_SetIteratorOfSetOfView.hxx>
#include <Visual3d_View.hxx>
#include <Visual3d_ViewManager.hxx>

Graphic3d_Vector V3d::GetProjAxis(const V3d_TypeOfOrientation Orientation) {
Standard_Real Xpn=0,Ypn=0,Zpn=0 ;
Graphic3d_Vector Vec ;

        switch (Orientation) {
            case V3d_Xpos :
                Xpn = 1. ; Ypn = 0. ; Zpn = 0. ;
                break ;
            case V3d_Ypos :
                Xpn = 0. ; Ypn = 1. ; Zpn = 0. ;
                break ;
            case V3d_Zpos :
                Xpn = 0. ; Ypn = 0. ; Zpn = 1. ;
                break ;
            case V3d_Xneg :
                Xpn = -1. ; Ypn = 0. ; Zpn = 0. ;
                break ;
            case V3d_Yneg :
                Xpn = 0. ; Ypn = -1. ; Zpn = 0. ;
                break ;
            case V3d_Zneg :
                Xpn = 0. ; Ypn = 0. ; Zpn = -1. ;
                break ;
            case V3d_XposYposZpos :
                Xpn = 1. ; Ypn = 1. ; Zpn = 1. ;
                break ;
            case V3d_XposYposZneg :
                Xpn = 1. ; Ypn = 1. ; Zpn = -1. ;
                break ;
            case V3d_XposYnegZpos :
                Xpn = 1. ; Ypn = -1. ; Zpn = 1. ;
                break ;
            case V3d_XposYnegZneg :
                Xpn = 1. ; Ypn = -1. ; Zpn = -1. ;
                break ;
            case V3d_XnegYposZpos :
                Xpn = -1. ; Ypn = 1. ; Zpn = 1. ;
                break ;
            case V3d_XnegYposZneg :
                Xpn = -1. ; Ypn = 1. ; Zpn = -1. ;
                break ;
            case V3d_XnegYnegZpos :
                Xpn = -1. ; Ypn = -1. ; Zpn = 1. ;
                break ;
            case V3d_XnegYnegZneg :
                Xpn = -1. ; Ypn = -1. ; Zpn = -1. ;
                break ;
            case V3d_XposYpos :
                Xpn = 1. ; Ypn = 1. ; Zpn = 0. ;
                break ;
            case V3d_XposYneg :
                Xpn = 1. ; Ypn = -1. ; Zpn = 0. ;
                break ;
            case V3d_XnegYpos :
                Xpn = -1. ; Ypn = 1. ; Zpn = 0. ;
                break ;
            case V3d_XnegYneg :
                Xpn = -1. ; Ypn = -1. ; Zpn = 0. ;
                break ;
            case V3d_XposZpos :
                Xpn = 1. ; Ypn = 0. ; Zpn = 1. ;
                break ;
            case V3d_XposZneg :
                Xpn = 1. ; Ypn = 0. ; Zpn = -1. ;
                break ;
            case V3d_XnegZpos :
                Xpn = -1. ; Ypn = 0. ; Zpn = 1. ;
                break ;
            case V3d_XnegZneg :
                Xpn = -1. ; Ypn = 0. ; Zpn = -1. ;
                break ;
            case V3d_YposZpos :
                Xpn = 0. ; Ypn = 1. ; Zpn = 1. ;
                break ;
            case V3d_YposZneg :
                Xpn = 0. ; Ypn = 1. ; Zpn = -1. ;
                break ;
            case V3d_YnegZpos :
                Xpn = 0. ; Ypn = -1. ; Zpn = 1. ;
                break ;
            case V3d_YnegZneg :
                Xpn = 0. ; Ypn = -1. ; Zpn = -1. ;
                break ;
        }
        Vec.SetCoord(Xpn,Ypn,Zpn) ; Vec.Normalize() ;
        return Vec ;
}

void V3d::ArrowOfRadius(const Handle(Graphic3d_Group)& garrow,const Standard_Real X0,const Standard_Real Y0,const Standard_Real Z0,const Standard_Real Dx,const Standard_Real Dy,const Standard_Real Dz,const Standard_Real Alpha,const Standard_Real Lng) {

Standard_Real Xc,Yc,Zc,Xi,Yi,Zi,Xj,Yj,Zj;
Standard_Real Xn,Yn,Zn,X,Y,Z,Norme;
Standard_Integer NbPoints = 10, i;
Graphic3d_Array1OfVertex VN(1,NbPoints+1);
Graphic3d_Array1OfVertex V2(1,2);

//      Centre du cercle base de la fleche :
        Xc = X0 - Dx * Lng;
        Yc = Y0 - Dy * Lng;
        Zc = Z0 - Dz * Lng;

//      Construction d'un repere i,j pour le cercle:
        Xn=0., Yn=0., Zn=0.;

        if ( Abs(Dx) <= Abs(Dy) && Abs(Dx) <= Abs(Dz)) Xn=1.;
        else if ( Abs(Dy) <= Abs(Dz) && Abs(Dy) <= Abs(Dx)) Yn=1.;
        else Zn=1.;
        Xi = Dy * Zn - Dz * Yn;
        Yi = Dz * Xn - Dx * Zn;
        Zi = Dx * Yn - Dy * Xn;

        Norme = Sqrt ( Xi*Xi + Yi*Yi + Zi*Zi );
        Xi= Xi / Norme; Yi = Yi / Norme; Zi = Zi/Norme;

        Xj = Dy * Zi - Dz * Yi;
        Yj = Dz * Xi - Dx * Zi;
        Zj = Dx * Yi - Dy * Xi;

        V2(1).SetCoord(X0,Y0,Z0);

        Standard_Real cosinus,sinus, Tg=Tan(Alpha);

        for (i = 1 ; i <= NbPoints ; i++) {

            cosinus = Cos ( 2 * Standard_PI / NbPoints * (i-1) );   
            sinus = Sin ( 2 * Standard_PI / NbPoints * (i-1) );

            X = Xc + (cosinus * Xi + sinus * Xj) * Lng * Tg;
            Y = Yc + (cosinus * Yi + sinus * Yj) * Lng * Tg;
            Z = Zc + (cosinus * Zi + sinus * Zj) * Lng * Tg;

            VN(i).SetCoord(X,Y,Z);
            if(i==1) VN(NbPoints+1).SetCoord(X,Y,Z);
            V2(2).SetCoord(X,Y,Z);
            garrow->Polyline(V2);
        }
        garrow->Polyline(VN);
}


void V3d::CircleInPlane(const Handle(Graphic3d_Group)& gcircle,const Standard_Real X0,const Standard_Real Y0,const Standard_Real Z0,const Standard_Real DX,const Standard_Real DY,const Standard_Real DZ,const Standard_Real Rayon) {

Standard_Real VX,VY,VZ,X,Y,Z,Xn,Yn,Zn,Xi,Yi,Zi,Xj,Yj,Zj,Norme;
Standard_Integer NFACES = 30 , i;
Standard_Real Alpha = 0. ;
Standard_Real Dalpha = 2.*Standard_PI/NFACES ;
Graphic3d_Array1OfVertex Points(0,NFACES);

      Norme = Sqrt ( DX*DX + DY*DY + DZ*DZ );
      if ( Norme >= 0.0001 ) {
        VX= DX/Norme; VY = DY/Norme; VZ = DZ/Norme;

//Construction of marker i,j for the circle:
        Xn=0., Yn=0., Zn=0.;   
        if ( Abs(VX) <= Abs(VY) && Abs(VX) <= Abs(VZ)) Xn=1.;
        else if ( Abs(VY) <= Abs(VZ) && Abs(VY) <= Abs(VX)) Yn=1.;
        else Zn=1.;
        Xi = VY * Zn - VZ * Yn;
        Yi = VZ * Xn - VX * Zn;
        Zi = VX * Yn - VY * Xn;
        
        Norme = Sqrt ( Xi*Xi + Yi*Yi + Zi*Zi );
        Xi= Xi / Norme; Yi = Yi / Norme; Zi = Zi/Norme;
        
        Xj = VY * Zi - VZ * Yi;
        Yj = VZ * Xi - VX * Zi;
        Zj = VX * Yi - VY * Xi;      
        
        for (i = 0 ; i <= NFACES ; i++) {
          X = X0 + (Cos(Alpha) * Xi + Sin(Alpha) * Xj) * Rayon;
          Y = Y0 + (Cos(Alpha) * Yi + Sin(Alpha) * Yj) * Rayon;
          Z = Z0 + (Cos(Alpha) * Zi + Sin(Alpha) * Zj) * Rayon;
          Alpha +=Dalpha;
          Points(i).SetCoord(X,Y,Z);
        }

        gcircle->Polyline(Points);
      }
}


void V3d::SwitchViewsinWindow(const Handle(V3d_View)& aPreviousView,
                              const Handle(V3d_View)& aNextView) {
  aPreviousView->Viewer()->SetViewOff(aPreviousView);
  if(!aNextView->IfWindow())
    aNextView->SetWindow(aPreviousView->Window());
  aNextView->Viewer()->SetViewOn(aNextView);
    
}
void V3d::DrawSphere(const Handle(V3d_Viewer)& aViewer,const Quantity_Length ray) {
  Standard_Boolean inf = ray < 0;
  Standard_Real aRadius = Abs(ray);
  Handle(Graphic3d_Structure) Struct = new Graphic3d_Structure(aViewer->Viewer()) ;
  Handle(Graphic3d_Group) Group = new Graphic3d_Group(Struct) ;

  Handle(Graphic3d_AspectLine3d) LineAttrib = new Graphic3d_AspectLine3d() ;
  LineAttrib->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
  Struct->SetPrimitivesAspect(LineAttrib) ;
  
  Standard_Integer NFACES = 30;
  Graphic3d_Array1OfVertex Points(0,NFACES) ;
  Standard_Real R,X,Y,Z ;
  Standard_Real Beta = 0. ;
  Standard_Real Alpha = 0. ;
  Standard_Real Dbeta = 2.*Standard_PI/NFACES ;
  Standard_Real Dalpha = 2.*Standard_PI/NFACES ;
  Standard_Integer i,j ;
  for( j=0 ; j<NFACES/2 ; j++ ) {
    Alpha = 0. ;
    R = aRadius*sin(Beta) ;
    Z = aRadius*cos(Beta) ;
    Beta += Dbeta ;
    for( i=0 ; i<NFACES ; i++ ) {
      X = R*cos(Alpha) ;
      Y = R*sin(Alpha) ;
      Alpha += Dalpha ;
      Points(i).SetCoord(X,Y,Z);
    }
    Points(0).Coord(X,Y,Z) ;
    Points(NFACES).SetCoord(X,Y,Z) ;
    Group->Polyline(Points) ;
  }
  for( j=0 ; j<NFACES/2 ; j++ ) {
    Alpha = 0. ;
    R = aRadius*sin(Beta) ;
    Y = aRadius*cos(Beta) ;
    Beta += Dbeta ;
    for( i=0 ; i<NFACES ; i++ ) {
      X = R*cos(Alpha) ;
      Z = R*sin(Alpha) ;
      Alpha += Dalpha ;
      Points(i).SetCoord(X,Y,Z);
    }
    Points(0).Coord(X,Y,Z) ;
    Points(NFACES).SetCoord(X,Y,Z) ;
    Group->Polyline(Points) ;
  }
  if(inf)  Struct->SetInfiniteState(Standard_True);
  Struct->Display();
  aViewer->Update();
}
void V3d::SetPlane(const Handle(V3d_Viewer)& aViewer, 
                   const Standard_Real x1,
                   const Standard_Real y1,
                   const Standard_Real z1,
                   const Standard_Real x2,
                   const Standard_Real y2,
                   const Standard_Real z2) {

  gp_Ax3 a(gp_Pnt(0.,0.,0),gp_Dir(x1,y1,z1),gp_Dir(x2,y2,z2));
  aViewer->SetPrivilegedPlane(a);

}
void V3d::PickGrid(const Handle(V3d_Viewer)& aViewer,
//                                const Quantity_Length ray) {
                                const Quantity_Length ) {
Standard_Real x1, y1, z1;
Standard_Real x2, y2, z2;
        cout << "Direction ? " << flush;
        cin >> x1; cin >> y1; cin >> z1;
        cout << "XDirection ? " << flush;
        cin >> x2; cin >> y2; cin >> z2;
Standard_Integer u, v;
        cout << "u, v ? " << flush;
        cin >> u; cin >> v;
        V3d::SetPlane (aViewer, x1, y1, z1, x2, y2, z2);

        // To restart the calculation on the new plane
        if (aViewer->Grid ()->IsActive ()) {
                Standard_Real xo, yo;
                Quantity_PlaneAngle angle;
                switch (aViewer->GridType ()) {
                        case Aspect_GT_Rectangular :
                                Standard_Real xstep, ystep;
                                aViewer->RectangularGridValues
                                        (xo, yo, xstep, ystep, angle);
                                aViewer->SetRectangularGridValues
                                        (xo, yo, xstep, ystep, angle);
                        break;
                        case Aspect_GT_Circular :
                                Standard_Real radiusstep;
                                Standard_Integer division;
                                aViewer->CircularGridValues
                                        (xo, yo, radiusstep, division, angle);
                                aViewer->SetCircularGridValues
                                        (xo, yo, radiusstep, division, angle);
                        break;
                }
        }

        for (aViewer->InitActiveViews ();
                aViewer->MoreActiveViews ();
                   aViewer->NextActiveViews()) {
Standard_Real X, Y, Z;
                aViewer->ActiveView ()->Convert (u, v, X, Y, Z);
        }
}
