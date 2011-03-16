#include <IGESSelect_SelectFromDrawing.ixx>
#include <IGESData_IGESEntity.hxx>
#include <Interface_Macros.hxx>

#define PourDrawing 404


    IGESSelect_SelectFromDrawing::IGESSelect_SelectFromDrawing ()    {  }


    Interface_EntityIterator  IGESSelect_SelectFromDrawing::RootResult
  (const Interface_Graph& G) const
{
  Interface_EntityIterator list, views;
  Interface_EntityIterator draws = InputResult(G);
  if (draws.NbEntities() == 0) return list;
  Standard_Integer nb = G.Size();
  char* nums = new char[nb+1];
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = 1; i <= nb; i ++) nums[i] = 0;

//  Pour chaque Drawing : prendre d une part l integralite de son contenu,
//  (c-a-d avec le "Frame"), d autre part les entites attachees a ses vues
  for (draws.Start(); draws.More(); draws.Next()) {
    DeclareAndCast(IGESData_IGESEntity,igesent,draws.Value());
    if (igesent.IsNull()) continue;
    if (igesent->TypeNumber() != PourDrawing) continue;
    list.GetOneItem (igesent);
    Interface_EntityIterator someviews = G.Shareds (draws.Value());
    list.AddList (someviews.Content());
    for (someviews.Start(); someviews.More(); someviews.Next()) {
      DeclareAndCast(IGESData_ViewKindEntity,igesview,someviews.Value());
      Standard_Integer nv = G.EntityNumber(igesview);
      if (nv > 0 && nv <= nb) nums[nv] = 1;
    }
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


    TCollection_AsciiString  IGESSelect_SelectFromDrawing::Label () const
{  return TCollection_AsciiString ("Entities attached to a Drawing");  }
