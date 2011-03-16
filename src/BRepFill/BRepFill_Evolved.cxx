// File:	BRepFill_Evolved.cxx
// Created:	Mon Oct  3 14:36:06 1994
// Author:	Bruno DUMORTIER
//		<dub@fuegox>

#include <BRepFill_Evolved.ixx>


#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRepFill_OffsetWire.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRep_Builder.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepLib.hxx>
#include <BRepMAT2d_Explorer.hxx>
#include <BRepFill_Pipe.hxx>
#include <BRepFill_OffsetAncestors.hxx>
#include <BRepAlgo_FaceRestrictor.hxx>
#include <BRepLib_FindSurface.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepLib_MakeWire.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepAlgo_Loop.hxx>
#include <BRepSweep_Revol.hxx>
#include <BRepSweep_Prism.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRepTools_TrsfModification.hxx>
#include <BRepTools_Modifier.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepLProp.hxx>

#include <BRepMAT2d_LinkTopoBilo.hxx>
#include <BRepMAT2d_BisectingLocus.hxx>
#include <BRepMAT2d_Explorer.hxx>

#include <GeomAPI.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_Geometry.hxx>
#include <GeomProjLib.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom2dAdaptor_Curve.hxx>

#include <Geom2dAPI_ExtremaCurveCurve.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <Geom2dInt_GInter.hxx>

#include <MAT2d_CutCurve.hxx>

#include <MAT_Graph.hxx>
#include <MAT_BasicElt.hxx>
#include <MAT_Side.hxx>
#include <MAT_Arc.hxx>
#include <MAT_Node.hxx>
#include <Bisector_Bisec.hxx>
#include <Bisector_BisecAna.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx> 
#include <TopTools_DataMapOfShapeSequenceOfShape.hxx>
#include <TopLoc_Location.hxx>
#include <TopAbs.hxx>

#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Circ2d.hxx>

#include <TColgp_SequenceOfPnt.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <BRepFill_TrimSurfaceTool.hxx>
#include <BRepFill_DataMapOfNodeDataMapOfShapeShape.hxx>
#include <BRepFill_DataMapOfShapeDataMapOfShapeListOfShape.hxx>
#include <BRepFill_DataMapOfShapeSequenceOfReal.hxx>       
#include <BRepFill_DataMapOfShapeSequenceOfPnt.hxx>       
#include <BRepFill_DataMapIteratorOfDataMapOfShapeDataMapOfShapeListOfShape.hxx>
#include <Precision.hxx>

#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>

#ifdef DRAW
#include <DBRep.hxx>
#include <DrawTrSurf.hxx>
#include <stdio.h>
#endif

#ifdef DEB
static Standard_Boolean AffichGeom = Standard_False;
static Standard_Boolean AffichEdge = Standard_False;
static Standard_Integer NbFACES       = 0;
static Standard_Integer NbTRIMFACES   = 0;
static Standard_Integer NbVEVOS       = 0;
static Standard_Integer NbPROFILS     = 0;
static Standard_Integer NbEDGES       = 0;
// POP pour NT
#ifndef WNT
static char name[100];
#endif
#endif

static const Standard_Real BRepFill_Confusion() 
{
  Standard_Real Tol = 1.e-6;
  return Tol;
}

static const TopoDS_Wire PutProfilAt (const TopoDS_Wire&     ProfRef,
				      const gp_Ax3&          AxeRef,
				      const TopoDS_Edge&     E,
				      const TopoDS_Face&     F,
				      const Standard_Boolean AtStart);

static void TrimFace(const TopoDS_Face&              Face,
		           TopTools_SequenceOfShape& TheEdges,
		           TopTools_SequenceOfShape& S);

static void TrimEdge (const TopoDS_Edge&              Edge,
		      const TopTools_SequenceOfShape& TheEdgesControle,
		            TopTools_SequenceOfShape& TheVer,
		            TColStd_SequenceOfReal&   ThePar,
		            TopTools_SequenceOfShape& S);

static TopAbs_Orientation OriEdgeInFace (const TopoDS_Edge&     E,
					 const TopoDS_Face&     F);

static Standard_Integer PosOnFace (Standard_Real d1,
				   Standard_Real d2,
				   Standard_Real d3);

static void ComputeIntervals (const TopTools_SequenceOfShape& VonF,
			      const TopTools_SequenceOfShape& VOnL,
			      const TColgp_SequenceOfPnt&     ParOnF,
			      const TColgp_SequenceOfPnt&     ParOnL,
			      const BRepFill_TrimSurfaceTool& Trim,
			      const Handle(Geom2d_Curve)&     Bis,
			      const TopoDS_Vertex&            VS,
			      const TopoDS_Vertex&            VE,
			            TColStd_SequenceOfReal&   FirstPar,
			            TColStd_SequenceOfReal&   LastPar,
			            TopTools_SequenceOfShape& FirstV,
			            TopTools_SequenceOfShape& LastV );	

static Standard_Real    DistanceToOZ (const TopoDS_Vertex& V);

static Standard_Real    Altitud (const TopoDS_Vertex& V);

static Standard_Boolean DoubleOrNotInFace (const TopTools_SequenceOfShape& EC,
					   const TopoDS_Vertex&            V);

static void SimpleExpression (const Bisector_Bisec&        B, 
			            Handle(Geom2d_Curve)&  Bis);

static TopAbs_Orientation Relative (const TopoDS_Wire&   W1,
				    const TopoDS_Wire&   W2,
				    const TopoDS_Vertex& V,
				    Standard_Boolean&    Commun);

static void CutEdge (const TopoDS_Edge& E, 
		     const TopoDS_Face& F,TopTools_ListOfShape& Cuts);

static void CutEdgeProf (const TopoDS_Edge&                  E,
			 const Handle(Geom_Plane)&           Plane,
			 const Handle(Geom2d_Line)&          Line,
			       TopTools_ListOfShape&         Cuts,
			       TopTools_DataMapOfShapeShape& MapVerRefMoved);

static Standard_Integer VertexFromNode 
(const Handle(MAT_Node)&                          aNode, 
 const TopoDS_Edge&                               E, 
 const TopoDS_Vertex&                             VF, 
 const TopoDS_Vertex&                             VL,
       BRepFill_DataMapOfNodeDataMapOfShapeShape& MapNodeVertex,
       TopoDS_Vertex&                             VS);

//=======================================================================
//function : EdgeVertices
//purpose  : 
//=======================================================================

static void EdgeVertices (const TopoDS_Edge&   E,
			        TopoDS_Vertex& V1, 
			        TopoDS_Vertex& V2)
{
  if (E.Orientation() == TopAbs_REVERSED) {
    TopExp::Vertices(E,V2,V1);
  }
  else {
    TopExp::Vertices(E,V1,V2);
  }
}
				      
//=======================================================================
//function : BRepFill_Evolved
//purpose  : 
//=======================================================================

BRepFill_Evolved::BRepFill_Evolved() 
:
myIsDone   (Standard_False),
mySpineType(Standard_True)
{
}


//=======================================================================
//function : BRepFill_Evolved
//purpose  : 
//=======================================================================

BRepFill_Evolved::BRepFill_Evolved(const TopoDS_Wire&      Spine, 
				   const TopoDS_Wire&      Profile,
				   const gp_Ax3&           AxeProf,
				   const GeomAbs_JoinType  Join,
				   const Standard_Boolean  Solid)
 
: myIsDone(Standard_False)
{
  Perform( Spine, Profile, AxeProf, Join, Solid);
}


//=======================================================================
//function : BRepFill_Evolved
//purpose  : 
//=======================================================================

BRepFill_Evolved::BRepFill_Evolved(const TopoDS_Face&     Spine, 
				   const TopoDS_Wire&     Profile,
				   const gp_Ax3&          AxeProf,
				   const GeomAbs_JoinType Join,
				   const Standard_Boolean Solid)
: myIsDone(Standard_False)
{
  Perform( Spine, Profile, AxeProf, Join, Solid);
}

//=======================================================================
//function : IsVertical
//purpose  : 
//=======================================================================

static Standard_Boolean IsVertical(const TopoDS_Edge& E) 
{
  TopoDS_Vertex V1,V2;
  TopExp::Vertices(E,V1,V2);
  gp_Pnt P1 = BRep_Tool::Pnt(V1);
  gp_Pnt P2 = BRep_Tool::Pnt(V2);
  
  if ( Abs(P1.Y() - P2.Y()) < BRepFill_Confusion()) {
    // It is a Line ?
    TopLoc_Location Loc;
    Standard_Real f,l;
    Handle(Geom_Curve) GC = BRep_Tool::Curve(E,Loc,f,l);
    if ( GC->DynamicType() == STANDARD_TYPE(Geom_Line))
      return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : IsPlanar
//purpose  : 
//=======================================================================

static Standard_Boolean IsPlanar(const TopoDS_Edge& E) 
{
  TopoDS_Vertex V1,V2;
  TopExp::Vertices(E,V1,V2);
  gp_Pnt P1 = BRep_Tool::Pnt(V1);
  gp_Pnt P2 = BRep_Tool::Pnt(V2);
  
  if ( Abs(P1.Z() - P2.Z()) < BRepFill_Confusion()) {
    // It is a Line ?
    TopLoc_Location Loc;
    Standard_Real f,l;
    Handle(Geom_Curve) GC = BRep_Tool::Curve(E,Loc,f,l);
    if ( GC->DynamicType() == STANDARD_TYPE(Geom_Line))
      return Standard_True;
  }
  return Standard_False;

}

//=======================================================================
//function : Side
//purpose  : determine la position du profil par rapport au plan XOZ.
//           Return 1 : MAT_Left.
//           Return 2 : MAT_Left and Planar.
//           Return 3 : MAT_Left and Vertical.
//           Return 4 : MAT_Right.
//           Return 5 : MAT_Right and Planar.
//           Return 6 : MAT_Right and Vertical.
//=======================================================================

static Standard_Integer Side(const TopoDS_Wire&  Profil,
			     const Standard_Real Tol)
{
#ifdef DEB
  Standard_Boolean OnLeft  = Standard_False;
  Standard_Boolean OnRight = Standard_False;
#endif
  TopoDS_Vertex    V1,V2;
  // Rem : il suffit de tester sur le premier edge du Wire.
  //       ( Correctement decoupe dans PrepareProfil)
  TopExp_Explorer Explo(Profil,TopAbs_EDGE);

  Standard_Integer TheSide;
  const TopoDS_Edge& E = TopoDS::Edge(Explo.Current());

  TopExp::Vertices(E,V1,V2);
  gp_Pnt P1 = BRep_Tool::Pnt(V1);
  gp_Pnt P2 = BRep_Tool::Pnt(V2);
  
  if ( P1.Y() < -Tol || P2.Y() < -Tol)  TheSide = 4;
  else                                  TheSide = 1;
  if      (IsVertical(E)) TheSide+=2;
  else if (IsPlanar(E))   TheSide++;
  return TheSide;
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepFill_Evolved::Perform(const TopoDS_Wire&      Spine, 
			       const TopoDS_Wire&      Profile,
			       const gp_Ax3&           AxeProf,
			       const GeomAbs_JoinType  Join,
			       const Standard_Boolean  Solid)
{
  mySpineType = Standard_False;
  TopoDS_Face aFace = BRepLib_MakeFace(Spine,Standard_True);
  PrivatePerform( aFace, Profile, AxeProf, Join, Solid);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepFill_Evolved::Perform(const TopoDS_Face&      Spine, 
			       const TopoDS_Wire&      Profile,
			       const gp_Ax3&           AxeProf,
			       const GeomAbs_JoinType  Join,
			       const Standard_Boolean  Solid)
{
  mySpineType = Standard_True;
  PrivatePerform( Spine, Profile, AxeProf, Join, Solid);
}

//=======================================================================
//function : PrivatePerform
//purpose  : 
//=======================================================================

void BRepFill_Evolved::PrivatePerform(const TopoDS_Face&     Spine, 
				      const TopoDS_Wire&     Profile,
				      const gp_Ax3&          AxeProf,
				      const GeomAbs_JoinType Join,
				      const Standard_Boolean Solid)
{
  TopoDS_Shape aLocalShape = Spine.Oriented(TopAbs_FORWARD);
  mySpine    = TopoDS::Face(aLocalShape);
//  mySpine    = TopoDS::Face(Spine.Oriented(TopAbs_FORWARD));
  aLocalShape = Profile.Oriented(TopAbs_FORWARD);
  myProfile  = TopoDS::Wire(aLocalShape);
//  myProfile  = TopoDS::Wire(Profile.Oriented(TopAbs_FORWARD));
  myJoinType = Join;
  myMap.Clear();
  
  if (myJoinType > GeomAbs_Arc)  {
    Standard_NotImplemented::Raise();
  }

  TopTools_ListOfShape               WorkProf;
  TopoDS_Face                        WorkSpine;
  TopTools_ListIteratorOfListOfShape WPIte;

  //-------------------------------------------------------------------
  // Positionnement de mySpine et de myProfil dans le repere de travail.
  //-------------------------------------------------------------------
  TopLoc_Location LSpine   = FindLocation(mySpine);
  gp_Trsf T;
  T.SetTransformation(AxeProf);
  TopLoc_Location LProfile  (T);
  TopLoc_Location InitLS = mySpine  .Location();
  TopLoc_Location InitLP = myProfile.Location();
  TransformInitWork(LSpine,LProfile);

  //------------------------------------------------------------------
  // projection du profil et decoupe du spine.
  //------------------------------------------------------------------
  TopTools_DataMapOfShapeShape MapProf, MapSpine;

  PrepareProfile(WorkProf , MapProf);  
  PrepareSpine  (WorkSpine, MapSpine);

  Standard_Real    Tol       = BRepFill_Confusion();
  Standard_Boolean YaLeft    = Standard_False;
  Standard_Boolean YaRight   = Standard_False;  
  TopoDS_Wire      SP;

  for (WPIte.Initialize(WorkProf); WPIte.More(); WPIte.Next()) {
    SP = TopoDS::Wire(WPIte.Value());
    if ( Side(SP,Tol) < 4) YaLeft  = Standard_True;
    else                   YaRight = Standard_True;
    if (YaLeft && YaRight) break;
  }

  TopoDS_Face              Face;
  BRepMAT2d_BisectingLocus Locus;

  //----------------------------------------------------------
  // Initialisation du volevo decoupe.
  // Pour chaque portion du profile on cree en volevo qui est
  // additionner a CutVevo
  //----------------------------------------------------------
  BRepFill_Evolved       CutVevo;
  TopoDS_Wire            WP;
  BRep_Builder           BB;
  BRepTools_WireExplorer WExp;

  BB.MakeWire(WP);

  for (WPIte.Initialize(WorkProf); WPIte.More(); WPIte.Next()) {     
    for (WExp.Init(TopoDS::Wire(WPIte.Value())); WExp.More(); WExp.Next()) {
      BB.Add(WP,WExp.Current());
    }
  }
  CutVevo.SetWork(WorkSpine,WP);

  BRepTools_Quilt  Glue;
  Standard_Integer CSide;
  
  //---------------------------------
  // Construction des vevos a gauche.
  //---------------------------------
  if (YaLeft) {
    //-----------------------------------------------------
    // Calcul de la carte des lieux bissecteurs a gauche.  
    // et des Liens Topologie -> elements de base de la carte.
    //-----------------------------------------------------
    BRepMAT2d_Explorer Exp(WorkSpine);
    Locus.Compute(Exp,1,MAT_Left);
    BRepMAT2d_LinkTopoBilo Link(Exp,Locus);

    for (WPIte.Initialize(WorkProf); WPIte.More(); WPIte.Next()) {    
      SP    = TopoDS::Wire(WPIte.Value());
      CSide = Side(SP,Tol);	
      //-----------------------------------------------
      // Construction et ajout d un volevo elementaire.
      //-----------------------------------------------
      BRepFill_Evolved Vevo;
      if ( CSide == 1) { 
        Vevo.ElementaryPerform (WorkSpine, SP, Locus, Link, Join);
      }
      else if (CSide == 2) {
        Vevo.PlanarPerform (WorkSpine, SP, Locus, Link, Join);
      }
      else if (CSide == 3) {
        Vevo.VerticalPerform (WorkSpine, SP, Locus, Link, Join);
      }
      CutVevo.Add (Vevo, SP, Glue);
    }
  }

  //---------------------------------
  // Construction des vevos a droite.
  //---------------------------------
  if (YaRight) {
    //-----------------------------------
    // Decomposition de la face en wires.
    //-----------------------------------
    TopExp_Explorer SpineExp (WorkSpine, TopAbs_WIRE);
    for ( ; SpineExp.More(); SpineExp.Next()) {
      //----------------------------------------------
      // Calcul de la carte a droite du wire courant.
      //----------------------------------------------
      BRepLib_MakeFace B(gp_Pln(0.,0.,1.,0.));
      TopoDS_Shape aLocalShape = SpineExp.Current().Reversed();
      B.Add(TopoDS::Wire(aLocalShape));
//      B.Add(TopoDS::Wire(SpineExp.Current().Reversed()));
      Face = B.Face();
      BRepMAT2d_Explorer Exp(Face);
      Locus.Compute(Exp,1,MAT_Left);
      BRepMAT2d_LinkTopoBilo Link(Exp,Locus);
      
      for (WPIte.Initialize(WorkProf); WPIte.More(); WPIte.Next()) {
	SP = TopoDS::Wire(WPIte.Value());
	CSide = Side(SP,Tol);	
	//-----------------------------------------------
	// Construction et ajout d un volevo elementaire.
	//-----------------------------------------------
	BRepFill_Evolved Vevo;
	if ( CSide == 4) { 
	  Vevo.ElementaryPerform (Face, SP, Locus, Link, Join);
	}
	else if (CSide == 5) {
	  Vevo.PlanarPerform (Face, SP, Locus, Link, Join);
	}
	else if (CSide == 6) {
	  Vevo.VerticalPerform (Face, SP, Locus, Link, Join);
	}
	CutVevo.Add (Vevo, SP, Glue);
      }      
    }
  }

  if (Solid) CutVevo.AddTopAndBottom(Glue);

  //-------------------------------------------------------------------------
  // Codage des regularites sur les edges paralleles generes par les vertex
  // de la decoupe du profil.
  //-------------------------------------------------------------------------
  CutVevo.ContinuityOnOffsetEdge(WorkProf);

  //-----------------------------------------------------------------
  // construction du shape via le quilt, ie:
  // - partage des topologies des volevos elementaires additionnes.
  // - Orientation des faces les unes par rapport aux autres.
  //-----------------------------------------------------------------
  TopoDS_Shape& SCV = CutVevo.ChangeShape();
  SCV = Glue.Shells();
  //------------------------------------------------------------------------
  // Transfert de la map des elements generes et du shape de Cutvevo dans 
  // myMap et Repositionnement dans l espace initial.
  //------------------------------------------------------------------------
  Transfert (CutVevo, MapProf, MapSpine, LSpine.Inverted(), InitLS, InitLP);

  //Orientation du solid.
  if (Solid) MakeSolid();

//  modified by NIZHNY-EAP Mon Jan 24 11:26:48 2000 ___BEGIN___
  BRepLib::UpdateTolerances(myShape,Standard_False);
//  modified by NIZHNY-EAP Mon Jan 24 11:26:50 2000 ___END___
  myIsDone = Standard_True;
}



//=======================================================================
//function : IsInversed
//purpose  : 
//=======================================================================

static void IsInversed(const TopoDS_Shape& S,
		       const TopoDS_Edge&  E1,
		       const TopoDS_Edge&  E2,
		       Standard_Boolean*   Inverse)
{  

  Inverse[0] = Inverse[1] = 0;
  if (S.ShapeType() != TopAbs_EDGE) return;

  gp_Pnt P;
  gp_Vec DS,DC1,DC2 ;
  BRepAdaptor_Curve CS(TopoDS::Edge(S));
  if (S.Orientation() == TopAbs_FORWARD) {
    CS.D1(CS.FirstParameter(),P,DS);
  }
  else {
    CS.D1(CS.LastParameter(),P,DS);
    DS.Reverse();
  }
    

  if (!BRep_Tool::Degenerated(E1)) {
    BRepAdaptor_Curve C1(TopoDS::Edge(E1));
    if (E1.Orientation() == TopAbs_FORWARD) {
      C1.D1(C1.FirstParameter(),P,DC1);
    }
    else {
     C1.D1(C1.LastParameter(),P,DC1);
     DC1.Reverse();
    }
    Inverse[0] = (DS.Dot(DC1) < 0.);
  }
  else Inverse[0] = 1;

  if (!BRep_Tool::Degenerated(E2)) {
    BRepAdaptor_Curve C2(TopoDS::Edge(E2));
    if (E2.Orientation() == TopAbs_FORWARD) {
      C2.D1(C2.FirstParameter(),P,DC2);
    }
    else {
     C2.D1(C2.LastParameter(),P,DC2);
     DC2.Reverse();
    }
    Inverse[1] = (DS.Dot(DC2) < 0.);
  }
  else Inverse[1] = 1;
}

//=======================================================================
//function : SetWork
//purpose  : 
//=======================================================================

void BRepFill_Evolved::SetWork(const TopoDS_Face& Sp,
			       const TopoDS_Wire& Pr)
{
  mySpine   = Sp;
  myProfile = Pr;
}
					
//=======================================================================
//function : ConcaveSide
//purpose  : Determine si les pipes ont ete construits du cote de la 
//           concavite. Dans ce cas il peuvent etre boucles.
//           WARNING: Pas fini... Seulement fait pour les cercles.
//=======================================================================

static Standard_Boolean ConcaveSide(const TopoDS_Shape& S,
				    const TopoDS_Face&  F)
{

  if (S.ShapeType() == TopAbs_VERTEX) return Standard_False;

  if (S.ShapeType() == TopAbs_EDGE) {
    Standard_Real f,l;
    Handle(Geom2d_Curve) G2d = 
      BRep_Tool::CurveOnSurface(TopoDS::Edge(S),F,f,l);
    Handle(Geom2d_Curve) G2dOC;
    
    Geom2dAdaptor_Curve  AC(G2d,f,l);
    if ( AC.GetType() == GeomAbs_Circle) {
      Standard_Boolean Direct = AC.Circle().IsDirect();
      if (S.Orientation() == TopAbs_REVERSED) Direct = (!Direct);
      return Direct;
    }
  }
  return Standard_False;
}

//=======================================================================
//function : ElementaryPerform
//purpose  : 
//=======================================================================

void BRepFill_Evolved::ElementaryPerform (const TopoDS_Face&              Sp,
					  const TopoDS_Wire&              Pr,
					  const BRepMAT2d_BisectingLocus& Locus,
					        BRepMAT2d_LinkTopoBilo&   Link,
					  const GeomAbs_JoinType          Join)
{

#ifdef DRAW
  if (AffichEdge) {	  
    sprintf(name,"PROFIL_%d",++NbPROFILS);	
    DBRep::Set(name,Pr);
  }
#endif
  TopoDS_Shape aLocalShape = Sp.Oriented(TopAbs_FORWARD);
  mySpine   = TopoDS::Face(aLocalShape);
//  mySpine   = TopoDS::Face(Sp.Oriented(TopAbs_FORWARD));
  myProfile = Pr;
  myMap.Clear();

  BRep_Builder myBuilder;
  myBuilder.MakeCompound(TopoDS::Compound(myShape));
  
  //---------------------------------------------------------------------
  // MapNodeVertex : associe a chaque noeud de la carte (key1) et
  //                 a chaque element du profil (key2) un vertex (item).
  // MapBis        : ensemble des edges ou vertex (item) generes par
  //                 une bisectrice sur une face ou un edge (key)des 
  //                 tuyaux ou revol.
  // MapVerPar     : Map des parametres des vertex sur les edges paralleles 
  //                 la liste contenue dans MapVerPar (E) correspond aux 
  //                 parametres sur E des vertex contenu dans  MapBis(E);
  // MapBS         : liens BasicElt de la carte => Topologie du spine.
  //---------------------------------------------------------------------


  BRepFill_DataMapOfNodeDataMapOfShapeShape MapNodeVertex; 
  TopTools_DataMapOfShapeSequenceOfShape    MapBis;  
  BRepFill_DataMapOfShapeSequenceOfReal     MapVerPar;

  TopTools_DataMapOfShapeShape              EmptyMap;
  TopTools_SequenceOfShape                  EmptySeq;
  TopTools_ListOfShape                      EmptyList;
  TColStd_SequenceOfReal                    EmptySeqOfReal;

  // Repere du profile.
  gp_Ax3 AxeRef(gp_Pnt(0.,0.,0.),
		gp_Dir(0.,0.,1.),
		gp_Dir(1.,0.,0.));  

  //---------------------------------------------------------------
  // Construction des revols et des tuyaux.
  //---------------------------------------------------------------
  BRepTools_WireExplorer ProfExp;
  TopExp_Explorer        FaceExp;
  BRepTools_WireExplorer WireExp;

  for (FaceExp.Init(mySpine,TopAbs_WIRE); FaceExp.More(); FaceExp.Next()){

    for (WireExp.Init(TopoDS::Wire(FaceExp.Current())); WireExp.More(); 
	 WireExp.Next()) {

      TopoDS_Edge   CurrentEdge = WireExp.Current();
      TopoDS_Vertex VFirst,VLast;
      EdgeVertices(CurrentEdge,VFirst,VLast);

      for (Link.Init(VLast); Link.More(); Link.Next()) {
	//------------------------.
	//Construction d un Revol
	//------------------------.
	MakeRevol (CurrentEdge, VLast, AxeRef);
      }

      for (Link.Init(CurrentEdge); Link.More(); Link.Next()) {
	//------------------------.
	//Construction d un Tuyau
	//-------------------------
	MakePipe (CurrentEdge, AxeRef);
      }
    }
  }

#ifdef DRAW
  if (AffichEdge) {
    cout << " Fin Construction des primitives geometriques"<<endl;
  }
#endif

  TopoDS_Vertex  VF,VL;
  
  //---------------------------------------------------
  // Constructions des edges associes aux bissectrices.
  //---------------------------------------------------
  Handle(MAT_Arc)        CurrentArc;
  Handle(Geom2d_Curve)   Bis, PCurve1, PCurve2 ;
  Handle(Geom_Curve)     CBis;
  Standard_Boolean       Reverse;
  TopoDS_Edge            CurrentEdge;
  TopoDS_Shape           S       [2];
  TopoDS_Face            F       [2];
  TopoDS_Edge            E       [4];
  TopLoc_Location        L;
  Standard_Integer       k;

  for (Standard_Integer i = 1; i <= Locus.Graph()->NumberOfArcs(); i++) {
    CurrentArc = Locus.Graph()->Arc(i);
    SimpleExpression(Locus.GeomBis(CurrentArc,Reverse), Bis); 
    
    //-----------------------------------------------------------------------
    // Recuperation des elements du spine correspondant aux basicElts separes.
    //-----------------------------------------------------------------------
    S [0] = Link.GeneratingShape(CurrentArc->FirstElement());
    S [1] = Link.GeneratingShape(CurrentArc->SecondElement());

    Standard_Boolean Concave0 = ConcaveSide(S[0],mySpine);
    Standard_Boolean Concave1 = ConcaveSide(S[1],mySpine);

    TopTools_SequenceOfShape VOnF,VOnL;
    TColgp_SequenceOfPnt     ParOnF,ParOnL;
    
    TopTools_DataMapOfShapeSequenceOfShape MapSeqVer;
    BRepFill_DataMapOfShapeSequenceOfPnt   MapSeqPar;

    Standard_Integer vv = 0;
    for(ProfExp.Init(myProfile); ProfExp.More(); ProfExp.Next()) {
      vv++;
      //-------------------------------------------------------
      // Recuperation des deux faces separees par la bissectrice.
      //-------------------------------------------------------
      F [0] = TopoDS::Face(myMap(S[0])(ProfExp.Current()).First());
      F [1] = TopoDS::Face(myMap(S[1])(ProfExp.Current()).First());
      
      //-----------------------------------------------------------
      // Recuperation des edges paralleles sur chaque face.
      //-----------------------------------------------------------
      TopoDS_Vertex VF,VL;

      EdgeVertices(ProfExp.Current(),VF,VL);

      E [0] = TopoDS::Edge(myMap(S[0])(VF).First());
      E [1] = TopoDS::Edge(myMap(S[0])(VL).First());
      E [2] = TopoDS::Edge(myMap(S[1])(VF).First());
      E [3] = TopoDS::Edge(myMap(S[1])(VL).First());

      Standard_Boolean Inv0[2];
      Standard_Boolean Inv1[2];

      Inv0[0] = Inv0[1] = Inv1[0]= Inv1[1] = 0;
      if (Concave0) IsInversed(S[0],E[0],E[1],Inv0);
      if (Concave1) IsInversed(S[1],E[2],E[3],Inv1);
      
      //---------------------------------------------
      // Construction des geometries.
      //---------------------------------------------
      BRepFill_TrimSurfaceTool Trim (Bis,F[0],F[1],
				     E[0],E[2],Inv0[0],Inv1[0]);
      //-----------------------------------------------------------
      //Construction des vertex correspondant au noeud de la carte.
      //-----------------------------------------------------------
      TopoDS_Vertex VS,VE;
      Handle(MAT_Node) Node1, Node2;

      if (Reverse) {
	Node1 = CurrentArc->SecondNode();
	Node2 = CurrentArc->FirstNode();
      }
      else  {
	Node1 = CurrentArc->FirstNode();
	Node2 = CurrentArc->SecondNode();
      }
      //--------------------------------------------------------
      // Cas Particulier ou le noeud est sur un vertex du spine.
      //--------------------------------------------------------
      if (Node1->OnBasicElt()) {
	if (S[0].ShapeType() == TopAbs_VERTEX) {
	  Node1 = CurrentArc->FirstElement()->StartArc()->FirstNode();
	}
	else if (S[1].ShapeType() == TopAbs_VERTEX) {
	  Node1 = CurrentArc->SecondElement()->StartArc()->FirstNode();
	}
      }	
      // Fin cas particulier.
      
      Standard_Integer 
	StartOnF  = VertexFromNode(Node1, 
				   TopoDS::Edge(ProfExp.Current()), 
				   VF, VL ,
				   MapNodeVertex,VS);
      
      Standard_Integer 
	EndOnF    = VertexFromNode(Node2, 
				   TopoDS::Edge(ProfExp.Current()), 
				   VF, VL ,
				   MapNodeVertex,VE);

      //-----------------------------------------------------------
      // Construction des vertex sur les edges paralleles au spine.
      //-----------------------------------------------------------
      if (!MapSeqVer.IsBound(VF)) {
	if (Inv0 [0] || Inv1 [0]) {
	  ParOnF.Clear();
	  VOnF  .Clear();
	}
	else {
	  Trim.IntersectWith(E [0], E [2], ParOnF);
	  VOnF  .Clear();
	  for (Standard_Integer s = 1; s <= ParOnF.Length(); s++) {
	    TopoDS_Vertex VC;
	    myBuilder.MakeVertex (VC);
	    VOnF.Append(VC);
	  }
	  if (StartOnF == 1) {
	    VOnF  .SetValue(1,VS);
	  }
	  if (EndOnF == 1) {	  
	    VOnF  .SetValue(ParOnF.Length(),VE);
	  }
	}
      }
      else {
	ParOnF = MapSeqPar(VF);
	VOnF   = MapSeqVer(VF);
      }
      
      if (!MapSeqVer.IsBound(VL)) {
	if (Inv0 [1] || Inv1 [1]) {
	  ParOnL.Clear();
	  VOnL  .Clear();
	}
	else {
	  Trim.IntersectWith(E [1], E [3], ParOnL);
	  VOnL.Clear();
	  for (Standard_Integer s = 1; s <= ParOnL.Length(); s++) {	
	    TopoDS_Vertex VC;
	    myBuilder.MakeVertex (VC); 
	    VOnL.Append(VC);
	  }
	  if (StartOnF == 3) {
	    VOnL  .SetValue(1,VS);
	  }
	  if (EndOnF == 3)   {
	    VOnL  .SetValue(ParOnL.Length(),VE);
	  }
	}
      }
      else {
	ParOnL = MapSeqPar(VL);
	VOnL   = MapSeqVer(VL);
      }
      
      //----------------------------------------------------------
      // Test si la Bissectrice ne se projette pas sur la face
      //----------------------------------------------------------
      if ((StartOnF == 0) && (EndOnF == 0) && 
	   VOnL.IsEmpty() && VOnF.IsEmpty())
	// Aucune trace de la bisectrice sur la face.
	continue;

      if ((StartOnF == 0) && (EndOnF == 0) && 
	   (VOnL.Length() + VOnF.Length() == 1)) 
	// le premier ou dernier noeud de l arc est sur une edge
	// mais l arc n est pas sur la face. 
	continue; 

      //---------------------------------------------------------
      // determination des intervalles de la bissectrice qui se
      // projettent sur F[0] et F[1].
      //---------------------------------------------------------
      TColStd_SequenceOfReal     LastPar,FirstPar;
      TopTools_SequenceOfShape   FirstV,LastV;

      ComputeIntervals (VOnF,VOnL,ParOnF,ParOnL,Trim,Bis,
			VS,VE,FirstPar,LastPar,FirstV,LastV);

      for (Standard_Integer Ti =  1; Ti <= FirstPar.Length(); Ti++) {
	TopoDS_Vertex V1 = TopoDS::Vertex(FirstV.Value(Ti));
	TopoDS_Vertex V2 = TopoDS::Vertex(LastV .Value(Ti));

	GeomAbs_Shape Continuity;

	Trim.Project(FirstPar.Value(Ti),LastPar.Value(Ti),
		     CBis,PCurve1,PCurve2,Continuity);
	
	//-------------------------------------
	// Codage de l edge.
	//-------------------------------------
	myBuilder.MakeEdge(CurrentEdge, CBis, 
			   BRepFill_Confusion());

	myBuilder.UpdateVertex(V1,CBis->Value(CBis->FirstParameter()),
			       BRepFill_Confusion()); 
	myBuilder.UpdateVertex(V2,CBis->Value(CBis->LastParameter()),
			       BRepFill_Confusion());

	myBuilder.Add(CurrentEdge,V1.Oriented(TopAbs_FORWARD));
	myBuilder.Add(CurrentEdge,V2.Oriented(TopAbs_REVERSED));

	myBuilder.Range(CurrentEdge,
			CBis->FirstParameter(),
			CBis->LastParameter());
	myBuilder.UpdateEdge(CurrentEdge,PCurve1,F[0],BRepFill_Confusion());
	myBuilder.UpdateEdge(CurrentEdge,PCurve2,F[1],BRepFill_Confusion());

	myBuilder.Continuity(CurrentEdge,F[0],F[1],Continuity);
	
#ifdef DRAW
	if (AffichEdge) {
	  sprintf(name,"ARCEDGE_%d_%d_%d",i,vv,Ti);	
	  DBRep::Set(name,CurrentEdge);
	}
#endif
	//-------------------------------------------
	// Stockage de l edge pour chacune des faces.
	//-------------------------------------------
	for (k = 0; k <= 1;k++) {
	  if (!MapBis.IsBound(F[k])) {
	    MapBis.Bind(F[k],EmptySeq);
	  }
	}
	//---------------------------------------------------------------
	// l orientation de l edge depend du sens de la depouille.
	// depouille => meme orientation E[0] , inverse orientation E[2]
	// si contredepouille c est l inverse.
	//--------------------------------------------------------------
	E[0].Orientation(OriEdgeInFace(E[0],F[0]));
	E[2].Orientation(OriEdgeInFace(E[2],F[1]));
			 
	if (DistanceToOZ(VF) < DistanceToOZ(VL)  ) { 
	  // Depouille
	  MapBis(F[0]).Append(CurrentEdge.Oriented  (E[0].Orientation()));
	  CurrentEdge.Orientation(TopAbs::Complement(E[2].Orientation()));
	  MapBis(F[1]).Append(CurrentEdge);
	}
	else {
	  //Contre Depouille
	  MapBis(F[1]).Append(CurrentEdge.Oriented  (E[2].Orientation()));
	  CurrentEdge.Orientation(TopAbs::Complement(E[0].Orientation()));
	  MapBis(F[0]).Append(CurrentEdge);
	}
      }

      //----------------------------------------------
      // Stockage des vertex sur les edges paralleles.
      // on remplit MapBis et MapVerPar.
      // VOnF pour E[0] et E[2].
      // VOnL pour E[1] et E[3].
      //----------------------------------------------
      for (k = 0; k <= 2; k = k+2) {
	if ( !MapSeqVer.IsBound(VF)) {
	  if (!VOnF.IsEmpty()) {
	    if (!MapBis.IsBound(E[k])) {
	      MapBis   .Bind(E[k],EmptySeq);
	      MapVerPar.Bind(E[k],EmptySeqOfReal);
	    } 
	    for (Standard_Integer ii = 1; ii <= VOnF.Length(); ii++) {
	      MapBis (E[k]).Append(VOnF.Value(ii));
	      if (k == 0) MapVerPar (E[k]).Append(ParOnF.Value(ii).Y());
	      else        MapVerPar (E[k]).Append(ParOnF.Value(ii).Z());
	    }
	  }
	}
      }
	
      for (k = 1; k <= 3; k = k+2) {
	if ( !MapSeqVer.IsBound(VL)) {
	  if (!VOnL.IsEmpty()) {
	    if (!MapBis.IsBound(E[k])) {
	      MapBis   .Bind(E[k],EmptySeq);
	      MapVerPar.Bind(E[k],EmptySeqOfReal);
	    }
	    for (Standard_Integer ii = 1; ii <= VOnL.Length(); ii++) {
	      MapBis(E[k]).Append(VOnL.Value(ii));
	      if (k == 1) MapVerPar (E[k]).Append(ParOnL.Value(ii).Y());
	      else        MapVerPar (E[k]).Append(ParOnL.Value(ii).Z());
	    }
	  }
	}
      }

      //----------------------------------------------------------------
      // Edge [1] de la face courante sera Edge [0] de la face suivante.
      // => copie de VonL dans VonF. Pour ne pas creer deux fois les memes
      // vertex.
      //-----------------------------------------------------------------

      MapSeqPar.Bind(VF,ParOnF);
      MapSeqVer.Bind(VF,VOnF);
      MapSeqPar.Bind(VL,ParOnL);
      MapSeqVer.Bind(VL,VOnL);

    }
  }

#ifdef DEB
 if (AffichEdge) {
   cout << " Fin Construction des edges et vertex sur les bissectrices"<<endl;
 }
#endif

  //----------------------------------
  // Construction des edges paralleles.
  //----------------------------------
  BRepFill_DataMapIteratorOfDataMapOfShapeDataMapOfShapeListOfShape ite1;
  TopoDS_Shape           CurrentProf,PrecProf;
  TopoDS_Face            CurrentFace;
  TopoDS_Shape           CurrentSpine;
  TopoDS_Vertex          VCF,VCL;

  for (ite1.Initialize(myMap); ite1.More(); ite1.Next()) {
    CurrentSpine = ite1.Key();

    for (ProfExp.Init(myProfile); ProfExp.More(); ProfExp.Next()){	
      CurrentProf = ProfExp.Current();
      EdgeVertices(TopoDS::Edge(CurrentProf),VCF,VCL);
      CurrentEdge  = TopoDS::Edge(myMap(CurrentSpine)(VCF).First());
      
      //-------------------------------------------------------------
      //RQ : Current Edge est oriente par rapport a la face (oriente forward)
      //     genere par l edge CurrentProf .
      //-------------------------------------------------------------
      if (MapBis.IsBound(CurrentEdge)) {
	
	//--------------------------------------------------------
	// Recherche si une des deux faces connexes de l edge
	// appartient au volevo. Les edges sur cette face servent
	// a eliminer certains vertex qui peuvent apparaitre deux
	// fois sur l edge parallele. Ces Vertex corespondent a des
	// noeuds de la carte.
	//---------------------------------------------------------
	TopoDS_Shape     FaceControle;
	Standard_Boolean YaFace = Standard_True;
	
	FaceControle = myMap(CurrentSpine)(CurrentProf).First();
	if (!MapBis.IsBound(FaceControle)){
	  YaFace = Standard_False;
	  if (!PrecProf.IsNull()) {
	    FaceControle = myMap(CurrentSpine)(PrecProf).First();
	    if (MapBis.IsBound(FaceControle)){
	      YaFace = Standard_True;
	    }
	  }
	}
	
	if (YaFace) {
	  //------------------------------------------------------------
	  // Pas de face connexe dans le volevo => pas d edge parallele.
	  //------------------------------------------------------------
	  TopTools_SequenceOfShape S;
	  TrimEdge (CurrentEdge,
		    MapBis   (FaceControle), 
		    MapBis   (CurrentEdge) ,  
		    MapVerPar(CurrentEdge) , S);
	  
	  for ( k = 1; k <= S.Length(); k++) {
	    myMap(CurrentSpine)(VCF).Append(S.Value(k));
	    
#ifdef DRAW	    
	    if (AffichEdge) {
	      sprintf(name,"PAREDGE_%d_%d",++NbEDGES,k);	
	      DBRep::Set(name,S.Value(k));
	    }
#endif	    
	  }
	}
      }
      PrecProf = CurrentProf;
    }
    
    //------------------------------------------------------------
    // Construction edge parallele du dernier vertex de myProfile.
    //------------------------------------------------------------
    CurrentEdge  = TopoDS::Edge(myMap(CurrentSpine)(VCL).First());
    
    if (MapBis.IsBound(CurrentEdge)) {
      Standard_Boolean YaFace = Standard_True;
      TopoDS_Shape     FaceControle;
      
      FaceControle = myMap(CurrentSpine)(CurrentProf).First();
      if (!MapBis.IsBound(FaceControle)){
	YaFace = Standard_False;
      }
      // le nombre d element de la liste permet de savoir
      // si les edges ont deja ete faite (profile ferme) .
      if (YaFace && myMap(CurrentSpine)(VCL).Extent()<= 1) {
	TopTools_SequenceOfShape S;
	TrimEdge (CurrentEdge, 
		  MapBis   (FaceControle), 
		  MapBis   (CurrentEdge) , 
		  MapVerPar(CurrentEdge) , S);
	
	for ( k = 1; k <= S.Length(); k++) {
	  myMap(CurrentSpine)(VCL).Append(S.Value(k));
	  
#ifdef DRAW	    
	  if (AffichEdge) {	  
	    sprintf(name,"PAREDGE_%d_%d",++NbEDGES,k);	
	    DBRep::Set(name,S.Value(k));
	  }
#endif
	}
      }
    }
  }
  
#ifdef DRAW
  if (AffichEdge) {
    cout <<" Fin Construction des edges paralleles"<<endl;
  }
#endif

  //-------------------------------------------------------------------
  // Decoupe des faces par les edges.
  //-------------------------------------------------------------------
  for (ite1.Initialize(myMap); ite1.More(); ite1.Next()) {
    CurrentSpine = ite1.Key();
    
    for (ProfExp.Init(myProfile); ProfExp.More(); ProfExp.Next()){
      CurrentProf = ProfExp.Current();
      CurrentFace = TopoDS::Face(myMap(CurrentSpine)(CurrentProf).First());
      myMap(CurrentSpine)(CurrentProf).Clear();
      
      if (MapBis.IsBound(CurrentFace)) {
	//----------------------------------------------------------
	// Si la face ne contient pas d edges pour la restreindre
	// c est qu'elle n apparait pas dans le volevo.
	// la decoupe de la face par les edges peut generer plusieurs
	// faces.
	//
	// On ajoute les edges generes sur les edges paralleles a 
	// l ensemble des edges qui limitent la face.
	//
	//------------------------------------------------------------
	EdgeVertices(TopoDS::Edge(CurrentProf),VCF,VCL);

	TopTools_ListIteratorOfListOfShape itl;
	const TopTools_ListOfShape& LF = myMap(CurrentSpine)(VCF);

	TopAbs_Orientation Ori = OriEdgeInFace(TopoDS::Edge(LF.First()),
					       CurrentFace);
	for (itl.Initialize(LF), itl.Next(); itl.More(); itl.Next()) {
	  TopoDS_Edge RE = TopoDS::Edge(itl.Value());
	  MapBis(CurrentFace).Append(RE.Oriented(Ori));
	}
	const TopTools_ListOfShape& LL = myMap(CurrentSpine)(VCL);	  
	Ori = OriEdgeInFace(TopoDS::Edge(LL.First()),CurrentFace);
	for (itl.Initialize(LL), itl.Next() ; itl.More(); itl.Next()) {	 
	  TopoDS_Edge RE = TopoDS::Edge(itl.Value());
	  MapBis(CurrentFace).Append(RE.Oriented(Ori));
	}
	
	//Decoupe de la face.
	TopTools_SequenceOfShape  S;

	TrimFace (CurrentFace, MapBis(CurrentFace), S);

	for (Standard_Integer ii = 1; ii <= S.Length(); ii++) {
	  myBuilder.Add (myShape,S.Value(ii));
	  myMap(CurrentSpine)(CurrentProf).Append(S.Value(ii));
	}
      }
    }
    //-----------------------------------------------------------------
    // Suppression premiere edge (edge d origine)des listes de myMap 
    // correspondant aux vertex du profil.
    //-----------------------------------------------------------------
    TopExp_Explorer Explo(myProfile,TopAbs_VERTEX);
    TopTools_MapOfShape vmap;

    for ( ; Explo.More(); Explo.Next()){
      if (vmap.Add(Explo.Current())) {
	myMap(CurrentSpine)(Explo.Current()).RemoveFirst();
      }
    }
  }	 
  myIsDone = Standard_True;

#ifdef DRAW  
  if (AffichEdge) {	  
    cout <<" Fin de construction d un volevo elementaire."<<endl;	    
    sprintf(name,"VEVO_%d",++NbVEVOS);	
    DBRep::Set(name,myShape);
  }
#endif
}

//=======================================================================
//function : PlanarPerform
//purpose  : 
//=======================================================================

void BRepFill_Evolved::PlanarPerform (const TopoDS_Face&              Sp,
				      const TopoDS_Wire&              Pr,
				      const BRepMAT2d_BisectingLocus& Locus,
					    BRepMAT2d_LinkTopoBilo&   Link,
				      const GeomAbs_JoinType          Join)
{
  TopoDS_Shape aLocalShape = Sp.Oriented(TopAbs_FORWARD);
  mySpine   = TopoDS::Face(aLocalShape);
//  mySpine   = TopoDS::Face(Sp.Oriented(TopAbs_FORWARD));
  myProfile = Pr;
  myMap.Clear();

  BRep_Builder B;
  B.MakeCompound(TopoDS::Compound(myShape));
   
  BRepTools_WireExplorer             ProfExp;
  TopExp_Explorer                    Exp,exp1,exp2;
  TopoDS_Shape                       Rest;
  TopTools_DataMapOfShapeListOfShape EmptyMap;
  TopTools_ListOfShape               EmptyList;
  TopTools_DataMapOfShapeShape       MapVP;
  BRepFill_OffsetWire                Paral;

  for (ProfExp.Init(myProfile); ProfExp.More(); ProfExp.Next()){
    const TopoDS_Edge& E = ProfExp.Current();
    BRepAlgo_FaceRestrictor FR;
    BRepFill_OffsetAncestors OffAnc;

    TopoDS_Vertex V[2];
    EdgeVertices(E,V[0],V[1]);
    Standard_Real Alt = Altitud(V[0]);
    Standard_Real Offset[2];
    Offset[0] = DistanceToOZ(V[0]);
    Offset[1] = DistanceToOZ(V[1]);
    Standard_Boolean IsMinV1 = ( Offset[0] < Offset[1]);

    for (Standard_Integer i = 0; i <= 1; i++) {
      if (!MapVP.IsBound(V[i])) {
	//------------------------------------------------
	// Calcul des paralleles correspondant aux vertex.
	//------------------------------------------------
	Paral.PerformWithBiLo(mySpine,Offset[i],Locus,Link,Join,Alt);
	OffAnc.Perform(Paral);
	MapVP.Bind(V[i],Paral.Shape());

	//-----------------------------
	// Mise a jour myMap (.)(V[i])
	//-----------------------------
	for (Exp.Init(Paral.Shape(),TopAbs_EDGE);
	     Exp.More();
	     Exp.Next()) {
	  const TopoDS_Edge& WC = TopoDS::Edge(Exp.Current());
	  const TopoDS_Shape& GS = OffAnc.Ancestor(WC);
	  if ( !myMap.IsBound(GS)) 
	    myMap.Bind(GS, EmptyMap);
	  if ( !myMap(GS).IsBound(V[i]))
	    myMap(GS).Bind(V[i],Paral.GeneratedShapes(GS));
	}
      }
      TopoDS_Shape Rest = MapVP(V[i]);
      
      Standard_Boolean ToReverse = Standard_False;
       if ( ( IsMinV1 && (i==1)) || (!IsMinV1 && (i==0)) )
	ToReverse = Standard_True;

      if (!Rest.IsNull()) {
	if (Rest.ShapeType() == TopAbs_WIRE) {
	  if ( ToReverse){
	    TopoDS_Shape aLocalShape  = Rest.Reversed();
	    TopoDS_Wire aWire = TopoDS::Wire(aLocalShape);
	    FR.Add(aWire);
	  }	  
	  else
	    FR.Add(TopoDS::Wire(Rest));
	}
	else {
	  for (Exp.Init(Rest,TopAbs_WIRE);Exp.More();Exp.Next()) {
	    TopoDS_Wire WCop = TopoDS::Wire(Exp.Current());
	    if ( ToReverse){
	      TopoDS_Shape aLocalShape = WCop.Reversed();
	      TopoDS_Wire bWire =   TopoDS::Wire(aLocalShape);
//	      TopoDS_Wire bWire =   TopoDS::Wire(WCop.Reversed());
	      FR.Add(bWire);
	    }	    
	    else
	      FR.Add(WCop);
	  }
	}
      }
    }
#ifdef DRAW  
    if (AffichEdge) {	  
      TopTools_DataMapIteratorOfDataMapOfShapeShape it(MapVP);
      Standard_Integer k = 0;
      for (; it.More(); it.Next()) {
	sprintf(name,"PARALI_%d",++k);	
	DBRep::Set(name,it.Value());
      }
    }
#endif

    //----------------------------------------------------
    // Construction des faces limitees par les paralleles.
    // - mise a hauteur de la face support.
    //----------------------------------------------------
    gp_Trsf T; T.SetTranslation(gp_Vec(0,0,Alt));
    TopLoc_Location LT(T);
    TopoDS_Shape aLocalShape = mySpine.Moved(LT);
    FR.Init(TopoDS::Face(aLocalShape));
//    FR.Init(TopoDS::Face(mySpine.Moved(LT)));
    FR.Perform();

    for ( ;FR.More(); FR.Next()) {
      const TopoDS_Face& F = FR.Current();
      B.Add(myShape,F);
      //---------------------------------------
      // Mise a jour myMap(.)(E)
      //---------------------------------------
      for ( Exp.Init(F,TopAbs_EDGE); Exp.More(); Exp.Next()) {
	const TopoDS_Edge& CE = TopoDS::Edge(Exp.Current());
	if (OffAnc.HasAncestor(CE)) {
	  const TopoDS_Shape& InitE = OffAnc.Ancestor(CE);
	  if ( !myMap.IsBound(InitE)) 
	    myMap.Bind(InitE, EmptyMap);
	  if ( !myMap(InitE).IsBound(E))
	    myMap(InitE).Bind(E,EmptyList);
	  myMap(InitE)(E).Append(F);
	}
      }
    }
  }  // Fin boucle sur profil.
}


//=======================================================================
//function : VerticalPerform
//purpose  : 
//=======================================================================

void BRepFill_Evolved::VerticalPerform (const TopoDS_Face&              Sp,
					const TopoDS_Wire&              Pr,
					const BRepMAT2d_BisectingLocus& Locus,
					      BRepMAT2d_LinkTopoBilo&   Link,
					const GeomAbs_JoinType          Join)
{
  TopoDS_Shape aLocalShape = Sp.Oriented(TopAbs_FORWARD);
  mySpine   = TopoDS::Face(aLocalShape);
//  mySpine   = TopoDS::Face(Sp.Oriented(TopAbs_FORWARD));
  myProfile = Pr;
  myMap.Clear();

  BRep_Builder B;
  B.MakeCompound(TopoDS::Compound(myShape));
   
  BRepTools_WireExplorer   ProfExp;
  TopExp_Explorer          Exp;
  BRepFill_OffsetWire      Paral;
  BRepFill_OffsetAncestors OffAnc;
  TopoDS_Vertex            V1,V2;

  Standard_Boolean First = Standard_True;
  TopoDS_Shape     Base;
  TopTools_DataMapOfShapeListOfShape  EmptyMap;

  for (ProfExp.Init(myProfile); ProfExp.More(); ProfExp.Next()){
    const TopoDS_Edge& E = ProfExp.Current();
    EdgeVertices(E,V1,V2);
    Standard_Real Alt1 = Altitud(V1);
    Standard_Real Alt2 = Altitud(V2);

     if (First) {
      Standard_Real Offset = DistanceToOZ(V1);
      if (Abs(Offset) < BRepFill_Confusion()) {
	Offset = 0.;
      }
      Paral.PerformWithBiLo(mySpine,Offset,Locus,Link,Join,Alt1);   
      OffAnc.Perform(Paral);
      Base = Paral.Shape();
      
      // MAJ myMap
      for (Exp.Init(Base,TopAbs_EDGE); Exp.More(); Exp.Next()) {
	const TopoDS_Edge&  E  = TopoDS::Edge(Exp.Current());
	const TopoDS_Shape& AE =  OffAnc.Ancestor(E);
	if (!myMap.IsBound(AE)) {
	  myMap.Bind(AE,EmptyMap);
	}
	if (!myMap(AE).IsBound(V1)) {
	  TopTools_ListOfShape L;
	  myMap(AE).Bind(V1,L);
	}
	myMap(AE)(V1).Append(E);
      }
      First = Standard_False;
    }
    
    
#ifdef DRAW  
    if (AffichEdge) {	  
      sprintf(name,"PARALI_%d",++NbVEVOS);	
      DBRep::Set(name,Base);
    }
#endif
    
    BRepSweep_Prism PS(Base,gp_Vec(0,0,Alt2 - Alt1),Standard_False);
#ifdef DRAW  
    if (AffichEdge) {	  
      sprintf(name,"PRISM_%d",NbVEVOS);	
      DBRep::Set(name,PS.Shape());
    }
#endif

    Base = PS.LastShape();
    
    for (Exp.Init(PS.Shape(),TopAbs_FACE); Exp.More(); Exp.Next()) {
      B.Add(myShape,Exp.Current());
    }
    
    // MAJ myMap
    BRepFill_DataMapIteratorOfDataMapOfShapeDataMapOfShapeListOfShape 
      it(myMap);
    
    for (; it.More(); it.Next()) {
      const TopTools_ListOfShape& LOF = it.Value()(V1);
      TopTools_ListIteratorOfListOfShape itLOF(LOF);
      if (!myMap(it.Key()).IsBound(V2)) {
	TopTools_ListOfShape L;
	myMap(it.Key()).Bind(V2,L);
      }
      
      if (!myMap(it.Key()).IsBound(E)) {
	TopTools_ListOfShape L;
	myMap(it.Key()).Bind(E,L);
      }

      for (; itLOF.More(); itLOF.Next()) {
	const TopoDS_Shape& OS = itLOF.Value();
	myMap(it.Key())(V2).Append(PS.LastShape(OS));
	myMap(it.Key())(E).Append(PS.Shape(OS));
      }
    }
  }
}

//=======================================================================
//function : Bubble
//purpose  : Ordonne la sequence de point en x croissant. 
//=======================================================================

static void Bubble(TColStd_SequenceOfReal& Seq) 
{
  Standard_Boolean Invert = Standard_True;
  Standard_Integer NbPoints = Seq.Length();

  while (Invert) {
    Invert = Standard_False;
    for ( Standard_Integer i = 1; i < NbPoints; i++) {
      if ( Seq.Value(i+1) < Seq.Value(i)) {
	Seq.Exchange(i,i+1);
	Invert = Standard_True;
      }
    }
  }
}


//=======================================================================
//function : PrepareProfile
//purpose  : - Projection du profil dans le plan de travail.
//           - Decoupe du profil aux extrema de distance du profil
//           a l axe Oz.
//           - On isole les parties verticales et horizontales.
//           - Reconstruction de wires a partir des edges decoupees.
//           Les nouveaux wires stockes dans <WorkProf> sont toujours du
//           meme cote de l axe OZ ou sont confondus avec celui-ci
//=======================================================================

void BRepFill_Evolved::PrepareProfile(TopTools_ListOfShape&         WorkProf, 
				      TopTools_DataMapOfShapeShape& MapProf  ) 
const 
{
  // Le profil est suppose place de telle sorte que la seule transformation 
  // a effectuer soit une projection dans le plan yOz.

  // initialise the projection Plane and the Line to evaluate the extrema.
  Handle(Geom_Plane) Plane = new Geom_Plane(gp_Ax3(gp::YOZ()));
  Handle(Geom2d_Line) Line = new Geom2d_Line(gp::OY2d());

  // Map vertex initiaux -> vertex projete.
  TopTools_DataMapOfShapeShape MapVerRefMoved;

  TopoDS_Vertex V1,V2,VRef1,VRef2;
  TopoDS_Wire   W;
  BRep_Builder  B;
  TopTools_ListOfShape WP;
  B.MakeWire(W);
  WP.Append(W);

  BRepTools_WireExplorer Exp(myProfile) ;

  while (Exp.More()) {
    TopTools_ListOfShape Cuts;
    Standard_Boolean     NewWire = Standard_False;
    const TopoDS_Edge&   E = TopoDS::Edge(Exp.Current());

    // Decoupe de l edge.
    CutEdgeProf (E ,Plane ,Line ,Cuts ,MapVerRefMoved);

    EdgeVertices(E,VRef1,VRef2);

    if ( Cuts.IsEmpty()) { 
      // Pas d extrema ni d intersections ni de vertex sur l axe.
      B.Add(W,E);
      MapProf.Bind(E,E);
    }
    else {
      while (!Cuts.IsEmpty()) {
	const TopoDS_Edge& NE = TopoDS::Edge(Cuts.First());
	MapProf.Bind(NE,E);
	EdgeVertices(NE,V1,V2);
	if (!MapProf.IsBound(V1)) MapProf.Bind(V1,E);
	if (!MapProf.IsBound(V2)) MapProf.Bind(V2,E);

	B.Add(W,NE);
	Cuts.RemoveFirst();

	if (DistanceToOZ(V2) < BRepFill_Confusion() &&
	    DistanceToOZ(V1) > BRepFill_Confusion()) {
	  // NE se termine sur l axe OZ => nouveau wire
	  if (Cuts.IsEmpty()) {
	    // derniere portion de l edge courante
	    // Si ce n est pas le dernier edge de myProfile 
	    // on creera un nouveau wire.
	    NewWire = Standard_True;
	  }
	  else {
	    // Nouveau wire.
	    B.MakeWire(W);
	    WP.Append(W);
	  }
	}
      }
    }
    Exp.Next();
    if (Exp.More() && NewWire) {
      B.MakeWire(W);
      WP.Append(W);
    }
  }

  // Dans la liste des Wires, on cherche les edges generant des vevo plans
  // ou verticaux.
  TopTools_ListIteratorOfListOfShape ite;
  TopoDS_Wire CurW,NW;
  TopExp_Explorer EW;
  

  for (ite.Initialize(WP); ite.More(); ite.Next()) {
    CurW = TopoDS::Wire(ite.Value());
    Standard_Boolean YaModif = Standard_False;
    for (EW.Init(CurW,TopAbs_EDGE); EW.More(); EW.Next()) {
      const TopoDS_Edge& EE = TopoDS::Edge(EW.Current());
      if (IsVertical(EE) || IsPlanar(EE)) {
	YaModif = Standard_True;
	break;
      }
    }
    
    if (YaModif) {
      //Status = 0 for the begining
      //         3 vertical
      //         2 horizontal
      //         1 other
      Standard_Integer Status = 0; 
                                    
      for (EW.Init(CurW,TopAbs_EDGE); EW.More(); EW.Next()) {
	const TopoDS_Edge& EE = TopoDS::Edge(EW.Current());
	if (IsVertical(EE)) {
	  if (Status != 3) {
	    B.MakeWire(NW);
	    WorkProf.Append(NW);
	    Status = 3;
	  }
	}
	else if (IsPlanar(EE)) {
	  if (Status != 2) {
	    B.MakeWire(NW);
	    WorkProf.Append(NW);
	    Status = 2;
	  }
	}
	else if ( Status != 1) {
	  B.MakeWire(NW);
	  WorkProf.Append(NW);
	  Status = 1;
	}
	B.Add(NW,EE);
      }
    }
    else {
      WorkProf.Append(CurW);
    }
  }

  //bind des vertex modifies dans MapProf;
  TopTools_DataMapIteratorOfDataMapOfShapeShape gilbert(MapVerRefMoved);
  for ( ;gilbert.More() ;gilbert.Next()) {
    MapProf.Bind(gilbert.Value(),gilbert.Key());
  }
}


//=======================================================================
//function : PrepareSpine
//purpose  : 
//=======================================================================

void BRepFill_Evolved::PrepareSpine(TopoDS_Face&                 WorkSpine, 
				    TopTools_DataMapOfShapeShape& MapSpine) 
const
{
  BRep_Builder      B;
  TopTools_ListOfShape Cuts;
  TopTools_ListIteratorOfListOfShape IteCuts;
  TopoDS_Vertex V1,V2;
  
  TopLoc_Location L;
  const Handle(Geom_Surface)& S    = BRep_Tool::Surface  (mySpine,L);
  Standard_Real               TolF = BRep_Tool::Tolerance(mySpine);
  B.MakeFace(WorkSpine,S,L,TolF);
  
  for (TopoDS_Iterator IteF(mySpine) ; IteF.More(); IteF.Next()) {

    TopoDS_Wire NW;
    B.MakeWire (NW);

    for (TopoDS_Iterator IteW(IteF.Value()); IteW.More(); IteW.Next()) {
      
      const TopoDS_Edge& E = TopoDS::Edge(IteW.Value());
      EdgeVertices(E,V1,V2);
      MapSpine.Bind(V1,V1);
      MapSpine.Bind(V2,V2);
      Cuts.Clear();

      // Decoupe
      CutEdge (E, mySpine, Cuts);
      
      if (Cuts.IsEmpty()) {
	B.Add(NW,E);
	MapSpine.Bind(E,E);
      }
      else {	
	for (IteCuts.Initialize(Cuts); IteCuts.More(); IteCuts.Next()) {
	  const TopoDS_Edge& NE = TopoDS::Edge(IteCuts.Value());
	  B.Add(NW,NE);
	  MapSpine.Bind(NE,E);
	  EdgeVertices(NE,V1,V2);
	  if (!MapSpine.IsBound(V1)) MapSpine.Bind(V1,E);
	  if (!MapSpine.IsBound(V2)) MapSpine.Bind(V2,E);
	}
      }
    }
    B.Add(WorkSpine, NW);
  }

  // On construit les courbes 3d de la spine Sinon aux fraise
  BRepLib::BuildCurves3d(WorkSpine);

#ifdef DRAW
  if (AffichEdge) {
    sprintf(name,"workspine");	
    DBRep::Set(name,WorkSpine);
  }
#endif 

}
//=======================================================================
//function : GeneratedShapes
//purpose  : 
//=======================================================================

const TopoDS_Shape&  BRepFill_Evolved::Top() const 
{
  return myTop;
}

//=======================================================================
//function : GeneratedShapes
//purpose  : 
//=======================================================================

const TopoDS_Shape&  BRepFill_Evolved::Bottom() const 
{
  return myBottom;
}

//=======================================================================
//function : GeneratedShapes
//purpose  : 
//=======================================================================

const TopTools_ListOfShape&  BRepFill_Evolved::GeneratedShapes ( 
   const TopoDS_Shape& SpineShape,
   const TopoDS_Shape& ProfShape )
const 
{
  if (myMap            .IsBound(SpineShape) &&
      myMap(SpineShape).IsBound(ProfShape)     ) {
    return myMap(SpineShape)(ProfShape);
  }
  else {
    static TopTools_ListOfShape Empty;
    return Empty;
  }
}

//=======================================================================
//function : Generated
//purpose  : 
//=================================================================== ====

BRepFill_DataMapOfShapeDataMapOfShapeListOfShape& BRepFill_Evolved::Generated()
{
  return myMap;
}

//=======================================================================
//function : Compare
//purpose  : 
//=======================================================================

static TopAbs_Orientation Compare (const TopoDS_Edge& E1,
				   const TopoDS_Edge& E2) 
{
  TopAbs_Orientation OO = TopAbs_FORWARD;
  TopoDS_Vertex V1[2],V2[2];
  TopExp::Vertices (E1,V1[0],V1[1]);
  TopExp::Vertices (E2,V2[0],V2[1]);
  gp_Pnt P1 = BRep_Tool::Pnt(V1[0]);
  gp_Pnt P2 =BRep_Tool::Pnt(V2[0]);
  gp_Pnt P3 =BRep_Tool::Pnt(V2[1]);
  if (P1.Distance(P3) < P1.Distance(P2)) OO = TopAbs_REVERSED; 

  return OO;
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void BRepFill_Evolved::Add(      BRepFill_Evolved& Vevo, 
			   const TopoDS_Wire&      Prof,
			         BRepTools_Quilt&  Glue)		

{  
  BRepFill_DataMapOfShapeDataMapOfShapeListOfShape& MapVevo = Vevo.Generated();
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape                  iteP;
  BRepFill_DataMapIteratorOfDataMapOfShapeDataMapOfShapeListOfShape    iteS;
  TopoDS_Shape CurrentSpine, CurrentProf;

  if (Vevo.Shape().IsNull()) return;

  //-------------------------------------------------
  // Recherche des wires communs a <me> et a <Vevo>.
  //-------------------------------------------------

  TopExp_Explorer ExProf;
  for (ExProf.Init(Prof,TopAbs_VERTEX); ExProf.More(); ExProf.Next()) {
    const TopoDS_Shape& VV = ExProf.Current();
     //---------------------------------------------------------------
    // Parcours des edge generes par VV dans myMap si elles existent 
    // et Bind dans Glue
    //---------------------------------------------------------------
   
    //------------------------------------------------- -------------
    // Remarque les courbes des edges a bindes sont dans le meme sens.
    //          si on reste du meme cote.
    //          si on passe de gauche a droite elles sont inversees.
    //------------------------------------------------- -------------
#ifndef DEB
    Standard_Boolean   Commun = Standard_False;
#else
    Standard_Boolean   Commun;
#endif
#ifdef DEB
    TopAbs_Orientation OriSide = 
#endif
      Relative(myProfile,Prof,
	       TopoDS::Vertex(VV),
	       Commun);

    if (Commun) {
      for (iteS.Initialize(myMap); iteS.More(); iteS.Next()) {
	const TopoDS_Shape& SP = iteS.Key();
	if (iteS.Value().IsBound(VV) && 
	    MapVevo.IsBound(SP) && MapVevo(SP).IsBound(VV)) {
	  
	  const TopTools_ListOfShape& MyList   = myMap(SP)(VV);
	  const TopTools_ListOfShape& VevoList = Vevo.GeneratedShapes(SP,VV);
	  TopTools_ListIteratorOfListOfShape MyIte  (MyList);
	  TopTools_ListIteratorOfListOfShape VevoIte(VevoList);
	  for (; MyIte.More(); MyIte.Next(), VevoIte.Next()) {
	    const TopoDS_Edge& ME = TopoDS::Edge(MyIte  .Value());
	    const TopoDS_Edge& VE = TopoDS::Edge(VevoIte.Value());
	    TopAbs_Orientation OG = Compare(ME,VE);
	    TopoDS_Shape aLocalShape  = VE.Oriented (TopAbs_FORWARD);
	    TopoDS_Shape aLocalShape2 = ME.Oriented (OG);
	    Glue.Bind(TopoDS::Edge(aLocalShape),TopoDS::Edge(aLocalShape2));
//	    Glue.Bind(TopoDS::Edge(VE.Oriented (TopAbs_FORWARD)),
//		      TopoDS::Edge(ME.Oriented (OG)));
	  }
	}
      }
    }
  }
  Glue.Add(Vevo.Shape());

  //----------------------------------------------------------
  // Ajout de la map des elements generes dans Vevo dans myMap.
  //----------------------------------------------------------
  TopTools_DataMapOfShapeListOfShape        EmptyMap;
  TopTools_ListOfShape                      EmptyList;

  for (iteS.Initialize(MapVevo); iteS.More() ; iteS.Next()) {
    CurrentSpine = iteS.Key();
    for (iteP.Initialize(MapVevo(CurrentSpine)); iteP.More(); iteP.Next()) {
      CurrentProf  = iteP.Key();
      if (!myMap.IsBound(CurrentSpine)) {
	//------------------------------------------------
	// L element du spine n etait pas encore present .
	// => profil precedent pas sur le bord.
	//-------------------------------------------------
	myMap.Bind(CurrentSpine,EmptyMap);
      }
      if (!myMap(CurrentSpine).IsBound(CurrentProf)) {
	myMap(CurrentSpine).Bind(CurrentProf,EmptyList);      
	const TopTools_ListOfShape& GenShapes 
	  = MapVevo (CurrentSpine)(CurrentProf);
	TopTools_ListIteratorOfListOfShape itl (GenShapes);
	for (; itl.More(); itl.Next()) {
	  // lors de Glue.Add les shapes partages son recrees.
	  if (Glue.IsCopied(itl.Value())) 
	    myMap(CurrentSpine)(CurrentProf).Append(Glue.Copy(itl.Value()));
	  else
	    myMap(CurrentSpine)(CurrentProf).Append(itl.Value());
	}
      }
    }
  }
}

//=======================================================================
//function : ChangeShape
//purpose  : 
//=======================================================================

TopoDS_Shape&  BRepFill_Evolved::ChangeShape()
{
  return myShape;
}

//=======================================================================
//function : Transfert
//purpose  : 
//=======================================================================

void BRepFill_Evolved::Transfert(      BRepFill_Evolved&             Vevo, 
				 const TopTools_DataMapOfShapeShape& MapProf, 
				 const TopTools_DataMapOfShapeShape& MapSpine,
				 const TopLoc_Location&              LS,
				 const TopLoc_Location&              InitLS,
				 const TopLoc_Location&              InitLP)
{  
  //--------------------------------------------------------------
  // Transfert du shape de Vevo dans myShape et Repositionnement
  // des shapes.
  //--------------------------------------------------------------
  myShape = Vevo.Shape();
  mySpine  .Location(InitLS); 
  myProfile.Location(InitLP);
  myShape  .Move    (LS);

  //
  // En attendant mieux, on force le Same Parameter ici 
  //  ( Pb Sameparameter entre YaPlanar et Tuyaux 
  //
  BRep_Builder B;
  TopExp_Explorer ex(myShape,TopAbs_EDGE);
  while (ex.More()) {
    B.SameRange(TopoDS::Edge(ex.Current()), Standard_False);
    B.SameParameter(TopoDS::Edge(ex.Current()), Standard_False);
    BRepLib::SameParameter(TopoDS::Edge(ex.Current()));
    ex.Next();
  }


  //--------------------------------------------------------------
  // Transfert de myMap de Vevo dans myMap.
  //--------------------------------------------------------------
  BRepFill_DataMapIteratorOfDataMapOfShapeDataMapOfShapeListOfShape iteS;
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape               iteP; 
  TopTools_DataMapOfShapeListOfShape EmptyMap;
  TopTools_ListOfShape               EmptyList;
  TopoDS_Shape                       InitialSpine, InitialProf;

  BRepFill_DataMapOfShapeDataMapOfShapeListOfShape& MapVevo 
    = Vevo.Generated();

  for (iteS.Initialize(MapVevo); iteS.More(); iteS.Next()) {
    InitialSpine = MapSpine(iteS.Key());
    InitialSpine.Move(LS);

    for (iteP.Initialize(MapVevo(iteS.Key())); iteP.More(); iteP.Next()) {
      InitialProf  = MapProf (iteP.Key());
      InitialProf.Location(InitLP);

      TopTools_ListOfShape& GenShapes = 
	MapVevo.ChangeFind(iteS.Key()).ChangeFind(iteP.Key());

      TopTools_ListIteratorOfListOfShape itl;
      for (itl.Initialize(GenShapes); itl.More(); itl.Next()) {
	itl.Value().Move(LS);
      }

      if (!myMap.IsBound(InitialSpine)) {
	myMap.Bind(InitialSpine,EmptyMap);
      }
     
      if (!myMap(InitialSpine).IsBound(InitialProf)) {
	myMap(InitialSpine).Bind(InitialProf,EmptyList);
      }
      myMap(InitialSpine)(InitialProf).Append(GenShapes);
    }
  }
  //--------------------------------------------------------------
  // Transfert de Top et Bottom de Vevo dans myTop et myBottom.
  //--------------------------------------------------------------
  myTop    = Vevo.Top()   ; myTop.Move(LS);
  myBottom = Vevo.Bottom(); myBottom.Move(LS);
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean BRepFill_Evolved::IsDone() const 
{
  return myIsDone;
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

const TopoDS_Shape& BRepFill_Evolved::Shape() const 
{
  return myShape;
}

//=======================================================================
//function : JoinType
//purpose  : 
//=======================================================================

GeomAbs_JoinType BRepFill_Evolved::JoinType() const 
{
  return myJoinType;
}

//=======================================================================
//function : AddTopAndBottom
//purpose  : 
//=======================================================================

void BRepFill_Evolved::AddTopAndBottom(BRepTools_Quilt& Glue)
{  
//  recuperation premier et dernier vertex du profil.
  TopoDS_Vertex V[2];
  TopExp::Vertices (myProfile,V[0],V[1]);
  if (V[0].IsSame(V[1])) return;

  TopTools_ListIteratorOfListOfShape itL;
  Standard_Boolean ToReverse=Standard_False;
  for (Standard_Integer i = 0; i<=1; i++) {
    
    BRepAlgo_Loop Loop;
    // Construction des supports.
    gp_Pln S (0.,0.,1.,- Altitud(V[i]));
    TopoDS_Face F = BRepLib_MakeFace(S);
    Loop.Init(F);

    TopExp_Explorer     ExpSpine(mySpine,TopAbs_EDGE);
    TopTools_MapOfShape View;

    for (; ExpSpine.More(); ExpSpine.Next()) {
      const TopoDS_Edge& ES = TopoDS::Edge(ExpSpine.Current());
      const TopTools_ListOfShape& L = GeneratedShapes(ES,V[i]);
      Standard_Boolean ComputeOrientation = 0;

      for (itL.Initialize(L); itL.More(); itL.Next()) {
	const TopoDS_Edge& E = TopoDS::Edge(itL.Value());
	
	if (!ComputeOrientation) {
	  BRepAdaptor_Curve C1(ES);
	  BRepAdaptor_Curve C2(E);
	  Standard_Real f,l,fs,ls;
	  BRep_Tool::Range(E ,f ,l);
	  BRep_Tool::Range(ES,fs,ls);
	  Standard_Real u  = 0.3*f  + 0.7*l;
	  Standard_Real us = 0.3*fs + 0.7*ls;
	  gp_Pnt P;
	  gp_Vec V1,V2;
	  C1.D1(us,P,V1); C2.D1(u,P,V2);
	  ToReverse = (V1.Dot(V2) < 0.);
	  ComputeOrientation = 1;
	}
	
	TopAbs_Orientation Or = ES.Orientation();
	if (ToReverse) Or = TopAbs::Reverse(Or);
	TopoDS_Shape aLocalShape = E.Oriented(Or);
	Loop.AddConstEdge(TopoDS::Edge(aLocalShape));
//	Loop.AddConstEdge(TopoDS::Edge(E.Oriented(Or)));
      }
    }

    gp_Pnt PV = BRep_Tool::Pnt(V[i]);
    Standard_Boolean IsOut = PV.Y() < 0;
    
    for (ExpSpine.Init(mySpine,TopAbs_VERTEX); ExpSpine.More(); ExpSpine.Next()) {
      const TopoDS_Vertex& ES = TopoDS::Vertex(ExpSpine.Current());
      if (View.Add(ES)) {
	const TopTools_ListOfShape& L = GeneratedShapes(ES,V[i]);
	for (itL.Initialize(L); itL.More(); itL.Next()) {
	  const TopoDS_Edge& E = TopoDS::Edge(itL.Value());
	  if (!BRep_Tool::Degenerated(E)){
	    // le centre du cercle (ie le vertex) est IN le bouchon si vertex IsOut
	    //                                        OUT                     !IsOut
	    BRepAdaptor_Curve C(E);
	    Standard_Real f,l;
	    BRep_Tool::Range(E,f,l);
	    Standard_Real u = 0.3*f + 0.7*l;
	    gp_Pnt P = BRep_Tool::Pnt(ES);
	    gp_Pnt PC;
	    gp_Vec VC;
	    C.D1(u,PC,VC);
	    gp_Vec PPC(P,PC);
	    gp_Vec Prod = PPC.Crossed(VC);
	    if (IsOut) {
	      ToReverse = Prod.Z() < 0.;
	    }
	    else { 
	      ToReverse = Prod.Z() > 0.;
	    }
	    TopAbs_Orientation Or = TopAbs_FORWARD;
	    if (ToReverse)     Or = TopAbs_REVERSED;
	    TopoDS_Shape aLocalShape = E.Oriented(Or);
	    Loop.AddConstEdge(TopoDS::Edge(aLocalShape));
//	    Loop.AddConstEdge(TopoDS::Edge(E.Oriented(Or)));
	  }
	}
      }
    }
    
    Loop.Perform();
    Loop.WiresToFaces();
    const TopTools_ListOfShape& L = Loop.NewFaces();
    TopTools_ListIteratorOfListOfShape itL(L);
    
    // Maj de myTop et myBottom pour l historique
    // et addition des faces construites.
    TopoDS_Compound  Bouchon;
    BRep_Builder     B;
    B.MakeCompound(Bouchon);
    Standard_Integer j = 0;

    for (itL.Initialize(L); itL.More(); itL.Next()) {
      j++;
      Glue.Add(itL.Value());
      if (j ==1 && i == 0) myTop    = itL.Value();
      if (j ==1 && i == 1) myBottom = itL.Value();
      B.Add(Bouchon,itL.Value());
    }
    if (i == 0 && j > 1) myTop    = Bouchon;
    if (i == 1 && j > 1) myBottom = Bouchon;
  }
}

//================================================================== =====
//function : MakePipe
//purpose  : 
//=======================================================================

void BRepFill_Evolved::MakeSolid()
{

  TopExp_Explorer  exp(myShape,TopAbs_SHELL);
  Standard_Integer ish=0;
  TopoDS_Compound  Res;
  TopoDS_Solid     Sol;
  BRep_Builder     B;
  B.MakeCompound(Res);

  for (; exp.More(); exp.Next()) {
    TopoDS_Shape Sh = exp.Current();
    B.MakeSolid(Sol);
    B.Add(Sol,Sh);
    BRepClass3d_SolidClassifier SC(Sol);
    SC.PerformInfinitePoint(BRepFill_Confusion());
    if (SC.State() == TopAbs_IN) {
      B.MakeSolid(Sol);
      B.Add(Sol,Sh.Reversed());
    }
    B.Add(Res,Sol);
    ish++;
  }
  if (ish == 1) { myShape = Sol;}
  else          { myShape = Res;}

}

//=======================================================================
//function : MakePipe
//purpose  : 
//=======================================================================

void BRepFill_Evolved::MakePipe(const TopoDS_Edge& SE,
				const gp_Ax3&      AxeRef)
{  
  BRepTools_WireExplorer ProfExp;
  TopExp_Explorer        FaceExp;

  gp_Trsf trsf;
  if (Side(myProfile,BRepFill_Confusion()) > 3) { // side right 
    trsf.SetRotation(gp::OZ(),PI);
  }
  TopLoc_Location DumLoc (trsf);
  TopoDS_Shape aLocalShape = myProfile.Moved(DumLoc);
  TopoDS_Wire DummyProf = 
    PutProfilAt (TopoDS::Wire(aLocalShape),
		 AxeRef,SE,
		 mySpine,Standard_True);
//  TopoDS_Wire DummyProf = 
//    PutProfilAt (TopoDS::Wire(myProfile.Moved(DumLoc)),
//		 AxeRef,SE,
//		 mySpine,Standard_True);

  // Copie du profil pour eviter l accumulation des 
  // locations sur les Edges de myProfile!
 
  Handle(BRepTools_TrsfModification) TrsfMod 
    = new BRepTools_TrsfModification(gp_Trsf());
  BRepTools_Modifier Modif(DummyProf,TrsfMod);

  TopoDS_Wire GenProf = TopoDS::Wire(Modif.ModifiedShape(DummyProf));

#ifdef DRAW
  if (AffichGeom) {
    sprintf(name,"EVOLBASE_%d",++NbFACES);
    DBRep::Set(name,SE);
    sprintf(name,"EVOLPROF_%d",NbFACES);
    DBRep::Set(name,GenProf);
  }
#endif

//  BRepFill_Pipe Pipe(BRepLib_MakeWire(SE),GenProf);	
  BRepFill_Pipe Pipe = BRepFill_Pipe(BRepLib_MakeWire(SE),GenProf);	
  
#ifdef DRAW
  if (AffichGeom) {
    sprintf(name,"EVOL_%d",++NbFACES);
    DBRep::Set(name,Pipe.Shape());
  }
#endif
  //---------------------------------------------
  // Rangement du Tuyau dans myMap.
  //---------------------------------------------

  BRepTools_WireExplorer GenProfExp;
  TopTools_ListOfShape   L;
  TopoDS_Vertex          VF,VL,VFG,VLG;
  Standard_Boolean       FirstVertex = Standard_True;
  TopTools_DataMapOfShapeListOfShape  P;
  
  myMap.Bind(SE,P);

  for (ProfExp.Init(myProfile),GenProfExp.Init(GenProf);
       ProfExp.More();
       ProfExp.Next(),GenProfExp.Next()) {

    EdgeVertices(ProfExp   .Current(),VF ,VL);
    EdgeVertices(GenProfExp.Current(),VFG,VLG);
    
    if (FirstVertex) {
      myMap(SE).Bind(VF,L);
      myMap(SE)(VF).Append(Pipe.Edge(SE,VFG)); 
      FirstVertex = Standard_False;
    }
    myMap(SE).Bind(VL,L);
    myMap(SE)(VL).Append(Pipe.Edge(SE,VLG));
    myMap(SE).Bind(ProfExp.Current(),L);
    myMap(SE)(ProfExp.Current()).Append
      (Pipe.Face(SE,GenProfExp.Current()));
  }
}


//=======================================================================
//function : MakeRevol
//purpose  : 
//=======================================================================

void BRepFill_Evolved::MakeRevol(const TopoDS_Edge&   SE,
				 const TopoDS_Vertex& VLast,
				 const gp_Ax3&        AxeRef)
{  
  BRepTools_WireExplorer ProfExp;
  TopExp_Explorer        FaceExp;

  gp_Trsf trsf;
  if (Side(myProfile,BRepFill_Confusion()) > 3) { // side right 
    trsf.SetRotation(gp::OZ(),PI);
  }
  TopLoc_Location DumLoc (trsf);
  TopoDS_Shape aLocalShape = myProfile.Moved(DumLoc);
  TopoDS_Wire GenProf = 
    PutProfilAt (TopoDS::Wire(aLocalShape),
		 AxeRef,SE,
		 mySpine,Standard_False);
//  TopoDS_Wire GenProf = 
//    PutProfilAt (TopoDS::Wire(myProfile.Moved(DumLoc)),
//		 AxeRef,SE,
//		 mySpine,Standard_False);

  gp_Ax1 AxeRev( BRep_Tool::Pnt(VLast), -gp::DZ());  

  // Positionnement de la couture sur l edge du spine
  // pour que les bissectrices ne traversent pas les coutures.
  gp_Trsf dummy;
  dummy.SetRotation(AxeRev, 1.5*PI);
  TopLoc_Location DummyLoc(dummy);
  GenProf.Move(DummyLoc);
  
  BRepSweep_Revol Rev(GenProf,AxeRev,Standard_True);
  
#ifdef DRAW  
  if (AffichGeom) {
    sprintf(name,"EVOLBASE_%d",++NbFACES);
    char* Temp = name ;
    DrawTrSurf::Set(Temp,new Geom_Line(AxeRev));
//    DrawTrSurf::Set(name,new Geom_Line(AxeRev));
    sprintf(name,"EVOLPROF_%d",NbFACES);
    DBRep::Set(name,GenProf);
    
    sprintf(name,"EVOL_%d",NbFACES);
    DBRep::Set(name,Rev.Shape());
  }
#endif
  //--------------------------------------------
  // Rangement du revol dans myMap.
  //---------------------------------------------
  BRepTools_WireExplorer GenProfExp;
  TopTools_ListOfShape   L;
  TopoDS_Vertex          VF,VL,VFG,VLG;
  Standard_Boolean       FirstVertex = Standard_True;
  TopTools_DataMapOfShapeListOfShape  R;
  
  myMap.Bind(VLast,R);
  
  for (ProfExp.Init(myProfile),GenProfExp.Init(GenProf);
       ProfExp.More();
       ProfExp.Next(),GenProfExp.Next()) {
    
    EdgeVertices(ProfExp   .Current(),VF ,VL);
    EdgeVertices(GenProfExp.Current(),VFG,VLG);
    
    TopAbs_Orientation Or = GenProfExp.Current().Orientation();

    if (FirstVertex) {
      myMap(VLast).Bind(VF,L);
      const TopoDS_Shape& RV = Rev.Shape(VFG);
//      TopAbs_Orientation OO = TopAbs::Compose(RV.Orientation(),Or);
      TopAbs_Orientation OO = RV.Orientation();
      myMap(VLast)(VF).Append(RV.Oriented(OO));
      FirstVertex = Standard_False;
    }
    myMap(VLast).Bind(ProfExp.Current(),L);	
    const TopoDS_Shape& RF = Rev.Shape(GenProfExp.Current());
    TopAbs_Orientation  OO = TopAbs::Compose(RF.Orientation(),Or);

    myMap(VLast)(ProfExp.Current()).Append(RF.Oriented(OO));
    myMap(VLast).Bind(VL,L);
    const TopoDS_Shape& RV = Rev.Shape(VLG);
//    OO = TopAbs::Compose(RV.Orientation(),Or);
    OO = RV.Orientation();
    myMap(VLast)(VL).Append(RV.Oriented(OO));
  }
}

//=======================================================================
//function : FindLocation
//purpose  : 
//=======================================================================

TopLoc_Location BRepFill_Evolved::FindLocation(const TopoDS_Face& Face)
const 
{
  TopLoc_Location L;
  Handle(Geom_Surface) S;
  S = BRep_Tool::Surface(Face, L);
  
  if ( !S->IsKind(STANDARD_TYPE(Geom_Plane))) {
    BRepLib_FindSurface FS( Face, -1, Standard_True);
    if ( FS.Found()) {
      S = FS.Surface();
      L = FS.Location();
    }
    else
      Standard_NoSuchObject::Raise
	("BRepFill_Evolved : The Face is not planar");
  }
  
  if (!L.IsIdentity())
    S = Handle(Geom_Surface)::DownCast(S->Transformed(L.Transformation()));

  Handle(Geom_Plane) P = Handle(Geom_Plane)::DownCast(S);
  gp_Ax3 Axis = P->Position();

  gp_Trsf T;
  gp_Ax3 AxeRef(gp_Pnt(0.,0.,0.),
		gp_Dir(0.,0.,1.),
		gp_Dir(1.,0.,0.));  
  T.SetTransformation(AxeRef,Axis);

  return TopLoc_Location(T);
}


//=======================================================================
//function : TransformInitWork
//purpose  : 
//=======================================================================

void BRepFill_Evolved::TransformInitWork(const TopLoc_Location& LS,
					 const TopLoc_Location& LP)
{
  mySpine.Move  (LS);
  myProfile.Move(LP);

#ifdef DRAW
  if (AffichEdge) {
    sprintf(name,"movedspine");
    TopoDS_Face SL = mySpine;
    DBRep::Set(name,SL);    
    sprintf(name,"movedprofile");
    TopoDS_Wire PL = myProfile;
    DBRep::Set(name,PL);
  }
#endif  
}


//=======================================================================
//function : ContinuityOnOffsetEdge
//purpose  : Codage des regularites sur les edges paralleles de CutVevo
//           communes aux parties gauches et droites du volevo.
//=======================================================================
void  BRepFill_Evolved::ContinuityOnOffsetEdge (const TopTools_ListOfShape& WorkProf) 
{
  BRepTools_WireExplorer WExp ; 
  BRepFill_DataMapIteratorOfDataMapOfShapeDataMapOfShapeListOfShape iteS;
  TopoDS_Vertex          VF,VL,V;
  TopoDS_Edge            PrecE,CurE,FirstE;
  BRep_Builder           B;


  WExp.Init(myProfile);
  FirstE = WExp.Current();
  PrecE  = FirstE;
  EdgeVertices (FirstE, VF, V);
  if (WExp.More()) WExp.Next();
  
  for (; WExp.More(); WExp.Next()) {
    CurE = WExp.Current();
    V    = WExp.CurrentVertex();
    
    if (DistanceToOZ(V) <= BRepFill_Confusion()) {
      // les regularites sont deja codes sur les edges des volevos elementaires
      Standard_Real     U1 = BRep_Tool::Parameter(V,CurE);
      Standard_Real     U2 = BRep_Tool::Parameter(V,PrecE);
      BRepAdaptor_Curve Curve1(CurE);
      BRepAdaptor_Curve Curve2(PrecE);
      GeomAbs_Shape     Continuity = BRepLProp::Continuity(Curve1,Curve2,U1,U2);
      
      if (Continuity >=1) {
	//-----------------------------------------------------
	//Code continuite pour toutes les edges generes par V.
	//-----------------------------------------------------
	for (iteS.Initialize(myMap); iteS.More(); iteS.Next()) {
	  const TopoDS_Shape& SP = iteS.Key(); 
	  if (myMap (SP).IsBound(V) 
	      && myMap (SP).IsBound(CurE) &&  myMap (SP).IsBound(PrecE)){
	    if (!myMap(SP)(V)    .IsEmpty() &&
		!myMap(SP)(CurE) .IsEmpty() &&
		!myMap(SP)(PrecE).IsEmpty()   ) 
	      B.Continuity (TopoDS::Edge(myMap(SP)(V)    .First()),
			    TopoDS::Face(myMap(SP)(CurE) .First()),
			    TopoDS::Face(myMap(SP)(PrecE).First()),
			    Continuity);
	  }
	}
      }
    }
    PrecE = CurE;
  }
  
  EdgeVertices (PrecE, V, VL);
  
  if (VF.IsSame(VL)) {
    //Profil ferme.
    Standard_Real     U1 = BRep_Tool::Parameter(VF,CurE);
    Standard_Real     U2 = BRep_Tool::Parameter(VF,FirstE);
    BRepAdaptor_Curve Curve1(CurE);
    BRepAdaptor_Curve Curve2(FirstE);
    GeomAbs_Shape     Continuity = BRepLProp::Continuity(Curve1,Curve2,U1,U2);
    
    if (Continuity >=1) {
      //-----------------------------------------------------
      //Code continuite pour toutes les edges generes par V.
      //-----------------------------------------------------
      for (iteS.Initialize(myMap); iteS.More(); iteS.Next()) {
	const TopoDS_Shape& SP = iteS.Key(); 
	if (myMap (SP).IsBound(VF) 
	    && myMap (SP).IsBound(CurE) &&  myMap (SP).IsBound(FirstE)){
	  if (!myMap(SP)(VF)    .IsEmpty() &&
	      !myMap(SP)(CurE)  .IsEmpty() &&
	      !myMap(SP)(FirstE).IsEmpty()   ) 
	    B.Continuity (TopoDS::Edge(myMap(SP)(VF)    .First()),
			  TopoDS::Face(myMap(SP)(CurE)  .First()),
			  TopoDS::Face(myMap(SP)(FirstE).First()),
			  Continuity);
	}
      }
    }
  }
}
    
//=======================================================================
//function : AddDegeneratedEdge
//purpose  : il peut manquer des edges degeneres dans certaine face
//           les edges degeneres manquantes ont des vertex correspondant 
//           aux node de la carte.
//           Aujourd hui on se contente de comparer les points UV des vertex
//           sur les edges a une certaine tolerance.
//=======================================================================

static void AddDegeneratedEdge(TopoDS_Face& F,
			       TopoDS_Wire& W) 
{
  TopLoc_Location      L;
  Handle(Geom_Surface) S = BRep_Tool::Surface(F,L);
  if (S->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
    Handle(Geom_Surface) SB = Handle(Geom_RectangularTrimmedSurface)::DownCast(S)->BasisSurface();
    if (SB->DynamicType() == STANDARD_TYPE(Geom_Plane)) {
      return;
    }
  }
  
  if (S->DynamicType() == STANDARD_TYPE(Geom_Plane)) {
    return;
  }
  

  BRep_Builder         B;
  Standard_Real        TolConf = 1.e-4;

  Standard_Boolean Change = Standard_True;
  
  while (Change) {
    Change = Standard_False;
    BRepTools_WireExplorer WE(W,F);
    gp_Pnt2d PF,PrevP,P1,P2;
    TopoDS_Vertex VF,V1,V2;
    
    for (; WE.More(); WE.Next()) {
      const TopoDS_Edge& CE = WE.Current();
      EdgeVertices (CE,V1,V2);
      if (CE.Orientation() == TopAbs_REVERSED)
	BRep_Tool::UVPoints(CE, F, P2, P1);
      else 
	BRep_Tool::UVPoints(CE, F, P1, P2);
      if (VF.IsNull()) {
	VF = V1;
	PF = P1;
      }
      else {
	if (!P1.IsEqual(PrevP,TolConf)) {
	  // edge degenere a inserer.
	  Change = Standard_True;
	  gp_Vec2d V(PrevP,P1);
	  Handle(Geom2d_Line) C2d = new Geom2d_Line(PrevP,gp_Dir2d(V));
	  Standard_Real f = 0, l = PrevP.Distance(P1);
	  Handle(Geom2d_TrimmedCurve) CT = new Geom2d_TrimmedCurve(C2d,f,l);
	  TopoDS_Edge NE = BRepLib_MakeEdge(C2d,S);
	  B.Degenerated(NE,Standard_True);
	  B.Add(NE,V1.Oriented(TopAbs_FORWARD));
	  B.Add(NE,V1.Oriented(TopAbs_REVERSED));
	  B.Range(NE,f,l);
	  B.Add(W,NE);
	  break;
	}
      }
      PrevP = P2;
    }
    if (!Change && VF.IsSame(V2)) {  // ferme
      if (!PF.IsEqual(P2,TolConf)) {
	// edge degenere a inserer.
	Change = Standard_True;
	gp_Vec2d V(P2,PF);
	Handle(Geom2d_Line) C2d = new Geom2d_Line(P2,gp_Dir2d(V));
	Standard_Real f = 0, l = P2.Distance(PF);
	Handle(Geom2d_TrimmedCurve) CT = new Geom2d_TrimmedCurve(C2d,f,l);
	TopoDS_Edge NE = BRepLib_MakeEdge(C2d,S);
	B.Degenerated(NE,Standard_True);
	B.Add(NE,VF.Oriented(TopAbs_FORWARD));
	B.Add(NE,VF.Oriented(TopAbs_REVERSED));
	B.Range(NE,f,l);
	B.Add(W,NE);
      }
    }
  }
}
  
//=======================================================================
//function : TrimFace
//purpose  : 
//=======================================================================

void TrimFace(const TopoDS_Face&              Face,
	            TopTools_SequenceOfShape& TheEdges,
	            TopTools_SequenceOfShape& S)
{

#ifdef DRAW
  Standard_Integer NB = TheEdges.Length();
  if ( AffichEdge) {
    char name[100];
    cout << " TrimFace " << ++NbTRIMFACES;
    cout << " : " << NB << " edges dans la restriction" << endl;
    for ( Standard_Integer j = 1; j <= NB; j++) {
      sprintf(name,"TRIMEDGE_%d_%d",NbTRIMFACES,j);
      DBRep::Set(name,TopoDS::Edge(TheEdges.Value(j)));
    }
  }
#endif


  //--------------------------------------
  // Creation des wires limitant les faces.
  //--------------------------------------
  BRep_Builder             TheBuilder;

  Standard_Integer NbEdges;
  Standard_Boolean NewWire  = Standard_True;
  Standard_Boolean AddEdge  = Standard_False;
  TopoDS_Wire      GoodWire;


  while ( !TheEdges.IsEmpty()) {

    BRepLib_MakeWire MWire(TopoDS::Edge(TheEdges.First()));
    GoodWire = MWire.Wire();
    TheEdges.Remove(1);
    NbEdges = TheEdges.Length();
    NewWire = Standard_False;

    while (!NewWire) {
      AddEdge  = Standard_False;

      for ( Standard_Integer i = 1; i <= NbEdges && !AddEdge; i++) {
	const TopoDS_Edge& E = TopoDS::Edge(TheEdges.Value(i));
	if ( BRep_Tool::Degenerated(E)) {
	    TheEdges.Remove(i);
	    AddEdge = Standard_True;
	    NbEdges = TheEdges.Length();
	    GoodWire = MWire.Wire();
	}
	else {
	  MWire.Add(E);
	  if ( MWire.Error() == BRepLib_WireDone) {
	    // on a reussi la connection 
	    // on l`enleve dans la sequence et on recommence au debut.
	    TheEdges.Remove(i);
	    AddEdge = Standard_True;
	    NbEdges = TheEdges.Length();
	    GoodWire = MWire.Wire();
	  }
	}
      }
      NewWire = (!AddEdge);
    }
    TopoDS_Shape aLocalShape = Face.EmptyCopied();
    TopoDS_Face FaceCut = TopoDS::Face(aLocalShape);
//    TopoDS_Face FaceCut = TopoDS::Face(Face.EmptyCopied());
    FaceCut.Orientation(TopAbs_FORWARD);
    BRepTools::Update  (FaceCut);
    AddDegeneratedEdge (FaceCut,GoodWire);
    TheBuilder.Add     (FaceCut,GoodWire);
    FaceCut.Orientation(Face.Orientation());
    S.Append(FaceCut);
  }
}



//=======================================================================
//function : PutProfilAt
//purpose  : 
//=======================================================================

const TopoDS_Wire PutProfilAt (const TopoDS_Wire&     ProfRef,
			       const gp_Ax3&          AxeRef, 
			       const TopoDS_Edge&     E,
			       const TopoDS_Face&     F,
			       const Standard_Boolean AtStart)
{	
  gp_Vec2d             D1;
  gp_Pnt2d             P;
  TopoDS_Wire          Prof;
  Handle(Geom2d_Curve) C2d;
  Standard_Real        First,Last;

  C2d = BRep_Tool::CurveOnSurface(E,F,First,Last);
  if (C2d.IsNull()) {
    Standard_ConstructionError::Raise("ConstructionError in PutProfilAt"); 
  }

  if (E.Orientation() == TopAbs_REVERSED) {
   if (!AtStart) C2d->D1(First,P,D1);else C2d->D1(Last,P,D1);
   D1.Reverse();
  }
  else {
   if (!AtStart) C2d->D1(Last,P,D1) ;else C2d->D1(First,P,D1); 
  }
  gp_Pnt P3d(P.X() ,P.Y() ,0.);
  gp_Vec V3d(D1.X(),D1.Y(),0.);

  gp_Ax3  Ax( P3d, gp::DZ(), V3d);
  gp_Trsf Trans;
  Trans.SetTransformation(Ax,AxeRef);
  TopoDS_Shape aLocalShape = ProfRef.Moved(TopLoc_Location(Trans));
  Prof = TopoDS::Wire(aLocalShape);
//  Prof = TopoDS::Wire(ProfRef.Moved(TopLoc_Location(Trans)));
  return Prof;
}

      
//=======================================================================
//function : TrimEdge
//purpose  : 
//=======================================================================

void TrimEdge (const TopoDS_Edge&        Edge,
	       const TopTools_SequenceOfShape& TheEdgesControle,
	             TopTools_SequenceOfShape& TheVer,
	             TColStd_SequenceOfReal&   ThePar,
	             TopTools_SequenceOfShape& S)
{
  Standard_Boolean         Change = Standard_True;
  BRep_Builder             TheBuilder;
  S.Clear();
  //-----------------------------------------------------------
  // Tri des deux sequence en fonction du parametre sur l edge.
  //-----------------------------------------------------------
  while (Change) {
    Change = Standard_False;
    for (Standard_Integer i = 1; i < ThePar.Length(); i++) {
      if (ThePar.Value(i) > ThePar.Value(i+1)) {
	ThePar.Exchange(i,i+1);
	TheVer.Exchange(i,i+1);
	Change = Standard_True;
      }
    }
  }

  //----------------------------------------------------------
  // Si un vertex n est pas dans le detrompeur il est elimine.
  //----------------------------------------------------------
  if (!BRep_Tool::Degenerated(Edge)) {
    for (Standard_Integer k = 1; k <= TheVer.Length(); k ++) {
      if ( DoubleOrNotInFace (TheEdgesControle,
			      TopoDS::Vertex(TheVer.Value(k)))) {
	TheVer.Remove(k);
	ThePar.Remove(k);
	k--;
      }
    }
  }

  //-------------------------------------------------------------------
  // Traitement des vertex doubles pour les edges non degeneres.
  // Si un vertex_double apparait deux fois dans les edges de contole
  // le vertex est elimine .
  // sinon on garde une seule de ces representations.
  //-------------------------------------------------------------------
  if (!BRep_Tool::Degenerated(Edge)) {
    for (Standard_Integer k = 1; k < TheVer.Length(); k ++) {
      if (TheVer.Value(k).IsSame(TheVer.Value(k+1))) {
	TheVer.Remove(k+1);
	ThePar.Remove(k+1);
	if ( DoubleOrNotInFace (TheEdgesControle,
				TopoDS::Vertex(TheVer.Value(k)))) {
	  TheVer.Remove(k);
	  ThePar.Remove(k);
//	  k--;
	}
	k--;
      }
    }
  }

  //-----------------------------------------------------------
  // Creation des edges.
  // le nombre de vertex doit etre pair les edges a creer vont 
  // d un vertex d indice impair i au vertex i+1;
  //-----------------------------------------------------------
  for (Standard_Integer k = 1; k < TheVer.Length(); k = k+2) {
    TopoDS_Shape aLocalShape = Edge.EmptyCopied();
    TopoDS_Edge NewEdge = TopoDS::Edge(aLocalShape);
//    TopoDS_Edge NewEdge = TopoDS::Edge(Edge.EmptyCopied());

    if (NewEdge.Orientation() == TopAbs_REVERSED) {
      TheBuilder.Add  (NewEdge,TheVer.Value(k)  .Oriented(TopAbs_REVERSED));
      TheBuilder.Add  (NewEdge,TheVer.Value(k+1).Oriented(TopAbs_FORWARD));
    }
    else {      
      TheBuilder.Add  (NewEdge,TheVer.Value(k)  .Oriented(TopAbs_FORWARD));
      TheBuilder.Add  (NewEdge,TheVer.Value(k+1).Oriented(TopAbs_REVERSED));
    }
    TheBuilder.Range(NewEdge,ThePar.Value(k),ThePar.Value(k+1));
//  modified by NIZHNY-EAP Wed Dec 22 12:09:48 1999 ___BEGIN___
    BRepLib::UpdateTolerances(NewEdge,Standard_False);
//  modified by NIZHNY-EAP Wed Dec 22 13:34:19 1999 ___END___
    S.Append(NewEdge);
  }
} 

//=======================================================================
//function : ComputeIntervals
//purpose  : 
//=======================================================================

void ComputeIntervals (const TopTools_SequenceOfShape& VOnF,
		       const TopTools_SequenceOfShape& VOnL,
		       const TColgp_SequenceOfPnt&     ParOnF,
		       const TColgp_SequenceOfPnt&     ParOnL,
		       const BRepFill_TrimSurfaceTool& Trim,
		       const Handle(Geom2d_Curve)&     Bis,
		       const TopoDS_Vertex&            VS,
		       const TopoDS_Vertex&            VE,
		             TColStd_SequenceOfReal&   FirstPar,
		             TColStd_SequenceOfReal&   LastPar,
		             TopTools_SequenceOfShape& FirstV,
		             TopTools_SequenceOfShape& LastV )
{
  Standard_Integer IOnF    = 1,IOnL = 1;
  Standard_Real    U1,U2;
  TopoDS_Shape     V1,V2;
  
  if (!VS.IsNull()) {
    U1  = Bis->FirstParameter();
    V1  = VS;
  }
  while ( IOnF <= VOnF.Length() || IOnL <= VOnL.Length()) {
    //---------------------------------------------------------
    // Recuperation du plus petit parametre sur la bissectrice
    // par rapport aux positions courrantes IOnF,IOnL.
    //---------------------------------------------------------
    if ( IOnL > VOnL.Length() ||
	(IOnF <= VOnF.Length() &&
	 ParOnF.Value(IOnF).X() < ParOnL.Value(IOnL).X())) {
	
      U2 = ParOnF.Value(IOnF).X();
      V2 = VOnF  .Value(IOnF);
      IOnF++;
    }     
    else{
      U2 = ParOnL.Value(IOnL).X();
      V2 = VOnL  .Value(IOnL);
      IOnL++;
    }
    //---------------------------------------------------------------------
    // Quand V2 et V1 sont differents on teste le point milieu P de 
    // l intervalle par rapport a la face. Si P est dans la face l interval
    // est valide.
    //--------------------------------------------------------------------- 
    if (!V1.IsNull() && !V2.IsSame(V1)) {
      gp_Pnt2d P = Bis->Value((U2 + U1)*0.5);
      if (Trim.IsOnFace(P)) {
	FirstPar.Append(U1); LastPar .Append(U2);
	FirstV.  Append(V1); LastV   .Append(V2);
      }
    }
    U1 = U2;
    V1 = V2;
  }  

  if (!VE.IsNull()) {
    U2 = Bis->LastParameter();
    V2 = VE;
    if (!V2.IsSame(V1)) {
      gp_Pnt2d P = Bis->Value((U2 + U1)*0.5);
      if (Trim.IsOnFace(P)) {
	FirstPar.Append(U1); LastPar .Append(U2);
	FirstV.Append  (V1);   LastV .Append(V2);
      }
    }
  }
  
}

//=======================================================================
//function : Relative
//purpose  : Commun est vrai si les deux wires ont V en commun
//           return FORWARD si les wires au voisinage du vertex sont
//           du meme cote. REVERSED sinon.
//=======================================================================
static TopAbs_Orientation Relative (const TopoDS_Wire&   W1,
				    const TopoDS_Wire&   W2,
				    const TopoDS_Vertex& V,
				    Standard_Boolean&    Commun)
{
  TopExp_Explorer Exp;
  TopoDS_Edge     E1,E2;
  TopoDS_Vertex   V1,V2;

  for (Exp.Init(W1,TopAbs_EDGE); Exp.More(); Exp.Next()) {
    const TopoDS_Edge& E = TopoDS::Edge(Exp.Current());
    TopExp::Vertices(E,V1,V2);
    if (V1.IsSame(V) || V2.IsSame(V)) {
      E1 = E;
      break;
    }
  }
  for (Exp.Init(W2,TopAbs_EDGE); Exp.More(); Exp.Next()) {
    const TopoDS_Edge& E = TopoDS::Edge(Exp.Current());
    TopExp::Vertices(E,V1,V2);
    if (V1.IsSame(V) || V2.IsSame(V)) {
      E2 = E;
      break;
    }
  }

  if (E1.IsNull() || E2.IsNull()) {
    Commun = Standard_False;
    return TopAbs_FORWARD;
  }
  Commun = Standard_True;

  TopoDS_Wire WW1 = BRepLib_MakeWire(E1);
  TopoDS_Wire WW2 = BRepLib_MakeWire(E2);
  Standard_Real Tol = BRepFill_Confusion();
  if (Side(WW1,Tol) < 4 && Side(WW2,Tol) < 4) // les deux a gauche
    return TopAbs_FORWARD;
  if (Side(WW1,Tol) > 4 && Side(WW2,Tol) > 4) // les deux a droite
    return TopAbs_FORWARD;
  
  return TopAbs_REVERSED;
}
//=======================================================================
//function : OriEdgeInFace
//purpose  : 
//=======================================================================

TopAbs_Orientation OriEdgeInFace (const TopoDS_Edge& E,
				  const TopoDS_Face& F )

{
#ifdef DEB
  TopAbs_Orientation O = 
#endif
    F.Orientation();

  TopExp_Explorer Exp(F.Oriented(TopAbs_FORWARD),TopAbs_EDGE);

  for (; Exp.More() ;Exp.Next()) {
    if (Exp.Current().IsSame(E)) {
      return Exp.Current().Orientation();
    }
  }
  Standard_ConstructionError::Raise("BRepFill_Evolved::OriEdgeInFace");
  return E.Orientation();
}



//=======================================================================
//function : IsOnFace
//purpose  : Retourne la position du point defini par d1
//           dans la face defini Par d2 d3.
//           
//           0 : le point est en dehors de la face.
//           1 : le point est sur ledge correspondant a d2.
//           2 : le point est a l interieur de la face.
//           3 : le point est sur ledge correspondant a d3.
//=======================================================================

Standard_Integer  PosOnFace (Standard_Real d1,
			     Standard_Real d2,
			     Standard_Real d3)
{
  if (Abs(d1 - d2) <= BRepFill_Confusion())
    return 1;
  if (Abs(d1 - d3) <= BRepFill_Confusion())
    return 3;

  if (d2 < d3) { 
    if (d1 > (d2 + BRepFill_Confusion()) && 
	d1 < (d3 - BRepFill_Confusion())    ) 
      return 2;
  }
  else {
    if (d1 > (d3 + BRepFill_Confusion()) && 
	d1 < (d2 - BRepFill_Confusion())   ) 
      return 2;
  }
  return 0;
}

//=======================================================================
//function : DoubleOrNotInFace
//purpose  : Return True if V apparait 0 ou  deux fois dans la sequence
//           d edges EC 
//=======================================================================

Standard_Boolean DoubleOrNotInFace(const TopTools_SequenceOfShape& EC,
				   const TopoDS_Vertex&            V)
{
  Standard_Boolean Vu = Standard_False;

  for (Standard_Integer i = 1; i <= EC.Length(); i++) {
    TopoDS_Vertex V1,V2;
    TopExp::Vertices(TopoDS::Edge(EC.Value(i)),V1,V2);
    if (V1.IsSame(V)) {
      if  (Vu) return Standard_True;
      else       Vu = Standard_True;
    }
    if (V2.IsSame(V)) {
      if  (Vu) return Standard_True;
      else       Vu = Standard_True;
    }
  }
  if (Vu) return Standard_False;
  else    return Standard_True;   
}


//=======================================================================
//function : DistanceToOZ
//purpose  : 
//=======================================================================

Standard_Real DistanceToOZ (const TopoDS_Vertex& V) 
{
  gp_Pnt PV3d = BRep_Tool::Pnt(V);
  return Abs(PV3d.Y()); 
}

//=======================================================================
//function : Altitud
//purpose  : 
//=======================================================================

Standard_Real Altitud (const TopoDS_Vertex& V) 
{
  gp_Pnt PV3d = BRep_Tool::Pnt(V);
  return PV3d.Z(); 
}

//=======================================================================
//function : SimpleExpression 
//purpose  : 
//=======================================================================

void SimpleExpression (const Bisector_Bisec&  B, 
		       Handle(Geom2d_Curve)&  Bis)
{
  Bis = B.Value();

  Handle(Standard_Type) BT = Bis->DynamicType();
  if (BT == STANDARD_TYPE(Geom2d_TrimmedCurve)) {
    Handle(Geom2d_TrimmedCurve) TrBis 
      = Handle(Geom2d_TrimmedCurve)::DownCast(Bis);
    Handle(Geom2d_Curve) BasBis = TrBis->BasisCurve();
    BT = BasBis->DynamicType();
    if (BT == STANDARD_TYPE(Bisector_BisecAna)) {
      Bis = Handle(Bisector_BisecAna)::DownCast(BasBis)->Geom2dCurve();
      Bis = new Geom2d_TrimmedCurve (Bis,
				     TrBis->FirstParameter(),
				     TrBis->LastParameter());
    }
  }
}

//=======================================================================
//function : CutEdgeProf
//purpose  : Projection et Decoupe d une edge aux extrema de distance a 
//           l axe OZ.
//=======================================================================

void CutEdgeProf (const TopoDS_Edge&                  E,
		  const Handle(Geom_Plane)&           Plane,
		  const Handle(Geom2d_Line)&          Line,
		        TopTools_ListOfShape&         Cuts,
		        TopTools_DataMapOfShapeShape& MapVerRefMoved)
{ 
  Cuts.Clear();

  Standard_Real             f,l;
  Handle(Geom_Curve)        C;
  Handle(Geom_TrimmedCurve) CT;
  Handle(Geom2d_Curve)      C2d;
  TopLoc_Location           L;

  // On recupere la courbe associee a chaque Edge
  C  = BRep_Tool::Curve(E,L,f,l);
  CT = new Geom_TrimmedCurve(C,f,l);
  CT->Transform(L.Transformation());
  
  // on la projete dans le plan et on recupere la PCurve associee
  gp_Dir Normal = Plane->Pln().Axis().Direction();
  C = 
    Handle(Geom_Curve)::DownCast(GeomProjLib::ProjectOnPlane(CT,Plane,
							       Normal,
							       Standard_False));
  C2d = GeomProjLib::Curve2d(C,Plane);
  
  // On calcule les extrema avec la droite
  TColStd_SequenceOfReal Seq;
  
  Standard_Real U1 = -Precision::Infinite();
  Standard_Real U2 =  Precision::Infinite();
  f= C2d->FirstParameter();
  l= C2d->LastParameter();

  Bnd_Box2d B;
  Geom2dAdaptor_Curve AC2d(C2d);
  BndLib_Add2dCurve::Add(AC2d,BRepFill_Confusion(),B);
  Standard_Real xmin,xmax;
  B.Get(xmin,U1,xmax,U2);

//  modified by NIZHNY-EAP Wed Feb  2 16:32:37 2000 ___BEGIN___
  // no sense if C2 is normal to Line or really is a point
  if (U1 != U2) {
    Geom2dAPI_ExtremaCurveCurve Extrema(Line,C2d,U1-1.,U2+1.,f,l);
    
    Standard_Integer i, Nb = Extrema.NbExtrema();
    for ( i = 1; i <= Nb; i++) {
      Extrema.Parameters(i,U1,U2);
      Seq.Append(U2);
    }
  }
//  modified by NIZHNY-EAP Wed Feb  2 16:33:05 2000 ___END___
  
  // On calcule les intersection avec Oy.
  Geom2dAdaptor_Curve ALine(Line);
  Standard_Real Tol = Precision::Intersection();
  Standard_Real TolC = 0.;
  
  Geom2dInt_GInter Intersector(ALine,AC2d,TolC,Tol);
  Standard_Integer i, Nb = Intersector.NbPoints();
  
  for ( i = 1; i <= Nb; i++) {
    Seq.Append(Intersector.Point(i).ParamOnSecond());
  }

  // Compute the new edges.
  BRep_Builder Builder;
  TopoDS_Vertex VV,Vf,Vl,VRf,VRl;
  TopExp::Vertices(E,VRf,VRl);

  if (!MapVerRefMoved.IsBound(VRf)) {
    Builder.MakeVertex(Vf,C->Value(f),BRep_Tool::Tolerance(VRf));
    MapVerRefMoved.Bind(VRf,Vf);
  }
  else {
    Vf = TopoDS::Vertex(MapVerRefMoved(VRf));
  }
  
  if (!MapVerRefMoved.IsBound(VRl)) {
    Builder.MakeVertex(Vl,C->Value(l),BRep_Tool::Tolerance(VRl));
    MapVerRefMoved.Bind(VRl,Vl);
  }
  else {
    Vl = TopoDS::Vertex(MapVerRefMoved(VRl));
  }

  if ( !Seq.IsEmpty()) {

    Bubble(Seq);

    Standard_Boolean Empty = Standard_False;

    Standard_Real CurParam = f;
    Standard_Real Param;
    
    while ( !Empty) {
      Param = Seq.First();
      Seq.Remove(1);
      Empty = Seq.IsEmpty();
      if (Abs( Param - CurParam) > BRepFill_Confusion() &&
	  Abs( Param - l)        > BRepFill_Confusion() ) {
	
	VV = BRepLib_MakeVertex( C->Value(Param));
	
	TopoDS_Edge EE = BRepLib_MakeEdge(C,Vf,VV);
	EE.Orientation(E.Orientation());
	if ( EE.Orientation() == TopAbs_FORWARD)
	  Cuts.Append(EE);
	else
	  Cuts.Prepend(EE);
	
	// on reinitialise 
	CurParam = Param;
	Vf = VV;
      }
    }
  }

  TopoDS_Edge EE = BRepLib_MakeEdge(C,Vf,Vl);
  EE.Orientation(E.Orientation());
  if ( EE.Orientation() == TopAbs_FORWARD)
    Cuts.Append(EE);
  else
    Cuts.Prepend(EE);
}

//=======================================================================
//function : CutEdge
//purpose  : Decoupe d une edge aux extrema de courbures et aux points
//           d inflexion.
//           Les cercles fermes sont aussi decoupes en deux.
//           Si <Cuts> est vide l edge n est pas modifie.
//           Le premier et le dernier vertex de l edge originale
//           appartiennent respectivement a la premiere et derniere
//           portions.
//=======================================================================
void CutEdge (const TopoDS_Edge& E, 
	      const TopoDS_Face& F,
	            TopTools_ListOfShape& Cuts)
{
  Cuts.Clear();
  MAT2d_CutCurve              Cuter;
  Standard_Real               f,l; 
  Handle(Geom2d_Curve)        C2d;
  Handle(Geom2d_TrimmedCurve) CT2d;
  
  TopoDS_Vertex V1,V2,VF,VL;
  TopExp::Vertices (E,V1,V2);
  BRep_Builder B;
  
  C2d  = BRep_Tool::CurveOnSurface (E,F,f,l);
  CT2d = new Geom2d_TrimmedCurve(C2d,f,l);

  if (CT2d->BasisCurve()->IsKind(STANDARD_TYPE(Geom2d_Circle)) &&
      E.Closed()) {
    //---------------------------
    // Decoupe cercle ferme.
    //---------------------------
    Standard_Real m1 = (2*f +   l)/3.;
    Standard_Real m2 = (  f + 2*l)/3.;
    gp_Pnt2d P1 = CT2d->Value(m1);
    gp_Pnt2d P2 = CT2d->Value(m2);

    TopoDS_Vertex VL1 = BRepLib_MakeVertex(gp_Pnt(P1.X(), P1.Y(), 0.));
    TopoDS_Vertex VL2 = BRepLib_MakeVertex(gp_Pnt(P2.X(), P2.Y(), 0.));
    TopoDS_Shape aLocalShape1 = E.EmptyCopied();
    TopoDS_Shape aLocalShape2 = E.EmptyCopied();
    TopoDS_Shape aLocalShape3 = E.EmptyCopied();
    TopoDS_Edge FE = TopoDS::Edge(aLocalShape1);    
    TopoDS_Edge ME = TopoDS::Edge(aLocalShape2);    
    TopoDS_Edge LE = TopoDS::Edge(aLocalShape3);
//    TopoDS_Edge FE = TopoDS::Edge(E.EmptyCopied());    
//   TopoDS_Edge ME = TopoDS::Edge(E.EmptyCopied());    
//    TopoDS_Edge LE = TopoDS::Edge(E.EmptyCopied());

    FE.Orientation(TopAbs_FORWARD);
    ME.Orientation(TopAbs_FORWARD);
    LE.Orientation(TopAbs_FORWARD );

    B.Add  (FE,V1);
    B.Add  (FE,VL1.Oriented(TopAbs_REVERSED));
    B.Range(FE, f, m1);    

    B.Add  (ME,VL1.Oriented(TopAbs_FORWARD));
    B.Add  (ME,VL2.Oriented(TopAbs_REVERSED));
    B.Range(ME, m1, m2);    

    B.Add  (LE,VL2.Oriented(TopAbs_FORWARD));
    B.Add  (LE,V2);
    B.Range(LE, m2, l);

    Cuts.Append(FE.Oriented(E.Orientation()));
    Cuts.Append(ME.Oriented(E.Orientation()));
    Cuts.Append(LE.Oriented(E.Orientation()));
    //--------
    // Retour.
    //--------
    return;
  }

  //-------------------------
  // Decoupe de la courbe.
  //-------------------------
  Cuter.Perform(CT2d);

  if (Cuter.UnModified()) {
    //-----------------------------
    // edge non modifiee => retour.
    //-----------------------------
    return;
  }
  else {
    //--------------------------------------
    // Creation des edges decoupees.
    //--------------------------------------
    VF = V1;

    for ( Standard_Integer k = 1; k <= Cuter.NbCurves(); k++) {
      Handle(Geom2d_TrimmedCurve)CC = Cuter.Value(k);
      if (k == Cuter.NbCurves()) {VL = V2;}
      else { 
	gp_Pnt2d P = CC->Value(CC->LastParameter());
	VL = BRepLib_MakeVertex(gp_Pnt(P.X(), P.Y(), 0.));
      }
      TopoDS_Shape aLocalShape = E.EmptyCopied();
      TopoDS_Edge NE = TopoDS::Edge(aLocalShape);
//      TopoDS_Edge NE = TopoDS::Edge(E.EmptyCopied());
      NE.Orientation(TopAbs_FORWARD);
      B.Add  (NE,VF.Oriented(TopAbs_FORWARD));
      B.Add  (NE,VL.Oriented(TopAbs_REVERSED));      
      B.Range(NE,CC->FirstParameter(),CC->LastParameter());
      Cuts.Append(NE.Oriented(E.Orientation()));
      VF = VL;
    }
  }
}

//=======================================================================
//function : VertexFromNode
//purpose  : Test si la position de aNode par rapport aux distance to OZ
//           des vertex VF et VL. retourne Status.
//           si Status est different de 0 Recupere ou cree le vertex 
//           correspondant a aNode.
//=======================================================================

Standard_Integer VertexFromNode
(const Handle(MAT_Node)&                          aNode, 
 const TopoDS_Edge&                                E, 
 const TopoDS_Vertex&                              VF, 
 const TopoDS_Vertex&                              VL,
       BRepFill_DataMapOfNodeDataMapOfShapeShape&  MapNodeVertex,
       TopoDS_Vertex&                              VN)
{      
  TopoDS_Shape                 ShapeOnNode;
  TopTools_DataMapOfShapeShape EmptyMap;
  Standard_Integer             Status = 0;
  BRep_Builder                 B;

  if (!aNode->Infinite()) {
    Status    = PosOnFace(aNode->Distance(),
			  DistanceToOZ(VF) , DistanceToOZ(VL));
  }
  if      (Status == 2) ShapeOnNode = E;
  else if (Status == 1) ShapeOnNode = VF;
  else if (Status == 3) ShapeOnNode = VL;
  
  if (!ShapeOnNode.IsNull()) {
    //------------------------------------------------
    // le vertex correspondra a un noeud de la carte
    //------------------------------------------------
    if (MapNodeVertex.IsBound(aNode) &&
	MapNodeVertex(aNode).IsBound(ShapeOnNode)) {
      VN = TopoDS::Vertex
	(MapNodeVertex(aNode)(ShapeOnNode));
    }
    else { 
      B.MakeVertex (VN);
      if (!MapNodeVertex.IsBound(aNode)) {
	MapNodeVertex.Bind(aNode,EmptyMap);
      }
      MapNodeVertex(aNode).Bind(ShapeOnNode,VN);
    }
  }
  return Status;
}

