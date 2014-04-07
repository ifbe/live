startofhelp:
dq "F1: ramdump(arrow:see anywhere in memory,alt:transform)        "
dq "F2: picture                                                    "
dq "F3: 3d                                                         "
dq "F4: console(ls,cd,assembly instructions)                       "

endofhelp:

times 0x1000-(endofhelp-startofhelp) db 0
