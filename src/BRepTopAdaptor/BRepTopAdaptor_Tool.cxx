// Created on: 1997-10-07
// Created by: Laurent BUCHARD
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <BRepTopAdaptor_Tool.ixx>

#include <BRepTopAdaptor_TopolTool.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <Adaptor3d_HSurface.hxx>


BRepTopAdaptor_Tool::BRepTopAdaptor_Tool() { 
  myTopolTool = new BRepTopAdaptor_TopolTool();

  myloaded=Standard_False;
}

BRepTopAdaptor_Tool::BRepTopAdaptor_Tool(const TopoDS_Face& F,
					 const Standard_Real Tol2d) { 
  myTopolTool = new BRepTopAdaptor_TopolTool();

  Handle(BRepAdaptor_HSurface) surface = new BRepAdaptor_HSurface();
  surface->ChangeSurface().Initialize(F,Standard_True);
  myTopolTool->Initialize(surface);
  myHSurface = surface;
  myloaded=Standard_True;
}

BRepTopAdaptor_Tool::BRepTopAdaptor_Tool(const Handle(Adaptor3d_HSurface)& surface,
					 const Standard_Real Tol2d) { 
  myTopolTool = new BRepTopAdaptor_TopolTool();

  myTopolTool->Initialize(surface);
  myHSurface = surface;
  myloaded=Standard_True;
}

void BRepTopAdaptor_Tool::Init(const TopoDS_Face& F,
			  const Standard_Real Tol2d) { 
  Handle(BRepAdaptor_HSurface) surface = new BRepAdaptor_HSurface();
  surface->ChangeSurface().Initialize(F);
  myTopolTool->Initialize(surface);
  myHSurface = surface;
  myloaded=Standard_True;
}

void BRepTopAdaptor_Tool::Init(const Handle(Adaptor3d_HSurface)& surface,
			  const Standard_Real Tol2d) { 
  myTopolTool->Initialize(surface);
  myHSurface = surface;
  myloaded=Standard_True;
}

Handle_BRepTopAdaptor_TopolTool BRepTopAdaptor_Tool::GetTopolTool() { 
  if(myloaded) { 
    return(myTopolTool);
  }
  else { 
    cout<<"\n*** Error ds Handle_BRepTopAdaptor_TopolTool BRepTopAdaptor_Tool::GetTopolTool()\n"<<endl;
    return(myTopolTool);
  }
}

Handle_Adaptor3d_HSurface  BRepTopAdaptor_Tool::GetSurface() { 
  if(myloaded) { 
    return(myHSurface);
  }
  else { 
    cout<<"\n*** Error ds Handle_BRepTopAdaptor_TopolTool BRepTopAdaptor_Tool::GetSurface()\n"<<endl;
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

