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

void client_appli(char *serveur, char *service, char *ip);

/*****************************************************************************/
/*--------------- programme client -----------------------*/

int main(int argc, char *argv[])
{

	char *serveur = SERVEUR_DEFAUT; /* serveur par defaut */
	char *service = SERVICE_DEFAUT; /* numero de service par defaut (no de port) */
	char *ip;
	/* Permet de passer un nombre de parametre variable a l'executable */
	switch (argc)
	{
	case 2: /* serveur renseigne  */
		printf("serveur par defaut: %s\n", serveur);
		printf("service par defaut: %s\n", service);
		ip = argv[1];
		break;
	case 3: /* serveur, service renseignes */
		serveur = argv[1];
		printf("service par defaut: %s\n", service);
		ip = argv[2];
		break;
	case 4:
		serveur = argv[1];
		service = argv[2];
		ip = argv[3];
		break;
	default:
		printf("Usage:client serveur(nom ou @IP)  service (nom ou port) ip_local\n");
		exit(1);
	}

	/* serveur est le nom (ou l'adresse IP) auquel le client va acceder */
	/* service le numero de port sur le serveur correspondant au  */
	/* service desire par le client */

	client_appli(serveur, service, ip);
}

/* fonction permettant de vérifier que les couleurs insérer sont biens des couleurs du jeu.*/
int verif_color(char *color, char res)
{
	int color_verif = 0;
	for (int i = 0; i < 8; i++)
	{
		if (res == color[i])
			color_verif = 1;
	}
	if (color_verif == 0)
		return 0;
	return 1;
}

/* fonction permettant de vérifier que le message écris est de la bonne taille*/
int verif_buffer(char *buffer, char *color, char *res, int difficulty)
{
	char c = *buffer;
	int ct = 0;
	int i = 0;
	while (c != '\0')
	{
		if (i == difficulty)
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
	if (i < difficulty)
	{
		printf("\n /!\\Erreurs message trop petit ou éronné\n");
		return 0;
	}
	return 1;
}

void print_color(char *color)
{
	printf("Les couleurs utilisées dans cette partie sont les suivantes : \n");
	for (int i = 0; i < 8; i++)
		printf("%c ", color[i]);
	printf("\n");
}

/*****************************************************************************/
void client_appli(char *serveur, char *service, char *ip)

/* procedure correspondant au traitement du client de votre application */
{
	// Tableau des couleurs disponibles dans la partie

	char *color = (char *)malloc(sizeof(char) * 8);

	// Initialisation des sockets

	struct sockaddr_in *c;
	struct sockaddr_in *s;

	int id_active = h_socket(AF_INET, SOCK_STREAM);

	/* Entrée votre Adresse IP*/
	adr_socket(0, ip, SOCK_STREAM, &c);

	h_bind(id_active, c);

	adr_socket(service, serveur, SOCK_STREAM, &s);

	h_connect(id_active, s);

	char *b_read = (char *)malloc(sizeof(char) * 100);
	char *b_write = (char *)malloc(sizeof(char) * 100);

	// Récupération des couleurs disponibles

	h_reads(id_active, color, 8);

	// Affichage des couleurs disponibles

	print_color(color);

	// Buffer permettant de stocker la réponse du serveur
	char *answer = (char *)malloc(sizeof(char) * 2);

	// Buffer permettant de stocker la réponse du serveur pour les vies
	char *lives_buffer = (char *)malloc(sizeof(char) * 2);

	int lives;

	int difficulty = 0;

	int res_value = -1;

	char *buffer_d = (char *)malloc(sizeof(char) * 100);
	do
	{
		printf("La valeur de la difficulté doit être un entier <= 10\n");
		fgets(buffer_d, 100, stdin);
		difficulty = atoi(&buffer_d[0]);
	} while (difficulty >= 10 || difficulty <= 0);

	// Buffer permettant de stocker le texte du joueur après formattage
	char *buffer_emission = (char *)malloc(sizeof(char) * difficulty);

	char car = difficulty + 40;
	h_writes(id_active, &car, 1);

	// Récupération du nombre de vie
	h_reads(id_active, lives_buffer, 2);

	// Conversion en int du buffer
	lives = atoi(lives_buffer);

	printf("Nombre d'essais pour cette partie : %d \n", lives);

	int value = -1;

	while (lives != 0 && value != difficulty)
	{
		printf("Nombre d'essais restant : %d \n", lives);

		do
		{
			fgets(b_write, 100, stdin);
		} while (!verif_buffer(b_write, color, buffer_emission, difficulty));

		h_writes(id_active, buffer_emission, difficulty);

		h_reads(id_active, answer, 2);

		value = answer[0];

		if (value != difficulty)
		{
			lives--;
			printf("Tu as %d bonne(s) couleur(s) à la bonne position et %d couleur(s) à la mauvaise ! \n", answer[0], answer[1]);
		}
	}

	if (value == difficulty)
		printf("Bravo ! Tu as gagné ! \n");
	else
		printf("T'as perdu, dommage ! \n");

	char *close = (char *)malloc(sizeof(char) * difficulty);
	for (int i = 0; i < difficulty; i++)
	{
		close[i] = '!';
	}

	h_writes(id_active, close, difficulty);

	h_close(id_active);
}

/*****************************************************************************/
