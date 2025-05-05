#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_CARDS 100

typedef struct {
    char numar[20];
    char nume[50];
    char dataExpirare[10];
    char cvv[10];
    int vizualizari;
} Card;

Card wallet[MAX_CARDS];
int numarCarduri = 0;

void clsScreen() {
    system("cls");
}

void pause() {
    printf("\nApasa Enter pentru a continua...");
    getchar();
}

void afiseaza_meniu() {
    printf("-= Sistem Wallet Electronic =-\n");
    printf("1. Adauga un card nou\n");
    printf("2. Vizualizeaza cardurile existente\n");
    printf("3. Sterge un card\n");
    printf("4. Iesire\n");
}

void salveaza_in_fisier(const char path[]) {
    FILE *f = fopen(path, "w");
    if (!f) {
        printf("Eroare la deschiderea fisierului!\n");
        return;
    }
    fprintf(f, "%d\n", numarCarduri);
    for (int i = 0; i < numarCarduri; i++) {
        fprintf(f, "%s\n%s\n%s\n%s\n%d\n",
                wallet[i].numar, wallet[i].nume,
                wallet[i].dataExpirare, wallet[i].cvv,
                wallet[i].vizualizari);
    }
    fclose(f);
}

void citire_din_fisier(const char path[]) {
    FILE *f = fopen(path, "r");
    if (!f) {
        printf("(Fisier inexistent, va fi creat la prima salvare.)\n");
        return;
    }
    fscanf(f, "%d\n", &numarCarduri);
    for (int i = 0; i < numarCarduri; i++) {
        fgets(wallet[i].numar, sizeof(wallet[i].numar), f);
        fgets(wallet[i].nume, sizeof(wallet[i].nume), f);
        fgets(wallet[i].dataExpirare, sizeof(wallet[i].dataExpirare), f);
        fgets(wallet[i].cvv, sizeof(wallet[i].cvv), f);

        char buffer[20];
        fgets(buffer, sizeof(buffer), f);
        wallet[i].vizualizari = atoi(buffer);
        wallet[i].numar[strcspn(wallet[i].numar, "\n")] = 0;
        wallet[i].nume[strcspn(wallet[i].nume, "\n")] = 0;
        wallet[i].dataExpirare[strcspn(wallet[i].dataExpirare, "\n")] = 0;
        wallet[i].cvv[strcspn(wallet[i].cvv, "\n")] = 0;
    }
    fclose(f);
}


void citeste_string(const char* mesaj, char* dest, int maxLen) {
    printf("%s", mesaj);
    fgets(dest, maxLen, stdin);
    dest[strcspn(dest, "\n")] = 0;
}

void citeste_numar_card(char numar[]) {
    int ok = 0;
    while (!ok) {
        citeste_string("Introdu numarul cardului (16 cifre, fara spatii): ", numar, 20);

        if (strlen(numar) != 16) {
            printf("Numarul cardului trebuie sa aiba exact 16 caractere.\n");
            continue;
        }

        ok = 1;
        for (int i = 0; i < 16; i++) {
            if (!isdigit(numar[i])) {
                ok = 0;
                if (isspace(numar[i])) {
                    printf("Numarul cardului nu trebuie sa contina spatii.\n");
                } else {
                    printf("Numarul cardului trebuie sa contina doar cifre.\n");
                }
                break;
            }
        }
    }
}


void citeste_nume_card(char nume[]) {
    citeste_string("Introdu numele posesorului cardului: ", nume, 50);
}


void citeste_data_expirare(char data[]) {
    int ok = 0;
    while (!ok) {
        printf("Introdu data expirarii cardului (MM/YY): ");
        scanf("%s", data);
        getchar();

        if (strlen(data) == 5 && data[2] == '/' &&
            isdigit(data[0]) && isdigit(data[1]) &&
            isdigit(data[3]) && isdigit(data[4])) {

            int luna = (data[0] - '0') * 10 + (data[1] - '0');
            int an = (data[3] - '0') * 10 + (data[4] - '0');

            time_t t = time(NULL);
            struct tm *now = localtime(&t);
            int luna_curenta = now->tm_mon + 1;
            int an_curent = (now->tm_year + 1900) % 100;

            int luna_valida = (luna >= 1 && luna <= 12);
            int data_in_viitor = (an > an_curent) || (an == an_curent && luna >= luna_curenta);

            if (!luna_valida || !data_in_viitor) {
                if (!luna_valida) {
                    printf("Luna invalida! Trebuie sa fie intre 01 si 12.\n");
                }
                if (!data_in_viitor) {
                    printf("Data introdusa este expirata! Introdu o data din viitor.\n");
                }
            } else {
                ok = 1;
            }
        } else {
            printf("Format invalid! Te rog sa folosesti formatul MM/YY.\n");
        }
    }
}



void citeste_cvv(char cvv[]) {
    int ok = 0;
    while (!ok) {
        citeste_string("Introdu codul CVV (3 cifre): ", cvv, 10);
        if (strlen(cvv) == 3 && isdigit(cvv[0]) && isdigit(cvv[1]) && isdigit(cvv[2])) {
            ok = 1;
        } else {
            printf("CVV invalid! Trebuie sa fie exact 3 cifre.\n");
        }
    }
}

void adauga_card(const char path[]) {
    clsScreen();
    printf("Adauga un card nou\n");

    if (numarCarduri >= MAX_CARDS) {
        printf("Portofelul este plin!\n");
        return;
    }

    int n = numarCarduri;
    citeste_numar_card(wallet[n].numar);
    citeste_nume_card(wallet[n].nume);
    citeste_data_expirare(wallet[n].dataExpirare);
    citeste_cvv(wallet[n].cvv);
    wallet[n].vizualizari = 0;
    numarCarduri++;

    salveaza_in_fisier(path);

    printf("Card adaugat cu succes!\n");
}

int compare(const void *a, const void *b) {
    return ((Card*)b)->vizualizari - ((Card*)a)->vizualizari;
}

void vizualizeaza_carduri() {
    clsScreen();
    printf("=== Vizualizare Carduri ===\n\n");

    if (numarCarduri == 0) {
        printf("Nu exista carduri in portofel.\n");
        return;
    }

    qsort(wallet, numarCarduri, sizeof(Card), compare);

    printf("Lista cardurilor:\n");
    printf("---------------------------------------------\n");
    for (int i = 0; i < numarCarduri; i++) {
        printf("%d) \033[0;91m%s\033[0m - \033[0;33m%s\033[0m (Expira: \033[0;32m%s\033[0m, CVV: \033[0;31m%s\033[0m, Vizualizari: %d)\n",
            i + 1,
            wallet[i].numar,
            wallet[i].nume,
            wallet[i].dataExpirare,
            wallet[i].cvv,
            wallet[i].vizualizari);
    }
    printf("---------------------------------------------\n\n");

    int index;
    printf("Alege un card pentru detalii (0 pentru a reveni): ");
    scanf("%d", &index);
    getchar();

    if (index > 0 && index <= numarCarduri) {
        wallet[index - 1].vizualizari++;

        salveaza_in_fisier("fisier.txt");

        clsScreen();

        printf("=== Detalii Card ===\n\n");
        printf("Numar: \033[0;91m%s\033[0m\n", wallet[index - 1].numar);
        printf("Nume: \033[0;33m%s\033[0m\n", wallet[index - 1].nume);
        printf("Data Expirare: \033[0;32m%s\033[0m\n", wallet[index - 1].dataExpirare);
        printf("CVV: \033[0;31m%s\033[0m\n", wallet[index - 1].cvv);
        printf("Vizualizari: %d\n\n", wallet[index - 1].vizualizari);
    }
}



void sterge_card(const char path[]) {
    clsScreen();
    printf("Sterge un card\n");

    if (numarCarduri == 0) {
        printf("Nu exista carduri de sters.\n");
        return;
    }

    for (int i = 0; i < numarCarduri; i++) {
    printf("%d) \033[0;91m%s\033[0m - \033[0;33m%s\033[0m (Expira: \033[0;32m%s\033[0m, CVV: \033[0;31m%s\033[0m, Vizualizari: %d)\n",
        i + 1,
        wallet[i].numar,
        wallet[i].nume,
        wallet[i].dataExpirare,
        wallet[i].cvv,
        wallet[i].vizualizari);
}


    int index;
    printf("Alege un card pentru stergere (1-%d, 0 pentru a reveni): ", numarCarduri);
    scanf("%d", &index);
    getchar();

    if (index > 0 && index <= numarCarduri) {
        for (int i = index - 1; i < numarCarduri - 1; i++) {
            wallet[i] = wallet[i + 1];
        }
        numarCarduri--;
        salveaza_in_fisier(path);
        printf("Card sters cu succes!\n");
    } else if (index == 0) {
        printf("Anulat.\n");
    } else {
        printf("Index invalid.\n");
    }
}

void proceseaza_optiune(int optiune, const char path[]) {
    switch (optiune) {
        case 1: adauga_card(path); break;
        case 2: vizualizeaza_carduri(); break;
        case 3: sterge_card(path); break;
        case 4: printf("La revedere!\n"); exit(0);
        default: printf("Optiune invalida. Reincearca.\n");
    }
}

int main() {
    int optiune;
    const char path[] = "fisier.txt";

    citire_din_fisier(path);

    while (1) {
        clsScreen();
        afiseaza_meniu();
        printf("Alege o optiune: ");
        scanf("%d", &optiune);
        getchar();
        clsScreen();
        proceseaza_optiune(optiune, path);
        pause();
    }

    return 0;
}
