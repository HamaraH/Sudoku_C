#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// STRUCTURES

typedef struct Case {

  int x;
  int y;
} Case;

typedef struct Cand {

  int nbc;
  int* tab;
} Cand;

// ENTETE DES FONCTIONS PRINCIPALES

void lireGrille(int G[][9], char*); // Permet de récupérer une grille depuis le dossier où est stocké le programme
void ecrireGrille(int G[][9]); // Permet d'afficher le contenu du tableau C sous forme de grille
void initJeu(int G[][9], Cand C[][9], Case*, int*); // Permet d'initialiser les principaux éléments du programme: C, O, NBO
int estCand(int, Case, int G[][9]); // Retourne 1 si le chiffre est candidat d'une case, sinon retourne 0
int appartient(int, Cand); // Retourne 1 si un chiffre appartient à une case de candidat, sinon retourne 0s
int estCandUnique(int, Case, Cand C[][9]); // Retourne 1 si le chiffre est candidat est unique, sinon retourne 0
Case rechCaseUnique(Case*, int, Cand C[][9]); // Retourne une case contenant un candidat unique
void fermerCase(Case, Case*, int*, int G[][9], Cand C[][9]); // Permet de fermer une case de les tableaux C et G
void fermerGrille(Case*, int*, int G[][9], Cand C[][9]); // Permet de lancer la fermeture de la grille de sudoku
void ecrireCand(Cand C[][9], int); // Permet d'afficher les candidats contenus dans le tableau C sous forme de grille

// ENTETE DES FONCTIONS SECONDAIRES

void rechRegion(int, int, int*, int*, int*, int*); // Permet de trouver la région on se trouve une case
void supprime_Case(int k, Case*, int* N); // Permet de supprimer une case dans le tableau O
void supprime_int(int k, int*, int* N); // Permet de supprimer un entier dans un tableau d'entier
void entrerGrille(int G[][9]); // Permet d'entrer une grille manuellement, case par case

/*********************** FONCTION MAIN ************************/

int main () {

  int G[9][9];
  Cand C[9][9];
  Case O[81];
  int NBO=0;
  char c, reponse_Utilisateur;
  char nom_Fichiertxt[30];

  printf("\t\t//////////////////////////////////////////////////////////////////////////////////\t\t\n\n");
  printf("\t\t\t\t\tAlgorithme de resolution de Sudoku\n\n");
  printf("\t\t//////////////////////////////////////////////////////////////////////////////////\t\t\n\n");
  printf("\t\t\t\t\t   Que souhaitez vous faire ?\n\n");
  printf("Entrez la lettre 'N' si vous souhaitez entrer un nom de grille a charger\n");
  printf("Entrez la lettre 'E' si vous souhaitez entrer une grille vous meme, case par case\n");
  printf("Entrez n'importe quelle autre caractere pour charger une grille par defaut\n\n");

  printf("Votre choix : ");
  reponse_Utilisateur= getchar();

  while ((c = getchar()) != '\n');        // On vide le buffer

  switch (reponse_Utilisateur) {        // On test la réponse de l'utlisateur
    case 'n':
    case 'N': printf("\nEntrez un nom de grille (maximum 30 caracteres)\n"); scanf("%s", nom_Fichiertxt); lireGrille(G, nom_Fichiertxt); while ((c = getchar()) != '\n');
    break;
    case 'e':
    case 'E': entrerGrille(G);
    break;
    default: printf("\nUne grille par defaut a ete chargee\n"); strcpy(nom_Fichiertxt, "grille_defaut.txt"); lireGrille(G, nom_Fichiertxt);
  }

  printf("\nVoici la grille initiale :\n\n");
  ecrireGrille(G);

  printf("\nEntrez n'importe quel caractere pour commencer l'algorithme\n"); while ((c = getchar()) != '\n');

  initJeu(G, C, O, &NBO);

  fermerGrille(O, &NBO, G, C);

  printf("\nFin du programme, appuyez sur n'importe quelle touche"); getchar();
}

/*********************** FONCTIONS ************************/

// LIRE GRILLE

void lireGrille(int G[][9], char nom[]) {

  FILE * lecture_Grille;
  char ligne[7];
  int i, j, n;
  char nouveau_Nom[30];

  lecture_Grille= fopen(nom, "r");

  while ( lecture_Grille == NULL ) {        // Si le fichier n'a pas été trouvé, on redemande un nom

    printf("Fichier inexistant ! Entrez un nouveau nom de fichier texte\n");
    scanf("%s", nouveau_Nom);
    lecture_Grille= fopen(nouveau_Nom, "r");
  }

  for (i=0; i<9; i++) for (j=0; j<9; j++) G[i][j]= 0;

  while ( fgets(ligne, 7, lecture_Grille) ){

    sscanf(ligne,"%d %d %d", &i, &j, &n );        // On scan une ligne du fichier texte

    G[i][j]= n;       // On stock la valeur de n aux bonnes coordonnées
  }

  fclose(lecture_Grille);
}

// ECRIRE GRILLE

void ecrireGrille(int G[][9]) {

  int i, j;

  printf("-------------------------------\n");

  for (i=0; i<9; i++) {

    if (i%3 == 0 && i != 0) printf("|---------|---------|---------|\n");

    for (j=0; j<9; j++) {

      if (j%3 == 0) printf("|");

      printf(" %d ", G[i][j] );
    }

    printf("|\n");
  }

  printf("-------------------------------\n");
}

// INITIALISATION DU JEU

void initJeu(int G[][9], Cand C[][9], Case O[], int* N) {

  int i, j, n=0;

  // INITIALISATION DU TABLEAU O ET DE NBO

  for (i=0; i<9; i++) {

    for (j=0; j<9; j++) {

      if (G[i][j] == 0) {         // Test d'ouverture de la case

        (O[n]).x= i;
        (O[n]).y= j;

        n++;            // Calcul du nombre de case ouverte
      }
    }
  }

  *N = n;

  // INITIALISATION DU TABLEAU C

  for (i=0; i<9; i++) for(j=0; j<9; j++) {       // De base, toutes les cases ont leur table de candidats initialisées à NULL

    ( C[i][j] ).tab= NULL;
    ( C[i][j] ).nbc= 0;
  }

  for (i=0; i< *N; i++) {

    n=0;

    for (j=1; j<=9; j++) {

      if ( estCand(j, O[i], G) ) n++;
    }

    C[(O[i]).x][(O[i]).y].nbc= n;
    C[(O[i]).x][(O[i]).y].tab = (int*) realloc(C[(O[i]).x][(O[i]).y].tab, sizeof(int)*n );

    for (j=1, n=0; j<=9; j++) if ( estCand(j, O[i], G) ) {

      ( C[(O[i]).x][(O[i]).y] ).tab[n]= j;
      n++;
    }
  }
}

// TEST CANDIDAT

int estCand(int chiffre, Case case_Ouverte, int G[][9]) {

  int i, j;
  int xMin_Region, xMax_Region, yMin_Region, yMax_Region;
  int candidat=1;

  // Test pour savoir si le chiffre est présent dans la colonne ou la ligne de la case ouverte

  for (i=0; i<9; i++) {

    if ( ( G[case_Ouverte.x][i] == chiffre ) || ( G[i][case_Ouverte.y] == chiffre ) ) candidat= 0;
  }

  // Test pour savoir dans quelle région se trouve la case ouverture

  rechRegion(case_Ouverte.x, case_Ouverte.y, &xMin_Region, &xMax_Region, &yMin_Region, &yMax_Region);

  // Test pour savoir si le chiffre est présent dans la région de la case ouverte

  for (i=xMin_Region; i<xMax_Region; i++) {

    for (j=yMin_Region; j<yMax_Region; j++) {

      if ( G[i][j] == chiffre ) candidat= 0;
    }
  }

  return candidat;
}

// TEST D'APPARTENNANCE

int appartient(int chiffre, Cand case_Candidat) {

  int i, appartient=0;

  for (i=0; i<case_Candidat.nbc; i++) {

    if (chiffre == case_Candidat.tab[i]) appartient=1;
  }

  return appartient;
}

// TEST D'UNICITE

int estCandUnique(int candidat, Case case_Ouverte, Cand C[][9]) {

  int i, j, compte_Appartient;
  int xMin_Region, xMax_Region, yMin_Region, yMax_Region;

  rechRegion(case_Ouverte.x, case_Ouverte.y, &xMin_Region, &xMax_Region, &yMin_Region, &yMax_Region);

  // On test si le candidat est l'unique candidat de la case

  if ( C[case_Ouverte.x][case_Ouverte.y].nbc == 1 ) return 1;

  // On test si le candidat est candidat dans une autre case de sa colonne

  compte_Appartient= 0;

  for (i=0; i<9; i++) {

    if (i == case_Ouverte.x ) continue;

    else if ( C[i][case_Ouverte.y].nbc != 0 ) {

      if ( appartient(candidat, C[i][case_Ouverte.y]) == 0 ) {}

      else compte_Appartient=1;
    }
  }

  if (compte_Appartient == 0) return 1;

  // On test si le candidat est candidat dans une autre case de sa ligne

  compte_Appartient= 0;

  for (i=0; i<9; i++) {

    if (i == case_Ouverte.y ) continue;

    else if ( C[case_Ouverte.x][i].nbc != 0 ) {

      if ( appartient(candidat, C[case_Ouverte.x][i]) == 0 ) {}

      else compte_Appartient=1;
    }
  }

  if (compte_Appartient == 0) return 1;

  // On test si le candidat est dans une autre case

  compte_Appartient= 0;

  for (i=xMin_Region; i<xMax_Region; i++) {

    for (j=yMin_Region; j<yMax_Region; j++) {

      if (i == case_Ouverte.x && j == case_Ouverte.y) continue;

      else if ( C[i][j].nbc != 0 ) {

        if ( appartient(candidat, C[i][j]) == 0 ) {}

        else compte_Appartient=1;
      }
    }
  }

  if (compte_Appartient == 0) return 1;

  return 0;
}

// RECHERCHE D'UN CANDIDAT UNIQUE

Case rechCaseUnique(Case O[], int N, Cand C[][9]) {

  int i, j;
  Case null= {-1, -1};

  for (i=0; i<N; i++) {

    for (j=0; j<(C[O[i].x][O[i].y].nbc); j++) {

      if ( estCandUnique( C[O[i].x][O[i].y].tab[j], O[i], C ) ) return O[i];
    }
  }

  return null;
}

// FERMER CASE

void fermerCase(Case case_Ouverte, Case O[], int* N, int G[][9], Cand C[][9]) {

  int i, j, k, candidat;
  int xMin_Region, xMax_Region, yMin_Region, yMax_Region;

  rechRegion(case_Ouverte.x, case_Ouverte.y, &xMin_Region, &xMax_Region, &yMin_Region, &yMax_Region);

  // On supprime de la table O la case que l'on souhaite fermer

  for (i=0; i< *N; i++) {

    if ( (O[i].x == case_Ouverte.x) && (O[i].y == case_Ouverte.y) ) supprime_Case(i, O, N);
  }

  // On cherche le candidat unique de la case

  for (i=0; i<(C[case_Ouverte.x][case_Ouverte.y].nbc); i++) {

    if ( estCandUnique( C[case_Ouverte.x][case_Ouverte.y].tab[i], case_Ouverte, C ) ) candidat= C[case_Ouverte.x][case_Ouverte.y].tab[i];
  }

  // On insère ce candidat à la place du 0 dans la table G

  for (i=0; i<9; i++) for(j=0; j<9; j++) {

    if ( (i == case_Ouverte.x) && (j == case_Ouverte.y) ) {

      G[i][j]= candidat;
    }
  }

  // On supprime ce candidat de la liste des candidats de toutes les autres cases voisines

  // Tout d'abord les cases voisines de la ligne

  for (i=0; i<9; i++) {

    for (k=0; k<(C[case_Ouverte.x][i].nbc); k++) {

      if ( C[case_Ouverte.x][i].tab[k] == candidat ) supprime_int(k, C[case_Ouverte.x][i].tab, &C[case_Ouverte.x][i].nbc);
    }
  }

  // Puis pour les cases voisines de la colonne

  for (j=0; j<9; j++) {

    for (k=0; k<(C[j][case_Ouverte.y].nbc); k++) {

      if ( C[j][case_Ouverte.y].tab[k] == candidat ) supprime_int(k, C[j][case_Ouverte.y].tab, &C[j][case_Ouverte.y].nbc);
    }
  }

  // Et enfin pour les cases voisines de la région

  for (i=xMin_Region; i<xMax_Region; i++) for (j=yMin_Region; j<yMax_Region; j++) {

    for (k=0; k<(C[i][j].nbc); k++) {

      if ( C[i][j].tab[k] == candidat ) supprime_int(k, C[i][j].tab, &C[i][j].nbc);
    }
  }

  // On ferme la case dans la table C et on libère son espace mémoire

  C[case_Ouverte.x][case_Ouverte.y].nbc= 0;
  free( C[case_Ouverte.x][case_Ouverte.y].tab );
  C[case_Ouverte.x][case_Ouverte.y].tab= NULL;
}

// FERMER GRILLE

void fermerGrille(Case O[], int* N, int G[][9], Cand C[][9]) {

	printf("\n\n//////// Resolution de la grille avec la methode des eliminations des candidats uniques ////////\n\n");

	ecrireCand(C, *N);

  while ( *N != 0 ) {

    Case case_Fermee;

    case_Fermee= rechCaseUnique(O, *N, C);

    if ( case_Fermee.x == -1 ) {

      printf("\nCette grille est une grille de difficultee trop elevee !\nLa methode des candidats uniques ne permet pas de la resoudre\n");
      ecrireCand(C, *N);
      return;
    }

    fermerCase(case_Fermee, O, N, G, C);

    printf("\nLa case (%d, %d) est fermee avec le chiffre %d\n\nVoici la nouvelle grille :\n\n", case_Fermee.x, case_Fermee.y,  G[case_Fermee.x][case_Fermee.y]);

    ecrireGrille(G);
    ecrireCand(C, *N);
  }

  printf("\n\n\n//////// Voici la grille une fois remplie ////////\n\n\n\n");
  ecrireGrille(G);
}

// ECRIRE CANDIDATS

void ecrireCand(Cand C[][9], int N) {

  int i,j,v,w;

  printf("\nVoici les candidats des %d cases ouvertes de la grille :\n\n", N );

  //Boucle des lignes

  for(i=0;i<9;i++){

  if(i%3 == 0) printf("****************************************************************************************************\n");

  else printf("*--------------------------------*--------------------------------*--------------------------------*\n");

  //Boucle des colonnes

    for(j=0;j<9;j++){

      if (j==0)printf("*");
      if (C[i][j].nbc==0)printf("         ");       // Cas où la case est déjà fermée

      else{

        for(w=0;w<9-C[i][j].nbc;w++) printf(" ");     // On ajoute des espaces pour que toutes les cases aient une largeur de 9
        for(v=0;v<C[i][j].nbc;v++) printf("%d",C[i][j].tab[v]); // Cas d'une case ouverte
      }

      printf(" ");
      if (j%3 == 2)printf("*");
      else printf("|");
    }

  printf("\n");

  }
  printf("****************************************************************************************************\n");
}

// RECHERCHE REGION

void rechRegion(int x, int y, int* xmin, int* xmax, int* ymin, int* ymax) {

  switch (x) {
    case 0:
    case 1:
    case 2: {

      *xmin=0; *xmax=3;

      switch (y) {
        case 0:
        case 1:
        case 2: *ymin=0; *ymax=3;
          break;
        case 3:
        case 4:
        case 5: *ymin=3; *ymax=6;
          break;
        case 6:
        case 7:
        case 8: *ymin=6; *ymax=9;
          break;
      }
    }
      break;
    case 3:
    case 4:
    case 5: {

      *xmin=3; *xmax=6;

      switch (y) {
        case 0:
        case 1:
        case 2: *ymin=0; *ymax=3;
          break;
        case 3:
        case 4:
        case 5: *ymin=3; *ymax=6;
          break;
        case 6:
        case 7:
        case 8: *ymin=6; *ymax=9;
          break;
      }
    }
      break;
    case 6:
    case 7:
    case 8: {

      *xmin=6; *xmax=9;

      switch (y) {
        case 0:
        case 1:
        case 2: *ymin=0; *ymax=3;
          break;
        case 3:
        case 4:
        case 5: *ymin=3; *ymax=6;
          break;
        case 6:
        case 7:
        case 8: *ymin=6; *ymax=9;
          break;
      }
    }
      break;
  }
}

// SUPPRIMER UN TYPE CASE DANS UN TABLEAU

void supprime_Case(int k, Case O[], int* N) {

  int i, j;

  for (i= *N-1; i>=0; i--) {

    if ( i == k ) {

      for (j=i; j< *N; j++) O[j]= O[j+1];

      -- *N;
    }
  }
}

// SUPPRIMER UN TYPE INT DANS UN TABLEAU

void supprime_int(int k, int* T, int* N) {

  int i, j;

  for (i= *N-1; i>=0; i--) {

    if ( i == k ) {

      for (j=i; j< *N; j++) T[j]= T[j+1];

      -- *N;
    }
  }
}

// ENTRER UNE GRILLE

void entrerGrille(int G[][9]) {

  printf("Pour marquer une case ouverte, utilisez le chiffre 0\n\n");

  int i, j, n;

  for (i=0; i<9; i++) for (j=0; j<9; j++) {

    do {
      printf("Entrez le contenu (chiffre entre 0 et 9) de la case (%d, %d)\n", i, j);
      scanf("%d", &n);
    } while (n<0 || n>9);

    G[i][j]= n;
  }
}
