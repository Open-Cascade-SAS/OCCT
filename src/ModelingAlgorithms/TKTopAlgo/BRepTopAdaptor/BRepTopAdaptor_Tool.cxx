// Created on: 1997-10-07
// Created by: Laurent BUCHARD
// Copyright (c) 1997-1999 Matra Datavision
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

#include <Adaptor3d_Surface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepTopAdaptor_Tool.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <TopoDS_Face.hxx>

BRepTopAdaptor_Tool::BRepTopAdaptor_Tool()
{
  myTopolTool = new BRepTopAdaptor_TopolTool();

  myloaded = false;
}

BRepTopAdaptor_Tool::BRepTopAdaptor_Tool(const TopoDS_Face& F, const double /*Tol2d*/)
{
  myTopolTool = new BRepTopAdaptor_TopolTool();

  occ::handle<BRepAdaptor_Surface> surface = new BRepAdaptor_Surface();
  surface->Initialize(F, true);
  const occ::handle<Adaptor3d_Surface>& aSurf = surface; // to avoid ambiguity
  myTopolTool->Initialize(aSurf);
  myHSurface = surface;
  myloaded   = true;
}

BRepTopAdaptor_Tool::BRepTopAdaptor_Tool(const occ::handle<Adaptor3d_Surface>& surface,
                                         const double /*Tol2d*/)
{
  myTopolTool = new BRepTopAdaptor_TopolTool();
  myTopolTool->Initialize(surface);
  myHSurface = surface;
  myloaded   = true;
}

void BRepTopAdaptor_Tool::Init(const TopoDS_Face& F, const double /*Tol2d*/)
{
  occ::handle<BRepAdaptor_Surface> surface = new BRepAdaptor_Surface();
  surface->Initialize(F);
  const occ::handle<Adaptor3d_Surface>& aSurf = surface; // to avoid ambiguity
  myTopolTool->Initialize(aSurf);
  myHSurface = surface;
  myloaded   = true;
}

void BRepTopAdaptor_Tool::Init(const occ::handle<Adaptor3d_Surface>& surface,
                               const double /*Tol2d*/)
{
  myTopolTool->Initialize(surface);
  myHSurface = surface;
  myloaded   = true;
}

occ::handle<BRepTopAdaptor_TopolTool> BRepTopAdaptor_Tool::GetTopolTool()
{
  if (myloaded)
  {
    return (myTopolTool);
  }
  else
  {
#ifdef OCCT_DEBUG
    std::cout << "\n*** Error ds occ::handle<BRepTopAdaptor_TopolTool> "
                 "BRepTopAdaptor_Tool::GetTopolTool()\n"
              << std::endl;
#endif
    return (myTopolTool);
  }
}

occ::handle<Adaptor3d_Surface> BRepTopAdaptor_Tool::GetSurface()
{
  if (myloaded)
  {
    return (myHSurface);
  }
  else
  {
#ifdef OCCT_DEBUG
    std::cout
      << "\n*** Error ds occ::handle<BRepTopAdaptor_TopolTool> BRepTopAdaptor_Tool::GetSurface()\n"
      << std::endl;
#endif
    return (myHSurface);
  }
}

void BRepTopAdaptor_Tool::SetTopolTool(const occ::handle<BRepTopAdaptor_TopolTool>& TT)
{
  myTopolTool = TT;
}

void BRepTopAdaptor_Tool::Destroy() {}
