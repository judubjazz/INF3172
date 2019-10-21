# Travail pratique 1 - Validateur sudoku

## Description

Ce projet est une application multithread permettant de déterminer si la
solution à un Sudoku est valide. La stratégie adoptée consiste à créer
des threads qui vérifient les critères suivants :
* Un thread pour vérifier que chaque colonne contient les chiffres 1 à 9
* Un thread pour vérifier que chaque ligne contient les chiffres 1 à 9
* Neuf threads pour vérifier que chacun des sous-grilles 3x3 contient les chiffres 1 à 9

Cela donne un total de onze threads distincts pour valider un Sudoku.

## Auteur

Julien Guité-Vinet (GUIJ09058407)


## Fonctionnement
Se référer à la section [Dépendances](#dépendances) pour installer les librairies

### Compilation
- Ouvrir le terminal à la racine du projet
- La commande `make` crée l'exécutable `TP1`

```sh
make 
```
### Exécution

Un fichier ayant pour nom `test.txt` doit se trouver obligatoirement à la racine du projet.

Les sudokus du fichier doivent être séparés obligatoirement par un seul retour à la ligne

```text
1 2 3 4 5 6 7 8 9
2 3 4 5 6 7 8 9 1
9 9 9 6 7 8 9 1 2 
4 5 6 7 8 9 1 2 3
5 6 7 6 5 4 2 3 4
6 7 8 3 2 $ 3 4 5 
7 8 9 1 2 3 4 5 6
8 9 1 2 3 4 5 6 7
9 1 2 3 4 5 6 7 8

6 2 4 5 3 9 1 8 7
5 1 9 7 2 8 6 3 4
8 3 7 6 1 4 2 9 5
1 4 3 8 6 5 7 2 9
9 5 8 2 4 7 3 6 1
7 6 2 3 9 1 4 5 8
3 7 1 9 5 6 8 4 2
4 9 6 1 8 2 5 7 3
2 8 5 4 7 3 9 1 6
```

Une fois le programme compilé, la validation des sudokus du fichier peut s'exécuter avec la commande 
```sh
./TP1
```
### Nettoyage
- La commande `make clean` supprime le fichier exécutable `TP1` 


## Tests
Il est ensuite possible de lancer une suite de tests de façon automatique avec la commande


```sh
make test
```


## Contenu du projet

* Makefile: Automatisation de la compilation, de la documentation et des tests unitaires

* README.md: Informations propre au projet

* TP1 : projet compilé

* TP1.c: code source du projet

* utils.c: utilitaire pour manipulation de string et de fichier

* test.txt: le fichier test contenant les sudokus


## Dépendances

* Pour compiler le code source, il faut au moins [GCC](https://gcc.gnu.org/) avec le standard c99. 


## Références

* https://www.tutorialspoint.com/c_standard_library/

## Statut

* [x] Le projet compile lorsqu'on entre `make`. Il supporte les cibles `test` et `clean`. 
* [x] Le fichier `README.md` est complété.
* [x] Les modules sont documentés.
* [x] Il n'y a aucune modification des signatures des fonctions et des définitions des types fournis.
* [x] Le programme ne contient pas de valeurs magiques.
* [x] Le programme fonctionne sur le serveur Java.
* [x] Tous les tests fonctionnent.