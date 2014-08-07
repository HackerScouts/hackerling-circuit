#ifndef __CHAT_SYSTEM_HACKERLNG_SHIELD_H__
#define __CHAT_SYSTEM_HACKERLNG_SHIELD_H__

#include <IR_COM.h>
#include <Wire.h>
#include <MCP23017.h>
#include <Hackerling_Shield.h>
#include <HardwareSerial.h>
#include <MCP23008.h>
#include <LCD.h>

// Represents the user of the arduino. Allows to read the messages he wrote and
// show him messages on the computer screen.
// Uses the Serial to communicate with the user.
class User {
 public:
  // Creates an user object.
  // params:
  //  * printer: pointer to the arduino Serial.
  User(HardwareSerial* printer);

  // Initializes the user. Need to be call before calling any other function.
  void Init();

  // Shows a message to the user screen.
  void ShowMessage(String text);

  // Returns yes if the user wrote a message that is waiting to be sent.
  bool HasMessageToSend();

  // Get the message the user wrote.
  String GetMessageToSend();
 
 private:
  String current_reading_line_;
  bool available_;
  String previous_line_;
  HardwareSerial* printer_;
};

struct Message {
  String name;
  String text;
};

// Allows to communicate with other shields though infra red.
class IRInterface {
 public:
  // Initializes the IR interface. Has to be called before any other function
  // call.
  void Init();

  // Returns true if a message from another user has been received.
  bool ReceivedMessage();

  // Get the message that has been received from another user.
  Message GetReceivedMessage();

  // Send a message to other users by infra red.
  void Send(const Message& message);

 private:
  String current_message_;
  bool available_;
};

#endif  // __CHAT_SYSTEM_HACKERLNG_SHIELD_H__
