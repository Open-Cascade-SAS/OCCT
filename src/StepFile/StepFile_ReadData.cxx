/* 
 Copyright (c) 1999-2020 OPEN CASCADE SAS

 This file is part of Open CASCADE Technology software library.

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License version 2.1 as published
 by the Free Software Foundation, with special exception defined in the file
 OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
 distribution for complete text of the license and disclaimer of any warranty.

 Alternatively, this file may be used under the terms of Open CASCADE
 commercial license or contractual agreement.
*/ 

#include <StepFile_ReadData.hxx>

#include <Interface_Check.hxx>
#include <TCollection_AsciiString.hxx>

// Constant litterales
namespace TextValue
{
  static char SubList[] = "/* (SUB) */";
  static char Scope[] = "SCOPE";
  static char Nil[] = " ";
  static char Sub1[] = "$1";  /* optimisation ... */
  static char Sub2[] = "$2";
  static char ArgType1[] = "(IF#TnEHBx";  /* types arguments (2 1es lettres) */
  static char ArgType2[] = ")nlIxdnxix";
  static char IdZero[] = "#0";
}

class StepFile_ReadData::CharactersPage {

public:

  CharactersPage(const Standard_Integer theMaxCar) :myNext(NULL), myUsed(0)
  {
    myCharacters = new char[theMaxCar];
  }

  ~CharactersPage()
  {
    if (myCharacters != NULL)
    {
      delete[] myCharacters;
      myCharacters = NULL;
    }
  }

public:

  CharactersPage* myNext; //!< Chaining of character pages
  char* myCharacters;     //!< Own characters page
  int myUsed;             //!< Counter employed characters
};

class StepFile_ReadData::Argument {

public:
  // Standard OCCT memory allocation stuff
  DEFINE_STANDARD_ALLOC

public:

  Argument() :myNext(NULL), myValue(NULL), myType(Interface_ParamSub) {}

  ~Argument() {}

public:

  Argument* myNext;    //!< Next argument in the list for this record
  char*  myValue;      //!< Character value of the argument
  Interface_ParamType myType; //!< Type of the argument
};

class StepFile_ReadData::ArgumentsPage {

public:
  // Standard OCCT memory allocation stuff
  DEFINE_STANDARD_ALLOC

public:

  ArgumentsPage(Standard_Integer theMaxArg) :myNext(NULL), myUsed(0)
  {
    myArgs = new Argument[theMaxArg];
  }

  ~ArgumentsPage()
  {
    delete[] myArgs;
    myArgs = NULL;
  }

public:

  ArgumentsPage*  myNext; //!< Chaining of arguments pages
  Argument* myArgs;       //!< Own arguments page
  int myUsed;             //!< Counter employed arguments
};

class StepFile_ReadData::Record {

public:
  // Standard OCCT memory allocation stuff
  DEFINE_STANDARD_ALLOC

public:

  Record() :myNext(NULL), myFirst(NULL), myIdent(NULL), myType(NULL) {}

  ~Record() {}

public:

  Record* myNext;    //!< Next record in the list
  Argument* myFirst; //!< First argument in the record
  char* myIdent;     //!< Record identifier (Example: "#12345") or scope-end
  char* myType;      //!< Type of the record
};

class StepFile_ReadData::Scope {

public:
  // Standard OCCT memory allocation stuff
  DEFINE_STANDARD_ALLOC

public:

  Scope() :myPrevious(NULL), myRecord(NULL) {}

  ~Scope()
  {
    if (myRecord != NULL)
    {
      delete[] myRecord;
      myRecord = NULL;
    }
  }

public:

  Scope*  myPrevious; //!< Previous scope, to which it will be necessary to return
  Record* myRecord;   //!< Record interrupted by the scope (to resume)
};

class StepFile_ReadData::RecordsPage
{

public:

  RecordsPage(const Standard_Integer theMaxRec) :myNext(NULL), myUsed(0)
  {
    myRecords = new Record[theMaxRec];
  }

  ~RecordsPage()
  {
    if (myRecords != NULL)
    {
      delete[] myRecords;
      myRecords = NULL;
    }
  }

public:

  RecordsPage* myNext; //!< Chaining of records pages
  Record* myRecords;   //!< Own records page
  int myUsed;          //!< Counter employed records
};

class StepFile_ReadData::ErrorsPage
{

public:

  ErrorsPage(Standard_CString theError) :myNext(NULL), myError(theError)
  {}

  //! Returns point to the next ErrorsPage
  ErrorsPage* NextErrorPage() const { return myNext; }

  //! Sets the next ErrorPage
  void SetNextErrorPage(ErrorsPage* theNextErrorsPage) { myNext = theNextErrorsPage; }

  //! Returns an error message
  Standard_CString ErrorMessage() const { return myError.ToCString(); }

private:

  ErrorsPage* myNext;              //!< Chaining of records pages
  TCollection_AsciiString myError; //!< Own error message
};

//=======================================================================
//function : StepFile_ReadData
//purpose  : 
//=======================================================================

StepFile_ReadData::StepFile_ReadData()
  :myMaxChar(50000), myMaxRec(5000), myMaxArg(10000), myModePrint(0),
  myNbRec(0), myNbHead(0), myNbPar(0), myYaRec(0),
  myNumSub(0), myErrorArg(Standard_False), myResText(NULL), myCurrType(TextValue::SubList),
  mySubArg(NULL), myTypeArg(Interface_ParamSub), myCurrArg(NULL), myFirstRec(NULL),
  myCurRec(NULL), myLastRec(NULL), myCurScope(NULL), myFirstError(NULL), myCurError(NULL)
{
  myOneCharPage = new CharactersPage(myMaxChar);
  myOneArgPage = new ArgumentsPage(myMaxArg);
  myOneRecPage = new RecordsPage(myMaxRec);
};

//=======================================================================
//function : CreateNewText
//purpose  : 
//=======================================================================

void StepFile_ReadData::CreateNewText(const char* theNewText, int theLenText)
{
  // Optimization for most frequent entity, CARTESIAN_POINT
  static char aTextOfCarPnt[] = "CARTESIAN_POINT";
  if (strcmp(theNewText, aTextOfCarPnt) == 0) {
    myResText = aTextOfCarPnt;
    return;
  }
  //  If error argument exists - prepare size to new text value and old result text
  int aLenght = (myErrorArg) ? theLenText + (int)strlen(myResText) : theLenText;

  if (myOneCharPage->myUsed > myMaxChar - aLenght - 1)
  {
    int aSizeOfPage = myMaxChar + 1;
    if (aLenght >= myMaxChar) aSizeOfPage += (aLenght + 1 - myMaxChar);
    CharactersPage* aNewPage = new CharactersPage(aSizeOfPage);
    aNewPage->myNext = myOneCharPage;
    myOneCharPage = aNewPage;
    myOneCharPage->myUsed = 0;
  }

  char* anOldResText = myResText;

  myResText = myOneCharPage->myCharacters + myOneCharPage->myUsed;
  myOneCharPage->myUsed += (aLenght + 1);

  // If error argument exists - append new text to old result text
  // Else create new result text
  if (myErrorArg)
  {
    strcpy(myResText, anOldResText);
    strcpy(myResText + (int)strlen(anOldResText), theNewText);
    return;
  }
  strcpy(myResText, theNewText);
}

//=======================================================================
//function : RecordNewEntity
//purpose  : 
//=======================================================================

void StepFile_ReadData::RecordNewEntity()
{
  myErrorArg = Standard_False; // Reset error argument mod
  AddNewRecord(myCurRec);
  SetTypeArg(Interface_ParamSub);
  mySubArg = myCurRec->myIdent;
  myCurRec = myCurRec->myNext;
  myLastRec->myNext = NULL;
}

//=======================================================================
//function : RecordIdent
//purpose  : 
//=======================================================================

void StepFile_ReadData::RecordIdent()
{
  myCurRec = CreateNewRecord();
  GetResultText(&myCurRec->myIdent);
  myCurRec->myNext = NULL;
  myCurRec->myFirst = NULL;
  myYaRec = 1;
}

//=======================================================================
//function : RecordType
//purpose  : 
//=======================================================================

void StepFile_ReadData::RecordType()
{
  if (!myYaRec)
  {
    myCurRec = CreateNewRecord();
    myCurRec->myIdent = TextValue::IdZero;
    myCurRec->myNext = NULL;
    myCurRec->myFirst = NULL;
  }
  GetResultText(&myCurRec->myType);
  myYaRec = myNumSub = 0;
}

//=======================================================================
//function : RecordListStart
//purpose  : 
//=======================================================================

void StepFile_ReadData::RecordListStart()
{
  if (myNumSub > 0)
  {
    Record* aSubRec;
    aSubRec = CreateNewRecord();
    switch (myNumSub)
    {
    case 1:
      aSubRec->myIdent = TextValue::Sub1; break;
    case 2:
      aSubRec->myIdent = TextValue::Sub2; break;
    default:
    {
      char aBufSub[10];
      if (myNumSub > 9) Sprintf(aBufSub, "$%d", myNumSub);
      else { aBufSub[0] = '$'; aBufSub[1] = (char)(myNumSub + 48); aBufSub[2] = '\0'; }
      aSubRec->myIdent = RecordNewText(aBufSub);
    }
    }
    aSubRec->myType = myCurrType;
    myCurrType = TextValue::SubList;
    aSubRec->myNext = myCurRec;
    aSubRec->myFirst = NULL;
    myCurRec = aSubRec;
  }
  myErrorArg = Standard_False; // Reset error arguments mode
  myNumSub++;
}


//=======================================================================
//function : CreateNewArg
//purpose  : 
//=======================================================================

void StepFile_ReadData::CreateNewArg()
{
  Argument* aNewArg;
  myNbPar++;
  if (myOneArgPage->myUsed >= myMaxArg)
  {
    ArgumentsPage* aNewArgPage;
    aNewArgPage = new ArgumentsPage(myMaxArg);
    aNewArgPage->myNext = myOneArgPage;
    myOneArgPage = aNewArgPage;
  }
  aNewArg = &myOneArgPage->myArgs[myOneArgPage->myUsed];
  myOneArgPage->myUsed++;
  aNewArg->myType = myTypeArg;
  if (myTypeArg == Interface_ParamSub)
    aNewArg->myValue = mySubArg;
  else
    GetResultText(&aNewArg->myValue);

  if (myTypeArg == Interface_ParamMisc)
    myErrorArg = Standard_True;

  if (myCurRec->myFirst == NULL)
  {
    myCurRec->myFirst = aNewArg;
  }
  else
  {
    Argument* aNextArg = myCurRec->myFirst;
    while (aNextArg->myNext != NULL)
      aNextArg = aNextArg->myNext;
    aNextArg->myNext = aNewArg;
  }
  aNewArg->myNext = NULL;
}

//=======================================================================
//function : CreateErrorArg
//purpose  : 
//=======================================================================

void StepFile_ReadData::CreateErrorArg()
{

  // If already exists - update text value
  if (!myErrorArg)
  {
    SetTypeArg(Interface_ParamMisc);
    CreateNewArg();
    myErrorArg = Standard_True;
    return;
  }

  Argument* aCurrArg = myCurRec->myFirst;
  while (aCurrArg->myNext != NULL)
    aCurrArg = aCurrArg->myNext;

  GetResultText(&aCurrArg->myValue);
}

//=======================================================================
//function : AddNewScope
//purpose  : 
//=======================================================================

void StepFile_ReadData::AddNewScope()
{
  Scope* aNewScope;
  Record* aRecord;
  aNewScope = new Scope;
  aNewScope->myRecord = myCurRec;
  aNewScope->myPrevious = myCurScope;
  myCurScope = aNewScope;
  aRecord = CreateNewRecord();
  aRecord->myIdent = TextValue::Scope;
  aRecord->myType = TextValue::Nil;
  aRecord->myFirst = NULL;
  AddNewRecord(aRecord);
}

//=======================================================================
//function : FinalOfScope
//purpose  : 
//=======================================================================

void StepFile_ReadData::FinalOfScope()
{
  Scope* anOldScope;
  Record* aRecord;
  if (myCurScope == NULL) return;

  aRecord = CreateNewRecord();
  aRecord->myIdent = TextValue::Scope;
  aRecord->myType = TextValue::Nil;
  aRecord->myFirst = NULL;

  if (mySubArg[0] == '$')
  {
    if (myModePrint > 0)
    {
      Printf("Export List : (List in Record n0 %d) -- ", myNbRec);
      PrintRecord(myLastRec);
    }
    myCurRec = aRecord;
    myTypeArg = Interface_ParamSub;
    CreateNewArg();
  }

  AddNewRecord(aRecord);

  myCurRec = myCurScope->myRecord;
  myYaRec = 1;
  anOldScope = myCurScope;
  myCurScope = anOldScope->myPrevious;
  delete anOldScope;
}

//=======================================================================
//function : ClearRecorder
//purpose  : 
//=======================================================================

void StepFile_ReadData::ClearRecorder(const Standard_Integer theMode)
{
  if (theMode & 1)
  {
    while (myOneRecPage != NULL)
    {
      RecordsPage* aNewPage = myOneRecPage->myNext;
      delete myOneRecPage;
      myOneRecPage = aNewPage;
    }
    while (myOneArgPage != NULL) {
      ArgumentsPage* aNewPage = myOneArgPage->myNext;
      delete myOneArgPage;
      myOneArgPage = aNewPage;
    }
    while (myFirstError != NULL)
    {
      ErrorsPage* aNewErrorPage = myFirstError->NextErrorPage();
      delete myFirstError;
      myFirstError = aNewErrorPage;
    }
  }
  if (theMode & 2)
  {
    while (myOneCharPage != NULL)
    {
      CharactersPage* aNewPage = myOneCharPage->myNext;
      delete myOneCharPage;
      myOneCharPage = aNewPage;
    }
  }
}

//=======================================================================
//function : GetArgDescription
//purpose  : 
//=======================================================================

Standard_Boolean StepFile_ReadData::GetArgDescription(Interface_ParamType* theType, char** theValue)
{
  if (myCurrArg == NULL)
    return Standard_False;
  *theType = myCurrArg->myType;
  *theValue = myCurrArg->myValue;
  myCurrArg = myCurrArg->myNext;
  return Standard_True;
}

//=======================================================================
//function : GetFileNbR
//purpose  : 
//=======================================================================

void StepFile_ReadData::GetFileNbR(Standard_Integer* theNbHead,
                                   Standard_Integer* theNbRec,
                                   Standard_Integer* theNbPage)
{
  myCurRec = myFirstRec;
  *theNbHead = myNbHead;
  *theNbRec = myNbRec;
  *theNbPage = myNbPar;
}

//=======================================================================
//function : GetRecordDescription
//purpose  : 
//=======================================================================

Standard_Boolean StepFile_ReadData::GetRecordDescription(char** theIdent,
                                                         char** theType,
                                                         int* theNbArg)
{
  if (myCurRec == NULL)
    return Standard_False;
  *theIdent = myCurRec->myIdent;
  *theType = myCurRec->myType;
  *theNbArg = (myCurRec->myFirst != NULL);
  myCurrArg = myCurRec->myFirst;
  return Standard_True;
}

//=======================================================================
//function : RecordTypeText
//purpose  : 
//=======================================================================

void StepFile_ReadData::RecordTypeText()
{
  GetResultText(&myCurrType);
}

//=======================================================================
//function : NextRecord
//purpose  : 
//=======================================================================

void StepFile_ReadData::NextRecord()
{
  myCurRec = myCurRec->myNext;
}

//=======================================================================
//function : PrintRecord
//purpose  : 
//=======================================================================

void StepFile_ReadData::PrintCurrentRecord()
{
  PrintRecord(myCurRec);
}

//=======================================================================
//function : PrepareNewArg
//purpose  : 
//=======================================================================

void StepFile_ReadData::PrepareNewArg()
{
  myErrorArg = Standard_False;
}

//=======================================================================
//function : FinalOfHead
//purpose  : 
//=======================================================================

void StepFile_ReadData::FinalOfHead()
{
  myNbHead = myNbRec;
}

//=======================================================================
//function : SetTypeArg
//purpose  : 
//=======================================================================

void StepFile_ReadData::SetTypeArg(const Interface_ParamType theArgType)
{
  myTypeArg = theArgType;
}

//=======================================================================
//function : SetModePrint
//purpose  : 
//=======================================================================

void StepFile_ReadData::SetModePrint(const Standard_Integer theMode)
{
  myModePrint = theMode;
}

//=======================================================================
//function : GetModePrint
//purpose  : 
//=======================================================================

Standard_Integer StepFile_ReadData::GetModePrint() const
{
  return myModePrint;
}

//=======================================================================
//function : GetNbRecord
//purpose  : 
//=======================================================================

Standard_Integer StepFile_ReadData::GetNbRecord() const
{
  return myNbRec;
}

//=======================================================================
//function : AddError
//purpose  :
//=======================================================================
void StepFile_ReadData::AddError(Standard_CString theErrorMessage)
{
  if (myFirstError == NULL)
  {
    myFirstError = new ErrorsPage(theErrorMessage);
    myCurError = myFirstError;
  }
  else
  {
    myCurError->SetNextErrorPage(new ErrorsPage(theErrorMessage));
    myCurError = myCurError->NextErrorPage();
  }
}

//=======================================================================
//function : ErrorHandle
//purpose  :
//=======================================================================
Standard_Boolean StepFile_ReadData::ErrorHandle(const Handle(Interface_Check)& theCheck) const
{
  if (myFirstError != NULL)
  {
    ErrorsPage* aCurrent = myFirstError;
    while (aCurrent != NULL)
    {
      theCheck->AddFail(aCurrent->ErrorMessage(), "Undefined Parsing");
      aCurrent = aCurrent->NextErrorPage();
    }
  }
  return myFirstError == NULL;
}

//=======================================================================
//function : GetLastError
//purpose  :
//=======================================================================
Standard_CString StepFile_ReadData::GetLastError() const
{
  return myCurError != NULL ? myCurError->ErrorMessage() : NULL;
}

//=======================================================================
//function : RecordNewText
//purpose  : 
//=======================================================================

char* StepFile_ReadData::RecordNewText(char* theText)
{
  char* aSavResText;
  char* aNewText;
  aSavResText = myResText;
  CreateNewText(theText, (int)strlen(theText));
  aNewText = myResText;
  myResText = aSavResText;
  return aNewText;
}

//=======================================================================
//function : GetResultText
//purpose  : 
//=======================================================================

void StepFile_ReadData::GetResultText(char** theText)
{
  *theText = myResText;
}

//=======================================================================
//function : AddNewRecord
//purpose  : 
//=======================================================================

void StepFile_ReadData::AddNewRecord(Record* theNewRecord)
{
  myNbRec++;
  if (myFirstRec == NULL) myFirstRec = theNewRecord;
  if (myLastRec != NULL) myLastRec->myNext = theNewRecord;
  myLastRec = theNewRecord;
}

//=======================================================================
//function : CreateNewRecord
//purpose  : 
//=======================================================================

StepFile_ReadData::Record* StepFile_ReadData::CreateNewRecord()
{
  Record* aNewRecord;
  if (myOneRecPage->myUsed >= myMaxRec)
  {
    RecordsPage* aNewRecPage;
    aNewRecPage = new RecordsPage(myMaxRec);
    aNewRecPage->myNext = myOneRecPage;
    myOneRecPage = aNewRecPage;
  }
  aNewRecord = &myOneRecPage->myRecords[myOneRecPage->myUsed];
  myOneRecPage->myUsed++;

  return aNewRecord;
}

//=======================================================================
//function : PrintRecord
//purpose  : 
//=======================================================================

void StepFile_ReadData::PrintRecord(Record* theRecord)
{
  int aNumArg = 0;
  int aNumLen = 0;
  int anArgLen = 0;
  if (theRecord == NULL) { Printf("Non defini\n"); return; }
  Printf("Ident : %s  Type : %s  Nb.Arg.s : %s\n",
    theRecord->myIdent, theRecord->myType,
    (theRecord->myFirst ? theRecord->myFirst->myValue : ""));
  if (myModePrint < 2) return;
  myCurrArg = theRecord->myFirst;
  while (myCurrArg != NULL)
  {
    aNumArg++;
    anArgLen = (int)strlen(myCurrArg->myValue) + 18;
    aNumLen += anArgLen;
    if (aNumLen > 132) { Printf("\n"); aNumLen = anArgLen; }
    Printf("  - Arg.%d[%c%c] : %s",
      aNumArg, TextValue::ArgType1[myCurrArg->myType],
      TextValue::ArgType2[myCurrArg->myType], myCurrArg->myValue);
    myCurrArg = myCurrArg->myNext;
  }
  if (anArgLen > 0) Printf("\n");
}
