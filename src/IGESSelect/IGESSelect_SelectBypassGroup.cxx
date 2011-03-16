#include <IGESSelect_SelectBypassGroup.ixx>
#include <IGESData_IGESEntity.hxx>
#include <IGESBasic_Group.hxx>
#include <IGESBasic_GroupWithoutBackP.hxx>
#include <IGESBasic_OrderedGroup.hxx>
#include <IGESBasic_OrderedGroupWithoutBackP.hxx>
#include <Interface_Macros.hxx>


#define TypePourGroup 402


    IGESSelect_SelectBypassGroup::IGESSelect_SelectBypassGroup
  (const Standard_Integer level)
  : IFSelect_SelectExplore (level)    {  }


    Standard_Boolean  IGESSelect_SelectBypassGroup::Explore
  (const Standard_Integer level, const Handle(Standard_Transient)& ent,
   const Interface_Graph& G,  Interface_EntityIterator& explored) const
{
  DeclareAndCast(IGESBasic_Group,gr,ent);    // Group les regroupe tous
  if (gr.IsNull()) return Standard_True;

  Standard_Integer i, nb = gr->NbEntities();
  for (i = 1; i <= nb; i ++)  explored.AddItem (gr->Entity(i));
  return Standard_True;
}


    TCollection_AsciiString IGESSelect_SelectBypassGroup::ExploreLabel () const
      {  return TCollection_AsciiString ("Content of Groups");  }
