// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_HVertex.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepTopAdaptor_HVertex.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepTopAdaptor_HVertex,Adaptor3d_HVertex)

BRepTopAdaptor_HVertex::BRepTopAdaptor_HVertex
  (const TopoDS_Vertex& V,
   const Handle(BRepAdaptor_HCurve2d)& C):
       myVtx(V),myCurve(C)
{}

gp_Pnt2d BRepTopAdaptor_HVertex::Value ()
{
//  return myCurve->Value(Parameter(myCurve));
  return gp_Pnt2d(RealFirst(),RealFirst()); // do nothing 
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

  // for lack of better options limit the parametric solution to 
  // 10 million*tolerance of the point

  if(tv > 1.e7*mag) ResUV = 1.e7;
  else ResUV = tv/mag;

  // Control
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
  // Refine if possible
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
      // Take the result of interpolation
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
      // Take the new estimation
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

