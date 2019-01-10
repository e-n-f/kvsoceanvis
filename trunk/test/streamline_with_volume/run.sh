#!/bin/sh
COMMAND=./streamline_with_volume

UDATA=~/Work/Data/Ocean/MOVE_19800101/U3D_intpo.ctl
VDATA=~/Work/Data/Ocean/MOVE_19800101/V3D_intpo.ctl
WDATA=~/Work/Data/Ocean/MOVE_19800101/W3D_intpo.ctl
TDATA=~/Work/Data/Ocean/MOVE_19800101/T3D_intpo.ctl

#UDATA=~/Work/Data/Ocean/MOVE_20061109/U3D_intpo.ctl
#VDATA=~/Work/Data/Ocean/MOVE_20061109/V3D_intpo.ctl
#WDATA=~/Work/Data/Ocean/MOVE_20061109/W3D_intpo.ctl
#TDATA=~/Work/Data/Ocean/MOVE_20061109/T3D_intpo.ctl

MINX=250
MINY=200
MINZ=0

MAXX=350
MAXY=300
MAXZ=20

$COMMAND $UDATA $VDATA $WDATA $TDATA $MINX $MINY $MINZ $MAXX $MAXY $MAXZ
