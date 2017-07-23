import globvar
from dataset import *
from basfunc import *

def recurprocyx(edge):
    yxval=""
    desce=findfinaldesc(edge)
    if (desce.prsid!=None and len(desce.son)==0 and len(globvar.prsrtable[desce.prsid].cfg.part)!=0):
        yxval=globvar.prsrtable[desce.prsid].cfg.part[0]
    return(yxval)

def findfinaldesc(edge):
    desce=edgec()
    if (len(edge.son)==0):
        desce=edge
    else:
        if (0<=globvar.prsrtable[edge.prsid].head<len(edge.son)):
            hsone=findedge(edge.son[globvar.prsrtable[edge.prsid].head])
            desce=findfinaldesc(hsone)
    return(desce)

def procyinjie(edge):
    yinjie=0
    if (edge.prsid!=None and globvar.prsrtable[edge.prsid].cfg.reduct!=""):
        yxstr=recurprocyx(edge)
        yinjie=len(yxstr)/len(globvar.shi)
    yinjieval=str(yinjie)
    return(yinjieval)

def procchongdie(edge):
    featval=globvar.fou
    if (len(edge.son)==2):
        firword=globvar.chart[edge.son[0]]
        secword=globvar.chart[edge.son[1]]
        if (firword.cfg.part[0]==secword.cfg.part[0]):
            if (firword.cfg.reduct=="a"):
                featval="AA"
            elif (firword.cfg.part.front()=="d"):
                featval="DD"
            elif (firword.cfg.part.front()=="v"):
                featval="VV"
        elif ((firword.cfg.reduct=="r") and (secword.cfg.part.front()==liang)):
            featval=Rliang 
        elif ((firword.cfg.reduct=="n") and (secword.cfg.part.front()==er)):
            featval=Ner 
    elif (len(edge.son)==3):
        firword=globvar.chart[edge.son[0]]
        secword=globvar.chart[edge.son[1]]
        thiword=globvar.chart[edge.son[2]]
        if ((firword.cfg.reduct=="m") and (secword.cfg.reduct=="n") and (thiword.cfg.part.front()==er)):
            featval=NMer 
        elif ((firword.cfg.reduct=="v") and (thiword.cfg.reduct=="v") and (secword.cfg.part.front()==le)):
            featval=VleV 
        elif ((firword.cfg.reduct=="v") and (thiword.cfg.reduct=="v") and (secword.cfg.part.front()==yi)):
            featval=VyiV 
        elif ((firword.cfg.reduct=="v") and (thiword.cfg.reduct=="n") and (secword.cfg.part.front()==le)):
            featval=VleN 
        elif ((firword.cfg.reduct=="v") and (secword.cfg.reduct=="v") and (thiword.cfg.reduct=="n")):
            featval="VVN"
        elif ((firword.cfg.reduct=="v") and (thiword.cfg.reduct=="n") and (secword.cfg.part.front()==guo)):
            featval=VguoN 
    elif (len(edge.son)==4):
        firword=globvar.chart[edge.son[0]]
        secword=globvar.chart[edge.son[1]]
        thiword=globvar.chart[edge.son[2]]
        fouword=globvar.chart[edge.son[3]]
        if ((firword.cfg.reduct=="a") and 
            (secword.cfg.reduct=="a") and 
            (thiword.cfg.reduct=="a") and 
            (fouword.cfg.reduct=="a") and
            (firword.cfg.part.front()==secword.cfg.part.front()) and
            (thiword.cfg.part.front()==fouword.cfg.part.front())):
            featval="AABB"
        elif ((firword.cfg.reduct=="a") and 
              (secword.cfg.reduct=="a") and 
              (thiword.cfg.reduct=="a") and 
              (fouword.cfg.reduct=="a") and
              (firword.cfg.part.front()==thiword.cfg.part.front()) and
              (secword.cfg.part.front()==fouword.cfg.part.front())): 
            featval="ABAB"
        elif ((firword.cfg.reduct=="u") and 
	     (thiword.cfg.reduct=="u") and
	     (secword.cfg.reduct=="v") and 
	     (fouword.cfg.reduct=="v") and
	     (firword.cfg.part.front()==thiword.cfg.part.front()) and
	     (secword.cfg.part.front()==fouword.cfg.part.front())):
            featval="UVUV"
        elif ((firword.cfg.reduct=="v") and
              (secword.cfg.part.front()==le) and
              (thiword.cfg.part.front()==yi) and
              (fouword.cfg.reduct=="n")):
            featval=VleyiN 
        elif ((firword.cfg.reduct=="v") and
              (secword.cfg.part.front()==guo) and
              (thiword.cfg.part.front()==yi) and
              (fouword.cfg.reduct=="n")):
            featval=VguoyiN 
        elif ((firword.cfg.reduct=="v") and
              (secword.cfg.part.front()==bu) and
              (thiword.cfg.part.front()==le) and
              (fouword.cfg.reduct=="n")):
            featval=VbuleN 
    return(featval)
