# Graduation project
The idea behind the graduation project is as follows:
* create a game engine / framework.
* create several games.
* create a neural network that will generate content that is _fun_

## deliverables
For the project, several deliverables will be produced.

### P0
### engine/framework
The wee engine(c) will be a minimalistic prototyping engine. Nothing fancy; essentially it will be an extension / wrapper library for SDL2.
### games
The games that will be produced using the wee engine are basic, proof of concept games that will serve as 
examples on the use of the middleware.
#### breakout
classic, the idea is that this game will serve as a proof of concept.
#### psycho pinball
angry birds meets pinball, get your ball as high as possible. This serve as an expansion of the proof of concept;
applying more complexity to the use-cases of the neural net.
#### skiing adventure
sport arcade game
### P1
### neural network middleware
The neural network is expected to be an A.I. middleware that will serve as a generic algorithm
to generate level data for generic games. This will include but shouldn't be limited to the games above. The deliverable
will consist of a docker image (or similar) that will serve the middleware.

The middleware will also provide developers with a library that will enable them to access the middleware and 
provide user feedback forms.

### research documentation
Along the way, I expect to do a significant amount of research; this will all be documented and presented as
a research document exploring the state-of-the-art of the subject matter.

## tools/frameworks/libraries used
[SDL2]
[OpenGL]
[SDL2_mixer]
[SDL2_image]
[nanogui]
[Box2D]
[CakePHP]
[MySQL]

## references
[Game level generation with recurrent neural networks](https://esc.fnwi.uva.nl/thesis/centraal/files/f1727262514.pdf)

[Super Mario Bros. Level Generation Using Torch-RNN](https://medium.com/@justin_michaud/super-mario-bros-level-generation-using-torch-rnn-726ddea7e9b7)

[Machine Learning is Fun! Part 2](https://medium.com/@ageitgey/machine-learning-is-fun-part-2-a26a10b68df3)



[Game Level Generation Using Neural Networks](https://www.gamasutra.com/blogs/SeungbackShin/20180227/315017/Game_Level_Generation_Using_Neural_Networks.php)

[textgenrnn](https://github.com/minimaxir/textgenrnn)

[MarioNet: generating realistic game levels through deep learning](https://lib.ugent.be/fulltxt/RUG01/002/367/384/RUG01-002367384_2017_0001_AC.pdf)

