#include <IGESSelect_RebuildDrawings.ixx>
#include <IGESSelect_ViewSorter.hxx>
#include <IFSelect_PacketList.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESDraw_Drawing.hxx>
#include <IGESDraw_DrawingWithRotation.hxx>
#include <IGESDraw_HArray1OfViewKindEntity.hxx>
#include <IGESData_ViewKindEntity.hxx>
#include <TColgp_HArray1OfXY.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <gp_XY.hxx>
#include <gp_Pnt2d.hxx>

#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>

#include <TColStd_Array1OfInteger.hxx>
#include <Interface_Macros.hxx>

#define PourDrawing 404


    IGESSelect_RebuildDrawings::IGESSelect_RebuildDrawings ()
    : IGESSelect_ModelModifier (Standard_True)    {  }


    void  IGESSelect_RebuildDrawings::Performing
  (IFSelect_ContextModif& ctx,
   const Handle(IGESData_IGESModel)& target,
   Interface_CopyTool& TC) const
{
//  On reconstruit les drawings qui peuvent l etre
//  Pour chaque drawing de l original, on regarde les composants transferes
//   (evt filtres par <ctx>). Pour cela, on s aide d un ViewSorter
//  Pour chaque drawing dont au moins un element a ete transfere :
//  - On passe le Drawing lui-meme, avec ses views, mais pas ses annotations
//    (c-a-d on le reconstruit)
//  - On reconnecte les views aux entites (cf Directory Part)

  DeclareAndCast(IGESData_IGESModel,original,ctx.OriginalModel());
  Standard_Integer nbo = original->NbEntities();
  TColStd_Array1OfInteger pris(0,nbo); pris.Init(0);

//  Entites a prendre en compte pour la reconstruction
//  NB : Les drawings deja transferes ne sont bien sur pas reconstruits !
  Handle(IGESSelect_ViewSorter) sorter = new IGESSelect_ViewSorter;
  sorter->SetModel(original);
  sorter->Add(original);
  for (ctx.Start(); ctx.More(); ctx.Next()) {
    pris.SetValue (original->Number(ctx.ValueOriginal()),1);
  }
  sorter->SortDrawings(ctx.OriginalGraph());
  Handle(IFSelect_PacketList) sets = sorter->Sets(Standard_True);
//  Regarder, pour chaque paquet, si au moins un element a ete copie
  Standard_Integer nbs = sets->NbPackets();
  for (Standard_Integer is = 1; is <= nbs; is ++) {
    Interface_EntityIterator setl = sets->Entities(is);
    Interface_EntityIterator newset;
    for (setl.Start(); setl.More(); setl.Next()) {
      Handle(Standard_Transient) newent;
      if (TC.Search(setl.Value(),newent)) newset.AddItem(newent);
    }
    if (newset.NbEntities() == 0) continue;
//    On en tient un : le transferer (le reconstruire)
    Handle(IGESData_IGESEntity) item = sorter->SetItem(is,Standard_True);
    if (item->TypeNumber() != PourDrawing) continue;
    if (item->IsKind(STANDARD_TYPE(IGESDraw_Drawing))) {
      DeclareAndCast(IGESDraw_Drawing,draw,item);
      Standard_Integer nbv = draw->NbViews();
      Handle(IGESDraw_HArray1OfViewKindEntity) views;
      if (nbv > 0) views = new IGESDraw_HArray1OfViewKindEntity (1,nbv);
//  Passer ses vues : toutes
//  Aussi les positions des vues
      Handle(TColgp_HArray1OfXY) origs;
      if (nbv > 0) origs = new TColgp_HArray1OfXY (1,nbv);
      for (Standard_Integer iv = 1; iv <= nbv; iv ++) {
	DeclareAndCast(IGESData_ViewKindEntity,aview,
		       TC.Transferred(draw->ViewItem(iv)));
	views->SetValue(iv,aview);
	target->AddEntity(aview);
	origs->SetValue(iv,draw->ViewOrigin(iv).XY());
      }
//  Frame : passer ce qui a ete transfere
      Handle(IGESData_HArray1OfIGESEntity) frame;
      Standard_Integer nba = draw->NbAnnotations();
      Interface_EntityIterator framelist;
      Standard_Integer ia; // svv Jan11 2000 : porting on DEC
      for (ia = 1; ia <= nba; ia ++) {
	Handle(Standard_Transient) annot;
	if (TC.Search(draw->Annotation(ia),annot)) framelist.GetOneItem(annot);
      }
      nba = framelist.NbEntities();  ia = 0;
      if (nba > 0) frame = new IGESData_HArray1OfIGESEntity (1,nba);
      for (framelist.Start(); framelist.More(); framelist.Next()) {
	ia ++;  frame->SetValue(ia,GetCasted(IGESData_IGESEntity,framelist.Value()));
      }
//  Cayest, fabriquer le nouveau Drawing et l ajouter
      Handle(IGESDraw_Drawing) newdraw = new IGESDraw_Drawing;
      newdraw->Init (views,origs,frame);
//  Reste le nom, + autres ? drawing unit, ...

//    Drawing With Rotation : quasiment identique
    } else if (item->IsKind(STANDARD_TYPE(IGESDraw_DrawingWithRotation))) {
      DeclareAndCast(IGESDraw_DrawingWithRotation,draw,item);
      Standard_Integer nbv = draw->NbViews();
      Handle(IGESDraw_HArray1OfViewKindEntity) views;
      if (nbv > 0) views = new IGESDraw_HArray1OfViewKindEntity (1,nbv);
//  Passer ses vues : toutes
//  Aussi les positions des vues .. et les rotations
      Handle(TColgp_HArray1OfXY) origs;
      if (nbv > 0) origs = new TColgp_HArray1OfXY (1,nbv);
      Handle(TColStd_HArray1OfReal) rots;
      if (nbv > 0) { rots  = new TColStd_HArray1OfReal (1,nbv); rots->Init(0.0); }

      for (Standard_Integer iv = 1; iv <= nbv; iv ++) {
	DeclareAndCast(IGESData_ViewKindEntity,aview,
		       TC.Transferred(draw->ViewItem(iv)));
	views->SetValue(iv,aview);
	target->AddEntity(aview);
	rots->SetValue(iv,draw->OrientationAngle(iv));
	origs->SetValue(iv,draw->ViewOrigin(iv).XY());
      }
//  Frame : passer ce qui a ete transfere
      Handle(IGESData_HArray1OfIGESEntity) frame;
      Standard_Integer nba = draw->NbAnnotations();
      Interface_EntityIterator framelist;
      Standard_Integer ia; // svv Jan11 2000 : porting on DEC
      for (ia = 1; ia <= nba; ia ++) {
	Handle(Standard_Transient) annot;
	if (TC.Search(draw->Annotation(ia),annot)) framelist.GetOneItem(annot);
      }
      nba = framelist.NbEntities();  ia = 0;
      if (nba > 0) frame = new IGESData_HArray1OfIGESEntity (1,nba);
      for (framelist.Start(); framelist.More(); framelist.Next()) {
	ia ++;  frame->SetValue(ia,GetCasted(IGESData_IGESEntity,framelist.Value()));
      }
//  Cayest, fabriquer le nouveau DrawingWithRotation et l ajouter
      Handle(IGESDraw_DrawingWithRotation) newdraw = new IGESDraw_DrawingWithRotation;
      newdraw->Init (views,origs,rots,frame);
//  Reste le nom, + autres ? drawing unit, ...
    }

//  Il faut encore mettre a jour les Views notees en Directory Part
//  Pour cela, considerer <setl>, pour chaque terme, regarder View()
//  si View() transfere, mettre a jour ...
    for (setl.Start(); setl.More(); setl.Next()) {
      DeclareAndCast(IGESData_IGESEntity,ent,setl.Value());
      Handle(IGESData_ViewKindEntity) vieworig, viewnew;
      vieworig = ent->View();
      if (vieworig.IsNull()) continue;
      if (!TC.Search(vieworig,viewnew)) continue;
      ent->InitView(viewnew);
    }
  }

}

    TCollection_AsciiString  IGESSelect_RebuildDrawings::Label () const
{  return TCollection_AsciiString("Rebuild Drawings (with empty views)");  }
