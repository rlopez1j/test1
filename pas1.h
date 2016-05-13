/*
Roman Lopez
this has pass one functions 
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "optable.h"
#include "symboltable.h"
#include "errors.h"

long int LOCCTR;
long int STARTADD;
struct Symbol *SYMTAB;
int length;
int *error[8];

void pass_one_algo(char *source)//this function reads from the source code
{
	memset(error, 0, 8);
	SYMTAB = malloc(100 * sizeof(struct Symbol));
	memset(SYMTAB, 0, 100 * sizeof(struct Symbol));

    FILE *fp;
    char line[160];
	FILE *p;
	char * ln = NULL;

	p = fopen("intermediate.txt","w");
	if ((fp = fopen(source, "r")) == NULL)
	{
		puts("Cannot open file.\n");
		return -1;
	}
	
	fgets(line, 159, fp);
	ln = malloc(4*sizeof(line));
	strcpy(ln, line);
	printf("%s \n", ln);

	char *LABEL = strtok(line, " \t");
	char *OPCODE = strtok(NULL, " \t");
	char *OPERAND = strtok(NULL, " \t");


	if (!strcmp(OPCODE, "START"))
	{
		if (isdigit(OPERAND))
			error[4] = 1; //sets error flag 5
		STARTADD = strtol(OPERAND, NULL, 16); //makes operenad starting address
		LOCCTR = STARTADD;
		fprintf(p, "%s", ln); //writes to intermediate
		fprintf(p, "%s", OPCODE);
		fprintf(p, "%X\n\n", LOCCTR);
	}
	else
		LOCCTR = 0x0;
	while (!feof(fp))
    {
		fgets(line, 159, fp);
		memset(error, 0, 8); //resets error flags
		
		printf("%s", line);		
		strcpy(ln, line);
		fprintf(p, "%s", ln); //write line to intermediate file		
		if (LABEL[0] != '.')
		{
			if (line[0] == '\t') //cases for no label
			{
				LABEL = " ";
				OPCODE = strtok(line, " \t");
				OPERAND = strtok(NULL, " \t");
			}
			else
			{
				LABEL = strtok(line, " \t");
				OPCODE = strtok(NULL, " \t");
				OPERAND = strtok(NULL, " \t");
			}
			if (!strcmp(OPCODE, "END"))
				break;

			fprintf(p, "%s\n", OPCODE);
			fprintf(p, "%s\n", OPERAND);
			fprintf(p, "%X\n", LOCCTR);//writes locc in file

			if (LABEL[0] !=  ' ') //if label is not empty
			{
				if (contains(LABEL, SYMTAB))
					error[0] = 1;
				else
				{
					symtab_insert(LABEL, LOCCTR, SYMTAB); //insert into SYMTAB
					set_error(find_error(LABEL, error),error);
				}
			}
			if (optable_contains(OPCODE)) //Search OPTABLE for OPCODE
				LOCCTR += 0x03;
			else if (!strcmp(OPCODE, "WORD"))
				LOCCTR += 0x03;
			else if (!strcmp(OPCODE, "RESW"))
			{
				if (isdigit(OPERAND))
					error[3] = 1;
				else
					LOCCTR += 0x03 * strtol(OPERAND, NULL, 16); //operand number
			}
			else if (!strcmp(OPCODE, "RESB"))
			{
				if (isdigit(OPCODE))
					error[3] = 1;
				else
					LOCCTR += strtol(OPERAND, NULL, 10);
			}
			else if (!strcmp(OPCODE, "BYTE"))
			{ //find length of constant in bytes
				if (OPERAND[0] == 'C')
					LOCCTR += ((int)strlen(OPERAND) - 4);
				else if (OPERAND[0] == 'X')
					LOCCTR += (((int)strlen(OPERAND) - 4) * 2) - 1;
				else
					error[3] = 1;
			}
			else
				error[2] = 1; //set error flag
		}
		write_errors(p,error);
	}
	if (strcmp(OPCODE, "END")) //if end is missing
		error[5] = 1;
	
	fprintf(p, "%s", ln);
	length = LOCCTR - STARTADD;
	printf("program length: %X\n", length);
	//check for length error
	fclose(fp);
	fclose(p);
}
void pass_one(char *filename)
{
	create_opcode_table(); //creates opcode table
    pass_one_algo(filename);
}
