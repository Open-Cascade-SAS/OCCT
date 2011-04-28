/***********************************************************************
 
     FONCTION :
     ----------
        Classe V3d_Camera :
 
     HISTORIQUE DES MODIFICATIONS   :
     --------------------------------
      00-09-92 : GG  ; Creation.
      18-06-96 : FMN ; Ajout MyGraphicStructure1 pour sauvegarder snopick
      24-12-97 : FMN ; Remplacement de math par MathGra
      31-12-97 : CAL ; Suppression de MathGra
      21-01-98 : CAL ; Window de Xw et WNT remplacee par Aspect_Window
      23-02-98 : FMN ; Remplacement PI par Standard_PI
      02.15.100 : JR : Clutter

************************************************************************/

/*----------------------------------------------------------------------*/
/*
 * Includes
 */

#include <V3d_Camera.ixx>
#include <V3d.hxx>
#include <V3d_PerspectiveView.hxx>
#include <Graphic3d_Vector.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Visual3d_ViewManager.hxx>
#include <Visual3d_ContextPick.hxx>
#include <Visual3d_PickDescriptor.hxx>
#include <Visual3d_HSequenceOfPickPath.hxx>
#include <Visual3d_PickPath.hxx>
#include <Viewer_BadValue.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax1.hxx>
#include <gp_Vec.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TCollection_AsciiString.hxx>
#include <Aspect_Window.hxx>

//-Declarations
#define DEUXPI (2.*Standard_PI)

//-Constructors

V3d_Camera::V3d_Camera(const Handle(V3d_View)& aView) {
  Standard_Real X,Y,Z;
  Graphic3d_Vertex P,T;

// The eye point defines the position of the camera
  aView->Eye(X,Y,Z);
  P.SetCoord(X,Y,Z);
  MyPosition = P;
  // The target point defines the target of the camera
  aView->At(X,Y,Z);
  T.SetCoord(X,Y,Z);
  MyTarget = T;
// Incline Angle of the camera
  MyAngle = aView->Twist();
// Aperture Angle of the camera
  MyAperture = (aView->DynamicType()==STANDARD_TYPE(V3d_PerspectiveView)) ?
    ((Handle(V3d_PerspectiveView)&)aView)->Angle() : 0.;
   
} 

//-Methods, in order

void V3d_Camera::SetPosition(const Standard_Real Xp, const Standard_Real Yp, const Standard_Real Zp) {

  MyPosition.SetCoord(Xp,Yp,Zp) ;
}

void V3d_Camera::SetAngle(const Standard_Real Angle) {

  Viewer_BadValue_Raise_if( Abs(Angle) > DEUXPI, "V3d_Camera::SetAngle, bad angle" );
  MyAngle = Angle ;
}

void V3d_Camera::SetAperture(const Standard_Real Angle) {

  Viewer_BadValue_Raise_if( Angle <= 0. || Angle >= Standard_PI ,"V3d_Camera::SetAperture, bad angle");
  MyAperture = Angle ;
}

void V3d_Camera::SetTarget(const Standard_Real X, const Standard_Real Y, const Standard_Real Z) {

  MyTarget.SetCoord(X,Y,Z);
}

void V3d_Camera::SetRadius(const Standard_Real Radius) {

  Viewer_BadValue_Raise_if( Radius <= 0., "V3d_Camera::SetRadius, bad radius");

  Standard_Real X0,Y0,Z0,Xn,Yn,Zn;

// Targer point remains unchanged, only the position of the camera is modified
// preserving the direction.
  Graphic3d_Vector D(MyTarget,MyPosition);
  D.Normalize();
  D.Coord(Xn,Yn,Zn);
  MyTarget.Coord(X0,Y0,Z0);
  Xn = X0 + Radius*Xn;
  Yn = Y0 + Radius*Yn;
  Zn = Z0 + Radius*Zn;
  MyPosition.SetCoord(Xn,Yn,Zn) ;

}

void V3d_Camera::OnHideFace(const Handle(V3d_View)& aView) {

  Standard_Real XP,YP,ZP,Rayon,X,Y,Z,VX,VY,VZ;

  Rayon = this->Radius();
  MyPosition.Coord(XP,YP,ZP);
  SymetricPointOnSphere(aView,MyTarget,MyPosition,Rayon,X,Y,Z,VX,VY,VZ);
  //      This point is visible
  if ( (VX*(X-XP) < 0.) && (VY*(Y-YP) < 0.) && (VZ*(Z-ZP) < 0.) ) {
    this->SetPosition(X,Y,Z);
  }
}

void V3d_Camera::OnSeeFace(const Handle(V3d_View)& aView) {

  Standard_Real XP,YP,ZP,Rayon,X,Y,Z,VX,VY,VZ;

  Rayon = this->Radius();
  MyPosition.Coord(XP,YP,ZP);
  SymetricPointOnSphere(aView,MyTarget,MyPosition,Rayon,X,Y,Z,VX,VY,VZ);
  //      This point is hidden
  if ( (VX*(X-XP) > 0.) && (VY*(Y-YP) > 0.) && (VZ*(Z-ZP) > 0.) ) {
    this->SetPosition(X,Y,Z);
  }
}

Standard_Boolean V3d_Camera::SeeOrHide(const Handle(V3d_View)& aView) const {

  Standard_Real XP,YP,ZP,Rayon,X,Y,Z,VX,VY,VZ;
  Standard_Boolean Val;

  Rayon = this->Radius();
  MyPosition.Coord(XP,YP,ZP);
  SymetricPointOnSphere(aView,MyTarget,MyPosition,Rayon,X,Y,Z,VX,VY,VZ);
  //      Is it a visible or a hidden point?
  if ( (VX*(X-XP) > 0.) || (VY*(Y-YP) > 0.) || (VZ*(Z-ZP) > 0.) )
//      the source is on the hidden face
    Val = Standard_False; 
  else
    //  the source is on the visible face.
    Val = Standard_True;

  return Val;

}

void V3d_Camera::Position(Standard_Real& Xp, Standard_Real& Yp, Standard_Real& Zp)const  {

  MyPosition.Coord(Xp,Yp,Zp) ;
}

Standard_Real V3d_Camera::Angle()const  {
  
  return MyAngle ;
}

Standard_Real V3d_Camera::Aperture()const  {

  return MyAperture ;
}

void V3d_Camera::Target(Standard_Real& Xp, Standard_Real& Yp, Standard_Real& Zp)const  {

//  Graphic3d_Vertex P ;
  MyTarget.Coord(Xp,Yp,Zp) ;
}

Standard_Real V3d_Camera::Radius() const {

  Standard_Real Xp,Yp,Zp,Xc,Yc,Zc,Radius;
  MyPosition.Coord(Xp,Yp,Zp);
  MyTarget.Coord(Xc,Yc,Zc);
  Radius = Square(Xc - Xp) + Square(Yc - Yp) + Square(Zc - Zp);
  Radius = Sqrt(Radius);
  return Radius ;
}

void V3d_Camera::Symbol (const Handle(Graphic3d_Group)& gsymbol,
//                         const Handle(V3d_View)& aView) const {
                         const Handle(V3d_View)& ) const {

  Standard_Real X0,Y0,Z0,XP,YP,ZP,Dx,Dy,Dz,Lng;
  Standard_Real Xc1,Yc1,Zc1,Xc2,Yc2,Zc2,Xi,Yi,Zi,Xj,Yj,Zj;
  Standard_Real Xn,Yn,Zn,X,Y,Z,Norme;
  Standard_Real cosinus,sinus;
  Standard_Real Alpha = Standard_PI/4.;
  Standard_Integer NbPoints = 4, i;
  Graphic3d_Array1OfVertex VN1(1,NbPoints+1);
  Graphic3d_Array1OfVertex VN2(1,NbPoints+1);
  Graphic3d_Array1OfVertex V2(1,2);

//      Direction of the camera
  MyPosition.Coord(XP,YP,ZP);
  MyTarget.Coord(X0,Y0,Z0);
  Dx = X0 - XP; Dy = Y0 - YP; Dz = Z0 - ZP;
  Lng = this->Radius()/10.;
  
//      Find centers of base squares of the case of the camera :
  Xc1 = XP - Dx * Lng; Yc1 = YP - Dy * Lng; Zc1 = ZP - Dz * Lng;
  Xc2 = XP + Dx * Lng; Yc2 = YP + Dy * Lng; Zc2 = ZP + Dz * Lng;
  
//      Construction of a mark i,j for rectangles :
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

//      Scheme of the case
  for (i = 1 ; i <= NbPoints ; i++) {

    cosinus = Cos ( Alpha + (i - 1) *  Standard_PI/2. );   
    sinus   = Sin ( Alpha + (i - 1) *  Standard_PI/2. );   
    
    //          First base square
    X = Xc1 + (cosinus * Xi + sinus * Xj) * Lng / 2.;
    Y = Yc1 + (cosinus * Yi + sinus * Yj) * Lng / 2.;
    Z = Zc1 + (cosinus * Zi + sinus * Zj) * Lng / 2.;
    VN1(i).SetCoord(X,Y,Z);
    if(i==1) VN1(NbPoints+1).SetCoord(X,Y,Z);
    V2(1).SetCoord(X,Y,Z);
    
//          Second base square
    X = Xc2 + (cosinus * Xi + sinus * Xj) * Lng / 2.;
    Y = Yc2 + (cosinus * Yi + sinus * Yj) * Lng / 2.;
    Z = Zc2 + (cosinus * Zi + sinus * Zj) * Lng / 2.;
    VN2(i).SetCoord(X,Y,Z);
    if(i==1) VN2(NbPoints+1).SetCoord(X,Y,Z);
    V2(2).SetCoord(X,Y,Z);

    gsymbol->Polyline(V2);
  }
  gsymbol->Polyline(VN1);
  gsymbol->Polyline(VN2);

//      Scheme of the objective
  for (i = 1 ; i <= NbPoints ; i++) {
    
    cosinus = Cos ( Alpha + (i - 1) *  Standard_PI/2. );   
    sinus   = Sin ( Alpha + (i - 1) *  Standard_PI/2. );   

    //          Premier carre de base
    X = Xc2 + (cosinus * Xi + sinus * Xj) * Lng / 6.;
    Y = Yc2 + (cosinus * Yi + sinus * Yj) * Lng / 6.;
    Z = Zc2 + (cosinus * Zi + sinus * Zj) * Lng / 6.;
    VN1(i).SetCoord(X,Y,Z);
    if(i==1) VN1(NbPoints+1).SetCoord(X,Y,Z);
    V2(1).SetCoord(X,Y,Z);

//          Second base square
    X = Xc2 + Dx * Lng / 6. + (cosinus * Xi + sinus * Xj) * Lng / 3.;
    Y = Yc2 + Dy * Lng / 6. + (cosinus * Yi + sinus * Yj) * Lng / 3.;
    Z = Zc2 + Dz * Lng / 6. + (cosinus * Zi + sinus * Zj) * Lng / 3.;
    VN2(i).SetCoord(X,Y,Z);
    if(i==1) VN2(NbPoints+1).SetCoord(X,Y,Z);
    V2(2).SetCoord(X,Y,Z);

    gsymbol->Polyline(V2);
        }
  gsymbol->Polyline(VN1);
  gsymbol->Polyline(VN2);
       
}
    
void V3d_Camera::Display( const Handle(V3d_View)& aView,
				const V3d_TypeOfRepresentation TPres) {

  Graphic3d_Array1OfVertex PRadius(0,1);
  Graphic3d_Vertex PText ;
  Standard_Real X,Y,Z,Rayon;
  Standard_Real X0,Y0,Z0,VX,VY,VZ;
  Standard_Real X1,Y1,Z1,XT,YT,ZT,PXT,PYT;
  Standard_Real DXRef,DYRef,DZRef,DXini,DYini,DZini;
  Standard_Integer IXP,IYP;
  V3d_TypeOfRepresentation Pres;
  V3d_TypeOfUpdate UpdSov;


//  Creation of a structure of markable elements (position of the
//  camera, and the sphere of displacement of this camera)

  Pres = TPres;
  Handle(V3d_Viewer) TheViewer = aView->Viewer();
  UpdSov = TheViewer->UpdateMode();
  TheViewer->SetUpdateMode(V3d_WAIT);
  if (!MyGraphicStructure.IsNull()) {
    MyGraphicStructure->Disconnect(MyGraphicStructure1);
    MyGraphicStructure->Clear();
    MyGraphicStructure1->Clear();
    if (Pres == V3d_SAMELAST) Pres = MyTypeOfRepresentation;
  }
  else {
    if (Pres == V3d_SAMELAST) Pres = V3d_SIMPLE;
    Handle(Graphic3d_Structure) scamera = new Graphic3d_Structure(TheViewer->Viewer()); 
    MyGraphicStructure = scamera;
    Handle(Graphic3d_Structure) snopick = new Graphic3d_Structure(TheViewer->Viewer()); 
    MyGraphicStructure1 = snopick;
  }
    
  Handle(Graphic3d_Group) gradius;
  Handle(Graphic3d_Group) gExtArrow;
  Handle(Graphic3d_Group) gIntArrow;
  if (Pres == V3d_COMPLETE) {
    gradius = new Graphic3d_Group(MyGraphicStructure);
    gExtArrow = new Graphic3d_Group(MyGraphicStructure);
    gIntArrow = new Graphic3d_Group(MyGraphicStructure);
  }
  Handle(Graphic3d_Group) gcamera = new Graphic3d_Group(MyGraphicStructure);
  Handle(Graphic3d_Group) gsphere;
  if (Pres == V3d_COMPLETE || Pres == V3d_PARTIAL) gsphere = new Graphic3d_Group(MyGraphicStructure);    
  
//  Creation of a structure of non-markable elements (meridian and 
//  parallel ).
  Handle(Graphic3d_Group) gnopick = new Graphic3d_Group(MyGraphicStructure1);
  MyGraphicStructure1->SetPick(Standard_False);

  MyTarget.Coord(X0,Y0,Z0);

// Display of the position of the camera.

  gcamera->SetPickId(1);
  if (Pres == V3d_SIMPLE) {
//    a viewfinder is drawn
    Graphic3d_Array1OfVertex PViseur(1,2);
    aView->Project(X0,Y0,Z0,PXT,PYT);
    aView->Convert(PXT,PYT,IXP,IYP);
    aView->Convert(IXP,IYP,XT,YT,ZT);
    aView->Convert(IXP+20,IYP,X,Y,Z);
    PViseur(1).SetCoord(X+X0-XT,Y+Y0-YT,Z+Z0-ZT);
    aView->Convert(IXP-20,IYP,X,Y,Z);
    PViseur(2).SetCoord(X+X0-XT,Y+Y0-YT,Z+Z0-ZT);
    gcamera->Polyline(PViseur);
    aView->Convert(IXP,IYP-20,X,Y,Z);
    PViseur(1).SetCoord(X+X0-XT,Y+Y0-YT,Z+Z0-ZT);
    aView->Convert(IXP,IYP+20,X,Y,Z);
    PViseur(2).SetCoord(X+X0-XT,Y+Y0-YT,Z+Z0-ZT);
    gcamera->Polyline(PViseur);
  }
//  a camera is drawn
  else this->Symbol(gcamera,aView);

// Display of the marking sphere (limited to circle).
    
  if (Pres == V3d_COMPLETE || Pres == V3d_PARTIAL) {

    Rayon = this->Radius(); 
    aView->Proj(VX,VY,VZ);
    gsphere->SetPickId(2);
    V3d::CircleInPlane(gsphere,X0,Y0,Z0,VX,VY,VZ,Rayon);
    
// Display of the radius of the sphere (line + text)

    if (Pres == V3d_COMPLETE) {
      gradius->SetPickId(3);
      gExtArrow->SetPickId(4);
      gIntArrow->SetPickId(5);
      PRadius(0).SetCoord(X0,Y0,Z0);
      MyPosition.Coord(X,Y,Z);
      PRadius(1).SetCoord(X,Y,Z);
      gnopick->Polyline(PRadius);
      V3d::ArrowOfRadius(gExtArrow,X-(X-X0)/10.,
			 Y-(Y-Y0)/10.,
			     Z-(Z-Z0)/10.,X-X0,Y-Y0,Z-Z0,Standard_PI/15.,Rayon/20.);
      V3d::ArrowOfRadius(gIntArrow,X0,Y0,Z0,X0-X,Y0-Y,Z0-Z,Standard_PI/15.,
			 Rayon/20.);
      TCollection_AsciiString ValOfRadius(Rayon);
      PText.SetCoord( (X0+X)/2., (Y0+Y)/2. , (Z0+Z)/2. );
      gradius->Text(ValOfRadius.ToCString(),PText,0.01);
    }
 
// Display of the meridian

    Quantity_Color Col2(Quantity_NOC_GREEN);
    Handle(Graphic3d_AspectLine3d) Asp2 = new Graphic3d_AspectLine3d
      (Col2,Aspect_TOL_SOLID,1.);
    gnopick->SetPrimitivesAspect(Asp2);
    
    // Definition of the axis of circle
    aView->Up(DXRef,DYRef,DZRef);
    this->Position(X,Y,Z);
    DXini = X-X0; DYini = Y-Y0; DZini = Z-Z0;
    VX = DYRef*DZini - DZRef*DYini;
    VY = DZRef*DXini - DXRef*DZini;
    VZ = DXRef*DYini - DYRef*DXini;
    
    V3d::CircleInPlane(gnopick,X0,Y0,Z0,VX,VY,VZ,Rayon);

// Display of the parallel

// Definition of the axis of circle
    aView->Proj(VX,VY,VZ);
    aView->Up(X1,Y1,Z1);
    DXRef = VY * Z1 - VZ * Y1;
    DYRef = VZ * X1 - VX * Z1;
    DZRef = VX * Y1 - VY * X1;
    this->Position(X,Y,Z);
    DXini = X-X0; DYini = Y-Y0; DZini = Z-Z0;
    VX = DYRef*DZini - DZRef*DYini;
    VY = DZRef*DXini - DXRef*DZini;
    VZ = DXRef*DYini - DYRef*DXini;
    
    V3d::CircleInPlane(gnopick,X0,Y0,Z0,VX,VY,VZ,Rayon);

  }

  MyGraphicStructure->Connect(MyGraphicStructure1,Graphic3d_TOC_DESCENDANT);
  MyTypeOfRepresentation = Pres;
  MyGraphicStructure->Display();
  TheViewer->SetUpdateMode(UpdSov);
}

void V3d_Camera::Erase() {

  if (!MyGraphicStructure.IsNull()) MyGraphicStructure->Erase();
  if (!MyGraphicStructure1.IsNull()) MyGraphicStructure1->Erase();
}

V3d_TypeOfPickCamera V3d_Camera::Pick(const Handle(V3d_View)& aView,
				      const Standard_Integer Xpix,
				      const Standard_Integer Ypix) const 
{
  
  Standard_Integer i, Lng, Id;
  Standard_Boolean kcont;
  V3d_TypeOfPickCamera TPick;
  
  Handle(Visual3d_ViewManager) VM = (aView->Viewer())->Viewer();
  Visual3d_ContextPick CTXP;
  Visual3d_PickDescriptor Pdes = VM->Pick(CTXP,aView->Window (),Xpix,Ypix);
  Visual3d_PickPath OnePPath;
  Handle(Visual3d_HSequenceOfPickPath) PPath = Pdes.PickPath();
  Lng = PPath->Length();      
  kcont = Standard_True;
  TPick = V3d_NOTHINGCAMERA;  
  for (i=1; i<=Lng && kcont; i++) {
    OnePPath = PPath->Value(i);
    Id = OnePPath.PickIdentifier();
    if (MyGraphicStructure == OnePPath.StructIdentifier()){
      switch (Id) {
      case 1 : TPick = V3d_POSITIONCAMERA;
	       break;
	     case 2 : TPick = V3d_SPACECAMERA;
	       break;
	     case 3 : TPick = V3d_RADIUSTEXTCAMERA;
	       break;
	     case 4 : TPick = V3d_ExtRADIUSCAMERA;
	       break;
	     case 5 : TPick = V3d_IntRADIUSCAMERA;
	       break;
	     }
      kcont = Standard_False;
    }
  }
  return TPick;
}

void V3d_Camera::Tracking( const Handle(V3d_View)& aView,
			  const V3d_TypeOfPickCamera WhatPick,
			  const Standard_Integer Xpix,
			  const Standard_Integer Ypix) {
  

  Standard_Real    XPp,YPp,PXT,PYT,X,Y,Z,Rayon,Ylim;
  Standard_Real    XMinTrack,XMaxTrack,YMinTrack,YMaxTrack;
  Standard_Real    XT,YT,ZT,X0,Y0,Z0,XP,YP,ZP,VX,VY,VZ,A,B,C,Delta;
  Standard_Real    DX,DY,PXP,PYP,Xproj,Yproj;
  Standard_Real    A1,A2,B1,B2,Rap,OldRprj,NewRprj;
  Standard_Real    Xi,Yi,Zi,DeltaX,DeltaY,DeltaZ,Lambda;
  Standard_Integer IPX,IPY;


  aView->Convert(Xpix,Ypix,XPp,YPp);
  MyTarget.Coord(X0,Y0,Z0);
  aView->Project(X0,Y0,Z0,PXT,PYT);
  aView->Convert(PXT,PYT,IPX,IPY);
  //      Coord 3d in the plane of projection of the target.
  aView->Convert(IPX,IPY,XT,YT,ZT);
  switch (WhatPick) {
  case V3d_POSITIONCAMERA : 
    // Coordinates should remain inside of the sphere
    Rayon = this->Radius();
    XMinTrack = PXT - Rayon;
    XMaxTrack = PXT + Rayon;
    Ylim = Sqrt( Square(Rayon) - Square(XPp - PXT) );
    YMinTrack = PYT - Ylim;
    YMaxTrack = PYT + Ylim;
    if (XPp >= XMinTrack && XPp <= XMaxTrack) {
      if (YPp >= YMinTrack && YPp <= YMaxTrack) {
	aView->ProjReferenceAxe(Xpix,Ypix,XP,YP,ZP,VX,VY,VZ);
	DeltaX = X0 - XP;
	DeltaY = Y0 - YP;
	DeltaZ = Z0 - ZP;
	
//             The point of intersection of straight lines defined by :
//              - Straight line passing by the point of projection and the eye
//                if this is a perspective, parallel to the normal of the 
//                view if there is an axonometric view.
//                position in the view is parallel to the normal of the view
//              - The distance position of the target camera cible is equal 
//                to the radius.

	A = VX*VX + VY*VY + VZ*VZ ;
	B = -2. * (VX*DeltaX + VY*DeltaY + VZ*DeltaZ);
	C = DeltaX*DeltaX + DeltaY*DeltaY + DeltaZ*DeltaZ 
	  - Rayon*Rayon ;
	Delta = B*B - 4.*A*C;
	if ( Delta >= 0 ) {
	  Lambda = (-B + Sqrt(Delta))/(2.*A);
	  X = XP + Lambda*VX;
	  Y = YP + Lambda*VY;
	  Z = ZP + Lambda*VZ;
	  MyPosition.SetCoord(X,Y,Z);
	  this->Display(aView,MyTypeOfRepresentation);
	  aView->Update();
	}
      }
      
      break;
    }
  case V3d_SPACECAMERA : 
    aView->Convert(PXT,PYT,IPX,IPY);
//               In case Xpix,Ypix corresponding to a distance , relative
//               to the translation that is planned to be done on the sphere. 
    aView->Convert(IPX+Xpix,IPY+Ypix,X,Y,Z);
    X = X+X0-XT;
    Y = Y+Y0-YT; 
    Z = Z+Z0-ZT;
    MyTarget.SetCoord(X,Y,Z);
    MyPosition.Coord(Xi,Yi,Zi);
    Xi = Xi + (X - X0);
    Yi = Yi + (Y - Y0);
    Zi = Zi + (Z - Z0);
    MyPosition.SetCoord(Xi,Yi,Zi);
    this->Display(aView,MyTypeOfRepresentation);
    aView->Update();
    break;
  
  case V3d_ExtRADIUSCAMERA :
//             It is attempted to preserve the target positioning direction of the  
//             camera ==> the point is projected on the target camera direction.
    this->Position(Xi,Yi,Zi);
    aView->Project(Xi,Yi,Zi,PXP,PYP);
    DX = PXP - PXT;
    DY = PYP - PYT;
    A1 = DY/DX ; B1 = PYT - A1*PXT;
    A2 = -DX/DY; B2 = YPp - A2*XPp;
    Xproj = (B2 - B1) / (A1 - A2);
    Yproj = A1*Xproj + B1;
    if ( (DX*(Xproj-PXT) > 0.) && (DY*(Yproj-PYT) > 0.) ) {
      OldRprj = Sqrt ( Square (PXP-PXT) + Square (PYP-PYT) );
      NewRprj = Sqrt ( Square (Xproj-PXT) + Square (Yproj-PYT) );
      Rap = NewRprj/OldRprj;
      Rayon = this->Radius();
      Rayon = Rayon * Rap;
      this->SetRadius(Rayon);
      this->Display(aView,MyTypeOfRepresentation);
      aView->Update();
    }
    break;
  case V3d_IntRADIUSCAMERA :
//               It is attempted to preserve the target positioning direction of the  
//             camera ==> the point is projected on the target camera direction.
    this->Position(Xi,Yi,Zi);
    aView->Project(Xi,Yi,Zi,PXP,PYP);
    DX = PXP - PXT;
    DY = PYP - PYT;
    A1 = DY/DX ; B1 = PYT - A1*PXT;
    A2 = -DX/DY; B2 = YPp - A2*XPp;
    Xproj = (B2 - B1) / (A1 - A2);
    Yproj = A1*Xproj + B1;
    if ( (DX*(Xproj-PXP) < 0.) && (DY*(Yproj-PYP) < 0.) ) {
      OldRprj = Sqrt ( Square (PXP-PXT) + Square (PYP-PYT) );
      NewRprj = Sqrt ( Square (Xproj-PXP) + Square (Yproj-PYP) );
      Rap = NewRprj/OldRprj;
      Rayon = this->Radius();
      Rayon = Rayon * Rap;
//                 the camera should remain at a fixed position, only the target is modified.
      Graphic3d_Vector Dir(MyPosition,MyTarget);
      Dir.Normalize();
      Dir.Coord(X,Y,Z);
      X = Xi + Rayon*X;
      Y = Yi + Rayon*Y;
      Z = Zi + Rayon*Z;
      MyTarget.SetCoord(X,Y,Z);
      this->Display(aView,MyTypeOfRepresentation);		 
      aView->Update();
    }
    break;
  case V3d_RADIUSTEXTCAMERA :
    break;
  case V3d_NOTHINGCAMERA : 
    break;
  }
}

void V3d_Camera::AerialPilot( const Handle(V3d_View)& aView,
			     const Standard_Integer Xpix,
			     const Standard_Integer Ypix) {

  Standard_Real    Xp,Yp,Zp,Xc,Yc,Zc,Xv,Yv,Zv,Xf,Yf,Zf;
  Standard_Real    VX,VY,VZ,DXH,DYH,DZH,A,B,C,PXT,PYT,X1,Y1,Z1,Dist;
  Standard_Real    Height,Width,Beta,CosBeta,SinBeta,Coef;
  Standard_Integer IPX,IPY,IHeight,IWidth;
  TColStd_Array2OfReal   MatRot(0,2,0,2);
  

  MyPosition.Coord(Xp,Yp,Zp);
  MyTarget.Coord(Xc,Yc,Zc);
  aView->At(Xv,Yv,Zv);
  aView->Project(Xv,Yv,Zv,PXT,PYT);
  aView->Convert(PXT,PYT,IPX,IPY);
//      Find the pitching ==> Xpix = IPX and Ypix inverted
//      The target point turns around an axis passing through the eye and the  
//      direction of vector belonging to the view plane  
//      and perpendicular to the vector above the view.
  if ( Ypix != IPY ) {
    aView->Size(Width,Height);
    IHeight = aView->Convert(Height);
    Beta  = ((IPY - Ypix)*Standard_PI)/(IHeight*2.);
    aView->Proj(VX,VY,VZ);
    aView->Up(DXH,DYH,DZH);
    A = VY*DZH - VZ*DYH;
    B = VZ*DXH - VX*DZH;
    C = VX*DYH - VY*DXH;
    Dist = Sqrt( A*A + B*B + C*C);
    A = A/Dist; B = B/Dist; C = C/Dist;
    CosBeta = Cos(Beta);
    SinBeta = Sin(Beta);
    Coef = 1. - CosBeta;
    MatRot(0,0) =  A * A + (1. - A * A) * CosBeta;
    MatRot(0,1) = -C * SinBeta + Coef * A * B;
    MatRot(0,2) =  B * SinBeta + Coef * A * C;
    MatRot(1,0) =  C * SinBeta + Coef * A * B; 
    MatRot(1,1) =  B * B + (1. - B * B) * CosBeta;
    MatRot(1,2) = -A * SinBeta + Coef * B * C;
    MatRot(2,0) = -B * SinBeta + Coef * A * C;
    MatRot(2,1) =  A * SinBeta + Coef * B * C;
    MatRot(2,2) =  C * C + (1. - C * C) * CosBeta;
    Xf = Xp * MatRot(0,0) + Yp * MatRot(0,1) + Zp * MatRot(0,2);
    Yf = Xp * MatRot(1,0) + Yp * MatRot(1,1) + Zp * MatRot(1,2);
    Zf = Xp * MatRot(2,0) + Yp * MatRot(2,1) + Zp * MatRot(2,2);
//        Rotation of the target point (target of the camera)
    X1 = Xc * MatRot(0,0) + Yc * MatRot(0,1) + Zc * MatRot(0,2);
    Y1 = Xc * MatRot(1,0) + Yc * MatRot(1,1) + Zc * MatRot(1,2);
    Z1 = Xc * MatRot(2,0) + Yc * MatRot(2,1) + Zc * MatRot(2,2);
    Xc = X1 + Xp - Xf ; Yc = Y1 + Yp - Yf ; Zc = Z1 + Zp - Zf;
    MyTarget.SetCoord(Xc,Yc,Zc);
  }
//      Find the rolling determined by Xpix 
  if ( Xpix != IPX ) {
    IWidth = aView->Convert(Width);
    Beta  = ((IPX - Xpix)*Standard_PI)/(IWidth*2.);
    MyAngle = MyAngle + Beta;
  }
}


void V3d_Camera::EarthPilot( const Handle(V3d_View)& aView,
			     const Standard_Integer Xpix,
			     const Standard_Integer Ypix) {
// Piloting in land mode, resembles to the car driving 
// In this case, only the target point is modified.

  Standard_Real    Xp,Yp,Zp,Xc,Yc,Zc,Xv,Yv,Zv,Xf,Yf,Zf;
  Standard_Real    VX,VY,VZ,DXH,DYH,DZH,A,B,C,PXT,PYT,X1,Y1,Z1,Dist;
  Standard_Real    Height,Width,Beta,CosBeta,SinBeta,Coef;
  Standard_Integer IPX,IPY,IHeight,IWidth;
  TColStd_Array2OfReal   MatRot(0,2,0,2);

  MyPosition.Coord(Xp,Yp,Zp);
  MyTarget.Coord(Xc,Yc,Zc);
  aView->At(Xv,Yv,Zv);
  aView->Project(Xv,Yv,Zv,PXT,PYT);
  aView->Convert(PXT,PYT,IPX,IPY);
//      Find the pitching ==> Xpix = IPX and Ypix inverted
//      The target point turns around an axis passing through the eye and the  
//      direction of vector belonging to the view plane  
//      and perpendicular to the vector above the view.
  if ( Ypix != IPY ) {
    aView->Size(Width,Height);
    IHeight = aView->Convert(Height);
    Beta  = ((IPY - Ypix)*Standard_PI)/(IHeight*2.);
    aView->Proj(VX,VY,VZ);
    aView->Up(DXH,DYH,DZH);
    A = VY*DZH - VZ*DYH;
    B = VZ*DXH - VX*DZH;
    C = VX*DYH - VY*DXH;
    Dist = Sqrt( A*A + B*B + C*C);
    A = A/Dist; B = B/Dist; C = C/Dist;
    CosBeta = Cos(Beta);
    SinBeta = Sin(Beta);
    Coef = 1. - CosBeta;
    MatRot(0,0) =  A * A + (1. - A * A) * CosBeta;
    MatRot(0,1) = -C * SinBeta + Coef * A * B;
    MatRot(0,2) =  B * SinBeta + Coef * A * C;
    MatRot(1,0) =  C * SinBeta + Coef * A * B; 
    MatRot(1,1) =  B * B + (1. - B * B) * CosBeta;
    MatRot(1,2) = -A * SinBeta + Coef * B * C;
    MatRot(2,0) = -B * SinBeta + Coef * A * C;
    MatRot(2,1) =  A * SinBeta + Coef * B * C;
    MatRot(2,2) =  C * C + (1. - C * C) * CosBeta;
    Xf = Xp * MatRot(0,0) + Yp * MatRot(0,1) + Zp * MatRot(0,2);
    Yf = Xp * MatRot(1,0) + Yp * MatRot(1,1) + Zp * MatRot(1,2);
    Zf = Xp * MatRot(2,0) + Yp * MatRot(2,1) + Zp * MatRot(2,2);
//        Rotation of the target point (target of the camera)
    X1 = Xc * MatRot(0,0) + Yc * MatRot(0,1) + Zc * MatRot(0,2);
    Y1 = Xc * MatRot(1,0) + Yc * MatRot(1,1) + Zc * MatRot(1,2);
    Z1 = Xc * MatRot(2,0) + Yc * MatRot(2,1) + Zc * MatRot(2,2);
	  Xc = X1 + Xp - Xf ; Yc = Y1 + Yp - Yf ; Zc = Z1 + Zp - Zf;
  }
//      Find turning ==> Ypix = IPY . The target point rotates around  
//      an axis // a vector above passing through the eye
  if ( Xpix != IPX ) {
    IWidth = aView->Convert(Width);
    Beta  = ((IPX - Xpix)*Standard_PI)/(IWidth*2.);
    aView->Up(A,B,C);
    Dist = Sqrt( A*A + B*B + C*C);
    A = A/Dist; B = B/Dist; C = C/Dist;
    CosBeta = Cos(Beta);
    SinBeta = Sin(Beta);
    Coef = 1. - CosBeta;
    MatRot(0,0) =  A * A + (1. - A * A) * CosBeta;
    MatRot(0,1) = -C * SinBeta + Coef * A * B;
    MatRot(0,2) =  B * SinBeta + Coef * A * C;
    MatRot(1,0) =  C * SinBeta + Coef * A * B; 
    MatRot(1,1) =  B * B + (1. - B * B) * CosBeta;
    MatRot(1,2) = -A * SinBeta + Coef * B * C;
    MatRot(2,0) = -B * SinBeta + Coef * A * C;
    MatRot(2,1) =  A * SinBeta + Coef * B * C;
    MatRot(2,2) =  C * C + (1. - C * C) * CosBeta;
    Xf = Xp * MatRot(0,0) + Yp * MatRot(0,1) + Zp * MatRot(0,2);
    Yf = Xp * MatRot(1,0) + Yp * MatRot(1,1) + Zp * MatRot(1,2);
    Zf = Xp * MatRot(2,0) + Yp * MatRot(2,1) + Zp * MatRot(2,2);
    //        Rotation of the target point (target of the camera)
    X1 = Xc * MatRot(0,0) + Yc * MatRot(0,1) + Zc * MatRot(0,2);
    Y1 = Xc * MatRot(1,0) + Yc * MatRot(1,1) + Zc * MatRot(1,2);
    Z1 = Xc * MatRot(2,0) + Yc * MatRot(2,1) + Zc * MatRot(2,2);
    Xc = X1 + Xp - Xf ; Yc = Y1 + Yp - Yf ; Zc = Z1 + Zp - Zf;
  }
  MyTarget.SetCoord(Xc,Yc,Zc);
}

void V3d_Camera::Move (const Standard_Real Dist) {
// Displacement of the camera by preserving the direction camera - target.

  Standard_Real XP,YP,ZP,X0,Y0,Z0,DX,DY,DZ,Norme;

  MyPosition.Coord(XP,YP,ZP);
  MyTarget.Coord(X0,Y0,Z0);
  DX = X0 - XP; DY = Y0 - YP; DZ = Z0 - ZP;
  Norme = Sqrt(DX*DX + DY*DY + DZ*DZ);
  DX = Dist*DX/Norme; DY = Dist*DY/Norme; DZ = Dist*DZ/Norme;
  XP = XP + DX; YP = YP + DY; ZP = ZP + DZ; 
  X0 = X0 + DX; Y0 = Y0 + DY; Z0 = Z0 + DZ; 
  MyPosition.SetCoord(XP,YP,ZP);
  MyTarget.SetCoord(X0,Y0,Z0);
}

void V3d_Camera::GoUp (const Standard_Real Haut) {

// Displacement of the camera by axis z, preserving the direction camera -
// target of the camera

  MyPosition.SetZCoord(MyPosition.Z()+Haut);
  MyTarget.SetZCoord(MyTarget.Z()+Haut);
}

void V3d_Camera::SymetricPointOnSphere (const Handle(V3d_View)& aView, const Graphic3d_Vertex &Center, const Graphic3d_Vertex &aPoint, const Standard_Real Rayon, Standard_Real& X, Standard_Real& Y, Standard_Real& Z, Standard_Real& VX, Standard_Real& VY, Standard_Real& VZ ) {

  Standard_Real X0,Y0,Z0,XP,YP,ZP;
  Standard_Real PXP,PYP,DeltaX,DeltaY,DeltaZ;
  Standard_Real A,B,C,Delta,Lambda;
  Standard_Integer IPX,IPY;

  Center.Coord(X0,Y0,Z0);
  aPoint.Coord(XP,YP,ZP);
  aView->Project(XP,YP,ZP,PXP,PYP);
  aView->Convert(PXP,PYP,IPX,IPY);
  aView->ProjReferenceAxe(IPX,IPY,X,Y,Z,VX,VY,VZ);
  DeltaX = X0 - XP;
  DeltaY = Y0 - YP;
  DeltaZ = Z0 - ZP;

//             The point of intersection of straight lines defined by :
//              - Straight line passing by the point of projection and the eye
//                if this is a perspective, parallel to the normal of the 
//                view if there is an axonometric view.
//                position in the view is parallel to the normal of the view
//              - The distance position of the target camera cible is equal 
//                to the radius.

  A = VX*VX + VY*VY + VZ*VZ ;
  B = -2. * (VX*DeltaX + VY*DeltaY + VZ*DeltaZ);
  C = DeltaX*DeltaX + DeltaY*DeltaY + DeltaZ*DeltaZ 
    - Rayon*Rayon ;
  Delta = B*B - 4.*A*C;
  if ( Delta >= 0 ) {
    Lambda = (-B + Sqrt(Delta))/(2.*A);
    if ( Lambda >= -0.0001 && Lambda <= 0.0001 ) 
      Lambda = (-B - Sqrt(Delta))/(2.*A);
    X = XP + Lambda*VX;
    Y = YP + Lambda*VY;
    Z = ZP + Lambda*VZ;
  }
  else {
    X = XP; Y = YP; Z = ZP;
  }
}
