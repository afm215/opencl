__kernel void hello()
{
 printf("Hello, World!\n");
};

void test2(){
	printf("test2 \n");
}
__kernel void montest(char displayer){
	hello();
	test2();
	printf("mon test \n");
	printf("yo %c\n", displayer);


	printf("yo \n");
}
