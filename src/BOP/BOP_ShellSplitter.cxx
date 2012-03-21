// Created on: 2001-04-09
// Created by: Peter KURNEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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



#include <BOP_ShellSplitter.ixx>

#include <TColStd_SequenceOfInteger.hxx>

#include <Geom_Surface.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>

#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeShape.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>

static
  void RemoveInternals(const TopoDS_Face& ,
		       TopoDS_Face& );

static
  Standard_Boolean GetShells(TopTools_SequenceOfShape& ,
			     const TopTools_MapOfShape& ,
			     TopTools_SequenceOfShape& ,
			     TopTools_DataMapOfShapeShape& ,
			     TopTools_SequenceOfShape& ) ;

static 
  Standard_Boolean AddMultiConexityFaces(TopTools_SequenceOfShape& ,
					 const TopTools_MapOfShape& ,
					 TopTools_SequenceOfShape& ,
					 const TopTools_DataMapOfShapeShape& ,
					 const TopTools_IndexedDataMapOfShapeListOfShape& ,
					 TopTools_SequenceOfShape& );

static
   Standard_Boolean SplitShell(const TopoDS_Shell& ,
			       TopoDS_Shape& );
static 
  void CreateClosedShell(TopTools_SequenceOfShape& ,
			 const TopTools_MapOfShape& ,
			 const TopTools_IndexedDataMapOfShapeListOfShape& );
// 
//=======================================================================
// function: BOP_ShellSplitter::BOP_ShellSplitter
// purpose: 
//=======================================================================
  BOP_ShellSplitter::BOP_ShellSplitter()
:
  myIsDone(Standard_False),
  myNothingToDo(Standard_False)
{
}

//=======================================================================
// function: IsNothingToDo
// purpose: 
//=======================================================================
  Standard_Boolean BOP_ShellSplitter::IsNothingToDo()const
{
  return myNothingToDo;
}

//=======================================================================
// function: IsDone
// purpose: 
//=======================================================================
  Standard_Boolean BOP_ShellSplitter::IsDone()const
{
  return myIsDone;
}

//=======================================================================
// function: Shapes
// purpose: 
//=======================================================================
  const BOPTColStd_ListOfListOfShape& BOP_ShellSplitter::Shapes()const
{
  return myShapes;
}

//=======================================================================
// function: SetShell
// purpose: 
//=======================================================================
   void BOP_ShellSplitter::SetShell(const TopoDS_Shell& aShell)
{
  myShell=aShell;
}
//=======================================================================
// function: Shell
// purpose: 
//=======================================================================
  const TopoDS_Shell& BOP_ShellSplitter::Shell()const 
{
  return myShell;
}

//=======================================================================
// function: DoWithShell
// purpose: 
//=======================================================================
  void BOP_ShellSplitter::DoWithShell ()
{
  myFaces.Clear();

  TopExp_Explorer anExpFaces (myShell, TopAbs_FACE);
  for (; anExpFaces.More(); anExpFaces.Next()) {
    const TopoDS_Face& aF = TopoDS::Face(anExpFaces.Current());
    myFaces.Append(aF);
  }
  Do();
}

//=======================================================================
// function: DoWithListOfEdges
// purpose: 
//=======================================================================
  void BOP_ShellSplitter::DoWithListOfEdges(const TopTools_ListOfShape& aLE)
{
  myFaces.Clear();
 
  TopTools_ListIteratorOfListOfShape anItList;

  anItList.Initialize(aLE);
  for (; anItList.More(); anItList.Next()) {
    const TopoDS_Face& aF = TopoDS::Face(anItList.Value());
    myFaces.Append(aF);
  }
  Do();
}

//=======================================================================
// function: Do
// purpose: 
//=======================================================================
  void BOP_ShellSplitter::Do()
{
  myIsDone=Standard_False;
  myNothingToDo=Standard_False;
  //
  TopTools_ListIteratorOfListOfShape anItList;
  TopTools_IndexedDataMapOfShapeShape  aMFNewOld;
  TopoDS_Shell aShell;
  BRep_Builder aBB;
  //
  // insert the code about myNothingToDo
  //
  // 1. Make the formal shell  
  aBB.MakeShell(aShell);
  //
  anItList.Initialize(myFaces);
  for (; anItList.More(); anItList.Next()) {
    const TopoDS_Face& aF = TopoDS::Face(anItList.Value());
    TopoDS_Face aFNew;
    RemoveInternals (aF, aFNew);
    aMFNewOld.Add (aFNew, aF);

    aBB.Add(aShell, aFNew);
  }
  //
  // 2. Split the Shell
  
  TopoDS_Shape aShape;
  SplitShell (aShell, aShape);
  //
  // 3. Post-pro the result aShape
  //    and filling the myShapes field .
  TopExp_Explorer aShellExp(aShape, TopAbs_SHELL);
  for (; aShellExp.More(); aShellExp.Next()) {
    const TopoDS_Shape& aSh= aShellExp.Current();

    TopTools_ListOfShape aLF;
    TopExp_Explorer aFaceExp(aSh, TopAbs_FACE);
    for (; aFaceExp.More(); aFaceExp.Next()) {
      const TopoDS_Shape& aFNew= aFaceExp.Current();
      
      const TopoDS_Shape& aFOld=aMFNewOld.FindFromKey(aFNew);
      aLF.Append(aFOld);
    }
    
    if (aLF.Extent()) {
      myShapes.Append(aLF);
    }
  }
  
  myIsDone=Standard_True;
}

//=======================================================================
// function: RemoveInternals
// purpose: 
//=======================================================================
void RemoveInternals(const TopoDS_Face& aF,
		     TopoDS_Face& aFNew)
{
  BRep_Builder aBB;
  Standard_Integer iCnt;
  Standard_Real    aTol;
  

  TopLoc_Location aLoc;
  Handle(Geom_Surface) aSurface=BRep_Tool::Surface(aF, aLoc);
  aTol=BRep_Tool::Tolerance(aF);
  aBB.MakeFace (aFNew, aSurface, aLoc, aTol);
  aFNew.Orientation(aF.Orientation());

  TopExp_Explorer aFExp(aF, TopAbs_WIRE);
  for (; aFExp.More(); aFExp.Next()) {
    const TopoDS_Wire& aW= TopoDS::Wire(aFExp.Current());
    TopoDS_Wire aWNew;
    aBB.MakeWire(aWNew);
    aWNew.Orientation(aW.Orientation());

    iCnt=0;
    TopExp_Explorer aWExp(aW, TopAbs_EDGE);
    for (; aWExp.More(); aWExp.Next()) {
      const TopoDS_Edge& aE=TopoDS::Edge(aWExp.Current());
      if (aE.Orientation()!=TopAbs_INTERNAL) {
	aBB.Add(aWNew, aE);
	iCnt++;
      }
    }
    if (iCnt) {
      aBB.Add(aFNew, aWNew);
    }
  }
}

////////
//
//=======================================================================
// function : SplitShell
// purpose  : 
//=======================================================================
  Standard_Boolean SplitShell    (const TopoDS_Shell& aShellIn,
				  TopoDS_Shape& aShellsOut) 
{
  Standard_Boolean done;
  Standard_Integer i, j, aNumMultShell;

  TopTools_SequenceOfShape aSeqShells, aErrFaces, Lface;
  TopTools_DataMapOfShapeShape aMapFaceShells;
  TopTools_IndexedDataMapOfShapeListOfShape aMapEdgeFaces;
  TopTools_MapOfShape aMapMultiConnectEdges;
  TopoDS_Compound aCmpErrFaces;
  //
  done = Standard_False;
  aNumMultShell =0;
  aShellsOut = aShellIn;

  TopoDS_Iterator iter(aShellIn);
  for (; iter.More(); iter.Next()) {
    Lface.Append(iter.Value());
  }
  //
  TopExp::MapShapesAndAncestors(aShellIn, TopAbs_EDGE, TopAbs_FACE, aMapEdgeFaces);
  //
  //Finds multishared edges
  Standard_Integer aNbEdges, aNbFaces;
  aNbEdges=aMapEdgeFaces.Extent();
  for(j=1; j<=aNbEdges; j++) {
    const TopTools_ListOfShape& aLF=aMapEdgeFaces(j);
    aNbFaces=aLF.Extent();
    if(aNbFaces>2) {
      const TopoDS_Shape& aE=aMapEdgeFaces.FindKey(j);
      aMapMultiConnectEdges.Add(aE);
    }
  }
  //
  //Gets shells without taking in account of multiconnexity.
  Standard_Boolean isGetShells = Standard_True;

  while(isGetShells && Lface.Length()) {
    TopTools_SequenceOfShape aTmpSeqShells;
    Standard_Boolean bGetShells;

    bGetShells=GetShells(Lface, 
			 aMapMultiConnectEdges, 
			 aTmpSeqShells,
			 aMapFaceShells,
			 aErrFaces);
    if(bGetShells) {
      done = Standard_True;
    }
    
    isGetShells = !aTmpSeqShells.IsEmpty();
    if(isGetShells) {
      aSeqShells.Append(aTmpSeqShells);
    }
  } // while(...)
  //
  //
  Standard_Boolean aIsDone = Standard_False;
  Standard_Integer aLfaceLength, aErrFacesLength;
  
  aLfaceLength=Lface.Length();
  aNumMultShell=aSeqShells.Length();

  if(aLfaceLength  && aNumMultShell) { 
    //Crating shells in the case of compsolid
    aIsDone = AddMultiConexityFaces(Lface,
				    aMapMultiConnectEdges,
				    aSeqShells,
				    aMapFaceShells,
				    aMapEdgeFaces,
				    aErrFaces);
  }
  //
  aNumMultShell = aSeqShells.Length();
  aErrFacesLength=aErrFaces.Length();
  //
  if (aErrFacesLength)  {
    BRep_Builder B;
    TopoDS_Compound aCompShells;
    
    B.MakeCompound  (aCmpErrFaces);
    B.MakeCompound(aCompShells);

    for(j =1; j <= aErrFacesLength; j++){
      B.Add(aCmpErrFaces, aErrFaces.Value(j));
    }

    if(aNumMultShell) {
      
      if(aNumMultShell == 1) {
        aShellsOut = aSeqShells.Value(1);
        B.Add(aCompShells, aSeqShells.Value(1));

        for(j=1; j <= aErrFacesLength; j++) {
          TopoDS_Shell aSh;
          B.MakeShell(aSh);
          B.Add(aSh, aErrFaces.Value(j));
	  B.Add(aCompShells, aSh);
        }
        aShellsOut = aCompShells;
      }
      
      else {
        for(i=1; i <= aNumMultShell; i++) {
          B.Add(aCompShells, aSeqShells.Value(i));
	}

        for(j=1; j<= aErrFacesLength; j++) {
          TopoDS_Shell aSh;
          B.MakeShell(aSh);
          B.Add(aSh,aErrFaces.Value(j));
	  B.Add(aCompShells, aSh);
        }
        aShellsOut = aCompShells;
      }
    } //if(aNumMultShell)

    done = Standard_True;
    return done;
  } // if (aErrFacesLength) 
  //
  //
  if(aNumMultShell>1) {
    TopTools_SequenceOfShape OpenShells;
    
    for(i=1; i <= aSeqShells.Length(); i++) {
      TopoDS_Shape aShell = aSeqShells.Value(i);
      if(!BRep_Tool::IsClosed(aShell)) {
        OpenShells.Append(aShell);
        aSeqShells.Remove(i--);
      }
    }

    j=OpenShells.Length();
    if(j>1) {
      // Attempt of creation closed shell from open shells 
      // with taking into account multiconnexity.
      //
      CreateClosedShell(OpenShells, aMapMultiConnectEdges, aMapEdgeFaces);
      aSeqShells.Append(OpenShells);
    }
  } //if(aNumMultShell>1)
  //
  //
  j=Lface.Length();
  if(j) {
    for(i=1; i <= j; i++) {
      BRep_Builder aB;
      TopoDS_Shell OneShell;
      aB.MakeShell(OneShell);
      aB.Add(OneShell, Lface.Value(i));
      aSeqShells.Append(OneShell);
    }
  }
  //
  //
  aNumMultShell = aSeqShells.Length();
  if(!done) {
    done = (aNumMultShell>1 || aIsDone);
  }

  BRep_Builder B;
  TopoDS_Compound aCompShells;
  B.MakeCompound(aCompShells);
  for(i=1; i <= aNumMultShell; i++){
    B.Add(aCompShells, aSeqShells.Value(i));  
  }
  aShellsOut = aCompShells;
  //
  return done;
}

//=======================================================================
// function : GetShells
// purpose  : 
//=======================================================================
Standard_Boolean GetShells(TopTools_SequenceOfShape& Lface,
			   const TopTools_MapOfShape& aMapMultiConnectEdges,
			   TopTools_SequenceOfShape& aSeqShells,
			   TopTools_DataMapOfShapeShape& aMapFaceShells,
			   TopTools_SequenceOfShape& ErrFaces) 
{
  Standard_Boolean done = Standard_False;
  Standard_Integer i, j, aNbLfaceLength;

  j=Lface.Length();
  if(!j) {
    return done;
  }

  Standard_Boolean isMultiConnex;
  TopoDS_Shell nshell;
  TopTools_MapOfShape dire, reve;
  BRep_Builder B;
  TopTools_SequenceOfShape aSeqUnconnectFaces;

  B.MakeShell(nshell);
  isMultiConnex = !aMapMultiConnectEdges.IsEmpty();
  i=1; 
  j=1;
  //
  for(; i<=Lface.Length(); i++)  {
    aNbLfaceLength=Lface.Length();
    TopTools_MapOfShape dtemp, rtemp;
    Standard_Integer nbbe=0, nbe = 0;
    
    TopoDS_Face aF = TopoDS::Face(Lface.Value(i));

    TopExp_Explorer anExpe(aF, TopAbs_EDGE);
    for(; anExpe.More(); anExpe.Next()) {
      const TopoDS_Edge& aE = TopoDS::Edge(anExpe.Current());
      
      if(isMultiConnex && aMapMultiConnectEdges.Contains(aE)){
        continue;
      }
      
      if (BRep_Tool::Degenerated (aE)) {
	continue;
      }

      if (BRep_Tool::IsClosed(aE, aF)) {
	continue;
      }

      TopAbs_Orientation anEOr;
      anEOr=aE.Orientation();

      Standard_Boolean bDireContains, bReveContains;

      bDireContains=dire.Contains(aE);
      bReveContains=reve.Contains(aE);

      if((anEOr == TopAbs_FORWARD  && bDireContains) || 
	 (anEOr == TopAbs_REVERSED && bReveContains)) {
	nbbe++;
      }
      else if((anEOr == TopAbs_FORWARD  && bReveContains) ||
	      (anEOr == TopAbs_REVERSED && bDireContains)) {   
	nbe++;
      }
      
      if(bDireContains) {
	dire.Remove(aE);
      }
      else if(bReveContains) {
	reve.Remove(aE);
      }
      else {
	if(anEOr == TopAbs_FORWARD) {
	  dtemp.Add(aE);
	}
	if(anEOr == TopAbs_REVERSED) {
	  rtemp.Add(aE);
	}
      }
    } // for(; expe.More(); expe.Next())
    //
    //
    if(!nbbe && !nbe && dtemp.IsEmpty() && rtemp.IsEmpty()) {
      continue;
    }
    //
    if( nbe != 0 && nbbe != 0) {
      ErrFaces.Append(aF);
      Lface.Remove(i);
      aNbLfaceLength=Lface.Length();
      j++;
      continue;
    }
    //
    if((nbe != 0 || nbbe != 0) || j == 1) {
      TopTools_MapIteratorOfMapOfShape ite;
      if(nbbe != 0) {
	aF.Reverse();
	
	ite.Initialize(dtemp);
	for(; ite.More(); ite.Next()) {
	  reve.Add(ite.Key());
	}
	
	ite.Initialize(rtemp);
	for(; ite.More(); ite.Next()){
	  dire.Add(ite.Key());
	}
	done = Standard_True;
      }
      else {
	ite.Initialize(dtemp);
	for(; ite.More(); ite.Next()) {
	  dire.Add(ite.Key());
	}
	
	ite.Initialize(rtemp);
	for(; ite.More(); ite.Next()){
	  reve.Add(ite.Key());
	}
      }

      j++;
      B.Add(nshell, aF);
      aMapFaceShells.Bind(aF, nshell);
      Lface.Remove(i);
      aNbLfaceLength=Lface.Length();
      if(isMultiConnex && BRep_Tool::IsClosed(nshell)) {
        aSeqShells.Append(nshell);
        TopoDS_Shell nshellnext;
        B.MakeShell(nshellnext);
        nshell = nshellnext;
        j=1;
      }
      i=0;
    } // if((nbe != 0 || nbbe != 0) || j == 1)
    //
    //
    if(Lface.Length() && i == Lface.Length() && j <=2) {
      TopoDS_Iterator aItf(nshell,Standard_False);
      if(aItf.More()){
        aSeqUnconnectFaces.Append(aItf.Value());
      }
      TopoDS_Shell nshellnext;
      B.MakeShell(nshellnext);
      nshell = nshellnext;
      i=0;
      j=1;
    }
  }//for(; i<=Lface.Length(); i++) 

  Standard_Boolean isContains = Standard_False;
  j=aSeqShells.Length();
  for(i=1 ; i <= j; i++){
    isContains = nshell.IsSame(aSeqShells.Value(i));
    if (isContains) {
      break;
    }
  }

  if(!isContains) {
    Standard_Integer numFace =0;
    TopoDS_Shape aFace;
    
    TopoDS_Iterator aItf(nshell, Standard_False) ;
    for(; aItf.More(); aItf.Next()) {
      aFace = aItf.Value();
      numFace++;
    }
    
    if(numFace >1) {
      aSeqShells.Append(nshell);
    }
    else if(numFace == 1) {
      Lface.Append(aFace);
    }
  }
  
  for(i=1; i<= aSeqUnconnectFaces.Length(); i++){
    Lface.Append(aSeqUnconnectFaces);
  }
  return done;
}

//=======================================================================
// function : AddMultiConexityFaces
// purpose  : 
//=======================================================================
Standard_Boolean AddMultiConexityFaces(TopTools_SequenceOfShape& Lface,
				       const TopTools_MapOfShape& aMapMultiConnectEdges,
				       TopTools_SequenceOfShape& SeqShells,
				       const TopTools_DataMapOfShapeShape& aMapFaceShells,
				       const TopTools_IndexedDataMapOfShapeListOfShape& aMapEdgeFaces,
				       TopTools_SequenceOfShape& ErrFaces)
{
  Standard_Boolean done = Standard_False;
  BRep_Builder aB;
  Standard_Integer i1;

  for(i1 = 1 ; i1<=Lface.Length(); )  {
    TopTools_MapOfShape dire, reve;
    TopTools_IndexedMapOfShape MapOtherShells;
    Standard_Integer aNbOtherShells;

    const TopoDS_Face& aFace = TopoDS::Face(Lface.Value(i1));
    //
    //Finds shells containg multishared edges from this face
    TopExp_Explorer aExpEdges(aFace, TopAbs_EDGE);
    for(; aExpEdges.More(); aExpEdges.Next()) {
      const TopoDS_Shape& aE = aExpEdges.Current();
      
      if(!aMapMultiConnectEdges.Contains(aE)) {
	continue;
      }

      if( aE.Orientation() == TopAbs_FORWARD) {
	dire.Add(aE);
      }
      else {
	reve.Add(aE);
      }

      const TopTools_ListOfShape& aLF = aMapEdgeFaces.FindFromKey(aE);
      TopTools_ListIteratorOfListOfShape aItl(aLF);
      for(; aItl.More(); aItl.Next()) {
        const TopoDS_Shape& aF = aItl.Value();
        
	if(aF.IsSame(aFace)) {
	  continue;
	}

        TopoDS_Shape aOthershell;
        if(aMapFaceShells.IsBound(aF)) {
          aOthershell = aMapFaceShells.Find(aF);
          if(!MapOtherShells.Contains(aOthershell)) {
            MapOtherShells.Add(aOthershell);
	  }
        }
      }
    }//for(; aExpEdges.More(); aExpEdges.Next())
    //
    //
    aNbOtherShells=MapOtherShells.Extent();
    //
    if(!aNbOtherShells) {
      i1++;
      continue;
    }
    
    else {
      //Adds face to open shells containg the same multishared edges.
      //For nonmanifold mode creation ine shell from face and shells 
      // containing the same multishared edges.
      done = Standard_True;
      
      Standard_Integer j, k;
      
      TColStd_SequenceOfInteger SeqOtherShells;
      for(j =1; j <= aNbOtherShells; j++) {
        Standard_Integer index=0;
        for(k =1; k <= SeqShells.Length() && !index; k++) {
          if(SeqShells.Value(k) == MapOtherShells.FindKey(j)){
            index = k;
	  }
	}
        SeqOtherShells.Append(index);
      }

      aNbOtherShells= SeqOtherShells.Length();

      for(j =1; j <= aNbOtherShells; j++) {
        Standard_Integer nbdir =0,nbrev =0;
        TopTools_MapOfShape mapEdges;
	
	k = SeqOtherShells.Value(j);
	const TopoDS_Shape& aShk=SeqShells.Value(k);
	
	TopExp_Explorer aExpF(aShk, TopAbs_FACE);
        for(; aExpF.More(); aExpF.Next()) {
	  const TopoDS_Shape& aFC=aExpF.Current();
	  
	  TopExp_Explorer aExpE(aFC,TopAbs_EDGE);
          for(; aExpE.More(); aExpE.Next()) {
            
	    const TopoDS_Shape& aEC = aExpE.Current();
            if(!mapEdges.Contains(aEC)){
              mapEdges.Add(aEC);
	    }
            else {
	      mapEdges.Remove(aEC);
	    }

          }// for(; aExpE.More(); aExpE.Next())
        }// for(; aExpF.More(); aExpF.Next()) {
	//
	//
	TopTools_MapIteratorOfMapOfShape aIte(mapEdges);
        for(;aIte.More(); aIte.Next()) {
          const TopoDS_Shape& aEC = aIte.Key();
	  TopAbs_Orientation anOrEC=aEC.Orientation();
	  
	  Standard_Boolean bDireContains, bReveContains;

	  bDireContains=dire.Contains(aEC);
	  bReveContains=reve.Contains(aEC);

          if((anOrEC == TopAbs_FORWARD  && bDireContains) || 
	     (anOrEC == TopAbs_REVERSED && bReveContains)) {
	    nbrev++;
	  }
          else if((anOrEC == TopAbs_FORWARD  && bReveContains)||
		  (anOrEC == TopAbs_REVERSED && bDireContains)) {
	    nbdir++;
	  }
        }// for(;aIte.More(); aIte.Next())

        if(nbdir && nbrev) {
          ErrFaces.Append(aFace);
	}

        else if(nbdir || nbrev) { 
	  // for manifold mode face containing multiconnexity 
	  // edges will be added in the each shell
	  // containing the same edges. ???
	  
	  TopoDS_Shape aShell;
	  aShell = SeqShells.Value(k);
	  if (!nbrev) {
	    aB.Add(aShell, aFace);
	    SeqShells.ChangeValue(k) = aShell;
	  }
        }// else if(nbdir || nbrev)
      }// for(j =1; j <= aNbOtherShells; j++)
      //
      dire.Clear();
      reve.Clear();
      Lface.Remove(i1);
    }
  }
  return done;
}
//=======================================================================
// function : CreateClosedShell
// purpose  : 
//=======================================================================
void CreateClosedShell(TopTools_SequenceOfShape& OpenShells,
		       const TopTools_MapOfShape& aMapMultiConnectEdges,
		       const TopTools_IndexedDataMapOfShapeListOfShape& aMapEdgeFaces)
{
  TopTools_MapOfShape amapFaces;
  
  TopTools_MapIteratorOfMapOfShape aItEdg(aMapMultiConnectEdges);
  for(; aItEdg.More(); aItEdg.Next()) {
    const TopTools_ListOfShape& aLF = aMapEdgeFaces.FindFromKey(aItEdg.Key());
    TopTools_ListIteratorOfListOfShape aItF(aLF);
    for(; aItF.More(); aItF.Next()) {
      amapFaces.Add(aItF.Value());
    }
  }
  //
  // Creating new shells if some open shells contain the same edges.
  Standard_Integer i, j;
  Standard_Boolean isClosedShell; 

  for(i=1; i <= OpenShells.Length(); i++)  {
    TopTools_MapOfShape dire, reve;

    isClosedShell = Standard_False;
    const TopoDS_Shape& anOpenShelli=OpenShells.Value(i);
    TopExp_Explorer aExpF(anOpenShelli, TopAbs_FACE);
    
    for(; aExpF.More(); aExpF.Next()) {
      const TopoDS_Shape& aFace = aExpF.Current();
      
      if(!amapFaces.Contains(aFace)) {
	continue;
      }

      TopExp_Explorer aExpEdges(aFace, TopAbs_EDGE);
      for(; aExpEdges.More(); aExpEdges.Next()) {
        const TopoDS_Shape& aE = aExpEdges.Current();
        
	if(!aMapMultiConnectEdges.Contains(aE)) {
	  continue;
	}
	
	TopAbs_Orientation anOrE;
	anOrE=aE.Orientation();
	
        if(anOrE == TopAbs_FORWARD) {
	  dire.Add(aE);
	}
        else if(anOrE == TopAbs_REVERSED) {
	  reve.Add(aE);
	}
      }
    }// for(; aExpF.More(); aExpF.Next())
    //
    // 
    for(j=i+1; j<=OpenShells.Length(); j++)  {
      Standard_Integer nbedge =0;
      Standard_Boolean isReversed = Standard_False;
      
      const TopoDS_Shape& anOpenShellj=OpenShells.Value(j);

      TopExp_Explorer aExpF2(anOpenShellj, TopAbs_FACE);
      for(; aExpF2.More() && !nbedge; aExpF2.Next()) {

	const TopoDS_Shape& aFace2 = aExpF2.Current();
        
	if(!amapFaces.Contains(aFace2)) {
	  continue;
	}

        TopExp_Explorer aExpEdges2(aFace2, TopAbs_EDGE);
        for(; aExpEdges2.More()&& !nbedge; aExpEdges2.Next()) {
          const TopoDS_Shape& aE2 = aExpEdges2.Current();

          if(!aMapMultiConnectEdges.Contains(aE2)) {
	    continue;
	  }

	  Standard_Boolean bDireContains, bReveContains;

	  bDireContains=dire.Contains(aE2);
	  bReveContains=reve.Contains(aE2);

          if(!bDireContains && !bReveContains) {
	    continue;
	  }

          isClosedShell = Standard_True;

	  TopAbs_Orientation anOrE2;
	  anOrE2=aE2.Orientation();
          if((anOrE2 == TopAbs_FORWARD  && bDireContains) || 
	     (anOrE2 == TopAbs_REVERSED && bReveContains)) {
            isReversed = Standard_True;
	  }
          nbedge++;
        }
      }// for(; aExpF2.More() && !nbedge; aExpF2.Next())
      
      if(!isClosedShell){
	continue;
      }

      BRep_Builder aB;
      TopoDS_Shape aShell = OpenShells.Value(i);

      TopExp_Explorer aExpF21(anOpenShellj, TopAbs_FACE);
      for(; aExpF21.More(); aExpF21.Next()) {
        const TopoDS_Shape& aFace = aExpF21.Current();
        //if(isReversed) {
        //  aFace.Reverse();
	//}
        aB.Add(aShell, aFace);
      }
      
      OpenShells.ChangeValue(i) = aShell;
      OpenShells.Remove(j--);
    }// for(j=i+1 ; j<=OpenShells.Length();j++ )
  }//for(i=1; i <= OpenShells.Length(); i++)
}
