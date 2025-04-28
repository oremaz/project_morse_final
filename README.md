# Projet Morse

Ce projet consiste à encoder un fichier txt en Morse (format WAV), puis à décoder en format txt un fichier Morse (WAV) encodé avec la méthode précédente.

## Points à souligner

- La méthode d'encodage du texte en Morse est simple : les seuls caractères acceptés dans le fichier texte initial sont ceux de la classe `MorseConverter`. Le programme renverra une erreur si un caractère du fichier texte n'est pas cité dans le code. Les minuscules sont confondues avec les majuscules.

- Le template `typename SampleType = int16_t` permet de moduler le type d’échantillon audio généré. Par exemple, le fichier test1.wav est l'encodage de test1.txt sur 16 bits, le fichier test2.wav est l'encodage de test1.txt sur 8 bits (donc de moins bonne qualité mais nécessite moins de stockage). Pour décoder un audio qu'on sait encodé sur un format f, il est nécessaire de fixer la variable 'SampleType' à f dans le code et de compiler avant d'éxécuter la commande.

- Si le fichier texte contient deux ou plusieurs espaces consécutifs (exemple : `Je  suis Orel`), le code encodera un seul espace (exemple : `Je suis Orel`).
