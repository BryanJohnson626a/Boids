# Boids

This is a demo project I made for fun, which implements the boids flocking algorithms as described by Craig Reynolds (https://www.red3d.com/cwr/boids/). The project is rendered using OpenGL and a simple phong lighting algorithm.

# Summary

This program simulates 1000 (or more) boids moving simultaneously in 3D space. None of the individual boids can get or receive any information from their fellows, other than what they can see (location and direction). Using that information, each boid decides how it should move at any given point in time. Despite this, the boids naturally form into flocks and move together.

Each boid has three primary behaviors: Alignment, Cohesion, and Avoidance. Alignment causes the boid to move in the same direction as other boids close by. Cohesion causes the boid to move toward the center of mass of boids in a larger area. Avoidance causes the boid to strongly move away from boids very close to it. A fourth behavior is added to cause boids to prefer to stay inside a bounding area just to keep them all on screen.

# Usage

By default, the program runs with 1000 boids. The boids will move and interact without any need for input from the user. Optionally, if a number is given as a command line argument, that number of boids will spawn instead. If two numbers are given, the first will be the number of normal boids, and the second will be the number of larger boids that scare away the smaller boids.

Command line input is processed naively so entering anything other than one or two integers could cause undefined behavior and is not recommended.

# Optimizations

In order to support thousands of boids moving simultaneously in real-time, I had to make some changes to the base algorithm. The first optimization is with how boids decide what other boids to look at. Having every boid calculate it's behavior based on information from every other boid in the scene means that each additional boid adds exponentially to the calculations needed each frame.

Most behaviors follow the inverse square law, with distant boids having little or no effect on the behavior of a boid, compared to the effect of those closer to it. Thus, much time is wasted making calculations that don't actually affect the simulation. Thus, my first optimization was to add a quick range check before one boid looks at another, to avoid making unnecessary calculations. To be sure this check is as fast as possible, distance squared is used instead of distance, saving  millions of expensive square-root calculations.

However while a distance check is much cheaper than doing a full behavior calculation, with 1000 boids looking at 999 other boids, that’s still nearly a million checks made each frame. My second optimization came from realising that for the most part, which boids are close to each other doesn’t change very much from frame to frame. Instead of checking for distance against the entire list of boids, it would be much more efficient to make that check once, keep a record of which boids are close by, and then re-use that list of boids for the next second or so.

But if every boid makes this check every 60 frames (for example), that means that we still do close to a million range checks in a single frame, causing an unbalanced load on the CPU. To remedy this, the timer for each boid’s neighbor recalculation is randomly staggered at the start of the program. This means that on any given frame, only a fraction of boids are doing their expensive neighbor lookup, while the rest simply calculate based on their cached neighbor list.

These optimizations allow the program to run with thousands of boids, compared to only a few hundred beforehand.

# Future plans

Some optimizations I plan to do in the future:

1) Move boids from a hybrid object-based system to a fully component-based system. Each boid becomes a simple POD struct. This allows for boids to be packed more tightly into cache, and removes dynamic function lookup.

2) Use an octree or similar data structure to hold boids, further speeding up the neighbor check and moving the algorithm fully from O(n^2) to O(nlogn).

3) Optimize rendering to more efficiently draw large numbers of boids. As of now there are not enough boids or complex enough geometry to cause any issues, but this could be a problem in the future as other optimizations allow for yet more boids.
