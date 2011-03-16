#include <IntSurf_LineOn2S.ixx>


IntSurf_LineOn2S::IntSurf_LineOn2S ()
{}


Handle(IntSurf_LineOn2S) IntSurf_LineOn2S::Split (const Standard_Integer Index)
{
  IntSurf_SequenceOfPntOn2S SS;
  mySeq.Split(Index,SS);
  Handle(IntSurf_LineOn2S) NS = new IntSurf_LineOn2S ();
  Standard_Integer i;
  Standard_Integer leng = SS.Length();
  for (i=1; i<=leng; i++) {
    NS->Add(SS(i));
  }
  return NS;
}


void IntSurf_LineOn2S::InsertBefore(const Standard_Integer index, const IntSurf_PntOn2S& P) { 
  if(index>mySeq.Length()) { 
    mySeq.Append(P);
  }
  else { 
    mySeq.InsertBefore(index,P);
  }
}

void IntSurf_LineOn2S::RemovePoint(const Standard_Integer index) { 
  mySeq.Remove(index);
}
