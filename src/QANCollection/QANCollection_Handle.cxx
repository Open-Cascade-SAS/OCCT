// Copyright (c) 2014 OPEN CASCADE SAS
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

#include <QANCollection.hxx>

#include <Draw.hxx>
#include <Draw_Interpretor.hxx>

#include <Message_Status.hxx>
#include <NCollection_StdAllocator.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_HeapAllocator.hxx>
#include <OSD_Timer.hxx>
#include <Standard_Assert.hxx>
#include <Standard_DefineHandle.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Surface.hxx>

#include <vector>
#include <memory>
#include <typeinfo>

// auxiliary macro to check and report status
#define CHECK(di,ok,what) di << "Checking " << what << (ok ? ": OK\n" : ": Error\n")

//=======================================================================
//function : QAHandleOps
//purpose  : Test Handle operations (mostly compile-time checks)
//=======================================================================

// set of overloaded functions for checking resolution of arguments
inline void f (const Handle(Geom_Curve)&) {}
inline void func (const Handle(Geom_Curve)&) {}
inline void func (const Handle(Geom_BSplineCurve)&) {}
inline void func (const Handle(Geom_Surface)&) {}
inline void func (const Handle(gp_Pnt)&) {}
inline void func (const Handle(gp_XYZ)&) {}
inline void func (const Handle(gp_Trsf)&) {}

static Standard_Integer QAHandleOps (Draw_Interpretor& theDI,
                                     Standard_Integer  /*theArgNb*/,
                                     const char**      /*theArgVec*/)
{
  // ===============================================================
  // Part 1: classes inheriting transient
  // ===============================================================

  Handle(Geom_Line) aLine = new Geom_Line (gp::Origin(), gp::DZ());
  CHECK(theDI, ! aLine.IsNull(), "handle for non-null");

  const Handle(Geom_Line)& cLine = aLine; // cast to self const ref
  const Handle(Geom_Curve)& cCurve = aLine; // cast to base const ref
  Geom_Line* pLine = aLine.get();
  const Geom_Line* cpLine = aLine.get();
  Geom_Line& rLine = *aLine;
  const Geom_Line& crLine = *cLine;
  Handle(Geom_Curve) aCurve = aLine; // copy from handle to derived type
  aCurve = cLine; // assignment to handle of derived type
  Handle(Geom_Line) dLine (cpLine); // copy from handle to derived type

  aLine = Handle(Geom_Line)::DownCast (cCurve);
  CHECK(theDI, ! aLine.IsNull(), "down cast");

  // comparison operators
  CHECK(theDI, aLine == aLine, "equality of handle to itself");
  CHECK(theDI, cLine == cLine, "equality of const handle to itself");
  CHECK(theDI, aLine == cLine, "equality of const and non-const handle");
  CHECK(theDI, aLine == cCurve, "equality of handle and base handle");
  CHECK(theDI, aLine == pLine,  "equality of handle and pointer");
  CHECK(theDI, pLine == aLine,  "equality of pointer and handle");
  CHECK(theDI, aLine == cpLine,  "equality of handle and const pointer");
  CHECK(theDI, cpLine == aLine,  "equality of const pointer and handle");
  CHECK(theDI, &rLine == aLine,  "equality of reference and handle");
  CHECK(theDI, &crLine == aLine,  "equality of reference and handle");
  CHECK(theDI, aLine, "cast to bool");

  Handle(Geom_Line) aLin2;
  CHECK(theDI, aLine != aLin2, "inequality of handle to the same type handle");
  CHECK(theDI, aLin2 != cLine, "inequality of const and non-const handle");
  CHECK(theDI, aLin2 != cCurve, "inequality of handle and base handle");
  CHECK(theDI, aLin2 != pLine,  "inequality of handle and pointer");
  CHECK(theDI, pLine != aLin2,  "inequality of pointer and handle");
  CHECK(theDI, aLin2 != cpLine,  "inequality of handle and const pointer");
  CHECK(theDI, cpLine != aLin2,  "inequality of const pointer and handle");

  Handle(Geom_Curve) aCur2;
  CHECK(theDI, aLine != aCur2, "inequality of handles of different types");
  CHECK(theDI, aCur2 != cLine, "inequality of const and non-const handle");
  CHECK(theDI, aCur2 != cCurve, "inequality of handle and base handle");
  CHECK(theDI, aCur2 != pLine,  "inequality of handle and pointer");
  CHECK(theDI, pLine != aCur2,  "inequality of pointer and handle");
  CHECK(theDI, aCur2 != cpLine,  "inequality of handle and const pointer");
  CHECK(theDI, cpLine != aCur2,  "inequality of const pointer and handle");

  // passing handle as reference to base class
  f (aLine);

  // passing handle to overloaded function accepting handle to another type
  // will fail on VC below 12 and GCC below 4.3 due to ambiguity of overloads
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1800) || (defined(__GNUC__) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 3)
  func (aLine);
  func (cLine);
#endif

  const Handle(Geom_Curve)& aCurve2 = aLine; // cast to base const ref

  Handle(Geom_Line) qLine = cpLine; // constructor from const pointer -- could be made explicit...

  // check whether compiler will destroy reference to temporary handle
  const Handle(Geom_Curve)& aTmpRef (Handle(Geom_Line)::DownCast (aCurve2));
  CHECK(theDI, ! aTmpRef.IsNull(),  "local reference of handle to base type to temporary handle object");

  // check operations with Handle_* classes
  Handle(Geom_Line) hLine = aLine;
  CHECK(theDI, ! hLine.IsNull(), "hhandle for non-null");

  const Handle_Geom_Line& chLine = aLine; // cast to self const ref
  const Handle_Geom_Curve& chCurve = aLine; // cast to base const ref
  const Handle_Geom_Line& hhLine = hLine; // cast to self const ref
  const Handle_Geom_Curve& hhCurve = hLine; // cast to base const ref
  Handle_Geom_Curve hCurve = aLine; // copy from handle to derived type
  Handle_Geom_Line phLine (aLine.get()); // construct from pointer

  hLine = Handle_Geom_Line::DownCast (cCurve); // inheritance of downcast
  CHECK(theDI, ! hLine.IsNull(), "down cast");

  // comparison operators
  CHECK(theDI, hLine == hLine, "equality of hhandle to itself");
  CHECK(theDI, hLine == aLine, "equality of hhandle to handle");
  CHECK(theDI, hhLine == hLine, "equality of hhandle to const");
  CHECK(theDI, chLine == hLine, "equality of hhandle to const");
  CHECK(theDI, hhCurve == hLine, "equality of hhandle to const");
  CHECK(theDI, chCurve == hLine, "equality of hhandle to const");
  CHECK(theDI, hLine, "cast to bool");

  // passing hhandle as reference to base class
  f (hLine);

  // passing handle to overloaded function accepting handle to another type
  // will fail on VC below 12 and GCC below 4.3 due to ambiguity of overloads
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1800) || (defined(__GNUC__) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 3)
  func (hLine);
  func (chLine);
#endif

  Handle_Geom_Line qhLine = cpLine; // constructor from const pointer -- could be made explicit...

  // check whether compiler will destroy reference to temporary handle
  const Handle_Geom_Curve& hTmpRef (Handle(Geom_Line)::DownCast (aCurve2));
  CHECK(theDI, ! hTmpRef.IsNull(),  "local reference of handle to base type to temporary handle object");

  Handle(Geom_Surface) aSurf;
  (void)aSurf;

#if 0
  // each test in this section must cause compiler error
  gunc (cLine); // passing const handle as non-const reference to base type
  pLine = cLine.get(); // getting non-const pointer to contained object from const handle 
  Handle(Geom_Line) xLine = cCurve; // copy from handle to base type
  Handle(Geom_BSplineCurve) aBSpl (new Geom_Line (gp::Origin(), gp::DX())); // construction from pointer to incompatible type

  CHECK(theDI, aLine == aSurf,  "equality of handles of incompatible types");
  CHECK(theDI, aSurf == cLine,  "equality of const and non-const handle");
  CHECK(theDI, aSurf == cCurve, "equality of handle and base handle");
  CHECK(theDI, aSurf == pLine,  "equality of handle and pointer");
  CHECK(theDI, pLine == aSurf,  "equality of pointer and handle");
  CHECK(theDI, aSurf == cpLine, "equality of handle and const pointer");
  CHECK(theDI, cpLine != aSurf, "equality of const pointer and handle");

  CHECK(theDI, aLine != aSurf,  "inequality of handles of incompatible types");
  CHECK(theDI, aSurf != cLine,  "inequality of const and non-const handle");
  CHECK(theDI, aSurf != cCurve, "inequality of handle and base handle");
  CHECK(theDI, aSurf != pLine,  "inequality of handle and pointer");
  CHECK(theDI, pLine != aSurf,  "inequality of pointer and handle");
  CHECK(theDI, aSurf != cpLine, "inequality of handle and const pointer");
  CHECK(theDI, cpLine != aSurf, "inequality of const pointer and handle");
#endif

  // ===============================================================
  // Part 2: classes not inheriting transient
  // ===============================================================
/*
  Handle(gp_Pnt) aPnt = new gp_Pnt (gp::Origin());
  CHECK(theDI, ! aPnt.IsNull(), "handle for non-null");

  const Handle(gp_Pnt)& cPnt = aPnt; // cast to self const ref
//  const Handle(gp_XYZ)& cXYZ = aPnt; // cast to base const ref
  gp_Pnt* pPnt = aPnt.get();
  const gp_Pnt* cpPnt = aPnt.get();
  gp_Pnt& rPnt = *aPnt;
  const gp_Pnt& crPnt = *cPnt;
//  Handle(gp_XYZ) aXYZ = aPnt; // copy from handle to derived type
//  aXYZ = cPnt; // assignment to handle of derived type

//  aPnt = Handle(gp_Pnt)::DownCast (cXYZ);
//  CHECK(theDI, ! aPnt.IsNull(), "down cast");

  // comparison operators
  CHECK(theDI, aPnt == aPnt, "equality of handle to itself");
  CHECK(theDI, cPnt == cPnt, "equality of const handle to itself");
  CHECK(theDI, aPnt == cPnt, "equality of const and non-const handle");
//  CHECK(theDI, aPnt == cXYZ, "equality of handle and base handle");
  CHECK(theDI, aPnt == pPnt,  "equality of handle and pointer");
  CHECK(theDI, pPnt == aPnt,  "equality of pointer and handle");
  CHECK(theDI, aPnt == cpPnt,  "equality of handle and const pointer");
  CHECK(theDI, cpPnt == aPnt,  "equality of const pointer and handle");
  CHECK(theDI, &rPnt == aPnt,  "equality of reference and handle");
  CHECK(theDI, &crPnt == aPnt,  "equality of reference and handle");

  Handle(gp_Pnt) aPnt2;
  CHECK(theDI, aPnt != aPnt2, "inequality of handle to the same type handle");
  CHECK(theDI, aPnt2 != cPnt, "inequality of const and non-const handle");
//  CHECK(theDI, aPnt2 != cXYZ, "inequality of handle and base handle");
  CHECK(theDI, aPnt2 != pPnt,  "inequality of handle and pointer");
  CHECK(theDI, pPnt != aPnt2,  "inequality of pointer and handle");
  CHECK(theDI, aPnt2 != cpPnt,  "inequality of handle and const pointer");
  CHECK(theDI, cpPnt != aPnt2,  "inequality of const pointer and handle");

  Handle(gp_XYZ) aXYZ2;
  CHECK(theDI, aLine != aPnt2, "inequality of handles of different types");
  CHECK(theDI, aXYZ2 != cPnt, "inequality of const and non-const handle");
//  CHECK(theDI, aXYZ2 != cXYZ, "inequality of handle and base handle");
//  CHECK(theDI, aXYZ2 != pPnt,  "inequality of handle and pointer");
//  CHECK(theDI, pPnt != aXYZ2,  "inequality of pointer and handle");
//  CHECK(theDI, aXYZ2 != cpPnt,  "inequality of handle and const pointer");
//  CHECK(theDI, cpPnt != aXYZ2,  "inequality of const pointer and handle");

  // passing handle as reference to base class
  func (aPnt);
  func (cPnt);
*/
  return 0;
}

//=======================================================================
//function : QAHandleBool
//purpose  : Test Handle -> bool conversion
//=======================================================================
static Standard_Integer QAHandleBool (Draw_Interpretor& theDI,
                                      Standard_Integer  /*theArgNb*/,
                                      const char**      /*theArgVec*/)
{
  Handle(NCollection_BaseAllocator) aPtr = new NCollection_IncAllocator();

  Handle(NCollection_IncAllocator) anInc = Handle(NCollection_IncAllocator)::DownCast (aPtr);
  CHECK (theDI, ! anInc.IsNull(), "cast to NCollection_IncAllocator");

  Handle(NCollection_BaseAllocator) anAlloc = aPtr;
  CHECK (theDI, ! anAlloc.IsNull(), "cast to NCollection_BaseAllocator");
  
  Handle(NCollection_HeapAllocator) aHAlloc = Handle(NCollection_HeapAllocator)::DownCast (aPtr);
  CHECK (theDI, aHAlloc.IsNull(), "cast to NCollection_HeapAllocator");

  return 0;
}

// Auxiliary class to define new virtual methods
class Transient_Root : public Standard_Transient
{
public:
  virtual const char* Name() const { return "Transient_Root"; }
  virtual Standard_Transient* CreateParent() const { return new Standard_Transient; }
  virtual Standard_Transient* Clone()        const { return new Transient_Root; }
  DEFINE_STANDARD_RTTI_INLINE(Transient_Root,Standard_Transient)
};
DEFINE_STANDARD_HANDLE(Transient_Root, Standard_Transient)

// Auxiliary macros to create hierarchy of 50 classes
#define QA_DEFINECLASS(theClass, theParent) \
class theClass : public theParent \
{ \
public:\
  virtual const char* Name() const Standard_OVERRIDE { return #theClass; } \
  virtual Standard_Transient* CreateParent() const Standard_OVERRIDE { return new theParent(); } \
  virtual Standard_Transient* Clone()        const Standard_OVERRIDE { return new theClass(); } \
  DEFINE_STANDARD_RTTI_INLINE(theClass,theParent) \
};\
DEFINE_STANDARD_HANDLE    (theClass, theParent) 

#define QA_NAME(theNum) qaclass ## theNum ## _ ## 50
#define QA_HANDLE_NAME(theNum) Handle(qaclass ## theNum ## _ ## 50)

#define QA_DEFINECLASS10(theParent, theTens) \
QA_DEFINECLASS(QA_NAME(theTens ## 0), theParent) \
QA_DEFINECLASS(QA_NAME(theTens ## 1), QA_NAME(theTens ## 0)) \
QA_DEFINECLASS(QA_NAME(theTens ## 2), QA_NAME(theTens ## 1)) \
QA_DEFINECLASS(QA_NAME(theTens ## 3), QA_NAME(theTens ## 2)) \
QA_DEFINECLASS(QA_NAME(theTens ## 4), QA_NAME(theTens ## 3)) \
QA_DEFINECLASS(QA_NAME(theTens ## 5), QA_NAME(theTens ## 4)) \
QA_DEFINECLASS(QA_NAME(theTens ## 6), QA_NAME(theTens ## 5)) \
QA_DEFINECLASS(QA_NAME(theTens ## 7), QA_NAME(theTens ## 6)) \
QA_DEFINECLASS(QA_NAME(theTens ## 8), QA_NAME(theTens ## 7)) \
QA_DEFINECLASS(QA_NAME(theTens ## 9), QA_NAME(theTens ## 8))

QA_DEFINECLASS10(Transient_Root,     0)
QA_DEFINECLASS10(qaclass09_50,       1)
QA_DEFINECLASS10(qaclass19_50,       2)
QA_DEFINECLASS10(qaclass29_50,       3)
QA_DEFINECLASS10(qaclass39_50,       4)
QA_DEFINECLASS  (qaclass50_50, qaclass49_50)

namespace
{
  class qaclass50_50ANON : public qaclass49_50
  {
  public:
    qaclass50_50ANON() {}
  };
}

namespace QaNamespace
{
  class qaclass50_50 : public qaclass49_50
  {
  public:
    qaclass50_50() {}
  };
}

namespace {
//! Timer sentry. Prints elapsed time information at destruction time.
class QATimer : public OSD_Timer
{
public:
  enum TimeFormat
  {
    Seconds,
    Milliseconds,
    Microseconds,
    Nanoseconds,
    s  = Seconds,
    ms = Milliseconds,
    ns = Nanoseconds,
  };

public:
  //! Main constructor - automatically starts the timer.
  QATimer (Draw_Interpretor& theDI,
           Standard_CString       theTitle,
           const TimeFormat       theFormat,
           const Standard_Integer theNbIters = 1,
           const Standard_Boolean theToPrintFormat = Standard_False)
  : myDI(&theDI),
    myTitle         (theTitle),
    myFormat        (theFormat),
    myNbIters       (theNbIters),
    myToPrintFormat (theToPrintFormat)
  {
    Start();
  }

  //! Destructor - stops the timer and prints statistics.
  ~QATimer()
  {
    Stop();
    if (myTitle != NULL)
    {
      (*myDI) << myTitle;
    }
    switch (myFormat)
    {
      case Seconds:
        (*myDI) <<  ElapsedTime() / Standard_Real(myNbIters);
        if (myToPrintFormat)
        {
          (*myDI) << " s";
        }
        break;
      case Milliseconds:
        (*myDI) << (ElapsedTime() / Standard_Real(myNbIters)) * 1000.0;
        if (myToPrintFormat)
        {
          (*myDI) << " ms";
        }
        break;
      case Microseconds:
        (*myDI) << (ElapsedTime() / Standard_Real(myNbIters)) * 1000000.0;
        if (myToPrintFormat)
        {
          (*myDI) << " microseconds";
        }
        break;
      case Nanoseconds:
        (*myDI) << (ElapsedTime() / Standard_Real(myNbIters)) * 1000000000.0;
        if (myToPrintFormat)
        {
          (*myDI) << " ns";
        }
        break;
    }
  }

private:
  Draw_Interpretor* myDI;
  Standard_CString myTitle;         //!< timer description
  TimeFormat       myFormat;        //!< time format
  Standard_Integer myNbIters;       //!< iterations number
  Standard_Boolean myToPrintFormat; //!< add time format
};
} // anonymous namespace

//=======================================================================
//function : QAHandleInc
//purpose  : Estimate the smart-pointer counter incrementing time
//=======================================================================
static Standard_Integer QAHandleInc (Draw_Interpretor& theDI,
                                     Standard_Integer  theArgNb,
                                     const char**      theArgVec)
{
  if (theArgNb > 2)
  {
    std::cout << "Error: wrong syntax! See usage:\n";
    theDI.PrintHelp (theArgVec[0]);
    return 1;
  }
  const Standard_Integer aNbIters = (theArgNb > 1) ? Draw::Atoi (theArgVec[1]) : 10000000;
  if (aNbIters < 1)
  {
    std::cout << "Error: number of iterations should be positive!\n";
    return 1;
  }

  Handle(Standard_Transient) aHandle  = new Standard_Transient();
  std::shared_ptr<Standard_Transient> aSharePtr (new Standard_Transient());
  theDI << "Time of creating and destroying " << aNbIters << " smart pointers to the same object, per item, ns:";
  {
    {
      QATimer aTimer (theDI, "\nOCCT Handle:    ", QATimer::ns, aNbIters);
      {
        std::vector<Handle(Standard_Transient)> aHandles (aNbIters);
        for (Standard_Integer anIter = 0; anIter < aNbIters; ++anIter)
        {
          aHandles[anIter] = aHandle;
        }
      }
    }
    {
      QATimer aTimer (theDI, "\nC++ shared_ptr: ", QATimer::ns, aNbIters);
      {
        std::vector< std::shared_ptr<Standard_Transient> > aSharePointers (aNbIters);
        for (Standard_Integer anIter = 0; anIter < aNbIters; ++anIter)
        {
          aSharePointers[anIter] = aSharePtr;
        }
      }
    }
  }
  return 0;
}

//=======================================================================
//function : QAHandleKind
//purpose  :
//=======================================================================
static Standard_Integer QAHandleKind (Draw_Interpretor& /*theDI*/,
                                      Standard_Integer  /*theArgNb*/,
                                      const char**      /*theArgVec*/)
{
  Handle(Standard_Type) aType00 = STANDARD_TYPE(qaclass00_50);
  Handle(Standard_Type) aType10 = STANDARD_TYPE(qaclass10_50);
  Handle(Standard_Type) aType20 = STANDARD_TYPE(qaclass20_50);
  Handle(Standard_Type) aType30 = STANDARD_TYPE(qaclass30_50);
  Handle(Standard_Type) aType40 = STANDARD_TYPE(qaclass40_50);
  Handle(Standard_Type) aType50 = STANDARD_TYPE(qaclass50_50);

  Handle(qaclass00_50) aHandle = new qaclass40_50();

  #define QA_CHECK(theDesc, theExpr, theValue) \
    {\
      const bool isTrue = !!(theExpr); \
      std::cout << theDesc << (isTrue ? " TRUE  " : " FALSE ") << (isTrue == theValue ? " is OK\n" : " is Error\n"); \
    }

  std::cout << "Check instance of " << aHandle->DynamicType()->Name() << "\n";
  for (Handle(Standard_Type) aType = aHandle->DynamicType(); ! aType.IsNull(); aType = aType->Parent())
  {
    std::cout << " - " << aType->Name() << "\n";
  }

  QA_CHECK ("Name == qaclass40_50       : ", TCollection_AsciiString("qaclass40_50") == aHandle->DynamicType()->Name(), true);

  QA_CHECK ("IsKind     (aType00)       : ", aHandle->IsKind (aType00), true);
  QA_CHECK ("IsKind     (aType10)       : ", aHandle->IsKind (aType10), true);
  QA_CHECK ("IsKind     (aType20)       : ", aHandle->IsKind (aType20), true);
  QA_CHECK ("IsKind     (aType30)       : ", aHandle->IsKind (aType30), true);
  QA_CHECK ("IsKind     (aType40)       : ", aHandle->IsKind (aType40), true);
  QA_CHECK ("IsKind     (aType50)       : ", aHandle->IsKind (aType50), false);

  QA_CHECK ("IsKind     (\"qaclass00_50\"): ", aHandle->IsKind ("qaclass00_50"), true);
  QA_CHECK ("IsKind     (\"qaclass10_50\"): ", aHandle->IsKind ("qaclass10_50"), true);
  QA_CHECK ("IsKind     (\"qaclass20_50\"): ", aHandle->IsKind ("qaclass20_50"), true);
  QA_CHECK ("IsKind     (\"qaclass30_50\"): ", aHandle->IsKind ("qaclass30_50"), true);
  QA_CHECK ("IsKind     (\"qaclass40_50\"): ", aHandle->IsKind ("qaclass40_50"), true);
  QA_CHECK ("IsKind     (\"qaclass50_50\"): ", aHandle->IsKind ("qaclass50_50"), false);

  QA_CHECK ("IsInstance (aType00)       : ", aHandle->IsInstance (aType00), false);
  QA_CHECK ("IsInstance (aType10)       : ", aHandle->IsInstance (aType10), false);
  QA_CHECK ("IsInstance (aType20)       : ", aHandle->IsInstance (aType20), false);
  QA_CHECK ("IsInstance (aType30)       : ", aHandle->IsInstance (aType30), false);
  QA_CHECK ("IsInstance (aType40)       : ", aHandle->IsInstance (aType40), true);
  QA_CHECK ("IsInstance (aType50)       : ", aHandle->IsInstance (aType50), false);

#ifdef HAVE_CPP11
  std::cout << "\nC++11:\n";
  std::type_index aCppType     = typeid(*aHandle.operator->());
  std::cout << "typeid().name()    = '" << typeid(*aHandle.operator->()).name()     << "'\n";
#ifdef _MSC_VER
  std::cout << "typeid().raw_name()= '" << typeid(*aHandle.operator->()).raw_name() << "'\n";
#endif

  std::cout << "[ANON]typeid().name()    = '" << typeid(qaclass50_50ANON).name()     << "'\n";
#ifdef _MSC_VER
  std::cout << "[ANON]typeid().raw_name()= '" << typeid(qaclass50_50ANON).raw_name() << "'\n";
#endif

  std::cout << "[NS]typeid().name()    = '" << typeid(QaNamespace::qaclass50_50).name()     << "'\n";
#ifdef _MSC_VER
  std::cout << "[NS]typeid().raw_name()= '" << typeid(QaNamespace::qaclass50_50).raw_name() << "'\n";
#endif

  QA_CHECK ("is typeid  (aType00)       : ", typeid(*aHandle.operator->()) == typeid(qaclass00_50), false);
  QA_CHECK ("is typeid  (aType10)       : ", typeid(*aHandle.operator->()) == typeid(qaclass10_50), false);
  QA_CHECK ("is typeid  (aType20)       : ", typeid(*aHandle.operator->()) == typeid(qaclass20_50), false);
  QA_CHECK ("is typeid  (aType30)       : ", typeid(*aHandle.operator->()) == typeid(qaclass30_50), false);
  QA_CHECK ("is typeid  (aType40)       : ", typeid(*aHandle.operator->()) == typeid(qaclass40_50), true);
  QA_CHECK ("is typeid  (aType50)       : ", typeid(*aHandle.operator->()) == typeid(qaclass50_50), false);

  QA_CHECK ("is type_index (aType00)    : ", aCppType == typeid(qaclass00_50), false);
  QA_CHECK ("is type_index (aType40)    : ", aCppType == typeid(qaclass40_50), true);

  QA_CHECK ("IsClass(Standard_Transient): ", std::is_class<Standard_Transient>::value == !!STANDARD_TYPE(Standard_Transient)->IsClass(), true);
  //QA_CHECK ("IsEnum (Message_Status)    : ", std::is_enum<Message_Status>::value == !!STANDARD_TYPE(Message_Status)->IsEnumeration(), true);
#endif

  return 0;
}

void QANCollection::CommandsHandle (Draw_Interpretor& theCommands)
{
  const char* THE_GROUP = "QANCollection";
  theCommands.Add ("QAHandleBool",
                   "Test handle boolean operator",
                   __FILE__, QAHandleBool, THE_GROUP);
  theCommands.Add ("QAHandleInc",
                   "QAHandleInc nbIter=1000000"
                   "\n\t\t: Test handle increment performance",
                   __FILE__, QAHandleInc,  THE_GROUP);
  theCommands.Add ("QAHandleKind",
                   "Test handle IsKind",
                   __FILE__, QAHandleKind, THE_GROUP);
  theCommands.Add ("QAHandleOps",
                   "Test handle operations",
                   __FILE__, QAHandleOps, THE_GROUP);
  return;
}
