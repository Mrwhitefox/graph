The goal of this project is to optimize the layout of graphs by a physic simulation:

* a node <=> a mass
* a link <=> a spring
* plus: repulsion forces, ...

A video is disponible [here](http://wassner.blogspot.fr/2014/06/analyse-litteraire-grace-un-logiciel.html)


Java package
------------
Use `./LAUNCH.sh` to compile and launch a test program.


C program
---------
Install CSFML (graphic library) on Linux:

`sudo apt-get install libcsfml-dev libgl1-mesa-dev libglu1-mesa-dev`

Compilation: `gcc -Wall -g simu.c -lcsfml-graphics -o simu -lm`

Execution: `./simu < graphs/exemple.txt`


Licence
-------

Creative Commmons BY-SA-NC

AUTHOR: V. Marquet