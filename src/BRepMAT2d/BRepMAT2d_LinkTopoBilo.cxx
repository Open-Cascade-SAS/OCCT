// File:	BRepMAT2d_LinkTopoBilo.cxx
// Created:	Fri Oct  7 16:50:40 1994
// Author:	Yves FRICAUD
//		<yfr@nonox>


#include <BRepMAT2d_LinkTopoBilo.ixx>
#include <TopAbs.hxx>
#include <TopExp_Explorer.hxx>
#include <TopExp.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx> 
#include <TopTools_SequenceOfShape.hxx>
#include <Standard_Type.hxx>
#include <Geom2d_Geometry.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <TColGeom2d_SequenceOfCurve.hxx>
#include <MAT_Graph.hxx>
#include <MAT_BasicElt.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRepMAT2d_SequenceOfBasicElt.hxx>
#include <BRepMAT2d_DataMapOfShapeSequenceOfBasicElt.hxx>
#include <TColStd_DataMapIteratorOfDataMapOfIntegerInteger.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <Precision.hxx>
#include <gp_Pnt2d.hxx>


//=======================================================================
//function : BRepMAT2d_LinkTopoBilo
//purpose  : 
//=======================================================================

BRepMAT2d_LinkTopoBilo::BRepMAT2d_LinkTopoBilo()
{
}


//=======================================================================
//function : BRepMAT2d_LinkTopoBilo
//purpose  : 
//=======================================================================

BRepMAT2d_LinkTopoBilo::BRepMAT2d_LinkTopoBilo(
   const BRepMAT2d_Explorer&       Explo,
   const BRepMAT2d_BisectingLocus& BiLo)
{
  Perform (Explo,BiLo);
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepMAT2d_LinkTopoBilo::Perform(const BRepMAT2d_Explorer&       Explo, 
				     const BRepMAT2d_BisectingLocus& BiLo)
{
  myMap.Clear();
  myBEShape.Clear();

  TopoDS_Shape     S          = Explo.Shape();
  Standard_Integer IndContour = 1;

  if (S.ShapeType() == TopAbs_FACE) {
    TopExp_Explorer  Exp (S,TopAbs_WIRE);
    
    while (Exp.More()) {
      LinkToWire(TopoDS::Wire (Exp.Current()),Explo,IndContour,BiLo);
      Exp.Next();
      IndContour++;
    }
  }
  else {
    Standard_ConstructionError::Raise("BRepMAT2d_LinkTopoBilo::Perform");
  }
  
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepMAT2d_LinkTopoBilo::Init(const TopoDS_Shape& S)
{
  isEmpty = Standard_False;
  current = 1;
  if (myMap.IsBound(S)) myKey = S; else isEmpty = Standard_True;
}


//=======================================================================
//function : More
//purpose  : 
//=======================================================================

Standard_Boolean  BRepMAT2d_LinkTopoBilo::More()
{
  if (isEmpty) return Standard_False;
  return (current <=  myMap(myKey).Length());
}


//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void BRepMAT2d_LinkTopoBilo::Next()
{
  current++;
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Handle(MAT_BasicElt) BRepMAT2d_LinkTopoBilo::Value() const 
{
  return myMap(myKey).Value(current);
}

//=======================================================================
//function : GeneratingShape
//purpose  : 
//=======================================================================

TopoDS_Shape  BRepMAT2d_LinkTopoBilo::GeneratingShape
(const Handle(MAT_BasicElt)& BE) const
{
  return myBEShape(BE);
}

static void LinkToContour(const BRepMAT2d_Explorer&              Explo,
			  const Standard_Integer                 IndC,
			  const BRepMAT2d_BisectingLocus&        BiLo,
			        TColStd_DataMapOfIntegerInteger& Link);

//=======================================================================
//function : LinkToWire
//purpose  : 
//=======================================================================

void BRepMAT2d_LinkTopoBilo::LinkToWire(const TopoDS_Wire&              W,
					const BRepMAT2d_Explorer&       Explo, 
					const Standard_Integer          IndC,
					const BRepMAT2d_BisectingLocus& BiLo)
{
  BRepTools_WireExplorer       TheExp (W);
  Standard_Integer             KC;
  TopoDS_Vertex                VF,VL;
  TopoDS_Shape                 S;
  Handle(MAT_BasicElt)         BE;
  Handle(Standard_Type)        Type;
  TopTools_SequenceOfShape     TopoSeq;
  BRepMAT2d_SequenceOfBasicElt EmptySeq;

  TColStd_DataMapIteratorOfDataMapOfIntegerInteger Ite;
  TColStd_DataMapOfIntegerInteger LinkBECont;


  for (;TheExp.More();TheExp.Next()) {
    TopoSeq.Append(TheExp.Current());
  }
  
  //-----------------------------------------------------
  // Construction Liens BasicElt => Curve du contour IndC.
  //-----------------------------------------------------
  LinkToContour(Explo,IndC,BiLo,LinkBECont);
  

  //---------------------------------------------------------------
  // Iteration sur les BasicElts. L indice associe est le meme pour
  // les courbes du contour et les edges.               .
  //---------------------------------------------------------------
  for (Ite.Initialize(LinkBECont); Ite.More(); Ite.Next()) {
    BE     = BiLo.Graph()->BasicElt(Ite.Key());    
    Type   = BiLo.GeomElt(BE)->DynamicType();
    KC     = Ite.Value();
    S      = TopoSeq.Value(Abs(KC));

    if (Type == STANDARD_TYPE(Geom2d_CartesianPoint)) {
      if (S.Orientation() == TopAbs_REVERSED) {
	TopExp::Vertices(TopoDS::Edge(S),VL,VF);
      }
      else {
	TopExp::Vertices(TopoDS::Edge(S),VF,VL);
      }
      if (KC > 0) S = VL; else S = VF;
    }
    if (!myMap.IsBound(S)) {       
      myMap.Bind(S,EmptySeq);
    }
    myMap(S).Append(BE);

    if (KC < 0) 
      myBEShape.Bind(BE, S.Oriented(TopAbs::Reverse(S.Orientation())));
    else
      myBEShape.Bind(BE, S);
  }
}


//=======================================================================
//function : LinkToContour
//purpose  : Association a chaque basicElt de la courbe du contour initial
//           dont il provient.
//=======================================================================

void LinkToContour (const BRepMAT2d_Explorer&              Explo,
		    const Standard_Integer                 IndC,
		    const BRepMAT2d_BisectingLocus&        BiLo,
		    TColStd_DataMapOfIntegerInteger&       Link)
{
  Handle (MAT_BasicElt)    BE;
  Handle (Geom2d_Geometry) GeomBE;
  Handle (Standard_Type)   Type;
  Standard_Boolean         DirectSense   = Standard_True;
  Standard_Boolean         LastPoint     = Standard_False;
  Standard_Integer         NbSect,ISect;     

  //---------------------------------------------------
  // NbSect : nombre de sections sur la courbe courrant.
  // ISect  : Compteur sur les sections.
  //---------------------------------------------------

  const TColGeom2d_SequenceOfCurve&  Cont = Explo.Contour(IndC);
  
  //------------------------------------------------------------------
  //Initialisation de l explorateur sur la premiere courbe du contour.
  //------------------------------------------------------------------
  Standard_Integer         IndOnCont     =  1;   
  Standard_Integer         PrecIndOnCont = -1;
  NbSect = BiLo.NumberOfSections(IndC,1);
  ISect  = 0;

  //------------------------------------------------------------------
  // Parcours des elements de base associes au contour IndC.
  // Rq : les elements de base sont ordonnes.
  //------------------------------------------------------------------
  for (Standard_Integer i = 1; i <= BiLo.NumberOfElts(IndC); i++) {

    BE     = BiLo.BasicElt(IndC,i);
    GeomBE = BiLo.GeomElt (BE);
    Type   = GeomBE->DynamicType();

    if (Type != STANDARD_TYPE(Geom2d_CartesianPoint)) {
      ISect++;
      //--------------------------------------------------------------------
      // l element de base est une courbe on lui associe la courbe courante.
      //--------------------------------------------------------------------
      if (DirectSense) {
	Link.Bind(BE->Index(),  IndOnCont);
      }
      else {
	Link.Bind(BE->Index(), -IndOnCont);
      }
    }
    else {      
      //--------------------------------------------------------------------
      // l element de base est un point on lui associe la courbe precedente
      //--------------------------------------------------------------------
      if (DirectSense || LastPoint) {
	Link.Bind(BE->Index(),  PrecIndOnCont);
      }
      else {
	Link.Bind(BE->Index(), -PrecIndOnCont);
      }
    }

    PrecIndOnCont = IndOnCont;
    //----------------------------------------------------------------------
    // Passage a la courbe suivante dans Explo, lorsqu on a parcouru toutes
    // les portions de courbes correspondante a la courbe initiale.
    //---------------------------------------------------------------------
    if (Type != STANDARD_TYPE(Geom2d_CartesianPoint) && ISect == NbSect) {
      if (IndOnCont < Cont.Length() && DirectSense) {
	IndOnCont++;  
	NbSect = BiLo.NumberOfSections(IndC,IndOnCont);
	ISect  = 0;
      }
      else {
	//-----------------------------------------------------
	// Pour les lignes ouvertes on repart dans l autre sens.
	//-----------------------------------------------------
	if (!DirectSense) {
	  IndOnCont--;
	  if (IndOnCont != 0) NbSect = BiLo.NumberOfSections(IndC,IndOnCont);
	  LastPoint = Standard_False;
	}
	else {
	  LastPoint = Standard_True;
	}
	ISect       = 0;
	DirectSense = Standard_False;
      }
    }
  }
}
