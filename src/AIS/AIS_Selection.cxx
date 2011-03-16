#include <AIS_Selection.ixx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_SequenceOfTransient.hxx>

#define BUC60953   // SAV_050701 : the array of selected objects has always the same length
                   // independently of number of objects selected. Thus, if there were selected
                   // more than MaxSizeOfResult objects we have got an exception.
                   // 
                   // Moreover, Select method was optimized a little bit.
                   // Now it checks the state of incoming owner. If the state is 0
                   // there is no searching for object in <myresult> array.

#define OCC138

#define OCC189     //SAV: 18//03/02 array was replaced with list.

#define OCC1039    //SAV: 25/11/02 clearing selected objects if any on the AIS_Selection remove.

#define USE_MAP    //san : 18/04/03 USE_MAP - additional datamap is used to speed up access 
                   //to certain owners in <myresult> list

#ifdef BUC60953
#include <SelectMgr_EntityOwner.hxx>
#endif

#ifdef OCC138      //VTN Avoding infinit loop in AddOrRemoveSelected and AddOrRemoveCurrentObject methods.
#include <AIS_InteractiveObject.hxx>
#endif

#ifndef USE_MAP
#define MaxSizeOfResult 10000
#else
#define MaxSizeOfResult 100000
#endif

// beurk mais moins qu''avant.... rob
static void AIS_Sel_CurrentSelection (Handle(AIS_Selection)& InputSel)     
{
  static Handle(AIS_Selection) theCurrentSelection;
  if(!InputSel.IsNull())
    theCurrentSelection = InputSel;
  else
    InputSel = theCurrentSelection;
}

static TColStd_SequenceOfTransient&  AIS_Sel_GetSelections()
{
  static TColStd_SequenceOfTransient Selections;
  return Selections;
}

//=======================================================================
//function : AIS_Selection
//purpose  : 
//=======================================================================
AIS_Selection::AIS_Selection(const Standard_CString aName) :
myName(TCollection_AsciiString(aName)),
mycuri(0), 
#if !defined USE_MAP && !defined OCC189
myresult(new TColStd_HArray1OfTransient(1,MaxSizeOfResult)),
#endif
myNb(0)
{
#ifdef USE_MAP 
  myResultMap.ReSize( MaxSizeOfResult ); // for maximum performnace on medium selections ( < 100000 objects )
#endif
}

//=======================================================================
//function : CreateSelection
//purpose  : 
//=======================================================================
Standard_Boolean AIS_Selection::CreateSelection(const Standard_CString aName)
{ 
  Handle(AIS_Selection) S = AIS_Selection::Selection(aName);
  if(!S.IsNull())
    return Standard_False;
  S = new AIS_Selection(aName);
  AIS_Sel_GetSelections().Prepend(S);
  AIS_Sel_CurrentSelection(S);
  return Standard_True;
}


//=======================================================================
//function : Selection
//purpose  : 
//=======================================================================
Handle(AIS_Selection) AIS_Selection::Selection(const Standard_CString aName) 
{
  Handle(AIS_Selection) S;
  if(AIS_Sel_GetSelections().IsEmpty()) return S;
  
  Handle(Standard_Transient) curobj;
  Handle(AIS_Selection) Sel;
//  Standard_Boolean found(Standard_False);
  for(Standard_Integer I =1; I<= AIS_Sel_GetSelections().Length();I++){
    curobj = AIS_Sel_GetSelections().Value(I);
    Sel = *((Handle(AIS_Selection)*)&curobj);
    if(Sel->myName.IsEqual(aName))
      return Sel;
  }
  
  return S;
}

//=======================================================================
//function : Find
//purpose  : 
//=======================================================================
Standard_Boolean AIS_Selection::Find(const Standard_CString aName) 
{
  Handle(AIS_Selection) S = AIS_Selection::Selection(aName);
  return !S.IsNull();
}

//=======================================================================
//function : SetCurrentSelection
//purpose  : 
//=======================================================================
Standard_Boolean AIS_Selection::SetCurrentSelection (const Standard_CString aName) 
{  
  AIS_Selection::CreateSelection(aName); // juste pour voir... ?????????????


  Handle(AIS_Selection) anAISSelection = AIS_Selection::Selection(aName) ;
  AIS_Sel_CurrentSelection ( anAISSelection ) ;
  return Standard_True;
}

//=======================================================================
//function : Select
//purpose  : 
//=======================================================================
void AIS_Selection::Select() 
{
  Handle(AIS_Selection) S;
  AIS_Sel_CurrentSelection (S);
  if(!S.IsNull()){
    S->myNb=0;
#if defined OCC189 || defined USE_MAP
    S->myresult.Clear();
#ifdef USE_MAP
    S->myResultMap.Clear();
#endif
#endif
  }
}

//=======================================================================
//function : CurrentSelection
//purpose  : 
//=======================================================================
Handle(AIS_Selection) AIS_Selection::CurrentSelection() {
  Handle(AIS_Selection) S;
  AIS_Sel_CurrentSelection (S);
  return S;
}
//=======================================================================
//function : Select
//purpose  : 
//=======================================================================
AIS_SelectStatus AIS_Selection::Select(const Handle(Standard_Transient)& anObject) 
{
  Handle(AIS_Selection) S;
  AIS_Sel_CurrentSelection (S);
  if(S.IsNull()) return AIS_SS_NotDone;
  Handle(AIS_InteractiveObject) anAISObj;
  Handle(SelectMgr_EntityOwner) owner = Handle(SelectMgr_EntityOwner)::DownCast( anObject );
  if ( owner.IsNull() )
    anAISObj = Handle(AIS_InteractiveObject)::DownCast( anObject );
#ifndef OCC189
  TColStd_Array1OfTransient& arr = S->myresult->ChangeArray1();
  
  Standard_Integer Found(-1);
  Standard_Integer i ;

#ifdef BUC60953
  
  Standard_Boolean selected = Standard_False;
  if ( !owner.IsNull() )
    selected = owner->State() != 0;
#ifdef OCC138
  else if(!anAISObj.IsNull())
    selected = anAISObj->State();
  }
#endif

  if ( selected ) // looking up index of object
#endif

  for( i=arr.Lower() && Found==-1;i<=S->myNb;i++){
    if(arr(i)==anObject)
      Found=i;
  }
  // Il n''est pas dedans, on le rajoute....

#ifdef BUC60953
  if ( !selected || Found == -1 ) {
#else
  if(Found==-1){
#endif  
    if((S->myNb)+1>arr.Length()){
      Handle(TColStd_HArray1OfTransient) NiouTab = new TColStd_HArray1OfTransient(1,arr.Length()+MaxSizeOfResult);
      // oui bon, mais...
      for(i=arr.Lower();i<=S->myNb;i++){
	const Handle(Standard_Transient)& T = S->myresult->Value(i);
	NiouTab->SetValue(i,T);
      }
#ifdef BUC60953
      S->myresult = NiouTab;
#endif
    }
    (S->myNb)++;
    S->myresult->SetValue(S->myNb,anObject);
    return AIS_SS_Added;
  }
  // il etait dedans on l''enleve...
  for(i=Found;i<=S->myNb;i++)
    arr(i)=arr(i+1);
  S->myNb--;
#elif !defined USE_MAP //OCC189
  AIS_NListTransient::Iterator anIter ( S->myresult );
  for ( ; anIter.More(); anIter.Next() )
    if ( anIter.Value() == anObject ) {
      S->myresult.Remove( anIter );
      return AIS_SS_Removed;
    }
      
  S->myresult.Append( anObject );
  return AIS_SS_Added;
#else //USE_MAP
  if ( S->myResultMap.IsBound( anObject ) ){
    AIS_NListTransient::Iterator aListIter = S->myResultMap.Find( anObject );
//skt-----------------------------------------------------------------
    if( S->myIterator == aListIter )
	if( S->myIterator.More() )
	    S->myIterator.Next();
	else
	    S->myIterator = AIS_NListTransient::Iterator();
//--------------------------------------------------------------------
#ifdef BUC60953
    // In the mode of advanced mesh selection only one owner is created
    // for all selection modes. It is necessary to check the current detected
    // entity and remove the owner from map only if the detected entity is
    // the same as previous selected (IsForcedHilight call)
    if( !anAISObj.IsNull() || ( !owner.IsNull() && !owner->IsForcedHilight() ) )
    {
#endif  
      S->myresult.Remove( aListIter );
      S->myResultMap.UnBind( anObject );
    
      // update list iterator for next object in <myresult> list if any
      if ( aListIter.More() ){
	const Handle(Standard_Transient)& aNextObject = aListIter.Value();
	if ( S->myResultMap.IsBound( aNextObject ) )
	  S->myResultMap( aNextObject ) = aListIter;
	else
	  S->myResultMap.Bind( aNextObject, aListIter );
      }
      return AIS_SS_Removed;
    }
    else
      return AIS_SS_Added;
  }
  
  AIS_NListTransient::Iterator aListIter;
  S->myresult.Append( anObject, aListIter );
  S->myResultMap.Bind( anObject, aListIter );
  return AIS_SS_Added;
#endif //USE_MAP
  return AIS_SS_Removed;
  
}

//=======================================================================
//function : AddSelect
//purpose  : Always add int the selection
//=======================================================================
AIS_SelectStatus AIS_Selection::AddSelect(const Handle(Standard_Transient)& anObject) 
{
  Handle(AIS_Selection) S;
  AIS_Sel_CurrentSelection (S);
  if(S.IsNull()) return AIS_SS_NotDone;

#ifndef OCC189
  TColStd_Array1OfTransient& arr = S->myresult->ChangeArray1();
  Standard_Integer i;
  if((S->myNb)+1>arr.Length()){
      Handle(TColStd_HArray1OfTransient) NiouTab = new TColStd_HArray1OfTransient(1,arr.Length()+MaxSizeOfResult);
      // oui bon, mais...
      for(i=arr.Lower();i<=S->myNb;i++){
	const Handle(Standard_Transient)& T = S->myresult->Value(i);
	NiouTab->SetValue(i,T);
      }
      S->myresult = NiouTab;
  }
  (S->myNb)++;
  S->myresult->SetValue(S->myNb,anObject);
#elif !defined USE_MAP //OCC189
  S->myresult.Append( anObject );
#else //USE_MAP
  if ( S->myResultMap.IsBound( anObject ) )
    return AIS_SS_NotDone;
      
  AIS_NListTransient::Iterator aListIter;
  S->myresult.Append( anObject, aListIter );
  S->myResultMap.Bind( anObject, aListIter );
#endif
  return AIS_SS_Added;
}


//=======================================================================
//function : ClearAndSelect
//purpose  : 
//=======================================================================

void AIS_Selection::ClearAndSelect(const Handle(Standard_Transient)& anObject) {
  Handle(AIS_Selection) S;
  AIS_Sel_CurrentSelection (S);
  if(S.IsNull()) return;
  
  Select();
  Select(anObject);

}


//=======================================================================
//function : Extent
//purpose  : 
//=======================================================================
Standard_Integer AIS_Selection::Extent() {
  Handle(AIS_Selection) S;
  AIS_Sel_CurrentSelection (S);
#if !defined OCC189 && !defined USE_MAP
  return S->myNb;
#else
  return S->myresult.Extent();
#endif
}

//=======================================================================
//function : Single
//purpose  : 
//=======================================================================
Handle(Standard_Transient)  AIS_Selection::Single() 
{
  Handle(AIS_Selection) S;
  AIS_Sel_CurrentSelection (S);
  
  S->Init();
  return S->Value();
}
//=======================================================================
//function : IsSelected
//purpose  : 
//=======================================================================
Standard_Boolean AIS_Selection::IsSelected(const Handle(Standard_Transient)& anObject)
{
  Handle(AIS_Selection) S;
  AIS_Sel_CurrentSelection (S);
  if(S.IsNull()) return Standard_False;
#ifndef USE_MAP  
  for(S->Init();S->More();S->Next()){
    if(S->Value()==anObject)
      return Standard_True;
  }
  return Standard_False;
#else
  return S->myResultMap.IsBound( anObject );
#endif
}

//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer AIS_Selection::Index(const Standard_CString aName)
{
  Handle (Standard_Transient) curobj;
  for(Standard_Integer I =1; I<= AIS_Sel_GetSelections().Length();I++){
    curobj = AIS_Sel_GetSelections().Value(I);
    if((*((Handle(AIS_Selection)*)&curobj))->myName.IsEqual(aName))
      return I;
  }
  return 0;
}

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

void AIS_Selection::Remove(const Standard_CString aName) 
{
  Standard_Integer I = AIS_Selection::Index(aName);
  if(I!=0) {
#ifdef OCC1039
    Handle(AIS_Selection) selection = Handle(AIS_Selection)::DownCast( AIS_Sel_GetSelections().Value(I) );
    Standard_Integer stored = selection->NbStored();
    if ( stored )
      selection->Select();
#endif
    AIS_Sel_GetSelections().Remove(I);
  }
  
}

