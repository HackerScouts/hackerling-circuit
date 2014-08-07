#include <IR_COM.h>
#include <Wire.h>
#include <MCP23017.h>
#include <Hackerling_Shield.h>
#include <HardwareSerial.h>
#include <MCP23008.h>
#include <LCD.h>

#include <chat_system.h>

User user(&Serial);
IRInterface ir_interface;
String my_name;

void setup() {
  // Initialises the user and IR interface.
  ir_interface.Init();
  user.Init();
  // Initializes username.
  my_name = "Adrien";
  // Show a welcome message to the user.
  user.ShowMessage("Welcome to the");
  user.ShowMessage("chat system");
}

void loop() {
  if (ir_interface.ReceivedMessage()) {
    Message message = ir_interface.GetReceivedMessage();
    user.ShowMessage(message.name + ": " + message.text);
    // Optional improvement.
    // Add forwarding of message to allow more than 2 users to join the chat.
  }

  if (user.HasMessageToSend()) {
    String line = user.GetMessageToSend();
    // Show to the user his own message.
    user.ShowMessage("you: " + line);
    // Initializes a message and send it.
    Message message;
    message.name = my_name;
    message.text = line;
    ir_interface.Send(message);
  }
}
