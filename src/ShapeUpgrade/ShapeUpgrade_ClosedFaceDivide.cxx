// Created on: 1999-07-22
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
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


#include <Bnd_Box2d.hxx>
#include <BRep_Tool.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeExtend.hxx>
#include <ShapeExtend_CompositeSurface.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeFix_ComposeShell.hxx>
#include <ShapeUpgrade_ClosedFaceDivide.hxx>
#include <ShapeUpgrade_SplitSurface.hxx>
#include <Standard_Type.hxx>
#include <TColStd_HSequenceOfReal.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Wire.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeUpgrade_ClosedFaceDivide,ShapeUpgrade_FaceDivide)

//=======================================================================
//function : ShapeUpgrade_ClosedFaceDivide
//purpose  : 
//=======================================================================
ShapeUpgrade_ClosedFaceDivide::ShapeUpgrade_ClosedFaceDivide():
       ShapeUpgrade_FaceDivide()
{
  myNbSplit = 1;
}

//=======================================================================
//function : ShapeUpgrade_ClosedFaceDivide
//purpose  : 
//=======================================================================

ShapeUpgrade_ClosedFaceDivide::ShapeUpgrade_ClosedFaceDivide(const TopoDS_Face& F):
       ShapeUpgrade_FaceDivide(F)
{
  myNbSplit = 1;
}

//=======================================================================
//function : SplitSurface
//purpose  : 
//=======================================================================

Standard_Boolean ShapeUpgrade_ClosedFaceDivide::SplitSurface()
{
  Handle(ShapeUpgrade_SplitSurface) SplitSurf = GetSplitSurfaceTool();
  if ( SplitSurf.IsNull() ) return Standard_False;
  
  if ( myResult.IsNull() || myResult.ShapeType() != TopAbs_FACE ) {
    myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_FAIL3 );
    return Standard_False;
  }
  TopoDS_Face face = TopoDS::Face ( myResult );
  
  Standard_Real Uf,Ul,Vf,Vl;
  ShapeAnalysis::GetFaceUVBounds ( myFace, Uf, Ul, Vf, Vl );
  // 01.10.99 pdn Porting on DEC 
  if( ::Precision::IsInfinite(Uf) || ::Precision::IsInfinite(Ul) || 
      ::Precision::IsInfinite(Vf) || ::Precision::IsInfinite(Vl) )
    return Standard_False;
  
  TopLoc_Location L;
  Handle(Geom_Surface) surf;
  surf = BRep_Tool::Surface ( face, L );
  
  Standard_Boolean isUSplit = Standard_False;
  Standard_Boolean doSplit = Standard_False;
  Handle(TColStd_HSequenceOfReal) split = new TColStd_HSequenceOfReal;
  
  for(TopoDS_Iterator iter(face); iter.More()&&!doSplit; iter.Next()) {
    if(iter.Value().ShapeType() != TopAbs_WIRE)
      continue;
    TopoDS_Wire wire = TopoDS::Wire(iter.Value());
    Handle(ShapeExtend_WireData) sewd = new ShapeExtend_WireData(wire);
    for(Standard_Integer i = 1; i <= sewd->NbEdges()&&!doSplit; i++)
      if(sewd->IsSeam(i)) {
	doSplit = Standard_True;
	TopoDS_Edge edge = sewd->Edge(i);
	ShapeAnalysis_Edge sae;
	Handle(Geom2d_Curve) c1, c2;
	Standard_Real f1,f2,l1,l2;
	if(!sae.PCurve(edge,face,c1,f1,l1,Standard_False))
	  continue;
//smh#8
	TopoDS_Shape tmpE = edge.Reversed();
	if(!sae.PCurve(TopoDS::Edge(tmpE),face,c2,f2,l2,Standard_False))
	  continue;
	if(c2==c1) continue;
	// splitting
	ShapeAnalysis_Curve sac;
	Bnd_Box2d B1, B2;
	sac.FillBndBox ( c1, f1, l1, 20, Standard_True, B1 );
	sac.FillBndBox ( c2, f2, l2, 20, Standard_True, B2 );
	Standard_Real x1min,y1min,x1max,y1max;
	Standard_Real x2min,y2min,x2max,y2max;
	B1.Get(x1min,y1min,x1max,y1max);
	B2.Get(x2min,y2min,x2max,y2max);
	Standard_Real xf,xl,yf,yl;
	if(x1min < x2min) {
	  xf = x1max;
	  xl = x2min;
	} else {
	  xf = x2max;
	  xl = x1min;
	}
	if(y1min < y2min) {
	  yf = y1max;
	  yl = y2min;
	} else {
	  yf = y2max;
	  yl = y1min;
	}
	
	Standard_Real dU = xl - xf;
	Standard_Real dV = yl - yf;
	if(dU > dV) {
	  Standard_Real step = dU/(myNbSplit+1);
	  Standard_Real val = xf+step;
	  for(Standard_Integer j = 1; j <= myNbSplit; j++, val+=step)
	    split->Append(val);
	  isUSplit = Standard_True;
	}
	else {
	  Standard_Real step = dV/(myNbSplit+1);
	  Standard_Real val = yf+step;
	  for(Standard_Integer j = 1; j <= myNbSplit; j++, val+=step)
	    split->Append(val);
	  isUSplit = Standard_False;
	}
      }
  }
  
  if(!doSplit) {
    //pdn try to define geometric closure.
    Handle(ShapeAnalysis_Surface) sas = new ShapeAnalysis_Surface( surf );
    Standard_Boolean uclosed = sas->IsUClosed(Precision());
    Standard_Boolean vclosed = sas->IsVClosed(Precision());
    Standard_Real U1, U2, V1, V2;
    if(uclosed) {
      surf -> Bounds(U1, U2, V1, V2);
      GeomAdaptor_Surface GAS ( surf );
      Standard_Real toler = GAS.UResolution ( Precision() );
      if((U2-U1) - (Ul-Uf) < toler ) {
	Handle(Geom_RectangularTrimmedSurface) rts = 
	  new Geom_RectangularTrimmedSurface(surf,U1,(U2+U1)/2,Standard_True);
	Handle(ShapeAnalysis_Surface) sast = new ShapeAnalysis_Surface( rts  );
	if ( !sast->IsUClosed(Precision())) {
	  doSplit = Standard_True;
	  Standard_Real step = (Ul-Uf)/(myNbSplit+1);
	  Standard_Real val = Uf+step;
	  for(Standard_Integer i = 1; i <= myNbSplit; i++, val+=step)
	    split->Append(val);
	  isUSplit = Standard_True;
	}
#ifdef OCCT_DEBUG
	else cout << "Warning: SU_ClosedFaceDivide: Thin face, not splitted" << endl;
#endif
      }
    }
    if( vclosed && !doSplit ) {
      surf -> Bounds(U1, U2, V1, V2);
      GeomAdaptor_Surface GAS ( surf );
      Standard_Real toler = GAS.VResolution ( Precision() );
      if((V2-V1) - (Vl-Vf) < toler) {
	Handle(Geom_RectangularTrimmedSurface) rts = 
	  new Geom_RectangularTrimmedSurface(surf,V1,(V2+V1)/2,Standard_False);
	Handle(ShapeAnalysis_Surface) sast = new ShapeAnalysis_Surface( rts  );
	if ( !sast->IsVClosed(Precision())) {
	  doSplit = Standard_True;
	  Standard_Real step = (Vl-Vf)/(myNbSplit+1);
	  Standard_Real val = Vf+step;
	  for(Standard_Integer i = 1; i <= myNbSplit; i++, val+=step)
	    split->Append(val);
	  isUSplit = Standard_False;
	}
#ifdef OCCT_DEBUG
	else cout << "Warning: SU_ClosedFaceDivide: Thin face, not splitted" << endl;
#endif
      }
    }
  }
  
  if(!doSplit)
    return Standard_False;
    
  SplitSurf->Init ( surf, Uf, Ul, Vf, Vl );
  if(isUSplit)
    SplitSurf->SetUSplitValues(split);
  else
    SplitSurf->SetVSplitValues(split);
  
  SplitSurf->Perform(mySegmentMode);
  if ( ! SplitSurf->Status ( ShapeExtend_DONE ) ) return Standard_False;
  Handle(ShapeExtend_CompositeSurface) Grid = SplitSurf->ResSurfaces();
  
  ShapeFix_ComposeShell CompShell;
  CompShell.Init( Grid, L, face, Precision() );
  CompShell.SetMaxTolerance(MaxTolerance());
  CompShell.SetContext(Context());
  CompShell.Perform();
  if ( CompShell.Status ( ShapeExtend_FAIL ) || 
      ! CompShell.Status ( ShapeExtend_DONE ) ) 
    myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_FAIL2 );
  
  TopoDS_Shape res = CompShell.Result();
  myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE2 );
  for(TopExp_Explorer exp(res, TopAbs_FACE); exp.More(); exp.Next()) {
//smh#8
    TopoDS_Shape tempf = Context()->Apply(exp.Current());
    TopoDS_Face f = TopoDS::Face(tempf);
    myResult = f;
    if(SplitSurface())
      Context()->Replace(f,myResult);
  }
  myResult = Context()->Apply(res);
  return Standard_True;
}

    
//=======================================================================
//function : SetFaceNumber
//purpose  : 
//=======================================================================

void ShapeUpgrade_ClosedFaceDivide::SetNbSplitPoints(const Standard_Integer num)
{
  if(num > 0)
    myNbSplit = num;
}

//=======================================================================
//function : GetNbSplitPoints
//purpose  : 
//=======================================================================

Standard_Integer ShapeUpgrade_ClosedFaceDivide::GetNbSplitPoints() const
{
  return myNbSplit;
}
