#include <BOP_HistoryCollector.ixx>

#include <TopExp_Explorer.hxx>

BOP_HistoryCollector::BOP_HistoryCollector()
{
  myOp = BOP_UNKNOWN;
  myHasDeleted = Standard_False;
}

BOP_HistoryCollector::BOP_HistoryCollector(const TopoDS_Shape& theShape1,
					   const TopoDS_Shape& theShape2,
					   const BOP_Operation theOperation)
{
  myOp = theOperation;
  myS1 = theShape1;
  myS2 = theShape2;
  myHasDeleted = Standard_False;
}

void BOP_HistoryCollector::SetResult(const TopoDS_Shape&       theResult,
				     const BOPTools_PDSFiller& theDSFiller) 
{
  myResult = theResult;
}

const TopTools_ListOfShape& BOP_HistoryCollector::Generated(const TopoDS_Shape& S) 
{
  if(S.IsNull())
    return myEmptyList;

  if(myGenMap.IsBound(S)) {
    return myGenMap.Find(S);
  }
  return myEmptyList;
}

const TopTools_ListOfShape& BOP_HistoryCollector::Modified(const TopoDS_Shape& S) 
{
  if(S.IsNull())
    return myEmptyList;

  if(myModifMap.IsBound(S)) {
    return myModifMap.Find(S);
  }
  return myEmptyList;
}

Standard_Boolean BOP_HistoryCollector::IsDeleted(const TopoDS_Shape& S) 
{
  if(S.IsNull())
    return Standard_True;

  TopAbs_ShapeEnum aType = S.ShapeType();
  TopExp_Explorer anExp(myResult, aType);

  for(; anExp.More(); anExp.Next()) {
    if(S.IsSame(anExp.Current()))
      return Standard_False;
  }

  if(myModifMap.IsBound(S)) {
    if(!myModifMap(S).IsEmpty())
      return Standard_False;
  }

  if(myGenMap.IsBound(S)) {
    if(!myGenMap(S).IsEmpty())
      return Standard_False;
  }
  return Standard_True;
}

Standard_Boolean BOP_HistoryCollector::HasGenerated() const
{
  if(!myGenMap.IsEmpty()) {
    return Standard_True;
  }
  return Standard_False;
}

Standard_Boolean BOP_HistoryCollector::HasModified() const
{
  if(!myModifMap.IsEmpty()) {
    return Standard_True;
  }
  return Standard_False;
}

Standard_Boolean BOP_HistoryCollector::HasDeleted() const
{
  return myHasDeleted;
}
