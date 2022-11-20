//#include <iostream>
//#include <string_view>
#include<string>
#include "general.h"
#include "subscriber.h"
#include "nlohmann/json.hpp"
#include<vector>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* Files required for transport initialization. */
#include <coresrv/nk/transport-kos.h>
#include <coresrv/sl/sl_api.h>

/* Description of the server interface used by the `client` entity. */
#include <mqtt_subscriber/Ping.idl.h>

#include <assert.h>

#define AUTO 5
#define FORW 1
#define BACK 2
#define LEFT 3
#define RIGHT 4
#define STOP 0

using namespace std;
using namespace std::literals;
using json = nlohmann::json;
constexpr auto Topic = "my/awesome/topic"sv;

struct task{
        int tas;
        int id;
        int x;
        int y;
    };
Subscriber::Subscriber(const char *id, const char *host, int port)
    : mosquittopp(id)
{
   // std::cout << app::AppTag << "Connecting to MQTT Broker with address "
     //         << host << " and port " << port << std::endl;

    const int keepAlive = 60;

    connect(host, port, keepAlive);
}

void Subscriber::on_connect(int rc)
{
    //std::cout << app::AppTag << "Subscriber connected with code "
      //        << rc << std::endl;

    if (rc == 0)
    {
        subscribe(NULL, Topic.data());
    }
}

void Subscriber::on_message(const struct mosquitto_message *message)
{
    if (Topic == message->topic)
    {
        const std::string payload{reinterpret_cast<char*>(message->payload),
                                       static_cast<size_t>(message->payloadlen)};
       // std::cout << app::AppTag << "Got message with topic: " << message->topic
        //          << ", payload: " << payload << std::endl;


        NkKosTransport transport;
    struct mqtt_subscriber_Ping_proxy proxy;
    int i;

    fprintf(stderr, "Hello I'm client\n");
    //(payload)
    json data=json::parse(payload);
    //std::cout<<data["cwd"]<<std::endl;
    
    //if()



    std::string s3{data["cwd"]};
    int com=0;
    double val=0;
    int task=0;
    int x=0;
    int y=0;
    if(s3=="forward"){
        com=FORW;
        val=std::stod(std::string(data["val"]));
    }
    if(s3=="back"){
        com=BACK;
        val=std::stod(std::string(data["val"]));
    }
    if(s3=="left"){
        com=LEFT;
        val=std::stod(std::string(data["val"]));
    }
    if(s3=="right"){
        com=RIGHT;
        val=std::stod(std::string(data["val"]));
    }
    if(s3=="stop"){
        com=STOP;
    }
    
    std::vector<struct task>realvec;
    if(s3=="auto"){
        json vec1=data["markers_screen"];
        json vec2=data["markers_floor"];
        json vec3=data["targets_screen"];
        json vec4=data["robot_screen"];

        //<<std::endl;
        //std::cout<<vec1[0]<<std::endl;
        for(int i=0;i<vec1.size();i++){
           struct task t1;
            t1.tas=1;
            t1.id=stoi(string(vec1[i]["id"]));
            t1.x=stoi(string(vec1[i]["x"]));
            t1.y=stoi(string(vec1[i]["y"]));
            realvec.push_back(t1);
        }
        for(int i=0;i<vec2.size();i++){
           struct task t1;
            t1.tas=2;
            t1.id=stoi(string(vec2[i]["id"]));
            t1.x=stoi(string(vec2[i]["x"]));
            t1.y=stoi(string(vec2[i]["y"]));
            realvec.push_back(t1);
        }
        for(int i=0;i<vec3.size();i++){
           struct task t1;
            t1.tas=3;
            t1.id=stoi(string(vec3[i]["id"]));
            t1.x=stoi(string(vec3[i]["x"]));
            t1.y=stoi(string(vec3[i]["y"]));
            realvec.push_back(t1);
        }
        for(int i=0;i<vec4.size();i++){
           struct task t1;
            t1.tas=4;
            t1.id=stoi(string(vec4[i]["id"]));
            t1.x=stoi(string(vec4[i]["x"]));
            t1.y=stoi(string(vec4[i]["y"]));
            realvec.push_back(t1);
        }

        /*for(auto it=vec1.begin();it!=vec1.end();it++){
            json js=json::parse(*it);
            std::cout<<js["id"]<<std::endl;//["id"]<<' '<<*it["x"]<<' '<<*it["y"]<<std::endl;
        }*/

    }




    //std::cout<<s3<<std::endl;
    //fprintf(stderr,"json:%s\n",s3);

    /**
     * Get the client IPC handle of the connection named
     * "server_connection".
     */
    Handle handle = ServiceLocatorConnect("server_connection");
    assert(handle != INVALID_HANDLE);

    /* Initialize IPC transport for interaction with the server entity. */
    NkKosTransport_Init(&transport, handle, NK_NULL, 0);

    /**
     * Get Runtime Interface ID (RIID) for interface mqtt_subscriber.Ping.ping.
     * Here ping is the name of the mqtt_subscriber.Ping component instance,
     * mqtt_subscriber.Ping.ping is the name of the Ping interface implementation.
     */
    nk_iid_t riid = ServiceLocatorGetRiid(handle, "mqtt_subscriber.Ping.ping");
    assert(riid != INVALID_RIID);

    /**
     * Initialize proxy object by specifying transport (&transport)
     * and server interface identifier (riid). Each method of the
     * proxy object will be implemented by s NkKosTransport transport;
    struct mqtt_subscriber_Ping_proxy proxy;
    int i;

    fprintf(stderr, "Hello I'm client\n");

    /**
     * Get the client IPC handle of the connection named
     * "server_connection".
     */
     handle = ServiceLocatorConnect("server_connection");
    assert(handle != INVALID_HANDLE);

    /* Initialize IPC transport for interaction with the server entity. */
    NkKosTransport_Init(&transport, handle, NK_NULL, 0);

    /**
     * Get Runtime Interface ID (RIID) for interface mqtt_subscriber.Ping.ping.
     * Here ping is the name of the mqtt_subscriber.Ping component instance,
     * mqtt_subscriber.Ping.ping is the name of the Pinending a request to the server.
     */
    mqtt_subscriber_Ping_proxy_init(&proxy, &transport.base, riid);

    /* Request and response structures */
    mqtt_subscriber_Ping_Ping_req req;
    mqtt_subscriber_Ping_Ping_res res;

    /* Test loop. */
    req.value = com;
    req.result=int(val);
    //for (i = 0; i < 10; ++i)
    {
        /**
         * Call Ping interface method.
         * Server will be sent a request for calling Ping interface method
         * ping_comp.ping_impl with the value argument. Calling thread is locked
         * until a response is received from the server.
         */
        if (mqtt_subscriber_Ping_Ping(&proxy.base, &req, NULL, &res, NULL) == rcOk) 

        {
        
            /**
             * Print result value from response
             * (result is the output argument of the Ping method).
             */
            //fprintf(stderr, "result = %d\n", (int) res.result);
            /**
             * Include received result value into value argument
             * to resend to server in next iteration.
             */
            //req.value = res.result+1;

        }
        //else
            //fprintf(stderr, "Failed to call mqtt_subscriber.Ping.Ping()\n");
    }
    //std::cout<<std::endl;
    //std::cout<<"312"<<std::endl;






    mqtt_subscriber_Ping_Pong_req req1;
    mqtt_subscriber_Ping_Pong_res res1;
req1.task = 0;
    req1.id=1;
    req1.x=2;
    req1.y=3;
    /* Test loop. */
    /*for(int i=0;i<realvec.size();i++){
      struct  task t2=realvec[i];
    req1.task = t2.tas;
    req1.id=t2.id;
    req1.x=t2.x;
    req1.y=t2.y;
    }*/
    //for (i = 0; i < 10; ++i)
    //{
        /**
         * Call Ping interface method.
         * Server will be sent a request for calling Ping interface method
         * ping_comp.ping_impl with the value argument. Calling thread is locked
         * until a response is received from the server.
         */
        //if (mqtt_subscriber_Ping_Pong(&proxy.base, &req1, NULL, &res1, NULL) == rcOk) 

        //{
        
            /**
             * Print result value from response
             * (result is the output argument of the Ping method).
             */
            //fprintf(stderr, "result = %d\n", (int) res.result);
            /**
             * Include received result value into value argument
             * to resend to server in next iteration.
             */
            //req.value = res.result+1;

        //}
        //else
            //fprintf(stderr, "Failed to call mqtt_subscriber.Ping.Pong()\n");
  //  }
    //}













    }

}

void Subscriber::on_subscribe(__rtl_unused int        mid,
                              __rtl_unused int        qos_count,
                              __rtl_unused const int *granted_qos)
{
    //std::cout << app::AppTag << "Subscription succeeded." << std::endl;
}
