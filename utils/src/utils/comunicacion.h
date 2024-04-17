#ifndef COMUNICACION_H_
#define COMUNICACION_H_

#include <stdlib.h>
#include <stdio.h>
#include <utils/server.h>
#include <utils/inicio.h>
#include <inttypes.h>
#include <commons/log.h>
#include <pthread.h>

#include <utils/mensajes.h>

typedef struct
{
    t_log *log;
    int c_socket;
    char *server_name;

} t_atender_cliente_args;

int server_escuchar(t_log* logger, char* server_name, int server_socket);

#endif