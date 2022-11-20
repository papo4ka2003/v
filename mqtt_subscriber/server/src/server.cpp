
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
//#include "opencv2/opencv.hpp"
/* Files required for transport initialization. */
#include <coresrv/nk/transport-kos.h>
#include <coresrv/sl/sl_api.h>

/* EDL description of the server entity. */
#include <mqtt_subscriber/Server.edl.h>

//#include <assert.h>

#include <unistd.h>
#include <gpio/gpio.h>
//#include <stdbool.h>
#include <sys/cdefs.h>
#include <bsp/bsp.h>
#include <rtl/countof.h>
#define GPIO_PIN_NUM   RTL_UINT32_C(28)
#define DELAY_S        2
#define HW_MODULE_NAME "gpio0"
#define HW_MODULE_CFG  "raspberry_pi4b.default"

#define GPIO_PIN_UART_TX 14
#define GPIO_PIN_UART_RX 15
#define LOW 0
#define HIGH 1
#define GPIO_PIN_NUM_IN1 12
#define GPIO_PIN_NUM_IN2 13
#define GPIO_PIN_NUM_IN3 20
#define GPIO_PIN_NUM_IN4 21
#define GPIO_PIN_NUM_ENA 6
#define GPIO_PIN_NUM_ENB 26
GpioHandle handle1 = NULL;





static const rtl_uint32_t ExceptionPinArr[] = { GPIO_PIN_UART_TX,
                                                GPIO_PIN_UART_RX };

static bool IsExceptionPin(const rtl_uint32_t pin)
{
    bool isExceptionPin = false;

    for (size_t i = 0; i < rtl_countof(ExceptionPinArr); i++)
    {
        if (ExceptionPinArr[i] == pin)
        {
            isExceptionPin = true;
            break;
        }
    }

    return isExceptionPin;
}


void forward(float value);
void stop(float value);


void backward(float value);


void left(float value);


void right(float value);








/* Type of interface implementing object. */
typedef struct IPingImpl {
    struct mqtt_subscriber_Ping base;     /* Base interface of object */
    rtl_uint32_t step;         /* Extra parameters */
} IPingImpl;

/* Ping method implementation. */
static nk_err_t Ping_impl(struct mqtt_subscriber_Ping *self,
                          const struct mqtt_subscriber_Ping_Ping_req *req,
                          const struct nk_arena *req_arena,
                          struct mqtt_subscriber_Ping_Ping_res *res,
                          struct nk_arena *res_arena)
{
    IPingImpl *impl = (IPingImpl *)self;
    /**
     * Increment value in client request by
     * one step and include into result argument that will be
     * sent to the client in the server response.
     */
    //fprintf(stderr,"res=%d req=%d",req->result,req->value);
    if(int(req->value)==0){
        stop(0);
    }
    if(req->value==1){
        forward(int(req->result));
    }
    if(req->value==2){
        backward(int(req->result));
    }
    if(req->value==3){
        left(int(req->result));
    }
    if(req->value==4){
        right(int(req->result));
    }
    //res->result = req->value; //+ impl->step;
    return NK_EOK;
}







/*   dasfsafsafasgasda                                                               */




static nk_err_t Pong_impl(struct mqtt_subscriber_Ping *self,
                          const struct mqtt_subscriber_Ping_Pong_req *req,
                          const struct nk_arena *req_arena,
                          struct mqtt_subscriber_Ping_Pong_res *res,
                          struct nk_arena *res_arena)
{
    IPingImpl *impl = (IPingImpl *)self;
    /**
     * Increment value in client request by
     * one step and include into result argument that will be
     * sent to the client in the server response.
     */
    //fprintf(stderr,"task=%d id=%d x=%d y=%d",req->task,req->id,req->x,req->y);
    //res->result = req->value; //+ impl->step;
    return NK_EOK;
}







/**
 * IPing object constructor.
 * step is the number by which the input value is increased.
 */
static struct mqtt_subscriber_Ping *CreateIPingImpl(rtl_uint32_t step)
{
    /* Table of implementations of IPing interface methods. */
    static const struct mqtt_subscriber_Ping_ops ops = {
        .Ping = Ping_impl,
        .Pong=Pong_impl
    };

    /* Interface implementing object. */
    static struct IPingImpl impl = {
        .base = {&ops}
    };

    impl.step = step;

    return &impl.base;
}
/*sadasdasfqwrqeggsagasdgs*/








/* Server entry point. */
int main(void)
{
    










    NkKosTransport transport;
    ServiceId iid;

    /* Get server IPC handle of "server_connection". */
    Handle handle = ServiceLocatorRegister("server_connection", NULL, 0, &iid);
    //assert(handle != INVALID_HANDLE);

    /* Initialize transport to client. */
    NkKosTransport_Init(&transport, handle, NK_NULL, 0);

    /**
     * Prepare the structures of the request to the server entity: constant
     * part and arena. Because none of the methods of the server entity has
     * sequence type arguments, only constant parts of the
     * request and response are used. Arenas are effectively unused. However,
     * valid arenas of the request and response must be passed to
     * server transport methods (nk_transport_recv, nk_transport_reply) and
     * to the server_entity_dispatch method.
     */
    mqtt_subscriber_Server_entity_req req;
    char req_buffer[mqtt_subscriber_Server_entity_req_arena_size];
    struct nk_arena req_arena = NK_ARENA_INITIALIZER(req_buffer,
                                        req_buffer + sizeof(req_buffer));

    /* Prepare response structures: constant part and arena. */
    mqtt_subscriber_Server_entity_res res;
    char res_buffer[mqtt_subscriber_Server_entity_res_arena_size];
    struct nk_arena res_arena = NK_ARENA_INITIALIZER(res_buffer,
                                        res_buffer + sizeof(res_buffer));

    /**
     * Initialize ping component dispatcher. 3 is the value of the step,
     * which is the number by which the input value is increased.
     */
    mqtt_subscriber_Ping_component component;
    mqtt_subscriber_Ping_component_init(&component, CreateIPingImpl(3));

    /* Initialize server entity dispatcher. */
    mqtt_subscriber_Server_entity entity;
    mqtt_subscriber_Server_entity_init(&entity, &component);

    //fprintf(stderr, "Hello I'm server\n");


    /*BspError rc = BspInit(NULL);
	if (rc != BSP_EOK) {
		fprintf(stderr, "Failed to initialize BSP\n");
		return EXIT_FAILURE;
	}
    //fprintf(stderr, "Hello I'm server1\n");
	rc = BspSetConfig(HW_MODULE_NAME, HW_MODULE_CFG);
	if (rc != BSP_EOK) {
		fprintf(stderr, "Failed to set mux configuration for gpio0 module\n");
		return EXIT_FAILURE;
	}*/
    if (GpioInit()) {
		fprintf(stderr, "GpioInit failed\n");
		return EXIT_FAILURE;
	}

    int returnValue = EXIT_FAILURE;
    
    //fprintf(stderr, "Hello I'm server3\n");
	if (GpioOpenPort("gpio0", &handle1) || handle1 == GPIO_INVALID_HANDLE) {
		fprintf(stderr, "GpioOpenPort failed\n");
		return EXIT_FAILURE;
	}//fprintf(stderr, "Hello I'm server4\n");
	GpioSetMode(handle1, GPIO_PIN_NUM_IN1, GPIO_DIR_OUT);
	GpioSetMode(handle1, GPIO_PIN_NUM_IN2, GPIO_DIR_OUT);
	GpioSetMode(handle1, GPIO_PIN_NUM_IN3, GPIO_DIR_OUT);
	GpioSetMode(handle1, GPIO_PIN_NUM_IN4, GPIO_DIR_OUT);
	GpioSetMode(handle1, GPIO_PIN_NUM_ENA, GPIO_DIR_OUT);
	GpioSetMode(handle1, GPIO_PIN_NUM_ENB, GPIO_DIR_OUT);
//fprintf(stderr, "Hello I'm server5\n");
fprintf(stderr, "Hello I'm server4\n");
    //forward(5);
    //left(4);










    /* Dispatch loop implementation. */
    do
    {
        /* Flush request/response buffers. */
        nk_req_reset(&req);
        nk_arena_reset(&req_arena);
        nk_arena_reset(&res_arena);

        /* Wait for request to server entity. */
        if (nk_transport_recv(&transport.base,
                              &req.base_,
                              &req_arena) != NK_EOK) {
            fprintf(stderr, "nk_transport_recv error\n");
        } else {
            /**
             * Handle received request by calling implementation Ping_impl
             * of the requested Ping interface method.
             */
            fprintf(stderr, "nk_transport_recv error1\n");
            mqtt_subscriber_Server_entity_dispatch(&entity, &req.base_, &req_arena,
                                        &res.base_, &res_arena);
        }

        /* Send response. */
        //fprintf(stderr, "%d\n",(int) res_arena.result);
        if (nk_transport_reply(&transport.base,
                               &res.base_,
                               &res_arena) != NK_EOK) {
            fprintf(stderr, "nk_transport_reply error\n");
        }
        
    }
    while (true);
    
       /*if (GPIO_INVALID_HANDLE != handle1)
    {
        Retcode tempRc;
        tempRc = GpioClosePort(handle1);
        if (rcOk != tempRc)
        {
            fprintf(stderr,
                    "GpioClosePort for %s port failed, error code: %d.\n",
                    HW_MODULE_NAME, RC_GET_CODE(tempRc));
            rc = tempRc;
        }
    }*/

    /*if (rcOk == rc)
    {
        //fprintf(stderr, "Test finished.\n");
        returnValue = EXIT_SUCCESS;
    }
    else
    {
        //fprintf(stderr, "Test failed.\n");
    }*/


    return EXIT_SUCCESS;
}









void forward(float value) {
	
	fprintf(stderr, "forward\n");
	GpioOut(handle1, GPIO_PIN_NUM_IN1, HIGH);
	GpioOut(handle1, GPIO_PIN_NUM_IN2, LOW);
	GpioOut(handle1, GPIO_PIN_NUM_IN3, LOW);
	GpioOut(handle1, GPIO_PIN_NUM_IN4, HIGH);
	GpioOut(handle1, GPIO_PIN_NUM_ENA, HIGH);
	GpioOut(handle1, GPIO_PIN_NUM_ENB, HIGH);
	sleep(value);
	stop(0);
}
void stop(float value) {

	fprintf(stderr, "stop\n");
	GpioOut(handle1, GPIO_PIN_NUM_IN1, LOW);
	GpioOut(handle1, GPIO_PIN_NUM_IN2, LOW);
	GpioOut(handle1, GPIO_PIN_NUM_IN3, LOW);
	GpioOut(handle1, GPIO_PIN_NUM_IN4, LOW);
	GpioOut(handle1, GPIO_PIN_NUM_ENA, LOW);
	GpioOut(handle1, GPIO_PIN_NUM_ENB, LOW);
	sleep(value);
}


void backward(float value) {

	//fprintf(stderr, "backward\n");
	GpioOut(handle1, GPIO_PIN_NUM_IN1, LOW);
	GpioOut(handle1, GPIO_PIN_NUM_IN2, HIGH);
	GpioOut(handle1, GPIO_PIN_NUM_IN3, HIGH);
	GpioOut(handle1, GPIO_PIN_NUM_IN4, LOW);
	GpioOut(handle1, GPIO_PIN_NUM_ENA, HIGH);
	GpioOut(handle1, GPIO_PIN_NUM_ENB, HIGH);
	sleep(value);
	stop(0);
}


void left(float value) {

	fprintf(stderr, "left\n");
	GpioOut(handle1, GPIO_PIN_NUM_IN1, LOW);
	GpioOut(handle1, GPIO_PIN_NUM_IN2, LOW);
	GpioOut(handle1, GPIO_PIN_NUM_IN3, LOW);
	GpioOut(handle1, GPIO_PIN_NUM_IN4, HIGH);
	GpioOut(handle1, GPIO_PIN_NUM_ENA, HIGH);
	GpioOut(handle1, GPIO_PIN_NUM_ENB, HIGH);
	sleep(value);
	stop(0);
}


void right(float value) {
//BspError rc = BspInit(NULL);
	//rc = BspSetConfig(HW_MODULE_NAME, HW_MODULE_CFG);
	//fprintf(stderr, "right\n");
	GpioOut(handle1, GPIO_PIN_NUM_IN1, HIGH);
	GpioOut(handle1, GPIO_PIN_NUM_IN2, LOW);
	GpioOut(handle1, GPIO_PIN_NUM_IN3, LOW);
	GpioOut(handle1, GPIO_PIN_NUM_IN4, LOW);
	GpioOut(handle1, GPIO_PIN_NUM_ENA, HIGH);
	GpioOut(handle1, GPIO_PIN_NUM_ENB, HIGH);
	sleep(value);
	stop(0);
}