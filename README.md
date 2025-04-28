# Projet Morse

Ce projet consiste à encoder un fichier texte en Morse (format WAV), puis à décoder un fichier Morse (WAV) encodé avec la méthode précédente en format texte.

## Points à souligner

- La méthode d'encodage du texte en Morse est simple : les seuls caractères acceptés dans le fichier texte initial sont ceux de la classe `MorseConverter`. Le programme renverra une erreur si un caractère du fichier texte n'est pas cité dans le code. Les minuscules sont confondues avec les majuscu

- Le template `typename SampleType = int16_t` permet de moduler le type d’échantillon audio généré. Par exemple, le fichier test1.wav est l'encodage de test1.txt sur 16 bits, le fichier test2.wav est l'encodage de test1.txt sur 8 bits (donc de moins bonne qualité mais prend moins d'espace mémoire).

- Si le fichier texte contient deux ou plusieurs espaces consécutifs (exemple : `Je  suis Orel`), le code encodera un seul espace (exemple : `Je suis Orel`).
