# opencl
For each folder make does compile on the scholl device (there is no problem for the make of video filter, you don't need to change it)
## TP1 &2
First of all please notice that the makefile will compile all the .cpp, each of them corresponding to one exercice.
### TP1 add vector
For this TP the corresponding executable files are vector_add and vector_map.
They compute the addition of two vectors randomly generated with the CPU and the GPU.
The advantage of map buffer is to use less buffer, which represents a gain of time( there are less buffer to create and no data transfer).
However this approach is only possible when both gpu's and cpu's DRAM are shared.

The size of the vector is defined by the N function declared in the main function of each cpp: vector_add.cpp, vector_map.cpp

The fps are not constant with the size: For N = 1000 for instance it seems that CPU presents better results than GPU while the results are even for N =100000.
It seems obviously that gpu is better when dealing with high size vector.




### TP2 cross matrix
For this part, be aware that the executable need aruments to run properly:
Thus launch: 

. "matrix_cross N", where N is a integer number, to compute the cross product between two squared matrix with size N  

. "group_matrix_cross N M" where N has the same fonction as previously and M is the size of a thread group

With the two approachs the GPU is more efficient than the CPU.
For N = 512 with matrix cross, GPU has 1071 MFLOPS while the cpu presents 728MFLOPS. 
It's interesting to notice that fore some M value grouep_matrix_cross is more efficient than matrix_cross and for other it is less.
For instance for M  = 10 and N = 1000 without group we obtain 311MFLOPS while with threads group we obtain 422MFLOPS but with M = 8 we get 2246 Mflops...
Furthermore the program will crash for a too big M value (>16).It is linked to the number of thread launchable by a core.
It will also crash if M doesn't divide N.

## video filter

It's again about comparing the gpu approach and the cpu one.
While it's running without problem on my device it raises randomly a mali rendere error on the school's device.
I can't understand the origin of this error as for the same program, Friday morning it didn't apppear but it appeared during the evneing each time I launch the program... 
However you can get the output video file in the output folder.

It's of course possible to make this program more optimized by using group of thread and thus increse the performance of the gpu. 
The version in this repository doesn't use that but I still made some test. 
The performance are in this case influenced by the size of the workgroup which apparently has to be superior than the size if the convolution kernel in order to be efficient.
It also seems to add stability to the FPS.
The gain is however not noticeable proabbly due to the fact that opencl sdk I use is intel's sdk and not nvidia's one. Thus I suppose it's not optimized for nvidia architecture.

I do not use the Map buffer because my personal device does not have a shared memory between the GPU and the CPU.



