/**
 * @file servidorvocalesTCP.c
 *
 * Programa *servidorvocalesTCP* que cuenta vocales.
 *
 * Uso: servidorvocalesTCP puerto
 *
 * El programa crea un socket TCP en el puerto especificado, lo pone en modo
 * escucha y atiende consecutivamente a los clientes que se van conectando.
 * Para cada cliente, recibe cadenas de texto, cuenta las vocales contenidas
 * y al acabar todas las recepciones envÃ­a al cliente el nÃºmero total de
 * vocales contadas.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

#include "comun.h"

#include <iostream>

using namespace std;

// definiciÃ³n de constantes
#define BUFF_SIZE 1000 ///< TamaÃ±o del buffer para las cadenas de texto.


/**
 * FunciÃ³n que crea la conexiÃ³n y espera conexiones entrantes.
 *
 * @param servinfo Estructura de direcciÃ³n local.
 * @param f_verbose Flag.
 * @return Descriptor de socket.
*node */
int establecer_servicio(struct addrinfo *servinfo, char f_verbose)
{
    int sock = -1;


    // crea un extremo de la comunicaciÃ³n y devuelve un descriptor
    if (f_verbose)
    {
        printf("Creando el socket (socket)... ");
        fflush(stdout);
    }
    sock = socket(servinfo->ai_family ,servinfo->ai_socktype, servinfo->ai_protocol);
    if (sock < 0)
    {
        perror("Error en la llamada socket: No se pudo crear el socket");
        // muestra por pantalla el valor de la cadena suministrada por el
        // programador, dos puntos y un mensaje de error que detalla la causa
        // del error cometido
        exit(1);
    }
    if (f_verbose) printf("hecho\n");

    // asocia el socket con un puerto
    if (f_verbose)
    {
        printf("Asociando socket a puerto (bind)... ");
        fflush(stdout);
    }
    if (bind(sock, servinfo->ai_addr, servinfo->ai_addrlen) < 0)
    {
        perror("Error asociando el socket");
        exit(1);
    }
    if (f_verbose) printf("hecho\n");

    // espera conexiones en un socket
    if (f_verbose)
    {
        printf("Permitiendo conexiones entrantes (listen)... ");
        fflush(stdout);
    }
    listen(sock, 5);
    // 5 es el nÃºmero mÃ¡ximo de conexiones pendientes en algunos sistemas
    if (f_verbose) printf("hecho\n");

    return sock;
}

//determina si ha habido una victoria
bool hasWinned(char t[]){
    bool win = (t[0] == t[1] && t[1] == t[2] && t[0] != '0') || 
            (t[3] == t[4] && t[4] == t[5] && t[3] != '0') || 
            (t[6] == t[7] && t[7] == t[8] && t[6] != '0') || 
            (t[0] == t[3] && t[3] == t[6] && t[0] != '0') || 
            (t[1] == t[4] && t[4] == t[7] && t[1] != '0') || 
            (t[2] == t[5] && t[5] == t[8] && t[2] != '0') || 
            (t[0] == t[4] && t[4] == t[8] && t[0] != '0') || 
            (t[2] == t[4] && t[4] == t[6] && t[2] != '0');
	bool draw = true;
	for(int i = 0; i < 9 && draw; i++){
		if(t[i] == '0'){
			draw = false;
		}
	}
	return win || draw;
}
/**
 * Programa principal
 *
 * @param argc NÃºmero de argumentos usados en la lÃ­nea de comandos.
 * @param argv Vector de punteros a cadenas de caracteres. argv[0]
 *             apunta al nombre del programa, argv[1] al primer
 *             argumento y asÃ­ sucesivamente.
 * @return 0 si todo ha ido bien, distinto de 0 si hay error.
 */
int main(int argc, char * argv[])
{
    //Variables del tablero
    char tablero[10] = {'0','0','0','0','0','0','0','0','0','0'};
    char gamemsg;
    int nummsg;


    // declaraciÃ³n de variables propias del programa principal (locales a main)
    char f_verbose = 0;         // flag, 1: imprimir informaciÃ³n extra
    struct addrinfo * servinfo; // direcciÃ³n propia (servidor)
    int sock, conn[2];       // descriptores de socket
    char msg[BUFF_SIZE];        // espacio para almacenar los datos recibidos
    ssize_t readbytes[2];          // numero de bytes recibidos
    struct sockaddr_storage caddr[2]; // direcciÃ³n del cliente
    socklen_t clen;             // longitud de la direcciÃ³n

    // verificaciÃ³n del nÃºmero de parÃ¡metros:
    if (argc != 2)
    {
        printf("Numero de parametros incorrecto \n");
        printf("Uso: %s puerto\n", argv[0]);
        exit(1); // finaliza el programa indicando salida incorrecta (1)
    }

    // obtiene estructura de direccion
    servinfo = obtener_struct_direccion(NULL, argv[1], f_verbose);

    // crea un extremo de la comunicaciÃ³n. Devuelve el descriptor del socket
    sock = establecer_servicio(servinfo, f_verbose);

    // hay que liberar la memoria dinÃ¡mica usada para la direcciÃ³n
    // cuando ya no se necesite
    freeaddrinfo(servinfo);
    servinfo = NULL;
    // como ya se ha liberado ese bloque de memoria,
    // dejamos de apuntarlo para evitar acceder a ella por error.
    // Si referenciamos esta variable el programa abortarÃ¡ con
    // ERROR: SEGMENTATION FAULT

    // bucle infinito para atender conexiones una tras otra
        printf("\nEsperando conexion (pulsa <Ctrl+c> para finalizar la ejecucion)...\n");

        // acepta la conexiÃ³n
        clen = sizeof caddr[0];
        if ((conn[0] = accept(sock, (struct sockaddr *)&caddr[0], &clen)) < 0)
        {
            perror("Error al aceptar una nueva conexiÃ³n");
            exit(1);
        }
        
        /*
        // imprime la dirección obtenida
        printf("Aceptada conexiÃ³n con cliente:\n");
        printsockaddr(&caddr[0]);
        */

        // acepta la conexión
        clen = sizeof caddr;
        if ((conn[1] = accept(sock, (struct sockaddr *)&caddr[1], &clen)) < 0)
        {
            perror("Error al aceptar una nueva conexiÃ³n");
            exit(1);
        }
        
        // imprime la direcciÃ³n obtenida
        printf("Aceptada conexiÃ³n con cliente:\n");
        printsockaddr(&caddr[1]);

        //Mandamos el numero de jugador
        msg[0] = '1';
        if (send(conn[0], msg, 1,0) < 0)
        {
            perror("Error de escritura en el socket");
            exit(1);
        }

        msg[0] = '2';
        if (send(conn[1], msg, 1,0) < 0)
        {
            perror("Error de escritura en el socket");
            exit(1);
        }
        
        //Mandar primer tablero
        if (send(conn[0], tablero, 10,0) < 0)
        {
            perror("Error de escritura en el socket");
            exit(1);
        }
        if (send(conn[1], tablero, 10,0) < 0)
        {
            perror("Error de escritura en el socket");
            exit(1);
        }
        // bucle de juego
        int fin = 0;
        do {

            //Recibir input p1
            if ((readbytes[0] = recv(conn[0],&gamemsg,1,0)) < 0){
                perror("Error de lectura en el socket");
                exit(1);
            }
            cout << "Recibido: " << gamemsg << endl;
            nummsg = gamemsg - '0';
            tablero[nummsg-1] = '1';
            
            if(hasWinned(tablero)){
                tablero[9] = '1';
            }

            //Send tablero
            if (send(conn[0], tablero, 10,0) < 0)
            {
                perror("Error de escritura en el socket");
                exit(1);
            }
            if (send(conn[1], tablero, 10,0) < 0)
            {
                perror("Error de escritura en el socket");
                exit(1);
            }

            fin = hasWinned(tablero);
            
            if(!fin){
                //Recibir input p2
                if ((readbytes[1] = recv(conn[1],&gamemsg, 1,0)) < 0)
                {
                    perror("Error de lectura en el socket");
                    exit(1);
                }
                nummsg = gamemsg - '0';
                tablero[nummsg-1] = '2';
                if(hasWinned(tablero)){
                    tablero[9] = '1';
                }

                cout << "Recibido: " << gamemsg << endl;

                //Send tablero
                if (send(conn[0], tablero, 10,0) < 0)
                {
                    perror("Error de escritura en el socket");
                    exit(1);
                }
                if (send(conn[1], tablero, 10,0) < 0)
                {
                    perror("Error de escritura en el socket");
                    exit(1);
                }

                fin = hasWinned(tablero);
            }
        // condiciÃ³n de final: haber recibido 0 bytes (fin de fichero alcanzado)
        }while (!fin);

        printf("\nSocket cerrado para lectura\n");
        if (f_verbose) printf("Enviado fin a los cliente\n");

        // cierra la conexiÃ³n con el cliente
        close(sock);
        if (f_verbose) printf("Cerrada la conexiÃ³n con el cliente\n");

    close(sock);
    exit(0);
}
