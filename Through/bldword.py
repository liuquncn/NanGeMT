import re
import globvar
from basfunc import *
from agree import *
from parse import *
from transform import *

def bldword(trnsresult):
    for i in range(0,len(trnsresult)):
        recurassignfather(globvar.chart[trnsresult[i]])
        bldtree(globvar.chart[trnsresult[i]])
    print "----- bldresult -----"
    printsent(trnsresult)

def bldtree(edge):
    if (len(edge.son)!=0):
        transition(edge)
        for i in range(0,len(edge.son)):
            bldtree(globvar.chart[edge.son[i]])
    else:
        bldedge(edge)

def bldedge(edge):
    for i in range(0,len(globvar.bldrset)):
        form,cat,feat,nform=sepbldrule(globvar.bldrset[i])
        nword,res=bldmatch(form,cat,feat,nform,edge)
        if (res):
            globvar.prsrtable[globvar.chart[edge.id].prsid].cfg.part=[]
            globvar.prsrtable[globvar.chart[edge.id].prsid].cfg.part.append(nword)
            break

def sepbldrule(bldrule):
    bldreg='([*|\w]+) -- ([A-Z]) (\[.+\]) >> (.+)'
    bldsrch=re.search(bldreg,bldrule)
    assert(bldsrch)
    return(bldsrch.group(1),bldsrch.group(2),bldsrch.group(3),bldsrch.group(4))

def bldmatch(form,cat,feat,nform,edge):
    nword=""
    res=False
    root,matched=formmatch(form,globvar.prsrtable[edge.prsid].cfg.part[0])
    if (matched and cat==globvar.prsrtable[edge.prsid].cfg.reduct):
        nval,res=featagree(edge.feat,feat)
        if (res):
            nword=makenword(root,nform,edge)
            res=True
    return(nword,res)

def formmatch(form,word):
    root=""
    matched=False
    formreg='(\*)?(\w+)?'
    formsrch=re.search(formreg,form)
    assert(formsrch)
    if (formsrch.group(1) and formsrch.group(2)):
        rail=formsrch.group(2)
        if (word[-len(rail):0]==rail):
            root=word[:-len(rail)]
            matched=True
    elif (formsrch.group(1)):
        root=word
        matched=True
    elif (formsrch.group(2)):
        if (word==formsrch.group(2)):
            root=""
            matched=True
    return(root,matched)
    
def makenword(root,nform,edge):
    nword=""
    formreg='(\*)?(\w+)'
    formsrch=re.search(formreg,nform)
    if (formsrch):
        nword=root+formsrch.group(2)
    else:
        flexreg='{ (\w+) }'
        flexsrch=re.search(flexreg,nform)
        assert(flexsrch)
        fval,res=getfeatval(flexreg.group(1),edge.feat)
        nword=fval
        assert(res)
    return(nword)
