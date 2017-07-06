// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

//szv#9:PRO19565:04Oct99 loss of rotation matrix corrected

#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_GeneralModule.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_Protocol.hxx>
#include <IGESData_SingleParentEntity.hxx>
#include <IGESData_ToolLocation.hxx>
#include <IGESData_TransfEntity.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESData_ToolLocation,Standard_Transient)

#define TYPEFORASSOC 402

IGESData_ToolLocation::IGESData_ToolLocation (const Handle(IGESData_IGESModel)& amodel,
						const Handle(IGESData_Protocol)& protocol)
: thelib (protocol),
  therefs (0,amodel->NbEntities()),
  theassocs (0,amodel->NbEntities())
{
  theprec  = 1.e-05;
  themodel = amodel;
  therefs.Init(0);  theassocs.Init(0);
  Load();
}

void  IGESData_ToolLocation::Load ()
{
  // Pour chaque Entite, sauf Transf et Assoc (sauf SingleParent), on considere
  // ses "OwnShared" comme etant dependents
  Standard_Integer nb = themodel->NbEntities();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Handle(IGESData_IGESEntity) ent = themodel->Entity(i);
    if (ent->IsKind(STANDARD_TYPE(IGESData_TransfEntity))) continue;
    // Cas de SingleParentEntity
    if (ent->IsKind(STANDARD_TYPE(IGESData_SingleParentEntity))) {
      DeclareAndCast(IGESData_SingleParentEntity,assoc,ent);
      Standard_Integer nbc = assoc->NbChildren();
      Handle(IGESData_IGESEntity) parent = assoc->SingleParent();
      for (Standard_Integer j = 1; j <= nbc; j ++)
	SetParentAssoc(parent,assoc->Child(j));
      continue;
    }
    if (ent->TypeNumber() == TYPEFORASSOC) continue; // Assoc sauf SingleParent
    // Cas courant
    SetOwnAsDependent (ent); // qui opere
  }
}

void IGESData_ToolLocation::SetPrecision (const Standard_Real prec)
{  theprec = prec;  }

void IGESData_ToolLocation::SetReference (const Handle(IGESData_IGESEntity)& parent,
					  const Handle(IGESData_IGESEntity)& child)
{
  Standard_Integer np = themodel->Number(parent);
  Standard_Integer nc = themodel->Number(child);
  if (np == 0 || nc == 0) return;
  if (therefs(nc) > 0) np = -1;    // note ambigu
  therefs.SetValue(nc,np);
}

void IGESData_ToolLocation::SetParentAssoc (const Handle(IGESData_IGESEntity)& parent,
					    const Handle(IGESData_IGESEntity)& child)
{
  Standard_Integer np = themodel->Number(parent);
  Standard_Integer nc = themodel->Number(child);
  if (np == 0 || nc == 0) return;
  if (theassocs(nc) > 0) np = -1;    // note ambigu
  theassocs.SetValue(nc,np);
}

void  IGESData_ToolLocation::ResetDependences (const Handle(IGESData_IGESEntity)& child)
{
  Standard_Integer nc = themodel->Number(child);
  if (nc == 0) return;
  therefs.SetValue(nc,0);
  theassocs.SetValue(nc,0);
}

void  IGESData_ToolLocation::SetOwnAsDependent (const Handle(IGESData_IGESEntity)& ent)
{
  Standard_Integer CN;
  Handle(Interface_GeneralModule) gmodule;
  if (!thelib.Select(ent,gmodule,CN)) return;
  Handle(IGESData_GeneralModule) module =
    Handle(IGESData_GeneralModule)::DownCast (gmodule);
  Interface_EntityIterator list;
  module->OwnSharedCase(CN,ent,list);
  // Remarque : en toute rigueur, il faudrait ignorer les entites referencees
  // dont le SubordinateStatus vaut 0 ou 2 ...
  // Question : ce Status est-il toujours bien comme il faut ?
  for (list.Start(); list.More(); list.Next())
    SetReference (ent, GetCasted(IGESData_IGESEntity,list.Value()) );
}

//  #########################################################################
//  ########                        RESULTATS                        ########

Standard_Boolean  IGESData_ToolLocation::IsTransf
  (const Handle(IGESData_IGESEntity)& ent) const
{  return ent->IsKind(STANDARD_TYPE(IGESData_TransfEntity));  }

Standard_Boolean  IGESData_ToolLocation::IsAssociativity
  (const Handle(IGESData_IGESEntity)& ent) const
{  return (ent->TypeNumber() == TYPEFORASSOC);  }

Standard_Boolean  IGESData_ToolLocation::HasTransf
  (const Handle(IGESData_IGESEntity)& ent) const
{  return ent->HasTransf();  }

gp_GTrsf  IGESData_ToolLocation::ExplicitLocation
  (const Handle(IGESData_IGESEntity)& ent) const
{  return ent->Location();  }


Standard_Boolean  IGESData_ToolLocation::IsAmbiguous
  (const Handle(IGESData_IGESEntity)& ent) const
{
  Standard_Integer num = themodel->Number(ent);
  if (num == 0) return Standard_False;
  if (therefs(num) <  0 || theassocs(num) <  0) return Standard_True;
  if (therefs(num) != 0 && theassocs(num) != 0) return Standard_True;
  return Standard_False;
}

Standard_Boolean  IGESData_ToolLocation::HasParent
  (const Handle(IGESData_IGESEntity)& ent) const
{
  Standard_Integer num = themodel->Number(ent);
  if (num == 0) return Standard_False;
  if (therefs(num) <  0 || theassocs(num) <  0) throw Standard_DomainError("IGESData_ToolLocation : HasParent");
  if (therefs(num) != 0 && theassocs(num) != 0) throw Standard_DomainError("IGESData_ToolLocation : HasParent");
  if (therefs(num) != 0 || theassocs(num) != 0) return Standard_True;
  return Standard_False;
}

Handle(IGESData_IGESEntity)  IGESData_ToolLocation::Parent
  (const Handle(IGESData_IGESEntity)& ent) const
{
  Handle(IGESData_IGESEntity) parent;
  Standard_Integer num = themodel->Number(ent);
  if (num == 0) return parent;
  if (therefs(num) <  0 || theassocs(num) <  0) throw Standard_DomainError("IGESData_ToolLocation : Parent");
  if (therefs(num) != 0 && theassocs(num) != 0) throw Standard_DomainError("IGESData_ToolLocation : Parent");
  if (therefs(num)   != 0) parent = themodel->Entity (therefs   (num));
  if (theassocs(num) != 0) parent = themodel->Entity (theassocs (num));
  return parent;
}

Standard_Boolean  IGESData_ToolLocation::HasParentByAssociativity
  (const Handle(IGESData_IGESEntity)& ent) const
{
  Standard_Integer num = themodel->Number(ent);
  if (num == 0) return Standard_False;
  if (therefs(num) <  0 || theassocs(num) <  0) throw Standard_DomainError("IGESData_ToolLocation : HasParentByAssociativity");
  if (therefs(num) != 0 && theassocs(num) != 0) throw Standard_DomainError("IGESData_ToolLocation : HasParentByAssociativity");
  if (theassocs(num) != 0) return Standard_True;
  return Standard_False;
}

gp_GTrsf  IGESData_ToolLocation::ParentLocation
  (const Handle(IGESData_IGESEntity)& ent) const
{
  gp_GTrsf locat;    // par defaut, identite
  Handle(IGESData_IGESEntity) parent = Parent(ent);
  // Definition recursive
  if (!parent.IsNull()) locat = EffectiveLocation(parent);
  return locat;
}

gp_GTrsf  IGESData_ToolLocation::EffectiveLocation
  (const Handle(IGESData_IGESEntity)& ent) const
{
  gp_GTrsf locat  = ent->Location();
  // Combiner Transf et ParentLocation
  locat.PreMultiply (ParentLocation(ent));    // ne pas se tromper de sens !
  return locat;
}

Standard_Boolean  IGESData_ToolLocation::AnalyseLocation (const gp_GTrsf& loc,
							  gp_Trsf& result) const
{  return ConvertLocation (theprec,loc,result);  }

Standard_Boolean  IGESData_ToolLocation::ConvertLocation (const Standard_Real prec,
							  const gp_GTrsf& loc,
							  gp_Trsf& result,
							  const Standard_Real unit)
{
  if (result.Form() != gp_Identity) result = gp_Trsf();  // Identite forcee au depart
  // On prend le contenu de <loc>. Attention a l adressage
  gp_XYZ v1 ( loc.Value(1,1), loc.Value(1,2), loc.Value(1,3) );
  gp_XYZ v2 ( loc.Value(2,1), loc.Value(2,2), loc.Value(2,3) );
  gp_XYZ v3 ( loc.Value(3,1), loc.Value(3,2), loc.Value(3,3) );
  // A-t-on affaire a une similitude ?
  Standard_Real m1 = v1.Modulus();
  Standard_Real m2 = v2.Modulus();
  Standard_Real m3 = v3.Modulus();
  // D abord est-elle singuliere cette matrice ?
  if (m1 < prec || m2 < prec || m3 < prec) return Standard_False;
  Standard_Real mm = (m1+m2+m3)/3.; // voici la Norme moyenne, cf Scale
  if ( Abs(m1 - mm) > prec*mm ||
       Abs(m2 - mm) > prec*mm ||
       Abs(m3 - mm) > prec*mm ) return Standard_False;
  v1.Divide(m1);
  v2.Divide(m2);
  v3.Divide(m3);
  if ( Abs(v1.Dot(v2)) > prec ||
       Abs(v2.Dot(v3)) > prec ||
       Abs(v3.Dot(v1)) > prec ) return Standard_False;
  // Ici, Orthogonale et memes normes. En plus on l a Normee
  // Restent les autres caracteristiques :
  if (Abs(mm - 1.) > prec) result.SetScale(gp_Pnt(0,0,0),mm);
  gp_XYZ tp = loc.TranslationPart();
  if (unit != 1.) tp.Multiply(unit);
  if (tp.X() != 0. || tp.Y() != 0. || tp.Z() != 0.) result.SetTranslationPart(tp);
  // On isole le cas de l Identite (tellement facile et avantageux)
  if (v1.X() != 1. || v1.Y() != 0. || v1.Z() != 0. ||
      v2.X() != 0. || v2.Y() != 1. || v2.Z() != 0. ||
      v3.X() != 0. || v3.Y() != 0. || v3.Z() != 1. ) {
    // Pas Identite : vraie construction depuis un Ax3
    gp_Dir d1(v1);
    gp_Dir d2(v2);
    gp_Dir d3(v3);
    gp_Ax3 axes (gp_Pnt(0,0,0),d3,d1);
    d3.Cross(d1);
    if (d3.Dot(d2) < 0) axes.YReverse();
    gp_Trsf transf;
    transf.SetTransformation(axes);
    result *= transf; //szv#9:PRO19565:04Oct99
  }
  return Standard_True;
}
