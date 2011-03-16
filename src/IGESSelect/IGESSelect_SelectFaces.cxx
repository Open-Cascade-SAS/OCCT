#include <IGESSelect_SelectFaces.ixx>
#include <IGESData_IGESEntity.hxx>
#include <IGESGeom_Plane.hxx>
#include <IGESBasic_SingleParent.hxx>
#include <IGESBasic_Group.hxx>
#include <IGESSolid_ManifoldSolid.hxx>
#include <IGESSolid_Shell.hxx>

#include <Interface_Macros.hxx>


    IGESSelect_SelectFaces::IGESSelect_SelectFaces ()
    : IFSelect_SelectExplore (-1)    {  }


    Standard_Boolean  IGESSelect_SelectFaces::Explore
  (const Standard_Integer level, const Handle(Standard_Transient)& ent,
   const Interface_Graph& G, Interface_EntityIterator& explored) const
{
  DeclareAndCast(IGESData_IGESEntity,igesent,ent);
  if (igesent.IsNull()) return Standard_False;
  Standard_Integer igt = igesent->TypeNumber();

//   Cas clairs et nets : Faces typees comme telles

  if (igt == 510 || igt == 144 || igt == 143) return Standard_True;

//   Surfaces Libres
  if (igt == 114 || igt == 118 || igt == 120 || igt == 122 || igt == 128 || igt == 140)
    return Standard_True;

//   Cas du Plane
  if (igt == 108) {
    DeclareAndCast(IGESGeom_Plane,pln,ent);
    return pln->HasBoundingCurve();
  }

//   A present, contenants possibles

//  SingleParent
  if (igt == 402 && igesent->FormNumber() == 9) {
    DeclareAndCast(IGESBasic_SingleParent,sp,ent);
    if (sp.IsNull()) return Standard_False;
    explored.AddItem (sp->SingleParent());
    Standard_Integer i,nb = sp->NbChildren();
    for (i = 1; i <= nb; i ++) explored.AddItem (sp->Child(i));
    return Standard_True;
  }

//  Groups ... en dernier de la serie 402
  if (igt == 402) {
    DeclareAndCast(IGESBasic_Group,gr,ent);
    if (gr.IsNull()) return Standard_False;
    Standard_Integer i, nb = gr->NbEntities();
    for (i = 1; i <= nb; i ++)  explored.AddItem (gr->Entity(i));
    return Standard_True;
  }

//  ManifoldSolid 186  -> Shells
  if (igt == 186) {
    DeclareAndCast(IGESSolid_ManifoldSolid,msb,ent);
    explored.AddItem (msb->Shell());
    Standard_Integer i, nb = msb->NbVoidShells();
    for (i = 1; i <= nb; i ++)  explored.AddItem (msb->VoidShell(i));
    return Standard_True;
  }

//  Shell 514 -> Faces
  if (igt == 514) {
    DeclareAndCast(IGESSolid_Shell,sh,ent);
    Standard_Integer i, nb = sh->NbFaces();
    for (i = 1; i <= nb; i ++)  explored.AddItem (sh->Face(i));
    return Standard_True;
  }

//  Pas trouve
  return Standard_False;
}

    TCollection_AsciiString IGESSelect_SelectFaces::ExploreLabel () const
      {  return TCollection_AsciiString ("Faces");  }
