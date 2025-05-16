/*
 Created on: 2000-08-10
 Created by: Michael SAZONOV
 Copyright (c) 2000-2014 OPEN CASCADE SAS

 This file is part of Open CASCADE Technology software library.

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License version 2.1 as published
 by the Free Software Foundation, with special exception defined in the file
 OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
 distribution for complete text of the license and disclaimer of any warranty.

 Alternatively, this file may be used under the terms of Open CASCADE
 commercial license or contractual agreement.
*/

#include <OSD_PerfMeter.hxx>

#include <OSD_Chronometer.hxx>

#include <unordered_map>

// Simple stopwatch class to measure elapsed time
// and provides methods to start, stop, and get the elapsed time in seconds.
class Stopwatch
{
public:
  // Constructor initializes the stopwatch.
  // It does not start the stopwatch.
  Stopwatch();

  // Starts the stopwatch. If it is already running, it resets the start time.
  void Start();

  // Stops the stopwatch. If it is already stopped, it does nothing.
  void Stop();

  // Returns the elapsed time in seconds since the stopwatch was started.
  // If the stopwatch is still running, it returns the time since the last start.
  // If the stopwatch was stopped, it returns the time between the last start and stop.
  // If the stopwatch was never started, it returns 0.
  double Elapsed() const;

  // Returns true if the stopwatch is currently running.
  // Returns false if the stopwatch is stopped or was never started.
  inline bool IsRunning() const;

  // Returns true if the stopwatch has been started at least once and has a non-zero elapsed time.
  // Returns false if the stopwatch was never started or has zero elapsed time.
  inline bool IsActive() const;

private:
  // Returns the current time in seconds.
  static double getTime();

private:
  double myStartTime; //< The time when the stopwatch was started.
  double myEndTime;   //< The time when the stopwatch was stopped.
                      //  Equal to myStartTime if the stopwatch is
                      //  running or was never started.
  bool myIsTicking;   //< Indicates whether the stopwatch is
                      //  currently running.
};

//==================================================================================================

Stopwatch::Stopwatch()
    : myStartTime(getTime()),
      myEndTime(myStartTime),
      myIsTicking(false)
{
}

//==================================================================================================

void Stopwatch::Start()
{
  myStartTime = getTime();
  myIsTicking = true;
}

//==================================================================================================

void Stopwatch::Stop()
{
  if (!myIsTicking)
  {
    return;
  }

  myEndTime   = getTime();
  myIsTicking = false;
}

//==================================================================================================

double Stopwatch::Elapsed() const
{
  const double anEndTime = myIsTicking ? getTime() : myEndTime;
  return anEndTime - myStartTime;
}

//==================================================================================================

bool Stopwatch::IsRunning() const
{
  return myIsTicking;
}

//==================================================================================================

bool Stopwatch::IsActive() const
{
  return myIsTicking || (myEndTime - myStartTime) > 0.0;
}

//==================================================================================================

double Stopwatch::getTime()
{
  Standard_Real aUserSeconds   = 0.0;
  Standard_Real aSystemSeconds = 0.0;
  OSD_Chronometer::GetThreadCPU(aUserSeconds, aSystemSeconds);
  (void)(aSystemSeconds); // Unused variable
  return aUserSeconds;
}

//==================================================================================================

// Singleton class to manage multiple stopwatches.
// It provides methods to create, retrieve, and print stopwatches by name.
// It also handles the lifetime of the stopwatches and prints their results when the program ends.
// The class is designed to be used as a singleton, ensuring that there is only one instance of the
// stopwatch manager throughout the program.
class StopwatchStorage
{
private:
  StopwatchStorage()                                   = default;
  StopwatchStorage(const StopwatchStorage&)            = delete;
  StopwatchStorage& operator=(const StopwatchStorage&) = delete;
  StopwatchStorage(StopwatchStorage&&)                 = delete;
  StopwatchStorage& operator=(StopwatchStorage&&)      = delete;

  ~StopwatchStorage() { PrintAll(); }

public:
  // Returns the singleton instance of the StopwatchStorage class.
  static StopwatchStorage& Instance();

  // Retrieves a stopwatch by name. If the stopwatch does not exist, it returns nullptr.
  // If the stopwatch exists, it returns a pointer to the stopwatch.
  // This allows the user to access and manipulate the stopwatch directly.
  // @param theName The name of the stopwatch to retrieve.
  // @return A pointer to the stopwatch if it exists, nullptr otherwise.
  Stopwatch* GetStopwatch(const std::string& theName);

  // Creates a new stopwatch with the specified name.
  // If a stopwatch with the same name already exists, it will be replaced.
  // @param theName The name of the stopwatch to create.
  // @return A reference to the created stopwatch.
  Stopwatch& CreateStopwatch(const std::string& theName);

  // Checks if a stopwatch with the specified name exists.
  // This method allows the user to check if a stopwatch is already created before attempting to
  // create or retrieve it.
  // @param theName The name of the stopwatch to check.
  // @return True if the stopwatch exists, false otherwise.
  bool HasStopwatch(const std::string& theName) const;

  // Deletes a stopwatch with the specified name.
  // If the stopwatch does not exist, it does nothing.
  // This method allows the user to remove a stopwatch from the storage.
  // @param theName The name of the stopwatch to delete.
  // @return True if the stopwatch was successfully deleted, false otherwise.
  void KillStopwatch(const std::string& theName);

  // Clears all stopwatches from the storage.
  // This method removes all stopwatches and resets the storage to its initial state.
  // It is useful for cleaning up the storage when it is no longer needed.
  void Clear();

  // Prints the results of a specific stopwatch by name.
  // If the stopwatch does not exist, it does nothing.
  // If the stopwatch is still running, it prints a warning message.
  // If the stopwatch was never started, it prints a message indicating that.
  // @param theName The name of the stopwatch to print.
  // @return A string containing the results of the stopwatch.
  std::string Print(const std::string& theName) const;

  // Prints the results of all stopwatches in the storage.
  // It iterates through all stopwatches and prints their results.
  // If a stopwatch is still running, it prints a warning message.
  // If a stopwatch was never started, it prints a message indicating that.
  // @return A string containing the results of all stopwatches.
  std::string PrintAll() const;

private:
  // Helper method to print the results of a specific stopwatch.
  // It formats the output and appends it to the provided output string.
  // @param theName The name of the stopwatch to print.
  // @param theOutput The output string to append the results to.
  void print(const std::string& theName, std::string& theOutput) const;

private:
  std::unordered_map<std::string, Stopwatch> myStopwatches; //< Map to store stopwatches by name.
};

//==================================================================================================

StopwatchStorage& StopwatchStorage::Instance()
{
  static StopwatchStorage instance;
  return instance;
}

//===================================================================================================

Stopwatch* StopwatchStorage::GetStopwatch(const std::string& theName)
{
  auto it = myStopwatches.find(theName);
  return (it != myStopwatches.end()) ? &it->second : nullptr;
}

//===================================================================================================

Stopwatch& StopwatchStorage::CreateStopwatch(const std::string& theName)
{
  myStopwatches[theName] = Stopwatch();
  return myStopwatches[theName];
}

//===================================================================================================

bool StopwatchStorage::HasStopwatch(const std::string& theName) const
{
  return myStopwatches.find(theName) != myStopwatches.end();
}

//===================================================================================================

void StopwatchStorage::KillStopwatch(const std::string& theName)
{
  myStopwatches.erase(theName);
}

//===================================================================================================

void StopwatchStorage::Clear()
{
  myStopwatches.clear();
}

//===================================================================================================

std::string StopwatchStorage::Print(const std::string& theName) const
{
  std::string anOutput;
  auto        it = myStopwatches.find(theName);
  if (it != myStopwatches.end())
  {
    print(theName, anOutput);
  }
  return anOutput;
}

//===================================================================================================

std::string StopwatchStorage::PrintAll() const
{
  std::string anOutput;
  for (const auto& aStopwatch : myStopwatches)
  {
    print(aStopwatch.first, anOutput);
  }
  return anOutput;
}

//===================================================================================================

void StopwatchStorage::print(const std::string& theName, std::string& theOutput) const
{
  auto it = myStopwatches.find(theName);
  if (it == myStopwatches.end())
  {
    return;
  }

  if (!it->second.IsActive())
  {
    theOutput += "Stopwatch " + theName + " have never been started.\n";
    return;
  }

  if (it->second.IsRunning())
  {
    theOutput += "Warning: Stopwatch " + theName + " is still running.\n";
    return;
  }
  theOutput += "Stopwatch " + theName + ": " + std::to_string(it->second.Elapsed()) + " sec\n";
}

//==================================================================================================

OSD_PerfMeter::OSD_PerfMeter(const TCollection_AsciiString& theMeterName, const bool theToAutoStart)
{
  Init(theMeterName);

  if (theToAutoStart)
  {
    Start();
  }
}

//==================================================================================================

OSD_PerfMeter::~OSD_PerfMeter() {}

//==================================================================================================

void OSD_PerfMeter::Init(const TCollection_AsciiString& theMeterName)
{
  myMeterName = theMeterName;
  if (!StopwatchStorage::Instance().HasStopwatch(myMeterName.ToCString()))
  {
    StopwatchStorage::Instance().CreateStopwatch(myMeterName.ToCString());
  }
}

//==================================================================================================

void OSD_PerfMeter::Start() const
{
  Stopwatch* aStopwatch = StopwatchStorage::Instance().GetStopwatch(myMeterName.ToCString());
  if (aStopwatch != nullptr)
  {
    aStopwatch->Start();
  }
}

//==================================================================================================

void OSD_PerfMeter::Stop() const
{
  Stopwatch* aStopwatch = StopwatchStorage::Instance().GetStopwatch(myMeterName.ToCString());
  if (aStopwatch != nullptr)
  {
    aStopwatch->Stop();
  }
}

//==================================================================================================

double OSD_PerfMeter::Elapsed() const
{
  Stopwatch* aStopwatch = StopwatchStorage::Instance().GetStopwatch(myMeterName.ToCString());
  return aStopwatch ? aStopwatch->Elapsed() : 0.0;
}

//==================================================================================================

void OSD_PerfMeter::Kill() const
{
  StopwatchStorage::Instance().KillStopwatch(myMeterName.ToCString());
}

//==================================================================================================

TCollection_AsciiString OSD_PerfMeter::Print() const
{
  Stopwatch* aStopwatch = StopwatchStorage::Instance().GetStopwatch(myMeterName.ToCString());
  if (aStopwatch != nullptr)
  {
    const std::string anOutput = StopwatchStorage::Instance().Print(myMeterName.ToCString());
    return anOutput.c_str();
  }
  return "";
}

//==================================================================================================

TCollection_AsciiString OSD_PerfMeter::PrintALL()
{
  const std::string anOutput = StopwatchStorage::Instance().PrintAll();
  return anOutput.c_str();
}

//==================================================================================================

void OSD_PerfMeter::ResetALL()
{
  StopwatchStorage::Instance().Clear();
}
