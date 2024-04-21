
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
 #include <sys/wait.h>
#define Max 10
#define BUFFER_SIZE 100

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

    // char snapshot[1024];

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

    if ((file1 = open(path, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR | S_IXUSR)) < 0)
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

        // Calea relativa totala

        char newline[] = "\n";
        snprintf(path, sizeof(path), "%s/%s", nume, d->d_name);
        if (lstat(path, &buf) == -1)
            printf("nu a mers bn lstat din parcur dir\n");

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
        close(file);
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
    int i = 1;

    while (i < argc && strcmp(argv[i], "-o") != 0)
    {
        // aici creez copii cu fork()
        pid_t cpid;
        if ((cpid = fork()) < 0)
        {
            perror("nu s a creat proces fork\n");
            exit(-1);
        }
        if (cpid == 0)
        { // codul fiului
            printf("snapshot for %s created successfully\n", argv[i]);
            struct stat buf;

            if ((lstat(argv[i], &buf)) == 0)
            {
                printf("a mers bine lstat\n");
            }
            else
                printf("nu a merrs \n");

            if (S_ISDIR(buf.st_mode))

                printf("%s este dir\n", argv[i]);
            // daca e dir fac fork

            else
                printf("%s nu este dir\n", argv[i]);

            DIR *dir;

            char snapshot[1024];
            char snapshotnou[1024];
            int file; // FILE care contine snapshot
                      // contine toate snapshoturile
            if ((dir = opendir(argv[i])) == NULL)
                printf("nu s a deschis directorul\n");
            ino_t d_ino = buf.st_ino;
            // ssize_t write(int fd, void *buff, size_t nbytes);
            // Vreau sa mi bag datele in fisierul meu snapshot.txt.prima data sa l deschid

            snprintf(snapshot, sizeof(snapshot), "snap%ld.txt", d_ino);
            snprintf(snapshotnou, sizeof(snapshot), "snapnou%ld.txt", d_ino);

            // snprintf(snapshotnou, sizeof(snapshotnou), "snapshotnou%d.txt", i);
            printf("snapshot din main %s \n ", snapshot);
            int file1;
            // int open(const char *pathname, int oflag, [, mode_t mode]);
            if ((file = open(snapshot, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR)) == -1)
                printf("nu s a deschis snapshot \n");
            if ((file1 = open(snapshotnou, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IXUSR)) == -1)
                printf("nu s a deschis snapshotnou \n");

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
            // hai sa mi bat capul cu compararea
            //  pt ultimul argument
            printf("ajunge inainte de copiere\n");
            copierea(argv[argc - 1], snapshot);

            // COMPARAREA snapshoturilor
            // daca return=1 inseamna ca sunt dif
            if (compare(snapshot, snapshotnou) == 0)
                printf("sunt la fel\n");
            else
            {
                printf("sunt diferite\n");
                copierefisiere(snapshotnou, snapshot);
            }

            i++;
            exit(0);
        }
    }
        i++;
         pid_t cpid;
        if ((cpid = fork()) < 0)
        {
            perror("nu s a creat proces fork\n");
            exit(-1);
        }
        if (cpid == 0)
        { 
            exit(0);
        
    }
    
    // ajunge la proces parinte
    //  aici wait(); in pagina de man pt wait codul de jos

      ssize_t wpid;
      int wstatus;
  do {
  
                   wpid = wait( &wstatus);
                   if (wpid == -1) {
                       perror("waitpid");
                       exit(EXIT_FAILURE);
         
                   }
          else {            /* Code executed by child */
               printf("Procesul cu PID %ld ",wpid );

                   if (WIFEXITED(wstatus)) {
                       printf("s a terminat cu codul%d\n", WEXITSTATUS(wstatus));
                   } else if (WIFSIGNALED(wstatus)) {
                       printf("killed by signal %d\n", WTERMSIG(wstatus));
                   } else if (WIFSTOPPED(wstatus)) {
                       printf("stopped by signal %d\n", WSTOPSIG(wstatus));
                   } else if (WIFCONTINUED(wstatus)) {
                       printf("continued\n");
                   }
           }
               } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));
    return 0;
}
