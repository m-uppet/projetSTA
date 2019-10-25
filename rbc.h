
#include <string.h>


typedef struct TRAIN {
    char identifiant[6];
    int EOA;
    int localisation;
} TRAIN;


typedef struct Ttrainsline {
    TRAIN listetrains[100];
    int nbTrains;
} Ttrainsline;

int enregistrement_train(Ttrainsline *enstrains, char identifiant[7], int localisation);
//Cette fonction ajoute un train a la liste des trains sur les rails. Elle renvoie 1 si l'ajout était possible. Si il y a deja 100 trains elle renvoie -1 et si il y a deja un train a la localisation demandéee elle renvoie 0

int supprimer_train(Ttrainsline *enstrains, char identifiant[7]);
//Cette fonction supprime un train de la liste. Elle renvoie 1 si le train a bien été supprimé (présent dans la liste) et 0 sinon.

int maj_position_train(Ttrainsline *enstrains, char identifiant[7], int nvPos);
//Cette fonction met à jour la localisation d'un train dont on spécifié la nouvelle position et l'identifiant
//Renvoie 1 si la MAJ est possible et 0 sinon

int maj_eoa_train(Ttrainsline *enstrains, char identifiant[7]);
//Met à jour l'EOA d'un train dont on a spécifié l'identifiant

int eoa_train(Ttrainsline *enstrains, char identifiant[7]);
//Lit l'EOA d'un train dont on a spécifié l'identifiant

void initligne(Ttrainsline *enstrains);
//Ajout de train dans la structure Ttrainsline (ligne de train) que l'on a déjà créée.
