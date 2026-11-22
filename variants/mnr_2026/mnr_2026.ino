// Sensores
#define TEMP_SENSOR      A0
#define GAS_SENSOR       A15

// Atuadores
#define LED_VERDE        13
#define LED_AMARELO      12
#define LED_VERMELHO     11
#define BUZZER           10

// Sensor Ultrassônico
#define TRIG_PIN         9
#define ECHO_PIN         8
const int motorE1 = 3;
const int motorE2 = 2;
const int motorD1 = 4;
const int motorD2 = 5;
int sensor_leitura[5] = {A1, A2, A3, A4, A5};
float sensor_bruto[5] {0,0,0,0,0};
float sensor_max[5]= {0,0,0,0,0};
float sensor_min[5] = { 1023,1023, 1023, 1023, 1023};
float sensor_normal[5] = {0,0,0,0,0};
int sensor_digital[5] = {0,0,0,0,0};
int pesos[5] = {-2,-1,0,1,2};
bool gap = false;
float kp = 130;
float kd = 2;
int velocidade_base = 140;

void setup(){
  Serial.begin(9600);
  pinMode(motorE1, OUTPUT);
  pinMode(motorE2, OUTPUT);
  pinMode(motorD1, OUTPUT);
  pinMode(motorD2, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  Serial.println("Calibrando...");
  calibracao();
}

void loop(){
  controle();
  float dist = readDistance();
  float temp = readTemperature();
  int gasRaw = readGasRaw();

  bool alertaLeve =
          (temp >45  && temp <= 55) ||
          (dist >= 10 && dist < 15) ||
          (gasRaw > 250 && gasRaw <= 350);

  bool alertaGrave =
          (temp > 55) ||
          (dist < 10) ||
          (gasRaw > 350);

  Serial.print("Distancia: ");
  Serial.print(dist);
  Serial.print(" cm | Temperatura: ");
  Serial.print(temp);
  Serial.print(" °C | Gas: ");
  Serial.println(gasRaw);

  if (alertaGrave) {
      tone(BUZZER, 1000);
      digitalWrite(LED_VERMELHO, HIGH);
      digitalWrite(LED_AMARELO, LOW);
      digitalWrite(LED_VERDE, LOW);
      while (dist < 10){
        analogWrite(motorD1, 0); analogWrite(motorE1, 0);
        analogWrite(motorD2, 0); analogWrite(motorE2, 0);
        dist = readDistance();
      }

  } else if (alertaLeve) {
      noTone(BUZZER);
      digitalWrite(LED_VERDE, LOW);
      digitalWrite(LED_AMARELO, HIGH);
      digitalWrite(LED_VERMELHO, LOW);

  } else {
      noTone(BUZZER);
      digitalWrite(LED_VERDE, HIGH);
      digitalWrite(LED_AMARELO, LOW);
      digitalWrite(LED_VERMELHO, LOW);
  }
}

void readraw(){
  for (int i = 0; i<5; i++){
    sensor_bruto[i] = analogRead(sensor_leitura[i]);
  }
}

void calibracao() {
  unsigned long timer = millis();
  while (millis() - timer < 5000) {
    readraw();
    for (int i = 0; i<5; i++){
      if (sensor_max[i] < sensor_bruto[i]) sensor_max[i] = sensor_bruto[i];
      if (sensor_min[i] > sensor_bruto[i]) sensor_min[i] = sensor_bruto[i];
    }
  }
}

float readDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH);
    float distance = duration * 0.034 / 2;

    return distance;
}

float readTemperature() {
    int value = analogRead(TEMP_SENSOR);
    float voltage = value * (5.0 / 1023.0);
    float temp = voltage * 100;
    return temp;
}

int readGasRaw() {
    return analogRead(GAS_SENSOR);
}

void normalizacao() {
  readraw();
  for (int i = 0; i<5; i++){
    sensor_normal[i] = constrain(map(sensor_bruto[i], sensor_min[i], sensor_max[i], 0, 100), 0, 100);
  }
}

float posicao_linha() {
  float posicao = 0;
  float total = 0;
  float soma = 0;
  for (int i = 0; i<5; i++) {
    soma += sensor_normal[i] * pesos[i];
    total += sensor_normal[i];
  }
  if (total > 10) {
      posicao = soma/total;
      gap = false;
  } else {
    gap = true;
  }
  return posicao;
}

float PID(float posicao){
  float setpoint = 0;
  static float erroanterior = 0;
  static unsigned long lastime = 0;
  float erro = setpoint-posicao;
  unsigned long now = micros();
  float dt = (now-lastime)/1000000.0;
  float derivada = (erro-erroanterior)/dt;
  float saida = kp*erro + kd*derivada;
  erroanterior = erro;
  lastime = now;
  return saida;
}

void motor(float pid) {
  int velocidade_esquerda = constrain(velocidade_base - pid, -255, 255);
  int velocidade_direita = constrain(velocidade_base + pid, -255, 255);
  if (velocidade_direita > -100 && velocidade_direita <0) velocidade_direita = -100;
  if (velocidade_direita <100 && velocidade_direita>0) velocidade_direita = 100;
  if (velocidade_esquerda> -100 && velocidade_esquerda <0) velocidade_esquerda = -100;
  if (velocidade_esquerda <100 &&  velocidade_esquerda >0) velocidade_esquerda = 100;

  if (velocidade_direita >= 0) {
      analogWrite(motorD1, velocidade_direita);
      analogWrite(motorD2, 0);
  } else {
      analogWrite(motorD1, 0);
      analogWrite(motorD2, -velocidade_direita);
  }

  if (velocidade_esquerda >= 0) {
      analogWrite(motorE1, velocidade_esquerda);
      analogWrite(motorE2, 0);
  } else {
      analogWrite(motorE1, 0);
      analogWrite(motorE2, -velocidade_esquerda);
  }
}

void digital(){
  normalizacao();
  for (int i = 0; i<5; i++) {
    if (sensor_normal[i] >= 80) {
      sensor_digital[i] = 1;
    } else {
      sensor_digital[i] = 0;
    }
  }
}

void controle(){
  digital();
  float pos = posicao_linha();
  if (sensor_digital[0] + sensor_digital[1] >= 2 && sensor_normal[4] == 0){
        analogWrite(motorD1, 160); analogWrite(motorE1, 0);
        analogWrite(motorD2, 0); analogWrite(motorE2,160);
        delay(300);
  } else if (sensor_digital[4] + sensor_digital[3] >=2 && sensor_normal[0] == 0){
        analogWrite(motorD1, 0); analogWrite(motorE1,160);
        analogWrite(motorD2, 160); analogWrite(motorE2, 0);
        delay(300);
  } else if (sensor_digital[0] && sensor_digital[1] && sensor_digital[2] && sensor_digital[3] && sensor_digital[4]) {
        unsigned long timer = millis();
      while(millis()-timer<850) {
        digital();
        if(!sensor_digital[2]) {
          analogWrite(motorD1, 0); analogWrite(motorE1, 0);
          analogWrite(motorD2, 0); analogWrite(motorE2, 0);
          delay(6000);
          return;
        }
      }
  } else if(gap) {
      motor(0);
      posicao_linha();
  } else {
      float pid = PID(pos);
      motor(pid);
  }
}
