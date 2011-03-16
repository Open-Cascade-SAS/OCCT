#include <IGESSelect_FloatFormat.ixx>
#include <Interface_FloatWriter.hxx>
#include <stdio.h>



    IGESSelect_FloatFormat::IGESSelect_FloatFormat ()
    : thezerosup (Standard_True) , themainform ("%E") ,
      theformrange ("%f") , therangemin (0.1) , therangemax (1000.)
      {  }

    void  IGESSelect_FloatFormat::SetDefault (const Standard_Integer digits)
{
  themainform.Clear();
  theformrange.Clear();
  if (digits <= 0) {
    themainform.AssignCat  ("%E");
    theformrange.AssignCat ("%f");
  } else {
    char format[20];
    char pourcent = '%'; char point = '.';
    sprintf(format,  "%c%d%c%dE",pourcent,digits+2,point,digits);
    themainform.AssignCat  (format);
    sprintf(format,  "%c%d%c%df",pourcent,digits+2,point,digits);
    theformrange.AssignCat (format);
  }
  therangemin = 0.1; therangemax = 1000.;
  thezerosup = Standard_True;
}

    void  IGESSelect_FloatFormat::SetZeroSuppress (const Standard_Boolean mode)
      {  thezerosup = mode;  }

    void  IGESSelect_FloatFormat::SetFormat (const Standard_CString format)
      {  themainform.Clear();  themainform.AssignCat(format);  }


    void  IGESSelect_FloatFormat::SetFormatForRange
  (const Standard_CString form, const Standard_Real R1, const Standard_Real R2)
{
  theformrange.Clear();  theformrange.AssignCat(form);
  therangemin = R1;  therangemax = R2;
}

    void  IGESSelect_FloatFormat::Format
  (Standard_Boolean& zerosup,  TCollection_AsciiString& mainform,
   Standard_Boolean& hasrange, TCollection_AsciiString& formrange,
   Standard_Real& rangemin,    Standard_Real& rangemax) const
{
  zerosup   = thezerosup;
  mainform  = themainform;
  hasrange  = (theformrange.Length() > 0);
  formrange = theformrange;
  rangemin  = therangemin;
  rangemax  = therangemax;
}


    void  IGESSelect_FloatFormat::Perform
  (IFSelect_ContextWrite& ctx,
   IGESData_IGESWriter& writer) const
{
  writer.FloatWriter().SetFormat (themainform.ToCString());
  writer.FloatWriter().SetZeroSuppress (thezerosup);
  if (theformrange.Length() > 0) writer.FloatWriter().SetFormatForRange
    (theformrange.ToCString(), therangemin, therangemax);
}

    TCollection_AsciiString  IGESSelect_FloatFormat::Label () const
{
  TCollection_AsciiString lab("Float Format ");
  if (thezerosup) lab.AssignCat(" ZeroSup ");
  lab.AssignCat (themainform);
  if (theformrange.Length() > 0) {
    char mess[30];
//    sprintf(mess,", in range %f %f %s",
//	    therangemin,therangemax,theformrange.ToCString());
//    lab.AssignCat(mess);
//    ... FloatFormat a droit aussi a un beau format pour son propre compte ...
    lab.AssignCat (", in range ");
    Standard_Integer convlen = Interface_FloatWriter::Convert
      (therangemin,mess,Standard_True,therangemin/2.,therangemax*2.,"%f","%f");
    mess[convlen] = ' ';  mess[convlen+1] = '\0';
    lab.AssignCat(mess);
    convlen = Interface_FloatWriter::Convert
      (therangemax,mess,Standard_True,therangemin/2.,therangemax*2.,"%f","%f");
    mess[convlen] = ':';  mess[convlen+1] = '\0';
    lab.AssignCat(mess);
    lab.AssignCat(theformrange.ToCString());
  }
  return lab;
}
