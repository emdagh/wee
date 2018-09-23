# Development diary

## 2018-09-23 (5 hrs)
Worked on planning into the wee hours; the descriptions on some of the ILO's are... cryptic.

## 2018-09-22 (8 hrs)
Work on getting TTF fonts into the framework in a nice way.

Arrgh! First memory management issue! Very nice. Worked GDB like crazy trying to figure this one out. Details are in
commit 250ec7d4eee9eb041570dbfe87c8e91ad598f579
Eventually it was a mixture of RTFM and GDB that saved me. Phew.

## 2018-09-21 (8 hrs)
Did some refactoring / rewriting of sprite sheets for use in sprite fonts. Generalized the classes a bit.
Had a call with Ahbishek, and the project proposal was approved! So, now he just needs to figure out how to mark it as such.
Make sure to call/contact him about this Monday next week.

## 2018-09-20 (8 hrs)
Created a sprite font class, created several helper classes as well as creating a prelimenary setup of the asset manager. 

## 2018-09-19 (8 hrs)
Spent time working on the build system. Some third-party libraries need rather specific build setups. Needed to purge 
some system libraries to test this. What I don't want with this framework is to end up saying 'it works on my laptop'.

## 2018-09-18 (8 hrs)
General programming work, mainly playing around with 3rd party libraries. Trying to make an informed decision about 
composition vs. inheritance. Composition won out; mainly based on modern notions that OOP is kind of dead for
most intents and purposes due to cache coherency issues with OOP (array of structs vs. struct of arrays [AOS vs SOA])


## 2018-09-17 (10hrs)
finished up most of the prototype of the imgui for now. Buttons and sliders are in there; now I need to
get some nice textures for it. Tomorrow I'll work on the gameplay of the Tiny Wings clone.

Also, a couple more layouts would be nice to have, Java has some nice examples. FlowLayout, BorderLayout
and GridLayout to name a few. Got to find a nice way to incorporate that into the imgui though.

Now might be a good idea to look for a spline editor I can use to create level chunks. If it doesn't exist, I'll have to create one.

## 2018-09-16 (4hrs)
Started work on GUI (layout mostly)

## 2018-09-15: (4 hrs)
Same as yesterday

## 2018-09-14: (7 hrs)
Today, I worked mainly on terrain genetation

## 2018-09-13 (8 hrs)
Created game concept for second game. Trying to fix Box2D build issues.

## 2018-09-12 (8 hrs)
Worked on flexing my C++ muscles again. Implemented some noise functions for procedural 
content generation


## 2018-09-11 (10 hrs)
Today I worked mainly on getting Box2D to play nice with the ECS I'm using. Getting 
to know C++ again is quite a challenge. It's noticeable that it's been about 2 years 
since I've written a game (or an engine for that matter).

Also, the ECS approach might not fit the terrain I am developing at the moment.

Currently I'm questioning if a spline based game is the way to go... There are no real editors
for it (i.e.: no standard like TMX for tile-based games for instance).

The trick with the splines is to make them interesting. Alto's Adventure seems to do 
a good job at that. Better than Tiny Wings any way...

Pehaps I should have a close look at that game.

I should start developing the second game in parallel, pinball should work quite nicely 
with TMX files as well.

## 2018-09-12 (4 hrs)
## 2018-09-13 ()
