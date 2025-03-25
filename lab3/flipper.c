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

unsigned long calculate_checksum(int fd) {            //liczenie sumy kontrolnej
    unsigned long sum = 0;
    unsigned char buffer[1024];
    ssize_t bytes_read;

    lseek(fd, 0, SEEK_SET);

    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytes_read; i++) {
            sum += buffer[i];
        }
    }

    return sum;
}

void reverse_line(char *line, int length, FILE *output) {      //odwracanie linii
    int start = (line[length - 1] == '\n') ? length - 2 : length - 1;
    for (int i = start; i >= 0; i--) {
        fputc(line[i], output);
    }
    if (start == length - 2) fputc('\n', output);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Nieprawidlowa ilosc argumentow!\n");
        return 22;
    }

    DIR *source = opendir(argv[1]);
    if (!source) {
        printf("Nie udalo sie otworzyc folderu zrodlowego");
        return 1;
    }

    mkdir(argv[2], S_IRWXU | S_IRGRP | S_IROTH);
    DIR *destination = opendir(argv[2]);
    if (!destination) {
        printf("Nie udalo sie otworzyc folderu docelowego");
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
            printf("Blad w otwarciu pliku");
            if (source_file) fclose(source_file);
            if (destination_file) fclose(destination_file);
            continue;
        }

        unsigned long checksum_before = calculate_checksum(fileno(source_file)); //suma kontrolna przed odwroceniem
        printf("\nSuma kontrolna przed odwroceniem (%s): %lu\n", current_file->d_name, checksum_before);

        rewind(source_file);  //wskaznik pliku na poczatek i odwracamy
        while (getline(&line, &length, source_file) != -1) {
            reverse_line(line, (int)strlen(line), destination_file);
        }

        fclose(destination_file); //zamykanie pliku żeby zapisać dane na dysk

        destination_file = fopen(path_to_output, "r"); //ponowne otwarcie
        if (!destination_file) {
            printf("Blad w ponownym otwarciu pliku docelowego");
            fclose(source_file);
            continue;
        }

        unsigned long checksum_after = calculate_checksum(fileno(destination_file)); //suma kontrolna po odwroceniu
        printf("Suma kontrolna po odwroceniu (%s):     %lu\n", current_file->d_name, checksum_after);

        if (checksum_before == checksum_after) {
            printf("Sumy kontrolne identyczne - dane nie zostaly utracone.\n");
        } else {
            printf("Sumy kontrolne roznia sie - wystapil blad.\n");
        }

        fclose(source_file);
        fclose(destination_file);
    }

    free(line);  //zwolnij pamięć przydzieloną przez getline
    closedir(source);
    closedir(destination);

    printf("\nDo sprawdzenia integralnosci danych:\n");   //suma kontrolna MD5
    printf("md5sum %s/*.txt %s/*.txt\n", argv[1], argv[2]);
    return 0;
}