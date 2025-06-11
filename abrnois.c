/*
 * abrnois.c
 * Implémentation d'un arbre ABRnois pour la génération d'une liste de mots fréquents
 * Auteurs : Vincent PLESSY
 * Date : 2 Mai 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

typedef struct _noeud
{
    char *mot;
    int nb_occ;
    struct _noeud *fg, *fd;
} Noeud, *ABRnois;

typedef struct _cell
{
    Noeud *n;
    struct _cell *suivant;
} Cell, *Liste;

/*
 * Fonction d'allocation d'un nouveau noeud
 * Entrée: mot à insérer
 * Sortie: pointeur vers le nouveau noeud alloué
 */
Noeud *alloue_noeud(char *mot)
{
    Noeud *nouveau = (Noeud *)malloc(sizeof(Noeud));
    if (nouveau == NULL)
    {
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        return NULL;
    }

    nouveau->mot = strdup(mot);
    if (nouveau->mot == NULL)
    {
        fprintf(stderr, "Erreur d'allocation mémoire pour le mot\n");
        free(nouveau);
        return NULL;
    }

    nouveau->nb_occ = 1;
    nouveau->fg = NULL;
    nouveau->fd = NULL;

    return nouveau;
}

/*
 * Fonction de libération de la mémoire d'un noeud
 * Entrée: pointeur vers le noeud à libérer
 */
void libere_noeud(Noeud *n)
{
    if (n != NULL)
    {
        free(n->mot);
        free(n);
    }
}

/*
 * Fonction de libération de la mémoire d'un arbre
 * Entrée: pointeur vers la racine de l'arbre
 */
void libere_arbre(ABRnois A)
{
    if (A != NULL)
    {
        libere_arbre(A->fg);
        libere_arbre(A->fd);
        libere_noeud(A);
    }
}

/*
 * Fonction de libération de la mémoire d'une liste
 * Entrée: pointeur vers la tête de liste
 */
void libere_liste(Liste L)
{
    Cell *courant = L;
    Cell *suivant;

    while (courant != NULL)
    {
        suivant = courant->suivant;
        free(courant);
        courant = suivant;
    }
}

/*
 * Fonction récursive pour calculer la hauteur d'un arbre
 * Entrée: pointeur vers la racine de l'arbre
 * Sortie: hauteur de l'arbre
 */
int hauteur_arbre(ABRnois A)
{
    if (A == NULL)
        return 0;

    int hauteur_gauche = hauteur_arbre(A->fg);
    int hauteur_droite = hauteur_arbre(A->fd);

    return 1 + (hauteur_gauche > hauteur_droite ? hauteur_gauche : hauteur_droite);
}

/*
 * Fonction pour écrire un noeud et ses fils dans un fichier DOT
 * Entrée: fichier DOT, pointeur vers le noeud, identifiant du noeud
 */
void ecrire_noeud_dot(FILE *f, ABRnois A, int id)
{
    if (A == NULL)
        return;

    // Écriture du noeud
    fprintf(f, "  node%d [label=\"{%s|%d}\"];\n", id, A->mot, A->nb_occ);

    // Écriture des liens vers les fils
    if (A->fg != NULL)
    {
        fprintf(f, "  node%d -> node%d;\n", id, 2 * id);
        ecrire_noeud_dot(f, A->fg, 2 * id);
    }

    if (A->fd != NULL)
    {
        fprintf(f, "  node%d -> node%d;\n", id, 2 * id + 1);
        ecrire_noeud_dot(f, A->fd, 2 * id + 1);
    }
}

/*
 * Fonction pour générer une représentation graphique d'un ABRnois
 * Entrée: nom du fichier pdf et pointeur vers la racine de l'arbre
 * Sortie: 1 si succès, 0 sinon
 */
int exporte_arbre(char *nom_pdf, ABRnois A)
{
    if (A == NULL)
        return 0;

    char nom_dot[256];
    snprintf(nom_dot, sizeof(nom_dot), "%s.dot", nom_pdf);

    FILE *f = fopen(nom_dot, "w");
    if (f == NULL)
    {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier %s\n", nom_dot);
        return 0;
    }

    // Écriture de l'en-tête du fichier DOT
    fprintf(f, "digraph ABRnois {\n");
    fprintf(f, "  node [shape=record, height=.1];\n");

    // Écriture des noeuds et des liens
    ecrire_noeud_dot(f, A, 1);

    // Fermeture du graphe
    fprintf(f, "}\n");
    fclose(f);

    // Génération du PDF avec Graphviz
    char commande[512];
    snprintf(commande, sizeof(commande), "dot -Tpdf %s -o %s", nom_dot, nom_pdf);

    int resultat = system(commande);
    if (resultat != 0)
    {
        fprintf(stderr, "Erreur lors de la génération du PDF. Vérifiez que Graphviz est installé.\n");
        return 0;
    }

    // Suppression du fichier DOT intermédiaire
    remove(nom_dot);

    return 1;
}

/*
 * Fonction pour effectuer une rotation gauche sur un arbre
 * Entrée: double pointeur vers la racine de l'arbre
 */
void rotation_gauche(ABRnois *A)
{
    if (*A == NULL || (*A)->fd == NULL)
        return;

    Noeud *pivot = (*A)->fd;
    (*A)->fd = pivot->fg;
    pivot->fg = *A;
    *A = pivot;
}

/*
 * Fonction pour effectuer une rotation droite sur un arbre
 * Entrée: double pointeur vers la racine de l'arbre
 */
void rotation_droite(ABRnois *A)
{
    if (*A == NULL || (*A)->fg == NULL)
        return;

    Noeud *pivot = (*A)->fg;
    (*A)->fg = pivot->fd;
    pivot->fd = *A;
    *A = pivot;
}

/*
 * Fonction récursive pour insérer un mot dans un ABRnois
 * Entrée: double pointeur vers la racine de l'arbre et mot à insérer
 * Sortie: 1 si succès, 0 sinon
 */
int insert_ABRnois(ABRnois *A, char *mot)
{
    // Cas d'un arbre vide
    if (*A == NULL)
    {
        *A = alloue_noeud(mot);
        return (*A != NULL) ? 1 : 0;
    }

    // Comparaison des mots
    int cmp = strcmp(mot, (*A)->mot);

    // Cas où le mot est déjà présent dans l'arbre
    if (cmp == 0)
    {
        (*A)->nb_occ += 1;
        return 1;
    }

    // Insertion dans le sous-arbre gauche
    if (cmp < 0)
    {
        int resultat = insert_ABRnois(&((*A)->fg), mot);
        if (resultat == 0)
            return 0;

        // Vérification de la condition d'arbre tournois
        if ((*A)->fg->nb_occ > (*A)->nb_occ)
        {
            rotation_droite(A);
        }

        return 1;
    }

    // Insertion dans le sous-arbre droit
    else
    {
        int resultat = insert_ABRnois(&((*A)->fd), mot);
        if (resultat == 0)
            return 0;

        // Vérification de la condition d'arbre tournois
        if ((*A)->fd->nb_occ > (*A)->nb_occ)
        {
            rotation_gauche(A);
        }

        return 1;
    }
}

/*
 * Fonction pour faire descendre un noeud dans l'arbre (pour la suppression)
 * Entrée: double pointeur vers la racine de l'arbre et valeur à descendre
 * Sortie: 1 si le noeud a été trouvé et est une feuille, 0 sinon
 */
int descendre_noeud(ABRnois *A, char *mot)
{
    if (*A == NULL)
        return 0;

    int cmp = strcmp(mot, (*A)->mot);

    // Noeud trouvé
    if (cmp == 0)
    {
        // Si c'est une feuille, on a terminé
        if ((*A)->fg == NULL && (*A)->fd == NULL)
        {
            return 1;
        }

        // Si un seul enfant, pas de choix sur la rotation
        if ((*A)->fg == NULL)
        {
            rotation_gauche(A);
            return descendre_noeud(A, mot);
        }

        if ((*A)->fd == NULL)
        {
            rotation_droite(A);
            return descendre_noeud(A, mot);
        }

        // Deux enfants, on remonte celui de priorité maximale
        if ((*A)->fg->nb_occ > (*A)->fd->nb_occ)
        {
            rotation_droite(A);
        }
        else
        {
            // En cas d'égalité, on privilégie la rotation gauche
            rotation_gauche(A);
        }

        return descendre_noeud(A, mot);
    }

    // Recherche dans le sous-arbre approprié
    if (cmp < 0)
    {
        return descendre_noeud(&((*A)->fg), mot);
    }
    else
    {
        return descendre_noeud(&((*A)->fd), mot);
    }
}

/*
 * Fonction pour supprimer un noeud de l'arbre
 * Entrée: double pointeur vers la racine de l'arbre et mot à supprimer
 * Sortie: 1 si succès, 0 sinon
 */
int supprime_ABRnois(ABRnois *A, char *mot)
{
    if (*A == NULL)
        return 0;

    // On fait descendre le noeud pour qu'il devienne une feuille
    if (!descendre_noeud(A, mot))
    {
        return 0; // Le mot n'a pas été trouvé
    }

    // Recherche du noeud à supprimer (qui est maintenant une feuille)
    ABRnois *parent = A;
    int cmp;

    while (*parent != NULL)
    {
        cmp = strcmp(mot, (*parent)->mot);

        if (cmp == 0)
            break;

        if (cmp < 0)
        {
            parent = &((*parent)->fg);
        }
        else
        {
            parent = &((*parent)->fd);
        }
    }

    if (*parent == NULL)
        return 0; // Le mot n'a pas été trouvé

    // Suppression de la feuille
    Noeud *temp = *parent;
    *parent = NULL;
    libere_noeud(temp);

    return 1;
}

/*
 * Fonction pour insérer un noeud dans une liste triée alphabétiquement
 * Entrée: double pointeur vers la tête de liste et noeud à insérer
 * Sortie: 1 si succès, 0 sinon
 */
int insere_dans_liste_triee(Liste *L, Noeud *n)
{
    Cell *nouvelle = (Cell *)malloc(sizeof(Cell));
    if (nouvelle == NULL)
    {
        return 0;
    }

    nouvelle->n = n;

    // Cas de la liste vide ou insertion en tête
    if (*L == NULL || strcmp(n->mot, (*L)->n->mot) < 0)
    {
        nouvelle->suivant = *L;
        *L = nouvelle;
        return 1;
    }

    // Recherche de la position d'insertion
    Cell *courant = *L;
    while (courant->suivant != NULL && strcmp(n->mot, courant->suivant->n->mot) >= 0)
    {
        courant = courant->suivant;
    }

    nouvelle->suivant = courant->suivant;
    courant->suivant = nouvelle;

    return 1;
}

/*
 * Fonction récursive pour extraire les noeuds de priorité maximale
 * Entrée: double pointeur vers la racine, double pointeur vers la liste, priorité max, pointeur vers le compteur
 * Sortie: 0 si échec, 1 si succès
 */
int extraire_noeuds_max_rec(ABRnois *A, Liste *lst, int priorite_max, int *nb_extraits)
{
    if (*A == NULL)
        return 1;

    // Extraction dans le sous-arbre gauche
    if (!extraire_noeuds_max_rec(&((*A)->fg), lst, priorite_max, nb_extraits))
    {
        return 0;
    }

    // Vérification de la priorité du noeud courant
    if ((*A)->nb_occ == priorite_max)
    {
        Noeud *n = *A;

        // Suppression du noeud de l'arbre
        if (n->fg == NULL && n->fd == NULL)
        {
            // Cas d'une feuille
            *A = NULL;
        }
        else if (n->fg == NULL)
        {
            // Cas d'un noeud avec un seul enfant droit
            *A = n->fd;
        }
        else if (n->fd == NULL)
        {
            // Cas d'un noeud avec un seul enfant gauche
            *A = n->fg;
        }
        else
        {
            // Cas d'un noeud avec deux enfants
            // On remplace le noeud par son successeur inorder
            ABRnois *succ = &(n->fd);
            while ((*succ)->fg != NULL)
            {
                succ = &((*succ)->fg);
            }

            char *temp_mot = n->mot;
            int temp_occ = n->nb_occ;

            n->mot = (*succ)->mot;
            n->nb_occ = (*succ)->nb_occ;

            (*succ)->mot = temp_mot;
            (*succ)->nb_occ = temp_occ;

            return extraire_noeuds_max_rec(&(n->fd), lst, priorite_max, nb_extraits);
        }

        // Ajout du noeud à la liste résultat
        if (!insere_dans_liste_triee(lst, n))
        {
            return 0; // Échec de l'insertion
        }

        (*nb_extraits)++;
        return 1;
    }

    // Extraction dans le sous-arbre droit
    return extraire_noeuds_max_rec(&((*A)->fd), lst, priorite_max, nb_extraits);
}

/*
 * Fonction pour extraire tous les noeuds de priorité maximale
 * Entrée: double pointeur vers la racine de l'arbre et double pointeur vers la liste de sortie
 * Sortie: nombre de noeuds extraits, 0 si échec
 */
int extrait_priorite_max(ABRnois *A, Liste *lst)
{
    if (*A == NULL)
        return 0;

    // Initialisation de la liste résultat
    *lst = NULL;

    // Priorité maximale (celle de la racine)
    int priorite_max = (*A)->nb_occ;

    // Extraction des noeuds de priorité maximale
    int nb_extraits = 0;
    if (!extraire_noeuds_max_rec(A, lst, priorite_max, &nb_extraits))
    {
        return 0;
    }

    return nb_extraits;
}

/*
 * Fonction pour nettoyer un mot (supprimer la ponctuation, mettre en minuscules)
 * Entrée: mot à nettoyer
 * Sortie: 1 si le mot est valide, 0 sinon
 */
int nettoie_mot(char *mot)
{
    int i, j = 0;
    int longueur = strlen(mot);

    // Si le mot est vide, on le considère invalide
    if (longueur == 0)
        return 0;

    // Suppression des caractères non alphabétiques au début et fin
    int debut = 0;
    while (debut < longueur && !isalpha(mot[debut]))
    {
        debut++;
    }

    int fin = longueur - 1;
    while (fin >= 0 && !isalpha(mot[fin]))
    {
        fin--;
    }

    // Si le mot ne contient pas de caractères alphabétiques, on le considère invalide
    if (debut > fin)
        return 0;

    // Copie des caractères valides en minuscules
    for (i = debut; i <= fin; i++)
    {
        mot[j++] = tolower(mot[i]);
    }

    mot[j] = '\0';

    // Vérification de la longueur minimale (par exemple, 2 caractères)
    return (j >= 2);
}

/*
 * Fonction pour calculer le total des occurrences
 * Entrée: pointeur vers la racine de l'arbre
 * Sortie: somme des occurrences
 */
int calcule_total_occurrences(ABRnois A)
{
    if (A == NULL)
        return 0;

    return A->nb_occ + calcule_total_occurrences(A->fg) + calcule_total_occurrences(A->fd);
}

/*
 * Fonction principale
 */
int main(int argc, char *argv[])
{
    ABRnois arbre = NULL;
    int generer_pdf = 0;
    int nb_max_mots = -1;
    char *fichier_frequents = NULL;
    int nb_corpus = 0;
    char **fichiers_corpus = NULL;
    int total_mots = 0;
    int compteur_insertion = 0;
    int compteur_suppression = 0;

    // Traitement des arguments de la ligne de commande
    int i;
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-g") == 0)
        {
            generer_pdf = 1;
        }
        else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc)
        {
            nb_max_mots = atoi(argv[i + 1]);
            i++; // Sauter le prochain argument
        }
        else if (fichier_frequents == NULL)
        {
            fichier_frequents = argv[i];
        }
        else
        {
            // Agrandissement du tableau des fichiers corpus
            nb_corpus++;
            fichiers_corpus = (char **)realloc(fichiers_corpus, nb_corpus * sizeof(char *));
            fichiers_corpus[nb_corpus - 1] = argv[i];
        }
    }

    // Vérification des arguments obligatoires
    if (fichier_frequents == NULL || nb_corpus == 0)
    {
        fprintf(stderr, "Usage: %s [-g] [-n nombre] fichier_frequents.txt fichier_corpus1.txt [fichier_corpus2.txt ...]\n", argv[0]);
        return 1;
    }

    // Lecture des fichiers corpus et construction de l'arbre ABRnois
    FILE *f;
    char mot[256];

    for (i = 0; i < nb_corpus; i++)
    {
        f = fopen(fichiers_corpus[i], "r");
        if (f == NULL)
        {
            fprintf(stderr, "Erreur lors de l'ouverture du fichier %s\n", fichiers_corpus[i]);
            continue;
        }

        printf("Lecture du fichier %s...\n", fichiers_corpus[i]);

        // Lecture mot par mot
        while (fscanf(f, "%255s", mot) == 1)
        {
            if (nettoie_mot(mot))
            {
                if (insert_ABRnois(&arbre, mot))
                {
                    total_mots++;
                    compteur_insertion++;

                    if (generer_pdf)
                    {
                        char nom_pdf[256];
                        snprintf(nom_pdf, sizeof(nom_pdf), "insertion_%d.pdf", compteur_insertion);
                        exporte_arbre(nom_pdf, arbre);
                    }
                }
            }
        }

        fclose(f);
    }

    printf("Total des mots insérés : %d\n", total_mots);

    // Calcul du total des occurrences pour les pourcentages
    int total_occurrences = calcule_total_occurrences(arbre);
    printf("Total des occurrences : %d\n", total_occurrences);

    // Ouverture du fichier de sortie
    FILE *out = fopen(fichier_frequents, "w");
    if (out == NULL)
    {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier %s\n", fichier_frequents);
        libere_arbre(arbre);
        free(fichiers_corpus);
        return 1;
    }

    // Extraction des mots les plus fréquents jusqu'à obtenir l'arbre vide
    // ou jusqu'à atteindre le nombre maximal de mots demandé
    int nb_mots_extraits = 0;
    int derniere_freq = -1;

    while (arbre != NULL && (nb_max_mots == -1 || nb_mots_extraits < nb_max_mots || arbre->nb_occ == derniere_freq))
    {
        Liste liste_mots_max = NULL;
        int nb = extrait_priorite_max(&arbre, &liste_mots_max);

        if (nb == 0)
        {
            fprintf(stderr, "Erreur lors de l'extraction des mots de priorité maximale\n");
            break;
        }

        compteur_suppression++;

        if (generer_pdf && arbre != NULL)
        {
            char nom_pdf[256];
            snprintf(nom_pdf, sizeof(nom_pdf), "suppression_%d.pdf", compteur_suppression);
            exporte_arbre(nom_pdf, arbre);
        }

        // Écriture des mots extraits dans le fichier de sortie
        Cell *courant = liste_mots_max;
        while (courant != NULL)
        {
            derniere_freq = courant->n->nb_occ;
            double pourcentage = 100.0 * courant->n->nb_occ / total_occurrences;
            fprintf(out, "%s %.2f%%\n", courant->n->mot, pourcentage);

            nb_mots_extraits++;

            // Libération de la mémoire du noeud
            Cell *temp = courant;
            courant = courant->suivant;
            libere_noeud(temp->n);
            free(temp);
        }
    }

    // Libération des ressources
    fclose(out);
    libere_arbre(arbre);
    free(fichiers_corpus);

    printf("Nombre de mots extraits : %d\n", nb_mots_extraits);
    printf("Résultats écrits dans %s\n", fichier_frequents);

    return 0;
}