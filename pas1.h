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
int error;

void pass_one_algo(char *source)//this function reads from the source code
{
	error = 0;
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
			error = 4; //sets error flag 5
		STARTADD = strtol(OPERAND, NULL, 16); //makes operenad starting address
		LOCCTR = STARTADD;
		fprintf(p, "%s", ln); //writes to intermediate
		fprintf(p, "%s\n", OPCODE);
		fprintf(p, "%X\n", LOCCTR);
		fprintf(p, "%i\n\n", error);
	}
	else
	{
		LOCCTR = 0x0;
		error = 4;
		fprintf(p, "%s", ln);
		fprintf(p, "%i\n\n", error);
	}

	while (!feof(fp))
    {
		fgets(line, 159, fp);
		error = 0; //resets error flags
		
		printf("%s", line);		
		strcpy(ln, line);
		fprintf(p, "%s", ln); //write line to intermediate file		
		if (LABEL[0] != '.')
		{
			if (line[0] == '\t') //cases for no label
			{
				LABEL = " ";
				OPCODE = strtok(line, " \t");
				OPERAND = strtok(NULL, " \n \t");
			}
			else
			{
				LABEL = strtok(line, " \t");
				OPCODE = strtok(NULL, " \t");
				OPERAND = strtok(NULL, " \n \t");
			}
			if (!strcmp(OPCODE, "END"))
			{
				fprintf(p, "%s\n", OPCODE);
				break;
			}

			fprintf(p, "%s\n", OPCODE);
			fprintf(p, "%s\n", OPERAND);
			fprintf(p, "%X\n", LOCCTR);//writes locc in file

			if (LABEL[0] != ' ') //if label is not empty
			{
				if (contains(LABEL, SYMTAB))
					error = 1;
				else
				{
					symtab_insert(LABEL, LOCCTR, SYMTAB); //insert into SYMTAB
					check_errors(LABEL, find_error(LABEL));
				}
			}
			if (optable_contains(OPCODE)) //Search OPTABLE for OPCODE
				LOCCTR += 0x03;
			else if (!strcmp(OPCODE, "WORD"))
				LOCCTR += 0x03;
			else if (!strcmp(OPCODE, "RESW"))
			{
				if (isdigit(OPERAND))
					error = 3;
				else
					LOCCTR += 0x03 * strtol(OPERAND, NULL, 16); //operand number
			}
			else if (!strcmp(OPCODE, "RESB"))
			{
				if (isdigit(OPCODE))
					error = 3;
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
					error = 3;
			}
			else
				error = 2; //set error flag

			fprintf(p, "%i\n\n", error);
		}
		else
			fprintf(p, "\n");
	}
	if (strcmp(OPCODE, "END")) //if end is missing
	{
		error = 5;
		fprintf(p, "%i", error);
	}
	
	length = LOCCTR - STARTADD;
	printf("program length: %X\n", length);
	//check for length error:
	if (length > 32768)
	{
		error = 6;
		fprintf(p, "%i", error);
	}

	fclose(fp);
	fclose(p);
}
void pass_one(char *filename)
{
	create_opcode_table(); //creates opcode table
    pass_one_algo(filename);
}
