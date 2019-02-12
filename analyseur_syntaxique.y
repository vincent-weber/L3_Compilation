%{
#include<stdlib.h>
#include<stdio.h>
#define YYDEBUG 1
#include"syntabs.h" // pour syntaxe abstraite
extern n_prog *n;   // pour syntaxe abstraite
extern FILE *yyin;    // declare dans compilo
extern int yylineno;  // declare dans analyseur lexical
int yylex();          // declare dans analyseur lexical
int yyerror(char *s); // declare ci-dessous
%}

%union { int nval;
 		 char* idval;
		  n_l_instr* nlinstr; 
		  n_instr* ninstr;
		  n_exp* nexp;
		  n_l_exp* nlexp;
		  n_var* nvar;
		  n_l_dec* nldec;
		  n_dec* ndec;
		  n_prog* nprog;
		  n_appel* nappel;
		}

%token POINT_VIRGULE 
%token PLUS 
%token MOINS 
%token FOIS 
%token DIVISE 
%token PARENTHESE_OUVRANTE 
%token PARENTHESE_FERMANTE 
%token CROCHET_OUVRANT 
%token CROCHET_FERMANT 
%token ACCOLADE_OUVRANTE 
%token ACCOLADE_FERMANTE 
%token EGAL 
%token INFERIEUR 
%token ET 
%token OU 
%token NON 
%token SI 
%token ALORS 
%token SINON 
%token TANTQUE 
%token FAIRE 
%token ENTIER 
%token RETOUR 
%token LIRE 
%token ECRIRE 
%token <idval> IDENTIF
%token <nval> NOMBRE 
%token VIRGULE 

%type <nlinstr> linstr
%type <ninstr> instr iaffect isi itantque iretour iappelfct ibloc
%type <nexp> exp exp0 exp1 exp2 exp3 exp4 exp5 exp6
%type <nlexp> lexp lexpbis
%type <nvar> var
%type <nldec> ldecvaropt largopt ldecvar ldecvarbis ldecfct
%type <ndec> decvar decfct
%type <nprog> programme
%type <nappel> appelfct


//...
//TODO: compléter avec la liste des terminaux

%start programme
%%

programme : ldecvaropt ldecfct {$$ = cree_n_prog($1,$2);};

ldecvaropt : ldecvar POINT_VIRGULE
	| {$$ = NULL;}
	;
ldecvar : decvar ldecvarbis {$$ = cree_n_l_dec($1, $2);} ;
ldecvarbis : VIRGULE decvar ldecvarbis {$$ = cree_n_l_dec($2, $3);}
	| {$$ = NULL;}
	; 
decvar : ENTIER IDENTIF  
		 {$$ = cree_n_dec_var($2);}
	| 	 ENTIER IDENTIF CROCHET_OUVRANT NOMBRE CROCHET_FERMANT 
		 {$$ = cree_n_dec_tab($2,$4);} ;
//type : ENTIER;

ldecfct : decfct ldecfct {$$ = cree_n_l_dec($1, $2);}
	| 	  decfct {$$ = cree_n_l_dec($1, NULL);}
	;

decfct : IDENTIF PARENTHESE_OUVRANTE largopt PARENTHESE_FERMANTE ldecvaropt ibloc 
	{$$ = cree_n_dec_fonc($1, $3, $5, $6);};

largopt : ldecvar 
	| {$$ = NULL;}
	;
ibloc : ACCOLADE_OUVRANTE linstr ACCOLADE_FERMANTE { $$ = cree_n_instr_bloc($2);}; 

linstr : instr linstr {$$ = cree_n_l_instr($1, $2);}
	| {$$ = NULL;}
	;

instr : iaffect 
	|	isi 
 	|	itantque 
	|	iretour
	|	iappelfct
	|	ibloc
	;
iaffect : var EGAL exp POINT_VIRGULE {$$ = cree_n_instr_affect($1, $3);}
	;
isi : SI exp ALORS ibloc {$$ = cree_n_instr_si($2, $4, NULL);}
	| SI exp ALORS ibloc SINON ibloc {$$ = cree_n_instr_si($2, $4, $6);}
	;

itantque : TANTQUE exp FAIRE ibloc {$$ = cree_n_instr_tantque($2, $4);} ;
iretour : RETOUR exp POINT_VIRGULE {$$ = cree_n_instr_retour($2);} ;
iappelfct : appelfct POINT_VIRGULE {$$ = cree_n_instr_appel($1);} ;
appelfct : IDENTIF PARENTHESE_OUVRANTE lexp PARENTHESE_FERMANTE {$$ = cree_n_appel($1, $3);};

lexp : exp lexpbis {$$ = cree_n_l_exp($1, $2);}
	| {$$ = NULL;}
	;

lexpbis : VIRGULE exp lexpbis {$$ = cree_n_l_exp($2, $3);}
	| {$$ = NULL;}
	;
exp : exp OU exp0 {$$ = cree_n_exp_op(ou, $1, $3);}
	| exp0
	;
exp0 : exp0 ET exp1 {$$ = cree_n_exp_op(et, $1, $3);}
	| exp1
	;
exp1 : exp1 EGAL exp2 {$$ = cree_n_exp_op(egal, $1, $3);}
	| exp1 INFERIEUR exp2 {$$ = cree_n_exp_op(inferieur, $1, $3);}
	| exp2
	;
exp2 : exp2 PLUS exp3 {$$ = cree_n_exp_op(plus, $1, $3);}
	| exp2 MOINS exp3 {$$ = cree_n_exp_op(moins, $1, $3);}
	| exp3
	;
exp3 : exp3 FOIS exp4 {$$ = cree_n_exp_op(fois, $1, $3);}
	| exp3 DIVISE exp4 {$$ = cree_n_exp_op(divise, $1, $3);}
	| exp4
	;
exp4 : NON exp4 {$$ = cree_n_exp_op(non, $2, NULL);}
	| exp5
	;
exp5 : PARENTHESE_OUVRANTE exp5 PARENTHESE_FERMANTE {$$ = $2;}
	| exp6
	;
exp6 : var {$$ = cree_n_exp_var($1);}
	| NOMBRE {$$ = cree_n_exp_entier($1);}
	| appelfct {$$ = cree_n_exp_appel($1);}
	;
var : IDENTIF {$$ = cree_n_var_simple($1);}
	| IDENTIF CROCHET_OUVRANT exp CROCHET_FERMANT {$$ = cree_n_var_indicee($1, $3);}
	;

 
//TODO: compléter avec les productions de la grammaire

%%

int yyerror(char *s) {
  fprintf(stderr, "erreur de syntaxe ligne %d\n", yylineno);
  fprintf(stderr, "%s\n", s);
  fclose(yyin);
  exit(1);
}
