
#define TICK_LENGTH_US 10

/*
   half clock is 500us = 50 ticks
   add 150 us buffer (15 additional ticks)
*/
#define HALF_CLOCK_PLUS_BUFFER_TICKS 130 //100 ticks + 30 tick buffer = 1300us

enum Direction {
  DIR_RISING,
  DIR_FALLING
};

struct Edge {
  Direction dir;
  unsigned long count;
};

enum State {
  //IDLE_LOW,
  //IDLE_HIGH,
  STATE_IDLE,
  STATE_LOW,
  STATE_HIGH,
};

class Sampler {
  public:
    Sampler() : prev_pin(0), count(0), report(0), sync(0), edge_idx(0) {
      state = STATE_IDLE;
      prev_pin = (PINE >> PE5) & 0x1;
    }

    void tick() {

      bool the_bit;

      if (edge_idx >= 100 || report) {
        return;
      }

      ++count;
      bool timeout = count >= 3250;
      bool pin = (PINE >> PE5) & 0x1;

      switch (state) {
        case STATE_IDLE:
          if (pin != prev_pin) {
            state = pin ? STATE_HIGH : STATE_LOW;
          }
          break;
        case STATE_LOW:

          if (timeout) {
            state = STATE_IDLE;
            report = true;
          } else if (pin) {
            state = STATE_HIGH;
            edges[edge_idx++] = {DIR_RISING, count};
            timeout = false;
          }
          break;

        case STATE_HIGH:

          if (timeout) {
            state = STATE_IDLE;
            report = true;
          } else if (!pin) {
            state = STATE_LOW;
            edges[edge_idx++] = {DIR_FALLING, count};
            timeout = false;
          }
          break;
      }

      if (state != STATE_IDLE) {

        if (prev_pin != pin) {

            if (count >= HALF_CLOCK_PLUS_BUFFER_TICKS) {
              sync = true;
              the_bit = pin;
              count = 0;
            }
          
        }
        
      }

      prev_pin = pin;

    }

    void do_report() {
      if (!report) {
        return;
      }

      cli();
      for (int i = 0; i < edge_idx; i++) {
        Serial.print(edges[i].dir == DIR_FALLING ? "Falling " : "Rising ");
        Serial.println(edges[i].count);
      }
      Serial.println("X");
      report = 0;
      edge_idx = 0;
      count = 0;
      sei();
    }

  private:
    unsigned long count;
    bool report;
    State state;
    Edge edges[100];
    unsigned int edge_idx;
    bool sync;
    bool prev_pin;
};

Sampler *s = new Sampler();

void setup() {

  Serial.begin(9600);
  DDRE &= ~_BV(PE5); //direction

  TCCR3B = _BV(WGM32) | _BV(CS30);
  TCCR3A = 0;

  OCR3A = 160;
  //OCR3B = 1;
  TCCR3C = _BV(FOC3A);// | _BV(FOC3B);
  TIMSK3 = _BV(OCIE3A);// | _BV(OCIE3B);

  Serial.println("Ready");
  sei();

}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  s->do_report();

}

ISR(TIMER3_COMPA_vect)
{
  s->tick();
}


