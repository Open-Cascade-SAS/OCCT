// Copyright:   Matra-Datavision 1995
// File:        SelectMgr_ViewerSelector.cxx
// Created:     Wed Feb 15 13:54:24 1995
// Author:      Roberc Coublanc
//              <rob>
// Modified by  ...
//              ROB JAN/07/98 : Improve Storage of detected entities
//              AGV OCT/23/03 : Optimize the method SortResult() (OCC4201)

#include <SelectMgr_ViewerSelector.ixx>
#include <SelectMgr_CompareResults.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Lin.hxx>
#include <Bnd_HArray1OfBox2d.hxx>
#include <Bnd_Array1OfBox2d.hxx>
#include <Precision.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TCollection_AsciiString.hxx>
#include <SelectBasics_ListIteratorOfListOfBox2d.hxx>
#include <SelectBasics_SensitiveEntity.hxx>
#include <SelectBasics_EntityOwner.hxx>
#include <SelectBasics_ListOfBox2d.hxx>
#include <SelectMgr_DataMapIteratorOfDataMapOfIntegerSensitive.hxx>
#include <SelectMgr_DataMapIteratorOfDataMapOfSelectionActivation.hxx>
#include <SelectMgr_SortCriterion.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <SortTools_QuickSortOfInteger.hxx>
#include <OSD_Environment.hxx>

static Standard_Boolean SelectDebugModeOnVS()
{
  static Standard_Integer isDebugMode( -1 );
  if ( isDebugMode < 0 ) {
    isDebugMode = 1;
    OSD_Environment selectdb("SELDEBUGMODE");
    if ( selectdb.Value().IsEmpty() )
      isDebugMode = 0;
  }                       
  return ( isDebugMode != 0 );
}

//==================================================
// Function: Initialize
// Purpose :
//==================================================
SelectMgr_ViewerSelector::SelectMgr_ViewerSelector():
toupdate(Standard_True),
tosort(Standard_True),
preferclosest(Standard_True),
mytolerance(0.),
myCurRank(0),
lastx (Precision::Infinite()),
lasty (Precision::Infinite()),
myUpdateSortPossible( Standard_True )
{
}


//==================================================
// Function: Activate
// Purpose :
//==================================================
void SelectMgr_ViewerSelector::
Activate (const Handle(SelectMgr_Selection)& aSelection,
          const Standard_Boolean AutomaticProj)
{
  tosort = Standard_True;

  if (!myselections.IsBound(aSelection))
  {
    myselections.Bind(aSelection,0);
  } 
  else if (myselections(aSelection)!=0)
  {
    myselections(aSelection)= 0;
  }
  if(AutomaticProj)
    Convert(aSelection);
}


//==================================================
// Function: Deactivate
// Purpose :
//==================================================
void SelectMgr_ViewerSelector::
Deactivate (const Handle(SelectMgr_Selection)& aSel)
{
  if(myselections.IsBound(aSel))
  {myselections(aSel)=1;
  tosort = Standard_True;}
}





//==================================================
// Function: Sleep
// Purpose :
//==================================================
void SelectMgr_ViewerSelector::Sleep()
{ SelectMgr_DataMapIteratorOfDataMapOfSelectionActivation It(myselections);
for (;It.More();It.Next()){
  if(It.Value()==0) myselections(It.Key())= 2;
}
UpdateSort();
}
//=======================================================================
//function : Sleep
//purpose  : 
//=======================================================================

void SelectMgr_ViewerSelector::Sleep(const Handle(SelectMgr_SelectableObject)& SO)
{ 

  for(SO->Init();SO->More();SO->Next()){
    if(myselections.IsBound(SO->CurrentSelection())){
      myselections(SO->CurrentSelection()) = 2;
    }
  }
  UpdateSort();
}


//==================================================
// Function: Awake
// Purpose :
//==================================================
void SelectMgr_ViewerSelector::Awake(const Standard_Boolean AutomaticProj)
{
  SelectMgr_DataMapIteratorOfDataMapOfSelectionActivation It(myselections);
  for (;It.More();It.Next()){
    if(It.Value()==2)
      myselections(It.Key())=0;
    if(AutomaticProj)
      UpdateConversion();
    UpdateSort();
  }
}

void SelectMgr_ViewerSelector::Awake(const Handle(SelectMgr_SelectableObject)& SO,
                                     const Standard_Boolean AutomaticProj)
{
  for(SO->Init();SO->More();SO->Next()){
    if(myselections.IsBound(SO->CurrentSelection())){
      myselections(SO->CurrentSelection()) =0;
      if(AutomaticProj)
        Convert(SO->CurrentSelection());
    }
  }

}
//==================================================
// Function: Clear
// Purpose :
//==================================================
void SelectMgr_ViewerSelector::Clear()
{
  myentities.Clear();
  myselections.Clear();
  toupdate = Standard_True;
  tosort = Standard_True;
  mystored.Clear();
  lastx = Precision::Infinite();
  lasty = Precision::Infinite();

}

//==================================================
// Function: UpdateConversion
// Purpose :
//==================================================
void SelectMgr_ViewerSelector::UpdateConversion()
{
  if( SelectDebugModeOnVS() )
    cout<<"\t\t\t\t\t SelectMgr_VS::UpdateConversion"<<endl;

  SelectMgr_DataMapIteratorOfDataMapOfSelectionActivation It(myselections);
  for(;It.More();It.Next()){
    //Convert only if active...
    if(It.Value()==0)
      Convert(It.Key());
  }
  toupdate = Standard_False;
  tosort = Standard_True;
}


//==================================================
// Function: Convert
// Purpose :
//==================================================
void SelectMgr_ViewerSelector::
Convert (const Handle(SelectMgr_Selection)& /*aSel*/) {tosort=Standard_True;}


//==================================================
// Function: UpdateSort
// Purpose :
//==================================================
void SelectMgr_ViewerSelector::UpdateSort()
{
  if( !myUpdateSortPossible )
    return;

  if( SelectDebugModeOnVS() )
    cout<<"\t\t\t\t\t SelectMgr_ViewerSelector::UpdateSort()"<<endl;
  mystored.Clear();
  myentities.Clear();
  myactivenb = NbBoxes();

  if(myactivenb > 0) {
    Standard_Boolean NoClip = myclip.IsVoid();
    Handle(Bnd_HArray1OfBox2d) refToTab = new Bnd_HArray1OfBox2d(1,myactivenb);
    Bnd_Array1OfBox2d & tab = refToTab->ChangeArray1();
    Standard_Real xmin=Precision::Infinite(),ymin=Precision::Infinite(),xmax=-Precision::Infinite(),ymax=-Precision::Infinite();
    Standard_Real curxmin,curymin,curxmax,curymax;
    //    Standard_Integer boxindex=0,indexsel=0,indexprim=0;
    Standard_Integer boxindex=0;

    SelectMgr_DataMapIteratorOfDataMapOfSelectionActivation It;
    SelectBasics_ListIteratorOfListOfBox2d LIt;
    Handle(SelectMgr_Selection) curEntity;
    Standard_Real ScaleFactor;
    for(It.Initialize(myselections);It.More();It.Next()){
      if(It.Value()== 0)
      { curEntity = It.Key();
      for(curEntity->Init();curEntity->More();curEntity->Next())
      {     
        static SelectBasics_ListOfBox2d BoxList;
        BoxList.Clear();
        curEntity->Sensitive()->Areas(BoxList);
        ScaleFactor = curEntity->Sensitive()->SensitivityFactor();


        for(LIt.Initialize(BoxList);LIt.More();LIt.Next()){
          boxindex++;

          tab.SetValue(boxindex,LIt.Value());

          tab(boxindex).SetGap(mytolerance*ScaleFactor);
          myentities.Bind(boxindex,curEntity->Sensitive());
          if(NoClip){
            if (!tab(boxindex).IsVoid()) {
              tab(boxindex).Get(curxmin,curymin,curxmax,curymax);
              if(curxmin<xmin) xmin=curxmin;
              if(curxmax>xmax) xmax=curxmax;
              if(curymin<ymin) ymin=curymin;
              if(curymax>ymax) ymax=curymax;
            }
          }
        }
      }
      }
    }


    if(NoClip) {myclip.SetVoid();myclip.Update(xmin,ymin,xmax,ymax);}
    myselector.Initialize(myclip, mytolerance,refToTab);
    tosort = Standard_False;
    if(NoClip) myclip.SetVoid();
  }
}


//==================================================
// Function: Remove
// Purpose :
//==================================================
void SelectMgr_ViewerSelector::
Remove(const Handle(SelectMgr_Selection)& aSel)
{
  if (myselections.IsBound(aSel))
  { myselections.UnBind(aSel);
  tosort = Standard_True;
  }
}

//==================================================
// Function: SetSensitivity
// Purpose :
//==================================================
void SelectMgr_ViewerSelector::SetSensitivity(const Standard_Real aVal)
{mytolerance = aVal;
tosort=Standard_True;}

//==================================================
// Function: SetClipping
// Purpose :
//==================================================
void SelectMgr_ViewerSelector::SetClipping(const Standard_Real Xc,
                                           const Standard_Real Yc,
                                           const Standard_Real Height,
                                           const Standard_Real Width)
{
  Bnd_Box2d aClip;
  aClip.Set(gp_Pnt2d(Xc-Width/2, Yc-Height/2));
  aClip.Add(gp_Pnt2d(Xc+Width/2, Yc+Height/2));
  myclip = aClip;
  tosort = Standard_True;
}


//==================================================
// Function: SetClipping
// Purpose :
//==================================================
void SelectMgr_ViewerSelector::SetClipping (const Bnd_Box2d& abox)
{myclip = abox;
tosort = Standard_True;
}

//==================================================
// Function: InitSelect
// Purpose :
//==================================================
void SelectMgr_ViewerSelector::InitSelect(const Standard_Real Xr,
                                          const Standard_Real Yr)
{
  Standard_OutOfRange_Raise_if(Abs(Xr-Precision::Infinite())<=Precision::Confusion() ||
    Abs(Yr-Precision::Infinite())<=Precision::Confusion(),
    " Infinite values in IniSelect");
  mystored.Clear();
  myprim.Clear();
  if (toupdate) UpdateConversion();
  if (tosort) UpdateSort();
  if(myactivenb!=0){
    myselector.InitSelect(Xr,Yr);
    if(myselector.More()) {lastx = Xr;lasty=Yr;}
    LoadResult();
  }
}

//==================================================
// Function: InitSelect
// Purpose :
//==================================================
void SelectMgr_ViewerSelector::InitSelect(const Bnd_Box2d& aBox)
{
  mystored.Clear();
  if(toupdate) UpdateConversion();
  if (tosort) UpdateSort();
  if (myactivenb!=0){
    myselector.InitSelect(aBox);
    LoadResult(aBox);
  }
}

//==================================================
// Function: InitSelect
// Purpose :
//==================================================
void SelectMgr_ViewerSelector::InitSelect(const Standard_Real Xmin,
                                          const Standard_Real Ymin,
                                          const Standard_Real Xmax,
                                          const Standard_Real Ymax)
{
  mystored.Clear();

  if (toupdate) UpdateConversion();
  if (tosort)   UpdateSort();
  if (myactivenb!=0){
    Bnd_Box2d aBox;
    aBox.Update(Xmin,Ymin,Xmax,Ymax);
    myselector.InitSelect(aBox);
    LoadResult(aBox);
  }
}

//==================================================
// Function: InitSelect
// Purpose : Polyline Selection
//==================================================
void SelectMgr_ViewerSelector::InitSelect(const TColgp_Array1OfPnt2d& aPoly)
{
  mystored.Clear();

  if (toupdate) UpdateConversion();
  if (tosort)   UpdateSort();
  if (myactivenb!=0){
    // the Bnd boxes are used for the first time  
    Bnd_Box2d aBox;
    Standard_Integer NbPnt = aPoly.Length();
    Standard_Integer i;
    for(i=1;i<=NbPnt;i++) {
      aBox.Update(aPoly(i).X(),aPoly(i).Y());
    }
    myselector.InitSelect(aBox);
    LoadResult(aPoly);
    //    LoadResult(aBox);
  }
}


//==================================================
// Function: LoadResult
// Purpose : for the moment the size of the primitive 
//           is not taken into account in the search criteriai...
//           The priority, the depth and the min. distance to CDG or Borders is taken...
//==================================================
void SelectMgr_ViewerSelector::
LoadResult()
{
  //  Handle(SelectMgr_EntityOwner)  OWNR;  
  if(myselector.More())
  {
    //      Standard_Boolean Found(Standard_False);
    Standard_Real DMin;
    Standard_Integer nument;
    for(;myselector.More();myselector.Next()){
      nument = myselector.Value();
      const Handle(SelectBasics_SensitiveEntity)& SE = myentities(nument);
      if (SE->Matches(lastx,lasty,mytolerance,DMin)) { 
        const Handle(SelectBasics_EntityOwner)& OWNR = SE->OwnerId();

        if(!OWNR.IsNull()){
          Standard_Real TheDepth = SE->Depth();
          Standard_Integer Prior = OWNR->Priority();

          SelectMgr_SortCriterion SC(Prior,TheDepth,DMin,mytolerance,preferclosest);
          if ( mystored.Contains(OWNR) )
          {
            SelectMgr_SortCriterion& Crit = mystored.ChangeFromKey(OWNR);
            if ( SC > Crit )
            {
              Crit = SC;

              // update previously recorded entity for this owner
              for (int i=1; i <= myprim.Length(); i++)
                if (myentities(myprim(i))->OwnerId() == OWNR) {
                  myprim.SetValue (i, nument);
                  break;
                }
            }
          }
          else
          {
            mystored.Add(OWNR,SC);

            // record entity
            myprim.Append(nument);
          }
        }
      }
    }
    SortResult();
  }
  if( SelectDebugModeOnVS() ){
    cout<<"\tSelectMgr_VS:: Resultat du move"<<endl;
    cout<<"\tNb Detectes :"<<mystored.Extent()<<endl;
    for(Standard_Integer i=1;i<=mystored.Extent();i++){
      const SelectMgr_SortCriterion& Crit = mystored(myIndexes->Value(i));
      cout<<"\t"<<i<<" - Prior"<<Crit.Priority()<<" - prof :"<<Crit.Depth()<<"  - Dist. :"<<Crit.MinDist()<<endl;
    }
  }
}
//==================================================
// Function: LoadResult
// Purpose :
//==================================================
void SelectMgr_ViewerSelector::LoadResult(const Bnd_Box2d& abox)
{
  mystored.Clear();

  //  Handle(SelectMgr_EntityOwner)  OWNR;  
  if(myselector.More())
  { Standard_Real xmin,ymin,xmax,ymax;
  abox.Get(xmin,ymin,xmax,ymax);
  //      Standard_Boolean Found(Standard_False);
  //      Standard_Real DMin=0.;
  Standard_Integer nument;
  for(;myselector.More();myselector.Next()){
    nument = myselector.Value();
    const Handle(SelectBasics_SensitiveEntity)& SE = myentities(nument);
    if (SE->Matches(xmin,ymin,xmax,ymax,0.0)){
      const Handle(SelectBasics_EntityOwner)& OWNR = SE->OwnerId();
      if(!OWNR.IsNull()){
        if(!mystored.Contains(OWNR)){
          SelectMgr_SortCriterion SC(OWNR->Priority(),Precision::Infinite(),
            Precision::Infinite(),mytolerance,preferclosest);
          mystored.Add(OWNR,SC);
          myprim.Append(nument);
        }
      }
    }
  }

  // do not parse in case of selection by elastic rectangle (BUG ANALYST)
  if(mystored.IsEmpty()) return; 
  if(myIndexes.IsNull()) 
    myIndexes = new TColStd_HArray1OfInteger(1,mystored.Extent()); 
  else if(mystored.Extent() !=myIndexes->Length()) 
    myIndexes = new TColStd_HArray1OfInteger (1,mystored.Extent()); 

  // to work faster... 
  TColStd_Array1OfInteger& thearr = myIndexes->ChangeArray1(); 
  for(Standard_Integer I=1;I<=mystored.Extent();I++) 
    thearr(I)=I; 
  } 
}
//==================================================
// Function: LoadResult
// Purpose :
//==================================================
void SelectMgr_ViewerSelector::LoadResult(const TColgp_Array1OfPnt2d& aPoly)
{
  mystored.Clear();
  Bnd_Box2d aBox;
  Standard_Integer NbPnt = aPoly.Length();
  Standard_Integer i;
  for(i=1;i<=NbPnt;i++) {
    aBox.Update(aPoly(i).X(),aPoly(i).Y());
  }
  Standard_Integer NB=0;
  //  Handle(SelectMgr_EntityOwner)  OWNR;  
  if(myselector.More())
  { 
    Standard_Integer nument;

    for(;myselector.More();myselector.Next()){
      NB++;
      nument = myselector.Value();
      const Handle(SelectBasics_SensitiveEntity)& SE = myentities(nument);
      if (SE->Matches(aPoly,aBox,0.0)){
        const Handle(SelectBasics_EntityOwner)& OWNR = SE->OwnerId();
        if(!OWNR.IsNull()){
          if(!mystored.Contains(OWNR)){
            SelectMgr_SortCriterion SC(OWNR->Priority(),Precision::Infinite(),
              Precision::Infinite(),mytolerance,preferclosest);
            mystored.Add(OWNR,SC);
            myprim.Append(nument);
          }
        }
      }
    }

    if(mystored.IsEmpty()) return; 
    if(myIndexes.IsNull()) 
      myIndexes = new TColStd_HArray1OfInteger(1,mystored.Extent()); 
    else if(mystored.Extent() !=myIndexes->Length()) 
      myIndexes = new TColStd_HArray1OfInteger (1,mystored.Extent()); 

    // to work faster... 
    TColStd_Array1OfInteger& thearr = myIndexes->ChangeArray1(); 
    for(Standard_Integer I=1;I<=mystored.Extent();I++) 
      thearr(I)=I; 
  }
}


//==================================================
// Function: HasStored
// Purpose :
//==================================================
Standard_Boolean SelectMgr_ViewerSelector::
HasStored ()
{
  if(Abs(lastx-Precision::Infinite())<=Precision::Confusion()) return Standard_False;
  if(Abs(lasty-Precision::Infinite())<=Precision::Confusion()) return Standard_False;
  InitSelect(lastx,lasty);
  Init();
  return More();
}




//==================================================
// Function: Picked
// Purpose :
//==================================================
Handle(SelectMgr_EntityOwner) SelectMgr_ViewerSelector
::Picked() const
{
  Standard_Integer RankInMap = myIndexes->Value(myCurRank);
  const Handle(SelectBasics_EntityOwner)& toto = mystored.FindKey(RankInMap);
  Handle(SelectMgr_EntityOwner) Ownr = *((Handle(SelectMgr_EntityOwner)*) &toto);
  return Ownr;
}



//=======================================================================
//function : More
//purpose  : 
//=======================================================================
Standard_Boolean SelectMgr_ViewerSelector::More() 
{
  if(mystored.Extent()==0) return Standard_False;
  if(myCurRank==0) return Standard_False;
  return myCurRank <= myIndexes->Length();
}

//==================================================
// Function: OnePicked
// Purpose : only the best one is chosen
//           depend on priority and mindist...
//==================================================

Handle(SelectMgr_EntityOwner) SelectMgr_ViewerSelector
::OnePicked()
{

  Init();
  if(More()){
    Standard_Integer RankInMap = myIndexes->Value(1);
    const Handle(SelectBasics_EntityOwner)& toto = mystored.FindKey(RankInMap);
    Handle(SelectMgr_EntityOwner) Ownr = *((Handle(SelectMgr_EntityOwner)*) &toto);
    return Ownr;
  }

  Handle (SelectMgr_EntityOwner) NullObj; //returns a null Handle if there was not successfull pick...
  return NullObj;
}


//=======================================================================
//function : NbPicked
//purpose  : 
//=======================================================================

Standard_Integer SelectMgr_ViewerSelector::NbPicked() const
{
  return mystored.Extent();
}
//=======================================================================
//function : Picked
//purpose  : 
//=======================================================================
Handle(SelectMgr_EntityOwner) SelectMgr_ViewerSelector::Picked(const Standard_Integer aRank) const
{

  Handle(SelectMgr_EntityOwner) Own; 
  if (aRank<1 || aRank>NbPicked())
    return Own;
  Standard_Integer Indx = myIndexes->Value(aRank);


  const Handle(SelectBasics_EntityOwner)& toto = mystored.FindKey(Indx);
  Own = *((Handle(SelectMgr_EntityOwner)*) &toto);
  return Own;
}
//=======================================================================
//function : Primitive
//purpose  : 
//=======================================================================
Handle(SelectBasics_SensitiveEntity) SelectMgr_ViewerSelector::Primitive
(const Standard_Integer /*Index*/) const
{
  return myentities(myprim(myCurRank));
}


//==================================================
// Function: LastPosition
// Purpose :
//==================================================
void SelectMgr_ViewerSelector::LastPosition(Standard_Real& Xlast,
                                            Standard_Real& YLast) const
{   Xlast = lastx;YLast = lasty;} 



//===================================================
//
//       INTERNAL METHODS ....
//
//==================================================




//==================================================
// Function: NbBoxes
// Purpose :
//==================================================
Standard_Integer SelectMgr_ViewerSelector::NbBoxes()
{
  SelectMgr_DataMapIteratorOfDataMapOfSelectionActivation It(myselections);
  //  Standard_Integer Nbb=0, first,last;
  Standard_Integer Nbb=0;

  for(;It.More();It.Next()){
    if(It.Value()==0){
      for(It.Key()->Init();It.Key()->More();It.Key()->Next())
      {Nbb+= It.Key()->Sensitive()->MaxBoxes();}
    }
  }
  return Nbb;
}




//==================================================
// Function: Contains
// Purpose : 
//==================================================
Standard_Boolean SelectMgr_ViewerSelector::
Contains(const Handle(SelectMgr_SelectableObject)& anObject) const
{
  for (anObject->Init();anObject->More();anObject->Next()){
    if(myselections.IsBound(anObject->CurrentSelection()))
      return Standard_True;
  }
  return Standard_False;
}



//==================================================
// Function: ActiveModes
// Purpose : return all the  modes with a given state for an object
//==================================================


Standard_Boolean SelectMgr_ViewerSelector::
Modes(const Handle(SelectMgr_SelectableObject)& SO,
      TColStd_ListOfInteger& TheActiveList,
      const SelectMgr_StateOfSelection WantedState) const 
{
  Standard_Boolean Found= Standard_False;
  for(SO->Init();SO->More();SO->Next()){
    if(myselections.IsBound(SO->CurrentSelection())){
      if(WantedState==SelectMgr_SOS_Any)
        TheActiveList.Append(SO->CurrentSelection()->Mode());
      else if( myselections(SO->CurrentSelection())==WantedState) 
        TheActiveList.Append(SO->CurrentSelection()->Mode());

      if(!Found) Found=Standard_True;
    }
  }
  return Found;
}


Standard_Boolean SelectMgr_ViewerSelector::
IsActive(const Handle(SelectMgr_SelectableObject)& SO,
         const Standard_Integer aMode) const
{
  for(SO->Init();SO->More();SO->Next()){
    if(aMode==SO->CurrentSelection()->Mode()){
      if(myselections.IsBound(SO->CurrentSelection()) && 
        myselections(SO->CurrentSelection())==SelectMgr_SOS_Activated) 
        return Standard_True;
      else return Standard_False;
    }
  }
  return Standard_False;
}


Standard_Boolean SelectMgr_ViewerSelector::
IsInside(const Handle(SelectMgr_SelectableObject)& SO,
         const Standard_Integer aMode) const
{
  for(SO->Init();SO->More();SO->Next()){
    if(aMode==SO->CurrentSelection()->Mode()){
      if(myselections.IsBound(SO->CurrentSelection())) return Standard_True;
      else return Standard_False;

    }
  }
  return Standard_False;
}


//=======================================================================
//function : Status
//purpose  : 
//=======================================================================

SelectMgr_StateOfSelection SelectMgr_ViewerSelector::Status(const Handle(SelectMgr_Selection)& aSel) const 
{
  if(!myselections.IsBound(aSel)) return SelectMgr_SOS_Unknown;
  //JR/Hp
  Standard_Integer ie = myselections(aSel) ;
  return SelectMgr_StateOfSelection( ie );
  //  return SelectMgr_StateOfSelection(myselections(aSel));

}



//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void SelectMgr_ViewerSelector::Dump(Standard_OStream& S) const
{
  S<<"=========================="<<endl;
  S<<" SelectMgr_ViewerSelector "<<endl;
  S<<"=========================="<<endl;
  S<<" "<<endl;
}



//==================================================
// Function: Status
// Purpose : gives Information about selectors
//==================================================

TCollection_AsciiString SelectMgr_ViewerSelector::
Status(const Handle(SelectMgr_SelectableObject)& SO) const
{
  TCollection_AsciiString Status("Status Object :\n\t");
  Standard_Boolean Found= Standard_False;
  for(SO->Init();SO->More();SO->Next()){
    if(myselections.IsBound(SO->CurrentSelection()))
    {
      Found = Standard_True;
      Status = Status + "Mode " + 
        TCollection_AsciiString(SO->CurrentSelection()->Mode()) +
        " present - " ;
      if(myselections(SO->CurrentSelection())) 
        Status = Status + " Active \n\t";
      else
        Status = Status + " Inactive \n\t";
    }
  }

  if(!Found) Status = Status + "Not Present in the selector\n\n";
  return Status;
}


TCollection_AsciiString SelectMgr_ViewerSelector::
Status () const 
{
  // sevsitive primitives present 
  //-----------------------------
  TCollection_AsciiString Status("\t\tSelector Status :\n\t");
  // selections
  //-----------
  Standard_Integer NbActive =0,NbPrim=0;
  Status = Status + "Number of already computed selections : " + 
    TCollection_AsciiString(myselections.Extent());

  SelectMgr_DataMapIteratorOfDataMapOfSelectionActivation It(myselections);
  for(;It.More();It.Next())
  {
    if(It.Value()==0) {NbActive++;
    for(It.Key()->Init();It.Key()->More();It.Key()->Next()){NbPrim++;}
    }
  }
  Status = Status + " - " + TCollection_AsciiString(NbActive) + " activated ones\n\t";
  Status = Status + "Number of active sensitive primitives : " + 
    TCollection_AsciiString(NbPrim)+"\n\t";
  Status = Status + "Real stored Pick Tolerance : " + TCollection_AsciiString(mytolerance) +"\n\t";
  if(toupdate) {
    Status = Status + "\nWARNING : those informations will be obsolete for the next Pick\n"
      +"to get the real status of the selector - make One pick and call Status again\n";
  }
  return Status;
}

//=======================================================================
//function : SortResult
//purpose  :  there is a certain number of entities ranged by criteria 
//            (depth, size, priority, mouse distance from borders or
//            CDG of the detected primitive. Parsing :
//             maximum priorities .
//             then a reasonable compromise between depth and distance...
// finally the ranges are stored in myindexes depending on the parsing.
// so, it is possible to only read 
//=======================================================================
void SelectMgr_ViewerSelector::SortResult()
{
  if(mystored.IsEmpty()) return;

  const Standard_Integer anExtent = mystored.Extent();
  if(myIndexes.IsNull() || anExtent != myIndexes->Length())
    myIndexes = new TColStd_HArray1OfInteger (1, anExtent);

  // to work faster...
  TColStd_Array1OfInteger& thearr = myIndexes->ChangeArray1();

  // indices from 1 to N are loaded
  Standard_Integer I ;
  for (I=1; I <= anExtent; I++)
    thearr(I)=I;

  // OCC4201 (AGV): This loop is inefficient on large arrays, so I replace it
  //                with a standard sort algo
  //  // on trie suivant les criteres  (i) (Owner) (SortCriterion)
  //  Standard_Boolean OKSort;
  //  Standard_Integer temp,indx,indx1;
  //  Standard_Integer tmprim;
  //  // merci lbr...
  //  do{
  //    OKSort =Standard_True;
  //    for(I=1;I<thearr.Length();I++){
  //      indx = thearr(I);
  //      indx1 = thearr(I+1);
  //      if(mystored(indx) < mystored(indx1)){
  //      OKSort = Standard_False;
  //
  //      temp = thearr(I+1);
  //      thearr(I+1) = thearr (I);
  //      thearr(I) = temp;
  //
  //      tmprim = myprim(I+1);
  //      myprim(I+1) = myprim(I);
  //      myprim(I) = tmprim;
  //
  //      }
  //    }
  //  } while (OKSort==Standard_False);
  //
  // OCC4201 (AGV): debut

  SortTools_QuickSortOfInteger::Sort (thearr,
    SelectMgr_CompareResults(mystored));
  TColStd_Array1OfInteger myPrimArr (1, myprim.Length());
  for (I = 1; I <= myPrimArr.Length(); I++)
    myPrimArr (I) = myprim (I);
  for (I = 1; I <= thearr.Length(); I++) {
    const Standard_Integer ind = thearr(I);
    if (ind > 0 && ind <= myPrimArr.Upper())
      myprim (I) = myPrimArr (ind);
  }
  // OCC4201 (AGV): fin
  // it is enough to return owners corresponding to parced indices...

}


//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean SelectMgr_ViewerSelector::IsUpdateSortPossible() const
{
  return myUpdateSortPossible;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void SelectMgr_ViewerSelector::SetUpdateSortPossible( const Standard_Boolean possible )
{
  myUpdateSortPossible = possible;
}
