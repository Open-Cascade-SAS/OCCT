// Created on: 1999-12-23
// Created by: Vladislav ROMASHKO
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


#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepFilletAPI_MakeChamfer.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <DBRep.hxx>
#include <DDF.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <gp_Pln.hxx>
#include <QADNaming.hxx>
#include <QANewBRepNaming_Box.hxx>
#include <QANewBRepNaming_Chamfer.hxx>
#include <QANewBRepNaming_Cylinder.hxx>
#include <QANewBRepNaming_Fillet.hxx>
#include <QANewBRepNaming_ImportShape.hxx>
#include <QANewBRepNaming_Prism.hxx>
#include <QANewBRepNaming_Revol.hxx>
#include <QANewBRepNaming_Sphere.hxx>
#include <QANewDBRepNaming.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Tool.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>

//=======================================================================
//function : QANewDBRepNaming_NameBox
//purpose  : NameCylinder Doc Label dx dy dz
//=======================================================================
static Standard_Integer QANewDBRepNaming_NameBox (Draw_Interpretor& di,
						  Standard_Integer nb, 
						  const char ** arg)
{
  if(nb == 6) {
    TDF_Label L;
    if (!QADNaming::Entry(arg, L)) return 1;

    BRepPrimAPI_MakeBox mkBox(Draw::Atof(arg[3]), Draw::Atof(arg[4]), Draw::Atof(arg[5]));
    mkBox.Build();

    if(!mkBox.IsDone()) {
      di << "The box hasn't been built"  << "\n";
      return 1;
    }

    QANewBRepNaming_Box nameBox(L);
    nameBox.Load(mkBox,QANewBRepNaming_SOLID);
    return 0;
  }
  di <<  "QANewDBRepNaming_NameBox : Error"  << "\n";
  return 1;
}           

//=======================================================================
//function : QANewDBRepNaming_NameCylinder
//purpose  : NameCylinder Doc Label R H Angle [ShapeType(Shell/Solid)]
//=======================================================================

static Standard_Integer QANewDBRepNaming_NameCylinder (Draw_Interpretor& di,
						  Standard_Integer nb, 
						  const char ** arg)
{
  if(nb == 6 || nb == 7) {
    TDF_Label L;
    if (!QADNaming::Entry(arg, L)) return 1;

    BRepPrimAPI_MakeCylinder mkCylinder(Draw::Atof(arg[3]), Draw::Atof(arg[4]));
    if (Draw::Atof(arg[5]) != 0.) {
      // for Mandrake-10 - mkv,02.06.06 - mkCylinder = BRepPrimAPI_MakeCylinder(Draw::Atof(arg[3]), Draw::Atof(arg[4]), Draw::Atof(arg[5]));
      BRepPrimAPI_MakeCylinder MakeCylinder(Draw::Atof(arg[3]), Draw::Atof(arg[4]), Draw::Atof(arg[5]));
      mkCylinder = MakeCylinder;
    }
    mkCylinder.Build();

    if(!mkCylinder.IsDone()) {
      di << "The cylinder hasn't been built"  << "\n";
      return 1;
    }

    QANewBRepNaming_Cylinder nameCylinder(L);
    if (nb == 6) nameCylinder.Load(mkCylinder, QANewBRepNaming_SHELL);
    else if (Draw::Atoi(arg[6]) == 0) nameCylinder.Load(mkCylinder, QANewBRepNaming_SHELL);
    else nameCylinder.Load(mkCylinder, QANewBRepNaming_SOLID);

    return 0;
  }
  di <<  "QANewDBRepNaming_NameCylinder : Error"  << "\n";
  return 1;
}           


//=======================================================================
//function : QANewDBRepNaming_NameSphere
//purpose  : NameSphere Doc Label R X Y Z Angle1 Angle2 Angle3 [ShapeType(Shell/Solid)]
//=======================================================================

 static Standard_Integer QANewDBRepNaming_NameSphere (Draw_Interpretor& di,
 						Standard_Integer nb, 
 						const char ** arg)
 {
   TDF_Label L;
   if (!QADNaming::Entry(arg, L)) return 1;
   Standard_Real R = Draw::Atof(arg[3]);
   Standard_Real X = Draw::Atof(arg[4]);
   Standard_Real Y = Draw::Atof(arg[5]);
   Standard_Real Z = Draw::Atof(arg[6]);
   Standard_Real DX = Draw::Atof(arg[7]);
   Standard_Real DY = Draw::Atof(arg[8]);
   Standard_Real DZ = Draw::Atof(arg[9]);
   QANewBRepNaming_TypeOfPrimitive3D type = QANewBRepNaming_SHELL;
   if (nb == 11) type = (QANewBRepNaming_TypeOfPrimitive3D) Draw::Atoi(arg[10]);
   
   QANewBRepNaming_Sphere nameSphere(L); 
   BRepPrimAPI_MakeSphere mkSphere(R);
   if (DX == 0 && DY == 0 && DZ == 0) {
     // for Mandrake-10 - mkv,02.06.06 - mkSphere = BRepPrimAPI_MakeSphere(gp_Pnt(X, Y, Z), R);
     BRepPrimAPI_MakeSphere MakeSphere1(gp_Pnt(X, Y, Z), R);
     mkSphere = MakeSphere1;
   } else {
     // for Mandrake-10 - mkv,02.06.06 - mkSphere = BRepPrimAPI_MakeSphere(gp_Pnt(X, Y, Z), R, DX, DY, DZ);
     BRepPrimAPI_MakeSphere MakeSphere2(gp_Pnt(X, Y, Z), R, DX, DY, DZ);
     mkSphere = MakeSphere2;
   }
   mkSphere.Build();
   if(!mkSphere.IsDone()) {
     di << "The sphere hasn't been built"  << "\n";
     return 1;
   }
   nameSphere.Load(mkSphere, type);
   return 0;
 }           

//===========================================================================
//function : QANewDBRepNaming_NamePrism
//purpose  : NamePrism Doc Label BasisLabel H Direction(X Y Z) [Inf/Semi-Inf]
//===========================================================================

static Standard_Integer QANewDBRepNaming_NamePrism (Draw_Interpretor& di,
					       Standard_Integer nb, 
					       const char ** arg)
{
  if (nb < 5 || nb > 9 || nb == 6 || nb == 7) {
    di <<  "QANewDBRepNaming_NamePrism : Error"  << "\n";
    return 1;
  }

  TDF_Label L,BL;
  if (!QADNaming::Entry(arg, L)) return 1;
  if (!DDF::AddLabel(L.Data(), arg[3], BL)) return 1;  

  Standard_Real H = Draw::Atof(arg[4]);

  Handle(TNaming_NamedShape) BasisNS;
  if (!BL.FindAttribute(TNaming_NamedShape::GetID(), BasisNS)) return 1;
  const TopoDS_Shape& Basis = TNaming_Tool::CurrentShape(BasisNS);

  QANewBRepNaming_Prism namePrism(L);

  if (nb >= 8) {
    gp_Dir Direction(Draw::Atof(arg[5]), Draw::Atof(arg[6]), Draw::Atof(arg[7]));
    gp_Vec Vector(Direction);
    Vector.Normalize();
    Vector *= H;

    if (nb == 8) {
      BRepPrimAPI_MakePrism mkPrism(Basis, Vector);  
      mkPrism.Build();
      
      if(!mkPrism.IsDone()) {
	di << "The prism hasn't been built"  << "\n";
	return 1;
      }
      
      namePrism.Load(mkPrism, Basis);
    }      
    else if (nb == 9) {
      BRepPrimAPI_MakePrism mkPrism(Basis, Direction, (Standard_Boolean)Draw::Atoi(arg[8]));  
      mkPrism.Build();
      
      if(!mkPrism.IsDone()) {
	di << "The prism hasn't been built"  << "\n";
	return 1;
      }
      
      namePrism.Load(mkPrism, Basis);
    }
  }
  
  return 0;
}           

//===========================================================================
//function : QANewDBRepNaming_NameRevol
//purpose  : NameRevol Doc Label BasisLabel AxisLabel Angle 
//===========================================================================

static Standard_Integer QANewDBRepNaming_NameRevol (Draw_Interpretor& di,
					       Standard_Integer nb, 
					       const char ** arg)
{
  if (nb < 5 || nb > 6) {
    di <<  "QANewDBRepNaming_NameRevol : Error"  << "\n";
    return 1;
  }

  TDF_Label L, BL, AL;
  if (!QADNaming::Entry(arg, L)) return 1;

  if (!DDF::AddLabel(L.Data(), arg[3], BL)) return 1;  
  if (!DDF::AddLabel(L.Data(), arg[4], AL)) return 1;  

  Handle(TNaming_NamedShape) BasisNS, AxisNS;
  if (!BL.FindAttribute(TNaming_NamedShape::GetID(), BasisNS)) return 1;
  if (!AL.FindAttribute(TNaming_NamedShape::GetID(), AxisNS)) return 1;
  const TopoDS_Shape& Basis = TNaming_Tool::CurrentShape(BasisNS);
  const TopoDS_Shape& Axis = TNaming_Tool::CurrentShape(AxisNS);

  if (Axis.ShapeType() != TopAbs_EDGE) return 1;

  gp_Ax1 axis;
  TopoDS_Vertex fV = TopExp::FirstVertex(TopoDS::Edge(Axis), Standard_True);
  TopoDS_Vertex lV = TopExp::LastVertex(TopoDS::Edge(Axis), Standard_True);
  if (!fV.IsNull() || !lV.IsNull()) {
    gp_Vec alocalV(BRep_Tool::Pnt(fV),BRep_Tool::Pnt(lV));
    gp_Dir Direction(alocalV);
//    gp_Dir Direction(gp_Vec(BRep_Tool::Pnt(fV), BRep_Tool::Pnt(lV)));
    axis.SetLocation(BRep_Tool::Pnt(fV));
    axis.SetDirection(Direction);
  }
  else {
    TDataXtd_Geometry::Axis(AL, axis);
  }

  QANewBRepNaming_Revol nameRevol(L);

  if(nb == 5) {

    BRepPrimAPI_MakeRevol mkRevol(Basis, axis);  
    mkRevol.Build();

    if(!mkRevol.IsDone()) {
      di << "The revol hasn't been built"  << "\n";
      return 1;
    }

    nameRevol.Load(mkRevol, Basis);
  }
  else if (nb == 6) {

    Standard_Real Angle = Draw::Atof(arg[5]);

    BRepPrimAPI_MakeRevol mkRevol(Basis, axis, Angle);  
    mkRevol.Build();

    if(!mkRevol.IsDone()) {
      di << "The revol hasn't been built"  << "\n";
      return 1;
    }

    nameRevol.Load(mkRevol, Basis);    

  }
  
  return 0;
}           

//===========================================================================
//function : QANewDBRepNaming_NameFillet
//purpose  : NameFillet Doc Label SourceShapeLabel PathLabel Radius
//===========================================================================

static Standard_Integer QANewDBRepNaming_NameFillet (Draw_Interpretor& di,
						Standard_Integer nb, 
						const char ** arg)
{
  if (nb != 6) {
    di <<  "QANewDBRepNaming_NameFillet : Error"  << "\n";
    return 1;
  }

  TDF_Label L, BL, PL;
  if (!QADNaming::Entry(arg, L)) return 1;

  if (!DDF::AddLabel(L.Data(), arg[3], BL)) return 1;  
  if (!DDF::AddLabel(L.Data(), arg[4], PL)) return 1;  
  Standard_Real aRadius = Draw::Atoi(arg[5]);
  Handle(TNaming_NamedShape) BasisNS, PathNS;
  if (!BL.FindAttribute(TNaming_NamedShape::GetID(), BasisNS)) return 1;
  if (!PL.FindAttribute(TNaming_NamedShape::GetID(), PathNS)) return 1;
  const TopoDS_Shape& Basis = TNaming_Tool::CurrentShape(BasisNS);
  const TopoDS_Shape& Path = TNaming_Tool::CurrentShape(PathNS);

  BRepFilletAPI_MakeFillet aFillet(Basis);
  TopExp_Explorer anExp(Path,TopAbs_EDGE);
  if (anExp.More()) {
    for(;anExp.More();anExp.Next()) {
      aFillet.Add(aRadius,TopoDS::Edge(anExp.Current()));
    }
  } else aFillet.Add(aRadius,TopoDS::Edge(Path));
  aFillet.Build();
  if (!aFillet.IsDone()) return 1;
  QANewBRepNaming_Fillet aNaming(L);
  aNaming.Load(Basis, aFillet);
  return 0;
}

//===========================================================================
//function : QANewDBRepNaming_NameChamfer
//purpose  : NameChamfer Doc Label SourceShapeLabel EdgeLabel FaceLabel Distance1 Distance2
//===========================================================================

static Standard_Integer QANewDBRepNaming_NameChamfer (Draw_Interpretor& di,
						 Standard_Integer nb, 
						 const char ** arg)
{
  if (nb != 8) {
    di <<  "QANewDBRepNaming_NameChamfer : Error"  << "\n";
    return 1;
  }

  TDF_Label L, BL, EL, FL;
  Standard_Real aDist1, aDist2;

  if (!QADNaming::Entry(arg, L)) return 1;

  if (!DDF::AddLabel(L.Data(), arg[3], BL)) return 1;  
  if (!DDF::AddLabel(L.Data(), arg[4], EL)) return 1;  
  if (!DDF::AddLabel(L.Data(), arg[5], FL)) return 1;  
  aDist1 = Draw::Atoi(arg[6]);
  aDist2 = Draw::Atoi(arg[7]);

  Handle(TNaming_NamedShape) BasisNS, EdgeNS, FaceNS;
  if (!BL.FindAttribute(TNaming_NamedShape::GetID(), BasisNS)) return 1;
  if (!EL.FindAttribute(TNaming_NamedShape::GetID(), EdgeNS)) return 1;
  if (!FL.FindAttribute(TNaming_NamedShape::GetID(), FaceNS)) return 1;
  const TopoDS_Shape& Basis = TNaming_Tool::CurrentShape(BasisNS);
  const TopoDS_Shape& Edge = TNaming_Tool::CurrentShape(EdgeNS);
  const TopoDS_Shape& Face = TNaming_Tool::CurrentShape(FaceNS);

  BRepFilletAPI_MakeChamfer aChamfer(Basis);
  TopExp_Explorer anExp(Edge,TopAbs_EDGE);
  if (anExp.More()) {
    for(;anExp.More();anExp.Next()) {
      aChamfer.Add(aDist1,aDist2,TopoDS::Edge(anExp.Current()),TopoDS::Face(Face));
    }
  } else {aChamfer.Add(aDist1,aDist2,TopoDS::Edge(Edge),TopoDS::Face(Face));}
  aChamfer.Build();
  if (!aChamfer.IsDone()) return 1;
  QANewBRepNaming_Chamfer aNaming(L);
  aNaming.Load(Basis, aChamfer);
  return 0;
}

//==============================================================
//function : QANewDBRepNaming_NameImportShape
//purpose  : NameImportShape Doc Label DrawShape 
//==============================================================

static Standard_Integer QANewDBRepNaming_NameImportShape (Draw_Interpretor& di,
						     Standard_Integer nb, 
						     const char ** arg)
{
  if (nb != 4) {
    di <<  "QANewDBRepNaming_NameImportShape : Error"  << "\n";
    return 1;
  }

  Handle(TDF_Data) D;
  if (!DDF::GetDF(arg[1],D)) return 1;   
  TDF_Label L;
  if (!DDF::AddLabel(D, arg[2], L)) return 1;  
  
  const TopoDS_Shape& S = DBRep::Get(arg[3]);

  QANewBRepNaming_ImportShape nameIS(L);
  nameIS.Load(S);

  return 0;
}    


//=======================================================================
//function : PrimitiveCommands
//purpose  : 
//=======================================================================

void QANewDBRepNaming::PrimitiveCommands (Draw_Interpretor& theCommands)
{  
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  const char* g = "Naming algorithm commands for primitives" ;
  

  theCommands.Add ("NameBox", 
		   "NameBox Doc Label dx dy dz",
		   __FILE__, QANewDBRepNaming_NameBox, g);

  theCommands.Add ("NameCylinder", 
		   "NameCylinder Doc Label R H Angle [ShapeType(Shell/Solid)]",
		   __FILE__, QANewDBRepNaming_NameCylinder, g);

  theCommands.Add ("NameSphere", 
		   "NameSphere Doc Label R X Y Z Angle1 Angle2 Angle3 [ShapeType(Shell/Solid)]",
		   __FILE__, QANewDBRepNaming_NameSphere, g);

  theCommands.Add ("NamePrism", 
		   "NamePrism Doc Label BasisLabel H Direction(X Y Z) Inf(1/0, by feafault = 0)",
		   __FILE__, QANewDBRepNaming_NamePrism, g);

  theCommands.Add ("NameRevol", 
		   "NameRevol Doc Label BasisLabel AxisLabel Angle",
		   __FILE__, QANewDBRepNaming_NameRevol, g);

  theCommands.Add ("NameFillet", 
		   "NameFillet Doc Label SourceShapeLabel PathLabel Radius",
		   __FILE__, QANewDBRepNaming_NameFillet, g);

  theCommands.Add ("NameChamfer", 
		   "NameChamfer Doc Label SourceShapeLabel EdgeLabel FaceLabel Distance1 Distance2",
		   __FILE__, QANewDBRepNaming_NameChamfer, g);

  theCommands.Add ("NameImportShape", 
		   "NameImportShape Doc Label DrawShape",
		   __FILE__, QANewDBRepNaming_NameImportShape, g);

}
