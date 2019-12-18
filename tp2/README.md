# Travail pratique 2 - Le problème des philosophes qui dînent

## Description

Ce projet synchronise les actions du [problème connu des philosophes qui dînent](https://en.wikipedia.org/wiki/Dining_philosophers_problem).
Le projet met en œuvre une solution utilisant des
verrous mutex et des variables de condition Pthreads.

## Auteur

Julien Guité-Vinet (GUIJ09058407)


## Fonctionnement
Se référer à la section [Dépendances](#dépendances) pour installer les librairies

### Compilation
- Ouvrir le terminal à la racine du projet
- La commande `make` crée l'exécutable `TP2` et une copie du programme nommé `valider`

```sh
make 
```
### Exécution
- Une fois le programme compilé, on exécute le programme avec la commande `./TP1`.
- On peut exécuter le validateur dans un autre terminal avec la commande `./valider`.

Le menu suivant s'affichera
``` sh
1)Consulter résultat
2)Modifier le nom d'un philosphe
3)Supprimer le nom d'un philosophe
4)Modifier l'action et le nom d'un philosophe
5)Quitter
```
L'utilisateur doit alors entrer un numéro dans le terminal
- L'option #1 affiche le contenu du fichier `resultat.txt`
- L'option #2 modifie le nom d'une philosophe. 
    - Lors de son exécution, le champ contenant le nom du philosophe est vérouillé.
- L'option #3 supprime les lignes associées à l'identifiant d'un philosophe.
    - Lors de son exécution, tout le fichier est vérouillé.
- L'option #4 modifie l'action et le nom d'un philosophe.
    - Lors de son exécution, les lignes associées au philosophe sont vérouillées. 
- L'option #5 affiche le résultat initial de l'algorithme et termine le programme.

### Gestion d'erreurs
Une erreur est soulevée lorsque deux processus tentent de modifier ou supprimer 
le même philosophe de manière concurrente.
L'utilisateur est alors redirigé vers le menu.


### Nettoyage
- La commande `make clean` supprime les fichiers exécutables `TP2`, et `valider` 
ainsi que tous les fichiers ayant une extention `.o` 


## Contenu du projet

* Makefile: Automatisation de la compilation, de la documentation et des tests unitaires

* README.md: Informations propre au projet

* TP2 : projet compilé

* TP2.c: code source du projet

* utils.c: utilitaire pour manipulation de string et de fichier

* resultat.txt: le fichier contenant les actions des philosophes


## Dépendances

* Pour compiler le code source, il faut au moins [GCC](https://gcc.gnu.org/) avec le standard c99. 


## Références

* Manuel des librairies standards: https://www.tutorialspoint.com/c_standard_library/
* Manuel des appels systèmes: http://man7.org/linux/man-pages/

## Statut

* [x] Le projet compile lorsqu'on entre `make`. Il supporte la cible `clean`. 
* [x] Le fichier `README.md` est complété.
* [x] Les modules sont documentés..
* [x] Le programme fonctionne sur le serveur Java.