#include <BndLib.ixx> // BUG BUG BUG pas .hxx

#include <ElCLib.hxx>
#include <gp_XYZ.hxx>
#include <gp_XY.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <BndLib_Compute.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>

   
static void OpenMin(const gp_Dir& V,Bnd_Box& B) {
  gp_Dir OX(1.,0.,0.);
  gp_Dir OY(0.,1.,0.);
  gp_Dir OZ(0.,0.,1.);
  if (V.IsParallel(OX,Precision::Angular())) 
    B.OpenXmin();
  else if (V.IsParallel(OY,Precision::Angular())) 
    B.OpenYmin();
  else if (V.IsParallel(OZ,Precision::Angular())) 
    B.OpenZmin();
  else {
    B.OpenXmin();B.OpenYmin();B.OpenZmin();
  }
}

static void OpenMax(const gp_Dir& V,Bnd_Box& B) {
  gp_Dir OX(1.,0.,0.);
  gp_Dir OY(0.,1.,0.);
  gp_Dir OZ(0.,0.,1.);
  if (V.IsParallel(OX,Precision::Angular())) 
    B.OpenXmax();
  else if (V.IsParallel(OY,Precision::Angular())) 
    B.OpenYmax();
  else if (V.IsParallel(OZ,Precision::Angular())) 
    B.OpenZmax();
  else {
    B.OpenXmax();B.OpenYmax();B.OpenZmax();
  }
}

static void OpenMinMax(const gp_Dir& V,Bnd_Box& B) {
  gp_Dir OX(1.,0.,0.);
  gp_Dir OY(0.,1.,0.);
  gp_Dir OZ(0.,0.,1.);
  if (V.IsParallel(OX,Precision::Angular())) {
    B.OpenXmax();B.OpenXmin();
  }
  else if (V.IsParallel(OY,Precision::Angular())) {
    B.OpenYmax();B.OpenYmin();
  }
  else if (V.IsParallel(OZ,Precision::Angular())) {
    B.OpenZmax();B.OpenZmin();
  }
  else {
    B.OpenXmin();B.OpenYmin();B.OpenZmin();
    B.OpenXmax();B.OpenYmax();B.OpenZmax();
  }
}

static void OpenMin(const gp_Dir2d& V,Bnd_Box2d& B) {
  gp_Dir2d OX(1.,0.);
  gp_Dir2d OY(0.,1.);
  if (V.IsParallel(OX,Precision::Angular())) 
    B.OpenXmin();
  else if (V.IsParallel(OY,Precision::Angular())) 
    B.OpenYmin();
  else {
    B.OpenXmin();B.OpenYmin();
  }
}

static void OpenMax(const gp_Dir2d& V,Bnd_Box2d& B) {
  gp_Dir2d OX(1.,0.);
  gp_Dir2d OY(0.,1.);
  if (V.IsParallel(OX,Precision::Angular())) 
    B.OpenXmax();
  else if (V.IsParallel(OY,Precision::Angular())) 
    B.OpenYmax();
  else {
    B.OpenXmax();B.OpenYmax();
  }
}

static void OpenMinMax(const gp_Dir2d& V,Bnd_Box2d& B) {
  gp_Dir2d OX(1.,0.);
  gp_Dir2d OY(0.,1.);
  if (V.IsParallel(OX,Precision::Angular())) {
    B.OpenXmax();B.OpenXmin();
  }
  else if (V.IsParallel(OY,Precision::Angular())) {
    B.OpenYmax();B.OpenYmin();
  }
  else {
    B.OpenXmin();B.OpenYmin();
    B.OpenXmax();B.OpenYmax();
  }
}


void BndLib::Add( const gp_Lin& L,const Standard_Real P1,
		 const Standard_Real P2,
		 const Standard_Real Tol, Bnd_Box& B) {

  if (Precision::IsNegativeInfinite(P1)) {
    if (Precision::IsNegativeInfinite(P2)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else if (Precision::IsPositiveInfinite(P2)) {
      OpenMinMax(L.Direction(),B);
      B.Add(ElCLib::Value(0.,L));
    }
    else {
      OpenMin(L.Direction(),B);
      B.Add(ElCLib::Value(P2,L));
    }
  }
  else if (Precision::IsPositiveInfinite(P1)) {
    if (Precision::IsNegativeInfinite(P2)) {
      OpenMinMax(L.Direction(),B);
      B.Add(ElCLib::Value(0.,L));
    }
    else if (Precision::IsPositiveInfinite(P2)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else {
      OpenMax(L.Direction(),B);
      B.Add(ElCLib::Value(P2,L));
    }
  }
  else  {
    B.Add(ElCLib::Value(P1,L));
    if (Precision::IsNegativeInfinite(P2)) {
      OpenMin(L.Direction(),B);
    }
    else if (Precision::IsPositiveInfinite(P2)){
      OpenMax(L.Direction(),B);
    }
    else {
      B.Add(ElCLib::Value(P2,L));
    }
  }
  B.Enlarge(Tol);
}

void BndLib::Add( const gp_Lin2d& L,const Standard_Real P1,
		 const Standard_Real P2,
		 const Standard_Real Tol, Bnd_Box2d& B) {

  if (Precision::IsNegativeInfinite(P1)) {
    if (Precision::IsNegativeInfinite(P2)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else if (Precision::IsPositiveInfinite(P2)) {
      OpenMinMax(L.Direction(),B);
      B.Add(ElCLib::Value(0.,L));
    }
    else {
      OpenMin(L.Direction(),B);
      B.Add(ElCLib::Value(P2,L));
    }
  }
  else if (Precision::IsPositiveInfinite(P1)) {
    if (Precision::IsNegativeInfinite(P2)) {
      OpenMinMax(L.Direction(),B);
      B.Add(ElCLib::Value(0.,L));
    }
    else if (Precision::IsPositiveInfinite(P2)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else {
      OpenMax(L.Direction(),B);
      B.Add(ElCLib::Value(P2,L));
    }
  }
  else  {
    B.Add(ElCLib::Value(P1,L));
    if (Precision::IsNegativeInfinite(P2)) {
      OpenMin(L.Direction(),B);
    }
    else if (Precision::IsPositiveInfinite(P2)){
      OpenMax(L.Direction(),B);
    }
    else {
      B.Add(ElCLib::Value(P2,L));
    }
  }
  B.Enlarge(Tol);
}

void BndLib::Add( const gp_Circ& C,const Standard_Real Tol, Bnd_Box& B) {

  Standard_Real R = C.Radius();
  gp_XYZ O  = C.Location().XYZ();
  gp_XYZ Xd = C.XAxis().Direction().XYZ();
  gp_XYZ Yd = C.YAxis().Direction().XYZ();
  B.Add(gp_Pnt(O -R*Xd -R*Yd));
  B.Add(gp_Pnt(O -R*Xd +R*Yd));
  B.Add(gp_Pnt(O +R*Xd -R*Yd));
  B.Add(gp_Pnt(O +R*Xd +R*Yd));
  B.Enlarge(Tol);
}

void BndLib::Add( const gp_Circ& C,const Standard_Real P1,
		const Standard_Real P2,
		const Standard_Real Tol, Bnd_Box& B) {

  Compute(P1,P2,C.Radius(),C.Radius(),C.XAxis().Direction().XYZ(), 
	    C.YAxis().Direction().XYZ(),C.Location().XYZ(),B);
  B.Enlarge(Tol);
}

void BndLib::Add( const gp_Circ2d& C,const Standard_Real Tol, Bnd_Box2d& B) {

  Standard_Real R = C.Radius();
  gp_XY O  = C.Location().XY();
  gp_XY Xd = C.XAxis().Direction().XY();
  gp_XY Yd = C.YAxis().Direction().XY();
  B.Add(gp_Pnt2d(O -R*Xd -R*Yd));
  B.Add(gp_Pnt2d(O -R*Xd +R*Yd));
  B.Add(gp_Pnt2d(O +R*Xd -R*Yd));
  B.Add(gp_Pnt2d(O +R*Xd +R*Yd));
  B.Enlarge(Tol);
}

void BndLib::Add(const gp_Circ2d& C,const Standard_Real P1,
		 const Standard_Real P2,
		 const Standard_Real Tol, Bnd_Box2d& B) {

  Compute(P1,P2,C.Radius(),C.Radius(),C.XAxis().Direction().XY(),
	    C.YAxis().Direction().XY(),C.Location().XY(),B);
  B.Enlarge(Tol);
}

void BndLib::Add( const gp_Elips& C,const Standard_Real Tol, Bnd_Box& B) {

  Standard_Real Ra = C.MajorRadius();
  Standard_Real Rb = C.MinorRadius();
  gp_XYZ Xd = C.XAxis().Direction().XYZ();
  gp_XYZ Yd = C.YAxis().Direction().XYZ();
  gp_XYZ O  = C.Location().XYZ();
  B.Add(gp_Pnt(O +Ra*Xd +Rb*Yd));
  B.Add(gp_Pnt(O -Ra*Xd +Rb*Yd));
  B.Add(gp_Pnt(O -Ra*Xd -Rb*Yd));
  B.Add(gp_Pnt(O +Ra*Xd -Rb*Yd));
  B.Enlarge(Tol);
}

void BndLib::Add( const gp_Elips& C,const Standard_Real P1,
		 const Standard_Real P2,
		 const Standard_Real Tol, Bnd_Box& B) {

  Compute(P1,P2,C.MajorRadius(),C.MinorRadius(),C.XAxis().Direction().XYZ(), 
	    C.YAxis().Direction().XYZ(),C.Location().XYZ(),B);
  B.Enlarge(Tol);
}

void BndLib::Add( const gp_Elips2d& C,const Standard_Real Tol, Bnd_Box2d& B) {

  Standard_Real Ra= C.MajorRadius();
  Standard_Real Rb= C.MinorRadius();
  gp_XY Xd = C.XAxis().Direction().XY();
  gp_XY Yd = C.YAxis().Direction().XY();
  gp_XY O  = C.Location().XY();
  B.Add(gp_Pnt2d(O +Ra*Xd +Rb*Yd));
  B.Add(gp_Pnt2d(O -Ra*Xd +Rb*Yd));
  B.Add(gp_Pnt2d(O -Ra*Xd -Rb*Yd));
  B.Add(gp_Pnt2d(O +Ra*Xd -Rb*Yd));
  B.Enlarge(Tol);
}

void BndLib::Add( const gp_Elips2d& C,const Standard_Real P1,
		 const Standard_Real P2,
		 const Standard_Real Tol, Bnd_Box2d& B) {

  Compute(P1,P2,C.MajorRadius(),C.MinorRadius(),
	  C.XAxis().Direction().XY(),
	  C.YAxis().Direction().XY(),C.Location().XY(),B);
  B.Enlarge(Tol);
}

void BndLib::Add( const gp_Parab& P,const Standard_Real P1,
		 const Standard_Real P2,
		 const Standard_Real Tol, Bnd_Box& B) {

  if (Precision::IsNegativeInfinite(P1)) {
    if (Precision::IsNegativeInfinite(P2)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else if (Precision::IsPositiveInfinite(P2)) {
      B.OpenXmax();B.OpenYmax();B.OpenZmax();
    }
    else {
      B.Add(ElCLib::Value(P2,P));
    }
    B.OpenXmin();B.OpenYmin();B.OpenZmin();
  }
  else if (Precision::IsPositiveInfinite(P1)) {
    if (Precision::IsNegativeInfinite(P2)) {
      B.OpenXmin();B.OpenYmin();B.OpenZmin();
    }
    else if (Precision::IsPositiveInfinite(P2)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else {
      B.Add(ElCLib::Value(P2,P));
    }
    B.OpenXmax();B.OpenYmax();B.OpenZmax();
  }
  else  {
    B.Add(ElCLib::Value(P1,P));
    if (Precision::IsNegativeInfinite(P2)) {
      B.OpenXmin();B.OpenYmin();B.OpenZmin();
    }
    else if (Precision::IsPositiveInfinite(P2)){
      B.OpenXmax();B.OpenYmax();B.OpenZmax();
    }
    else {
      B.Add(ElCLib::Value(P2,P));
      if (P1*P2<0) B.Add(ElCLib::Value(0.,P));
    }
  }
  B.Enlarge(Tol);
}

void BndLib::Add( const gp_Parab2d& P,const Standard_Real P1,
		 const Standard_Real P2,
		 const Standard_Real Tol, Bnd_Box2d& B) {

  if (Precision::IsNegativeInfinite(P1)) {
    if (Precision::IsNegativeInfinite(P2)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else if (Precision::IsPositiveInfinite(P2)) {
      B.OpenXmax();B.OpenYmax();
    }
    else {
      B.Add(ElCLib::Value(P2,P));
    }
    B.OpenXmin();B.OpenYmin();
  }
  else if (Precision::IsPositiveInfinite(P1)) {
    if (Precision::IsNegativeInfinite(P2)) {
      B.OpenXmin();B.OpenYmin();
    }
    else if (Precision::IsPositiveInfinite(P2)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else {
      B.Add(ElCLib::Value(P2,P));
    }
    B.OpenXmax();B.OpenYmax();
  }
  else  {
    B.Add(ElCLib::Value(P1,P));
    if (Precision::IsNegativeInfinite(P2)) {
      B.OpenXmin();B.OpenYmin();
    }
    else if (Precision::IsPositiveInfinite(P2)){
      B.OpenXmax();B.OpenYmax();
    }
    else {
      B.Add(ElCLib::Value(P2,P));
      if (P1*P2<0) B.Add(ElCLib::Value(0.,P));
    }
  }
  B.Enlarge(Tol);
}

void BndLib::Add( const gp_Hypr& H,const Standard_Real P1,
		 const Standard_Real P2,
		 const Standard_Real Tol, Bnd_Box& B) {

  if (Precision::IsNegativeInfinite(P1)) {
    if (Precision::IsNegativeInfinite(P2)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else if (Precision::IsPositiveInfinite(P2)) {
      B.OpenXmax();B.OpenYmax();B.OpenZmax();
    }
    else {
      B.Add(ElCLib::Value(P2,H));
    }
    B.OpenXmin();B.OpenYmin();B.OpenZmin();
  }
  else if (Precision::IsPositiveInfinite(P1)) {
    if (Precision::IsNegativeInfinite(P2)) {
      B.OpenXmin();B.OpenYmin();B.OpenZmin();
    }
    else if (Precision::IsPositiveInfinite(P2)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else {
      B.Add(ElCLib::Value(P2,H));
    }
    B.OpenXmax();B.OpenYmax();B.OpenZmax();
  }
  else  {
    B.Add(ElCLib::Value(P1,H));
    if (Precision::IsNegativeInfinite(P2)) {
      B.OpenXmin();B.OpenYmin();B.OpenZmin();
    }
    else if (Precision::IsPositiveInfinite(P2)){
      B.OpenXmax();B.OpenYmax();B.OpenZmax();
    }
    else {
      B.Add(ElCLib::Value(P2,H));
      if (P1*P2<0) B.Add(ElCLib::Value(0.,H));
    }
  }
  B.Enlarge(Tol);
}

void BndLib::Add(const gp_Hypr2d& H,const Standard_Real P1,
		 const Standard_Real P2,
		 const Standard_Real Tol, Bnd_Box2d& B) {
  
  if (Precision::IsNegativeInfinite(P1)) {
    if (Precision::IsNegativeInfinite(P2)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else if (Precision::IsPositiveInfinite(P2)) {
      B.OpenXmax();B.OpenYmax();
    }
    else {
      B.Add(ElCLib::Value(P2,H));
    }
    B.OpenXmin();B.OpenYmin();
  }
  else if (Precision::IsPositiveInfinite(P1)) {
    if (Precision::IsNegativeInfinite(P2)) {
      B.OpenXmin();B.OpenYmin();
    }
    else if (Precision::IsPositiveInfinite(P2)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else {
      B.Add(ElCLib::Value(P2,H));
    }
    B.OpenXmax();B.OpenYmax();
  }
  else  {
    B.Add(ElCLib::Value(P1,H));
    if (Precision::IsNegativeInfinite(P2)) {
      B.OpenXmin();B.OpenYmin();
    }
    else if (Precision::IsPositiveInfinite(P2)){
      B.OpenXmax();B.OpenYmax();
    }
    else {
      B.Add(ElCLib::Value(P2,H));
      if (P1*P2<0) B.Add(ElCLib::Value(0.,H));
    }
  }
  B.Enlarge(Tol);
}

void BndLib::Add( const gp_Cylinder& S,const Standard_Real UMin,
		 const Standard_Real UMax,const Standard_Real VMin,
		 const Standard_Real VMax,const Standard_Real Tol, Bnd_Box& B) {

  if (Precision::IsNegativeInfinite(VMin)) {
    if (Precision::IsNegativeInfinite(VMax)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else if (Precision::IsPositiveInfinite(VMax)) {
       OpenMinMax(S.Axis().Direction(),B);
    }
    else {
      Compute(UMin,UMax,S.Radius(),S.Radius(),
	      S.XAxis().Direction().XYZ(),
	      S.YAxis().Direction().XYZ(),
	      S.Location().XYZ() + VMax*S.Axis().Direction().XYZ(),B);
      OpenMin(S.Axis().Direction(),B);
    }
  }
  else if (Precision::IsPositiveInfinite(VMin)) {
    if (Precision::IsNegativeInfinite(VMax)) {
      OpenMinMax(S.Axis().Direction(),B);
    }
    else if (Precision::IsPositiveInfinite(VMax)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else {
      Compute(UMin,UMax,S.Radius(),S.Radius(),
	      S.XAxis().Direction().XYZ(),
	      S.YAxis().Direction().XYZ(),
	      S.Location().XYZ() + VMax*S.Axis().Direction().XYZ(),B);
      OpenMax(S.Axis().Direction(),B);
    }

  }
  else {
    Compute(UMin,UMax,S.Radius(),S.Radius(),
	    S.XAxis().Direction().XYZ(),
	    S.YAxis().Direction().XYZ(),
	    S.Location().XYZ() + VMin*S.Axis().Direction().XYZ(),B);
    if (Precision::IsNegativeInfinite(VMax)) {
      OpenMin(S.Axis().Direction(),B);
    }
    else if (Precision::IsPositiveInfinite(VMax)) {
      OpenMax(S.Axis().Direction(),B); 
    }
    else {
      Compute(UMin,UMax,S.Radius(),S.Radius(),
	      S.XAxis().Direction().XYZ(),
	      S.YAxis().Direction().XYZ(),
	      S.Location().XYZ() + VMax*S.Axis().Direction().XYZ(),B);
    }
  }

  B.Enlarge(Tol);

}

void BndLib::Add( const gp_Cylinder& S,const Standard_Real VMin,
		 const Standard_Real VMax,const Standard_Real Tol, Bnd_Box& B) {

  BndLib::Add(S,0.,2.*PI,VMin,VMax,Tol,B);
}

void BndLib::Add(const gp_Cone& S,const Standard_Real UMin,
		 const Standard_Real UMax,const Standard_Real VMin,
		 const Standard_Real VMax,const Standard_Real Tol, Bnd_Box& B) {

  Standard_Real R = S.RefRadius();
  Standard_Real A = S.SemiAngle();
  if (Precision::IsNegativeInfinite(VMin)) {
    if (Precision::IsNegativeInfinite(VMax)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else if (Precision::IsPositiveInfinite(VMax)) {
      gp_Dir D(Cos(A)*S.Axis().Direction());
      OpenMinMax(D,B); 
    }
    else {
      Compute(UMin,UMax,R+VMax*Sin(A),R+VMax*Sin(A),
	      S.XAxis().Direction().XYZ(),
	      S.YAxis().Direction().XYZ(),
	      S.Location().XYZ() + 
	      VMax*Cos(A)*S.Axis().Direction().XYZ(),B);
      gp_Dir D(Cos(A)*S.Axis().Direction());
      OpenMin(D,B);     
    }

  }
  else if (Precision::IsPositiveInfinite(VMin)) {
    if (Precision::IsNegativeInfinite(VMax)) {
      gp_Dir D(Cos(A)*S.Axis().Direction());
      OpenMinMax(D,B);
    }
    else if (Precision::IsPositiveInfinite(VMax)) {
      Standard_Failure::Raise("BndLib::bad parameter");
    }
    else {
      Compute(UMin,UMax,R+VMax*Sin(A),R+VMax*Sin(A),
	      S.XAxis().Direction().XYZ(),
	      S.YAxis().Direction().XYZ(),
	      S.Location().XYZ() + 
	      VMax*Cos(A)*S.Axis().Direction().XYZ(),B);
      gp_Dir D(Cos(A)*S.Axis().Direction());
      OpenMax(D,B);
    }

  }
  else {
    Compute(UMin,UMax,R+VMin*Sin(A),R+VMin*Sin(A),
	    S.XAxis().Direction().XYZ(),
	    S.YAxis().Direction().XYZ(),
	    S.Location().XYZ() + 
	    VMin*Cos(A)*S.Axis().Direction().XYZ(),B);
    if (Precision::IsNegativeInfinite(VMax)) {
      gp_Dir D(Cos(A)*S.Axis().Direction());
      OpenMin(D,B);
    }
    else if (Precision::IsPositiveInfinite(VMax)) {
      gp_Dir D(Cos(A)*S.Axis().Direction());
      OpenMax(D,B);
    }
    else {
      Compute(UMin,UMax,R+VMax*Sin(A),R+VMax*Sin(A),
	      S.XAxis().Direction().XYZ(),
	      S.YAxis().Direction().XYZ(),
	      S.Location().XYZ() + 
	      VMax*Cos(A)*S.Axis().Direction().XYZ(),B);
    }
  }


  B.Enlarge(Tol);
}

void BndLib::Add( const gp_Cone& S,const Standard_Real VMin,
		 const Standard_Real VMax,const Standard_Real Tol, Bnd_Box& B) {

  BndLib::Add(S,0.,2.*PI,VMin,VMax,Tol,B);
}

void BndLib::Add(const gp_Sphere& S,const Standard_Real UMin,
		 const Standard_Real UMax,const Standard_Real VMin,
		 const Standard_Real VMax,const Standard_Real Tol, Bnd_Box& B) {
//------------------------------------------------------------
//-- lbr le 26 novembre 97
//-- je ne comprends pas comment ce code marche
//-- et en plus il ne marche pas sur certains cas 
//-- Temporairement on choisit une solution plus simple. 
//------------------------------------------------------------
#if 0
 Standard_Real Fi1;
 Standard_Real Fi2;
 if (VMax<VMin) {
   Fi1 = VMax;
   Fi2 = VMin;
 }
 else {
   Fi1 = VMin;
   Fi2 = VMax;
 }

 if (-Fi1>Precision::Angular()) {
   if (-Fi2>Precision::Angular()) {
     Compute(UMin,UMax,S.Radius(),S.Radius(),
	       S.XAxis().Direction().XYZ(),S.YAxis().Direction().XYZ(),
	       S.Location().XYZ(),B);
     Compute(UMin,UMax,S.Radius(),S.Radius(),
	       S.XAxis().Direction().XYZ(),S.YAxis().Direction().XYZ(),
     S.Location().XYZ()- S.Radius()*S.Position().Axis().Direction().XYZ(),B);
   }
   else {
     Compute(UMin,UMax,S.Radius(),S.Radius(),
     S.XAxis().Direction().XYZ(),S.YAxis().Direction().XYZ(),
     S.Location().XYZ()+ S.Radius()*S.Position().Axis().Direction().XYZ(),B);
     Compute(UMin,UMax,S.Radius(),S.Radius(),
     S.XAxis().Direction().XYZ(),S.YAxis().Direction().XYZ(),
     S.Location().XYZ()- S.Radius()*S.Position().Axis().Direction().XYZ(),B);
   }
 }
 else {
   Compute(UMin,UMax,S.Radius(),S.Radius(),
	     S.XAxis().Direction().XYZ(),S.YAxis().Direction().XYZ(),
	     S.Location().XYZ(),B);
   Compute(UMin,UMax,S.Radius(),S.Radius(),
   S.XAxis().Direction().XYZ(),S.YAxis().Direction().XYZ(),
   S.Location().XYZ() +S.Radius()*S.Position().Axis().Direction().XYZ(),B);
 } 
 B.Enlarge(Tol);
#else
 Standard_Real u,v,du,dv;
 Standard_Integer iu,iv;
 du = (UMax-UMin)/10;
 dv = (VMax-VMin)/10;
 Standard_Real COSV[11];
 Standard_Real SINV[11];
 for(iv=0,v=VMin;iv<=10;iv++) { 
   COSV[iv]=cos(v);
   SINV[iv]=sin(v);   
   v+=dv;
 }
 for(u=UMin,iu=0; iu<=10 ; iu++) { 
   Standard_Real Radiuscosu=S.Radius()*cos(u);
   Standard_Real Radiussinu=S.Radius()*sin(u);
   for(v=VMin,iv=0; iv<=10 ; iv++) { 
     Standard_Real sinv=SINV[iv];
     Standard_Real cosv=COSV[iv];
     gp_XYZ M;
     M.SetLinearForm (cosv*Radiuscosu, S.Position().XDirection().XYZ(),
		      cosv*Radiussinu, S.Position().YDirection().XYZ(),
		      S.Radius()*sinv,     S.Position().Direction().XYZ() ,
		      S.Position().Location().XYZ()  );
     //-- static int t=0;
     //-- cout<<"point p"<<++t<<" "<<M.X()<<" "<<M.Y()<<" "<<M.Z()<<endl;
     B.Add(gp_Pnt(M));
     v+=dv;
   }
   u+=du;
 }
 
 Standard_Real Maxduv = Max(du,dv)*0.5;
 Standard_Real Fleche = S.Radius() * (1 - cos(Maxduv));
 B.Enlarge(Fleche);
 B.Enlarge(10*Tol);
#endif
}

void BndLib::Add( const gp_Sphere& S,const Standard_Real Tol, Bnd_Box& B) {

  Standard_Real R = S.Radius();
  gp_XYZ O = S.Location().XYZ();
  gp_XYZ Xd = S.XAxis().Direction().XYZ();
  gp_XYZ Yd = S.YAxis().Direction().XYZ();
  gp_XYZ Zd = S.Position().Axis().Direction().XYZ();
  B.Add(gp_Pnt(O -R*Xd -R*Yd+ R*Zd)); 
  B.Add(gp_Pnt(O -R*Xd +R*Yd+ R*Zd)); 
  B.Add(gp_Pnt(O +R*Xd -R*Yd+ R*Zd)); 
  B.Add(gp_Pnt(O +R*Xd +R*Yd+ R*Zd)); 
  B.Add(gp_Pnt(O +R*Xd -R*Yd- R*Zd)); 
  B.Add(gp_Pnt(O -R*Xd -R*Yd- R*Zd)); 
  B.Add(gp_Pnt(O +R*Xd +R*Yd- R*Zd)); 
  B.Add(gp_Pnt(O -R*Xd +R*Yd- R*Zd)); 
  B.Enlarge(Tol);
}

void BndLib::Add(const gp_Torus& S,const Standard_Real UMin,
		 const Standard_Real UMax,const Standard_Real VMin,
		 const Standard_Real VMax,const Standard_Real Tol, Bnd_Box& B) {

  Standard_Integer Fi1;
  Standard_Integer Fi2;
  if (VMax<VMin) {
    Fi1 = (Standard_Integer )( VMax/(PI/4.));
    Fi2 = (Standard_Integer )( VMin/(PI/4.));
  }
  else {
    Fi1 = (Standard_Integer )( VMin/(PI/4.));
    Fi2 = (Standard_Integer )( VMax/(PI/4.));
  }
  Fi2++;
  
  Standard_Real Ra = S.MajorRadius();
  Standard_Real Ri = S.MinorRadius();

  if (Fi2<Fi1) return;

#define SC 0.71
#define addP0    (Compute(UMin,UMax,Ra+Ri,Ra+Ri,S.XAxis().Direction().XYZ(),S.YAxis().Direction().XYZ(),S.Location().XYZ(),B))
#define addP1    (Compute(UMin,UMax,Ra+Ri*SC,Ra+Ri*SC,S.XAxis().Direction().XYZ(),S.YAxis().Direction().XYZ(),S.Location().XYZ()+(Ri*SC)*S.Axis().Direction().XYZ(),B))
#define addP2    (Compute(UMin,UMax,Ra,Ra,S.XAxis().Direction().XYZ(),S.YAxis().Direction().XYZ(),S.Location().XYZ()+Ri*S.Axis().Direction().XYZ(),B))
#define addP3    (Compute(UMin,UMax,Ra-Ri*SC,Ra-Ri*SC,S.XAxis().Direction().XYZ(),S.YAxis().Direction().XYZ(),S.Location().XYZ()+(Ri*SC)*S.Axis().Direction().XYZ(),B))
#define addP4    (Compute(UMin,UMax,Ra-Ri,Ra-Ri,S.XAxis().Direction().XYZ(),S.YAxis().Direction().XYZ(),S.Location().XYZ(),B))
#define addP5    (Compute(UMin,UMax,Ra-Ri*SC,Ra-Ri*SC,S.XAxis().Direction().XYZ(),S.YAxis().Direction().XYZ(),S.Location().XYZ()-(Ri*SC)*S.Axis().Direction().XYZ(),B))
#define addP6    (Compute(UMin,UMax,Ra,Ra,S.XAxis().Direction().XYZ(),S.YAxis().Direction().XYZ(),S.Location().XYZ()-Ri*S.Axis().Direction().XYZ(),B))
#define addP7    (Compute(UMin,UMax,Ra+Ri*SC,Ra+Ri*SC,S.XAxis().Direction().XYZ(),S.YAxis().Direction().XYZ(),S.Location().XYZ()-(Ri*SC)*S.Axis().Direction().XYZ(),B))
  
  switch (Fi1) {
  case 0 : 
    {
      addP0;
      if (Fi2 <= 0) break;
    }
  case 1 : 
    {
      addP1;
      if (Fi2 <= 1) break;
    }
  case 2 :  
    {
      addP2;
      if (Fi2 <= 2) break;
    }
  case 3 :  
    {
      addP3;
      if (Fi2 <= 3) break;
    }
  case 4 :  
    {
      addP4;
      if (Fi2 <= 4) break;
    }
  case 5 :  
    {
      addP5;
      if (Fi2 <= 5) break;
    }
  case 6 :  
    {
      addP6;
      if (Fi2 <= 6) break;
    }
  case 7 :  
    {
      addP7;
      if (Fi2 <= 7) break;
    }
  case 8 :  
  default :
    {
      addP0;
      switch (Fi2) {
      case 15 :  
	addP7;
      case 14 :  
	addP6;
      case 13 :  
	addP5;
      case 12 :  
	addP4;
      case 11 :  
	addP3;
      case 10 :  
	addP2;
      case 9 : 
	addP1;
      case 8 : 
	break;
      }    
    }
  }    
  B.Enlarge(Tol);
}


void BndLib::Add( const gp_Torus& S,const Standard_Real Tol, Bnd_Box& B) {

  Standard_Real RMa = S.MajorRadius();
  Standard_Real Rmi = S.MinorRadius(); 
  gp_XYZ O = S.Location().XYZ();
  gp_XYZ Xd = S.XAxis().Direction().XYZ();
  gp_XYZ Yd = S.YAxis().Direction().XYZ();
  gp_XYZ Zd = S.Axis().Direction().XYZ();
  B.Add(gp_Pnt(O -(RMa+Rmi)*Xd -(RMa+Rmi)*Yd+ Rmi*Zd)); 
  B.Add(gp_Pnt(O -(RMa+Rmi)*Xd -(RMa+Rmi)*Yd- Rmi*Zd)); 
  B.Add(gp_Pnt(O +(RMa+Rmi)*Xd -(RMa+Rmi)*Yd+ Rmi*Zd)); 
  B.Add(gp_Pnt(O +(RMa+Rmi)*Xd -(RMa+Rmi)*Yd- Rmi*Zd)); 
  B.Add(gp_Pnt(O -(RMa+Rmi)*Xd +(RMa+Rmi)*Yd+ Rmi*Zd)); 
  B.Add(gp_Pnt(O -(RMa+Rmi)*Xd +(RMa+Rmi)*Yd- Rmi*Zd)); 
  B.Add(gp_Pnt(O +(RMa+Rmi)*Xd +(RMa+Rmi)*Yd+ Rmi*Zd)); 
  B.Add(gp_Pnt(O +(RMa+Rmi)*Xd +(RMa+Rmi)*Yd- Rmi*Zd)); 
  B.Enlarge(Tol);
}



