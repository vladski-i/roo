# Roo
---
## Prerequisites
 - docker, jack2 running
## How to build
 - To build the docker image, run   
 `docker build -t roo-build .`
 - The image's entrypont is `make` so any arguments passed after  
 `docker run -v $(pwd):/roo roo-build roo`  
  are passed to make e.g.  
  `docker run -v $(pwd):/roo roo-build roo clean`


## How to run
 - `./roo` should work and generate a sine

 ## Recommendations
  - Although not needede for running roo, `libjack-jackd2-dev` is nice for VSCode intellisense
   `sudo apt install libjack-jackd2-dev` should do the trick for you