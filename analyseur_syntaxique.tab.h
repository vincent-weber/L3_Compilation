/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_ANALYSEUR_SYNTAXIQUE_TAB_H_INCLUDED
# define YY_YY_ANALYSEUR_SYNTAXIQUE_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    POINT_VIRGULE = 258,
    PLUS = 259,
    MOINS = 260,
    FOIS = 261,
    DIVISE = 262,
    PARENTHESE_OUVRANTE = 263,
    PARENTHESE_FERMANTE = 264,
    CROCHET_OUVRANT = 265,
    CROCHET_FERMANT = 266,
    ACCOLADE_OUVRANTE = 267,
    ACCOLADE_FERMANTE = 268,
    EGAL = 269,
    INFERIEUR = 270,
    ET = 271,
    OU = 272,
    NON = 273,
    SI = 274,
    ALORS = 275,
    SINON = 276,
    TANTQUE = 277,
    FAIRE = 278,
    ENTIER = 279,
    RETOUR = 280,
    LIRE = 281,
    ECRIRE = 282,
    IDENTIF = 283,
    NOMBRE = 284,
    VIRGULE = 285
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 13 "analyseur_syntaxique.y" /* yacc.c:1909  */
 int nval;
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
		

#line 99 "analyseur_syntaxique.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_ANALYSEUR_SYNTAXIQUE_TAB_H_INCLUDED  */
