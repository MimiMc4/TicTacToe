/**
 * @file clientevocalesTCP.c
 *
 * Programa *clientevocalesTCP* que envía cadenas de texto a un servidor.
 *
 * Uso: clientevocalesTCP servidor puerto
 *
 * El programa crea un socket TCP y lo conecta al servidor y puerto especificado.
 * A través del socket envía cadenas de caracteres hasta llegar a fin de fichero
 * (Control+d para provocarlo desde la entrada estándar).
 * Finalmente, espera como respuesta el número total de vocales en las
 * cadenas enviadas e imprime dicho valor por pantalla.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>


#include "comun.h"

#include <iostream>
#include <string>
using namespace std;

// definición de constantes
#define MAX_BUFF_SIZE 1000 ///< Tamaño del buffer para las cadenas de texto.

/**
 * Función que crea la conexión y se conecta al servidor.
 *
 * @param servinfo Estructura de dirección a la que conectarse.
 * @param f_verbose Flag.
 * @return Descriptor de socket.
 */
int initsocket(struct addrinfo *servinfo, char f_verbose)
{
    int sock = -1;
    int numdir = 1;

    while (servinfo != NULL && sock < 0)
    {   // bucle que recorre la lista de direcciones
        // crea un extremo de la comunicación y devuelve un descriptor
        if (f_verbose)
        {
            printf("Creando el socket (socket)... ");
            fflush(stdout);
        }
        sock = socket(servinfo->ai_family,servinfo->ai_socktype ,servinfo->ai_protocol );
        if (sock < 0)
        {
            perror("Error en la llamada socket: No se pudo crear el socket");
            // muestra por pantalla el valor de la cadena suministrada por el
            // programador, dos puntos y un mensaje de error que detalla la
            // causa del error cometido
        }
        else
        {   // socket creado correctamente
            if (f_verbose) printf("hecho\n");

            // inicia una conexión en el socket:
            if (f_verbose)
            {
                printf("Estableciendo la comunicación a través del socket (connect)... ");
                fflush(stdout);
            }
            if (connect(sock, servinfo->ai_addr, servinfo->ai_addrlen) < 0)
            {
                perror("Error en la llamada connect: No se pudo conectar con el destino");
                close(sock);
                sock = -1;
            }
            else
                if (f_verbose) printf("hecho\n");
        }

        // "avanzamos" a la siguiente estructura de direccion
        servinfo = servinfo->ai_next;
        numdir++;
    }

    if (sock < 0)
    {
        perror("No se ha podido establecer la comunicación");
        exit(1);
    }

    return sock;
}

bool checkFin(char tablero[]){
    return tablero[9] == '1';
}

/*
void printBoard(){
    cout << endl;
}

void updateBoard(const char tablero[]){
    cout << tablero[0] << tablero[1] << tablero [2] << endl;
    cout << tablero[3] << tablero[4] << tablero [5] << endl;
    cout << tablero[6] << tablero[7] << tablero [8] << endl;
    cout << endl;
}

*/


//=====================================================================//
//                              ANSILIB                                //
//=====================================================================//

//Es un endline que dispone de un margen, para usarse antes debes llamar a setCursor()
//para actualizar el margen desde la pseudo esquina donde se setea el cursor
string endl_q = "\n\033[6D";
//Esta funcion le da el valor necesario (APLICA UN MARGEN)
void endl_qSet(int col){
    if(col!=0){
        endl_q = "\n\033[" + to_string(col) +"D";   
    }
    
}

//Detiene la ejecución hasta que se presione <ENTER>
void waitEnterKey(){
    cout << "\033[s";
        getchar();
    cout << "\033[u";
}

//Mueve el cursor a la posición (0,0)
//siendo (0,0) la esquina superior izquierda de la pantalla y los ejes (col,fil)
//Además, actualiza el valor de la pseudo esquina que usa endl_ si le pasas la opcion
//(true) como tercer parametro opcional
void resetCursor(bool update = true){
    cout << "\033[H";
    if (update){
        endl_qSet(0);
    }
}

//Mueve el cursor a la posición (0,0) o a la posición determinada por (x,y)
//siendo (0,0) la esquina superior izquierda de la pantalla y los ejes (col,fil)
//Además, actualiza el valor de la pseudo esquina que usa endl_ salvo si le pasas la opcion
//(x,y,false) como tercer parametro opcional
void setCursor(const unsigned x = 0, const unsigned y = 0, bool update = true){
    cout << "\033[H";
    if(x!=0){cout << "\033[" + to_string(x) + "C";}
    if(y!=0){cout << "\033[" + to_string(y) + "B";}
    if (update){
        endl_qSet(x);
    }
}

//Mueve el cursor a la posición x en el eje y a la posicion y desde el punto actual 
void moveCursor(const int x = 0, const int y = 0){
    cout << "\033[H";
    if(x<0){
        cout << "\033[" + to_string(x) + "D";
    }else if(x>0){
        cout << "\033[" + to_string(x) + "C";
    }
    if(y<0){
        cout << "\033[" + to_string(y) + "A";
    }else if(y>0){
        cout << "\033[" + to_string(y) + "B";
    }
}

//Guarda la posición del cursor para revisitarla en un futuro con loadCursor
void saveCursor(){
    cout << "\033[7";
}

//Carga la posición del cursor para revisitarla en un futuro con loadCursor
void loadCursor(){
    cout << "\033[8";
}

//Esconde o muestra el cursor
void setCursorVisibility(const bool hide){
    if(hide){
        cout << "\033[?25l";
    }else{
        cout << "\033[?25h";
    }
}

//Limpia la pantalla actual y pone el cursor al inicio, con la opcion (false) mantiene su posicion
void cls(bool doresetCursor = true){
    if(!doresetCursor){cout << "\033[s";}
    cout << "\033[2J";
    resetCursor();
    cout << "\033[1J";
    if(!doresetCursor){cout << "\033[u";}
}

//Limpia la linea actual y pone el cursor al inicio, con la opcion (false) mantiene su posicion
void cll(bool resetCursor = true){
    if(!resetCursor){cout << "\033[s";}
    cout << "\033[K" << "\r";
    if(!resetCursor){cout << "\033[u";}
}

//==============================================================================//
//                              CODIGO TIC TAC TOE                              //
//==============================================================================//

void place_x(int pos, string skin){
    int x_cord = pos%3, y_cord = pos/3;
    setCursor(8*x_cord+1,4*y_cord);
    cout << " \\ / ";
    setCursor(8*x_cord+1,4*y_cord+1);
    cout << "  X  ";
    setCursor(8*x_cord+1,4*y_cord+2);
    cout << " / \\ " << endl;
}

void place_x(int pos, string skin, bool colored){
    int x_cord = pos%3, y_cord = pos/3;
    if(colored){cout << "\033[1;31m";}
    setCursor(8*x_cord+1,4*y_cord);
    cout << " \\ / ";
    setCursor(8*x_cord+1,4*y_cord+1);
    cout << "  X  ";
    setCursor(8*x_cord+1,4*y_cord+2);
    cout << " / \\ " << endl;
    if(colored){cout << "\033[0m";}
}


void place_o(int pos, string skin, bool colored){
    
    int x_cord = pos%3, y_cord = pos/3;

    setCursor(8*x_cord+1,4*y_cord);
    if(colored){cout << "\033[1;32m";}
    cout << " .-. ";
    setCursor(8*x_cord+1,4*y_cord+1);
    cout << "|   |";
    setCursor(8*x_cord+1,4*y_cord+2);
    cout << " *-* "  << endl;
    if(colored){cout << "\033[0m";}
}

void ticTacToe_head(){
cout <<"\033[1m";
cout <<"    _____ _     _____         _____             " << endl;
cout <<"   |_   _|_|___|_   _|___ ___|_   _|___ ___     " << endl;
cout <<"     | | | |  _| | | | . |  _| | | | . | -_|    " << endl;
cout <<"     |_| |_|___| |_| |_|_|___| |_| |___|___|    " << endl;
cout <<".-============================================-." << endl;
cout <<"\033[0m";                                                                                         
}

void printBoard(){
cout << "\033[30m";
cout << "       |       |       " << endl;
cout << "   1   |   2   |   3   " << endl;
cout << "       |       |       " << endl;
cout << "-------+-------+-------" << endl;
cout << "       |       |       " << endl;
cout << "   4   |   5   |   6   " << endl;
cout << "       |       |       " << endl;
cout << "-------+-------+-------" << endl;
cout << "       |       |       " << endl;
cout << "   7   |   8   |   9   " << endl;
cout << "       |       |       " << endl;
cout << "\033[0m";
}

// determina qué línea del tablero ofrece la victoria
void colorWin(char t[], bool c[]) {
    // Inicialización de las posiciones de la línea ganadora
    for (int i = 0; i < 9; i++) {
        c[i] = false; // Ninguna posición está ganada inicialmente
    }

    if (t[0] == t[1] && t[1] == t[2] && t[0] != '0') {
        // Primera línea horizontal
        c[0] = 1;
        c[1] = 1;
        c[2] = 1;
    } else if (t[3] == t[4] && t[4] == t[5] && t[3] != '0') {
        // Segunda línea horizontal
        c[3] = 1;
        c[4] = 1;
        c[5] = 1;
    } else if (t[6] == t[7] && t[7] == t[8] && t[6] != '0') {
        // Tercera línea horizontal
        c[6] = 1;
        c[7] = 1;
        c[8] = 1;
    } else if (t[0] == t[3] && t[3] == t[6] && t[0] != '0') {
        // Primera línea vertical
        c[0] = 1;
        c[3] = 1;
        c[6] = 1;
    } else if (t[1] == t[4] && t[4] == t[7] && t[1] != '0') {
        // Segunda línea vertical
        c[1] = 1;
        c[4] = 1;
        c[7] = 1;
    } else if (t[2] == t[5] && t[5] == t[8] && t[2] != '0') {
        // Tercera línea vertical
        c[2] = 1;
        c[5] = 1;
        c[8] = 1;
    } else if (t[0] == t[4] && t[4] == t[8] && t[0] != '0') {
        // Diagonal arriba a abajo
        c[0] = 1;
        c[4] = 1;
        c[8] = 1;
    } else if (t[2] == t[4] && t[4] == t[6] && t[2] != '0') {
        // Diagonal abajo a arriba
        c[2] = 1;
        c[4] = 1;
        c[6] = 1;
    }
}

//actualiza el tablero incluyendo victorias
void updateBoard(char tablero[], string oskin = " ",string xskin = "X" ){
    //maneja impresion de tablero de victoria
    bool c[9];
    colorWin(tablero,c);
    //imprime tablero
    for(int i = 0; i < 9; i++){
        if(tablero[i] == '1'){
            place_x(i,xskin,c[i]);
        }else if(tablero[i] == '2'){
            place_o(i,oskin,c[i]);
        }
    }
}




/**
 * Programa principal
 *
 * @param argc Número de argumentos usados en la línea de comandos.
 * @param argv Vector de punteros a cadenas de caracteres. argv[0]
 *             apunta al nombre del programa, argv[1] al primer
 *             argumento y así sucesivamente.
 * @return 0 si todo ha ido bien, distinto de 0 si hay error.
 */
int main(int argc, char * argv[])
{
    //variables del tres en raya
    char pnum = '0';
    char gamemsg = '0';
    int gamenum = 0;
    char tablero[10] = {'0','0','0','0','0','0','0','0','0','0'};
    string myskin = "0";
    string yourskin = "0";
    string skX = "x", skO =" ";

    // declaración de variables propias del programa principal (locales a main)
    char f_verbose = 0;         // flag, 1: imprimir información extra
    struct addrinfo * servinfo; // puntero a estructura de dirección destino
    int sock;                   // descriptor del socket
    char msg[MAX_BUFF_SIZE];    // buffer donde almacenar datos para enviar

    // verificación del número de parámetros:
    if (argc != 3)
    {
        printf("Número de parámetros incorrecto \n");
        printf("Uso: %s servidor puerto/servicio\n", argv[0]);
        exit(1); // finaliza el programa indicando salida incorrecta (1)
    }

    // obtiene estructura de direccion
    servinfo = obtener_struct_direccion(argv[1], argv[2], f_verbose);

    // crea un extremo de la comunicación con la primera de las
    // direcciones de servinfo e inicia la conexión con el servidor.
    // Devuelve el descriptor del socket
    sock = initsocket(servinfo, f_verbose);

    // hay que liberar la memoria dinámica usada para la dirección
    // cuando ya no se necesite
    freeaddrinfo(servinfo);
    servinfo = NULL;


    ////////////////////////////////////////////////////////////////////



    cls();
    ticTacToe_head();

    //bucle de espera conexión (aqui es solo un for pocho)
    setCursor(1,6);
    cout << "Esperando a un adversario..." << endl;

    if ((recv(sock,&pnum,1,0)) < 0)
    {
        perror("Error de lectura en el socket");
        exit(1);
    }   

    

    cout << "Eres el jugador " << pnum << endl;
    cout << "Pulsa enter para continuar...";
    cout.flush();
    waitEnterKey();

    if ((recv(sock,tablero,10,0)) < 0)
    {
        perror("Error de lectura en el socket");
        exit(1);
    }
    cls();
    printBoard();
    updateBoard(tablero);

    while (!checkFin(tablero))
    {
        if(pnum == '1'){
            do{
                setCursor(0,13);
                cll();
                cout << "[1-9]: ";
                cout.flush();
                cin >> gamemsg;
                gamenum = gamemsg - '0';
                if(gamenum <= 0 || gamenum > 9 || tablero[gamenum-1] - '0' != 0)
                    cout << "Indice inválido" << endl;
            }while(gamenum <= 0 || gamenum > 9 || tablero[gamenum-1] - '0' != 0);
            // envía jugada
            if ((send(sock, &gamemsg, 1, 0)) < 0)
            {
                perror("Error de escritura en el socket");
                exit(1);
            }
            //recibe tablero
            if ((recv(sock,tablero,10,0)) < 0)
            {
                perror("Error de lectura en el socket");
                exit(1);
            }   
            if(checkFin(tablero)) break;
            updateBoard(tablero);

            //recibe tablero despues de que mueva el oponente
            if ((recv(sock,tablero,10,0)) < 0)
            {
                perror("Error de lectura en el socket");
                exit(1);
            }   
            if(checkFin(tablero)) break;
            updateBoard(tablero);

        }else if(pnum == '2'){
            setCursor(0,13);
            cout << "[1-9]: ";
            cout.flush();
            //recibe tablero despues de que mueva el oponente
            if ((recv(sock,tablero,10,0)) < 0)
            {
                perror("Error de lectura en el socket");
                exit(1);
            }   
            if(checkFin(tablero)) break; 
            updateBoard(tablero);

            do{
                setCursor(0,13);
                cll();
                cout << "[1-9]: ";
                cout.flush();
                cin >> gamemsg;
                gamenum = gamemsg - '0';
                if(gamenum <= 0 || gamenum > 9 || tablero[gamenum-1] - '0' != 0)
                    cout << "Indice inválido" << endl;
            }while(gamenum <= 0 || gamenum > 9 || tablero[gamenum-1] - '0'  != 0);



            // envía jugada
            if ((send(sock, &gamemsg, 1, 0)) < 0)
            {
                perror("Error de escritura en el socket");
                exit(1);
            }

            //recibe tablero
            if ((recv(sock,tablero,10,0)) < 0)
            {
                perror("Error de lectura en el socket");
                exit(1);
            }   
            if(checkFin(tablero)) break;
            updateBoard(tablero);
        }
    }

    updateBoard(tablero);

    // ya no queremos enviar más, cerramos para escritura
    if (f_verbose)
    {
        printf("Cerrando el socket para escritura...");
        fflush(stdout);
    }
    if (shutdown(sock, 1) < 0)
    {
        perror("Error al cerrar el socket para escritura");
        exit(1);
    }

    // el servidor verá la conexión cerrada y enviará el número de vocales
    if (f_verbose)
    {
        printf("hecho\nEsperando respuesta del servidor...");
        fflush(stdout);
    }

    // cierra la conexión del socket
    if (close(sock) < 0)
    {
        perror("Error al cerrar el socket");
        exit(1);
    }
    else
    {
        if (f_verbose) printf("Socket cerrado\n");
    }

    setCursor(0,14);
    cout << "Presiona enter para acabar..." << endl;
    cout.flush();
    waitEnterKey();

    exit(0); // finaliza el programa indicando salida correcta (0)
}
