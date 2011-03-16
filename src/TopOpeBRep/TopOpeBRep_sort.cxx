// File:	TopOpeBRep_sort.cxx
// Created:	Thu Jun  4 17:58:30 1998
// Author:	Jean Yves LEBEY
//		<jyl@langdox.paris1.matra-dtv.fr>

#include <TopOpeBRepDS_TKI.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <stdlib.h>
#include <TopOpeBRep_define.hxx>

typedef Standard_Address BOA_t;

int compll(const void* v1, const void* v2) {
  const TopOpeBRepDS_ListOfInterference* l1 = (const TopOpeBRepDS_ListOfInterference*)(*((long int*)v1));
  const TopOpeBRepDS_ListOfInterference* l2 = (const TopOpeBRepDS_ListOfInterference*)(*((long int*)v2));
  if      (l1->Extent() == 0) return (0);
  else if (l2->Extent() == 0) return (0);

  Handle(TopOpeBRepDS_CurvePointInterference) i1 = Handle(TopOpeBRepDS_CurvePointInterference)::DownCast(l1->First()); if (i1.IsNull()) return (0);
  Handle(TopOpeBRepDS_CurvePointInterference) i2 = Handle(TopOpeBRepDS_CurvePointInterference)::DownCast(l2->First()); if (i2.IsNull()) return (0);
  Standard_Real p1 = i1->Parameter();
  Standard_Real p2 = i2->Parameter();
  if      (p1 < p2) return (-1);
  else if (p1 > p2) return (1);
  else return (0);
  return (0);
}
     
Standard_EXPORT void BREP_sortonparameter2(TopOpeBRepDS_ListOfInterference& LOI)
{
  TopOpeBRepDS_TKI tki;tki.FillOnGeometry(LOI);
  Standard_Integer ng=0; for(tki.Init();tki.More();tki.Next()) ng++;
  size_t sng=(size_t)ng;size_t sad=sizeof(BOA_t);
  BOA_t* T = (BOA_t*)malloc((size_t)(sng*sad));TopOpeBRepDS_Kind K;Standard_Integer G;Standard_Integer j=0;
  for(tki.Init();tki.More();tki.Next(),j++) T[j]=(BOA_t)&(tki.Value(K,G));
  qsort(T,sng,sad,compll);
  LOI.Clear();for(j=0;j<ng;j++) {TopOpeBRepDS_ListOfInterference& l=*((TopOpeBRepDS_ListOfInterference*)T[j]);LOI.Append(l);}
  free(T);
}

Standard_EXPORT void BREP_sortonparameter(const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  Standard_Integer i=1, n=HDS->NbShapes();
  for (; i<=n; i++) {
    const TopoDS_Shape& s = HDS->Shape(i);
    if (s.ShapeType() != TopAbs_EDGE) continue;
    TopOpeBRepDS_ListOfInterference& LOI = BDS.ChangeShapeInterferences(s);
    BREP_sortonparameter2(LOI);
  }
}
