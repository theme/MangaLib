#!/bin/bash

for s in 16 32 64 96 128 ;
do
   inkscape --export-png icon${s}.png -w $s icon.svg ;
done

