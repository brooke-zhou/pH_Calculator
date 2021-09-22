#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<conio.h>
#include<ctype.h>
#include<string.h> 

// Ion product constant of water
#define Kw (1e-14) 

// define reagent data structure 
typedef  struct reagent{
         char name[20];
         double K1,K2,K3;
         char AB;
         char SW;
         struct reagent *next; 
}Reagent;

// a list for reagent data storage
static Reagent *Reagent_List;

// partition coefficients of acids. x = concentration of H+ ion
#define a1 (x*x*x / (x*x*x + x*x*K1 + x*K1*K2 + K1*K2*K3))
#define a2 (x*x*K1 / (x*x*x + x*x*K1 + x*K1*K2 + K1*K2*K3))
#define a3 (x*K1*K2 / (x*x*x + x*x*K1 + x*K1*K2 + K1*K2*K3))
#define a4 (K1*K2*K3 / (x*x*x + x*x*K1 + x*K1*K2 + K1*K2*K3))

 
// the main program
int main()
{
       Reagent* create();
       void check(int flag); 
       int one_species();
       int double_weak();
       int mix();
       int buffer();
       int list_mamage();
       int add_reagent();
       int help_aboutme();
       int bye(); 
       
       char choice;
       
       Reagent_List=create();// create list of reagents from data source
       
       do{
           system("color f0");
           system("cls"); 
           printf("\n");
           printf("       _   _    _____       _            _       _             \n");
           printf("      | | | |  /  __ \\     | |          | |     | |            \n");
           printf(" _ __ | |_| |  | /  \\/ __ _| | ___ _   _| | __ _| |_ ___  _ __ \n");
           printf("| '_ \\|  _  |  | |    / _` | |/ __| | | | |/ _` | __/ _ \\| '__|\n");
           printf("| |_) | | | |  | \\__/\\ (_| | | (__| |_| | | (_| | || (_) | |   \n");
           printf("| .__/\\_| |_/   \\____/\\__,_|_|\\___|\\__,_|_|\\__,_|\\__\\___/|_|   \n");
           printf("| |                                                            \n");
           printf("|_|                                                            \n");

           printf("\n\n\n********************************* What do you want to do? *****************************\n\n\n");
           printf("1. General calculations \t\t");
           printf("2. Calculate weak acid, base or salt system\n\n");
           printf("3. Solution mixing calculation\t\t");
           printf("4. Guide for buffer solution\n\n");
           printf("5. Reagent lookup and management\t");
           printf("6. Add new reagent\n\n");
           printf("7. User guide\t\t\t\t");
           printf("0. Exit");
           printf("\n\n\n***************************************************************************************\n\n\n");
        
           fflush(stdin); 
           scanf("%c",&choice) ;        
           switch (choice) {
                     case '1': check(one_species());break;
                     case '2': check(double_weak());break;
                     case '3': check(mix());break;
                     case '4': check(buffer());break;
                     case '5': check(list_mamage());break;
                     case '6': check(add_reagent());break;
                     case '7': check(help_aboutme());break;
                     case '0': check(bye());exit(1);
                     default: 
                         printf("\n\t\t\t   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\t\t\t   Invalid command!  \n\t\t\t   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^"),
                         fflush(stdin),getchar();
                     }
       }while(1); 
}

// dissociation constant file reader: convert char to double
double chartodouble(char K[])
{
       double k=0;
       int index=0,sign=1;
       
       k=K[0]-'0'+(K[2]-'0')*0.1+(K[3]-'0')*0.01;
       if(K[5]=='-')
           sign=-1;
       index=(10*(K[6]-'0')+(K[7]-'0'))*sign;
       k*=pow(10,index);
       
       return k; 
}

// file writer: convert double to char with specific format
int doubletochar(double k,char *storage)
{
      int i,j;

      if(k > 100)
      {
           strcpy(storage,"9.99e+99");
           return 0;
      }
      if(k < 1e-30)
      {
           strcpy(storage,"0.00e-00");
           return 0;
      }
      for(i=0;k<1000;k*=10,i++);
      k+=5;
      i-=3; 
      j=(int)(k/=10);// round numbers with too many sig figs
      storage[0]=j/100+'0';
      storage[1]='.';
      storage[2]=j%100/10+'0';
      storage[3]=j%10+'0';
      storage[4]='e';
      storage[5]='-';
      storage[6]=i/10+'0';
      storage[7]=i%10+'0';
      
      return 0;
}

// read reagent data and create a linked list
Reagent* create()
{
     Reagent *p1=NULL,*p2=NULL,*head=NULL;
     int n=0,i;
     double k;
     char item[50]={0},K[10]={0};
     FILE *fp; 
     if((fp=fopen("list.k","r")) == NULL)
     { 
         printf("´Fail to open reagent data file!\n");
         system("pause");
         exit(1);
     }      
     
     while(fgets(item,50,fp))
     {
         p1=(Reagent*)malloc(sizeof(Reagent));
         n++; 
         if(n==1)
             head=p2=p1;
         else
         { 
             p2->next=p1;
             p2=p1;
             p1->next=NULL;
         }
         for(i=0;i<20 && (isalnum(item[i]) || item[i] =='(' || item[i] ==')' || item[i] =='+');i++)
             p1->name[i]=item[i];
         p1->name[i]=0;
         strncpy(K,item+20,8);
         k=chartodouble(K); 
         p1->K1=k;
         strncpy(K,item+29,8);
         k=chartodouble(K); 
         p1->K2=k;
         strncpy(K,item+38,8);
         k=chartodouble(K);
         p1->K3=k;
         p1->AB=item[46];
         p1->SW=item[47];
     }
     fclose(fp);
     return head;      
}

// Detect runtime error by function return value
void check(int flag)
{
     system("cls"); 
     if(flag!=0)
     { 
         printf("\nSorry, something went wrong!\n");
         system("pause");
     } 
} 

// Reagent look up in linked list 
Reagent* find(char key[],Reagent *head) 
{
    Reagent *p=head;
     
    while(strcmp(key,p->name) && p->next != NULL)
        p=p->next;
    if(!(strcmp(key,p->name)))
        return p; 
    if(p->next == NULL)
    {
        printf("This reagent is not yet available.\n\n");
        system("pause"); 
        return NULL;
    }
    return p;// return the pointer to the found node or NULL if not found
} 

// Calculate pH from the concentrations of acid/base
double calculate_pH(double c,double K1,double K2,double K3,char ji,char AB)
{ 
        double ji0(double c,double K1,double K2,double K3,double x);
        double ji1(double c,double K1,double K2,double K3,double x);
        double ji2(double c,double K1,double K2,double K3,double x);
        double ji3(double c,double K1,double K2,double K3,double x);
        
        double (*pji)(double c,double K1,double K2,double K3,double x);
        double a=1e-14,b=10,x,eps=1e-15,t;
        
        // degree of dissociation correspond to different functions
        if(ji=='0')
            pji=ji0;
        else if(ji=='1')
            pji=ji1;
        else if(ji=='2')
            pji=ji2;
        else if(ji=='3')
            pji=ji3;
        
        if((fabs(pji(c,K1,K2,K3,a)) < eps) && (fabs(a) > eps))
            x=a;
        else if(fabs(pji(c,K1,K2,K3,b)) < eps && (fabs(b) > eps))
            x=b;
        else 
        {
             x=(b+a)/2;
             while(fabs(pji(c,K1,K2,K3,x)) > eps && (fabs(b-a) > eps))
             {
                 printf("a=%e\tb=%e\tx=%e\n",a,b,x);
                 if(pji(c,K1,K2,K3,a)*pji(c,K1,K2,K3,x) < 0)
                     b=x;
                 else if(pji(c,K1,K2,K3,b)*pji(c,K1,K2,K3,x) < 0)
                    a=x;
                 x=(b+a)/2;
             }
        }
        if(AB=='A')
            return -log10(x);
        else
            return 14+log10(x);      
}

// the functions derived from proton conservation and partition coefficients
double ji0(double c,double K1,double K2,double K3,double x)
{
    return x-c*(a2+2*a3+3*a4)-Kw/x;
}

double ji1(double c,double K1,double K2,double K3,double x)
{
    return x+c*(a1-a3-2*a4)-Kw/x;
}

double ji2(double c,double K1,double K2,double K3,double x)
{
    return x+c*(2*a1+a2-a4)-Kw/x;
}

double ji3(double c,double K1,double K2,double K3,double x)
{
    return x+c*(3*a1+2*a2+a3)-Kw/x;
}

// single solute calculation
int one_species()
{
    char name[20]={0},ji,choice;
    Reagent *solute; 
    double mass,mole,c;
    
    int givenc(Reagent *solute,char ji);
    int givenn(Reagent *solute,char ji);
    
    system("color f0");
    system("cls");
    printf("\n");
    printf(" _____ _             _                   _       _                _   _ \n");
    printf("/  ___(_)           | |                 | |     | |              | | | |\n");
    printf("\\ `--. _ _ __   __ _| | ___    ___  ___ | |_   _| |_ ___    _ __ | |_| |\n");
    printf(" `--. \\ | '_ \\ / _` | |/ _ \\  / __|/ _ \\| | | | | __/ _ \\  | '_ \\|  _  |\n");
    printf("/\\__/ / | | | | (_| | |  __/  \\__ \\ (_) | | |_| | ||  __/  | |_) | | | |\n");
    printf("\\____/|_|_| |_|\\__, |_|\\___|  |___/\\___/|_|\\__,_|\\__\\___|  | .__/\\_| |_/\n");
    printf("                __/ |                                      | |          \n");
    printf("               |___/                                       |_|          \n");

    loop1:printf("Please enter the chemical formula of the original acid/base corresponding to your substance of interest:\n\n");
    printf("E.g., to calculate H2CO3, please enter H2CO3\n");
    printf("      to calculate NaAc, please enter HAc\n"); 
    printf("      to calculate NH4Cl, please enter NH3\n\nEnter your chemical formula here:");
    fflush(stdin);  
    gets(name);
    if((solute=find(name,Reagent_List)) == NULL)
        main(),exit(1);
        
    printf("\nHow many degrees of dissociation are you interested in?\n\n");
    printf("E.g., to calculate H3PO4, please enter 0\n");
    printf("      to calculate NaH2PO4, please enter 1\n");
    printf("      to calculate Na2HPO4, please enter 2\n");
    printf("      to calculate Na3PO4, please enter 3\n\nEnter your degree of dissociation here:");
    fflush(stdin);
    scanf("%c",&ji); 
    
    printf("\nKnown quantity: 1. Concentration \tor  2. Amount of substance\n");
    fflush(stdin); 
    scanf("%c",&choice);
    switch (choice) 
    {
            case '1': check(givenc(solute,ji));break;
            case '2': check(givenn(solute,ji));break;
            default:
                    printf("Invalid command!\n");
                    system("COLOR f0");
                    system("cls");
                    goto loop1; 
     }
     return 0; 
}

// Calculated pH using concentration
int givenc(Reagent *solute,char ji)
{
    double c,pH=999.999;
    
    printf("Concentration (mol/L)  c=");
    fflush(stdin); 
    scanf("%lf",&c);
    
    if((solute->AB) == 'A') 
        pH=calculate_pH(c,solute->K1,solute->K2,solute->K3,ji,solute->AB);
    else if((solute->AB) == 'B') 
        pH=calculate_pH(c,solute->K1,solute->K2,solute->K3,ji,solute->AB);
    printf("\nThe pH of this solution is: %.3f\n\n",pH);
    
    system("pause"); 
     
    return 0;  
}

// Calculated pH using amount of substance and volume
int givenn(Reagent *solute,char ji)
{
    double v,c,n,pH=999.999;
     
    printf("Amount of solute (mol) n=");
    fflush(stdin); 
    scanf("%lf",&n);
    printf("Volume of solution (L) V=");
    fflush(stdin); 
    scanf("%lf",&v);
    c=n/v;
    
    if((solute->AB) == 'A') 
        pH=calculate_pH(c,solute->K1,solute->K2,solute->K3,ji,solute->AB);
    else if((solute->AB) == 'B') 
        pH=calculate_pH(c,solute->K1,solute->K2,solute->K3,ji,solute->AB);
    printf("\nThe pH of this solution is: %.3f\n\n",pH);
    
    system("pause"); 
    
    return 0; 
}

// Calculate pH of weak acid/base salt, using approximation formula
int double_weak()
{
    double Ka,Kb,c,H;
    
    system("COLOR f0");
    system("cls");
    printf("\n");
    printf(" _    _            _       ___       _     _     _______                  _____       _ _   \n");
    printf("| |  | |          | |     / _ \\     (_)   | |   / / ___ \\                /  ___|     | | |  \n");
    printf("| |  | | ___  __ _| | __ / /_\\ \\ ___ _  __| |  / /| |_/ / __ _ ___  ___  \\ `--.  __ _| | |_ \n");
    printf("| |/\\| |/ _ \\/ _` | |/ / |  _  |/ __| |/ _` | / / | ___ \\/ _` / __|/ _ \\  `--. \\/ _` | | __|\n");
    printf("\\  /\\  /  __/ (_| |   <  | | | | (__| | (_| |/ /  | |_/ / (_| \\__ \\  __/ /\\__/ / (_| | | |_ \n");
    printf(" \\/  \\/ \\___|\\__,_|_|\\_\\ \\_| |_/\\___|_|\\__,_/_/   \\____/ \\__,_|___/\\___| \\____/ \\__,_|_|\\__|\n");

    printf("\n-->Please enter the dissociation constant of the acid: Ka=");
    scanf("%lf",&Ka);
    printf("-->Please enter the dissociation constant of the base: Kb=");
    scanf("%lf",&Kb);
    printf("-->please enter the concentration of your substance of interest, unit mol/L: c=");
    scanf("%lf",&c);
    
    H=sqrt((Ka*(Kb*c+Kw))/(Ka+c));
    printf("\n>>>> pH=%.3f\n\n",-log10(H));
    
    system("pause"); 
    return 0;
}

// pH calculation of mixtures, given the formula and concentration of each ingredient
int mix()
{
    double mix_cal_H2SO4(Reagent *H2SO4,Reagent *acid,double c_H2SO4,double c);
    
    char name1[20]={0},name2[20]={0};
    double c1,c2,t,result;
    Reagent *reag1=NULL,*reag2=NULL,*pa=NULL,*pb=NULL,*tp=NULL;
    
    system("color f0"),system("cls"); 
    printf("\n");
    printf("___  ____      _                    _____       _            _       _   _             \n");
    printf("|  \\/  (_)    | |                  /  __ \\     | |          | |     | | (_)            \n");
    printf("| .  . |___  _| |_ _   _ _ __ ___  | /  \\/ __ _| | ___ _   _| | __ _| |_ _  ___  _ __  \n");
    printf("| |\\/| | \\ \\/ / __| | | | '__/ _ \\ | |    / _` | |/ __| | | | |/ _` | __| |/ _ \\| '_ \\ \n");
    printf("| |  | | |>  <| |_| |_| | | |  __/ | \\__/\\ (_| | | (__| |_| | | (_| | |_| | (_) | | | |\n");
    printf("\\_|  |_/_/_/\\_\\__|\\__,_|_|  \\___|  \\____/\\__,_|_|\\___|\\__,_|_|\\__,_|\\__|_|\\___/|_| |_|\n");
    
    loop1:printf("\nPlease enter the chemical formula of component #1: ");
    fflush(stdin); 
    gets(name1);
    if((reag1=find(name1,Reagent_List)) == NULL)
        {main();exit(1);} 
    printf("Please enter the chemical formula of component #2: ");
    fflush(stdin); 
    gets(name2);
    if((reag2=find(name2,Reagent_List)) == NULL)
        {main();exit(1);} 

    printf("Please enter the concentration of %s (mol/L): ",reag1->name);
    fflush(stdin); 
    scanf("%lf",&c1);
    printf("Please enter the concentration of %s (mol/L): ",reag2->name);
    fflush(stdin);
    scanf("%lf",&c2);
    
    // Discuss the relative strength and dose of the given acids/bases
    if(reag1->AB != reag2->AB)
    { 
        printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        printf("~~ Attention! Neutralization will occur! ~~\n");
        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

        if(reag1->AB == 'A')
            pa=reag1,pb=reag2; 
        else
            pa=reag2,pb=reag1; 
        if(fabs(c1-c2) < 1e-14)
        { 
            if(pa->SW=='W' && pb->SW=='W')// All acid and base are consumed in reaction and weak-acid weak-base salt is formed
                result=-(log10(pa->K1)+log10(Kw/pb->K1*c1+Kw)-log10(pa->K1+c1))/2;
            else if(pa->SW=='S' && pb->SW=='S')// All acid and base are consumed in reaction and strong-acid strong-base salt is formed
                if(strcmp(pa->name,"H2SO4"))// if not Sulfuric Acid 
                    result=7;
                else result=calculate_pH(c1,pa->K2,0,0,'0','A');
            else if(pa->SW=='W' && pa->K2 < 1e-3)
                result=calculate_pH(c1,pa->K1,0,0,'1','A');
            else if(pb->SW=='W' && pb->K2 < 1e-3)
                result=calculate_pH(c1,pa->K1,0,0,'1','B');
        } 
        else // swap pointers such that reag1 correspond to the excess reactant
        { 
            if(c1 < c2)
            {
                t=c1,c1=c2,c2=t;
                tp=reag1,reag1=reag2,reag2=tp;
            }
            if(reag1->SW=='S' && reag2->SW=='S')
                if(reag1->AB=='A' && reag1->AB=='B')
                    result=-log10(c1-c2);
                else result=14+log10(c1-c2);
            else if(reag1->SW=='S' && reag2->SW=='W')
                 if(reag1->AB=='A')
                     result=-log10(c1-c2);
                 else result=14+log10(c1-c2);
            else if(reag1->SW=='W' && reag2->SW=='S')
                if(reag1->AB=='A')
                     result=-log10(reag1->K1)-log10((c1-c2)/c2);
                else result=14+log10(reag1->K1)-log10(c2/(c1-c2));
            else
            {
                printf("\nWeak-acid weak-base unequal mixing is not solvable in this calculator!\n");
                system("pause");
                return 0;
            }   
        } 
    }
    else if(reag1->AB == 'A')// mix two acids
    {
         if(reag1->SW == reag2->SW)
             if(reag2->SW == 'W')// two weak acids
                 result=-0.5*log10(reag1->K1*c1+reag2->K1*c2);
             else // two strong acids, regardless of H2SO4 or not, H2SO4 is monoprotic by approx: S+A=S
                // if(strcmp(reag1->name,"H2SO4") && strcmp(reag2->name,"H2SO4"))// neither is not sulfuric acid
                //     result=-log10(c1+c2);
                // else if(strcmp(reag1->name,"H2SO4"))// reag2 is sulfuric acid
                //     result=-log10(c1+c2);//????
                // else // reag1 is sulfuric acid
                    result=-log10(c1+c2);
         else if(reag1->SW == 'S')// reag1 is a strong acid and reag2 is a weak acid
             if(strcmp(reag1->name,"H2SO4"))// reag1 is not sulfuric acid
                      result=-log10(c1);
             else result=mix_cal_H2SO4(reag1,reag2,c1,c2);//Reag1 is sulfuric acid
         else// reag1 is a weak acid and reag2 is a strong acid
             if(strcmp(reag2->name,"H2SO4"))// reag2 is not sulfuric acid
                      result=-log10(c2);
             else result=mix_cal_H2SO4(reag2,reag1,c2,c1);//Reag2 is sulfuric acid
    }
    else // mix two bases
    {
        if(reag1->SW == reag2->SW)
            if(reag2->SW == 'W')// two weak bases
                 result=14+0.5*log10(reag1->K1*c1+reag2->K1*c2);
            else result=14+log10(c1+c2);// two strong bases
        else if(reag1->SW == 'S')// reag1 is a strong base and reag2 is a weak base
             result=14+log10(c1);
        else// reag1 is a weak base and reag2 is a strong base
             result=14+log10(c2);
    }
    printf("pH=%.2f\n",result);
    
    system("pause");
    
     return 0; 
}

// Mixing sulfuric acid with another acid. Use bisection method to approximate the root
double mix_cal_H2SO4(Reagent *H2SO4,Reagent *acid,double c_H2SO4,double c)
{
       #define x(c,K,cH) (c*K/(cH+K)) 
       #define equation(cH,KS,K1,cS,c) (x(cS,KS,cH)+x(c,K1,cH)+Kw/cH-cH+cS) 
       double cH,a=1e-10,b=10,eps=1e-15;
       if(fabs(equation(a,H2SO4->K2,acid->K1,c_H2SO4,c)) < eps && a!=0)
           cH=a; 
       else if(fabs(equation(b,H2SO4->K2,acid->K1,c_H2SO4,c)) < eps)
           cH=b;
       else
       {
           cH=(a+b)/2; 
           while(fabs(equation(cH,H2SO4->K2,acid->K1,c_H2SO4,c)) > eps && fabs(a-b) > eps)
           {
            // printf("a=%e\tb=%e\tcH=%e\n",a,b,cH);
                if(equation(a,H2SO4->K2,acid->K1,c_H2SO4,c)*equation(cH,H2SO4->K2,acid->K1,c_H2SO4,c) < 0)
                     b=cH;
                else if(equation(b,H2SO4->K2,acid->K1,c_H2SO4,c)*equation(cH,H2SO4->K2,acid->K1,c_H2SO4,c) < 0)
                    a=cH;
                cH=(a+b)/2;
           }
       }
       // printf("a=%e\tb=%e\tcH=%e\n",a,b,cH);
       return -log10(cH); 
       #undef x
       #undef equation
}

// Buffer solution making guide
int buffer()
{
    int buffer_preparation();
    int buffer_calculate();
    
    char choice;
     
    system("color f0");
    system("cls");
    printf("\n");
    printf("______        __  __            _____       _       _   _             \n");
    printf("| ___ \\      / _|/ _|          /  ___|     | |     | | (_)            \n");
    printf("| |_/ /_   _| |_| |_ ___ _ __  \\ `--.  ___ | |_   _| |_ _  ___  _ __  \n");
    printf("| ___ \\ | | |  _|  _/ _ \\ '__|  `--. \\/ _ \\| | | | | __| |/ _ \\| '_ \\ \n");
    printf("| |_/ / |_| | | | ||  __/ |    /\\__/ / (_) | | |_| | |_| | (_) | | | |\n");
    printf("\\____/ \\__,_|_| |_| \\___|_|    \\____/ \\___/|_|\\__,_|\\__|_|\\___/|_| |_|\n");
    printf("\nWhat do you want to do?\n\n");
    printf("        1. Make a buffer solution\n\n");
    printf("        2. Calculate the pH of a buffer system\n\n");
    printf("        0. Return to last page\n");
        
           fflush(stdin); 
           scanf("%c",&choice) ;        
           switch (choice) 
           {
                     case '1': check(buffer_preparation());break;
                     case '2': check(buffer_calculate());break;
                     default:main();exit(1); 
           }
        return 0; 
}

// Select a reagent according to the given pH and print the required ratio
// Buffer selected using the (pK-1 < pH < pK+1) rule
int buffer_preparation() 
{
    double aim_pH,ratio;
    Reagent *p;
    p=Reagent_List; 
    int n=0; 
     
    system("cls"); 
    printf("\n\t\tPlease enter the desired buffer pH: ");
    fflush(stdin); 
    scanf("%lf",&aim_pH);
    putchar('\n'); 
    while(p!=NULL) 
    {
        if(fabs(p->K1) > 1e-10 && fabs(p->K2) < 1e-15)// Monoprotic acid 
            if((-log10(p->K1)+1 > aim_pH) && (-log10(p->K1)-1 < aim_pH))
            {
                if(p->AB == 'A' && (ratio=1+pow(10,-aim_pH)/p->K1)<10)
                {
                    printf("\tOption #%d: ",++n); 
                    printf("%s : NaOH = %.3f : 1\n\n",p->name,ratio);
                }
                if(p->AB == 'B' && (ratio=1+Kw*pow(10,aim_pH)/p->K1)<10)
                {
                    printf("\tOption #%d: ",++n);
                    printf("%s : HCl = %.3f : 1\n\n",p->name,ratio);
                } 
            }
            if(fabs(p->K2) > 1e-10)// Diprotic acid
            if((-log10(p->K2)+1 > aim_pH) && (-log10(p->K2)-1 < aim_pH) && strcmp(p->name,"H2SO4"))
            {
                 if(p->AB == 'A' && ((ratio=(2*pow(10,aim_pH)*p->K2+1)/(1+pow(10,aim_pH)*p->K2)) < 10)) 
                {
                    printf("\tOption #%d: ",++n); 
                    printf("%s : NaOH = 1 : %.3f\n\n",p->name,ratio);
                }
                if(p->AB == 'B' && ((ratio=(2*(p->K2/Kw/pow(10,aim_pH))+1)/(1+(p->K2/Kw/pow(10,aim_pH)))) < 10))
                { 
                    printf("\tOption #%d: ",++n);
                    printf("%s : HCl = %.3f : 1\n\n",p->name,ratio);
                } 
            }
            if(fabs(p->K3) > 1e-14)// Triprotic acid
            if((-log10(p->K3)+1 > aim_pH) && (-log10(p->K3)-1 < aim_pH))
            {
                 if(p->AB == 'A' && ((ratio=(3*p->K3*pow(10,aim_pH)+2)/(p->K3*pow(10,aim_pH)+1))) < 10) 
                {
                    printf("\tOption #%d: ",++n); 
                    printf("%s : NaOH = 1 : %.3f\n\n",p->name,ratio);
                }
                if(p->AB == 'B' && ((ratio=(3*p->K3/Kw/pow(10,aim_pH)+2)/(p->K3/Kw/pow(10,aim_pH)+1)) < 10)) 
                { 
                    printf("\tOption #%d: ",++n);
                    printf("%s : HCl = 1 : %.3f\n\n",p->name,ratio);
                } 
            }
                       
        
        p=p->next; 
    }
    if(n!=0)
        printf("\nThere are %d option(s) listted as above. Please choose according to your actual situation.\n\n",n);
    else
        printf("\nSorry, there is no reagent in the data base that matches your requirement.\n\n"); 
       system("pause");    

    return 0;
} 

// Calculate pH of a given buffer system using pH=pK+log(A-/HA)
int buffer_calculate()
{
    double ca,cb,ratio,result;
    Reagent *p=Reagent_List;
    char name[20]={0},choice,ji; 
     
    printf("Please enter the formula of the original acid or base of your buffer solution\n");
    printf("E.g., for NaAc-HAc buffer solution, enter HAc\n");
    fflush(stdin);
    gets(name);
    p=find(name,Reagent_List);
    if(p==NULL)
        main(),exit(1); 
    if(p->SW == 'S')
    { 
        printf("Error: strong acid or base alone cannot make a buffer system!\n");
        system("pause"); 
        return 0; 
    } 
    printf("Please enter the degree of dissociation of the substance with a higher degree of dissociation\n");
    printf("E.g., if you want to calculate Na3PO4-Na2HPO4 buffer solution, enter 2\n");
    printf("      if you want to calculate H2NC2H4NH2-H2NC2H4NH3Cl buffer solution, enter 0\n");
    fflush(stdin);
    scanf("%c",&ji);
    if(ji-'0' >= 3)
        printf("Invalid degree of dissociation!\n"); 
    printf("What else do you know about your buffer system?\n");
    printf("1. Ratio of concentration of the buffer pair\n");
    printf("2. Mole or concentration of the buffer pair\n");
    fflush(stdin);
    scanf("%c",&choice);
    switch (choice)
    {
           case '1': 
           {
               printf("Concentration of acid form : concentration of base form = ");
               fflush(stdin);
               scanf("%lf",&ratio);
               break; 
           }
           case '2': 
           {
               printf("Concentration or mole of acid form = ");
               fflush(stdin);
               scanf("%lf",&ca);
               printf("Concentration or mole of base form = ");
               fflush(stdin);
               scanf("%lf",&cb);
               ratio=ca/cb; 
               break; 
           }
           default : printf("Invalid command!\n");
    }
    if(p->AB == 'A')// acid
    {
        if(ji == '0') 
            result=-log10(p->K1)-log10(ratio);
        else if(ji == '1')
            result=-log10(p->K2)-log10(ratio);
        else if(ji == '2') 
            result=-log10(p->K3)-log10(ratio);
    } 
    else// base
    {
        if(ji == '0') 
            result=14+log10(p->K1)+log10(ratio);
        else if(ji == '1') 
            result=14+log10(p->K2)+log10(ratio);
        else if(ji == '2') 
            result=14+log10(p->K3)+log10(ratio);
    }
    printf("pH of this buffer solution is: %.2f\n",result);
    
    system("pause");
    
    return 0; 
            
} 

// add new reagent to database: modify linked list then write to file
int add_reagent()
{
    int from_keyboard(Reagent *head); 
    int from_file();
     
    int from_keyboard(Reagent *head);
    int from_file();
    
    char choice;
    
    system("cls");
    printf("\n");
    printf("  ___      _     _                                   _   \n");
    printf(" / _ \\    | |   | |                                 | |  \n");
    printf("/ /_\\ \\ __| | __| |  _ __ ___  __ _  __ _  ___ _ __ | |_ \n");
    printf("|  _  |/ _` |/ _` | | '__/ _ \\/ _` |/ _` |/ _ \\ '_ \\| __|\n");
    printf("| | | | (_| | (_| | | | |  __/ (_| | (_| |  __/ | | | |_ \n");
    printf("\\_| |_/\\__,_|\\__,_| |_|  \\___|\\__,_|\\__, |\\___|_| |_|\\__|\n");
    printf("                                     __/ |               \n");
    printf("                                    |___/                \n");
    printf("\n\n----------------------How do you want to add new reagent?--------------------------\n\n");
    printf("\t\t\t      >>>1. Enter manually\n\n\t\t\t      >>>2. Import from file\n");
    fflush(stdin);
    choice=getchar(); 
    switch (choice) 
    {
           case '1':check(from_keyboard(Reagent_List));break; 
           case '2':check(from_file());
    }
    return 0;
} 

// Manually enter new reagent. Interactive Q&A with formatting tips
int from_keyboard(Reagent *head)
{
    Reagent *new_item;
    char name[20]={0},choice;
    char new_AB,new_SW;
    double K1,K2,K3;
    
    check(!(new_item=(Reagent*)malloc(sizeof(Reagent))));
    printf("Please enter the chemical formula of your new reagent: ");
    fflush(stdin); 
    gets(name);
    if(find(name,Reagent_List))
    { 
        printf("This reagent is already included!\n");
        system("pause"); 
        return 1;
    }
    system("cls");
    printf("Please enter the chemical formula of your new reagent: "); 
    puts(name);
    printf("Is it an acid or a base?\nA---Acid    B---Base\n");
    fflush(stdin); 
    new_AB=getchar();
    if(new_AB != 'A' && new_AB != 'B')
    { 
        printf("\t( ¡¦_ ¡¦) \n\nInvalid command!\n");
        return 2; 
    }
    else if(new_AB=='B')
    { 
        printf("Can its OH- fully dissociate?\nS---Yes    W---No\n");
        fflush(stdin); 
        new_SW=getchar();
    }
    else
    { 
        printf("Can its H+ fully dissociate?\nS---Yes    W---No\n");
        fflush(stdin); 
        new_SW=getchar();
    }
       
    printf("Enter its dissociation coefficient of each order\n");
    printf("Tip: if it can fully dissociate, enter 9.99e99; if not applicable, enter 0\n");
    printf("First order dissociation coefficient is: ");
    fflush(stdin); 
    scanf("%lf",&K1);
    printf("Second order dissociation coefficient is: ");
    fflush(stdin); 
    scanf("%lf",&K2);
    printf("Third order dissociation coefficient is: ");
    fflush(stdin); 
    scanf("%lf",&K3);
    
    strcpy(new_item->name,name);
    new_item->AB=new_AB,new_item->SW=new_SW;
    new_item->K1=K1,new_item->K2=K2,new_item->K3=K3;
    while(head)
        head=head->next;
    head=new_item;
    new_item->next=NULL;
    
    system("cls"); 
    printf("Successfully added %s to the database!\n",new_item->name);
    if(new_item->SW == 'S') 
        printf("It is a strong ");
    else printf("It is a weak ");
    if(new_item->AB == 'A') 
        printf("acid. \n");
    else printf("base. \n");
    printf("Its first order dissociation coefficient K1 = %.3e\n",new_item->K1);
    printf("Its second order dissociation coefficient K2 = %.3e\n",new_item->K2); 
    printf("Its third order dissociation coefficient K3 = %.3e\n",new_item->K3);     
    
    system("pause"); 
    
    return 0; 
}

// Add new reagent from file
int from_file()
{
    FILE *fp_source,*fp_destination;
    char filename[20]={0},item[50]={0},namech;
    int i=0,j,n=0; 
    
    printf("Please enter the name of your new data file: ");
    fflush(stdin);
    gets(filename);
    if((fp_source=fopen(filename,"r+")) == NULL)
    { 
        printf("Fail to open the new data file!\n");
        system("pause");
        return 0;
    }
    if((fp_destination=fopen("list.k","a+")) == NULL)
    { 
        printf("Fail to open existing data file!\n");
        system("pause");
        return 0;
    }
    while((namech=getc(fp_source)) != EOF)
    {
        if(namech == '\n')
        { 
            i=0;
            fputs(item,fp_destination);
            for(j=0;j<50;j++)
                item[j]=0; 
            fputc('\n',fp_destination); 
            n++;
            continue;
        }
        item[i++]=namech;
    }
    fputs(item,fp_destination);
    fclose(fp_source);
    fclose(fp_destination);
    printf("Congratuations! You've successfully added %d new reagent(s)!",n);
    system("pause");
    return 0;
}

// manage reagent list
int list_mamage()
{
     int select();
     int revise();
     int sort_reag();
     int delete_reag();
     
     char choice;
     
     system("color f0"),system("cls");printf("\n");
     printf("______      _        _                    \n");
     printf("|  _  \\    | |      | |                   \n");
     printf("| | | |__ _| |_ __ _| |__   __ _ ___  ___ \n");
     printf("| | | / _` | __/ _` | '_ \\ / _` / __|/ _ \\\n");
     printf("| |/ / (_| | || (_| | |_) | (_| \\__ \\  __/\n");
     printf("|___/ \\__,_|\\__\\__,_|_.__/ \\__,_|___/\\___|\n");
     printf("\nPlease select a function to continue:\n\n");
     printf("1. Reagent lookup\n");
     printf("2. Modify reagent parameters\n");
     printf("3. Delete a reagent\n");
     printf("0. Return to last page\n");
     fflush(stdin); 
     scanf("%c",&choice);       
     switch (choice) 
     {
            case '1': check(select());break;
            case '2': check(revise());break;
            case '3': check(delete_reag());break;
            case '0': check(main());exit(1);
            default: printf("Invalid command!\n");system("pause"); 
     }
     
     return 0;
}

int select()
{
    int select_name();
    int view_category();
    int view_all();
    
    char choice;
    
    system("cls");
    printf("Please select a method for reagent lookup\n");
    printf("1. Lookup by name\n"); 
    printf("2. Browse by category\n");
    printf("3. Show all reagents\n");
    fflush(stdin); 
    scanf("%c",&choice);  
    switch (choice) 
     {
            case '1': check(select_name());break;
            case '2': check(view_category());break;
            case '3': check(view_all());break;
            default: printf("Invalid command!\n");system("pause"); 
     }
     return 0; 
}

// Find reagent by name and print to screen
int select_name()
{
    char name[20]={0},description[12]={0};
    Reagent *p=Reagent_List;
    int i; 
    
    printf("Please enter the chemical formula of the reagent:");
    fflush(stdin); 
    gets(name);
    while(p != NULL)
    {
        if(!strcmp(p->name,name))
        {
            printf("The properties of this reagent are listed below\n");
            printf("-----------------------------------------------------------------------------------\n");
            if(p->SW == 'S')
                strcat(description,"Strong ");
            else 
                strcat(description,"Weak "); 
            if(p->AB == 'A')
                strcat(description,"Acid");
            else 
                strcat(description,"Base");
            printf("Reagent Name\t\tK1\t\tK2\t\tK3\t\tDescription\n");
            printf("-----------------------------------------------------------------------------------\n");
            printf("%-24s%8.2e\t%8.2e\t%8.2e\t%s\n",p->name,p->K1,p->K2,p->K3,description);
            printf("-----------------------------------------------------------------------------------\n");
            for(i=0;i<12;i++) description[i]=0; 
            break; 
        }
        p=p->next;
    }
    if(p == NULL)
        printf("This reagent is not available in the database.\n\n");
            
        system("pause");
           
        return 0;
} 

// Print reagents by cateroty
int view_category() 
{
    char AB,SW,description[12]={0};
    Reagent *p=Reagent_List;
    
    printf("Which category do you want to check?\n");
    printf("Note: S=strong, W=weak, A=acid, B=base\n");
    printf("E.g., if you want to view all strong acids in the database, enter S-A\n");
    printf("Enter your category here:"); 
    fflush(stdin);
    SW=getchar(),getchar(),AB=getchar();
    if((AB != 'A' && AB != 'B') || (SW != 'S' && SW != 'W'))
    {
          printf("Invalid category keyword!\n");
          system("pause");
          check(select());
    }
    if(SW == 'S')
        strcat(description,"Strong ");
    else strcat(description,"Weak "); 
    if(AB == 'A')
        strcat(description,"Acid");
    else strcat(description,"Base");
    printf("The category you want to check is: %s\n",description);
    printf("-----------------------------------------------------------------------------------\n");
    printf("Reagent Name\t\tK1\t\tK2\t\tK3\t\tDescription\n");
    printf("-----------------------------------------------------------------------------------\n");
    while(p != NULL)
    { 
        if(p->AB == AB && p->SW == SW)
        { 
            printf("%-24s%8.2e\t%8.2e\t%8.2e\t%s\n",p->name,p->K1,p->K2,p->K3,description);
        } 
        p=p->next;
    }
    printf("-----------------------------------------------------------------------------------\n");

    system("pause");
     
    return 0; 
} 

// show all reagents in the database
int view_all()
{
    char description[12]={0};
    Reagent *p=Reagent_List;
    int i; 
    
    system("cls"); 
    printf("\n*************** Acid/Base Dissociation Coefficient Database ***************\n\n");
    printf("-----------------------------------------------------------------------------------\n");
    printf("Reagent Name\t\tK1\t\tK2\t\tK3\t\tDescription\n");
    printf("-----------------------------------------------------------------------------------\n");
    while(p != NULL)
    {
        if(p->SW == 'S')
            strcat(description,"Strong ");
        else strcat(description,"Weak "); 
        if(p->AB == 'A')
            strcat(description,"Acid");
        else strcat(description,"Base");
        printf("%-24s%8.2e\t%8.2e\t%8.2e\t%s\n",p->name,p->K1,p->K2,p->K3,description);
        p=p->next;
        for(i=0;i<12;i++) description[i]=0;
    }
    printf("-----------------------------------------------------------------------------------\n");
    system("pause");
    
    return 0;
}

// Modify specific entry in database
int revise()
{
    char name[20];
    Reagent *p=Reagent_List; 
    
    printf("Please enter the chemical formula of the reagent to modify:");
    fflush(stdin);
    gets(name);
    while(p != NULL)
        if(!strcmp(p->name,name))
            break;
        else
        {
            printf("This reagent is not available in the database!\n\n");
            system("pause");
            main();
            exit(1); 
        } 
    printf("Is it an acid or a base?\nA---Acid    B---Base\n");
    fflush(stdin); 
    p->AB=getchar();
    printf("Can it fully dissociate?\nS---Yes    W---No\n");
    fflush(stdin); 
    p->SW=getchar();
    printf("Enter its dissociation coefficient of each order\n");
    printf("Tip: if it can fully dissociate, enter 9.99e99; if not applicable, enter 0\n");
    printf("First order dissociation coefficient is: ");
    fflush(stdin); 
    scanf("%lf",&(p->K1));
    printf("´Second order dissociation coefficient is: ");
    fflush(stdin); 
    scanf("%lf",&(p->K2));
    printf("Third order dissociation coefficient is: ");
    fflush(stdin); 
    scanf("%lf",&(p->K3));
    system("cls"); 
    printf("Successfully revised reagent %s!\n",p->name);
    printf("It is a ");
    if(p->SW == 'S') 
        printf("strong ");
    else printf("weak ");
    if(p->AB == 'A')
        printf("acid.\n");
    else printf("base.\n");
    printf("Dissociation constants: K1 = %.3e;",p->K1);
    printf(" K2 = %.3e;",p->K2); 
    printf(" K3 = %.3e.",p->K3);     
    
    system("pause"); 
    
    return 0;
}

// Delete specific entry from database
int delete_reag()
{
    char name [20]={0};
    Reagent *p1=Reagent_List,*p2=NULL,*t=NULL;
     
    printf("Enter the name of the reagent you want to delete: "); 
    fflush(stdin);
    gets(name);
    if(!strcmp(Reagent_List->name,name))
    { 
        t=Reagent_List;
        Reagent_List=Reagent_List->next;
        free(t);
        printf("\nYou successfully deleted %s from the database!\n\n",name);
        system("pause"); 
        return 0; 
    } 
    while(p1->next != NULL)
    {
        if(!strcmp(p1->name,name))
            break;
        p2=p1,p1=p1->next;
    }
    if(p1->next != NULL)
    { 
        p2->next=p1->next;
        free(p1);
        printf("\nYou successfully deleted %s from the database!\n\n",name);
    } 
    else if(!strcmp(p1->name,name))// delete the last entry
    { 
        p2->next=NULL;
        free(p1);
        printf("\nYou successfully deleted %s from the database!\n\n",name);
    } 
    else
    {
        printf("Sorry, %s is not found in the database!\n",name);
        system("pause");
        main();
    } 
    system("pause");
    
    return 0; 
}

// user guide & help
int help_aboutme()
{
    int help();
    char choice;
    
    loop3:system("COLOR f0"),system("cls");
    printf("\n \t  # # # # # # # # # # # # # # # # # # # # # # # # # # # #\n");
    printf("\t  #   _   _                  _____       _     _        # \n");
    printf("\t  #  | | | |                |  __ \\     (_)   | |       # \n");
    printf("\t  #  | | | |___  ___ _ __   | |  \\/_   _ _  __| | ___   # \n");
    printf("\t  #  | | | / __|/ _ \\ '__|  | | __| | | | |/ _` |/ _ \\  # \n");
    printf("\t  #  | |_| \\__ \\  __/ |     | |_\\ \\ |_| | | (_| |  __/  # \n");
    printf("\t  #   \\___/|___/\\___|_|      \\____/\\__,_|_|\\__,_|\\___|  # \n");
    printf("\t  # # # # # # # # # # # # # # # # # # # # # # # # # # # #\n");
    printf("\n\n        What do you want to do?                         \n\n");
    printf("\t----->1. Read program introduction\n\n");
    printf("\t----->2. Learn more about me\n\n");
    printf("\t----->0. Return to last page\n");
    fflush(stdin);
    choice=getchar(); 
    switch (choice)
    {
           case '1':check(help());break; 
           case '2':
                {
                    system("cls"); printf("\n");
                    printf("         888    888       .d8888b.           888                   888          888                    \n");
                    printf("         888    888      d88P  Y88b          888                   888          888                    \n");
                    printf("         888    888      888    888          888                   888          888                    \n");
                    printf("88888b.  8888888888      888         8888b.  888  .d8888b 888  888 888  8888b.  888888 .d88b.  888d888 \n");
                    printf("888 \"88b 888    888      888            \"88b 888 d88P\"    888  888 888     \"88b 888   d88\"\"88b 888P\"   \n");
                    printf("888  888 888    888      888    888 .d888888 888 888      888  888 888 .d888888 888   888  888 888     \n");
                    printf("888 d88P 888    888      Y88b  d88P 888  888 888 Y88b.    Y88b 888 888 888  888 Y88b. Y88..88P 888     \n");
                    printf("88888P\"  888    888       \"Y8888P\"  \"Y888888 888  \"Y8888P  \"Y88888 888 \"Y888888  \"Y888 \"Y88P\"  888     \n");
                    printf("888                                                                                                    \n");
                    printf("888                                                                                                    \n");
                    printf("888                                                                                                    \n\n");
                    printf("Created on Tue Nov 17 2012\n");
                    printf("Author: Yacong (Brooke) Zhou\n");
                    printf("Contact: yacong.zhou@gmail.com\n");
                    system("pause");
                    break;
                } 
           case '0':main(),exit(1);break;
           default :printf("Invalid command!\n");system("pause");goto loop3;
    } 
    return 0;
}

int help()
{
    system("cls");
    printf("\n     This program contains 32 functions and 1 linked list.\n\n"); 
    printf("\t\t\tFunctions available:\n\n\n");
    printf("\t  1. Accurately calculate pH of common acid, base and salt solutions\n\n");
    printf("\t  2. Calculate pH of the mixture of two reagents from initial moles, concentrations or ratio\n\n");
    printf("\t  3. Generate buffer solution formula based on given pH targets\n\n");
    printf("\t  4. Lookup reagents in database and browse by category\n\n");
    printf("\t  5. Add, delete and modify chemical database\n\n\n");
    printf("       Tip: your changes are not saved until exit by command 0\n\n\n");
    system("pause");
    return 0;
}

// colse files, release memory and return to main
int bye()
{
    FILE *fp;
    char name[20],K[10],Ch;
    Reagent *head=Reagent_List,*p=NULL;
    int i; 
     
    if((fp=fopen("list.k","w")) == NULL)
    { 
        printf("Unable to open data file and your changes will be lost!\n");
        system("pause");
        while(head->next != NULL)
        {
            p=head; 
            head=head->next;
            free(p);
        }
        free(head);
        return 0;
    }
    
    while(head->next != NULL)
    {
        p=head;
        fputs(p->name,fp);
        for(i=20-strlen(p->name);i>0;i--)
            putc(' ',fp);
        doubletochar(p->K1,K);
        fputs(K,fp),putc(' ',fp);
        doubletochar(p->K2,K);
        fputs(K,fp),putc(' ',fp); 
        doubletochar(p->K3,K);
        fputs(K,fp); 
        putc(p->AB,fp),putc(p->SW,fp);
        putc('\n',fp);
        head=head->next;
        free(p);
    }
    fputs(head->name,fp);
    for(i=20-strlen(head->name);i>0;i--)
        putc(' ',fp);
    doubletochar(head->K1,K);
    fputs(K,fp),putc(' ',fp);
    doubletochar(head->K2,K);
    fputs(K,fp),putc(' ',fp);
    doubletochar(head->K3,K);
    fputs(K,fp);
    putc(head->AB,fp),putc(head->SW,fp);
    free(head);
    
    fclose(fp);
    system("cls");
    printf(" _____ _                 _            __                        _              \n");
    printf("|_   _| |               | |          / _|                      (_)             \n");
    printf("  | | | |__   __ _ _ __ | | _____   | |_ ___  _ __    _   _ ___ _ _ __   __ _  \n");
    printf("  | | | '_ \\ / _` | '_ \\| |/ / __|  |  _/ _ \\| '__|  | | | / __| | '_ \\ / _` | \n");
    printf("  | | | | | | (_| | | | |   <\\__ \\  | || (_) | |     | |_| \\__ \\ | | | | (_| | \n");
    printf("  \\_/ |_| |_|\\__,_|_| |_|_|\\_\\___/  |_| \\___/|_|      \\__,_|___/_|_| |_|\\__, | \n");
    printf("                                                                         __/ | \n");
    printf("                                                                        |___/  \n");
    printf("______                 _        _                                              \n");
    printf("| ___ \\               | |      ( )                                             \n");
    printf("| |_/ /_ __ ___   ___ | | _____|/ ___                                          \n");
    printf("| ___ \\ '__/ _ \\ / _ \\| |/ / _ \\ / __|                                         \n");
    printf("| |_/ / | | (_) | (_) |   <  __/ \\__ \\                                         \n");
    printf("\\____/|_|  \\___/ \\___/|_|\\_\\___| |___/                                         \n");
    printf("                                                                               \n");
    printf("                                                                               \n");
    printf("       _   _    _____       _            _       _               _             \n");
    printf("      | | | |  /  __ \\     | |          | |     | |             | |            \n");
    printf(" _ __ | |_| |  | /  \\/ __ _| | ___ _   _| | __ _| |_ ___  _ __  | |            \n");
    printf("| '_ \\|  _  |  | |    / _` | |/ __| | | | |/ _` | __/ _ \\| '__| | |            \n");
    printf("| |_) | | | |  | \\__/\\ (_| | | (__| |_| | | (_| | || (_) | |    |_|            \n");
    printf("| .__/\\_| |_/   \\____/\\__,_|_|\\___|\\__,_|_|\\__,_|\\__\\___/|_|    (_)            \n");
    printf("| |                                                                            \n");
    printf("|_|                                                                            \n");

    system("pause"); 
    return 0; 
}
