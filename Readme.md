## Real Time Video Augmentation from Green Screen to 3D environemnt
The goal of this project is to place a human in a 3D environment by segmenting him from a green screen video. 

Please see the result video in the demo video folder for an example. 

We use Chroma Keying in YCrCb space to get a matte from the green screen video. Also we use Unreal Engine to generate videos of a 3D environemnt from a moving camera and also take a similar video with a white plane (using same camera motion). 

Additionally to enhance results we also use a light mask for the keying output to make the video more realistic. 

Since we did not find any proper open source Chroma Keying (green screen keying) code we wrote our own with a builtin parameter adjustment tool (written in Python using OpenCV). 



