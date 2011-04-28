// File:	Select3D_SensitiveGroup.cxx
// Created:	Thu Apr 16 16:17:00 1998
// Author:	Robert COUBLANC
//		<rob@robox.paris1.matra-dtv.fr>


#include <Select3D_SensitiveGroup.ixx>
#include <Select3D_ListIteratorOfListOfSensitive.hxx>
#include <Precision.hxx>

Select3D_SensitiveGroup::Select3D_SensitiveGroup(const Handle(SelectBasics_EntityOwner)& OwnerId,
						 const Standard_Boolean MatchAll):
Select3D_SensitiveEntity(OwnerId),
myMustMatchAll(MatchAll),
myLastRank(0),
myX(0.),
myY(0.)
{
}

Select3D_SensitiveGroup::Select3D_SensitiveGroup(const Handle(SelectBasics_EntityOwner)& OwnerId,
						  Select3D_ListOfSensitive& TheList,
						 const Standard_Boolean MatchAll):

Select3D_SensitiveEntity(OwnerId),
myMustMatchAll(MatchAll),
myLastRank(0),
myX(0.),
myY(0.)
{
  myList.Append(TheList);
}

//=======================================================================
//function : Add
//purpose  : No control of  entities inside 
//=======================================================================
void Select3D_SensitiveGroup::Add(Select3D_ListOfSensitive& LL) 
{myList.Append(LL);}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void Select3D_SensitiveGroup::Add(const Handle(Select3D_SensitiveEntity)& aSensitive) 
{
  for(Select3D_ListIteratorOfListOfSensitive It(myList);It.More();It.Next()){
    if(It.Value()==aSensitive) return;
  }
  myList.Append(aSensitive);
}

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================
void Select3D_SensitiveGroup::Remove(const Handle(Select3D_SensitiveEntity)& aSensitive) 
{
  for(Select3D_ListIteratorOfListOfSensitive It(myList);It.More();It.Next()){
    if(It.Value()==aSensitive){
      myList.Remove(It);
      return;
    }
  }
}

//=======================================================================
//function : IsIn
//purpose  : 
//=======================================================================
Standard_Boolean Select3D_SensitiveGroup::IsIn(const Handle(Select3D_SensitiveEntity)& aSensitive) const
{
  for(Select3D_ListIteratorOfListOfSensitive It(myList);It.More();It.Next()){
    if(It.Value()==aSensitive)
      return Standard_True;
  }
  return Standard_False;

}
//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void Select3D_SensitiveGroup::Clear()
{myList.Clear();}

//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

void Select3D_SensitiveGroup::Project(const Handle(Select3D_Projector)& aProjector) 
{
  Select3D_SensitiveEntity::Project(aProjector); // to set the field last proj...

  for(Select3D_ListIteratorOfListOfSensitive It(myList);It.More();It.Next()){
    It.Value()->Project(aProjector);
  }
}

//=======================================================================
//function : Areas
//purpose  : 
//=======================================================================

void Select3D_SensitiveGroup::Areas(SelectBasics_ListOfBox2d& boxes) 
{
  for(Select3D_ListIteratorOfListOfSensitive It(myList);It.More();It.Next()){
    It.Value()->Areas(boxes);
  }
}

//=======================================================================
//function : GetConnected
//purpose  : 
//=======================================================================

Handle(Select3D_SensitiveEntity) Select3D_SensitiveGroup::GetConnected(const TopLoc_Location& aLocation) 
{
  Handle(Select3D_SensitiveGroup) newgroup = new Select3D_SensitiveGroup(myOwnerId,myMustMatchAll);
  Select3D_ListOfSensitive LL;
  for(Select3D_ListIteratorOfListOfSensitive It(myList);It.More();It.Next()){
    LL.Append(It.Value()->GetConnected(aLocation));
  }
  newgroup->Add(LL);
  return newgroup;
}

//=======================================================================
//function : SetLocation
//purpose  : 
//=======================================================================

void Select3D_SensitiveGroup::SetLocation(const TopLoc_Location& aLoc) 
{
  if(aLoc.IsIdentity()) return;

  if(HasLocation())
    if(aLoc == Location()) return;
  
  Select3D_SensitiveEntity::SetLocation(aLoc);
  for(Select3D_ListIteratorOfListOfSensitive It(myList);It.More();It.Next()){
    if(It.Value()->HasLocation()){
      if(It.Value()->Location()!=aLoc)
	It.Value()->SetLocation(It.Value()->Location()*aLoc);
    }
    else
      It.Value()->SetLocation(aLoc);
    
  }
}

//=======================================================================
//function : ResetLocation
//purpose  : 
//=======================================================================
void Select3D_SensitiveGroup::ResetLocation() 
{
 if(!HasLocation()) return;
 for(Select3D_ListIteratorOfListOfSensitive It(myList);It.More();It.Next()){
   if(It.Value()->HasLocation() && It.Value()->Location()!=Location())
     It.Value()->SetLocation(It.Value()->Location()*Location().Inverted());
   else
     It.Value()->ResetLocation();
   
 }
 Select3D_SensitiveEntity::ResetLocation();
}

//=======================================================================
//function : Matches
//purpose  : 
//=======================================================================
Standard_Boolean Select3D_SensitiveGroup::Matches(const Standard_Real X,
						  const Standard_Real Y,
						  const Standard_Real aTol,
						  Standard_Real& DMin) 
{
  myLastRank = 0;
  myLastTol = aTol;
  for(Select3D_ListIteratorOfListOfSensitive It(myList);It.More();It.Next()){
    myLastRank++;
    if (It.Value()->Matches (X, Y, aTol, DMin))
    {
      myX = X; myY = Y; myLastTol = aTol;
      // compute and validate the depth (will call ::ComputeDepth())
      return Select3D_SensitiveEntity::Matches (X, Y, aTol, DMin);
    }
  }
  // no match
  myLastRank = 0;
  SetLastDepth (ShortRealLast());
  return Standard_False;
}

//=======================================================================
//function : Matches
//purpose  :  si on doit tout matcher, on ne repond oui que si toutes
//            les primitives repondent oui
//=======================================================================
Standard_Boolean Select3D_SensitiveGroup::Matches(const Standard_Real XMin,
						  const Standard_Real YMin,
						  const Standard_Real XMax,
						  const Standard_Real YMax,
						  const Standard_Real aTol) 
{
  Standard_Boolean result(Standard_True);
  
  for(Select3D_ListIteratorOfListOfSensitive It(myList);It.More();It.Next()){
    if(It.Value()->Matches(XMin,YMin,XMax,YMax,aTol)){
      if(!myMustMatchAll)
	return Standard_True;
    }
    // ca ne matches pas..
    else {
      if(myMustMatchAll)
	return Standard_False;
      else
	result = Standard_False;
    }
  }
  return result;
}

//=======================================================================
//function : Matches
//purpose  : 
//=======================================================================

Standard_Boolean Select3D_SensitiveGroup::
Matches (const TColgp_Array1OfPnt2d& aPoly,
	 const Bnd_Box2d& aBox,
	 const Standard_Real aTol)
{ 
  Standard_Boolean result(Standard_True);
  
  for(Select3D_ListIteratorOfListOfSensitive It(myList);It.More();It.Next()){
    if(It.Value()->Matches(aPoly, aBox, aTol)){
      if(!myMustMatchAll)
	return Standard_True;
    }
    else {
      if(myMustMatchAll)
	return Standard_False;
      else
	result = Standard_False;
    }
  }
  return result;
}


//=======================================================================
//function : ComputeDepth
//purpose  : to optimise, the minimum depth for 
//          entities that answer YES to Matches(X,Y,...) is taken
//          the test is started from mylastRank...
//=======================================================================
Standard_Real Select3D_SensitiveGroup::ComputeDepth(const gp_Lin& EyeLine) const
{
  Standard_Integer currank = 0;
  Standard_Real DMin, thedepth (Precision::Infinite());
  for (Select3D_ListIteratorOfListOfSensitive It(myList);It.More();It.Next())
  {
    currank++;
    if (currank >= myLastRank)
    {
      // this recomputes and validates the depth for the entity
      if (It.Value()->Matches (myX, myY, myLastTol, DMin))
      {
        It.Value()->ComputeDepth (EyeLine);
        if (It.Value()->Depth() < thedepth)
        {
          // search for topmost entity
          thedepth = It.Value()->Depth();
          //myLastRank = currank; // can not do this here...
        }
      }
    }
  }
  return thedepth;
}

//=======================================================================
//function : MaxBoxes
//purpose  : 
//=======================================================================
Standard_Integer Select3D_SensitiveGroup::MaxBoxes() const
{
  Standard_Integer nbboxes(0);
  for(Select3D_ListIteratorOfListOfSensitive It(myList);It.More();It.Next()){
    nbboxes+=It.Value()->MaxBoxes();
  }
  return nbboxes;
}

void Select3D_SensitiveGroup::SetLastPrj(const Handle(Select3D_Projector)& Prj)
{
  Select3D_SensitiveEntity::SetLastPrj(Prj);
  for(Select3D_ListIteratorOfListOfSensitive It(myList);It.More();It.Next())
    It.Value()->SetLastPrj(Prj);
}
