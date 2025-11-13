// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <Message_Messenger.hxx>
#include <Message_Printer.hxx>
#include <Message.hxx>
#include <TCollection_AsciiString.hxx>

#include <gtest/gtest.h>

#include <sstream>

namespace
{
//! Custom message printer that collects messages into a string stream
class TestMessagePrinter : public Message_Printer
{
public:
  TestMessagePrinter(std::ostringstream& theStream)
      : myStream(theStream)
  {
  }

  virtual void send(const TCollection_AsciiString& theString,
                    const Message_Gravity          theGravity) const override
  {
    if (theGravity >= Message_Info)
    {
      myStream << theString << "\n";
    }
  }

private:
  std::ostringstream& myStream;
};
} // namespace

TEST(Message_Messenger_Test, OCC31189_StreamBufferMessageOrdering)
{
  // Bug OCC31189: Test consistency of messages output using stream buffer interface
  // Verify that messages sent via stream buffers and directly to messenger don't intermix

  std::ostringstream               anOutput;
  Handle(TestMessagePrinter)       aPrinter = new TestMessagePrinter(anOutput);
  const Handle(Message_Messenger)& aMsgMgr  = ::Message::DefaultMessenger();

  // Save original printers
  Message_SequenceOfPrinters anOriginalPrinters;
  anOriginalPrinters.Append(aMsgMgr->ChangePrinters());

  // Replace with our test printer
  aMsgMgr->ChangePrinters().Clear();
  aMsgMgr->AddPrinter(aPrinter);

  // Scope block to test output of message on destruction of a stream buffer
  {
    Message_Messenger::StreamBuffer aSender = ::Message::SendInfo();

    // Check that messages output to sender and directly to messenger do not intermix
    aSender << "Sender message 1: start ...";
    aMsgMgr->Send("Direct message 1");
    aSender << "... end" << std::endl; // endl should send the message

    // Check that empty stream buffer does not produce output on destruction
    ::Message::SendInfo();

    // Additional message to check that they go in expected order
    aMsgMgr->Send("Direct message 2");

    // Check that empty stream buffer does produce empty line if std::endl is passed
    ::Message::SendInfo() << std::endl;

    // Last message should be sent on destruction of a sender
    aSender << "Sender message 2";
  }

  // Restore original printers
  aMsgMgr->ChangePrinters().Clear();
  aMsgMgr->ChangePrinters().Append(anOriginalPrinters);

  // Verify the output matches expected format
  std::string anOutputStr = anOutput.str();

  // Expected messages in order:
  // 1. Direct message 1
  // 2. Sender message 1: start ...... end
  // 3. Direct message 2
  // 4. Empty line
  // 5. Sender message 2
  EXPECT_NE(anOutputStr.find("Direct message 1"), std::string::npos)
    << "Direct message 1 should be present";
  EXPECT_NE(anOutputStr.find("Sender message 1: start ...... end"), std::string::npos)
    << "Sender message 1 should be complete and present";
  EXPECT_NE(anOutputStr.find("Direct message 2"), std::string::npos)
    << "Direct message 2 should be present";
  EXPECT_NE(anOutputStr.find("Sender message 2"), std::string::npos)
    << "Sender message 2 should be present";

  // Verify order: Direct 1 should come before Sender 1
  size_t aDirect1Pos = anOutputStr.find("Direct message 1");
  size_t aSender1Pos = anOutputStr.find("Sender message 1");
  size_t aDirect2Pos = anOutputStr.find("Direct message 2");
  size_t aSender2Pos = anOutputStr.find("Sender message 2");

  EXPECT_LT(aDirect1Pos, aSender1Pos) << "Direct message 1 should appear before Sender message 1";
  EXPECT_LT(aSender1Pos, aDirect2Pos) << "Sender message 1 should appear before Direct message 2";
  EXPECT_LT(aDirect2Pos, aSender2Pos) << "Direct message 2 should appear before Sender message 2";
}

TEST(Message_Messenger_Test, StreamBufferBasicUsage)
{
  // Test basic stream buffer functionality
  std::ostringstream               anOutput;
  Handle(TestMessagePrinter)       aPrinter = new TestMessagePrinter(anOutput);
  const Handle(Message_Messenger)& aMsgMgr  = Message::DefaultMessenger();

  Message_SequenceOfPrinters anOriginalPrinters;
  anOriginalPrinters.Append(aMsgMgr->ChangePrinters());

  aMsgMgr->ChangePrinters().Clear();
  aMsgMgr->AddPrinter(aPrinter);
  {
    Message_Messenger::StreamBuffer aBuffer = Message::SendInfo();
    aBuffer << "Test message" << std::endl;
  }

  aMsgMgr->ChangePrinters().Clear();
  aMsgMgr->ChangePrinters().Append(anOriginalPrinters);

  std::string anOutputStr = anOutput.str();
  EXPECT_NE(anOutputStr.find("Test message"), std::string::npos)
    << "Stream buffer should output message";
}
