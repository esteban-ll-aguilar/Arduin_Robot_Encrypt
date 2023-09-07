// Autor: Esteban Leon Aguilar  ft. - Santiago Tamayo && - Cristian Robles

#include <LiquidCrystal_I2C.h>
#include <PS2Keyboard.h>
#include <Wire.h>
/////////////////////// COMUNICACION CON EL TECLADO ///////////////////////

LiquidCrystal_I2C lcd(0x27, 16, 2);
const int DataPin = 2;
const int IRQpin = 3;
PS2Keyboard keyboard;

/////////////////////// API ///////////////////////
// OPCIONES Y DECORACIONES Y MENSAJES
void animacionForanix();
void animacionDePalabras(const char *texto, int posX, int posY);
void mensajeBienvenida();
void mostrarMenu();
void mostrarLasOpcionesDisponibles();
void opcion1Encriptado();
void opcion2Desencriptado();
void mensajeOpcionInvalida();
void mensajePalabraExedeLimites();
void mensajeOpcionSeleccionada();
void mensajeFinal();
bool laLetraIngresadaEstaPermitida(char c);
void ProcesadoDeLaPalabra(const int llave[3][3], bool estaEncriptando);
void simularReseteoPrograma();

//PARTES DEL ENCRIPTADO
int obtenerValorDeCadaCaracter(char letra);
void llenarMatriz(const char *palabra, int matriz[3][5]);
void multiplicarMatrices(const int MatrizPalabra[3][5], const int MatrizLlave[3][3], int resultado[3][5]);
void modulo28Matriz(int resultado[3][5]);
void convertirMatrizAFila(const int matriz[3][5], int fila[15]);
void reemplazarLosValoresPorComparacion(int fila[15]);
void presentarEnPantalla(const int fila_resultado[15]); // Nueva funcion para mostrar el resultado en la pantalla LCD
void mantenerPalabraEnPantalla();
//FUNCION DEL ENCRIPTADO
void P_Encrip_Desencript(const int llave[3][3], char palabra[16], bool estaEncriptando);

/////////////////////// INICIALIZAR EL PROGRAMA ///////////////////////
void setup() {
  lcd.init();
  lcd.clear();
  lcd.backlight(); /*Encendemos Pantalla*/
  Serial.begin(9600);
  animacionForanix();
  keyboard.begin(2, 3);
}

/////////////////////// FUNCION LOOP ///////////////////////
void loop() {
  mostrarMenu();                          /*Mostramos Menu*/
  if (keyboard.available()) {
    char c = keyboard.read();             /*Leeremos lo que nos es ingresado en el teclado*/
    switch (c){
    case PS2_ESC:                         /*Si aplastamos "ESC"*/
        simularReseteoPrograma();         
        mostrarMenu();                    
        break;
    case PS2_ENTER:                        /*Si aplastamos "ENTER"*/
        mostrarLasOpcionesDisponibles();       
        while (!keyboard.available()) {
        }
        c = keyboard.read();              /*Leemos nuevamente lo que se esta ingresando*/
        lcd.print(c);                 
        switch (c){                         
        case '1':                         /*Si la opcion es 1 llamamos al proceso de encriptado*/
            opcion1Encriptado();
            break;
        case '2':                         /*Si la opcion es 2 llamamos al proceso de desencriptado*/
            opcion2Desencriptado(); 
            break;
        default:                          /*Si no es ninguna de las 2 mostramos un mensaje*/
          if(c != '1' && c != '2'){
          mensajeOpcionInvalida();
          }
          break;
        }
    case PS2_TAB:
      break;          
    default:                                /*Si no aplastamos alguna tecla deferente de "ESC && ENTER && TAB mostramos un mensaje"*/
        if(c != PS2_ESC && c != PS2_ENTER && c != PS2_TAB){
        mensajeOpcionInvalida();
        }
      break;
    }
  }
}

////////////////////////////////////////////// PROCESADO DE LA PALABRA //////////////////////////////////////////////

void ProcesadoDeLaPalabra(const int llave[3][3], bool estaEncriptando) {
  char palabra[17];
  int i = 0;
  while (true) {                                /*Ejecucion permanente*/
    if (keyboard.available()) {
      char c = keyboard.read();                 /*Leeremos lo que se va a ingresar*/
      if (c == PS2_ENTER) { 
        palabra[i] = '\0';                      /*En caso de ser la tecla ENTER damos fin a la palabra que ingresamos*/                    
        break;
      } else if (c == PS2_DELETE) {
        if (i > 0) {
          i--;
          lcd.setCursor(i, 1);                  /*En caso de ser la tecla "DETELE" movemos a la pocision anterior*/
          lcd.print(" ");                       /*Imprimimos un espacio en blanco y regresamos al misma pocision anterior*/
          lcd.setCursor(i, 1);
        }
      } else if (i < 16) {
        if (laLetraIngresadaEstaPermitida(c)) { /* Verificar si el caracter ingresado es una letra, espacio o guion*/
          palabra[i] = c;                       /*En cada pocision asignamos el caracter leido*/
          lcd.print(c);                         /*Mostrar el caracter ingresado en la pantalla LCD*/
          i++;
        }
      }
    }
  }
  int longitud = strlen(palabra);
  if (longitud >= 3 && longitud <= 15) {       /*Si la longitud esta dentro de los limites, llamamos al proceso de Encript y Desencript*/
    P_Encrip_Desencript(llave, palabra, estaEncriptando); 
  } else {
    mensajePalabraExedeLimites();              /*Si no esta dentro de los limites mostramos un mensaje*/
  }
}

////////////////////////////////////////////// ENCRIPTADO Y DESENCRIPTADO //////////////////////////////////////////////
void P_Encrip_Desencript(const int llave[3][3], char palabra[16], bool estaEncriptando) {
  int matriz[3][5];
  llenarMatriz(palabra, matriz);                       /*Llenamos la matriz con los valores correspondientes*/

  int resultado[3][5];
  multiplicarMatrices(matriz, llave, resultado);            
  modulo28Matriz(resultado);

  int fila_resultado[15];
  convertirMatrizAFila(resultado, fila_resultado);
  reemplazarLosValoresPorComparacion(fila_resultado);
  
  lcd.setCursor(0, 0);
  mensajeOpcionSeleccionada(estaEncriptando);          /*Mostramos un mensaje de la opcion que selecciono*/            
  presentarEnPantalla(fila_resultado);                 /*Presentamos nuestro resultado y lo mantenemos en pantalla hasta que el usuario quiera*/
  mantenerPalabraEnPantalla();
  mensajeFinal();                                             
}


////////////////////////////////////////////// MENSAJES Y ANIMACIONES //////////////////////////////////////////////

// ANIMACION FORANIX 
void animacionForanix() {   
  delay(100);
  lcd.setCursor(0, 0);
  animacionDePalabras("---  FORANIX ---", 0, 0); //Posicion (2, 0)
  delay(150);
  lcd.setCursor(0, 1);
  lcd.print("----------------");
  delay(150);
  animacionDePalabras(" Robot Cifrador ", 0, 1); //Posicion (2, 0)
  delay(3000);
  lcd.clear();
}
// ANIMACION DE LA PRESENTACION 
void animacionDePalabras(const char *texto, int posX, int posY) {
  int longitudTexto = strlen(texto);
  lcd.setCursor(posX, posY);                        /*Simplemente ingresamos El texto y su ubicacion*/
  for (int i = 0; i < longitudTexto; i++) {         /*Imprimimos la palabra cada cierto tiempo*/
    lcd.print(texto[i]);
    delay(180); // Veocidad de animacion
  }
  delay(500);
}
// BIENVENIDA 
void mensajeBienvenida() {
  delay(1500);
  lcd.clear();
  lcd.print("¡Bienvenido");
  lcd.setCursor(0, 1);
  lcd.print("    de nuevo!!!");
  delay(2000);
}
// MENU 
void mostrarMenu() {
  static bool MostrarMenu = false;
  if (!MostrarMenu) {
    mensajeBienvenida();
    lcd.clear();
    lcd.print("Press| 1~Encrypt");
    lcd.setCursor(0, 1);
    lcd.print("[INT]| 2~Decrypt");
    MostrarMenu = true;
  }
}
// RESETEO PROGRAMA 
void simularReseteoPrograma() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Resetting...");
  delay(1000);             
  animacionForanix();
}
// MOSTRAR SELECCIONAR OPCIONES
void mostrarLasOpcionesDisponibles(){
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Select option:");
  lcd.setCursor(0, 1);
  lcd.print(" 1-Encr  2-Decr ");
  lcd.setCursor(0, 16);
}
// PALABRA CON LIMITES EXEDIDOS 
void mensajePalabraExedeLimites(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Error!");
  lcd.setCursor(0, 1);
  lcd.print("Caract. >2 y <16");
  delay(2000); 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Presione Enter: ");
}
// MENSAJE OPCION BOOLEANA
void mensajeOpcionSeleccionada(bool estaEncriptando){;
  if (estaEncriptando) {
    lcd.clear();                     /*Para entenderlo deriamos saber el valor de nuestro "estaEncriptado"*/
    lcd.print("Encriptado: ");
  } else {
    lcd.clear();
    lcd.print("Desencriptado: ");
  }
}
// MENSAJE FINAL
void mensajeFinal(){;
  delay(500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Reset [ESC]");
  lcd.setCursor(0, 1);
  lcd.print("  Opt. [ENTER]");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Foranix X");
  lcd.setCursor(0, 1);
  lcd.print("          U.N.L");
}
// MANTENER PALABRA EN PANTALLA
void mantenerPalabraEnPantalla(){
  while(true){                   /*Ejecucion permanente*/
    if (keyboard.available()) {
      char c = keyboard.read();  /*Solo si la tecla ingresada es "TAB" el bucle se rompera*/
      if (c == PS2_TAB) {
        break;
      } else {
      }
    }  
  }
}

////////////////////////////////////////////// OPCIONES //////////////////////////////////////////////
/* En las opciones declararemos nuestra llave de encriptado y desencriptado junto con el valor booleano
  Llamaremos a nuestro proceso de  la palabra*/
// OPCION 1 
void opcion1Encriptado() {
  int llaveEncript[3][3] = {
    {2, 5, 9},
    {2, 5, 10},
    {1, 2, 8}
  };                                    
  lcd.clear();
  bool estaEncriptando = true;
  lcd.print("Word to Encrypt: ");
  lcd.setCursor(0, 1);
  ProcesadoDeLaPalabra(llaveEncript, estaEncriptando);
}
// OPCION 2
void opcion2Desencriptado() {
  int llaveDesencript[3][3] = {
    {20, 6, 5},
    {22, 7, 26},
    {27, 1, 0}
  };
  lcd.clear();
  bool estaEncriptando = false;
  lcd.print("Word to Decrypt: ");
  lcd.setCursor(0, 1);
  ProcesadoDeLaPalabra(llaveDesencript, estaEncriptando);
}
// OPCION INVALIDA
/*Si la opcion es invalida mostramos un mensaje*/
void mensajeOpcionInvalida(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Invalid option:");
  lcd.setCursor(0, 1);
  lcd.print("Press Enter|INT|");
}

///////////////////////////////////////// PROCESOS DE LA PALABRA Y ENCRIPTADO_DESENCERIPTADO /////////////////////////////////////////
// VERIFICAR SI LA LETRA INGRESADA PERTENESE 
/*Retorna los valores solo si cumple con los criterios dados*/
bool laLetraIngresadaEstaPermitida(char c) {
  return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == ' ' || c == '-');
}
// OBRENER VALORES POR CADA CARACTER
/*Primero convertimos nuestra palabra asignada en este caso como(letra) a mayuscula 
  Buscamos la posicion de cada letra en nuestra palabra en la constante "letras" con strchr(devuelve un puntero, sino, devuelve nulo)
  restamos las direcciones de memoria, ya que la distancia entre ellas nos da como resultado el indice del valor en la palabra
  retornamos el valor de la fila en la pocision del indice*/
int obtenerValorDeCadaCaracter(char letra) {
    static const char *letras = " ABCDEFGHIJKLMNOPQRSTUVWXYZ-";
    static const int valor_letra[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,27};

    letra = toupper(letra);
    const char *pos = strchr(letras, letra);
    if (pos != NULL) {
        int indice = pos - letras;
        return valor_letra[indice];

    } else {
        return -1; // Valor de letra no valida
    }
}
// LLENAR MATRIZ 
/*Llenamos a nuestra matriz de 0 y despues de acuerdo a la longitud de nuestra palabra
creamos una variaable tipo caracter llamada "letra" con lo cual su valor sera igual a la pocision en la palabra
con esto creamos una nueva variable en donde remplazaremos en caracter de la letra por su respectivo valor,
seguido de esto dicho valor se lo asignaremos a la pocicion de matriz*/
void llenarMatriz(const char *palabra, int matriz[3][5]) {
    int longitud = strlen(palabra);
    int contador = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 5; j++) {
            matriz[i][j] = 0;
        }
    }
    // Llenar matriz con los valores correspondientes
    for (int j = 0; j < 5; j++) {
        for (int i = 0; i < 3; i++) {
            if (contador < longitud) {
                char letra = palabra[contador];
                int valor = obtenerValorDeCadaCaracter(letra);
                matriz[i][j] = valor;
                contador++;
            } else {
                break;
            }
        }
    }
}

// MULTIPLICAR MATRIZ 
void multiplicarMatrices(const int MatrizPalabra[3][5], const int MatrizLlave[3][3], int resultado[3][5]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 5; j++) {
            resultado[i][j] = 0;
            for (int k = 0; k < 3; k++) {
                resultado[i][j] += MatrizLlave[i][k] * MatrizPalabra[k][j];
            }
        }
    }
}

// SACAR MODULO MATRIZ 
void modulo28Matriz(int resultado[3][5]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 5; j++) {
            resultado[i][j] = resultado[i][j] % 28;
        }
    }
}

// CONVERTIR MATRIZ A FILA 
void convertirMatrizAFila(const int matriz[3][5], int fila[15]) {
    int contador = 0;
    for (int j = 0; j < 5; j++) {
        for (int i = 0; i < 3; i++) {
            fila[contador] = matriz[i][j];
            contador++;
        }
    }
}

// REEMPLAZAR LA FILA DE NUMEROS A LETRAS 
/*En este procedimiento, creamos una variable "valor" que es igual al valor en la posicion de nuestra fila.
  Si nuestro valor está dentro del rango, el resultado de nuestra fila en cada posicion será igual 
  al caracter que está en la posicion ("valor") de la constante "letras"*/
void reemplazarLosValoresPorComparacion(int fila[15]) {
  static const char letras[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ-"; 
    for (int i = 0; i < 15; i++) {
        int valor = fila[i];
        if (valor >= 0 && valor <= 27) {
            fila[i] = letras[valor];
        } else {
            fila[i] = ' '; // Mostrar espacios en blanco para los valores no validos (fuera del rango 0-27)
        }
    }
}

// PRESENTAR EN PANTALLA 
void presentarEnPantalla(const int fila_resultado[15]) {
  lcd.setCursor(0, 1);
  for (int i = 0; i < 15; i++) {
    if (fila_resultado[i] != '\0') {
      lcd.print((char)fila_resultado[i]); // Mostrar el resultado encriptado en la pantalla LCD
    }
  } 
}