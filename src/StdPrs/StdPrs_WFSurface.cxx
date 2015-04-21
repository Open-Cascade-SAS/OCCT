// Created on: 1995-08-04
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
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_Group.hxx>
#include <Precision.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_NListIteratorOfListOfSequenceOfPnt.hxx>
#include <Prs3d_NListOfSequenceOfPnt.hxx>
#include <Prs3d_Presentation.hxx>
#include <StdPrs_Curve.hxx>
#include <StdPrs_WFSurface.hxx>
#include <TColgp_SequenceOfPnt.hxx>

static void FindLimits(const Handle(Adaptor3d_HSurface)& surf ,
		       const Standard_Real             aLimit,
		       Standard_Real&                  UFirst,
		       Standard_Real&                  ULast,
		       Standard_Real&                  VFirst,
		       Standard_Real&                  VLast)
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

void StdPrs_WFSurface::Add (const Handle(Prs3d_Presentation)& aPresentation,
			    const Handle(Adaptor3d_HSurface)&   aSurface,
			    const Handle(Prs3d_Drawer)&       aDrawer)
{
  
    Standard_Real  U1, U2, V1, V2;
    Standard_Real MaxP = aDrawer->MaximalParameterValue();

    FindLimits(aSurface, MaxP, U1, U2, V1, V2);

    Prs3d_NListOfSequenceOfPnt freeCurves; 
    Prs3d_NListOfSequenceOfPnt UIsoCurves; 
    Prs3d_NListOfSequenceOfPnt VIsoCurves; 
    
    Standard_Boolean UClosed = aSurface->IsUClosed();
    Standard_Boolean VClosed = aSurface->IsVClosed();

    Adaptor3d_IsoCurve anIso;
    anIso.Load(aSurface);

    // Trace des frontieres.
    // *********************
    //
    if (!(UClosed && VClosed))
    {
      Prs3d_Root::CurrentGroup (aPresentation)->SetPrimitivesAspect (aDrawer->FreeBoundaryAspect()->Aspect());
      if (!UClosed)
      {
        anIso.Load (GeomAbs_IsoU, U1, V1, V2);
        Handle(TColgp_HSequenceOfPnt) aPntsU1 = new TColgp_HSequenceOfPnt;
        StdPrs_Curve::Add (aPresentation, anIso, aDrawer, aPntsU1->ChangeSequence(), Standard_False);
        freeCurves.Append (aPntsU1);

        anIso.Load (GeomAbs_IsoU,U2,V1,V2);
        Handle(TColgp_HSequenceOfPnt) aPntsU2 = new TColgp_HSequenceOfPnt;
        StdPrs_Curve::Add (aPresentation, anIso, aDrawer, aPntsU2->ChangeSequence(), Standard_False);
        freeCurves.Append(aPntsU2);
      }
      if (!VClosed)
      {
        anIso.Load (GeomAbs_IsoV, V1, U1, U2);
        Handle(TColgp_HSequenceOfPnt) aPntsV1 = new TColgp_HSequenceOfPnt;
        StdPrs_Curve::Add (aPresentation, anIso, aDrawer, aPntsV1->ChangeSequence(), Standard_False);
        freeCurves.Append (aPntsV1);

        anIso.Load (GeomAbs_IsoV, V2, U1, U2);
        Handle(TColgp_HSequenceOfPnt) aPntsV2 = new TColgp_HSequenceOfPnt;
        StdPrs_Curve::Add (aPresentation, anIso, aDrawer, aPntsV2->ChangeSequence(), Standard_False);
        freeCurves.Append(aPntsV2);
      }
    }
    //
    // Trace des isoparametriques.
    // ***************************
    //
    Standard_Integer fin = aDrawer->UIsoAspect()->Number();
    if (fin != 0)
    {
      Prs3d_Root::CurrentGroup (aPresentation)->SetPrimitivesAspect (aDrawer->UIsoAspect()->Aspect());

      Standard_Real du= UClosed ? (U2-U1) / fin : (U2-U1) / (1 + fin);
      for (Standard_Integer i = 1; i <= fin; i++)
      {
        anIso.Load (GeomAbs_IsoU, U1 + du * i, V1, V2);
        Handle(TColgp_HSequenceOfPnt) Pnts = new TColgp_HSequenceOfPnt;
        StdPrs_Curve::Add (aPresentation, anIso, aDrawer, Pnts->ChangeSequence(), Standard_False);
        UIsoCurves.Append (Pnts);
      }
    }
    fin = aDrawer->VIsoAspect()->Number();
    if (fin != 0)
    {
      Prs3d_Root::CurrentGroup (aPresentation)->SetPrimitivesAspect (aDrawer->VIsoAspect()->Aspect());
      
      Standard_Real dv = VClosed ? (V2 - V1) / fin : (V2 - V1) / (1 + fin);
      for (Standard_Integer i = 1; i <= fin; i++)
      {
        anIso.Load (GeomAbs_IsoV, V1 + dv * i, U1, U2);
        Handle(TColgp_HSequenceOfPnt) Pnts = new TColgp_HSequenceOfPnt;
        StdPrs_Curve::Add (aPresentation, anIso, aDrawer, Pnts->ChangeSequence(), Standard_False);
        VIsoCurves.Append (Pnts);
      }
    }

    Standard_Integer nbVertices = 0, nbBounds = 0; 
    //Draw surface via primitive array
    if(UIsoCurves.Size() > 0) {
      nbBounds = UIsoCurves.Size();
      Prs3d_NListIteratorOfListOfSequenceOfPnt It;
      for( It.Init(UIsoCurves); It.More(); It.Next())
	nbVertices += It.Value()->Length();
      Handle(Graphic3d_ArrayOfPolylines) UIsoArray =
	new Graphic3d_ArrayOfPolylines(nbVertices,nbBounds);
      for( It.Init(UIsoCurves); It.More(); It.Next()) {
        const Handle(TColgp_HSequenceOfPnt)& Pnts = It.Value();
	UIsoArray->AddBound(Pnts->Length());
	for(int i=1; i<=Pnts->Length(); i++)
	  UIsoArray->AddVertex(Pnts->Value(i));
      }      
      Handle(Graphic3d_Group) TheGroup = Prs3d_Root::NewGroup(aPresentation);
      TheGroup->SetPrimitivesAspect(aDrawer->UIsoAspect()->Aspect());
      TheGroup->AddPrimitiveArray(UIsoArray);
    }
    
    if(VIsoCurves.Size() > 0) {
      nbBounds = VIsoCurves.Size();
      Prs3d_NListIteratorOfListOfSequenceOfPnt It;
      for( It.Init(VIsoCurves); It.More(); It.Next())
	nbVertices += It.Value()->Length();
      Handle(Graphic3d_ArrayOfPolylines) VIsoArray =
	new Graphic3d_ArrayOfPolylines(nbVertices,nbBounds);
      for( It.Init(VIsoCurves); It.More(); It.Next()) {
	const Handle(TColgp_HSequenceOfPnt)& Pnts = It.Value();
	VIsoArray->AddBound(Pnts->Length());
	for(int i=1; i<=Pnts->Length(); i++)
	  VIsoArray->AddVertex(Pnts->Value(i));
      }
      Handle(Graphic3d_Group) TheGroup = Prs3d_Root::NewGroup(aPresentation);
      TheGroup->SetPrimitivesAspect(aDrawer->VIsoAspect()->Aspect());
      TheGroup->AddPrimitiveArray(VIsoArray);
    } 
    if(freeCurves.Size() > 0) {
      nbBounds = freeCurves.Size();
      Prs3d_NListIteratorOfListOfSequenceOfPnt It;
      for( It.Init(freeCurves); It.More(); It.Next())
	nbVertices += It.Value()->Length();
      Handle(Graphic3d_ArrayOfPolylines) freeArray =
	new Graphic3d_ArrayOfPolylines(nbVertices,nbBounds);
      for( It.Init(freeCurves); It.More(); It.Next()) {
	const Handle(TColgp_HSequenceOfPnt)& Pnts = It.Value();
	freeArray->AddBound(Pnts->Length());
	for(int i=1; i<=Pnts->Length(); i++)
	  freeArray->AddVertex(Pnts->Value(i));
      }
      Handle(Graphic3d_Group) TheGroup = Prs3d_Root::NewGroup(aPresentation);
      TheGroup->SetPrimitivesAspect(aDrawer->FreeBoundaryAspect()->Aspect());
      TheGroup->AddPrimitiveArray(freeArray);
    }
  }


