//projeto LED

void setup() {
  pinMode(6, OUTPUT);
}

void loop() {
  digitalWrite(6, HIGH); 
  delay(1500);            
  digitalWrite(6, LOW);  
  delay(500);            
}


//void setup() {
//  pinMode(LED_BUILTIN, OUTPUT);
//}

//void loop() {
//  digitalWrite(LED_BUILTIN, HIGH);
//  delay(500);                      
//  digitalWrite(LED_BUILTIN, LOW);   
//  delay(2500);                      
//}





//void setup() {
//  pinMode(13, OUTPUT); // Define o pino 13 como saída
//}

//void loop() {
//  digitalWrite(13, HIGH); 
//  delay(1500);            
//  digitalWrite(13, LOW);  
//  delay(500);            
//}