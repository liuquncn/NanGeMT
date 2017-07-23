CFLAG=-c -ggdb -Wall -D UNIX -D KBMS

cemt : main/cemt.o basic/error.o kbase/kbase.o trans/trans.o kbase/model.o kbase/mkbase.o kbase/prsrbase.o kbase/dictn.o kbase/makrbase.o trans/struct.o kbase/cutrbase.o kbase/bldrbase.o kbase_ce/chncutrbase.o kbase_ce/engbldrbase.o kbase_ce/engmakrbase.o kbase_ce/lexdbase.o trans_ce/cetrans.o kbase/template.o kbase_ce/chndictn.o basic/token.o translib/translib.o trans/unify.o basic/agree.o basic/function.o trans/result.o 
	g++ -o cemt main/cemt.o basic/error.o kbase/kbase.o trans/trans.o kbase/model.o kbase/mkbase.o kbase/prsrbase.o kbase/dictn.o kbase/makrbase.o trans/struct.o kbase/cutrbase.o kbase/bldrbase.o kbase_ce/chncutrbase.o kbase_ce/engbldrbase.o kbase_ce/engmakrbase.o kbase_ce/lexdbase.o trans_ce/cetrans.o kbase/template.o kbase_ce/chndictn.o basic/token.o translib/translib.o trans/unify.o basic/agree.o basic/function.o trans/result.o 

kbms : main/kbms.o kbase/kbase.o kbase/model.o kbase/mkbase.o kbase/prsrbase.o kbase/dictn.o kbase/makrbase.o kbase/cutrbase.o kbase/bldrbase.o kbase_ce/chncutrbase.o kbase_ce/engbldrbase.o kbase_ce/engmakrbase.o kbase_ce/lexdbase.o kbase/template.o kbase_ce/chndictn.o basic/token.o translib/translib.o trans/unify.o basic/agree.o basic/error.o basic/function.o
	g++ -o kbms main/kbms.o basic/error.o kbase/kbase.o trans/trans.o kbase/model.o kbase/mkbase.o kbase/prsrbase.o kbase/dictn.o kbase/makrbase.o trans/struct.o kbase/cutrbase.o kbase/bldrbase.o kbase_ce/chncutrbase.o kbase_ce/engbldrbase.o kbase_ce/engmakrbase.o kbase_ce/lexdbase.o trans_ce/cetrans.o kbase/template.o kbase_ce/chndictn.o basic/token.o translib/translib.o trans/unify.o basic/agree.o basic/function.o trans/result.o

basic/agree.o : basic/agree.cpp
	cd basic; gcc $(CFLAG) agree.cpp; cd ..

kbase/bldrbase.o : kbase/bldrbase.cpp
	cd kbase; gcc $(CFLAG) bldrbase.cpp; cd ..

trans_ce/cetrans.o : trans_ce/cetrans.cpp
	cd trans_ce; gcc $(CFLAG) cetrans.cpp; cd ..

kbase_ce/chncutrbase.o : kbase_ce/chncutrbase.cpp
	cd kbase_ce; gcc $(CFLAG) chncutrbase.cpp; cd ..

kbase_ce/chndictn.o : kbase_ce/chndictn.cpp
	cd kbase_ce; gcc $(CFLAG) chndictn.cpp; cd ..

kbase/cutrbase.o : kbase/cutrbase.cpp
	cd kbase; gcc $(CFLAG) cutrbase.cpp; cd ..

kbase/dictn.o : kbase/dictn.cpp
	cd kbase; gcc $(CFLAG) dictn.cpp; cd ..

kbase_ce/engbldrbase.o : kbase_ce/engbldrbase.cpp
	cd kbase_ce; gcc $(CFLAG) engbldrbase.cpp; cd ..

kbase_ce/engmakrbase.o : kbase_ce/engmakrbase.cpp
	cd kbase_ce; gcc $(CFLAG) engmakrbase.cpp; cd ..

basic/error.o : basic/error.cpp
	cd basic; gcc $(CFLAG) error.cpp; cd ..

basic/function.o : basic/function.cpp
	cd basic; gcc $(CFLAG) function.cpp; cd ..

kbase/kbase.o : kbase/kbase.cpp
	cd kbase; gcc $(CFLAG) kbase.cpp; cd ..

kbase_ce/lexdbase.o : kbase_ce/lexdbase.cpp 
	cd kbase_ce; gcc $(CFLAG) lexdbase.cpp; cd ..

kbase/makrbase.o : kbase/makrbase.cpp 
	cd kbase; gcc $(CFLAG) makrbase.cpp; cd ..

kbase/mkbase.o : kbase/mkbase.cpp
	cd kbase; gcc $(CFLAG) mkbase.cpp; cd ..

kbase/model.o : kbase/model.cpp
	cd kbase; gcc $(CFLAG) model.cpp; cd ..

main/cemt.o : main/cemt.cpp
	cd main; gcc $(CFLAG) cemt.cpp; cd ..

kbase/prsrbase.o : kbase/prsrbase.cpp 
	cd kbase; gcc $(CFLAG) prsrbase.cpp; cd ..

trans/result.o : trans/result.cpp
	cd trans; gcc $(CFLAG) result.cpp; cd ..

trans/struct.o : trans/struct.cpp
	cd trans; gcc $(CFLAG) struct.cpp; cd ..

kbase/template.o : kbase/template.cpp
	cd kbase; gcc $(CFLAG) template.cpp; cd ..

basic/token.o : basic/token.cpp
	cd basic; gcc $(CFLAG) token.cpp; cd ..

trans/trans.o : trans/trans.cpp
	cd trans; gcc $(CFLAG) trans.cpp; cd ..

translib/translib.o : translib/translib.cpp
	cd translib; gcc $(CFLAG) translib.cpp; cd ..

trans/unify.o : trans/unify.cpp
	cd trans; gcc $(CFLAG) unify.cpp; cd ..

main/kbms.o : main/kbms.cpp
	cd main; gcc $(CFLAG) kbms.cpp; cd ..
