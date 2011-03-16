#include <IGESSelect_SelectFromSingleView.ixx>
#include <IGESData_IGESEntity.hxx>
#include <Interface_Macros.hxx>



    IGESSelect_SelectFromSingleView::IGESSelect_SelectFromSingleView ()    {  }


    Interface_EntityIterator  IGESSelect_SelectFromSingleView::RootResult
  (const Interface_Graph& G) const
{
  Interface_EntityIterator list;
  Interface_EntityIterator views = InputResult(G);
  if (views.NbEntities() == 0) return list;
  Standard_Integer nb = G.Size();
  char* nums = new char[nb+1];
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = 1; i <= nb; i ++) nums[i] = 0;
  for (views.Start(); views.More(); views.Next()) {
    Standard_Integer nv = G.EntityNumber(views.Value());
    if (nv > 0 && nv <= nb) nums[nv] = 1;
  }
  for (i = 1; i <= nb; i ++) {
//    if (!G.IsPresent(i)) continue;
    DeclareAndCast(IGESData_IGESEntity,igesent,G.Entity(i));
    if (igesent.IsNull()) continue;
    Standard_Integer nv = G.EntityNumber (igesent->View());
    if (nv > 0 && nv <= nb) list.GetOneItem(igesent);
  }
  delete nums;
  return list;
}


    TCollection_AsciiString  IGESSelect_SelectFromSingleView::Label () const
{  return TCollection_AsciiString ("Entities attached to a single View");  }
