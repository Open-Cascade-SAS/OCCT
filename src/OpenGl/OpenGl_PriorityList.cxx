// File:      OpenGl_PriorityList.cxx
// Created:   2 November 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#include <OpenGl_PriorityList.hxx>

#include <OpenGl_Structure.hxx>

/*----------------------------------------------------------------------*/

void OpenGl_PriorityList::Add (const OpenGl_Structure *AStructure,const Standard_Integer APriority)
{
  Standard_Integer anIndex = APriority;
  if (anIndex < 0) anIndex = 0;
  else if (anIndex >= myArray.Length()) anIndex = myArray.Length()-1;
  myArray(anIndex).Append(AStructure);
  myNbStructures++;
}

/*----------------------------------------------------------------------*/

Standard_Integer OpenGl_PriorityList::Remove (const OpenGl_Structure *AStructure)
{
  const Standard_Integer aNbPr = myArray.Length();
  Standard_Integer i = 0;
  OpenGl_SequenceOfStructure::Iterator its;
  for (; i < aNbPr; i++)
  {
    OpenGl_SequenceOfStructure &aSeq = myArray(i);
    for (its.Init(aSeq); its.More(); its.Next())
    {
      if (its.Value() == AStructure)
      {
        aSeq.Remove(its);
        myNbStructures--;
        return i;
      }
    }
  }

  return -1;
}

/*----------------------------------------------------------------------*/

void OpenGl_PriorityList::Render (const Handle(OpenGl_Workspace) &AWorkspace) const
{
  const Standard_Integer aNbPr = myArray.Length();
  Standard_Integer i = 0;
  OpenGl_SequenceOfStructure::Iterator its;
  for (; i < aNbPr; i++)
  {
    for (its.Init(myArray(i)); its.More(); its.Next())
      its.Value()->Render(AWorkspace);
  }
}

//=======================================================================
//function : Append
//purpose  : 
//=======================================================================

Standard_Boolean OpenGl_PriorityList::Append (const OpenGl_PriorityList& theOther)
{
  // the source priority list shouldn't have more priorities
  const Standard_Integer aNbPriorities = theOther.NbPriorities ();
  if (aNbPriorities > NbPriorities ())
    return Standard_False;

  // add all structures to destination priority list
  Standard_Integer aIdx = 0;
  OpenGl_SequenceOfStructure::Iterator anIts;
  for (; aIdx < aNbPriorities; aIdx++)
  {
    const OpenGl_SequenceOfStructure& aSeq = theOther.myArray (aIdx);
    for (anIts.Init (aSeq); anIts.More (); anIts.Next ())
      Add (anIts.Value (), aIdx);
  }

  return Standard_True;
}

//=======================================================================
//function : NbPriorities
//purpose  : 
//=======================================================================

Standard_Integer OpenGl_PriorityList::NbPriorities() const
{
  return myArray.Length();
}
