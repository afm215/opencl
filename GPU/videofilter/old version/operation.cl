__kernel void Filter(__global const unsigned char *input, __constant  float *kernelfun, __global unsigned char * z, int line, int colonne){
    //printf("out \n");
    //manque taille ligne et colonne

    int x = get_global_id(0);
    int y = get_global_id(1);
    unsigned char res = 0;
    float test1= 2.1;
    unsigned char test;

    float test2 = 3;
    double tmp = 0;
    if(x<=0|| x >=line-1||y>=colonne||y<=0){
        tmp = input[x *colonne + y];
    }
    else{
        for (int i = 0; i<3; i++){
            for (int j=0; j<3; j++){
                tmp += (double)input[(x+(1-i))*colonne +(1-j) + y] *(double)kernelfun[i * 3 + j];
            }
        }
        }

    tmp = floor(tmp + 0.5);
    test = convert_uchar(tmp);
    res = (unsigned char)( 18 * test1 + test2 * input [x* colonne + y]);
    //test = (unsigned char) res;
    if (test <0){
    printf("in gpu %u \n", test);
    }
    //printf(" in gpu %u \n", res);
    //z[x* colonne + y] =(char) res;
    z[x* colonne + y] = test;
}