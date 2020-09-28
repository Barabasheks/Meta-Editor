#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    const option options[5] = {
            {"filepath", required_argument, NULL, 'F'},
            {"show", no_argument, NULL, 'H'},
            {"get", required_argument, NULL, 'G'},
            {"set", required_argument, NULL, 'S'},
            {"value", required_argument, NULL, 'V'}
    };
    char opt = getopt_long(argc, argv,"", options, NULL);
    char* filepath = (char*)malloc(20);
    while(opt != -1){
        switch(opt)
        {
            case 'F':{
                filepath = optarg;
                break;
            }
            case 'H': {
                FILE *file = fopen(filepath, "rb");
                char* header = (char*)malloc(10);
                fread(header, 10, 1, file);
                int size_frame = 0, dop_num = 1;
                char* frame_header = (char*)malloc(10);
                fread(frame_header, 10, 1, file);
                for(int j = 0; j < 4; j++)
                {
                    size_frame += (int)frame_header[7 - j] * dop_num;
                    dop_num *= 256;
                }
                while (size_frame != 0)
                {
                    printf("%c%c%c%c: ", frame_header[0], frame_header[1], frame_header[2], frame_header[3]);
                    char* frame_content = (char*)malloc(size_frame);
                    fread(frame_content, size_frame, 1, file);
                    for(int j = 1; j < size_frame; j++)
                        printf("%c", frame_content[j]);
                    printf("\n");
                    frame_header = (char*)malloc(10);
                    fread(frame_header, 10, 1, file);
                    size_frame = 0;
                    dop_num = 1;
                    for(int j = 0; j < 4; j++)
                    {
                        size_frame += (int)frame_header[7 - j] * dop_num;
                        dop_num *= 256;
                    }
                }
                break;
            }
            case 'G': {
                FILE *file = fopen(filepath, "rb");
                char *header = (char *) malloc(10);
                fread(header, 10, 1, file);
                int size_frame = 0, dop_num = 1;
                char *frame_header = (char *) malloc(10);
                fread(frame_header, 10, 1, file);
                for (int j = 0; j < 4; j++) {
                    size_frame += (int) frame_header[7 - j] * dop_num;
                    dop_num *= 256;
                }
                    bool flag = true;
                while (size_frame != 0) {
                    if (frame_header[0] == optarg[0] && frame_header[1] == optarg[1] && frame_header[2] == optarg[2] &&
                        frame_header[3] == optarg[3]) {
                        printf("%c%c%c%c: ", frame_header[0], frame_header[1], frame_header[2], frame_header[3]);
                        char *frame_content = (char *) malloc(size_frame);
                        fread(frame_content, size_frame, 1, file);
                        for (int j = 1; j < size_frame; j++)
                            printf("%c", frame_content[j]);
                        printf("\n");
                        flag = false;
                        break;
                    } else {
                        char *frame_content = (char *) malloc(size_frame);
                        fread(frame_content, size_frame, 1, file);
                        frame_header = (char *) malloc(10);
                        fread(frame_header, 10, 1, file);
                        size_frame = 0;
                        dop_num = 1;
                        for (int j = 0; j < 4; j++) {
                            size_frame += (int) frame_header[7 - j] * dop_num;
                            dop_num *= 256;
                        }
                    }
                }
                if (flag)
                    printf("This frame does not exist");
                break;
            }
            case 'S': {
                FILE *file = fopen(filepath, "rb");
                char *header = (char *) malloc(10);
                fread(header, 10, 1, file);
                int size = 0, dop_num = 1;
                for (int i = 0; i < 4; i++)
                {
                    size += (int) header[9 - i] * dop_num;
                    dop_num *= 256;
                }
                char* tags = (char*)malloc(size);
                fread(tags, size, 1, file);
                bool flag = true;
                for (int i = 0; i < size; i++)
                {
                    if (tags[i] == optarg[0] && tags[i + 1] == optarg[1] && tags[i + 2] == optarg[2] &&
                        tags[i + 3] == optarg[3])
                    {
                        flag = false;
                        opt = getopt_long(argc, argv,"", options, NULL);
                        fclose(file);
                        FILE *file1 = fopen(filepath, "r+b");
                        fseek(file, 0, SEEK_SET);
                        fwrite(header, 10, 1, file);
                        fwrite(tags, i + 4, 1, file);
                        char* size_new_frame = (char*)malloc(4);
                        for(int i = 0; i < 3; i++)
                            size_new_frame[i] = 0;
                        size_new_frame[3] = strlen(optarg) + 1;
                        fwrite(size_new_frame, 4, 1, file);
                        char* flags = (char*)malloc(3);
                        fwrite(flags, 3, 1, file);
                        fwrite(optarg, strlen(optarg), 1, file);
                        int size_frame = 0, dop_num = 1;
                        for (int j = 0; j < 4; j++)
                        {
                            size_frame += (int) tags[i + 7 - j] * dop_num;
                            dop_num *= 256;
                        }
                        fwrite(&tags[i + 10 + size_frame], size - i - 10 - size_frame, 1, file);
                        fclose(file);
                    }
                }
                if(flag)
                    printf("This frame does not exist");
                break;
            }
            default:
                break;
        }
        opt = getopt_long(argc, argv,"", options, NULL);
    }
    return 0;
}
