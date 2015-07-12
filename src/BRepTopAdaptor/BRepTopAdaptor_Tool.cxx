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


#include <Adaptor3d_HSurface.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepTopAdaptor_Tool.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <TopoDS_Face.hxx>

BRepTopAdaptor_Tool::BRepTopAdaptor_Tool() { 
  myTopolTool = new BRepTopAdaptor_TopolTool();

  myloaded=Standard_False;
}

BRepTopAdaptor_Tool::BRepTopAdaptor_Tool(const TopoDS_Face& F,
                                         const Standard_Real /*Tol2d*/) { 
  myTopolTool = new BRepTopAdaptor_TopolTool();

  Handle(BRepAdaptor_HSurface) surface = new BRepAdaptor_HSurface();
  surface->ChangeSurface().Initialize(F,Standard_True);
  const Handle(Adaptor3d_HSurface)& aSurf = surface; // to avoid ambiguity
  myTopolTool->Initialize(aSurf);
  myHSurface = surface;
  myloaded=Standard_True;
}

BRepTopAdaptor_Tool::BRepTopAdaptor_Tool(const Handle(Adaptor3d_HSurface)& surface,
                                         const Standard_Real /*Tol2d*/)
{ 
  myTopolTool = new BRepTopAdaptor_TopolTool();
  myTopolTool->Initialize(surface);
  myHSurface = surface;
  myloaded=Standard_True;
}

void BRepTopAdaptor_Tool::Init(const TopoDS_Face& F,
                               const Standard_Real /*Tol2d*/) 
{ 
  Handle(BRepAdaptor_HSurface) surface = new BRepAdaptor_HSurface();
  surface->ChangeSurface().Initialize(F);
  const Handle(Adaptor3d_HSurface)& aSurf = surface; // to avoid ambiguity
  myTopolTool->Initialize(aSurf);
  myHSurface = surface;
  myloaded=Standard_True;
}

void BRepTopAdaptor_Tool::Init(const Handle(Adaptor3d_HSurface)& surface,
                               const Standard_Real /*Tol2d*/) 
{ 
  myTopolTool->Initialize(surface);
  myHSurface = surface;
  myloaded=Standard_True;
}

Handle(BRepTopAdaptor_TopolTool) BRepTopAdaptor_Tool::GetTopolTool() { 
  if(myloaded) { 
    return(myTopolTool);
  }
  else { 
#ifdef OCCT_DEBUG
    cout<<"\n*** Error ds Handle(BRepTopAdaptor_TopolTool) BRepTopAdaptor_Tool::GetTopolTool()\n"<<endl;
#endif
    return(myTopolTool);
  }
}

Handle(Adaptor3d_HSurface)  BRepTopAdaptor_Tool::GetSurface() { 
  if(myloaded) { 
    return(myHSurface);
  }
  else { 
#ifdef OCCT_DEBUG
    cout<<"\n*** Error ds Handle(BRepTopAdaptor_TopolTool) BRepTopAdaptor_Tool::GetSurface()\n"<<endl;
#endif
    return(myHSurface);
  }
}

void BRepTopAdaptor_Tool::SetTopolTool(const Handle(BRepTopAdaptor_TopolTool)& TT) { 
  myTopolTool=TT;
}

void BRepTopAdaptor_Tool::Destroy() { 
  int i;
  i=0;
  i++;
  
}

