// run this program using gcc test.c beep.o sintable.o -lasound
// and then ./a.out

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "beep.h"

int big_to_little(int);                   // a function to convert big endian to little endian.Used for 4-byte numbers.
int big_to_little_2(int);                 // a function to convert big endian to little endian .Used for 2-byte number.
void delta_time_cal(short[], int *, int); // a function to find delta-time.
void dur(int, int, int, int *);           // a function to find duration.
void freq_finder(int, double *);          // a function to find frequency.

int main()
{
    FILE *infile;

    printf("Please select a song from the list below by its number:\n");
    printf("1.test 1\n2.test 2\n3.test 3\n4.test 4\n5.test 5\n6.test 6\n7.test 7\n");

    int choice;

    scanf("%d", &choice);

    switch (choice)
    {
    case 1:
        infile = fopen("MIDI files/1.mid", "rb");
        break;

    case 2:
        infile = fopen("MIDI files/2.mid", "rb");
        break;

    case 3:
        infile = fopen("MIDI files/3.mid", "rb"); // This is a bad midi file. If you open this file , you will see an error.
        break;

    case 4:
        infile = fopen("MIDI files/4.mid", "rb"); // This is a bad midi file. If you open this file , you will see an error.
        break;

	case 5:
        infile = fopen("MIDI files/5.mid", "rb"); // This is a bad midi file. If you open this file , you will see an error.
        break;

    case 6:
        infile = fopen("MIDI files/6.mid", "rb"); // This is a bad midi file. If you open this file , you will see an error.
        break;

	case 7:
        infile = fopen("MIDI files/7.mid", "rb"); // This is a bad midi file. If you open this file , you will see an error.
        break;
    
	default:
        printf("Unvalid choice!\n");
        exit(1);
    }

    if (infile == NULL)
    {
        // To check if the file is opened successfully.
        printf("No such file!\n");
        //This will terminate the program if the is not found.
        exit(1);
    }

    else
    {
        char header_chunk[4], reader[2];

        int length;

        fread(header_chunk, 4, 1, infile); //reading the header chunck from the file.

        printf("%s\n", header_chunk);

        fread(&length, 4, 1, infile); //reading the header length from the file.

        length = big_to_little(length);

        printf("lenght : %d\n", length);

        short format;

        fread(&format, 2, 1, infile); //reading the midi format from the file.

        format = big_to_little_2(format);

        printf("format : %d\n", format);

        if (format == 2)
        {
            // This will terminate the program if the format is 2.
            printf("This program does not support format 2!\nPlease try another file!\n");
            exit(1);
        }

        short ntrks;

        fread(&ntrks, 2, 1, infile); //reading the number of tracks from the file.

        ntrks = big_to_little_2(ntrks);

        printf("track : %d\n", ntrks);

        unsigned short division;

        fread(&division, 2, 1, infile);

        division = big_to_little_2(division); //reading the division from the file.

        printf("division : %d\n", division);

        int track_length;

        short num_reader, i = 0, j;

        short data_storage[1000000];

        int delta_time = 0, tempo = 0, duration, kk;
        double freq = 0;

        for (j = 0; j < ntrks; j++)
        {
            fread(header_chunk, 4, 1, infile);
            printf("Track header: %s\n", header_chunk);

            if (!strstr(header_chunk, "MTrk"))
            {
                printf("This file type is not supported!\n");

                exit(1);
            }

            fread(&track_length, 4, 1, infile);

            track_length = big_to_little(track_length);

            printf("Track header length: %d dec\n", track_length);

            while (i < track_length)
            {
                // Here we are going to store all the track data and read the events.
                fread(&data_storage[i], 1, 1, infile);

                if (data_storage[i] == 0xFF)
                {
                    i++;
                    fread(&data_storage[i], 1, 1, infile);

                    if (data_storage[i] == 0x2F)
                    {
                        i++;
                        fread(&data_storage[i], 1, 1, infile);
                        if (data_storage[i] == 0x00)
                            printf("The end of track!\n");
                        //This is a method to recognize the end of the track event.
                    }

                    if (data_storage[i] == 0x51)
                    {
                        i++;
                        fread(&data_storage[i], 1, 1, infile);
                        i++;
                        fread(&data_storage[i], 1, 1, infile);
                        i++;
                        fread(&data_storage[i], 1, 1, infile);
                        i++;
                        fread(&data_storage[i], 1, 1, infile);

                        tempo = data_storage[i] + (data_storage[i - 1] << 8) + (data_storage[i - 2] << 16);
                        tempo = 60000000 / tempo;
                        printf("tempo : %d\n", tempo);
                    }
                }

                if (data_storage[i] == 0x90)
                {

                    delta_time_cal(data_storage, &delta_time, i);

                    dur(tempo, division, delta_time, &duration);

                    printf("Starting a note!\n");
                    printf("delta time : %d\n", delta_time);
                    printf("duration : %d\n", duration);

                    i++;
                    fread(&data_storage[i], 1, 1, infile);
                    kk = data_storage[i];
                    freq_finder(kk, &freq);
                    printf("note number :%d\n", data_storage[i]);
                    printf("frequency :%lf \n", freq);

                    beep(freq, duration);
                }

                if (data_storage[i] == 0x80)
                {
                    delta_time_cal(data_storage, &delta_time, i);

                    dur(tempo, division, delta_time, &duration);

                    printf("note off!\n");
                    printf("delta time : %d\n", delta_time);
                    printf("duration : %d\n", duration);

                    i++;
                    fread(&data_storage[i], 1, 1, infile);
                    kk = data_storage[i];
                    freq_finder(kk, &freq);
                    printf("note number :%d\n", data_storage[i]);
                    printf("frequency :%lf \n", freq);

                    beep(freq, duration);
                }
                i++;
            }
            printf("%d\n", i);
        }
    }
}
int big_to_little(int x)
{
    int swapped;
    swapped = ((x >> 24) & 0x000000ff) |
              ((x << 8) & 0x00ff0000) |
              ((x >> 8) & 0x0000ff00) |
              ((x << 24) & 0xff000000);
    return swapped;
}

int big_to_little_2(int x)
{
    int swapped2 = (x >> 8) | (x << 8);
    return swapped2;
}

void delta_time_cal(short data_storage[], int *delta_time, int i)
{
    if (data_storage[i - 2] > 127 && data_storage[i - 3] < 128)
    {
        *delta_time = data_storage[i - 1] + (data_storage[i - 2] - 128) * 128;
    }
    if (data_storage[i - 2] > 127 && data_storage[i - 3] > 127 && data_storage[i - 4] < 128)
        *delta_time = (data_storage[i - 2] - 128) * 128 + data_storage[i - 1] + (data_storage[i - 3] - 128) * 16384;
    if (data_storage[i - 2] > 127 && data_storage[i - 3] > 127 && data_storage[i - 4] > 127)
        *delta_time = (data_storage[i - 2] - 128) * 128 + data_storage[i - 1] + (data_storage[i - 3] - 128) * 16384 + (data_storage[i - 4] - 128) * 2097152;
    if (data_storage[i - 2] < 128)
        *delta_time = data_storage[i - 1];
}

void dur(int tempo, int division, int delta_time, int *duration)
{
    (*duration) = tempo;

    (*duration) *= division;

    (*duration) = 60000 / (*duration);

    (*duration) *= delta_time;

    if (*duration < 800)
        (*duration) += 250;
}

void freq_finder(int x, double *y)
{
    if (x > 119 || x < 12)
    {
        printf("The note is out of range!\n");
        exit(1);
    }
    float fr[] = {16.35, 17.32, 18.35, 19.45, 20.60, 21.83, 23.12, 24.50, 16.35, 27.50, 29.14, 30.87, 32.70, 34.65, 36.71, 38.89, 41.20, 43.65, 46.25, 49.00, 51.91, 55.00, 58.27, 61.74, 65.41, 69.30, 73.42, 77.78, 82.41, 87.31, 92.50, 98.00, 103.83, 110.00, 116.54, 123.47, 130.81, 138.59, 146.83, 155.56, 164.81, 174.61, 185.00, 196.00, 207.65, 220.00, 233.08, 246.94, 261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392.00, 415.30, 440.00, 466.16, 493.88, 523.25, 554.37, 587.33, 622.25, 659.25, 698.46, 739.99, 783.99, 830.61, 880.00, 932.33, 987.77, 1046.50, 1108.7, 1174.6, 1244.5, 1318.51, 1396.91, 1479.98, 1567.98, 1661.22, 1760.00, 1864.66, 1975.53, 2093.00, 2217.46, 2349.32, 2489.02, 2637.02, 2793.83, 2959.96, 3135.96, 3322.44, 3520.00, 3729.31, 3951.07, 4186.01, 4434.92, 4698.63, 4978.03, 5274.04, 5587.65, 5919.91, 6271.93, 6644.88, 7040.00, 7458.62, 7902.13};

    *y = fr[x - 12];
}
