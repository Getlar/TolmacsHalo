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

char buffer1[BUFFER1];
char buffer2[BUFFER2];
int helper;
//Kilepo metodus. Ha valami hiba tortenik, bezarja a szervert
void kilepo(const char* hibauzenet){
  perror(hibauzenet);
  exit(5);
}
//Megcsinalja a szerver socketet, bindolja es elkezd hallgatni
int szerverSocketCsinalo(const char* port){
  struct sockaddr_in szerverTulajd;
  memset(&szerverTulajd, '0', sizeof(szerverTulajd));
  szerverTulajd.sin_family = AF_INET;
  szerverTulajd.sin_addr.s_addr = INADDR_ANY;
  szerverTulajd.sin_port = htons(atoi(port));

  int szerver_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (szerver_socket < 0) {
    kilepo("Nem sikerult szerver socketet letrehozni!\n");
  }
  char on = 1;
  setsockopt(szerver_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof on);
  setsockopt(szerver_socket, SOL_SOCKET, SO_KEEPALIVE, (char *)&on, sizeof on);
  int bindEredmeny = bind(szerver_socket, (struct sockaddr*)& szerverTulajd, sizeof(szerverTulajd));
  if (bindEredmeny < 0) {
    kilepo("Nem sikerult bind-olni szerver socketet.\n");
  }
  int listenEredmeny = listen(szerver_socket, 10);
  if (listenEredmeny < 0) {
    kilepo("Nem sikerult hallgatni a szerver sockettel.\n");
  }
  return szerver_socket;
}
//Elfogadja a kliens altali csatlakozast
int elfogadas(int szerver_socket){
  struct sockaddr_in kliensTulajd;
  int klienshossz = sizeof(kliensTulajd);
  int kliens_socket = accept(szerver_socket, (struct sockaddr*)& kliensTulajd,&klienshossz );
  if (kliens_socket < 0) {
    kilepo("Nem sikerult elfogadni az osszekottetest.\n");
  }
  return kliens_socket;
}
//Kiuriti a buffert es megkapja az uzenetet klienstol, majd ellenorzi, mennyi jott at belole
int uzenet_kapas(int kliens_socket){
  memset(&buffer1, '\0', BUFFER1);
  int kapott = recv(kliens_socket, buffer1, BUFFER1,0);
  if (kapott < 0) {
    kilepo("Nem sikerult uzenetet kapni.\n");
  }
  return kapott;
}
//Ugyanaz mint az elozo csak 255 byte-ra limitalt
int uzenet_kapasKicsi(int kliens_socket){
  memset(&buffer2, '\0', BUFFER2);
  int kapott = recv(kliens_socket, buffer2, BUFFER2,0);
  if ( kapott < 0 ) {

    kilepo("Nem sikerult uzenetet kapni.\n");
  }
  return kapott;
}
//Tovabbkuldi a buffer tartalmat a kliensnek akivel eppen kommunikal
void uzenetKuldes(int kliens_socket){
  int uzenethossz = strlen(buffer1);
  int kuldottbyte = send(kliens_socket, buffer1, uzenethossz,0);
  memset(&buffer1, '\0', BUFFER1);
  if (kuldottbyte != uzenethossz) {
    kilepo("Nem ugyanannyi byte ment ki mint buffer.\n");
    }
}
//Ugyanaz mint az elozo csak 255 byte-ra limitalt
void uzenetKuldesKicsi(int kliens_socket){
  int uzenethossz = strlen(buffer2);
  if (uzenethossz > BUFFER2) {
    kilepo("Tul hosszu volt az uzenet.\n");
  }
  int kuldottbyte = send(kliens_socket, buffer2, uzenethossz,0);
  memset(&buffer2, '\0', BUFFER2);
  if (kuldottbyte != uzenethossz) {
    kilepo("Nem ugyanannyi byte ment ki mint buffer.\n");
    }
}
//Metodus ami buffert forditja magyar-rol MAGYAR-ra
void forditonagyra(char t[]){
  for (int i = 0; i < strlen(t); i++) {
    buffer1[i] = toupper(t[i]);
  }
  for (int i = 0; i < strlen(t); i++) {
    buffer2[i] = toupper(t[i]);
  }
}
//Metodus ami buffert forditja MAGYAR-rol magyar-ra
void forditokicsire(char t[]){
  for (int i = 0; i < strlen(t); i++) {
    buffer1[i] = tolower(t[i]);
  }
  for (int i = 0; i < strlen(t); i++) {
    buffer2[i] = tolower(t[i]);
  }
}

int main(int argc, char const *argv[]) {
  if (argc != 2) {
    kilepo("Hibas bemeneti argumentumok!\n");
  }
  //Megcsinaljuk szerver socket es elfogadjuk a ket klienst
  int szerver_socket = szerverSocketCsinalo(argv[1]);
  int kliens_socket1 = elfogadas(szerver_socket);
  printf("Kliens(1) csatlakozott a szerverhez!\n");
  printf("-------------- Varakozas a masik felre! --------------\n");
  int kliens_socket2 = elfogadas(szerver_socket);
  printf("Kliens(2) csatlakozott a szerverhez!\n");
  printf("------------------------------------\n");
  printf("Kliens(1), valassz egy nyelvet!\n");
  //Atkuldjuk hogy ki hanyadik
  strcpy(buffer1, "1");
  uzenetKuldes(kliens_socket1);
  strcpy(buffer1, "2");
  uzenetKuldes(kliens_socket2);
  //Kiiratjuk alapallapotokat
  int kapo = uzenet_kapas(kliens_socket1);
  if (kapo == 0) {
    close(kliens_socket1);
    close(kliens_socket2);
    close(szerver_socket);
  }
  char magyar[] = "magyar\n";
  char magyar2[] = "MAGYAR\n";
  printf("-----------------------------------\n");
  if (strcmp(buffer1,magyar)==0) {
    printf("Kliens(1): magyar.\n");
    printf("Kliens(2): MAGYAR.\n");
    helper = 1;
  }
  if (strcmp(buffer1,magyar2)==0) {
    printf("Kliens(1): MAGYAR.\n");
    printf("Kliens(2): magyar.\n");
    helper = 2;
  }
  printf("-----------------------------------\n");
  //Ha a "helper" 1 erteket kap akkor az elso kliensnek megmondja hogy o magyar es a masodiknak hogy o MAGYAR
  if (helper == 1) {
    buffer1[0] = '\0';
    strcpy(buffer1,"MAGYAR");
    uzenetKuldes(kliens_socket1);
    buffer1[0] = '\0';
    strcpy(buffer1,"magyar");
    uzenetKuldes(kliens_socket2);
  }
  if (helper == 2) {
    buffer1[0] = '\0';
    strcpy(buffer1,"magyar");
    uzenetKuldes(kliens_socket1);
    buffer1[0] = '\0';
    strcpy(buffer1,"MAGYAR");
    uzenetKuldes(kliens_socket2);
  }
  //szamlalo ami azt nezi hanyszor indult ujra a beszelgetes
  int i = 0;
  //beszelgeto ciklus
  while (1) {
    char szavazat[BUFFER2];
    char szavazat2[BUFFER2];
    if (i == 0) {
      printf("Kezdodhet a konferencia!\n");
      printf("Kliens(2) fogja kezdeni, mivel valasztasi lehetosege nem sok volt.\n");
    }
    else{
      printf("Konferencia folytatodik a Kliens(2) oldalan!\n");
    }
    // Az az eset amikor Kliens1 magyar es Kliens2 MAGYAR
    if (helper == 1) {
      uzenet_kapas(kliens_socket2);
      forditokicsire(buffer1);
      uzenetKuldes(kliens_socket1);
      printf("Kliens(1) reagalhat!\n");
      uzenet_kapasKicsi(kliens_socket1);
      forditonagyra(buffer2);
      uzenetKuldesKicsi(kliens_socket2);
      printf("Kliens(1) kovetkezik a felszolalassal!\n");
      uzenet_kapas(kliens_socket1);
      forditonagyra(buffer1);
      uzenetKuldes(kliens_socket2);
      printf("Kliens(2) reagalhat!\n");
      uzenet_kapasKicsi(kliens_socket2);
      forditokicsire(buffer2);
      uzenetKuldesKicsi(kliens_socket1);
      printf("Johetnek a valasztasok!\n");
      printf("Kliens(1) fogja kezdeni!\n");
      //strcpy(buffer2,"Te kezded. Add le a szavazatodat!");
      //forditokicsire(buffer2);
      //uzenetKuldesKicsi(kliens_socket1);
      uzenet_kapasKicsi(kliens_socket1);
      strcpy(szavazat,buffer2);
      //strcpy(buffer2,"Add le a szavazatodat te is!");
      //forditonagyra(buffer2);
      //uzenetKuldesKicsi(kliens_socket2);
      uzenet_kapasKicsi(kliens_socket2);
      forditokicsire(buffer2);
      strcpy(szavazat2,buffer2);
    }
    // Az az eset amikor Kliens1 MAGYAR es kliens2 magyar
    if (helper == 2) {
      uzenet_kapas(kliens_socket2);
      forditonagyra(buffer1);
      uzenetKuldes(kliens_socket1);
      printf("Kliens(1) reagalhat!\n");
      uzenet_kapasKicsi(kliens_socket1);
      forditokicsire(buffer2);
      uzenetKuldesKicsi(kliens_socket2);
      printf("Kliens(1) kovetkezik a felszolalassal!\n");
      uzenet_kapas(kliens_socket1);
      forditokicsire(buffer1);
      uzenetKuldes(kliens_socket2);
      printf("Kliens(2) reagalhat!\n");
      uzenet_kapasKicsi(kliens_socket2);
      forditonagyra(buffer2);
      uzenetKuldesKicsi(kliens_socket1);
      printf("Johetnek a valasztasok!\n");
      printf("Kliens(1) fogja kezdeni!\n");;
      //strcpy(buffer2,"Te kezded. Add le a szavazatodat!");
      //forditonagyra(buffer2);
      //uzenetKuldesKicsi(kliens_socket1);
      uzenet_kapasKicsi(kliens_socket1);
      forditokicsire(buffer2);
      strcpy(szavazat,buffer2);
      //strcpy(buffer2,"Add le a szavazatodat te is!");
      //forditokicsire(buffer2);
      //uzenetKuldesKicsi(kliens_socket2);
      uzenet_kapasKicsi(kliens_socket2);
      forditokicsire(buffer2);
      strcpy(szavazat2,buffer2);
    }
    //Megvizsgaljuk szavazat, ha mindketto yes akkor elkuldjuk klienseknek alljanak le es szerver leall
    if ((strcmp(szavazat,szavazat2)==0) && (strcmp(szavazat,"igen\n")==0)) {
      printf("Beleegyezes megtortent! Konferencianak vege!\n");
      buffer1[0] = '\0';
      strcpy(buffer1,"Q");
      uzenetKuldes(kliens_socket1);
      strcpy(szavazat,buffer1);
      buffer1[0] = '\0';
      strcpy(buffer1,"Q");
      uzenetKuldes(kliens_socket2);
      break;
    }
    //Ha nem mindketto yes akkor atkuldunk egy szemetet es megy tovabb a ciklus amig meg nem egyeznek
    else{
      buffer1[0] = '\0';
      strcpy(buffer1,"Z");
      uzenetKuldes(kliens_socket1);
      strcpy(szavazat,buffer1);
      buffer1[0] = '\0';
      strcpy(buffer1,"Z");
      uzenetKuldes(kliens_socket2);
    }
    i++;
  }
  //Becsukjuk mindharom socketet
  close(kliens_socket2);
  close(kliens_socket1);
  close(szerver_socket);
  printf("Szerver Bezarva\n");
  return 0;
}
