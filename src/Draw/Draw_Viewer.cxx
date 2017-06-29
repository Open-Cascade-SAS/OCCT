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
#include <TCollection_AsciiString.hxx>

#define precpers 0.95
#define ButtonPress 4
#define MotionNotify 6
static const Standard_Real DRAWINFINITE = 1e50;
Standard_EXPORT Standard_Boolean Draw_Bounds = Standard_True;
extern Standard_Boolean Draw_Batch;
const Standard_Integer MAXSEGMENT = 1000;
Segment segm[MAXSEGMENT];
static int nbseg=0;
static Draw_View* curview = NULL;
static Standard_Integer curviewId = 0;
static char blank[2] = "";
static Standard_Real xmin,xmax,ymin,ymax;
static Standard_Boolean found = Standard_False;
static Standard_Integer  xpick, ypick, precpick;
static gp_Pnt lastPickP1;
static gp_Pnt lastPickP2;
static Standard_Real   lastPickParam;
static Draw_Color highlightcol;
static Draw_Color currentcolor;
static Standard_Boolean highlight = Standard_False;
static Standard_Integer ps_vx, ps_vy;
static Standard_Real ps_kx, ps_ky;
static Standard_Integer ps_px, ps_py;
static ostream* ps_stream;
static Standard_Integer ps_width[MAXCOLOR];
static Standard_Real    ps_gray[MAXCOLOR];

enum DrawingMode {DRAW, PICK, POSTSCRIPT};
static DrawingMode CurrentMode = DRAW;

//=======================================================================
//function : Create
//purpose  :
//=======================================================================

Draw_Viewer::Draw_Viewer()
{
  if (Draw_Batch) return;
  Standard_Integer i;
  for ( i = 0; i < MAXVIEW; i++) myViews[i] = NULL;
  for (i = 0; i < MAXCOLOR; i++) {
    ps_width[i] = 1;
    ps_gray[i]  = 0;
  }
}

//=======================================================================
//function : DefineColor
//purpose  :
//=======================================================================

Standard_Boolean Draw_Viewer::DefineColor (const Standard_Integer i, const char* colname)
{
  if (Draw_Batch) return 1;
  return Draw_Window::DefineColor(i,colname);
}


//=======================================================================
//function : MakeView
//purpose  :
//=======================================================================

void Draw_Viewer::MakeView(const Standard_Integer id,
			   const char* typ,
			   const Standard_Integer X, const Standard_Integer Y,
			   const Standard_Integer W, const Standard_Integer H)
{
  if (Draw_Batch) return;
  if (id < MAXVIEW) {

    DeleteView(id);
    myViews[id] = new Draw_View(id,this,X , Y, W, H);

    // View fields
    myViews[id]->SetDx(W / 2);
    myViews[id]->SetDy(- H / 2);

    if (!myViews[id]->Init(typ))
      DeleteView(id);

    RepaintView(id);
  }
}

#ifdef _WIN32
//=======================================================================
//function : MakeView
//purpose  :
//=======================================================================

void Draw_Viewer::MakeView(const Standard_Integer id,
			   const char* typ,
			   const Standard_Integer X, const Standard_Integer Y,
			   const Standard_Integer W, const Standard_Integer H,
         HWND win, Standard_Boolean useBuffer)
{
  if (Draw_Batch) return;
  if (id < MAXVIEW) {

    DeleteView(id);
    myViews[id] = new Draw_View(id, this, X, Y, W, H, win);
    myViews[id]->SetUseBuffer(useBuffer);

    // View fields
    myViews[id]->SetDx( W / 2);
    myViews[id]->SetDy(-H / 2);

    if (!myViews[id]->Init(typ))
      DeleteView(id);
    RepaintView(id);
  }
}
#endif

//=======================================================================
//function : MakeView
//purpose  :
//=======================================================================

void Draw_Viewer::MakeView    (const Standard_Integer id,
			       const char*  typ,
			       const char*  window)
{
  if (Draw_Batch) return;
  if (id < MAXVIEW) {

    DeleteView(id);
    myViews[id] = new Draw_View(id,this,window);


    myViews[id]->SetDx(myViews[id]->WidthWin() / 2);
    myViews[id]->SetDy(-myViews[id]->HeightWin() / 2);

    if (!myViews[id]->Init(typ))
      DeleteView(id);

    RepaintView(id);
  }
}


//=======================================================================
//function : SetTitle
//purpose  :
//=======================================================================

void Draw_Viewer::SetTitle (const Standard_Integer id, const char* name)
{
  if (Draw_Batch) return;
  if(myViews[id]) myViews[id]->SetTitle (name);
}

//=======================================================================
//function : ResetView
//purpose  : reset view zoom and axes
//=======================================================================

void Draw_Viewer::ResetView(const Standard_Integer id)
{
  if (Draw_Batch) return;
  if (myViews[id]) {
    myViews[id]->Init(myViews[id]->Type());
    ConfigView(id);
  }
}

//=======================================================================
//function : SetZoom
//purpose  :
//=======================================================================

void Draw_Viewer::SetZoom (const Standard_Integer id, const Standard_Real z)
{
  if (Draw_Batch)
    return;

  Draw_View* aView = myViews[id];
  if (aView)
  {
    Standard_Real zz = z / aView->GetZoom();
    aView->SetZoom(z);
    Standard_Integer X,Y,W,H;
    GetPosSize(id,X,Y,W,H);

    const Standard_Real w = 0.5 * static_cast<Standard_Real>(W);
    const Standard_Real h = 0.5 * static_cast<Standard_Real>(H);

    const Standard_Integer aDx = static_cast<Standard_Integer>
      (  w - zz * (w - aView->GetDx()) );
    const Standard_Integer aDy = static_cast<Standard_Integer>
      ( -h + zz * (h + aView->GetDy()) );

    aView->SetDx(aDx);
    aView->SetDy(aDy);
  }
}

//=======================================================================
//function : RotateView
//purpose  :
//=======================================================================

void   Draw_Viewer::RotateView  (const Standard_Integer id,
				 const gp_Dir2d& D,
				 const Standard_Real A)
{
  if (Draw_Batch) return;
  if (myViews[id]) {
    gp_Trsf T = myViews[id]->GetMatrix();

    T.Invert();
    gp_Pnt PP(0,0,0);
    gp_Dir DD(D.X(),D.Y(),0);
    PP.Transform(T);
    DD.Transform(T);
    RotateView(id,PP,DD,A);
  }
}

//=======================================================================
//function : RotateView
//purpose  :
//=======================================================================

void   Draw_Viewer::RotateView  (const Standard_Integer id,
				 const gp_Pnt& P,
				 const gp_Dir& D,
				 const Standard_Real A)
{
  if (Draw_Batch) return;
  if (myViews[id]) {
    gp_Trsf T;
    T.SetRotation(gp_Ax1(P,D),A);
    myViews[id]->Transform(T);
  }
}


//=======================================================================
//function : SetFocal
//purpose  :
//=======================================================================

void Draw_Viewer::SetFocal (const Standard_Integer id, const Standard_Real F)
{
  if (Draw_Batch) return;
  if (myViews[id])
    myViews[id]->SetFocalDistance(F);
}

//=======================================================================
//function : GetType
//purpose  :
//=======================================================================

char* Draw_Viewer::GetType (const Standard_Integer id) const
{
  if (Draw_Batch) return blank;
  if (myViews[id])
    return const_cast<char*>(myViews[id]->Type());
  else
    return blank;
}

//=======================================================================
//function : Zoom
//purpose  :
//=======================================================================

Standard_Real Draw_Viewer::Zoom (const Standard_Integer id) const
{
  if (Draw_Batch) return Standard_False;
  if (myViews[id])
    return myViews[id]->GetZoom();
  else
    return 0.0;
}

//=======================================================================
//function : Focal
//purpose  :
//=======================================================================

Standard_Real Draw_Viewer::Focal (const Standard_Integer id) const
{
  if (Draw_Batch) return 1.;
  if (myViews[id])
    return myViews[id]->GetFocalDistance();
  else
    return 0;
}

//=======================================================================
//function : GetTrsf
//purpose  :
//=======================================================================

void Draw_Viewer::GetTrsf (const Standard_Integer id,gp_Trsf& T) const
{
  if (Draw_Batch) return;
  if (myViews[id])
    T = myViews[id]->GetMatrix();
}

//=======================================================================
//function : Is3D
//purpose  :
//=======================================================================

Standard_Boolean Draw_Viewer::Is3D (const Standard_Integer id) const
{
  if (Draw_Batch) return Standard_False;
  if (myViews[id])
    return !myViews[id]->Is2D();
  else
    return Standard_False;
}

//=======================================================================
//function : SetTrsf
//purpose  :
//=======================================================================

void Draw_Viewer::SetTrsf (const Standard_Integer id,gp_Trsf& T)
{
  if (Draw_Batch) return;
  if (myViews[id])
    myViews[id]->SetMatrix(T);
}

//=======================================================================
//function : GetPosSize
//purpose  :
//=======================================================================

void Draw_Viewer::GetPosSize(const Standard_Integer id,
			     Standard_Integer& X, Standard_Integer& Y,
			     Standard_Integer& W, Standard_Integer& H)
{
  if (Draw_Batch) return;
  if (myViews[id] != NULL) {
    myViews[id]->GetPosition(X, Y);
    W = myViews[id]->WidthWin();
    H = myViews[id]->HeightWin();
  }
}

//=======================================================================
//function : GetFrame
//purpose  :
//=======================================================================

void Draw_Viewer::GetFrame(const Standard_Integer id,
			   Standard_Integer& xminf, Standard_Integer& yminf,
			   Standard_Integer& xmaxf, Standard_Integer& ymaxf)
{
  if (Draw_Batch) return;
  if (myViews[id]) {
    Standard_Integer X,Y,H,W;
    GetPosSize(id,X,Y,W,H);
    xminf =   - myViews[id]->GetDx();
    xmaxf = W - myViews[id]->GetDx();
    yminf =   - myViews[id]->GetDy() - H;
    ymaxf =   - myViews[id]->GetDy();
  }
}

//=======================================================================
//function : FitView
//purpose  :
//=======================================================================

void Draw_Viewer::FitView(const Standard_Integer id, const Standard_Integer frame)
{
  if (Draw_Batch) return;
  if (myViews[id]) {

    // is this the only view in its category
    Standard_Boolean is2d = myViews[id]->Is2D();
    Standard_Integer i,nbviews = 0;
    for (i = 1; i < MAXVIEW; i++) {
      if (myViews[i]) {
        if (myViews[i]->Is2D() == is2d)
          ++nbviews;
      }
    }
    Standard_Boolean only = (nbviews == 1);

    Standard_Integer X,Y,H,W;
    GetPosSize(id,X,Y,W,H);
    // compute the min max
    Standard_Integer n = myDrawables.Length();
    if (n == 0) return;
//    Draw_Display DF;
    curview = myViews[id];
    Standard_Real umin,umax,vmin,vmax;
    Standard_Real u1,u2,v1,v2;
    umin = vmin = DRAWINFINITE;
    umax = vmax = -DRAWINFINITE;

    for (i = 1; i <= n; i++) {
      Standard_Boolean d3d = myDrawables(i)->Is3D();
      if ((d3d && !is2d) || (!d3d && is2d)) {
	// if this is not the only view recompute...
	if (!only)
	  DrawOnView(id,myDrawables(i));
	myDrawables(i)->Bounds(u1,u2,v1,v2);
	if (u1 < umin) umin = u1;
	if (u2 > umax) umax = u2;
	if (v1 < vmin) vmin = v1;
	if (v2 > vmax) vmax = v2;
      }
    }
    Standard_Real z;
    umin = umin / curview->GetZoom();
    vmin = vmin / curview->GetZoom();
    umax = umax / curview->GetZoom();
    vmax = vmax / curview->GetZoom();
    if ((umax - umin) < 1.e-6) {
      if ((vmax - vmin) < 1.e-6)
	return;
      else
	z = ((Standard_Real)(H - 2*frame)) / (vmax - vmin);
    }
    else {
      z = ((Standard_Real)(W - 2*frame)) /((Standard_Real) (umax - umin));
      if ((vmax - vmin) > 1.e-6) {
	Standard_Real z2 = ((Standard_Real)(H - 2*frame)) /(vmax - vmin);
	if (z2 < z) z = z2;
      }
    }
    curview->SetZoom(z);
    curview->SetDx( static_cast<Standard_Integer>( W / 2 - 0.5 * (umin+umax) * z) );
    curview->SetDy( static_cast<Standard_Integer>(-H / 2 - 0.5 * (vmin+vmax) * z) );
  }
}

//=======================================================================
//function : PanView
//purpose  :
//=======================================================================

void Draw_Viewer::PanView(const Standard_Integer id,
			  const Standard_Integer DX, const Standard_Integer DY)
{
  if (Draw_Batch) return;
  if (myViews[id]) {
    myViews[id]->SetDx(myViews[id]->GetDx() + DX);
    myViews[id]->SetDy(myViews[id]->GetDy() + DY);
  }
}


//=======================================================================
//function : SetPan
//purpose  :
//=======================================================================

void Draw_Viewer::SetPan(const Standard_Integer id,
			 const Standard_Integer DX, const Standard_Integer DY)
{
  if (Draw_Batch) return;
  if (myViews[id]) {
    myViews[id]->SetDx(DX);
    myViews[id]->SetDy(DY);
  }
}

//=======================================================================
//function : GetPan
//purpose  :
//=======================================================================

void Draw_Viewer::GetPan(const Standard_Integer id,
			 Standard_Integer& DX, Standard_Integer& DY)
{
  if (Draw_Batch) return;
  if (myViews[id]) {
    DX = myViews[id]->GetDx();
    DY = myViews[id]->GetDy();
  }
}

//=======================================================================
//function : HasView
//purpose  :
//=======================================================================

Standard_Boolean Draw_Viewer::HasView(const Standard_Integer id) const
{
  if (Draw_Batch) return Standard_False;
  if ((id < 0) || id >= MAXVIEW) return Standard_False;
  return myViews[id] != NULL;
}

//=======================================================================
//function : DisplayView
//purpose  :
//=======================================================================

void Draw_Viewer::DisplayView (const Standard_Integer id) const
{
  if (Draw_Batch) return;
  if (myViews[id]) myViews[id]->DisplayWindow();
}

//=======================================================================
//function : HideView
//purpose  :
//=======================================================================

void Draw_Viewer::HideView (const Standard_Integer id) const
{
  if (Draw_Batch) return;
  if (myViews[id]) {
    //
  }
}

//=======================================================================
//function : ClearView
//purpose  :
//=======================================================================

void Draw_Viewer::ClearView(const Standard_Integer id) const
{
  if (Draw_Batch) return;
  if (myViews[id]) myViews[id]->Clear();
}

//=======================================================================
//function : RemoveView
//purpose  :
//=======================================================================

void Draw_Viewer::RemoveView(const Standard_Integer id)
{
  if (Draw_Batch) return;
  if (myViews[id]) {
    delete myViews[id];
    myViews[id] = NULL;
  }
}

//=======================================================================
//function : RepaintView
//purpose  :
//=======================================================================
void Draw_Viewer::RepaintView (const Standard_Integer id) const
{
  if (Draw_Batch) return;
  if (myViews[id]) {
    ClearView(id);
    Standard_Integer n = myDrawables.Length();
    for (Standard_Integer i = 1; i <= n; i++)
      DrawOnView(id,myDrawables(i));
  }
}


#ifdef _WIN32
//=======================================================================
//function : ResizeView
//purpose  : WNT re-drawing optimization
//=======================================================================
void Draw_Viewer::ResizeView (const Standard_Integer id) const
{
  if (Draw_Batch) return;
  if (myViews[id] && myViews[id]->GetUseBuffer()) {
    myViews[id]->InitBuffer();
    RepaintView(id);
  }
}

//=======================================================================
//function : UpdateView
//purpose  : WNT re-drawing optimization
//=======================================================================
void Draw_Viewer::UpdateView (const Standard_Integer id, const Standard_Boolean forced) const
{
  if (Draw_Batch) return;
  if (myViews[id]) {
    if (!myViews[id]->GetUseBuffer() || forced) {
      RepaintView(id);
    }
    // Fast redrawing on WNT
    if (myViews[id]->GetUseBuffer()) myViews[id]->Redraw();
  }
}
#endif

//=======================================================================
//function : ConfigView
//purpose  :
//=======================================================================

void Draw_Viewer::ConfigView (const Standard_Integer id) const
{
  if (Draw_Batch) return;
  if (myViews[id])
  {
    myViews[id]->SetDx(myViews[id]->WidthWin()   / 2);
    myViews[id]->SetDy(-myViews[id]->HeightWin() / 2);
  }
}

//=======================================================================
//function : PostScriptView
//purpose  :
//=======================================================================

void Draw_Viewer::PostScriptView (const Standard_Integer id,
				  const Standard_Integer VXmin,
				  const Standard_Integer VYmin,
				  const Standard_Integer VXmax,
				  const Standard_Integer VYmax,
				  const Standard_Integer PXmin,
				  const Standard_Integer PYmin,
				  const Standard_Integer PXmax,
				  const Standard_Integer PYmax,
				  ostream& sortie) const
{
  if (Draw_Batch) return;
  if (myViews[id]) {
    ps_vx = VXmin;
    ps_vy = VYmin;
    ps_px = PXmin;
    ps_py = PYmin;
    ps_kx = ((Standard_Real) (PXmax - PXmin)) / ((Standard_Real) (VXmax - VXmin));
    ps_ky = ((Standard_Real) (PYmax - PYmin)) / ((Standard_Real) (VYmax - VYmin));
    ps_stream = &sortie;
    Standard_Integer n = myDrawables.Length();
    if (n == 0) return;
    CurrentMode = POSTSCRIPT;
    Draw_Display DF = MakeDisplay(id);
    Standard_Boolean view2d = myViews[id]->Is2D();
    for (Standard_Integer i = 1; i <= n; i++)
      if (myDrawables(i)->Is3D()) {
	if (!view2d) myDrawables(i)->DrawOn(DF);
      }
    else {
      if (view2d) myDrawables(i)->DrawOn(DF);
    }
    sortie << "stroke\n";
    CurrentMode = DRAW;
  }
}

//=======================================================================
//function : PostColor
//purpose  :
//=======================================================================

void Draw_Viewer::PostColor(const Standard_Integer icol,
			    const Standard_Integer width,
			    const Standard_Real    gray)
{
  if (Draw_Batch) return;
  if ((icol < 0) || (icol >= MAXCOLOR)) return;
  ps_width[icol] = width;
  ps_gray[icol] = gray;
}

//=======================================================================
//function : SaveView
//purpose  :
//=======================================================================

Standard_Boolean Draw_Viewer::SaveView(const Standard_Integer id,
                                       const char* filename)
{
  if (Draw_Batch) return Standard_False;;
  Flush();
  if (myViews[id]) {
    return myViews[id]->Save(filename);
  }
  else
  {
    std::cerr << "View " << id << " doesn't exists!\n";
    return Standard_False;
  }
}

//=======================================================================
//function : RepaintAll
//purpose  :
//=======================================================================

void Draw_Viewer::RepaintAll () const
{
  if (Draw_Batch) return;
  for (Standard_Integer id = 0; id < MAXVIEW; id++)
    RepaintView(id);
}

//=======================================================================
//function : Repaint2D
//purpose  :
//=======================================================================

void Draw_Viewer::Repaint2D () const
{
  if (Draw_Batch) return;
  for (Standard_Integer id = 0; id < MAXVIEW; id++)
    if (myViews[id]) {
      if (myViews[id]->Is2D())
        RepaintView(id);
    }
}

//=======================================================================
//function : Repaint3D
//purpose  :
//=======================================================================

void Draw_Viewer::Repaint3D () const
{
  if (Draw_Batch) return;
  for (Standard_Integer id = 0; id < MAXVIEW; id++)
    if (myViews[id]) {
      if (!myViews[id]->Is2D())
        RepaintView(id);
    }
}

//=======================================================================
//function : DeleteView
//purpose  :
//=======================================================================

void Draw_Viewer::DeleteView(const Standard_Integer id)
{
  if (Draw_Batch) return;
  if (myViews[id] != NULL) {
    delete myViews[id];
    myViews[id] = NULL;
  }
}

//=======================================================================
//function : Clear
//purpose  :
//=======================================================================

void Draw_Viewer::Clear()
{
  if (Draw_Batch) return;
  for (Standard_Integer i = 1; i <= myDrawables.Length(); i++)
    myDrawables(i)->Visible(Standard_False);
  myDrawables.Clear();
  for (Standard_Integer id = 0; id < MAXVIEW; id++)
    ClearView(id);
}

//=======================================================================
//function : Clear2D
//purpose  :
//=======================================================================

void Draw_Viewer::Clear2D()
{
  if (Draw_Batch) return;
  Standard_Integer i = 1;
  while (i <= myDrawables.Length()) {
    if (myDrawables(i)->Is3D())
      i++;
    else {
      myDrawables(i)->Visible(Standard_False);
      myDrawables.Remove(i);
    }
  }
  for (Standard_Integer id = 0; id < MAXVIEW; id++) {
    if (myViews[id]) {
      if (myViews[id]->Is2D())
        ClearView(id);
    }
  }
}

//=======================================================================
//function : Clear3D
//purpose  :
//=======================================================================

void Draw_Viewer::Clear3D()
{
  if (Draw_Batch) return;
  Standard_Integer i = 1;
  while (i <= myDrawables.Length()) {
    if (myDrawables(i)->Is3D()) {
      myDrawables(i)->Visible(Standard_False);
      myDrawables.Remove(i);
    }
    else
      i++;
  }
  for (Standard_Integer id = 0; id < MAXVIEW; id++) {
    if (myViews[id]) {
      if (!myViews[id]->Is2D())
        ClearView(id);
    }
  }
}

//=======================================================================
//function : Flush
//purpose  :
//=======================================================================

void Draw_Viewer::Flush()
{
  if (Draw_Batch) return;
  Draw_Window::Flush();
}

//=======================================================================
//function : DrawOnView
//purpose  :
//=======================================================================

void Draw_Viewer::DrawOnView(const Standard_Integer id,
			     const Handle(Draw_Drawable3D)& D) const
{
  if (Draw_Batch) return;
  if (myViews[id]) {
    Draw_Display d = MakeDisplay(id);
    xmin = ymin = DRAWINFINITE;
    xmax = ymax = -DRAWINFINITE;

    Standard_Boolean view2d = myViews[id]->Is2D();
    myViews[id]->ResetFrame();
    if ((D->Is3D() && !view2d) || (!D->Is3D() && view2d))
    {
      D->DrawOn(d);
      if (CurrentMode == DRAW)
        D->SetBounds(xmin,xmax,ymin,ymax);
      d.Flush();
    }
  }
}

//=======================================================================
//function : HighlightOnView
//purpose  :
//=======================================================================

void Draw_Viewer::HighlightOnView (const Standard_Integer id,
				   const Handle(Draw_Drawable3D)& D,
				   const Draw_ColorKind C) const
{
  if (Draw_Batch) return;
  highlight = Standard_True;
  highlightcol = C;
  DrawOnView(id,D);
  highlight = Standard_False;
}

//=======================================================================
//function : AddDrawable
//purpose  :
//=======================================================================

void Draw_Viewer::AddDrawable (const Handle(Draw_Drawable3D)& D)
{
  if (Draw_Batch) return;
  if (!D.IsNull() && !D->Visible()) {
    myDrawables.Append(D);
    D->Visible(Standard_True);
  }
}

//=======================================================================
//function : RemoveDrawable
//purpose  :
//=======================================================================

void Draw_Viewer::RemoveDrawable (const Handle(Draw_Drawable3D)& D)
{
  if (Draw_Batch) return;
  if (!D.IsNull() && D->Visible()) {
    Standard_Integer index;
    for (index = 1; index <= myDrawables.Length(); index++) {
      if (myDrawables(index) == D) {
	D->Visible(Standard_False);
	myDrawables.Remove(index);
	return;
      }
    }
  }
}

//=======================================================================
//function : MakeDisplay
//purpose  : return a display on the view
//=======================================================================

Draw_Display Draw_Viewer::MakeDisplay (const Standard_Integer id) const
{
  if (Draw_Batch) {Draw_Display dis;return dis;}
  curviewId = id;
  curview = myViews[id];
  nbseg = 0;
  Draw_Color initcol(Draw_blanc);
  // to force setting the color
  currentcolor = Draw_Color(Draw_rouge);
  Draw_Display dis;
  dis.SetColor(initcol);
  dis.SetMode(0x3 /*GXcopy*/);
  return dis;
}

//=======================================================================
//function : Select
//purpose  :
//=======================================================================

void Draw_Viewer::Select (Standard_Integer& id, Standard_Integer& X, Standard_Integer& Y,
			  Standard_Integer& Button, Standard_Boolean wait)
{
  if (Draw_Batch) return;
  id = X = Y = Button = 0;
  Standard_Boolean hasView = Standard_False;
  for (int aViewIter = 0; aViewIter < MAXVIEW; ++aViewIter)
  {
    if (myViews[aViewIter] != NULL
     && myViews[aViewIter]->IsMapped())
    {
      hasView = Standard_True;
      break;
    }
  }
  if (!hasView)
  {
    std::cerr << "No selection is possible with no open views\n";
    return;
  }
  Flush();
#if !defined(_WIN32) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
  if (!wait) {
    if (id >=0 && id < MAXVIEW) {
      if (myViews[id]) myViews[id]->Wait(wait);
    }
  }
  else {
    for(int i=0 ; i<MAXVIEW ; i++)
      if (myViews[i])  myViews[i]->Wait(wait);
  }

  Standard_Boolean again = Standard_True;
  while (again) {

    Event ev;
    ev.type = 0;

    GetNextEvent(ev);

    switch (ev.type) {

    case ButtonPress :
      Standard_Integer iv;
      for (iv = 0; iv < MAXVIEW; iv++) {
	if (myViews[iv]) {
	  if (myViews[iv]->win == ev.window)
	    break;
	}
      }
      if (wait || id == iv) {
	if (iv < MAXVIEW) {
	  id = iv;
	  X = ev.x;
	  Y = ev.y;
	  Button = ev.button;
	}
	else {
	  id = -1;
	}
	again = Standard_False;
      }
      break;

    case MotionNotify :
      if (wait) break;
      X = ev.x;
      Y = ev.y;
      Button = 0;
      again = Standard_False;
      break;
    }
  }

  if (id != -1) {
    X =  X - myViews[id]->GetDx();
    Y = -Y - myViews[id]->GetDy();
  }
  if (!wait) myViews[id]->Wait(!wait);
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  Standard_Integer aWindowNumber;

  id = MAXVIEW;
  while (id >= MAXVIEW)
  {
    GetNextEvent(wait, aWindowNumber, X, Y, Button);

    if (Y < 0)
    {
      continue; // mouse clicked on window title
    }

    for (Standard_Integer anIter = 0; anIter < MAXVIEW; anIter++)
    {
      if (myViews[anIter] && myViews[anIter]->IsEqualWindows (aWindowNumber))
      {
        id = anIter;
      }
    }
  }

  X =  X - myViews[id]->GetDx();
  Y = -Y - myViews[id]->GetDy();

#else
  HANDLE hWnd;

  id = MAXVIEW; //:abv 29.05.02: cycle for working in console mode
  while ( id >= MAXVIEW ) {
    if (wait)
      Draw_Window::SelectWait(hWnd, X, Y, Button);
    else
      Draw_Window::SelectNoWait(hWnd, X, Y, Button);

    // Recherche du numero de la vue grace au HANDLE
    for(int i=0 ; i<MAXVIEW ; i++)
      if (myViews[i] && myViews[i]->win == hWnd )  id = i;
  }
  X =  X - myViews[id]->GetDx();
  Y = -Y - myViews[id]->GetDy();
#endif
}

//=======================================================================
//function : Pick
//purpose  :
//=======================================================================

Standard_Integer Draw_Viewer::Pick(const Standard_Integer id,
			  const Standard_Integer X, const Standard_Integer Y, const Standard_Integer Prec,
			  Handle(Draw_Drawable3D)& D,
			  const Standard_Integer first) const
{
  if (Draw_Batch) return 0;
  if (myViews[id] == NULL)
    return 0;

  // is this the only view in its category
  Standard_Boolean is2d = myViews[id]->Is2D();
  Standard_Integer i,nbviews = 0;
  for (i = 0; i < MAXVIEW; i++)
  {
    if (myViews[i])
      if (myViews[i]->Is2D() == is2d)
        ++nbviews;
  }
  Standard_Boolean only = (nbviews == 1);

  CurrentMode = PICK;
  xpick = X;
  ypick = Y;
  precpick = Prec;
  found = Standard_False;
  Standard_Real x1,x2,y1,y2;
  for (i = first+1; i <= myDrawables.Length(); i++) {
    Standard_Boolean reject = Standard_False;
    // rejection if only view
    if (only) {
      myDrawables(i)->Bounds(x1,x2,y1,y2);
      if ((xpick+Prec < x1) || (xpick-Prec > x2) ||
	  (ypick+Prec < y1) || (ypick-Prec > y2))
	reject = Standard_True;
    }
    if (!reject) {
      DrawOnView(id,myDrawables(i));
      if (found)
	break;
    }
  }
  CurrentMode = DRAW;
  found = Standard_False;
  if (i <= myDrawables.Length())
    D = myDrawables(i);
  else
    i = 0;
  return i;
}

//=======================================================================
//function : LastPick
//purpose  :
//=======================================================================

void Draw_Viewer::LastPick(gp_Pnt& P1, gp_Pnt& P2, Standard_Real& Param)
{
  if (Draw_Batch) return;
  P1    = lastPickP1;
  P2    = lastPickP2;
  Param = lastPickParam;
}

//=======================================================================
//function : ~Draw_Viewer
//purpose  :
//=======================================================================

Draw_Viewer::~Draw_Viewer()
{
  if (Draw_Batch) return;
  for (Standard_Integer id = 0; id < MAXVIEW; id++)
    DeleteView(id);
}

//=======================================================================
//function : operator<<
//purpose  :
//=======================================================================

Draw_Viewer& Draw_Viewer::operator<<(const Handle(Draw_Drawable3D)& d3d)
{
  if (Draw_Batch) return *this;
  if (!d3d.IsNull()) {
    AddDrawable(d3d);
    for (Standard_Integer id = 0; id < MAXVIEW; id++)
      DrawOnView(id,d3d);
  }
  return *this;
}

//=======================================================================
//function : GetDrawables
//purpose  :
//=======================================================================
const Draw_SequenceOfDrawable3D& Draw_Viewer::GetDrawables()
{
  return myDrawables;
}

// *******************************************************************
// DISPLAY methods
// *******************************************************************


void Draw_Flush()
{
  if (Draw_Batch) return;
  if (highlight) curview->SetColor(highlightcol.ID());
  curview->DrawSegments(segm,nbseg);
  nbseg = 0;
}

//=======================================================================
//function : SetColor
//purpose  :
//=======================================================================

void Draw_Display::SetColor (const Draw_Color& col) const
{
  if (Draw_Batch) return;
  if (col.ID() == currentcolor.ID()) return;

  currentcolor = col;
  switch (CurrentMode) {

  case DRAW :
    Draw_Flush();
    curview->SetColor(col.ID());
    break;

  case POSTSCRIPT :
    (*ps_stream) << "stroke\nnewpath\n";
    (*ps_stream) << ps_width[col.ID()]<<" setlinewidth\n";
    (*ps_stream) << ps_gray[col.ID()]<<" setgray\n";

  case PICK :
    break;
  }
}

//=======================================================================
//function : SetMode
//purpose  :
//=======================================================================

void Draw_Display::SetMode (const Standard_Integer M) const
{
  if (Draw_Batch) return;
  switch (CurrentMode) {

  case DRAW :
    Draw_Flush();
    curview->SetMode(M);
    break;

  case PICK :
  case POSTSCRIPT :
    break;
  }
}

//=======================================================================
//function : Zoom
//purpose  :
//=======================================================================

Standard_Real Draw_Display::Zoom() const
{
  if (Draw_Batch) return 1.;
  return curview->GetZoom();
}

//=======================================================================
//function : Flush
//purpose  :
//=======================================================================

void Draw_Display::Flush () const
{
  if (Draw_Batch) return;
  Draw_Flush();
}

//=======================================================================
//function : DrawString
//purpose  :
//=======================================================================

void Draw_Display::DrawString(const gp_Pnt2d& ppt,
			      const Standard_CString S,
			      const Standard_Real moveX,
			      const Standard_Real moveY)
{
  if (Draw_Batch) return;
  if (ppt.X() > 1.e09 || ppt.X() < -1.e09 ) return;
  if (ppt.Y() > 1.e09 || ppt.Y() < -1.e09 ) return;

  gp_Pnt2d pt(ppt.X()*curview->GetZoom(),ppt.Y()*curview->GetZoom());

  if (pt.X() > 1.e09 || pt.X() < -1.e09 ) return;
  if (pt.Y() > 1.e09 || pt.Y() < -1.e09 ) return;

  switch (CurrentMode) {

  case DRAW :
    {
      int X =   (int) ( pt.X() + moveX + curview->GetDx());
      int Y =   (int) (-pt.Y() + moveY - curview->GetDy());
      curview->DrawString(X,Y,(char *)S);
      if (Draw_Bounds) {
	if (pt.X() + moveX > xmax) xmax = pt.X();
	if (pt.X() + moveX < xmin) xmin = pt.X();
	if (-pt.Y() - moveY > ymax) ymax = -pt.Y();
	if (-pt.Y() - moveY < ymin) ymin = -pt.Y();
      }
    }
    break;

  case POSTSCRIPT :
    {
      Standard_Integer x = (Standard_Integer )( (pt.X() + moveX - ps_vx) * ps_kx + ps_px);
      Standard_Integer y = (Standard_Integer )( (pt.Y() + moveY - ps_vy) * ps_ky + ps_py);
      (*ps_stream) <<"stroke\n";
      (*ps_stream) << x << " " << y << " m\n";
      (*ps_stream) <<"("<<S<<") show\nnewpath\n";
    }
    break;

  case PICK :
    break;
  }
}

//=======================================================================
//function : DrawString
//purpose  :
//=======================================================================

void Draw_Display::DrawString(const gp_Pnt2d& ppt,
			      const Standard_CString S)
{
  if (Draw_Batch) return;
  DrawString(ppt,S,0.0,0.0);
}

//=======================================================================
//function : DrawString
//purpose  :
//=======================================================================

void Draw_Display::DrawString(const gp_Pnt& pt,
			      const Standard_CString S,
			      const Standard_Real moveX,
			      const Standard_Real moveY)
{
  if (Draw_Batch) return;
  DrawString(Project(pt),S,moveX,moveY);
}

//=======================================================================
//function : DrawString
//purpose  :
//=======================================================================

void Draw_Display::DrawString(const gp_Pnt& pt,
			      const Standard_CString S)
{
  if (Draw_Batch) return;
  DrawString(Project(pt),S,0.0,0.0);
}


// *******************************************************************
// Drawing data static variables
// *******************************************************************
static gp_Pnt2d PtCur;  // current 2D point
static gp_Pnt PtPers;   // current 3D point for Pers

//=======================================================================
//function : Project
//purpose  :
//=======================================================================

void Draw_Display::Project(const gp_Pnt& p, gp_Pnt2d& p2d) const
{
  if (Draw_Batch) return;
  gp_Pnt pt = p;
  pt.Transform(curview->GetMatrix());
  Standard_Real xp,yp,zp;
  pt.Coord(xp,yp,zp);
  if (curview->IsPerspective()) {
    const Standard_Real aDistance = curview->GetFocalDistance();
    xp = xp * aDistance / (aDistance-zp);
    yp = yp * aDistance / (aDistance-zp);
  }
  p2d.SetCoord(xp,yp);
}

//=======================================================================
//function : Draw_Display
//purpose  :
//=======================================================================

Draw_Display::Draw_Display ()
{
  if (Draw_Batch) return;
  if (curview) {
    PtPers.SetCoord(0., 0., 0.);
    PtPers.Transform(curview->GetMatrix());
    PtCur.SetCoord(PtPers.X()*curview->GetZoom(),PtPers.Y()*curview->GetZoom());
  }
}

//=======================================================================
//function : MoveTo 2D
//purpose  :
//=======================================================================

void Draw_Display::MoveTo (const gp_Pnt2d& pp)
{
  if (Draw_Batch) return;
  const Standard_Real aZoom = curview->GetZoom();
  gp_Pnt2d pt(pp.X() * aZoom, pp.Y() * aZoom);
  switch (CurrentMode) {

  case DRAW :
    PtCur = pt;
    if (Draw_Bounds) {
      if (pt.X() > xmax) xmax = pt.X();
      if (pt.X() < xmin) xmin = pt.X();
      if (pt.Y() > ymax) ymax = pt.Y();
      if (pt.Y() < ymin) ymin = pt.Y();
    }
    break;

  case PICK :
    PtCur = pt;
    break;

  case POSTSCRIPT :
    {
      Standard_Integer x = (Standard_Integer )( (pt.X() - ps_vx) * ps_kx + ps_px);
      Standard_Integer y = (Standard_Integer )( (pt.Y() - ps_vy) * ps_ky + ps_py);
      (*ps_stream) <<"stroke\nnewpath\n"<< x << " " << y << " m\n";
    }
    break;
  }
}

//=======================================================================
//function : DrawTo 2D
//purpose  :
//=======================================================================
inline  Standard_Integer CalculRegion(const Standard_Real x,
				      const Standard_Real y,
				      const Standard_Real x1,
				      const Standard_Real y1,
				      const Standard_Real x2,
				      const Standard_Real y2) {
  Standard_Integer r;
  if(x<x1) { r=1; }  else { if(x>x2) { r=2; } else { r=0; } }
  if(y<y1) { r|=4; } else { if(y>y2) { r|=8; } }
  return(r);
}

Standard_Boolean Trim(gp_Pnt2d& P1,gp_Pnt2d& P2,
		      Standard_Real x0,
		      Standard_Real y0,
		      Standard_Real x1,
		      Standard_Real y1) {
  Standard_Real xa=P1.X(),ya=P1.Y(),xb=P2.X(),yb=P2.Y();

  Standard_Integer regiona=0,regionb=0;
  regiona = CalculRegion(xa,ya,x0,y0,x1,y1);
  regionb = CalculRegion(xb,yb,x0,y0,x1,y1);
  if((regiona & regionb)==0) {
    Standard_Real dx=xb-xa;
    Standard_Real dy=yb-ya;
    Standard_Real dab=sqrt(dx*dx+dy*dy);
    if(dab<1e-10) return(Standard_False);
    dx/=dab;
    dy/=dab;

    Standard_Real xm,ym,mfenx,mfeny;
    mfenx=xm=0.5*(x0+x1);
    mfeny=ym=0.5*(y0+y1);
    x1-=x0;     y1-=y0;
    Standard_Real d=sqrt(x1*x1+y1*y1) * 2;

    Standard_Real p=(xm-xa)*dx+(ym-ya)*dy;
    xm=xa+p*dx;
    ym=ya+p*dy;
    gp_Pnt2d Pm(xm,ym);

    gp_Pnt2d MFen(mfenx,mfeny);
    if(MFen.SquareDistance(Pm) > d*d) return(Standard_False);

    Standard_Real PmDistP1 = Pm.Distance(P1);
    Standard_Real PmDistP2 = Pm.Distance(P2);

    Standard_Real amab = (xm-xa)*(xb-xa)+(ym-ya)*(yb-ya);

    if(amab > 0) { //-- M est compris entre A et B
      if(PmDistP1 > d) {
	P1.SetCoord(xm-d*dx,ym-d*dy);
      }
      if(PmDistP2 >d) {
	P2.SetCoord(xm+d*dx,ym+d*dy);
      }
    }
    else if(PmDistP1 < PmDistP2) {  //-- On a     M    P1 P2
      if(PmDistP2 > d) {
	P2.SetCoord(xm+d*dx,ym+d*dy);
      }
    }
    else { //-- On a      P1 P2 M
      if(PmDistP1 > d) {
	P1.SetCoord(xm-d*dx,ym-d*dy);
      }
    }
    return(Standard_True);
  }
  else return(Standard_False);
}




void Draw_Display::DrawTo (const gp_Pnt2d& pp2)
{
  if (Draw_Batch) return;
  if (pp2.X() > 1.e09 || pp2.X() < -1.e09 ) return;
  if (pp2.Y() > 1.e09 || pp2.Y() < -1.e09 ) return;

  gp_Pnt2d p2(pp2.X() * curview->GetZoom(), pp2.Y() * curview->GetZoom());

  if (p2.X() > 1.e09 || p2.X() < -1.e09 ) return;
  if (p2.Y() > 1.e09 || p2.Y() < -1.e09 ) return;

  gp_Pnt2d p1 = PtCur;
  if (p1.X() > 1.e09 || p1.X() < -1.e09 ) return;
  if (p1.Y() > 1.e09 || p1.Y() < -1.e09 ) return;

  PtCur = p2;

  switch (CurrentMode) {

  case DRAW : {

#if 1
    Standard_Integer x0,y0,x1,y1;
    curview->GetFrame(x0,y0,x1,y1);


    //Standard_Integer qx0,qy0,qx1,qy1;
    //curview->viewer->GetFrame(curview->id,qx0,qy0,qx1,qy1);
    //if(qx0!=x0 || qx1!=x1 || qy0!=y0 || qy1!=y1) {
    //  x0=qx0; x1=qx1; y0=qy0; y1=qy1;
    //}



    gp_Pnt2d PI1(p1);
    gp_Pnt2d PI2(p2);

    if(Trim(PI1,PI2,x0,y0,x1,y1))
    {
      segm[nbseg].Init(static_cast<Standard_Integer>( PI1.X() + curview->GetDx()),
                       static_cast<Standard_Integer>(-PI1.Y() - curview->GetDy()),
                       static_cast<Standard_Integer>( PI2.X() + curview->GetDx()),
                       static_cast<Standard_Integer>(-PI2.Y() - curview->GetDy()));
      ++nbseg;
    }
#else
    segm[nbseg].Init(static_cast<Standard_Integer>( p1.X() + curview->GetDx()),
                     static_cast<Standard_Integer>(-p1.Y() - curview->GetDy()),
                     static_cast<Standard_Integer>( p2.X() + curview->GetDx()),
                     static_cast<Standard_Integer>(-p2.Y() - curview->GetDy()));
    nbseg++;
#endif
    if (nbseg == MAXSEGMENT) {
      Draw_Flush();
    }
    if (Draw_Bounds) {
      if (p2.X() > xmax) xmax = p2.X();
      if (p2.X() < xmin) xmin = p2.X();
      if (p2.Y() > ymax) ymax = p2.Y();
      if (p2.Y() < ymin) ymin = p2.Y();
    }
  }
    break;

  case PICK :
    if (!found) {
      Standard_Integer x1 =  (int) p1.X() ;
      Standard_Integer y1 =  (int) p1.Y() ;
      Standard_Integer x2 =  (int) p2.X() ;
      Standard_Integer y2 =  (int) p2.Y() ;
      if ((x1 >= xpick + precpick) && (x2 >= xpick + precpick)) break;
      if ((x1 <= xpick - precpick) && (x2 <= xpick - precpick)) break;
      if ((y1 >= ypick + precpick) && (y2 >= ypick + precpick)) break;
      if ((y1 <= ypick - precpick) && (y2 <= ypick - precpick)) break;

      Standard_Boolean inside = Standard_True;
      if ((x1 > xpick + precpick) || (x2 > xpick + precpick)) {
	Standard_Real y = (Standard_Real) y1 +
	  (Standard_Real) (y2-y1) * (Standard_Real) (xpick+precpick-x1) /
	    (Standard_Real) (x2 - x1);
	if ( (y < ypick+precpick) && (y > ypick - precpick)) {
	  found = Standard_True;
	  lastPickParam = (Standard_Real) (xpick - x1) /
	    (Standard_Real) (x2 - x1);
	  break;
	}
	else
	  inside = Standard_False;
      }

      if ((x1 < xpick - precpick) || (x2 < xpick - precpick)) {
	Standard_Real y = (Standard_Real) y1 +
	  (Standard_Real) (y2-y1) * (Standard_Real) (xpick-precpick-x1) /
	    (Standard_Real) (x2 - x1);
	if ( (y < ypick+precpick) && (y > ypick - precpick)) {
	  found = Standard_True;
	  lastPickParam = (Standard_Real) (xpick - x1) /
	    (Standard_Real) (x2 - x1);
	  break;
	}
	else
	  inside = Standard_False;
      }


      if ((y1 > ypick + precpick) || (y2 > ypick + precpick)) {
	Standard_Real x = (Standard_Real) x1 +
	  (Standard_Real) (x2-x1) * (Standard_Real) (ypick+precpick-y1) /
	    (Standard_Real) (y2 - y1);
	if ( (x < xpick+precpick) && (x > xpick - precpick)) {
	  found = Standard_True;
	  lastPickParam = (Standard_Real) (ypick - y1) /
	    (Standard_Real) (y2 - y1);
	  break;
	}
	else
	  inside = Standard_False;
      }


      if ((y1 < ypick - precpick) || (y2 < ypick - precpick)) {
	Standard_Real x = (Standard_Real) x1 +
	  (Standard_Real) (x2-x1) * (Standard_Real) (ypick-precpick-y1) /
	    (Standard_Real) (y2 - y1);
	if ( (x < xpick+precpick) && (x > xpick - precpick)) {
	  found = Standard_True;
	  lastPickParam = (Standard_Real) (ypick - y1) /
	    (Standard_Real) (y2 - y1);
	  break;
	}
	else
	  inside = Standard_False;
      }
    found = found || inside;
      if (found) {
	if (Abs(x2-x1) > Abs(y2-y1)) {
	  if (Abs(x2-x1) < 1e-5)  lastPickParam = 0;
	  else
	    lastPickParam = (Standard_Real)(xpick - x1) /
	      (Standard_Real)(x2 - x1);
	}
	else {
	  if (Abs(y2-y1) < 1e-5)  lastPickParam = 0;
	  else
	    lastPickParam = (Standard_Real)(ypick - y1) /
	      (Standard_Real)(y2 - y1);
	}
      }
    }
    break;

  case POSTSCRIPT :
    {
      Standard_Integer x = (Standard_Integer )( (p2.X() - ps_vx) * ps_kx + ps_px);
      Standard_Integer y = (Standard_Integer )( (p2.Y() - ps_vy) * ps_ky + ps_py);
      (*ps_stream) << x << " " << y << " l\n";
    }
    break;
  }

}

//=======================================================================
//function : MoveTo
//purpose  :
//=======================================================================

void Draw_Display::MoveTo (const gp_Pnt& pt)
{
  if (Draw_Batch) return;
  if (CurrentMode == PICK) {
    if (!found) lastPickP1 = pt;
    else return;
  }
  PtPers = pt;
  PtPers.Transform(curview->GetMatrix());
  Standard_Real xp = PtPers.X();
  Standard_Real yp = PtPers.Y();
  if (curview->IsPerspective())
  {
    Standard_Real ZPers = PtPers.Z();
    const Standard_Real aDistance = curview->GetFocalDistance();
    if (ZPers < aDistance * precpers)
    {
      xp=xp * aDistance / (aDistance-ZPers);
      yp=yp * aDistance / (aDistance-ZPers);
    }
  }
  MoveTo(gp_Pnt2d(xp,yp));
}

//=======================================================================
//function : DrawTo
//purpose  :
//=======================================================================

void Draw_Display::DrawTo (const gp_Pnt& pt)
{
  if (Draw_Batch) return;
  if ((CurrentMode == PICK) && found) return;

  gp_Pnt pt2 = pt.Transformed(curview->GetMatrix());
  Standard_Real xp2 = pt2.X();
  Standard_Real yp2 = pt2.Y();

  if (curview->IsPerspective())
  {
    const Standard_Real aZoom     = curview->GetZoom();
    const Standard_Real aDistance = curview->GetFocalDistance();

    Standard_Real xp1 = PtPers.X();
    Standard_Real yp1 = PtPers.Y();
    Standard_Real zp1 = PtPers.Z();
    Standard_Real zp2 = pt2.Z();
    PtPers   = pt2;
    if ((zp1 >= aDistance*precpers) && (zp2 >= aDistance*precpers) )
    {
      return;  // segment is not visible in perspective (behind the eye)
    }
    else if (zp1 >= aDistance*precpers)
    {
      xp1=xp1+(xp2-xp1)*(aDistance*precpers-zp1)/(zp2-zp1);
      yp1=yp1+(yp2-yp1)*(aDistance*precpers-zp1)/(zp2-zp1);
      zp1=aDistance*precpers;
      xp1=xp1*aDistance/(aDistance-zp1);
      yp1=yp1*aDistance/(aDistance-zp1);
      MoveTo( gp_Pnt2d(xp1 * aZoom, yp1 * aZoom) );
    }
    else if (zp2 >= aDistance*precpers)
    {
      xp2=xp2+(xp1-xp2)*(aDistance*precpers-zp2)/(zp1-zp2);
      yp2=yp2+(yp1-yp2)*(aDistance*precpers-zp2)/(zp1-zp2);
      zp2=aDistance*precpers;
    }
    xp2 = xp2 * aDistance / (aDistance - zp2);
    yp2 = yp2 * aDistance / (aDistance - zp2);
  }
  DrawTo(gp_Pnt2d(xp2,yp2));
  if (CurrentMode == PICK) {
    if      (!found)    lastPickP1 = pt;
    else                lastPickP2 = pt;
  }
}

//=======================================================================
//function : Draw
//purpose  :
//=======================================================================

void Draw_Display::Draw (const gp_Pnt& p1, const gp_Pnt& p2)
{
  if (Draw_Batch) return;
  MoveTo(p1);
  DrawTo(p2);
}

//=======================================================================
//function : Draw
//purpose  :
//=======================================================================

void Draw_Display::Draw(const gp_Pnt2d& p1, const gp_Pnt2d& p2)
{
  if (Draw_Batch) return;
  MoveTo(p1);
  DrawTo(p2);
}


//=======================================================================
//function : ViewId
//purpose  :
//=======================================================================

Standard_Integer Draw_Display::ViewId() const
{
  if (Draw_Batch) return 0;
  return curviewId;
}


//=======================================================================
//function : HasPicked
//purpose  :
//=======================================================================

Standard_Boolean Draw_Display::HasPicked() const
{
  if (Draw_Batch) return Standard_False;
  return found;
}

