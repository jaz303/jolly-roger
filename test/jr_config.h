#ifndef JR_CONFIG_
#define JR_CONFIG_

#define JR_MAX_EVENT_SIZE 		16
#define JR_MAX_PENDING_EVENTS	10

#define NVIC_SetPriority(a, b)
#define __DMB()
#define __disable_irq()
#define __set_PRIMASK(x)
#define __get_PRIMASK() 1
#define SCB_ICSR_PENDSVSET_Msk 1

typedef struct SCB_Dummy {
	uint32_t ICSR;
} SCB_Dummy; 

#define SCB ((SCB_Dummy*)0)

#endif