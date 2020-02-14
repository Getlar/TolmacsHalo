#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <ctype.h>

#define BUFFER1 1023
#define BUFFER2 255

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

char buffer1[BUFFER1];
char buffer2[BUFFER2];

int hanyadik = 0;
int nyelvswitch = 0;
char nyelv[7];

//Kilepo metodus. Ha valami hiba tortenik, bezarja a szervert
void kilepo(const char* hibauzenet){
  perror(hibauzenet);
  exit(5);
}
//Megcsinalja kliens oldalon a socketet
int socketCsinalo(){
  int kliens_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (kliens_socket < 0) {
    kilepo("Nem sikerult letrehozni a kliens socket-jet\n");
  }
  char on = 1;
  setsockopt(kliens_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof on);
  setsockopt(kliens_socket, SOL_SOCKET, SO_KEEPALIVE, (char *)&on, sizeof on);
  return kliens_socket;
}
//Racsatlakozik socket segitsegevel a szerverre
void szerverCsatlakozo(int kliens_socket, const char* loopbackCim, const char* port ){
  struct sockaddr_in szerverTulajd;
  memset(&szerverTulajd, '0', sizeof(szerverTulajd));
  szerverTulajd.sin_family = AF_INET;
  szerverTulajd.sin_addr.s_addr = inet_addr(loopbackCim);
  szerverTulajd.sin_port = htons(atoi(port));

  int csatlakozottE = connect(kliens_socket, (struct sockaddr *)&szerverTulajd, sizeof(szerverTulajd));
  if (csatlakozottE < 0) {
    kilepo("Nem sikerult csatlakozni a szerverhez");
  }
}
//Ellenorzi hogy valos uzenetet kuldunk es beolvas standard bemenetrol
bool uzenetOlvasoEllenorzo(char s[]){
  if (strcmp(nyelv,"magyar")==0) {
    printf("%skliens(%s): ",KCYN,s);
  }
  if (strcmp(nyelv,"MAGYAR")==0) {
    printf("%sKLIENS(%s): ",KCYN,s);
  }
  memset(&buffer1, '\0', BUFFER1);
  char atmenet[10000];
  fgets(atmenet, 9999, stdin);
  bool uzenetellenorzoNagy(const char* nyelv){
    for (int i = 0; i< strlen(atmenet); i++) {
      if (strcmp(nyelv,"magyar") == 0) {
        if (isupper(atmenet[i])) {
          return false;
        }
      }
      if (strcmp(nyelv,"MAGYAR") == 0) {
        if (islower(atmenet[i])) {
          return false;
        }
      }
    }
    return true;
  }
  while ((strlen(atmenet) > BUFFER1) || (uzenetellenorzoNagy(nyelv) == false)) {
    printf("%s-----------------------------------------------------------------------\n",KWHT);
    printf("%s|Tul hosszu az uzenet, vagy nem a te nyelveden irtad, kerlek irj ujat!|\n",KRED);
    printf("%s-----------------------------------------------------------------------\n",KWHT);
    printf("%sKliens(%s): ",KCYN,s);
    fgets(atmenet, 9999, stdin);
  }
  strcpy(buffer1,atmenet);
  if (strlen(buffer1) > 0) {
    return true;
  }
  return false;
}
//Ugyanaz mint az elozo csak 255 byte-ra limitalt
bool uzenetOlvasoEllenorzoKicsi(char s[]){
  if (strcmp(nyelv,"magyar")==0) {
    printf("%skliens(%s): ",KCYN,s);
  }
  if (strcmp(nyelv,"MAGYAR")==0) {
    printf("%sKLIENS(%s): ",KCYN,s);
  }
  memset(&buffer2, '\0', BUFFER2);
  char atmenet[10000];
  fgets(atmenet, 9999, stdin);
  bool uzenetellenorzo(const char* nyelv){
    for (int i = 0; i< strlen(atmenet); i++) {
      if (strcmp(nyelv,"magyar") == 0) {
        if (isupper(atmenet[i])) {
          return false;
        }
      }
      if (strcmp(nyelv,"MAGYAR") == 0) {
        if (islower(atmenet[i])) {
          return false;
        }
      }
    }
    return true;
  }
  while ((strlen(atmenet) > BUFFER2) || (uzenetellenorzo(nyelv) == false)) {
    printf("%s-----------------------------------------------------------------------\n",KWHT);
    printf("%s|Tul hosszu az uzenet, vagy nem a te nyelveden irtad, kerlek irj ujat!|\n",KRED);
    printf("%s-----------------------------------------------------------------------\n",KWHT);
    printf("%sKliens(%s): ",KCYN,s);
    fgets(atmenet, 9999, stdin);
  }
  strcpy(buffer2,atmenet);
  if (strlen(buffer2) > BUFFER2) {
    kilepo("Tul hosszu az uzenet\n");
  }
  if (strlen(buffer2) > 0) {
    return true;
  }
  return false;
}
// Megvizsgalja az uzenetet, elkuldi es visszaadja hogy mennyi ment at belole
void uzenetKuldes(int kliens_socket){
  int hossz = strlen(buffer1);
  int kuldes = send(kliens_socket, buffer1, hossz, 0);
  memset(&buffer1, '\0', BUFFER1);
  if (kuldes != hossz) {
    kilepo("Nem annyi byte ment at mint az eredeti bufferbe volt!\n");
  }
}
// Ugyanaz mint az elozo csak 255 byte-ra limitalt
void uzenetKuldesKicsi(int kliens_socket){
  int hossz = strlen(buffer2);
  int kuldes = send(kliens_socket, buffer2, hossz,0);
  memset(&buffer2, '\0', BUFFER2);
  if (kuldes != hossz) {
    kilepo("Nem annyi byte ment at mint az eredeti bufferbe volt!\n");
  }
}
// Kiuriti a buffert, kap egy uzenetet a bufferbe es Megvizsgalja hogy mennyi erkezett meg belole
bool uzenetKapo(int kliens_socket){
  memset(&buffer1, '\0', BUFFER1);
  int kapott = recv(kliens_socket, buffer1, BUFFER1,0);
  if (kapott < 0) {
    kilepo("Nem kaptunk uzenetet.\n");
  }
  if (kapott == 0) {
    return true;
  }
  return false;
}
//Ugyanaz mint az elozo csak 255 byte-ra limitalt
bool uzenetKapoKicsi(int kliens_socket){
  memset(&buffer2, '\0', BUFFER2);
  int kapott = recv(kliens_socket, buffer2, BUFFER2,0);
  if (kapott < 0) {
    kilepo("Nem kaptunk uzenetet.\n");
  }
  if (kapott == 0) {
    return true;
  }
  return false;
}

int main(int argc, char const *argv[]) {

  //Megvizsgalja ne legyen tobb paranccsori argumentum, csak loopback Cim
  if (argc != 3) {
    kilepo("Hibas bemeneti argumentumok!\n");
  }
  //Megcsinalja socketet es csatlakozik szerverhez
  int kliens_socket = socketCsinalo();
  szerverCsatlakozo(kliens_socket, argv[1], argv[2]);
  //Megkapja hogy hanyadiknak kapcsolodott ra a szerverre
  char micsoda[3];
  if (!uzenetKapo(kliens_socket)) {
    if (strcmp(buffer1,"1")==0) {
      strcpy(micsoda,buffer1);
      hanyadik = 1;
    }
    if (strcmp(buffer1,"2")==0) {
      strcpy(micsoda,buffer1);
      hanyadik = 2;
    }
  }
  // Elso kliens fogja a nyelvet diktalni
  if (hanyadik == 2) {
    printf("%s-------------- Varakozas a masik fel nyelvkiosztasara! --------------\n",KYEL);
  }
  int help = 1;
  if (hanyadik == 1) {
    printf("%sKerlek valassz egy nyelvet!\n",KMAG);
    printf("%sNyelv: ",KBLU);
    if(uzenetOlvasoEllenorzo(micsoda)){
      if ((strcmp(buffer1,"magyar\n")!=0) && (strcmp(buffer1,"MAGYAR\n")!=0)) {
        help = 2;
      }
      while (help == 2) {
        printf("%s-----------------------------------------------------\n",KWHT);
        printf("%s|Kerlek valassz *magyar* vagy *MAGYAR* nyelvek kozul|\n",KRED);
        printf("%s-----------------------------------------------------\n",KWHT);
        uzenetOlvasoEllenorzo(micsoda);
        if ((strcmp(buffer1,"magyar\n")!=0) && (strcmp(buffer1,"MAGYAR\n")!=0)) {
          help = 2;
        }
        else{
          help = 1;
        }
      }
      uzenetKuldes(kliens_socket);
    }
  }
  // Kliens megkapja milyen nyelvet beszel
  if (!uzenetKapo(kliens_socket)){
    if (strcmp(buffer1,"magyar")) {
      nyelvswitch = 1;
      strcpy(nyelv,"magyar");
    }
    if (strcmp(buffer1,"MAGYAR")) {
      nyelvswitch = 2;
      strcpy(nyelv,"MAGYAR");
    }
  }
  if (strcmp(nyelv,"MAGYAR")==0) {
    printf("%s-----------------------------------\n",KYEL);
    printf("%sSIKERESEN MEGKAPTAD A %s NYELVET\n",KMAG,nyelv);
    printf("%s-----------------------------------\n",KYEL);
    printf("%sELSO BESZELGETES!\n",KBLU);
  }
  if (strcmp(nyelv,"magyar")==0) {
    printf("%s-----------------------------------\n",KYEL);
    printf("%ssikeresen megkaptad a %s nyelvet\n",KMAG,nyelv);
    printf("%s-----------------------------------\n",KYEL);
    printf("%selso beszelgetes!\n",KBLU);
  }
  if ((hanyadik == 1) && (strcmp(nyelv,"magyar")==0)) {
    printf("%s-------------- varakozas a masik fel kezdesere! --------------\n",KYEL);
  }
  if ((hanyadik == 1) && (strcmp(nyelv,"MAGYAR")==0)) {
    printf("%s-------------- VARAKOZAS A MASIK FEL KEZDESERE! --------------\n",KYEL);
  }
  int loopszam = 0;
  //Miutan mindenki megkapta a nyelvet indul a beszelgetes
  while (1) {
    // Ez az if irja le hogy a masodik kliens mit kell csinaljon
    if ((loopszam != 0) && (strcmp(nyelv,"magyar")==0)) {
      printf("%s---------------------\n",KWHT);
      printf("%s|a konferencia folytatodik!|\n",KRED);
      printf("%s---------------------\n",KWHT);
      if (hanyadik == 1) {
        printf("%s-------------- varakozas a masik fel kezdesere! --------------\n",KYEL);
      }
    }
    if ((loopszam != 0) && (strcmp(nyelv,"MAGYAR")==0)) {
      printf("%s----------------------------\n",KWHT);
      printf("%s|A KONFERENCIA FOLYTATODIK!|\n",KRED);
      printf("%s----------------------------\n",KWHT);
      if (hanyadik == 1) {
        printf("%s-------------- VARAKOZAS A MASIK FEL KEZDESERE! --------------\n",KYEL);
      }
    }
    if ((hanyadik == 2) && (strcmp(nyelv,"magyar")==0)) {
      printf("%s-------------------------\n",KWHT);
      printf("%ste kezded a felszolalast!\n",KGRN);
      printf("%s-------------------------\n",KWHT);
      if(uzenetOlvasoEllenorzo(micsoda)){
        uzenetKuldes(kliens_socket);
      }
      uzenetKapoKicsi(kliens_socket);
      printf("%s---------------------------------------\n",KWHT);
      printf("%sa masik fel reagalasa a kovetkezo volt:\n",KGRN);
      printf("%s---------------------------------------\n",KWHT);
      printf("%skliens(1): %s",KMAG,buffer2);
      printf("%s------------------------------------------\n",KWHT);
      uzenetKapo(kliens_socket);
      printf("%sa masik fel felszolalasa a kovetkezo volt:\n",KGRN);
      printf("%s------------------------------------------\n",KWHT);
      printf("%skliens(1): %s",KMAG,buffer1);
      printf("%s----------------------------\n",KWHT);
      printf("%ste kovetkezel a reagalassal!\n",KGRN);
      if(uzenetOlvasoEllenorzoKicsi(micsoda)){
        uzenetKuldesKicsi(kliens_socket);
      }
      //uzenetKapoKicsi(kliens_socket);
      printf("\n");
      //printf("%s\n",buffer2);
      printf("%s---------\n",KWHT);
      printf("%sszavazas!\n",KRED);
      printf("%s---------\n",KWHT);
      printf("\n");
      if(uzenetOlvasoEllenorzoKicsi(micsoda)){
        uzenetKuldesKicsi(kliens_socket);
      }
    }
    if ((hanyadik == 2) && (strcmp(nyelv,"MAGYAR")==0)) {
      printf("%s-------------------------\n",KWHT);
      printf("%sTE KEZDED A FELSZOLALAST!\n",KGRN);
      printf("%s-------------------------\n",KWHT);
      if(uzenetOlvasoEllenorzo(micsoda)){
        uzenetKuldes(kliens_socket);
      }
      uzenetKapoKicsi(kliens_socket);
      printf("%s---------------------------------------\n",KWHT);
      printf("%sA MASIK FEL REAGALASA A KOVETKEZO VOLT:\n",KGRN);
      printf("%s---------------------------------------\n",KWHT);
      printf("%sKLIENS(1): %s",KMAG,buffer2);
      uzenetKapo(kliens_socket);
      printf("%s------------------------------------------\n",KWHT);
      printf("%sA MASIK FEL FELSZOLALASA A KOVETKEZO VOLT:\n",KGRN);
      printf("%s------------------------------------------\n",KWHT);
      printf("%sKLIENS(1): %s",KMAG,buffer1);
      printf("%s----------------------------\n",KWHT);
      printf("%sTE KOVETKEZEL A REAGALASSAL!\n",KGRN);
      printf("%s----------------------------\n",KWHT);
      if(uzenetOlvasoEllenorzoKicsi(micsoda)){
        uzenetKuldesKicsi(kliens_socket);
      }
      //uzenetKapoKicsi(kliens_socket);
      printf("\n");
      //printf("%s\n",buffer2);
      printf("%s---------\n",KWHT);
      printf("%sSZAVAZAS!\n",KRED);
      printf("%s---------\n",KWHT);
      printf("\n");
      if(uzenetOlvasoEllenorzoKicsi(micsoda)){
        uzenetKuldesKicsi(kliens_socket);
      }
    }
    // Ez az if irja le hogy az elso kliens mit kell csinaljon
    if ((hanyadik == 1) && (strcmp(nyelv,"magyar")==0)) {
      uzenetKapo(kliens_socket);
      printf("%s------------------------------------------\n",KWHT);
      printf("%sa masik fel felszolalasa a kovetkezo volt:\n",KGRN);
      printf("%s------------------------------------------\n",KWHT);
      printf("%skliens(2): %s",KMAG,buffer1);
      printf("%s----------------------------\n",KWHT);
      printf("%ste kovetkezel a reagalassal!\n",KGRN);
      printf("%s----------------------------\n",KWHT);
      if(uzenetOlvasoEllenorzoKicsi(micsoda)){
        uzenetKuldesKicsi(kliens_socket);
      }
      printf("%s-------------------------------\n",KWHT);
      printf("%ste kovetkezel a felszolalassal!\n",KGRN);
      printf("%s-------------------------------\n",KWHT);
      if(uzenetOlvasoEllenorzo(micsoda)){
        uzenetKuldes(kliens_socket);
      }
      uzenetKapoKicsi(kliens_socket);
      printf("%s---------------------------------------\n",KWHT);
      printf("%sa masik fel reagalasa a kovetkezo volt:\n",KGRN);
      printf("%s---------------------------------------\n",KWHT);
      printf("%skliens(2): %s",KMAG,buffer2);
      //uzenetKapoKicsi(kliens_socket);
      //printf("%s\n",buffer2);
      printf("%s---------\n",KWHT);
      printf("%sszavazas!\n",KRED);
      printf("%s---------\n",KWHT);
      printf("\n");
      if(uzenetOlvasoEllenorzoKicsi(micsoda)){
        uzenetKuldesKicsi(kliens_socket);
      }
    }
    if ((hanyadik == 1) && (strcmp(nyelv,"MAGYAR")==0)) {
      uzenetKapo(kliens_socket);
      printf("%s------------------------------------------\n",KWHT);
      printf("%sA MASIK FEL FELSZOLALASA A KOVETKEZO VOLT:\n",KGRN);
      printf("%s------------------------------------------\n",KWHT);
      printf("%sKLIENS(2): %s",KMAG,buffer1);
      printf("%s----------------------------\n",KWHT);
      printf("%sTE KOVETKEZEL A REAGALASSAL!\n",KGRN);
      printf("%s----------------------------\n",KWHT);
      if(uzenetOlvasoEllenorzoKicsi(micsoda)){
        uzenetKuldesKicsi(kliens_socket);
      }
      printf("%s-------------------------------\n",KWHT);
      printf("%sTE KOVETKEZEL A FELSZOLALASSAL!\n",KGRN);
      printf("%s-------------------------------\n",KWHT);
      if(uzenetOlvasoEllenorzo(micsoda)){
        uzenetKuldes(kliens_socket);
      }
      uzenetKapoKicsi(kliens_socket);
      printf("%s---------------------------------------\n",KWHT);
      printf("%sA MASIK FEL REAGALASA A KOVETKEZO VOLT:\n",KGRN);
      printf("%s---------------------------------------\n",KWHT);
      printf("%sKLIENS(2): %s",KMAG,buffer2);
      //uzenetKapoKicsi(kliens_socket);
      //printf("%s\n",buffer2);
      printf("%s---------\n",KWHT);
      printf("%sSZAVAZAS!\n",KRED);
      printf("%s---------\n",KWHT);
      printf("\n");
      if(uzenetOlvasoEllenorzoKicsi(micsoda)){
        uzenetKuldesKicsi(kliens_socket);
      }
    }
    loopszam ++;
    //Q-t kap ha megegyeztek, mast ha nem, Q-ra kilep es becsukja socketet, kliens leall
    uzenetKapo(kliens_socket);
    if (strcmp(buffer1,"Q")==0) {
      break;
    }
    buffer1[0] = '\0';
  }
  close(kliens_socket);

  return 0;
}
