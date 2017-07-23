import globvar
from parse import *
from agree import *

def transform(parseresult):
    trnsresult=[]
    for i in range(0,len(parseresult)):
        transformtree(globvar.chart[parseresult[i]])
        addtrnsresult(globvar.chart[parseresult[i]],trnsresult);
    print "------- trnsresult --------"
    printresult(trnsresult)
    printsent(trnsresult)
    return(trnsresult)

def addtrnsresult(root,trnsresult):
    idx=globvar.chtoenidx.get(root.id)
    if (idx and (len(idx)!=0)):
        minid=choosebesttree(idx)
        trnsresult.append(minid)
    else:
        for i in range(0,len(root.son)):
            addtrnsresult(globvar.chart[root.son[i]],trnsresult)

def transformtree(root):
    enedges=[]
    succ=True
    if (len(root.son)!=0):
        transition(root)
        for i in range(0,len(root.son)):
            succ=transformtree(globvar.chart[root.son[i]]) and succ
    printedge(root)
    if (succ):
        enedges=transformedge(root)
    if (len(enedges)==0):
        print "----- transform fail ------"
        return(False)
    else:
        printedges(enedges)
        return(True)

def transformedge(edge):
    enedges=[]
    if (len(edge.son)==0):
        if (globvar.prsrtable[edge.prsid].prsid=="unk"):
            if (globvar.prsrtable[edge.prsid].cfg.reduct=="m"): 
                entran="M<"+globvar.prsrtable[edge.prsid].cfg.part[0]+">"
            else:
                entran="N<"+globvar.prsrtable[edge.prsid].cfg.part[0]+">"
            eid=createatomedge(entran)
            globvar.chart[eid].sentreg=edge.sentreg
            enedges.append(globvar.chart[eid])
            globvar.entochidx[eid]=edge.id
        else:
            entry=getentry(globvar.prsrtable[edge.prsid].cfg.part[0])
            catentry=getcatentry(globvar.prsrtable[edge.prsid].prsid,entry)
            tranentry=gettranentry(edge,catentry)
            if (tranentry!=""):
                transet=tranentry.split("=>")
                enedges=prsrtranedge(edge,transet)
    else:
        rule=getrule(edge.source,globvar.prsrtable[edge.prsid].prsid)
        tranentry=gettranentry(edge,rule)
        if (tranentry!=""):
            transet=tranentry.split("=>")
            enedges=prsrtranedge(edge,transet)
  
    edgeids=[]
    for i in range(0,len(enedges)):
        edgeids.append(enedges[i].id)
    globvar.chtoenidx[edge.id]=edgeids

    return(enedges)

def prsrtranedge(edge,transet):
    enedges=[]
    for i in range(0,len(transet)):
        tranid,trancfg,tranbind=sepatranentry(transet[i])
        subenegs=prsronetran(edge,trancfg,tranbind)
        for j in range(0,len(subenegs)):
            subenegs[j].sentreg=edge.sentreg
            subenegs[j].ruleid+=":"+tranid
            globvar.entochidx[subenegs[j].id]=edge.id
        enedges+=subenegs
    return(enedges)

def prsronetran(edge,trancfg,tranbind):
    locchart=[]
    parts=[]
    enred,itemset=getitemset(trancfg)
    for i in range(0,len(itemset)):
        phreg='(\w+?)\(.+\)'
        phsrch=re.search(phreg,itemset[i])
        if phsrch:
            parts.append(phsrch.group(1))
            subenegs=prsronetran(edge,itemset[i],"")
            for j in range(0,len(subenegs)):
                subenegs[j].sentreg.begin=i
                subenegs[j].sentreg.end=i+1
                globvar.chtoenidx[-1].append(subenegs[j].id)
                globvar.entochidx[subenegs[j].id]=-1
            locchart+=subenegs
        else:
            parts.append(itemset[i])
            locchart+=getatomlocchart(edge,itemset[i],i)
    flatcfg=getflatcfg(trancfg,enred,parts)
    enedges=flattranedge(edge,flatcfg,tranbind,locchart)
    return(enedges)

def getflatcfg(trancfg,enred,parts):
    if (len(parts)==0):
        flatcfg=trancfg
    else:
        flatcfg=enred+"("
        for i in range(0,len(parts)):
            flatcfg+=parts[i]+" "
        flatcfg+=")"
    return(flatcfg)

def getatomlocchart(edge,atomtran,loc):
    enedges=[]
    atreg="!?([^/]+)((/+)([^<]+)(<.+>)?)?"
    atsrch=re.search(atreg,atomtran)
    if (not atsrch.group(2)):
        eid=createatomedge(atomtran)
        globvar.chart[eid].sentreg.begin=loc
        globvar.chart[eid].sentreg.end=loc+1
        globvar.chtoenidx[-1].append(eid)
        globvar.entochidx[eid]=-1
        enedges=[globvar.chart[eid]]
    elif (atsrch.group(5)):
        atomtran=atsrch.group(1)+atsrch.group(5)
        eid=createatomedge(atomtran)
        globvar.chart[eid].sentreg.begin=loc
        globvar.chart[eid].sentreg.end=loc+1
        globvar.chtoenidx[-1].append(eid)
        globvar.entochidx[eid]=-1
        enedges=[globvar.chart[eid]]
    else:
        cheid=getchedge(edge,len(atsrch.group(3)),atsrch.group(4))
        enedges=findenedges(cheid,atsrch.group(1))
        for i in range(0,len(enedges)):
            enedges[i].sentreg.begin=loc
            enedges[i].sentreg.end=loc+1
    return(enedges)

def getchedge(edge,repeat,chcat):
    for i in range(len(globvar.prsrtable[edge.prsid].cfg.part)):
        if (globvar.prsrtable[edge.prsid].cfg.part[i]==chcat):
            repeat-=1
        if (repeat==0):
            return(edge.son[i])
    assert(False)

def findenedges(chsonid,encat):
    enedges=[]
    idx=globvar.chtoenidx[chsonid]
    for i in range(0,len(idx)):
        if (globvar.prsrtable[globvar.chart[idx[i]].prsid].cfg.reduct==encat):
            enedges.append(globvar.chart[idx[i]])
    return(enedges)

def flattranedge(edge,trancfg,tranbind,locchart):
    enedges=[]
    enred,itemset=getitemset(trancfg)
    if (len(itemset)!=0):
        enedge=getenedge(edge,enred,itemset,tranbind)
        enedges=parseonerule(enedge,locchart)
    else:
        eid=createatomedge(trancfg)
        globvar.chart[eid].ruleid=edge.ruleid
        if (tranbind!=""):
            globvar.chart[eid].feat=bindtofeat([tranbind])
        else:
            globvar.chart[eid].feat=""
        enedges.append(globvar.chart[eid])
    return(enedges)

def getenedge(edge,enred,itemset,tranbind):
    prsr=prsrc()
    prsr.prsid=edge.ruleid
    prsr.cfg.reduct=enred
    prsr.bind=tranbind
    prsr.head,prsr.cfg.part=getheadpart(itemset)
    globvar.prsrtable.append(prsr)
    
    enedge=edgec()
    enedge.source='t'
    enedge.father=-2
    enedge.prsid=len(globvar.prsrtable)-1
    enedge.ruleid=edge.ruleid
    enedge.dotreg.begin=0
    enedge.dotreg.end=0
    enedge.sentreg.begin=0
    enedge.sentreg.end=0
    return(enedge)

def getheadpart(itemset):
    head=-1
    part=[]
    atreg="(!?)([^/|^<]+)"
    for i in range(0,len(itemset)):
        atsrch=re.search(atreg,itemset[i])
        assert(atsrch)
        if (atsrch.group(1)):
            head=i
        part.append(atsrch.group(2))
    return(head,part)

def parseonerule(iniedge,locchart):
    edges=[]
    agenda=[]
    agenda.append(iniedge)
    while (len(agenda)!=0):
        edge=agenda.pop(0)
        if (complete(edge)):
            nbind,res=trnagree(globvar.prsrtable[edge.prsid].bind,edge)
            if (res):
                edge.id=len(globvar.chart)
                edge.feat=bindtofeat(nbind)
                edges.append(edge)
                globvar.chart.append(edge)
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

def createatomedge(tranitem):
    aedge=edgec()
    tranitem=removespace(tranitem)
    itemreg='!?(\w+)(<(.+)>)?'
    itemsrch=re.search(itemreg,tranitem)
    prsr=prsrc()
    prsr.cfg.reduct=itemsrch.group(1)
    if (itemsrch.group(3)):
        prsr.cfg.part.append(itemsrch.group(3))
    else:
        prsr.cfg.part.append("")
    globvar.prsrtable.append(prsr)
    #aedge.id=len(globvar.enchart)
    aedge.id=len(globvar.chart)
    aedge.prsid=len(globvar.prsrtable)-1
    aedge.source='t'
    aedge.father=-2
    aedge.dotreg.begin=0
    aedge.dotreg.end=1
    #globvar.enchart.append(aedge)
    globvar.chart.append(aedge)
    return(aedge.id)

def getentry(word):
    entryreg="(\$\$ "+word+".+?)\$\$"
    entrysrch=re.search(entryreg,globvar.dictn,re.S)
    assert(entrysrch)
    return(entrysrch.group(1))

def getcatentry(catid,entry):
    intro="**{"+catid+"}"
    bpos=entry.find(intro)
    assert(bpos!=-1)
    nrpos=entry.find("&&",bpos+len(intro)) & 0xffffffff
    ncpos=entry.find("**",bpos+len(intro)) & 0xffffffff
    catentry=entry[bpos+len(intro):min(nrpos,ncpos)]
    return(catentry)

def gettranentry(edge,catentry):
    tranentry=""
    if (catentry.find("||")!=-1):
        swiset=catentry.split("||")
        del swiset[0]
        for i in range(0,len(swiset)):
            intropos=swiset[i].find("=>")
            assert(intropos!=-1)
            temp=removespace(swiset[i][:intropos])
            tranreg="({[\w|\.]+})?(.+)?"
            transrch=re.search(tranreg,temp)
            res=True
            if (transrch.group(2)):
                nbind,res=swiagree(transrch.group(2),edge)
            if (res):
                if (transrch.group(1)):
                    edge.ruleid+=":"+transrch.group(1)[1:-1]
                tranentry=swiset[i][intropos+2:]
                break
    else:
        intropos=catentry.find("=>")
        if (intropos!=-1):
            tranentry=catentry[intropos+2:]
        else:
            tranentry=""
    return(tranentry)

def getrule(source,prsid):
    rule=""
    if (source=='p'):
        bpos=globvar.prsrbase.find("{"+prsid+"}")
        epos=globvar.prsrbase.find("&&",bpos)
        rule=globvar.prsrbase[bpos+len(prsid)+2:epos]
    elif (source=='l'):
        bpos=globvar.dictn.find("{"+prsid+"}")
        nrpos=globvar.dictn.find("&&",bpos) & 0xffffffff
        ncpos=globvar.dictn.find("**",bpos) & 0xffffffff
        nepos=globvar.dictn.find("$$",bpos) & 0xffffffff
        epos=min(nrpos,min(ncpos,nepos))
        rule=globvar.dictn[bpos+len(prsid)+2:epos]
    else:
        assert(False)
    return(rule)

def sepatranentry(tranentry):
    tranentry=removespace(tranentry)
    tranreg='({[\w|\.]+})?([^{|^}|^\$|^%]+)([\$|%](.+))?'
    transrch=re.search(tranreg,tranentry)
    assert(transrch)
    tranid=""
    if (transrch.group(1)):
        tranid=transrch.group(1)[1:-1]
    trancfg=removespace(transrch.group(2))
    tranbind=""
    if (transrch.group(3)):
        tranbind=transrch.group(3)
    return(tranid,trancfg,tranbind)

def getitemset(trancfg):
    phreg='(\w+)(<.+>)?(\(.+\))?'
    phsrch=re.search(phreg,trancfg)
    assert(phsrch)
    enred=phsrch.group(1)
    itemset=[]
    if (phsrch.group(3)):
        temp=phsrch.group(3)[1:-1]
        while (temp!=""):
            oneu,temp=getoneunit(temp)
            itemset.append(oneu)
    return(enred,itemset)

def trnagree(bind,edge):
    nbind=[]
    res=True
    bindset=getabindset(bind)
    for i in range(len(bindset)):
        anbind,res=trnaagree(bindset[i],edge)
        if (res==False):
            break
        else:
            nbind+=anbind
    return(nbind,res)

def trnaagree(bind,edge):
    nbind=[]
    res=True
    if (bind.find("@")!=-1):
        nbind,res=trnatagree(bind,edge)
    else:
        lfeat,opera,rfeat=getopeval(bind)
        rval,rvaltype,rvalset=trnprocfeatval(rfeat,edge)
        if (lfeat.find("$")==0 and edge.feat==""):
            nbind.append(lfeat+opera+rval)
            res=True
        else:
            lval,lvaltype,lvalset=trnprocfeatval(lfeat,edge)
            if (opera=="==" and (lval=="" or rval=="")):
                if (lval=="" and rval==""):
                    #nbind.append(bind)
                    pass
                elif (lval==""):
                    nbind.append(lfeat+opera+rval)
                else:
                    nbind.append(rfeat+opera+lval)
                res=True
            else:
                if (lvaltype!=""):
                    newval,res=valagree(lval,rval,opera,lvaltype,lvalset)
                    if (res==True and lfeat.find(".")!=-1):
                        nbind.append(lfeat+"="+newval)
                elif (rvaltype!=""):
                    newval,res=valagree(lval,rval,opera,rvaltype,rvalset)
                    if (res==True and rfeat.find(".")!=-1):
                        nbind.append(rfeat+"="+newval)
    return(nbind,res)

def trnatagree(bind,edge):
    nbind=[]
    res=True
    atset=bind.split("@")
    atset[0]=removespace(atset[0])
    lfeat,opera,rfeat=getopeval(atset[0])
    for i in range(1,len(atset)):
        temp=removespace(atset[i])
        atbind=lfeat+"."+temp+opera+rfeat+"."+temp
        natbind,res=trnaagree(atbind,edge)
        if (res==False):
            break
        else:
            nbind+=natbind
    return(nbind,res)

def trnprocfeatval(featname,edge):
    featval=""
    feattype=""
    featvalset=[]
    if (featname.find("%")==0):
        dotreg='(.+?)(\.)(.+)'
        dotsrch=re.search(dotreg,featname)
        if (dotsrch):
            sone=findsonedge(dotsrch.group(1),edge)
            fname=dotsrch.group(3)
            featval,feattype,featvalset=trnfindfeatval(fname,sone)
        else:
            sonid=findsonid(featname,edge)
            featval="eid"+str(sonid)
    elif (featname.find("$")==0):
        dotreg='(.+?)(\.)(.+)'
        dotsrch=re.search(dotreg,featname)
        if (dotsrch):
            fname=dotsrch.group(3)
            featval,feattype,featvalset=trnfindfeatval(fname,edge)
        else:
            featval="eid"+str(edge.id)
    else:
        featval=featname
    return(featval,feattype,featvalset)

def trnfindfeatval(featname,edge):
    featval=""
    featype=""
    featvalset=[]
    nameset=featname.split(".")
    curredge=edge
    for i in range(0,len(nameset)):
        featval,featype,featvalset=trnbasfeatval(nameset[i],curredge)
        if (featype=="Edge"):
            if (featval.find("[")==0):
                nedge=edgec()
                nedge.feat=featval
                curredge=nedge
            elif (featval.find("eid")==0):
                eid=string.atoi(featval[3:])
                curredge=findedge(eid)
            else:
                curredge=edgec()
    if (featype==""):
        print featname,featval
        assert(False)
    return(featval,featype,featvalset)

def trnbasfeatval(featname,edge):
    featval=""
    feattype=""
    featvalset=[]
    if (featname.find("eid")!=-1):
        featval=featname
        feattype="Edge"
    elif (featname==globvar.hanzi):
        if (len(globvar.prsrtable[edge.prsid].cfg.part)!=0):
            featval=globvar.prsrtable[edge.prsid].cfg.part[0]
        else:
            featval=""
        feattype="Symbol"
    elif (featname=="yx" or featname=="YX"):
        featval=recurprocyx(edge)
        feattype="Symbol"
    elif (featname==globvar.chongdie):
        featval=procchongdie(edge)
        feattype="Symbol"
    elif (featname=="ccat" or featname==globvar.hanyucilei):
        featval=findcat(edge)
        feattype="Symbol"
    elif (featname=="ECAT" or featname==globvar.yingyucilei):
        featval=findcat(edge)
        feattype="Symbol"
    elif (featname=="cpcat" or featname==globvar.hanyuduanyu):
        featval=globvar.prsrtable[edge.prsid].cfg.reduct
        feattype="Symbol"
    elif (featname==globvar.yinjie):
        featval=procyinjie(edge)
        feattype="Number"
    elif (featname=="sem" or featname==globvar.yuyilei):
        featval,res=moregetfeatval(featname,edge)
        feattype="Hierar"
    elif (globvar.jufachengfen.find(","+featname+",")!=-1):
        defval,feattype,featvalset=checkmodel(featname)
        featval,res=getfeatval(featname,edge.feat)
        if ((not res) and (defval!="")):
            featval=defval
        feattype="Edge"
    elif (globvar.yuyichengfen.find(","+featname+",")!=-1):
        defval,feattype,featvalset=checkmodel(featname)
        featval,res=moregetfeatval(featname,edge)
        if (not res):
            featval=defval
        feattype="Edge"
    else:
        defval,feattype,featvalset=checkmodel(featname)
        featval,res=moregetfeatval(featname,edge)
    return(featval,feattype,featvalset)

def transition(edge):
    feat=edge.feat[1:-1]
    featset=getabindset(feat)
    feat=""
    for i in range(0,len(featset)):
        featreg='(%\w+?)\.(.+?):(.+)'
        featsrch=re.search(featreg,featset[i])
        if (not featsrch):
            feat+=featset[i]+","
        else:
            sonid=findsonid(featsrch.group(1),edge)
            modifeatval(sonid,featsrch.group(2),featsrch.group(3))
    globvar.chart[edge.id].feat="["+feat[:-1]+"]"

def modifeatval(eid,featname,nval):
    res=False
    featval=""
    nameset=featname.split(".")
    for i in range(0,len(nameset)):
        edge=findedge(eid)
        featval,res=getfeatval(nameset[i],edge.feat)
        if (featval.find("eid")==0):
            eid=string.atoi(featval[3:])
    if (featval!=nval):
        modifyone(eid,nameset[-1],featval,nval)

def modifyone(eid,fname,fval,nval):
    nvreg="[,|\[]"+fname+":("+fval+")[,|\]]"
    edge=findedge(eid)
    nvsrch=re.search(nvreg,edge.feat)
    if (nvsrch):
        globvar.chart[edge.id].feat=edge.feat[:nvsrch.span(1)[0]]+nval+edge.feat[nvsrch.span(1)[1]:]
    else:
        if (edge.feat==""):
            globvar.chart[edge.id].feat="["+fname+":"+nval+"]"
        else:
            globvar.chart[edge.id].feat=edge.feat[:-1]+","+fname+":"+nval+"]"

