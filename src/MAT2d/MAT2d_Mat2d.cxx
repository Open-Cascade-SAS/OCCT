// Created on: 1992-09-22
// Created by: Gilles DEBARBOUILLE
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


#include <MAT2d_Mat2d.hxx>
#include <MAT2d_Tool2d.hxx>
#include <MAT_Bisector.hxx>
#include <MAT_DataMapOfIntegerBisector.hxx>
#include <MAT_Edge.hxx>
#include <MAT_ListOfBisector.hxx>
#include <MAT_ListOfEdge.hxx>
#include <Precision.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>

//========================================================================
//  function : MAT2d_Mat2d
//  purpose  :
//========================================================================
MAT2d_Mat2d::MAT2d_Mat2d(const Standard_Boolean IsOpenResult)
{
  myIsOpenResult = IsOpenResult;
  thenumberofbisectors = 0;
  thenumberofedges     = 0;
}


//========================================================================
//  function : CreateMat
//  purpose  : Calcul des lieux Bisecteurs.
//
//  Structure de la carte.
//  ======================
//  La carte des lieux bisecteurs se presente sous la forme d un ou plusieurs
//  arbres de bisectrices.
//  ( un arbre, si calcul a l interieur du contour, plusieurs sinon).
//
//  Les branches de plus bas niveau de l arbre separent deux elements voisins 
//  du contour.
// 
//  Principe de l algorithme.
//  -------------------------
//  l arbre est construit des branches les plus basses vers les plus hautes.
//
//  0 . Calcul des bisectrices entre elements voisins du contour.
//  1 . Elimination de certains element du contour => nouveau contour
//  2 . Retour en 0.
//  
//  Principales etapes de l algorithme.
//  ===================================
//
//  etape 1: Initialisation de l algorithme .
//  -----------------------------------------
//   Recuperation via le tool du nombre d'elements sur le contour
//   Initialisation de <theedgelist>, un edge correspond a chaque 
//   element du contour.
//
//  etape 2 : Boucle principale.
//  ----------------------------  
//    0 - Tant que Nombre d'edge > 1
//         
//      1. Pour chaque edge: construction de la bissectrice entre l edge 
//         et l edge suivante.
//         La bissectrice est semi_infinie, elle est soit trimmee par le
//         point commun des deux edges, soit par l intersection de deux
//         bissectrices antecedentes.
//
//      2. Intersection des Bisectrices issue du meme edge
//         => une bisectrice est intersectee avec sa voisine a gauche
//            et sa voisine a droite.
//
//      3. Analyse des intersections.
//         Si pas d'intersection entre deux bisectrices B1 et B2
//         - Recherche de l intersection la plus basse de B1 avec les 
//           Bisectrices antecedentes a B2 du cote de B1. Soit Bi la 
//           bissectrice intersectee. Toutes les bissectrices constituant la
//           branche qui relie B2 a Bi sont marquees a effacer
//         - idem pour B2.
//  
//      4. Suppresion des bisectrices a effacer.
//         une bisectrise est a effacer :
//          - Anulation de l intersection dont la bissectrice est issue
//            => Prolongement des deux bisectrices posterieures.
//          - Reinsertion des edge correspondant dans <theedgelist>.       
//
//      5. Pour chaque edge, analyse des distances entre les points d inter
//         section et l edge.
//         B1 B2 les bisectrices liee a l edge
//         Soit P0 le point d intersection des bissectrices .
//         Soit P1 le point d intersection de B1 avec son autre voisine .
//         Soit P2 le point d intersection de B2 avec son autre voisine .
//        
//         si sur B1 le parametre de P0 < parametre de P1 et
//         si sur B2 le parametre de P0 < parametre de P2 
//         alors suppression de l edge de la liste des edges <theedgelist>.
//
//         rq: le parametre sur une bissectirce est croissant par rapport
//         a la distance du point courant aux edges.

//      6. Si aucune edge est elimine alors sortie de la boucle principale.
//
//      7. Retour en 0.
//
//  etape 3 : Creation des racines des arbres de bisectrices.
//  ---------------------------------------------------------
//            Recuperation des bissectrices calculees lors du dernier passage
//            dans la boucle.
//
//========================================================================
void MAT2d_Mat2d::CreateMatOpen(MAT2d_Tool2d& atool)
{

#ifdef ICONTINUE
  Standard_Boolean Icontinue;
#endif

  Standard_Boolean interrupt = Standard_False;

  Handle(MAT_Edge) edgetoremove;
  Handle(MAT_Edge) previousedge,currentedge;

  Standard_Integer      noofbisectorstoremove;
  Handle(MAT_Bisector)  firstbisector,secondbisector;
  Handle(MAT_Edge)      edge;
  Standard_Integer      intersectionpoint;
  Standard_Integer      beginbisector;
  Standard_Integer      noofbisectors;

  Standard_Integer	NbIterBis = 0;
  Standard_Integer	EvenNbIterBis = 10;
  TColStd_Array1OfInteger EdgeNumbers(1, EvenNbIterBis+1);
  EdgeNumbers.Init(-1);
  Standard_Boolean	ToNullifyNoofbisectorstoremove = Standard_False;

  Handle(MAT_ListOfBisector) currentbisectorlist;

  Handle(MAT_Bisector) bisectortoremove,lastbisector,currentbisector;
  Handle(MAT_Bisector) previousbisector;

  Standard_Integer     i,j,k,narea,shift,compact,all;
  Standard_Integer     noofedges;
  Standard_Integer     NumberMaxOfIte;
  Standard_Real        toleranceofconfusion;

  noofedges            = atool.NumberOfItems();
  toleranceofconfusion = atool.ToleranceOfConfusion();
  NumberMaxOfIte       = noofedges*noofedges;

  TColStd_Array1OfInteger firstarea(0, noofedges);
  TColStd_Array1OfInteger lastarea(0, noofedges);
  TColStd_Array1OfInteger noofarea(0, noofedges);

  Standard_Integer  parama[2];
  Standard_Integer  paramb[2];

// -----------------------------------------
// Initialisation et remise a zero des maps.
// -----------------------------------------
  bisectoronetoremove.Clear();
  bisectortwotoremove.Clear();
  typeofbisectortoremove.Clear();
  bisectormap.Clear();

  isDone        = Standard_True;
  noofbisectors = noofedges-1;
  beginbisector = 0;

// --------------------------------------------------------------------
// Construction de <theedgelist> un edge correspond a un element simple
// du contour.
// --------------------------------------------------------------------
  theedgelist = new MAT_ListOfEdge();

  for(i=0; i<noofedges; i++) {
    edge = new MAT_Edge();
    edge->EdgeNumber(i+1);
    edge->Distance(-1);
    theedgelist->BackAdd(edge);
  }
  
  theedgelist->Loop();

//---------------------------------------------------
// Initialisation des bissectrices issues du contour.
//---------------------------------------------------
  Standard_Real Dist;
  theedgelist->First();

  for(i=0; i<theedgelist->Number()-1; i++) {
    bisectormap.Bind(i,new MAT_Bisector());
    bisectormap(i)->IndexNumber(i);
    bisectormap(i)->FirstEdge(theedgelist->Current());
    bisectormap(i)->FirstVector
      (atool.TangentBefore(theedgelist->Current()->EdgeNumber(), myIsOpenResult));
    theedgelist->Next();
    bisectormap(i)->SecondEdge(theedgelist->Current());
    bisectormap(i)->IssuePoint
      (atool.FirstPoint(theedgelist->Current()->EdgeNumber(),Dist));  
    bisectormap(i)->DistIssuePoint(Dist);
    bisectormap(i)->SecondVector
      (atool.TangentAfter(theedgelist->Current()->EdgeNumber(), myIsOpenResult));
  }

//----------------------------------------------------
// Affectation a chaque edge de ses deux bissectrices.
//----------------------------------------------------
  theedgelist->First();
  theedgelist->Current()->FirstBisector(bisectormap(0));
  theedgelist->Current()->SecondBisector(bisectormap(0));
  theedgelist->Next();

  for(i=1; i<theedgelist->Number()-1; i++) {
    theedgelist->Current()->FirstBisector
      (bisectormap(i-1));
    theedgelist->Current()->SecondBisector
      (bisectormap(i));
    theedgelist->Next();
  }

  theedgelist->Current()->FirstBisector(bisectormap(theedgelist->Number()-2));
  theedgelist->Current()->SecondBisector(bisectormap(theedgelist->Number()-2));

//===========================================================================
//                         Boucle Principale   (etape 2)
//===========================================================================
  Standard_Integer NumberOfIte = 0;

  while(theedgelist->Number()>1) {


    // ------------------------------------------------------------------
    //  Creation des geometries des bissectrices via le tool. (etape 2.1)
    // -------------------------------------------------------------------

    for(i=beginbisector; i<noofbisectors; i++) {

      atool.CreateBisector(bisectormap(i));
      thenumberofbisectors++;
      
#ifdef OCCT_DEBUG_Mat
      atool.Dump(bisectormap(i)->BisectorNumber(),1);
#ifdef ICONTINUE
      cin>>Icontinue;
#endif
#endif
    }

    // ---------------------------------------------
    //  Condition de sortie de la boucle principale.
    // ---------------------------------------------

//  Modified by Sergey KHROMOV - Fri Nov 17 10:28:28 2000 Begin
    if (theedgelist->Number() < 3)
      break;
//  Modified by Sergey KHROMOV - Fri Nov 17 10:28:37 2000 End
    
    //---------------------------------------------------
    // boucle 2 Tant qu il y a des bisectrices a effacer.
    //---------------------------------------------------
    for(;;) {
      NbIterBis++;

      noofbisectorstoremove = 0;
      theedgelist->First();
      theedgelist->Next();

      //--------------------------------------------------------------
      // Calcul des intersections des bisectrices voisines.(etape 2.2)
      //--------------------------------------------------------------

      if (NbIterBis <= EvenNbIterBis+1)
	EdgeNumbers(NbIterBis) = theedgelist->Number();
      else
	{
	  for (k = 1; k <= EvenNbIterBis; k++)
	    EdgeNumbers(k) = EdgeNumbers(k+1);
	  EdgeNumbers(EvenNbIterBis+1) = theedgelist->Number();
	}
      if (EdgeNumbers(EvenNbIterBis+1) == EdgeNumbers(1))
	ToNullifyNoofbisectorstoremove = Standard_True;

      for(i=1; i<theedgelist->Number()-1; i++) {
	edge = theedgelist->Current();
	if(edge->Distance() == -1.) {
	  firstbisector = edge->FirstBisector();
	  secondbisector = edge->SecondBisector();
	  edge->Distance(atool.IntersectBisector
			 (firstbisector,secondbisector,intersectionpoint));
	  edge->IntersectionPoint(intersectionpoint);

	  if(edge->Distance() == Precision::Infinite()) {
	    if(firstbisector->IndexNumber() >= beginbisector ||
	       secondbisector->IndexNumber() >= beginbisector) 
	      Intersect(atool,0,noofbisectorstoremove,
			firstbisector,secondbisector );
	  }
	  else {
	    if(firstbisector->IndexNumber() >= beginbisector) {
	      Intersect(atool,1,noofbisectorstoremove,
			firstbisector,secondbisector );
	    }
	    if(secondbisector->IndexNumber() >= beginbisector) {
	      Intersect(atool,2,noofbisectorstoremove,
			firstbisector,secondbisector );
	    }
	  }
	}
	theedgelist->Next();
      }
      
      //-------------------------------
      // Test de sortie de la boucle 2.
      //-------------------------------

      if (ToNullifyNoofbisectorstoremove)
	noofbisectorstoremove = 0;
      if(noofbisectorstoremove == 0) break;

      //---------------------------------------------------
      // Annulation des bissectrices a effacer. (etape 2.4)
      //---------------------------------------------------

      for(i=0; i<noofbisectorstoremove; i++) {

	bisectortoremove = bisectoronetoremove(i);

	//---------------------------------------------------------------
	// Destruction des bisectrices descendantes de <bisectortoremove>
	// On descend dans l arbre jusqu a ce qu on atteigne
	// <bisectortwotoremove(i).
	//---------------------------------------------------------------

	for(;;){

#ifdef OCCT_DEBUG_Mat
	  atool.Dump(bisectortoremove->BisectorNumber(),0);
#endif
	  // ----------------------------------
	  // Annulation de <bisectortoremove>.
	  // ----------------------------------
	  thenumberofbisectors--;
	  currentbisectorlist = bisectortoremove->List();
	  currentbisectorlist->First();
	  currentbisector = currentbisectorlist->FirstItem();
	  previousedge = currentbisector->FirstEdge();
	  theedgelist->Init(previousedge);
	  previousedge->Distance(-1.);
	  previousedge->FirstBisector()->SecondParameter(Precision::Infinite());
	  previousedge->SecondBisector()->FirstParameter(Precision::Infinite());

	  //------------------------------------------
	  // Annulation des fils de <currentbisector>.
	  //------------------------------------------

	  while(currentbisectorlist->More()) {
	    currentbisector = currentbisectorlist->Current();
	    currentedge  = currentbisector->SecondEdge();

	    //---------------------------------------
	    // Reinsertion de l edge dans le contour.
	    //---------------------------------------
	    theedgelist->LinkAfter(currentedge);
	    theedgelist->Next();
	    
	    currentedge->FirstBisector(currentbisector);
	    previousedge->SecondBisector(currentbisector);
#ifdef OCCT_DEBUG_Mat		      
	    atool.Dump(currentbisector->BisectorNumber(),0);
#endif

	    //------------------------------------------------------
	    // Annulation de l intersection ie les fils qui
	    // ont generes l intersection sont prolonges a l infini.
	    //------------------------------------------------------

	    currentbisector->FirstParameter (Precision::Infinite());
	    currentbisector->SecondParameter(Precision::Infinite());
		      
	    atool.TrimBisector(currentbisector);
	    
#ifdef OCCT_DEBUG_Mat
	    atool.Dump(currentbisector->BisectorNumber(),1);
#endif
	    currentedge->Distance(-1.);
	    currentedge->FirstBisector()->SecondParameter(Precision::Infinite());
	    currentedge->SecondBisector()->FirstParameter(Precision::Infinite());
	    
	    previousedge = currentedge;
	    currentbisectorlist->Next();
	  }
	  
	  theedgelist->Unlink();

	  //-----------------------------------------------------------
	  // Test de sortie de la boucle d annulation des bissectrices.
	  //-----------------------------------------------------------

	  if(bisectortoremove->BisectorNumber() ==
	     bisectortwotoremove(i)->BisectorNumber()) break;

	  //-----------------------
	  // Descente dans l arbre.
	  //-----------------------

	  if(typeofbisectortoremove(i) == 1)
	    bisectortoremove = bisectortoremove->FirstBisector();
	  else
	    bisectortoremove = bisectortoremove->LastBisector();
	
	}  //----------------------------------------------------
	   // Fin boucle d annulation des bissectrices issue de 
	   // <bisectoronetoremove(i)>.
	   //----------------------------------------------------

      } //------------------------------------------
        // Fin boucle d annulation des bissectrices.
        //-------------------------------------------

#ifdef ICONTINUE
      cin>>Icontinue;
#endif
    } //--------------
      // Fin Boucle 2.
      //--------------
    
    // ----------------------------------------------------------------------
    // Analyse des parametres des intersections sur les bisectrices de chaque
    // edge et determination des portions de contour a supprimees. (etape 2.5)
    // ----------------------------------------------------------------------

    theedgelist->First();
    theedgelist->Next();
      
    currentbisector = theedgelist->Current()->FirstBisector();
    if (currentbisector->FirstParameter()  == Precision::Infinite() &&
	currentbisector->SecondParameter() == Precision::Infinite()) {
      parama[0] = -1;
      paramb[0] = -1;
    }
    else if(currentbisector->FirstParameter() == Precision::Infinite()) {
      parama[0] = -1;
      paramb[0] =  1;
    }
    else if(currentbisector->SecondParameter() == Precision::Infinite()) {
      paramb[0] = -1;
      parama[0] =  1;
    }
    else if (atool.Distance(currentbisector,
			    currentbisector->FirstParameter(),
			    currentbisector->SecondParameter()) 
	     > toleranceofconfusion) {
      if((currentbisector->FirstParameter() - 
	  currentbisector->SecondParameter())
	 *currentbisector->Sense() > 0.) {      
	parama[0] = -1;
	paramb[0] =  1;
      }
      else {
	paramb[0] = -1;
	parama[0] =  1;
      }
    }
    else {
      parama[0] = 1;
      paramb[0] = 1;
    }
    
    narea = -1;
    
    for(i=1; i<theedgelist->Number()-1; i++) {
      currentbisector = theedgelist->Current()->SecondBisector();
      if (currentbisector->FirstParameter()  == Precision::Infinite() &&
	  currentbisector->SecondParameter() == Precision::Infinite()) {
	parama[1] = -1;
	paramb[1] = -1;
      }
      else if(currentbisector->FirstParameter() == Precision::Infinite()) {
	parama[1] = -1;
	paramb[1] =  1;
      }
      else if(currentbisector->SecondParameter() == Precision::Infinite()) {
	paramb[1] = -1;
	parama[1] =  1;
      }
      else if (atool.Distance(currentbisector,
			      currentbisector->FirstParameter(),
			      currentbisector->SecondParameter()) 
	       > toleranceofconfusion) {
	if((currentbisector->FirstParameter() - 
	    currentbisector->SecondParameter()) 
	   *currentbisector->Sense() > 0.) {      
	  parama[1] = -1;
	  paramb[1] =  1;
	}
	else {
	  paramb[1] = -1;
	  parama[1] =  1;
	}
      }
      else {
	parama[1] = 1;
	paramb[1] = 1;
      }

      //-----------------------------------------------------------------
      // Test si l edge est a enlever du contour
      // Construction des portions de contour a eliminer.
      //
      //  narea : nombre de portions continues du contour a eliminer.
      //  firstarea[i] : indice premier edge de la portion i.
      //  lastarea[i]  : indice dernier edge de la portion i.
      //-----------------------------------------------------------------

#ifdef OCCT_DEBUG_Mat
      cout <<" Test sur les parametres pour elimination"<<endl;
      cout << " Edge number :"<<theedgelist->Current()->EdgeNumber()<<endl;
#endif

      if(paramb[0] > 0 && parama[1] > 0) {

#ifdef OCCT_DEBUG_Mat
      cout <<" A ELIMINER "<<endl;
#endif	
	if(narea < 0) {
	  firstarea(++narea) = theedgelist->Index();
	  lastarea(narea) = firstarea(narea);
	  noofarea(narea) = 1;
	}
	else {
	  if(theedgelist->Index() == lastarea(narea)+1) {
	    lastarea(narea)++;
	    noofarea(narea)++;
	  }
	  else {
	    firstarea(++narea) = theedgelist->Index();
	    lastarea(narea) = firstarea(narea);
	    noofarea(narea) = 1;
	  }
	}
      }
      parama[0] = parama[1];
      paramb[0] = paramb[1];
      theedgelist->Next();
    
    } 
    
    compact = 0;
    if(narea > 0) {
      if(lastarea(narea) == theedgelist->Number() && firstarea(0) == 1) {
	firstarea(0) = firstarea(narea);
	noofarea(0) = noofarea(0)+noofarea(narea);
	compact = noofarea(narea);
	narea--;
      }
    }
    
    narea++;

    //------------------------------------------------------------------
    // Sortie de la boucle principale si il n y a pas d edge a eliminer.
    // (etape 2.6)
    //------------------------------------------------------------------
    if(narea == 0) {
      interrupt = Standard_True;
      break;
    }
    

    //----------------------------------------------------------------
    // Elimination des edges a enlever du contour
    // => Mise a jour du nouveau contour.
    // => Creation des bissectrices entre les nouvelles edges voisines.
    //----------------------------------------------------------------

    beginbisector = noofbisectors;
    shift = 0;
    all = 0;
    if(narea == 1 && noofarea(0) == theedgelist->Number()) all = 1;

    for(i=0; i<narea; i++) {
      if(i == 1)shift = shift-compact;
      theedgelist->First();
      theedgelist->Next();
      edgetoremove = theedgelist->Brackets(firstarea(i)-shift);
      
      edgetoremove->FirstBisector()->EndPoint(edgetoremove
					      ->IntersectionPoint());
      
#ifdef OCCT_DEBUG_Mat
      atool.Dump(edgetoremove->FirstBisector()->BisectorNumber(),0);
#endif

      edgetoremove->FirstBisector()->FirstParameter
	(edgetoremove->FirstBisector()->SecondParameter());
	  
#ifdef OCCT_DEBUG_Mat
      if(atool.TrimBisector(edgetoremove->FirstBisector()))
	atool.Dump(edgetoremove->FirstBisector()->BisectorNumber(),1);
#else
      atool.TrimBisector(edgetoremove->FirstBisector());
#endif

      bisectormap.Bind(noofbisectors,new MAT_Bisector());
      bisectormap(noofbisectors)->IndexNumber(noofbisectors);
      bisectormap(noofbisectors)->DistIssuePoint(edgetoremove->Distance());
      bisectormap(noofbisectors)->IssuePoint(edgetoremove
						->IntersectionPoint());
      bisectormap(noofbisectors)->FirstEdge(theedgelist->PreviousItem());
      bisectormap(noofbisectors)->AddBisector(edgetoremove
						 ->FirstBisector());

      for(j=0; j<noofarea(i); j++) {
	theedgelist->Unlink();
	theedgelist->Next();
	shift++;

#ifdef OCCT_DEBUG_Mat
	cout<<" Suppression de l'arete : "<<edgetoremove->EdgeNumber()<<endl;
#endif

	if(all == 0 || j+1 != noofarea(i)) {
	  bisectormap(noofbisectors)->AddBisector(edgetoremove
						     ->SecondBisector());
	}
	edgetoremove->SecondBisector()->EndPoint(edgetoremove
						 ->IntersectionPoint());

#ifdef OCCT_DEBUG_Mat
	atool.Dump(edgetoremove->SecondBisector()->BisectorNumber(),0);
#endif

	edgetoremove->SecondBisector()->SecondParameter
	  (edgetoremove->SecondBisector()->FirstParameter());
#ifdef OCCT_DEBUG_Mat
	if(atool.TrimBisector(edgetoremove->SecondBisector()))
	  atool.Dump(edgetoremove->SecondBisector()->BisectorNumber(),1);
#else
	atool.TrimBisector(edgetoremove->SecondBisector());
#endif
	edgetoremove = theedgelist->Current();
      }
      bisectormap(noofbisectors)->SecondEdge(theedgelist->Current());

      theedgelist->PreviousItem()
        ->SecondBisector(bisectormap(noofbisectors));
      theedgelist->Current()->FirstBisector(bisectormap(noofbisectors));
	  
      bisectormap(noofbisectors)->FirstVector
	(atool.Tangent
	 (bisectormap(noofbisectors)->FirstBisector()
	  ->BisectorNumber()));
      
      bisectormap(noofbisectors)->SecondVector
	(atool.Tangent
	 (bisectormap(noofbisectors)->LastBisector()
	  ->BisectorNumber()));
      
      noofbisectors++;
      
      theedgelist->PreviousItem()->Distance(-1);
      theedgelist->Current()->Distance(-1);

      theedgelist->PreviousItem()->FirstBisector()
        ->SecondParameter(Precision::Infinite());
      theedgelist->Current()->SecondBisector()->FirstParameter(Precision::Infinite());
    }

    //-----------------------------------------------------------------------
    // Test sur le nombre d iterations :
    // A chaque iteration est elimine un element du contour qui ne sera plus
    // reinsere par la suite => le nombre d iterartions doit etre < au nombre
    // d elements.
    // Le nombre d iteration maximum est fixe a numberofedges*numberofedges.
    //-----------------------------------------------------------------------
    if (NumberOfIte > NumberMaxOfIte) {
      isDone = Standard_False;             //Echec calcul de la carte.
      break;
    }
    NumberOfIte++;

  }  //===============================================
     //            Fin Boucle Principale.
     //===============================================
     
  //----------
  // etape 3.
  //----------


  //----------------------------------------------
  // interupt = True => bissectrices semi_infinies.
  //----------------------------------------------
  
  if(interrupt)
    semiInfinite = Standard_True;
  else {
    semiInfinite = Standard_False;

    //------------------------------------------------------------------
    // Si le nombre d edge > 1 => le nombre d edge = 2 
    //              (cf test sortie boucle principale)
    // Les deux dernieres bisectrices separent les memes edges .
    // Soit elles sont confondues si calcul a l interieur, soit elles
    // sont semi-Infinies (exemple : contour compose seulement de deux
    // arcs de cercles).			   
    //------------------------------------------------------------------

    if(theedgelist->Number() > 1) { //Now this branch is never reachable
                                    //because the case edgenumber = 2 is processed in the main loop
      theedgelist->First();
      edge = theedgelist->Current();
      if(edge->FirstBisector()->IndexNumber() == noofbisectors-1) {
//  Modified by skv - Tue Sep 13 12:13:28 2005 IDEM Begin
	if (atool.TrimBisector(edge->SecondBisector(),
			       edge->FirstBisector()->IssuePoint())) {
	  if (edge->SecondBisector()->EndPoint() == 0)
	    edge->SecondBisector()->EndPoint(edge->FirstBisector()->IssuePoint());
	  bisectormap(noofbisectors-1)->AddBisector(edge->SecondBisector());
	} else
	  semiInfinite = Standard_True;
//  Modified by skv - Tue Sep 13 12:13:28 2005 IDEM End
      }
      else {
//  Modified by skv - Tue Sep 13 12:13:28 2005 IDEM Begin
	if (atool.TrimBisector(edge->FirstBisector(),
			       edge->SecondBisector()->IssuePoint())) {
	  if (edge->FirstBisector()->EndPoint() == 0)
	    edge->FirstBisector()->EndPoint(edge->SecondBisector()->IssuePoint());
	  bisectormap(noofbisectors-1)->AddBisector(edge->FirstBisector());
	} else 
	  semiInfinite = Standard_True;
//  Modified by skv - Tue Sep 13 12:13:28 2005 IDEM End
      }
      if (!semiInfinite) {     
 	thenumberofbisectors--;
	bisectormap(noofbisectors-1)->SecondEdge(edge);
	bisectormap(noofbisectors-1)->BisectorNumber(-1);
      }
    }
  }

  if(semiInfinite) {
    beginbisector = noofbisectors;
    theedgelist->First();
    for(i=1; i<theedgelist->Number(); i++) {
      edge = theedgelist->Current();
      bisectormap.Bind(noofbisectors,edge->SecondBisector());
      noofbisectors++;
      theedgelist->Next();
    }

  }

  //---------------------------
  // Recuperations des racines.
  //---------------------------

  roots = new MAT_ListOfBisector;
  
  if (bisectormap(noofbisectors-1)->BisectorNumber() == -1) {
    roots = bisectormap(noofbisectors-1)->List();
    roots->First();
    roots->Current()->FirstEdge()
      ->Distance(bisectormap(noofbisectors-1)->DistIssuePoint());
  }
  else {
    for (i=beginbisector;i<noofbisectors;i++) {
      roots->BackAdd(bisectormap(i));
    }
  }
  
}

void MAT2d_Mat2d::CreateMat(MAT2d_Tool2d& atool)
{

#ifdef ICONTINUE
  Standard_Boolean Icontinue;
#endif

  Standard_Boolean interrupt = Standard_False;

  Handle(MAT_Edge) edgetoremove;
  Handle(MAT_Edge) previousedge,currentedge;

  Standard_Integer      noofbisectorstoremove;
  Handle(MAT_Bisector)  firstbisector,secondbisector;
  Handle(MAT_Edge)      edge;
  Standard_Integer      intersectionpoint;
  Standard_Integer      beginbisector;
  Standard_Integer      noofbisectors;

  Standard_Integer	NbIterBis = 0;
  Standard_Integer	EvenNbIterBis = 10;
  TColStd_Array1OfInteger EdgeNumbers(1, EvenNbIterBis+1);
  EdgeNumbers.Init(-1);
  Standard_Boolean	ToNullifyNoofbisectorstoremove = Standard_False;

  Handle(MAT_ListOfBisector) currentbisectorlist;

  Handle(MAT_Bisector) bisectortoremove,lastbisector,currentbisector;
  Handle(MAT_Bisector) previousbisector;

  Standard_Integer     i,j,k,narea,shift,compact,all;
  Standard_Integer     noofedges;
  Standard_Integer     NumberMaxOfIte;
  Standard_Real        toleranceofconfusion;

  noofedges            = atool.NumberOfItems();
  toleranceofconfusion = atool.ToleranceOfConfusion();
  NumberMaxOfIte       = noofedges*noofedges;

  TColStd_Array1OfInteger firstarea(0, noofedges);
  TColStd_Array1OfInteger lastarea(0, noofedges);
  TColStd_Array1OfInteger noofarea(0, noofedges);

  Standard_Integer  parama[2];
  Standard_Integer  paramb[2];
  //
  Standard_Integer aNbOfNarea1 = 0, aPrefNarea = 0, aNbMaxNarea1 = 10;
  Standard_Integer aNbElts[2] = {0, 0}, aCountElts[2] = {0, 0};
  Standard_Boolean isBreak = Standard_False;

  // -----------------------------------------
  // Initialisation et remise a zero des maps.
  // -----------------------------------------
  bisectoronetoremove.Clear();
  bisectortwotoremove.Clear();
  typeofbisectortoremove.Clear();
  bisectormap.Clear();

  isDone        = Standard_True;
  noofbisectors = noofedges;
  beginbisector = 0;

  // --------------------------------------------------------------------
  // Construction de <theedgelist> un edge correspond a un element simple
  // du contour.
  // --------------------------------------------------------------------
  theedgelist = new MAT_ListOfEdge();

  for(i=0; i<noofedges; i++) {
    edge = new MAT_Edge();
    edge->EdgeNumber(i+1);
    edge->Distance(-1);
    theedgelist->BackAdd(edge);
  }

  theedgelist->Loop();

  //---------------------------------------------------
  // Initialisation des bissectrices issues du contour.
  //---------------------------------------------------
  Standard_Real Dist;
  theedgelist->First();

  for(i=0; i<theedgelist->Number(); i++) {
    bisectormap.Bind(i,new MAT_Bisector());
    bisectormap(i)->IndexNumber(i);
    bisectormap(i)->FirstEdge(theedgelist->Current());
    bisectormap(i)->FirstVector
      (atool.TangentBefore(theedgelist->Current()->EdgeNumber(), myIsOpenResult));
    theedgelist->Next();
    bisectormap(i)->SecondEdge(theedgelist->Current());
    bisectormap(i)->IssuePoint
      (atool.FirstPoint(theedgelist->Current()->EdgeNumber(),Dist));  
    bisectormap(i)->DistIssuePoint(Dist);
    bisectormap(i)->SecondVector
      (atool.TangentAfter(theedgelist->Current()->EdgeNumber(), myIsOpenResult));
  }

  //----------------------------------------------------
  // Affectation a chaque edge de ses deux bissectrices.
  //----------------------------------------------------
  theedgelist->First();

  for(i=0; i<theedgelist->Number(); i++) {
    theedgelist->Current()->FirstBisector
      (bisectormap((i-1+noofbisectors)%noofbisectors));
    theedgelist->Current()->SecondBisector
      (bisectormap(i));
    theedgelist->Next();
  }

  //===========================================================================
  //                         Boucle Principale   (etape 2)
  //===========================================================================
  Standard_Integer NumberOfIte = 0;

  while(theedgelist->Number()>1) {


    // ------------------------------------------------------------------
    //  Creation des geometries des bissectrices via le tool. (etape 2.1)
    // -------------------------------------------------------------------
    Standard_Integer aNbBis = noofbisectors - beginbisector;
    for(i=beginbisector; i<noofbisectors; i++) {

      atool.CreateBisector(bisectormap(i));
      thenumberofbisectors++;

#ifdef OCCT_DEBUG_Mat
      atool.Dump(bisectormap(i)->BisectorNumber(),1);
#ifdef ICONTINUE
      cin>>Icontinue;
#endif
#endif
    }

    //Patch to prevent infinit loop because of
    //bad geometry
    if(aNbBis == 1)
    {
      if(aPrefNarea == 1)
      {
        aNbOfNarea1++;
        Standard_Integer edge1number  = bisectormap(beginbisector)->FirstEdge()->EdgeNumber();
        Standard_Integer edge2number  = bisectormap(beginbisector)->SecondEdge()->EdgeNumber();
        if(aNbElts[0] == edge1number)
        {
          aCountElts[0]++;
        }
        else
        {
          aCountElts[0] = 0;
          aNbElts[0] = edge1number;
        }
        if(aNbElts[1] == edge2number)
        {
          aCountElts[1]++;
        }
        else
        {
          aCountElts[1] = 0;
          aNbElts[1] = edge2number;
        }
        if(aNbOfNarea1 >= aNbMaxNarea1 && (aCountElts[0] >= aNbMaxNarea1 || aCountElts[1] >= aNbMaxNarea1))
        {
          isBreak = Standard_True;
        }
      }
      else
      {
        aNbOfNarea1 = 0;
        aCountElts[0] = 0;
        aCountElts[1] = 0;
      }
    }
    aPrefNarea = aNbBis;
    // ---------------------------------------------
    //  Condition de sortie de la boucle principale.
    // ---------------------------------------------

    //  Modified by Sergey KHROMOV - Fri Nov 17 10:28:28 2000 Begin
    if (theedgelist->Number() < 3)
      break;
    //  Modified by Sergey KHROMOV - Fri Nov 17 10:28:37 2000 End

    //---------------------------------------------------
    // boucle 2 Tant qu il y a des bisectrices a effacer.
    //---------------------------------------------------
    for(;;) {
      NbIterBis++;

      noofbisectorstoremove = 0;
      theedgelist->First();

      //--------------------------------------------------------------
      // Calcul des intersections des bisectrices voisines.(etape 2.2)
      //--------------------------------------------------------------

      if (NbIterBis <= EvenNbIterBis+1)
        EdgeNumbers(NbIterBis) = theedgelist->Number();
      else
      {
        for (k = 1; k <= EvenNbIterBis; k++)
          EdgeNumbers(k) = EdgeNumbers(k+1);
        EdgeNumbers(EvenNbIterBis+1) = theedgelist->Number();
      }
      if (EdgeNumbers(EvenNbIterBis+1) == EdgeNumbers(1))
        ToNullifyNoofbisectorstoremove = Standard_True;

      for(i=0; i<theedgelist->Number(); i++) {
        edge = theedgelist->Current();
        if(edge->Distance() == -1.) {
          firstbisector = edge->FirstBisector();
          secondbisector = edge->SecondBisector();
          edge->Distance(atool.IntersectBisector
            (firstbisector,secondbisector,intersectionpoint));
          edge->IntersectionPoint(intersectionpoint);

          if(edge->Distance() == Precision::Infinite()) {
            if(firstbisector->IndexNumber() >= beginbisector ||
              secondbisector->IndexNumber() >= beginbisector) 
              Intersect(atool,0,noofbisectorstoremove,
              firstbisector,secondbisector );
          }
          else {
            if(firstbisector->IndexNumber() >= beginbisector) {
              Intersect(atool,1,noofbisectorstoremove,
                firstbisector,secondbisector );
            }
            if(secondbisector->IndexNumber() >= beginbisector) {
              Intersect(atool,2,noofbisectorstoremove,
                firstbisector,secondbisector );
            }
          }
        }
        theedgelist->Next();
      }

      //-------------------------------
      // Test de sortie de la boucle 2.
      //-------------------------------

      if (ToNullifyNoofbisectorstoremove)
        noofbisectorstoremove = 0;
      if(noofbisectorstoremove == 0) break;

      //---------------------------------------------------
      // Annulation des bissectrices a effacer. (etape 2.4)
      //---------------------------------------------------

      for(i=0; i<noofbisectorstoremove; i++) {

        bisectortoremove = bisectoronetoremove(i);

        //---------------------------------------------------------------
        // Destruction des bisectrices descendantes de <bisectortoremove>
        // On descend dans l arbre jusqu a ce qu on atteigne
        // <bisectortwotoremove(i).
        //---------------------------------------------------------------

        for(;;){

#ifdef OCCT_DEBUG_Mat
          atool.Dump(bisectortoremove->BisectorNumber(),0);
#endif
          // ----------------------------------
          // Annulation de <bisectortoremove>.
          // ----------------------------------
          thenumberofbisectors--;
          currentbisectorlist = bisectortoremove->List();
          currentbisectorlist->First();
          currentbisector = currentbisectorlist->FirstItem();
          previousedge = currentbisector->FirstEdge();
          theedgelist->Init(previousedge);
          previousedge->Distance(-1.);
          previousedge->FirstBisector()->SecondParameter(Precision::Infinite());
          previousedge->SecondBisector()->FirstParameter(Precision::Infinite());

          //------------------------------------------
          // Annulation des fils de <currentbisector>.
          //------------------------------------------

          while(currentbisectorlist->More()) {
            currentbisector = currentbisectorlist->Current();
            currentedge  = currentbisector->SecondEdge();

            //---------------------------------------
            // Reinsertion de l edge dans le contour.
            //---------------------------------------
            theedgelist->LinkAfter(currentedge);
            theedgelist->Next();

            currentedge->FirstBisector(currentbisector);
            previousedge->SecondBisector(currentbisector);
#ifdef OCCT_DEBUG_Mat		      
            atool.Dump(currentbisector->BisectorNumber(),0);
#endif

            //------------------------------------------------------
            // Annulation de l intersection ie les fils qui
            // ont generes l intersection sont prolonges a l infini.
            //------------------------------------------------------

            currentbisector->FirstParameter (Precision::Infinite());
            currentbisector->SecondParameter(Precision::Infinite());

            atool.TrimBisector(currentbisector);

#ifdef OCCT_DEBUG_Mat
            atool.Dump(currentbisector->BisectorNumber(),1);
#endif
            currentedge->Distance(-1.);
            currentedge->FirstBisector()->SecondParameter(Precision::Infinite());
            currentedge->SecondBisector()->FirstParameter(Precision::Infinite());

            previousedge = currentedge;
            currentbisectorlist->Next();
          }

          theedgelist->Unlink();

          //-----------------------------------------------------------
          // Test de sortie de la boucle d annulation des bissectrices.
          //-----------------------------------------------------------

          if(bisectortoremove->BisectorNumber() ==
            bisectortwotoremove(i)->BisectorNumber()) break;

          //-----------------------
          // Descente dans l arbre.
          //-----------------------

          if(typeofbisectortoremove(i) == 1)
            bisectortoremove = bisectortoremove->FirstBisector();
          else
            bisectortoremove = bisectortoremove->LastBisector();

        }  //----------------------------------------------------
        // Fin boucle d annulation des bissectrices issue de 
        // <bisectoronetoremove(i)>.
        //----------------------------------------------------

      } //------------------------------------------
      // Fin boucle d annulation des bissectrices.
      //-------------------------------------------

#ifdef ICONTINUE
      cin>>Icontinue;
#endif
    } //--------------
    // Fin Boucle 2.
    //--------------

    // ----------------------------------------------------------------------
    // Analyse des parametres des intersections sur les bisectrices de chaque
    // edge et determination des portions de contour a supprimees. (etape 2.5)
    // ----------------------------------------------------------------------

    theedgelist->First();

    currentbisector = theedgelist->Current()->FirstBisector();
    if (currentbisector->FirstParameter()  == Precision::Infinite() &&
      currentbisector->SecondParameter() == Precision::Infinite()) {
        parama[0] = -1;
        paramb[0] = -1;
    }
    else if(currentbisector->FirstParameter() == Precision::Infinite()) {
      parama[0] = -1;
      paramb[0] =  1;
    }
    else if(currentbisector->SecondParameter() == Precision::Infinite()) {
      paramb[0] = -1;
      parama[0] =  1;
    }
    else if (atool.Distance(currentbisector,
      currentbisector->FirstParameter(),
      currentbisector->SecondParameter()) 
      > toleranceofconfusion) {
        if((currentbisector->FirstParameter() - 
          currentbisector->SecondParameter())
          *currentbisector->Sense() > 0.) {      
            parama[0] = -1;
            paramb[0] =  1;
        }
        else {
          paramb[0] = -1;
          parama[0] =  1;
        }
    }
    else {
      parama[0] = 1;
      paramb[0] = 1;
    }

    narea = -1;

    for(i=0; i<theedgelist->Number(); i++) {
      currentbisector = theedgelist->Current()->SecondBisector();
      if (currentbisector->FirstParameter()  == Precision::Infinite() &&
        currentbisector->SecondParameter() == Precision::Infinite()) {
          parama[1] = -1;
          paramb[1] = -1;
      }
      else if(currentbisector->FirstParameter() == Precision::Infinite()) {
        parama[1] = -1;
        paramb[1] =  1;
      }
      else if(currentbisector->SecondParameter() == Precision::Infinite()) {
        paramb[1] = -1;
        parama[1] =  1;
      }
      else if (atool.Distance(currentbisector,
        currentbisector->FirstParameter(),
        currentbisector->SecondParameter()) 
    > toleranceofconfusion) {
      if((currentbisector->FirstParameter() - 
        currentbisector->SecondParameter()) 
        *currentbisector->Sense() > 0.) {      
          parama[1] = -1;
          paramb[1] =  1;
      }
      else {
        paramb[1] = -1;
        parama[1] =  1;
      }
      }
      else {
        parama[1] = 1;
        paramb[1] = 1;
      }

      //-----------------------------------------------------------------
      // Test si l edge est a enlever du contour
      // Construction des portions de contour a eliminer.
      //
      //  narea : nombre de portions continues du contour a eliminer.
      //  firstarea[i] : indice premier edge de la portion i.
      //  lastarea[i]  : indice dernier edge de la portion i.
      //-----------------------------------------------------------------

#ifdef OCCT_DEBUG_Mat
      cout <<" Test sur les parametres pour elimination"<<endl;
      cout << " Edge number :"<<theedgelist->Current()->EdgeNumber()<<endl;
#endif

      if(paramb[0] > 0 && parama[1] > 0) {

#ifdef OCCT_DEBUG_Mat
        cout <<" A ELIMINER "<<endl;
#endif	
        if(narea < 0) {
          firstarea(++narea) = theedgelist->Index();
          lastarea(narea) = firstarea(narea);
          noofarea(narea) = 1;
        }
        else {
          if(theedgelist->Index() == lastarea(narea)+1) {
            lastarea(narea)++;
            noofarea(narea)++;
          }
          else {
            firstarea(++narea) = theedgelist->Index();
            lastarea(narea) = firstarea(narea);
            noofarea(narea) = 1;
          }
        }
      }
      parama[0] = parama[1];
      paramb[0] = paramb[1];
      theedgelist->Next();

    } 

    compact = 0;
    if(narea > 0) {
      if(lastarea(narea) == theedgelist->Number() && firstarea(0) == 1) {
        firstarea(0) = firstarea(narea);
        noofarea(0) = noofarea(0)+noofarea(narea);
        compact = noofarea(narea);
        narea--;
      }
    }

    narea++;

    //------------------------------------------------------------------
    // Sortie de la boucle principale si il n y a pas d edge a eliminer.
    // (etape 2.6)
    //------------------------------------------------------------------
    //
    //Patch to break infinite loop.
    if(narea == 1 && isBreak)
    {
      narea = 0;
    }
    //
    if(narea == 0) {
      interrupt = Standard_True;
      break;
    }


    //----------------------------------------------------------------
    // Elimination des edges a enlever du contour
    // => Mise a jour du nouveau contour.
    // => Creation des bissectrices entre les nouvelles edges voisines.
    //----------------------------------------------------------------

    beginbisector = noofbisectors;
    shift = 0;
    all = 0;
    if(narea == 1 && noofarea(0) == theedgelist->Number()) all = 1;

    for(i=0; i<narea; i++) {
      if(i == 1)shift = shift-compact;
      theedgelist->First();
      edgetoremove = theedgelist->Brackets(firstarea(i)-shift);

      edgetoremove->FirstBisector()->EndPoint(edgetoremove
        ->IntersectionPoint());

#ifdef OCCT_DEBUG_Mat
      atool.Dump(edgetoremove->FirstBisector()->BisectorNumber(),0);
#endif

      edgetoremove->FirstBisector()->FirstParameter
        (edgetoremove->FirstBisector()->SecondParameter());

#ifdef OCCT_DEBUG_Mat
      if(atool.TrimBisector(edgetoremove->FirstBisector()))
        atool.Dump(edgetoremove->FirstBisector()->BisectorNumber(),1);
#else
      atool.TrimBisector(edgetoremove->FirstBisector());
#endif

      bisectormap.Bind(noofbisectors,new MAT_Bisector());
      bisectormap(noofbisectors)->IndexNumber(noofbisectors);
      bisectormap(noofbisectors)->DistIssuePoint(edgetoremove->Distance());
      bisectormap(noofbisectors)->IssuePoint(edgetoremove
        ->IntersectionPoint());
      bisectormap(noofbisectors)->FirstEdge(theedgelist->PreviousItem());
      bisectormap(noofbisectors)->AddBisector(edgetoremove
        ->FirstBisector());

      for(j=0; j<noofarea(i); j++) {
        theedgelist->Unlink();
        theedgelist->Next();
        shift++;

#ifdef OCCT_DEBUG_Mat
        cout<<" Suppression de l'arete : "<<edgetoremove->EdgeNumber()<<endl;
#endif

        if(all == 0 || j+1 != noofarea(i)) {
          bisectormap(noofbisectors)->AddBisector(edgetoremove
            ->SecondBisector());
        }
        edgetoremove->SecondBisector()->EndPoint(edgetoremove
          ->IntersectionPoint());

#ifdef OCCT_DEBUG_Mat
        atool.Dump(edgetoremove->SecondBisector()->BisectorNumber(),0);
#endif

        edgetoremove->SecondBisector()->SecondParameter
          (edgetoremove->SecondBisector()->FirstParameter());
#ifdef OCCT_DEBUG_Mat
        if(atool.TrimBisector(edgetoremove->SecondBisector()))
          atool.Dump(edgetoremove->SecondBisector()->BisectorNumber(),1);
#else
        atool.TrimBisector(edgetoremove->SecondBisector());
#endif
        edgetoremove = theedgelist->Current();
      }
      bisectormap(noofbisectors)->SecondEdge(theedgelist->Current());

      theedgelist->PreviousItem()
        ->SecondBisector(bisectormap(noofbisectors));
      theedgelist->Current()->FirstBisector(bisectormap(noofbisectors));

      bisectormap(noofbisectors)->FirstVector
        (atool.Tangent
        (bisectormap(noofbisectors)->FirstBisector()
        ->BisectorNumber()));

      bisectormap(noofbisectors)->SecondVector
        (atool.Tangent
        (bisectormap(noofbisectors)->LastBisector()
        ->BisectorNumber()));

      noofbisectors++;

      theedgelist->PreviousItem()->Distance(-1);
      theedgelist->Current()->Distance(-1);

      theedgelist->PreviousItem()->FirstBisector()
        ->SecondParameter(Precision::Infinite());
      theedgelist->Current()->SecondBisector()->FirstParameter(Precision::Infinite());
    }

    //-----------------------------------------------------------------------
    // Test sur le nombre d iterations :
    // A chaque iteration est elimine un element du contour qui ne sera plus
    // reinsere par la suite => le nombre d iterartions doit etre < au nombre
    // d elements.
    // Le nombre d iteration maximum est fixe a numberofedges*numberofedges.
    //-----------------------------------------------------------------------
    if (NumberOfIte > NumberMaxOfIte) {
      isDone = Standard_False;             //Echec calcul de la carte.
      break;
    }
    NumberOfIte++;

  }  //===============================================
  //            Fin Boucle Principale.
  //===============================================

  //----------
  // etape 3.
  //----------


  //----------------------------------------------
  // interupt = True => bissectrices semi_infinies.
  //----------------------------------------------

  if(interrupt)
    semiInfinite = Standard_True;
  else {
    semiInfinite = Standard_False;

    //------------------------------------------------------------------
    // Si le nombre d edge > 1 => le nombre d edge = 2 
    //              (cf test sortie boucle principale)
    // Les deux dernieres bisectrices separent les memes edges .
    // Soit elles sont confondues si calcul a l interieur, soit elles
    // sont semi-Infinies (exemple : contour compose seulement de deux
    // arcs de cercles).			   
    //------------------------------------------------------------------

    if(theedgelist->Number() > 1) { //Now this branch is never reachable
      //because the case edgenumber = 2 is processed in the main loop
      theedgelist->First();
      edge = theedgelist->Current();
      if(edge->FirstBisector()->IndexNumber() == noofbisectors-1) {
        //  Modified by skv - Tue Sep 13 12:13:28 2005 IDEM Begin
        if (atool.TrimBisector(edge->SecondBisector(),
          edge->FirstBisector()->IssuePoint())) {
            if (edge->SecondBisector()->EndPoint() == 0)
              edge->SecondBisector()->EndPoint(edge->FirstBisector()->IssuePoint());
            bisectormap(noofbisectors-1)->AddBisector(edge->SecondBisector());
        } else
          semiInfinite = Standard_True;
        //  Modified by skv - Tue Sep 13 12:13:28 2005 IDEM End
      }
      else {
        //  Modified by skv - Tue Sep 13 12:13:28 2005 IDEM Begin
        if (atool.TrimBisector(edge->FirstBisector(),
          edge->SecondBisector()->IssuePoint())) {
            if (edge->FirstBisector()->EndPoint() == 0)
              edge->FirstBisector()->EndPoint(edge->SecondBisector()->IssuePoint());
            bisectormap(noofbisectors-1)->AddBisector(edge->FirstBisector());
        } else 
          semiInfinite = Standard_True;
        //  Modified by skv - Tue Sep 13 12:13:28 2005 IDEM End
      }
      if (!semiInfinite) {     
        thenumberofbisectors--;
        bisectormap(noofbisectors-1)->SecondEdge(edge);
        bisectormap(noofbisectors-1)->BisectorNumber(-1);
      }
    }
  }
  if(semiInfinite) {
    beginbisector = noofbisectors;
    theedgelist->First();
    for(i=0; i<theedgelist->Number(); i++) {
      edge = theedgelist->Current();
      bisectormap.Bind(noofbisectors,edge->SecondBisector());
      noofbisectors++;
      theedgelist->Next();
    }

  } 

  //---------------------------
  // Recuperations des racines.
  //---------------------------

  roots = new MAT_ListOfBisector;

  if (bisectormap(noofbisectors-1)->BisectorNumber() == -1) {
    roots = bisectormap(noofbisectors-1)->List();
    roots->First();
    roots->Current()->FirstEdge()
      ->Distance(bisectormap(noofbisectors-1)->DistIssuePoint());
  }
  else {
    for (i=beginbisector;i<noofbisectors;i++) {
      roots->BackAdd(bisectormap(i));
    }
  }

}

//========================================================================
//  function : LoadBisectorsToRemove
//  purpose  : Chargement des bisectrices a effacer.
//========================================================================
void MAT2d_Mat2d::LoadBisectorsToRemove
  (      Standard_Integer&     noofbisectorstoremove,
  const Standard_Real         distance1,
  const Standard_Real         distance2,
  const Handle(MAT_Bisector)& firstbisectortoremove1,
  const Handle(MAT_Bisector)& firstbisectortoremove2,
  const Handle(MAT_Bisector)& lastbisectortoremove1,
  const Handle(MAT_Bisector)& lastbisectortoremove2  )
{

  Standard_Integer found,index;
  Handle(MAT_Bisector) firstbisectortoremove[2];
  Handle(MAT_Bisector) lastbisectortoremove[2];

  firstbisectortoremove[0] = firstbisectortoremove1;
  firstbisectortoremove[1] = firstbisectortoremove2;
  lastbisectortoremove[0]  = lastbisectortoremove1;
  lastbisectortoremove[1]  = lastbisectortoremove2;

  if     (distance1  < Precision::Infinite() && 
    distance2 == Precision::Infinite()    )   index =  0;
  else if(distance2  < Precision::Infinite() && 
    distance1 == Precision::Infinite()    )   index =  1;
  else                                              index = -1;

  if(index != -1) {
    found = noofbisectorstoremove;
    for(int j=0; j<noofbisectorstoremove; j++) {
      if(bisectoronetoremove(j)->BisectorNumber() ==
        firstbisectortoremove[index]->BisectorNumber()) {
          found = j;
          if(bisectortwotoremove(j)->BisectorNumber() <
            lastbisectortoremove[index]->BisectorNumber())found = -1;
          break;
      }
    }

    if(found != -1) {
#ifdef OCCT_DEBUG_Mat
      cout<<" first last bisector to remove :"<<
        firstbisectortoremove[index]->BisectorNumber()<<" "<<
        lastbisectortoremove[index]->BisectorNumber()<<endl;
#endif
      bisectoronetoremove.Bind(found,firstbisectortoremove[index]);
      bisectortwotoremove.Bind(found,lastbisectortoremove[index]);
      typeofbisectortoremove.Bind(found,index+1);

      if(found == noofbisectorstoremove)noofbisectorstoremove++;
    }
  }
}

//========================================================================
//  function : Intersect
//  purpose  : Si <aside=0> Intersection de <firstbisector> avec les
//                 descendants de <secondbisector> les plus a gauche 
//                (ie secondbisector->FirstBisector()->FirstBisector...)
//                          Intersection de <secondbisector> avec les
//                 descendants de <firstbisector> les plus a droite 
//                (ie firstbisector->LastBisector()->LastBisector...)
//                
//             Si <aside=1> Intersection de <firstbisector> avec ses 
//                descendants les plus a gauche et les plus a droite.
//
//             Si <aside=2> Intersection de <secondbisector> avec ses 
//                descendants les plus a gauche et les plus a droite.
//========================================================================v
void MAT2d_Mat2d::Intersect(      MAT2d_Tool2d&                 atool,
  const Standard_Integer      aside,
  Standard_Integer&     noofbisectortoremove,
  const Handle(MAT_Bisector)& firstbisector,
  const Handle(MAT_Bisector)& secondbisector)
{
  Standard_Integer      bisectornumber;
  Standard_Real         distant,saveparameter;
  Standard_Real         distance[2];
  Standard_Integer      intersectionpoint;
  Handle(MAT_Bisector)  lastbisector,previousbisector;
  Handle(MAT_Bisector)  firstbisectortoremove[2];
  Handle(MAT_Bisector)  lastbisectortoremove[2];

  distance[0] = Precision::Infinite();
  distance[1] = Precision::Infinite();

  for(bisectornumber = 0; bisectornumber<2; bisectornumber++) {
    if(aside == 0) {
      if(bisectornumber == 0) 
        firstbisectortoremove[bisectornumber] = secondbisector;
      else                    
        firstbisectortoremove[bisectornumber] = firstbisector;
    }
    else if(aside == 1) {
      firstbisectortoremove[bisectornumber] = firstbisector;
    }
    else {
      firstbisectortoremove[bisectornumber] = secondbisector;
    }

    lastbisector = firstbisectortoremove[bisectornumber];

    if(aside == 0) {
      previousbisector = firstbisectortoremove[bisectornumber];
    }
    else {
      if(firstbisectortoremove[bisectornumber]->List()->IsEmpty())continue;

      if(bisectornumber == 0)
        previousbisector = firstbisectortoremove[bisectornumber]
      ->FirstBisector();
      else
        previousbisector = firstbisectortoremove[bisectornumber]
      ->LastBisector();
    }

    distant = distance[bisectornumber];
    while(!previousbisector->List()->IsEmpty()) {

      if(bisectornumber == 0)
        previousbisector = previousbisector->FirstBisector();
      else
        previousbisector = previousbisector->LastBisector();

      if(aside == 1 || (aside == 0 && bisectornumber == 0)) {
        saveparameter = previousbisector->FirstParameter();
        distant = atool.IntersectBisector
          (firstbisector,previousbisector,intersectionpoint);
        previousbisector->FirstParameter(saveparameter);
      }
      else {
        saveparameter = previousbisector->SecondParameter();
        distant = atool.IntersectBisector
          (previousbisector,secondbisector,intersectionpoint);
        previousbisector->SecondParameter(saveparameter);
      }

      if(distant < Precision::Infinite()) {
        distance[bisectornumber] = distant;
        lastbisectortoremove[bisectornumber] = lastbisector;
      }

      lastbisector = previousbisector;
    }
  }

  //---------------------------------------
  // Chargement des bissectrices a effacer.
  //---------------------------------------

  LoadBisectorsToRemove(noofbisectortoremove,
    distance[0],distance[1],
    firstbisectortoremove[0],firstbisectortoremove[1],
    lastbisectortoremove[0] ,lastbisectortoremove[1]);
}

//========================================================================
//  function : Init
//  purpose  :
//========================================================================
void MAT2d_Mat2d::Init()
{
  roots->First();
}

//========================================================================
//  function : More
//  purpose  :
//========================================================================
Standard_Boolean MAT2d_Mat2d::More() const
{
  return roots->More();
}

//========================================================================
//  function : Next
//  purpose  :
//========================================================================
void MAT2d_Mat2d::Next()
{
  roots->Next();
}

//========================================================================
//  function : Bisector 
//  purpose  :
//========================================================================
Handle(MAT_Bisector) MAT2d_Mat2d::Bisector() const
{
  return roots->Current();
}

//========================================================================
//  function : NumberOfBisectors
//  purpose  :
//========================================================================
Standard_Integer MAT2d_Mat2d::NumberOfBisectors() const
{
  return thenumberofbisectors;
}

//========================================================================
//  function : SemiInfinite
//  purpose  :
//========================================================================
Standard_Boolean MAT2d_Mat2d::SemiInfinite() const
{
  return semiInfinite;
}

//========================================================================
//  function : IsDone
//  purpose  :
//========================================================================
Standard_Boolean MAT2d_Mat2d::IsDone() const
{
  return isDone;
}

