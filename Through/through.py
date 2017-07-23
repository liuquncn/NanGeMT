#!/usr/bin/env python

from loadkbase import *
from segment import *
from iniedge import *
from transform import *
from maktree import *
from bldword import *

loadspeword()
loadkbase()
chnsentf=open('chnsent.txt','r')
while (True):
    compsent=removespace(chnsentf.readline())
    if (compsent==""):
        break
    print compsent
    resetglobvar()
    chnsegs=compseg(compsent)
    parseresult=parse(chnsegs)
    trnsresult=transform(parseresult)
    maktree(trnsresult)
    bldword(trnsresult)
chnsentf.close()
