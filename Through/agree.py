import re
import string
import globvar
from basfunc import *
from chongdie import *

def agree(bind,edge):
    nbind=[]
    res=True
    bindset=getbindset(bind)
    for i in range(0,len(bindset)):
        btype,ifbind,thbind,elbind=bindtype(bindset[i])
        if (btype=="basic"):
            anbind,res=basagree(bindset[i],edge)
        elif (btype=="iftrue"):
            anbind,res=basagree(ifbind,edge)
        elif (btype=="iffalse"):
            anbind,res=iffalseagree(ifbind,edge)
        elif (btype=="ifthen"):
            anbind,res=ifthenagree(ifbind,thbind,edge)
        elif (btype=="ifelse"):
            anbind,res=ifelseagree(ifbind,elbind,edge)
        elif (btype=="iffull"):
            anbind,res=iffullagree(ifbind,thbind,elbind,edge)
        else:
            assert(False)
        if (res==False):
            break
        else:
            nbind+=anbind
    return(nbind,res)

def getbindset(bind):
    ifreg=re.compile('(IF )(.+?)( ENDIF|TRUE|FALSE)(,?)',re.S)
    bindset=ifreg.findall(bind)
    remain=ifreg.sub("",bind)
    if (remain!=""):
        bindset.append(remain)
    return(bindset)

def iffalseagree(bind,edge):
    nbind,res=basagree(bind,edge)
    res=not(res)
    return([],res)

def ifthenagree(ifbind,thbind,edge):
    nbind=[]
    res=True
    nifbind,ifres=basagree(ifbind,edge)
    nthbind,thres=basagree(thbind,edge)
    if (ifres==False):
        res=True
    elif (ifres==True and thres==False):
        res=False
        nbind=notbind(nthbind)
    elif (ifres==True and thres==True):
        res=True
        nbind=nthbind
    return(nbind,res)

def ifelseagree(ifbind,elbind,edge):
    nbind=[]
    res=True
    nifbind,ifres=basagree(ifbind,edge)
    nelbind,elres=basagree(elbind,edge)
    if (ifres==True):
        res=True
    elif (ifres==False and elres==True):
        res=True
        nbind=nelbind
    elif (ifres==False and elres==False):
        res=False
        nbind=notbind(nelbind)
    return(nbind,res)

def iffullagree(ifbind,thbind,elbind,edge):
    nbind=[]
    res=True
    nifbind,ifres=basagree(ifbind,edge)
    nthbind,thres=basagree(thbind,edge)
    nelbind,elres=basagree(elbind,edge)
    if (ifres==True and thres==True):
        res=True
        nbind=nthbind
    elif (ifres==True and thres==False):
        res=False
        nbind=notbind(nthbind)
    elif (ifres==False and elres==True):
        res=True
        nbind=nelbind
    elif (ifres==False and elres==False):
        res=False
        nbind=notbind(nelbind)
    return(nbind,res)

def bindtype(bind):
    btype=""
    ifbind=""
    thbind=""
    elbind=""
    if (str(type(bind))=="<type 'str'>"):
        btype="basic"
    elif (bind[2].find("TRUE")!=-1):
        btype="iftrue"
        ifbind=bind[1]
    elif (bind[2].find("FALSE")!=-1):
        btype="iffalse"
        ifbind=bind[1]
    elif (bind[1].find("THEN")!=-1 and bind[1].find("ELSE")==-1):
        btype="ifthen"
        thenreg='(.+)( THEN )(.+)'
        thensrch=re.match(thenreg,bind[1])
        ifbind=thensrch.group(1)
        thbind=thensrch.group(3)
    elif (bind[1].find("THEN")==-1 and bind[1].find("ELSE")!=-1):
        btype="ifelse"
        elsereg='(.+)( ELSE )(.+)'
        elsesrch=re.match(elsereg,bind[1])
        ifbind=elsesrch.group(1)
        elbind=elsesrch.group(3)
    elif (bind[1].find("THEN")!=-1 and bind[1].find("ELSE")!=-1):
        btype="iffull"
        fullreg='(.+)( THEN )(.+)( ELSE )(.+)'
        fullsrch=re.match(fullreg,bind[1])
        ifbind=fullsrch.group(1)
        thbind=fullsrch.group(3)
        elbind=fullsrch.group(5)
    else:
        print bind
        assert(False)
    return(btype,ifbind,thbind,elbind)

def basagree(bind,edge):
    nbind=[]
    res=True
    bindset=getabindset(bind)
    for i in range(0,len(bindset)):
        if (bindset[i]!=""):
            anbind,res=aagree(bindset[i],edge)
            if (res==False):
                break
            else:
                nbind+=anbind
    return(nbind,res)

def aagree(bind,edge):
    nbind=[]
    res=True
    if (bind.find("@")!=-1):
        nbind,res=atagree(bind,edge)
    else:
        lfeat,opera,rfeat=getopeval(bind)
        rval,rvaltype,rvalset=procfeatval(rfeat,edge)
        if (lfeat.find("$")==0 and edge.feat==""):
            nbind.append(lfeat+opera+rval)
            res=True
        else:
            lval,lvaltype,lvalset=procfeatval(lfeat,edge)
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

def atagree(bind,edge):
    nbind=[]
    res=True
    atset=bind.split("@")
    atset[0]=removespace(atset[0])
    lfeat,opera,rfeat=getopeval(atset[0])
    for i in range(1,len(atset)):
        temp=removespace(atset[i])
        atbind=lfeat+"."+temp+opera+rfeat+"."+temp
        natbind,res=aagree(atbind,edge)
        if (res==False):
            break;
        else:
            nbind+=natbind
    return(nbind,res)

def getopeval(bind):
    opereg='(.+?)(=+|!=)(.*)'
    opesrch=re.search(opereg,bind)
    if (not opesrch):
        print bind
    assert(opesrch)
    return(opesrch.group(1),opesrch.group(2),opesrch.group(3))

def procfeatval(featname,edge):
    featval=""
    feattype=""
    featvalset=[]
    if (featname.find("%")==0):
        dotreg='(.+?)(\.)(.+)'
        dotsrch=re.search(dotreg,featname)
        if (dotsrch):
            sone=findsonedge(dotsrch.group(1),edge)
            fname=dotsrch.group(3)
            featval,feattype,featvalset=findfeatval(fname,sone)
        else:
            sonid=findsonid(featname,edge)
            featval="eid"+str(sonid)
    elif (featname.find("$")==0):
        dotreg='(.+?)(\.)(.+)'
        dotsrch=re.search(dotreg,featname)
        if (dotsrch):
            fname=dotsrch.group(3)
            featval,feattype,featvalset=findfeatval(fname,edge)
        else:
            featval="eid"+str(edge.id)
    else:
        featval=featname
    return(featval,feattype,featvalset)

def findfeatval(featname,edge):
    featval=""
    featype=""
    featvalset=[]
    nameset=featname.split(".")
    curredge=edge
    for i in range(0,len(nameset)):
        featval,featype,featvalset=basfeatval(nameset[i],curredge)
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

def basfeatval(featname,edge):
    featval=""
    feattype=""
    featvalset=[]
    if (featname.find("eid")!=-1):
        featval=featname
        feattype="Edge"
    elif (featname==globvar.hanzi):
        if (edge.prsid!=None and len(globvar.prsrtable[edge.prsid].cfg.part)!=0):
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
        if (edge.prsid==None):
            featval=""
        else:
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
        if (not res):
            featval=defval
    return(featval,feattype,featvalset)

def findcat(edge):
    cat=""
    if (edge.prsid>=0):
        curedge=edge
        syncat=globvar.prsrtable[edge.prsid].cfg.reduct
        while (not (ischnpos(syncat) or isengpos(syncat))):
            assert(globvar.prsrtable[curedge.prsid].head<len(curedge.son))
            hsonedge=findedge(curedge.son[globvar.prsrtable[curedge.prsid].head])
            syncat=globvar.prsrtable[hsonedge.prsid].cfg.reduct
            curedge=hsonedge
        assert(ischnpos(syncat) or isengpos(syncat))
        cat=syncat
    return(cat)

def getfeatval(featname,feat):
    featval=""
    res=False
    fnamereg="(,|\[|{)("+featname+":)"
    fnamesrch=re.search(fnamereg,feat)
    if (fnamesrch):
        temp=feat[fnamesrch.span()[1]:]
        rem,featval=getabindunit(temp)
        res=True
    return(featval,res)

def moregetfeatval(featname,edge):
    curredge=edge
    featval,res=getfeatval(featname,curredge.feat)
    while (not res):
        if (curredge.prsid!=None and 0<=globvar.prsrtable[curredge.prsid].head<len(curredge.son)):
            hsone=findedge(curredge.son[globvar.prsrtable[curredge.prsid].head])
            featval,res=getfeatval(featname,hsone.feat)
            curredge=hsone
        else:
            break
    return(featval,res)

def checkmodel(featname):
    defval=""
    valtype=""
    valset=[]
    featname=formalname(featname)
    if (featname=="notexist"):
        valtype="Symbol Unlimited"
    else:
        modelreg="(\("+featname+"\)\s*:\s*)"+"([^D^{^\n]+)"
        modelsrch=re.search(modelreg,globvar.model)
        if (not modelsrch):
            print modelreg,"|"+featname+"|"
        assert(modelsrch)
        valtype=modelsrch.group(2)
        retpos=globvar.model.find(":",modelsrch.span()[1])
        entry=globvar.model[modelsrch.span()[1]:retpos]
        setreg="{.+?}"
        setsrch=re.search(setreg,entry,re.S)
        if (setsrch):
            temp=setsrch.group(0)[1:-1]
            temp=re.sub('\s',"",temp)
            valset=temp.split(",")
        defreg="(Default=)(\S+)"
        defsrch=re.search(defreg,entry)
        if (defsrch):
            defval=defsrch.group(2)
    return(defval,valtype,valset)

def formalname(featname):
    if (globvar.model.find("("+featname+")")==-1):
        aliasreg="(\w+)(="+featname+")"
        aliasrch=re.search(aliasreg,globvar.model)
        if (aliasrch):
            featname=aliasrch.group(1)
        else:
            featname="notexist"
    return(featname)

def valagree(lval,rval,opera,valtype,modelset):
    nval=""
    res=True
    if (valtype=="Edge"):
        if (lval.find("[")==0):
            if (rval.find("[")==0):
                nval,res=featagree(lval,rval)
            elif (rval.find("eid")==0):
                edge=findedge(string.atoi(rval[3:]))
                nval,res=featagree(lval,edge.feat)
            elif (rval.find("~[")==0):
                nval,res=featagree(lval,rval[1:])
                res=not(res)
                nval=lval
            else:
                rval=procshifou(rval)
                res=(rval==globvar.fou)
                nval=lval
        elif (lval.find("eid")==0):
            if (rval.find("[")==0):
                edge=findedge(string.atoi(lval[3:]))
                nval,res=featagree(edge.feat,rval)
            elif (rval.find("eid")==0):
                res=(lval==rval)
                nval=lval
            elif (rval.find("~[")==0):
                edge=findedge(string.atoi(lval[3:]))
                nval,res=(edge.feat,rval[1:])
                res=not(res)
                nval=lval
            else:
                rval=procshifou(rval)
                res=(rval==globvar.fou)
                nval=lval
        elif (lval.find("~[")==0):
            if (rval.find("[")==0):
                nval,res=featagree(lval[1:],rval)
                res=not(res)
                nval=lval
            elif (rval.find("eid")==0):
                edge=findedge(string.atoi(rval[3:]))
                nval,res=featagree(lval[1:],edge.feat)
                res=not(res)
                nval=lval
            elif (rval.find("~[")==0):
                res=True
                nval=lval
            else:
                rval=procshifou(rval)
                res=(rval==globvar.fou)
                nval=lval
        else:
            lval=procshifou(lval)
            if (lval==globvar.shi):
                if (rval==""):
                    res=False
                    nval=lval
                else:
                    res=True
                    nval=rval
            elif (lval==globvar.fou):
                if (rval==""):
                    res=True
                    nval=lval
                else:
                    res=False
                    nval=lval
            elif (lval==""):
                rval=procshifou(rval)
                if (rval==globvar.fou):
                    res=True
                    nval=rval
                else:
                    res=False
                    nval=rval
            #else:
            #assert(False)
    else:
        lorset=lval.split("|")
        rorset=rval.split("|")
        nval,res=orsetagree(lorset,rorset,opera,valtype,modelset)
    return(nval,res)

def featagree(lval,rval):
    nval=""
    edge=edgec()
    edge.feat=lval
    bind=feattobind(rval)
    nbind,res=basagree(bind,edge)
    for i in range(0,len(nbind)):
        nval+=nbind[i]+","
    nval=nval[:-1]
    return(nval,res)

def orsetagree(lorset,rorset,opera,valtype,modelset):
    nval=""
    res=False
    for i in range(0,len(lorset)):
        for j in range(0,len(rorset)):
            naval,ares=opagree(lorset[i],rorset[j],opera,valtype,modelset)
            if (ares):
                nval=naval+"|"
    if (nval==""):
        res=False
    else:
        nval=nval[:-1]
        res=True
    return(nval,res)

def opagree(lval,rval,opera,valtype,modelset):
    res=eqagree(lval,rval,valtype,modelset)
    if (opera=="!="):
        res=not(res)
    if (res):
        if (lval!=""):
            nval=lval
        else:
            nval=rval
    else:
        nval=""
    return(nval,res)

def eqagree(lval,rval,valtype,modelset):
    res=True
    landset=getandset(lval) 
    randset=getandset(rval) 
    for i in range(0,len(landset)):
        for j in range(0,len(randset)):
            res=aeqagree(landset[i],randset[j],valtype,modelset)
            if (res==False):
                break
    return(res)

def aeqagree(lval,rval,valtype,modelset):
    if (lval.find("~")!=0 and rval.find("~")!=0):
        res=pureagree(lval,rval,valtype,modelset)
    elif (lval.find("~")!=0 and rval.find("~")==0):
        res=pureagree(lval,rval[1:],valtype,modelset)
        res=not(res)
    elif (lval.find("~")==0 and rval.find("~")!=0):
        res=pureagree(lval[1:],rval,valtype,modelset)
        res=not(res)
    else:
        res=True
    return(res)
        
def pureagree(lval,rval,valtype,modelset):
    if (valtype.find("Boolean")!=-1):
        if (len(modelset)!=0):
            lval=findval(lval,modelset)
            rval=findval(rval,modelset)
        else:
            lval=procshifou(lval)
            rval=procshifou(rval)
        res=(lval==rval)
    elif (valtype.find("Number")!=-1):
        res=(lval==rval)
    elif (valtype.find("Symbol")!=-1):
        if (len(modelset)!=0):
            lval=findval(lval,modelset)
            rval=findval(rval,modelset)
        res=(lval==rval)
    elif (valtype.find("Hierar")!=-1):
        lcode=gethiercode(lval)
        rcode=gethiercode(rval)
        res=(lcode.find(rcode)==0)
    elif (valtype.find("Edge")!=-1):
        rval=procshifou(rval)
        if ((lval==rval) or
            (lval=="" and rval==globvar.fou) or
            (lval=="" and rval.find("eid")==0)):
            res=True
        else:
            res=False
    else:
        print valtype
        assert(False)
    return(res)

def findval(val,modelset):
    val1=procshifou(val)
    for i in range(0,len(modelset)):
        temp=removespace(modelset[i])
        aliaset=temp.split("=")
        for j in range(0,len(aliaset)):
            val2=procshifou(aliaset[j])
            if (val1==val2):
                return(aliaset[0])
    assert(False)

def feattobind(feat):
    bind=""
    if (feat!="" and feat!="[]"):
        featreg='(\[)(.+)(\])'
        featsrch=re.search(featreg,feat)
        assert(featsrch)
        featset=featsrch.group(2).split(",")
        for i in range(0,len(featset)):
            bind+="$."+featset[i].replace(":","=")+","
        bind=bind[:-1]
    return(bind)

def bindtofeat(nbind):
    feat=""
    for i in range(0,len(nbind)):
        fname,opera,fval=getopeval(nbind[i])
        if (fname=="$" and opera=="="):
            return(fval)
        if (fval!=""):
            if (fname.find("$.")==0):
                fname=fname[2:]
            if (opera=="!="):
                if (fval[0]=='~'):
                    fval=fval[1:]
                else:
                    fval="~"+fval
            feat+=fname+":"+fval+","
    feat="["+feat[:-1]+"]"
    return(feat)

def getincedgekey(prsid,dotreg,sentreg,sons):
    key=prsid+":"+str(dotreg.begin)+"-"+str(dotreg.end)+":"+str(sentreg.begin)+"-"+str(sentreg.end)+":"
    for i in range(0,len(sons)):
        key+=str(sons[i])+","
    return(key)

def setsilence(edge):
    for i in range(0,len(globvar.chart)):
        if (globvar.chart[i].sentreg.end>edge.sentreg.begin and globvar.chart[i].sentreg.begin<edge.sentreg.end and globvar.chart[i].source!='l'):
            globvar.chart[i].state='s'

def prsagree(bind,edge):
    nbind,res=agree(bind,edge)
    return(nbind,res)

def swiagree(bind,edge):
    nbind,res=agree(bind,edge)
    return(nbind,res)

def addtoagenda(edge):
    if (complete(edge)):
        nbind,res=prsagree(globvar.prsrtable[edge.prsid].bind,edge)
        if (res):
            edge.feat=bindtofeat(nbind)
            compedgekey=globvar.prsrtable[edge.prsid].cfg.reduct+str(edge.sentreg.begin)+str(edge.sentreg.end)+edge.feat
            if (compedgekey not in globvar.compedgemap):
                edge.id=len(globvar.chart)
                edge.father=-2
                edge.state='a'
                globvar.chart.append(edge)
                globvar.compedgemap[compedgekey]=edge.id
                if (edge.source=='l'):
                    setsilence(edge)
    else:
        incedgekey=getincedgekey(globvar.prsrtable[edge.prsid].prsid,edge.dotreg,edge.sentreg,edge.son)
        if (incedgekey not in globvar.incedgemap):
            edge.id=len(globvar.chart)
            edge.father=-2
            edge.state='a'
            globvar.chart.append(edge)
            globvar.incedgemap[incedgekey]=edge.id
