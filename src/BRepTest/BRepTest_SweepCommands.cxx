// Created on: 1993-07-22
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

#include <BRepTest.hxx>
#include <DBRep.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>

#include <BRepFill.hxx>
#include <BRepBuilderAPI_PipeError.hxx>
#include <BRepFill_Generator.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>
#include <BRepOffsetAPI_MakeEvolved.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepOffsetAPI_MakePipeShell.hxx>
#include <BRepOffsetAPI_MiddlePath.hxx>

#include <BRepLib_MakeWire.hxx>
#include <TopoDS.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopExp_Explorer.hxx>

#include <Precision.hxx>
#include <Law_Interpol.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pnt2d.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

static BRepOffsetAPI_MakePipeShell* Sweep= 0;
static BRepOffsetAPI_ThruSections* Generator = 0;

#include <stdio.h>
#include <Geom_Curve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomFill_Pipe.hxx>
#include <Geom_Surface.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Geom_Circle.hxx>
#include <gp_Ax2.hxx>


//=======================================================================
// prism
//=======================================================================

static Standard_Integer prism(Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n < 6) return 1;

  TopoDS_Shape base = DBRep::Get(a[2]);
  if (base.IsNull()) return 1;

  gp_Vec V(Draw::Atof(a[3]),Draw::Atof(a[4]),Draw::Atof(a[5]));
  
  Standard_Boolean copy = Standard_False;
  Standard_Boolean inf  = Standard_False;
  Standard_Boolean sinf = Standard_False;

  if (n > 6) {
    copy = (*a[6] == 'c') || (*a[6] == 'C');
    inf  = (*a[6] == 'i') || (*a[6] == 'I');
    sinf = (*a[6] == 's') || (*a[6] == 'S');
  }

  TopoDS_Shape res;

  if (inf || sinf) 
    res = BRepPrimAPI_MakePrism(base,gp_Dir(V),inf);
  else
    res = BRepPrimAPI_MakePrism(base,V,copy);

 DBRep::Set(a[1],res);

  return 0;
}


//=======================================================================
// revol
//=======================================================================

static Standard_Integer revol(Draw_Interpretor& ,
			      Standard_Integer n, const char** a)
{
  if (n < 10) return 1; 

  TopoDS_Shape base = DBRep::Get(a[2]);
  if (base.IsNull()) return 1;

  gp_Pnt P(Draw::Atof(a[3]),Draw::Atof(a[4]),Draw::Atof(a[5]));
  gp_Dir D(Draw::Atof(a[6]),Draw::Atof(a[7]),Draw::Atof(a[8]));
  gp_Ax1 A(P,D);

  Standard_Real angle = Draw::Atof(a[9]) * (M_PI / 180.0);
  
  Standard_Boolean copy = n > 10;

  TopoDS_Shape res = BRepPrimAPI_MakeRevol(base,A,angle,copy);

  DBRep::Set(a[1],res);

  return 0;
}


//=======================================================================
// pipe
//=======================================================================

static Standard_Integer pipe(Draw_Interpretor& di,
			     Standard_Integer n, const char** a)
{
  if (n == 1)
  {
    di << "pipe result Wire_spine Profile [Mode [Approx]]\n";
    di << "Mode = 0 - CorrectedFrenet,\n";
    di << "     = 1 - Frenet,\n";
    di << "     = 2 - DiscreteTrihedron\n";
    di << "Approx - force C1-approximation if result is C0\n";
    return 0;
  }
  
  if (n > 1 && n < 4) return 1;

  TopoDS_Shape Spine = DBRep::Get(a[2],TopAbs_WIRE);
  if ( Spine.IsNull()) return 1;

  TopoDS_Shape Profile = DBRep::Get(a[3]);
  if ( Profile.IsNull()) return 1;

  GeomFill_Trihedron Mode = GeomFill_IsCorrectedFrenet;
  if (n >= 5)
  {
    Standard_Integer iMode = atoi(a[4]);
    if (iMode == 1)
      Mode = GeomFill_IsFrenet;
    else if (iMode == 2)
      Mode = GeomFill_IsDiscreteTrihedron;
  }

  Standard_Boolean ForceApproxC1 = Standard_False;
  if (n >= 6)
    ForceApproxC1 = Standard_True;
  
  TopoDS_Shape S = BRepOffsetAPI_MakePipe(TopoDS::Wire(Spine),
                                          Profile,
                                          Mode,
                                          ForceApproxC1);

  DBRep::Set(a[1],S);
  
  return 0;
}

//=======================================================================

static Standard_Integer geompipe(Draw_Interpretor& ,
			     Standard_Integer n, const char** a)
{
  TopoDS_Shape Spine = DBRep::Get(a[2],TopAbs_EDGE);
  if ( Spine.IsNull()) return 1;
  if ( n < 5) return 1;
  TopoDS_Shape Profile = DBRep::Get(a[3],TopAbs_EDGE);
  if ( Profile.IsNull()) return 1;
  Standard_Real aSpFirst,aSpLast,aPrFirst,aPrLast;
  Handle(Geom_Curve) SpineCurve = BRep_Tool::Curve(TopoDS::Edge(Spine),aSpFirst,aSpLast);
  Handle(Geom_Curve) ProfileCurve = BRep_Tool::Curve(TopoDS::Edge(Profile),aPrFirst,aPrLast);
  Handle(GeomAdaptor_HCurve) aAdaptCurve = new GeomAdaptor_HCurve(SpineCurve,aSpFirst,aSpLast);
  Standard_Boolean ByACR = Standard_False;
  Standard_Boolean rotate = Standard_False;
  Standard_Real Radius = Draw::Atof(a[4]);
  gp_Pnt ctr;
  gp_Vec norm;
  ProfileCurve->D1(aSpFirst,ctr,norm);
  gp_Vec xAxisStart(ctr,SpineCurve->Value(aSpFirst));
  gp_Ax2 aAx2Start(ctr,norm,xAxisStart);
  Handle(Geom_Circle) cStart=new Geom_Circle(aAx2Start,Radius);                       
  Standard_Integer k =5;
  if(n > k)
    ByACR = (Draw::Atoi(a[k++]) ==1);
  if(n > k)
    rotate = (Draw::Atoi(a[k++])==1);
  GeomFill_Pipe aPipe(ProfileCurve,aAdaptCurve,cStart,ByACR,rotate);
  aPipe.Perform(Standard_True);
  if (!aPipe.IsDone())
  {
    cout << "GeomFill_Pipe cannot make a surface" << endl;
    return 1;
  }
  Handle(Geom_Surface) Sur=aPipe.Surface();
  TopoDS_Face F;
  if(!Sur.IsNull())
    F = BRepBuilderAPI_MakeFace(Sur, Precision::Confusion());
  DBRep::Set(a[1],F);
  return 0;
}

//=======================================================================
//function : evolved
//purpose  : 
//=======================================================================

Standard_Integer evolved(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if ( n == 1) {
    //cout << " 1) evolved result base profil : "<< endl;
    //cout << "        The relative position of the profil on the base" << endl;
    //cout << "        is given in the referencial axis. " << endl;
    //cout << " 2) evolved result base profil o : "<< endl;
    //cout << "        This position is automatically computed." << endl;
    di << " 1) evolved result base profil : \n";
    di << "        The relative position of the profil on the base\n";
    di << "        is given in the referencial axis. \n";
    di << " 2) evolved result base profil o : \n";
    di << "        This position is automatically computed.\n";
    return 0;
  }

  if ( n < 4 ) return 1;
  Standard_Boolean IsAFace = Standard_False;
  Standard_Boolean Solid   = (!strcmp(a[0],"evolvedsolid"));


 
  TopoDS_Shape Base = DBRep::Get(a[2],TopAbs_WIRE,Standard_False);
  if ( Base.IsNull()) {
    Base = DBRep::Get(a[2],TopAbs_FACE,Standard_False);
    IsAFace = Standard_True; 
  }
  if ( Base.IsNull()) return 1;

  TopoDS_Shape InpuTShape(DBRep::Get(a[3],TopAbs_WIRE,Standard_False));
  TopoDS_Wire Prof = TopoDS::Wire(InpuTShape);
//  TopoDS_Wire Prof = 
//    TopoDS::Wire(DBRep::Get(a[3],TopAbs_WIRE,Standard_False));
  if ( Prof.IsNull()) return 1;

  if (IsAFace) {
    TopoDS_Shape Volevo 
      = BRepOffsetAPI_MakeEvolved(TopoDS::Face(Base),Prof,GeomAbs_Arc,n == 4,Solid);
    DBRep::Set(a[1],Volevo);
  }
  else {
    TopoDS_Shape Volevo 
      = BRepOffsetAPI_MakeEvolved(TopoDS::Wire(Base),Prof,GeomAbs_Arc,n == 4,Solid);
    DBRep::Set(a[1],Volevo);
  }

  return 0;
}


//=======================================================================
//function : pruled
//purpose  : 
//=======================================================================

static Standard_Integer pruled(Draw_Interpretor& ,
			       Standard_Integer n, const char** a)
{
  if ( n != 4) return 1;

  Standard_Boolean YaWIRE = Standard_False;
  TopoDS_Shape S1 = DBRep::Get(a[2],TopAbs_EDGE);
  if ( S1.IsNull()) {
    S1 = DBRep::Get(a[2],TopAbs_WIRE);
    if (S1.IsNull()) return 1;
    YaWIRE = Standard_True;
  }

  TopoDS_Shape S2 = DBRep::Get(a[3],TopAbs_EDGE);
  if ( S2.IsNull()) {
    S2 = DBRep::Get(a[3],TopAbs_WIRE);
    if ( S2.IsNull()) return 1;
    if (!YaWIRE) {
      S1 = BRepLib_MakeWire(TopoDS::Edge(S1));
      YaWIRE = Standard_True;
    }
  }
  else if ( YaWIRE) {
    S2 = BRepLib_MakeWire(TopoDS::Edge(S2));
  }

  TopoDS_Shape Result;
  if ( YaWIRE) {
    Result = BRepFill::Shell(TopoDS::Wire(S1),TopoDS::Wire(S2));
  }
  else {
    Result = BRepFill::Face(TopoDS::Edge(S1),TopoDS::Edge(S2));
  }

  DBRep::Set(a[1],Result);
  return 0;
}


//=======================================================================
//function : gener
//purpose  : Create a surface between generating wires
//=======================================================================

Standard_Integer gener(Draw_Interpretor&, Standard_Integer n, const char** a)
{
  if ( n < 4) return 1;

  TopoDS_Shape Shape; 

  BRepFill_Generator aGenerator;
  
  for ( Standard_Integer i = 2; i<= n-1 ; i++) {
    Shape = DBRep::Get(a[i],TopAbs_WIRE);
    if ( Shape.IsNull()) 
      return 1;

    aGenerator.AddWire(TopoDS::Wire(Shape));
  }

  aGenerator.Perform();

  TopoDS_Shell Shell = aGenerator.Shell();
  
  DBRep::Set(a[1], Shell);


  return 0;
}


//=======================================================================
//function : thrusections
//purpose  : 
//=======================================================================

Standard_Integer thrusections(Draw_Interpretor&, Standard_Integer n, const char** a)
{
  if (n<6) return 1;

  Standard_Boolean check = Standard_True;
  Standard_Boolean samenumber = Standard_True;
  Standard_Integer index = 2;
    // Lecture option
  if (!strcmp(a[1],"-N")) {
    if (n<7) return 1;
    check = Standard_False;
    index++;
  }

  TopoDS_Shape Shape; 

  Standard_Boolean issolid = ( Draw::Atoi(a[index]) == 1 );
  Standard_Boolean isruled = ( Draw::Atoi(a[index+1]) == 1 );

  if (Generator != 0)
  {
    delete Generator; 
    Generator = 0;
  }
  Generator = new BRepOffsetAPI_ThruSections(issolid,isruled);
  
  Standard_Integer NbEdges = 0;
  Standard_Boolean IsFirstWire = Standard_False;
  for ( Standard_Integer i = index+2; i<= n-1 ; i++) {
    Standard_Boolean IsWire = Standard_True;
    Shape = DBRep::Get(a[i], TopAbs_WIRE);
    if (!Shape.IsNull())
      {
	Generator->AddWire(TopoDS::Wire(Shape));
	if (!IsFirstWire)
	  IsFirstWire = Standard_True;
	else
	  IsFirstWire = Standard_False;
      }
    else
      {
	Shape = DBRep::Get(a[i], TopAbs_VERTEX);
	IsWire = Standard_False;
	if (!Shape.IsNull())
	  Generator->AddVertex(TopoDS::Vertex(Shape));
	else
	  return 1;
      }

    Standard_Integer cpt = 0;
    TopExp_Explorer PE;
    for (PE.Init(Shape, TopAbs_EDGE); PE.More(); PE.Next()) {
      cpt++;
    }
    if (IsFirstWire) 
      NbEdges = cpt;
    else
      if (IsWire && cpt != NbEdges)
	samenumber = Standard_False;
    
  }

  check = (check || !samenumber);
  Generator->CheckCompatibility(check);

  Generator->Build();

  if (Generator->IsDone()) {
    TopoDS_Shape Shell = Generator->Shape();
    DBRep::Set(a[index-1], Shell);
  }
  else {
    cout << "Algorithm is not done" << endl;
  }
  return 0;
}

//============================================================================
//function : genthrus
//purpose  : returns generated shape for subshape of a section of thrusections
//           Thrusections must be done previously
//============================================================================
static Standard_Integer genthrus(Draw_Interpretor& di,
                                 Standard_Integer n, const char** a)
{
  if (n != 3)
  {
    di << "genthrus: it is called after thrusections command\n";
    di << "returns:\n";
    di << "- chain of generated faces for sub-edge of a profile;\n";
    di << "- chain of generated edges for sub-vertex of a profile;\n";
    di << "- bunch of chains of generated edges for start or end vertex if it is degenerated section.\n";
    di << "Usage: genthrus res subshape_of_profile, thrusections must be done\n";
    return 1;
  }
  if (Generator == 0)
  {
    di << "You have forgotten the <<thrusections>> command !\n";
    return 1;
  }
  if (!Generator->IsDone())
  {
    di << "Thrusections is not done\n";
    return 1;
  }
  TopoDS_Shape aShape = DBRep::Get(a[2]);
  if (aShape.IsNull())
  {
    cout<<"Null subshape"<<endl;
    return 1;
  }
  const TopTools_ListOfShape& Edges = Generator->Generated(aShape);
  TopoDS_Compound aCompound;
  BRep_Builder BB;
  BB.MakeCompound(aCompound);
  TopTools_ListIteratorOfListOfShape iter(Edges);
  for (; iter.More(); iter.Next())
  {
    const TopoDS_Shape& anEdge = iter.Value();
    BB.Add(aCompound, anEdge);
  }

  DBRep::Set(a[1], aCompound);
  return 0;
}

//=======================================================================
//  mksweep
//=======================================================================
static Standard_Integer mksweep(Draw_Interpretor& ,
			     Standard_Integer n, const char** a)
{
  if ( n != 2) return 1;
  TopoDS_Shape Spine = DBRep::Get(a[1],TopAbs_WIRE);
  if ( Spine.IsNull()) return 1;
  if (Sweep !=0)  {
    delete Sweep; 
    Sweep = 0;
  }
  Sweep = new BRepOffsetAPI_MakePipeShell(TopoDS::Wire(Spine));
  return 0;
}

//=======================================================================
//  setsweep
//=======================================================================
static Standard_Integer setsweep(Draw_Interpretor& di,
				 Standard_Integer n, const char** a)
{
  if ( n == 1) {
    //cout << "setsweep options [arg1 [arg2 [...]]] : options are :" << endl;
    //cout << "   -FR : Tangent and Normal are given by Frenet trihedron" <<endl;
    //cout << "   -CF : Tangente is given by Frenet," << endl;
    //cout << "         the Normal is computed to minimize the torsion " << endl;
    //cout << "   -DX Surf : Tangent and Normal are given by Darboux trihedron,"
    //  <<endl;     
    //cout << "       Surf have to be a shell or a face" <<endl;
    //cout << "   -CN dx dy dz : BiNormal is given by dx dy dz" << endl;
    //cout << "   -FX Tx Ty TZ [Nx Ny Nz] : Tangent and Normal are fixed" <<endl;
    //cout << "   -G guide  0|1(ACR|Plan)  0|1(contact|no contact) : with guide"<<endl;
    di << "setsweep options [arg1 [arg2 [...]]] : options are :\n";
    di << "   -FR : Tangent and Normal are given by Frenet trihedron\n";
    di << "   -CF : Tangente is given by Frenet,\n";
    di << "         the Normal is computed to minimize the torsion \n";
    di << "   -DT : discrete trihedron\n";
    di << "   -DX Surf : Tangent and Normal are given by Darboux trihedron,\n";     
    di << "       Surf have to be a shell or a face\n";
    di << "   -CN dx dy dz : BiNormal is given by dx dy dz\n";
    di << "   -FX Tx Ty TZ [Nx Ny Nz] : Tangent and Normal are fixed\n";
    di << "   -G guide  0|1(Plan|ACR)  0|1|2(no contact|contact|contact on border) : with guide\n";
    return 0;
  }

   if (Sweep ==0) {
     //cout << "You have forgotten the <<mksweep>> command  !"<< endl;
     di << "You have forgotten the <<mksweep>> command  !\n";
     return 1;
   }
  if (!strcmp(a[1],"-FR")) {
    Sweep->SetMode(Standard_True);
  }
  else if (!strcmp(a[1],"-CF")) {
    Sweep->SetMode(Standard_False);
  }
  else if (!strcmp(a[1],"-DT")) {
    Sweep->SetDiscreteMode();
  }
  else if (!strcmp(a[1],"-DX")) {
    if (n!=3) {
      //cout << "bad arguments !" << endl;
      di << "bad arguments !\n";
      return 1;
    }
    TopoDS_Shape Surf;
    Surf = DBRep::Get(a[2],TopAbs_SHAPE);
    if (Surf.IsNull()) {
       //cout << a[2] <<"is not a shape !" << endl;
       di << a[2] <<"is not a shape !\n";
      return 1;
    }
    Sweep->SetMode(Surf);
  }
  else if (!strcmp(a[1],"-CN")) {
    if (n!=5) {
      //cout << "bad arguments !" << endl;
      di << "bad arguments !\n";
      return 1;
    }
    gp_Dir D(Draw::Atof(a[2]), Draw::Atof(a[3]), Draw::Atof(a[4]));
    Sweep->SetMode(D);;
  }
  else if (!strcmp(a[1],"-FX")) {
    if ((n!=5)&&(n!=8)) {
      //cout << "bad arguments !" << endl;
      di << "bad arguments !\n";
      return 1;
    }
    gp_Dir D(Draw::Atof(a[2]), Draw::Atof(a[3]), Draw::Atof(a[4]));
    if (n==8) {
      gp_Dir DN(Draw::Atof(a[5]), Draw::Atof(a[6]), Draw::Atof(a[7]));
      gp_Ax2 Axe(gp_Pnt(0., 0., 0.), D, DN);
      Sweep->SetMode(Axe);
    }
    else {
      gp_Ax2 Axe(gp_Pnt(0., 0., 0.), D);
      Sweep->SetMode(Axe);
    }
  }
  else if (!strcmp(a[1],"-G"))  // contour guide
    {
     if (n != 5)
       {
	 //cout << "bad arguments !" << endl;
	 di << "bad arguments !\n";
	 return 1; 
       }
     else
	{  
	  TopoDS_Shape Guide = DBRep::Get(a[2],TopAbs_WIRE);
          Standard_Boolean CurvilinearEquivalence = Draw::Atoi(a[3]) != 0;
          Standard_Integer KeepContact = Draw::Atoi(a[4]);
          Sweep->SetMode(TopoDS::Wire(Guide),
                         CurvilinearEquivalence,
                         (BRepFill_TypeOfContact)KeepContact);
	}
    }
 
  else {
    //cout << "The option "<< a[1] << " is unknown !" << endl;
    di << "The option "<< a[1] << " is unknown !\n";
    return 1;
  }
  return 0;
}


//=======================================================================
//  addsweep
//=======================================================================
static Standard_Integer addsweep(Draw_Interpretor& di,
			     Standard_Integer n, const char** a)
{
  if ( n == 1) {
    //cout << "addsweep wire/vertex [Vertex] [-T] [-R] [u0 v0 u1 v1 [...[uN vN]]] : options are :" << endl;
    //cout << "   -T : the wire/vertex have to be translated to assume contact"<< endl;
    //cout << "        with the spine" <<endl;
    //cout << "   -R : the wire have to be rotated to assume orthogonality"<<endl;
    //cout << "        with the spine's tangent" << endl;
    di << "addsweep wire/vertex [Vertex] [-T] [-R] [u0 v0 u1 v1 [...[uN vN]]] : options are :\n";
    di << "   -T : the wire/vertex have to be translated to assume contact\n";
    di << "        with the spine\n";
    di << "   -R : the wire have to be rotated to assume orthogonality\n";
    di << "        with the spine's tangent\n";
    return 0;
  }

  if (Sweep ==0) {
    //cout << "You have forgotten the <<mksweep>> command  !"<< endl;
    di << "You have forgotten the <<mksweep>> command  !\n";
    return 1;
  }

  TopoDS_Shape  Section;
  TopoDS_Vertex Vertex;
  Handle(Law_Interpol) thelaw;

  Section = DBRep::Get(a[1], TopAbs_SHAPE);
  if (Section.IsNull() ||
      (Section.ShapeType() != TopAbs_WIRE &&
       Section.ShapeType() != TopAbs_VERTEX))
    {
      //cout << a[1] <<"is not a wire and is not a vertex!" << endl;
      di << a[1] <<" is not a wire and is not a vertex!\n";
      return 1;
    }

  Standard_Boolean HasVertex=Standard_False, 
                   isT=Standard_False, 
                   isR=Standard_False;

  if (n > 2) { 
    Standard_Integer cur = 2;
    // Reading of Vertex
    TopoDS_Shape InputVertex(DBRep::Get(a[cur],TopAbs_VERTEX));
    Vertex = TopoDS::Vertex(InputVertex);
//    Vertex = TopoDS::Vertex(DBRep::Get(a[cur],TopAbs_VERTEX));
    if (!Vertex.IsNull()) {
      cur++;
      HasVertex = Standard_True;
    }
   
    // Reading of the translation option
    if ((n>cur) && !strcmp(a[cur],"-T")) {
      cur++;
      isT = Standard_True;
    }

    // Reading of the rotation option
    if ((n>cur) && !strcmp(a[cur],"-R")) {
      cur++;
      isR = Standard_True;
    }

    // law ?
    if (n>cur) {
      Standard_Integer nbreal = n-cur;
      if ( (nbreal < 4) || (nbreal % 2 != 0) ) {
	//cout << "bad arguments ! :" <<a[cur] << endl;
	di << "bad arguments ! :" <<a[cur] << "\n";
      } else { //law of interpolation
	Standard_Integer ii, L= nbreal/2;
	TColgp_Array1OfPnt2d ParAndRad(1, L);
	for (ii=1; ii<=L; ii++, cur+=2) {
          ParAndRad(ii).SetX(Draw::Atof(a[cur]));
          ParAndRad(ii).SetY(Draw::Atof(a[cur+1]));
        }
	thelaw = new (Law_Interpol) ();
	thelaw->Set(ParAndRad, 
		   Abs(ParAndRad(1).Y() - ParAndRad(L).Y()) < Precision::Confusion());
      }
    }
  }

  if (thelaw.IsNull()) {
    if (HasVertex) Sweep->Add(Section, Vertex, isT, isR);
    else           Sweep->Add(Section, isT, isR);
  }
  else {
    if (HasVertex) Sweep->SetLaw(Section, thelaw, Vertex, isT, isR);
    else           Sweep->SetLaw(Section, thelaw, isT, isR);
  }

  return 0;
}

//=======================================================================
//  deletesweep
//=======================================================================
static Standard_Integer deletesweep(Draw_Interpretor& di,
				    Standard_Integer n, const char** a)
{
  if ( n != 2) {
    return 1;
  }
  TopoDS_Wire Section;
  TopoDS_Shape InputShape(DBRep::Get(a[1],TopAbs_SHAPE));
  Section = TopoDS::Wire(InputShape);
//  Section = TopoDS::Wire(DBRep::Get(a[1],TopAbs_SHAPE));
  if (Section.IsNull()) {
    //cout << a[1] <<"is not a wire !" << endl;
    di << a[1] <<"is not a wire !\n";
    return 1;
  }  

  Sweep->Delete(Section);

  return 0;
}

//=======================================================================
//  buildsweep
//=======================================================================
static Standard_Integer buildsweep(Draw_Interpretor& di,
				   Standard_Integer n, const char** a)
{
  if ( n == 1) {
    //cout << "build sweep result [-M/-C/-R] [-S] [tol] : options are" << endl;
    //cout << "   -M : Discontinuities are treated by Modfication of"<< endl; 
    //cout << "        the sweeping mode : it is the default" <<endl;
    //cout << "   -C : Discontinuities are treated like Right Corner" << endl;
    //cout << "        Treatement is Extent && Intersect" << endl;
    //cout << "   -R : Discontinuities are treated like Round Corner" << endl;
    //cout << "        Treatement is Intersect and Fill" << endl;
    //cout << "   -S : To build a Solid" << endl;
    di << "build sweep result [-M/-C/-R] [-S] [tol] : options are\n";
    di << "   -M : Discontinuities are treated by Modfication of\n"; 
    di << "        the sweeping mode : it is the default\n";
    di << "   -C : Discontinuities are treated like Right Corner\n";
    di << "        Treatement is Extent && Intersect\n";
    di << "   -R : Discontinuities are treated like Round Corner\n";
    di << "        Treatement is Intersect and Fill\n";
    di << "   -S : To build a Solid\n";
    return 0;
  }

  Standard_Boolean mksolid = Standard_False;
  if (Sweep ==0) {
    //cout << "You have forgotten the <<mksweep>> command  !"<< endl;
    di << "You have forgotten the <<mksweep>> command  !\n";
    return 1;
  }

  if (!Sweep->IsReady()) {
    //cout << "You have forgotten the <<addsweep>> command  !"<< endl;
    di << "You have forgotten the <<addsweep>> command  !\n";
    return 1;
  }

  TopoDS_Shape result;
  Standard_Integer cur=2;
  if (n>cur) {
    BRepBuilderAPI_TransitionMode Transition = BRepBuilderAPI_Transformed;

    // Reading Transition
    if (!strcmp(a[cur],"-C")) {
      Transition = BRepBuilderAPI_RightCorner;
      cur++;
    }
    else if (!strcmp(a[cur],"-R")) {
      Transition = BRepBuilderAPI_RoundCorner;
      cur++;
    }
    Sweep->SetTransitionMode(Transition);
  }
  // Reading solid ?
  if ((n>cur) && (!strcmp(a[cur],"-S")) ) mksolid = Standard_True;

  // Calcul le resultat
  Sweep->Build();
  if (!Sweep->IsDone()) {
    //cout << "Buildsweep : Not Done" << endl;
    di << "Buildsweep : Not Done\n";
    BRepBuilderAPI_PipeError Stat = Sweep->GetStatus(); 
    if (Stat == BRepBuilderAPI_PlaneNotIntersectGuide) {
      //cout << "Buildsweep : One Plane not intersect the guide" << endl;
      di << "Buildsweep : One Plane not intersect the guide\n";
    }
    if (Stat == BRepBuilderAPI_ImpossibleContact) {
      //cout << "BuildSweep : One section can not be in contact with the guide" << endl;
      di << "BuildSweep : One section can not be in contact with the guide\n";
    }
    return 1;
  }
  else {
    if (mksolid) {
      Standard_Boolean B;
      B = Sweep->MakeSolid();
      //if (!B) cout << " BuildSweep : It is impossible to make a solid !" << endl;
      if (!B) di << " BuildSweep : It is impossible to make a solid !\n";
    }
    result = Sweep->Shape();
    DBRep::Set(a[1],result);
  }

  return 0;
}

//=======================================================================
//function : gensweep
//purpose  : returns generated shape for subshape of a section of sweep
//           Sweep must be done previously
//=======================================================================
static Standard_Integer gensweep(Draw_Interpretor&,
                                 Standard_Integer n, const char** a)
{
  if (n != 3)
  {
    cout<<"Usage: gensweep res subshape_of_profile, sweep must be done"<<endl;
    return 1;
  }
  if (!Sweep->IsDone())
  {
    cout<<"Sweep is not done"<<endl;
    return 1;
  }
  TopoDS_Shape aShape = DBRep::Get(a[2]);
  if (aShape.IsNull())
  {
    cout<<"Null subshape"<<endl;
    return 1;
  }
  TopTools_ListOfShape Shells = Sweep->Generated(aShape);
  TopoDS_Compound aCompound;
  BRep_Builder BB;
  BB.MakeCompound(aCompound);
  TopTools_ListIteratorOfListOfShape itsh(Shells);
  for (; itsh.More(); itsh.Next())
  {
    const TopoDS_Shape& aShell = itsh.Value();
    BB.Add(aCompound, aShell);
  }

  DBRep::Set(a[1], aCompound);
  return 0;
}

//=======================================================================
//function : errorsweep
//purpose  : returns the summary error on resulting surfaces
//           reached by Sweep
//=======================================================================
static Standard_Integer errorsweep(Draw_Interpretor& di,
                                   Standard_Integer, const char**)
{
  if (!Sweep->IsDone())
  {
    di << "Sweep is not done\n";
    return 1;
  }
  Standard_Real ErrorOnSurfaces = Sweep->ErrorOnSurface();
  di << "Tolerance on surfaces = " << ErrorOnSurfaces << "\n";
  return 0;
}

//=======================================================================
//  simulsweep
//=======================================================================
static Standard_Integer simulsweep(Draw_Interpretor& di,
				   Standard_Integer n, const char** a)
{
  if ( (n!=3) && (n!=4) ) return 1;
  
  if (Sweep ==0) {
    //cout << "You have forgotten the <<mksweep>> command  !"<< endl;
    di << "You have forgotten the <<mksweep>> command  !\n";
    return 1;
  }
  
  if (!Sweep->IsReady()) {
    //cout << "You have forgotten the <<addsweep>> command  !"<< endl;
    di << "You have forgotten the <<addsweep>> command  !\n";
    return 1;
  }
  
  char name[100];
  TopTools_ListOfShape List;
  TopTools_ListIteratorOfListOfShape it;
  Standard_Integer N, ii;
  N = Draw::Atoi(a[2]);

  if (n>3) {
    BRepBuilderAPI_TransitionMode Transition = BRepBuilderAPI_Transformed;
    // Lecture Transition
    if (!strcmp(a[3],"-C")) {
      Transition = BRepBuilderAPI_RightCorner;
    }
    else if (!strcmp(a[3],"-R")) {
      Transition = BRepBuilderAPI_RoundCorner;
    }
    Sweep->SetTransitionMode(Transition);
  }

  // Calculate the result
  Sweep->Simulate(N, List);
  for (ii=1, it.Initialize(List); it.More(); it.Next(), ii++) {
    Sprintf(name,"%s_%d",a[1],ii);
    DBRep::Set(name, it.Value());
  }

  return 0;
}

//=======================================================================
//  middlepath
//=======================================================================
static Standard_Integer middlepath(Draw_Interpretor& /*di*/,
				   Standard_Integer n, const char** a)
{
  if (n < 5) return 1;

  TopoDS_Shape aShape = DBRep::Get(a[2]);
  if (aShape.IsNull()) return 1;

  TopoDS_Shape StartShape = DBRep::Get(a[3]);
  if (StartShape.IsNull()) return 1;
  
  TopoDS_Shape EndShape   = DBRep::Get(a[4]);
  if (EndShape.IsNull()) return 1;

  BRepOffsetAPI_MiddlePath Builder(aShape, StartShape, EndShape);
  Builder.Build();

  TopoDS_Shape Result = Builder.Shape();
  DBRep::Set(a[1], Result);

  return 0;
}

//=======================================================================
//function : SweepCommands
//purpose  : 
//=======================================================================

void  BRepTest::SweepCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  DBRep::BasicCommands(theCommands);

  const char* g = "Sweep commands";
  
  theCommands.Add("prism",
		  "prism result base dx dy dz [Copy | Inf | Seminf]",
		  __FILE__,prism,g);
  
  theCommands.Add("revol",
		  "revol result base px py pz dx dy dz angle [Copy]",
		  __FILE__,revol,g);
  
  theCommands.Add("pipe",
		  "pipe result Wire_spine Profile [Mode [Approx]], no args to get help",
		  __FILE__,pipe,g);
  
  theCommands.Add("evolved",
		  "evolved , no args to get help",
		  __FILE__,evolved,g);  

  theCommands.Add("evolvedsolid",
		  "evolved , no args to get help",
		  __FILE__,evolved,g);  
  
  theCommands.Add("pruled",
		  "pruled result Edge1/Wire1 Edge2/Wire2",
		  __FILE__,pruled,g);

  theCommands.Add("gener", "gener result wire1 wire2 [..wire..]",
		  __FILE__,gener,g);

  theCommands.Add("thrusections", "thrusections [-N] result issolid isruled shape1 shape2 [..shape..], the option -N means no check on wires, shapes must be wires or vertices (only first or last)",
		  __FILE__,thrusections,g);

  theCommands.Add("genthrus", "genthrus res subshape_of_profile",
		  __FILE__,genthrus,g);
  
  theCommands.Add("mksweep", "mksweep wire",
		  __FILE__,mksweep,g);

  theCommands.Add("setsweep", "setsweep  no args to get help",
		  __FILE__,setsweep,g);
  
  theCommands.Add("addsweep", 
		  "addsweep wire [vertex] [-M ] [-C] [auxiilaryshape]:no args to get help",
		  __FILE__,addsweep,g);

 theCommands.Add("deletesweep", 
		  "deletesweep wire, To delete a section",
		  __FILE__,deletesweep,g);

 theCommands.Add("buildsweep", "builsweep [r] [option] [Tol] , no args to get help",
		  __FILE__,buildsweep,g);

 theCommands.Add("gensweep", "gensweep res subshape_of_profile",
		  __FILE__,gensweep,g);

 theCommands.Add("errorsweep", "errorsweep: returns the summary error on resulting surfaces reached by Sweep",
		  __FILE__,errorsweep,g);

  theCommands.Add("simulsweep", "simulsweep r [n] [option]"
		  __FILE__,simulsweep,g);
  theCommands.Add("geompipe", "geompipe r spineedge profileedge radius [byACR [byrotate]]"
		  __FILE__,geompipe,g);
  
  theCommands.Add("middlepath", "middlepath res shape startshape endshape",
		  __FILE__,middlepath,g);
}

