#include <stdio.h>
#include "mpi.h"

#define DIM_BUF 10

int main(int argc, char **argv[])
{
    int myrank, size;
    int buf[DIM_BUF];
    int offset, intsize;

    MPI_File fh;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    for (int i = 0; i < DIM_BUF; i++)
    {
        buf[i] = i + myrank * DIM_BUF; //ubacivanje nekih vrednosti u buffer
    }

    //////////////upis u fajl//////////////////////

    MPI_File_open(MPI_COMM_WORLD, "fajl1.dat", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh);
    MPI_Type_size(MPI_INT, &intsize);    //ova funkcija se koristi da se dobije velicina nekog tipa (dobra je za
                                         //custom tipove za koje nismo sigurni koje su velicine, dobra je praksa i za ovo cak i ako je samo integer u pitanju)
    offset = myrank * DIM_BUF * intsize; //offset = pocetak odakle treba da krene da cita proces iz fajla
    MPI_File_seek(fh, offset, MPI_SEEK_SET);
    MPI_File_write(fh, buf, DIM_BUF, MPI_INT, &status);

    MPI_File_close(&fh);
    MPI_Finalize();

    //////////////upis u fajl//////////////////////

    //////////////citanje iz fajla/////////////////

    MPI_File_open(MPI_COMM_WORLD, "fajl1.dat", MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
    //definisanje citanje fajla na drugi nacin namerno
    MPI_Offset file_size;
    MPI_File_get_size(fh, &file_size);
    offset = file_size / size * myrank;
    printf("my_rank: %d, filesize: %lld, offset: %d\n", myrank, file_size, offset); //samo provera
    MPI_File_read_at(fh, offset, buf, DIM_BUF, MPI_INT, &status);

    printf("my_rank: %d, buf nakon citanja:\n", myrank);
    for (int i = 0; i < DIM_BUF; i++)
    {
        printf(" %d", buf[i]);
        printf("\n\n");
    }
    //provera da li je citanje dobro proslo (ne mora se pise)

    MPI_File_close(&fh);
    MPI_Finalize();

    //////////////citanje iz fajla/////////////////

    //////////////treci deo zadatka////////////////
    //svaki proces upisuje upravo procitane podatke u novu datokteku na sledeci nacin: (za 4 procesa)
    // 0 1 10 11 20 21 30 31 2 3 12 13 22 .............. 8 9 18 19 28 29 38 39

    MPI_Datatype filetype;
    MPI_File_open(MPI_COMM_WORLD, "fajl2.dat", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh);
    MPI_Type_vector(DIM_BUF / 2, 2, size * 2, MPI_INT, &filetype);
    MPI_Type_commit(&filetype);

    offset = 2 * intsize * myrank;
    MPI_File_set_view(fh, offset, MPI_INT, filetype, "native", MPI_INFO_NULL);
    MPI_File_write_all(fh, buf, DIM_BUF, MPI_INT, &status);                        //grupna operacija (zbog poslednje recenice)
    MPI_File_get_size(fh, &file_size);                                             //provera
    printf("myrank: %d, filesize: %lld, offset: %d\n", myrank, file_size, offset); //provera

    MPI_File_close(&fh);

    MPI_Finalize();
    //////////////treci deo zadatka////////////////
}