// Created on: 1993-03-08
// Created by: Didier PIFFAULT
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


#include <Bnd_Array1OfBox.hxx>
#include <Bnd_BoundSortBox2d.hxx>
#include <Bnd_Box2d.hxx>
#include <Standard_MultiplyDefined.hxx>
#include <Standard_NullValue.hxx>
#include <TColStd_Array1OfListOfInteger.hxx>
#include <TColStd_DataMapIteratorOfDataMapOfIntegerInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

//=======================================================================
//function : Bnd_BoundSortBox2d
//purpose  : 
//=======================================================================
Bnd_BoundSortBox2d::Bnd_BoundSortBox2d()
     : discrX(0), discrY(0)
{}


//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void Bnd_BoundSortBox2d::Initialize(const Bnd_Box2d& CompleteBox,
				    const Handle(Bnd_HArray1OfBox2d)& SetOfBox)
{
  myBox=CompleteBox;
  myBndComponents=SetOfBox;
  discrX=SetOfBox->Length();
  discrY=discrX;

  Standard_Real xmin, ymin, xmax, ymax;
  Standard_Real middleX=0.;
  Standard_Real middleY=0.;

  const Bnd_Array1OfBox2d & taBox=myBndComponents->Array1();

  Standard_Integer labox;
  for (labox=taBox.Lower(); labox<=taBox.Upper(); labox++) {
    if (!taBox(labox).IsVoid()) {
      taBox.Value(labox).Get(xmin, ymin, xmax, ymax);
      middleX+=xmax-xmin;
      middleY+=ymax-ymin;
    }
  }
  middleX=middleX/taBox.Length();
  middleY=middleY/taBox.Length();

  Standard_Real Xmax, Ymax;
  CompleteBox.Get(Xmin, Ymin, Xmax, Ymax);
  deltaX=(Xmax-Xmin)/(Standard_Real)discrX;
  deltaY=(Ymax-Ymin)/(Standard_Real)discrY;

  if (middleX < Epsilon(100.)) {
    discrX=1;
    deltaX=Xmax-Xmin;
  }
  else if (middleX > deltaX) {
    discrX=(Standard_Integer)((Xmax-Xmin)/middleX);
    deltaX=middleX;
    discrX++;
  }

  if (middleY < Epsilon(100.)) {
    discrY=1;
    deltaY=Ymax-Ymin;
  }
  else if (middleY > deltaY) {
    discrY=(Standard_Integer)((Ymax-Ymin)/middleY + 0.1);
    deltaY=middleY;
    discrY++;
  }

  SortBoxes();
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void  Bnd_BoundSortBox2d::Initialize(const Handle(Bnd_HArray1OfBox2d)& SetOfBox)
{
  myBndComponents=SetOfBox;
  discrX=SetOfBox->Length();
  discrY=discrX;
  
  Standard_Real xmin, ymin, xmax, ymax;
  Standard_Real middleX=0.;
  Standard_Real middleY=0.;
  
  const Bnd_Array1OfBox2d & taBox=myBndComponents->Array1();
  
  Standard_Integer labox;
  for (labox=taBox.Lower(); labox<=taBox.Upper(); labox++) {
    if (!taBox(labox).IsVoid()) {
      myBox.Add(taBox(labox));
      taBox.Value(labox).Get(xmin, ymin, xmax, ymax);
      middleX+=xmax-xmin;
      middleY+=ymax-ymin;
    }
  }
  middleX=middleX/taBox.Length();
  middleY=middleY/taBox.Length();

  Standard_Real Xmax, Ymax;
  myBox.Get(Xmin, Ymin, Xmax, Ymax);
  deltaX=(Xmax-Xmin)/(Standard_Real)discrX;
  deltaY=(Ymax-Ymin)/(Standard_Real)discrY;

  if (middleX < Epsilon(100.)) {
    discrX=1;
    deltaX=Xmax-Xmin;
  }
  else if (middleX > deltaX) {
    discrX=(Standard_Integer)((Xmax-Xmin)/middleX);
    deltaX=middleX;
    discrX++;
  }

  if (middleY < Epsilon(100.)) {
    discrY=1;
    deltaY=Ymax-Ymin;
  }
  else if (middleY > deltaY) {
    discrY=(Standard_Integer)((Ymax-Ymin)/middleY + 0.1);
    deltaY=middleY;
    discrY++;
  }

  SortBoxes();
}


//=======================================================================
//function : SortBoxes
//purpose  : 
//=======================================================================

void  Bnd_BoundSortBox2d::SortBoxes()
{
  Standard_NullValue_Raise_if (discrX+discrY <=0, "BoundSortBox2d nul!");

  Standard_Integer labox, lacase, firstcase, lastcase;
  Standard_Real xmin, ymin, xmax, ymax;
  const  Bnd_Array1OfBox2d & taBox=myBndComponents->Array1();

  axisX=new TColStd_HArray1OfListOfInteger(1, discrX); 
  TColStd_Array1OfListOfInteger & tabListX=axisX->ChangeArray1();

  axisY=new TColStd_HArray1OfListOfInteger(1, discrY);
  TColStd_Array1OfListOfInteger & tabListY=axisY->ChangeArray1();

  for (labox=taBox.Lower(); labox<=taBox.Upper(); labox++) {
    if (!taBox(labox).IsVoid()) {
      taBox(labox).Get(xmin, ymin, xmax, ymax);

      if (discrX>1) {
	firstcase=(Standard_Integer ) Max(1.0, (xmin-Xmin)/deltaX);
	lastcase=(Standard_Integer ) Min((Standard_Real)discrX, ((xmax-Xmin)/deltaX)+1);
	for (lacase=firstcase; lacase<=lastcase; lacase++) {
	  tabListX(lacase).Append(labox);
	}
      }

      if (discrY >1) {
	firstcase=(Standard_Integer ) Max(1.0, (ymin-Ymin)/deltaY);
	lastcase=(Standard_Integer ) Min((Standard_Real)discrY, ((ymax-Ymin)/deltaY)+1);
	for (lacase=firstcase; lacase<=lastcase; lacase++) {
	  tabListY(lacase).Append(labox);
	}
      }
    }
  }
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void  Bnd_BoundSortBox2d::Initialize(const Bnd_Box2d& CompleteBox,
				     const Standard_Integer nbComponents)
{
  Standard_NullValue_Raise_if (nbComponents <=0, "BoundSortBox nul!");

  myBox=CompleteBox;
  myBndComponents=new Bnd_HArray1OfBox2d(1, nbComponents);

  Bnd_Box2d emptyBox; 
  myBndComponents->Init( emptyBox ); 

  discrX=nbComponents;
  discrY=nbComponents;
  
  Standard_Real Xmax, Ymax;
  CompleteBox.Get(Xmin, Ymin, Xmax, Ymax);
  
  deltaX=(Xmax-Xmin)/(Standard_Real)discrX;
  deltaY=(Ymax-Ymin)/(Standard_Real)discrY;
  
  if (deltaX < Epsilon(100.)) {
    discrX=1;
    deltaX=Xmax-Xmin;
  }
  else axisX=new TColStd_HArray1OfListOfInteger(1, discrX); 

  if (deltaY < Epsilon(100.)) {
    discrY=1;
    deltaY=Ymax-Ymin;
  }
  else axisY=new TColStd_HArray1OfListOfInteger(1, discrY);
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void  Bnd_BoundSortBox2d::Add(const Bnd_Box2d& theBox,
			      const Standard_Integer boxIndex)
{
  Standard_MultiplyDefined_Raise_if
    (!(myBndComponents->Value(boxIndex).IsVoid()),
     " This box is already defined !");

  if (!theBox.IsVoid()) {
    Bnd_Array1OfBox2d & taBox=myBndComponents->ChangeArray1();
    Standard_Integer theGap, firstGap , lastGap;
    Standard_Real xmin, ymin, xmax, ymax;
    theBox.Get(xmin, ymin, xmax, ymax);

    if (taBox.Lower()<=boxIndex  && boxIndex<=taBox.Upper()) 
      taBox(boxIndex).Update(xmin, ymin, xmax, ymax);

    TColStd_Array1OfListOfInteger & tabListX=axisX->ChangeArray1();

    if (discrX>1) {
      firstGap=(Standard_Integer ) Max(1.0, ((xmin-Xmin)/deltaX)+1);
      lastGap=(Standard_Integer ) Min((Standard_Real)discrX, ((xmax-Xmin)/deltaX)+1);
      for (theGap=firstGap; theGap<=lastGap; theGap++) {
	tabListX(theGap).Append(boxIndex);
      }
    }

    TColStd_Array1OfListOfInteger & tabListY=axisY->ChangeArray1();

    if (discrY >1) {
      firstGap=(Standard_Integer ) Max(1.0, ((ymin-Ymin)/deltaY)+1);
      lastGap=(Standard_Integer ) Min((Standard_Real)discrY, ((ymax-Ymin)/deltaY)+1);
      for (theGap=firstGap; theGap<=lastGap; theGap++) {
	tabListY(theGap).Append(boxIndex);
      }
    }
  }
}


//=======================================================================
//function : Compare
//purpose  : 
//=======================================================================


const TColStd_ListOfInteger& Bnd_BoundSortBox2d::Compare
  (const Bnd_Box2d& theBox)
{
  Standard_NullValue_Raise_if (discrX+discrY <=0, 
			       "Compare sur 1 BoundSortBox2d nul!");

  lastResult.Clear();
  if (theBox.IsVoid()) return lastResult;
  if (theBox.IsOut(myBox)) return lastResult;

  Standard_Integer lacase, firstcase, lastcase;
  Standard_Real xmin, ymin, xmax, ymax;
  theBox.Get(xmin, ymin, xmax, ymax);

  const Bnd_Array1OfBox2d & taBox=myBndComponents->Array1();
  Crible.Clear();
  theFound=2;

  Standard_Integer cardY=0;
  if (discrY>1 && (!theBox.IsOpenYmin() || !theBox.IsOpenYmax())) {
    const TColStd_Array1OfListOfInteger & tabList=axisY->Array1();
    firstcase=(Standard_Integer ) Max(1.0, (ymin-Ymin)/deltaY);
    lastcase=(Standard_Integer ) Min((Standard_Real)discrY, ((ymax-Ymin)/deltaY)+1);
    for (lacase=firstcase; lacase<=lastcase; lacase++) {
      TColStd_ListIteratorOfListOfInteger theList(tabList(lacase));
      for (; theList.More(); theList.Next()) {
	cardY++;
	Crible.Bind(theList.Value(), 2);
      }
    }
    if (cardY==0) return lastResult;
  }
  else {
    if (ymin > Ymin+deltaY || ymax < Ymin)
      return lastResult;
    theFound-=2;
  }

  if (discrX>1 && (!theBox.IsOpenXmin() || !theBox.IsOpenXmax())) {
    const TColStd_Array1OfListOfInteger & tabList=axisX->Array1();
    firstcase=(Standard_Integer ) Max(1.0, (xmin-Xmin)/deltaX);
    lastcase=(Standard_Integer ) Min((Standard_Real)discrX, ((xmax-Xmin)/deltaX)+1);
    for (lacase=firstcase; lacase<=lastcase; lacase++) {
      TColStd_ListIteratorOfListOfInteger theList(tabList(lacase));
      for (; theList.More(); theList.Next()) {
	if (Crible.IsBound(theList.Value())) {
	  if (Crible(theList.Value())==theFound) {
	    if (!taBox.Value(theList.Value()).IsOut(theBox)){
	      lastResult.Append(theList.Value());
	      Crible(theList.Value())=0;
	    }
	  }
	}
      }
    }
    return lastResult;
  }

  else {
    if (xmin > Xmin+deltaX || xmax < Xmin) return lastResult;
    else if (discrY==1)
      {
	lacase=1;
	for(Standard_Integer i=taBox.Lower();i<=taBox.Upper();i++)
	  {
	    lastResult.Append(i);
	  } 
      }
    else{

      TColStd_DataMapIteratorOfDataMapOfIntegerInteger itDM(Crible);
      for (; itDM.More(); itDM.Next()) {
	if (itDM.Value()==theFound) {
	  if (taBox.Lower()<=itDM.Key() && itDM.Key()<=taBox.Upper()) {
	    if (!taBox(itDM.Key()).IsOut(theBox)) 
	      lastResult.Append(itDM.Key());
	  }
	  else {
	    lastResult.Append(itDM.Key());
	  }
	}
      }
    }
  }
  return lastResult;
}


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Bnd_BoundSortBox2d::Dump() const
{
  Standard_Integer lacase;

  cout <<   "axis X : " << discrX << " intervalles de " << deltaX << endl;
  if (discrX>1) {
    const TColStd_Array1OfListOfInteger & tabList=axisX->Array1();
    for (lacase=1; lacase<=discrX; lacase++) {
      cout << "     X " << lacase << " : " ;
      TColStd_ListIteratorOfListOfInteger theList(tabList(lacase));
      for (; theList.More(); theList.Next()) {
	cout << theList.Value() << " ";
      }
      cout << "\n";
    }
  }

  cout <<   "axis Y : " << discrY << " intervalles de " << deltaY << endl;
  if (discrY>1) {
    const TColStd_Array1OfListOfInteger & tabList=axisY->Array1();
    for (lacase=1; lacase<=discrY; lacase++) {
      cout << "     Y " << lacase << " : " ;
      TColStd_ListIteratorOfListOfInteger theList(tabList(lacase));
      for (; theList.More(); theList.Next()) {
	cout << theList.Value() << " ";
      }
      cout << "\n";
    }
  }
}
