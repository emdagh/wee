S: box2d has two ways of detecting ray cast collisions, with two special cases for chain- and edge shape. 

T: There was a decision to be made which to use. Option 1 was to use the fixture's RayCast function which is inefficient (all fixtures of all bodies
against all fixtures of all bodies). Which could be fixed by using a quadtree-like structure (Kd-tree comes to mind as well). 
The second option was to use the world's RayCast method and implement a b2RayCastCallback class.

A: In the end, the overhead of creating a spatial subdivision structure and integrating it with Box2D is too far outside of the scope
of the assignment. A set difference method was implemented where a cache std::set is compared with a current raycast. The set inner/outer is then used 
to message the relevant entities.

R: The result is that we have an efficient, well integrated method of detecting when a raycast hits, and when it stops hitting that fixture.

R: I believe this took a bit too long because I wanted the control the other option provided. 

T: Take the pragmatic option if it exists.

