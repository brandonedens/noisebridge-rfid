
===========================
 Noisebridge RFID Software
===========================

:author: Brandon Edens
:date: 2013-01-29

Introduction
============

This repository houses software used by Noisebridge to read and interact with
the RFID tag reader located in the space. Here you'll find software that reads
tags from serial and then multicasts that information onto Noisebridge's local
ethernet network. This repo also contains various sample programs for reading
that information off of mcast and performing various tasks with that
information.


rfid2mcast
==========

This is the software that synchronously monitors the RS232 port on the SAMSys
RFID reader, resending that information onto Noisebridge's local network.

At the time of writing the multicast group address is 224.1.1.1 and the port
used is 11332. However, please treat the MCAST_GRP and MCAST_PORT variables in
rfid2mcast.py as the definitive values.


rfid-mcast-print
================

This software opens a socket to the multicast IP address and port that receives
information about RFID tags and begins printing the TAG data.

