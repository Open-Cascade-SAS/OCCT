// Created on: 2000-02-05
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

//06.01.99 pdn renaming ShapeFix_Wire::FixLittle to ShapeFix_Wire::FixSmall
//:n1 abv 20.01.99: BUC60328.rle (loopback): call FixSmall with Max(1.,preci)
//smh #1  11.03.99 PRO15640/15641, szv#2 PRO17393: call FixSameParameter after BuildCurve3d
//#74 rln 10.03.99 S4135: handling use of BRepLib::SameParameter by new static parameter
// pdn 18.03.99 S4135: using mintol instead of resolution from IGES file
// pdn 18.03.99 S4135: projecting with resolution from IGES file.
//%14 pdn 06.03.99 implementing fixFace in IGES
// S4135 pdn improvements in using of stdsameparameter flag
//#21 gka 12.04.99 CSR UKI60998 entities number - 13, 24 lost of boundaries during translation of tabulated cylinders
//S4181 pdn 15.04.99 implementing of reading IGES elementary surfaces. (instead of shift, reverce and 
//scale factors the transformation matrix and scale factor used)

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <IGESControl_IGESBoundary.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESGeom_BSplineCurve.hxx>
#include <IGESToBRep.hxx>
#include <IGESToBRep_CurveAndSurface.hxx>
#include <IGESToBRep_TopoCurve.hxx>
#include <Interface_Static.hxx>
#include <Message_Msg.hxx>
#include <Precision.hxx>
#include <ShapeAlgo.hxx>
#include <ShapeAlgo_AlgoContainer.hxx>
#include <ShapeAlgo_ToolContainer.hxx>
#include <ShapeAnalysis_ShapeTolerance.hxx>
#include <ShapeAnalysis_Wire.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeFix_Edge.hxx>
#include <ShapeFix_ShapeTolerance.hxx>
#include <ShapeFix_Wire.hxx>
#include <Standard_Type.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESControl_IGESBoundary,IGESToBRep_IGESBoundary)

//=======================================================================
//function : IGESControl_IGESBoundary
//purpose  : 
//=======================================================================
IGESControl_IGESBoundary::IGESControl_IGESBoundary() : IGESToBRep_IGESBoundary()
{
}

//=======================================================================
//function : IGESControl_IGESBoundary
//purpose  : 
//=======================================================================

IGESControl_IGESBoundary::IGESControl_IGESBoundary(const IGESToBRep_CurveAndSurface& CS) :
       IGESToBRep_IGESBoundary (CS)
{
}

//=======================================================================
//function : Check
//purpose  : 
//=======================================================================

 void IGESControl_IGESBoundary::Check(const Standard_Boolean result,const Standard_Boolean checkclosure,
				      const Standard_Boolean aokCurve3d, const Standard_Boolean aokCurve2d) 
{
  Standard_Boolean Result = result;
  Standard_Boolean okCurve3d = aokCurve3d, okCurve2d = aokCurve2d; 
  Standard_Real maxtol = myCS.GetMaxTol();
  
  if (Result && checkclosure) {
    //USA60022 7277 check of closure
    Handle(ShapeAnalysis_Wire) saw = new ShapeAnalysis_Wire;
    saw->Load (mysewd);
    saw->SetPrecision (maxtol);
    saw->CheckConnected (1);
    if (saw->LastCheckStatus (ShapeExtend_FAIL)) {
      saw->Load (mysewd3d);
      saw->CheckConnected (1);
      if (saw->LastCheckStatus (ShapeExtend_FAIL)) okCurve3d = Standard_False;
      else                                         okCurve2d = Standard_False;
      Result = Standard_False;
    }
  }
  if (!Result) {
    mysewd->Clear();
    if (okCurve3d && mysewd3d->NbEdges() > 0) {
      Message_Msg Msg1070("IGES_1070");//"Representations in the file are inconsistent. Recomputation from 3d"
      Msg1070.Arg(3);
      myCS.SendWarning(myentity,Msg1070);
      mysewd = mysewd3d;
    }
    else if (okCurve2d && mysewd2d->NbEdges() > 0) {
      Message_Msg Msg1070("IGES_1070");//"Representations in the file are inconsistent. Recomputation from 2d"
      Msg1070.Arg(2);
      myCS.SendWarning(myentity,Msg1070);
      mysewd = mysewd2d;
    }
  }
}

//=======================================================================
//function : Connect
//purpose  : Connects theNextWD to theWD using theSAW. 
//           First, connects edges of theNextWD by calling ShapeFix_Wire::FixConnected(). This
//           is necessary when theNextWD was built using multiple curves from the Composite
//           Curve (as ShapeExtend_WireData::Wire() would otherwise produce a wrong 
//           disconnected TopoDS_Wire).
//           FixConnected() will only update the edges resulting from different composite
//           curve segments. Edges resulting from splitting C0 curve will already be
//           connected.
//=======================================================================
static Standard_Boolean Connect (const Handle(ShapeAnalysis_Wire)& theSAW,
                                 const Handle(ShapeExtend_WireData)& theWD,
                                 const Handle(ShapeExtend_WireData)& theNextWD,
                                 const Standard_Boolean theConnectNextWD,
                                 const Standard_Real theMaxTol,
                                 Standard_Real& theDistMin,
                                 Standard_Boolean& theReverseWD,
                                 Standard_Boolean& theReverseNextWD)
{
    theSAW->Load (theWD);
    if (theConnectNextWD) {
      Handle(ShapeFix_Wire) sfw = new ShapeFix_Wire;
      sfw->Load (theNextWD);
      sfw->ClosedWireMode() = Standard_False; //internal connections are enough
      sfw->FixConnected();
    }
    return ShapeAlgo::AlgoContainer()->ConnectNextWire (theSAW, theNextWD,
      theMaxTol, theDistMin, theReverseWD, theReverseNextWD);
}

//=======================================================================
//function : Transfer
//purpose  : 
//=======================================================================

 Standard_Boolean IGESControl_IGESBoundary::Transfer(Standard_Boolean& okCurve,
                                                     Standard_Boolean& okCurve3d,
                                                     Standard_Boolean& okCurve2d,
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

  Standard_Boolean revsewd, revnextsewd;
  Standard_Real distmin, precision = myCS.GetEpsGeom() * myCS.GetUnitFactor(), maxtol = myCS.GetMaxTol();
  
  Handle(ShapeAnalysis_Wire) saw   = new ShapeAnalysis_Wire,
                             saw3d = new ShapeAnalysis_Wire,
                             saw2d = new ShapeAnalysis_Wire;
  saw->Load   (Gsewd);   saw->SetPrecision   (precision);
  saw3d->Load (Gsewd3d); saw3d->SetPrecision (precision);
  saw2d->Load (Gsewd2d); saw2d->SetPrecision (precision);
  
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

  Standard_Integer surfcurv = myCS.GetSurfaceCurve();
  if ((surfcurv == -2 && len2d > 0) || len3d == 0)
    GTranslate3d = Standard_False;
  else if (( surfcurv == -3 && len3d > 0) || len2d == 0)
    GTranslate2d = Standard_False;
 
  if (GTranslate3d && GTranslate2d) {
    //Setting preference in the case of inconsitency between 3D and 2D
    if      (surfcurv == 2)         Preferred3d = Standard_False;
    else if (surfcurv == 3)         Preferred2d = Standard_False;
    else if (myfilepreference == 2) Preferred3d = Standard_False;
    else if (myfilepreference == 3) Preferred2d = Standard_False;
    else                            Preferred3d = Standard_False;
  }
  if (GTranslate3d && GTranslate2d && len3d != len2d) {
    GTranslate3d = Preferred3d;
    GTranslate2d = Preferred2d;
  }
    
  IGESToBRep_TopoCurve TC (myCS);
  
  if (GTranslate3d && !GTranslate2d) {
    if (usescurve) {
      Gsewd->Add(scurve3d->Wire());
    }
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
    if (toreverse2d) {
      ReverseCurves2d (Gsewd2d, myface);
    }
    Gsewd->Add (Gsewd2d->Wire());
  }
  else if( GTranslate3d && GTranslate2d ) {
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
	  if (toreverse3d) {
	    ReverseCurves3d (Lsewd3d);
          }
	}
	else LTranslate3d = Standard_False;
      }
      Standard_Boolean bad3d = TC.BadCase(); //:27
      okCurve3d = okCurve3d && ShapeAlgo::AlgoContainer()->ConnectNextWire (saw3d, Lsewd3d, maxtol, distmin, revsewd, revnextsewd);
      
      Handle(ShapeExtend_WireData) Lsewd2d = new ShapeExtend_WireData;
      TC.SetBadCase (Standard_False); //:27
      TopoDS_Shape shape2d = TC.Transfer2dTopoCurve (Handle(IGESData_IGESEntity)::DownCast (seq2d->Value (i)),
						     myface, mytrsf, myuFact);
      Standard_Boolean bad2d = TC.BadCase(); //:27
      if (!shape2d.IsNull()) {
	Lsewd2d->Add  (shape2d);
	if (toreverse2d) {
	  ReverseCurves2d (Lsewd2d, myface);
        }
	okCurve2d = okCurve2d && ShapeAlgo::AlgoContainer()->ConnectNextWire (saw2d, Lsewd2d, maxtol, distmin, revsewd, revnextsewd);
      }
      else LTranslate2d = Standard_False;
      
//     if (LTranslate3d && LTranslate2d && (Lsewd3d->NbEdges() != Lsewd2d->NbEdges() || bad3d || bad2d)) {
      Standard_Boolean isBSpline = Standard_False;
      if(!usescurve && !seq3d->Value (i).IsNull() && !seq2d->Value (i).IsNull())
        isBSpline = seq3d->Value (i)->IsKind(STANDARD_TYPE(IGESGeom_BSplineCurve)) &&
          seq2d->Value (i)->IsKind(STANDARD_TYPE(IGESGeom_BSplineCurve));
      
      if (LTranslate3d && LTranslate2d && 
          (
           (isBSpline && (Lsewd3d->NbEdges() != Lsewd2d->NbEdges())) ||
           (!isBSpline && (Lsewd3d->NbEdges() != 1 || Lsewd2d->NbEdges() != 1)) ||
           bad3d || bad2d
           )) {
        
	LTranslate3d = Preferred3d;
	LTranslate2d = Preferred2d;
      }
      Handle(ShapeExtend_WireData) Lsewd;//Lsewd3d or Lsewd2d or Lsewd3d+pcurve
      if      ( LTranslate3d && !LTranslate2d) Lsewd = Lsewd3d;
      else if (!LTranslate3d &&  LTranslate2d) Lsewd = Lsewd2d;
      else {
	Lsewd = Lsewd3d;
	//copying pcurve to edge with 3D curve
	Handle(ShapeFix_Edge) sfe = new ShapeFix_Edge;
	for (Standard_Integer iedge = 1; iedge <= Lsewd3d->NbEdges(); iedge++) {
	  TopoDS_Edge edge3d = Lsewd3d->Edge (iedge), edge2d = Lsewd2d->Edge (iedge);
	  if (!IGESToBRep::TransferPCurve (edge2d, edge3d, myface)) continue;
	  if (sfe->FixReversed2d (edge3d, myface)) {
#ifdef OCCT_DEBUG
	    cout << "Warning: IGESToBRep_IGESBoundary: 2D curve of edge was reversed" << endl;
#endif
	  }
	  //#74 rln 10.03.99 S4135: handling use of BRepLib::SameParameter by new static parameter
	  if (Interface_Static::IVal ("read.stdsameparameter.mode")) {
	    Standard_Real first, last;
	    BRep_Tool::Range(edge3d,first,last);
	    // pdn 08.04.99 S4135 optimizing in computation of SPTol
	    //choosing tolerance according to Approx_SameParameter: 50 * 22
	    Standard_Real SPTol = Min (precision, Abs (last - first)/1000);
	    BRep_Builder B;
	    B.SameParameter (edge3d, Standard_False);
	    sfe->FixSameParameter (edge3d, SPTol);
	  }
	  else
	    sfe->FixSameParameter (edge3d);
	  Standard_Real maxdev = BRep_Tool::Tolerance (edge3d);
	  //pdn 08.04.99 S4135 recomputing only if deviation is greater than maxtol
	  if (maxdev > maxtol) { //:e2
#ifdef OCCT_DEBUG
	    cout << "Warning: IGESToBRep_IGESBoundary: Deviation = " << maxdev << endl;
#endif
	    ShapeFix_ShapeTolerance().SetTolerance (edge3d, Precision::Confusion());
	    for (Standard_Integer ie = 1; ie <= iedge; ie++)
	      ShapeBuild_Edge().RemovePCurve (Lsewd3d->Edge (ie), myface);
	    if (Preferred3d) {
#ifdef OCCT_DEBUG
	      cout << "Warning: IGESToBRep_IGESBoundary: 3D and 2D curves are inconsistent; 2D is ignored" << endl;
#endif
	    }
	    else {
#ifdef OCCT_DEBUG
	      cout << "Warning: IGESToBRep_IGESBoundary: 3D and 2D curves are inconsistent; 3D is ignored" << endl;
#endif
	      Lsewd = Lsewd2d;
	    }
	    break;
	  }
	}
      }
      okCurve = okCurve && ShapeAlgo::AlgoContainer()->ConnectNextWire (saw, Lsewd, maxtol, distmin, revsewd, revnextsewd);
      if (!okCurve) {
#ifdef OCCT_DEBUG
	cout << "Warning: IGESToBRep_IGESBoundary: Curves " << i - 1 << " and " << i << " cannot be connected" << endl;
#endif
        Gsewd3d = new ShapeExtend_WireData;
        for (Standard_Integer j = 1; j <= len3d; j++) {
          if (usescurve)
            Gsewd3d->Add (scurve3d->Edge (j));
          else {
            TopoDS_Shape Sh =
              TC.TransferTopoCurve (Handle(IGESData_IGESEntity)::DownCast (seq3d->Value (j)));
            if (!Sh.IsNull()) Gsewd3d->Add (Sh);
          }
        }
        if (toreverse3d) {
          ReverseCurves3d(Gsewd3d);
        }
        Gsewd2d = new ShapeExtend_WireData;
        for (Standard_Integer k = 1; k <= len2d; k++) {
          TopoDS_Shape Sh = TC.Transfer2dTopoCurve (Handle(IGESData_IGESEntity)::DownCast (seq2d->Value (k)),
                                                    myface, mytrsf, myuFact);
          if (!Sh.IsNull()) Gsewd2d->Add (Sh);
        }
        if (toreverse2d) {
          ReverseCurves2d (Gsewd2d, myface);
        }
        Handle(ShapeFix_Wire) sfw3 = new ShapeFix_Wire(Gsewd3d->Wire(),myface,precision);
        sfw3->Perform();
        TopoDS_Wire w3 = sfw3->Wire();
        Handle(ShapeFix_Wire) sfw2 = new ShapeFix_Wire(Gsewd2d->Wire(),myface,precision);
        sfw2->Perform();
        TopoDS_Wire w2 = sfw2->Wire();
        ShapeAnalysis_ShapeTolerance ST;
        double tol3 = ST.Tolerance(w3,1);
        double tol2 = ST.Tolerance(w2,1);
        Gsewd3d = new ShapeExtend_WireData;
        Gsewd2d = new ShapeExtend_WireData;
        Gsewd3d->Add(w3);
        Gsewd2d->Add(w3);
        if(tol3<tol2)
          Gsewd->Add(w3);
        else
          Gsewd->Add(w2);
        okCurve = Standard_True;
        okCurve2d = Standard_True;
        okCurve3d = Standard_True;
      }
    }
  }
  
  if (number > 1) {
    okCurve   = okCurve && Connect (saw, mysewd, Gsewd, (len3d > 1) || (len2d > 1), maxtol,
      distmin, revsewd, revnextsewd);
    okCurve3d = okCurve3d && Connect (saw3d, mysewd3d, Gsewd3d, len3d > 1, maxtol,
      distmin, revsewd, revnextsewd);
    okCurve2d = okCurve2d && Connect (saw2d, mysewd2d, Gsewd2d, len2d > 1, maxtol,
      distmin, revsewd, revnextsewd);
  }
  else {
    mysewd   = Gsewd;
    mysewd3d = Gsewd3d;
    mysewd2d = Gsewd2d;
  }
  return okCurve;
}

