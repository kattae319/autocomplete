#include "autocomplete.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int compare_lexicographic(const void *a, const void *b) {
    const char *s1 = ((struct term *)a)->term;
    const char *s2 = ((struct term *)b)->term;
    return strcmp(s1, s2);
}

int compare_weight(const void *a, const void *b) {
    double w1 = ((struct term *)a)->weight;
    double w2 = ((struct term *)b)->weight;
    return (w1 < w2) - (w1 > w2);
}

void read_in_terms(struct term **terms, int *pnterms, char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening file");
        exit(1);
    }

    char line[512];
    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return;
    }
    *pnterms = atoi(line);

    *terms = (struct term *)malloc((*pnterms) * sizeof(struct term));
    
    for (int i = 0; i < *pnterms; i++) {
        if (fgets(line, sizeof(line), fp)) {
            
            char *ptr = line;
            while (*ptr == ' ') ptr++; 
            
            (*terms)[i].weight = atof(ptr);
            
            while (*ptr != '\t' && *ptr != ' '){
                ptr++;
            }  
            while (*ptr == '\t' || *ptr == ' '){
                ptr++;
            }  
            
            strncpy((*terms)[i].term, ptr, 200);
            (*terms)[i].term[strcspn((*terms)[i].term, "\r\n")] = 0;
        }
    }
    fclose(fp);

    qsort(*terms, *pnterms, sizeof(struct term), compare_lexicographic);
}

int lowest_match(struct term *terms, int nterms, char *sub) {
    int low = 0;
    int high = nterms - 1;
    int ans = -1;
    int len = strlen(sub);

    while (low <= high) {
        int mid = low + (high - low) / 2;
        int res = strncmp(terms[mid].term, sub, len);

        if (res == 0) {
            ans = mid;     
            high = mid - 1; 
        } else if (res < 0) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return ans;
}

int highest_match(struct term *terms, int nterms, char *sub) {
    int low = 0;
    int high = nterms - 1;
    int ans = -1;
    int len = strlen(sub);

    while (low <= high) {
        int mid = low + (high - low) / 2;
        int res = strncmp(terms[mid].term, sub, len);

        if (res == 0) {
            ans = mid;    
            low = mid + 1; 
        } else if (res < 0) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return ans;
}

void autocomplete(struct term **answer, int *n_answer, struct term *terms, int nterms, char *substr) {
    int first = lowest_match(terms, nterms, substr);
    int last = highest_match(terms, nterms, substr);

    if (first == -1 || last == -1) {
        *n_answer = 0;
        *answer = NULL;
        return;
    }

    *n_answer = last - first + 1;
    *answer = (struct term *)malloc((*n_answer) * sizeof(struct term));

    for (int i = 0; i < *n_answer; i++) {
        (*answer)[i] = terms[first + i];
    }

    qsort(*answer, *n_answer, sizeof(struct term), compare_weight);
}