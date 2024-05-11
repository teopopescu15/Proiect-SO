



#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <sys/types.h>

#include <dirent.h>

#include <sys/stat.h>

#include <fcntl.h>

#include <unistd.h>

#include <sys/wait.h>

#include <libgen.h>

#include <errno.h>

#define Max 10

#define BUFFER_SIZE 100

// fisierul=cale diriz=calecorupt



void Coruptie(char *cale, const char *caleCorupt)

{

    char destPath[PATH_MAX];

    sprintf(destPath, "%s/%s", caleCorupt, basename(cale));



    if (rename(cale, destPath) != 0)

    {

        perror("Nu s a mutat fisierul\n");

        exit(-1);

    }

    else

    {

        printf("S a mutat fisierul corupt\n");

    }

}



//Imi copiaza continutul din snapshot in snapshotnou

void copierefisiere(char v[], char s[])

{

    int file, file1;



    if ((file = open(s, O_RDONLY)) < 0)

    {

        perror("Nu s-a putut deschide fișierul sursă");

        exit(EXIT_FAILURE);

    }



    if ((file1 = open(v, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR)) < 0)

    {

        perror("Nu s-a putut deschide fișierul de destinație");

        exit(EXIT_FAILURE);

    }



    char buff[BUFFER_SIZE];

    ssize_t size_bytes;



    while ((size_bytes = read(file, buff, BUFFER_SIZE)) > 0)

    {

        if (write(file1, buff, size_bytes) != size_bytes)

        {

            perror("Eroare la scrierea în fișierul de destinație");

            exit(EXIT_FAILURE);

        }

    }



    if (size_bytes < 0)

    {

        perror("Eroare la citirea din fișierul sursă");

        exit(EXIT_FAILURE);

    }



    if (close(file) < 0)

    {

        perror("Eroare la închiderea fișierului sursă");

        exit(EXIT_FAILURE);

    }



    if (close(file1) < 0)

    {

        perror("Eroare la închiderea fișierului de destinație");

        exit(EXIT_FAILURE);

    }

}



int compare(char snapshot1[], char snapshot2[])

{

    int file1 = open(snapshot1, O_RDONLY);

    int file2 = open(snapshot2, O_RDONLY);



    if (file1 < 0 || file2 < 0)

    {

        perror("Eroare la deschiderea fișierelor");

        return -1;

    }



    char buffer1[BUFFER_SIZE];

    char buffer2[BUFFER_SIZE];

    ssize_t size_bytes1, size_bytes2;



    do

    {

        size_bytes1 = read(file1, buffer1, BUFFER_SIZE);

        size_bytes2 = read(file2, buffer2, BUFFER_SIZE);



        if (size_bytes1 != size_bytes2 || memcmp(buffer1, buffer2, size_bytes1) != 0)

        {

            close(file1);

            close(file2);

            return 1; // Snapshot-urile sunt diferite

        }

    } while (size_bytes1 > 0 && size_bytes2 > 0);



    close(file1);

    close(file2);

    return 0; // Snapshot-urile sunt identice

}



void copierea(char nume[], char snapshot[])

{

    struct stat buf;

    if (lstat(nume, &buf) < 0)

        perror("lstat din copiere nu a mers\n ");

    if (S_ISDIR(buf.st_mode))

        printf("este dir in copiere\n");



    



    DIR *dir;

    if ((dir = opendir(nume)) == NULL)

    {

        perror("nu se poate deschide directorul IN COPIERE");

        exit(-1);

    }

    else

    {

        printf("s a deschis  directorul in COPIERE  \n");

    }



    char path[1024];



    // Calea relativa totala



    snprintf(path, sizeof(path), "%s/%s", nume, snapshot);

    if (lstat(path, &buf) == -1)

        printf("nu a mers bn lstat din parcur dir in COPIERE  \n");



    int file;

    if ((file = open(snapshot, O_RDONLY)) < 0)

    {

        perror("nu s a putut deschide fisierul din file");

        exit(-1);

    }

    else

        printf("s a deschis file in copiere\n");

    int file1;



    if ((file1 = open(path, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IXUSR)) < 0)

    {

        perror("nu s a putut deschide fisierul din file1");

        exit(-1);

    }

    else

        printf("s a deschis file1 in copiere\n");



    // ssize_t read(int fd, void *buff, size_t nbytes);



    char buffer[1024];

    ssize_t size;

    while ((size = read(file, buffer, sizeof(buffer))) > 0)

    {

        if (write(file1, buffer, size) != size)

        {

            perror("Eroare la scrierea in fisierul destinatar");

            close(file);

            close(file1);

            return;

        }

    }



    close(file);

    close(file1);



    closedir(dir);

}



void parcurg_dir(const char *nume, char snapshot[], char dir_izolat[], int *count)

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

        printf(" %s s a deschis  directorul in PARG_DIR\n", nume);

    }

    while ((d = readdir(dir)) != NULL)

    {



        if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)

            continue;



        char path[1024];



        // Calea relativa totala



        char newline[] = "\n";

        snprintf(path, sizeof(path), "%s/%s", nume, d->d_name);

        if (lstat(path, &buf) == -1)

            printf("nu a mers bn lstat din parcur dir\n");



        int k = 0;

        if (S_ISREG(buf.st_mode))

        {



            printf("%s AJUNGE CA E FISIER\n", d->d_name);

            if ((buf.st_mode & S_IRUSR) == 0 && (buf.st_mode & S_IRGRP) == 0 && (buf.st_mode & S_IROTH) == 0)

            {

               // printf("no reading rights  %s.\n", path);

                k++;

               

            }



            if ((buf.st_mode & S_IWUSR) == 0 && (buf.st_mode & S_IWGRP) == 0 && (buf.st_mode & S_IWOTH) == 0)

            {

               // printf("no writing rights  %s.\n", path);

                k++;

               

            }



            if ((buf.st_mode & S_IXUSR) == 0 && (buf.st_mode & S_IXGRP) == 0 && (buf.st_mode & S_IXOTH) == 0)

            {

               //printf("no execution rights  %s.\n", path);

                k++;

             

            }



            int pipefd[2];

            pid_t pidnepot;

            

            

            char buffer[BUFFER_SIZE];



            if (k == 3)

            {

            printf("imi ajunge la k=%d\n",k);

                if (pipe(pipefd) < 0)

                {

                    printf("Eroare la crearea pipe-ului\n");

                    exit(1);

                }

                if ((pidnepot = fork()) < 0)

                {

                    perror("eroare la fork\n");

                    exit(1);

                }

                if (pidnepot == 0)

                {                              // nepot

                    if (close(pipefd[0]) != 0) // inchid capătul de citire

                    {

                        perror("pipe citire");

                        exit(-1);

                    }

                    if (dup2(pipefd[1], 1) == -1)

                    {

                        close(pipefd[1]);

                        printf("nu a mers dup2\n");

                        exit(EXIT_FAILURE);

                    }



                      

                    execlp("./bash.sh", "./bash.sh", path, NULL);

                    perror("exec failed if it got here\n");

                }

                else

                {

                    close(pipefd[1]);

                    ssize_t biti;

                  //  buffer[strlen(buffer) - 1] = '\0';

                    // citesc din pipe

                    if ((biti = read(pipefd[0], buffer, BUFFER_SIZE)) == -1)

                    {

                        close(pipefd[0]);

                        perror("err pfd\n");

                        exit(-1);

                    }

                buffer[biti]='\0';

                if (close(pipefd[0]) != 0)

                {

                    perror("err pfd\n");

                    exit(-1);

                }

                pid_t wpidnepot;

                int wstatus;



                if((wpidnepot= wait(&wstatus))==-1){

                    perror("eroare la wait\n");

                    exit(EXIT_FAILURE);

                }



                if (WIFEXITED(wstatus))

                {

                    printf("Nepot proces cu count %d cu pid-ul %d \n", *count, wpidnepot);

                    if (strstr(buffer, "SAFE") != NULL)

                    {

                        {

                            printf("\n %s Nu e corupt\n",path);

                            k = 0;

                        }

                    }

                    else

                    {

                       printf(" \n %s  E corupt\n",path);

                        (*count)++;

                        Coruptie(path, dir_izolat);

                        continue;

                    }

                }

                else

                {printf(" sunt la eroare la  %s\n",path);

                    perror("Eroare la wifexited coruptie\n");

                    exit(-1);

                }

            }

        

            }

        }

        if (k == 0) // sa putem pune in screenshot doar cele cu drepturi(resp nemalitioase) sau directoarele

        {

            ino_t d_ino = buf.st_ino;

            // printf("i-nod din fis %ld", d_ino);

            // snprintf(snapshot, sizeof(snapshot), "snap%ld.txt", d_ino);



            if ((file = open(snapshot, O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR | S_IXUSR)) < 0)

            {

                perror("nu s a putut deschide fisierul din file");

                exit(-1);

            }



            // bag numele

            write(file, newline, strlen(newline));



            write(file, path, strlen(path));



            // bag i nod



            char info[50] = "\ni-nod: ";

            char container[50]; // i-nod

            sprintf(container, "\n %ld", d_ino);

            write(file, info, strlen(info));

            write(file, container, strlen(container));

           

            // size

            char c[35];

            strcpy(container, "\n se scrie size:");

            write(file, container, strlen(container));

            // write(file1, container, strlen(container));



            sprintf(c, "%lu", buf.st_size);

            if (write(file, c, strlen(c)) < 0)

                printf("nu se scrie size\n");

            

            if (S_ISDIR(buf.st_mode))

            {

                printf("%s este dir\n", d->d_name);

                parcurg_dir(path, snapshot, dir_izolat, count);

            }

            close(file);

        }

    }



closedir(dir);

}



int main(int argc, char **argv)

{



    if (argc < 2 || argc > Max + 1)

    {

        printf("not enough arguments\n");

        exit(2);

    }

    char output[100], dir_izolat[100];

    int i, nr_proces = 0;



    for (i = 1; i < argc; i++)

    {



        if (strcmp(argv[i], "-o") == 0)

        {

            strcpy(output, argv[i + 1]);

        }

        if (strcmp(argv[i], "-x") == 0)

        {

            strcpy(dir_izolat, argv[i + 1]);

        }

    }

    int count;

    for (i = 1; i < argc && strcmp(argv[i], "-o") != 0; i++)

    {

        // aici creez copii cu fork()

        count = 0;

        pid_t cpid;



        if ((cpid = fork()) < 0)

        {

            perror("nu s a creat proces fork\n");

            exit(-1);

        }

        if (cpid == 0)

        {

            // codul fiului

            printf("snapshot for %s created successfully\n", argv[i]);

            struct stat buf;



            if ((lstat(argv[i], &buf)) == 0)

            {

                printf("a mers bine lstat\n");

            }

            else

               { printf("nu a mers lstat\n");

              

               }



            if (S_ISDIR(buf.st_mode))



                printf("%s este dir\n", argv[i]);

           

             else

               {  printf("%s nu este un director\n", argv[i]);

                continue;

        

               }



            DIR *dir;



            char snapshot[1024];

            char snapshotnou[1024];

            int file; // FILE care contine snapshot

                    



            if ((dir = opendir(argv[i])) == NULL)

                printf("nu s a deschis directorul\n");

            ino_t d_ino = buf.st_ino;

           

            // Vreau sa mi bag datele in fisierul meu snapshot.txt.prima data sa l deschid



            snprintf(snapshot, sizeof(snapshot), "snap%ld.txt", d_ino);

            snprintf(snapshotnou, sizeof(snapshot), "snapnou%ld.txt", d_ino);



            

            printf("snapshot din main %s \n ", snapshot);

            int file1;

            

            if ((file = open(snapshot, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR)) == -1)

                printf("nu s a deschis snapshot \n");

            if ((file1 = open(snapshotnou, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IXUSR)) == -1)

                printf("nu s a deschis snapshotnou \n");



            if (write(file, argv[i], strlen(argv[i])) == -1)

                printf("eroare de scriere\n");



            // pt i-nod



            char info[35] = "\ni-nod: ";

            char container[35];

            sprintf(container, "%ld", d_ino);

            write(file, info, strlen(info));

            write(file, container, strlen(container));



            // size

            char c[35];

            strcpy(container, "\n se scrie size:");

            write(file, container, strlen(container));

            sprintf(c, "%lu", buf.st_size);



            close(file);

            closedir(dir);



            // parcurg directorul

            printf("parcurg directorul pentru %s\n", argv[i]);

            parcurg_dir(argv[i], snapshot, dir_izolat, &count);



            copierea(output, snapshot);

            // COMPARAREA snapshoturilor

            // daca return=1 inseamna ca sunt dif



            if (compare(snapshot, snapshotnou) == 0)

                printf(" Snapshoturile sunt la fel\n");

            else

            {

                printf(" Snapshoturi sunt diferite\n");

                copierefisiere(snapshotnou, snapshot);

            }



            exit(count);

        }

        else

        {

            nr_proces++;

        }

    }

    // ajunge la proces parinte

    pid_t wpid;

    int wstatus;

    for (i = 0; i < nr_proces; i++)

    {



        wpid = wait(&wstatus);

        if (wpid == -1)

        {

            perror("waitpid");

            exit(EXIT_FAILURE);

        }

        else

        { /* Code executed by child */

            printf("Procesul cu PID %d ", wpid);



            if (WIFEXITED(wstatus))

            {

                printf("s a terminat cu codul %d\n", WEXITSTATUS(wstatus));

            }

            else if (WIFSIGNALED(wstatus))

            {

                printf("killed by signal %d\n", WTERMSIG(wstatus));

            }

            else if (WIFSTOPPED(wstatus))

            {

                printf("stopped by signal %d\n", WSTOPSIG(wstatus));

            }

            else if (WIFCONTINUED(wstatus))

            {

                printf("continued\n");

            }

        }

    }

    return 0;

}