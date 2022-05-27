const int motor = 9;
const int echo = 3;
const int trigger = 2;
const int ref = A0;
const int periodo = 300; //Periodo del PWM en milisegundos
long t = 0; 
long d = 0;
double eng, enm, eop, epm, epg, Ap, y;
double r[8];
int P = 75; //Potencia (min 52.5)


void setup() {
  Serial.begin(9600);//iniciailzamos la comunicación
  pinMode(trigger, OUTPUT); //pin como salida
  pinMode(motor, OUTPUT);
  pinMode(echo, INPUT);  //pin como entrada
  digitalWrite(trigger, LOW);//Inicializamos el pin con 0
}

void loop()
{
  double k, e;
  k = referencia();
  distancia();
  e = k-d;
  Serial.print("Error: ");
  Serial.println(e);
  delay(1);
  conjuntos_difusos(e, k);
  antecedente();
  defusificar();
  delay(4);
  pwm();
}

void distancia(){
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);          //Enviamos un pulso de 10us
  digitalWrite(trigger, LOW);
  t = pulseIn(echo, HIGH); //obtenemos el ancho del pulso
  d = (10*t)/59;             //escalamos el tiempo a una distancia en mm
  if(d>400) d=400;
}

double referencia(){
  int lect = analogRead(ref);
  double dif = (0.09775)*lect+80;
  return dif;
}

void conjuntos_difusos(double e, double k){
  eng = gamma_inv(e, 0.004, -120);
  enm = gaussiana(e, 0.001, -75);
  eop = gaussiana(e, 0.003, 0);
  epm = gaussiana(e, 0.001, 75);
  epg = gamma(e, 0.001, 110);
  Ap = exp(-0.0001*(e-k)*(e-k));
  if (false){
    imprimir_conjuntos();
  }
}

void imprimir_conjuntos(){
  Serial.print("Error negativo grande= ");
  Serial.println(eng);
  Serial.print("Error negativo mediano= ");
  Serial.println(enm);
  Serial.print("Error optimo= ");
  Serial.println(eop);
  Serial.print("Error positivo mediano= ");
  Serial.println(epm);
  Serial.print("Error positivo grande= ");
  Serial.println(epg);
  delay(1);
}

double gaussiana(double e,double k, int m){
  double miu = 0.0;
  miu = exp(-k*(e-m)*(e-m));
  return miu;
}

double gamma(double e,double k, int a){
  double miu = 0.0;
  if(e>=a){
    miu = 1-exp(-k*(d-a)*(d-a));
  }
  else{
    miu = 0.0;
  }
  return miu;
}

double gamma_inv(double e,double k, int a){
  double miu = 0.0;
  if(e<=a){
    miu = 1-exp(-k*(e-a)*(e-a));
  }
  else{
    miu = 0.0;
  }
  return miu;
}

void antecedente(){
  r[0] = mini(Ap, eng);
  r[1] = mini(Ap, enm);
  r[2] = mini(Ap, enm);
  r[3] = mini(Ap, eop);
  r[4] = mini(Ap, eop);
  r[5] = mini(Ap, epm);
  r[6] = mini(Ap, epm);
  r[7] = mini(Ap, epg);
}

void defusificar(){
  y = (P*(r[0]+r[1])+(P-12.5)*(r[2]+r[3])+(P-31)*(r[4]+r[5])+(P-52.5)*(r[6]+r[7]))/(r[0]+r[1]+r[2]+r[3]+r[4]+r[5]+r[6]+r[7]);
}

void pwm(){
  int tiempo = 0; //Tiempo de encendido del motor
  double  conv = 0.0;
  conv = (y*periodo)/100; //Conversion de potencia a tiempo
  tiempo = (int) conv;  
  digitalWrite(motor, HIGH);
  delay(tiempo);
  digitalWrite(motor, LOW);
  delay(periodo-tiempo);
}

double mini(double a, double b){
  if(a>b) {
    return b;
  }
  else {
    return a;
  }
}
  