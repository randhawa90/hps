Gravitational Voronoi Game
========
For now the algorithm I am using is block-base greedy algorithm. I am trying to find the best point for the next move and just the 
next move. So this is greedy. But since the computation for each choice is really huge, I'm iterate every point in the grid, I try
every point in a 40\*40 block and if it's not that good, I try another block to find the best block. In that block I iterate all points.
