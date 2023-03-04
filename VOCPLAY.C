//███████████████████████████████████████████████████████████████████████████
//██                                                                       ██
//██   VOCPLAY.C                                                           ██
//██                                                                       ██
//██   Creative Voice File (.VOC) performance utility                      ██
//██                                                                       ██
//██   V2.00 of 09-Oct-91                                                  ██
//██   V2.01 of 09-Apr-92: .VOC preformatting call added                   ██
//██                                                                       ██
//██   Project: IBM Audio Interface Library                                ██
//██    Author: John Miles                                                 ██
//██                                                                       ██
//██   C source compatible with Turbo C++ v1.0 or later                    ██
//██                                                                       ██
//███████████████████████████████████████████████████████████████████████████
//██                                                                       ██
//██   vocplay.obj: vocplay.c gen.h ail.h                                  ██
//██      bcc -ml -c -v vocplay.c                                          ██
//██                                                                       ██
//██   vocplay.exe: vocplay.obj gen.lib ail.obj                            ██
//██      tlink @vocplay.lls                                               ██
//██                                                                       ██
//██   Contents of VOCPLAY.LLS:                                            ██
//██     /c /v /x +                                                        ██
//██     \bc\lib\c0l.obj +                                                 ██
//██     vocplay ail, +                                                    ██
//██     vocplay.exe, +                                                    ██
//██     vocplay.map, +                                                    ██
//██     \bc\lib\cl.lib gen.lib                                            ██
//██                                                                       ██
//███████████████████████████████████████████████████████████████████████████
//██                                                                       ██
//██   Copyright (C) 1991, 1992 Miles Design, Inc.                         ██
//██                                                                       ██
//██   Miles Design, Inc.                                                  ██
//██   10926 Jollyville #308                                               ██
//██   Austin, TX 78759                                                    ██
//██   (512) 345-2642 / FAX (512) 338-9630 / BBS (512) 454-9990            ██
//██                                                                       ██
//███████████████████████████████████████████████████████████████████████████

#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <alloc.h>

#include "ail.h"        // Audio Interface Library API header file
#include "gen.h"        // General DOS and system functions

const char VERSION[] = "2.01";

/*****************************************************************/
void main(int argc, char *argv[])
{
   HDRIVER hdriver;
   char far *vfile;
   char far *drvr;
   drvr_desc far *desc;

   printf("\nVOCPLAY version %s                Copyright (C) 1991, 1992 Miles Design, Inc.\n",VERSION);
   printf("-------------------------------------------------------------------------------\n\n");

   if (argc != 3)
      {
      printf("This program plays a Creative Voice File (.VOC) through any Audio Interface\n");
      printf("Library digital sound driver.\n\n");
      printf("Usage: VOCPLAY filename.voc driver.adv\n");
      exit(1);
      }

   AIL_startup();

   drvr = load_driver(argv[2]);
   if (drvr == NULL)
      {
      printf("Couldn't load driver.\n");
      AIL_shutdown(NULL);
      exit(1);
      }

   hdriver = AIL_register_driver(drvr);
   if (hdriver==-1)
      {
      printf("Driver %s not compatible with linked API version.\n"
         ,argv[2]);
      AIL_shutdown(NULL);
      exit(1);
      }

   desc = AIL_describe_driver(hdriver);

   if (desc->drvr_type != DSP_DRVR)
      {
      printf("%s is not a digital sound driver.\n",argv[2]);
      AIL_shutdown(NULL);
      exit(1);
      }
   
   if (!AIL_detect_device(hdriver,desc->default_IO,desc->default_IRQ,
      desc->default_DMA,desc->default_DRQ))
         {
         printf("Sound hardware not found.\n");
         AIL_shutdown(NULL);
         exit(1);
         }

   AIL_init_driver(hdriver,desc->default_IO,desc->default_IRQ,
      desc->default_DMA,desc->default_DRQ);

   vfile = read_file(argv[1],NULL);
   if (vfile == NULL)
      {
      printf("Couldn't load %s.\n",argv[1]);
      AIL_shutdown(NULL);
      exit(1);
      }

   AIL_format_VOC_file(hdriver,vfile,-1);

   AIL_play_VOC_file(hdriver,vfile,-1);
   AIL_start_digital_playback(hdriver);

   printf("Launching DOS shell.  Type 'EXIT' to stop playback and ");
   printf("exit VOCPLAY.");
   spawnlp(P_WAIT,"command.com",NULL);

   printf("VOCPLAY stopped.\n");
   AIL_shutdown(NULL);
}

