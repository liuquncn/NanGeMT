#include "../kbase/model.h"
#include "../trans_ce/cetrans.h"
#include "../kbase/template.h"
#include "../trans/result.h"

ChnEngTranslator GlobalTranslator;

int GlobalSrcLang;
int GlobalTgtLang;
Model * GlobalModel;
ChnDictn * GlobalDictn;
ChnDictn * GlobalDictnref;
Template * GlobalTemplate;
ChnOvlRbase * GlobalOvlRbase;
ChnSegRbase * GlobalSegRbase;
ChnTagRbase * GlobalTagRbase;
ChnPrsRbase * GlobalPrsRbase;
EngMakRbase * GlobalMakRbase;
EngBldRbase * GlobalBldRbase;
ChnLexDbase * GlobalLexDbase;

unsigned char OptionNewLine;
unsigned char OptionSpaceLine;
unsigned char OptionEmptyLine;
unsigned char OptionCommaColon;
unsigned char OptionLexdbase;
unsigned char OptionDictnref;
unsigned char OptionAttribute;
unsigned char OptionTrace;
unsigned int  OptionMultiUser;
unsigned int  OptionResultNumber;
unsigned int  OptionPreEdit;

Translator * TheTranslator=&GlobalTranslator;

// from transdoc.h

fstream m_ThisFile;
fstream m_TempFile;
fstream m_SpyFile;
fstream m_TraceFile;
fstream m_CountFile;
fstream m_ResultFile;
unsigned char m_EndStage;

int main(int argc, char* argv[])
{
  GlobalModel = new Model;
  GlobalDictn = new ChnDictn;
  GlobalDictnref = new ChnDictn;
  GlobalTemplate = new Template;
  GlobalOvlRbase = new ChnOvlRbase;
  GlobalSegRbase = new ChnSegRbase;
  GlobalTagRbase = new ChnTagRbase;
  GlobalPrsRbase = new ChnPrsRbase;
  GlobalMakRbase = new EngMakRbase;
  GlobalBldRbase = new EngBldRbase;
  GlobalLexDbase = new ChnLexDbase;
  
  Kbase::setHome("mtoutput/kbase"); 
  
  GlobalSrcLang=CHINESE;
  GlobalTgtLang=ENGLISH;
  GlobalModel->setPath("model");
  GlobalModel->setName("model");
  GlobalDictn->setPath("dictn");
  GlobalDictn->setName("dictn");
  GlobalDictn->setTgtLang(GlobalTgtLang);
  GlobalDictnref->setPath("dictnref");
  GlobalDictnref->setName("dictnref");
  GlobalDictnref->setTgtLang(GlobalTgtLang);
  GlobalTemplate->setPath("template");
  GlobalTemplate->setName("template");
  GlobalOvlRbase->setPath("ovlrbase");
  GlobalOvlRbase->setName("ovlrbase");
  GlobalSegRbase->setPath("segrbase");
  GlobalSegRbase->setName("segrbase");
  GlobalTagRbase->setPath("tagrbase");
  GlobalTagRbase->setName("tagrbase");
  GlobalPrsRbase->setPath("prsrbase");
  GlobalPrsRbase->setName("prsrbase");
  GlobalPrsRbase->setTgtLang(GlobalTgtLang);
  GlobalMakRbase->setPath("makrbase");
  GlobalMakRbase->setName("makrbase");
  GlobalBldRbase->setPath("bldrbase");
  GlobalBldRbase->setName("bldrbase");
  GlobalLexDbase->setPath("lexdbase");
  GlobalLexDbase->setName("lexdbase");
  
  // setKbaseA()
  GlobalTranslator.setModel(GlobalModel);
  GlobalTranslator.setChnDictn(GlobalDictn);
  GlobalTranslator.setChnDictnref(GlobalDictnref);
  GlobalTranslator.setChnPrsRbase(GlobalPrsRbase);
  GlobalTranslator.setEngMakRbase(GlobalMakRbase);
  GlobalTranslator.setChnOvlRbase(GlobalOvlRbase);
  GlobalTranslator.setChnSegRbase(GlobalSegRbase);
  GlobalTranslator.setChnTagRbase(GlobalTagRbase);
  GlobalTranslator.setChnLexDbase(GlobalLexDbase);
  GlobalTranslator.setEngBldRbase(GlobalBldRbase);

  // buildAll();
  GlobalModel->build();
  GlobalDictn->build();
  GlobalDictnref->build();
  GlobalTemplate->build(); 
  GlobalOvlRbase->build(); 
  GlobalSegRbase->build(); 
  GlobalTagRbase->build(); 
  GlobalPrsRbase->build(); 
  GlobalMakRbase->build(); 
  GlobalBldRbase->build(); 
  GlobalLexDbase->build(); 
  
  TheTranslator->open();
  
  ResultRecord record;
  
  int resultnumber=0;
  int OptionResultNumber=5;
  
  char srcsent[100];
  ifstream srcfile;
  srcfile.open("mtoutput/chnsent.txt");
  //TheTranslator->WchFile.open("mtcon.trc",ios::out|ios::trunc);
  while (!srcfile.eof()) {
    srcfile.getline(srcsent,99);
    cout << srcsent << endl;
    if (!strcmp(srcsent,"") || !strcmp(srcsent,"\r")) break;
    TheTranslator->setSourceSentence(srcsent);
    TheTranslator->translateSentence();
    TheTranslator->makTargetSentence();
    cout << TheTranslator->getTargetSentence() << endl;
    /*
    while (TheTranslator->translateSentence())
    {
    while (TheTranslator->makTargetSentence()) {
	cout << TheTranslator->getTargetSentence() << endl;
	//record.AddResult(TheTranslator->getTargetSentence(),TheTranslator->getTargetTree(),TheTranslator->getSourceTree());
	resultnumber++;
      }
	if (OptionResultNumber!=0 && resultnumber>=OptionResultNumber) break;
    }
    */
    TheTranslator->reset();
  }
  
  TheTranslator->WchFile.close();
  TheTranslator->close();
  
  //freeAll();
  GlobalLexDbase->free();
  GlobalBldRbase->free();
  GlobalMakRbase->free();
  GlobalPrsRbase->free();
  GlobalTagRbase->free(); 
  GlobalSegRbase->free(); 
  GlobalOvlRbase->free(); 
  GlobalTemplate->free(); 
  GlobalDictnref->free(); 
  GlobalDictn->free(); 
  
  m_TempFile.close();
  m_CountFile.close();
  m_ResultFile.close();
  
  return 0;
}
