    #include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TEA_STA.h"

Ttrainsline enstrains;

void initligne(Ttrainsline *enstrains) {
    //Fonction permettant de créer une liste de trains
    enregistrement_train(enstrains, "TGV230", 84);
    enregistrement_train(enstrains, "TER651", 70);
    enregistrement_train(enstrains, "TER967", 54);
    enregistrement_train(enstrains, "TGV324", 48);
    enregistrement_train(enstrains, "TGV900", 36);
    enregistrement_train(enstrains, "TER934", 29);
    enregistrement_train(enstrains, "TER977", 21);
    enregistrement_train(enstrains, "TGV007", 13);
    enregistrement_train(enstrains, "TGV548", 10);
    enregistrement_train(enstrains, "TGV123", 5);

}

int enregistrement_train(Ttrainsline *enstrains, char identifiant[7],
                         int localisation) {
    //La localisation du nouveau train a ajouter ne doit pas depasser la localisation du dernier train sur la ligne.
    TRAIN train;
    int nbTrainListe = enstrains->nbTrains;

    for (int i = 0; i < 6; i++) {
        //Création d'un train avec son identifiant
        train.identifiant[i] = identifiant[i];
    }
    train.localisation = localisation;

    if (nbTrainListe == 0) {
        enstrains->listetrains[0] = train;
        enstrains->listetrains[0].EOA = 100;
        //On traite d'abord le cas ou il n'y a pas de trains dans la liste, dans ce cas on ajoute le train avec sa
        // position et un EOA de 100 qui correspond à  la fin de la ligne
        enstrains->nbTrains = 1;
    } else if (nbTrainListe < 100) {
        int i = 0;
        while (i < nbTrainListe && enstrains->listetrains[i].localisation < localisation) {
            //On cherche le train qui est le plus proche de la localisation souhaitée par valeur infèrieure pour mettre
            // le nouveau train dans la liste de telle sorte que les trains soient toujours classés par ordre croissant de localisation.
            i++;
        }
        if (enstrains->listetrains[i].localisation == localisation) {
            //Si il y a déjà un train a la position demandée alors c'est impossible donc on retourne -1
            return 0;
        }
        for (int j = nbTrainListe; j > i; j--) {
            enstrains->listetrains[j] = enstrains->listetrains[j - 1];
            //On créé un nouvel espace en décalant tout les trains suivants
        }
        enstrains->listetrains[i] = train; //On ajoute le nouveau train
        enstrains->nbTrains += 1;        //On augemente de un le compteur de train dans la liste

        maj_eoa_train(enstrains, enstrains->listetrains[i - 1].identifiant);
        //On met a jour l'EOA du train precedent (c'est le seul affecté)
        maj_eoa_train(enstrains, identifiant);
        //On met à jour l'EOA du train que l'on viens d'ajouter.


    } else {
        return -1;  //Si la liste des trains est remplie on retourne 0
    }

}

int supprimer_train(Ttrainsline *enstrains, char identifiant[7]) {
    for (int i = 0; i <= enstrains->nbTrains; i++) {
        if (strcmp(&identifiant[0], enstrains->listetrains[i].identifiant) == 0) {        //Si le train est bien présent dans la liste
            for (int j = i; j < enstrains->nbTrains; j++) {
                enstrains->listetrains[j] = enstrains->listetrains[j + 1];
                //Alors on le supprime et on décale tout les autres trains dans la liste

            }

            enstrains->nbTrains -= 1; //Il y a un train en moins
            maj_eoa_train(enstrains, enstrains->listetrains[i - 1].identifiant);
            //Maj de l'EOA du train qui précède celui qui viens d'être supprimé.
            return 1;
        }
    }
    return 0;
}

int maj_position_train(Ttrainsline *enstrains, char identifiant[7], int nvPos) {
    for (int i = 0; i <= enstrains->nbTrains; i++) {
        if (strcmp(&identifiant[0], enstrains->listetrains[i].identifiant) ==
            0) {        //Si le train est bien présent dans la liste et si la nouvelle position ne rentre pas en conflit avec son EOA
            if (nvPos > enstrains->listetrains[i].EOA) {
                return 0;
            }
            enstrains->listetrains[i].localisation = nvPos;            //On met a jour la nouvelle position
            return 1;
        }
    }
    return 0; //Pas trouvé


}

int maj_eoa_train(Ttrainsline *enstrains, char identifiant[7]) {

    for (int i = 0; i <= enstrains->nbTrains; i++) {
        if (strcmp(&identifiant[0], enstrains->listetrains[i].identifiant) == 0) {
            enstrains->listetrains[i].EOA = enstrains->listetrains[i + 1].localisation - 1;
            //MAJ de la nouvelle EOA en prenant la position du train suivant.(Cela suppose bien evidemment que les trains sont classés par position).
            return 1;
        }
    }
    return 0; //Retourne 0 si le train n'a pas été trouvé.
}

int eoa_train(Ttrainsline *enstrains, char identifiant[7]) {
    for (int i = 0; i <= enstrains->nbTrains; i++) {
        if (strcmp(&identifiant[0], enstrains->listetrains[i].identifiant) == 0) {
            return enstrains->listetrains[i].EOA; //Retourne l'EOA du train demandé.
        }
    }
    return 0; //Train pas trouvé
}

void imprimer_trains(Ttrainsline *enstrains) {
    printf("\n");
    printf("----------------------------------------------------------------\n");
    for (int i = 0; i < enstrains->nbTrains; i++) {
        printf("Train n°= %i  | ID : %s  |  Localisation : %i  |  EOA : %i  \n", i + 1,
               (*enstrains).listetrains[i].identifiant, (*enstrains).listetrains[i].localisation,
               (*enstrains).listetrains[i].EOA);
    }
    printf("----------------------------------------------------------------");
    printf("\n\nNbTrains : %i\n",
           enstrains->nbTrains);    //Permet d'afficher sous forme de tableaux les trains présents sur la ligne

}

void main(void) {
    char entreec = 0;
    int entree = 0;
    char a = 0;
    char identifiant[7];
    int eoa = 0;
    int localisation = 0;

    initligne(&enstrains);
    imprimer_trains(&enstrains);

    while (1) {
        //printf("%s\n",enstrains.listetrains[0].identifiant);
        //if(supprimer_train(&enstrains,"TGV123")==1){
        //printf("hourra");
        //}
        printf("\n\n");
        printf("Entrez 1 pour ajouter un train \n");
        printf("Entrez 2 pour supprimer un train \n");
        printf("Entrez 3 pour mettre à jour la position d'un train \n");
        printf("Entrez 4 pour mettre à jour l'EOA d'un train \n");
        printf("Entrez 5 pour lire l'EOA d'un train \n");

        fflush(stdin);
        scanf(" %c", &entreec);   //permet de ne récupérer que le nombre donné et pas les espace blancs.
        entree = entreec - 48;    //Conversion en int



        switch (entree) {
            case (1):
                printf("Ajout d'un train, entrer l'identifiant, et la localisation\n");            //Menu qui permet de choisir ce que l'on veut faire
                scanf("%s", &identifiant[0]);
                scanf("%i", &localisation);
                enregistrement_train(&enstrains, identifiant, localisation);
                break;

            case (2):
                printf("Supression d'un train, entrer l'identifiant\n");
                scanf("%s", &identifiant[0]);
                supprimer_train(&enstrains, identifiant);
                break;

            case (3):
                printf("Maj de la position d'un train, entrer l'identifiant puis la nouvelle localisation\n");
                scanf("%s", &identifiant[0]);
                scanf("%i", &localisation);
                maj_position_train(&enstrains, identifiant, localisation);
                break;

            case (4):
                printf("Maj de l'EOA d'un train, entrer l'identifiant du train\n");
                scanf("%s", &identifiant[0]);
                maj_eoa_train(&enstrains, identifiant);
                break;

            case (5):
                printf("Lecture de l'EOA d'un train, entrer l'identifiant du train\n");
                scanf("%s", &identifiant[0]);
                printf("%i", eoa_train(&enstrains, identifiant));
                break;
            default:
                printf("Erreur de saisie\n");
        }
        imprimer_trains(&enstrains);
    }
}







