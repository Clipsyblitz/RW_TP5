/******************************************************************************/
/*			Application: ...					*/
/******************************************************************************/
/*									      */
/*			 programme  CLIENT				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs : ... 					*/
/*									      */
/******************************************************************************/

#include <stdio.h>
#include <curses.h> /* Primitives de gestion d'ecran */
#include <sys/signal.h>
#include <sys/wait.h>
#include <stdlib.h>

#include "fon.h" /* primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"
#define SERVEUR_DEFAUT "0.0.0.0"

void client_appli(char *serveur, char *service);

/*****************************************************************************/
/*--------------- programme client -----------------------*/

int main(int argc, char *argv[])
{

	char *serveur = SERVEUR_DEFAUT; /* serveur par defaut */
	char *service = SERVICE_DEFAUT; /* numero de service par defaut (no de port) */

	/* Permet de passer un nombre de parametre variable a l'executable */
	switch (argc)
	{
	case 1: /* arguments par defaut */
		printf("serveur par defaut: %s\n", serveur);
		printf("service par defaut: %s\n", service);
		break;
	case 2: /* serveur renseigne  */
		serveur = argv[1];
		printf("service par defaut: %s\n", service);
		break;
	case 3: /* serveur, service renseignes */
		serveur = argv[1];
		service = argv[2];
		break;
	default:
		printf("Usage:client serveur(nom ou @IP)  service (nom ou port) \n");
		exit(1);
	}

	/* serveur est le nom (ou l'adresse IP) auquel le client va acceder */
	/* service le numero de port sur le serveur correspondant au  */
	/* service desire par le client */

	client_appli(serveur, service);
}

/* fonction permettant de vérifier que les couleurs insérer sont biens des couleurs du jeu.*/
int verif_color(char *color, char res)
{
	int color_verif = 0;
	for (int i = 0; i < 5; i++)
	{
		if (res == color[i])
			color_verif = 1;
	}
	if (color_verif == 0)
		return 0;
	return 1;
}

/* fonction permettant de vérifier que le message écris est de la bonne taille*/
int verif_buffer(char *buffer, char *res, char *color)
{
	char c = *buffer;
	int ct = 0;
	int i = 0;
	while (c != '\0')
	{
		if (i == 5)
		{
			return 1;
		}
		if (verif_color(color, c))
		{
			res[i] = c;
			i++;
		}
		ct++;
		c = *(buffer + ct);
	}
	if (i < 5)
	{
		printf("\n /!\\Erreurs message trop petit\n");
		return 0;
	}
	return 1;
}

/* fonction appelée pour vérifier la conformité du message à envoyé.*/
int verif_entry(char *buffer, char *color, char *res)
{
	return verif_buffer(buffer, res, color);
}

void print_color(char *color)
{
	printf("Les couleurs utilisées dans cette partie sont les suivantes : \n");
	for (int i = 0; i < 5; i++)
		printf(" %c,", color[i]);
	printf("\n");
}

/*****************************************************************************/
void client_appli(char *serveur, char *service)

/* procedure correspondant au traitement du client de votre application */
{
	// Tableau des couleurs disponibles dans la partie

	char *color = (char *)malloc(sizeof(char) * 5);

	// Initialisation des sockets

	struct sockaddr_in *c;
	struct sockaddr_in *s;

	int id_active = h_socket(AF_INET, SOCK_STREAM);

	adr_socket(0, "192.168.1.13", SOCK_STREAM, &c);

	h_bind(id_active, c);

	adr_socket(service, serveur, SOCK_STREAM, &s);

	h_connect(id_active, s);

	char *b_read = (char *)malloc(sizeof(char) * 100);
	char *b_write = (char *)malloc(sizeof(char) * 100);

	// Récupération des couleurs disponibles

	h_reads(id_active, color, 5);

	// Affichage des couleurs disponibles

	print_color(color);

	char *buffer_emission = (char *)malloc(sizeof(char) * 5);
	char *answer = (char *)malloc(sizeof(char) * 2);
	char *lives_buffer = (char *)malloc(sizeof(char) * 2);

	int lives;

	h_reads(id_active, lives_buffer, 2);

	lives = atoi(lives_buffer);

	printf("Nombre d'essais pour cette partie : %d \n", lives);

	int value = -1;

	while (lives != 0 && value != 5)
	{
		printf("Nombre d'essais restant : %d \n", lives);

		do
		{
			fgets(b_write, 15, stdin);
		} while (!verif_entry(b_write, color, buffer_emission));

		h_writes(id_active, buffer_emission, 5);

		h_reads(id_active, answer, 2);

		value = answer[1];

		if (value != 5)
		{
			lives--;
			printf("Tu as %d bonne(s) couleur(s) et %d couleur(s) à la bonne position ! \n", answer[0], answer[1]);
		}
	}

	if (value == 5)
		printf("Bravo ! Tu as gagné ! \n");
	else
		printf("T'as perdu, dommage ! \n");

	h_writes(id_active, "!!!!!", 5);

	h_close(id_active);
}

/*****************************************************************************/
