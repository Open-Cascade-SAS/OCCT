// File:	BRepOffset_MakeLoops.cxx
// Created:	Thu Sep  5 15:48:34 1996
// Author:	Yves FRICAUD
//		<yfr@claquox.paris1.matra-dtv.fr>

#include <stdio.h>

#include <BRepOffset_MakeLoops.ixx>
#include <BRepAlgo_Loop.hxx>


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopoDS_Iterator.hxx>
#include <BRep_TVertex.hxx>

#ifdef DRAW
#include <DBRep.hxx>
#endif
#ifdef DEB
Standard_Integer NbF = 1;
static Standard_Boolean Affich = Standard_False;
//POP pour NT
//char name[100];
#endif


BRepOffset_MakeLoops::BRepOffset_MakeLoops()
{
}

//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void BRepOffset_MakeLoops::Build(const TopTools_ListOfShape&   LF, 
				 const Handle(BRepAlgo_AsDes)& AsDes, 
				 BRepAlgo_Image&               Image)
{
  TopTools_ListIteratorOfListOfShape    it(LF);
  TopTools_ListIteratorOfListOfShape    itl,itLCE;
  BRepAlgo_Loop                       Loops;
  Loops.VerticesForSubstitute( myVerVerMap );

  for (; it.More(); it.Next()) {
    const TopoDS_Face& F = TopoDS::Face(it.Value());
    //---------------------------
    // Initialisation de Loops.
    //---------------------------
    Loops.Init(F);
    //-----------------------------
    // recuperation des edges de F.
    //-----------------------------
    const TopTools_ListOfShape& LE = AsDes->Descendant(F);
    TopTools_ListOfShape        AddedEdges;

    for (itl.Initialize(LE); itl.More(); itl.Next()) {
      TopoDS_Edge E = TopoDS::Edge(itl.Value());
      if (Image.HasImage(E)) {
	//-------------------------------------------
	// E a deja ete decoupeee dans une autre face.
	// Recuperation des edges decoupees et reorientation
	// de ces edges comme E.
	// Voir pb pour les edges qui ont disparu?
	//-------------------------------------------
	const TopTools_ListOfShape& LCE = Image.Image(E);
	for (itLCE.Initialize(LCE); itLCE.More(); itLCE.Next()) {
	  TopoDS_Shape CE = itLCE.Value().Oriented(E.Orientation()); 
	  Loops.AddConstEdge(TopoDS::Edge(CE));
	}
      }
      else {
	Loops     .AddEdge(E, AsDes->Descendant(E));
	AddedEdges.Append (E);
      }
    }
    //------------------------
    // Debouclage.
    //------------------------
    Loops.Perform();
    Loops.WiresToFaces();      
    //------------------------
    // MAJ SD.
    //------------------------
    const TopTools_ListOfShape&  NF = Loops.NewFaces();
    //-----------------------
    // F => Nouvelles faces;
    //-----------------------
    Image.Bind(F,NF);

    TopTools_ListIteratorOfListOfShape itAdded;
    for (itAdded.Initialize(AddedEdges); itAdded.More(); itAdded.Next()) {
      const TopoDS_Edge& E = TopoDS::Edge(itAdded.Value());
      //-----------------------
      //  E => Nouvelles edges;
      //-----------------------
      const TopTools_ListOfShape& LoopNE = Loops.NewEdges(E);
      if (Image.HasImage(E)) {
	Image.Add(E,LoopNE);
      }
      else {
	Image.Bind(E,LoopNE);
      }
    }
  }
  Loops.GetVerticesForSubstitute( myVerVerMap );
  if (myVerVerMap.IsEmpty())
    return;
  BRep_Builder BB;
  for (it.Initialize( LF ); it.More(); it.Next())
    {
      TopoDS_Shape F = it.Value();
      TopTools_ListOfShape LIF;
      Image.LastImage( F, LIF );
      for (itl.Initialize(LIF); itl.More(); itl.Next())
	{
	  const TopoDS_Shape& IF = itl.Value();
	  TopExp_Explorer EdExp( IF, TopAbs_EDGE );
	  for (; EdExp.More(); EdExp.Next())
	    {
	      TopoDS_Shape E = EdExp.Current();
	      TopTools_ListOfShape VList;
	      TopoDS_Iterator VerExp( E );
	      for (; VerExp.More(); VerExp.Next())
		VList.Append( VerExp.Value() );
	      TopTools_ListIteratorOfListOfShape itlv( VList );
	      for (; itlv.More(); itlv.Next())
		{
		  const TopoDS_Shape& V = itlv.Value();
		  if (myVerVerMap.IsBound( V ))
		    {
		      TopoDS_Shape NewV = myVerVerMap( V );
		      E.Free( Standard_True );
		      NewV.Orientation( V.Orientation() );
		      Handle(BRep_TVertex)& TV = *((Handle(BRep_TVertex)*) &V.TShape());
		      Handle(BRep_TVertex)& NewTV = *((Handle(BRep_TVertex)*) &NewV.TShape());
		      if (TV->Tolerance() > NewTV->Tolerance())
			NewTV->Tolerance( TV->Tolerance() );
		      NewTV->ChangePoints().Append( TV->ChangePoints() );
		      AsDes->Replace( V, NewV );
		      BB.Remove( E, V );
		      BB.Add( E, NewV );
		    }
		}
	    }
	}
    }
}

//=======================================================================
//function : IsBetweenCorks
//purpose  : 
//=======================================================================

static Standard_Boolean IsBetweenCorks(const TopoDS_Shape& E,
				       const Handle(BRepAlgo_AsDes)& AsDes,
				       const TopTools_ListOfShape&   LContext) 
{
  if (!AsDes->HasAscendant(E)) return 1;
  const TopTools_ListOfShape& LF = AsDes->Ascendant(E);
  TopTools_ListIteratorOfListOfShape it;
  for (it.Initialize(LF); it.More(); it.Next()) {
    const TopoDS_Shape& S = it.Value();
    Standard_Boolean found = 0;
    TopTools_ListIteratorOfListOfShape it2;
    for (it2.Initialize(LContext); it2.More(); it2.Next()) {
      if(S.IsSame(it2.Value())) {
	found = 1;
	break;
      }
    }
    if (!found) return 0;
  }
  return 1;
}
//=======================================================================
//function : BuildOnContext
//purpose  : 
//=======================================================================

void BRepOffset_MakeLoops::BuildOnContext(const TopTools_ListOfShape&   LContext,  
					  const BRepOffset_Analyse&     Analyse, 
					  const Handle(BRepAlgo_AsDes)& AsDes, 
					  BRepAlgo_Image&               Image,
					  const Standard_Boolean        InSide)
{
  //-----------------------------------------
  // debouclage des bouchons.
  //-----------------------------------------
  TopTools_ListIteratorOfListOfShape  it(LContext);
  TopTools_ListIteratorOfListOfShape  itl,itLCE;
  BRepAlgo_Loop                     Loops;
  Loops.VerticesForSubstitute( myVerVerMap );
  TopExp_Explorer                     exp; 
  TopTools_MapOfShape                 MapExtent;

  for (; it.More(); it.Next()) {
    const TopoDS_Face& F = TopoDS::Face(it.Value());
    TopTools_MapOfShape                 MBound;
    //-----------------------------------------------
    // Initialisation de Loops.
    // F est reversed elle sera ajoute dans myOffC.
    // et myOffC sera reversed dans le resultat final.
    //-----------------------------------------------
    TopoDS_Shape aLocalShape = F.Reversed();
    if (InSide) Loops.Init(TopoDS::Face(aLocalShape));
//    if (InSide) Loops.Init(TopoDS::Face(F.Reversed()));
    else        Loops.Init(F);
    //--------------------------------------------------------
    // recuperation des edges de F non modifie par definition.
    //--------------------------------------------------------
    for (exp.Init(F.Oriented(TopAbs_FORWARD),TopAbs_EDGE);
	 exp.More();
	 exp.Next()) {
      TopoDS_Edge CE = TopoDS::Edge(exp.Current());
      MBound.Add(CE);   
      if (Analyse.HasAncestor(CE)) {
	// les arretes des bouchons sauf les arretes de conexite entre bouchons.
	//      if (!AsDes->HasAscendant(CE)) {
	aLocalShape = CE.Reversed();
	if (InSide) Loops.AddConstEdge(CE);
	else        Loops.AddConstEdge(TopoDS::Edge(aLocalShape));
//	else        Loops.AddConstEdge(TopoDS::Edge(CE.Reversed()));
      }
    }
    //------------------------------------------------------
    // Trace des offsets + edge de connexite entre bouchons.
    //------------------------------------------------------    
    const TopTools_ListOfShape& LE = AsDes->Descendant(F);
    TopTools_ListOfShape        AddedEdges;
    
    for (itl.Initialize(LE); itl.More(); itl.Next()) {
      TopoDS_Edge E = TopoDS::Edge(itl.Value());
      if (Image.HasImage(E)) {
	//-------------------------------------------
	// E a deja ete decoupeee dans une autre face.
	// Recuperation des edges decoupees et reorientation
	// de ces edges comme E.
	// Voir pb pour les edges qui ont disparu?
	//-------------------------------------------
	const TopTools_ListOfShape& LCE = Image.Image(E);
	for (itLCE.Initialize(LCE); itLCE.More(); itLCE.Next()) {
	  TopoDS_Shape CE = itLCE.Value().Oriented(E.Orientation()); 	  
	  if (MapExtent.Contains(E)) {
	    Loops.AddConstEdge(TopoDS::Edge(CE));
	    continue;
	  }
	  if (!MBound.Contains(E)) CE.Reverse();
	  if (InSide) Loops.AddConstEdge(TopoDS::Edge(CE));
	  else
	    {
	      TopoDS_Shape aLocalShape = CE.Reversed();
	      Loops.AddConstEdge(TopoDS::Edge(aLocalShape));
	    }
//	  else        Loops.AddConstEdge(TopoDS::Edge(CE.Reversed()));
	}
      }
      else {
	if (IsBetweenCorks(E,AsDes,LContext) && AsDes->HasDescendant(E)) {
	  //conexite entre 2 bouchons
	  MapExtent.Add(E);
	  TopTools_ListOfShape LV;
	  if (InSide) {
	    for (itLCE.Initialize(AsDes->Descendant(E)); itLCE.More(); itLCE.Next()) {
	      LV.Append(itLCE.Value().Reversed());
	    }
	    Loops.AddEdge(E,LV);
	  }
	  else {
	    Loops.AddEdge(E,AsDes->Descendant(E));
	  }
	  AddedEdges.Append (E);
	}
	else if (IsBetweenCorks(E,AsDes,LContext)) {
	TopoDS_Shape aLocalShape = E.Reversed();
	  if (InSide) Loops.AddConstEdge(E);
	  else        Loops.AddConstEdge(TopoDS::Edge(aLocalShape));
//	  if (InSide) Loops.AddConstEdge(TopoDS::Edge(E));
//	  else        Loops.AddConstEdge(TopoDS::Edge(E.Reversed()));
	}
	else { 
	  TopoDS_Shape aLocalShape = E.Reversed();
	  if (InSide) Loops.AddConstEdge(TopoDS::Edge(aLocalShape));
	  else        Loops.AddConstEdge(E);
//	  if (InSide) Loops.AddConstEdge(TopoDS::Edge(E.Reversed()));
//	  else        Loops.AddConstEdge(TopoDS::Edge(E));
	}
      }
    }
    //------------------------
    // Debouclage.
    //------------------------
    Loops.Perform();
    Loops.WiresToFaces();      
    //------------------------
    // MAJ SD.
    //------------------------
    const TopTools_ListOfShape&  NF = Loops.NewFaces();
    //-----------------------
    // F => Nouvelles faces;
    //-----------------------
    Image.Bind(F,NF);   

    TopTools_ListIteratorOfListOfShape itAdded;
    for (itAdded.Initialize(AddedEdges); itAdded.More(); itAdded.Next()) {
      const TopoDS_Edge& E = TopoDS::Edge(itAdded.Value());
      //-----------------------
      //  E => Nouvelles edges;
      //-----------------------
      if (Image.HasImage(E)) {
	Image.Add(E,Loops.NewEdges(E));
      }
      else {
	Image.Bind(E,Loops.NewEdges(E));
      }
    }
  }
  Loops.GetVerticesForSubstitute( myVerVerMap );
  if (myVerVerMap.IsEmpty())
    return;
  BRep_Builder BB;
  for (it.Initialize( LContext ); it.More(); it.Next())
    {
      TopoDS_Shape F = it.Value();
      TopTools_ListOfShape LIF;
      Image.LastImage( F, LIF );
      for (itl.Initialize(LIF); itl.More(); itl.Next())
	{
	  const TopoDS_Shape& IF = itl.Value();
	  TopExp_Explorer EdExp( IF, TopAbs_EDGE );
	  for (; EdExp.More(); EdExp.Next())
	    {
	      TopoDS_Shape E = EdExp.Current();
	      TopTools_ListOfShape VList;
	      TopoDS_Iterator VerExp( E );
	      for (; VerExp.More(); VerExp.Next())
		VList.Append( VerExp.Value() );
	      TopTools_ListIteratorOfListOfShape itlv( VList );
	      for (; itlv.More(); itlv.Next())
		{
		  const TopoDS_Shape& V = itlv.Value();
		  if (myVerVerMap.IsBound( V ))
		    {
		      TopoDS_Shape NewV = myVerVerMap( V );
		      E.Free( Standard_True );
		      NewV.Orientation( V.Orientation() );
		      Handle(BRep_TVertex)& TV = *((Handle(BRep_TVertex)*) &V.TShape());
		      Handle(BRep_TVertex)& NewTV = *((Handle(BRep_TVertex)*) &NewV.TShape());
		      if (TV->Tolerance() > NewTV->Tolerance())
			NewTV->Tolerance( TV->Tolerance() );
		      NewTV->ChangePoints().Append( TV->ChangePoints() );
		      AsDes->Replace( V, NewV );
		      BB.Remove( E, V );
		      BB.Add( E, NewV );
		    }
		}
	    }
	}
    }
}


//=======================================================================
//function : BuildFaces
//purpose  : 
//=======================================================================
  
void BRepOffset_MakeLoops::BuildFaces(const TopTools_ListOfShape&   LF, 
				      const Handle(BRepAlgo_AsDes)& AsDes, 
				      BRepAlgo_Image&               Image)
{
  TopTools_ListIteratorOfListOfShape itr,itl,itLCE;
  Standard_Boolean                   ToRebuild;
  BRepAlgo_Loop                    Loops;
  Loops.VerticesForSubstitute( myVerVerMap );
  BRep_Builder                       B;

  //----------------------------------
  // Boucle sur toutes les faces //.
  //----------------------------------
  for (itr.Initialize(LF); itr.More(); itr.Next()) {
    TopoDS_Face F = TopoDS::Face(itr.Value());
    Loops.Init(F);
    ToRebuild = Standard_False;
    TopTools_ListOfShape        AddedEdges;
    
    if (!Image.HasImage(F)) {
      //----------------------------------
      // Face F non deja reconstruite.
      //----------------------------------
      const TopTools_ListOfShape& LE = AsDes->Descendant(F);
      //----------------------------------------------------------------
      // premiere boucle pour determiner si des edges de la face ont ete
      // reconstruite.
      // - maj de la map MONV. certains vertex sur les edges reconstruites
      // coincide geometriquement avec les anciens mais ne sont pas IsSame.
      //----------------------------------------------------------------
      TopTools_DataMapOfShapeShape MONV;
      TopoDS_Vertex OV1,OV2,NV1,NV2;
      
      for (itl.Initialize(LE); itl.More(); itl.Next()) {
	TopoDS_Edge E = TopoDS::Edge(itl.Value());
	if (Image.HasImage(E)) {
	  const TopTools_ListOfShape& LCE = Image.Image(E);
	  if (LCE.Extent() == 1 && LCE.First().IsSame(E)) {
	    TopoDS_Shape aLocalShape = LCE.First().Oriented(E.Orientation());
	    TopoDS_Edge CE = TopoDS::Edge(aLocalShape);
//	    TopoDS_Edge CE = TopoDS::Edge(LCE.First().Oriented(E.Orientation()));
	    Loops.AddConstEdge(CE);
	    continue;
	  }
	  //----------------------------------
	  // F doit etre reconstruite.
	  //----------------------------------
	  ToRebuild = Standard_True;
	  for (itLCE.Initialize(LCE); itLCE.More(); itLCE.Next()) {
	    TopoDS_Shape aLocalShape = itLCE.Value().Oriented(E.Orientation());
	    TopoDS_Edge CE = TopoDS::Edge(aLocalShape); 
//	    TopoDS_Edge CE = TopoDS::Edge(itLCE.Value().Oriented(E.Orientation())); 
	    TopExp::Vertices (E ,OV1,OV2);
	    TopExp::Vertices (CE,NV1,NV2);
	    if (!OV1.IsSame(NV1)) MONV.Bind(OV1,NV1);
	    if (!OV2.IsSame(NV2)) MONV.Bind(OV2,NV2);
	    Loops.AddConstEdge(CE);
	  }
	}
      }
      if (ToRebuild) {
#ifdef DRAW
//POP pour NT
	if ( Affich) {
	  char* name = new char[100];
	  sprintf(name,"CF_%d",NbF++);
	  DBRep::Set(name,F);
	}
#endif

	//-----------------------------------------------------------
	// les edges non reconstruites dans d autre faces sont 
	// ajoutees .si leurs vertex ont ete reconstruits elles
	// seront reconstruites.
	//-----------------------------------------------------------
	for (itl.Initialize(LE); itl.More(); itl.Next()) {
	  Standard_Real f,l;
	  TopoDS_Edge E = TopoDS::Edge(itl.Value());
	  BRep_Tool::Range(E,f,l);
	  if (!Image.HasImage(E)) {
	    TopExp::Vertices (E,OV1,OV2);
	    TopTools_ListOfShape LV;
	    if (MONV.IsBound(OV1)) {
	      TopoDS_Vertex VV = TopoDS::Vertex(MONV(OV1));
	      VV.Orientation(TopAbs_FORWARD);
	      LV.Append(VV);
	      TopoDS_Shape aLocalShape = VV.Oriented(TopAbs_INTERNAL);
	      B.UpdateVertex(TopoDS::Vertex(aLocalShape),
			     f,E,BRep_Tool::Tolerance(VV));
	    }
	    if (MONV.IsBound(OV2)) {
	      TopoDS_Vertex VV = TopoDS::Vertex(MONV(OV2));
	      VV.Orientation(TopAbs_REVERSED);
	      LV.Append(VV);
	      TopoDS_Shape aLocalShape = VV.Oriented(TopAbs_INTERNAL);
	      B.UpdateVertex(TopoDS::Vertex(aLocalShape),
			     l,E,BRep_Tool::Tolerance(VV));
//	      B.UpdateVertex(TopoDS::Vertex(VV.Oriented(TopAbs_INTERNAL)),
//			     l,E,BRep_Tool::Tolerance(VV));
	    }
	    if (LV.IsEmpty()) Loops.AddConstEdge(E);
	    else  {
	      Loops.AddEdge    (E,LV);
	      AddedEdges.Append(E);
	    }
	  }
	}
      }  
    }
    if (ToRebuild) {
      //------------------------
      // Reconstruction.
      //------------------------
      Loops.Perform();
      Loops.WiresToFaces();
      //------------------------
      // MAJ SD.
      //------------------------
      const TopTools_ListOfShape&  NF = Loops.NewFaces();
      //-----------------------
      // F => Nouvelles faces;
      //-----------------------
      Image.Bind(F,NF); 

      TopTools_ListIteratorOfListOfShape itAdded;
      for (itAdded.Initialize(AddedEdges); itAdded.More(); itAdded.Next()) {
	const TopoDS_Edge& E = TopoDS::Edge(itAdded.Value());
	//-----------------------
	//  E => Nouvelles edges;
	//-----------------------
	if (Image.HasImage(E)) {
	  Image.Add(E,Loops.NewEdges(E));
	}
	else {
	  Image.Bind(E,Loops.NewEdges(E));
	}
      }
    }
  }
  Loops.GetVerticesForSubstitute( myVerVerMap );
  if (myVerVerMap.IsEmpty())
    return;
  BRep_Builder BB;
  for (itr.Initialize( LF ); itr.More(); itr.Next())
    {
      TopoDS_Shape F = itr.Value();
      TopTools_ListOfShape LIF;
      Image.LastImage( F, LIF );
      for (itl.Initialize(LIF); itl.More(); itl.Next())
	{
	  const TopoDS_Shape& IF = itl.Value();
	  TopExp_Explorer EdExp( IF, TopAbs_EDGE );
	  for (; EdExp.More(); EdExp.Next())
	    {
	      TopoDS_Shape E = EdExp.Current();
	      TopTools_ListOfShape VList;
	      TopoDS_Iterator VerExp( E );
	      for (; VerExp.More(); VerExp.Next())
		VList.Append( VerExp.Value() );
	      TopTools_ListIteratorOfListOfShape itlv( VList );
	      for (; itlv.More(); itlv.Next())
		{
		  const TopoDS_Shape& V = itlv.Value();
		  if (myVerVerMap.IsBound( V ))
		    {
		      TopoDS_Shape NewV = myVerVerMap( V );
		      E.Free( Standard_True );
		      NewV.Orientation( V.Orientation() );
		      Handle(BRep_TVertex)& TV = *((Handle(BRep_TVertex)*) &V.TShape());
		      Handle(BRep_TVertex)& NewTV = *((Handle(BRep_TVertex)*) &NewV.TShape());
		      if (TV->Tolerance() > NewTV->Tolerance())
			NewTV->Tolerance( TV->Tolerance() );
		      NewTV->ChangePoints().Append( TV->ChangePoints() );
		      AsDes->Replace( V, NewV );
		      BB.Remove( E, V );
		      BB.Add( E, NewV );
		    }
		}
	    }
	}
    }
}
