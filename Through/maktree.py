import re
import globvar
from dataset import *
from basfunc import *
from agree import *
from parse import *
from transform import *

def maktree(trnsresult):
    for i in range(0,len(trnsresult)):
        while (True):
            recurassignfather(globvar.chart[trnsresult[i]])
            res=maketree(globvar.chart[trnsresult[i]])
            if (not res):
                break
    print "----- makresult ------"
    printresult(trnsresult)
    printsent(trnsresult)

def maketree(edge):
    changed=False
    if (len(edge.son)!=0):
        transition(edge)
        for i in range(0,len(edge.son)):
            changed=maketree(globvar.chart[edge.son[i]]) or changed
    changed=makeedge(edge) or changed
    return(changed)

def makeedge(edge):
    changed=False
    for i in range(0,len(globvar.makrset)):
        prsid,ltree,lbind,rtree,rbind=sepmakrule(globvar.makrset[i])
        lnetable={}
        matched=treematch(lnetable,ltree,edge)
        if (matched):
            res=makagree(lnetable,lbind,edge)
            if (res):
                rnetable={}
                nedge=makenewtree(lnetable,rtree,rnetable)
                res=makagree(rnetable,rbind,nedge)
                if (res):
                    nedge.id=edge.id
                    globvar.chart[edge.id]=nedge
                    changed=True
    return(changed)

def sepmakrule(makrule):
    makreg='{[ ]*(\w+)[ ]*}([^%|^=]+\))(.+)?=>([^=]+\))(.+)?'    
    maksrch=re.search(makreg,makrule,re.S)
    assert(maksrch)
    prsid=maksrch.group(1)
    ltree=maksrch.group(2)
    if (maksrch.group(3)):
        lbind=advrmspace(maksrch.group(3))
    else:
        lbind=""
    rtree=maksrch.group(4)
    if (maksrch.group(5)):
        rbind=advrmspace(maksrch.group(5))
    else:
        rbind=""
    return(prsid,ltree,lbind,rtree,rbind)

def treematch(netable,tree,edge):
    red,sons,head=septree(tree)
    cat,word=sepnode(red)
    if ((not nodematch(cat,word,edge)) or (len(sons)!=0 and len(sons)!=len(edge.son))):
        return(False)
    else:
        addnetable(netable,cat,edge.id)
        for i in range(0,len(sons)):
            matched=treematch(netable,sons[i],globvar.chart[edge.son[i]])
            if (not matched):
                return(False)
        return(True)

def nodematch(cat,word,edge):
    if (word==""):
        return(cat==globvar.prsrtable[edge.prsid].cfg.reduct)
    else:
        return(cat==globvar.prsrtable[edge.prsid].cfg.reduct and word==globvar.prsrtable[edge.prsid].cfg.part[0])

def addnetable(netable,cat,eid):
    if (netable!={}):
        repeat=1
        nodename=""
        while (True):
            nodename="%"*repeat+cat
            if (not netable.get(nodename)):
                break
            else:
                repeat+=1
        netable[nodename]=eid
    else:
        netable[cat]=eid

def makagree(netable,bind,edge):
    bindset=getabindset(bind)
    for i in range(0,len(bindset)):
        if (not makaagree(netable,bindset[i],edge)):
            return(False)
    return(True)

def makaagree(netable,bind,edge):
    lfeat,opera,rfeat=getopeval(bind)
    lval,lvaltype,lvalset=makprocfeatval(netable,edge,lfeat)
    rval,rvaltype,rvalset=makprocfeatval(netable,edge,rfeat)
    if (len(lvaltype)!=0):
        nval,res=valagree(lval,rval,opera,lvaltype,lvalset)
        return(res)
    else:
        nval,res=valagree(lval,rval,opera,rvaltype,rvalset)
        return(res)
    
def makprocfeatval(netable,edge,fname):
    fval=""
    valtype=""
    valset=[]
    if (len(fname)!=0):
        if (fname[0]=='%'):
            dotpos=fname.find(".")
            if (dotpos!=-1):
                dese=globvar.chart[netable[fname[:dotpos]]]
                fval,valtype,valset=findfeatval(fname[dotpos+1:],dese)
            else:
                fval=fname
                valtype="Edge"
        elif (fname[0]=='$'):
            dotpos=fname.find(".")
            if (dotpos!=-1):
                fval,valtype,valset=findfeatval(fname[2:],edge)
            else:
                valtype="Edge"
                if (len(edge.feat)!=0):
                    fval=edge.feat
                else:
                    fval="[]"
        else:
            fval=fname
    return(fval,valtype,valset)

def makenewtree(lnetable,cfg,rnetable):
    red,sons,head=septree(cfg)
    cat,word=sepnode(red)
    addnetable(rnetable,cat,len(globvar.chart))
    newsonids=[]
    for i in range(0,len(sons)):
        sone=makenewtree(lnetable,sons[i],rnetable)
        newsonids.append(sone.id)
    edge=makenewatom(lnetable,red)
    edge.son=newsonids
    #edge.id=len(globvar.enchart)
    #globvar.enchart.append(edge)
    edge.id=len(globvar.chart)
    globvar.chart.append(edge)
    return(edge)

def makenewatom(lnetable,dualred):
    edge=edgec()
    newred,oldred=getoriname(dualred)
    if (len(oldred)!=0):
        edge=copy.deepcopy(globvar.chart[lnetable[oldred]])
    else:
        cat,word=sepnode(newred)
        if (word==""):
            edge=copy.deepcopy(globvar.chart[lnetable[cat]])
        else:
            edge.source='m'
            edge.father=-2
            globvar.prsrtable[edge.prsid].cfg.reduct=cat
            globvar.prsrtable[edge.prsid].cfg.part.append(word)
    return(edge)

def getoriname(corrname):
    oldname=""
    newname=""
    corrreg="([^/]+)(/+)(.+)"
    corrsrch=re.search(corrreg,corrname)
    if (corrsrch):
        newname=corrsrch.group(1)
        oldname="%"*len(corrsrch.group(2))+corrsrch.group(3)
    else:
        newname=corrname
        oldname=""
    return(newname,oldname)

def septree(tree):
    treereg='([^\(]+)(\((.+)\))?'
    treesrch=re.search(treereg,tree)
    red=removespace(treesrch.group(1))
    sons=[]
    head=-1
    if (treesrch.group(3)):
        sons,head=getsons(treesrch.group(3))
    return(red,sons,head)

def getsons(sonseq):
    sons=[]
    head=-1
    n=0
    temp=sonseq
    while (len(temp)!=0):
        son,temp=getoneunit(temp)
        if (son.find("!")==0):
            son=son[1:]
            head=n
        sons.append(son)
        n+=1
    return(sons,head)

def sepnode(node):
    cwreg='([^<]+)(<([^<|^>]+)>)?'
    cwsrch=re.search(cwreg,node)
    assert(cwsrch)
    cat=cwsrch.group(1)
    if (cwsrch.group(3)):
        word=cwsrch.group(3)
    else:
        word=""
    return(cat,word)

