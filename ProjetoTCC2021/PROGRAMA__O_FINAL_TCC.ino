#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <SPI.h>


#define      n     30 //número de pontos da média móvel 

#define endereco  0x27 
#define colunas   16
#define linhas    2

File myFile;
boolean acionado = false;
static long int timerfinal = 0;
#define tempo                              1000
 
int pinoSS = 10; 

int z = 0;

LiquidCrystal_I2C lcd(endereco, colunas, linhas);

float moving_average();       //Função para filtro de média móvel
int       contaPulso, t;          //recebe o valor do sensor
float       mediaPulso;          //recebe o valor original filtrado
float       numbers[n];        //vetor com os valores para média móvel
float vazao;

void setup()
{
  Serial.begin(9600);    //serial inicia em 9600 baud rate
  pinMode(2, INPUT);   //configura entrada para o sensor
  attachInterrupt(0, incpulso, RISING); //Configura o pino 2(Interrupção 0) para trabalhar como interrupção
  pinMode(3,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(3), interruptbotao, FALLING);
  Serial.println("\n\nInicio\n\n"); //Imprime Inicio na serial

  
  
  pinMode(pinoSS, OUTPUT); // Declara pinoSS como saída
  if (SD.begin()) { // Inicializa o SD Card
  Serial.println("SD Card pronto para uso."); // Imprime na tela
  myFile = SD.open("VAZAO.txt", FILE_WRITE); // Cria / Abre arquivo .txt
  }
  else {
  Serial.println("Falha na inicialização do SD Card.");
  return;
  }

  
  lcd.init(); // INICIA A COMUNICAÇÃO COM O DISPLAY
  lcd.backlight(); // LIGA A ILUMINAÇÃO DO DISPLAY
  lcd.clear(); // LIMPA O DISPLAY

//  
   
} 


// ===============================================================================
// --- Loop Infinito ---
void loop()
{
     
  contaPulso = 0;   //Zera a variável para contar os giros por segundos
  sei();      //Habilita interrupção
  delay (1000); //Aguarda 1 segundo
  
      lcd.print("Vazao:");
      lcd.setCursor(0,1);
      lcd.print("Media:");
      lcd.setCursor(7,0);
      lcd.print(vazao);
      lcd.setCursor(11,0);
      lcd.print(" L/m");
      lcd.setCursor(7,1);
      lcd.print(mediaPulso);
      lcd.setCursor(11,1);
      lcd.print(" L/m ");

      if(t>= 10){
      lcd.setCursor(15,0);
      lcd.print(char(255));
      lcd.setCursor(15,1);
      lcd.print(char(255));
      }
      

  cli();      //Desabilita interrupção
  
   vazao = (contaPulso/5.5) ;
   mediaPulso = moving_average();

    

   Serial.print(vazao);
   Serial.print(" L/m    ");
   Serial.print(" ");
   Serial.print(mediaPulso);
   Serial.println("L/m");


   if (t >= 10){
    Serial.print("Pronto para salvar -->"); 
   }
   
 t = t + 1;    
   
 delay(1000); 
} //end loop 


  



// ===============================================================================
// --- Desenvolvimento da Função ---
float moving_average()
{

   //desloca os elementos do vetor de média móvel
   for(int i= n-1; i>0; i--) numbers[i] = numbers[i-1];

   numbers[0] = vazao; //posição inicial do vetor recebe a leitura original

   float acc = 0;          //acumulador para somar os pontos da média móvel

   for(int i=0; i<n; i++) acc += numbers[i]; //faz a somatória do número de pontos


   return acc/n;  //retorna a média móvel
   

 
} //end moving_average
void incpulso ()
  { 
  contaPulso++; 
  }

void interruptbotao(){
  //VERIFICA SE NÃO ESTA SALVANDO
 
  
if (acionado == false){
  if (t >=10){
    z = z + 1;
    myFile = SD.open("VAZAO.txt", FILE_WRITE); // Cria / Abre arquivo .txt
    if(myFile){
     myFile.print(z);
     myFile.print(" > ");
     myFile.println(mediaPulso);
      t = 0;
      myFile.close();      
    }
      }
   else{
    Serial.println("Erro ao Abrir Arquivo .txt");
    return;}
   }
  }    
     
  
