// Created on: 1994-09-21
// Created by: Laurent BOURESCHE
// Copyright (c) 1994-1999 Matra Datavision
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

#include <cstdio>

#include <TopOpeBRepDS_DataStructure.hxx>
#include <ChFiDS_SurfData.hxx>
#include <TopOpeBRepDS_Surface.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>
#include <Geom_Surface.hxx>
#include <Geom2d_Line.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>

#ifdef OCCT_DEBUG
  #include <OSD_Chronometer.hxx>
OSD_Chronometer simul, elspine, chemine;
#endif

//*********************************
// timing of the simulation
//*********************************

static bool ChFi3d_traceCHRON = false;

void ChFi3d_SettraceCHRON(const bool b)
{
  ChFi3d_traceCHRON = b;
}

bool ChFi3d_GettraceCHRON()
{
  return ChFi3d_traceCHRON;
}

//*********************************
// trace a line of path
//*********************************

static bool ChFi3d_traceDRAWWALK = false;

void ChFi3d_SettraceDRAWWALK(const bool b)
{
  ChFi3d_traceDRAWWALK = b;
}

bool ChFi3d_GettraceDRAWWALK()
{
  return ChFi3d_traceDRAWWALK;
}

//**********************************
// trace a line of intersection
//**********************************

static bool ChFi3d_traceDRAWINT = false;

void ChFi3d_SettraceDRAWINT(const bool b)
{
  ChFi3d_traceDRAWINT = b;
}

bool ChFi3d_GettraceDRAWINT()
{
  return ChFi3d_traceDRAWINT;
}

//*************************************************
// return surfaces of approximated fillets.
//*************************************************

static bool ChFi3d_traceDRAWFIL = false;

void ChFi3d_SettraceDRAWFIL(const bool b)
{
  ChFi3d_traceDRAWFIL = b;
}

bool ChFi3d_GettraceDRAWFIL()
{
  return ChFi3d_traceDRAWFIL;
}

//*************************************************
// return extended faces for the path.
//*************************************************

static bool ChFi3d_traceDRAWENLARGE = false;

void ChFi3d_SettraceDRAWENLARGE(const bool b)
{
  ChFi3d_traceDRAWENLARGE = b;
}

bool ChFi3d_GettraceDRAWENLARGE()
{
  return ChFi3d_traceDRAWENLARGE;
}

//*************************************************
// return the guideline for the triple corners.
//*************************************************

static bool ChFi3d_traceDRAWSPINE = false;

void ChFi3d_SettraceDRAWSPINE(const bool b)
{
  ChFi3d_traceDRAWSPINE = b;
}

bool ChFi3d_GettraceDRAWSPINE()
{
  return ChFi3d_traceDRAWSPINE;
}

//*************************************************
// set the type of guideline for the triple corners.
//*************************************************

void ChFi3d_SetcontextSPINEBEZIER(const bool b);
void ChFi3d_SetcontextSPINECIRCLE(const bool b);
void ChFi3d_SetcontextSPINECE(const bool b);

static bool ChFi3d_contextSPINEBEZIER = false;

void ChFi3d_SetcontextSPINEBEZIER(const bool b)
{
  ChFi3d_contextSPINEBEZIER = b;
  if (b)
  {
    ChFi3d_SetcontextSPINECIRCLE(false);
    ChFi3d_SetcontextSPINECE(false);
  }
}

bool ChFi3d_GetcontextSPINEBEZIER()
{
  return ChFi3d_contextSPINEBEZIER;
}

static bool ChFi3d_contextSPINECIRCLE = false;

void ChFi3d_SetcontextSPINECIRCLE(const bool b)
{
  ChFi3d_contextSPINECIRCLE = b;
  if (b)
  {
    ChFi3d_SetcontextSPINEBEZIER(false);
    ChFi3d_SetcontextSPINECE(false);
  }
}

bool ChFi3d_GetcontextSPINECIRCLE()
{
  return ChFi3d_contextSPINECIRCLE;
}

static bool ChFi3d_contextSPINECE = false;

void ChFi3d_SetcontextSPINECE(const bool b)
{
  ChFi3d_contextSPINECE = b;
  if (b)
  {
    ChFi3d_SetcontextSPINEBEZIER(false);
    ChFi3d_SetcontextSPINECIRCLE(false);
  }
}

bool ChFi3d_GetcontextSPINECE()
{
  return ChFi3d_contextSPINECE;
}

//*************************************************
// Forced passage by the path for KPart
//*************************************************
static bool ChFi3d_contextFORCEBLEND = false;

void ChFi3d_SetcontextFORCEBLEND(const bool b)
{
  ChFi3d_contextFORCEBLEND = b;
}

bool ChFi3d_GetcontextFORCEBLEND()
{
  return ChFi3d_contextFORCEBLEND;
}

static bool ChFi3d_contextFORCEFILLING = false;

void ChFi3d_SetcontextFORCEFILLING(const bool b)
{
  ChFi3d_contextFORCEFILLING = b;
}

bool ChFi3d_GetcontextFORCEFILLING()
{
  return ChFi3d_contextFORCEFILLING;
}

//*************************************************
// No optimization for approx
//*************************************************
static bool ChFi3d_contextNOOPT = false;

void ChFi3d_SetcontextNOOPT(const bool b)
{
  ChFi3d_contextNOOPT = b;
}

bool ChFi3d_GetcontextNOOPT()
{
  return ChFi3d_contextNOOPT;
}

#ifdef OCCT_DEBUG
// ***********************************************
//    initialization and result of a chrono
//************************************************
Standard_EXPORT void ChFi3d_InitChron(OSD_Chronometer& ch)
{
  ch.Reset();
  ch.Start();
}

Standard_EXPORT void ChFi3d_ResultChron(OSD_Chronometer& ch, double& time)
{
  double tch;
  ch.Stop();
  ch.Show(tch);
  time = time + tch;
}
#endif

//==============================================================
// function : ChFi3d_CheckSurfData
// purpose  : function allows to trace SurfData to check
//            construction of all elements, namely pcurves
//==============================================================
void ChFi3d_CheckSurfData(const TopOpeBRepDS_DataStructure&   DStr,
                          const occ::handle<ChFiDS_SurfData>& Data)
{
  // trace of the surface defined by the chamfer or the fillet
  //  corresponding to SurfData

  occ::handle<Geom_Surface> surf = (DStr.Surface(Data->Surf())).Surface();
  if (!surf.IsNull())
  {
    BRep_Builder B;
    TopoDS_Face  F;
    B.MakeFace(F, surf, 0.);
    TopoDS_Wire W;
    B.MakeWire(W);

    TopoDS_Vertex V1, V2, V3, V4;
    B.MakeVertex(V1, Data->VertexFirstOnS1().Point(), 0.);
    B.MakeVertex(V2, Data->VertexLastOnS1().Point(), 0.);
    B.MakeVertex(V3, Data->VertexLastOnS2().Point(), 0.);
    B.MakeVertex(V4, Data->VertexFirstOnS2().Point(), 0.);

    TopoDS_Edge E1, E2, E3, E4;
    B.MakeEdge(E1);
    B.MakeEdge(E2);
    B.MakeEdge(E3);
    B.MakeEdge(E4);

    B.UpdateEdge(E1, Data->InterferenceOnS1().PCurveOnSurf(), F, 0.);
    B.UpdateEdge(E3, Data->InterferenceOnS2().PCurveOnSurf(), F, 0.);
    V1.Orientation(TopAbs_FORWARD);
    B.Add(E1, V1);
    B.UpdateVertex(V1, Data->InterferenceOnS1().FirstParameter(), E1, 0.);
    V2.Orientation(TopAbs_REVERSED);
    B.Add(E1, V2);
    B.UpdateVertex(V2, Data->InterferenceOnS1().LastParameter(), E1, 0.);

    V4.Orientation(TopAbs_FORWARD);
    B.Add(E3, V4);
    B.UpdateVertex(V4, Data->InterferenceOnS2().FirstParameter(), E3, 0.);
    V3.Orientation(TopAbs_REVERSED);
    B.Add(E3, V3);
    B.UpdateVertex(V3, Data->InterferenceOnS2().LastParameter(), E3, 0.);

    gp_Pnt2d pp1, pp2, pp3, pp4;
    pp1 = Data->InterferenceOnS1().PCurveOnSurf()->Value(Data->InterferenceOnS1().FirstParameter());
    pp2 = Data->InterferenceOnS1().PCurveOnSurf()->Value(Data->InterferenceOnS1().LastParameter());
    pp3 = Data->InterferenceOnS2().PCurveOnSurf()->Value(Data->InterferenceOnS2().LastParameter());
    pp4 = Data->InterferenceOnS2().PCurveOnSurf()->Value(Data->InterferenceOnS2().FirstParameter());
    gp_Dir2d                 d1(gp_Vec2d(pp1, pp4));
    gp_Dir2d                 d2(gp_Vec2d(pp2, pp3));
    occ::handle<Geom2d_Line> l1 = new Geom2d_Line(pp1, d1);
    occ::handle<Geom2d_Line> l2 = new Geom2d_Line(pp2, d2);

    B.UpdateEdge(E4, l1, F, 0.);
    V1.Orientation(TopAbs_FORWARD);
    B.Add(E4, V1);
    B.UpdateVertex(V1, 0., E4, 0.);
    V4.Orientation(TopAbs_REVERSED);
    B.Add(E4, V4);
    B.UpdateVertex(V4, pp4.Distance(pp1), E4, 0.);

    B.UpdateEdge(E2, l2, F, 0.);
    V2.Orientation(TopAbs_FORWARD);
    B.Add(E2, V2);
    B.UpdateVertex(V2, 0., E2, 0.);
    V3.Orientation(TopAbs_REVERSED);
    B.Add(E2, V3);
    B.UpdateVertex(V3, pp3.Distance(pp2), E2, 0.);

    gp_Pnt pw1, pw2, ppp;
    ppp = surf->Value(pp1.X(), pp1.Y());
    pw1 = surf->Value(0.9 * pp1.X() + 0.1 * pp2.X(), 0.9 * pp1.Y() + 0.1 * pp2.Y());
    pw2 = surf->Value(0.9 * pp1.X() + 0.1 * pp4.X(), 0.9 * pp1.Y() + 0.1 * pp4.Y());
    gp_Vec vv1(ppp, pw1);
    gp_Vec vv2(ppp, pw2);
    gp_Vec Vwire = vv1 ^ vv2;

    surf->D1(pp1.X(), pp1.Y(), pw1, vv1, vv2);
    gp_Vec Vsurf = vv1 ^ vv2;
    bool   rev   = Vsurf.Dot(Vwire) <= 0.;

    E1.Orientation(TopAbs_FORWARD);
    E2.Orientation(TopAbs_FORWARD);
    E3.Orientation(TopAbs_REVERSED);
    E4.Orientation(TopAbs_REVERSED);
    if (rev)
    {
      E1.Orientation(TopAbs_REVERSED);
      E2.Orientation(TopAbs_REVERSED);
      E3.Orientation(TopAbs_FORWARD);
      E4.Orientation(TopAbs_FORWARD);
    }
    B.Add(W, E1);
    B.Add(W, E2);
    B.Add(W, E3);
    B.Add(W, E4);

    W.Orientation(TopAbs_FORWARD);
    B.Add(F, W);
  }
}
