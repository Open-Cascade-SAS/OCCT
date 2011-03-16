/***********************************************************************
 
  V3d_PositionLight.cxx
  Created:    30-03-98  ZOV (ZELENKOV Oleg)

************************************************************************/

#include <V3d.hxx>
#include <V3d_PositionLight.ixx>
#include <Graphic3d_Vector.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <V3d_SpotLight.hxx>
#include <Visual3d_Light.hxx>
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
#include <TCollection_AsciiString.hxx>
#ifdef WNT
# include <WNT_Window.hxx>
#else
# include <Xw_Window.hxx>
#endif

V3d_PositionLight::V3d_PositionLight(const Handle(V3d_Viewer)& VM) : V3d_Light(VM) {
}

void V3d_PositionLight::SetTarget(const Standard_Real X, const Standard_Real Y, const Standard_Real Z) {

  Standard_Real Xc,Yc,Zc, Xp,Yp,Zp;
  
  // Recalcul de la position
  MyTarget.Coord(Xc,Yc,Zc);
  Position (Xp,Yp,Zp) ;

  Xp = Xp + (X - Xc);
  Yp = Yp + (Y - Yc);
  Zp = Zp + (Z - Zc);

  // Affectation
  MyTarget.SetCoord(X,Y,Z);
  SetPosition(Xp,Yp,Zp) ;
}

void V3d_PositionLight::SetRadius(const Standard_Real Radius) {

  Viewer_BadValue_Raise_if( Radius <= 0. , "V3d_PositionLight::SetRadius, bad radius");
  Viewer_BadValue_Raise_if( MyType == V3d_DIRECTIONAL , "V3d_PositionLight::SetRadius, bad light type");

  Standard_Real X0,Y0,Z0, Xn,Yn,Zn, Xp,Yp,Zp;
  
  // Le point cible reste inchange, seul la position de la lumiere est modifiee
  // en conservant la direction.
	Position (Xp,Yp,Zp);
  Graphic3d_Vector  D(MyTarget, Graphic3d_Vertex(Xp, Yp, Zp));
  D.Normalize();
  D.Coord(Xn,Yn,Zn);
  MyTarget.Coord(X0,Y0,Z0);
  Xn = X0 + Radius*Xn;
  Yn = Y0 + Radius*Yn;
  Zn = Z0 + Radius*Zn;

  SetPosition(Xn,Yn,Zn) ;
}

void V3d_PositionLight::OnHideFace(const Handle(V3d_View)& aView) {

  Standard_Real Xp,Yp,Zp, X,Y,Z, VX,VY,VZ;
  
  Position (Xp,Yp,Zp);
	V3d_Light::SymetricPointOnSphere (aView, 
		MyTarget, Graphic3d_Vertex(Xp,Yp,Yp), Radius(), X,Y,Z, VX,VY,VZ);

  // Actuellement est on sur le point vu
  if ((VX*(X-Xp) < 0.) && (VY*(Y-Yp) < 0.) && (VZ*(Z-Zp) < 0.))
    SetPosition (X,Y,Z);
}

void V3d_PositionLight::OnSeeFace(const Handle(V3d_View)& aView) {

  Standard_Real Xp,Yp,Zp, X,Y,Z, VX,VY,VZ;
  
  Position (Xp,Yp,Zp);
	V3d_Light::SymetricPointOnSphere (aView, 
		MyTarget, Graphic3d_Vertex(Xp,Yp,Yp), Radius(), X,Y,Z, VX,VY,VZ);

  // Actuellement est on sur le point cache
  if ((VX*(X-Xp) > 0.) && (VY*(Y-Yp) > 0.) && (VZ*(Z-Zp) > 0.))
    SetPosition (X,Y,Z);
}

Standard_Boolean V3d_PositionLight::SeeOrHide(const Handle(V3d_View)& aView) const {

  Standard_Real Xp,Yp,Zp, X,Y,Z, VX,VY,VZ;
  
  Position (Xp,Yp,Zp);
	V3d_Light::SymetricPointOnSphere (aView, 
		MyTarget, Graphic3d_Vertex(Xp,Yp,Yp), Radius(), X,Y,Z, VX,VY,VZ);

  // Avons nous le point cache ou le point vu
  return ( (VX*(X-Xp) > 0.) || (VY*(Y-Yp) > 0.) || (VZ*(Z-Zp) > 0.) )?
    // la source est sur la face cachee
    Standard_False:
    // la source est sur la face vue.
    Standard_True;
}

void V3d_PositionLight::Target(Standard_Real& Xp, Standard_Real& Yp, Standard_Real& Zp)const  {
  
  MyTarget.Coord(Xp,Yp,Zp) ;
}

void V3d_PositionLight::Display( const Handle(V3d_View)& aView,
                                    const V3d_TypeOfRepresentation TPres ) {

  Graphic3d_Array1OfVertex PRadius(0,1);
  Graphic3d_Vertex PText ;
  Standard_Real X,Y,Z,Rayon;
  Standard_Real X0,Y0,Z0,VX,VY,VZ;
  Standard_Real X1,Y1,Z1;
  Standard_Real DXRef,DYRef,DZRef,DXini,DYini,DZini;
  Standard_Real R1,G1,B1;
  V3d_TypeOfRepresentation Pres;
  V3d_TypeOfUpdate UpdSov;

//  Creation d'une structure slight d'elements reperables (la position de
//  la light, et le domaine d'eclairage represente par un cercle)
//  Creation d'une structure snopick d'elements non reperables ( cible, meridien et 
//  parallele ).

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
      Handle(Graphic3d_Structure) slight = new Graphic3d_Structure(TheViewer->Viewer());
      MyGraphicStructure = slight;
      Handle(Graphic3d_Structure) snopick = new Graphic3d_Structure(TheViewer->Viewer()); 
      MyGraphicStructure1 = snopick;
    }

		
	Handle(Graphic3d_Group) gradius;
	Handle(Graphic3d_Group) gExtArrow;
	Handle(Graphic3d_Group) gIntArrow;

	if (MyType != V3d_DIRECTIONAL) {
		if (Pres == V3d_COMPLETE) {
			gradius = new Graphic3d_Group(MyGraphicStructure);
			gExtArrow = new Graphic3d_Group(MyGraphicStructure);
			gIntArrow = new Graphic3d_Group(MyGraphicStructure);
		}
	}
  Handle(Graphic3d_Group) glight  = new Graphic3d_Group(MyGraphicStructure);
  Handle(Graphic3d_Group) gsphere;
  if (Pres == V3d_COMPLETE || Pres == V3d_PARTIAL) gsphere = new Graphic3d_Group(MyGraphicStructure);
  
  Handle(Graphic3d_Group) gnopick = new Graphic3d_Group(MyGraphicStructure1);
  MyGraphicStructure1->SetPick(Standard_False);
  
  X0 = MyTarget.X();
  Y0 = MyTarget.Y();
  Z0 = MyTarget.Z();
  
//Affichage de la position de la light.

  glight->SetPickId(1);
  this->Color(Quantity_TOC_RGB,R1,G1,B1);
  Quantity_Color Col1(R1,G1,B1,Quantity_TOC_RGB);
  Handle(Graphic3d_AspectLine3d) Asp1 = new Graphic3d_AspectLine3d();
  Asp1->SetColor(Col1);
  glight->SetPrimitivesAspect(Asp1);
  this->Symbol(glight,aView);

//Affichage de la sphere de reperage (limite au cercle).

  if (Pres == V3d_COMPLETE || Pres == V3d_PARTIAL) {
      
    Rayon = this->Radius();
    aView->Proj(VX,VY,VZ);
    gsphere->SetPickId(2);
    V3d::CircleInPlane(gsphere,X0,Y0,Z0,VX,VY,VZ,Rayon);


		if (MyType != V3d_DIRECTIONAL) {

      //Affichage du rayon de la sphere (ligne + texte)

			if (Pres == V3d_COMPLETE) {
				gradius->SetPickId(3);
				gExtArrow->SetPickId(4);
				gIntArrow->SetPickId(5);
				PRadius(0).SetCoord(X0,Y0,Z0);
				this->Position(X,Y,Z);
				PRadius(1).SetCoord(X,Y,Z);
				gnopick->Polyline(PRadius);
				V3d::ArrowOfRadius(gExtArrow,X-(X-X0)/10.,
				 Y-(Y-Y0)/10.,
				 Z-(Z-Z0)/10.,X-X0,Y-Y0,Z-Z0,PI/15.,Rayon/20.);
				V3d::ArrowOfRadius(gIntArrow,X0,Y0,Z0,X0-X,Y0-Y,Z0-Z,PI/15.,Rayon/20.);
				TCollection_AsciiString ValOfRadius(Rayon);
				PText.SetCoord( (X0+X)/2., (Y0+Y)/2. , (Z0+Z)/2. );
				gradius->Text(ValOfRadius.ToCString(),PText,0.01);
			}
		}


    //Affichage du meridien

    Quantity_Color Col2(Quantity_NOC_GREEN);
    Handle(Graphic3d_AspectLine3d) Asp2 = new Graphic3d_AspectLine3d
      (Col2,Aspect_TOL_SOLID,1.);
    gnopick->SetPrimitivesAspect(Asp2);
    
    // Definition de l'axe du cercle
    aView->Up(DXRef,DYRef,DZRef);
    this->Position(X,Y,Z);
    DXini = X-X0; DYini = Y-Y0; DZini = Z-Z0;
    VX = DYRef*DZini - DZRef*DYini;
    VY = DZRef*DXini - DXRef*DZini;
    VZ = DXRef*DYini - DYRef*DXini;
    
    V3d::CircleInPlane(gnopick,X0,Y0,Z0,VX,VY,VZ,Rayon);
      
    // Affichage de la parallele

    // Definition de l'axe du cercle
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

V3d_TypeOfPickLight V3d_PositionLight::Pick(const Handle(V3d_View)& aView,
					      const Standard_Integer Xpix,
					      const Standard_Integer Ypix) 
const {
  
  Standard_Integer i, Lng, Id;
  Standard_Boolean kcont;
  V3d_TypeOfPickLight TPick;
  
  Handle(Visual3d_ViewManager) VM = (aView->Viewer())->Viewer();
#ifdef WNT
  Handle( WNT_Window ) WW = Handle( WNT_Window ) :: DownCast (  aView -> Window ()  );
#else
  Handle( Xw_Window  ) WW = Handle( Xw_Window  ) :: DownCast (  aView -> Window ()  );
#endif
  Visual3d_ContextPick CTXP;
  Visual3d_PickDescriptor Pdes = VM->Pick(CTXP,WW,Xpix,Ypix);
  Visual3d_PickPath OnePPath;
  Handle(Visual3d_HSequenceOfPickPath) PPath = Pdes.PickPath();
  Lng = PPath->Length();      
  kcont = Standard_True;
  TPick = V3d_NOTHING;  
  for (i=1; i<=Lng && kcont; i++) {
    OnePPath = PPath->Value(i);
    Id = OnePPath.PickIdentifier();
    if (MyGraphicStructure == OnePPath.StructIdentifier()){
      switch (Id) {
      case 1 : 
	TPick = V3d_POSITIONLIGHT;
	break;
      case 2 : 
	TPick = V3d_SPACELIGHT;
	break;
      case 3 :
	if (MyType != V3d_DIRECTIONAL)
	  TPick = V3d_RADIUSTEXTLIGHT;
	break;
      case 4 : 
	if (MyType != V3d_DIRECTIONAL)
  	TPick = V3d_ExtRADIUSLIGHT;
	break;
      case 5 : 
	if (MyType != V3d_DIRECTIONAL)
  	TPick = V3d_IntRADIUSLIGHT;
	break;
      }
      kcont = Standard_False;
    }
  }
  return TPick;
}

void V3d_PositionLight::Tracking( const Handle(V3d_View)& aView,
			  const V3d_TypeOfPickLight WhatPick,
			  const Standard_Integer Xpix,
			  const Standard_Integer Ypix) {

//  Quantity_Color   Col ;
  Standard_Real    xPos, yPos, zPos;
  Standard_Real    XPp,YPp,PXT,PYT,X,Y,Z,Rayon,Ylim;
  Standard_Real    XMinTrack,XMaxTrack,YMinTrack,YMaxTrack;
  Standard_Real    XT,YT,ZT,X0,Y0,Z0,XP,YP,ZP,VX,VY,VZ,A,B,C,Delta;
  Standard_Real    DX,DY,PXP,PYP,Xproj,Yproj;
  Standard_Real    A1,A2,B1,B2,Rap,OldRprj,NewRprj;
  Standard_Real    Xi,Yi,Zi,DeltaX,DeltaY,DeltaZ,Lambda;
  Standard_Integer IPX,IPY;
  
  aView->Convert(Xpix,Ypix,XPp,YPp);
  X0 = MyTarget.X();
  Y0 = MyTarget.Y();
  Z0 = MyTarget.Z();
  aView->Project(X0,Y0,Z0,PXT,PYT);
  aView->Convert(PXT,PYT,IPX,IPY);
//      Coord 3d dans le plan de projection de la cible.
  aView->Convert(IPX,IPY,XT,YT,ZT);
  switch (WhatPick) {
  case V3d_POSITIONLIGHT :
	  // Les Coordonnees doivent rester a l'interieur de la sphere
    Rayon = Radius();
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
	
//                   On recherche le point d'intersection des droites definies
//                   par :
//                   - Droite passant par le point de projection et l'oeil
//                     si on est en perspective, parralele a la normale de la 
//                     vue si on a une vue axonometrique.
//                     position dans la vue est // a la normale de la vue
//                   - La distance position de la camera cible est egale au 
//                     rayon.

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
	  SetPosition(X,Y,Z);

		if (MyType == V3d_SPOT)
	    ((V3d_SpotLight*)this)->SetDirection(X0-X,Y0-Y,Z0-Z);

	  Display(aView,MyTypeOfRepresentation);
	  (aView->Viewer())->UpdateLights();
	}
      }
    }
    break;

  case V3d_SPACELIGHT :
    aView->Convert(PXT,PYT,IPX,IPY);
//               Dans ce cas Xpix,Ypix correspondent a une distance , relative
//               a la translation que l'on veut effectuer sur la sphere. 
    aView->Convert(IPX+Xpix,IPY+Ypix,X,Y,Z);
    X = X+X0-XT;
    Y = Y+Y0-YT; 
    Z = Z+Z0-ZT;
    SetTarget(X,Y,Z);
    Display(aView,MyTypeOfRepresentation);
    (aView->Viewer())->UpdateLights();
    break;

  case V3d_ExtRADIUSLIGHT :
		if (MyType == V3d_DIRECTIONAL)
			break;
//             on cherche a conserver la direction cible positionnement de la 
//             source ==> on projette le point sur la direction cible source.
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
      Rayon = Radius();
      Rayon = Rayon * Rap;
      SetRadius(Rayon);
      Display(aView,MyTypeOfRepresentation);
      (aView->Viewer())->UpdateLights();
    }
    break;

  case V3d_IntRADIUSLIGHT :
		if (MyType == V3d_DIRECTIONAL)
			break;
//               on cherche a conserver la direction cible positionnement 
//               de la source ==> on projette le point sur la direction 
//               cible source.
    Position(Xi,Yi,Zi);
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
      Rayon = Radius();
      Rayon = Rayon * Rap;
//                 la source doit rester a une position fixe, seule la cible
//                 est modifie.
      Position (xPos, yPos, zPos);
      Graphic3d_Vector  Dir(Graphic3d_Vertex(xPos,yPos,zPos), MyTarget);
      Dir.Normalize();
      Dir.Coord(X,Y,Z);
      X = Xi + Rayon*X;
      Y = Yi + Rayon*Y;
      Z = Zi + Rayon*Z;
//                 la source doit rester a une position fixe, seule la cible
//                 est modifie.
      MyTarget.SetCoord(X,Y,Z);
      Display(aView,MyTypeOfRepresentation);		 
      (aView->Viewer())->UpdateLights();
    }
		break;

  case V3d_RADIUSTEXTLIGHT :
    break;

  case V3d_NOTHING : 
    break;
  }
}

Standard_Real V3d_PositionLight::Radius() const {

  Standard_Real  Xp,Yp,Zp, Xc,Yc,Zc;
  
  Position (Xp,Yp,Zp);
  MyTarget.Coord(Xc,Yc,Zc);

  return Sqrt (Square(Xc - Xp) + Square(Yc - Yp) + Square(Zc - Zp));
}

void V3d_PositionLight::Erase() {

  if (!MyGraphicStructure.IsNull()) MyGraphicStructure->Erase();
  if (!MyGraphicStructure1.IsNull()) MyGraphicStructure1->Erase();
}

