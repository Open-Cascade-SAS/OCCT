// Created on: 1995-07-24
// Created by: Modelistation
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


#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_IsoCurve.hxx>
#include <Bnd_Box.hxx>
#include <BndLib_AddSurface.hxx>
#include <GeomAbs_IsoType.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_Group.hxx>
#include <Precision.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <StdPrs_DeflectionCurve.hxx>
#include <StdPrs_WFDeflectionSurface.hxx>

static void FindLimits(const Handle(Adaptor3d_HSurface)& surf ,
		       const Standard_Real    aLimit,
		       Standard_Real&         UFirst,
		       Standard_Real&         ULast,
		       Standard_Real&         VFirst,
		       Standard_Real&         VLast)
{
  UFirst = surf->FirstUParameter();
  ULast = surf->LastUParameter();
  VFirst = surf->FirstVParameter();
  VLast = surf->LastVParameter();
  
  Standard_Boolean UfirstInf = Precision::IsNegativeInfinite(UFirst);
  Standard_Boolean UlastInf  = Precision::IsPositiveInfinite(ULast);
  Standard_Boolean VfirstInf = Precision::IsNegativeInfinite(VFirst);
  Standard_Boolean VlastInf  = Precision::IsPositiveInfinite(VLast);
  
  if (UfirstInf || UlastInf) {
    gp_Pnt P1,P2;
    Standard_Real v;
    if (VfirstInf && VlastInf) 
      v = 0;
    else if (VfirstInf)
      v = VLast;
    else if (VlastInf)
      v = VFirst;
    else
      v = (VFirst + VLast) / 2;
    
    Standard_Real delta = aLimit * 2;

    if (UfirstInf && UlastInf) {
      do {
	delta /= 2;
	UFirst = - delta;
	ULast  =   delta;
	surf->D0(UFirst,v,P1);
	surf->D0(ULast,v,P2);
      } while (P1.Distance(P2) > aLimit);
    }
    else if (UfirstInf) {
      surf->D0(ULast,v,P2);
      do {
	delta /= 2;
	UFirst = ULast - delta;
	surf->D0(UFirst,v,P1);
      } while (P1.Distance(P2) > aLimit);
    }
    else if (UlastInf) {
      surf->D0(UFirst,v,P1);
      do {
	delta /= 2;
	ULast = UFirst + delta;
	surf->D0(ULast,v,P2);
      } while (P1.Distance(P2) > aLimit);
    }
  }

  if (VfirstInf || VlastInf) {
    gp_Pnt P1,P2;
    Standard_Real u = (UFirst + ULast) /2 ;

    Standard_Real delta = aLimit * 2;

    if (VfirstInf && VlastInf) {
      do {
	delta /= 2;
	VFirst = - delta;
	VLast  =   delta;
	surf->D0(u,VFirst,P1);
	surf->D0(u,VLast,P2);
      } while (P1.Distance(P2) > aLimit);
    }
    else if (VfirstInf) {
      surf->D0(u,VLast,P2);
      do {
	delta /= 2;
	VFirst = VLast - delta;
	surf->D0(u,VFirst,P1);
      } while (P1.Distance(P2) > aLimit);
    }
    else if (VlastInf) {
      surf->D0(u,VFirst,P1);
      do {
	delta /= 2;
	VLast = VFirst + delta;
	surf->D0(u,VLast,P2);
      } while (P1.Distance(P2) > aLimit);
    }
  }



}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void StdPrs_WFDeflectionSurface::Add (
			      const Handle (Prs3d_Presentation)& aPresentation,
			      const Handle(Adaptor3d_HSurface)&    aSurface,
			      const Handle (Prs3d_Drawer)&       aDrawer)
{
    Standard_Real  U1, U2, V1, V2;
    Standard_Real MaxP = aDrawer->MaximalParameterValue();
    FindLimits(aSurface, MaxP, U1, U2, V1, V2);
    
    Standard_Boolean UClosed = aSurface->IsUClosed();
    Standard_Boolean VClosed = aSurface->IsVClosed();
      
    Standard_Real TheDeflection;
    Aspect_TypeOfDeflection TOD = aDrawer->TypeOfDeflection();    
    if (TOD == Aspect_TOD_RELATIVE) {
// On calcule la fleche en fonction des min max globaux de la piece:
       Bnd_Box Total;
       BndLib_AddSurface::Add(aSurface->Surface(),U1, U2, V1, V2, 0.,Total);
       Standard_Real m = aDrawer->MaximalChordialDeviation()/
	 aDrawer->DeviationCoefficient();
       Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
       Total.Get( aXmin, aYmin, aZmin, aXmax, aYmax, aZmax );
       if ( ! (Total.IsOpenXmin() || Total.IsOpenXmax() ))
	  m = Min ( m , Abs (aXmax-aXmin));
       if ( ! (Total.IsOpenYmin() || Total.IsOpenYmax() ))
	  m = Min ( m , Abs (aYmax-aYmin));
       if ( ! (Total.IsOpenZmin() || Total.IsOpenZmax() ))
	  m = Min ( m , Abs (aZmax-aZmin));

       TheDeflection = m * aDrawer->DeviationCoefficient();
    }
    else
      TheDeflection = aDrawer->MaximalChordialDeviation();  

    Adaptor3d_IsoCurve anIso;
    anIso.Load(aSurface);

    // Trace des frontieres.
    // *********************
    //
    if ( !(UClosed && VClosed) ) {
	
      (Prs3d_Root::CurrentGroup(aPresentation))->SetPrimitivesAspect
	(aDrawer->FreeBoundaryAspect()->Aspect());
      if ( !UClosed ) 
	{ 
	  anIso.Load(GeomAbs_IsoU,U1,V1,V2);
	  StdPrs_DeflectionCurve::Add(aPresentation,anIso,TheDeflection, MaxP);
	  anIso.Load(GeomAbs_IsoU,U2,V1,V2);
	  StdPrs_DeflectionCurve::Add(aPresentation,anIso,TheDeflection, MaxP);
	}
      if ( !VClosed )
	{
	  anIso.Load(GeomAbs_IsoV,V1,U1,U2);
	  StdPrs_DeflectionCurve::Add(aPresentation,anIso,TheDeflection, MaxP);
	  anIso.Load(GeomAbs_IsoV,V2,U1,U2);
	  StdPrs_DeflectionCurve::Add(aPresentation,anIso,TheDeflection, MaxP);
	}
    }
    //
    // Trace des isoparametriques.
    // ***************************
    //
    Standard_Integer fin = aDrawer->UIsoAspect()->Number();
    if ( fin != 0) {
      
      (Prs3d_Root::CurrentGroup(aPresentation))->SetPrimitivesAspect
	(aDrawer->UIsoAspect()->Aspect());
      
      Standard_Real du= UClosed ? (U2-U1)/fin : (U2-U1)/(1+fin);
      for (Standard_Integer i=1; i<=fin;i++){
	anIso.Load(GeomAbs_IsoU,U1+du*i,V1,V2);
	StdPrs_DeflectionCurve::Add(aPresentation,anIso,TheDeflection, MaxP);
      }
    }
    fin = aDrawer->VIsoAspect()->Number();
    if ( fin != 0) {
      
      (Prs3d_Root::CurrentGroup(aPresentation))->SetPrimitivesAspect
	(aDrawer->VIsoAspect()->Aspect());
      
      Standard_Real dv= VClosed ?(V2-V1)/fin : (V2-V1)/(1+fin);
      for (Standard_Integer i=1; i<=fin;i++){
	anIso.Load(GeomAbs_IsoV,V1+dv*i,U1,U2);
	StdPrs_DeflectionCurve::Add(aPresentation,anIso,TheDeflection, MaxP);
      }
    }
  }




