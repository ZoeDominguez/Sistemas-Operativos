#include "funciones_tlb.h"

TLB* tlb;

void iniciar_tlb() { 
    tlb = malloc(sizeof(TLB));
    tlb->entradas = (Entrada*)malloc(cant_entradas_tlb * sizeof(Entrada));
    for (int i = 0; i < cant_entradas_tlb; i++) {
        tlb->entradas[i].validez = false;  
    }
    tlb->siguiente_fifo = 0;
    tlb->contador_lru = 0;
}

int buscar_tlb(uint32_t pid, int num_pag){
  for (int i = 0; i < cant_entradas_tlb; i++) {
        if (tlb->entradas[i].validez && tlb->entradas[i].pid == pid && tlb->entradas[i].pagina == num_pag) {
            tlb->entradas[i].ultimo_acceso = tlb->contador_lru++; 
            return tlb->entradas[i].frame; //HIT
        }
    }
    return -1;  //MISS
}

t_dir_fisica* posible_mmu(int dir_logica, uint32_t pid){

    int numero_pagina = floor(dir_logica / tam_pag);
    int desplazamiento = dir_logica - numero_pagina * tam_pag;
    int frame = buscar_tlb(pid, numero_pagina);
   if (frame != -1) {  // HIT
        printf("TLB Hit! Página: %d, Frame: %d\n", numero_pagina, frame);
    } else {  // MISS
        printf("TLB Miss!\n");
        // obtener la tabla de páginas correspondiente al pid
        // Obtener el frame desde la tabla de páginas
        if (string_equals_ignore_case(algoritmo, "FIFO")) {
            remplazo_fifo(pid, numero_pagina, frame);
        } else if (string_equals_ignore_case(algoritmo, "LRU")) {
           // remplazo_lru(pid, numero_pagina, frame);
        }
    }
   t_dir_fisica* direccionFisica = malloc(sizeof(t_dir_fisica));
   direccionFisica->nro_frame = frame;
   direccionFisica->desplazamiento = desplazamiento;
   return direccionFisica;
}

void remplazo_fifo(uint32_t pid, int pagina, int frame){
    tlb->entradas[tlb->siguiente_fifo].pid = pid;
    tlb->entradas[tlb->siguiente_fifo].pagina = pagina;
    tlb->entradas[tlb->siguiente_fifo].frame = frame;
    tlb->entradas[tlb->siguiente_fifo].ultimo_acceso = tlb->contador_lru++;
    tlb->entradas[tlb->siguiente_fifo].validez = true; 
    tlb->siguiente_fifo = (tlb->siguiente_fifo + 1) % cant_entradas_tlb;
}

void remplazo_lru(uint32_t pid, int pagina, int frame){

}