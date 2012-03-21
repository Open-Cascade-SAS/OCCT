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

#include <XSControl_SignTransferStatus.ixx>
#include <Transfer_Binder.hxx>
#include <TCollection_AsciiString.hxx>
#include <Interface_Check.hxx>
//#include <stdio.h>

static TCollection_AsciiString& themes()
{
  static TCollection_AsciiString tm;
  return tm;
}
// si resultat avec type: a exploiter tout de suite !

XSControl_SignTransferStatus::XSControl_SignTransferStatus ()
: IFSelect_Signature("Transfer Status")    {  }

XSControl_SignTransferStatus::XSControl_SignTransferStatus
  (const Handle(XSControl_TransferReader)& TR)
: IFSelect_Signature("Transfer Status") ,
  theTR (TR)    {  }

void  XSControl_SignTransferStatus::SetReader
  (const Handle(XSControl_TransferReader)& TR)
      {  theTR = TR;  }

void  XSControl_SignTransferStatus::SetMap
  (const Handle(Transfer_TransientProcess)& TP)
      {  theTP = TP;  }

Handle(Transfer_TransientProcess)  XSControl_SignTransferStatus::Map () const
      {  return theTP;  }

Handle(XSControl_TransferReader)  XSControl_SignTransferStatus::Reader () const
      {  return theTR;  }



// BinderStatus retourne une valeur :
// 0 Binder Null.   1 void  2 Warning seul  3 Fail seul
// 11 Resultat OK. 12 Resultat+Warning. 13 Resultat+Fail
// 20 Abnormal (Interrupted)

static Standard_Integer BinderStatus (const Handle(Transfer_Binder)& binder)
{
  Standard_Integer stat = 0;
  if (binder.IsNull())  return 0;
  Interface_CheckStatus cst = binder->Check()->Status();
  Transfer_StatusExec est = binder->StatusExec ();
  Standard_Boolean res = binder->HasResult();
  if (est == Transfer_StatusRun || est == Transfer_StatusLoop) return 20;
  if      (cst == Interface_CheckOK)      stat = (res ? 11 : 1);
  else if (cst == Interface_CheckWarning) stat = (res ? 12 : 2);
  else if (cst == Interface_CheckFail)    stat = (res ? 13 : 3);

  return stat;
}


Standard_CString  XSControl_SignTransferStatus::Value
  (const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& /*model*/) const
{
  if (ent.IsNull()) return "";
  Handle(Transfer_TransientProcess)  TP = theTP;
  if (TP.IsNull() && !theTR.IsNull()) TP = theTR->TransientProcess();
  if (TP.IsNull()) return "";

  Handle(Transfer_Binder) binder = TP->Find(ent);

  Standard_Integer stat = BinderStatus (binder);

  if (stat <= 1) return "";
  if (stat == 2) return "Warning";
  if (stat == 3) return "Fail";
  if (stat == 20) return "Fail on run";

  themes().Clear();
  if (stat > 10) {
//  Y a un resultat : donner son type
    Handle(Transfer_Binder) bnd = binder;
    Standard_Integer hasres = Standard_False;
    while (!bnd.IsNull()) {
      if (bnd->Status() != Transfer_StatusVoid) {
	if (!hasres) themes().AssignCat("Result:");
	else themes().AssignCat(",");
	themes().AssignCat(bnd->ResultTypeName());
	hasres = Standard_True;
      }
      bnd = bnd->NextResult();
    }
//    if (stat == 11) sprintf(themes,"Result:%s",binder->ResultTypeName());
    if (stat == 12) themes().AssignCat("/Warning");
    if (stat == 13) themes().AssignCat("/Fail");
  }
  return themes().ToCString();
}
