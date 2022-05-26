#include <stdio.h>
#include "xil_printf.h"
#include "xintc.h"
#include "xgpio.h"
#include "xparameters.h"

XGpio gpio;
XIntc intc;
void gpioHandler(void* callBackRef)
{
	XGpio *gpioPtr =(XGpio *)callBackRef;
	xil_printf("GPIO Int\n");
	XGpio_InterruptClear(gpioPtr,1);
}
int main()
{
	int status;
	//gpio init
	status = XGpio_Initialize(&gpio,XPAR_GPIO_0_DEVICE_ID);
	if(status != XST_SUCCESS) return XST_FAILURE;
	XGpio_SetDataDirection(&gpio,1,1);

	//intc init
	status = XIntc_Initialize(&intc,XPAR_INTC_0_DEVICE_ID);
	if(status != XST_SUCCESS) return XST_FAILURE;

	//connect the interrupt source to its handler
	status = XIntc_Connect(&intc,XPAR_INTC_0_GPIO_0_VEC_ID,
			(XInterruptHandler)gpioHandler,(void *)&gpio);
	if(status != XST_SUCCESS) return XST_FAILURE;

	//intc start
	status = XIntc_Start(&intc,XIN_REAL_MODE);
	if(status != XST_SUCCESS) return XST_FAILURE;

	//enable interrupt on gpio
	XIntc_Enable(&intc,XPAR_INTC_0_GPIO_0_VEC_ID);

	//enable microblaze interrupts
	Xil_ExceptionInit();
	//register intc interrupt into microblaze

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler)XIntc_InterruptHandler,
			&intc);
	//enable microblaze interrupts
	Xil_ExceptionEnable();

	//enable gpio interrupt
	XGpio_InterruptEnable(&gpio,1);
	XGpio_InterruptGlobalEnable(&gpio);
	while(1);
}
