// Created on: 1998-06-04
// Created by: Jean Yves LEBEY
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
