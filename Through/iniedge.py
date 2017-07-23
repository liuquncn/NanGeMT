import copy
from segment import *
from postag import *
from prsrbase import *
from agree import *

def iniedges(segs):
    for i in range (0,len(segs)):
        if (segs[i].flag=="w"):
            edges=wordstrtoedge(segs[i].seq,'a')
        elif (segs[i].flag=="t"):
            edges=wordtagtoedge(segs[i].seq,'a')
        elif (segs[i].flag=="p"):
            edges=phrasetoedge(segs[i].seq,'a')
        else:
            assert(false)

def wordtagtoedge(wordtag,state):
    bspos=wordtag.find("/")
    word=wordtag[:bspos]
    cat=wordtag[bspos+1:]
    subtags=maketag(word,cat,globvar.initpos)
    edges=makeedge(subtags,'d',state)
    globvar.chnsent+=word
    globvar.initpos+=len(word)
    return(edges)

def wordstrtoedge(wordstr,state):
    subwords=dictsegment(wordstr)
    subtags=postag(subwords,globvar.initpos)
    edges=makeedge(subtags,'d',state)
    globvar.chnsent+=wordstr
    globvar.initpos+=len(wordstr)
    return(edges)

def makeedge(tags,source,state):
    edges=[]
    for i in range(0,len(tags)):
        prsr=prsrc()
        prsr.prsid=tags[i].catid
        prsr.cfg.reduct=tags[i].cat
        prsr.cfg.part.append(tags[i].word)
        prsr.head=0
        globvar.prsrtable.append(prsr)

        edge=edgec()
        edge.id=len(globvar.chart)
        edge.father=-2
        edge.state=state
        edge.source=source
        edge.prsid=len(globvar.prsrtable)-1
        edge.ruleid=tags[i].catid
        edge.feat=tags[i].feat
        edge.dotreg.begin=0
        edge.dotreg.end=1
        edge.sentreg=tags[i].sentreg
        edges.append(edge)
        globvar.chart.append(edge)
        printedge(edge)
        compedgekey=compedgekeyc(tags[i].cat,edge.sentreg,tags[i].feat)
        globvar.compedgemap[compedgekey]=edge.id
        for j in range(0,len(tags[i].ruleset)):
            prsr=parserulentry(tags[i].ruleset[j])
            globvar.prsrtable.append(prsr)
            locnewe=edgec()
            locnewe.source='l'
            locnewe.sentreg=tags[i].sentreg
            locnewe.prsid=len(globvar.prsrtable)-1
            locnewe.ruleid=prsr.prsid
            locnewe.dotreg=getdotreg(tags[i].ruleset[j],tags[i])
            locnewe.son.append(edge.id)
            addtoagenda(locnewe)
    return(edges)

def phrasetoedge(phrase,state):
    red,body=sepredbody(phrase)
    temppos=globvar.initpos
    edgem=bodytoedge(body)
    edges=createedge(red,edgem,temppos,state)
    return(edges)

def sepredbody(phrase):
    lbpos=phrase.find("(")
    rbpos=phrase.rfind(")")
    red=phrase[:lbpos]
    body=phrase[lbpos+1:rbpos]
    return(red,body)

def bodytoedge(body):
    edgem=[]
    subsegs=compseg(body)
    for i in range(0,len(subsegs)):
        if (subsegs[i].flag=="t"):
            edges=wordtagtoedge(subsegs[i].seq,'f')
        elif (subsegs[i].flag=="p"):
            edges=phrasetoedge(subsegs[i].seq,'f')
        else:
            assert(False)
        edgem.append(edges)
    return(edgem)

def createedge(red,edgem,initpos,state):
    edges=[]
    cfg=createcfg(red,edgem)
    prsrs=findprsr(cfg)
    for i in range(0,len(prsrs)):
        edge=getedge(prsrs[i],initpos)
        locchart=makelocchart(edgem)
        subedges=parseoneprsr(edge,locchart,state)
        edges+=subedges
    if (len(edges)==0):
        print "incorrect input structure"
        print len(prsrs)
        print globvar.prsrtable[prsrs[0]].prsid
        assert(False)
    return(edges)

def createcfg(red,edgem):
    cfg=cfgc()
    cfg.reduct=red
    for i in range(0,len(edgem)):
        cfg.part.append(globvar.prsrtable[edgem[i][0].prsid].cfg.reduct)
    return(cfg)

def findprsr(cfg):
    prsrs=[]
    prsids=globvar.prsredmap[cfg.reduct]
    assert(prsids!=None)
    for i in range (0,len(prsids)):
        if (cfg.part==globvar.prsrtable[prsids[i]].cfg.part):
            prsrs.append(prsids[i])
    return(prsrs)

def getedge(prsid,initpos):
    edge=edgec()
    edge.source='p'
    edge.state='f'
    edge.father=-2
    edge.prsid=prsid
    edge.ruleid=globvar.prsrtable[prsid].prsid
    edge.dotreg.begin=0
    edge.dotreg.end=0
    edge.sentreg.begin=initpos
    edge.sentreg.end=initpos
    return(edge)

def makelocchart(edgem):
    locchart=[]
    for i in range(0,len(edgem)):
        locchart+=edgem[i]
    return(locchart)

def parseoneprsr(iniedge,locchart,state):
    edges=[]
    agenda=[]
    agenda.append(iniedge)
    while (len(agenda)!=0):
        edge=agenda.pop(0)
        if (complete(edge)):
            bind=globvar.prsrtable[edge.prsid].bind
            nbind,res=agree(bind,edge)
            if (res==True):
                edge.id=len(globvar.chart)
                edge.state=state
                edge.feat=bindtofeat(nbind)
                edges.append(edge)
                globvar.chart.append(edge)
                printedge(edge)
        else:
            for i in range(0,len(locchart)):
                if (edge.sentreg.end==locchart[i].sentreg.begin):
                    newedge=copy.deepcopy(edge)
                    newedge.father=-2
                    newedge.dotreg.end+=1
                    newedge.sentreg.end=locchart[i].sentreg.end
                    newedge.son.append(locchart[i].id)
                    agenda.append(newedge)
    return(edges)

