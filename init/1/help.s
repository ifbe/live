startofhelp:
dq "F1: ramdump(arrow:see anywhere in memory,alt:transform)        "
dq "F2: picture                                                    "
dq "F3: 3d                                                         "
dq "F4: console(ls,cd,assembly instructions)                       "

times 0x400 db 0

dq "[0,7ff]:free                                                   "
dq "[800,fff]:keyboard buffer                                      "
dq "[1000,1fff]:idt                                                "
dq "[2000,27ff]:memory info                                        "
dq "[2800,2fff]:disk info                                          "
dq "[3000,3fff]:vesa info                                          "
dq "[4000,4fff]:acpi info                                          "
dq "[5000,5fff]:pci info                                           "
dq "[6000,6fff]:operation help                                     "
dq "[7000,77ff]:anscii pixel                                       "
dq "[7800,7fff]:scancode to anscii transform table                 "
dq "[8000,bfff]:function address                                   "
dq "[c000,cfff]:boot journal                                       "
dq "[40000,7ffff]:cached file allocation table                     "
dq "[80000,83fff]:current directory                                "
dq "[84000,8ffff]:stack                                            "
dq "[90000,97fff]:pml4,pdpt,pd,pt                                  "
dq "[98000,9ffff]:ahci physical region table                       "


endofhelp:

times 0x1000-(endofhelp-startofhelp) db 0
