# Guide d'instructions - ABRnois

## 1. Compilation du programme

Pour compiler le programme `abrnois.c`, utilisez la commande suivante :

```bash
gcc -Wall -o abrnois abrnois.c -lm
```

L'option `-lm` permet de lier la bibliothèque mathématique, nécessaire pour les calculs de pourcentages.

## 2. Prérequis

Le programme utilise Graphviz pour générer les représentations graphiques des arbres au format PDF. Assurez-vous que Graphviz est installé sur votre système si vous souhaitez utiliser l'option `-g`.

### Installation de Graphviz

- **Sur Ubuntu/Debian** :
  ```bash
  sudo apt-get install graphviz
  ```

- **Sur Fedora/CentOS** :
  ```bash
  sudo dnf install graphviz
  ```

- **Sur macOS** (avec Homebrew) :
  ```bash
  brew install graphviz
  ```

- **Sur Windows** :
  Téléchargez et installez Graphviz depuis le site officiel : https://graphviz.org/download/

## 3. Utilisation du programme

### Syntaxe générale

```bash
./abrnois [options] fichier_frequents.txt fichier_corpus1.txt [fichier_corpus2.txt ...]
```

### Options disponibles

- `-g` : Génère une représentation graphique au format PDF des arbres intermédiaires créés lors du processus d'insertion et de suppression.
- `-n nombre` : Limite l'extraction aux `nombre` mots les plus fréquents. Si des mots ont la même fréquence que le dernier mot extrait, ils seront également inclus dans le résultat.

### Exemples d'utilisation

1. **Analyse d'un seul fichier corpus sans options** :
   ```bash
   ./abrnois frequents.txt corpus.txt
   ```

2. **Analyse de plusieurs fichiers corpus avec génération de PDF** :
   ```bash
   ./abrnois -g frequents.txt corpus1.txt corpus2.txt corpus3.txt
   ```

3. **Extraction des 100 mots les plus fréquents** :
   ```bash
   ./abrnois -n 100 frequents.txt corpus.txt
   ```

4. **Combinaison des options** :
   ```bash
   ./abrnois -g -n 50 frequents.txt corpus1.txt corpus2.txt
   ```

## 4. Format des fichiers d'entrée et de sortie

### Fichiers corpus

Les fichiers corpus doivent être des fichiers texte contenant les mots à analyser. Le programme lira ces fichiers mot par mot, en ignorant la ponctuation et en convertissant tous les caractères en minuscules.

### Fichier de sortie

Le fichier de sortie contiendra la liste des mots les plus fréquents, triés par ordre décroissant de fréquence. En cas d'égalité de fréquence, les mots seront triés par ordre alphabétique.

Chaque ligne du fichier de sortie aura le format suivant :
```
mot pourcentage%
```

Par exemple :
```
le 7.82%
de 5.94%
la 3.65%
...
```

## 5. Interprétation des fichiers PDF générés

Si l'option `-g` est utilisée, le programme générera plusieurs fichiers PDF :

- `insertion_1.pdf`, `insertion_2.pdf`, ... : Représentations de l'arbre ABRnois après chaque insertion.
- `suppression_1.pdf`, `suppression_2.pdf`, ... : Représentations de l'arbre ABRnois après chaque suppression (extraction des mots de priorité maximale).

Dans ces représentations graphiques :
- Chaque nœud contient le mot et son nombre d'occurrences.
- Les arêtes représentent les liens entre les nœuds de l'arbre.
- La structure respecte à la fois les propriétés d'un arbre binaire de recherche (ordre lexicographique) et d'un arbre tournois (priorité basée sur le nombre d'occurrences).

## 6. Conseils d'utilisation

- Pour des corpus très volumineux, il est recommandé de ne pas utiliser l'option `-g`, car elle générera un grand nombre de fichiers PDF et ralentira l'exécution du programme.
- Si vous souhaitez obtenir une liste des mots les plus fréquents comparable à celle du linguiste Étienne Brunet, utilisez l'option `-n 1500`.
- Le programme traite les mots indépendamment de leur forme fléchie ou de leur base lexicale. Pour une analyse linguistique plus poussée, un prétraitement des corpus avec des outils de lemmatisation pourrait être envisagé.

## 7. Dépannage

### Problèmes courants

1. **Erreur lors de la génération du PDF** :
   Vérifiez que Graphviz est correctement installé et accessible dans votre PATH.

2. **Mémoire insuffisante pour les grands corpus** :
   Pour les corpus très volumineux, vous pourriez rencontrer des problèmes de mémoire. Dans ce cas, considérez l'utilisation de l'option `-n` pour limiter le nombre de mots extraits.

3. **Fichiers introuvables** :
   Assurez-vous que les chemins vers les fichiers corpus sont corrects. Si les fichiers ne sont pas dans le répertoire courant, spécifiez le chemin complet.

### Nettoyage des fichiers générés

Après l'exécution des tests, vous pouvez supprimer tous les fichiers générés avec la commande suivante :

```bash
rm -f frequents_test*.txt insertion_*.pdf suppression_*.pdf abrnois
```

Cette commande supprime :
- Les fichiers de sortie (`frequents_test*.txt`)
- Les fichiers PDF des étapes d'insertion (`insertion_*.pdf`)
- Les fichiers PDF des étapes de suppression (`suppression_*.pdf`)
- L'exécutable compilé (`abrnois`)

Si vous souhaitez conserver l'exécutable, utilisez cette commande à la place :

```bash
rm -f frequents_test*.txt insertion_*.pdf suppression_*.pdf
```

Vous pouvez ajouter cette commande à la fin de votre script de test pour un nettoyage automatique.

## 8. Script de test automatisé

Voici un script bash que vous pouvez utiliser pour tester automatiquement votre implémentation avec différents corpus et options. Créez un fichier nommé `test_abrnois.sh` avec le contenu suivant :

```bash
#!/bin/bash

# Script de test pour ABRnois
# Auteur : Vincent PLESSY
# Date : 2 Mai 2025

# Vérification que le programme est compilé
if [ ! -f ./abrnois ]; then
  echo "Compilation du programme..."
  gcc -Wall -o abrnois abrnois.c -lm
  
  if [ $? -ne 0 ]; then
    echo "Erreur lors de la compilation. Abandon des tests."
    exit 1
  fi
fi

echo "=== Test de ABRnois ==="
echo ""

# Test 1: Test avec un seul fichier corpus court
echo "Test 1: Analyse d'un seul fichier corpus_court.txt"
./abrnois frequents_test1.txt corpus_court.txt
echo "Résultat écrit dans frequents_test1.txt"
echo ""

# Test 2: Test avec option -g sur corpus court
echo "Test 2: Analyse avec génération de PDF"
./abrnois -g frequents_test2.txt corpus_court.txt
echo "Résultat écrit dans frequents_test2.txt"
echo "Fichiers PDF générés dans le répertoire courant"
echo ""

# Test 3: Test avec option -n sur corpus court
echo "Test 3: Extraction des 10 mots les plus fréquents"
./abrnois -n 10 frequents_test3.txt corpus_court.txt
echo "Résultat écrit dans frequents_test3.txt"
echo ""

# Test 4: Test avec corpus littéraire
echo "Test 4: Analyse du corpus littéraire"
./abrnois frequents_test4.txt corpus_litteraire.txt
echo "Résultat écrit dans frequents_test4.txt"
echo ""

# Test 5: Test avec corpus technique
echo "Test 5: Analyse du corpus technique"
./abrnois frequents_test5.txt corpus_technique.txt
echo "Résultat écrit dans frequents_test5.txt"
echo ""

# Test 6: Test avec corpus varié
echo "Test 6: Analyse du corpus varié"
./abrnois frequents_test6.txt corpus_varié.txt
echo "Résultat écrit dans frequents_test6.txt"
echo ""

# Test 7: Test avec tous les corpus
echo "Test 7: Analyse de tous les corpus combinés"
./abrnois frequents_test7.txt corpus_court.txt corpus_litteraire.txt corpus_technique.txt corpus_varié.txt
echo "Résultat écrit dans frequents_test7.txt"
echo ""

# Test 8: Test complet avec options
echo "Test 8: Test complet avec options -g -n 20"
./abrnois -g -n 20 frequents_test8.txt corpus_court.txt corpus_litteraire.txt
echo "Résultat écrit dans frequents_test8.txt"
echo "Fichiers PDF générés dans le répertoire courant"
echo ""

echo "=== Tous les tests ont été effectués ==="
echo "Vérifiez les fichiers de sortie pour analyser les résultats"

# Décommentez la ligne suivante si vous souhaitez nettoyer automatiquement après les tests
# rm -f frequents_test*.txt insertion_*.pdf suppression_*.pdf *.dot
```

### Utilisation du script de test

1. Créez le fichier `test_abrnois.sh` avec le contenu ci-dessus.

2. Rendez-le exécutable avec la commande :
   ```bash
   chmod +x test_abrnois.sh
   ```

3. Assurez-vous que tous les fichiers corpus sont dans le même répertoire que votre script et votre programme.

4. Exécutez le script :
   ```bash
   ./test_abrnois.sh
   ```
