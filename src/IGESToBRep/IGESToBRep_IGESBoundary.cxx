// Created on: 1998-12-16
// Created by: Roman LYGIN
// Copyright (c) 1998-1999 Matra Datavision
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


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Trsf2d.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESToBRep.hxx>
#include <IGESToBRep_CurveAndSurface.hxx>
#include <IGESToBRep_IGESBoundary.hxx>
#include <IGESToBRep_TopoCurve.hxx>
#include <Precision.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeFix_Wire.hxx>
#include <Standard_Type.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESToBRep_IGESBoundary,Standard_Transient)

//=======================================================================
//function : IGESToBRep_IGESBoundary
//purpose  : 
//=======================================================================
IGESToBRep_IGESBoundary::IGESToBRep_IGESBoundary()
{
}

//=======================================================================
//function : IGESToBRep_IGESBoundary
//purpose  : 
//=======================================================================

IGESToBRep_IGESBoundary::IGESToBRep_IGESBoundary(const IGESToBRep_CurveAndSurface& CS)
     : myCS (CS)
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

 void IGESToBRep_IGESBoundary::Init(const IGESToBRep_CurveAndSurface& CS,
				    const Handle(IGESData_IGESEntity)& entity,
				    const TopoDS_Face& face,
				    const gp_Trsf2d& trans,
				    const Standard_Real uFact,
				    const Standard_Integer filepreference) 
{
  myCS             = CS;
  myentity         = entity;
  myface           = face;
  mytrsf           = trans;
  myuFact          = uFact;
  myfilepreference = filepreference;
}

//=======================================================================
//function : Transfer
//purpose  : 
//=======================================================================

 Standard_Boolean IGESToBRep_IGESBoundary::Transfer(Standard_Boolean& okCurve,
						    Standard_Boolean& okCurve3d,
						    Standard_Boolean& okCurve2d,
						    const Handle(IGESData_IGESEntity)& curve3d,
						    const Standard_Boolean toreverse3d,
						    const Handle(IGESData_HArray1OfIGESEntity)& curves2d,
						    const Standard_Integer number) 
{
  Handle(ShapeExtend_WireData) scurve3d, lsewd; //temporary objects
  return Transfer (okCurve, okCurve3d, okCurve2d,
		   curve3d, scurve3d, Standard_False, toreverse3d,
		   curves2d, Standard_False,
		   number, lsewd);
}

//=======================================================================
//function : Transfer
//purpose  : 
//=======================================================================

 Standard_Boolean IGESToBRep_IGESBoundary::Transfer(Standard_Boolean& okCurve,
						    Standard_Boolean& okCurve3d,
						    Standard_Boolean& okCurve2d,
						    const Handle(ShapeExtend_WireData)& curve3d,
						    const Handle(IGESData_HArray1OfIGESEntity)& curves2d,
						    const Standard_Boolean toreverse2d,
						    const Standard_Integer number,
						    Handle(ShapeExtend_WireData)& lsewd) 
{
  Handle(IGESData_IGESEntity) icurve3d; //temporary object
  return Transfer (okCurve, okCurve3d, okCurve2d,
		   icurve3d, curve3d, Standard_True, Standard_False,
		   curves2d, toreverse2d,
		   number, lsewd);
}

//=======================================================================
//function : Check
//purpose  : 
//=======================================================================

 void IGESToBRep_IGESBoundary::Check(const Standard_Boolean,const Standard_Boolean,const Standard_Boolean,const Standard_Boolean) 
{
  // Implemented in Advanced Data Exchange
}

//=======================================================================
//function : Transfer
//purpose  : 
//=======================================================================

 Standard_Boolean IGESToBRep_IGESBoundary::Transfer(Standard_Boolean&,
						    Standard_Boolean&,
						    Standard_Boolean&,
						    const Handle(IGESData_IGESEntity)& icurve3d,
						    const Handle(ShapeExtend_WireData)& scurve3d,
						    const Standard_Boolean usescurve,
						    const Standard_Boolean toreverse3d,
						    const Handle(IGESData_HArray1OfIGESEntity)& curves2d,
						    const Standard_Boolean toreverse2d,
						    const Standard_Integer number,
						    Handle(ShapeExtend_WireData)& Gsewd) 
{
  Gsewd                                = new ShapeExtend_WireData;//local translation (for mysewd)
  Handle(ShapeExtend_WireData) Gsewd3d = new ShapeExtend_WireData;//local translation (for mysewd3d)
  Handle(ShapeExtend_WireData) Gsewd2d = new ShapeExtend_WireData;//local translation (for mysewd2d)

  Standard_Boolean GTranslate3d = Standard_True, GTranslate2d = Standard_True,
                   Preferred3d  = Standard_True, Preferred2d  = Standard_True;

  Standard_Integer len3d = 0, len2d = 0;
  Handle(TColStd_HSequenceOfTransient) seq3d, seq2d;
  if (usescurve)
    len3d = scurve3d->NbEdges();
  else {
    IGESToBRep::IGESCurveToSequenceOfIGESCurve (icurve3d, seq3d);
    len3d = seq3d->Length();
  }
  if (!curves2d.IsNull()) {
    for (Standard_Integer i = 1; i <= curves2d->Length(); i++)
      IGESToBRep::IGESCurveToSequenceOfIGESCurve (curves2d->Value (i), seq2d);
    len2d = seq2d->Length();
  }

  if (len3d == 0)
    GTranslate3d = Standard_False;
  else if (len2d == 0)
    GTranslate2d = Standard_False;
  
  if (GTranslate3d && GTranslate2d) {
    //Setting preference in the case of inconsitency between 3D and 2D
    if      (myfilepreference == 2) Preferred3d = Standard_False;
    else if (myfilepreference == 3) Preferred2d = Standard_False;
    else                            Preferred3d = Standard_False;
  }
  if (GTranslate3d && GTranslate2d && len3d != len2d) {
    GTranslate3d = Preferred3d;
    GTranslate2d = Preferred2d;
  }
    
  IGESToBRep_TopoCurve TC (myCS);
  
  if (GTranslate3d && !GTranslate2d) {
    if (usescurve)
      Gsewd->Add (scurve3d->Wire());
    else {
      TopoDS_Shape Sh = TC.TransferTopoCurve (icurve3d);
      if (!Sh.IsNull()) {
	Gsewd3d->Add (Sh);
	if (toreverse3d) {
	  ReverseCurves3d (Gsewd3d);
	  Gsewd->Add (Gsewd3d->Wire());
	}
	else Gsewd->Add (Sh);//Gsewd = Gsewd3d is impossible to avoid sharing of sewd (UK1.igs entity 7)
      }
    }
  }
  else if (!GTranslate3d && GTranslate2d) {
    for (Standard_Integer i = curves2d->Lower(); i <= curves2d->Upper(); i++) {
      TopoDS_Shape Sh = TC.Transfer2dTopoCurve (curves2d->Value (i), myface, mytrsf, myuFact);
      if (!Sh.IsNull()) Gsewd2d->Add (Sh);
    }
    if (toreverse2d)
      ReverseCurves2d (Gsewd2d, myface);
    Gsewd->Add (Gsewd2d->Wire());
  }
  else if (GTranslate3d && GTranslate2d) {
    //Translate both curves 3D and 2D
    //Suppose that i-th segment in 2D curve corresponds to i-th segment in 3D curve
    for (Standard_Integer i = 1; i <= len3d; i++) {
      Standard_Boolean LTranslate3d = Standard_True, LTranslate2d = Standard_True;
      
      Handle(ShapeExtend_WireData) Lsewd3d = new ShapeExtend_WireData;
      TC.SetBadCase (Standard_False); //:27
      if (usescurve)
	Lsewd3d->Add (scurve3d->Edge (i));
      else {
	TopoDS_Shape shape3d = TC.TransferTopoCurve (Handle(IGESData_IGESEntity)::DownCast (seq3d->Value (i)));
	if (!shape3d.IsNull()) {
	  Lsewd3d->Add (shape3d);
	  if (toreverse3d)
	    ReverseCurves3d (Lsewd3d);
	}
	else LTranslate3d = Standard_False;
      }
      Gsewd3d->Add (Lsewd3d->Wire());
      
      Handle(ShapeExtend_WireData) Lsewd2d = new ShapeExtend_WireData;
      TopoDS_Shape shape2d = TC.Transfer2dTopoCurve (Handle(IGESData_IGESEntity)::DownCast (seq2d->Value (i)),
						     myface, mytrsf, myuFact);
      if (!shape2d.IsNull()) {
	Lsewd2d->Add  (shape2d);
	if (toreverse2d)
	  ReverseCurves2d (Lsewd2d, myface);
	Gsewd2d->Add (Lsewd2d->Wire());
      }
      else LTranslate2d = Standard_False;
      
      if (LTranslate3d && LTranslate2d && Lsewd3d->NbEdges() != Lsewd2d->NbEdges()) {
	LTranslate3d = Preferred3d;
	LTranslate2d = Preferred2d;
      }
      Handle(ShapeExtend_WireData) Lsewd;//Lsewd3d or Lsewd2d or Lsewd3d+pcurve
      if      ( LTranslate3d && !LTranslate2d) Lsewd = Lsewd3d;
      else if (!LTranslate3d &&  LTranslate2d) Lsewd = Lsewd2d;
      else {
	Lsewd = Lsewd3d;
	//copying pcurve to edge with 3D curve
	for (Standard_Integer iedge = 1; iedge <= Lsewd3d->NbEdges(); iedge++) {
	  TopoDS_Edge edge3d = Lsewd3d->Edge (iedge), edge2d = Lsewd2d->Edge (iedge);
	  if (!IGESToBRep::TransferPCurve (edge2d, edge3d, myface)) continue;
	}
      }
      Gsewd->Add (Lsewd->Wire());
    }
  }
  
  if (number > 1) {
    mysewd  ->Add (Gsewd  ->Wire());
    mysewd3d->Add (Gsewd3d->Wire());
    mysewd2d->Add (Gsewd2d->Wire());
  }
  else {
    mysewd   = Gsewd;
    mysewd3d = Gsewd3d;
    mysewd2d = Gsewd2d;
  }
  return Standard_True;
}

//=======================================================================
//function : ReverseCurves3d
//purpose  : Reverses 3D curves of the edges in the wire and reverses
//           the order of edges in the wire.
//           Orientation of each edge is not changed
//=======================================================================

void IGESToBRep_IGESBoundary::ReverseCurves3d (const Handle(ShapeExtend_WireData)& sewd)
{
  sewd->Reverse();
  BRep_Builder B;
  TopoDS_Wire W;
  B.MakeWire(W);
  for (Standard_Integer i = 1; i <= sewd->NbEdges(); i++) {
    TopoDS_Edge oldedge = sewd->Edge (i), newedge;
    TopLoc_Location L;
    Standard_Real p1, p2;
    Handle(Geom_Curve) curve = BRep_Tool::Curve (oldedge, L, p1, p2);
    if (curve->IsPeriodic())                                   //#21
      ShapeBuild_Edge().MakeEdge (newedge, curve->Reversed(), L,
				curve->ReversedParameter (p2),
				curve->ReversedParameter (p1));
    else ShapeBuild_Edge().MakeEdge (newedge, curve->Reversed(), L,
				Max (curve->ReversedParameter(curve->LastParameter()), curve->ReversedParameter (p2)),
				Min (curve->ReversedParameter(curve->FirstParameter()),  curve->ReversedParameter (p1)));
    newedge.Orientation(TopAbs::Reverse (oldedge.Orientation()));
    //sewd->Set (newedge, i);
    B.Add(W,newedge);
  }
  Handle(ShapeFix_Wire) sfw = new ShapeFix_Wire();
  sfw->Load(W);
  sfw->FixConnected();
  sewd->Init(sfw->Wire());
}

//=======================================================================
//function : ReverseCurves2d
//purpose  : Reverses pcurves of the edges in the wire and reverses
//           the order of edges in the wire.
//           Orientation of each edge is also changed
//=======================================================================

 void IGESToBRep_IGESBoundary::ReverseCurves2d (const Handle(ShapeExtend_WireData)& sewd, const TopoDS_Face& face)
{
  sewd->Reverse(face);
  for (Standard_Integer i = 1; i <= sewd->NbEdges(); i++) {
    TopoDS_Edge oldedge = sewd->Edge (i), newedge;
    Standard_Real p1, p2;
    Handle(Geom2d_Curve) curve = BRep_Tool::CurveOnSurface (oldedge, face, p1, p2);

    // skl 24.04.2002 for OCC314
    if(curve->IsPeriodic())
      ShapeBuild_Edge().MakeEdge (newedge, curve->Reversed(), face,
                                  curve->ReversedParameter (p2),
                                  curve->ReversedParameter (p1));
    else
      ShapeBuild_Edge().MakeEdge (newedge, curve->Reversed(), face,
                                  Max (curve->FirstParameter(), curve->ReversedParameter (p2)),//BUC50001 entity 936 2DForced
                                  Min (curve->LastParameter(),  curve->ReversedParameter (p1)));
    newedge.Orientation(oldedge.Orientation());
    sewd->Set (newedge, i);
  }
}

