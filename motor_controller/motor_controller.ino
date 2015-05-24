void setup() {
  pinMode(9, OUTPUT);  // h-bridge n1
  pinMode(10, OUTPUT); // h-bridge n2
  pinMode(11, OUTPUT); // h-bridge n3
  pinMode(12, OUTPUT); // h-bridge n4
  pinMode(13, OUTPUT); // status LED
  
  pinMode(2, INPUT);   // pulse input
  
  digitalWrite(9, 0);
  digitalWrite(10, 0);
  digitalWrite(11, 0);
  digitalWrite(12, 0);
  digitalWrite(13, 0);
  
  digitalWrite(2, 1); // enable pulup
}

int previous_state = 1;
void loop() {
  int state = digitalRead(2);
  
  if (previous_state == 1 && state == 0)
  {
    // pulse condition
    digitalWrite(13, 1);
    digitalWrite(9, 1);
    delay(15000);
    digitalWrite(9, 0);
    digitalWrite(13, 0);
  }
  
  previous_state = state;
}
