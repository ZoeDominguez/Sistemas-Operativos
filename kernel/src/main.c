#include <main.h>

t_log* kernel_log; 
t_config* kernel_config;
t_queue* colaNew;
t_queue* colaReady;
t_queue* colaExec;
t_queue* colaBlocked;
t_queue* colaExit;


int main(void) {

    //t_log* kernel_log;
	//t_config* kernel_config;    
    int kernel_server;
    char* puerto_escucha;

    char* ip_memoria;
    char* puerto_memoria;
    int conexion_memoria;

    char* ip_cpu;
    char* puerto_cpu_dispatch;
    char* puerto_cpu_interrupt;
	
    int conexion_cpu_dispatch;
    int conexion_cpu_interrupt;


    kernel_log = iniciar_logger("kernel.log","kernel");
    kernel_config = iniciar_config("kernel.config");
    
	/* KERNEL - Cliente */

    // Extraer configs
    /*
	ip_memoria = config_get_string_value(kernel_config,"IP_MEMORIA");
	puerto_memoria = config_get_string_value(kernel_config, "PUERTO_MEMORIA");
    
	ip_cpu = config_get_string_value(kernel_config, "IP_CPU");
	puerto_cpu_dispatch = config_get_string_value(kernel_config, "PUERTO_CPU_DISPATCH");
	puerto_cpu_interrupt = config_get_string_value(kernel_config, "PUERTO_CPU_INTERRUPT");
    */
    // Establecer conexiones
/*
	conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
    log_info(kernel_log, "KERNEL se conectó a MEMORIA");
    send_handshake(conexion_memoria, kernel_log, "KERNEL / MEMORIA");
    
    conexion_cpu_dispatch = crear_conexion(ip_cpu, puerto_cpu_dispatch);
    log_info(kernel_log, "KERNEL se conectó a CPU DISPATCH");
    send_handshake(conexion_cpu_dispatch, kernel_log, "KERNEL / CPU DISPATCH");

    conexion_cpu_interrupt = crear_conexion(ip_cpu, puerto_cpu_interrupt);
    log_info(kernel_log, "KERNEL se conectó a CPU INTERRUPT");
    send_handshake(conexion_cpu_interrupt, kernel_log, "KERNEL / CPU INTERRUPT");

    */
 	/* KERNEL - Servidor */
    /*
    // Extraer configs

    puerto_escucha = config_get_string_value(kernel_config, "PUERTO_ESCUCHA");
*/
    // Inicio server
/*
    kernel_server = iniciar_servidor(puerto_escucha, kernel_log);
    log_info(kernel_log, "KERNEL listo para recibir clientes");
    server_escuchar(kernel_log, "kernel", kernel_server);
    
    */
    /* KERNEL - PCB */

    // Inicializo Colas de Estado //ver  si es necesario para execute y exit

    inicializar_colas_estados();
    iniciar_proceso();
    iniciar_proceso();

    proceso_estado();

    finalizar_proceso(1);

    return 0;
}

// INICIAR PROCESO //


void iniciar_proceso (/*const char *nombre_archivo*/){

    //FILE* archivo;
    t_pcb* pcb;

    // archivo = fopen(nombre_archivo, "r");

    /*if (archivo == NULL) {
        log_error(kernel_log, "No se pudo abrir el archivo.\n");
        return;
    }
*/
    static uint32_t pid_contador = 0;

    pcb = crear_nuevo_pcb(&pid_contador);
    queue_push(colaNew,pcb);

    //avisar a memoria

    //fclose(archivo);
}

t_pcb* crear_nuevo_pcb(uint32_t *pid_contador){

    t_pcb* nuevo_pcb = malloc(sizeof(t_pcb)); 
    
    if (nuevo_pcb == NULL) {
        log_warning(kernel_log,"Se creo un PCB NULL\n");
        return NULL;
    }
    
    nuevo_pcb->pid = *pid_contador;
    nuevo_pcb->p_counter = 0; //ver de donde salen todas las instrucciones
    nuevo_pcb->quantum =  config_get_int_value(kernel_config,"QUANTUM");
    nuevo_pcb->tabla_paginas = NULL;  
    nuevo_pcb->algoritmo_planif= config_get_string_value(kernel_config,"ALGORITMO_PLANIFICACION");
    nuevo_pcb->estado = NEW;
    
    log_info(kernel_log, "Se crea el proceso con PID = %u en NEW", nuevo_pcb->pid);
    
    (*pid_contador)++;

    return nuevo_pcb;
}

void liberar_pcb(t_pcb* pcb) {
    if (pcb != NULL) {
        free(pcb->tabla_paginas);  
        free(pcb);            
    }
}

void finalizar_proceso(int pid){}

////CAMBIO MULTIPROGRAMACION /////

void multiprogramacion(int nuevo_grado){
    
    config_set_value(kernel_config,"GRADO_MULTIPROGRAMACION", (char*) nuevo_grado);

    int cambio_ok=config_get_int_value(kernel_config,"GRADO_MULTIPROGRAMACION");
    
    if(cambio_ok==nuevo_grado){
        log_info(kernel_log, "Se cambio el grado de multiprogramacion a %d", nuevo_grado);
    }else {
        log_warning(kernel_log, "No se pudo cambiar el grado de multiprogramacion");
    }
}

void inicializar_colas_estados(){
    colaNew = queue_create();
    colaReady = queue_create();
    colaExec = queue_create();
    colaBlocked = queue_create();
    colaExit = queue_create();
}

void proceso_estado(){
    
    mostrar_estado_cola(colaNew, NEW);
    mostrar_estado_cola(colaReady,READY);
    mostrar_estado_cola(colaExec,EXEC);
    mostrar_estado_cola(colaBlocked,BLOCKED);
    mostrar_estado_cola(colaExit,EXIT);

}

void mostrar_estado_cola(t_queue* cola, t_proceso_estado estado){

    int tamanio_cola = queue_size(cola);
    int contador = 0;

    if(tamanio_cola != 0)
    {    

        log_info(kernel_log, "Los PID en la COLA %s son:",estado_a_string(estado));

        while(contador < tamanio_cola){

         t_pcb* pcb_apunta = queue_pop(cola);
        
         if(pcb_apunta != NULL){
            uint32_t pid = pcb_apunta->pid;    
            queue_push(colaNew,pcb_apunta);
            log_info(kernel_log, "PID: %u", pid);
         }
         
        contador++;
        }
    }
    else{

     log_warning(kernel_log, "La COLA %s esta VACIA", estado_a_string(estado));
     
    }
    
}
/// funcion para especificar cual elemento del enum es (estado)
char* estado_a_string(t_proceso_estado estado) 
{
    switch(estado){
        case 0:
            return "NEW";
        case 1:
            return "READY";
        case 2:
            return "EXEC";
        case 3:
          return "BLOCKED";
        case 4:
            return "EXIT";

    }
}

void finalizar_proceso(uint32_t num_pid){

    if(find_queue(num_pid,colaNew)){
        borrarElemento(colaNew,num_pid);
    }
    mostrar_estado_cola(colaNew, NEW);

}

/*bool buscar_pid_cola(uint32_t num_pid,t_queue* procesos_cola){
    
   t_link_element* actual = procesos_cola->elements->head;
    while (actual != NULL) {
        uint32_t* dato = actual->data;
        if (*dato == num_pid) {
            return 1; 
        }
        actual = actual->next;
    }
    return 0; 
}*/

// Devuelve 1. TRUE o 0. FALSE si encuentra el elemento en la cola COMMONS QUEUE
int find_queue((void*) elem, t_queue* cola){

    if (cola == NULL || cola->elements == NULL || cola->elements->head == NULL) {
        return 0; 
    }

    t_link_element* actual = cola->elements->head;

    while (actual != NULL) {
        int* dato = (int*) actual->data;
        if (*dato == *((int*) elem)) {
            return 1; 
        }
        actual = actual->next;
    }
    return 0; 
}

    
/*void borrarElemento(t_queue *cola, uint32_t elemento) {
    t_link_element *actual = cola->elements->head;
    t_link_element *anterior = NULL;

    // Buscar el elemento en la cola
    while (actual != NULL && actual->data != elemento) {
        anterior = actual;
        actual = actual->next;
    }

    // Si se encontró el elemento
    if (actual != NULL) {
        // Si el elemento es el primero en la cola
        if (anterior == NULL) {
            cola->elements->head = actual->next;
        } else {
            anterior->next = actual->next;
        }

        free(actual->data); // Liberar la memoria de la estructura almacenada
        free(actual); // Liberar la memoria del elemento de enlace
        cola->elements->elements_count--;
        printf("Elemento eliminado de la cola.\n");
    } else {
        printf("El elemento no está presente en la cola.\n");
    }
}*/



