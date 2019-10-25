
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wiringPi.h>

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>

#include <linux/can.h>
#include <linux/can/raw.h>


/// pour compilation
/// gcc -o Surveillance_arret surveillance_arret.c -lwiringPi

#define PIN_LED2 13
#define PIN_LED3 5
//#define LEGACY
#ifdef LEGACY
	#define PIN_BUMPER 17
#else
	#define PIN_BUMPER 4
#endif

int
main(void)
{
	int s;
	int nbytes;
	struct sockaddr_can addr;
	struct can_frame frame;
	struct ifreq ifr;
	struct can_filter rfilter[1]; // attention a bien adapter la taille du tableau 


	wiringPiSetupGpio () ;
	pinMode (PIN_LED2, OUTPUT) ;
	pinMode (PIN_LED3, OUTPUT) ;
	pinMode (PIN_BUMPER, INPUT) ;
	digitalWrite(PIN_LED2, HIGH);
	const char *ifname = "can0";
	int count;


	if((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) 
	{
		perror("Error while opening socket");
		return -1;
	}

	strcpy(ifr.ifr_name, ifname);
	ioctl(s, SIOCGIFINDEX, &ifr);

	addr.can_family  = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	printf("%s at index %d\n", ifname, ifr.ifr_ifindex);

	if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("Error in socket bind");
		return -2;
	}
	 
	rfilter[0].can_id   = 0x015;
	rfilter[0].can_mask = CAN_SFF_MASK;

	setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter)); 

	// MCP2515
	//Receive buffers, masks and filters:
	//-  Two receive buffers with prioritized message storage
	//-  Six 29-bit filters
	//- Two 29-bit mask

	//To disable the reception of CAN frames on the selected CAN_RAW socket:
	//setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

	while(1)
	{
		if (!digitalRead(PIN_BUMPER))
		{
			digitalWrite(PIN_LED2, LOW);
			frame.can_id  = 0x16;	// consigneVistesseUrgence
			frame.can_dlc = 1;	
			frame.data[0] = 0x00;

			nbytes = write(s, &frame, sizeof(struct can_frame));
			close(s);
			printf("Arret suite a accident\n");	
			system ("sudo shutdown -h now");
		}
		fcntl(s, F_SETFL, fcntl(s, F_GETFL) | O_NONBLOCK);
		char buf[20];
		count = recv(s, buf, sizeof(buf), (MSG_DONTWAIT | MSG_PEEK) );
		printf("%X\n", count);
		if (count)
		{
			read(s,&frame,sizeof(struct can_frame));
			if (frame.can_id==0x015)
			{  
				digitalWrite(PIN_LED2, LOW);
				printf("Arret hardware\n");
				system ("sudo shutdown -h now");
			}
		}
		usleep(10000);
	}
	return 0;
}

