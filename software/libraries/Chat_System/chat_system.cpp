#include "chat_system.h"

LCD lcd;
IR_COM ir_com;

User::User(HardwareSerial* printer)
  : printer_(printer) {}

void User::ShowMessage(String text) {
  Serial.println(text);
  if (text.length() > 16) {
    text = text.substring(0, 14) + "..";
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(previous_line_);
  lcd.setCursor(0,1);
  lcd.print(text);
  previous_line_ = text;
}

void User::Init() {
  Serial.begin(9600);
  lcd.begin(16,2);
  // Useful?
  hs.begin();
}

bool User::HasMessageToSend() {
  if (available_) {
    return true;
  }
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n') {
      available_ = true;
      return true;
    } else {
      current_reading_line_ += c;
    }
  }
  return false;
}

String User::GetMessageToSend() {
  String result;
  if (available_) {
    result = current_reading_line_;
    current_reading_line_ = "";
    available_ = false;
  }
  return result;
}

void IRInterface::Init() {
  ir_com.begin();
}

void IRInterface::Send(const Message& message) {
  String text = message.name + ":" + message.text;
  for (int i = 0; i < text.length(); ++i) {
    ir_com.write(text[i]);
  }
  ir_com.write('\0');
}

bool IRInterface::ReceivedMessage() {
  if (available_) {
    return true;
  }
  while (ir_com.available() > 0) {
    char c = ir_com.read();
    if (c == '\0') {
      available_ = true;
      return true;
    } else {
      current_message_ += c;
    }
  }
  return false;
}

Message IRInterface::GetReceivedMessage() {
   Message result;
   if (available_) {
     int i = 0;
     while (i < current_message_.length() && current_message_[i] != ':') {
       result.name += current_message_[i];
       ++i;
     }
     result.text = current_message_.substring(i + 1, current_message_.length());
     current_message_ = "";
     available_ = false;
   }
   return result;
}
