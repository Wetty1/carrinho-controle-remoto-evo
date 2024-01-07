#include <PS2X_lib.h>  //for v1.6
#include "RF24.h"

PS2X ps2x;  // create PS2 Controller Class
RF24 radio(7, 8);

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

//right now, the library does NOT support hot pluggable controllers, meaning
//you must always either restart your Arduino after you conect the controller,
//or call config_gamepad(pins) again after connecting the controller.
int error = 0;
byte type = 0;
byte vibrate = 0;

int speed = 0;

void setup() {
  Serial.begin(115200);

  //CHANGES for v1.6 HERE!!! **************PAY ATTENTION*************

  error = ps2x.config_gamepad(2, 3, 4, 5, true, true);  //setup pins and settings:  GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error

  radio.begin();                          //Inicialidando o MÓDULO RF24l01 para comunicação.
  radio.setAutoAck(false);                // Desativando pacotes ACK (Pacote de Confirmação de Recebimento de Mensagem)
  radio.setChannel(100);                    // Configurando Módulo para operar no canal numéro 1 (você pode escolher um canal de 0 a 127) (Canal Padrão é o 76)
  radio.setDataRate(RF24_250KBPS);        // Configurando Módulo para operar em uma taxa de 250kbps (Menor taxa de dados possível para um maior alcance do rádio)
  radio.setPALevel(RF24_PA_HIGH);         // Configurando Módulo para transmitir em potência máxima
  radio.powerUp();                        // Ativando Módulo, caso entre em estado de baixo consumo.
  radio.openWritingPipe(0xE8E8F0F0E1LL);  // Abrindo o meio de comunicação entre transmissor e receptor e configurando endereço de comunicação(0xE8E8F0F0E1LL)

  radio.stopListening();   
  radio.printDetails();

  if (error == 0) {
    Serial.println("Found Controller, configured successful");
    Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
    Serial.println("holding L1 or R1 will print out the analog stick values.");
    Serial.println("Go to www.billporter.info for updates and to report bugs.");
  }

  else if (error == 1)
    Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");

  else if (error == 2)
    Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");

  else if (error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");

  type = ps2x.readType();
  switch (type) {
    case 0:
      Serial.println("Unknown Controller type");
      break;
    case 1:
      Serial.println("DualShock Controller Found");
      break;
    case 2:
      Serial.println("GuitarHero Controller Found");
      break;
  }
}

void loop() {
  /* You must Read Gamepad to get new values
   Read GamePad and set vibration values
   ps2x.read_gamepad(small motor on/off, larger motor strenght from 0-255)
   if you don't enable the rumble, use ps2x.read_gamepad(); with no values
   
   you should call this at least once a second
   */

  if (error == 1)  //skip loop if no controller found
    return;

  if (type == 2) {  //Guitar Hero Controller

    ps2x.read_gamepad();  //read controller

    if (ps2x.ButtonPressed(GREEN_FRET))
      Serial.println("Green Fret Pressed");
    if (ps2x.ButtonPressed(RED_FRET))
      Serial.println("Red Fret Pressed");
    if (ps2x.ButtonPressed(YELLOW_FRET))
      Serial.println("Yellow Fret Pressed");
    if (ps2x.ButtonPressed(BLUE_FRET))
      Serial.println("Blue Fret Pressed");
    if (ps2x.ButtonPressed(ORANGE_FRET))
      Serial.println("Orange Fret Pressed");


    if (ps2x.ButtonPressed(STAR_POWER))
      Serial.println("Star Power Command");

    if (ps2x.Button(UP_STRUM))  //will be TRUE as long as button is pressed
      Serial.println("Up Strum");
    if (ps2x.Button(DOWN_STRUM))
      Serial.println("DOWN Strum");


    if (ps2x.Button(PSB_START))  //will be TRUE as long as button is pressed
      Serial.println("Start is being held");
    if (ps2x.Button(PSB_SELECT))
      Serial.println("Select is being held");


    if (ps2x.Button(ORANGE_FRET))  // print stick value IF TRUE
    {
      Serial.print("Wammy Bar Position:");
      Serial.println(ps2x.Analog(WHAMMY_BAR), DEC);
    }
  }

  else {  //DualShock Controller
    ps2x.read_gamepad(false, vibrate);  //read controller and set large motor to spin at 'vibrate' speed

    if (ps2x.Button(PSB_START)) {  //will be TRUE as long as button is pressed
      data.startButton = 1;
      Serial.println("Start is being held");
    }
    if (ps2x.Button(PSB_SELECT)) {
      Serial.println("Select is being held");
      data.selectButton = 1;
    } else {
      data.selectButton = 0;
    }
      

    if (ps2x.Button(PSB_PAD_UP)) {  //will be TRUE as long as button is pressed
      Serial.print("Up held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_UP), DEC);
      data.upButton = 1;
    }
    if (ps2x.Button(PSB_PAD_RIGHT)) {
      Serial.print("Right held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_RIGHT), DEC);
      data.rightButton = 1;
    }
    if (ps2x.Button(PSB_PAD_LEFT)) {
      Serial.print("LEFT held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_LEFT), DEC);
      data.leftButton = 1;
    }
    if (ps2x.Button(PSB_PAD_DOWN)) {
      Serial.print("DOWN held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_DOWN), DEC);
      data.downButton = 1;
    }

    vibrate = ps2x.Analog(PSAB_BLUE);  //this will set the large motor vibrate speed based on
                                       //how hard you press the blue (X) button
    if (ps2x.NewButtonState())  //will be TRUE if any button changes state (on to off, or off to on)
    {
      if (ps2x.Button(PSB_L3)) {
        Serial.println("L3 pressed");
        data.l3Button = 1;
      }
      if (ps2x.Button(PSB_R3)) {
        Serial.println("R3 pressed");
        data.r3Button = 1;
      }
      if (ps2x.Button(PSB_L2)) {
        Serial.println("L2 pressed");
        data.l2Button = 1;
      }
      if (ps2x.Button(PSB_R2)) {
        Serial.println("R2 pressed");
        data.r2Button = 1;
      }
      if (ps2x.Button(PSB_L1)) {
        Serial.println("L1 pressed");
        data.l1Button = 1;
      } else {
        data.l1Button = 0;
      }
      if (ps2x.Button(PSB_R1)) {
        Serial.println("R1 pressed");
        data.r1Button = 1;
      } else {
        data.r1Button = 0;
      }
      if (ps2x.Button(PSB_GREEN)) {
        Serial.println("Triangle pressed");
        data.triangleButton = 1;
      } 


      if (ps2x.ButtonPressed(PSB_RED)){ //will be TRUE if button was JUST pressed
        Serial.println("Circle just pressed");
        data.circleButton = 1;
      }

      if (ps2x.ButtonReleased(PSB_PINK)){  //will be TRUE if button was JUST released
        Serial.println("Square just released");
        data.squareButton = 1;
      }

      if (ps2x.NewButtonState(PSB_BLUE)){  //will be TRUE if button was JUST pressed OR released
        Serial.println("X just changed");
      }
      if (ps2x.Button(PSB_L1) || ps2x.Button(PSB_R1))  // print stick values if either is TRUE
      {
        Serial.print("Stick Values:");
        Serial.print(ps2x.Analog(PSS_LY), DEC);  //Left stick, Y axis. Other options: LX, RY, RX
        Serial.print(",");
        Serial.print(ps2x.Analog(PSS_LX), DEC);
        Serial.print(",");
        Serial.print(ps2x.Analog(PSS_RY), DEC);
        Serial.print(",");
        Serial.println(ps2x.Analog(PSS_RX), DEC);
         
      }
    }
    data.analogRX = ps2x.Analog(PSS_RX);
    data.analogRY = ps2x.Analog(PSS_RY);
    data.analogLX = ps2x.Analog(PSS_LX);
    data.analogLY = ps2x.Analog(PSS_LY); 

    Serial.println("Select pressed");
    Serial.println(data.selectButton);

    Serial.println("R1 pressed");
    Serial.println(data.r1Button);

    Serial.println("L1 pressed");
    Serial.println(data.l1Button);

    radio.write(&data, sizeof(rfTypeData));
    Serial.println(sizeof(rfTypeData));
    delay(50);
  }
}