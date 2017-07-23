import globvar
from dataset import *
from basfunc import *

def shallowprsrbase():
    for i in range(0,len(globvar.prsrset)):
        prsr=parserulentry(globvar.prsrset[i])
        globvar.prsrtable.append(prsr)
        addprsrmap(globvar.prsredmap,prsr.cfg.reduct,i)
        addprsrmap(globvar.prsfirstmap,prsr.cfg.part[0],i)
        addprsrmap(globvar.prslastmap,prsr.cfg.part[-1],i)

def addprsrmap(prsrmap,str,idx):
    idxs=prsrmap.get(str)
    if (idxs==None):
        idxs=[]
    idxs.append(idx)
    prsrmap[str]=idxs

def parserulentry(entry):
    prsr=prsrc()
    
    idsrch=re.search(globvar.idreg,entry)
    if (idsrch):
        prsr.prsid=idsrch.group(0)[1:-1]
    
    cfgreg="([\w| ]+)(->)(.+?)(::|\|\||=>)" 
    cfgsrch=re.search(cfgreg,entry,re.S)
    if (cfgsrch):
        cfgent=advrmspace(cfgsrch.group(1)+cfgsrch.group(2)+cfgsrch.group(3))
        prsr.cfg,prsr.head,prsr.bind=parsecfg(cfgent)
    else:
        print entry
        assert(False)

    bindreg="(::)(.+?)(\|\||=>)"
    bindsrch=re.search(bindreg,entry,re.S)
    if (bindsrch):
        prsr.bind+=advrmspace(bindsrch.group(2))
    
    return(prsr)
    
def parsecfg(entry):
    cfg=cfgc()
    head=0
    yxbind=""

    arrowpos=entry.find("->")
    cfg.reduct=removespace(entry[:arrowpos])
    remain=removespace(entry[arrowpos+2:])
    while (remain!=""):
        oneu,remain=getoneunit(remain)
        if (oneu[0]=='!'):
            head=len(cfg.part)
            oneu=oneu[1:]
        lanpos=oneu.find("<")
        if (lanpos!=-1):
            ranpos=oneu.find(">")
            cat=oneu[0:lanpos]
            yx=oneu[lanpos+1:ranpos]
            yx.replace("\"","")
            repeat=1
            for i in range(0,len(cfg.part)):
                if (cfg.part[i]==cat):
                    repeat+=1
            intro=""
            for i in range(0,repeat):
                intro+="%"
            intro+=cat
            yxbind+=intro+".yx="+yx+","
        else:
            cat=oneu
        cfg.part.append(cat)
    return(cfg,head,yxbind)

def getdotreg(entry,tag):
    dotreg=regionc()
    bodyreg='(->)(.+)(::|\|\||=>)'
    bodysrch=re.search(bodyreg,entry,re.S)
    if (bodysrch):
        dotpos=0
        remain=advrmspace(bodysrch.group(0)[2:-3])
        while (remain!=""):
            oneu,remain=getoneunit(remain)
            if (oneu[0]=='!'):
                oneu=oneu[1:]
            lanpos=oneu.find("<")
            if (lanpos!=-1):
                ranpos=oneu.find(">")
                cat=oneu[:lanpos]
                word=oneu[lanpos+1:ranpos]
                word=re.sub('\"',"",word)
                if (cat==tag.cat and word==tag.word):
                    dotreg.begin=dotpos
                    dotreg.end=dotpos+1
                    break
            dotpos+=1
    return(dotreg)
