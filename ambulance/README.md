Ambulance
========
The algorithm this main.cpp file are using is kmeans and greedy routing.

**Kmeans**

First we need to locate the hospitals. And we are using the kmeans algorithm. For kmeans, first we have to get five initial spots. after that we will fine turn the five positions.

For the initial positions, we use get them step by step. First initial a single position, which is the center gravity of the all points. Then we separate one position into two positions.
After the number of the positons reaches 5 we stop.

For the fine-tune phase, we simple recluster and recompute the central gravity of new positions and stop util no point changes its group.

**Greedy Routing**

Ambulance always takes the closest and shortest survival time patient to pick.(There are different weights for these two parts, but now I simply set both of them to 1). Since we have to hurry up and make it back to hospital, everytime the ambulance is choosing candidates, it has to take that into account.
After the first ambulance finished its route, the second one set up and reset the survival time, but we have to keep a mark for those saved patients in order to avoid the following ambulances not to pick them up.
