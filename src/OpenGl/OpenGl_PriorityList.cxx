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

void OpenGl_PriorityList::Remove (const OpenGl_Structure *AStructure)
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
        return;
      }
    }
  }
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

/*----------------------------------------------------------------------*/
