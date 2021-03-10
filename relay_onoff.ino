#define SYS_LED    13
#define TURN_UP   11
#define TURN_DN   12
#define SENSOR_UP 7
#define SENSOR_DN 8

#define RELAY_ON  0
#define RELAY_OFF  1

#define BOOT_MODE 0
#define TURN_UP_MODE 10
#define TURN_DN_MODE 20
#define INTERVAL_DELAY_MODE 50

#define T_ON_TIME         500  //100msec unit
#define T_ON_AFTER_DELAY  400  //100msec unit
#define T_OFF_TIME        500  //100msec unit
#define T_OFF_AFTER_DELAY 400  //100msec unit
#define T_INTERVAL_DELAY  0  //100msec unit

#define SENSING_TRUE    0


void Main100mSec(void);
void Main1Sec(void);
void Timer_Int_flag(void);
void timer_setup();

uint8_t timer100mSecFalg = 0;
uint8_t timer1SecFalg = 0;
uint8_t sys_mode = 0;
uint8_t sys_step = 0;
uint16_t  op_timer = 0, sys_timer = 0;
uint16_t  repeat=0;

void setup() {
  // put your setup code here, to run once:
  pinMode(SYS_LED, OUTPUT);
  pinMode(TURN_UP, OUTPUT);
  pinMode(TURN_DN, OUTPUT);

  pinMode(SENSOR_UP, INPUT_PULLUP);
  pinMode(SENSOR_DN, INPUT_PULLUP);

  Serial.begin(9600); // serial communication for debug monitoring
  Serial.println("\nProgram Start");
  timer_setup();
}
void print_logs(uint8_t status) {
  Serial.print("repeat = ");
  Serial.print(repeat);
  Serial.print(" times, diff time = ");
  Serial.print(sys_timer/10);
  Serial.print(".");
  Serial.print(sys_timer%10);
  Serial.print(" sec, ");
  if (!status)  {
    Serial.println("be touched while going up");
  }
  else {
    Serial.println("be touched while going down");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  Timer_Int_flag();
  switch (sys_mode) {

    case BOOT_MODE :
      Serial.println("\nBOOT_MODE");
      digitalWrite(TURN_UP, RELAY_OFF);
      digitalWrite(TURN_DN, RELAY_OFF);
      sys_mode++;
      break;
    case BOOT_MODE +1 :
    repeat = 0;
      sys_mode = TURN_UP_MODE;
      break;

    case TURN_UP_MODE :
//      Serial.println("\nTURN_UP");
      digitalWrite(TURN_UP, RELAY_ON);
      sys_timer = 0;
      sys_mode++;
      break;
    case TURN_UP_MODE+1 :
      if (sys_timer >= T_ON_TIME || digitalRead(SENSOR_UP) == SENSING_TRUE) {
        digitalWrite(TURN_UP, RELAY_OFF);
        print_logs(1);
        sys_timer = 0;
        sys_mode++;
      }
      break;
    case TURN_UP_MODE+2 :
      if (sys_timer >= T_ON_AFTER_DELAY) {
        sys_timer = 0;
        sys_mode = TURN_DN_MODE;
      }
      break;

    case TURN_DN_MODE :
//      Serial.println("\nTURN_DN");
      digitalWrite(TURN_DN, RELAY_ON);
      sys_timer = 0;
      sys_mode++;
    case TURN_DN_MODE+1 :
      if (sys_timer >= T_OFF_TIME || digitalRead(SENSOR_DN) == SENSING_TRUE) {
        digitalWrite(TURN_DN, RELAY_OFF);
        print_logs(0);
        sys_timer = 0;
        sys_mode++;
      }
      break;
    case TURN_DN_MODE+2 :
      if (sys_timer >= T_OFF_AFTER_DELAY) {
        sys_timer = 0;
        sys_mode = INTERVAL_DELAY_MODE;
      }
      break;

    case INTERVAL_DELAY_MODE :
      if (sys_timer >= T_INTERVAL_DELAY) {
        repeat++;
        sys_timer = 0;
        sys_mode = TURN_UP_MODE;
      }
      break;

    default :
      sys_mode = BOOT_MODE;
      break;
  }
}

void Main100mSec(void)
{
  sys_timer++;
  op_timer++;
}

void Main1Sec(void)
{
  digitalWrite(SYS_LED, digitalRead(SYS_LED) ^ 1);
}

void Timer_Int_flag(void)
{
  if (timer100mSecFalg)
  {
    timer100mSecFalg = 0;
    Main100mSec();
  }
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
      timer100mSecFalg = 1;
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
