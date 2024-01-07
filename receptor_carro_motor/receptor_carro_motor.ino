#include <RF24.h>     // Biblioteca com as funções de controle do módulo NRF24.
#include <Servo.h>   

uint8_t m_in1 = 4;
uint8_t m_in2 = 6;
uint8_t pinSpeedMotor = 5;

Servo myservo; // Criando um objeto do tipo Servo para controle do Servo Motor

RF24 radio(7, 8);  

int speed = 0;

struct dataSctructSend {
  byte xButton = 0;
  byte circleButton = 0;
  byte squareButton = 0;
  byte triangleButton = 0;
  byte l1Button = 0;
  byte l2Button = 0;
  byte l3Button = 0;
  byte r1Button = 0;
  byte r2Button = 0;
  byte r3Button = 0;
  byte analogRX = 0;
  byte analogRY = 0;
  byte analogLX = 0;
  byte analogLY = 0;
  byte upButton = 0;
  byte downButton = 0;
  byte leftButton = 0;
  byte rightButton = 0;
  byte startButton = 0;
  byte selectButton = 0;
};

typedef struct dataSctructSend rfTypeData;
rfTypeData data;

bool headlights = false;

void setup() {
  Serial.begin(115200);
  delay(100);   

  // put your setup code here, to run once:
  myservo.attach(9);  // SERVO MOTOR DIREÇÃO 

  radio.begin();                            
  radio.setAutoAck(false);                 
  radio.setChannel(100);                   
  radio.setDataRate(RF24_250KBPS);         
  radio.setPALevel(RF24_PA_HIGH);         
  radio.powerUp();                          
  radio.openReadingPipe(1, 0xE8E8F0F0E1LL); 

  pinMode(A0, OUTPUT);

  radio.startListening();                 
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10); 
  // Serial.println("Available");
  // Serial.println(radio.available());
  if (radio.available()) { 

    radio.read( &data, sizeof(data) );  
    Serial.println(data.selectButton);

    int auxLY = 128;
    int auxLX = 128;
    int auxRY = 128;
    int auxRX = 128;

    Serial.println("analogLX");
    Serial.println(data.analogLX);
    if (data.analogLX >= 130) {
      int auxServo = map(data.analogLX, 128, 255, 90, 55);
      myservo.write(auxServo);
    } else if (data.analogLX <= 126) {
      int auxServo = map(data.analogLX, 128, 0, 90, 125);
      myservo.write(auxServo);
    } else {
      myservo.write(90);
    }

    int speed = 0;
    Serial.println("Motor Y");
    Serial.println(data.analogRY);
    Serial.println("R1");
    Serial.println(data.r1Button);
    if (data.r1Button) {
      digitalWrite(m_in1, LOW);
      digitalWrite(m_in2, HIGH);
      Serial.println("R1");
      speed = 120;
    } else if (data.analogRY >= 130) {
      digitalWrite(m_in1, HIGH);
      digitalWrite(m_in2, LOW);
      speed = map(data.analogRY, 128, 255, 0, 175);
      // analogWrite(pinSpeedMotor, speed);    
    } else if (data.analogRY <= 126) {
      digitalWrite(m_in1, LOW);
      digitalWrite(m_in2, HIGH);
      speed = map(data.analogRY, 128, 0, 0, 175);
    } else {
      digitalWrite(m_in1, LOW);
      digitalWrite(m_in2, LOW);
    }

    if (data.l1Button && !data.r1Button) {
      speed = 255;
    }

    if (data.selectButton) {
      if (headlights) {
        digitalWrite(A0, HIGH);
      } else {
        digitalWrite(A0, LOW);
      }
      headlights = !headlights;
    }

    analogWrite(pinSpeedMotor, speed);    
    Serial.println("Velocidade");
    Serial.println(speed);
  } 
}
