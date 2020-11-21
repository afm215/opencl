# opencl
## TP1 &2
First of all please notice that the makefile will compile all the .cpp which correcpnd, each of them to one exercice.
### TP1 add vector
For this TP the corresponding executable files are vector_add and vector_map.
They compute the addiition of two vectors randomly generated with the CPU and the GPU.
You can check the output of the program in order to cmpare this two situations.

It appears that Map buffer are more efficient. Please launch the programs if you want more details.
The size of the vector is defined by the N function declared in the main function of each cpp: vector_add.cpp, vector_map.cpp

The fps are not constant with the size

### TP2 cross matrix
For this tp be aware that the executable need aruments to run properly:
Thus launch: 

. "matrix_cross N" where N is a integer nomber to compute the cross product between two squared matrix with size N  

. "group_matrix_cross N M" where N has the same fonction as previously and M is the size of a thread group

It's interesting to notice that fore some M value grouep_matrix_cross is more efficient than matrix_cross and for other it isn't.
It is linked to the number of thread launchable by a core.

## video filter
It's again comparing the gpu approach and the cpu one.
While it's tunning without problem on my device it raises randomly a mali rendere error on the school's device.
However you can get the output file in the output folder.
It's of course possible to make this program more optimized by using group of thread and thus increse the performance of the gpu.
I do not use the Map buffer because my personal device doezs not have a shared memory between the GPU and the CPU.

The performance are in this case influenced by the size of the workgroup which apparently has to be superior ythan the size if the convolution kernel to be efficient.

It also seems to add stability to the FPS


