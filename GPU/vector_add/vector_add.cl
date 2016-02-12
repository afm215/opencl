__kernel void vector_add(__global const float *x, 
                        __global const float *y, 
                        __global float *restrict z)
{
    int index = get_global_id(0);
    z[index] = x[index] + y[index];
}

float getValue(__global const float *x, __global const float *y, const int size, const int i, const int j){
    float result = 0.f;
    for (int k =0; k < size; k++){
        result += x[i * size + k] * y [k * size + j]; 
    }
    return result;
}
__kernel void cross_matrix(__global const float *x, 
                        __global const float *y,
                        __global float *restrict z, const int size)
    {
        int i = get_global_id(0);
        int j = get_global_id(1);
        z[i*size + j] = getValue(x, y, size, i,j);
    }

