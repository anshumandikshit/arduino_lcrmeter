#include <LiquidCrystal.h>

LiquidCrystal lcd(13, 8, 7, 5, 4, 2);

#define R_1 A1
#define R_2 A2
#define IND_1 6
#define IND_2 12
#define fuente_pin 11
#define switch_pin 10
#define descarga_pin 9

#define PIN_1 A3
#define PIN_2 A4

//Variables leer pines
int pin_1;
int pin_2;

//Variables para inductometro
double pulso;
double frecuencia;
double capacitancia;
double inductancia;

//Capacimetro
float R = 1.0e6;
float C = 0;
float RC = 0;
long t_inicio = 0;
volatile long t_alto = 0;
long T = 0;
float VCC = 4.50;
float Vref = VCC / 2;
float V0 = 0;
float error_correccion = 40.;
int retardo_delay = 20;


//Variables resistometro
int vR_1 = 0;
int vR_2 = 0;
float Vin = 5;
float Vout = 0;
float Res_1 = 10000;
float Res_2 = 9000000;
float r_1 = 0;
float r_2 = 0;
float Resistor_1[8];
float Resistor_2[5];
float ResArreglo_1;
float ResArreglo_2;

void setup() {
  lcd.begin(16, 2);

  //Configuracion de pines
  pinMode(PIN_1, INPUT);
  pinMode(PIN_2, INPUT);
  //Configuracion Inductometro
  pinMode(IND_1, INPUT);
  pinMode(IND_2, OUTPUT);
  capacitancia = 0.000001021;
  //delay(200);
  //Configuracion de Resistometro
  pinMode(R_1, INPUT);
  pinMode(R_2, INPUT);

  //Configuracion Capacimetro
  attachInterrupt(1, stop, RISING);
  Vref = VCC / 2;
  pinMode(fuente_pin, OUTPUT);
  digitalWrite(fuente_pin, LOW);
  pinMode(switch_pin, INPUT);
  pinMode(descarga_pin, INPUT);
}

void loop() {
  leerpines();

  if (pin_1 == LOW && pin_2 == LOW) {
    digitalWrite(IND_2, HIGH);
    delay(5);
    digitalWrite(IND_2, LOW);

    delayMicroseconds(100);

    pulso = pulseIn(IND_1, HIGH, 5000);
    lcd.clear();
    if (pulso > 0.1) {
      frecuencia = 1.E6 / (2 * pulso);
      inductancia = 1. / (capacitancia * frecuencia * frecuencia * 4.*3.1459 * 3.14159);
      inductancia *= 1E6;

      lcd.setCursor(2, 0);
      lcd.print("INDUCTANCIA:");
      //delay(200);
      if (inductancia >= 1000) {
        lcd.setCursor(0, 1);
        int valor = (inductancia / 1000) - 0.5;
        lcd.print(valor);
        lcd.setCursor(6, 1);
        lcd.print("mH");
      } else {
        lcd.setCursor(0, 1);
        int valor_2 = inductancia + 10;
        lcd.print(valor_2);
        lcd.setCursor(6, 1);
        lcd.print("uH");
      }
    } else if (pulso < 0.1) {
      lcd.setCursor(2, 0);
      lcd.print("INSERTAR IND");
    }
    delay(300);

  }

  if (pin_1 == LOW && pin_2 == HIGH) {
    lcd.clear();
    for (int i = 0 ; i <= 7; i++) {
      Resistor_1[i] = analogRead(R_1);
      ResArreglo_1 = ResArreglo_1 + Resistor_1[i];
    }
    vR_1 = (ResArreglo_1 / 8.0);
    Vout = (Vin * vR_1) / 1023;
    r_1 = Res_1 * (1 / ((Vin / Vout) - 1));
    lcd.setCursor(2, 0);
    lcd.print("RESISTENCIA:");
    if (r_1 <= 999) {
      lcd.setCursor(0, 1);
      lcd.print(r_1);
      lcd.setCursor(9, 1);
      lcd.print("Omhs");
    } else if (r_1 >= 1000) {
      r_1 = r_1 / 1000;
      lcd.setCursor(0, 1);
      lcd.print(r_1);
      lcd.setCursor(9, 1);
      lcd.print("KOmhs");
    }
    delay(500);
    ResArreglo_1 = 0;
  }

  if (pin_1 == HIGH && pin_2 == LOW) {
    lcd.clear();
    if (debounce(switch_pin) == LOW)
    {
      pinMode(descarga_pin, OUTPUT);
      digitalWrite(descarga_pin, LOW);
      delay(100);
      pinMode(descarga_pin, INPUT);
      digitalWrite(fuente_pin, HIGH);
      t_inicio = micros();

    }

    if (t_alto > 0 && t_inicio > 0 && (t_alto - t_inicio) > 0 )
    {
      T = (t_alto - t_inicio);
      RC = -T / log((Vref - VCC) / (V0 - VCC));
      //Vref = VCC/2
      //V0 = 0V
      C = RC / R;				//Valor en uF



      lcd.setCursor(0, 0);
      lcd.print("C:");
      lcd.setCursor(3, 0);
      lcd.print(C * 1000, 1);
      lcd.setCursor(13, 0);
      lcd.print("nF");
      lcd.setCursor(0, 1);
      lcd.print("C:");
      lcd.setCursor(3, 1);
      lcd.print(C * 1000000 - error_correccion , 0);
      lcd.setCursor(13, 1);
      lcd.print("pF");

      t_inicio = 0;
      t_alto = 0;

      digitalWrite(fuente_pin, LOW);
      delay(2000);
    }
  }
}

void leerpines() {
  pin_1 = digitalRead(PIN_1);
  pin_2 = digitalRead(PIN_2);
}

void stop()
{
  t_alto = micros();
}

int debounce(int pin)
{
  int estado;
  int previo_estado;
  previo_estado = digitalRead(pin);
  for (int i = 0; i < retardo_delay; i++)
  {
    delay(1);
    estado = digitalRead(pin);
    if ( estado != previo_estado)
    {
      i = 0;
      previo_estado = estado;
    }
  }
  return estado;
}
