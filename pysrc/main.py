# Copyright (c) 2014, Kate Stone
# All rights reserved.
#
# This file is covered by the 3-clause BSD license.
# See the LICENSE file in this program's distribution for details.

import logging
from logging import FileHandler

main_log = logging.getLogger ("PyDoom")
main_log.setLevel ("INFO")
main_log.addHandler (FileHandler ("pydoom.log"))

from version import GITVERSION
from time import time, sleep
from sys import argv, path
import PyDoom_OpenGL
from pydoom.arguments import ArgumentParser
from pydoom.graphics import MakePalettes, Image
from pydoom.resources import WadFile

def main ():
    main_log.info ("=== PyDoom revision {} ===".format (GITVERSION))
    args = ArgumentParser (argv[1:])
    args.CollectArgs ()
    width, height = (640, 480)
    fullscreen = False
    if args.resolution[0] is not None:
        width = args.resolution[0]
    if args.resolution[1] is not None:
        height = args.resolution[1]
    if args.fullscreen is not None:
        fullscreen = args.fullscreen
    del args
    
    doom2 = WadFile ("doom2.wad")
    palette = MakePalettes (doom2.ReadLump (doom2.FindFirstLump ("playpal")))[0]
    titlepic = Image.LoadDoomGraphic (doom2.ReadLump (doom2.FindFirstLump ("titlepic")), palette)
    PyDoom_OpenGL.CreateWindow ((width, height), fullscreen)
    titletex = PyDoom_OpenGL.LoadTexture (titlepic)
    stoptime = time () + 30.0
    while time () < stoptime:
        PyDoom_OpenGL.BeginDrawing ()
        PyDoom_OpenGL.Draw2D (titletex, (0, 0, width, height))
        PyDoom_OpenGL.FinishDrawing ()
        sleep (0.1)
    PyDoom_OpenGL.DestroyWindow ()
