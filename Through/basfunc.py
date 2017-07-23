import re
import globvar

def removespace(val):
    nval,number=re.subn('^[\s]+|[\s]+$',"",val)
    return(nval)

def advrmspace(val):
    nval=re.sub('[\t\n]',"",val)
    nval=re.sub('[ ]+'," ",nval)
    nval=removespace(nval)
    return(nval)

def getoneunit(bigunit):
    bigunit=removespace(bigunit)
    mpos=0
    branum=0
    angnum=0
    while (mpos<len(bigunit)):
        if (bigunit[mpos]==' ' and branum==0 and angnum==0):
            break
        if (bigunit[mpos]=='('):
            branum+=1
        elif (bigunit[mpos]==')'):
            branum-=1
        elif (bigunit[mpos]=='<'):
            angnum+=1
        elif (bigunit[mpos]=='>'):
            angnum-=1
        mpos+=1
    subunit=bigunit[:mpos]
    while (mpos<len(bigunit) and bigunit[mpos]==' '):
        mpos+=1
    rem=bigunit[mpos:]
    return(subunit,rem)

def getabindunit(bind):
    mpos=0
    braknum=0
    while (mpos<len(bind)):
        if ((bind[mpos]==',' or bind[mpos]==']' or bind[mpos]=='}') and braknum==0):
            abind=removespace(bind[:mpos])
            rem=removespace(bind[mpos+1:])
            return(rem,abind)
        elif (bind[mpos]=='['):
            braknum+=1
        elif (bind[mpos]==']'):
            braknum-=1
        mpos+=1
    bind=removespace(bind)
    return("",bind)
    
def getandset(val):
    andset=val.split("~")
    for i in range(1,len(andset)):
        andset[i]="~"+andset[i]
    if (andset[0]==""):
        del andset[0]
    return(andset)

def notbind(binds):
    nbinds=[]
    for i in range(0,len(binds)):
        bindreg='(.+)(=|!=)(.+)'
        bindsrch=re.search(bindreg,binds[i])
        assert(bindsrch)
        lfeat=bindsrch.group(1)
        opera=bindsrch.group(2)
        rfeat=bindsrch.group(3)
        if (opera=="="):
            if (rfeat.find("~")!=0):
                bind=lfeat+"!="+rfeat
            else:
                bind=lfeat+"="+rfeat[1:]
        else:
            bind=lfeat+"="+rfeat
        nbinds.append(bind)
    return(nbinds)

def gethiercode(valname):
    if (valname!="" and (not valname[0].isalpha())):
        codereg="(\w+)(="+valname+",)"
        codesrch=re.search(codereg,globvar.model)
        if (not codesrch):
            pass
        assert(codesrch)
        valcode=codesrch.group(1)
    else:
        valcode=valname
    return(valcode)

def procshifou(val):
    if (isshi(val)):
        val=globvar.shi
    elif (isfou(val)):
        val=globvar.fou
    return(val)

def isshi(val):
    return(val=="YES" or val=="Yes" or val=="yes" or val==globvar.shi)

def isfou(val):
    return(val=="NO" or val=="No" or val=="no" or val=="NONE" or val==globvar.fou)

def ischnpos(syncat):
    ccatreg="(ccat=.+?:.+?)({[\w|\s|,]+})"
    ccatsrch=re.search(ccatreg,globvar.model,re.S)
    assert(ccatsrch)
    return(ccatsrch.group(2).find(syncat)!=-1)

def isengpos(syncat):
    ecatreg="(ECAT=.+?:.+?)({[\w|\s|,]+})"
    ecatsrch=re.search(ecatreg,globvar.model,re.S)
    assert(ecatsrch)
    return(ecatsrch.group(2).find(syncat)!=-1)

def findsonedge(nota,edge):
    sonid=findsonid(nota,edge)
    sone=findedge(sonid)
    return(sone)

def findedge(eid):
    return(globvar.chart[eid])

def findsonid(nota,edge):
    notareg='(%+)(\w+)'
    notasrch=re.search(notareg,nota)
    assert(notasrch)
    repeat=len(notasrch.group(1))
    cat=notasrch.group(2)
    repeat,sonid=recurfindsonid(cat,repeat,edge)
    assert(repeat==0 and sonid>=0)
    return(sonid)

def recurfindsonid(cat,repeat,edge):
    sonid=-1
    if (repeat>0):
        for i in range(0,len(edge.son)):
            if (globvar.prsrtable[edge.prsid].cfg.part[i]==cat):
                repeat-=1
                if (repeat==0):
                    sonid=edge.son[i]
                    break
            if (globvar.entochidx.get(edge.son[i]) and globvar.entochidx[edge.son[i]]==-1):
                sone=findedge(edge.son[i])
                repeat,sonid=recurfindsonid(cat,repeat,sone)
                if (repeat==0):
                    break
    return(repeat,sonid)

def complete(edge):
    return(edge.dotreg.end-edge.dotreg.begin==len(globvar.prsrtable[edge.prsid].cfg.part))

def recurassignfather(edge):
    for i in range(0,len(edge.son)):
        globvar.chart[edge.son[i]].father=edge.id
        recurassignfather(globvar.chart[edge.son[i]])

def resetglobvar():
    globvar.initpos=0
    globvar.chnsent=""
    globvar.chart=[]
    globvar.compbeginmap={}
    globvar.compendmap={}
    globvar.incompbeginmap={}
    globvar.incompendmap={}
    globvar.compedgemap={}
    globvar.incedgemap={}
    globvar.compsentregmap={}
    globvar.incompsentregmap={}
    #globvar.enchart=[]
    globvar.chtoenidx={}
    globvar.entochidx={}
    globvar.trnsresult=[]
    globvar.chtoenidx[-1]=[]
    del globvar.prsrtable[len(globvar.prsrset):]

def printchart(bpos):
    print "------printchart new edge--------"
    for i in range(bpos,len(globvar.chart)):
        print " "*3,
        printedge(globvar.chart[i])
    print "-------end of printchart---------"

def printedge(edge):
    print str(edge.id)+"("+edge.source+":"+edge.state+";"+edge.ruleid+";"+str(edge.father)+":"+str(globvar.prsrtable[edge.prsid].head)+")"+"sentreg("+str(edge.sentreg.begin)+"-"+str(edge.sentreg.end)+") "+"dotreg("+str(edge.dotreg.begin)+"-"+str(edge.dotreg.end)+") "+globvar.prsrtable[edge.prsid].cfg.reduct+"->",
    for i in range(len(globvar.prsrtable[edge.prsid].cfg.part)):
        print globvar.prsrtable[edge.prsid].cfg.part[i],
    print "->",
    for i in range(0,len(edge.son)):
        print edge.son[i],
    if (complete(edge)):
        print edge.feat
    else:
        print

def printedges(edges):
    for i in range(0,len(edges)):
        print " "*3,
        printedge(edges[i])

def printsent(trnsresult):
    for i in range(0,len(trnsresult)):
        printsection(trnsresult[i])
    print
        
def printsection(eid):
    edge=findedge(eid)
    if (len(edge.son)!=0):
        for i in range(0,len(edge.son)):
            printsection(edge.son[i])
    elif (len(globvar.prsrtable[edge.prsid].cfg.part)!=0):
        print globvar.prsrtable[edge.prsid].cfg.part[0],

def printresult(parseresult):
    for i in range(0,len(parseresult)):
        recurprinttree(parseresult[i])
        print
        recurshowtree(parseresult[i],0,0)
        recurshowtree(parseresult[i],0,1)

def recurprinttree(eid):
    edge=findedge(eid)
    print "("+edge.ruleid+") ",
    print globvar.prsrtable[edge.prsid].cfg.reduct,"[",
    if (len(edge.son)!=0):
        for i in range(0,len(edge.son)):
            recurprinttree(edge.son[i])
    elif (len(globvar.prsrtable[edge.prsid].cfg.part)!=0):
        print globvar.prsrtable[edge.prsid].cfg.part[0],
    print "]",

def recurshowtree(eid,spanum,flag):
    edge=findedge(eid)
    if (spanum>=4):
        print ' '*(spanum-4)+'+'+'-'*2,
    if (flag==0):
        print globvar.prsrtable[edge.prsid].cfg.reduct +" "+ str(edge.id)+":"+str(edge.father)+" ("+edge.ruleid+")"
    else:
        print globvar.prsrtable[edge.prsid].cfg.reduct +" "+ str(edge.id)+":"+str(edge.father)+" ("+edge.ruleid+")"+edge.feat
    if (len(edge.son)!=0):
        for i in range(0,len(edge.son)):
            recurshowtree(edge.son[i],spanum+4,flag)
    elif (len(globvar.prsrtable[edge.prsid].cfg.part)!=0):
        print ' '*spanum+'+'+'-'*3+globvar.prsrtable[edge.prsid].cfg.part[0]

def getabindset(bind):
    bindset=[]
    remain=removespace(bind)
    while (remain!=""):
        remain,abind=getabindunit(remain)
        if (abind!=""):
            bindset.append(abind)
    return(bindset)

