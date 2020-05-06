I am [Miquel Suau](https://www.linkedin.com/in/miquel-suau-gonz%C3%A0lez-346b5617a/), student of the [Bachelor’s Degree in Video Games by UPC at CITM](https://www.citm.upc.edu/ing/estudis/graus-videojocs/). This content is generated for the second year’s subject Project 2, under supervision of lecturer [Marc Garrigó](https://www.linkedin.com/in/mgarrigo/).

---
## Why is Space Partitioning necessary

Lets set a context here, lets say you have 100 objects in a scene, and some are really close, some are really far away. We as humans who can see distances, instinctively only look around an object to check if something is colliding with it, right?

Well computers can't do that, when we ask the computer to check collisions, it's pretty common to just make a double FOR loop and check every object with every other object in the scene (brute force). This results in an operating time of O(n²) which means that if we have 100 units, we'll need to compute O(100²) = 10.000 checks. Keep that number in mind.

Also note that we can't just ask the computer to get the distance of every object and then check only the closest ones, because... we would be iterating every object anyway so it would be even worst.

So, we don't want to work with O(n²) because if we start adding objects, checks can grow fast and create problems. We need to work with something like O(n log n), which is the operating time of a Quadtrees and AABB Trees. This means that in the same 100 unit's context, we just need to compute O(100 * log(100)) = 200, big improvement from 10.000 to 200 isn't it? (Keep in mind that usually Quad trees operate in O(log n), but we are checking every element of the leaf with every other element so we need to multiply that O(log n) with every element in the leaf).

Keep in mind that if we are working with a limited number of objects and this number is relatively small, we could just brute force the checks. Trees are useful when we work with a dynamic unlimited number of objects. Also, a bad implemented tree can be slower than some brute force checks, so I recommend to read this page until the end.

Here is a small table to represent the big improvement:

| Units         | O(n²)              |  O(n log n)  |
| ------------- |:------------------:| ------------ |
| 100           | 10.000      checks | 200 checks   |
| 1.000         | 1.000.000   checks | 3.000 checks |
| 10.000        | 100,000,000‬ checks | 40.000 checks|

There's also lots of data trees structure, like Octree used in 3D space.

---
## Different approaches by different games

There are a lot of games using Space Partitioning these days, it's almost a "must", but in the old days of video games, space partitioning wasn't so common. This is some of the games that used implementations of space partitioning:

- DOOM

It's safe to say that almost any modern game engine uses trees for camera culling, collision detection, raycast rendering...

---
## Description in detail for the selected approach

First of all, let's set up our context, we are working with C++ and SDL. We'll also work with 100 static and 100 dynamic units.

The data structure of the trees will consist in a Node struct and a Tree class. We'll also create a custom namespace with some basic math functions and some structs like Point, Rect...

**Add all the structure code here**
#### Rect
```cpp
struct Rect
{
	int x, y, w, h;

	Point GetCentralPoint() 
	{
		return {x + w / 2, y + h / 2};
	}
};
```

We will create a working QuadTree and AABBTree and some functions to use them, recursivity will be a recursive topic here (pun intended).



---
## Links to more documentation

![](assets/TreeGif.gif)

[Introductory Guide to AABB Tree Collision Detection](https://www.azurefromthetrenches.com/introductory-guide-to-aabb-tree-collision-detection/)

---
## Custom trees documentation

#### AABB Tree data documentation

| Type         | Function Declaration    | Function description  |
| ------------ |:-----------------------:|:---------------------:|
| int          | GetData()               | Returns something     |

---
## TODOs and Solution inside the repository as VS projects

---
## Homework

If you think that this is an interesting subject to work on, I dare you to take my code and optimize it, feel free to play around with it, I tried to make it as c++ independent by avoiding std's so it's easy to move to other languages. If you can get it running in a more efficient way (which I'm sure can be done, like really sure), please contact me so I can update the code and credit you properly, let's work together to make this code as fast as possible.

##### **REMEMBER that removing a license or the name of the creator from the code is A CRIME, so don't try to steal the code, proper credit MUST be added when using this code.**

---
## Explanation of any other improvements on the system

### Sources
[Medieval Building 01 - Bleed's Game Art](https://opengameart.org/content/medieval-building-01-bleeds-game-art-0)

[DOOM under the hood](https://www.youtube.com/watch?v=e0W65ScZmQw)

# THINGS TO TALK ABOUT
Release mode goes brbrbr but debug mode goes brbrbn't
Making the list of entities not pointers also goes brbrbr, but if you are using polymorphism you must work with pointers.
