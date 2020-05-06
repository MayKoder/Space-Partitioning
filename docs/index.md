I am [Miquel Suau](https://www.linkedin.com/in/miquel-suau-gonz%C3%A0lez-346b5617a/), student of the [Bachelor’s Degree in Video Games by UPC at CITM](https://www.citm.upc.edu/ing/estudis/graus-videojocs/). This content is generated for the second year’s subject Project 2, under supervision of lecturer [Marc Garrigó](https://www.linkedin.com/in/mgarrigo/).

## Why is Space Partitioning necessary
Lets set a context here, lets say you have 100 objects in a scene, and some are really close, some are really far away. We as humans who can see distances, instinctively only look around an object to check if something is colliding with it, right?

Well computers can't do that, when we ask the computer to check collisions it's pretty common to just make a double FOR loop and check every object with every other object in the scene (brute force). This results in an operating time of O(n²) which means that if we have 100 units, we'll need to compute O(100²) = 10.000 checks. Keep that number in mind.

So, we don't want to work with O(n²) because if we start adding objects, checks can grow fast and create problems. We need to work with something like O(n log n), which is the operating time of a Quadtrees and AABB Trees. This means that in the same 100 units context, we just need to compute O(100 * log(100)) = 200, big improvement from 10.000 to 200 isn't it? (Keep in mind that usually Quad trees operate in O(log n), but we are checking every element of the leaf with every other element so we need to multiply that O(log n) with every element in the leaf).

Keep in mind that if we are working with a limited number of objects and this number is relatively small, we could just brute force the checks. Trees are usefull when we work with a dinamic unlimitd number of objects. Also, a bad implemented tree can be slower than some brute force checks, so i recommend to read this page until the end.

Here is a small table to represent the big improvement:

| Units         | O(n²)              |  O(n log n)  |
| ------------- |:------------------:|:------------:|
| 100           | 10.000      checks | 200 checks   |
| 1.000         | 1.000.000   checks | 3.000 checks |
| 10.000        | 100,000,000‬ checks | 40.000 checks|

There's also lots of data trees structures, like Octree used in 3D space.

## Different approaches by different games (use animated gifs when possible)

## Description in detail for the selected approach

## Links to more documentation

## TODOs and Solution inside the repository as VS projects

## Optional Homework for practicing

## Explanation of any other improvements on the system

### Sources
[Medieval Building 01 - Bleed's Game Art](https://opengameart.org/content/medieval-building-01-bleeds-game-art-0)
[DOOM under the hood](https://www.youtube.com/watch?v=e0W65ScZmQw)

![](assets/TreeGif.gif)
