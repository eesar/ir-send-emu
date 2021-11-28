//--------------------------------------------------------------------------------------------------
// 29.11.2021 - Stephan Rau: initial version
//--------------------------------------------------------------------------------------------------
// Assumptions:
//  - long low phase is start sequence
//  - data is not more than 32 bits, else long variable for code is not enough
//  - a zero is repesented by a longer low than high phase
//  - a one is repesentend by a longer high than low phase
//--------------------------------------------------------------------------------------------------

// protocol encoding times -------------------------------------------------------------------------
const unsigned int start_0l     =  9000; // start sequece   low
const unsigned int start_1h     =  4500; // start sequece   high
const unsigned int start_2l     =   550; // start sequece   low
const unsigned int zeros_h      =   550; // zero  encoding  high
const unsigned int zeros_l      =   550; // zero  encoding  low
const unsigned int ones_h       =  1600; // one   encoding  high
const unsigned int ones_l       =   550; // one   encoding  low
const unsigned int end_0h       =  5000; // end   sequece   high 39400
const unsigned int end_1l       =  9000; // end   sequece   low
const unsigned int end_2h       =  2250; // end   sequece   high
const unsigned int end_3l       =   550; // end   sequece   low
const unsigned int num_of_bits  = 31;

// connected pins ----------------------------------------------------------------------------------
const byte IR_RCV_PIN           = 3;    // main send output
const byte IR_SEND_PIN          = 4;    // main send output
const byte IR_VAL_PIN           = 13;   // copy rcv value to on board LED

// serial settings ---------------------------------------------------------------------------------
const String ENABLE_CMD     = "on";  // turn on  led stripe
const String DISABLE_CMD    = "off"; // turn off led stripe
const char   CMD_END        = '\n';  // character for command end
const String UP_CMD         = "up";
const String DOWN_CMD       = "down";
const String RED_CMD        = "red";
const String GREEN_CMD      = "green";
const String BLUE_CMD       = "blue";
const String WHITE_CMD      = "white";
const String FLASH_CMD      = "flash";
const String STROBE_CMD     = "strobe";
const String FADE_CMD       = "fade";
const String JUMP_CMD       = "jump";
const String R1_CMD         = "r1";
const String R2_CMD         = "r2";
const String R3_CMD         = "r3";
const String R4_CMD         = "r4";
const String G1_CMD         = "g1";
const String G2_CMD         = "g2";
const String G3_CMD         = "g3";
const String G4_CMD         = "g4";
const String B1_CMD         = "b1";
const String B2_CMD         = "b2";
const String B3_CMD         = "b3";
const String B4_CMD         = "b4";
const String LOOP_CMD       = "loop";

// global variables --------------------------------------------------------------------------------
static String serialInStr;  // read string from serial port


//--------------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(19200); // 9600 higher speed needed else print is not finshed before next interrupt
  pinMode(IR_VAL_PIN, OUTPUT);
  pinMode(IR_RCV_PIN, INPUT);  
  pinMode(IR_SEND_PIN, OUTPUT);
  digitalWrite(IR_SEND_PIN, HIGH); // default state
}


//--------------------------------------------------------------------------------------------------
void loop() {

  // indicate that something will be send
  digitalWrite(IR_VAL_PIN, digitalRead(IR_RCV_PIN));
 
  // check and read command from PC
  if ( Serial.available() > 0 ) {
    serialInStr = Serial.readStringUntil(CMD_END);

    unsigned long code;
    if ( serialInStr == DISABLE_CMD ) { code = 0x00FFF807; }
    if ( serialInStr == ENABLE_CMD  ) { code = 0x00FFB04F; }
    if ( serialInStr == UP_CMD      ) { code = 0x00FF906F; }
    if ( serialInStr == DOWN_CMD    ) { code = 0x00FFB847; }
    if ( serialInStr == RED_CMD     ) { code = 0x00FF9867; }
    if ( serialInStr == GREEN_CMD   ) { code = 0x00FFD827; }
    if ( serialInStr == BLUE_CMD    ) { code = 0x00FF8877; }
    if ( serialInStr == WHITE_CMD   ) { code = 0x00FFA857; }
    if ( serialInStr == FLASH_CMD   ) { code = 0x00FFB24D; }
    if ( serialInStr == STROBE_CMD  ) { code = 0x00FF00FF; }
    if ( serialInStr == FADE_CMD    ) { code = 0x00FF58A7; }
    if ( serialInStr == JUMP_CMD    ) { code = 0x00FF30CF; }
    if ( serialInStr == R1_CMD      ) { code = 0x00FFE817; }
    if ( serialInStr == R2_CMD      ) { code = 0x00FF02FD; }
    if ( serialInStr == R3_CMD      ) { code = 0x00FF50AF; }
    if ( serialInStr == R4_CMD      ) { code = 0x00FF38C7; }
    if ( serialInStr == G1_CMD      ) { code = 0x00FF48B7; }
    if ( serialInStr == G2_CMD      ) { code = 0x00FF32CD; }
    if ( serialInStr == G3_CMD      ) { code = 0x00FF7887; }
    if ( serialInStr == G4_CMD      ) { code = 0x00FF28D7; }
    if ( serialInStr == B1_CMD      ) { code = 0x00FF6897; }
    if ( serialInStr == B2_CMD      ) { code = 0x00FF20DF; }    
    if ( serialInStr == B3_CMD      ) { code = 0x00FF708F; }
    if ( serialInStr == B4_CMD      ) { code = 0x00FFF00F; }

    rgb_send( code );

    if ( serialInStr == LOOP_CMD ) {
      /*
      rgb_send( 0x00FFB04F );
      for ( code = 0x00FF0000 ; code <= 0x00FFFFFF ; code++ ) {
        delay(500);
        rgb_send( code );
      }
      */
      rgb_send( 0x00FFB04F );
      rgb_send( 0x00FF9867 );
      for ( int i = 0 ; i < 10 ; i++ ) {
        rgb_send( 0x00FFB847 );
      }
     
    }
  }

}


void rgb_send( unsigned long code ) {

  Serial.print(code, HEX);
  Serial.print(": ");

  // send start sequence
  digitalWrite(IR_SEND_PIN, LOW ); delayMicroseconds(start_0l);
  digitalWrite(IR_SEND_PIN, HIGH); delayMicroseconds(start_1h);
  digitalWrite(IR_SEND_PIN, LOW ); delayMicroseconds(start_2l);

  // send command code
  for (int i = num_of_bits; i >= 0; i--) {
    if (code & (1L << i)) {
      Serial.print(1);
      digitalWrite(IR_SEND_PIN, HIGH); delayMicroseconds(ones_h);
      digitalWrite(IR_SEND_PIN, LOW ); delayMicroseconds(ones_l);
    } else {
      Serial.print(0);
      digitalWrite(IR_SEND_PIN, HIGH); delayMicroseconds(zeros_h);
      digitalWrite(IR_SEND_PIN, LOW ); delayMicroseconds(zeros_l);
    }
  }

  // send end sequence
  for (int i = 0 ; i < 8 ; i++ ) {
    digitalWrite(IR_SEND_PIN, HIGH); delayMicroseconds(end_0h);
  }
  digitalWrite(IR_SEND_PIN, LOW ); delayMicroseconds(end_1l);
  digitalWrite(IR_SEND_PIN, HIGH); delayMicroseconds(end_2h);
  digitalWrite(IR_SEND_PIN, LOW ); delayMicroseconds(end_3l);
  digitalWrite(IR_SEND_PIN, HIGH); // just make sure pin is high
  
  Serial.println("");
}
