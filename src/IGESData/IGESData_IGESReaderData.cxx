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


#include <IGESData_DirPart.hxx>
#include <IGESData_GlobalSection.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESType.hxx>
#include <IGESData_UndefinedEntity.hxx>
#include <Interface_Check.hxx>
#include <Interface_FileParameter.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ParamList.hxx>
#include <Interface_ParamSet.hxx>
#include <Interface_ParamType.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESData_IGESReaderData,Interface_FileReaderData)

IGESData_IGESReaderData::IGESData_IGESReaderData
  (const Standard_Integer nbe, const Standard_Integer nbp)
    : Interface_FileReaderData (nbe,nbp) , thectyp (0,0) , thedirs(0,nbe)  
{
  thestep = IGESData_ReadDir;  thedefw = 0.;
  theparh = new Interface_ParamSet(30);
  thestar = new TColStd_HSequenceOfHAsciiString();
  thechk = new Interface_Check;
}


    void IGESData_IGESReaderData::AddStartLine
  (const Standard_CString aval)
{
  thestar->Append (new TCollection_HAsciiString(aval));
}

    Handle(TColStd_HSequenceOfHAsciiString) IGESData_IGESReaderData::StartSection
  () const
      {  return thestar;  }

    void IGESData_IGESReaderData::AddGlobal
  (const Interface_ParamType atype, const Standard_CString aval)
{
  theparh->Append(aval,(int)strlen(aval),atype,0);
}

    void IGESData_IGESReaderData::SetGlobalSection ()
      {  thehead.Init(theparh,thechk);  }

    const IGESData_GlobalSection& IGESData_IGESReaderData::GlobalSection () const
      {  return thehead;  }

    void IGESData_IGESReaderData::SetDirPart
  (const Standard_Integer num,
   const Standard_Integer  i1, const Standard_Integer  i2,
   const Standard_Integer  i3, const Standard_Integer  i4,
   const Standard_Integer  i5, const Standard_Integer  i6,
   const Standard_Integer  i7, const Standard_Integer  i8,
   const Standard_Integer  i9, const Standard_Integer i10,
   const Standard_Integer i11, const Standard_Integer i12,
   const Standard_Integer i13, const Standard_Integer i14,
   const Standard_Integer i15, const Standard_Integer i16,
   const Standard_Integer i17,
   const Standard_CString res1,  const Standard_CString res2,
   const Standard_CString label, const Standard_CString subs)
{
  IGESData_DirPart& DP = thedirs(num);
  DP.Init(i1,i2,i3,i4,i5,i6,i7,i8,i9,i10,i11,i12,i13,i14,
	  i15,i16,i17,res1,res2,label,subs);
////  thedirs(num) = DP;  // verifier si utile
  //InitParams(num); gka optimization memory
}


    const IGESData_DirPart& IGESData_IGESReaderData::DirPart
  (const Standard_Integer num) const
      {  return thedirs(num);  }

    void IGESData_IGESReaderData::DirValues
  (const Standard_Integer num,
   Standard_Integer&  i1, Standard_Integer&  i2, Standard_Integer&  i3,
   Standard_Integer&  i4, Standard_Integer&  i5, Standard_Integer&  i6,
   Standard_Integer&  i7, Standard_Integer&  i8, Standard_Integer&  i9,
   Standard_Integer& i10, Standard_Integer& i11, Standard_Integer& i12,
   Standard_Integer& i13, Standard_Integer& i14, Standard_Integer& i15,
   Standard_Integer& i16, Standard_Integer& i17,
   Standard_CString& res1,  Standard_CString& res2,
   Standard_CString& label, Standard_CString& subs) const
{
  thedirs(num).Values(i1,i2,i3,i4,i5,i6,i7,i8,i9,i10,i11,i12,i13,i14,
		      i15,i16,i17,res1,res2,label,subs);
}

    IGESData_IGESType IGESData_IGESReaderData::DirType
  (const Standard_Integer num) const
      {  return thedirs(num).Type();  }

    Standard_Integer IGESData_IGESReaderData::NbEntities () const
      {  return thedirs.Upper();  }

    Standard_Integer IGESData_IGESReaderData::FindNextRecord
  (const Standard_Integer num) const
{
  if (num >= thedirs.Upper()) return 0;
  else return (num + 1);
}


// Reference a d'autres entites : c'est a la fois tres simple et problematique
// Tres simple : une reference a une entite est un numero (dans directory list)
// qui vaut (2*N-1) si N est le rang vrai de l'entite
// Problematique : ce numero est un Entier ... rien ne le distingue d'un autre
// D'ou critere : tout entier impair inferieur a 2*NbRecords PEUT etre une
// reference ... C'est a chaque entite de faire ensuite son tri ...
//  Attention, une reference peut etre donnee en "Pointeur Negatif"
// N.B.: DirPart non concernes (lecture specifique assuree par IGESEntity)

    void IGESData_IGESReaderData::SetEntityNumbers ()
{
//   On essaie de se baser uniquement sur calcul de IGESRead
/*
  Standard_Integer nbd = thedirs.Upper();
  for (Standard_Integer i = 1; i <= nbd; i ++) {
    Standard_Integer nbp = NbParams(i);
    for (Standard_Integer j = 1; j <= nbp; j ++) {
      Interface_FileParameter& FP = ChangeParam(i,j);
      if (FP.ParamType() == Interface_ParamInteger) {
	Standard_Integer val = atoi(FP.CValue());
	if (val > 0) {
	  if (val != ((val/2) *2) && val < 2*nbd) {  // candidat possible
	    FP.SetEntityNumber((val+1)/2);
	  }
	} else if (val < 0) {
	  Standard_Integer mval = -val;
	  if (mval != ((mval/2) *2) && mval < 2*nbd) {  // candidat possible
	    FP.SetEntityNumber((mval+1)/2);
	  }
	}

      }
    }
  }
*/
}


//=======================================================================
//function : GlobalCheck
//purpose  : 
//=======================================================================

Handle(Interface_Check) IGESData_IGESReaderData::GlobalCheck () const
{
  return thechk;
}


//=======================================================================
//function : SetDefaultLineWeight
//purpose  : 
//=======================================================================

void IGESData_IGESReaderData::SetDefaultLineWeight (const Standard_Real defw)
{
  thedefw = defw;
}


//=======================================================================
//function : DefaultLineWeight
//purpose  : 
//=======================================================================

Standard_Real IGESData_IGESReaderData::DefaultLineWeight () const
{
  return thedefw;
}
