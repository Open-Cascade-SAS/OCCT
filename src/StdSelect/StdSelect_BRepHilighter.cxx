// Created on: 1995-03-22
// Created by: Robert COUBLANC
// Copyright (c) 1995-1999 Matra Datavision
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




#include <StdSelect_BRepHilighter.ixx>
#include <StdSelect_BRepOwner.hxx>
#include <StdSelect_Prs.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <Graphic3d_Structure.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <Prs3d_PointAspect.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <Prs3d_Presentation.hxx>
#include <StdPrs_WFShape.hxx>
#include <OSD_Chronometer.hxx>
#include <Visual3d_TransientManager.hxx>
static TColStd_MapIteratorOfMapOfInteger it;
static Standard_Boolean ImmediateIsEnable;
static  Standard_Boolean ImmediateIsEnableIsDef= Standard_False;
//==================================================
// Function: 
// Purpose :
//==================================================

StdSelect_BRepHilighter::
StdSelect_BRepHilighter():mylastindex(0)
,myManager(new Visual3d_TransientManager){}

//==================================================
// Function: 
// Purpose :
//==================================================

StdSelect_BRepHilighter::
StdSelect_BRepHilighter(const Handle(StdSelect_ViewerSelector3d)& aSelector,
			const Handle(V3d_Viewer)& aViewer,
			const Quantity_NameOfColor aColor,
			const StdSelect_TypeOfResult aTOR):
myselector(aSelector),
myviewer(aViewer),
mycolor(aColor),
mydrwr(new Prs3d_Drawer()),
mytype(aTOR),
mylastindex(0),
myManager(new Visual3d_TransientManager)

{
  Handle(Prs3d_PointAspect) PA = new Prs3d_PointAspect(Aspect_TOM_O,Quantity_NOC_INDIANRED3,3.);
  mydrwr->SetPointAspect(PA);
}
//==================================================
// Function: 
// Purpose :
//==================================================

void StdSelect_BRepHilighter::
Set(const Handle(StdSelect_ViewerSelector3d)& aSelector)
{myselector=aSelector;}
//==================================================
// Function: 
// Purpose :
//==================================================

void StdSelect_BRepHilighter::
Set(const StdSelect_TypeOfResult aTOR)
{mytype =aTOR;}

//==================================================
// Function: Set
// Purpose :
//==================================================

void StdSelect_BRepHilighter::
Set(const Handle(V3d_Viewer)& aViewer)
{myviewer=aViewer;}
//==================================================
// Function: 
// Purpose :
//==================================================

void StdSelect_BRepHilighter::
Set(const Quantity_NameOfColor aColor)
{mycolor=aColor;} 
//==================================================
// Function: 
// Purpose :
//==================================================
void StdSelect_BRepHilighter::
Process()
{
  Update();
  myviewer->Update();
}
//==================================================
// Function: 
// Purpose :
//==================================================
void StdSelect_BRepHilighter::
Process(const Handle(V3d_View)& aViou,const Standard_Boolean DoubleBuffer)
{
  if(aViou->Viewer() == myviewer){
    Update(aViou,DoubleBuffer);
//    aViou->Update();
  }
}

//==================================================
// Function: 
// Purpose :
//==================================================

void StdSelect_BRepHilighter::
Update()
{
  myselector->Init();
  
  switch(mytype){
  case StdSelect_TOR_MULTIPLE:{
    TColStd_MapIteratorOfMapOfInteger it;
    if(!myselector->More())
      {
	for(it.Initialize(myold);it.More();it.Next())
	  {myhimap.FindFromIndex(it.Key())->UnHighlight();
	   myhimap.FindFromIndex(it.Key())->SetVisible(Standard_False);
	 }
	myold.Clear();
	mynew.Clear();
      }
    else{

      for (;myselector->More();myselector->Next())
	{
	  const Handle(SelectBasics_EntityOwner)& OO = myselector->Picked();
	  if (!myhimap.Contains(OO))
	    {
	      if(!Handle(StdSelect_BRepOwner)::DownCast(OO).IsNull()){
		Handle(StdSelect_Prs) NiouPrs = new StdSelect_Prs
		  (myviewer->Viewer());
		
		StdPrs_WFShape::Add(NiouPrs,
				    Handle(StdSelect_BRepOwner)::DownCast(OO)->Shape(),
				    mydrwr);
		NiouPrs->Display();
		myhimap.Add(OO,NiouPrs);
	      mynew.Add(myhimap.FindIndex(OO));
	      if(myold.Contains(myhimap.FindIndex(OO))) 
		myold.Remove(myhimap.FindIndex(OO));
	      }
	    }
	}
      for(it.Initialize(myold);it.More();it.Next()){
	myhimap.FindFromIndex(it.Key())->UnHighlight();
	myhimap.FindFromIndex(it.Key())->SetVisible(Standard_False);
      }
      
      myold = mynew;
      mynew.Clear();
      for(it.Initialize(myold);it.More();it.Next()){
	myhimap.FindFromIndex(it.Key())->SetVisible(Standard_True);
	myhimap.FindFromIndex(it.Key())->SetDisplayPriority(10);
	myhimap.FindFromIndex(it.Key())->Color(mycolor);
      }
    }
    break;
  }
    
  case StdSelect_TOR_SIMPLE:
    {
      if(myselector->More()){
	const Handle(SelectBasics_EntityOwner)& OO = myselector->OnePicked();    
	if(!Handle(StdSelect_BRepOwner)::DownCast(OO).IsNull()){
	  if (!myhimap.Contains(OO)){    
	    Handle(StdSelect_Prs) NiouPrs = new StdSelect_Prs
	      (myviewer->Viewer());
	    StdPrs_WFShape::Add(NiouPrs,
				Handle(StdSelect_BRepOwner)::DownCast(OO)->Shape(),
				mydrwr);
	    NiouPrs->Display();
	    myhimap.Add(OO,NiouPrs);
	  } 
	}
	if(mylastindex!=myhimap.FindIndex(OO))
	  {
	    if(mylastindex!=0) 
	      {
		myhimap.FindFromIndex(mylastindex)->UnHighlight();
		myhimap.FindFromIndex(mylastindex)->SetVisible(Standard_False);
	      }
	    mylastindex = myhimap.FindIndex(OO);
	    myhimap.FindFromIndex(mylastindex)->SetVisible(Standard_True);
	    myhimap.FindFromIndex(mylastindex)->SetDisplayPriority(10);
	    myhimap.FindFromIndex(mylastindex)->Color(mycolor);
	  }
      }
      else {   
	if(mylastindex!=0) 
	      {
		myhimap.FindFromIndex(mylastindex)->UnHighlight();
		myhimap.FindFromIndex(mylastindex)->SetVisible(Standard_False);
		mylastindex=0;
	      }
	for (it.Initialize(myold);it.More();it.Next()){
	  myhimap.FindFromIndex(it.Key())->UnHighlight();
	  myhimap.FindFromIndex(it.Key())->SetVisible(Standard_False);}
	myold.Clear();
      }
    }
  }
}

void StdSelect_BRepHilighter::
Update(const Handle(V3d_View)& aView,const Standard_Boolean DoubleBuffer)
{
  if(!ImmediateIsEnableIsDef) {
    ImmediateIsEnable = aView->TransientManagerBeginDraw();
    ImmediateIsEnableIsDef = Standard_True;
    Visual3d_TransientManager::EndDraw();
  }
  if(!ImmediateIsEnable) {
    Update();
  }
  else {
    myselector->Init();
    mynew.Clear();
  
    switch(mytype){
    case StdSelect_TOR_MULTIPLE:{
      
      for (;myselector->More();myselector->Next()) {
	const Handle(SelectBasics_EntityOwner)& OO = myselector->Picked();
	if(!Handle(StdSelect_BRepOwner)::DownCast(OO).IsNull()){
	  if (!myhimap.Contains(OO)) {
	    Handle(StdSelect_Prs) NiouPrs = new StdSelect_Prs(myviewer->Viewer());
	    
	    StdPrs_WFShape::Add(NiouPrs,Handle(StdSelect_BRepOwner)::DownCast(OO)->Shape(),mydrwr);
	    NiouPrs->Color(mycolor);
	    myhimap.Add(OO,NiouPrs);
	  }
	  mynew.Add(myhimap.FindIndex(OO));
	}
      }
      break;
    }
      
    case StdSelect_TOR_SIMPLE: {
      
      if(myselector->More()){
	const Handle(SelectBasics_EntityOwner)& OO = myselector->OnePicked();    
	if(!Handle(StdSelect_BRepOwner)::DownCast(OO).IsNull()){
	  if (!myhimap.Contains(OO)){    
	    Handle(StdSelect_Prs) NiouPrs = new StdSelect_Prs(myviewer->Viewer());
	    StdPrs_WFShape::Add(NiouPrs,Handle(StdSelect_BRepOwner)::DownCast(OO)->Shape(),mydrwr);
	    NiouPrs->Color(mycolor);
	    myhimap.Add(OO,NiouPrs);
	  }
	  mynew.Add(myhimap.FindIndex(OO));
	}
      }
    }
      break;
    } 
    Standard_Boolean First = Standard_True; // to avoid begindraw if there is nothing (expensive)
    
    for(it.Initialize(mynew);it.More();it.Next()){
      
      if(First) {
	aView->TransientManagerBeginDraw(DoubleBuffer);
	First = Standard_False;
      }
      
      Visual3d_TransientManager::DrawStructure(myhimap.FindFromIndex(it.Key()));
    }
    
    if(!First) 
      Visual3d_TransientManager::EndDraw();
  }
}

void StdSelect_BRepHilighter::Clear()
{
  if(!myhimap.IsEmpty()){
    for (Standard_Integer i=1;i<=myhimap.Extent();i++){
      myhimap.FindFromIndex(i)->UnHighlight();
      myhimap.FindFromIndex(i)->Erase();
      myhimap.FindFromIndex(i)->Clear();
    }
    myold.Clear();
    mynew.Clear();
    mylastindex=0;   
  }
  myhimap.Clear();
}

const Handle(Prs3d_Drawer)& StdSelect_BRepHilighter::Drawer() const 
{
  return mydrwr;
}

