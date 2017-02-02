// Created on: 1992-07-02
// Created by: Philippe DAUTRY
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


#include <BRepSweep_Builder.hxx>
#include <BRepSweep_Iterator.hxx>
#include <BRepSweep_NumLinearRegularSweep.hxx>
#include <BRepSweep_Tool.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoMoreObject.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_RangeError.hxx>
#include <Sweep_NumShape.hxx>
#include <Sweep_NumShapeIterator.hxx>
#include <Sweep_NumShapeTool.hxx>
#include <TopAbs.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_SequenceOfShape.hxx>

//=======================================================================
//function : BRepSweep_NumLinearRegularSweep
//purpose  : Create a Regular Sweep.
//=======================================================================
BRepSweep_NumLinearRegularSweep::BRepSweep_NumLinearRegularSweep
  (const BRepSweep_Builder& aBuilder,
  const TopoDS_Shape& aGenShape,
  const Sweep_NumShape& aDirShape):

myBuilder(aBuilder),
  myGenShape(aGenShape),
  myDirWire(aDirShape),

  myGenShapeTool(aGenShape),
  myDirShapeTool(aDirShape),

  // *****************************************************************
  // Les Tableaux
  // *****************************************************************

  myShapes(1,myGenShapeTool.NbShapes(),
  1,myDirShapeTool.NbShapes()),
  myBuiltShapes(1,myGenShapeTool.NbShapes(),
  1,myDirShapeTool.NbShapes())
{
  myBuiltShapes.Init(Standard_False); 
}

//=======================================================================
//function : ~BRepSweep_NumLinearRegularSweep
//purpose  : Destructor
//=======================================================================

BRepSweep_NumLinearRegularSweep::~BRepSweep_NumLinearRegularSweep()
{
}

//=======================================================================
//function : Shape
//purpose  : Returns the global Shape.
//=======================================================================

TopoDS_Shape BRepSweep_NumLinearRegularSweep::Shape () 
{
  if (HasShape(myGenShape,myDirWire)) return Shape(myGenShape,myDirWire);
  else {
    TopoDS_Shape bidon;
    return bidon;
  }
}


//=======================================================================
//function : Shape
//purpose  : Returns the Shape generated with aGenS.
//=======================================================================

TopoDS_Shape BRepSweep_NumLinearRegularSweep::Shape (const TopoDS_Shape& aGenS) 
{
  if (myGenShapeTool.Index(aGenS) != 0 &&
    HasShape(aGenS,myDirWire)) return Shape(aGenS,myDirWire);
  else {
    TopoDS_Shape bidon;
    return bidon;
  }
}


//=======================================================================
//function : Shape
//purpose  : Returns the Shape indexed by the arguments.
//=======================================================================

TopoDS_Shape BRepSweep_NumLinearRegularSweep::Shape (const TopoDS_Shape& aGenS, 
  const Sweep_NumShape& aDirS)
{
  Standard_Integer iGenS = myGenShapeTool.Index(aGenS);
  Standard_Integer iDirS = myDirShapeTool.Index(aDirS);
  if (!myBuiltShapes(iGenS,iDirS)){
    TopoDS_Shape newShape;
    TopoDS_Shape bGenS,cGenS,subGenS,subsubGenS;
    Sweep_NumShape bDirS,subDirS;
    BRepSweep_Iterator It;
    Sweep_NumShapeIterator Kt;
    BRepSweep_Iterator Lt;
    TopAbs_Orientation Or,Pr;
    if (myDirShapeTool.Type(aDirS)==TopAbs_VERTEX){
      //Ici on construit les "planchers" du Shape.
      TopAbs_ShapeEnum aGenSType = myGenShapeTool.Type(aGenS);
      switch (aGenSType){
      case TopAbs_VERTEX : 
        myShapes(iGenS,iDirS)=MakeEmptyVertex(aGenS,aDirS);
        break;
      case TopAbs_EDGE :
        myShapes(iGenS,iDirS)=MakeEmptyGeneratingEdge(aGenS,aDirS);
        break;
      case TopAbs_WIRE :
        myBuilder.MakeWire(myShapes(iGenS,iDirS));
        break;
      case TopAbs_FACE :
        myShapes(iGenS,iDirS)=MakeEmptyFace(aGenS,aDirS);
        break;
      case TopAbs_SHELL :
        myBuilder.MakeShell(myShapes(iGenS,iDirS));
        break;
      case TopAbs_SOLID :
        throw Standard_NoSuchObject("Solids are not Processed");
        break;
      case TopAbs_COMPSOLID :
        throw Standard_NoSuchObject("Solids are not Processed");
        break;
      case TopAbs_COMPOUND :
        myBuilder.MakeCompound(myShapes(iGenS,iDirS));
        break;
      default:
        throw Standard_NoSuchObject("Unknown Shape");
        break;
      }
      bGenS = aGenS;
      myGenShapeTool.SetOrientation(bGenS,TopAbs_FORWARD);
      for (It.Init(bGenS);It.More();It.Next()){
        subGenS = It.Value();
        Or = It.Orientation();
        if(HasShape(subGenS,aDirS)){
          newShape = Shape(subGenS,aDirS);
          if (GGDShapeIsToAdd(myShapes(iGenS,iDirS),newShape,
            aGenS,subGenS,aDirS)){
              //Les "planchers" doivent etre construits par les 
              //fonctions de construcion geometrique identiquement 
              //au shape generateur.
              //On leur recolle juste une orientation pour etre bien 
              //sur.

              myBuilder.Add(myShapes(iGenS,iDirS),newShape,Or);
              TopAbs_ShapeEnum subGenSType = myGenShapeTool.Type(subGenS);
              if (aGenSType==TopAbs_FACE){   
                if(subGenSType==TopAbs_VERTEX){
                  SetParameters(myShapes(iGenS,iDirS),
                    newShape,aGenS,subGenS,aDirS);
                }
                else if(subGenSType==TopAbs_EDGE){
                  SetPCurve(myShapes(iGenS,iDirS),newShape,
                    aGenS,subGenS,aDirS,Or);
                }	    
                else if(subGenSType==TopAbs_WIRE){
                  BRepSweep_Iterator Jt;
                  cGenS = subGenS;
                  myGenShapeTool.SetOrientation(cGenS,TopAbs_FORWARD);
                  for (Jt.Init(cGenS);Jt.More();Jt.Next()){
                    subsubGenS = Jt.Value();
                    Pr = Jt.Orientation();
                    if(HasShape(subsubGenS,aDirS)){
                      TopoDS_Shape newsubEdge = Shape(subsubGenS,aDirS);
                      SetPCurve(myShapes(iGenS,iDirS),newsubEdge,
                        aGenS,subsubGenS,aDirS,Pr);
                    }
                  }
                }
              }
              else if(aGenSType==TopAbs_EDGE){
                SetGeneratingParameter(myShapes(iGenS,iDirS),
                  newShape,bGenS,subGenS,aDirS);
              }
          }
        }
      }
    }
    else if (myDirShapeTool.Type(aDirS)==TopAbs_EDGE){
      //Ici on construit les murs du Shape.
      TopAbs_ShapeEnum aGenSType = myGenShapeTool.Type(aGenS);
      TopoDS_Shape newWire,newShell;
      TopTools_SequenceOfShape WireSeq;
      Standard_Boolean sepwires = Standard_False;
      switch (aGenSType){
      case TopAbs_VERTEX : 
        myShapes(iGenS,iDirS)=MakeEmptyDirectingEdge(aGenS,aDirS);
        break;
      case TopAbs_EDGE :
        //On cree un wire intermediaire qui contient tous les edges
        //du montant (face) du Shape pour le cas standard, et une 
        //sequence de wires pour les cas merdiques necessitant des
        //wires independants.
        myBuilder.MakeWire(newWire);
        myShapes(iGenS,iDirS)=MakeEmptyFace(aGenS,aDirS);
        break;
      case TopAbs_WIRE :
        myBuilder.MakeShell(myShapes(iGenS,iDirS));
        break;
      case TopAbs_FACE :
        //On cree un shell intermediaire dans lequel on jette toutes 
        //les faces en direct, pour eviter les empilages compliques 
        //de shells et sous shells dans la structure du solide.
        myBuilder.MakeShell(newShell);
        myBuilder.MakeSolid(myShapes(iGenS,iDirS));
        break;
      case TopAbs_SHELL :
        myBuilder.MakeCompSolid(myShapes(iGenS,iDirS));
        break;
      case TopAbs_SOLID :
        throw Standard_NoSuchObject("Solids are not Processed");
        break;
      case TopAbs_COMPSOLID :
        throw Standard_NoSuchObject("Solids are not Processed");
        break;
      case TopAbs_COMPOUND :
        myBuilder.MakeCompound(myShapes(iGenS,iDirS));
        break;
      default:
        throw Standard_NoSuchObject("Unknown Shape");
        break;
      }
      bGenS = aGenS;
      myGenShapeTool.SetOrientation(bGenS,TopAbs_FORWARD);
      for (It.Init(bGenS);It.More();It.Next()){
        subGenS = It.Value();
        if(HasShape(subGenS,aDirS)){
          newShape = Shape(subGenS,aDirS);
          if (GGDShapeIsToAdd(myShapes(iGenS,iDirS),newShape,
            aGenS,subGenS,aDirS)){
              TopAbs_ShapeEnum subGenSType = myGenShapeTool.Type(subGenS);
              if (aGenSType==TopAbs_EDGE){   
                Or = It.Orientation();
                if (SeparatedWires(myShapes(iGenS,iDirS),newShape,
                  aGenS,subGenS,aDirS)){
                    sepwires = Standard_True;
                    TopoDS_Shape wi;
                    myBuilder.MakeWire(wi);
                    myBuilder.Add(wi,newShape,Or);
                    wi.Closed(BRep_Tool::IsClosed(wi));
                    WireSeq.Append(wi);
                }
                else{
                  myBuilder.Add(newWire,newShape,Or);
                }
                SetDirectingPCurve (myShapes(iGenS,iDirS),
                  newShape,bGenS,subGenS,aDirS,Or);
              }
              else if (aGenSType==TopAbs_WIRE){
                Or = It.Orientation();
                myBuilder.Add(myShapes(iGenS,iDirS),newShape,Or);
              }
              else if (aGenSType==TopAbs_FACE){
                Or = It.Orientation();
                if(subGenSType == TopAbs_WIRE) {
                  for (Lt.Init(newShape);Lt.More();Lt.Next()){
                    myBuilder.Add(newShell,Lt.Value(),
                      TopAbs::Compose(Lt.Orientation(),Or));
                  }
                }
                else if(subGenSType == TopAbs_EDGE) {
                  myBuilder.Add(newShell,newShape,Or);
                }
              }
              else if(aGenSType == TopAbs_SHELL){
                Or = TopAbs_FORWARD;
                myBuilder.Add(myShapes(iGenS,iDirS),newShape,Or);
              }
              else if(aGenSType == TopAbs_COMPOUND){
                Or = TopAbs_FORWARD;
                myBuilder.Add(myShapes(iGenS,iDirS),newShape,Or);
              }
              else{
                Or = It.Orientation();
                myBuilder.Add(myShapes(iGenS,iDirS),newShape,Or);
              }
          }
        }
      }
      bDirS = aDirS;
      for (Kt.Init(bDirS);Kt.More();Kt.Next()){
        subDirS = Kt.Value();
        if(HasShape(aGenS,subDirS)){
          newShape = Shape(aGenS,subDirS);
          if (GDDShapeIsToAdd(myShapes(iGenS,iDirS),newShape,
            aGenS,aDirS,subDirS)){
              if (aGenSType==TopAbs_EDGE){   
                Or = TopAbs::Reverse(Kt.Orientation());
                myBuilder.Add(newWire,newShape,Or);
                SetGeneratingPCurve
                  (myShapes(iGenS,iDirS),newShape,aGenS,aDirS,subDirS,Or);
              }
              else if(aGenSType==TopAbs_VERTEX){
                Or = Kt.Orientation();
                myBuilder.Add(myShapes(iGenS,iDirS),newShape,Or);
                SetDirectingParameter
                  (myShapes(iGenS,iDirS),newShape,aGenS,aDirS,subDirS);
              }
              else if(aGenSType==TopAbs_FACE){
                Or = Kt.Orientation();
                myBuilder.Add(newShell,newShape,Or);
              }
          }
        }
      }
      if (aGenSType==TopAbs_EDGE){
        if (sepwires){
          for(Standard_Integer ij = 1;ij <= WireSeq.Length();ij++){
            myBuilder.Add(myShapes(iGenS,iDirS),WireSeq.Value(ij));
          }
        }
        else{
          newWire.Closed(BRep_Tool::IsClosed(newWire));
          myBuilder.Add(myShapes(iGenS,iDirS),newWire);
        }
        myBuiltShapes(iGenS,iDirS) = Standard_True;
        SetContinuity(aGenS,aDirS);
      }
      if (aGenSType==TopAbs_WIRE){
        SetContinuity(aGenS,aDirS);
      }	
      if (aGenSType==TopAbs_FACE){
        newShell.Closed (BRep_Tool::IsClosed (newShell));
        TopoDS_Shape temp = SplitShell(newShell);
        TopAbs_Orientation ShellOri = DirectSolid(aGenS,aDirS);
        Lt.Init(temp);
        if(Lt.More()) Lt.Next();
        if(Lt.More()){
          for (Lt.Init(temp);Lt.More();Lt.Next()){
            myBuilder.Add(myShapes(iGenS,iDirS),Lt.Value(), ShellOri);
          }
        }
        else myBuilder.Add(myShapes(iGenS,iDirS),newShell, ShellOri);
      }
    }
    else if (myDirShapeTool.Type(aDirS)==TopAbs_WIRE){
      TopAbs_ShapeEnum aGenSType = myGenShapeTool.Type(aGenS);
      switch (aGenSType){
      case TopAbs_VERTEX : 
        myBuilder.MakeWire(myShapes(iGenS,iDirS));
        break;
      case TopAbs_EDGE :
        myBuilder.MakeShell(myShapes(iGenS,iDirS));
        break;
      case TopAbs_WIRE :
        myBuilder.MakeShell(myShapes(iGenS,iDirS));
        break;
      case TopAbs_FACE :
        myBuilder.MakeCompSolid(myShapes(iGenS,iDirS));
        break;
      case TopAbs_SHELL :
        myBuilder.MakeCompSolid(myShapes(iGenS,iDirS));
        break;
      case TopAbs_SOLID :
        throw Standard_NoSuchObject("Solids are not Processed");
        break;
      case TopAbs_COMPSOLID :
        throw Standard_NoSuchObject("Solids are not Processed");
        break;
      case TopAbs_COMPOUND :
        myBuilder.MakeCompound(myShapes(iGenS,iDirS));
        break;
      default:
        throw Standard_NoSuchObject("Unknown Shape");
        break;
      }
      bDirS = aDirS;
      for (Kt.Init(aDirS);Kt.More();Kt.Next()){
        subDirS = Kt.Value();
        if(HasShape(aGenS,subDirS)){
          Or = Kt.Orientation();
          newShape = Shape(aGenS,subDirS);
          myBuilder.Add(myShapes(iGenS,iDirS),newShape,Or);
        }
      }
    }
    myBuiltShapes(iGenS,iDirS) = Standard_True;
  }
  // Change the "Closed" flag only for Wires and Shells
  if (myShapes(iGenS, iDirS).ShapeType() == TopAbs_WIRE ||
    myShapes(iGenS, iDirS).ShapeType() == TopAbs_SHELL)
    myShapes(iGenS,iDirS).Closed (BRep_Tool::IsClosed (myShapes(iGenS,iDirS)));
  return myShapes(iGenS,iDirS);
}


//=======================================================================
//function : FirstShape
//purpose  : Returns the Shape indexed by the arguments.
//=======================================================================

TopoDS_Shape BRepSweep_NumLinearRegularSweep::FirstShape ()
{
  TopoDS_Shape result;
  if (myDirShapeTool.HasFirstVertex()){
    if(HasShape(myGenShape,myDirShapeTool.FirstVertex()))
      result = Shape(myGenShape,myDirShapeTool.FirstVertex());
  }
  return result;
}


//=======================================================================
//function : LastShape
//purpose  : Returns the Shape indexed by the arguments.
//=======================================================================

TopoDS_Shape BRepSweep_NumLinearRegularSweep::LastShape ()
{
  TopoDS_Shape result;
  if (myDirShapeTool.HasLastVertex()){
    if(HasShape(myGenShape,myDirShapeTool.LastVertex()))
      result = Shape(myGenShape,myDirShapeTool.LastVertex());
  }
  return result;
}


//=======================================================================
//function : FirstShape
//purpose  : Returns the Shape indexed by the arguments.
//=======================================================================

TopoDS_Shape BRepSweep_NumLinearRegularSweep::FirstShape (const TopoDS_Shape& aGenS)
{
  TopoDS_Shape result;
  if (myDirShapeTool.HasFirstVertex()){
    if(HasShape(aGenS,myDirShapeTool.FirstVertex()))
      result = Shape(aGenS,myDirShapeTool.FirstVertex());
  }
  return result;
}


//=======================================================================
//function : LastShape
//purpose  : Returns the Shape indexed by the arguments.
//=======================================================================

TopoDS_Shape BRepSweep_NumLinearRegularSweep::LastShape (const TopoDS_Shape& aGenS)
{
  TopoDS_Shape result;
  if (myDirShapeTool.HasLastVertex()){
    if(HasShape(aGenS,myDirShapeTool.LastVertex()))
      result = Shape(aGenS,myDirShapeTool.LastVertex());
  }
  return result;
}

//=======================================================================
//function : Closed
//purpose  : 
//=======================================================================

Standard_Boolean BRepSweep_NumLinearRegularSweep::Closed()const 
{
  return myDirWire.Closed();
}

//=======================================================================
//function : SplitShell
//purpose  : 
//=======================================================================

TopoDS_Shape BRepSweep_NumLinearRegularSweep::SplitShell(const TopoDS_Shape& aNewShape)const 
{
  TopoDS_Shape comp;
  myBuilder.MakeCompound(comp);
  myBuilder.Add(comp,aNewShape);
  return comp;
}

