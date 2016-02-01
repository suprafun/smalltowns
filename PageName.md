# Introduction #

This is a work in progress technical theory of how animation works on the client


# Details #

The resource manager loads and stores all the plain animations, these suffice for animated nodes.

Calling setAnimation passing the name of an existing animation to an Animated Node will set an animation to be used.

Animated Node's logic function should be called each frame to update any animation set.

Beings work more or less the same, except the setAnimation needs to create a new texture for every frame of animation, to apply clothes and hairstyle, which then is stored in a new animation.