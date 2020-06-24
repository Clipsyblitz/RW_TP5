/******************************************************************************/
/*			Application: ....			              */
/******************************************************************************/
/*									      */
/*			 programme  SERVEUR 				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs :  ....						      */
/*		Date :  ....						      */
/*									      */
/******************************************************************************/

#include <stdio.h>
#include <curses.h>

#include <sys/signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "fon.h" /* Primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"

void serveur_appli(char *service); /* programme serveur */

/******************************************************************************/
/*---------------- programme serveur ------------------------------*/

int main(int argc, char *argv[])
{

	char *service = SERVICE_DEFAUT; /* numero de service par defaut */

	/* Permet de passer un nombre de parametre variable a l'executable */
	switch (argc)
	{
	case 1:
		printf("defaut service = %s\n", service);
		break;
	case 2:
		service = argv[1];
		break;

	default:
		printf("Usage:serveur service (nom ou port) \n");
		exit(1);
	}

	/* service est le service (ou numero de port) auquel sera affecte
	ce serveur*/

	serveur_appli(service);
}

void interprete_answer(char *code, char *receive, char *answer, int difficulty)
{
	if (receive[0] != '!')
	{

		int ok_couleur = 0, ok_place = 0, couleur_presente = 0;
		for (int i = 0; i < difficulty; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				if (i == j && code[i] == receive[i])
				{
					ok_place++;
					if (couleur_presente)
						ok_couleur--;
					couleur_presente = 1;
				}
				else if (couleur_presente != 1 && receive[i] == code[j])
				{
					couleur_presente = 1;
					ok_couleur++;
				}
			}
			couleur_presente = 0;
		}

		answer[1] = ok_couleur;
		answer[0] = ok_place;
	}
	else
		answer[0] = '@';
}

void init_code(char *code, char *color, int difficulty)
{
	for (int i = 0; i < difficulty; i++)
	{
		code[i] = color[rand() % 5];
	}
}

/******************************************************************************/
void serveur_appli(char *service)

/* Procedure correspondant au traitemnt du serveur de votre application */

{
	// Tableau des couleurs disponibles dans la partie

	srand(time(NULL));

	char color[8] = {'r', 'v', 'B', 'b', 'j', 'o', 'V', 'f'};

	// Initialisation des sockets

	struct sockaddr_in *s;

	int id_passive = h_socket(AF_INET, SOCK_STREAM);

	adr_socket(service, NULL, SOCK_STREAM, &s);

	h_bind(id_passive, s);

	h_listen(id_passive, 5);

	int id_active = h_accept(id_passive, s);

	char *buffer = (char *)malloc(sizeof(char));

	// Envoie des couleurs disponibles au client

	h_writes(id_active, color, 8);

	// Récupération de la difficulté

	h_reads(id_active, buffer, 1);
	int difficulty = *buffer - 40;
	// Envoie du nombre de vies au client

	h_writes(id_active, "10", 2);

	// Initialisation du code
	
	char code[difficulty];
	init_code(code, color, difficulty);

	// Récupération des message et interpretation

	char receive[difficulty];
	char answer[2];

	while (receive[0] != '!')
	{
		h_reads(id_active, receive, difficulty);

		interprete_answer(code, receive, answer, difficulty);

		if (answer[0] != '@')
		{
			h_writes(id_active, answer, 2);
		}
	}

	// Fermeture des sockets

	h_close(id_passive);
	h_close(id_active);
}

/******************************************************************************/
