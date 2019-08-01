#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
struct instruction
{
	char label[10];
	char opcode[10];
	char operand[12];
};

struct symbol
{
	char lab[10];
	unsigned int location;
};

struct opc
{
	char mnemonic[10];
	unsigned int code;
};

bool isComment(char line[])
{
	if (line[0]==';')
		return true;
	else 
		return false;
}

int lengthofConstant(char line[])
{
	if (line[0]=='X')
		return 1;
	else
		return 3;
}
bool searchSymtab(char label[])
{
	printf("searching symtab\n");
	FILE *fp=fopen("symtab.dat","r");
	struct symbol obj;
	while(fread(&obj,sizeof(struct symbol),1,fp))
	{
		if(strcmp(label,obj.lab)==0){
			fclose(fp);
			return true;
		}
	}


	fclose(fp);
	return false;
}

/*struct opcode optabSplitter(char line[])
{
	char word[2][12];
    struct opcode obj;
    int i, j, count;
    j = 0; count = 0;

    for (i = 0; i <= strlen(line); i++) 
    {
        if (line[i] == ' ' || line[i] == '\0')
        {
            word[count][j] = '\0';
            count = count + 1;
            j = 0;
        }

        else
        {
            word[count][j] = line[i];
            j = j + 1;
        }
    } 
    strcpy(obj.mnemonic,word[0]);
    obj.code=atoi(word[1]);

    return obj;
}
*/



void addSymbol(char label[],int LOCCTR)
{
	printf("Adding symbol\n");
	FILE *fp=fopen("symtab.dat","a");
	struct symbol obj;
	strcpy(obj.lab,label);
	obj.location=LOCCTR;
	fwrite(&obj,sizeof(struct symbol),1,fp);
	fclose(fp);
}

bool isSymbol(struct instruction obj)
{
	if(strcmp("*****",obj.label)==0)
		return false;
	else
		return true;
}



struct instruction instructionSplitter(char line[])
{
    char word[3][12];
    struct instruction obj;
    int i, j, count;
    j = 0; count = 0;

    for (i = 0; i <= strlen(line); i++) 
    {
        if (line[i] == ' ' || line[i] == '\n'||line[i]=='\0')
        {
            word[count][j] = '\0';
            count = count + 1;
            j = 0;
        }

        else
        {
            word[count][j] = line[i];
            j = j + 1;
        }
    } 
    strcpy(obj.label,word[0]);
    strcpy(obj.opcode,word[1]);
    strcpy(obj.operand,word[2]);

    return obj;
}

unsigned int stringToHex(char line[])
{
	unsigned int hex;
	hex=hexadecimalToDecimal(line);
	return hex;
}

int hexadecimalToDecimal(char hexVal[]) 
{    
    int len = strlen(hexVal); 
      
    // Initializing base value to 1, i.e 16^0 
    int base = 1; 
      
    int dec_val = 0; 
      
    // Extracting characters as digits from last character 
    for (int i=len-1; i>=0; i--) 
    {    
        // if character lies in '0'-'9', converting  
        // it to integral 0-9 by subtracting 48 from 
        // ASCII value. 
        if (hexVal[i]>='0' && hexVal[i]<='9') 
        { 
            dec_val += (hexVal[i] - 48)*base; 
                  
            // incrementing base by power 
            base = base * 16; 
        } 
  
        // if character lies in 'A'-'F' , converting  
        // it to integral 10 - 15 by subtracting 55  
        // from ASCII value 
        else if (hexVal[i]>='A' && hexVal[i]<='F') 
        { 
            dec_val += (hexVal[i] - 55)*base; 
          
            // incrementing base by power 
            base = base*16; 
        } 
    } 
      
    return dec_val; 
} 

bool searchOptab(char opcode[])
{

	FILE *fp=fopen("optab.txt","r");
	printf("opened optab file\n");
	struct opc obj;
	char buf[12];
	while(EOF!=fscanf(fp,"%s %s",obj.mnemonic,buf))
	{
		obj.code=stringToHex(buf);
		if(strcmp(obj.mnemonic,opcode)==0)
		{
			fclose(fp);
			return true;
		}
	}

	fclose(fp);
	return false;
}

int main(){
	int p=0;
	struct instruction obj;
	
	FILE *fp= fopen("alp.txt","r");
	
	FILE *fo= fopen("intermediate.txt","w");

	FILE *flen= fopen("length.txt","w");
	
	unsigned int starting_address;
	unsigned int LOCCTR;
	unsigned int length;

	//read first input line
	fscanf(fp,"%s %s %s",obj.label,obj.opcode,obj.operand);
	
	if (strcmp("START",obj.opcode)==0)	{
		//save operand as starting address
		printf("flag\n");
		starting_address=stringToHex(obj.operand);
		//initialize LOCCTR to starting address
		LOCCTR=starting_address;

		//write line to intermediate file
		fprintf(fo,"%x %s %s %s\n",LOCCTR,obj.label,obj.opcode,obj.operand);

		//read next input line
		fscanf(fp,"%s %s %s",obj.label,obj.opcode,obj.operand);


	}// end {if START}
	else
		LOCCTR=0x0;

	fprintf(fo, "%x ",LOCCTR);

	while(strcmp(obj.opcode,"END")!=0)//while OPCODE!=END
	{
		if (!isComment(obj.label))// if this is not a comment line
		{

			if (isSymbol(obj))// if there is a symbol in the LABEL field
			{
				bool found;
				//search SYMTAB for LABEL
				found=searchSymtab(obj.label);
				if(found)
				{
					printf("duplicate symbol %s\n",obj.label);
					exit(0);
				}
				else
					addSymbol(obj.label,LOCCTR);
			}// end if symbol

			//search OPTAB for OPCODE
			bool found=searchOptab(obj.opcode);
			printf("flag 1\n");
			if (found)
			{
				LOCCTR+=3;//add 3 {instruction length} to LOCCTR
			}
			else if (strcmp(obj.opcode,"WORD")==0)
			{
				LOCCTR+=3;
			}
			else if (strcmp(obj.opcode,"RESW")==0)
			{
				int op=atoi(obj.operand);
				LOCCTR+=(3*op);
			}
			else if(strcmp(obj.opcode,"RESB")==0)
			{
				int op=atoi(obj.operand);
				LOCCTR+=op;
			}
			else if(strcmp(obj.opcode,"BYTE")==0)
			{
				LOCCTR+=lengthofConstant(obj.operand);
			}
			else
			{
				printf("Invalid operation code.\n");
				exit(0);
			}
		}//end {if not a comment}
		//write line to intermediate file
		fprintf(fo,"%s %s %s\n%x ",obj.label,obj.opcode,obj.operand,LOCCTR);
		//read next input line
		fscanf(fp,"%s %s %s",obj.label,obj.opcode,obj.operand);
	}// end {while not END}
	//write last line to intermediate file
	fprintf(fo,"%s %s %s\n",obj.label,obj.opcode,obj.operand);
	//save {LOCCTR-starting address} as program length
	length=LOCCTR-starting_address;
	fprintf(flen, "%x\n",length);
}
