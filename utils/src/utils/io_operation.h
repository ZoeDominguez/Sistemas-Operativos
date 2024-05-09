#ifndef IO_OP_H
#define IO_OP_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include "mensajesPropios.h"
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>


typedef enum
{
    IO_GEN_SLEEP,
    IO_STDIN_READ,
    IO_STDOUT_WRITE,
    IO_FS_CREATE,
    IO_FS_DELETE,
    IO_FS_TRUNCATE,
    IO_FS_WRITE,
    IO_FS_READ
} IO_OPERATION;

typedef struct
{
    IO_OPERATION codigo_operacion;
    t_buffer_ins* buffer;
} t_paquete_instruccion;

typedef struct 
{   
     union {
        struct {
            char* unidades_trabajo;
        } io_gen_sleep_params;
         struct {
            char* nombre_archivo;
        } io_fs_create_params;
     }params;
}instruccion_params;


t_buffer_ins* serializar_io_gen_sleep(instruccion_params* param);
void enviar_instruccion(t_paquete_instruccion* instruccion, instruccion_params* parametros ,int socket_cliente);


#endif