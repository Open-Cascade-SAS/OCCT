// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef _Message_AttributeMeter_HeaderFile
#define _Message_AttributeMeter_HeaderFile

#include <Message_Attribute.hxx>
#include <Message_MetricType.hxx>

#include <NCollection_IndexedDataMap.hxx>

class Message_AlertExtended;

//! Alert object storing alert metrics values.
//! Start and stop values for each metric.
class Message_AttributeMeter : public Message_Attribute
{
public:
  //! Returns default value of the metric when it is not defined
  //! @return undefined value
  static Standard_Real UndefinedMetricValue() { return -1.0; }

public:
  //! Constructor with string argument
  Standard_EXPORT Message_AttributeMeter(
    const TCollection_AsciiString& theName = TCollection_AsciiString());

  //! Checks whether the attribute has values for the metric
  //! @param[in] theMetric  metric type
  //! @return true if the metric values exist in the attribute
  Standard_EXPORT Standard_Boolean HasMetric(const Message_MetricType& theMetric) const;

  //! Returns true when both values of the metric are set.
  //! @param[in] theMetric  metric type
  //! @return true if metric values are valid
  Standard_EXPORT Standard_Boolean IsMetricValid(const Message_MetricType& theMetric) const;

  //! Returns start value for the metric
  //! @param[in] theMetric  metric type
  //! @return real value
  Standard_EXPORT Standard_Real StartValue(const Message_MetricType& theMetric) const;

  //! Sets start values for the metric
  //! @param[in] theMetric  metric type
  Standard_EXPORT void SetStartValue(const Message_MetricType& theMetric,
                                     const Standard_Real       theValue);

  //! Returns stop value for the metric
  //! @param[in] theMetric  metric type
  //! @return real value
  Standard_EXPORT Standard_Real StopValue(const Message_MetricType& theMetric) const;

  //! Sets stop values for the metric
  //! @param[in] theMetric  metric type
  Standard_EXPORT void SetStopValue(const Message_MetricType& theMetric,
                                    const Standard_Real       theValue);

public:
  //! Sets start values of default report metrics into the alert
  //! @param theAlert an alert
  static void StartAlert(const Handle(Message_AlertExtended)& theAlert)
  {
    SetAlertMetrics(theAlert, Standard_True);
  }

  //! Sets stop values of default report metrics into the alert
  //! @param theAlert an alert
  static void StopAlert(const Handle(Message_AlertExtended)& theAlert)
  {
    SetAlertMetrics(theAlert, Standard_False);
  }

  //! Sets current values of default report metrics into the alert.
  //! Processed only alert with Message_AttributeMeter attribute
  //! @param theAlert an alert
  //! @param theStartValue flag, if true, the start value is collected otherwise stop
  static Standard_EXPORT void SetAlertMetrics(const Handle(Message_AlertExtended)& theAlert,
                                              const Standard_Boolean               theStartValue);

  //! Dumps the content of me into the stream
  virtual Standard_EXPORT void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Message_AttributeMeter, Message_Attribute)

private:
  typedef std::pair<Standard_Real, Standard_Real> StartToStopValue;
  // clang-format off
  NCollection_IndexedDataMap<Message_MetricType, StartToStopValue> myMetrics; //!< computed metrics
  // clang-format on
};

#endif // _Message_AttributeMeter_HeaderFile
