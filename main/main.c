#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include <stdio.h>
#include "freertos/event_groups.h"
#include "lwip/sockets.h"


#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include <string.h>
#include "driver/gpio.h"

#include <math.h>       /* ceil */

#include "LEDMatrix.h"
#include <stdint.h>

#define A GPIO_NUM_19
#define B GPIO_NUM_21
#define C GPIO_NUM_23
#define D GPIO_NUM_27
#define OE GPIO_NUM_26
#define CLK GPIO_NUM_25
#define R1 GPIO_NUM_22
#define R2 GPIO_NUM_2
#define STB GPIO_NUM_4



#define WIDTH   64
#define HEIGHT  32

uint8_t blinktime=4;
uint8_t blinkinterval=80;
uint8_t displaybufg[256];

uint8_t closed[256] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x3f, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x0f,
  0x7f, 0x87, 0x80, 0x00, 0x00, 0x00, 0x00, 0x0f,
  0xff, 0xc7, 0x80, 0x00, 0x00, 0x00, 0x00, 0x0f,
  0xf3, 0xc7, 0x87, 0xe0, 0x7c, 0x0f, 0x81, 0xef,
  0xf3, 0xc7, 0x8f, 0xf0, 0xfe, 0x1f, 0xe3, 0xff,
  0xf3, 0xc7, 0x9f, 0xf9, 0xff, 0x3f, 0xe3, 0xff,
  0xf3, 0xc7, 0x9e, 0x79, 0xe7, 0x3c, 0xf3, 0xcf,
  0xf0, 0x07, 0x9e, 0x79, 0xe7, 0x3c, 0xf3, 0xcf,
  0xf0, 0x07, 0x9e, 0x79, 0xf0, 0x3c, 0xf3, 0xcf,
  0xf0, 0x07, 0x9e, 0x78, 0xf8, 0x3f, 0xf3, 0xcf,
  0xf3, 0xc7, 0x9e, 0x78, 0x7e, 0x3f, 0xf3, 0xcf, //Closed
  0xf3, 0xc7, 0x9e, 0x78, 0x3f, 0x3c, 0x03, 0xcf,
  0xf3, 0xc7, 0x9e, 0x78, 0x0f, 0x3c, 0xf3, 0xcf,
  0xf3, 0xc7, 0x9e, 0x79, 0xcf, 0x3c, 0xf3, 0xcf,
  0xf3, 0xc7, 0x9e, 0x79, 0xcf, 0x3c, 0xf3, 0xcf,
  0xff, 0x87, 0x9f, 0xf9, 0xff, 0x3f, 0xf3, 0xff,
  0x7f, 0x87, 0x8f, 0xf0, 0xfe, 0x1f, 0xe3, 0xff,
  0x3e, 0x07, 0x87, 0xe0, 0x7c, 0x0f, 0xc1, 0xef,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


uint8_t fonts[] ={
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xe0,0x0f,0xf0,0x1f,0xf8,0x3e,0x7c,0x3e,0x7c,0x3e,0x7c,0x3e,0x7c,0x3e,0x7c,0x3e,0x7c,0x3e,0x7c,0x3e,0x7c,0x3e,0x7c,0x3e,0x7c,0x3e,0x7c,0x3e,0x7c,0x3e,0x7c,0x3e,0x7c,0x3e,0x7c,0x3e,0x7c,0x3e,0x7c,0x1e,0x7c,0x1f,0xf8,0x0f,0xf0,0x07,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0xe0, 0x03, 0xe0, 0x0f, 0xe0, 0x3f, 0xe0, 0x3f, 0xe0, 0x3f, 0xe0, 0x03, 0xe0, 0x03, 0xe0, 0x03, 0xe0, 0x03, 0xe0, 0x03, 0xe0, 0x03, 0xe0, 0x03, 0xe0, 0x03, 0xe0, 0x03, 0xe0, 0x03, 0xe0, 0x03, 0xe0, 0x03, 0xe0, 0x03, 0xe0, 0x03, 0xe0, 0x03, 0xe0, 0x03, 0xe0, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x0f, 0xf0, 0x0f, 0xf8, 0x1e, 0x78, 0x1e, 0x7c, 0x1e, 0x7c, 0x1e, 0x7c, 0x1e, 0x7c, 0x00, 0x7c, 0x00, 0xfc, 0x00, 0xf8, 0x01, 0xf8, 0x01, 0xf8, 0x03, 0xf0, 0x03, 0xf0, 0x03, 0xe0, 0x07, 0xe0, 0x07, 0xc0, 0x0f, 0xc0, 0x0f, 0x80, 0x1f, 0x80, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x1f, 0xf0, 0x1f, 0xf8, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x00, 0x7c, 0x00, 0x7c, 0x03, 0xf8, 0x03, 0xf0, 0x03, 0xf8, 0x00, 0xfc, 0x00, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x1f, 0xf8, 0x1f, 0xf8, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xfc, 0x03, 0xfc, 0x03, 0xfc, 0x07, 0xfc, 0x07, 0xfc, 0x07, 0xfc, 0x0f, 0x7c, 0x0f, 0x7c, 0x0f, 0x7c, 0x1e, 0x7c, 0x1e, 0x7c, 0x3e, 0x7c, 0x3c, 0x7c, 0x3c, 0x7c, 0x7c, 0x7c, 0x7f, 0xff, 0x7f, 0xff, 0x7f, 0xff, 0x7f, 0xff, 0x7f, 0xff, 0x00, 0x7c, 0x00, 0x7c, 0x00, 0x7c, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfc, 0x3f, 0xfc, 0x3f, 0xfc, 0x3e, 0x00, 0x3e, 0x00, 0x3e, 0x00, 0x3e, 0xf0, 0x3f, 0xf8, 0x3f, 0xfc, 0x3e, 0x7c, 0x00, 0x7c, 0x00, 0x7c, 0x00, 0x7c, 0x00, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x1e, 0x7c, 0x1f, 0xf8, 0x0f, 0xf0, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x0f, 0xf8, 0x1f, 0xf8, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x00, 0x3e, 0x00, 0x3e, 0xf0, 0x3f, 0xf8, 0x3f, 0xfc, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x1f, 0xf8, 0x0f, 0xf0, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfc, 0x3f, 0xfc, 0x3f, 0xfc, 0x00, 0x7c, 0x00, 0x7c, 0x00, 0x7c, 0x00, 0x78, 0x00, 0xf8, 0x00, 0xf8, 0x00, 0xf8, 0x00, 0xf0, 0x01, 0xf0, 0x01, 0xf0, 0x01, 0xf0, 0x03, 0xe0, 0x03, 0xe0, 0x03, 0xe0, 0x03, 0xe0, 0x07, 0xc0, 0x07, 0xc0, 0x07, 0xc0, 0x07, 0xc0, 0x0f, 0x80, 0x0f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x07, 0xe0, 0x0f, 0xf0, 0x1f, 0xf8, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x1f, 0xf8, 0x0f, 0xf0, 0x1f, 0xf8, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x1f, 0xf8, 0x1f, 0xf8, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x0f, 0xf0, 0x1f, 0xf8, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3f, 0xfc, 0x1f, 0xfc, 0x0f, 0x7c, 0x00, 0x7c, 0x00, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x1f, 0xf8, 0x1f, 0xf0, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xC0,0x03,0xE0,0x03,0xE0,0x03,0xE0,0x03,0xE0,0x03,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xC0,0x03,0xE0,0x03,0xE0,0x03,0xE0,0x03,0xE0,0x03,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xC0,0x03,0xE0,0x03,0xE0,0x03,0xE0,0x03,0xE0,0x03,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xC0,0x03,0xE0,0x03,0xE0,0x03,0xE0,0x03,0xE0,0x03,0xC0,0x03,0xC0,0x03,0x80,0x03,0x80,0x07,0x00,0x07,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x1E,0x00,0x7E,0x01,0xFE,0x07,0xF8,0x1F,0xE0,0x3F,0x80,0x7E,0x00,0x78,0x00,0x7E,0x00,0x3F,0x80,0x0F,0xF0,0x07,0xFC,0x01,0xFE,0x00,0x7E,0x00,0x1E,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFE,0x7F,0xFE,0x7F,0xFE,0x7F,0xFE,0x3F,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFE,0x7F,0xFE,0x7F,0xFE,0x7F,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x70,0x00,0x7C,0x00,0x7F,0x00,0x3F,0xC0,0x1F,0xE0,0x07,0xF8,0x00,0xFE,0x00,0x3E,0x00,0x1E,0x00,0x7E,0x01,0xFE,0x07,0xF8,0x1F,0xE0,0x7F,0x80,0x7E,0x00,0x78,0x00,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x01,0xC0,0x0F,0xF0,0x1F,0xF8,0x1F,0xFC,0x1E,0x7C,0x18,0x3C,0x00,0x3C,0x00,0x3C,0x00,0x3C,0x00,0x78,0x00,0xF8,0x00,0xF0,0x01,0xE0,0x03,0xC0,0x03,0xC0,0x03,0xC0,0x03,0x80,0x03,0xC0,0x03,0xC0,0x00,0x00,0x00,0x00,0x03,0x80,0x03,0xC0,0x03,0xC0,0x03,0xC0,0x03,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFC,0xFF,0xFC,0xFF,0xFC,0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00
};




TaskHandle_t t1h=NULL;

wifi_config_t sta_config = {
    	        .sta = {




    					 .ssid = "testsssid2",
    	                 .password = "testpasswd2",
    	        		 .bssid_set = false
    	        }
    	    };

const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;

char olddigit[5];

void http_polling();
void led_matrix();
void drawd(int,int,char);


void led_matrix()
{

	while (true){

			scan();


		}



}
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {

    case SYSTEM_EVENT_STA_GOT_IP:





    	break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
    	printf("disoncocted---------\n");



//
    	strcpy((char*)sta_config.sta.ssid,"testssid");
    	strcpy((char*)sta_config.sta.password,"testpsswd");


//
		ESP_ERROR_CHECK( esp_wifi_stop() );
		ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
		ESP_ERROR_CHECK( esp_wifi_start() );
		ESP_ERROR_CHECK( esp_wifi_connect() );



        break;
    default:
        break;
    }
    return ESP_OK;
}



void http_polling()
{	extern uint8_t mask;
extern uint8_t enable;

 	char *REQUEST ="GET /data HTTP/1.1\r\nHost: test.com\r\nConnection: close\r\n\r\n";
	char recv_buf[2000];
	int err = getaddrinfo("test.com", "80", &hints, &res);


 	 while (true) {

			vTaskDelay(400);


			if(err != 0){

				freeaddrinfo(res);
				err = getaddrinfo("test.com", "80", &hints, &res);
				continue;
			}


			bzero(recv_buf,sizeof(recv_buf));


	    	int s = socket(AF_INET, SOCK_STREAM, 0);

	    	if(connect(s, res->ai_addr, res->ai_addrlen)<0){

	    	continue;
	    	}


	    	if(write(s, REQUEST, strlen(REQUEST))<0)
	    	{
	    		close(s);

	    		continue;
	    	}

	    	int full_size=0;
	    	int r=0;
	    	  do {

	    	            r = read(s, recv_buf+full_size, 1000);

	    	       	 full_size+=r;

	    	  } while(r >0);

	    	close(s);

	    	if(full_size>0){
	    		printf("**********************************get_data\n");

	    	char *number=strrchr(recv_buf,'=');

	    	int ds;
	    	if(strlen(number)>6){

	    		 ds=strlen(number)-7-1+1;
	    	}
	    	else{
	    		ds=strlen(number)-1+1;
	    	}

	    	char *substr=malloc(ds);

	    	if(ds){
	    	bzero(substr,ds);
	    	strncpy(substr, number+1,ds-1);



             if(strcmp(olddigit,substr)!=0){


            	 bzero(olddigit,sizeof(olddigit));
            	 strncpy(olddigit,substr,ds-1);

            	 clear();
                 int n=strlen(olddigit);


                 int i=0;
                 int st=(4*16-n*16)/2;

                 while(i<n)
                 {

                   drawd(st,0,olddigit[i]);
                   st+=16;
                   i+=1;
                 }

//                 blinking
                 for(i=0;i<blinktime;i++){

                vTaskDelay(blinkinterval);
                 enable=0;
//                 mask=0xff;

                 vTaskDelay(blinkinterval);
                 enable=0xff;
//                 mask=0x00;


                 }



             }
             free(substr);
	    	}
	    	}



	    };

}

void drawd(int x,int y,char n){


	switch(n)
	{
	case ':':
		n=10;
		break;
	case ';':
		n=11;
		break;
	case '<':
		n=12;
	case '=':
		n=12;
		break;
	case '>':
		n=14;
		break;
	case '?':
		n=15;
		break;
	case '-':
		n=16;
		break;

	default:
 			n=n - '0';
	        break;
	}

	int dsti=y*8+x/8;
	int src=n*64;
	int i=0;
	for (i=0;i<32;i++)
	{
		displaybufg[dsti]=fonts[src];
		displaybufg[dsti+1]=fonts[src+1];
		dsti+=8;
		src+=2;

	}

}


void app_main(void)
{


	 nvs_flash_init();
	    tcpip_adapter_init();
	    	configure(A,B,C,D,OE,R1,R2,STB,CLK);

	   	        memcpy(displaybufg, closed, 256);


	   	    	begin(displaybufg, WIDTH, HEIGHT);




    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );

    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );

    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_connect() );
    xTaskCreatePinnedToCore(led_matrix,"led_matrix",4000,NULL,configMAX_PRIORITIES-1,NULL,1);
    xTaskCreatePinnedToCore(http_polling,"http_polling",10000,NULL,configMAX_PRIORITIES-1,NULL,0);


}

