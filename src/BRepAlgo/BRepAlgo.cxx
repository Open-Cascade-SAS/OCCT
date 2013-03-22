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

#include <TColGeom_SequenceOfCurve.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <GeomAbs_CurveType.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <ElCLib.hxx>
#include <BRepLib.hxx>
#include <TopoDS.hxx>
#include <ShapeFix_Shape.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>

//Temporary
#//include <DrawTrSurf.hxx>


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
      Handle(Geom_Curve) aCurve = BRep_Tool::Curve(edge, L, First, Last);
      Handle(Geom_TrimmedCurve) aTrCurve = new Geom_TrimmedCurve(aCurve, First, Last);
      tab(index) = GeomConvert::CurveToBSplineCurve(aTrCurve); //storage in a array 
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

//=======================================================================
//function : ConcatenateWireC0
//purpose  : 
//=======================================================================

TopoDS_Edge  BRepAlgo::ConcatenateWireC0(const TopoDS_Wire& aWire)
{
  Standard_Real LinTol = Precision::Confusion();
  Standard_Real AngTol = Precision::Angular();

  TopoDS_Edge ResEdge;

  TopoDS_Wire theWire = aWire;
  BRepLib::BuildCurves3d(theWire);
  Handle(ShapeFix_Shape) Fixer = new ShapeFix_Shape(theWire);
  Fixer->SetPrecision(LinTol);
  Fixer->SetMaxTolerance(LinTol);
  Fixer->Perform();
  theWire = TopoDS::Wire(Fixer->Shape());

  TColGeom_SequenceOfCurve CurveSeq;
  TopTools_SequenceOfShape LocSeq;
  TColStd_SequenceOfReal FparSeq;
  TColStd_SequenceOfReal LparSeq;
  TColStd_SequenceOfReal TolSeq;
  GeomAbs_CurveType CurType;
  TopoDS_Vertex FirstVertex, LastVertex;
  Standard_Boolean FinalReverse = Standard_False;

  BRepTools_WireExplorer wexp(theWire) ;
  for (; wexp.More(); wexp.Next())
    {
      TopoDS_Edge anEdge = wexp.Current();
      Standard_Real fpar, lpar;
      TopLoc_Location aLoc;
      Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aLoc, fpar, lpar);
      if (aCurve.IsNull())
	continue;

      BRepAdaptor_Curve BAcurve(anEdge);
      GeomAbs_CurveType aType = BAcurve.GetType();

      Handle(Geom_Curve) aBasisCurve = BAcurve.Curve().Curve();

      if (aBasisCurve->IsPeriodic())
	ElCLib::AdjustPeriodic(aBasisCurve->FirstParameter(), aBasisCurve->LastParameter(),
			       Precision::PConfusion(), fpar, lpar);

      if (CurveSeq.IsEmpty())
	{
	  CurveSeq.Append(aCurve);
	  TopoDS_Shape aLocShape;
	  aLocShape.Location(aLoc);
	  aLocShape.Orientation(wexp.Orientation());
	  LocSeq.Append(aLocShape);
	  FparSeq.Append(fpar);
	  LparSeq.Append(lpar);
	  CurType = aType;
	  FirstVertex = wexp.CurrentVertex();
          if (anEdge.Orientation() == TopAbs_REVERSED)
            FinalReverse = Standard_True;
	}
      else
	{
	  Standard_Boolean Done = Standard_False;
	  Standard_Real NewFpar, NewLpar;
	  GeomAdaptor_Curve GAprevcurve(CurveSeq.Last());
	  TopoDS_Vertex CurVertex = wexp.CurrentVertex();
	  TopoDS_Vertex CurFirstVer = TopExp::FirstVertex(anEdge);
	  TopAbs_Orientation ConnectByOrigin = (CurVertex.IsSame(CurFirstVer))? TopAbs_FORWARD : TopAbs_REVERSED;
	  if (aCurve == CurveSeq.Last())
	    {
	      NewFpar = fpar;
	      NewLpar = lpar;
	      if (aBasisCurve->IsPeriodic())
		{
		  if (NewLpar < NewFpar)
		    NewLpar += aBasisCurve->Period();
		  if (ConnectByOrigin == TopAbs_FORWARD)
		    ElCLib::AdjustPeriodic(FparSeq.Last(),
					   FparSeq.Last() + aBasisCurve->Period(),
					   Precision::PConfusion(), NewFpar, NewLpar);
		  else
		    ElCLib::AdjustPeriodic(FparSeq.Last() - aBasisCurve->Period(),
					   FparSeq.Last(),
					   Precision::PConfusion(), NewFpar, NewLpar);
		}
	      Done = Standard_True;
	    }
	  else if (aType == CurType &&
		   aType != GeomAbs_BezierCurve &&
		   aType != GeomAbs_BSplineCurve &&
		   aType != GeomAbs_OtherCurve)
	    switch (aType)
	      {
	      case GeomAbs_Line:
		{
		  gp_Lin aLine    = BAcurve.Line();
		  gp_Lin PrevLine = GAprevcurve.Line(); 
		  if (aLine.Contains(PrevLine.Location(), LinTol) &&
		      aLine.Direction().IsParallel(PrevLine.Direction(), AngTol))
		    {
		      gp_Pnt P1 = ElCLib::Value(fpar, aLine);
		      gp_Pnt P2 = ElCLib::Value(lpar, aLine);
		      NewFpar = ElCLib::Parameter(PrevLine, P1);
		      NewLpar = ElCLib::Parameter(PrevLine, P2);
		      if (NewLpar < NewFpar)
			{
			  Standard_Real MemNewFpar = NewFpar;
			  NewFpar = NewLpar;
			  NewLpar = MemNewFpar;
			  ConnectByOrigin = TopAbs::Reverse(ConnectByOrigin);
			}
		      Done = Standard_True;
		    }
		  break;
		}
	      case GeomAbs_Circle:
		{
		  gp_Circ aCircle    = BAcurve.Circle();
		  gp_Circ PrevCircle = GAprevcurve.Circle();
		  if (aCircle.Location().Distance(PrevCircle.Location()) <= LinTol &&
		      Abs(aCircle.Radius() - PrevCircle.Radius()) <= LinTol &&
		      aCircle.Axis().IsParallel(PrevCircle.Axis(), AngTol))
		    {
		      if (aCircle.Axis().Direction() * PrevCircle.Axis().Direction() < 0.)
			{
			  Standard_Real memfpar = fpar;
			  fpar = lpar;
			  lpar = memfpar;
			  ConnectByOrigin = TopAbs::Reverse(ConnectByOrigin);
			}
		      gp_Pnt P1 = ElCLib::Value(fpar, aCircle);
		      gp_Pnt P2 = ElCLib::Value(lpar, aCircle);
		      NewFpar = ElCLib::Parameter(PrevCircle, P1);
		      NewLpar = ElCLib::Parameter(PrevCircle, P2);
		      if (NewLpar < NewFpar)
			NewLpar += 2.*M_PI;
		      //Standard_Real MemNewFpar = NewFpar, MemNewLpar =  NewLpar;
		      if (ConnectByOrigin == TopAbs_FORWARD)
			ElCLib::AdjustPeriodic(FparSeq.Last(),
					       FparSeq.Last() + 2.*M_PI,
					       Precision::PConfusion(), NewFpar, NewLpar);
		      else
			ElCLib::AdjustPeriodic(FparSeq.Last() - 2.*M_PI,
					       FparSeq.Last(),
					       Precision::PConfusion(), NewFpar, NewLpar);
		      Done = Standard_True;
		    }
		  break;
		}
	      case GeomAbs_Ellipse:
		{
		  gp_Elips anEllipse   = BAcurve.Ellipse();
		  gp_Elips PrevEllipse = GAprevcurve.Ellipse();
		  if (anEllipse.Focus1().Distance(PrevEllipse.Focus1()) <= LinTol &&
		      anEllipse.Focus2().Distance(PrevEllipse.Focus2()) <= LinTol &&
		      Abs(anEllipse.MajorRadius() - PrevEllipse.MajorRadius()) <= LinTol &&
		      Abs(anEllipse.MinorRadius() - PrevEllipse.MinorRadius()) <= LinTol &&
		      anEllipse.Axis().IsParallel(PrevEllipse.Axis(), AngTol))
		    {
		      if (anEllipse.Axis().Direction() * PrevEllipse.Axis().Direction() < 0.)
			{
			  Standard_Real memfpar = fpar;
			  fpar = lpar;
			  lpar = memfpar;
			  ConnectByOrigin = TopAbs::Reverse(ConnectByOrigin);
			}
		      gp_Pnt P1 = ElCLib::Value(fpar, anEllipse);
		      gp_Pnt P2 = ElCLib::Value(lpar, anEllipse);
		      NewFpar = ElCLib::Parameter(PrevEllipse, P1);
		      NewLpar = ElCLib::Parameter(PrevEllipse, P2);
		      if (NewLpar < NewFpar)
			NewLpar += 2.*M_PI;
		      if (ConnectByOrigin == TopAbs_FORWARD)
			ElCLib::AdjustPeriodic(FparSeq.Last(),
					       FparSeq.Last() + 2.*M_PI,
					       Precision::PConfusion(), NewFpar, NewLpar);
		      else
			ElCLib::AdjustPeriodic(FparSeq.Last() - 2.*M_PI,
					       FparSeq.Last(),
					       Precision::PConfusion(), NewFpar, NewLpar);
		      Done = Standard_True;
		    }
		  break;
		}
	      case GeomAbs_Hyperbola:
		{
		  gp_Hypr aHypr    = BAcurve.Hyperbola();
		  gp_Hypr PrevHypr = GAprevcurve.Hyperbola();
		  if (aHypr.Focus1().Distance(PrevHypr.Focus1()) <= LinTol &&
		      aHypr.Focus2().Distance(PrevHypr.Focus2()) <= LinTol &&
		      Abs(aHypr.MajorRadius() - PrevHypr.MajorRadius()) <= LinTol &&
		      Abs(aHypr.MinorRadius() - PrevHypr.MinorRadius()) <= LinTol &&
		      aHypr.Axis().IsParallel(PrevHypr.Axis(), AngTol))
		    {
		      gp_Pnt P1 = ElCLib::Value(fpar, aHypr);
		      gp_Pnt P2 = ElCLib::Value(lpar, aHypr);
		      NewFpar = ElCLib::Parameter(PrevHypr, P1);
		      NewLpar = ElCLib::Parameter(PrevHypr, P2);
		      if (NewLpar < NewFpar)
			{
			  Standard_Real MemNewFpar = NewFpar;
			  NewFpar = NewLpar;
			  NewLpar = MemNewFpar;
			  ConnectByOrigin = TopAbs::Reverse(ConnectByOrigin);
			}
		      Done = Standard_True;
		    }
		  break;
		}
	      case GeomAbs_Parabola:
		{
		  gp_Parab aParab    = BAcurve.Parabola();
		  gp_Parab PrevParab = GAprevcurve.Parabola();
		  if (aParab.Location().Distance(PrevParab.Location()) <= LinTol &&
		      aParab.Focus().Distance(PrevParab.Focus()) <= LinTol &&
		      Abs(aParab.Focal() - PrevParab.Focal()) <= LinTol &&
		      aParab.Axis().IsParallel(PrevParab.Axis(), AngTol))
		    {
		      gp_Pnt P1 = ElCLib::Value(fpar, aParab);
		      gp_Pnt P2 = ElCLib::Value(lpar, aParab);
		      NewFpar = ElCLib::Parameter(PrevParab, P1);
		      NewLpar = ElCLib::Parameter(PrevParab, P2);
		      if (NewLpar < NewFpar)
			{
			  Standard_Real MemNewFpar = NewFpar;
			  NewFpar = NewLpar;
			  NewLpar = MemNewFpar;
			  ConnectByOrigin = TopAbs::Reverse(ConnectByOrigin);
			}
		      Done = Standard_True;
		    }
		  break;
		}
	      } //end of switch and else
	  if (Done)
	    {
	      if (NewFpar < FparSeq.Last())
		FparSeq(FparSeq.Length()) = NewFpar;
	      else
		LparSeq(LparSeq.Length()) = NewLpar;
	    }
	  else
	    {
	      CurveSeq.Append(aCurve);
	      TopoDS_Shape aLocShape;
	      aLocShape.Location(aLoc);
	      aLocShape.Orientation(wexp.Orientation());
	      LocSeq.Append(aLocShape);
	      FparSeq.Append(fpar);
	      LparSeq.Append(lpar);
	      TolSeq.Append(BRep_Tool::Tolerance(CurVertex));
	      CurType = aType;
	    }
	} //end of else (not first time)
    }
  LastVertex = wexp.CurrentVertex();
  TolSeq.Append(BRep_Tool::Tolerance(LastVertex));

  TopoDS_Vertex FirstVtx_final = (FinalReverse)? LastVertex : FirstVertex;
  FirstVtx_final.Orientation(TopAbs_FORWARD);
  TopoDS_Vertex LastVtx_final = (FinalReverse)? FirstVertex : LastVertex;
  LastVtx_final.Orientation(TopAbs_REVERSED);
  
  if (CurveSeq.IsEmpty())
    return ResEdge;

  Standard_Integer nb_curve = CurveSeq.Length();   //number of curves
  TColGeom_Array1OfBSplineCurve tab(0,nb_curve-1);                    //array of the curves
  TColStd_Array1OfReal tabtolvertex(0,nb_curve-1); //(0,nb_curve-2);  //array of the tolerances

  Standard_Integer i;

  if (nb_curve > 1)
    {
      for (i = 1; i <= nb_curve; i++)
	{
	  if (CurveSeq(i)->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
	    CurveSeq(i) = (*((Handle(Geom_TrimmedCurve)*)&(CurveSeq(i))))->BasisCurve();
	  
	  Handle(Geom_TrimmedCurve) aTrCurve = new Geom_TrimmedCurve(CurveSeq(i), FparSeq(i), LparSeq(i));
	  tab(i-1) = GeomConvert::CurveToBSplineCurve(aTrCurve);
	  tab(i-1)->Transform(LocSeq(i).Location().Transformation());
	  GeomConvert::C0BSplineToC1BSplineCurve(tab(i-1), Precision::Confusion());
	  if (LocSeq(i).Orientation() == TopAbs_REVERSED)
	    tab(i-1)->Reverse();
	  
	  //Temporary
	  //char* name = new char[100];
	  //sprintf(name, "c%d", i);
	  //DrawTrSurf::Set(name, tab(i-1));
	  
	  if (i > 1)
	    tabtolvertex(i-2) = TolSeq(i-1) * 5.;
	}
      tabtolvertex(nb_curve-1) = TolSeq(TolSeq.Length()) * 5.;
      
      Standard_Boolean closed_flag = Standard_False;
      Standard_Real closed_tolerance = 0.;
      if (FirstVertex.IsSame(LastVertex) &&
	  GeomLProp::Continuity(tab(0), tab(nb_curve-1),
				tab(0)->FirstParameter(),
				tab(nb_curve-1)->LastParameter(),
				Standard_False, Standard_False, LinTol, AngTol) >= GeomAbs_G1)
	{
	  closed_flag = Standard_True ;
	  closed_tolerance = BRep_Tool::Tolerance(FirstVertex);
	}
      
      Handle(TColGeom_HArray1OfBSplineCurve)  concatcurve;     //array of the concatenated curves
      Handle(TColStd_HArray1OfInteger)        ArrayOfIndices;  //array of the remining Vertex
      GeomConvert::ConcatC1(tab,
			    tabtolvertex,
			    ArrayOfIndices,
			    concatcurve,
			    closed_flag,
			    closed_tolerance);   //C1 concatenation

      if (concatcurve->Length() > 1)
	{
          Standard_Real MaxTolVer = LinTol;
          for (i = 1; i <= TolSeq.Length(); i++)
            if (TolSeq(i) > MaxTolVer)
              MaxTolVer = TolSeq(i);
          MaxTolVer *= 5.;
          
	  GeomConvert_CompCurveToBSplineCurve Concat(concatcurve->Value(concatcurve->Lower()));
	  
	  for (i = concatcurve->Lower()+1; i <= concatcurve->Upper(); i++)
	    Concat.Add( concatcurve->Value(i), MaxTolVer, Standard_True );
	  
	  concatcurve->SetValue(concatcurve->Lower(), Concat.BSplineCurve());
	}

      ResEdge = BRepLib_MakeEdge(concatcurve->Value(concatcurve->Lower()),
				 FirstVtx_final, LastVtx_final,
                                 concatcurve->Value(concatcurve->Lower())->FirstParameter(),
                                 concatcurve->Value(concatcurve->Lower())->LastParameter());
    }
  else
    {
      if (CurveSeq(1)->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
	CurveSeq(1) = (*((Handle(Geom_TrimmedCurve)*)&(CurveSeq(1))))->BasisCurve();
	  
      CurveSeq(1)->Transform(LocSeq(1).Location().Transformation());
      ResEdge = BRepLib_MakeEdge(CurveSeq(1),
				 FirstVtx_final, LastVtx_final,
				 FparSeq(1), LparSeq(1));
    }

  if (FinalReverse)
    ResEdge.Reverse();
  
  return ResEdge;
}
