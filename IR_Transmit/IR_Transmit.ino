

bool bits[8] = {
  true, false,
  true, false, 
  true, false,
  true, false,
  };

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  TCCR2A = _BV(WGM21) | _BV(COM2A0);
  OCR2A = 215;
  pinMode(11, OUTPUT);

  //TCCR2A = 0;
  //TCCR2B = 0;
  PORTB &= ~_BV(PB3);

  Serial.println("Ready");

  delay(1000);

  for (int i = 0; i < 8; i++) {

    if (bits[i]) {
      transmit_low();
      transmit_high();
    } else {
      transmit_high();
      transmit_low();
    }
  }
  Serial.println("Done");
  /*

  for (int j = 0; j < 10; j++) {
    Serial.println("Transmit");
    cli();
    for (int i = 0; i < 2; i++) {
      loopx();
    }
    sei();
    delay(5000);
  }

  */
}

void transmit_low() {

  TCCR2B = 0;

  PORTB &= ~_BV(PB3);

  delayMicroseconds(1000);
}

void transmit_high() {
  TCNT2 = 0;

  TCCR2B = _BV(CS20);

  delayMicroseconds(1000);
  
}

void loop() {
}

void loopx() {


  TCNT2 = 0;

  TCCR2B = _BV(CS20);

  delayMicroseconds(300);

  //TCCR2A = 0;
  TCCR2B = 0;

  PORTB &= ~_BV(PB3);

  delayMicroseconds(1000);

}
