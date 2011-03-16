// File:	BOPTools_DEInfo.cxx
// Created:	Wed Sep 12 13:02:17 2001
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOPTools_DEInfo.ixx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

//=======================================================================
// function: BOPTools_DEInfo::BOPTools_DEInfo
// purpose: 
//=======================================================================
   BOPTools_DEInfo::BOPTools_DEInfo()
{}

//=======================================================================
// function: SetVertex
// purpose: 
//=======================================================================
  void BOPTools_DEInfo::SetVertex(const Standard_Integer nV)
{
  myVertex=nV;
}

//=======================================================================
// function: Vertex
// purpose: 
//=======================================================================
  Standard_Integer BOPTools_DEInfo::Vertex()const  
{
  return myVertex;
}
//=======================================================================
// function: SetFaces
// purpose: 
//=======================================================================
  void BOPTools_DEInfo::SetFaces(const TColStd_ListOfInteger& aLI)
{
  myFaces.Clear();

  TColStd_ListIteratorOfListOfInteger anIt(aLI);
  for (; anIt.More();  anIt.Next()) {
    Standard_Integer nF=anIt.Value();
    myFaces.Append(nF);
  }
}
//=======================================================================
// function: Faces
// purpose: 
//=======================================================================
  const TColStd_ListOfInteger& BOPTools_DEInfo::Faces()const 
{
  return myFaces;
}
//=======================================================================
// function: ChangeFaces
// purpose: 
//=======================================================================
  TColStd_ListOfInteger& BOPTools_DEInfo::ChangeFaces() 
{
  return myFaces;
}


