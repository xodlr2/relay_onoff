#define SYS_LED   13
#define TURN_ON   12
#define TURN_OFF  11

#define RELAY_ON  0
#define RELAY_OFF  1

#define BOOT_MODE 0
#define TURN_ON_MODE 10
#define TURN_OFF_MODE 20

#define T_ON_AFTER_DELAY  59
#define T_OFF_AFTER_DELAY 59

void Main1Sec(void);
void Timer_Int_flag(void);
void timer_setup();

unsigned char timer1SecFalg = 0;
unsigned char sys_mode = 0;
unsigned char sys_step = 0;
unsigned int  sys_timer = 0;


void setup() {
  // put your setup code here, to run once:
  pinMode(SYS_LED, OUTPUT);
  pinMode(TURN_ON, OUTPUT);
  pinMode(TURN_OFF, OUTPUT);
  Serial.begin(9600); // serial communication for debug monitoring
  Serial.println("Program Start");
  timer_setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  Timer_Int_flag();
  switch (sys_mode) {
  case BOOT_MODE :
    Serial.println("boot mode");
    digitalWrite(TURN_ON, RELAY_OFF);
    digitalWrite(TURN_OFF, RELAY_OFF);
    sys_mode++;
    break;
  case BOOT_MODE +1 :
    sys_mode = TURN_ON_MODE;
    break;
  case TURN_ON_MODE :
    Serial.println("TURN_ON mode");
    sys_mode++;
    break;
  case TURN_ON_MODE+1 :
    digitalWrite(TURN_ON, RELAY_ON);
    sys_timer = 0;
    sys_mode++;
    break;
  case TURN_ON_MODE+2 :
    if(sys_timer) {
      digitalWrite(TURN_ON, RELAY_OFF);
      sys_timer = 0;
      sys_mode++;
    }
    break;
  case TURN_ON_MODE+3 :
    if(sys_timer>T_ON_AFTER_DELAY) {
      sys_mode = TURN_OFF_MODE;
    }
    break;
  case TURN_OFF_MODE :
    Serial.println("TURN_OFF mode");
    sys_mode++;
    break;
  case TURN_OFF_MODE+1 :
    digitalWrite(TURN_OFF, RELAY_ON);
    sys_timer = 0;
    sys_mode++;
    break;
  case TURN_OFF_MODE+2 :
    if(sys_timer) {
      digitalWrite(TURN_OFF, RELAY_OFF);
      sys_timer = 0;
      sys_mode++;
    }
    break;
  case TURN_OFF_MODE+3 :
    if(sys_timer>T_ON_AFTER_DELAY) {
      sys_mode = TURN_ON_MODE;
    }
    break;
  default :
    sys_mode = BOOT_MODE;
    break;
  }
}

void Main1Sec(void)
{
  digitalWrite(SYS_LED, digitalRead(SYS_LED) ^ 1);
  sys_timer++;
}
void Timer_Int_flag(void)
{
  if (timer1SecFalg)
  {
    timer1SecFalg = 0;
    Main1Sec();
  }
}

ISR(TIMER1_COMPA_vect) //, ISR_NOBLOCK)
{
  static uint8_t ms10CntDown = 10, ms100CntDown = 10, Sec1CntDown = 10;
  if (--ms10CntDown == 0)
  {
    ms10CntDown = 10;
    if (--ms100CntDown == 0)
    {
      ms100CntDown = 10;
      if (--Sec1CntDown == 0)
      {
        Sec1CntDown = 10;
        timer1SecFalg = 1;
      }
    }
  }
}

void timer_setup(void)
{
  TCCR1A = 0;
  TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10); // CTC mode, set to 1/256 prescaler
  TCCR1C = 0;
  OCR1AH = (0 >> 8);
  OCR1AL = 249;
  TIMSK1 = (1 << OCIE1A);
}
