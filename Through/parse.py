import globvar
from dataset import *
from iniedge import *
from agree import *

def parse(chnsegs):
    iniedges(chnsegs)
    agenda=0
    while (agenda<len(globvar.chart)):
        edge=copy.deepcopy(globvar.chart[agenda])
        printedge(edge)
        oldpos=len(globvar.chart)
        process(edge)
        printchart(oldpos)
        agenda+=1
    parseresult=choosebestforest()
    printresult(parseresult)
    return(parseresult)

def process(edge):
    if (edge.state!='f'):
        addtochart(edge)
        if (not complete(edge)):
            forward(edge)
        else:
            backward(edge)
            bupredict(edge)

def forward(ice):
    if (ice.dotreg.end<len(globvar.prsrtable[ice.prsid].cfg.part)):
        cebegin=str(ice.sentreg.end)+globvar.prsrtable[ice.prsid].cfg.part[ice.dotreg.end]
        idxs=globvar.compbeginmap.get(cebegin)
        if (idxs):
            for i in range(0,len(idxs)):
                ce=globvar.chart[idxs[i]]
                if (ice.source=='l' or ce.source=='l' or (ice.state=="a" and ce.state=="a")):
                    newedge=copy.deepcopy(ice)
                    newedge.dotreg.end+=1
                    newedge.sentreg.end=ce.sentreg.end
                    newedge.son.append(ce.id)
                    addtoagenda(newedge)
    if (ice.dotreg.begin>0):
        ceend=str(ice.sentreg.begin)+globvar.prsrtable[ice.prsid].cfg.part[ice.dotreg.begin-1]
        idxs=globvar.compendmap.get(ceend)
        if (idxs):
            for i in range(0,len(idxs)):
                ce=globvar.chart[idxs[i]]
                if (ice.source=='l' or ce.source=='l' or (ice.state=="a" and ce.state=="a")):
                    newedge=copy.deepcopy(ice)
                    newedge.dotreg.begin-=1
                    newedge.sentreg.begin=ce.sentreg.begin
                    newedge.son.insert(0,ce.id)
                    addtoagenda(newedge)

def backward(ce):
    iceend=str(ce.sentreg.begin)+globvar.prsrtable[ce.prsid].cfg.reduct
    idxs=globvar.incompendmap.get(iceend)
    if (idxs):
        for i in range(0,len(idxs)):
            ice=globvar.chart[idxs[i]]
            if (ice.source=='l' or ce.source=='l' or (ice.state=="a" and ce.state=="a")):
                newedge=copy.deepcopy(ice)
                newedge.dotreg.end+=1
                newedge.sentreg.end=ce.sentreg.end
                newedge.son.append(ce.id)
                addtoagenda(newedge)
    icebegin=str(ce.sentreg.end)+globvar.prsrtable[ce.prsid].cfg.reduct
    idxs=globvar.incompbeginmap.get(icebegin)
    if (idxs):
        for i in range(0,len(idxs)):
            ice=globvar.chart[idxs[i]]
            if (ice.source=='l' or ce.source=='l' or (ice.state=="a" and ce.state=="a")):
                newedge=copy.deepcopy(ice)
                newedge.dotreg.begin-=1
                newedge.sentreg.begin=ce.sentreg.begin
                newedge.son.insert(0,ce.id)
                addtoagenda(newedge)

def bupredict(ce):
    idxs=globvar.prsfirstmap.get(globvar.prsrtable[ce.prsid].cfg.reduct)
    if (idxs):
        for i in range(0,len(idxs)):
            newedge=edgec()
            newedge.source='p'
            newedge.prsid=idxs[i]
            newedge.ruleid=globvar.prsrtable[idxs[i]].prsid
            newedge.dotreg.begin=0;
            newedge.dotreg.end=1;
            newedge.sentreg=ce.sentreg;
            newedge.son.append(ce.id);
            addtoagenda(newedge);

def addtochart(edge):
    if (complete(edge)):
        addcompmap(edge)
        addsentregmap(globvar.compsentregmap,edge)
    else:
        addincompmap(edge)
        addsentregmap(globvar.incompsentregmap,edge)

def addcompmap(ce):
    cebegin=str(ce.sentreg.begin)+globvar.prsrtable[ce.prsid].cfg.reduct
    idxs=globvar.compbeginmap.get(cebegin)
    if (idxs==None):
        idxs=[]
    idxs.append(ce.id)
    globvar.compbeginmap[cebegin]=idxs
    
    ceend=str(ce.sentreg.end)+globvar.prsrtable[ce.prsid].cfg.reduct
    idxs=globvar.compendmap.get(ceend)
    if (idxs==None):
        idxs=[]
    idxs.append(ce.id)
    globvar.compendmap[ceend]=idxs

def addincompmap(ice):
    if (ice.dotreg.begin>0):
        icebegin=str(ice.sentreg.begin)+globvar.prsrtable[ice.prsid].cfg.part[ice.dotreg.begin-1]
        idxs=globvar.incompbeginmap.get(icebegin)
        if (idxs==None):
            idxs=[]
        idxs.append(ice.id)
        globvar.incompbeginmap[icebegin]=idxs
    if (ice.dotreg.end<len(globvar.prsrtable[ice.prsid].cfg.part)):
        iceend=str(ice.sentreg.end)+globvar.prsrtable[ice.prsid].cfg.part[ice.dotreg.end]
        idxs=globvar.incompendmap.get(iceend)
        if (idxs==None):
            idxs=[]
        idxs.append(ice.id)
        globvar.incompendmap[iceend]=idxs

def addsentregmap(sentregmap,edge):
    regkey=str(edge.sentreg.begin)+"-"+str(edge.sentreg.end)
    idxs=sentregmap.get(regkey)
    if (idxs==None):
        idxs=[]
    idxs.append(edge.id)
    sentregmap[regkey]=idxs

def choosebestforest():
    bestforest=[]
    sentlen=len(globvar.chnsent)
    wordcount=[10000000 for i in range(sentlen+1)]
    besttree=[-1 for i in range(sentlen+1)]
    traceback=[i-1 for i in range(sentlen+1)]
    tracefront=[i+1 for i in range(sentlen+1)]
    wordcount[0]=0
    for i in range(0,sentlen+1):
        for j in range(0,i):
            regkey=str(j)+"-"+str(i)
            idxs=globvar.compsentregmap.get(regkey)
            if (idxs):
                currcount=wordcount[j]-2*(i-j)
            else:
                currcount=wordcount[j]+2*(i-j)
            if (currcount<wordcount[i]):
                traceback[i]=j
                wordcount[i]=currcount
                if (idxs):
                    besttree[i]=choosebesttree(idxs)
    i=sentlen
    while (i>0):
        assert(besttree[i]!=-1)
        bestforest.insert(0,besttree[i])
        i=traceback[i]
    return(bestforest)

def choosebesttree(idxs):
    minid=-1
    minmum=100000
    for i in range(0,len(idxs)):
        edge=globvar.chart[idxs[i]]
        nn=countnode(edge)
        if (edge.source=='d'):
            nn-=2
        if (nn<minmum):
            minmum=nn
            minid=idxs[i]
    return(minid)

def countnode(edge):
    nn=1
    for i in range(0,len(edge.son)):
        sone=findedge(edge.son[i])
        nn+=countnode(sone)
    return(nn)
