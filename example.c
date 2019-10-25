


#include "Libs_Unirail/CAN/canLinux.h"
#include "Libs_Unirail/CAN/MESCAN1_VarTrain.h"
#include "Libs_Unirail/CAN/MESCAN1_MAIN_PARAMS.h"

#define ID_balises_3 MC_ID_MESSAGE_EBT_RECEIVED+3


#define DISTANCE_PARCOURS 50

typedef struct TrainInfo
{
	float distance;  // en cm
	float vit_consigne;
	float vit_mesuree;
	int nb_impulsions;
}TrainInfo;

unsigned char status, varDebug1, varDebug2;

//////////////////////////////////////////
/// Ecriture de la Trame vitesse limite
///////////////////////////////////////////
int WriteVitesseLimite(float vitesseLimite)
{

	char *ifname = "can0";
	int portNumber = canLinux_Init(ifname);
	uCAN1_MSG consigneUrgence;
	
	if(vitesseLimite > MAX_CONSIGNE_VITESSE_AUTORISEE) //vitesse supérieur à 50 cm/s
	vitesseLimite = MAX_CONSIGNE_VITESSE_AUTORISEE;                   
	
	consigneUrgence.frame.id  = MC_ID_CONSIGNE_VITESSE_LIMITE;
	consigneUrgence.frame.dlc = MC_DLC_CONSIGNE_VITESSE_LIMITE;
	MESCAN_SetData8(&consigneUrgence, cdmc_consigneVitesseLimite, vitesseLimite);
	
	ECAN1_transmit(portNumber, CANLINUX_PRIORITY_HIGH, &consigneUrgence);
	close(portNumber);
	return 1;
}

//////////////////////////////////////////
/// Ecriture de la Trame Consigne
///////////////////////////////////////////
int WriteVitesseConsigne(unsigned int vitesse, unsigned char sens)
{
	
	char *ifname = "can0";
	int portNumber = canLinux_Init(ifname);
	uCAN1_MSG consigneVitesse;

	if(vitesse>MAX_CONSIGNE_VITESSE_AUTORISEE) //vitesse supérieur à 50 cm/s
		vitesse = MAX_CONSIGNE_VITESSE_AUTORISEE;
	
	consigneVitesse.frame.id  = MC_ID_CONSIGNE_VITESSE;
	consigneVitesse.frame.dlc = MC_DLC_CONSIGNE_VITESSE;
	MESCAN_SetData8(&consigneVitesse, cdmc_consigneVitesse, vitesse);
	MESCAN_SetBit(&consigneVitesse, bdmc_sensDeplacementLoco, sens);
	
	ECAN1_transmit(portNumber, CANLINUX_PRIORITY_HIGH, &consigneVitesse);
	close(portNumber);
	return 1;
}


//////////////////////////////////////////
/// Envoi de la trame status de RPI1
///////////////////////////////////////////
int  WriteTrameStatusRUNRP1(unsigned char status, unsigned char varDebug1, unsigned char varDebug2)
{
	char *ifname = "can0";
	int portNumber = canLinux_Init(ifname);
	uCAN1_MSG trameStatusRP1;
	trameStatusRP1.frame.id  = MC_ID_RP1_STATUS_RUN;
	trameStatusRP1.frame.dlc = MC_DLC_RP1_STATUS_RUN;
	MESCAN_SetData8(&trameStatusRP1, cdmc_RP1_erreurs, 0);
	MESCAN_SetData8(&trameStatusRP1, cdmc_RP1_warnings, 0);
	MESCAN_SetBit(&trameStatusRP1, bdmc_RP1_etatConnexionWIFI, 1);
	MESCAN_SetData8(&trameStatusRP1, cdmc_RP1_configONBOARD, status);
	MESCAN_SetData8(&trameStatusRP1, cdmc_RP1_var1Debug, varDebug1);
	MESCAN_SetData8(&trameStatusRP1, cdmc_RP1_var2Debug, varDebug2);
	
	ECAN1_transmit(portNumber, CANLINUX_PRIORITY_HIGH, &trameStatusRP1);
	close(portNumber);
	return 1;	
}

//////////////////////////////////////////
/// Identification de la trame CAN
///////////////////////////////////////////
void TraitementDonnee (uCAN1_MSG *recCanMsg, TrainInfo *infos)
{
	
    switch (recCanMsg->frame.id)
    {
		/** Recuperer Info BALISE  **/
		case ID_balises_3 :
        //MAJ_Info_BALISE (XXX);
        break ;
		case MC_ID_MESSAGE_GSM_RECEIVED:
        //MAJ_GSMR(XXX);
        break;
		case MC_ID_SCHEDULEUR_MESURES  : /** Envoi la vitesse instantannée (consigne vitesse) ,	le nombre d’impulsions, la vitesse mesurée, l’erreur du PID **/
		if(MESCAN_GetData8(recCanMsg, cdmc_ordonnancementId)==MC_ID_RP1_STATUS_RUN)
			WriteTrameStatusRUNRP1(status, varDebug1, varDebug2);
        infos-> vit_mesuree= (int)MESCAN_GetData8(recCanMsg, cdmc_vitesseMesuree);/** le nbre d'implusion envoyé ici
		// est le nombre d'impulsion entre 2 mesures **/
		infos-> nb_impulsions+= infos-> vit_mesuree;
        infos-> distance= PAS_ROUE_CODEUSE * (infos->nb_impulsions);
		infos-> vit_consigne= (float)MESCAN_GetData8(recCanMsg, cdmc_vitesseConsigneInterne);
		printf("Actualisation: Postition: %lf, Vit: %lf.\n", infos-> distance, infos-> vit_mesuree);
		break;
		
		default :
		printf("frame est de can id %X \n",recCanMsg->frame.id);
		break;
	}
}

////////////////////////////////////////
/// MAIN
////////////////////////////////////////
int main(void)
{
	uCAN1_MSG recCanMsg;
	int fd;
	struct ifreq ifr;
	struct TrainInfo train1;
	int canPort;
	char *NomPort = "can0";
	struct can_filter rfilter[1]; 
	rfilter[0].can_id   = 0x02F;
	rfilter[0].can_mask = CAN_SFF_MASK;

	train1.distance =0;
	train1.vit_consigne =0;
	train1.vit_mesuree =0;
	train1.nb_impulsions =0;
	
 
    printf("je suis dans main \n");

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want IP address attached to "wlan1" */
    strncpy(ifr.ifr_name, "wlan1", IFNAMSIZ-1);
    ioctl(fd, SIOCGIFADDR, &ifr);
    close(fd);

    /* display result */
    printf("mon IP WLAN1 : %s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

    /* Start CAN bus connexion */
    canPort = canLinux_Init(NomPort);
	canLinux_InitFilter(canPort, rfilter, sizeof(rfilter));
	/* Unlock speed limits */
    WriteVitesseLimite(MAX_CONSIGNE_VITESSE_AUTORISEE);
    usleep(150000); //150ms
	
    while(train1.distance < DISTANCE_PARCOURS)
    {
		if(ECAN1_receive(canPort, &recCanMsg))
		{
			printf("Lecture trame CAN.\n");
			TraitementDonnee(&recCanMsg, &train1);
			WriteVitesseConsigne(((DISTANCE_PARCOURS-train1.distance)/2)+1, 1);
			
		}
    usleep(15000); //Sampling period ~= 17ms
	}
	WriteVitesseConsigne(0, 1);

    close(canPort);
	printf("EXIT\n");


    return 0;
}

