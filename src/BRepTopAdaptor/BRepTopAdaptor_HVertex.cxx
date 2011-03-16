#include <BRepTopAdaptor_HVertex.ixx>

#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>


BRepTopAdaptor_HVertex::BRepTopAdaptor_HVertex
  (const TopoDS_Vertex& V,
   const Handle(BRepAdaptor_HCurve2d)& C):
       myVtx(V),myCurve(C)
{}

gp_Pnt2d BRepTopAdaptor_HVertex::Value ()
{
//  return myCurve->Value(Parameter(myCurve));
  return gp_Pnt2d(RealFirst(),RealFirst()); // rien a faire
}

Standard_Real BRepTopAdaptor_HVertex::Parameter 
  (const Handle(Adaptor2d_HCurve2d)& C)
{
  Handle(BRepAdaptor_HCurve2d) brhc = 
    Handle(BRepAdaptor_HCurve2d)::DownCast(C);
  return BRep_Tool::Parameter(myVtx,
			      ((BRepAdaptor_Curve2d *)&(brhc->Curve2d()))->Edge(),
			      ((BRepAdaptor_Curve2d *)&(brhc->Curve2d()))->Face());
}


Standard_Real BRepTopAdaptor_HVertex::Resolution 
  (const Handle(Adaptor2d_HCurve2d)& C)
{
  Handle(BRepAdaptor_HCurve2d) brhc = 
    Handle(BRepAdaptor_HCurve2d)::DownCast(C);
  const TopoDS_Face& F = ((BRepAdaptor_Curve2d *)&(brhc->Curve2d()))->Face();
  BRepAdaptor_Surface S(F,0);
  Standard_Real tv = BRep_Tool::Tolerance(myVtx);
  Standard_Real pp, p = BRep_Tool::Parameter
    (myVtx,
     ((BRepAdaptor_Curve2d *)&(brhc->Curve2d()))->Edge(),
     ((BRepAdaptor_Curve2d *)&(brhc->Curve2d()))->Face());
  TopAbs_Orientation Or = Orientation();
  gp_Pnt2d p2d; gp_Vec2d v2d;
  C->D1(p,p2d,v2d);
  gp_Pnt P, P1; 
  gp_Vec DU, DV, DC;
  S.D1(p2d.X(),p2d.Y(),P,DU,DV);
  DC.SetLinearForm(v2d.X(),DU,v2d.Y(),DV);
  Standard_Real ResUV, mag = DC.Magnitude();

  Standard_Real URes = S.UResolution(tv);
  Standard_Real VRes = S.VResolution(tv);
  Standard_Real tURes = C->Resolution(URes);
  Standard_Real tVRes = C->Resolution(VRes);
  Standard_Real ResUV1 = Max(tURes, tVRes);

  if(mag<1e-12) { 

    return(ResUV1);

  }

  // a defaut de mieux on borne la resolution parametrique a 
  // 10 million*la tolerance du point

  if(tv > 1.e7*mag) ResUV = 1.e7;
  else ResUV = tv/mag;

  // Controle
  if (Or == TopAbs_REVERSED) pp = p+ResUV;
  else pp = p-ResUV;

  Standard_Real UMin=C->FirstParameter();
  Standard_Real UMax=C->LastParameter();
  if(pp>UMax) pp=UMax;
  if(pp<UMin) pp=UMin;

  C->D0(pp, p2d);
  S.D0(p2d.X(),p2d.Y(),P1);

  Standard_Real Dist=P.Distance(P1);
  if ((Dist>1e-12) && ((Dist > 1.1*tv) || (Dist< 0.8*tv))) {
  // Raffinement si possible
    Standard_Real Dist1;
    if (Or == TopAbs_REVERSED) pp = p+tv/Dist;
    else pp = p-tv/Dist;

    if(pp>UMax) pp=UMax;
    if(pp<UMin) pp=UMin;

    C->D1(pp, p2d, v2d);
    S.D1(p2d.X(),p2d.Y(),P1,DU,DV);
    DC.SetLinearForm(v2d.X(),DU,v2d.Y(),DV);
    Dist1 = P.Distance(P1);
    if (Abs(Dist1-tv) < Abs(Dist-tv)) {
      // On prend le resultat d'interpolation
      ResUV = tv/Dist;
      Dist = Dist1;
    }
    
    mag = DC.Magnitude();
    if(tv > 1.e7*mag) mag = tv*1.e-7;
    if (Or == TopAbs_REVERSED) pp = p+tv/mag;    
    else pp = p-tv/mag;

    if(pp>UMax) pp=UMax;
    if(pp<UMin) pp=UMin;

    C->D0(pp, p2d);
    S.D0(p2d.X(),p2d.Y(),P1);
    Dist1 = P.Distance(P1);
    if (Abs(Dist1-tv) < Abs(Dist-tv)) {
      // On prend la nouvelle estimation
      ResUV = tv/mag;
      Dist = Dist1;
    }        
  }
  
  return Min(ResUV, ResUV1);
}


TopAbs_Orientation BRepTopAdaptor_HVertex::Orientation ()
{
  return myVtx.Orientation();
}

Standard_Boolean BRepTopAdaptor_HVertex::IsSame
  (const Handle(Adaptor3d_HVertex)& Other)
{
  Handle(BRepTopAdaptor_HVertex) brhv = 
    Handle(BRepTopAdaptor_HVertex)::DownCast(Other);
  return myVtx.IsSame(brhv->Vertex());
}

