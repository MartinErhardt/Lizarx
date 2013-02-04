#include <mt/cpustate.h>
#include <mt/ts.h>
#include <mm/pmm.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
static uint32_t num_tasks = 0;

static struct task* first_task = NULL;
static struct task* current_task = NULL;

void init_mt(){
    
}
/*
 * Jeder Task braucht seinen eigenen Stack, auf dem er beliebig arbeiten kann,
 * ohne dass ihm andere Tasks Dinge ueberschreiben. Ausserdem braucht ein Task
 * einen Einsprungspunkt.
 */
struct task* init_task(void* entry)
{
    uint8_t* stack = kpmm_malloc_4k();
    uint8_t* user_stack = kpmm_malloc_4k();
    
    num_tasks++;
    /*
     * CPU-Zustand fuer den neuen Task festlegen
     */
    cpu_state new_state = {
        .eax = 0,
        .ebx = 0,
        .ecx = 0,
        .edx = 0,
        .esi = 0,
        .edi = 0,
        .ebp = 0,
        .esp = (uint32_t) user_stack+STDRD_STACKSIZE,
        .eip = (uint32_t) entry,
 
        /* Ring-0-Segmentregister nicht mehr benutzt*/
        //.cs  = 0x08,
	/* Ring-3-Segmentregister */
        .cs  = 0x18 | 0x03,
        .ss  = 0x20 | 0x03,
        /* IRQs einschalten (IF = 1) */
        .eflags = 0x202,
    };
    /*
     * Den angelegten CPU-Zustand auf den Stack des Tasks kopieren, damit es am
     * Ende so aussieht als waere der Task durch einen Interrupt unterbrochen
     * worden. So kann man dem Interrupthandler den neuen Task unterschieben
     * und er stellt einfach den neuen Prozessorzustand "wieder her".
     */
    cpu_state* state = (void*) (stack + STDRD_STACKSIZE - sizeof(new_state));
    *state = new_state;
    
    /*
     * neuen Task erstellen
     */
    struct task* task = kpmm_malloc_4k();
    task->state = state;
    task->pid = num_tasks;
    task->stack = stack;
    task->user_stack = user_stack;
    task->next = first_task;
    first_task = task;
    
    return task;
}
/*
 * Gibt den Prozessorzustand des naechsten Tasks zurueck. Der aktuelle
 * Prozessorzustand wird als Parameter uebergeben und gespeichert, damit er
 * beim naechsten Aufruf des Tasks wiederhergestellt werden kann
 */
cpu_state* schedule(cpu_state* cpu)
{
    /*
     * Wenn schon ein Task laeuft, Zustand sichern. Wenn nicht, springen wir
     * gerade zum ersten Mal in einen Task. Diesen Prozessorzustand brauchen
     * wir spaeter nicht wieder.
     */
    if (current_task != NULL) {
        current_task->state = cpu;
    }

    /*
     * Naechsten Task auswaehlen. Wenn alle durch sind, geht es von vorne los
     */
    if (current_task == NULL) {
        current_task = first_task;
    } else {
        current_task = current_task->next;
        if (current_task == NULL) {
            current_task = first_task;
        }
    }

    /* Prozessorzustand des neuen Tasks aktivieren */
    cpu = current_task->state;

    return cpu;
}
