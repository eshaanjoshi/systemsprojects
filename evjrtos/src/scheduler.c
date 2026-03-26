#include "scheduler.h"
#include "port.h"
#include "uart.h"
#include <avr/interrupt.h>
#define NULL 0x0

volatile uint8_t current_index = 0;
volatile TCB_t *pxCurrentTCB;

volatile uint32_t tick_counter=0;

TCB_t *top_priority;

TCB_t **get_top_priority(){
    return &top_priority;
}

TCB_t *blocked_list;
void assert_no_cycle(TCB_t *list, const char *name){
    //uart_print_str(name);
    TCB_t *slow = list;
    TCB_t *fast = list;
    while(fast != NULL && fast->next != NULL){
        slow = slow->next;
        fast = fast->next->next;
        if(slow == fast){
            while(1); // halt
        }
    }
}

void task_delay(uint32_t ticks){
    
    TCB_t *_curr = (TCB_t*)pxCurrentTCB;
    _curr->state = BLOCKED;
    _curr->wake_time = tick_counter+ticks;
    remove_from_list(&top_priority, _curr);
    insert_to_list(&blocked_list, _curr, _curr->priority);
    assert_no_cycle(blocked_list, "blockeddelay");
    assert_no_cycle(top_priority, "topdelay");
    
    vPortYield();
}
void idle_task(void){
    while(1);
}

static uint8_t idle_stack[STACK_SIZE];
static TCB_t idle_tcb;

TCB_t *task_init(void) {

    task_create(&idle_tcb, idle_stack, idle_task, 0);
    return &idle_tcb;
}



void insert_to_list(TCB_t **list, TCB_t *tcb, int prio) {
    //assert_no_cycle(top_priority, "top");
    //assert_no_cycle(blocked_list, "blocked");
    
    tcb->next = NULL;
    TCB_t *_curr = *list;
    TCB_t *_prev = NULL;

    while (_curr != NULL && _curr->priority >= prio) {
        _prev = _curr;
        _curr = _curr->next;
    }

    tcb->next = _curr;

    if (_prev == NULL) {
        *list = tcb;
    } else {
        _prev->next = tcb;
    }
}

void insert_to_ready(TCB_t *ready){
    insert_to_list(&top_priority, ready, ready->priority);
}


void remove_from_list(TCB_t **list, TCB_t *tcb) {
    if (*list == NULL || tcb==NULL) return;

    TCB_t *_curr = *list;
    TCB_t *_prev = NULL;
    while (_curr != NULL && _curr != tcb) {
        _prev = _curr;
        _curr = _curr->next;
    }
    

    if (_curr == NULL) return;

    if (_prev == NULL) {
        *list = _curr->next;
    } else {
        _prev->next = _curr->next;
    }
    _curr->next = NULL;
}

void task_create(TCB_t *tcb, uint8_t *stack, void (*task_func)(void), int prio) {

    

    uint8_t *top = stack+STACK_SIZE-1;
    tcb->state = READY;
    tcb->wake_time = 0;
    tcb->pxTopOfStack = pxPortInitialiseStack(top, task_func);
    tcb->priority = prio;
    tcb->next=NULL;
    insert_to_list(&top_priority, tcb, prio);
}

void unblock_if_finished(void){
    TCB_t *_next = blocked_list;
    while(_next!=NULL){
        TCB_t *_after = _next->next;
        if(_next->state==BLOCKED && _next->wake_time<=tick_counter){
            _next->state = READY;
            _next->wake_time = 0;
            remove_from_list(&blocked_list, _next);
            insert_to_list(&top_priority, _next, _next->priority);
            assert_no_cycle(top_priority, "topunblock");
        }
        _next = _after;
    }
}


void task_switch_context(void){
    //assert_no_cycle(top_priority, "topswitchpre");
    if (top_priority == NULL) return;
        //current_index = (current_index+1) % MAX_TASKS;
    TCB_t *_curr = top_priority;
    top_priority = _curr->next;


    if((TCB_t *)pxCurrentTCB != _curr && pxCurrentTCB->state == RUNNING){
        ((TCB_t *)pxCurrentTCB)->next = NULL;
        pxCurrentTCB->state = READY;
        insert_to_list(&top_priority, (TCB_t *)pxCurrentTCB, pxCurrentTCB->priority);
        assert_no_cycle(top_priority, "topswitch");
    }
    _curr->next = NULL;
    pxCurrentTCB = _curr;
    pxCurrentTCB->state=RUNNING;
}