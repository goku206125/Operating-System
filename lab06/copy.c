#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>


int help_message();
int copy_read_write(int fd_from, int fd_to);
int copy_mmap(int fd_from, int fd_to);
bool file_exists (char *filename);


int main(int argc, char *argv[]){

    int map_files = 0;
    int help_msg = 0;
    int file_read;
    int file_write;
    int cont_flag = 0;
    int opt;

    //controls
    /*printf("First argument %s\n", argv[0]);
    printf("second argument %s\n", argv[1]);
    printf("third argument %s\n", argv[2]);
    printf("forth argument %s\n", argv[3]);
    */
    //geekforgeeks
    //printf("written file name ==> %s\n",argv[2]);
    while((opt = getopt(argc, argv, "mh" )) != -1) 
    { 
        switch (opt)
        {
        case 'm':
        //if -m option chosen, make the flag for it 1 
            map_files = 1;
        //if there is an option chosen, make option control flag as 1
            cont_flag = 1;
            break;
        
        case 'h':
            //if -h option chosen, make the flag for it 1 
            help_msg = 1;
            //if there is an option chosen, make option control flag as 1
            cont_flag = 1;
            break;

        default:
        //if entered options are not valid print error
            printf("Command not found.\n");
            help_message();
            _exit(0);
        }
    }
    //check the the number of input arguments
    if (map_files == 1 && argc > 4 || cont_flag == 0 && argc > 3){
        //if while -m option chosen, there is more than 4 argument print error and help
        //if while no option chosen, there is more than 3 arguments print error and help
		printf("Too many arguments. See usage:\n");
		help_message();
		_exit(0);
	} else if(map_files == 1 && argc < 4 || cont_flag == 0 && argc < 3){
        //if while -m option chosen, there is less than 4 argument print error and help
        //if while no option chosen, there is less than 3 arguments print error and help
		printf("Not enough arguments. See usage:\n");
		help_message();
		_exit(0);
	}

    //if help flag is one
    if (help_msg == 1)
    {
        
        //print help message by calling the help function
        help_message();

    }
/////////////////////////////////////////////////////////////////////////////
    //if map flag is one
    else if (map_files == 1)
    {   
        //check if the file exists
        if (file_exists(argv[2]) == 1)
        {
            //if exist, try to open it
            file_read = open(argv[2], O_RDONLY);
            if (file_read == -1){ //if you can not open, print error
                printf("File is not readable.\n");

            }
            else{

                //printf("File can not be read.\n");
                printf("File exist.\n");
                //open or create the destination file
                file_write = open(argv[3],O_RDWR | O_CREAT | O_TRUNC, 0644);
                if (file_write == -1){
                    //print error if can not create
                    printf("Can not create destination file.\n");
                }
                else{
                    //if everything is fine, call copy_mmap function
                    printf("%s will be copied to %s with mmap().\n",argv[2],argv[3]);
                    copy_mmap(file_read,file_write);
                }

            }
            
        }
        else
        {
            printf("File does not exist.\n");
        }
    }
/////////////////////////////////////////////////////////////////////////////
    //if no option was given
    else if (map_files == 0 && help_msg == 0)
    {
        if (file_exists(argv[1]) == 1)
        {
            file_read = open(argv[1], O_RDONLY);
            if (file_read == -1){
                printf("File is not readable.\n");

            }
            else{

                //printf("File can not be read.\n");
                printf("File exist.\n");
                
                file_write = open(argv[2],O_RDWR | O_CREAT | O_TRUNC, 0644);
                if (file_write == -1){
                    printf("Can not create destination file.\n");
                }
                else{
                    printf("%s will be copied to %s with read()/write().\n",argv[1],argv[2]);
                    copy_read_write(file_read,file_write);
                }

            }
            
        }


    }
    else 
    {
        printf("ERROR OCCURED\n");
        help_message();
        
    }
    

    close(file_read);
    close(file_write);
    return 0;
}


//stackoverflow Adam Liss
bool file_exists (char *filename) {
  struct stat   buffer;   
  int exist_or_not = stat (filename, &buffer);
  if (exist_or_not == 0)
  {
      return 1;
  }
  else
  {
      return 0;
  }
}

int help_message(){
    printf("===>!!HELP INFORMATION!!<===\n");
    printf("Syntax for the program is:\n");
    printf("copy [-m] <file_name> <new_file_name>\ncopy [-h]\n");


}

int copy_read_write(int fd_from, int fd_to){
    struct stat crw;
    if(fstat(fd_from,&crw) != 0){
        printf("Error during fstat in copy_read_write.\n");
        _exit(0);
    }
    ftruncate(fd_to,crw.st_size);
    int length = crw.st_size;
    int sek = 0;
    int c_r = 0;
    char buff[100];
    while(c_r > 0){
    	    
	    int check_read = read(fd_from, buff, 100);
	    // printf("Readded string : %s\n",readed);
	    if (check_read == -1)
	    {
		printf("Error occured while reading the file...\n");
	    }
	    else{
		//printf("Successful reading...\n");

	    }
	    int c_r = check_read;
	    int check_write = write(fd_to, buff, 100);
	    if (check_write == -1)
	    {
		printf("Error occured while writing the file...\n");
	    }
	    else{
		//printf("Successful writing...\n");

	    }
	    int lseek_check = lseek(fd_from, 100, SEEK_CUR);
     }
    
    


    printf("copy_read_write is done sauccessfully.\n");
    
}

int copy_mmap(int fd_from, int fd_to){
    struct stat mm;

    if (fstat(fd_from, &mm) != 0){
      //perror("fstat() error");
      printf("Error during fstat in copy_mmap.\n");
      //help_message();
      _exit(0);
    }
    else {
        ftruncate(fd_to,mm.st_size);
        void *source = mmap(NULL, mm.st_size, PROT_READ, MAP_SHARED, fd_from,0);
        void *desti = mmap(NULL, mm.st_size, PROT_WRITE, MAP_SHARED, fd_to,0);
        //memcopy(desti,source,mm.st_size);
        if (source == MAP_FAILED)
        {
            printf("Source mmap ERROR.\n");

        }
        else if (desti == MAP_FAILED)
        {
            printf("Destination mmap ERROR.\n");
            
        }
        else
        {
            memcpy(desti, source, mm.st_size);

        }
        
        
        int source_munmap = munmap(source,mm.st_size);
        int desti_munmap = munmap(desti,mm.st_size);
        if (source_munmap == 0 && desti_munmap == 0)
        {
            printf("munmap is successful.\n");

        }
        else{
            printf("Error during munmap.\n");
        }
        
    
    }
    printf("operation with mmap is done.\n");

}
