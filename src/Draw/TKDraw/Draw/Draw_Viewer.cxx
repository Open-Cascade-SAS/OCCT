// Created on: 1992-04-06
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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

#include <Draw_Viewer.hxx>
#include <Draw_View.hxx>

#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Draw_Window.hxx>
#include <Draw_Display.hxx>

#define precpers 0.95
#define ButtonPress 4
#define MotionNotify 6
static const double  DRAWINFINITE = 1e50;
Standard_EXPORT bool Draw_Bounds  = true;
extern bool          Draw_Batch;
const int            MAXSEGMENT = 1000;
Draw_XSegment        segm[MAXSEGMENT];
static int           nbseg     = 0;
static Draw_View*    curview   = nullptr;
static int           curviewId = 0;
static char          blank[2]  = "";
static double        xmin, xmax, ymin, ymax;
static bool          found = false;
static int           xpick, ypick, precpick;
static gp_Pnt        lastPickP1;
static gp_Pnt        lastPickP2;
static double        lastPickParam;
static Draw_Color    highlightcol;
static Draw_Color    currentcolor;
static bool          highlight = false;
static int           ps_vx, ps_vy;
static double        ps_kx, ps_ky;
static int           ps_px, ps_py;
static std::ostream* ps_stream;
static int           ps_width[MAXCOLOR];
static double        ps_gray[MAXCOLOR];

enum DrawingMode
{
  DRAW,
  PICK,
  POSTSCRIPT
};

static DrawingMode CurrentMode = DRAW;

//=================================================================================================

Draw_Viewer::Draw_Viewer()
{
  if (Draw_Batch)
    return;
  int i;
  for (i = 0; i < MAXVIEW; i++)
    myViews[i] = nullptr;
  for (i = 0; i < MAXCOLOR; i++)
  {
    ps_width[i] = 1;
    ps_gray[i]  = 0;
  }
}

//=================================================================================================

bool Draw_Viewer::DefineColor(const int i, const char* colname)
{
  if (Draw_Batch)
    return true;
  return Draw_Window::DefineColor(i, colname);
}

//=================================================================================================

void Draw_Viewer::MakeView(const int   id,
                           const char* typ,
                           const int   X,
                           const int   Y,
                           const int   W,
                           const int   H)
{
  if (Draw_Batch)
    return;
  if (id < MAXVIEW)
  {

    DeleteView(id);
    myViews[id] = new Draw_View(id, this, X, Y, W, H);

    // View fields
    myViews[id]->SetDx(W / 2);
    myViews[id]->SetDy(-H / 2);

    if (!myViews[id]->Init(typ))
      DeleteView(id);

    RepaintView(id);
  }
}

#ifdef _WIN32
//=================================================================================================

void Draw_Viewer::MakeView(const int   id,
                           const char* typ,
                           const int   X,
                           const int   Y,
                           const int   W,
                           const int   H,
                           HWND        win,
                           bool        useBuffer)
{
  if (Draw_Batch)
    return;
  if (id < MAXVIEW)
  {

    DeleteView(id);
    myViews[id] = new Draw_View(id, this, X, Y, W, H, win);
    myViews[id]->SetUseBuffer(useBuffer);

    // View fields
    myViews[id]->SetDx(W / 2);
    myViews[id]->SetDy(-H / 2);

    if (!myViews[id]->Init(typ))
      DeleteView(id);
    RepaintView(id);
  }
}
#endif

//=================================================================================================

void Draw_Viewer::MakeView(const int id, const char* typ, const char* window)
{
  if (Draw_Batch)
    return;
  if (id < MAXVIEW)
  {

    DeleteView(id);
    myViews[id] = new Draw_View(id, this, window);

    myViews[id]->SetDx(myViews[id]->WidthWin() / 2);
    myViews[id]->SetDy(-myViews[id]->HeightWin() / 2);

    if (!myViews[id]->Init(typ))
      DeleteView(id);

    RepaintView(id);
  }
}

//=================================================================================================

void Draw_Viewer::SetTitle(const int id, const char* name)
{
  if (Draw_Batch)
    return;
  if (myViews[id])
    myViews[id]->SetTitle(name);
}

//=======================================================================
// function : ResetView
// purpose  : reset view zoom and axes
//=======================================================================

void Draw_Viewer::ResetView(const int id)
{
  if (Draw_Batch)
    return;
  if (myViews[id])
  {
    myViews[id]->Init(myViews[id]->Type());
    ConfigView(id);
  }
}

//=================================================================================================

void Draw_Viewer::SetZoom(const int id, const double z)
{
  if (Draw_Batch)
    return;

  Draw_View* aView = myViews[id];
  if (aView)
  {
    double zz = z / aView->GetZoom();
    aView->SetZoom(z);
    int X, Y, W, H;
    GetPosSize(id, X, Y, W, H);

    const double w = 0.5 * static_cast<double>(W);
    const double h = 0.5 * static_cast<double>(H);

    const int aDx = static_cast<int>(w - zz * (w - aView->GetDx()));
    const int aDy = static_cast<int>(-h + zz * (h + aView->GetDy()));

    aView->SetDx(aDx);
    aView->SetDy(aDy);
  }
}

//=================================================================================================

void Draw_Viewer::RotateView(const int id, const gp_Dir2d& D, const double A)
{
  if (Draw_Batch)
    return;
  if (myViews[id])
  {
    gp_Trsf T = myViews[id]->GetMatrix();

    T.Invert();
    gp_Pnt PP(0, 0, 0);
    gp_Dir DD(D.X(), D.Y(), 0);
    PP.Transform(T);
    DD.Transform(T);
    RotateView(id, PP, DD, A);
  }
}

//=================================================================================================

void Draw_Viewer::RotateView(const int id, const gp_Pnt& P, const gp_Dir& D, const double A)
{
  if (Draw_Batch)
    return;
  if (myViews[id])
  {
    gp_Trsf T;
    T.SetRotation(gp_Ax1(P, D), A);
    myViews[id]->Transform(T);
  }
}

//=================================================================================================

void Draw_Viewer::SetFocal(const int id, const double F)
{
  if (Draw_Batch)
    return;
  if (myViews[id])
    myViews[id]->SetFocalDistance(F);
}

//=================================================================================================

char* Draw_Viewer::GetType(const int id) const
{
  if (Draw_Batch)
    return blank;
  if (myViews[id])
    return const_cast<char*>(myViews[id]->Type());
  else
    return blank;
}

//=================================================================================================

double Draw_Viewer::Zoom(const int id) const
{
  if (Draw_Batch)
    return false;
  if (myViews[id])
    return myViews[id]->GetZoom();
  else
    return 0.0;
}

//=================================================================================================

double Draw_Viewer::Focal(const int id) const
{
  if (Draw_Batch)
    return 1.;
  if (myViews[id])
    return myViews[id]->GetFocalDistance();
  else
    return 0;
}

//=================================================================================================

void Draw_Viewer::GetTrsf(const int id, gp_Trsf& T) const
{
  if (Draw_Batch)
    return;
  if (myViews[id])
    T = myViews[id]->GetMatrix();
}

//=================================================================================================

bool Draw_Viewer::Is3D(const int id) const
{
  if (Draw_Batch)
    return false;
  if (myViews[id])
    return !myViews[id]->Is2D();
  else
    return false;
}

//=================================================================================================

void Draw_Viewer::SetTrsf(const int id, gp_Trsf& T)
{
  if (Draw_Batch)
    return;
  if (myViews[id])
    myViews[id]->SetMatrix(T);
}

//=================================================================================================

void Draw_Viewer::GetPosSize(const int id, int& X, int& Y, int& W, int& H)
{
  if (Draw_Batch)
    return;
  if (myViews[id] != nullptr)
  {
    myViews[id]->GetPosition(X, Y);
    W = myViews[id]->WidthWin();
    H = myViews[id]->HeightWin();
  }
}

//=================================================================================================

void Draw_Viewer::GetFrame(const int id, int& xminf, int& yminf, int& xmaxf, int& ymaxf)
{
  if (Draw_Batch)
    return;
  if (myViews[id])
  {
    int X, Y, H, W;
    GetPosSize(id, X, Y, W, H);
    xminf = -myViews[id]->GetDx();
    xmaxf = W - myViews[id]->GetDx();
    yminf = -myViews[id]->GetDy() - H;
    ymaxf = -myViews[id]->GetDy();
  }
}

//=================================================================================================

void Draw_Viewer::FitView(const int id, const int frame)
{
  if (Draw_Batch)
    return;
  if (myViews[id])
  {

    // is this the only view in its category
    bool is2d = myViews[id]->Is2D();
    int  i, nbviews = 0;
    for (i = 1; i < MAXVIEW; i++)
    {
      if (myViews[i])
      {
        if (myViews[i]->Is2D() == is2d)
          ++nbviews;
      }
    }
    bool only = (nbviews == 1);

    int X, Y, H, W;
    GetPosSize(id, X, Y, W, H);
    // compute the min max
    int n = myDrawables.Length();
    if (n == 0)
      return;
    //    Draw_Display DF;
    curview = myViews[id];
    double umin, umax, vmin, vmax;
    double u1, u2, v1, v2;
    umin = vmin = DRAWINFINITE;
    umax = vmax = -DRAWINFINITE;

    for (i = 1; i <= n; i++)
    {
      bool d3d = myDrawables(i)->Is3D();
      if ((d3d && !is2d) || (!d3d && is2d))
      {
        // if this is not the only view recompute...
        if (!only)
          DrawOnView(id, myDrawables(i));
        myDrawables(i)->Bounds(u1, u2, v1, v2);
        if (u1 < umin)
          umin = u1;
        if (u2 > umax)
          umax = u2;
        if (v1 < vmin)
          vmin = v1;
        if (v2 > vmax)
          vmax = v2;
      }
    }
    double z;
    umin = umin / curview->GetZoom();
    vmin = vmin / curview->GetZoom();
    umax = umax / curview->GetZoom();
    vmax = vmax / curview->GetZoom();
    if ((umax - umin) < 1.e-6)
    {
      if ((vmax - vmin) < 1.e-6)
        return;
      else
        z = ((double)(H - 2 * frame)) / (vmax - vmin);
    }
    else
    {
      z = ((double)(W - 2 * frame)) / ((double)(umax - umin));
      if ((vmax - vmin) > 1.e-6)
      {
        double z2 = ((double)(H - 2 * frame)) / (vmax - vmin);
        if (z2 < z)
          z = z2;
      }
    }
    curview->SetZoom(z);
    curview->SetDx(static_cast<int>(W / 2 - 0.5 * (umin + umax) * z));
    curview->SetDy(static_cast<int>(-H / 2 - 0.5 * (vmin + vmax) * z));
  }
}

//=================================================================================================

void Draw_Viewer::PanView(const int id, const int DX, const int DY)
{
  if (Draw_Batch)
    return;
  if (myViews[id])
  {
    myViews[id]->SetDx(myViews[id]->GetDx() + DX);
    myViews[id]->SetDy(myViews[id]->GetDy() + DY);
  }
}

//=================================================================================================

void Draw_Viewer::SetPan(const int id, const int DX, const int DY)
{
  if (Draw_Batch)
    return;
  if (myViews[id])
  {
    myViews[id]->SetDx(DX);
    myViews[id]->SetDy(DY);
  }
}

//=================================================================================================

void Draw_Viewer::GetPan(const int id, int& DX, int& DY)
{
  if (Draw_Batch)
    return;
  if (myViews[id])
  {
    DX = myViews[id]->GetDx();
    DY = myViews[id]->GetDy();
  }
}

//=================================================================================================

bool Draw_Viewer::HasView(const int id) const
{
  if (Draw_Batch)
    return false;
  if ((id < 0) || id >= MAXVIEW)
    return false;
  return myViews[id] != nullptr;
}

//=================================================================================================

void Draw_Viewer::DisplayView(const int id) const
{
  if (Draw_Batch)
    return;
  if (myViews[id])
    myViews[id]->DisplayWindow();
}

//=================================================================================================

void Draw_Viewer::HideView(const int id) const
{
  if (Draw_Batch)
    return;
  if (myViews[id])
  {
    //
  }
}

//=================================================================================================

void Draw_Viewer::ClearView(const int id) const
{
  if (Draw_Batch)
    return;
  if (myViews[id])
    myViews[id]->Clear();
}

//=================================================================================================

void Draw_Viewer::RemoveView(const int id)
{
  if (Draw_Batch)
    return;
  delete myViews[id];
  myViews[id] = nullptr;
}

//=================================================================================================

void Draw_Viewer::RepaintView(const int id) const
{
  if (Draw_Batch)
    return;
  if (myViews[id])
  {
    ClearView(id);
    int n = myDrawables.Length();
    for (int i = 1; i <= n; i++)
      DrawOnView(id, myDrawables(i));
  }
}

#ifdef _WIN32
//=======================================================================
// function : ResizeView
// purpose  : WNT re-drawing optimization
//=======================================================================
void Draw_Viewer::ResizeView(const int id) const
{
  if (Draw_Batch)
    return;
  if (myViews[id] && myViews[id]->GetUseBuffer())
  {
    myViews[id]->InitBuffer();
    RepaintView(id);
  }
}

//=======================================================================
// function : UpdateView
// purpose  : WNT re-drawing optimization
//=======================================================================
void Draw_Viewer::UpdateView(const int id, const bool forced) const
{
  if (Draw_Batch)
    return;
  if (myViews[id])
  {
    if (!myViews[id]->GetUseBuffer() || forced)
    {
      RepaintView(id);
    }
    // Fast redrawing on WNT
    if (myViews[id]->GetUseBuffer())
      myViews[id]->Redraw();
  }
}
#endif

//=================================================================================================

void Draw_Viewer::ConfigView(const int id) const
{
  if (Draw_Batch)
    return;
  if (myViews[id])
  {
    myViews[id]->SetDx(myViews[id]->WidthWin() / 2);
    myViews[id]->SetDy(-myViews[id]->HeightWin() / 2);
  }
}

//=================================================================================================

void Draw_Viewer::PostScriptView(const int     id,
                                 const int     VXmin,
                                 const int     VYmin,
                                 const int     VXmax,
                                 const int     VYmax,
                                 const int     PXmin,
                                 const int     PYmin,
                                 const int     PXmax,
                                 const int     PYmax,
                                 std::ostream& sortie) const
{
  if (Draw_Batch)
    return;
  if (myViews[id])
  {
    ps_vx     = VXmin;
    ps_vy     = VYmin;
    ps_px     = PXmin;
    ps_py     = PYmin;
    ps_kx     = ((double)(PXmax - PXmin)) / ((double)(VXmax - VXmin));
    ps_ky     = ((double)(PYmax - PYmin)) / ((double)(VYmax - VYmin));
    ps_stream = &sortie;
    int n     = myDrawables.Length();
    if (n == 0)
      return;
    CurrentMode         = POSTSCRIPT;
    Draw_Display DF     = MakeDisplay(id);
    bool         view2d = myViews[id]->Is2D();
    for (int i = 1; i <= n; i++)
      if (myDrawables(i)->Is3D())
      {
        if (!view2d)
          myDrawables(i)->DrawOn(DF);
      }
      else
      {
        if (view2d)
          myDrawables(i)->DrawOn(DF);
      }
    sortie << "stroke\n";
    CurrentMode = DRAW;
  }
}

//=================================================================================================

void Draw_Viewer::PostColor(const int icol, const int width, const double gray)
{
  if (Draw_Batch)
    return;
  if ((icol < 0) || (icol >= MAXCOLOR))
    return;
  ps_width[icol] = width;
  ps_gray[icol]  = gray;
}

//=================================================================================================

bool Draw_Viewer::SaveView(const int id, const char* filename)
{
  if (Draw_Batch)
  {
    return false;
  }
  Flush();
  if (myViews[id])
  {
    return myViews[id]->Save(filename);
  }
  else
  {
    std::cerr << "View " << id << " doesn't exists!\n";
    return false;
  }
}

//=================================================================================================

void Draw_Viewer::RepaintAll() const
{
  if (Draw_Batch)
    return;
  for (int id = 0; id < MAXVIEW; id++)
    RepaintView(id);
}

//=================================================================================================

void Draw_Viewer::Repaint2D() const
{
  if (Draw_Batch)
    return;
  for (int id = 0; id < MAXVIEW; id++)
    if (myViews[id])
    {
      if (myViews[id]->Is2D())
        RepaintView(id);
    }
}

//=================================================================================================

void Draw_Viewer::Repaint3D() const
{
  if (Draw_Batch)
    return;
  for (int id = 0; id < MAXVIEW; id++)
    if (myViews[id])
    {
      if (!myViews[id]->Is2D())
        RepaintView(id);
    }
}

//=================================================================================================

void Draw_Viewer::DeleteView(const int id)
{
  if (Draw_Batch)
    return;
  delete myViews[id];
  myViews[id] = nullptr;
}

//=================================================================================================

void Draw_Viewer::Clear()
{
  if (Draw_Batch)
    return;
  for (int i = 1; i <= myDrawables.Length(); i++)
    myDrawables(i)->Visible(false);
  myDrawables.Clear();
  for (int id = 0; id < MAXVIEW; id++)
    ClearView(id);
}

//=================================================================================================

void Draw_Viewer::Clear2D()
{
  if (Draw_Batch)
    return;
  int i = 1;
  while (i <= myDrawables.Length())
  {
    if (myDrawables(i)->Is3D())
      i++;
    else
    {
      myDrawables(i)->Visible(false);
      myDrawables.Remove(i);
    }
  }
  for (int id = 0; id < MAXVIEW; id++)
  {
    if (myViews[id])
    {
      if (myViews[id]->Is2D())
        ClearView(id);
    }
  }
}

//=================================================================================================

void Draw_Viewer::Clear3D()
{
  if (Draw_Batch)
    return;
  int i = 1;
  while (i <= myDrawables.Length())
  {
    if (myDrawables(i)->Is3D())
    {
      myDrawables(i)->Visible(false);
      myDrawables.Remove(i);
    }
    else
      i++;
  }
  for (int id = 0; id < MAXVIEW; id++)
  {
    if (myViews[id])
    {
      if (!myViews[id]->Is2D())
        ClearView(id);
    }
  }
}

//=================================================================================================

void Draw_Viewer::Flush()
{
  if (Draw_Batch)
    return;
  Draw_Window::Flush();
}

//=================================================================================================

void Draw_Viewer::DrawOnView(const int id, const occ::handle<Draw_Drawable3D>& D) const
{
  if (Draw_Batch)
    return;
  if (myViews[id])
  {
    Draw_Display d = MakeDisplay(id);
    xmin = ymin = DRAWINFINITE;
    xmax = ymax = -DRAWINFINITE;

    bool view2d = myViews[id]->Is2D();
    myViews[id]->ResetFrame();
    if ((D->Is3D() && !view2d) || (!D->Is3D() && view2d))
    {
      D->DrawOn(d);
      if (CurrentMode == DRAW)
        D->SetBounds(xmin, xmax, ymin, ymax);
      d.Flush();
    }
  }
}

//=================================================================================================

void Draw_Viewer::HighlightOnView(const int                           id,
                                  const occ::handle<Draw_Drawable3D>& D,
                                  const Draw_ColorKind                C) const
{
  if (Draw_Batch)
    return;
  highlight    = true;
  highlightcol = C;
  DrawOnView(id, D);
  highlight = false;
}

//=================================================================================================

void Draw_Viewer::AddDrawable(const occ::handle<Draw_Drawable3D>& D)
{
  if (Draw_Batch)
    return;
  if (!D.IsNull() && !D->Visible())
  {
    myDrawables.Append(D);
    D->Visible(true);
  }
}

//=================================================================================================

void Draw_Viewer::RemoveDrawable(const occ::handle<Draw_Drawable3D>& D)
{
  if (Draw_Batch)
    return;
  if (!D.IsNull() && D->Visible())
  {
    int index;
    for (index = 1; index <= myDrawables.Length(); index++)
    {
      if (myDrawables(index) == D)
      {
        D->Visible(false);
        myDrawables.Remove(index);
        return;
      }
    }
  }
}

//=======================================================================
// function : MakeDisplay
// purpose  : return a display on the view
//=======================================================================

Draw_Display Draw_Viewer::MakeDisplay(const int id) const
{
  if (Draw_Batch)
  {
    Draw_Display dis;
    return dis;
  }
  curviewId = id;
  curview   = myViews[id];
  nbseg     = 0;
  Draw_Color initcol(Draw_blanc);
  // to force setting the color
  currentcolor = Draw_Color(Draw_rouge);
  Draw_Display dis;
  dis.SetColor(initcol);
  dis.SetMode(0x3 /*GXcopy*/);
  return dis;
}

//=================================================================================================

void Draw_Viewer::Select(int& theId, int& theX, int& theY, int& theButton, bool theToWait)
{
  if (Draw_Batch)
  {
    return;
  }

  theId = theX = theY = theButton = 0;
  bool hasView                    = false;
  for (int aViewIter = 0; aViewIter < MAXVIEW; ++aViewIter)
  {
    if (myViews[aViewIter] != nullptr && myViews[aViewIter]->IsMapped())
    {
      hasView = true;
      break;
    }
  }
  if (!hasView)
  {
    std::cerr << "No selection is possible with no open views\n";
    return;
  }
  Flush();

#ifdef _WIN32
  HANDLE hWnd = NULL;

  theId = MAXVIEW; //: abv 29.05.02: cycle for working in console mode
  while (theId >= MAXVIEW)
  {
    if (theToWait)
    {
      Draw_Window::SelectWait(hWnd, theX, theY, theButton);
    }
    else
    {
      Draw_Window::SelectNoWait(hWnd, theX, theY, theButton);
    }

    // Recherche du numero de la vue grace au HANDLE
    for (int aViewIter = 0; aViewIter < MAXVIEW; ++aViewIter)
    {
      if (myViews[aViewIter] != NULL && myViews[aViewIter]->IsEqualWindows(hWnd))
      {
        theId = aViewIter;
      }
    }
  }
  theX = theX - myViews[theId]->GetDx();
  theY = -theY - myViews[theId]->GetDy();
#elif defined(HAVE_XLIB)
  if (!theToWait)
  {
    if (theId >= 0 && theId < MAXVIEW)
    {
      if (myViews[theId] != nullptr)
      {
        myViews[theId]->Wait(theToWait);
      }
    }
  }
  else
  {
    for (int aViewIter = 0; aViewIter < MAXVIEW; ++aViewIter)
    {
      if (myViews[aViewIter] != nullptr)
      {
        myViews[aViewIter]->Wait(theToWait);
      }
    }
  }

  bool again = true;
  while (again)
  {
    Draw_Window::Draw_XEvent ev;
    ev.type = 0;
    Draw_Window::GetNextEvent(ev);
    switch (ev.type)
    {
      case ButtonPress: {
        int aViewIter = 0;
        for (; aViewIter < MAXVIEW; ++aViewIter)
        {
          if (myViews[aViewIter] != nullptr && myViews[aViewIter]->IsEqualWindows(ev.window))
          {
            break;
          }
        }
        if (theToWait || theId == aViewIter)
        {
          if (aViewIter < MAXVIEW)
          {
            theId     = aViewIter;
            theX      = ev.x;
            theY      = ev.y;
            theButton = ev.button;
          }
          else
          {
            theId = -1;
          }
          again = false;
        }
        break;
      }
      case MotionNotify: {
        if (theToWait)
        {
          break;
        }
        theX      = ev.x;
        theY      = ev.y;
        theButton = 0;
        again     = false;
        break;
      }
    }
  }

  if (theId != -1)
  {
    theX = theX - myViews[theId]->GetDx();
    theY = -theY - myViews[theId]->GetDy();
  }
  if (!theToWait)
  {
    myViews[theId]->Wait(!theToWait);
  }
#elif defined(__APPLE__)
  theId = MAXVIEW;
  while (theId >= MAXVIEW)
  {
    long aWindowNumber = 0;
    Draw_Window::GetNextEvent(theToWait, aWindowNumber, theX, theY, theButton);
    if (theY < 0)
    {
      continue; // mouse clicked on window title
    }

    for (int aViewIter = 0; aViewIter < MAXVIEW; ++aViewIter)
    {
      if (myViews[aViewIter] != nullptr && myViews[aViewIter]->IsEqualWindows(aWindowNumber))
      {
        theId = aViewIter;
      }
    }
  }

  theX = theX - myViews[theId]->GetDx();
  theY = -theY - myViews[theId]->GetDy();
#else
  // not implemented
  (void)theToWait;
#endif
}

//=================================================================================================

int Draw_Viewer::Pick(const int                     id,
                      const int                     X,
                      const int                     Y,
                      const int                     Prec,
                      occ::handle<Draw_Drawable3D>& D,
                      const int                     first) const
{
  if (Draw_Batch)
    return 0;
  if (myViews[id] == nullptr)
    return 0;

  // is this the only view in its category
  bool is2d = myViews[id]->Is2D();
  int  i, nbviews = 0;
  for (i = 0; i < MAXVIEW; i++)
  {
    if (myViews[i])
      if (myViews[i]->Is2D() == is2d)
        ++nbviews;
  }
  bool only = (nbviews == 1);

  CurrentMode = PICK;
  xpick       = X;
  ypick       = Y;
  precpick    = Prec;
  found       = false;
  double x1, x2, y1, y2;
  for (i = first + 1; i <= myDrawables.Length(); i++)
  {
    bool reject = false;
    // rejection if only view
    if (only)
    {
      myDrawables(i)->Bounds(x1, x2, y1, y2);
      if ((xpick + Prec < x1) || (xpick - Prec > x2) || (ypick + Prec < y1) || (ypick - Prec > y2))
        reject = true;
    }
    if (!reject)
    {
      DrawOnView(id, myDrawables(i));
      if (found)
        break;
    }
  }
  CurrentMode = DRAW;
  found       = false;
  if (i <= myDrawables.Length())
    D = myDrawables(i);
  else
    i = 0;
  return i;
}

//=================================================================================================

void Draw_Viewer::LastPick(gp_Pnt& P1, gp_Pnt& P2, double& Param)
{
  if (Draw_Batch)
    return;
  P1    = lastPickP1;
  P2    = lastPickP2;
  Param = lastPickParam;
}

//=================================================================================================

Draw_Viewer::~Draw_Viewer()
{
  if (Draw_Batch)
    return;
  for (int id = 0; id < MAXVIEW; id++)
    DeleteView(id);
}

//=======================================================================
// function : operator<<
// purpose  :
//=======================================================================

Draw_Viewer& Draw_Viewer::operator<<(const occ::handle<Draw_Drawable3D>& d3d)
{
  if (Draw_Batch)
    return *this;
  if (!d3d.IsNull())
  {
    AddDrawable(d3d);
    for (int id = 0; id < MAXVIEW; id++)
      DrawOnView(id, d3d);
  }
  return *this;
}

//=================================================================================================

const NCollection_Sequence<occ::handle<Draw_Drawable3D>>& Draw_Viewer::GetDrawables()
{
  return myDrawables;
}

// *******************************************************************
// DISPLAY methods
// *******************************************************************

void Draw_Flush()
{
  if (Draw_Batch)
    return;
  if (highlight)
    curview->SetColor(highlightcol.ID());
  curview->DrawSegments(segm, nbseg);
  nbseg = 0;
}

//=================================================================================================

void Draw_Display::SetColor(const Draw_Color& col) const
{
  if (Draw_Batch)
    return;
  if (col.ID() == currentcolor.ID())
    return;

  currentcolor = col;
  switch (CurrentMode)
  {

    case DRAW:
      Draw_Flush();
      curview->SetColor(col.ID());
      break;

    case POSTSCRIPT:
      (*ps_stream) << "stroke\nnewpath\n";
      (*ps_stream) << ps_width[col.ID()] << " setlinewidth\n";
      (*ps_stream) << ps_gray[col.ID()] << " setgray\n";

    case PICK:
      break;
  }
}

//=================================================================================================

void Draw_Display::SetMode(const int M) const
{
  if (Draw_Batch)
    return;
  switch (CurrentMode)
  {

    case DRAW:
      Draw_Flush();
      curview->SetMode(M);
      break;

    case PICK:
    case POSTSCRIPT:
      break;
  }
}

//=================================================================================================

double Draw_Display::Zoom() const
{
  if (Draw_Batch)
    return 1.;
  return curview->GetZoom();
}

//=================================================================================================

void Draw_Display::Flush() const
{
  if (Draw_Batch)
    return;
  Draw_Flush();
}

//=================================================================================================

void Draw_Display::DrawString(const gp_Pnt2d&   ppt,
                              const char* const S,
                              const double      moveX,
                              const double      moveY)
{
  if (Draw_Batch)
    return;
  if (ppt.X() > 1.e09 || ppt.X() < -1.e09)
    return;
  if (ppt.Y() > 1.e09 || ppt.Y() < -1.e09)
    return;

  gp_Pnt2d pt(ppt.X() * curview->GetZoom(), ppt.Y() * curview->GetZoom());

  if (pt.X() > 1.e09 || pt.X() < -1.e09)
    return;
  if (pt.Y() > 1.e09 || pt.Y() < -1.e09)
    return;

  switch (CurrentMode)
  {

    case DRAW: {
      int X = (int)(pt.X() + moveX + curview->GetDx());
      int Y = (int)(-pt.Y() + moveY - curview->GetDy());
      curview->DrawString(X, Y, (char*)S);
      if (Draw_Bounds)
      {
        if (pt.X() + moveX > xmax)
          xmax = pt.X();
        if (pt.X() + moveX < xmin)
          xmin = pt.X();
        if (-pt.Y() - moveY > ymax)
          ymax = -pt.Y();
        if (-pt.Y() - moveY < ymin)
          ymin = -pt.Y();
      }
    }
    break;

    case POSTSCRIPT: {
      int x = (int)((pt.X() + moveX - ps_vx) * ps_kx + ps_px);
      int y = (int)((pt.Y() + moveY - ps_vy) * ps_ky + ps_py);
      (*ps_stream) << "stroke\n";
      (*ps_stream) << x << " " << y << " m\n";
      (*ps_stream) << "(" << S << ") show\nnewpath\n";
    }
    break;

    case PICK:
      break;
  }
}

//=================================================================================================

void Draw_Display::DrawString(const gp_Pnt2d& ppt, const char* const S)
{
  if (Draw_Batch)
    return;
  DrawString(ppt, S, 0.0, 0.0);
}

//=================================================================================================

void Draw_Display::DrawString(const gp_Pnt&     pt,
                              const char* const S,
                              const double      moveX,
                              const double      moveY)
{
  if (Draw_Batch)
    return;
  DrawString(Project(pt), S, moveX, moveY);
}

//=================================================================================================

void Draw_Display::DrawString(const gp_Pnt& pt, const char* const S)
{
  if (Draw_Batch)
    return;
  DrawString(Project(pt), S, 0.0, 0.0);
}

// *******************************************************************
// Drawing data static variables
// *******************************************************************
static gp_Pnt2d PtCur;  // current 2D point
static gp_Pnt   PtPers; // current 3D point for Pers

//=================================================================================================

void Draw_Display::Project(const gp_Pnt& p, gp_Pnt2d& p2d) const
{
  if (Draw_Batch)
    return;
  gp_Pnt pt = p;
  pt.Transform(curview->GetMatrix());
  double xp, yp, zp;
  pt.Coord(xp, yp, zp);
  if (curview->IsPerspective())
  {
    const double aDistance = curview->GetFocalDistance();
    xp                     = xp * aDistance / (aDistance - zp);
    yp                     = yp * aDistance / (aDistance - zp);
  }
  p2d.SetCoord(xp, yp);
}

//=================================================================================================

Draw_Display::Draw_Display()
{
  if (Draw_Batch)
    return;
  if (curview)
  {
    PtPers.SetCoord(0., 0., 0.);
    PtPers.Transform(curview->GetMatrix());
    PtCur.SetCoord(PtPers.X() * curview->GetZoom(), PtPers.Y() * curview->GetZoom());
  }
}

//=================================================================================================

void Draw_Display::MoveTo(const gp_Pnt2d& pp)
{
  if (Draw_Batch)
    return;
  const double aZoom = curview->GetZoom();
  gp_Pnt2d     pt(pp.X() * aZoom, pp.Y() * aZoom);
  switch (CurrentMode)
  {

    case DRAW:
      PtCur = pt;
      if (Draw_Bounds)
      {
        if (pt.X() > xmax)
          xmax = pt.X();
        if (pt.X() < xmin)
          xmin = pt.X();
        if (pt.Y() > ymax)
          ymax = pt.Y();
        if (pt.Y() < ymin)
          ymin = pt.Y();
      }
      break;

    case PICK:
      PtCur = pt;
      break;

    case POSTSCRIPT: {
      int x = (int)((pt.X() - ps_vx) * ps_kx + ps_px);
      int y = (int)((pt.Y() - ps_vy) * ps_ky + ps_py);
      (*ps_stream) << "stroke\nnewpath\n" << x << " " << y << " m\n";
    }
    break;
  }
}

//=================================================================================================

inline int CalculRegion(const double x,
                        const double y,
                        const double x1,
                        const double y1,
                        const double x2,
                        const double y2)
{
  int r;
  if (x < x1)
  {
    r = 1;
  }
  else
  {
    if (x > x2)
    {
      r = 2;
    }
    else
    {
      r = 0;
    }
  }
  if (y < y1)
  {
    r |= 4;
  }
  else
  {
    if (y > y2)
    {
      r |= 8;
    }
  }
  return (r);
}

bool Trim(gp_Pnt2d& P1, gp_Pnt2d& P2, double x0, double y0, double x1, double y1)
{
  double xa = P1.X(), ya = P1.Y(), xb = P2.X(), yb = P2.Y();

  int regiona = 0, regionb = 0;
  regiona = CalculRegion(xa, ya, x0, y0, x1, y1);
  regionb = CalculRegion(xb, yb, x0, y0, x1, y1);
  if ((regiona & regionb) == 0)
  {
    double dx  = xb - xa;
    double dy  = yb - ya;
    double dab = sqrt(dx * dx + dy * dy);
    if (dab < 1e-10)
      return (false);
    dx /= dab;
    dy /= dab;

    double xm, ym, mfenx, mfeny;
    mfenx = xm = 0.5 * (x0 + x1);
    mfeny = ym = 0.5 * (y0 + y1);
    x1 -= x0;
    y1 -= y0;
    double d = sqrt(x1 * x1 + y1 * y1) * 2;

    double p = (xm - xa) * dx + (ym - ya) * dy;
    xm       = xa + p * dx;
    ym       = ya + p * dy;
    gp_Pnt2d Pm(xm, ym);

    gp_Pnt2d MFen(mfenx, mfeny);
    if (MFen.SquareDistance(Pm) > d * d)
      return (false);

    double PmDistP1 = Pm.Distance(P1);
    double PmDistP2 = Pm.Distance(P2);

    double amab = (xm - xa) * (xb - xa) + (ym - ya) * (yb - ya);

    if (amab > 0)
    { //-- M est compris entre A et B
      if (PmDistP1 > d)
      {
        P1.SetCoord(xm - d * dx, ym - d * dy);
      }
      if (PmDistP2 > d)
      {
        P2.SetCoord(xm + d * dx, ym + d * dy);
      }
    }
    else if (PmDistP1 < PmDistP2)
    { //-- On a     M    P1 P2
      if (PmDistP2 > d)
      {
        P2.SetCoord(xm + d * dx, ym + d * dy);
      }
    }
    else
    { //-- On a      P1 P2 M
      if (PmDistP1 > d)
      {
        P1.SetCoord(xm - d * dx, ym - d * dy);
      }
    }
    return (true);
  }
  else
    return (false);
}

void Draw_Display::DrawTo(const gp_Pnt2d& pp2)
{
  if (Draw_Batch)
    return;
  if (pp2.X() > 1.e09 || pp2.X() < -1.e09)
    return;
  if (pp2.Y() > 1.e09 || pp2.Y() < -1.e09)
    return;

  gp_Pnt2d p2(pp2.X() * curview->GetZoom(), pp2.Y() * curview->GetZoom());

  if (p2.X() > 1.e09 || p2.X() < -1.e09)
    return;
  if (p2.Y() > 1.e09 || p2.Y() < -1.e09)
    return;

  gp_Pnt2d p1 = PtCur;
  if (p1.X() > 1.e09 || p1.X() < -1.e09)
    return;
  if (p1.Y() > 1.e09 || p1.Y() < -1.e09)
    return;

  PtCur = p2;

  switch (CurrentMode)
  {

    case DRAW: {

#if 1
      int x0, y0, x1, y1;
      curview->GetFrame(x0, y0, x1, y1);

      // int qx0,qy0,qx1,qy1;
      // curview->viewer->GetFrame(curview->id,qx0,qy0,qx1,qy1);
      // if(qx0!=x0 || qx1!=x1 || qy0!=y0 || qy1!=y1) {
      //   x0=qx0; x1=qx1; y0=qy0; y1=qy1;
      // }

      gp_Pnt2d PI1(p1);
      gp_Pnt2d PI2(p2);

      if (Trim(PI1, PI2, x0, y0, x1, y1))
      {
        segm[nbseg].Init(static_cast<int>(PI1.X() + curview->GetDx()),
                         static_cast<int>(-PI1.Y() - curview->GetDy()),
                         static_cast<int>(PI2.X() + curview->GetDx()),
                         static_cast<int>(-PI2.Y() - curview->GetDy()));
        ++nbseg;
      }
#else
      segm[nbseg].Init(static_cast<int>(p1.X() + curview->GetDx()),
                       static_cast<int>(-p1.Y() - curview->GetDy()),
                       static_cast<int>(p2.X() + curview->GetDx()),
                       static_cast<int>(-p2.Y() - curview->GetDy()));
      nbseg++;
#endif
      if (nbseg == MAXSEGMENT)
      {
        Draw_Flush();
      }
      if (Draw_Bounds)
      {
        if (p2.X() > xmax)
          xmax = p2.X();
        if (p2.X() < xmin)
          xmin = p2.X();
        if (p2.Y() > ymax)
          ymax = p2.Y();
        if (p2.Y() < ymin)
          ymin = p2.Y();
      }
    }
    break;

    case PICK:
      if (!found)
      {
        int x1 = (int)p1.X();
        int y1 = (int)p1.Y();
        int x2 = (int)p2.X();
        int y2 = (int)p2.Y();
        if ((x1 >= xpick + precpick) && (x2 >= xpick + precpick))
          break;
        if ((x1 <= xpick - precpick) && (x2 <= xpick - precpick))
          break;
        if ((y1 >= ypick + precpick) && (y2 >= ypick + precpick))
          break;
        if ((y1 <= ypick - precpick) && (y2 <= ypick - precpick))
          break;

        bool inside = true;
        if ((x1 > xpick + precpick) || (x2 > xpick + precpick))
        {
          double y =
            (double)y1 + (double)(y2 - y1) * (double)(xpick + precpick - x1) / (double)(x2 - x1);
          if ((y < ypick + precpick) && (y > ypick - precpick))
          {
            found         = true;
            lastPickParam = (double)(xpick - x1) / (double)(x2 - x1);
            break;
          }
          else
            inside = false;
        }

        if ((x1 < xpick - precpick) || (x2 < xpick - precpick))
        {
          double y =
            (double)y1 + (double)(y2 - y1) * (double)(xpick - precpick - x1) / (double)(x2 - x1);
          if ((y < ypick + precpick) && (y > ypick - precpick))
          {
            found         = true;
            lastPickParam = (double)(xpick - x1) / (double)(x2 - x1);
            break;
          }
          else
            inside = false;
        }

        if ((y1 > ypick + precpick) || (y2 > ypick + precpick))
        {
          double x =
            (double)x1 + (double)(x2 - x1) * (double)(ypick + precpick - y1) / (double)(y2 - y1);
          if ((x < xpick + precpick) && (x > xpick - precpick))
          {
            found         = true;
            lastPickParam = (double)(ypick - y1) / (double)(y2 - y1);
            break;
          }
          else
            inside = false;
        }

        if ((y1 < ypick - precpick) || (y2 < ypick - precpick))
        {
          double x =
            (double)x1 + (double)(x2 - x1) * (double)(ypick - precpick - y1) / (double)(y2 - y1);
          if ((x < xpick + precpick) && (x > xpick - precpick))
          {
            found         = true;
            lastPickParam = (double)(ypick - y1) / (double)(y2 - y1);
            break;
          }
          else
            inside = false;
        }
        found = found || inside;
        if (found)
        {
          if (std::abs(x2 - x1) > std::abs(y2 - y1))
          {
            if (std::abs(x2 - x1) < 1e-5)
              lastPickParam = 0;
            else
              lastPickParam = (double)(xpick - x1) / (double)(x2 - x1);
          }
          else
          {
            if (std::abs(y2 - y1) < 1e-5)
              lastPickParam = 0;
            else
              lastPickParam = (double)(ypick - y1) / (double)(y2 - y1);
          }
        }
      }
      break;

    case POSTSCRIPT: {
      int x = (int)((p2.X() - ps_vx) * ps_kx + ps_px);
      int y = (int)((p2.Y() - ps_vy) * ps_ky + ps_py);
      (*ps_stream) << x << " " << y << " l\n";
    }
    break;
  }
}

//=================================================================================================

void Draw_Display::MoveTo(const gp_Pnt& pt)
{
  if (Draw_Batch)
    return;
  if (CurrentMode == PICK)
  {
    if (!found)
      lastPickP1 = pt;
    else
      return;
  }
  PtPers = pt;
  PtPers.Transform(curview->GetMatrix());
  double xp = PtPers.X();
  double yp = PtPers.Y();
  if (curview->IsPerspective())
  {
    double       ZPers     = PtPers.Z();
    const double aDistance = curview->GetFocalDistance();
    if (ZPers < aDistance * precpers)
    {
      xp = xp * aDistance / (aDistance - ZPers);
      yp = yp * aDistance / (aDistance - ZPers);
    }
  }
  MoveTo(gp_Pnt2d(xp, yp));
}

//=================================================================================================

void Draw_Display::DrawTo(const gp_Pnt& pt)
{
  if (Draw_Batch)
    return;
  if ((CurrentMode == PICK) && found)
    return;

  gp_Pnt pt2 = pt.Transformed(curview->GetMatrix());
  double xp2 = pt2.X();
  double yp2 = pt2.Y();

  if (curview->IsPerspective())
  {
    const double aZoom     = curview->GetZoom();
    const double aDistance = curview->GetFocalDistance();

    double xp1 = PtPers.X();
    double yp1 = PtPers.Y();
    double zp1 = PtPers.Z();
    double zp2 = pt2.Z();
    PtPers     = pt2;
    if ((zp1 >= aDistance * precpers) && (zp2 >= aDistance * precpers))
    {
      return; // segment is not visible in perspective (behind the eye)
    }
    else if (zp1 >= aDistance * precpers)
    {
      xp1 = xp1 + (xp2 - xp1) * (aDistance * precpers - zp1) / (zp2 - zp1);
      yp1 = yp1 + (yp2 - yp1) * (aDistance * precpers - zp1) / (zp2 - zp1);
      zp1 = aDistance * precpers;
      xp1 = xp1 * aDistance / (aDistance - zp1);
      yp1 = yp1 * aDistance / (aDistance - zp1);
      MoveTo(gp_Pnt2d(xp1 * aZoom, yp1 * aZoom));
    }
    else if (zp2 >= aDistance * precpers)
    {
      xp2 = xp2 + (xp1 - xp2) * (aDistance * precpers - zp2) / (zp1 - zp2);
      yp2 = yp2 + (yp1 - yp2) * (aDistance * precpers - zp2) / (zp1 - zp2);
      zp2 = aDistance * precpers;
    }
    xp2 = xp2 * aDistance / (aDistance - zp2);
    yp2 = yp2 * aDistance / (aDistance - zp2);
  }
  DrawTo(gp_Pnt2d(xp2, yp2));
  if (CurrentMode == PICK)
  {
    if (!found)
      lastPickP1 = pt;
    else
      lastPickP2 = pt;
  }
}

//=================================================================================================

void Draw_Display::Draw(const gp_Pnt& p1, const gp_Pnt& p2)
{
  if (Draw_Batch)
    return;
  MoveTo(p1);
  DrawTo(p2);
}

//=================================================================================================

void Draw_Display::Draw(const gp_Pnt2d& p1, const gp_Pnt2d& p2)
{
  if (Draw_Batch)
    return;
  MoveTo(p1);
  DrawTo(p2);
}

//=================================================================================================

int Draw_Display::ViewId() const
{
  if (Draw_Batch)
    return 0;
  return curviewId;
}

//=================================================================================================

bool Draw_Display::HasPicked() const
{
  if (Draw_Batch)
    return false;
  return found;
}
