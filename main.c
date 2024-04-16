
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define Max 10

int compare(char s[], char v[])
{
    FILE *file = fopen(s, "r");
    FILE *file1 = fopen(v, "r");

    if (file == NULL && file1 == NULL)
    {
        perror("Eroare la deschiderea fișierelor");
        return -1; // Ieșiți din funcție dacă nu se pot deschide fișierele
    }

    int ch1, ch2;

    while ((ch1 = fgetc(file)) != EOF && ((ch2 = fgetc(file1)) != EOF))
    {
        if (ch1 != ch2)
        {
            printf("Fișierele sunt diferite.\n");

            return 0; // adica sunt diferite
        }
    }
    printf("Fis identice\n");
    fclose(file);
    fclose(file1);
    return 1;
}
void copierea(char elem[], char snapshot[])
{

    int file;
    if ((file = open(snapshot, O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR | S_IXUSR)) < 0)
    {
        perror("nu s a putut deschide fisierul din file");
        exit(-1);
    }
    int file1;
    char new[2048];
    snprintf(new, sizeof(new), "%s/%s", elem, snapshot);
    if ((file1 = open(new, O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR | S_IXUSR)) < 0)
    {
        perror("nu s a putut deschide fisierul din file1");
        exit(-1);
    }
    char buffer[1024];
    // ssize_t read(int fd, void *buff, size_t nbytes);
    while (read(file, buffer, sizeof(buffer)) > 0)
    {
        write(file1, buffer, sizeof(buffer));
    }
    close(file);
    close(file1);
}
void parcurg_dir(const char *nume, char snapshot[])
{
    struct stat buf;

    int file;
    // char snapshot[1024];

    struct dirent *d; // asta cu d_name e calea abs
    DIR *dir;
    if ((dir = opendir(nume)) == NULL)
    {
        perror("nu se poate deschide directorul");
        exit(-1);
    }
    else
    {
        printf("s a deschis  directorul in functie\n");
    }
    while ((d = readdir(dir)) != NULL)
    {

        if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
            continue;

        char path[1024];
        // write(int fd, void *buff, size_t nbytes);
        char newline[] = "\n";
        snprintf(path, sizeof(path), "%s/%s", nume, d->d_name);
        if (lstat(path, &buf) == -1)
            printf("nu a mers bn lstat din parcur dir\n");

        // AICI FAC PENTRU CEL DIN DREICTORUL ULTIM

        ino_t d_ino = buf.st_ino;
        // printf("i-nod din fis %ld", d_ino);
        // snprintf(snapshot, sizeof(snapshot), "snap%ld.txt", d_ino);
        if ((file = open(snapshot, O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR | S_IXUSR)) < 0)
        {
            perror("nu s a putut deschide fisierul din file");
            exit(-1);
        }
        /*int file1;
             char new[2048];
            snprintf(new, sizeof(new), "%s/%s", elem, snapshot);

            if((file1=open(new, O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR | S_IXUSR)) < 0)
            {
                perror("nu s a putut deschide fisierul din file1");
                exit(-1);
            }
            */
        // bag numele
        write(file, newline, strlen(newline));

        write(file, path, strlen(path));

        /* write(file1, newline, strlen(newline));

        write(file1, path, strlen(path));
        */

        // bag i nod

        char info[50] = "\ni-nod: ";
        char container[50]; // i-nod
        sprintf(container, "\n %ld", d_ino);
        write(file, info, strlen(info));
        write(file, container, strlen(container));
        /*  write(file1, info, strlen(info));
         write(file1, container, strlen(container));
         */
        // size
        char c[35];
        strcpy(container, "\n se scrie size:");
        write(file, container, strlen(container));
        // write(file1, container, strlen(container));

        sprintf(c, "%lu", buf.st_size);
        if (write(file, c, strlen(c)) < 0)
            printf("nu se scrie size\n");
        /*   if (write(file1, c, strlen(c)) < 0)
        printf("nu se scrie size\n");*/

        if (S_ISREG(buf.st_mode))
            printf("%s este file\n", d->d_name);
        if (S_ISDIR(buf.st_mode))
        {
            printf("%s este dir\n", d->d_name);
            parcurg_dir(path, snapshot);
        }
    }
}
int main(int argc, char **argv)
{

    if (argc < 2 || argc > Max + 1)
    {
        printf("not enough arguments\n");
        exit(2);
    }
    int i = 1;

    // tipul fisierului int stat(const char *file_name, struct stat *buf);

    while (i < argc && strcmp(argv[i], "-o") != 0)
    {
        struct stat buf;

        if ((lstat(argv[i], &buf)) == 0)
        {
            printf("a mers bine lstat\n");
        }
        else
            printf("nu a merrs \n");

        if (S_ISDIR(buf.st_mode))

            printf("%s este dir\n", argv[i]);

        else
            printf("%s nu este dir\n", argv[i]);

        DIR *dir;

        char snapshot[1024];
        char snapshotvechi[1024];
        int file; // FILE care contine snapshot
                  // contine toate snapshoturile
        if ((dir = opendir(argv[i])) == NULL)
            printf("nu s a deschis directorul\n");
        ino_t d_ino = buf.st_ino;
        // ssize_t write(int fd, void *buff, size_t nbytes);
        // Vreau sa mi bag datele in fisierul meu snapshot.txt.prima data sa l deschid

        snprintf(snapshot, sizeof(snapshot), "snap%ld.txt", d_ino);
        snprintf(snapshotvechi, sizeof(snapshot), "snapvechi%ld.txt", d_ino);

        // snprintf(snapshotvechi, sizeof(snapshotvechi), "snapshotvechi%d.txt", i);
        printf("snapshot din main %s \n ", snapshot);

        // int open(const char *pathname, int oflag, [, mode_t mode]);
        if ((file = open(snapshot, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR)) == -1)
            printf("nu s a deschis snapshot \n");

        // PENTRU ULTIMUL DIRECTOR
        /* int file1;
            char new[2048];
          snprintf(new, sizeof(new), "%s/%s", argv[argc], snapshot);

          if((file1=open(new, O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR | S_IXUSR)) < 0)
          {
              perror("nu s a putut deschide fisierul din file1");
              exit(-1);
          }
          */
        if (write(file, argv[i], strlen(argv[i])) == -1)
            printf("eroare de scriere\n");

        /*    if (write(file1, argv[i], strlen(argv[i])) == -1)
       printf("eroare de scriere\n");*/

        // pt i-nod

        char info[35] = "\ni-nod: ";
        char container[35];
        sprintf(container, "%ld", d_ino);
        write(file, info, strlen(info));
        write(file, container, strlen(container));

        // pt ultim dir
        /* write(file1, info, strlen(info));
        write(file1, container, strlen(container));
*/
        // size
        char c[35];
        strcpy(container, "\n se scrie size:");
        write(file, container, strlen(container));
        // write(file1, container, strlen(container));
        sprintf(c, "%lu", buf.st_size);
        if (write(file, c, strlen(c)) < 0)
            printf("nu se scrie size\n");

        /*  if (write(file1, c, strlen(c)) < 0)
                   printf("nu se scrie size\n");
       */
        // parcurg directorul
        close(file);
        closedir(dir);
        parcurg_dir(argv[i], snapshot);
        // copierea(argv[argc],snapshot);

        // COMPARAREA snapshoturilor
        if (snapshotvechi == NULL)
        {
            copierea(snapshotvechi, snapshot);
            printf("e null;\n");
        }

        else
        {
            printf("imi intra aici\n");
            if (compare(snapshot, snapshotvechi) == 0)
            // strcpy(snapshotvechi, snapshot);
            // strcpy(snapuri[i], snapshot);
        }

        i++;
        // close(file);
        //  close(file);
        // closedir(dir);
    }
    /*DIR *dir;
    i++;
    if (strcmp(argv[i - 1], "-o") == 0)
    {
        printf("pot sa bag in ultim\n");

        struct stat buf;

        if ((lstat(argv[i], &buf)) == 0)
        {
            printf("a mers bine lstat\n");
        }
        else
            printf("nu a merrs \n");

        if (S_ISDIR(buf.st_mode))

            printf("%s este dir\n", argv[i]);

        else
            printf("%s nu este dir\n", argv[i]);

        if ((dir = opendir(argv[i])) == NULL)
        {
            perror("nu se poate deschide directorul");
            // exit(-1);
        }
        else
        {
            printf("s a deschis in dir de iesire\n");
        }
        int file, filesursa;
        // struct dirent *d;
        int j;
        char dest_file[1024];
        snprintf(dest_file, sizeof(dest_file), "%s/%s", argv[i], "total1.txt");

        if ((file = open(dest_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR)) < 0)
            printf("nu se poate deschided in dir");
        else
            printf("se deschide in dir\n");
        // char snapshoturi[1024];
        for (j = 1; j < i - 1; j++)
        {
            if ((filesursa = open(snapuri[j], O_RDONLY, S_IRUSR | S_IWUSR | S_IXUSR)) < 0)
                printf("nu deschid fiec snapshot\n");
            else
                printf("se deschide fiec snapshot\n");
            char buffer[1024];
            // ssize_t read(int fd, void *buff, size_t nbytes);
            while (read(filesursa, buffer, sizeof(buffer)) > 0)
            {
                write(file, buffer, sizeof(buffer));
            }
            close(filesursa);
        }
        close(file);
    }
*/
    return 0;
}
