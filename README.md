# [CAS-SGL](https://xenobyte.xyz/projects/?nav=cas-sgl)

## What is CAS-SGL?

  CAS-SGL (short for 'Cellular Automatta Simulator - SkeletonGL') is a high performance Conway's game of life
  simulator that allows for both custom and random universe seeding, includes seed saving and loading 
  support, as well as real time analysis of the current state of the simulated universe. It takes full advantage
  of modern rendering techniques to prevent the GPU from bottlnecking performance as well as both VSYNC and
  unlocked FPS modes to keep it from hogging resources in weaker machines, and taking full advantage of high
  tier processors on enthusiast builds.
  
  It's also a bitchin' screensaver. 

This program was made as an example application for the [SkeletonGL](https://xenobyte.xyz/projects/?nav=skeletongl) rendering engine. 

## System Requirements

* OS: Any modern UNIX system
* RAM: 64MB
* CPU: Yes


## Preview

<p align="center"><img src="https://i.imgur.com/YVkylYt.gif" /></p>


## Installation

1. Install the [SkeletonGL](https://xenobyte.xyz/projects/?nav=skeletongl) dependencies

    For Debia/Ubuntu systems:

    ```sh
    sudo apt-get update
    sudo apt-get install libsdl2-dev libglew-dev libfreetype6-dev
    ```
    
    For Arch based systems:
    
    ```sh
    sudo pacman -S sdl2 glew freetype2
    ```
    
2. Clone the Cas-SGL repo & play

    ```sh
    git clone https://git.xenobyte.xyz/XENOBYTE/cas-sgl
    cd cas-sgl/
    make
    ./cas-sgl
    ```
  Visit [xenobyte.xyz](https://xenobyte.xyz/projects/?nav=cas-sgl) for more information.
