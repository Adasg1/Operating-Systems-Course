#include <stdio.h>
#include <fcntl.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

// Funkcja do obliczania sumy kontrolnej (prostej sumy bajtów)
unsigned long calculate_checksum(FILE *file) {
    unsigned long sum = 0;
    int ch;
    rewind(file); // Przewiń plik na początek
    while ((ch = fgetc(file)) != EOF) {
        sum += (unsigned char)ch;
    }
    return sum;
}

// Funkcja do odwracania linii
void reverse_line(char *line, int length, FILE *output) {
    int start = (line[length - 1] == '\n') ? length - 2 : length - 1;
    for (int i = start; i >= 0; i--) {
        fputc(line[i], output);
    }
    if (start == length - 2) fputc('\n', output);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Nieprawidlowa ilosc argumentow!\n");
        return 22;
    }

    DIR *source = opendir(argv[1]);
    if (!source) {
        perror("Nie udalo sie otworzyc folderu zrodlowego");
        return 1;
    }

    mkdir(argv[2], S_IRWXU | S_IRGRP | S_IROTH);
    DIR *destination = opendir(argv[2]);
    if (!destination) {
        perror("Nie udalo sie otworzyc folderu docelowego");
        closedir(source);
        return 1;
    }

    struct dirent *current_file;
    char path_to_file[PATH_MAX], path_to_output[PATH_MAX];
    char *line = NULL;
    size_t length = 0;

    while ((current_file = readdir(source)) != NULL) {
        if (strcmp(current_file->d_name, ".") == 0 || strcmp(current_file->d_name, "..") == 0 ||
            strstr(current_file->d_name, ".txt") == NULL) continue;

        snprintf(path_to_file, PATH_MAX, "%s/%s", argv[1], current_file->d_name);
        snprintf(path_to_output, PATH_MAX, "%s/%s", argv[2], current_file->d_name);

        FILE *source_file = fopen(path_to_file, "r");
        FILE *destination_file = fopen(path_to_output, "w");

        if (!source_file || !destination_file) {
            perror("Blad w otwarciu pliku");
            if (source_file) fclose(source_file);
            if (destination_file) fclose(destination_file);
            continue;
        }

        // Oblicz sumę kontrolną przed odwróceniem
        unsigned long checksum_before = calculate_checksum(source_file);
        printf("Suma kontrolna przed odwroceniem (%s): %lu\n", current_file->d_name, checksum_before);

        // Odwróć plik
        rewind(source_file); // Przewiń plik źródłowy na początek przed odwróceniem
        while (getline(&line, &length, source_file) != -1) {
            reverse_line(line, (int)strlen(line), destination_file);
        }

        // Zamknij plik docelowy, aby zapisać dane na dysk
        fclose(destination_file);

        // Otwórz plik docelowy ponownie do odczytu
        destination_file = fopen(path_to_output, "r");
        if (!destination_file) {
            perror("Blad w otwarciu pliku docelowego do weryfikacji");
            fclose(source_file);
            continue;
        }

        // Oblicz sumę kontrolną po odwróceniu
        unsigned long checksum_after = calculate_checksum(destination_file);
        printf("Suma kontrolna po odwroceniu (%s): %lu\n", current_file->d_name, checksum_after);

        // Sprawdź, czy sumy kontrolne są równe
        if (checksum_before == checksum_after) {
            printf("Sumy kontrolne sa identyczne. Dane nie zostaly utracone.\n");
        } else {
            printf("Sumy kontrolne roznia sie. Wystapil blad podczas przetwarzania.\n");
        }

        fclose(source_file);
        fclose(destination_file);
    }

    free(line);
    closedir(source);
    closedir(destination);

    printf("\nUzyj polecenia md5sum, aby sprawdzic integralnosc danych:\n");
    printf("md5sum %s/*.txt %s/*.txt\n", argv[1], argv[2]);
    return 0;
}