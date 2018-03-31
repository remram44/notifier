# Notifier -- Programme indiquant la présence de joueurs sur des serveurs de jeu

## Description

Notifier est un petit programme se plaçant dans la zone de notification de votre système, et vous indiquant (via un son, le changement de couleur de l'icône et des info-bulles) quand des joueurs sont présents sur l'un des serveurs de jeu surveillés.

Pour le moment, Notifier est configuré (via le fichier de configuration inclu) pour surveiller les serveurs de Supélec Rézo. Une future version permettra de choisir les serveurs que l'on désire via une interface graphique.

## Choses à faire (TODO-LIST)

* Afficher toutes les infos dans une seule bulle, plutôt qu'ouvrir une info-bulle par évènement.
* Ajouter la possibilité de configurer les serveurs à surveiller, les icônes du programme et le son joué.

## Compilation

Ce programme utilise le système de build de Qt, qmake. Taper "qmake" dans un terminal génerera un Makefile, et vous pourrez construire les binaires via "make".

## Installation (Windows)

Placez tous les fichiers de l'archive quelque part et double-cliquez sur notifier.exe. Si vous voulez que le programme se lance automatiquement à chaque démarrage, cochez l'option correspondante dans le menu contextuel.
