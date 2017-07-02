/*                                                          
 +---------------------------------------------------------++ 
 +---------------------------------------------------------|| 
 ||                                                        || 
 ||   Exercício da Prova:                                  || 
 ||                                                        || 
 ||        - Cancela operando com:                         || 
 ||                {Painel LCD}                            || 
 ||                {Leitor de RFID}                        || 
 ||                {Leds Indicativos}                      || 
 ||                {Servo Motor}                           || 
 ||                                                        || 
 ||        No exemplo, existe um sensor óptico-reflexivo   || 
 ||         que detecta a presença do carro. Ao detectar   || 
 ||         liga o display LCD, ativa o leitor RFID e      || 
 ||         liga o led Vermelho.                           || 
 ||        Se o cartão estiver autorizado indica com Led   || 
 ||         verde, exibe no Display e abre a cancela.      || 
 ||        Se cartão nao autorizado. Indica no Display.    || 
 ||        Saindo o carro da posição, desliga o Display e  || 
 ||         desliga o leitor RFID.                         || 
 ||                                                        || 
 ||    Material Necessário:                                || 
 ||      /--------------------------------------------\    || 
 ||      | - Arduino UNO                              |    || 
 ||      | - Display LCD 16x2                         |    || 
 ||      | - Módulo I2C (poupar pinos no arduino UNO) |    || 
 ||      | - RFID (RC522) + TAG                       |    || 
 ||      | - Servo Tower Pro MG90S Microservo         |    || 
 ||      | - (Canudinho = Cancela!! :) )              |    || 
 ||      | - Led Vermelho / Led Verde                 |    || 
 ||      | - Sensor Óptico Reflexivo (TRCT5000)       |    || 
 ||      | - 3 resistores de 150 ohms                 |    || 
 ||      | - 1 resistor de 10K ohms                   |    || 
 ||      \--------------------------------------------/    || 
 ||                                                        || 
 ||          .  .:::.                                      || 
 ||            :(o o):  .  Yuri 02/07/17                   || 
 ++---------ooO--(_)--Ooo----------------------------------++ 
 
*/

#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h> // SCL - porta A5 e SDA - porta A4
#include <Servo.h>

// Definições do RFID 
#define SS_PIN 10
#define RST_PIN 9

// Definição do sensor de proximidade  
#define sensorPin 7 // 1 é sem obstáculo, 0 é com obstáculo

#define ledVerde 8
#define ledVermelho 4

// Variáveis e inicializações
boolean estado;                       // estado do sensor de proximidade
MFRC522 mfrc522(SS_PIN, RST_PIN);     // Cria a instância do RFID
Servo servo;                          // Cria a instância do Servo
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // I2C
boolean existeCarro = false;
boolean ligaLCD = false;

void setup()
{
  Serial.begin(9600);   // Inicia a serial
  SPI.begin();      // Inicia  SPI bus
  mfrc522.PCD_Init();   // Inicia MFRC522

  pinMode(ledVermelho, OUTPUT);
  pinMode(ledVerde, OUTPUT);

  servo.attach(5);
  delay(50);
  servo.write(2);

  lcd.begin(16, 2);
  lcd.setBacklight(LOW);
  
}

void loop(){
  
  existeCarro = !digitalRead(sensorPin); // Apenas para ficar mais intuitivo. 1 = tem carro, 0 = não tem carro
  
  if (existeCarro == 1){
    if (ligaLCD == 0){
      exibeLCD(0,"Aproxime o seu",0, "Cartao do leitor");
      led("vermelho", 1);
    }
    leituraRFID();
  } else if (existeCarro == 0 && ligaLCD == 1){
    delay(3000);
    servoClose();
    led("verde", 0);
    led("vermelho", 0);
    desligaDisplay();
    ligaLCD = 0;
  } 
    
}

void desligaDisplay(){
  lcd.setBacklight(LOW);
  lcd.noDisplay();
}
void exibeLCD(byte posLinha1, String msg1, byte posLinha2, String msg2){
  lcd.setBacklight(HIGH);
  lcd.clear();
  lcd.display();
  lcd.setCursor(posLinha1,0);
  lcd.print(msg1);
  lcd.setCursor(posLinha2,1);
  lcd.print(msg2);
  ligaLCD = 1;
}

void leituraRFID(){
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Mostra UID na serial
  Serial.print("UID da tag :");
  String conteudo = "";
  byte letra;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     //Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     //Serial.print(mfrc522.uid.uidByte[i], HEX);
     conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  //Serial.println();
  //Serial.print("Mensagem : ");
  conteudo.toUpperCase();
  if (conteudo.substring(1) == "15 B2 11 88") //UID 1 - Chaveiro
  {
    led("verde", 1);
    led("vermelho", 0);
    exibeLCD(0, "Ola Yuri", 0, "PODE PASSAR!!");
    delay(500);
    servoOpen();

  } else {
    led("vermelho", 1);
    exibeLCD(0, "(X-x-X-x-X-x-X)", 0, "Acesso Negado!!");
    delay(3000);
    //led("vermelho",0);
    exibeLCD(0,"Aproxime o seu",0, "Cartao do leitor");

  }
} 

void servoOpen(){
  
  servo.write(90);
  
}

void servoClose(){
  
  servo.write(2);
  
}

void led(String led, boolean valor){
  int pin;
  valor?HIGH:LOW;
  if (led == "verde"){
    pin = ledVerde;
  } else if (led == "vermelho"){
    pin = ledVermelho;
  }
  digitalWrite(pin, valor);
}
