// Created on: 1995-02-23
// Created by: Remi LEQUETTE
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

// **************************************************************
// Modif : DFO 05/11/96

#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <Draw_Display.hxx>
#include <Draw_Drawable3D.hxx>
#include <Draw_Grid.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <Draw_Text2D.hxx>
#include <Draw_Text3D.hxx>
#include <Message.hxx>
#include <Standard_Macro.hxx>
#include <iostream>
#include <iomanip>
#include <fstream>

#include <cstdio>

#ifndef M_SQRT2
  #define M_SQRT2 1.41421356237309504880168872420969808
#endif

#ifdef _WIN32
extern Draw_Viewer dout;
extern bool        Draw_Batch;
#endif

extern bool Draw_BlackBackGround;

#define DEFROTATE (5 * M_PI / 180.)
#define DEFMAGNIFY 1.1
#define DEFPANNING 0.1
#define DEFFOCAL 1.1
#define DEFFRAME 10
#define DEFGRIDSTEP 100.0
static double steprot         = DEFROTATE;
static double steppan         = DEFPANNING;
static double stepmagnify     = DEFMAGNIFY;
static double stepfocal       = DEFFOCAL;
static double frame           = DEFFRAME;
static double DefaultGridStep = DEFGRIDSTEP;

#define FONTLENGTH
static char Draw_fontname[FONTLENGTH]        = "Helvetica";
static char Draw_fontsize[FONTLENGTH]        = "150";
static char Draw_fontnamedefault[FONTLENGTH] = "Helvetica";
static char Draw_fontsizedefault[FONTLENGTH] = "150";

// *******************************************************************
// Graphic commands
// *******************************************************************

static int ViewId(const char* a)
{
  int id = Draw::Atoi(a);
  if ((id < 0) || (id >= MAXVIEW))
  {
    std::cout << "Incorrect view-id, must be in 0.." << MAXVIEW - 1 << std::endl;
    return -1;
  }
  if (!dout.HasView(id))
  {
    std::cout << "View " << id << " does not exist." << std::endl;
    return -1;
  }
  return id;
}

static void SetTitle(const int id)
{
  if (dout.HasView(id))
  {
    char title[255];
    Sprintf(title, "%d : %s - Zoom %f", id, dout.GetType(id), dout.Zoom(id));
    dout.SetTitle(id, title);
  }
}

//=================================================================================================

static int zoom(Draw_Interpretor&, int n, const char** a)
{
  // one argument -> All Views
  // two argument -> First is the view
  bool z2d = !strcasecmp(a[0], "2dzoom");
  if (n == 2)
  {
    double z = Draw::Atof(a[1]);
    for (int id = 0; id < MAXVIEW; id++)
    {
      if (dout.HasView(id))
      {
        if ((z2d && !dout.Is3D(id)) || (!z2d && dout.Is3D(id)))
        {
          dout.SetZoom(id, z);
          SetTitle(id);
          dout.RepaintView(id);
        }
      }
    }
    return 0;
  }
  else if (n >= 3)
  {
    int id = ViewId(a[1]);
    if (id < 0)
      return 1;
    double z = Draw::Atof(a[2]);
    dout.SetZoom(id, z);
    dout.RepaintView(id);
    SetTitle(id);
    return 0;
  }
  else
    return 1;
}

//=================================================================================================

static int wzoom(Draw_Interpretor& di, int argc, const char** argv)
{
  int    id, X, Y, W, H, X1, Y1, X2 = 0, Y2 = 0, b;
  double dX1, dY1, dX2, dY2, zx, zy;
  if (argc != 1 && argc != 6)
  {
    di << "Usage : " << argv[0] << " [view-id X1 Y1 X2 Y2]\n";
    return 1;
  }
  if (argc == 1)
  {
    di << "Pick first corner\n";
    dout.Select(id, X1, Y1, b);

    gp_Trsf T;
    gp_Pnt  P0(0, 0, 0);
    dout.GetTrsf(id, T);
    T.Invert();
    P0.Transform(T);
    double z = dout.Zoom(id);

    dX1 = X1;
    dY1 = Y1;
    dX1 -= P0.X();
    dY1 -= P0.Y();
    dX1 /= z;
    dY1 /= z;

    if (b != 1)
      return 0;
    if (id < 0)
      return 0;
    Draw_Display d = dout.MakeDisplay(id);
    d.SetColor(Draw_blanc);
    d.SetMode(10);
    double dOX2 = dX1;
    double dOY2 = dY1;
    d.Draw(gp_Pnt2d(dX1, dY1), gp_Pnt2d(dX1, dOY2));
    d.Draw(gp_Pnt2d(dX1, dOY2), gp_Pnt2d(dOX2, dOY2));
    d.Draw(gp_Pnt2d(dOX2, dOY2), gp_Pnt2d(dOX2, dY1));
    d.Draw(gp_Pnt2d(dOX2, dY1), gp_Pnt2d(dX1, dY1));
    d.Flush();
    dout.GetPosSize(id, X, Y, W, H);
    di << "Pick second corner\n";
    b = 0;
    while (b == 0)
    {
      dout.Select(id, X2, Y2, b, false);
      dX2 = X2;
      dY2 = Y2;
      dX2 -= P0.X();
      dY2 -= P0.Y();
      dX2 /= z;
      dY2 /= z;

      d.Draw(gp_Pnt2d(dX1, dY1), gp_Pnt2d(dX1, dOY2));
      d.Draw(gp_Pnt2d(dX1, dOY2), gp_Pnt2d(dOX2, dOY2));
      d.Draw(gp_Pnt2d(dOX2, dOY2), gp_Pnt2d(dOX2, dY1));
      d.Draw(gp_Pnt2d(dOX2, dY1), gp_Pnt2d(dX1, dY1));
      d.Draw(gp_Pnt2d(dX1, dY1), gp_Pnt2d(dX1, dY2));
      d.Draw(gp_Pnt2d(dX1, dY2), gp_Pnt2d(dX2, dY2));
      d.Draw(gp_Pnt2d(dX2, dY2), gp_Pnt2d(dX2, dY1));
      d.Draw(gp_Pnt2d(dX2, dY1), gp_Pnt2d(dX1, dY1));
      d.Flush();
      dOX2 = dX2;
      dOY2 = dY2;
    }
    d.Draw(gp_Pnt2d(dX1, dY1), gp_Pnt2d(dX1, dOY2));
    d.Draw(gp_Pnt2d(dX1, dOY2), gp_Pnt2d(dOX2, dOY2));
    d.Draw(gp_Pnt2d(dOX2, dOY2), gp_Pnt2d(dOX2, dY1));
    d.Draw(gp_Pnt2d(dOX2, dY1), gp_Pnt2d(dX1, dY1));
    d.Flush();
    if (b != 1)
      return 0;
    d.SetMode(0);
  }
  else
  {
    id = atoi(argv[1]);
    if ((id < 0) || (id >= MAXVIEW))
    {
      Message::SendFail() << "Incorrect view-id, must be in 0.." << (MAXVIEW - 1);
      return 1;
    }
    if (!dout.HasView(id))
    {
      Message::SendFail() << "View " << id << " does not exist";
      return 1;
    }
    X1 = atoi(argv[2]);
    Y1 = atoi(argv[3]);
    X2 = atoi(argv[4]);
    Y2 = atoi(argv[5]);

    dout.GetPosSize(id, X, Y, W, H);
  }

  if ((X1 == X2) || (Y1 == Y2))
    return 0;
  zx = (double)std::abs(X2 - X1) / (double)W;
  zy = (double)std::abs(Y2 - Y1) / (double)H;
  if (zy > zx)
    zx = zy;
  zx = 1 / zx;
  if (X2 < X1)
    X1 = X2;
  if (Y2 > Y1)
    Y1 = Y2;
  X1 = (int)(X1 * zx);
  Y1 = (int)(Y1 * zx);
  dout.SetZoom(id, zx * dout.Zoom(id));
  dout.SetPan(id, -X1, -Y1);
  dout.RepaintView(id);
  SetTitle(id);
  return 0;
}

//=================================================================================================

static int wclick(Draw_Interpretor& di, int, const char**)
{
  int id1, X1, Y1, b;
  dout.Flush();
  di << "Just click.\n";
  dout.Select(id1, X1, Y1, b);
  return 0;
}

//=================================================================================================

static int view(Draw_Interpretor& di, int n, const char** a)
{
  if (Draw_Batch)
    return 1;

  if ((n >= 3) && (n != 4))
  {
    int id = Draw::Atoi(a[1]);
    if ((id < 0) || (id >= MAXVIEW))
    {
      di << "View-id must be in 0.." << MAXVIEW - 1 << "\n";
      return 1;
    }
    int X = 0;
    int Y = 0;
    int W = 500;
    int H = 500;
    // if view exist, get old values
    if (dout.HasView(id))
      dout.GetPosSize(id, X, Y, W, H);
    if (n >= 4)
      X = Draw::Atoi(a[3]);
    if (n >= 5)
      Y = Draw::Atoi(a[4]);
    if (n >= 6)
      W = Draw::Atoi(a[5]);
    if (n >= 7)
      H = Draw::Atoi(a[6]);
    dout.MakeView(id, a[2], X, Y, W, H);
    if (!dout.HasView(id))
    {
      di << "View creation failed\n";
      return 1;
    }
    SetTitle(id);
    dout.DisplayView(id);
    return 0;
  }
  else if (n == 4)
  {
    // create a view on a given window
    int id = Draw::Atoi(a[1]);
    if ((id < 0) || (id >= MAXVIEW))
    {
      di << "View-id must be in 0.." << MAXVIEW - 1 << "\n";
      return 1;
    }
    dout.MakeView(id, a[2], a[3]);
    if (!dout.HasView(id))
    {
      di << "View creation failed\n";
      return 1;
    }
    SetTitle(id);
    dout.DisplayView(id);
    return 0;
  }
  else
    return 1;
}

//=================================================================================================

static int delview(Draw_Interpretor&, int n, const char** a)
{
  if (n == 1)
  {
    for (int id = 0; id < MAXVIEW; id++)
      dout.DeleteView(id);
    return 0;
  }
  else if (n >= 2)
  {
    int id = ViewId(a[1]);
    if (id < 0)
      return 1;
    dout.DeleteView(id);
    return 0;
  }
  else
    return 1;
}

//=================================================================================================

static int fit(Draw_Interpretor&, int n, const char** a)
{
  bool f2d = !strcasecmp(a[0], "2dfit");
  if (n == 1)
  {
    double zoom = RealLast();
    int    id;
    for (id = 0; id < MAXVIEW; id++)
    {
      if (dout.HasView(id))
      {
        if ((f2d && !dout.Is3D(id)) || (!f2d && dout.Is3D(id)))
        {
          //	  dout.FitView(id,frame);
          dout.FitView(id, (int)frame);
          if (dout.Zoom(id) < zoom)
            zoom = dout.Zoom(id);
        }
      }
    }
    for (id = 0; id < MAXVIEW; id++)
    {
      if (dout.HasView(id))
      {
        if ((f2d && !dout.Is3D(id)) || (!f2d && dout.Is3D(id)))
        {
          dout.SetZoom(id, zoom);
          dout.RepaintView(id);
          SetTitle(id);
        }
      }
    }
    return 0;
  }
  else if (n >= 2)
  {
    int id = ViewId(a[1]);
    if (id < 0)
      return 1;
    //    dout.FitView(id,frame);
    dout.FitView(id, (int)frame);
    dout.RepaintView(id);
    SetTitle(id);
    return 0;
  }
  else
    return 1;
}

//=================================================================================================

static int focal(Draw_Interpretor&, int n, const char** a)
{
  int start = 0;
  int end   = MAXVIEW - 1;
  if (n >= 2)
  {
    int anid = ViewId(a[1]);
    if (anid < 0)
      return 1;
    start = end = anid;
  }
  double df = 1.;
  if (!strcasecmp(a[0], "fu"))
    df = stepfocal;
  if (!strcasecmp(a[0], "fd"))
    df = 1. / stepfocal;

  for (int id = start; id <= end; id++)
  {
    if (!strcasecmp(dout.GetType(id), "PERS"))
    {
      dout.SetFocal(id, dout.Focal(id) * df);
      dout.RepaintView(id);
    }
  }
  return 0;
}

//=================================================================================================

static int setfocal(Draw_Interpretor& di, int n, const char** a)
{
  if (n == 1)
  {
    for (int id = 0; id < MAXVIEW; id++)
    {
      if (!strcasecmp(dout.GetType(id), "PERS"))
        di << "Focal view " << id << " is " << dout.Focal(id) << "\n";
    }
  }
  else
  {
    double f = Draw::Atof(a[1]);
    for (int id = 0; id < MAXVIEW; id++)
    {
      if (!strcasecmp(dout.GetType(id), "PERS"))
        dout.SetFocal(id, f);
    }
    dout.RepaintAll();
  }
  return 0;
}

//=================================================================================================

// static int magnify(Draw_Interpretor& di, int n, const char** a)
static int magnify(Draw_Interpretor&, int n, const char** a)
{
  int start = 0;
  int end   = MAXVIEW - 1;
  if (n >= 2)
  {
    int anid = ViewId(a[1]);
    if (anid < 0)
      return 1;
    start = end = anid;
  }
  bool        v2d = (a[0][0] == '2'); // 2dmu, 2dmd
  const char* com = a[0];
  if (v2d)
    com += 2;
  double dz = 1.;
  if (!strcasecmp(com, "mu")) // mu, 2dmu
    dz = stepmagnify;
  else // md, 2dmd
    dz = 1 / stepmagnify;

  for (int id = start; id <= end; id++)
  {
    if (dout.HasView(id))
    {
      if ((v2d && !dout.Is3D(id)) || (!v2d && dout.Is3D(id)))
      {
        dout.SetZoom(id, dout.Zoom(id) * dz);
        SetTitle(id);
        dout.RepaintView(id);
      }
    }
  }
  return 0;
}

Standard_EXPORT int Draw_magnify(Draw_Interpretor& di, int n, const char** a)
{
  return magnify(di, n, a);
}

//=================================================================================================

static int rotate(Draw_Interpretor&, int n, const char** a)
{
  int start = 0;
  int end   = MAXVIEW - 1;
  if (n >= 2)
  {
    int anid = ViewId(a[1]);
    if (anid < 0)
      return 1;
    start = end = anid;
  }

  gp_Dir2d D;
  double   ang = 0;
  if (!strcasecmp(a[0], "u"))
  {
    D.SetCoord(1., 0.);
    ang = -steprot;
  }
  if (!strcasecmp(a[0], "d"))
  {
    D.SetCoord(1., 0.);
    ang = steprot;
  }
  if (!strcasecmp(a[0], "l"))
  {
    D.SetCoord(0., 1.);
    ang = -steprot;
  }
  if (!strcasecmp(a[0], "r"))
  {
    D.SetCoord(0., 1.);
    ang = steprot;
  }

  for (int id = start; id <= end; id++)
  {
    if ((!strcasecmp(dout.GetType(id), "AXON")) || (!strcasecmp(dout.GetType(id), "PERS")))
    {
      dout.RotateView(id, D, ang);
      dout.RepaintView(id);
    }
  }
  return 0;
}

//=================================================================================================

static int panning(Draw_Interpretor&, int n, const char** a)
{
  int start = 0;
  int end   = MAXVIEW - 1;
  if (n >= 2)
  {
    int anid = ViewId(a[1]);
    if (anid < 0)
      return 1;
    start = end = anid;
  }
  int DX = 0;
  int DY = 0;
  int X, Y, W, H;

  bool        v2d = (a[0][0] == '2'); // pu2d, pd2d, pr2d, pl2d
  const char* com = a[0];
  if (v2d)
    com += 2;

  if (!strcasecmp(com, "pu")) // pu , 2dpu
    DY = 1;
  if (!strcasecmp(com, "pd")) // pd , 2dpd
    DY = -1;
  if (!strcasecmp(com, "pl")) // pl , 2dpl
    DX = -1;
  if (!strcasecmp(com, "pr")) // pr , 2dpr
    DX = 1;

  for (int id = start; id <= end; id++)
  {
    if (dout.HasView(id))
    {
      if ((v2d && !dout.Is3D(id)) || (!v2d && dout.Is3D(id)))
      {
        dout.GetPosSize(id, X, Y, W, H);
        //	dout.PanView(id,W * DX * steppan, H * DY * steppan);
        dout.PanView(id, (int)(W * DX * steppan), (int)(H * DY * steppan));
        dout.RepaintView(id);
      }
    }
  }
  return 0;
}

//=================================================================================================

static int ptv(Draw_Interpretor&, int n, const char** a)
{
  double X, Y, Z;
  int    start = 0;
  int    end   = MAXVIEW - 1;
  if (n < 4)
    return 1;
  if (n >= 5)
  {
    int anid = ViewId(a[1]);
    if (anid < 0)
      return 1;
    start = end = anid;
    X           = Draw::Atof(a[2]);
    Y           = Draw::Atof(a[3]);
    Z           = Draw::Atof(a[4]);
  }
  else
  {
    X = Draw::Atof(a[1]);
    Y = Draw::Atof(a[2]);
    Z = Draw::Atof(a[3]);
  }

  for (int id = start; id <= end; id++)
  {
    gp_Trsf T;
    dout.GetTrsf(id, T);
    T.SetTranslationPart(gp_Vec(0, 0, 0));
    gp_Trsf T1;
    T1.SetTranslationPart(gp_Vec(-X, -Y, -Z));
    gp_Trsf aLocalTrsf(T * T1);
    dout.SetTrsf(id, aLocalTrsf);
    //    dout.SetTrsf(id,T*T1);
    dout.RepaintView(id);
  }
  return 0;
}

//=================================================================================================

static int dptv(Draw_Interpretor&, int n, const char** a)
{
  double DX, DY, DZ;
  int    start = 0;
  int    end   = MAXVIEW - 1;
  if (n < 4)
    return 1;
  if (n >= 5)
  {
    int anid = ViewId(a[1]);
    if (anid < 0)
      return 1;
    start = end = anid;
    DX          = Draw::Atof(a[2]);
    DY          = Draw::Atof(a[3]);
    DZ          = Draw::Atof(a[4]);
  }
  else
  {
    DX = Draw::Atof(a[1]);
    DY = Draw::Atof(a[2]);
    DZ = Draw::Atof(a[3]);
  }

  for (int id = start; id <= end; id++)
  {
    gp_Trsf T;
    dout.GetTrsf(id, T);
    gp_Trsf T1;
    T1.SetTranslationPart(gp_Vec(-DX, -DY, -DZ));
    gp_Trsf M = T * T1;
    dout.SetTrsf(id, M);
    dout.RepaintView(id);
  }
  return 0;
}

//=================================================================================================

static int color(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 3)
  {
    Draw_BlackBackGround = !Draw_BlackBackGround;
  }
  else if (!dout.DefineColor(Draw::Atoi(a[1]), a[2]))
  {
    di << "Could not allocate color " << a[2] << "\n";
    return 1;
  }
  return 0;
}

//=======================================================================
// function : hardcopy
// purpose  : hardcopy                  --> hardcopy of view 1
//                                                  in file a4.ps
//                                                  in format a4
//           hardcopy name view        --> hardcopy of view <view>
//                                                  in file <name>
//                                                  in format a4
//           hardcopy name view format --> hardcopy of view <view>
//                                                  in file <name>
//                                                  in format <a4,a3,a2,a1,a0>
//=======================================================================

static int hardcopy(Draw_Interpretor&, int n, const char** a)
{
  // Inch = 25.40001969 mm.
  // 28.4 pixels / mm.
  // format par default papier a4 210 297 mm avec marge de 3 mm.

  double rap = 28.4;
  double cad = 3;
  double dx  = 210;
  double dy  = 210 * std::sqrt(2.);

  int         iview = 1;
  const char* file  = "a4.ps";
  if (n >= 2)
  {
    file = a[1];
    if (n >= 3)
    {
      iview = ViewId(a[2]);
      if (iview < 0)
        return 1;
      if (n >= 4)
      {
        if (!strcmp(a[3], "a7"))
        {
          cad = cad / (2 * M_SQRT2);
          dx  = dx / (2 * M_SQRT2);
          dy  = dy / (2 * M_SQRT2);
        }
        else if (!strcmp(a[3], "a6"))
        {
          cad = cad / 2;
          dx  = dx / 2;
          dy  = dy / 2;
        }
        else if (!strcmp(a[3], "a5"))
        {
          cad = cad / M_SQRT2;
          dx  = dx / M_SQRT2;
          dy  = dy / M_SQRT2;
        }
        else if (!strcmp(a[3], "a4"))
        {
          // Do nothing
          // cad == cad;
          // dx  == dx;
          // dy  == dy;
        }
        else if (!strcmp(a[3], "a3"))
        {
          cad = cad * M_SQRT2;
          dx  = dx * M_SQRT2;
          dy  = dy * M_SQRT2;
        }
        else if (!strcmp(a[3], "a2"))
        {
          cad = cad * 2;
          dx  = dx * 2;
          dy  = dy * 2;
        }
        else if (!strcmp(a[3], "a1"))
        {
          cad = cad * 2 * M_SQRT2;
          dx  = dx * 2 * M_SQRT2;
          dy  = dy * 2 * M_SQRT2;
        }
        else if (!strcmp(a[3], "a0"))
        {
          cad = cad * 4;
          dx  = dx * 4;
          dy  = dy * 4;
        }
      }
    }
  }

  int pxmin = (int)(cad * rap);
  int pymin = (int)(cad * rap);
  int pxmax = (int)((dx - cad) * rap);
  int pymax = (int)((dy - cad) * rap);

  std::ofstream os(file);

  int vxmin, vymin, vxmax, vymax;
  if (dout.HasView(iview))
  {
    dout.GetFrame(iview, vxmin, vymin, vxmax, vymax);
    double kx = (double)(pxmax - pxmin) / (vxmax - vxmin);
    double ky = (double)(pymax - pymin) / (vymax - vymin);
    double k  = std::min(std::abs(kx), std::abs(ky));
    kx        = (kx > 0) ? k : -k;
    ky        = (ky > 0) ? k : -k;
    pxmax     = (int)(pxmin + kx * (vxmax - vxmin));
    pymax     = (int)(pymin + ky * (vymax - vymin));

    // si on veut choisir l'orientation : 90 rotate

    // ecriture du header

    os << "%!PS-Adobe-3.0 EPSF-3.0\n";
    os << "%%BoundingBox: " << pxmin << " " << pymin << " " << pxmax << " " << pymax << "\n";
    os << "%%Pages: 1\n";
    os << "%%DocumentFonts: " << Draw_fontname << "\n";
    os << "%%EndComments\n";

    os << "/" << Draw_fontname << " findfont\n" << Draw_fontsize << " scalefont\nsetfont\n";
    os << "/m {moveto} bind def\n";
    os << "/l {lineto} bind def\n";
    os << ".1 .1 scale\n";

    // draw the frame

    os << "3 setlinewidth\n0 setgray\nnewpath\n";
    os << pxmin << " " << pymin << " m\n";
    os << pxmax << " " << pymin << " l\n";
    os << pxmax << " " << pymax << " l\n";
    os << pxmin << " " << pymax << " l\n";
    os << "closepath\nstroke\n";

    // frame the view

    os << "newpath\n";
    os << pxmin << " " << pymin << " m\n";
    os << pxmax << " " << pymin << " l\n";
    os << pxmax << " " << pymax << " l\n";
    os << pxmin << " " << pymax << " l\n";
    os << "closepath\nclip\n";

    dout.PostScriptView(iview, vxmin, vymin, vxmax, vymax, pxmin, pymin, pxmax, pymax, os);
    os << "showpage\n";
    os << "%%EOF\n";
  }

  return 0;
}

static int dfont(Draw_Interpretor& di, int n, const char** a)
{
  if (n == 1)
  {
    strcpy(Draw_fontname, Draw_fontnamedefault);
    strcpy(Draw_fontsize, Draw_fontsizedefault);
  }
  else if (n == 2)
  {
    strcpy(Draw_fontname, a[1]);
  }
  else if (n == 3)
  {
    strcpy(Draw_fontname, a[1]);
    strcpy(Draw_fontsize, a[2]);
  }
  di << Draw_fontname << " " << Draw_fontsize << "\n";
  return 0;
} // dfont

//=================================================================================================

static int hcolor(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 4)
  {
    di << "code de couleur (Draw.cxx) : \n";
    di << "0 = White,\t 1 = Red,\t 2 = Green,\t 3 = Blue\n";
    di << "4 = Cyan,\t 5 = Gold,\t 6 = Magenta,\t 7 = Maroon" << "\n";
    di << "8 = Orange,\t 9 = Pink,\t 10 = Salmon,\t 11 = Violet\n";
    di << "12 = Yellow,\t 13 = Khaki,\t 14 = Coral\n";
    di << "1 <= width <= 11,  0 (noir)  <= gray <= 1 (blanc)\n";
  }
  else
  {
    dout.PostColor(Draw::Atoi(a[1]), Draw::Atoi(a[2]), Draw::Atof(a[3]));
  }
  return 0;
}

//=================================================================================================

extern void Draw_RepaintNowIfNecessary();

static int xwd(Draw_Interpretor&, int n, const char** a)
{
  if (n < 2)
    return 1;

  // enforce repaint if necessary
  Draw_RepaintNowIfNecessary();

  int         id   = 1;
  const char* file = a[1];
  if (n > 2)
  {
    id   = Draw::Atoi(a[1]);
    file = a[2];
  }
  if (!dout.SaveView(id, file))
    return 1;

  return 0;
}

//=======================================================================
// function : grid
// purpose  : Creation/Suppression d'une grille.
//=======================================================================

static int grid(Draw_Interpretor&, int NbArg, const char** Arg)
{
  double StepX, StepY, StepZ;

  switch (NbArg)
  {
    case 1:
      StepX = DefaultGridStep;
      StepY = DefaultGridStep;
      StepZ = DefaultGridStep;
      break;
    case 2:
      StepX = std::abs(Draw::Atof(Arg[1]));
      StepY = std::abs(Draw::Atof(Arg[1]));
      StepZ = std::abs(Draw::Atof(Arg[1]));
      break;
    case 3:
      StepX = std::abs(Draw::Atof(Arg[1]));
      StepY = std::abs(Draw::Atof(Arg[2]));
      StepZ = std::abs(Draw::Atof(Arg[2]));
      break;
    case 4:
      StepX = std::abs(Draw::Atof(Arg[1]));
      StepY = std::abs(Draw::Atof(Arg[2]));
      StepZ = std::abs(Draw::Atof(Arg[3]));
      break;
    default:
      return 1;
  }

#ifdef HPUX
  const char* temp = "grid";
#else
  char        temp1[] = "grid";
  const char* temp    = temp1;
#endif
  occ::handle<Draw_Grid> Grille = occ::down_cast<Draw_Grid>(Draw::Get(temp));

  Grille->Steps(StepX, StepY, StepZ);
  dout.RepaintAll();

  return 0;
}

//=================================================================================================

static int dflush(Draw_Interpretor&, int, const char**)
{
  dout.Flush();
  return 0;
}

//=================================================================================================

static int dtext(Draw_Interpretor& di, int n, const char** a)
{
  gp_Pnt P;
  bool   is3d;
  if (n == 2)
  {
    int id, X, Y, b;
    di << "Pick position with button 1, other button escape\n";
    dout.Select(id, X, Y, b);
    if (b != 1)
      return 0;
    double z = dout.Zoom(id);
    P.SetCoord((double)X / z, (double)Y / z, 0);
    gp_Trsf T;
    dout.GetTrsf(id, T);
    T.Invert();
    P.Transform(T);
    is3d = dout.Is3D(id);
  }
  else if (n >= 4)
  {
    is3d = n > 4;
    P.SetCoord(Draw::Atof(a[1]), Draw::Atof(a[2]), is3d ? Draw::Atof(a[3]) : 0);
  }
  else
    return 0;

  if (is3d)
  {
    occ::handle<Draw_Text3D> D = new Draw_Text3D(P, a[n - 1], Draw_vert);
    dout << D;
  }
  else
  {
    occ::handle<Draw_Text2D> D = new Draw_Text2D(gp_Pnt2d(P.X(), P.Y()), a[n - 1], Draw_vert);
    dout << D;
  }
  return 0;
}

void Draw::GraphicCommands(Draw_Interpretor& theCommands)
{
  static bool Done = false;
  if (Done)
    return;
  Done = true;

  const char* g = "DRAW Graphic Commands";
  theCommands.Add("wclick", "wait for a mouse click", __FILE__, wclick, g);
  theCommands.Add("zoom", "zoom [view-id] z, or zoom z for all 3d views", __FILE__, zoom, g);
  theCommands.Add("2dzoom", "2dzoom [view-id] z, or zoom2d z for all 2d views", __FILE__, zoom, g);
  theCommands.Add("wzoom",
                  "wzoom [view-id X1 Y1 X2 Y2]\n"
                  "- fits the contents of a given rectangle into a view window.\n"
                  "- The view window and rectangle corners are specified through the arguments\n"
                  "- or selected interactively by the user if no arguments are given",
                  __FILE__,
                  wzoom,
                  g);
  theCommands.Add("view", "view view-id type X(0) Y(0) W(500) H(500)", __FILE__, view, g);
  theCommands.Add("delete", "delete [view-id]", __FILE__, delview, g);
  theCommands.Add("fit", "fit [view-id]", __FILE__, fit, g);
  theCommands.Add("2dfit", "2dfit [view-id]", __FILE__, fit, g);
  theCommands.Add("fu", "fu [view-id], focal up", __FILE__, focal, g);
  theCommands.Add("fd", "fd [view-id], focal down", __FILE__, focal, g);
  theCommands.Add("focal", "focal [f]", __FILE__, setfocal, g);
  theCommands.Add("mu", "mu [view-id], magnify up", __FILE__, magnify, g);
  theCommands.Add("2dmu", "2dmu [view-id], magnify up", __FILE__, magnify, g);
  theCommands.Add("md", "md [view-id], magnify down", __FILE__, magnify, g);
  theCommands.Add("2dmd", "2dmd [view-id], magnify down", __FILE__, magnify, g);
  theCommands.Add("u", "u [view-id], rotate up", __FILE__, rotate, g);
  theCommands.Add("d", "d [view-id], rotate down", __FILE__, rotate, g);
  theCommands.Add("l", "l [view-id], rotate left", __FILE__, rotate, g);
  theCommands.Add("r", "r [view-id], rotate right", __FILE__, rotate, g);
  theCommands.Add("pu", "pu [view-id], panning up", __FILE__, panning, g);
  theCommands.Add("pd", "pd [view-id], panning down", __FILE__, panning, g);
  theCommands.Add("pl", "pl [view-id], panning left", __FILE__, panning, g);
  theCommands.Add("pr", "pr [view-id], panning right", __FILE__, panning, g);
  theCommands.Add("2dpu", "2dpu [view-id], panning up", __FILE__, panning, g);
  theCommands.Add("2dpd", "2dpd [view-id], panning down", __FILE__, panning, g);
  theCommands.Add("2dpl", "2dpl [view-id], panning left", __FILE__, panning, g);
  theCommands.Add("2dpr", "2dpr [view-id], panning right", __FILE__, panning, g);
  theCommands.Add("ptv", "ptv [view-id], X , Y , Z", __FILE__, ptv, g);
  theCommands.Add("dptv", "dptv [view-id], dX , dY , dZ", __FILE__, dptv, g);
  theCommands.Add("color", "color i colorname, define color i", __FILE__, color, g);
  theCommands.Add("hardcopy",
                  "hardcopy [file = a4.ps] [view-id = 1] [format = a4]",
                  __FILE__,
                  hardcopy,
                  g);
  theCommands.Add("xwd",
                  "xwd [id = 1] <filename>.{png|bmp|jpg|gif}\n\t\t: Dump contents of viewer window "
                  "to PNG, BMP, JPEG or GIF file",
                  __FILE__,
                  xwd,
                  g);
  theCommands.Add("hcolor", "hcolor icol width gray (< 1, 0 black)", __FILE__, hcolor, g);
  theCommands.Add("grid", "grid [stepX(100) [stepY [stepZ]]] / 0", __FILE__, grid, g);
  theCommands.Add("dflush", "dflush, flush the viewer", __FILE__, dflush, g);
  theCommands.Add("dtext", "dtext [x y [z]] string", __FILE__, dtext, g);
  theCommands.Add("dfont",
                  "dfont [name size] : set name and size of Draw font, or reset to default",
                  __FILE__,
                  dfont,
                  g);
}
