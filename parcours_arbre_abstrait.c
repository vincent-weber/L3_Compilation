#include <stdio.h>
#include "syntabs.h"
#include "tabsymboles.h"
#include "code3a.h"
#include "util.h"
#include <string.h>
#include <stdlib.h>
int adresseGlobaleCourante = 0;


void parcours_n_prog(n_prog *n);
void parcours_l_instr(n_l_instr *n);
void parcours_instr(n_instr *n);
void parcours_instr_si(n_instr *n);
void parcours_instr_tantque(n_instr *n);
void parcours_instr_affect(n_instr *n);
void parcours_instr_appel(n_instr *n);
void parcours_instr_retour(n_instr *n);
void parcours_instr_ecrire(n_instr *n);
void parcours_l_exp(n_l_exp *n);
operande* parcours_exp(n_exp *n);
operande* parcours_varExp(n_exp *n);
operande* parcours_opExp(n_exp *n);
operande* parcours_intExp(n_exp *n);
operande* parcours_lireExp(n_exp *n);
operande* parcours_appelExp(n_exp *n);
void parcours_l_dec(n_l_dec *n);
void parcours_dec(n_dec *n);
void parcours_foncDec(n_dec *n);
void parcours_varDec(n_dec *n);
void parcours_tabDec(n_dec *n);
operande* parcours_var(n_var *n);
operande* parcours_var_simple(n_var *n);
operande* parcours_var_indicee(n_var *n);
void parcours_appel(n_appel *n);

int trace_abs_parcours = 1;

/*-------------------------------------------------------------------------*/

void parcours_n_prog(n_prog *n)
{
	portee = P_VARIABLE_GLOBALE;
	adresseLocaleCourante = 0;
	adresseArgumentCourant= 0;
	printf("debut parcours arbre\n");
  parcours_l_dec(n->variables);
  parcours_l_dec(n->fonctions); 
  if (rechercheExecutable("main") == -1) {
	printf("warning : pas de main \n");}
	
}

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

void parcours_l_instr(n_l_instr *n)
{
  if(n){
	  parcours_instr(n->tete);
	  parcours_l_instr(n->queue);
  }
}

/*-------------------------------------------------------------------------*/

void parcours_instr(n_instr *n)
{
  if(n){
    if(n->type == blocInst) parcours_l_instr(n->u.liste);
    else if(n->type == affecteInst) parcours_instr_affect(n);
    else if(n->type == siInst) parcours_instr_si(n);
    else if(n->type == tantqueInst) parcours_instr_tantque(n);
    else if(n->type == appelInst) parcours_instr_appel(n);
    else if(n->type == retourInst) parcours_instr_retour(n);
    else if(n->type == ecrireInst) parcours_instr_ecrire(n);
  }
}

/*-------------------------------------------------------------------------*/

void parcours_instr_si(n_instr *n)
{  
  operande* constante0 = code3a_new_constante(0);
  operande *result_test = parcours_exp(n->u.si_.test);
  operande *etiquette1 = code3a_new_etiquette_auto();
  operande* etiquette2 = code3a_new_etiquette_auto();
  if(n->u.si_.sinon){
    code3a_ajoute_instruction(jump_if_equal,result_test,constante0,etiquette1,"si test faux");
  } else {
    code3a_ajoute_instruction(jump_if_equal,result_test,constante0,etiquette2,"si test faux");
  }
  parcours_instr(n->u.si_.alors);
  if (n->u.si_.alors->type != retourInst)
  	code3a_ajoute_instruction(jump,etiquette2, NULL ,NULL,"jump fin si");
  if(n->u.si_.sinon){
  	code3a_ajoute_etiquette(etiquette1->u.oper_nom);
    parcours_instr(n->u.si_.sinon);
  }
  code3a_ajoute_etiquette(etiquette2->u.oper_nom);
}

/*-------------------------------------------------------------------------*/

void parcours_instr_tantque(n_instr *n)
{
  operande* constante0 = code3a_new_constante(0);
  operande *etiquette1 = code3a_new_etiquette_auto();
  operande *etiquette2 = code3a_new_etiquette_auto();
  code3a_ajoute_etiquette(etiquette1->u.oper_nom);
  operande* result_test = parcours_exp(n->u.tantque_.test);
  code3a_ajoute_instruction(jump_if_equal,result_test,constante0,etiquette2,"tq test faux");
  parcours_instr(n->u.tantque_.faire);
  code3a_ajoute_instruction(jump,etiquette1, NULL ,NULL,"jump test tq");
  code3a_ajoute_etiquette(etiquette2->u.oper_nom);
}

/*-------------------------------------------------------------------------*/

void parcours_instr_affect(n_instr *n)
{
  
  operande* var = parcours_var(n->u.affecte_.var);
  operande* exp = parcours_exp(n->u.affecte_.exp);
  code3a_ajoute_instruction(assign,exp, NULL ,var,"affect");
  
}

/*-------------------------------------------------------------------------*/

void parcours_instr_appel(n_instr *n)
{
  parcours_appel(n->u.appel);
char *etiquette = malloc(20*sizeof(char));
  sprintf(etiquette, "f%s", n->u.appel->fonction);
  operande *etiquette1 = code3a_new_etiquette(etiquette);
  code3a_ajoute_instruction(func_call,etiquette1, NULL ,NULL,"call");
  
}
/*-------------------------------------------------------------------------*/

void parcours_appel(n_appel *n)
{
  operande* constante1 = code3a_new_constante(1);
  code3a_ajoute_instruction(alloc,constante1, NULL ,NULL,"alloc val ret");
  int line = rechercheExecutable(n->fonction);
  if ( line != -1){
	int argcount = 0;
	n_l_exp *ncourant = n->args;
	while (ncourant != NULL) {
		ncourant = ncourant->queue;
		++argcount;
	}
	if (tabsymboles.tab[line].complement == argcount){
		parcours_l_exp(n->args);
	}
	else {} // erreur nb arg
  }
  else{
	printf("warning : pas de fonction %s\n", n->fonction);
  }
}

/*-------------------------------------------------------------------------*/

void parcours_instr_retour(n_instr *n)
{
  operande* ret = parcours_exp(n->u.retour_.expression);
  code3a_ajoute_instruction(func_val_ret,ret, NULL ,NULL,"ret");
  code3a_ajoute_instruction(func_end,NULL, NULL ,NULL,"fend apres ret");
}

/*-------------------------------------------------------------------------*/

void parcours_instr_ecrire(n_instr *n)
{
  operande* op1 = parcours_exp(n->u.ecrire_.expression);
  code3a_ajoute_instruction(sys_write, op1, NULL, NULL,
                               "write");
}

/*-------------------------------------------------------------------------*/

void parcours_l_exp(n_l_exp *n)
{
  if(n){
    operande* tete = parcours_exp(n->tete);
	code3a_ajoute_instruction(func_param,tete, NULL ,NULL,"param");
    parcours_l_exp(n->queue);
  }
}

/*-------------------------------------------------------------------------*/

operande* parcours_exp(n_exp *n)
{
  operande *result;
  if(n->type == varExp) result = parcours_varExp(n);
  else if(n->type == opExp) result=parcours_opExp(n);
  else if(n->type == intExp) result=parcours_intExp(n);
  else if(n->type == appelExp) result=parcours_appelExp(n);
  else if(n->type == lireExp) result= parcours_lireExp(n);
  return result;
}

/*-------------------------------------------------------------------------*/

operande* parcours_varExp(n_exp *n)
{
  int l;
  if ((l=rechercheExecutable(n->u.var->nom)) != -1) {
  	operande *var = parcours_var(n->u.var);
    return var;
  }
  else {
	printf("warning : pas de variable %s\n", n->u.var->nom);
	return NULL;
  }
}

/*-------------------------------------------------------------------------*/
operande* parcours_opExp(n_exp *n)
{
  operande* constante0 = code3a_new_constante(0);
  operande* constante1 = code3a_new_constante(-1);
  operande *op1;
  operande *op2;
  operande *r =code3a_new_temporaire();
  if( n->u.opExp_.op1 != NULL ) {
    op1 = parcours_exp(n->u.opExp_.op1);
  }
  if( n->u.opExp_.op2 != NULL ) {
    op2 = parcours_exp(n->u.opExp_.op2);
  }
  if(n->u.opExp_.op == plus){
	code3a_ajoute_instruction(arith_add,op1,op2,r,"opé arith +");
  }
  else if(n->u.opExp_.op == moins){
	code3a_ajoute_instruction(arith_sub,op1,op2,r,"opé arith -");
  }
  else if(n->u.opExp_.op == fois){
	code3a_ajoute_instruction(arith_mult,op1,op2,r,"opé arith *");
  }
  else if(n->u.opExp_.op == divise){
	code3a_ajoute_instruction(arith_div,op1,op2,r,"opé arith /");
  }
  else if(n->u.opExp_.op == egal){
	operande *etiquette1 = code3a_new_etiquette_auto();
    code3a_ajoute_instruction(assign,constante1,NULL,r,"vrai");
	code3a_ajoute_instruction(jump_if_equal,op1,op2,etiquette1,"opé arith ==");
	code3a_ajoute_instruction(assign,constante0,NULL,r,"faux");
	code3a_ajoute_etiquette(etiquette1->u.oper_nom);
  }
  else if(n->u.opExp_.op == inferieur){
	operande *etiquette1 = code3a_new_etiquette_auto();
    code3a_ajoute_instruction(assign,constante1,NULL,r,"vrai");
	code3a_ajoute_instruction(jump_if_less,op1,op2,etiquette1,"opé arith <");
	code3a_ajoute_instruction(assign,constante0,NULL,r,"faux");
	code3a_ajoute_etiquette(etiquette1->u.oper_nom);
  }
  else if(n->u.opExp_.op == ou){
	operande *etiquette1 = code3a_new_etiquette_auto();
	operande *etiquette2 = code3a_new_etiquette_auto();
	code3a_ajoute_instruction(jump_if_equal,op1,constante1,etiquette1,"opé ou op1");
	code3a_ajoute_instruction(jump_if_equal,op2,constante1,etiquette1,"opé ou op2");
	code3a_ajoute_instruction(assign,constante0,NULL,r,"faux");
	code3a_ajoute_instruction(jump,etiquette2,NULL,NULL,"goto");
	code3a_ajoute_etiquette(etiquette1->u.oper_nom);
	code3a_ajoute_instruction(assign,constante1,NULL,r,"vrai");
	code3a_ajoute_etiquette(etiquette2->u.oper_nom);
  }
  else if(n->u.opExp_.op == et){
	operande *etiquette1 = code3a_new_etiquette_auto();
	operande *etiquette2 = code3a_new_etiquette_auto();
	code3a_ajoute_instruction(jump_if_equal,op1,constante1,etiquette1,"opé ou op1");
	code3a_ajoute_instruction(jump_if_equal,op2,constante1,etiquette1,"opé ou op2");
	code3a_ajoute_instruction(assign,constante0,NULL,r,"faux");
	code3a_ajoute_instruction(jump,etiquette2,NULL,NULL,"goto");
	code3a_ajoute_etiquette(etiquette1->u.oper_nom);
	code3a_ajoute_instruction(assign,constante1,NULL,r,"vrai");
	code3a_ajoute_etiquette(etiquette2->u.oper_nom);
  }
  else if(n->u.opExp_.op == non){
	operande *etiquette1 = code3a_new_etiquette_auto();
	operande *etiquette2 = code3a_new_etiquette_auto();
	code3a_ajoute_instruction(jump_if_equal,op1,constante0,etiquette1,"opé non op1");
	code3a_ajoute_instruction(assign,constante1,NULL,r,"vrai");
	code3a_ajoute_instruction(jump,etiquette2,NULL,NULL,"goto");
	code3a_ajoute_etiquette(etiquette1->u.oper_nom);
	code3a_ajoute_instruction(assign,constante0,NULL,r,"faux");
	code3a_ajoute_etiquette(etiquette2->u.oper_nom);
  }
  return r;
}

/*-------------------------------------------------------------------------*/

operande* parcours_intExp(n_exp *n)
{
  char texte[ 50 ]; // Max. 50 chiffres
  sprintf(texte, "%d", n->u.entier);
  return code3a_new_constante(n->u.entier);
}

/*-------------------------------------------------------------------------*/
operande* parcours_lireExp(n_exp *n)
{
	operande* r =code3a_new_temporaire();
	code3a_ajoute_instruction(sys_read,NULL,NULL,r,"read");
	return r;
}

/*-------------------------------------------------------------------------*/

operande* parcours_appelExp(n_exp *n)
{
  
  parcours_appel(n->u.appel);
  operande *ret =code3a_new_temporaire();
  char *etiquette = malloc(20*sizeof(char));
  sprintf(etiquette, "f%s", n->u.appel->fonction);
  operande *etiquette1 = code3a_new_etiquette(etiquette);
  code3a_ajoute_instruction(func_call,etiquette1, NULL ,ret,"call");
  return ret;
}

/*-------------------------------------------------------------------------*/

void parcours_l_dec(n_l_dec *n)
{
  if( n ){
    parcours_dec(n->tete);
    parcours_l_dec(n->queue);
  }
}

/*-------------------------------------------------------------------------*/

void parcours_dec(n_dec *n)
{

  if(n){
    if(n->type == foncDec) {
      parcours_foncDec(n);
    }
    else if(n->type == varDec) {
      parcours_varDec(n);
    }
    else if(n->type == tabDec) { 
      parcours_tabDec(n);
    }
  }
}

/*-------------------------------------------------------------------------*/

void parcours_foncDec(n_dec *n)
{
  	if (rechercheDeclarative(n->nom) == -1) {
	  int argcount = 0;
	  n_l_dec *ncourant = n->u.foncDec_.param;
	  while (ncourant != NULL) {
		ncourant = ncourant->queue;
		++argcount;
	  }
	  ajouteIdentificateur(n->nom, portee, T_FONCTION, 0, argcount);

      char *etiquette = malloc(20*sizeof(char));
	  sprintf(etiquette, "f%s", n->nom);
	  code3a_ajoute_etiquette(etiquette);
	  code3a_ajoute_instruction(func_begin, NULL, NULL, NULL, "début fonc");

	  entreeFonction();
	  parcours_l_dec(n->u.foncDec_.param);
	  portee = P_VARIABLE_LOCALE;
	  parcours_l_dec(n->u.foncDec_.variables);
	  parcours_instr(n->u.foncDec_.corps);
		printf("Table symboles de la fonction %s\n",n->nom); 
	  sortieFonction(trace_abs_parcours);
	  code3a_ajoute_instruction(func_end, NULL, NULL, NULL, "fin fonc");
  	}
}

/*-------------------------------------------------------------------------*/

void parcours_varDec(n_dec *n)
{
  if (rechercheDeclarative(n->nom) == -1) {
	if (portee == P_VARIABLE_GLOBALE) {
		ajouteIdentificateur(n->nom, portee, T_ENTIER, adresseGlobaleCourante, 1);
		code3a_ajoute_instruction(alloc, code3a_new_constante(1), code3a_new_var(n->nom, portee, adresseGlobaleCourante), NULL, "alloc var glob");
		adresseGlobaleCourante += 4;
	} 
	else if (portee == P_VARIABLE_LOCALE) {
		ajouteIdentificateur(n->nom, portee, T_ENTIER, adresseLocaleCourante, 1);
		code3a_ajoute_instruction(alloc, code3a_new_constante(1), code3a_new_var(n->nom, portee, adresseLocaleCourante), NULL, "alloc var loc");
		adresseLocaleCourante += 4;
	}
	else if (portee == P_ARGUMENT) {
		ajouteIdentificateur(n->nom, portee, T_ENTIER, adresseArgumentCourant, 1);
		adresseArgumentCourant += 4;
	}
  }
  else{printf("attention la variable %s existe déjà dans la même portée\n", n->nom);}
}

/*-------------------------------------------------------------------------*/

void parcours_tabDec(n_dec *n)
{
  if (rechercheDeclarative(n->nom) == -1) {
	if (portee == P_VARIABLE_GLOBALE) {
		ajouteIdentificateur(n->nom, portee, T_TABLEAU_ENTIER, adresseGlobaleCourante, n->u.tabDec_.taille);
		adresseGlobaleCourante += 4 * n->u.tabDec_.taille;
		code3a_ajoute_instruction(alloc, code3a_new_constante(n->u.tabDec_.taille), code3a_new_var(n->nom, portee, adresseGlobaleCourante), NULL, "alloc tableau");
	} 
	else if (portee == P_VARIABLE_LOCALE) {
		printf("un tableau n'est que global\n");
	}
  }
}

/*-------------------------------------------------------------------------*/

operande* parcours_var(n_var *n)
{
  if(n->type == simple) {
    return parcours_var_simple(n);
  }
  else if(n->type == indicee) {
    return parcours_var_indicee(n);
  }
	else{
		return NULL;
	}
}

/*-------------------------------------------------------------------------*/
operande* parcours_var_simple(n_var *n)
{
	int l;
  if ((l=rechercheExecutable(n->nom)) == -1) {
	printf("warning : pas de variable %s\n", n->nom);
	return NULL;
  }
	else{
		return code3a_new_var(n->nom, tabsymboles.tab[l].portee, tabsymboles.tab[l].adresse);
	}
}

char* enleve_indice(char* tableau) {

	char* result = malloc(sizeof(tableau)*sizeof(char));
	for (int i = 0; i < sizeof(tableau) && tableau[i] != '[' ; ++i) {
		result[i] = tableau[i];
	}
	return result;
}

/*-------------------------------------------------------------------------*/
operande* parcours_var_indicee(n_var *n)
{
	int l;
	char *nom = malloc(20*sizeof(char));
  if ((l=rechercheExecutable(n->nom)) == -1){
	printf("warning : pas de variable %s\n", n->nom);
	return NULL;
	}
	else {
	  operande* indice = parcours_exp( n->u.indicee_.indice );
	  if( indice->oper_type==O_VARIABLE ){
			operande *temp = code3a_new_temporaire();
			code3a_ajoute_instruction(assign, indice, NULL, temp, "indice var");
			
	  		sprintf(nom, "%s[t%i]", n->nom,temp->u.oper_temp.oper_tempnum);
		}
	  else if( indice->oper_type==O_TEMPORAIRE ){
	  		sprintf(nom, "%s[t%i]", n->nom,indice->u.oper_temp.oper_tempnum);
		}
	  else if( indice->oper_type==O_CONSTANTE ){
	  		sprintf(nom, "%s[%i]", n->nom,indice->u.oper_valeur);
		}

	  /*char* nom_tableau = enleve_indice(nom);
	  operande* oper_tableau = code3a_new_var(nom_tableau, tabsymboles.tab[l].portee, tabsymboles.tab[l].adresse + indice->u.oper_valeur*4);
	  oper_tableau->u.oper_var.oper_indice = indice;
	  return oper_tableau;*/
	  return code3a_new_var(nom, tabsymboles.tab[l].portee, tabsymboles.tab[l].adresse + indice->u.oper_valeur*4);
	}
}
/*-------------------------------------------------------------------------*/
