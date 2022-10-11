/*Universidad de Costa Rica
IE-0624 Laboratorio de Microcontroladores
Prof: Marco Villalta Fallas
Estudiantes: Juan Montealegre B95001 y Luis Herrera B93840
Laboratorio 3
Simulador de Incubadora de huevos
Ciclo: II-2022
*/

//Librerías utilizadas para manejo de pantalla LCD PCD854
#include <SPI.h>
#include <Adafruit_PCD8544.h>
#include <Adafruit_GFX.h>
#include <PID_v1.h> //Libería para manejo de controlador PID
#include <math.h>  //Operaciones matemáticas

#define HUMEDAD A3 //Pin analógico para la humedad
#define RESISTENCIA 10000 // Valor de resistencia
#define CALENTADOR 10 //Pin analógico para circuito calentador
#define TEMPERATURA A2 //Pin analógico para termistor
#define POT A4 //Pin analógico para el potenciometro

// Parámetros para ecuación de Steinhart-Hart
#define A 8.2485E-4
#define B 2.665E-4
#define C 9.239E-8


//Se definen todas las variables a utilizar
double humedad;
double mapeo_humedad;
double temp_sensada;
double resistencia;
double temp_op;
double mapeo_op;
double salida_PID; 
double mapeo_salida; 
double temperatura; 

//Parametros de controlador
double Kp = 3.25, Ki=0, Kd=0;

//Definimos nuestro controlador PID utilizando la libreria PID_v1
PID PID(&temperatura, &salida_PID, &mapeo_op, Kp, Ki, Kd, DIRECT);

// Pines de la pantalla conectados al arduino (CLK-> 2| DIN-> 3| D/C-> 4| CS-> 5| RST-> 6)
Adafruit_PCD8544 display = Adafruit_PCD8544(2, 3, 4, 5, 6);


void setup() {

  Serial.begin(9600); // Habilita los puertos de comunicación 
  
  // Configuración de pines para los LEDs
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  //Configuración switch
  pinMode(9, INPUT);
  
  // Se activa el display
  display.begin();

  //Inicializamos el controlador PID
  // Sensor humedad
  humedad = analogRead(HUMEDAD);
  mapeo_humedad = map(humedad, 0.0, 1023.0, 0.0, 100.0);

   //Sensor temperatura
  temp_sensada = analogRead(TEMPERATURA);
  resistencia = ((temp_sensada*5)/1023)/((5-((temp_sensada*5)/1023))/ RESISTENCIA );
  temperatura = A + B*(log(resistencia))+ C*(pow(log(resistencia),3));
  temperatura = (1/temperatura) - 273.15;

  //Potenciometro de operacion
  temp_op = analogRead(POT);
  mapeo_op = map(temp_op, 0.0, 1012.0, 30.0, 42.0);
  PID.SetMode(AUTOMATIC);
  PID.SetTunings(Kp, Ki, Kd);
}

void loop() {


  // Sensor humedad
  humedad = analogRead(HUMEDAD);
  mapeo_humedad = map(humedad, 0.0, 1023.0, 0.0, 100.0);

  //  Sensor temperatura
  temp_sensada = analogRead(TEMPERATURA);
  //Primero se calcula la resistencia
  resistencia = ((temp_sensada*5)/1023)/((5-((temp_sensada*5)/1023))/ RESISTENCIA );
  //Ahora se calcula la temperatura con la ecuación de Steinhart-Hart  
  temperatura = A + B*(log(resistencia))+ C*(pow(log(resistencia),3));
  temperatura = (1/temperatura) - 273.15;

  //Potenciometro de operacion
  temp_op = analogRead(POT);
  mapeo_op = map(temp_op, 0.0, 1012.0, 30.0, 42.0);

  delay(100);

  //Aplica el control
  PID.Compute();  
  mapeo_salida = map(salida_PID, 0.0, 255.0, 0.0, 80.0);

  analogWrite (CALENTADOR, salida_PID) ;
  if ((int)mapeo_salida==0) analogWrite (CALENTADOR, 0) ;
  //Encender LEDs
  if(((int)temperatura+(int)mapeo_salida)<30){
    digitalWrite(12, HIGH);
    digitalWrite(13, LOW);
  }
  else if(((int)temperatura+(int)mapeo_salida)>42){
    digitalWrite(13, HIGH);
    digitalWrite(12, LOW);
  }

  else{
    digitalWrite(12, LOW);
    digitalWrite(13, LOW);    
  }

  // Código para imprimir información en pantalla LCD
  display.clearDisplay(); 
  display.setCursor(0,0);
  display.print("Datos medidos"); 
  display.print("\n"); 
  display.print("--------------"); 
  display.print("\n"); 
  display.print("Humedad(%):"); // Se imprime la humedad medida
  display.println((int)mapeo_humedad);
  display.print("Control:"); // Se imprime la señal de control 
  display.println((int)mapeo_salida);
  display.print("Temp_s:"); // Se imprime la temperatura medida
  display.println((int)temperatura);
  display.print("Temp_op:"); // Se imprime la temperatura de operación elegida
  display.println((int)mapeo_op);
  display.display();

  if(digitalRead(9)==HIGH){  
    Serial.print((int)mapeo_humedad);
    Serial.print(",");
    Serial.print((int)mapeo_salida);
    Serial.print(",");
    Serial.print((int)temperatura);
    Serial.print(",");
    Serial.println((int)mapeo_op);
    }
}
