// File:	BRepIntCurveSurface_Inter.cxx
// Created:	Mon Feb  7 15:52:37 1994
// Author:	Modelistation
//		<model@nonox>
//-Copyright:	 Matra Datavision 1994


#include <BRepIntCurveSurface_Inter.ixx>
#include <Geom_Line.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <BRepAdaptor_HSurface.hxx>

//modified by NIZNHY-PKV Sun Dec 15 16:57:10 2002 f
/*
static Standard_Real currentU,currentV;
TopAbs_State currentstate;
static Handle(BRepTopAdaptor_TopolTool) FastClass
       = new  BRepTopAdaptor_TopolTool();

static Handle(BRepAdaptor_HSurface) SurfForFastClass
       = new  BRepAdaptor_HSurface();
*/
//modified by NIZNHY-PKV Sun Dec 15 16:57:14 2002 t

//===========================================================================
//function :BRepIntCurveSurface_Inter::BRepIntCurveSurface_Inte
//purpose  : 
//===========================================================================
BRepIntCurveSurface_Inter::BRepIntCurveSurface_Inter()
     : empty(Standard_True) 
{
//modified by NIZNHY-PKV Sun Dec 15 16:58:10 2002 f
  FastClass = new  BRepTopAdaptor_TopolTool();
  SurfForFastClass = new  BRepAdaptor_HSurface();
//modified by NIZNHY-PKV Sun Dec 15 16:58:13 2002 t
}

void BRepIntCurveSurface_Inter::Init(const TopoDS_Shape& ashape,
				     const GeomAdaptor_Curve& acurve,
				     const Standard_Real tol) { 
  empty = Standard_False;
  curveisaline = Standard_False;
  curve = acurve;
  explorer.Init(ashape,TopAbs_FACE);
  currentnbpoints = 0;
  tolerance = tol;
  Find();
}

void BRepIntCurveSurface_Inter::Init(const TopoDS_Shape& ashape,
				     const gp_Lin&       aline,
				     const Standard_Real tol) { 
  empty = Standard_False;
  curveisaline = Standard_True;
  line = aline;
  Handle(Geom_Line) geomline = new Geom_Line(aline);
  curve.Load(geomline);
  explorer.Init(ashape,TopAbs_FACE);
  currentnbpoints = 0;
  tolerance = tol;
  Find();
}

Standard_Boolean BRepIntCurveSurface_Inter::More() const {
  return(explorer.More());
}

void BRepIntCurveSurface_Inter::Next() { 
  Find();
}

void BRepIntCurveSurface_Inter::Find() { 

  static Standard_Real UMin = 0.0;
  static Standard_Real UMax = 0.0;
  static Standard_Real VMin = 0.0;
  static Standard_Real VMax = 0.0;
  static Standard_Real PeriodU = 0.0;
  static Standard_Real PeriodV = 0.0;
  
  if(currentnbpoints) { 
    while(currentindex < currentnbpoints) { 
      currentindex++;
      Standard_Real U = intcs.Point(currentindex).U();
      Standard_Real V = intcs.Point(currentindex).V();
      //-------------------------------------------------------
      //-- On Cherche a recadrer le point U,V ds la face UV
      //-- 
      if(PeriodU) { 
	while(U>UMin) 
	  U-=PeriodU;
      }
      if(PeriodV) { 
	while(V>VMin)
	  V-=PeriodV;
      }
//    Standard_Real UInit = U;
      Standard_Real VInit = V;
      do { //-- Boucle Sur U  
	V = VInit;
	do { //-- Boucle sur V
	  gp_Pnt2d Puv(U,V);
	  //--- 
	  //-- classifier.Perform(TopoDS::Face(explorer.Current()),Puv,tolerance);
	  currentstate = FastClass->Classify(Puv,tolerance); //-- MODIF
	  //-- TopAbs_State currentstate = classifier.State();

	  if(currentstate == TopAbs_ON || currentstate == TopAbs_IN) { 
	    currentU = U;
	    currentV = V;	
	    return; 	
	  }
	  V+=PeriodV;	    
	}
	while(PeriodV && V< VMax);
	U+=PeriodU;
      }
      while(PeriodU && U<UMax);
    }
    explorer.Next();
  }
  
  if(explorer.More()) { 
    //---------------------------------------------
    BRepAdaptor_Surface brepadaptsurf;
    
    TopoDS_Face face=TopoDS::Face(explorer.Current());
    face.Orientation(TopAbs_FORWARD);
    
    brepadaptsurf.Initialize(face,Standard_True);
    //----------------------------------------------
    //-- Mise a jour des variables PeriodU,PeriodV
    //--

    SurfForFastClass->ChangeSurface().Initialize(face); //-- MODIF
//    SurfForFastClass->ChangeSurface().Initialize(TopoDS::Face(face)); //-- MODIF
    FastClass->Initialize(SurfForFastClass); //-- MODIF
 
    if( brepadaptsurf.Surface().IsUPeriodic()) { 
      PeriodU = brepadaptsurf.Surface().UPeriod();
      UMin = brepadaptsurf.Surface().FirstUParameter();
      UMax = brepadaptsurf.Surface().LastUParameter();
    }
    else { 
      PeriodU = 0.0;
    }
    if( brepadaptsurf.Surface().IsVPeriodic()) { 
      PeriodV = brepadaptsurf.Surface().VPeriod();
      VMin = brepadaptsurf.Surface().FirstVParameter();
      VMax = brepadaptsurf.Surface().LastVParameter();
    }
    else { 
      PeriodV = 0.0;
    }

    //----------------------------------------------
    Handle(GeomAdaptor_HCurve)   HC = new GeomAdaptor_HCurve(curve);
    Handle(BRepAdaptor_HSurface) HS = new BRepAdaptor_HSurface(brepadaptsurf);
    //----------------------------------------------
    //-- intcs.Perform(curve,brepadaptsurf);
    intcs.Perform(HC,HS);

    currentindex = 0;
    currentnbpoints = intcs.NbPoints();
    if(currentnbpoints) { 
      Find();
    }
    else { 
      explorer.Next();
      Find(); 
    }
  }
  else { 
    currentnbpoints=0; 
  }
}


IntCurveSurface_IntersectionPoint BRepIntCurveSurface_Inter::Point() const {
  if(currentindex==0) 
    StdFail_NotDone::Raise();
  const IntCurveSurface_IntersectionPoint& ICPS = intcs.Point(currentindex);
  return(IntCurveSurface_IntersectionPoint(ICPS.Pnt(),
					   currentU,     // ICPS.U(),
					   currentV,     // ICPS.V(),
					   ICPS.W(),
					   ICPS.Transition()));
  //-- return(intcs.Point(currentindex));
}

Standard_Real BRepIntCurveSurface_Inter::U() const {
  if(currentindex==0) 
    StdFail_NotDone::Raise();
  //-- return(intcs.Point(currentindex).U());
  return(currentU);
}

Standard_Real BRepIntCurveSurface_Inter::V() const {
  if(currentindex==0) 
    StdFail_NotDone::Raise();
  //-- return(intcs.Point(currentindex).V());
  return(currentV);
}

Standard_Real BRepIntCurveSurface_Inter::W() const {
  if(currentindex==0) 
    StdFail_NotDone::Raise();
  return(intcs.Point(currentindex).W());
}

TopAbs_State BRepIntCurveSurface_Inter::State() const {
  if(currentindex==0) 
    StdFail_NotDone::Raise();
  //-- return(classifier.State());
  return(currentstate);
}

IntCurveSurface_TransitionOnCurve BRepIntCurveSurface_Inter::Transition() const {
  if(currentindex==0) 
    StdFail_NotDone::Raise();
  return(intcs.Point(currentindex).Transition());
}

const TopoDS_Face& BRepIntCurveSurface_Inter::Face() const { 
  return(TopoDS::Face(explorer.Current()));
}

const gp_Pnt& BRepIntCurveSurface_Inter::Pnt() const { 
  if(currentindex==0) 
    StdFail_NotDone::Raise();
  return(intcs.Point(currentindex).Pnt());
}
