// File:	StepFile_Read.cxx
// Created:	Fri Aug 30 11:31:31 1991
// Author:	Christian CAILLET
//		<cky@phobox>

//   StepFile_Read

//   routine assurant l enchainement des operations de lecture d un fichier
//   STEP dans un StepModel, en fonction d une cle de reconnaissance
//   Retour de la fonction :
//     0 si OK  (le StepModel a ete charge)
//    -1 si abandon car fichier pas pu etre ouvert
//     1 si erreur en cours de lecture

//   Compilation conditionnelle : concerne les mesures de performances


#include <stdio.h>
#include "recfile.ph"
#include "stepread.ph"
extern "C" void recfile_modeprint (int mode);  // controle trace recfile
          // recfile_modeprint est declare a part

#include <Interface_ParamType.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_Check.hxx>
#include <StepData_Protocol.hxx>

#include <StepData_StepReaderData.hxx>
#include <StepData_StepReaderTool.hxx>
#include <StepFile_Read.hxx>

#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Interface_InterfaceError.hxx>

#include <Message_Messenger.hxx>
#include <Message.hxx>

#define CHRONOMESURE
#ifdef CHRONOMESURE
# include <OSD_Timer.hxx>
#endif

//  ##  ##  ##  ##    ON SAURA AU MOINS TRAITER UndefinedEntity  ##  ##  ##  ##

static Handle(Interface_Check) checkread = new Interface_Check;
static Standard_Integer modepr = 1;

void StepFile_ReadTrace (const Standard_Integer mode)
{
  modepr = mode;   // recfile_modeprint est rappele a chaque lecture de fichier
}


static Standard_Integer StepFile_Read
 (char* nomfic,
  const Handle(StepData_StepModel)& stepmodel,
  const Handle(StepData_Protocol)& protocol,
  const Handle(StepData_FileRecognizer)& recoheader,
  const Handle(StepData_FileRecognizer)& recodata);


Standard_Integer StepFile_Read
 (char* nomfic,
  const Handle(StepData_StepModel)& stepmodel,
  const Handle(StepData_FileRecognizer)& recoheader,
  const Handle(StepData_FileRecognizer)& recodata)
{
  return StepFile_Read
    (nomfic,stepmodel,
     Handle(StepData_Protocol)::DownCast(Interface_Protocol::Active()),
     recoheader,recodata);
}

Standard_Integer StepFile_Read
 (char* nomfic,
  const Handle(StepData_StepModel)& stepmodel,
  const Handle(StepData_FileRecognizer)& recoheader,
  const Handle(StepData_Protocol)& protocol)
{
  Handle(StepData_FileRecognizer) nulreco;
  return StepFile_Read (nomfic,stepmodel,protocol,recoheader,nulreco);
}

Standard_Integer StepFile_Read
 (char* nomfic,
  const Handle(StepData_StepModel)& stepmodel,
  const Handle(StepData_Protocol)& protocol)
{
  Handle(StepData_FileRecognizer) nulreco;
  return StepFile_Read (nomfic,stepmodel,protocol,nulreco,nulreco);
}

//  ##  ##  ##  ##  ##  ##    Corps de la Routine    ##  ##  ##  ##  ##  ##

static Interface_ParamType LesTypes[10];   // passage types (recstep/Interface)

Standard_Integer StepFile_Read
 (char* nomfic,
  const Handle(StepData_StepModel)& stepmodel,
  const Handle(StepData_Protocol)& protocol,
  const Handle(StepData_FileRecognizer)& recoheader,
  const Handle(StepData_FileRecognizer)& recodata)

{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  char *ficnom = nomfic ;  // because const (non reconnu par C)

  checkread->Clear();
  recfile_modeprint ( (modepr > 0 ? modepr-1 : 0) );
  FILE* newin = stepread_setinput(ficnom);
  if (!newin) return -1;
#ifdef CHRONOMESURE
  Standard_Integer n ; 
  OSD_Timer c ; 
  c.Reset () ; 
  c.Start();
  sout << "      ...    Step File Reading : " << ficnom << "" << endl;  
#endif

  try {
    OCC_CATCH_SIGNALS
    if (stepread () != 0) {  lir_file_fin(3);  stepread_endinput (newin,ficnom);  return 1;  }
  }
  catch (Standard_Failure) {
    sout << " ...  Exception Raised while reading Step File : " << ficnom << ":\n" << endl;
    sout << Standard_Failure::Caught()->GetMessageString();  
    sout << "    ..." << endl;
    lir_file_fin(3);  
    stepread_endinput (newin,ficnom);  
    return 1;
  }
  // Continue reading of file despite of possible fails
  //if (checkread->HasFailed()) {  lir_file_fin(3);  stepread_endinput (newin,ficnom);  return 1;  }
#ifdef CHRONOMESURE
  sout << "      ...    STEP File   Read    ... " << endl;  
  c.Show(); 
#endif


//  Creation du StepReaderData

  LesTypes[rec_argNondef]    = Interface_ParamVoid ;
  LesTypes[rec_argSub]       = Interface_ParamSub ;
  LesTypes[rec_argIdent]     = Interface_ParamIdent ;
  LesTypes[rec_argInteger]   = Interface_ParamInteger ;
  LesTypes[rec_argFloat]     = Interface_ParamReal ;
  LesTypes[rec_argEnum]      = Interface_ParamEnum ;
  LesTypes[rec_argBinary]    = Interface_ParamBinary ;
  LesTypes[rec_argText]      = Interface_ParamText ;
  LesTypes[rec_argHexa]      = Interface_ParamHexa ;
  LesTypes[rec_argMisc]      = Interface_ParamMisc ;

  Standard_Integer nbhead, nbrec, nbpar;
  lir_file_nbr (&nbhead,&nbrec,&nbpar);  // renvoi par lex/yacc
  Handle(StepData_StepReaderData) undirec =
    new StepData_StepReaderData(nbhead,nbrec,nbpar);  // creation tableau de records

  for ( Standard_Integer nr = 1; nr <= nbrec; nr ++) {
    int nbarg; char* ident; char* typrec ;
    lir_file_rec (&ident, &typrec, &nbarg);
    undirec->SetRecord (nr, ident, typrec, nbarg);

    if (nbarg>0) {
      int typa; char* val;
      Interface_ParamType newtype;
      while(lir_file_arg (&typa, &val) == 1) {
        newtype = LesTypes[typa] ;
        undirec->AddStepParam (nr, val, newtype);
      }
    }
    undirec->InitParams(nr);
    lir_file_finrec();
  }
  lir_file_fin(1);
//  on a undirec pret pour la suite

#ifdef CHRONOMESURE
  sout << "      ... Step File loaded  ... " << endl; 
  c.Show();
  sout << "   "<< undirec->NbRecords () <<
      " records (entities,sub-lists,scopes), "<< nbpar << " parameters\n" << endl;
#endif

//   Analyse : par StepReaderTool

  StepData_StepReaderTool readtool (undirec,protocol);
  readtool.SetErrorHandle (Standard_True);

  readtool.PrepareHeader(recoheader);  // Header. reco nul -> pour Protocol
  readtool.Prepare(recodata);          // Data.   reco nul -> pour Protocol
#ifdef CHRONOMESURE
  sout << "      ... Parameters prepared ... "; 
  c.Show(); 
#endif

  readtool.LoadModel(stepmodel);
  if (stepmodel->Protocol().IsNull()) stepmodel->SetProtocol (protocol);
  lir_file_fin(2);
#ifdef CHRONOMESURE
  sout << "      ...   Objets analysed  ... " << endl; 
  c.Show();
  n = stepmodel->NbEntities() ;
  sout << "  STEP Loading done : " << n << " Entities" << endl;
#endif

  stepread_endinput (newin,ficnom);  return 0 ;
}

void StepFile_Interrupt (char* mess)
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  sout << "    ****    StepFile Error : " << mess << "    ****" << endl;
  checkread->AddFail(mess);
}
