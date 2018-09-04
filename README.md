ZCU104 Video Mixer Demo
=======================

This repository contains the Video demo presented by Xilinx at FPL 2018
show-casing PYNQ on the ZCU104 as well as some of capabilities of the board. To
run the demo copy the notebooks folder on to the board and follow the
instructions in the \_README.ipynb notebook. This demo is designed for a
particular set of input and output resolutions and will need to be modified to
support anything other than 1080p in and 2160p output. 

Each notebook is designed to show off a particular aspect of PYNQ - I'm
claiming the untidy nature of some of this as a feature rather than a function
of time limits.

 1. VideoMixer.ipynb shows how an interactive set of widgets can be used to
    build a compelling demo for a new piece of IP - in this case the Video
    Mixer and the Optical Flow calculation
 2. Stats.ipynb shows interaction with the new PMBus API in PYNQ and how
    on-board power can be measured. It also plots graphs of the data in pseudo
    real time and writes the output to a window on the monitor so that you can see
    how the various layers affect the power consumption on the board. The Optical
    Flow layer in particular causes a noticeable increase in power when activated.
 3. Filter.ipynb is designed to showcase a workshop-style use where people can
    interact with the board to see in real time how adjusting filter
    coefficients change the output image. Ideally it should be shown in Jupyterlab
    alongside a optical filtering tutorial but this wasn't ready in time for FPL.
 4. Logo.ipynb was a last-minute addition to the demo to add the PYNQ logo to
    the HDMI output. It makes a good example of how easily a custom IP can be
    interfaced with from PYNQ using a combination of MMIO, numpy and PIL for doing
    image processing and manipulation to get the image into the correct format for
    the IP I put together.

The `ip` folder contains all of the custom IP needed to rebuild the design. It
has been tested with Vivado HLS 2018.2 and will need the xfopencv library to
compile the Optical Flow IP. Rebuilding the design will need Vivado 2018.1.  
