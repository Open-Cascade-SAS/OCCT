// Created on: 1997-03-10
// Created by: Stagiaire Francois DUMONT
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



#include <BRepAlgo.ixx>
#include <BRepTools_WireExplorer.hxx>
#include <BRep_Tool.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeWire.hxx>
#include <gp_Pnt.hxx>
#include <GeomConvert.hxx>
#include <GeomLProp.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <TColGeom_Array1OfBSplineCurve.hxx>
#include <TColGeom_HArray1OfBSplineCurve.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopLoc_Location.hxx>
#include <TopExp.hxx>

//=======================================================================
//function : ConcatenateWire
//purpose  : 
//=======================================================================

TopoDS_Wire  BRepAlgo::ConcatenateWire(const TopoDS_Wire& W,
				       const GeomAbs_Shape Option,
				       const Standard_Real TolAngular) 
{


  Standard_Integer        nb_curve,                         //number of curves in the Wire
                          index;
  BRepTools_WireExplorer  WExp(W) ;
  TopoDS_Edge             edge;
  TopLoc_Location         L ;
  Standard_Real           First=0.,Last=0.,                       //extremal values for the curve
                          First0 =0.,
                          toler =0.,
                          tolleft,tolright;                 //Vertex tolerances
  TopoDS_Vertex           Vfirst,Vlast;                     //Vertex of the Wire
  gp_Pnt                  Pfirst,Plast;            //, Pint;  corresponding points

  BRepLib_MakeWire        MakeResult;                       
  Standard_Real           closed_tolerance =0.0;
  Standard_Boolean        closed_flag = Standard_False ;
  
  nb_curve = 0;
  
  while ( WExp.More()){                                     //computation of the curve number
    nb_curve++ ;
    WExp.Next();
  }
  
  if (nb_curve > 1) {
    TColGeom_Array1OfBSplineCurve tab(0,nb_curve-1);          //array of the wire's curve
    TColStd_Array1OfReal tabtolvertex(0,nb_curve-2);          //array of the tolerance's vertex
    
    WExp.Init(W);
    
    for (index=0 ;index<nb_curve; index++){                   //main loop
      edge = WExp.Current() ;
      tab(index)  = GeomConvert::CurveToBSplineCurve(new      //storage in a array 
						     Geom_TrimmedCurve(BRep_Tool::Curve(edge,L,First,Last),First,Last));
      tab(index)->Transform(L.Transformation());
      GeomConvert::C0BSplineToC1BSplineCurve(tab(index),Precision::Confusion());
      
      if (index >= 1){                                         //continuity test loop
	if (edge.Orientation()==TopAbs_REVERSED)
	  tab(index)->Reverse();
	tolleft=BRep_Tool::Tolerance(TopExp::LastVertex(edge));
	tolright=BRep_Tool::Tolerance(TopExp::FirstVertex(edge));
	tabtolvertex(index-1)=Max(tolleft,tolright);
      }
      
      if(index==0){                                           //storage of the first edge features
	First0=First;
	if(edge.Orientation()==TopAbs_REVERSED){             //(usefull for the closed wire) 
	  Vfirst=TopExp::LastVertex(edge);
	  tab(index)->Reverse();
	}
	else
	  Vfirst=TopExp::FirstVertex(edge);
      }
      
      if(index==nb_curve-1){                                  //storage of the last edge features
	if(edge.Orientation()==TopAbs_REVERSED)
	  Vlast=TopExp::FirstVertex(edge);
	else
	  Vlast=TopExp::LastVertex(edge);
      }
      WExp.Next() ; 
    }
  
    if (BRep_Tool::Tolerance(Vfirst)>BRep_Tool::Tolerance(Vlast)) //computation of the closing tolerance
      toler=BRep_Tool::Tolerance(Vfirst);
    else
      toler=BRep_Tool::Tolerance(Vlast);
    
    Pfirst=BRep_Tool::Pnt(Vfirst);
    Plast=BRep_Tool::Pnt(Vlast); 
    
    if ((Pfirst.Distance(Plast)<=toler)&&                   //C0 continuity test at the closing point
	(GeomLProp::Continuity(tab(nb_curve-1),tab(0),Last,First0,
			       Standard_True,Standard_True, 
			       toler, TolAngular)>=GeomAbs_G1)) 
      {
	closed_tolerance =toler;                                        //if ClosedG1!=0 it will be True and
	closed_flag = Standard_True ;
      }                                                        //with the toler value
    Handle(TColGeom_HArray1OfBSplineCurve)  concatcurve;     //array of the concatenated curves
    Handle(TColStd_HArray1OfInteger)        ArrayOfIndices;  //array of the remining Vertex
    if (Option==GeomAbs_G1)
      GeomConvert::ConcatG1(tab,
			    tabtolvertex,
			    concatcurve,
			    closed_flag,
			    closed_tolerance) ;    //G1 concatenation
    else
      GeomConvert::ConcatC1(tab,
			    tabtolvertex,
			    ArrayOfIndices,
			    concatcurve,
			    closed_flag,
			    closed_tolerance);   //C1 concatenation
    
    for (index=0;index<=(concatcurve->Length()-1);index++){    //building of the resulting Wire
      BRepLib_MakeEdge EdgeBuilder(concatcurve->Value(index));
      edge = EdgeBuilder.Edge();
      MakeResult.Add(edge);
    } 

  }
  else {
    TColGeom_Array1OfBSplineCurve tab(0,0);          //array of the wire's curve
    TColStd_Array1OfReal tabtolvertex(0,0);          //array of the tolerance's vertex 
    WExp.Init(W);
    
    edge = WExp.Current() ;
    tab(0)  = GeomConvert::CurveToBSplineCurve(new      //storage in a array 
						     Geom_TrimmedCurve(BRep_Tool::Curve(edge,L,First,Last),First,Last));
    tab(0)->Transform(L.Transformation());
    GeomConvert::C0BSplineToC1BSplineCurve(tab(0),Precision::Confusion());
    if (edge.Orientation()==TopAbs_REVERSED)
	  tab(0)->Reverse();
    tolleft=BRep_Tool::Tolerance(TopExp::LastVertex(edge));
    tolright=BRep_Tool::Tolerance(TopExp::FirstVertex(edge));
    tabtolvertex(0)=Max(tolleft,tolright);
    if(edge.Orientation()==TopAbs_REVERSED){             //(usefull for the closed wire) 
	  Vfirst=TopExp::LastVertex(edge);
	  Vlast=TopExp::FirstVertex(edge);
    }
    else {
      
      Vfirst=TopExp::FirstVertex(edge);
      Vlast = TopExp::LastVertex(edge) ;
    }
    Pfirst=BRep_Tool::Pnt(Vfirst);
    Plast=BRep_Tool::Pnt(Vlast); 
    if ((Pfirst.Distance(Plast)<=toler)&&                   //C0 continuity test at the closing point
	(GeomLProp::Continuity(tab(0),tab(0),Last,First,
			       Standard_True,Standard_True,
			       toler, TolAngular)>=GeomAbs_G1)) 
      {
	closed_tolerance =toler;                                        //if ClosedG1!=0 it will be True and
	closed_flag = Standard_True ;
      }                                                        //with the toler value
    Handle(TColGeom_HArray1OfBSplineCurve)  concatcurve;     //array of the concatenated curves
    Handle(TColStd_HArray1OfInteger)        ArrayOfIndices;  //array of the remining Vertex
    if (Option==GeomAbs_G1)
      GeomConvert::ConcatG1(tab,
			    tabtolvertex,
			    concatcurve,
			    closed_flag,
			    closed_tolerance) ;    //G1 concatenation
    else
      GeomConvert::ConcatC1(tab,
			    tabtolvertex,
			    ArrayOfIndices,
			    concatcurve,
			    closed_flag,
			    closed_tolerance);   //C1 concatenation
    
    for (index=0;index<=(concatcurve->Length()-1);index++){    //building of the resulting Wire
      BRepLib_MakeEdge EdgeBuilder(concatcurve->Value(index));
      edge = EdgeBuilder.Edge();
      MakeResult.Add(edge);
    } 
  }
  return MakeResult.Wire() ;  
  
}




